/********************************************
File Name: 			    header_rds190000.h
Author: 				del_dilettante
Last Modification Date: 11/21/2020
Last Modifier:          del_dilettante

Struct:
	dataPoint - A struct holding a single data point for the graph to be plotted.

	secTrackPair - A struct to hold the pair of track number and sector number that address
				   a certain block of memory.

Procedures: Helper methods

	increasing(secTrackPair i, j)
		- A boolean method supplied to the sorting algorithm to get a increasing sort of secTrackPairs

	decreasing(secTrackPair i, j)
		- A boolean method supplied to the sorting algorithm to get a decreasing sort of secTrackPairs
	
	calculateRequestTimeMS(secTrackPair src, dest)
		- A method to calculate the time to process a request given the initial and 
		  final locations of the cylinder head. Returns a float value of the time 
		  taken in milliseconds.

Class:
    simulation - A class to hold the details of one simulation instance. The instance is then simulated and
				 the data from in generated to plot the graph. 

Procedures: Members of the simulation class.

    simulation(secTrackPair*, int)
		- The parametric constructor for the simulation class.

    dataPoint* getGraphDataPoints()
		- A method to get the average time per request for all four algorithms

    void runSimulation()
		- A mehtod that runs the simulation instance with the list of requests provided.
********************************************/

#ifndef HEADER_RDS190000_H
#define HEADER_RDS190000_H

#include <list>
#include <vector>
#include <iostream>
#include <cmath>
#include <queue>
#include <string>
#include <random>
#include <fstream>
using namespace std;

/*Macro Constants*/

/* Macro denoting the total number of simulations per request count per algo*/
#define num_sims 1000
/* Macro denoting the rotational speed of HDD in RPM*/
#define rpm 12000
/* Macro denoting the rotational speed of HDD in RPS*/
#define rps rpm/60
/* Macro denoting the average seek time for the disk head*/
#define avgSeekTime 2.5
/* Macro denoting the transfer rate for the HDD in GB/s */
#define xferRate 6
/* Macro denoting the number of tracks in the HDD platter*/
#define numTracks 201
/* Macro denoting the number of sectors per track of the HDD*/
#define numSectors 360
/* Macro denoting the size of 1 block/sector in KB*/
#define blockSize 4

/********************************************
Structure Name: 		dataPoint
Author: 				del_dilettante
Date: 					11/21/2020
Parameters:
	Defined and their uses commented within the struct.

Description:
          A struct holding a single data point for the graph to be plotted.
********************************************/
struct dataPoint
{
	/*The average request time for all requests in one simulation*/
	float avgRequestTime;
	/*Total number of requests in that simulation*/
	int   numRequests;
};

/********************************************
Structure Name: 		secTrackPair
Author: 				del_dilettante
Date: 					11/21/2020
Parameters:
	Defined and their uses commented within the struct.

Description:
          A struct to hold the pair of track number and sector number that address
		  a certain block of memory.
********************************************/
struct secTrackPair
{
	/*Location of the track for a certain block*/
	int trackLoc;
	/*Location of the sector for a certain block/sector*/
	int sectorLoc;
};

/********************************************
Procedure Name: 		decreasing(secTrackPair i, j)
Author: 				del_dilettante
Date: 					11/21/2020
Parameters:
	Defined and their uses commented within the procedure.

Description:
          A boolean method supplied to the sorting algorithm to get a decreasing sort of secTrackPairs
********************************************/

bool decreasing(secTrackPair i, secTrackPair j)
{
	if (i.trackLoc == j.trackLoc)
	{
		return i.sectorLoc > j.sectorLoc;
	}
	else
	{
		return i.trackLoc > j.trackLoc;
	}
}

/********************************************
Procedure Name: 		increasing(secTrackPair i, j)
Author: 				del_dilettante
Date: 					11/21/2020
Parameters:
	Defined and their uses commented within the procedure.

Description:
          A boolean method supplied to the sorting algorithm to get a increasing sort of secTrackPairs
********************************************/
bool increasing(secTrackPair i, secTrackPair j)
{
	if (i.trackLoc == j.trackLoc)
	{
		return i.sectorLoc < j.sectorLoc;
	}
	else
	{
		return i.trackLoc < j.trackLoc;
	}
}

/********************************************
Procedure Name: 		calculateRequestTimeMS(secTrackPair src, dest)
Author: 				del_dilettante
Date: 					11/21/2020
Parameters:
	Defined and their uses commented within the procedure.

Description:
    A method to calculate the time to process a request given the initial and 
	final locations of the cylinder head. Returns a float value of the time 
	taken in milliseconds.
********************************************/
float calculateRequestTimeMS(secTrackPair src, secTrackPair dest)
{
	int diffTracks = abs(dest.trackLoc - src.trackLoc); //absolute diff between init and final tracks
	int diffSectors = abs(dest.sectorLoc - src.sectorLoc); //absolute diff between init and final sectors

	float seekSpeed = (avgSeekTime * 2)/numTracks; //unit: millisecond/track
	float seekTime = diffTracks * seekSpeed; //millisecond

	float time_per_rotation = 1/rps; //seconds
	float time_per_sector = time_per_rotation/numSectors; //seconds/sector
	float rotational_delay = diffSectors * time_per_sector * 1000; //milliseconds

	float xfer_time = (blockSize * exp2(10) * 1000) / (xferRate * pow(10, 9)); //milliseconds


	return (seekTime + rotational_delay + xfer_time); //total time for a request
}


/********************************************
Class Name: 		    simulation
Author: 				del_dilettante
Date: 					11/21/2020
Parameters:
	Defined and their uses commented within the struct.

Description:
	A class to hold the details of one simulation instance. The instance is then simulated and
	the data from in generated to plot the graph. 
********************************************/
class simulation
{
public:
	//Total number of requests
    int          numRequests; 

	//The set of randomized I/O requests, for small requests sizes the array is partially used
    secTrackPair requestQueue[150]; 
    
	//The current location of the cylinder head for all four algorithms
	secTrackPair curLoc[4];

	//The total time for all the reuqests processed for all four algorithms
    float        totalRequestTime[4];

	//The total number of tracks traversed by the cylinder head for all four algorithms
	int 		 totalTracksTraversed[4];

	//The 4 data points generated by one simulation, one for each algo.
    dataPoint    simInfo[4];
    

	//Constructor
    simulation(secTrackPair*, int);

	//A method to get the average time per request for all four algorithms
    dataPoint* getGraphDataPoints();

	//A mehtod that runs the simulation instance with the list of requests provided.
    void runSimulation();
};

/********************************************
Procedure Name: 		simulation(secTrackPair*, int)
Author: 				del_dilettante
Date: 					11/21/2020
Parameters:
	Defined and their uses commented within the procedure.

Description:
    The constructor for the simulation class. Takes an array of requests and the total number of
	requests as the input parameters.
********************************************/
simulation::simulation(secTrackPair* reqQ, int numReq)
{
	//Total number of requests
	numRequests = numReq;
	//Copy the random sector and track locations to the internal array.
	for(int i = 0; i < numRequests; i++)
	{
		requestQueue[i] = reqQ[i];
	}
	//Initialize the initial sector and track locations for all algos
	//And initialize the values of total request times and tracks traversed.
	for(int i = 0; i < 4; i++)
	{
		curLoc[i].trackLoc = 100;
		curLoc[i].sectorLoc = 0;
		totalRequestTime[i] = 0;
		totalTracksTraversed[i] = 0;		
	}
}

/********************************************
Procedure Name: 		getGraphDataPoints()
Author: 				del_dilettante
Date: 					11/21/2020
Parameters:
	Defined and their uses commented within the procedure.

Description:
    A method to get the average time per request for all four algorithms.
********************************************/
dataPoint* simulation::getGraphDataPoints()
{
	for(int i = 0; i < 4; i++)
		{
			simInfo[i].avgRequestTime = (float) totalRequestTime[i]/numRequests;
			simInfo[i].numRequests = numRequests;				
		}

	return simInfo;
}


/********************************************
Procedure Name: 		runSimulation()
Author: 				del_dilettante
Date: 					11/21/2020
Parameters:
	Defined and their uses commented within the procedure.

Description:
    A mehtod that runs the simulation instance with the list of requests provided.
********************************************/

void simulation::runSimulation()
{
	//Create a fifo queue
	queue<secTrackPair> fifo_queue;
	//Create a lifo stack
	vector<secTrackPair> lifo_queue;
	//Create a sstf queue
	vector<secTrackPair> sstf_queue;
	//Create a pair of lists for the scan algo
	list<secTrackPair> scan_upper, scan_lower;

	for (size_t i = 0; i < numRequests; i++)
	{
		secTrackPair fif, lif, sst, sca;

		fif.trackLoc = requestQueue[i].trackLoc;
		lif.trackLoc = requestQueue[i].trackLoc;
		sst.trackLoc = requestQueue[i].trackLoc;
		sca.trackLoc = requestQueue[i].trackLoc;

		fif.sectorLoc = requestQueue[i].sectorLoc;
		lif.sectorLoc = requestQueue[i].sectorLoc;
		sst.sectorLoc = requestQueue[i].sectorLoc;
		sca.sectorLoc = requestQueue[i].sectorLoc;

		//Add a request to the fifo queue
		fifo_queue.push(fif);
		//Add a request to the lifo stack
		lifo_queue.push_back(lif);
		//Add a reuquest to the sstf queue
		sstf_queue.push_back(sst);

		//If the request track is greater than starting track pos
		//then add it to the upper list for scan.
		if(sca.trackLoc > 100)
		{
			scan_upper.push_back(sca);
		}
		//else add it to the lower list for scan.
		else
		{
			scan_lower.push_back(sca);
		}
	}

	//Sort the upper list in ascending order
	scan_upper.sort(increasing);
	//Sort the lower list in descending order
	scan_lower.sort(decreasing);
	
	for (size_t z = 0; z < numRequests; z++)
	{
		//Debug Line//cout << z << "|";

		/*
		  Concurrent thread for FIFO
		  Simple queue based operation, the oldest request is processed first		
		*/
		secTrackPair currReq0;
		currReq0 = fifo_queue.front();
		totalRequestTime[0] += calculateRequestTimeMS(curLoc[0], currReq0);
		//Debug Line//cout << " FIFO = " <<  abs(curLoc[0].trackLoc - currReq0.trackLoc) << " |";
		totalTracksTraversed[0] += abs(curLoc[0].trackLoc - currReq0.trackLoc);
		curLoc[0].sectorLoc = currReq0.sectorLoc;
		curLoc[0].trackLoc = currReq0.trackLoc;
		fifo_queue.pop();


		/*
			Concurrent thread for LIFO
			Simple stack based operation, the newest request is processed first
		*/
		secTrackPair currReq1;
		currReq1 = lifo_queue.back();
		totalRequestTime[1] += calculateRequestTimeMS(curLoc[1], currReq1);
		//Debug Line//cout << " LIFO = " <<  abs(curLoc[1].trackLoc - currReq1.trackLoc) << " |";
		totalTracksTraversed[1] += abs(curLoc[1].trackLoc - currReq1.trackLoc);
		curLoc[1].sectorLoc = currReq1.sectorLoc;
		curLoc[1].trackLoc = currReq1.trackLoc;
		lifo_queue.pop_back();


		/*
		  Concurrent thread for SSTF
		  Finds the request in the queue that has the shortest service time
		  That is, it looks for the request whose block is closest to the
		  current location of the cylinder head.
		*/
		vector<secTrackPair>::iterator currReq2;
		int minDiffTrack = 202; 
		for (vector<secTrackPair>::iterator i = sstf_queue.begin(); i < sstf_queue.end(); i++)
		{
			int thisDiffTrack = abs(curLoc[2].trackLoc - i->trackLoc);
			if (thisDiffTrack < minDiffTrack)
			{
				minDiffTrack = thisDiffTrack;
				currReq2 = i;
			}
		}
		totalRequestTime[2] += calculateRequestTimeMS(curLoc[2], *currReq2);
		//Debug Line//cout << " SSTF = " <<  abs(curLoc[2].trackLoc - currReq2->trackLoc) << " |";
		totalTracksTraversed[2] += abs(curLoc[2].trackLoc - currReq2->trackLoc);
		curLoc[2].sectorLoc = currReq2->sectorLoc;
		curLoc[2].trackLoc = currReq2->trackLoc;
		sstf_queue.erase(currReq2);


		/*
		  Concurrent thread for SCAN
		  The requests greater than the starting pos of the cylinder head are sorted in an ascending order
		  Thus these locations are accessed in one straight motion away from the start
		  Once the last position is reached, the lower list's requests are accessed
		  These requests are in a descending order and a sweep in the opposite direction covers them all.
		*/
		secTrackPair currReq3;
		bool upper = true;
		if (!scan_upper.empty())
		{
			currReq3 = scan_upper.front();
		}
		else
		{
			currReq3 = scan_lower.front();
			upper = false;
		}
		totalRequestTime[3] += calculateRequestTimeMS(curLoc[3], currReq3);
		//Debug Line//cout << " SCAN = " <<  abs(curLoc[3].trackLoc - currReq3.trackLoc) << " |";
		totalTracksTraversed[3] += abs(curLoc[3].trackLoc - currReq3.trackLoc);
		curLoc[3].sectorLoc = currReq3.sectorLoc;
		curLoc[3].trackLoc = currReq3.trackLoc;
		if(upper)
		{
			scan_upper.pop_front();
		}
		else
		{
			scan_lower.pop_front();
		}
		//Debug Line//cout << endl;
	}

	//Debug Line//cout << "\n\nAvg seek length for FIFO = " << (float) totalTracksTraversed[0]/numRequests << endl;
	//Debug Line//cout << "Avg seek length for LIFO = " << (float) totalTracksTraversed[1]/numRequests << endl;
	//Debug Line//cout << "Avg seek length for SSTF = " << (float) totalTracksTraversed[2]/numRequests << endl; 
	//Debug Line//cout << "Avg seek length for SCAN = " << (float) totalTracksTraversed[3]/numRequests << endl;
}



#endif // HEADER_H


