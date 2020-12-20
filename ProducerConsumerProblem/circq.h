/********************************************
File Name: 				circq.h
Author: 				del_dilettante
Last Modification Date: 09/28/2020
Last Modifier:          del_dilettante
Classes:
	circular_q - A class to create a circular queue

	
Procedures: Class methods for the circular queue class.

	  void enqueue(int data) - Enqueue data into the circular queue.

	  int  dequeue()         - Remove data from the circular queue
	  						   and return it.

	  bool empty()           - Check if the circular queue is empty. 

	  bool full()            - Check if the circular queue is full.

	  int  max_capacity()    - Return the maximum size of the circular
	  						   queue.

	  int  current_size()    - Return the current fill level.

	  circular_q& operator=(circular_q &&data)
	  						- Move equals operator, allowing deep copy 
	  						  and movement of data from one instance of
	  						  the class to another.
********************************************/

#ifndef CIRCQ_H
#define CIRCQ_H

#include <memory> //Needed for use of smart pointers.

using namespace std;

/********************************************
Class Name: 		    shmem_struct
Author: 				del_dilettante
Date: 					09/26/2020
Parameters:
	Defined and their uses commented within the class.

Description:
	A class to create and operate upon a circular queue of integers.
********************************************/

class circular_q
{
private:
  unique_ptr<int[]> buffer; //Create a smart pointer to an int array
  int head; //The int value to store the index value of the head of the q 
  int tail; //The int value to store the index value of the tail of the q
  int buff_size; //Size of the circular queue 
  bool isFull; //Boolean value of whether the buffer is full or not.

public:

  /*
  Constructor for the circular queue object, only requires the max_size
  the rest are default values.
  */
  circular_q(int size) :
          buffer(unique_ptr<int[]>(new int[size])),
          buff_size(size),
          head(0),
          tail(0),
          isFull(false)
  {}

  /*
  Public class methods, each declared here and defined outside the class.
  See definition for detailed description.
  */
  void enqueue(int data);
  int  dequeue();
  bool empty();
  bool full();
  int  max_capacity();
  int  current_size();
  circular_q& operator=(circular_q &&data);
};

/********************************************
Method Name: 		    enqueue()
Author: 				del_dilettante
Date: 					09/26/2020
Parameters:
	I/P int  The int value to be enqueued into the buffer/queue.

Description:
	Stores the data at the index pointed to by head currently.
	Increments the head index val.
	Increments the tail index val only if the queue was already full,
	this effectively 'overwrites' the data.
	Checks to see if this was the element that caused queue to fill up,
	if so, it makes the isFull boolean val true.
********************************************/
void circular_q::enqueue(int data)
{

  //Store data at head index in the queue
  buffer[head] = data; 

  /*
  increment head, and if head >= than buffer max size,
  wrap it around. Thus the modulo op. 
  */
  head = (head + 1) % buff_size;  

  //Check to see if the queue was already full before the addition
  if(isFull)
  {
    /*
    Increment tail, and if tail >= max buffer size,
    wrap it around. Thus, the modulo op.
    */
    tail = (tail + 1) % buff_size;	
  } 

  /*
  If this enq op cause the buffer to become full or 
  if it was already full then set the isFull flag to
  true.
  */
  isFull = head == tail;
}


/********************************************
Method Name: 		    dequeue()
Author: 				del_dilettante
Date: 					09/26/2020
Parameters:
	O/P int  The int value dequeued from the buffer/queue.

Description:
	Reads the data at the index pointed to by tail currently.
	Increments the tail index val.
	
********************************************/

int circular_q::dequeue()
{
  //Read data pointed to by the tail index value.
  int data = buffer[tail];

  /*
  Set the isFull flag to false, remains false if it was so
  or becomes false if the queue was full and one item is taken
  out.
  */
  isFull = false;

  /*
  Increment tail, and if tail >= max buffer size,
  wrap it around. Thus, the modulo op.
  */
  tail = (tail + 1) % buff_size;
	
  //return the int value that has been dequeued.
  return data;
}

/********************************************
Method Name: 		    empty()
Author: 				del_dilettante
Date: 					09/26/2020
Parameters:
	O/P bool  Whether the buffer/queue is empty or not.

Description:
	Checks if the buffer is empty.
********************************************/

bool circular_q::empty()
{
  /*
  The buffer is empty if it isn't full and the head and tail 
  index vals have become equal.
  */
  return (!isFull && (tail==head));
}


/********************************************
Method Name: 		    full()
Author: 				del_dilettante
Date: 					09/26/2020
Parameters:
	O/P bool  Whether the buffer/queue is full or not.

Description:
	Checks if the buffer is full.
********************************************/

bool circular_q::full()
{
  //return the value of the private member variable isFull
  return isFull;
}



/********************************************
Method Name: 		    max_capacity()
Author: 				del_dilettante
Date: 					09/26/2020
Parameters:
	O/P int  Max size of the buffer/queue.

Description:
	Returns int val of max buffer size.
********************************************/

int circular_q::max_capacity()
{
  //return the val of private data member buff_size
  return buff_size;
}


/********************************************
Method Name: 		    current_size()
Author: 				del_dilettante
Date: 					09/26/2020
Parameters:
	O/P int  The current fill level of buffer/queue.

Description:
	Returns the number of elements currently in the buffer.
********************************************/

int circular_q::current_size()
{
  //Set current size to max size, this is returned if the buffer is full.
  int curr_size = buff_size;

  //If the buffer is not full, check the following conditions.
  if(!isFull)
  {
      //Is the head idx geq tail idx, the curr size is their diff.
      if(head >= tail)
      {
        curr_size = head - tail;
      }
      /*
      Is the head idx less than tail idx, then curr size is their diff
      plus the max size.
      */
      else
      {
        curr_size = buff_size + head - tail;
      }
  }
  
  //return current fill level.
  return curr_size;
}

/********************************************
Method Name: 		    circular_q::operator=()
Author: 				del_dilettante
Date: 					09/26/2020
Parameters:
	O/P circular_q&  - A pass by reference of a circular_q object.
	I/P circular_q&& - A circular_q object converted to a rvalue reference

Description:
	Move assignment operator to allow passing of an rvalue reference
	instead of a const lvalue reference. This is necessited since
	the default operator= is delete for smart pointers. Smart
	pointers can only be moved (single ownership) and thus our class
	object has to be moved since it has a smart pointer data member.
********************************************/

circular_q& circular_q::operator=(circular_q &&data)
{
  /*
  Move the smart pointer to the new object using move semantics of C++11,
  coverting a const lvalue to a rvalue reference.
  */
  buffer = move(data.buffer);

  /*Copy head idx val as is*/
  head = data.head;

  /*Copy tail idx val as is*/
  tail = data.tail;

  /*Copy buffer size as is*/
  buff_size = data.buff_size;

  /*
  Dereference 'this' pointer the object has to itself are return the
  object by reference
  */
  return *this;
}

#endif
