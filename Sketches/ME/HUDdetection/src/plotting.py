import os
import numpy as np
import matplotlib.pyplot as plt

rootFolder = os.path.dirname(os.path.dirname(__file__))
savePathEDP = os.path.join(rootFolder, "outputs", "EPDandEDT.pdf")

# Expected Detection Prevalence
EDPMeanPath = os.path.join(rootFolder, "outputs", "EDPmean.txt")
EDPstdPath = os.path.join(rootFolder, "outputs", "EDPstd.txt")
Nsentinel = np.genfromtxt(EDPMeanPath, delimiter = ',', skip_header = 1)[:,0]
EDPmean = np.genfromtxt(EDPMeanPath, delimiter = ',', skip_header = 1)[:,1]
EDPstd = np.genfromtxt(EDPstdPath, delimiter = ',', skip_header = 1)[:,1]

#Expected Detection Time
EDTMeanPath = os.path.join(rootFolder, "outputs", "EDTmean.txt")
EDTstdPath = os.path.join(rootFolder, "outputs", "EDTstd.txt")
EDTmean = np.genfromtxt(EDTMeanPath, delimiter = ',', skip_header = 1)[:,1]
EDTstd = np.genfromtxt(EDTstdPath, delimiter = ',', skip_header = 1)[:,1]

plt.figure()
plt.errorbar(Nsentinel, EDPmean, EDPstd, color='k', linestyle = '--', fmt = 'x')
plt.xlabel('Number of Sentinel Plants in Sample')
plt.ylabel('Expected Detection Prevalance (%)')
plt.savefig(savePathEDP)
plt.show()