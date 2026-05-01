import os
import numpy as np
import matplotlib.pyplot as plt

Delta = 30
numberOfCrops = 100
numberOfSentinels = 5
numberOfEnsembleCopies = 1000

rootFolder = os.path.dirname(os.path.dirname(__file__))
outputFolder = os.path.join(rootFolder,"outputs")
savePathEDP = os.path.join(outputFolder, f"EDP(R).pdf")

Rlist = np.array([1.0, 1.2, 1.4]) # radii
minEDPlist = np.zeros_like(Rlist) # minimum expected detection prevalence 
minSEMlist = np.zeros_like(Rlist) # Standard Error on the mean for the min EDP case
for i, radius in enumerate(Rlist):
    dataFolder = os.path.join(outputFolder,f"radius={radius:.2f}",
                              f"Pcrops{numberOfCrops}_Psentinels{numberOfSentinels}_"
                              f"NumberOfEnsembleCopies={numberOfEnsembleCopies}")
    EDPMeanPath = os.path.join(dataFolder, f"EDPmeanN={numberOfSentinels},Delta={Delta}.txt")
    EDPstdPath = os.path.join(dataFolder, f"EDPstdN={numberOfSentinels},Delta={Delta}.txt")
    EDPmean = np.genfromtxt(EDPMeanPath, delimiter = ',', skip_header = 1)[:,1]
    EDPstd = np.genfromtxt(EDPstdPath, delimiter = ',', skip_header = 1)[:,1]
    minEDPlist[i] = EDPmean[np.argmin(EDPmean)]
    minSEMlist[i] = EDPstd[np.argmin(EDPmean)]/np.sqrt(numberOfEnsembleCopies)
    print(minEDPlist[i])
    print(minSEMlist[i])

plt.figure()
plt.errorbar(Rlist, 100*minEDPlist, 100*minSEMlist, color='k', fmt = 'x', capsize=10)
plt.xlabel('Infection Radius')
plt.ylabel('Minimum Expected Detection Prevalence (%)')
plt.savefig(savePathEDP)
plt.show()

