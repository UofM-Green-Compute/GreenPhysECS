import os
import numpy as np
import matplotlib.pyplot as plt 

root_folder = os.path.dirname(os.path.dirname(__file__))
data_path_SEIR = os.path.join(root_folder, "outputs", "SEIR.txt")

SEIR_count = np.genfromtxt(data_path_SEIR, delimiter = ',', skip_header = 1)

figSEIR, axSEIR = plt.subplots()
axSEIR.plot(SEIR_count[:,0],SEIR_count[:,1],label='Susceptible')
axSEIR.plot(SEIR_count[:,0],SEIR_count[:,2],label='Exposed')
axSEIR.plot(SEIR_count[:,0],SEIR_count[:,3],label='Infected')
axSEIR.plot(SEIR_count[:,0],SEIR_count[:,4],label='Recovered')
axSEIR.set_xlabel("Time (t)")
axSEIR.set_ylabel("No. People")
figSEIR.legend(loc='upper center', ncol=4) 

figSEIR.savefig("SEIR")

plt.show()
