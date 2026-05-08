import os
import numpy as np
import matplotlib.pyplot as plt

rootFolder = os.path.dirname(os.path.dirname(__file__))
dataPathState1 = os.path.join(rootFolder, "outputs", "state1.txt")
dataPathState2 = os.path.join(rootFolder, "outputs", "state2.txt")
dataPathState3 = os.path.join(rootFolder, "outputs", "state3.txt")
savePathSIR = os.path.join(rootFolder, "outputs", "SIRensemble.pdf")

#susceptible
susceptibleData  = np.genfromtxt(dataPathState1, delimiter = ',', skip_header = 1)
susceptibleMean = susceptibleData[:,1]
lowerSusceptible = susceptibleData[:,2]
upperSusceptible = susceptibleData[:,3]

#infected
infectedData  = np.genfromtxt(dataPathState2, delimiter = ',', skip_header = 1)
infectedMean = infectedData[:,1]
lowerInfected = infectedData[:,2]
upperInfected = infectedData[:,3]

#recovered
recoveredData  = np.genfromtxt(dataPathState3, delimiter = ',', skip_header = 1)
recoveredMean = recoveredData[:,1]
lowerRecovered = recoveredData[:,2]
upperRecovered = recoveredData[:,3]

time = susceptibleData[:,0]

figSIR, axSIR = plt.subplots()
#susceptible
axSIR.plot(time,susceptibleMean,label='Susceptible', color = 'tab:blue')
axSIR.plot(time,upperSusceptible, color = 'tab:blue', alpha = 0.5)
axSIR.plot(time,lowerSusceptible, color = 'tab:blue', alpha = 0.5)
axSIR.fill_between(time, susceptibleMean, upperSusceptible, color = 'tab:blue', alpha = 0.1)
axSIR.fill_between(time, susceptibleMean, lowerSusceptible, color = 'tab:blue', alpha = 0.1)
#infected
axSIR.plot(time,infectedMean,label='Infected', color = 'tab:orange')
axSIR.plot(time,upperInfected, color = 'tab:orange', alpha = 0.5)
axSIR.plot(time,lowerInfected, color = 'tab:orange', alpha = 0.5)
axSIR.fill_between(time, infectedMean, upperInfected, color = 'tab:orange', alpha = 0.1)
axSIR.fill_between(time, infectedMean, lowerInfected, color = 'tab:orange', alpha = 0.1)
#recovered
axSIR.plot(time,recoveredMean,label='Recovered', color = 'tab:green')
axSIR.plot(time,upperRecovered, color = 'tab:green', alpha = 0.5)
axSIR.plot(time,lowerRecovered, color = 'tab:green', alpha = 0.5)
axSIR.fill_between(time, recoveredMean, upperRecovered, color = 'tab:green', alpha = 0.1)
axSIR.fill_between(time, recoveredMean, lowerRecovered, color = 'tab:green', alpha = 0.1)
#legend
figSIR.legend(loc='upper center', ncol=3)
figSIR.savefig(savePathSIR)
plt.show()
