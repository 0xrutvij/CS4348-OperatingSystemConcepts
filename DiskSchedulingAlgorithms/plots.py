'''********************************************
File Name: 			        plots_rds190000.py
Author: 				    del_dilettante
Last Modification Date:     11/21/2020
Last Modifier:              del_dilettante
Description:
    A python3 script to parse the information from the csv file generated by the c++ program
    and plot the graphs for avg. request time vs number of requests for each of the four algorithms.
********************************************'''

import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

#Read in the csv file into a data frame using python pandas
df = pd.read_csv("data.csv")
#Convert the panda data frame into an array
arr = df.to_numpy()
#Create a plot figure with 4 subplots
fig, axs = plt.subplots(2, 2)

#Plot the FIFO graph in the upper left quad
axs[0, 0].plot(arr[:,0],arr[:,1], 'tab:orange', label='FIFO')
axs[0, 0].set_title('FIFO')
#Plot the LIFO graph in the upper right quad
axs[0, 1].plot(arr[:,0],arr[:,2], label='LIFO')
axs[0, 1].set_title('LIFO')
#Plot the SSTF graph in the lower left quad
axs[1, 0].plot(arr[:,0],arr[:,3], 'tab:green', label='SSTF')
axs[1, 0].set_title('SSTF')
#Plot the SCAN graph in the lower right quad
axs[1, 1].plot(arr[:,0],arr[:,4], 'tab:red', label='SCAN')
axs[1, 1].set_title('SCAN')

#Set the titles for the x and y axes
for ax in axs.flat:
    ax.set(xlabel='Number of Requests', ylabel='Average Request Time (ms)')
for ax in axs.flat:
    ax.label_outer()

#Show the plot and save it as the image file named
plt.savefig("compare.png")
plt.show()
#Clear the plot
plt.clf()

#Plot all the four algorithms separately and save them as image, also make them viewable.


plt.plot(arr[:,0],arr[:,1], 'tab:orange', label='FIFO')
plt.xlabel('Number of Requests')
plt.ylabel('Average Request Time (ms)')
plt.legend()
plt.savefig("fifo.png")
plt.show()
plt.clf()

plt.plot(arr[:,0],arr[:,2], label='LIFO')
plt.xlabel('Number of Requests')
plt.ylabel('Average Request Time (ms)')
plt.legend()
plt.savefig("lifo.png")
plt.show()
plt.clf()

plt.plot(arr[:,0],arr[:,3], 'tab:green', label='SSTF')
plt.xlabel('Number of Requests')
plt.ylabel('Average Request Time (ms)')
plt.legend()
plt.savefig("sstf.png")
plt.show()
plt.clf()

plt.plot(arr[:,0],arr[:,4], 'tab:red', label='SCAN')
plt.xlabel('Number of Requests')
plt.ylabel('Average Request Time (ms)')
plt.legend()
plt.savefig("scan.png")
plt.show()
plt.clf()