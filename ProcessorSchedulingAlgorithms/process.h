/********************************************
File Name: 			        process.h
Author: 				        del_dilettante
Last Modification Date: 11/8/2020
Last Modifier:          del_dilettante

Class:
    process - An object that holds the information for a process.

Procedures: Members of the process class.

process()
    - The default constructor for process class

process(string, int, int)
    - The parametric constructor for process class

process(const process&)
    - The copy constructor for process class

float getRR();
    - A getter method to calculate the response ratio for a process

process& operator=(const process&);
    - A copy assignment operator for the process class.
********************************************/

#ifndef PROCESS_RDS190000_H
#define PROCESS_RDS190000_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <random>
#include <fstream>
using namespace std;

/********************************************
Class Name: 		    process
Author: 				    del_dilettante
Date: 					    11/8/2020
Parameters:
	Defined and their uses commented within the struct.

Description:
          An object that holds the information for a process.
********************************************/
class process
{
public:
    //Unique id for the process
    string pid;
    //Time when it arrived in the system
    int arrival_time;
    //Time when it finished executing
    int finish_time;
    //How many total time units of service will it need
    int service_time;
    //The turnaround time for the process
    int ta_time;
    //How many time units of service left
    int service_left;
    //How long has the process been waiting before execution
    int wait_time;

    // Default Constructor
    process();

    // Parametric Constructor
    process(string, int, int);

    // Copy Constructor
    process(const process&);

    // Get Response Ratio
    float getRR();

    // Overloaded Assigment Operator.
    process& operator=(const process&);
};

/********************************************
Procedure Name: 		process()
Author: 				    del_dilettante
Date: 					    11/8/2020
Parameters:
	Defined and their uses commented within the procedure.

Description:
          The default constructor for the process class.
********************************************/
process::process()
{
    pid = "";
    arrival_time = 0;
    finish_time = 0;
    service_left = 0;
    service_time = 0;
    ta_time = 0;
    wait_time = 0;
}


/********************************************
Procedure Name: 		process(string, int, int)
Author: 				    del_dilettante
Date: 					    11/8/2020
Parameters:
	Defined and their uses commented within the procedure.

Description:
          The parametric constructor for the process class. Takes in a process id string,
          along with arrival time and service time ints.
********************************************/
process::process(string x, int at, int st)
{
    pid = x;
    arrival_time = at;
    service_time = st;
    service_left = st;
    wait_time = 0;
    ta_time = 0;
    finish_time = 0;
}

/********************************************
Procedure Name: 		process(const process&)
Author: 				    del_dilettante
Date: 					    11/8/2020
Parameters:
	Defined and their uses commented within the procedure.

Description:
          The copy constructor for the process class. Takes in a const reference
          to the process that is to be copied.
********************************************/
process::process(const process& x)
{
    pid = x.pid;
    arrival_time = x.arrival_time;
    finish_time = x.finish_time;
    service_left = x.service_left;
    service_time = x.service_time;
    ta_time = x.ta_time;
    wait_time = x.wait_time;
}

/********************************************
Procedure Name: 		getRR()
Author: 				    del_dilettante
Date: 					    11/8/2020
Parameters:
	Defined and their uses commented within the procedure.

Description:
          A getter method to calculate the response ratio for a process.
          Return the response ratio as a float value.
********************************************/
float process::getRR()
{
    return (float) (wait_time + service_time)/service_time;
}

/********************************************
Procedure Name: 		process::operator=(const process&)
Author: 				    del_dilettante
Date: 					    11/8/2020
Parameters:
	Defined and their uses commented within the procedure.

Description:
          A copy assignment operator for the process class.
          Returns a reference to the process object to which
          the input const process was copied.
********************************************/
process& process::operator=(const process& x)
{
    pid = x.pid;
    arrival_time = x.arrival_time;
    finish_time = x.finish_time;
    service_left = x.service_left;
    service_time = x.service_time;
    ta_time = x.ta_time;
    wait_time = x.wait_time;
    return *this;
}


#endif // PROCESS_H
