import os
import numpy as np
import matplotlib.pyplot as plt 

# Set data path
root_folder = os.path.dirname(os.path.dirname(__file__))
data_path_SIS = os.path.join(root_folder, "outputs", "SIS.txt")

# ********** Read in data **********
# --- "Normal" SIS ---
SIS_data = np.genfromtxt(data_path_SIS, delimiter = ',', skip_header = 1)
# --- Phase portrait --- 
data = []
for i in range(10):
    data_path_SIS = os.path.join(root_folder, "outputs", f"SIS_{i}.txt")
    data.append(np.genfromtxt(data_path_SIS, delimiter = ',', skip_header = 1))

# ********** Graph Plotting **********
figSIS, axSIS = plt.subplots()
figPhase, axPhase = plt.subplots()
# --- Single SIS --- 
axSIS.plot(SIS_data[:,0],SIS_data[:,1],label='Susceptible')
axSIS.plot(SIS_data[:,0],SIS_data[:,2],label='Infected')
figSIS.legend(loc='upper center', ncol=3) 
# --- Single SIS Phase Portrait ---  
figPhase, axPhase = plt.subplots()
axPhase.plot(SIS_data[:,1],SIS_data[:,2],color='C0')
axPhase.set_xlabel("Susceptible (S)")
axPhase.set_ylabel("Infected (I)")
# --- Phase Portrait ---  
for l in range(10):
    axPhase.plot(data[l][:,1],data[l][:,2],color='C0')

figSIS.savefig("SIS")
figPhase.savefig("SIS_Phase_Portrait")
plt.show()
