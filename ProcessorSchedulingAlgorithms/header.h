/********************************************
File Name: 			        header.h
Author: 				        del_dilettante
Last Modification Date: 11/8/2020
Last Modifier:          del_dilettante

Struct:
	 simInfo - A structure to hold the total of the turnaround times and the normalise turnaround
             times for a given simulation.
Class:
    simulation - A class object that represents in simulation instance.

Procedures: Members of the simulation class.

simulation (vector<int>&)
    - The constructor for the simulation class. Takes a vector of ints by reference as an input.

simInfo& runSimulation()
    - The core logic of the program. Runs the simulation on a set of 1000 processes using the four
      scheduling algos, FCFS, RR, HRRN and FB.
********************************************/

#include "process_rds190000.h"
#ifndef HEADER_RDS190000_H
#define HEADER_RDS190000_H

/* Macro denoting the size of one simulation */
#define sim_size 1000

/*Macro denoting the number of time steps per simulation*/
#define sim_time 20000

/*Macro denoting the total number of simulations to be run */
#define num_sims 1000

/*Macro denoting the number of queues in the feedback algorithm*/
#define no_rqs 20

/********************************************
Structure Name: 		simInfo
Author: 				    del_dilettante
Date: 					    11/8/2020
Parameters:
	Defined and their uses commented within the struct.

Description:
          A structure to hold the total of the turnaround times and the normalised
          turnaround times for a given simulation.
********************************************/
struct simInfo
{
    /* Total TaTs for all 4 algos */
    int totalTat[4];
    /* Total Normalized TaTs for all 4 algos */
    float totalNorTat[4];
};

/********************************************
Class Name: 		    simulation
Author: 				    del_dilettante
Date: 					    11/8/2020
Parameters:
	Defined and their uses commented within the struct.

Description:
          A structure to hold the total of the turnaround times and the normalised
          turnaround times for a given simulation.
********************************************/
class simulation
{
public:
    /* An array of processes that'll be entering the system */
    process in_proc[sim_size];
    /* The service times for the processes */
    int     ser_times[sim_size];
    /* The TaTs and NorTats for the 4 algos */
    simInfo simInfoInstance;

    /* Constructor */
    simulation(vector<int>&);

    /* Core logic of the program, simulate scheduling algos */
    simInfo& runSimulation();
};

/********************************************
Procedure Name: 		simulation(vector<int>&)
Author: 				    del_dilettante
Date: 					    11/8/2020
Parameters:
	Defined and their uses commented within the procedure.

Description:
          The constructor for the simulation class. Takes a vector of ints by reference as an input.
********************************************/
simulation::simulation(vector<int>& times)
{
    /* Instantiate and store 1000 process objects in an array */
    for (int i = 0; i < sim_size; i++)
    {
        ser_times[i] = times[i];
        char buffer[100];
        sprintf(buffer, "%d", i);
        string temp = buffer;
        process x(temp, 0, ser_times[i]);
        in_proc[i] = x;
    }

    /* Initialise the totals of TaTs and NorTats to 0*/
    for(int i = 0; i < 4; i++)
    {
        simInfoInstance.totalNorTat[i] = 0;
        simInfoInstance.totalTat[i] = 0;
    }

}

/********************************************
Procedure Name: 		runSimulation()
Author: 				    del_dilettante
Date: 					    11/8/2020
Parameters:
	Defined and their uses commented within the procedure.

Description:
  The core logic of the program. Runs the simulation on a set of 1000 processes using the four
  scheduling algos, FCFS, RR, HRRN and FB.
********************************************/

simInfo& simulation::runSimulation()
{
    /* FCFS data */
    queue<process> fcfs;
    vector<process> fcfs_done;

    /* RR data */
    queue<process> rr;
    vector<process> rr_done;

    /* HRRN data */
    vector<process> hrrn_q;
    vector<process>::iterator current_process;
    vector<process> hrrn_done;
    process curr_copy;
    bool processing = false;

    /* FB data */
    vector<queue<process> > rq;
    vector<process> fb_done;
    process temp;

    /* Initialise an empty queue for each level of the rq for feedback algo*/
    for (size_t i = 0; i < no_rqs; i++)
    {
        queue<process> x;
        rq.push_back(x);
    }

    int proc_counter = 0;

    /*The four algorithms run as concurrent threads operating on 4 copies of the same
      processes arriving at the same time. */
    for (int current_time = 0; current_time < sim_time; current_time++)
    {
        /* FB thread */
        /* Contains the algorithm for the feedback algorithm
           Loop over all the queues rq0 through rq19 */
        for(int i = 0; i < no_rqs; i++)
        {
            //If a queue isn't empty and the time isn't zero
            if(!rq[i].empty() && current_time != 0)
            {
                //Next process to be executed is the one at the front of this queue
                temp = rq[i].front();
                //Pop it out from the queue
                rq[i].pop();
                //Reduce the service time left (process it for q=1)
                temp.service_left--;
                //If the process has complete execution
                if (temp.service_left==0)
                {
                    //Update finish time
                    temp.finish_time = current_time;
                    //Update turnaround time
                    temp.ta_time = current_time - temp.arrival_time;
                    //Put it in the vector containing processes that have finished executing
                    fb_done.push_back(temp);
                    //Exit the loop since work for this time step is over.
                    break;
                }
                //If the process still has some service left
                else
                {
                    //Put it in the next lower priority queue.
                    if (i != no_rqs-1)
                    {
                        rq[i+1].push(temp);
                    }
                    //If it is already in the lowest priority queue then put it at the back of it.
                    else
                    {
                        rq[i].push(temp);
                    }
                    //Exit the loop since work for this time step is over.
                    break;
                }
            }
        }

        /* Common Code region for all algos to add the next process */
        /* For the first 1000 time steps a process arrives at each time step
           Add a copy of the process object to the queues for each algorithm */
        if(current_time < sim_size)
        {
            process a = in_proc[proc_counter];
            process b = in_proc[proc_counter];
            process c = in_proc[proc_counter];
            process d = in_proc[proc_counter];
            //Set the arrival times for the process to the current time.
            a.arrival_time = current_time;
            b.arrival_time = current_time;
            c.arrival_time = current_time;
            d.arrival_time = current_time;
            // Add the process to each of the queues.
            fcfs.push(a);
            rr.push(b);
            hrrn_q.push_back(c);
            rq[0].push(d);
            //Increment process id number
            proc_counter++;
        }


        /*FCFS thread*/
        /* The algorithm for the first come first serve policy. */
        //If the process queue isn't empty and time != 0
        if(!fcfs.empty() && current_time != 0)
        {
            //If this is the last time quantum of service for a process
            if(fcfs.front().service_left == 1)
            {
                fcfs.front().service_left--;
                //Update finish time
                fcfs.front().finish_time = current_time;
                //Update turnaround time
                fcfs.front().ta_time = current_time - fcfs.front().arrival_time;
                //Create a copy of the process
                process tempF(fcfs.front());
                /* Store the copy of the process into a vector of processes that have finished
                   executing */
                fcfs_done.push_back(tempF);
                /* Remove the process from the queue */
                fcfs.pop();
            }
            //If this isn't the last quantum of service
            else
            {
                //Reduce service time left by 1
                fcfs.front().service_left--;
            }
        }

        /* RR thread */
        /* This part of the code implements the round robin scheduling algo */
        //If the process queue isn't empty and time != 0
        if(!rr.empty() && current_time != 0)
        {
            //If this is the last time quantum of service for a process
            if(rr.front().service_left == 1)
            {
                rr.front().service_left--;
                //Update finish time
                rr.front().finish_time = current_time;
                //Update turnaround time
                rr.front().ta_time = current_time - rr.front().arrival_time;
                //Create a copy of the process
                process tempG(rr.front());
                /* Store the copy of the process into a vector of processes that have finished
                   executing */
                rr_done.push_back(tempG);
                /* Remove the process from the queue */
                rr.pop();
            }
            //If this isn't the last quantum of service
            else
            {
                //Reduce service time left by 1
                rr.front().service_left--;
                //Create a copy of the process
                process tempX = rr.front();
                /* Remove the process from the queue */
                rr.pop();
                /*Add the process back at the end of the queue*/
                rr.push(tempX);
            }
        }

        /* HRRN thread */
        /*This part of the code implements the highest response ration next scheduling
          algorithm. */

        //If there is process currently executing in the hrrn core.
        if(processing)
        {
            //Check if this is the last quantum of service for the process
            if (curr_copy.service_left == 1)
            {
                curr_copy.service_left--;
                //Update finish time
                curr_copy.finish_time = current_time;
                //Update turnaround time.
                curr_copy.ta_time = current_time - curr_copy.arrival_time;
                //Add the process to the list of those that have finished executing
                hrrn_done.push_back(curr_copy);
                //Set the processor busy flag to false
                processing = false;
            }
            //If this isn't the last time step of service for the process
            else
            {
                //Reduce the service time left by 1.
                curr_copy.service_left--;
            }

        }

        //If the hrrn queue isn't empty
        if(!hrrn_q.empty())
        {
            //If there is process currently executing in the hrrn core.
            if (processing)
            {
                for (vector<process>::iterator i = hrrn_q.begin(); i != hrrn_q.end(); i++)
                {
                    //Increment the wait times for all the process that are waiting in the queue
                    i->wait_time++;
                }
            }
            //If there is no process currently executing in the hrrn core.
            if (!processing)
            {
                //to store the max response ratio
                double max_rr = 0;
                //iterate over the queue of processes and find the max response ratio
                for (vector<process>::iterator i = hrrn_q.begin(); i != hrrn_q.end(); i++)
                {
                    if (i->getRR() > max_rr)
                    {
                        max_rr = i->getRR();
                        current_process = i;
                        curr_copy = *i;
                    }
                    //Update wait times for all the processes since they haven't yet been picked.
                    i->wait_time++;
                }

                //Remove the process with the highest response ratio from the queue as it is now processing
                hrrn_q.erase(current_process);
                /* Reduce the wait time only for this process that was incremented earlier since there is
                   no wait for it */
                curr_copy.wait_time--;
                /* Since there is a process in the hrrn core set the procesing flag to true */
                processing = true;
            }
        }
    } //End of core simulation.

    /* Update the totals of Tat and NorTat for this sim*/

    vector<process>::iterator a = fcfs_done.begin();
    vector<process>::iterator b = rr_done.begin();
    vector<process>::iterator c = hrrn_done.begin();
    vector<process>::iterator d = fb_done.begin();

    //Calculate the total TaTs and NorTaTs of the processes for all four algorithms
    for (int i = 0; i < sim_size; i++)
    {
        simInfoInstance.totalTat[0] += a->finish_time - a->arrival_time;
        simInfoInstance.totalTat[1] += b->finish_time - b->arrival_time;
        simInfoInstance.totalTat[2] += c->finish_time - c->arrival_time;
        simInfoInstance.totalTat[3] += d->finish_time - d->arrival_time;

        simInfoInstance.totalNorTat[0] += (float)(a->finish_time - a->arrival_time) / a->service_time;
        simInfoInstance.totalNorTat[1] += (float)(b->finish_time - b->arrival_time) / b->service_time;
        simInfoInstance.totalNorTat[2] += (float)(c->finish_time - c->arrival_time) / c->service_time;
        simInfoInstance.totalNorTat[3] += (float)(d->finish_time - d->arrival_time) / d->service_time;

        a++;
        b++;
        c++;
        d++;
    }

    //Return the performance data.
    return simInfoInstance;
}




#endif // HEADER_H
