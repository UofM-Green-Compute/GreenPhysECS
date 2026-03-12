import os
import numpy as np
import matplotlib.pyplot as plt 

# Set data path
root_folder = os.path.dirname(os.path.dirname(__file__))
data_path_SIR = os.path.join(root_folder, "outputs", "SIR.txt")

# ********** Read in data **********
# --- "Normal" SIR ---
SIR_data = np.genfromtxt(data_path_SIR, delimiter = ',', skip_header = 1)
# --- Phase portrait --- 
data = []
for i in range(10):
    data_path_SIR = os.path.join(root_folder, "outputs", f"SIR_{i}.txt")
    data.append(np.genfromtxt(data_path_SIR, delimiter = ',', skip_header = 1))

# ********** Graph Plotting **********
figSIR, axSIR = plt.subplots()
figPhase, axPhase = plt.subplots()
# --- Single SIR --- 
axSIR.plot(SIR_data[:,0],SIR_data[:,1],label='Susceptible')
axSIR.plot(SIR_data[:,0],SIR_data[:,2],label='Infected')
axSIR.plot(SIR_data[:,0],SIR_data[:,3],label='Recovered')
figSIR.legend(loc='upper center', ncol=3) 
# --- Single SIR Phase Portrait ---  
figPhase, axPhase = plt.subplots()
axPhase.plot(SIR_data[:,1],SIR_data[:,2],color='C0')
axPhase.set_xlabel("Susceptible (S)")
axPhase.set_ylabel("Infected (I)")
# --- Phase Portrait ---  
for l in range(10):
    axPhase.plot(data[l][:,1],data[l][:,2],color='C0')

figPhase.savefig("SIR_Phase_Portrait")
plt.show()
