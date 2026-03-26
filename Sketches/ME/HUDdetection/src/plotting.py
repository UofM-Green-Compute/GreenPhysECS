import os
import numpy as np
import matplotlib.pyplot as plt

NumberCrops = 1000
NumberSentinels = 50

rootFolder = os.path.dirname(os.path.dirname(__file__))
folder = os.path.join(rootFolder, "outputs", f"Pcrops{NumberCrops}_Psentinels{NumberSentinels}")
savePathEDP = os.path.join(folder, "EPD.pdf")

# Expected Detection Prevalence
EDPMeanPath = os.path.join(folder, "EPDmean.txt")
EDPstdPath = os.path.join(folder, "EPDstd.txt")
Nsentinel = np.genfromtxt(EDPMeanPath, delimiter = ',', skip_header = 1)[:,0]
EDPmean = np.genfromtxt(EDPMeanPath, delimiter = ',', skip_header = 1)[:,1]
EDPstd = np.genfromtxt(EDPstdPath, delimiter = ',', skip_header = 1)[:,1]

plt.figure()
#plt.errorbar(Nsentinel, EDPmean, EDPstd, color='k', linestyle = '--', fmt = 'x')
plt.plot(Nsentinel, EDPmean, color='k', linestyle = '--')
plt.scatter(Nsentinel, EDPmean, color='k', marker = 'x')
plt.xlabel('Number of Sentinel Plants in Sample')
plt.ylabel('Expected Detection Prevalance (%)')
plt.savefig(savePathEDP)
plt.show()