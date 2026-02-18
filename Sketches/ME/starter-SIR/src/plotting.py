import os
import numpy as np
import matplotlib.pyplot as plt

root_folder = os.path.dirname(os.path.dirname(__file__))
data_path_SIR = os.path.join(root_folder, "outputs", "SIR.txt")
save_path_SIR = os.path.join(root_folder, "outputs", "SIR.png")


SIR_count = np.genfromtxt(data_path_SIR, delimiter = ',', skip_header = 1)

figSIR, axSIR = plt.subplots()
axSIR.plot(SIR_count[:,0],SIR_count[:,1],label='Susceptible')
axSIR.plot(SIR_count[:,0],SIR_count[:,2],label='Infected')
axSIR.plot(SIR_count[:,0],SIR_count[:,3],label='Recovered')
figSIR.legend(loc='upper center', ncol=3)
figSIR.savefig(save_path_SIR)
plt.show()