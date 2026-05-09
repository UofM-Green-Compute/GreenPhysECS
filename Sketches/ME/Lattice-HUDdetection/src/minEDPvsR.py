import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

Delta = 30
numberOfCrops = 100
numberOfSentinels = 5
numberOfEnsembleCopies = 1000

rootFolder = os.path.dirname(os.path.dirname(__file__))
outputFolder = os.path.join(rootFolder,"outputs")
savePathFigure = os.path.join(outputFolder, f"EDP(R).pdf")
savePathData = os.path.join(outputFolder, f"EDP(R).txt")

Rlist = np.array([0, 0.05, 0.1, 0.11, 0.12, 0.13, 0.14, 0.15, 0.16, 0.17, 0.18, 0.19, 0.2, 
                  0.21, 0.22, 0.23, 0.24, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7, 
                  0.75, 0.8, 0.85, 0.9, 0.95, 1.0, 1.05, 1.1, 1.15, 1.2, 1.25, 1.3, 1.35, 1.4]) # radii
saveData = np.empty((0, 3))
for i, radius in enumerate(Rlist):
    dataFolder = os.path.join(outputFolder,f"radius={radius:.2f}",
                              f"Pcrops{numberOfCrops}_Psentinels{numberOfSentinels}_"
                              f"NumberOfEnsembleCopies={numberOfEnsembleCopies}")
    EDPMeanPath = os.path.join(dataFolder, f"EDPmeanN={numberOfSentinels},Delta={Delta}.txt")
    EDPstdPath = os.path.join(dataFolder, f"EDPstdN={numberOfSentinels},Delta={Delta}.txt")
    EDPmean = np.genfromtxt(EDPMeanPath, delimiter = ',', skip_header = 1)[:,1]
    EDPstd = np.genfromtxt(EDPstdPath, delimiter = ',', skip_header = 1)[:,1]
    minEDP = 100*EDPmean[np.argmin(EDPmean)]
    minSEM = 100*EDPstd[np.argmin(EDPmean)]/np.sqrt(numberOfEnsembleCopies)
    saveData = np.vstack([saveData, [radius, minEDP, minSEM]])


headers = ['# Radius', 'Expected Disease Prevalence (%)', 'Standard Error']
EDP_df = pd.DataFrame(saveData, columns=headers)
EDP_df.to_csv(savePathData, index=False, sep=",")

plt.figure()
plt.errorbar(saveData[:,0], saveData[:,1], saveData[:,2], color='k', fmt = 'x', capsize=5)
plt.xlabel('Infection Radius')
plt.ylabel('Minimum Expected Detection Prevalence (%)')
plt.savefig(savePathFigure)
plt.show()
