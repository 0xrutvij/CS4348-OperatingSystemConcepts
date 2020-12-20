/********************************************
File Name: 			main.cc
Author: 				del_dilettante
Last Modification Date: 10/18/2020
Last Modifier:          del_dilettante
Procedures:

	main() : The test porgram that runs the resource allocation algorithm and makes
  the procedure call to run the Banker's algorithm.

********************************************/
#include "header.h"

int main(int argc, char** argv)
{
  //Number of resources
  int m = 0;
  //Number of processes
  int n = 0;
  //A variable to hold values read in.
  int temp = 0;
  //File stream object to hold the file buffer opened.
  fstream fs;
  //State struct which holds the current state info for resource alloc and banker's algo.
  state info;

  //File string containing the name of the input file to be opened.
  char* file = argv[1];
  //Open the input file and create a copy of the buffer in fs object.
  fs.open(file, fstream::in);

  //Read the number of resources.
  fs >> m;

  //Read in the resource vector and create a copy of it in the available vector.
  for(int i = 0; i < m; i++)
  {
    fs >> temp;
    info.resource.push_back(temp);
    info.available.push_back(temp);
  }

  //Read the number of processes.
  fs >> n;

  //Read in the claim matrix.
  for(int i = 0; i < n; i++)
  {
    vector<int> x(m, 0);
    info.claim.push_back(x);

    for(int j = 0; j < m; j++)
    {
      fs >> temp;
      info.claim[i][j] = temp;
    }
  }

  //Read in the allocation matrix and calculate the request matrix and available vector using it.
  for(int i = 0; i < n; i++)
  {
    vector<int> x(m, 0);
    info.allocation.push_back(x);
    info.request.push_back(x);

    for(int j = 0; j < m; j++)
    {

      fs >> temp;
      info.allocation[i][j] = temp;
      info.available[j] -= temp;
      info.request[i][j] = info.claim[i][j] - temp;
    }
  }
  //Create a vector to hold the list of processes
  vector<int> processes(n, 0);
  //Create a vector to hold the list of suspended processes
  vector<int> suspended;
  //Create a string to hold the process allocation path.
  string route;

  //Assign the process numbers to the vector of processes.
  for(int i = 0; i < n; i++)
  {
      processes[i] = i;
  }

  //Integer to store the index of the current process.
  int cur_proc = 0;

  //Flag that informs whether or not a route that prevents deadlock was found.
  bool found_a_route = false;

  //While there are processes which aren't suspended...
  while(!processes.empty())
  {
      //Create an instance of the state struct to save the newstate.
      state newstate;
      //Copy the current state into the newstate object.
      state_copy(&newstate, &info, m);
      //Get a process from the list of processes.
      cur_proc = processes.front();
      //Delete that process from the list of processes.
      processes.erase(processes.begin());

      //If the request for this process is greater than the claim it had, then it is an error state.
      if(reqGTclaim(info, cur_proc, m))
      {
        cout << "Error State";
        continue;
      }
      //If the request for this process is greater than the resources available, then suspend it.
      else if(reqGTavail(info, cur_proc, m))
      {
        suspended.push_back(cur_proc);
        continue;
      }
      //Else, calculate and update newstate values.
      else
      {
          for(int i = 0; i < m; i++)
          {
              newstate.allocation[cur_proc][i] = info.allocation[cur_proc][i] + info.request[cur_proc][i];
              newstate.available[i] = info.available[i] - info.request[cur_proc][i];
          }
      }

      //Check whether the newstate leads to a potentially safe sequence of execution of the processes.
      if(safe(&newstate, n, m, cur_proc, suspended, processes, &route))
      {
        //If there exists a safe sequence mark the flag true.
        found_a_route = true;
        //Break the while loop and go to exit sequence..
        break;
      }
      else
      {
        //If no safe sequence is found then suspend the process.
        suspended.push_back(cur_proc);
        //Make the string object with the route empty for the next process to be checked.
        route = "";
      }

    }

    //If a sequence of process execution avoiding deadlock was found.
    if(found_a_route)
    {
      //Print the sequence as found.
      cout << "The sequence for preventing deadlock is: \n" << route << endl;
    }
    //If a sequence wasn't found.
    else
    {
      //The system encountered an unsafe state for all processes.
      cout << "The system was in an unsafe state.\n";
      //Print out the last process' route after which unsafe state was encountered.
      cout << route;
    }

    return 0;
}
