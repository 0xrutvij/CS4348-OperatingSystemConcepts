/********************************************
File Name: 				main.cc
Author: 				del_dilettante
Last Modification Date: 09/26/2020
Last Modifier:          del_dilettante
Procedures:

	main() : The test porgram that controls and spawns the producer and consumer
			 Thus simulating the producer-consumer problem on an OS.
			 
	producer(): The forked process that produces and enqueues items in the buffer.

	consumer(): The forked processes that consume items from the buffer.

Structs:

	shmem_struct: An object to be placed in shared memory to aid IPC.
********************************************/

#include <iostream>     //cout
#include <vector>       //vectors
#include <unistd.h>     //fork() getpid()
#include <stdlib.h>     //exit()
#include <sys/types.h>  //fork() getpid()
#include <sys/types.h>  //waitpid()
#include <sys/wait.h>   //waitpid()
#include <sys/mman.h>   //shm_open(), mmap()
#include <sys/stat.h>   /* For mode constants */
#include <fcntl.h>      /* For O_* constants */
#include <semaphore.h>

#include "circq.h"      /*Header file with the circular queue class*/

using namespace std;

//Name of the POSIX shared memory space to be created
#define NAME "/shmem_space"   

//Size of the circular queue/buffer as specified in requirementents
#define BUFFER_SIZE 1000  

//Number of items to be produced by the producer as specified in requirements
#define PROD_VAL 1000000


/********************************************
Structure Name: 		shmem_struct
Author: 				del_dilettante
Date: 					09/26/2020
Parameters:
	Defined and their uses commented within the struct.

Description:
	Is the structure to be saved within the POSIX shared memory space to
	facilitate inter-process communication.
********************************************/

struct shmem_struct
{
  sem_t s;          //The buffer access semaphore
  sem_t fillCount;  //The semaphore tracking the current fill level
  sem_t emptyCount; //The semaphore that tracks the number of empty spaces in the buffer.
  int x;            //Tracks the total number of items consumed across all consumers
  bool prod_done;   //A flag set to true when producer finishes production
  circular_q queue; //The buffer as described in the requirements.
};


//Constant that defines size of the shared memory to be created by the OS.
const int PAGE_SIZE = sizeof(shmem_struct);

//Procedure declarations, defined below (after main()
void producer();
void consumer(int);


/********************************************
Procedure Name: 		main()
Author: 				del_dilettante
Date: 					09/26/2020
Parameters:
	O/P int  Exit status code

Description:
	Creates a shared memory object and the producer and consumers.
	Once the latter processes exit, it cleans up any zombie processes.
	Finally signals the exit condition of each child process and reports
	the total number of items consumed by each child process.
	Closes the shared memory object and unlinks it from memory after destroying
	the semaphores contained within it.
********************************************/


int main()
{
  //A vector of size eleven and all elements initialised to zero.
  vector<int> pid(11, 0); 

  /*
  The int fd stands for the file descriptor returned by the unix function
  shm_open(). A call to this function opens and creates a POSIX shared memory space
  with the name supplied by the NAME param, the bitwise ORed macro flags denote
  that the space is to be created with mutual exclusion and read-write features.
  The 0666 are the file permissions associated with the memory space file stored at
  /dev/shm
  */
  int fd = shm_open(NAME, O_CREAT | O_EXCL | O_RDWR, 0666);
  
  /*
  For the file descriptor specified by int fd, in this case are shmem space, reserve
  a space of size as defined by the page_size macro parameter supplied.
  */
  ftruncate(fd, PAGE_SIZE);

  /*
  Create a pointer to a shmem_strut and associated the pointer to the file descriptor fd,
  our shared memory space. Now map this space to the virtual address space of the calling 
  process. The adress for mapping isn't specified hence the NULL param. The OS's kernel will
  thus choose a pre-aligned address at which to map it. The size of the mapping is as supplied
  by the PAGE_SIZE param. The bitwise ORed flags signifies the protection of memory required,
  we're allowing read and write. The mapping is to be shared and any updates to the memory in it
  will be shared with all processes that are mapping this same region. The contents of the mapped 
  area are pointed to by the file descriptor fd. 0 is the offset for the content at fd which is to 
  be mapped.

  The return val is usually a pointer to the mapped area, but we cast it as ptr to our struct since that is
  the only object to be mapped in the space.
  */
  shmem_struct *shmem_queue = (shmem_struct *)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  
  //Create an instance of circular queue object of the size defined by BUFFER_SIZE
  circular_q z(BUFFER_SIZE);

  //Set the production done false in the shmem_struct to false, since the production hasn't even started.
  shmem_queue->prod_done = false;

  /*
  Supply the queue instantiated earlier to the struct and store it in the circular_q member variable within it
  thereby storing it in the shared mem space.
  */
  shmem_queue->queue = move(z);

  /*
  Debugging code retained for modifications and debug.
  //cout << "Max size of buffer = " <<shmem_queue->queue.max_capacity() << endl;
  */

  //Set the total number of items consumed, x, to zero within the struct in shmem.
  shmem_queue->x = 0;

  /*
  Initialise the sempahores in shmem with the values supplied as the third parameter and the second parameter
  is 1 for all three signifying that these semaphores are to be shared between different processes.
  */
  sem_init(&(shmem_queue->s), 1, 1);
  sem_init(&(shmem_queue->fillCount), 1, 0);
  sem_init(&(shmem_queue->emptyCount), 1, BUFFER_SIZE);

  //Close the file pointed to by the file  descriptor
  close(fd);

  //Create a variable to supply and hold the ids for the various consumers	
  int cons_id = -1;

  //Record the pid of the parent for debugging purposes.
  int parent_pid = (int)getpid();

  /*
  The for loop runs through 11 iterations
  On the first iteration it creates the Producer as a child process by forking
  On iterations 2 through 11 it'll create the 10 Consumers by forking.
  */
  
  for(int i = 0; i < 11; i++)
  {
  	//Increment the consumer id, we start at negative to allow for pre-increment and non-assignation on first itr.
    cons_id++;

    /*
    Fork the current process to create a child
    The retval is stored in the pid array.
    The retval is the pid of the child in the parent section of code after fork and 
    it is zero (0) in the child section of code after the fork occurs.
    */
    pid[i] = fork();

	//Child Process if the pid on the i_th itr  is zero as discussed above.
    if(pid[i] == 0) 
    {
      //Producer if it's the first itr and pid == 0
      if(pid[0] == 0) 
      {
      	//Call producer proc. 
        producer(); 
        /*
		Debug code retained and commented out for future use
        //cout << "Producer returns\n";
        Once the producer proc. finishes and returns terminate the process via exiting.
        */
        exit(0);
      }
      //Consumer if it isn't the first itr and pid == 0
      else 
      {
      	//Call consumer proc.
        consumer(cons_id);
        /*
   		  Debug code retained and commented out for future use
           //cout << "Consumer " << k <<  " returns \n";
           Once the consumer proc. finishes and returns terminate the process via exiting.
        */
        exit(0);
      }
    }
  }

  //The children process never enter this section of code since their forked code segment exits earlier

  //Make an int var to store the status of the child being waited upon.
  int status = 0;

  //Total number of children whose status is to be reported.
  int numChild = 11;

  //A variable to store the pid of the child process which has changed state.
  pid_t child_pid;

  //A loop that runs until all children processes have been accounted for
  while(numChild != 0)
  {
  	/*
  	Waitpid function waits for any child process changing state (-1 param) and the status is stored in the
    corresponding variable with the child who changed state's pid being returned.
  	*/
    child_pid = waitpid(-1, &status, 0);

    /*
    Loop through to check what child is it that has actually change state
    Child 0 is the producer
    Child's 1 through 10 are the consumers with ids from 1 through 10.
    */
    for(int j = 0; j < 11; j++)
    {
      if(child_pid == pid[j])
      {
        cout << "\nMain: Child " << j << " has stopped, ";
        /*
        Using the status info we check the state change
        we expect normal termination for all children.
        
        If they're stopped using SIGSTP, then we get case 2

        If they exit due to some other error and are killed then we get case 3
        Case 3 also reports the signal number that caused termination
        For example while debugging signal 8 and 11 were caught
        - 8 being aritmethic error (issue was w/ a pointer being incremented instead of the underlying val)
        - 11 being a seg fault (improperly initialised class member in the struct was being accessed and causing
        memory access to an undefined area.)

        Case 4 is the default case and in case the state change is other than the three being checked for.
        */
        if(WIFEXITED(status))
        {
            cout << "normally...";
        }
        else if(WIFSTOPPED(status))
        {
          cout << "stop signalled...";
        }
        else if(WIFSIGNALED(status))
        {
          cout << "terminate signalled..." << "number " << WTERMSIG(status);
        }
        else
        {
          cout << "abnormally...";
        }
        cout << endl;
      }
    }
    //Reduce the number of children that need to be accounted for.
    --numChild;
  }

  //By accessing the shmem space variable x, we find the total consumption by all consumers.
  cout << "\nMain: Total Consumption =  " << shmem_queue->x << endl;

  //The three sempahores in the shmem space are destroyed to prevent memory leaks.
  sem_destroy(&(shmem_queue->fillCount));
  sem_destroy(&(shmem_queue->emptyCount));
  sem_destroy(&(shmem_queue->s));

  //Unmap the shmem area to prevent memory leaks and free resources.
  munmap(shmem_queue, PAGE_SIZE);

  /*
  Unlink and delete the shmem file created at /dev/shem to prevent mem leaks, free resources
  and prevent seg faults next time the program is called. 
  */
  shm_unlink(NAME);

  /*
  Lastly signal the parent process is exiting and thus signify normal program execution
  the suppliance of parent_pid was an earlier debugging requirement retained.
  */
  cout << "\nMain: Parent with pid: " << parent_pid << " is exiting.\n" << endl;

  //Exit with code 0
  return 0;
}

/********************************************
Procedure Name: 		producer()
Author: 				del_dilettante
Date: 					09/26/2020
Parameters:
	None

Description:
	Uses semaphores to simulate a single producer in the producer-consumer problem
	which deals with mutual exclusion and concurrency. Produces an object and places
	it on the circular buffer in a concurrent process/thread safe manner.
********************************************/

void producer()
{
  //Open the shmem space given by name, NAME, w/ read and write permission.
  int fd = shm_open(NAME, O_RDWR, 0666);

  /*
  Access the data supplied at file descriptor fd pointing to the shmem space and map it
  in this process' virtual memory space for use. Cast the pointer to the shmem space as a
  pointer to the struct stored there and that will be accessed.
  */
  shmem_struct *shmem_queue = (shmem_struct *)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  /*
  Debug messages to ensure the code has entered the producer section.
  And that it can access the buffer of size defined and created earlier.

  //cout << "Hello from producer!\n";
  //cout << "Max size of buffer = " <<shmem_queue->queue.max_capacity() << endl;

  */

  
  for(int i = 0; i < PROD_VAL; i++)
  {
  	//The int to be enqueued isn't explicitly produced but rather we store the value of i in the queue

  	//Wait till number of empty spaces are non-zero or block till they're non-zero again.
    sem_wait(&(shmem_queue->emptyCount));
    //Wait until the counting sempahore is positive or is decremented and release by some other process.
    sem_wait(&(shmem_queue->s));
    //Enqueue an 'item' into the circular buffer
    shmem_queue->queue.enqueue(i);
    /*
    Increment the buffer control semaphore signalling exit from the critical section of the code
    and allowing access to the next queued process
    */
    sem_post(&(shmem_queue->s));
    //Increase the fill count semaphore to signal a new item has been added.
    sem_post(&(shmem_queue->fillCount));
  }

  //Let the user know production is done
  cout << "\nProducer: production done, exiting...\n";

  /*
  After iterations equal to the number of products to be produced production is complete
  thus set the production complete flag to true.
  */
  shmem_queue->prod_done = true;

  //Unmap the memory space mapped in the virtual address space
  munmap(shmem_queue, PAGE_SIZE);
  
  /*
  Close the fd opened with shem_open, the unlinking is the responsibility of the parent once 
  all the processes are done. Return to main.
  */
  close(fd);
}


/********************************************
Procedure Name: 		consumer(int id)
Author: 				del_dilettante
Date: 					09/26/2020
Parameters:
	I/P int  The consumer's unique id

Description:
	The code for 10 similar consumer processes in the simulation of the prod-consumer problem.
	Each consumer consumer items from the circular buffer until production stops and the buffer
	is empty.
********************************************/
void  consumer(int id)
{
  //To store the number of items consumed by this consumer
  int items_consumed = 0;
  
  //To store an item that is to be 'consumed'
  int item = 0;

  //Open the shmem space given by name, NAME, w/ read and write permission.
  int fd = shm_open(NAME, O_RDWR, 0666);
  
  /*
   Access the data supplied at file descriptor fd pointing to the shmem space and map it
   in this process' virtual memory space for use. Cast the pointer to the shmem space as a
   pointer to the struct stored there and that will be accessed.
  */
  shmem_struct *shmem_queue = (shmem_struct *)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);


  //As long as either the production isn't done OR (inclusive) the queue isn't empty keep running. 
  while(!(shmem_queue->prod_done)||!(shmem_queue->queue.empty()))
  {
	/*
	Check again to ensure the queue isn't empty
	Usually if the queue is empty and production is underway, the fillCount semaphore is incremented and this
	isn't an issue, but the edge case of when production is done and there are less than 10 items on the queue
	the issue occurs where all consumers might detect the queue isn't empty and thus wait for the fill_Count to 
	increase this has led to a deadlock where no consumer proceeds as all of them have executed a wait for fillCount
	and the production was done thus there was no increment from the producer to release them from the deadlock.

	Debugging indicated that all items had also been consumed but the consumer processes were in a suspended state
	because of the sem_wait on fillCount. After having to kill the child processes with sigkill term, this soln.
	was instituted.
	*/
    if(!(shmem_queue->queue.empty()))
    {
      sem_wait(&(shmem_queue->fillCount));
    }
    else
    {
      //if the queue is empty go and check the pre-condition again and see if the production is also complete
      continue;
    }

    //Wait for the release of the buffer control semaphore.
    sem_wait(&(shmem_queue->s));
    
    //Consume the item by dequeuing it from the buffer.
    item = shmem_queue->queue.dequeue();

	//release the buffer access semaphore to the next process inline.
    sem_post(&(shmem_queue->s));
    
    //increment the emptyCount sempahore to let the producer know there are more empty spaces on the buffer
    sem_post(&(shmem_queue->emptyCount));

    //increment the global consumption total in shmem
    shmem_queue->x += 1;

    //increment the personal consumption total.
    items_consumed++;

	/*
	Debugging code and alternative solutions to the problem of overconsumption
	where the consumers didn't stop accessing the buffer even after end of production
	and the buffer being empty.
    
    //if(shmem_queue->x > (PROD_VAL - 10))
    //break;
    

    //if(items_consumed == (PROD_VAL/10))
    //  break;
      
    */

  }

  //Report the number of items consumed by consumer i.
  cout << "\nConsumer " << id << " consumed " << items_consumed << " items" << endl;

  //Unmap the memory space mapped in the virtual address space
  munmap(shmem_queue, PAGE_SIZE);
    
  /*
  Close the fd opened with shem_open, the shmem  unlinking is the responsibility of the parent once 
  all the processes are done. Return to main.
  */
  close(fd);
}
