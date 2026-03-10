import os
import numpy as np
import matplotlib.pyplot as plt

rootFolder = os.path.dirname(os.path.dirname(__file__))
dataPathState1 = os.path.join(rootFolder, "outputs", "sample.txt")
savePathSIR = os.path.join(rootFolder, "outputs", "SIRsample.pdf")

data  = np.genfromtxt(dataPathState1, delimiter = ',', skip_header = 1)
time = data[:,0]
susceptible = data[:,1]
infected = data[:,2]
recovered = data[:,3]

figSIR, axSIR = plt.subplots()
axSIR.plot(time, susceptible ,label='Susceptible')
axSIR.plot(time, infected, label = 'Infected')
axSIR.plot(time, recovered, label = 'Recovered')
figSIR.legend(loc='upper center', ncol=3)
figSIR.savefig(savePathSIR)
plt.show()