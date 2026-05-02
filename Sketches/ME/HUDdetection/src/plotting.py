import os
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

NumberSample = 25000
NumberCrops = 1000
NumberSentinels = 50
Delta=30

rootFolder = os.path.dirname(os.path.dirname(__file__))
folder = os.path.join(rootFolder, "outputs", f"Pcrops{NumberCrops}_Psentinels{NumberSentinels}")
savePathEDP = os.path.join(folder, f"EDP,N={NumberSentinels},Delta={Delta}.pdf")
baselineFolder = os.path.join(rootFolder, "outputs", f"Pcrops{NumberCrops}_Psentinels{0}")
savePathData = os.path.join(rootFolder, "outputs", f"EDPchange.txt")

# Expected Detection Prevalence
EDPMeanPath = os.path.join(folder, f"EDPmeanN={NumberSentinels},Delta={Delta}.txt")
EDPstdPath = os.path.join(folder, f"EDPstdN={NumberSentinels},Delta={Delta}.txt")
EDPMeanBaselinePath = os.path.join(baselineFolder, f"EDPmeanN={NumberSentinels},Delta={Delta}.txt")
EDPstdBaselinePath = os.path.join(baselineFolder, f"EDPstdN={NumberSentinels},Delta={Delta}.txt")
Nsentinel = np.genfromtxt(EDPMeanPath, delimiter = ',', skip_header = 1)[:,0]
EDPmean = np.genfromtxt(EDPMeanPath, delimiter = ',', skip_header = 1)[:,1]
EDPstd = np.genfromtxt(EDPstdPath, delimiter = ',', skip_header = 1)[:,1]
EDPSEM = EDPstd/np.sqrt(NumberSample)
baselineEDPmean = np.genfromtxt(EDPMeanBaselinePath, delimiter = ',', skip_header = 1)
baselineEDPstd = np.genfromtxt(EDPstdBaselinePath, delimiter = ',', skip_header = 1)
baselineSEM = baselineEDPstd/np.sqrt(NumberSample)

change = 100 * (EDPmean - baselineEDPmean) / baselineEDPmean
changeError = (100*EDPSEM)/baselineEDPmean

saveData = np.empty((0, 3))
for i in range(len(Nsentinel)):
    saveData = np.vstack([saveData, [Nsentinel[i], change[i], changeError[i]]])

headers = ['Number of Sentinels', 'Change in EDP from baseline (%)', 'Standard Error']
EDP_df = pd.DataFrame(saveData, columns=headers)
EDP_df.to_csv(savePathData, index=False, sep=",")

plt.figure()
#plt.errorbar(Nsentinel, EDPmean, EDPstd, color='k', linestyle = '--', fmt = 'x')
plt.errorbar(saveData[:,0], saveData[:,1], saveData[:,2], color='k', fmt = 'x', capsize=5)
plt.xlabel('Number of Sentinel Plants in Sample')
plt.ylabel('Change in EDP from baseline (%)')
plt.savefig(savePathEDP)
plt.show()
