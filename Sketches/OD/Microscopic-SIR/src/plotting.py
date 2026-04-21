import os
import numpy as np
import matplotlib.pyplot as plt

rootFolder = os.path.dirname(os.path.dirname(__file__))
savePathSIR = os.path.join(rootFolder, "outputs", "SIRensemble.pdf")
savePathSIR2 = os.path.join(rootFolder, "outputs", "SIRensembleNE.pdf")

# ** Read in Susceptible data **
susceptibleMeanPathE = os.path.join(rootFolder, "outputs", "SusceptibleMeanE.txt")
susceptibleUpperPathE = os.path.join(rootFolder, "outputs", "SusceptibleUpperE.txt")
susceptibleLowerPathE = os.path.join(rootFolder, "outputs", "SusceptibleLowerE.txt")
susceptibleMeanE  = np.genfromtxt(susceptibleMeanPathE, delimiter = ',', skip_header = 1)[:,1]
upperSusceptibleE  = np.genfromtxt(susceptibleUpperPathE, delimiter = ',', skip_header = 1)[:,1]
lowerSusceptibleE  = np.genfromtxt(susceptibleLowerPathE, delimiter = ',', skip_header = 1)[:,1]

susceptibleMeanPathNE = os.path.join(rootFolder, "outputs", "SusceptibleMeanNE.txt")
susceptibleUpperPathNE= os.path.join(rootFolder, "outputs", "SusceptibleUpperNE.txt")
susceptibleLowerPathNE = os.path.join(rootFolder, "outputs", "SusceptibleLowerNE.txt")
susceptibleMeanNE  = np.genfromtxt(susceptibleMeanPathNE, delimiter = ',', skip_header = 1)[:,1]
upperSusceptibleNE  = np.genfromtxt(susceptibleUpperPathNE, delimiter = ',', skip_header = 1)[:,1]
lowerSusceptibleNE  = np.genfromtxt(susceptibleLowerPathNE, delimiter = ',', skip_header = 1)[:,1]

time = np.genfromtxt(susceptibleMeanPathE, delimiter = ',', skip_header = 1)[:,0]

# ** Read in Infected data **
infectedMeanPathE = os.path.join(rootFolder, "outputs", "InfectedMeanE.txt")
infectedUpperPathE = os.path.join(rootFolder, "outputs", "InfectedUpperE.txt")
infectedLowerPathE = os.path.join(rootFolder, "outputs", "InfectedLowerE.txt")
infectedMeanE  = np.genfromtxt(infectedMeanPathE, delimiter = ',', skip_header = 1)[:,1]
upperInfectedE = np.genfromtxt(infectedUpperPathE, delimiter = ',', skip_header = 1)[:,1]
lowerInfectedE  = np.genfromtxt(infectedLowerPathE, delimiter = ',', skip_header = 1)[:,1]

infectedMeanPathNE = os.path.join(rootFolder, "outputs", "InfectedMeanNE.txt")
infectedUpperPathNE = os.path.join(rootFolder, "outputs", "InfectedUpperNE.txt")
infectedLowerPathNE = os.path.join(rootFolder, "outputs", "InfectedLowerNE.txt")
infectedMeanNE  = np.genfromtxt(infectedMeanPathNE, delimiter = ',', skip_header = 1)[:,1]
upperInfectedNE = np.genfromtxt(infectedUpperPathNE, delimiter = ',', skip_header = 1)[:,1]
lowerInfectedNE  = np.genfromtxt(infectedLowerPathNE, delimiter = ',', skip_header = 1)[:,1]

# ** Read in Recovered data ** 
recoveredMeanPathE = os.path.join(rootFolder, "outputs", "RecoveredMeanE.txt")
recoveredUpperPathE = os.path.join(rootFolder, "outputs", "RecoveredUpperE.txt")
recoveredLowerPathE = os.path.join(rootFolder, "outputs", "RecoveredLowerE.txt")
recoveredMeanE  = np.genfromtxt(recoveredMeanPathE, delimiter = ',', skip_header = 1)[:,1]
upperRecoveredE = np.genfromtxt(recoveredUpperPathE, delimiter = ',', skip_header = 1)[:,1]
lowerRecoveredE  = np.genfromtxt(recoveredLowerPathE, delimiter = ',', skip_header = 1)[:,1]

recoveredMeanPathNE = os.path.join(rootFolder, "outputs", "RecoveredMeanNE.txt")
recoveredUpperPathNE = os.path.join(rootFolder, "outputs", "RecoveredUpperNE.txt")
recoveredLowerPathNE = os.path.join(rootFolder, "outputs", "RecoveredLowerNE.txt")
recoveredMeanNE  = np.genfromtxt(recoveredMeanPathNE, delimiter = ',', skip_header = 1)[:,1]
upperRecoveredNE = np.genfromtxt(recoveredUpperPathNE, delimiter = ',', skip_header = 1)[:,1]
lowerRecoveredNE  = np.genfromtxt(recoveredLowerPathNE, delimiter = ',', skip_header = 1)[:,1]

fig, ax = plt.subplots()
fig2, ax2 = plt.subplots()
#susceptibleE
ax.plot(time,susceptibleMeanE,label='SusceptibleE', color = 'tab:blue')
ax.plot(time,upperSusceptibleE, color = 'tab:blue', alpha = 0.5)
ax.plot(time,lowerSusceptibleE, color = 'tab:blue', alpha = 0.5)
ax.fill_between(time, susceptibleMeanE, upperSusceptibleE, color = 'tab:blue', alpha = 0.1)
ax.fill_between(time, susceptibleMeanE, lowerSusceptibleE, color = 'tab:blue', alpha = 0.1)
#susceptibleNE
ax2.plot(time,susceptibleMeanNE,label='SusceptibleNE', color = 'tab:blue')
ax2.plot(time,upperSusceptibleNE, color = 'tab:blue', alpha = 0.5)
ax2.plot(time,lowerSusceptibleNE, color = 'tab:blue', alpha = 0.5)
ax2.fill_between(time, susceptibleMeanNE, upperSusceptibleNE, color = 'tab:blue', alpha = 0.1)
ax2.fill_between(time, susceptibleMeanNE, lowerSusceptibleNE, color = 'tab:blue', alpha = 0.1)
#infectedE
ax.plot(time,infectedMeanE,label='InfectedE', color = 'tab:orange')
ax.plot(time,upperInfectedE, color = 'tab:orange', alpha = 0.5)
ax.plot(time,lowerInfectedE, color = 'tab:orange', alpha = 0.5)
ax.fill_between(time, infectedMeanE, upperInfectedE, color = 'tab:orange', alpha = 0.1)
ax.fill_between(time, infectedMeanE, lowerInfectedE, color = 'tab:orange', alpha = 0.1)
#infectedNE
ax2.plot(time,infectedMeanNE,label='InfectedNE', color = 'tab:orange')
ax2.plot(time,upperInfectedNE, color = 'tab:orange', alpha = 0.5)
ax2.plot(time,lowerInfectedNE, color = 'tab:orange', alpha = 0.5)
ax2.fill_between(time, infectedMeanNE, upperInfectedNE, color = 'tab:orange', alpha = 0.1)
ax2.fill_between(time, infectedMeanNE, lowerInfectedNE, color = 'tab:orange', alpha = 0.1)
#recoveredE
ax.plot(time,recoveredMeanE,label='RecoveredE', color = 'tab:green')
ax.plot(time,upperRecoveredE, color = 'tab:green', alpha = 0.5)
ax.plot(time,lowerRecoveredE, color = 'tab:green', alpha = 0.5)
ax.fill_between(time, recoveredMeanE, upperRecoveredE, color = 'tab:green', alpha = 0.1)
ax.fill_between(time, recoveredMeanE, lowerRecoveredE, color = 'tab:green', alpha = 0.1)
#recoveredNE
ax2.plot(time,recoveredMeanNE,label='RecoveredNE', color = 'tab:green')
ax2.plot(time,upperRecoveredNE, color = 'tab:green', alpha = 0.5)
ax2.plot(time,lowerRecoveredNE, color = 'tab:green', alpha = 0.5)
ax2.fill_between(time, recoveredMeanNE, upperRecoveredNE, color = 'tab:green', alpha = 0.1)
ax2.fill_between(time, recoveredMeanNE, lowerRecoveredNE, color = 'tab:green', alpha = 0.1)
#axis labels and limits
ax.set_xlabel("time (days)")
ax.set_ylabel("population (no. people)")
ax.set_ylim(0, 100)
ax.set_xlim(0,8)
ax2.set_xlabel("time (days)")
ax2.set_ylabel("population (no. people)")
ax2.set_ylim(0, 100)
ax2.set_xlim(0,8)
#legend
fig.legend(loc='upper center', ncol=3)
fig.savefig(savePathSIR)
fig2.legend(loc='upper center', ncol=3)
fig2.savefig(savePathSIR2)
plt.show()