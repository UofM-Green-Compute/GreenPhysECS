import os
import numpy as np
import matplotlib.pyplot as plt 
import networkx as nx

root_folder = os.path.dirname(os.path.dirname(__file__))
data_path_SEIR = os.path.join(root_folder, "outputs", "SEIR_1.txt")
savePath = os.path.join(root_folder, "outputs", "SEIRsample.pdf")

SEIR_data = np.genfromtxt(data_path_SEIR, delimiter = ',', skip_header = 0)

time = SEIR_data[:,0]
susceptible = SEIR_data[:,1]
exposed = SEIR_data[:,2]
infectious = SEIR_data[:,3]
recovered = SEIR_data[:,4]

figSIR, axSIR = plt.subplots()
axSIR.plot(time,susceptible,label='Susceptible', color = 'tab:blue')
axSIR.plot(time,exposed,label='Exposed', color = 'tab:red')
axSIR.plot(time,infectious,label='Infectious', color = 'tab:orange')
axSIR.plot(time,recovered,label='Recovered', color = 'tab:green')
axSIR.set_xlabel("Time")
axSIR.set_ylabel("No. People")
figSIR.legend(loc='upper center', ncol=4) 
figSIR.savefig(savePath)
plt.show()
