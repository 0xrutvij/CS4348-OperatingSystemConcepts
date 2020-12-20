/********************************************
File Name: 			        main.cc
Author: 				        del_dilettante
Last Modification Date: 11/8/2020
Last Modifier:          del_dilettante
Procedures:

	main() : The test porgram that runs the simulations as many times as specified by the 'num_sims' macro in
  the header file header.h.

********************************************/
#include "process_rds190000.h"
#include "header_rds190000.h"

int main()
{
    /* Create a vector of int vectors to store the process service times for a 1000 batches of 1000 processes */
    vector<vector<int> > times(num_sims, vector<int>(sim_size, 0));
    /* Stores the running total of the means of the Turnaround times for all 4 algos */
    float total_TatMean[4] = {0};
    /* Stores the running total of the means of the Normalised Turnaround times for all 4 algos */
    float total_NorTatMean[4] = {0};

    /*Code to generate normally distributed ints*/
    for (size_t i = 0; i < num_sims; i++)
    {
        /* Initialise a random device */
        random_device rd;
        /* Use a mersenne twister PRNG with the seed supplied by the random device */
        mt19937 gen(rd());
        /*Create a normal distribution with mean = 10 and standard deviation = 5 */
        normal_distribution<> dist(10, 5);
        /*This loop generates a 1000 *approximately* normally distributed ints
          Values that lie 2 standard devs beyond the mean are discarded. */
        for (int n = 0; n < sim_size; ++n)
        {
            int x;
            do
            {
              x = round(dist(gen));
            } while (x < 1 || x > 21);
            times[i][n] = x;
        }
    }

    cout << endl << endl;

    /*A string array containing the names of the algos*/
    string algos[4];
    algos[0] = "FCFS";
    algos[1] = "RR";
    algos[2] = "HRRN";
    algos[3] = "FB";


    /*Run the simulation 1000 times*/
    for (size_t i = 0; i < num_sims; i++)
    {
        /* Create a simulation object with one of the 1000 vectors of 1000 ints
           These 1000 ints will serve as the service time for the processes */
        simulation test(times[i]);

        /* An instance of the simInfo struct that is used to store the values returned
            by the simulation instance */
        simInfo x;

        /* Run the simulation */
        x = test.runSimulation();

        /* Calculate the mean Tat and mean NorTat for the simulation and add it to the total of means */
        for (int i = 0; i < 4; i++)
        {
             total_TatMean[i] += (double)x.totalTat[i]/sim_size;
             total_NorTatMean[i] += (double)x.totalNorTat[i]/sim_size;
        }

    }

    /* Calculate the mean of means for all the four algorithms and report the values to draw inferences */
    for (int i = 0; i < 4; i++)
    {
        cout << "Mean Tat for " << algos[i] << " = " << (double)total_TatMean[i]/num_sims << "\n"
             << "Mean NorTat for " << algos[i] << " = " << (double)total_NorTatMean[i]/num_sims << "\n"
             << endl;
    }

    return 0;
}
