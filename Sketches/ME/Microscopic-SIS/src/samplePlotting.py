import os
import numpy as np
import matplotlib.pyplot as plt 
import networkx as nx

root_folder = os.path.dirname(os.path.dirname(__file__))
data_path_SIS = os.path.join(root_folder, "outputs", "SIS_1.txt")
savePath = os.path.join(root_folder, "outputs", "SISsample.pdf")

SIS_data = np.genfromtxt(data_path_SIS, delimiter = ',', skip_header = 0)

time = SIS_data[:,0]
susceptible = SIS_data[:,1]
infectious = SIS_data[:,2]

figSIR, axSIR = plt.subplots()
axSIR.plot(time,susceptible,label='Susceptible', color = 'tab:blue')
axSIR.plot(time,infectious,label='Infectious', color = 'tab:orange')
axSIR.set_xlabel("Time")
axSIR.set_ylabel("No. People")
figSIR.legend(loc='upper center', ncol=4) 
figSIR.savefig(savePath)
plt.show()
