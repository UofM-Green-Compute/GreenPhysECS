import os
import numpy as np
import matplotlib.pyplot as plt

rootFolder = os.path.dirname(os.path.dirname(__file__))
dataPathState1 = os.path.join(rootFolder, "outputs", "state1.txt")
savePathSIR = os.path.join(rootFolder, "outputs", "SIRensemble.pdf")

susceptibleData  = np.genfromtxt(dataPathState1, delimiter = ',', skip_header = 1)
susceptibleTime = susceptibleData[:,0]
susceptibleMean = susceptibleData[:,1]
susceptibleSTD = susceptibleData[:,2]
time = susceptibleTime
upperSusceptible = susceptibleMean + susceptibleSTD
lowerSusceptible = susceptibleMean - susceptibleSTD
figSIR, axSIR = plt.subplots()
axSIR.plot(time,susceptibleMean,label='Susceptible')
axSIR.plot(time,upperSusceptible)
axSIR.plot(time,lowerSusceptible)
figSIR.legend(loc='upper center', ncol=3)
figSIR.savefig(savePathSIR)
plt.show()