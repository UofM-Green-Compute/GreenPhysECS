import os
import numpy as np
import matplotlib.pyplot as plt

rootFolder = os.path.dirname(os.path.dirname(__file__))
savePathSIS = os.path.join(rootFolder, "outputs", "SEIRensemble.pdf")

#susceptible
susceptibleMeanPath = os.path.join(rootFolder, "outputs", "SusceptibleMean.txt")
susceptibleUpperPath = os.path.join(rootFolder, "outputs", "SusceptibleUpper.txt")
susceptibleLowerPath = os.path.join(rootFolder, "outputs", "SusceptibleLower.txt")
time = np.genfromtxt(susceptibleMeanPath, delimiter = ',', skip_header = 1)[:,0]
susceptibleMean  = np.genfromtxt(susceptibleMeanPath, delimiter = ',', skip_header = 1)[:,1]
upperSusceptible  = np.genfromtxt(susceptibleUpperPath, delimiter = ',', skip_header = 1)[:,1]
lowerSusceptible  = np.genfromtxt(susceptibleLowerPath, delimiter = ',', skip_header = 1)[:,1]

#exposed
exposedMeanPath = os.path.join(rootFolder, "outputs", "ExposedMean.txt")
exposedUpperPath = os.path.join(rootFolder, "outputs", "ExposedUpper.txt")
exposedLowerPath = os.path.join(rootFolder, "outputs", "ExposedLower.txt")
exposedMean  = np.genfromtxt(exposedMeanPath, delimiter = ',', skip_header = 1)[:,1]
upperExposed = np.genfromtxt(exposedUpperPath, delimiter = ',', skip_header = 1)[:,1]
lowerExposed  = np.genfromtxt(exposedLowerPath, delimiter = ',', skip_header = 1)[:,1]

#infected
infectedMeanPath = os.path.join(rootFolder, "outputs", "InfectedMean.txt")
infectedUpperPath = os.path.join(rootFolder, "outputs", "InfectedUpper.txt")
infectedLowerPath = os.path.join(rootFolder, "outputs", "InfectedLower.txt")
infectedMean  = np.genfromtxt(infectedMeanPath, delimiter = ',', skip_header = 1)[:,1]
upperInfected = np.genfromtxt(infectedUpperPath, delimiter = ',', skip_header = 1)[:,1]
lowerInfected  = np.genfromtxt(infectedLowerPath, delimiter = ',', skip_header = 1)[:,1]

#recovered
recoveredMeanPath = os.path.join(rootFolder, "outputs", "RecoveredMean.txt")
recoveredUpperPath = os.path.join(rootFolder, "outputs", "RecoveredUpper.txt")
recoveredLowerPath = os.path.join(rootFolder, "outputs", "RecoveredLower.txt")
recoveredMean  = np.genfromtxt(recoveredMeanPath, delimiter = ',', skip_header = 1)[:,1]
upperRecovered = np.genfromtxt(recoveredUpperPath, delimiter = ',', skip_header = 1)[:,1]
lowerRecovered = np.genfromtxt(recoveredLowerPath, delimiter = ',', skip_header = 1)[:,1]

fig, ax = plt.subplots()
#susceptible
ax.plot(time,susceptibleMean,label='Susceptible', color = 'tab:blue')
ax.plot(time,upperSusceptible, color = 'tab:blue', alpha = 0.5)
ax.plot(time,lowerSusceptible, color = 'tab:blue', alpha = 0.5)
ax.fill_between(time, susceptibleMean, upperSusceptible, color = 'tab:blue', alpha = 0.1)
ax.fill_between(time, susceptibleMean, lowerSusceptible, color = 'tab:blue', alpha = 0.1)
#exposed
ax.plot(time,exposedMean,label='Exposed', color = 'tab:red')
ax.plot(time,upperExposed, color = 'tab:red', alpha = 0.5)
ax.plot(time,lowerExposed, color = 'tab:red', alpha = 0.5)
ax.fill_between(time, exposedMean, upperExposed, color = 'tab:red', alpha = 0.1)
ax.fill_between(time, exposedMean, lowerExposed, color = 'tab:red', alpha = 0.1)
#infectious
ax.plot(time,infectedMean,label='Infectious', color = 'tab:orange')
ax.plot(time,upperInfected, color = 'tab:orange', alpha = 0.5)
ax.plot(time,lowerInfected, color = 'tab:orange', alpha = 0.5)
ax.fill_between(time, infectedMean, upperInfected, color = 'tab:orange', alpha = 0.1)
ax.fill_between(time, infectedMean, lowerInfected, color = 'tab:orange', alpha = 0.1)
#recovered
ax.plot(time,recoveredMean,label='Recovered', color = 'tab:green')
ax.plot(time,upperRecovered, color = 'tab:green', alpha = 0.5)
ax.plot(time,lowerRecovered, color = 'tab:green', alpha = 0.5)
ax.fill_between(time, recoveredMean, upperRecovered, color = 'tab:green', alpha = 0.1)
ax.fill_between(time, recoveredMean, lowerRecovered, color = 'tab:green', alpha = 0.1)
#legend
fig.legend(loc='upper center', ncol=3)
fig.savefig(savePathSIS)
plt.show()
