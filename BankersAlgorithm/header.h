/********************************************
File Name: 			header.h
Author: 				del_dilettante
Last Modification Date: 10/18/2020
Last Modifier:          del_dilettante
Struct:
	 state - A structure to hold the resource and available vectors.
           As well as the claim, allocation and the request matrices.

Procedures: Helper methods for the main program.

void print_matrix(const char * str, vector<vector<int> > matrix, int n, int m)
    - A helper method to print a matrix.

bool reqGTclaim(struct state &x, int cur_proc, int m)
    - A method to check whether or not a process's request for all resources is more than its claims.

bool reqGTavail(struct state &x, int cur_proc, int m)
    - A method to check whether or not a process's request for all resources is more than what is available.

void state_copy(struct state* s1, state* s2, int m)
    - A method to create a deep copy of a state struct object.

bool safe(struct state* s, int n, int m, int cur_proc, vector<int> suspended, vector<int> processes, string* route)
	  - The method that implements the Banker's algorithm.
********************************************/

#ifndef header_h
#define header_h

#include <stdio.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stdlib.h>

using namespace std;

/********************************************
Structure Name: 		state
Author: 				del_dilettante
Date: 					10/28/2020
Parameters:
	Defined and their uses commented within the struct.

Description:
        A structure to hold the resource and available vectors.
        As well as the claim, allocation and the request matrices.
********************************************/
struct state
{
  //resource vector
  vector<int> resource;
  //vector of available resources
  vector<int> available;
  //matrix of resource claims by each process
  vector<vector<int> > claim;
  //matrix of resource allocation to each process
  vector<vector<int> > allocation;
  //matrix of resource current request by each process
  vector<vector<int> > request;
};

/********************************************
Procedure Name: print_matrix()
Author: 				del_dilettante
Date: 					 10/18/2020
Parameters:
  - matrix = the 2d vector containing the matrix.
  - m & n = dimensions of the matrix.

Description:
	A helper method to print a matrix.
********************************************/
void print_matrix(const char * str, vector<vector<int> > matrix, int n, int m)
{
  cout << endl;
  cout << str << "\n";
  for(int i = 0; i < n; i++)
  {
      for (int j = 0; j < m; j++) {
          cout << matrix[i][j] << " ";
      }
      cout << endl;
  }
}

/********************************************
Procedure Name: reqGTclaim()
Author: 				del_dilettante
Date: 					 10/18/2020
Parameters:
  - x = pass by reference copy of the state struct/object.
  - cur_proc = the process whose resource claim and requests are to be verified.
  - m = the number of resources.

Description:
	A method to check whether or not a process's request for all resources is more than its claims.
********************************************/
bool reqGTclaim(struct state &x, int cur_proc, int m)
{
  bool ret_val = false;

  for(int i = 0; i < m; i++)
  {
    if(x.allocation[cur_proc][i] + x.request[cur_proc][i] > x.claim[cur_proc][i])
    {
      ret_val = true;
      break;
    }
  }

  return ret_val;
}

/********************************************
Procedure Name: reqGTavail()
Author: 				del_dilettante
Date: 					10/18/2020
Parameters:
  - x = pass by reference copy of the state struct/object.
  - cur_proc = the process whose resource claim and requests are to be verified.
  - m = the number of resources.

Description:
	A method to check whether or not a process's request for all resources is more than what is available.
********************************************/
bool reqGTavail(struct state &x, int cur_proc, int m)
{
  bool ret_val = false;

  for(int i = 0; i < m; i++)
  {
    if(x.request[cur_proc][i] > x.available[i])
    {
      ret_val = true;
      break;
    }
  }

  return ret_val;
}

/********************************************
Procedure Name: state_copy()
Author: 				del_dilettante
Date: 					 10/18/2020
Parameters:
  - s1 = the object where the copy is to be stored.
  - s2 = the object to be copied.
  - m = the size of the second dimensions.

Description:
	A method to create a deep copy of a state struct object.
********************************************/
void state_copy(struct state* s1, state* s2, int m)
{
  s1->resource = s2->resource;
  s1->available = s2->available;
  s1->claim = s2->claim;
  s1->allocation = s2->allocation;
  s1->request = s2->request;
  for(int i = 0; i < m; i++)
  {
    s1->claim[i] = s2->claim[i];
    s1->allocation[i] = s2->allocation[i];
    s1->request[i] = s2->request[i];
  }
}

/********************************************
Procedure Name: safe()
Author: 				del_dilettante
Date: 					 10/18/2020
Parameters:
  - s = the state object which has the initial state.
  - n & m = number of processes and resources.
  - cur_proc = the process in question
  - suspend = the vector of suspended processes
  - processes = the vector of processes yet to be executed.
  - route = a string to which the sequence of processes will be stored.
Description:
	The method that implements the Banker's algorithm.
********************************************/
bool safe(struct state* s, int n, int m, int cur_proc, vector<int> suspended, vector<int> processes, string* route)
{
    //Create a copy of vector containing the resources currently available.
    vector<int> current_available = s->available;

    //Create a copy of the state object.
    state x;
    state_copy(&x, s, m);

    //A vector of all the processes to be executed.
    vector<int> rest;

    //Char array to hold int to string conversions.
    char buffer[10];


    //Add all the processes; the current process, those in queue and those suspened to the rest vector.
    rest.push_back(cur_proc);

    for(int i = 0; i < suspended.size(); i++)
    {
        rest.push_back(suspended[i]);
    }


    for(int i = 0; i < processes.size(); i++)
    {
        rest.push_back(processes[i]);
    }

    //A flag denote whether the system is still in a safe state and can continue.
    bool possible = true;
    //An iterator object to hold the iterator for a vector container.
    vector<int>::iterator it;
    //A flag to denote whether a process has been found for which claim - allocation <= available resources
    bool found;

    while(possible)
    {
        found = false;
        //Among all the vectors remaining in the rest vector find one s.t. claim - allocation <= available resources
        for(it = rest.begin(); it != rest.end(); ++it)
        {
            bool all_true = false;
            for(int i = 0; i < m; i++)
            {
                if(x.claim[*it][i] - x.allocation[*it][i] <= current_available[i])
                {
                    all_true = true;
                }
                else
                {
                    all_true = false;
                    break;
                }
            }
            if(all_true)
            {
                found = true;
                break;
            }
        }

        //If such a vector was found, simulate allocation and update the state object's variables.
        if(found)
        {
            for(int i = 0; i < m; i++)
            {
                current_available[i] = current_available[i] + x.allocation[*it][i];
                x.allocation[*it][i] = 0;
                x.claim[*it][i] = 0;
                x.request[*it][i] = 0;
            }
            //add the current process as a part of the possible deadlock-free route.
            *route += "P";
            snprintf(buffer, 10, "%d", *it+1);
            *route += buffer;
            if(rest.size() > 1)
                *route += " -> ";

            //Print out the route up until now and the updated claim, allocation and request matrices.
            cout << *route;
            print_matrix("Claim Matrix", x.claim, n, m);
            print_matrix("Allocation Matrix", x.allocation, n, m);
            print_matrix("Request Matrix", x.request, n, m);

            //Print out the vector containing the resoruces currently available
            cout << "\nAvailable Vector\n";
            for(int i = 0; i < m; i++)
            {
                cout << current_available[i] << " ";
            }
            cout << endl << endl << "**************************************" << endl << endl;

            //As long the rest vector isn't empty delete the current process from it since it has
            // already executed.
            if(!rest.empty())
                rest.erase(it);
        }
        else
        {
            //If no safe route is found break the while loop and return.
            possible = false;
        }
    }

    /*
    If all the process could execute without the chance of a deadlock then the rest vector is empty
    and it is a safe state with a corresponding sequence. If an unsafe state was reached then the
    rest vector isn't empty and the same is reported back.
    */
    return rest.empty();
}


#endif /* header_h */
