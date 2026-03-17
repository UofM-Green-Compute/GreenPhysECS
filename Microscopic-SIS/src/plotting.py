import os
import numpy as np
import matplotlib.pyplot as plt

rootFolder = os.path.dirname(os.path.dirname(__file__))
savePathSIS = os.path.join(rootFolder, "outputs", "SISensemble.pdf")

#susceptible
susceptibleMeanPath = os.path.join(rootFolder, "outputs", "SusceptibleMean.txt")
susceptibleUpperPath = os.path.join(rootFolder, "outputs", "SusceptibleUpper.txt")
susceptibleLowerPath = os.path.join(rootFolder, "outputs", "SusceptibleLower.txt")
time = np.genfromtxt(susceptibleMeanPath, delimiter = ',', skip_header = 1)[:,0]
susceptibleMean  = np.genfromtxt(susceptibleMeanPath, delimiter = ',', skip_header = 1)[:,1]
upperSusceptible  = np.genfromtxt(susceptibleUpperPath, delimiter = ',', skip_header = 1)[:,1]
lowerSusceptible  = np.genfromtxt(susceptibleLowerPath, delimiter = ',', skip_header = 1)[:,1]

#susceptible
infectedMeanPath = os.path.join(rootFolder, "outputs", "InfectedMean.txt")
infectedUpperPath = os.path.join(rootFolder, "outputs", "InfectedUpper.txt")
infectedLowerPath = os.path.join(rootFolder, "outputs", "InfectedLower.txt")
infectedMean  = np.genfromtxt(infectedMeanPath, delimiter = ',', skip_header = 1)[:,1]
upperInfected = np.genfromtxt(infectedUpperPath, delimiter = ',', skip_header = 1)[:,1]
lowerInfected  = np.genfromtxt(infectedLowerPath, delimiter = ',', skip_header = 1)[:,1]

fig, ax = plt.subplots()
#susceptible
ax.plot(time,susceptibleMean,label='Susceptible', color = 'tab:blue')
ax.plot(time,upperSusceptible, color = 'tab:blue', alpha = 0.5)
ax.plot(time,lowerSusceptible, color = 'tab:blue', alpha = 0.5)
ax.fill_between(time, susceptibleMean, upperSusceptible, color = 'tab:blue', alpha = 0.1)
ax.fill_between(time, susceptibleMean, lowerSusceptible, color = 'tab:blue', alpha = 0.1)
#infected
ax.plot(time,infectedMean,label='Infected', color = 'tab:orange')
ax.plot(time,upperInfected, color = 'tab:orange', alpha = 0.5)
ax.plot(time,lowerInfected, color = 'tab:orange', alpha = 0.5)
ax.fill_between(time, infectedMean, upperInfected, color = 'tab:orange', alpha = 0.1)
ax.fill_between(time, infectedMean, lowerInfected, color = 'tab:orange', alpha = 0.1)
#legend
fig.legend(loc='upper center', ncol=3)
fig.savefig(savePathSIS)
plt.show()
