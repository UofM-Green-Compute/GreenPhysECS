import os
import numpy as np
import matplotlib.pyplot as plt

NumberCrops = 200
NumberSentinels = 10
Delta=30
Radius="1.41"
NumberOfCopies = 100

rootFolder = os.path.dirname(os.path.dirname(__file__))

folder = os.path.join(rootFolder, "outputs", f"radius={Radius}", 
                      f"Pcrops{NumberCrops}_Psentinels{NumberSentinels}"
                      f"_NumberOfEnsembleCopies={NumberOfCopies}")
savePathEDP = os.path.join(folder, f"EDP,N={NumberSentinels},Delta={Delta}.pdf")
baselineFolder = os.path.join(rootFolder, "outputs", f"radius={Radius}", 
                              f"Pcrops{NumberCrops}_Psentinels{0}"
                              f"_NumberOfEnsembleCopies={NumberOfCopies}")

# Expected Detection Prevalence
EDPMeanPath = os.path.join(folder, f"EDPmeanN={NumberSentinels},Delta={Delta}.txt")
EDPstdPath = os.path.join(folder, f"EDPstdN={NumberSentinels},Delta={Delta}.txt")
EDPMeanBaselinePath = os.path.join(baselineFolder, f"EDPmeanN={NumberSentinels},Delta={Delta}.txt")
EDPstdBaselinePath = os.path.join(baselineFolder, f"EDPstdN={NumberSentinels},Delta={Delta}.txt")
Nsentinel = np.genfromtxt(EDPMeanPath, delimiter = ',', skip_header = 1)[:,0]
EDPmean = np.genfromtxt(EDPMeanPath, delimiter = ',', skip_header = 1)[:,1]
EDPstd = np.genfromtxt(EDPstdPath, delimiter = ',', skip_header = 1)[:,1]
baselineEDPmean = np.genfromtxt(EDPMeanBaselinePath, delimiter = ',', skip_header = 1)
baselineEDPstd = np.genfromtxt(EDPstdBaselinePath, delimiter = ',', skip_header = 1)
print(baselineEDPmean)

surveillanceSEM = EDPstd/np.sqrt(NumberOfCopies)
baselineSEM = EDPstd/np.sqrt(NumberOfCopies)

percentageChange = 100 * (EDPmean - baselineEDPmean) / baselineEDPmean
totalError = 100*np.sqrt((surveillanceSEM/baselineEDPmean)**2+
                         ((EDPmean*baselineSEM)/(baselineEDPmean*baselineEDPmean))**2)

plt.figure()
plt.errorbar(Nsentinel, percentageChange, totalError, color='k', linestyle = '--', fmt = 'x')
plt.plot(Nsentinel, 100 * (EDPmean - baselineEDPmean) / baselineEDPmean, color='k', linestyle = '--')
plt.scatter(Nsentinel, 100 * (EDPmean - baselineEDPmean) / baselineEDPmean, color='k', marker = 'x')
plt.xlabel('Number of Sentinel Plants in Sample')
plt.ylabel('Change in EDP from baseline (%)')
plt.savefig(savePathEDP)
plt.show()
