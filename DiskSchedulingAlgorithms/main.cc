/********************************************
File Name: 			        main_rds190000.cc
Author: 				    del_dilettante
Last Modification Date:     11/21/2020
Last Modifier:              del_dilettante
Procedures:

	main() : The test porgram that runs the simulations as many times as specified by the 'num_sims' macro in
  the header file header_rds190000.h.

********************************************/
#include "header_rds190000.h"

int main()
{
    /*String array storing the names of the 4 algos*/
    string algos[4] = {"FIFO","LIFO","SSTF","SCAN"};
    /*The total of average times of for all simulations for each combination of request size and algorithm*/
    float total_avg_req_times[11][4] = {0};
    /*Instantiate a true random number generator*/
    random_device rd;
    /*Use the random device rd to seed the pseudo random number generator mersenne twister*/
    mt19937 gen(rd());
    /*Create a uniform random distribution of ints from 1 to 360 for the sector numbers*/
    uniform_int_distribution<int> sector(1, 360);
    /*Create a uniform random distribution of ints from 1 to 201 for the track numbers*/
    uniform_int_distribution<int> tracks(1, 201);
    /*
      Run 1000 simulations for each of the 11 request sizes
      Outer loop represents the request size
      Inner loop has the 1000 simulations.
    */
    for (size_t param = 50; param < 160; param+=10)
    {
        int reqIndex = (param-50)/10;
        for (int j = 0; j < num_sims; j++)
        {
            secTrackPair locs[param];

            /*
              Generate a set of random I/O requests using the mersenne twister genrator
              bounded by the distribution type and range.
            */
            for (size_t i = 0; i < param; i++)
            {
                locs[i].sectorLoc = sector(gen);
                locs[i].trackLoc = tracks(gen);
            }

            /* Instantiate a simulation instance with the given request array and number of requests*/
            simulation x(locs, param);

            /* Run the simulation */
            x.runSimulation();

            dataPoint* thisSimInfo;

            /* Get the data from the simulation for all four algos */
            thisSimInfo = x.getGraphDataPoints();
            /*Add the average times for the algos in this simulation to the total of averages for all sims*/
            for (size_t i = 0; i < 4; i++)
            {
                total_avg_req_times[reqIndex][i] += thisSimInfo[i].avgRequestTime;
            }  
        }
    }
    
    /* Instantiate a file stream object */
    fstream ops;
    /* Open the intermediate data file with the given name for output */
    ops.open("data.csv", fstream::out | fstream::trunc);
    /* Supply the names of the columns for the data in the csv file */
    ops << "NumReqs,FIFO,LIFO,SSTF,SCAN" << endl;

    /* Store and display the info for all the four algorithms' 11 request sizes and averages of a 1000 sims */
    for (int reqIndex = 0; reqIndex < 11; reqIndex++)
    {
        cout << "Stats for " << (reqIndex*10)+50 << " requests:\n\n";
        ops << (reqIndex*10)+50;
        for (size_t i = 0; i < 4; i++)
        {
            cout << algos[i] << ": Avg Request Time = " << total_avg_req_times[reqIndex][i]/num_sims << endl;
            ops << "," << total_avg_req_times[reqIndex][i]/num_sims;
        }
        cout << "--------------------------------------------------------------------" << endl;
        ops << endl;
    }
    
    /* Close the output csv file and exit program */
    ops.close();
    return 0;
}