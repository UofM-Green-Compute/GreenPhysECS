import os
import numpy as np
import matplotlib.pyplot as plt

rootFolder = os.path.dirname(os.path.dirname(__file__))
outputFolder = os.path.join(rootFolder, "outputs")
savePathHUD = os.path.join(outputFolder, "HUDensemble.pdf")

#Healthy Crops
healthyCropsMeanPath = os.path.join(rootFolder, "outputs", "cropH_mean.txt")
healthyCropsSTDPath = os.path.join(rootFolder, "outputs", "cropH_std.txt")
time = np.genfromtxt(healthyCropsMeanPath, delimiter = ',', skip_header = 1)[:,0]
healthyCropsMean = np.genfromtxt(healthyCropsMeanPath, delimiter = ',', skip_header = 1)[:,1]
healthyCropsSTD = np.genfromtxt(healthyCropsSTDPath, delimiter = ',', skip_header = 1)[:,1]
healthyCropsUpper = healthyCropsMean+healthyCropsSTD
healthyCropsLower = healthyCropsMean-healthyCropsSTD
savePath_cropsH_upper = os.path.join(outputFolder, "cropsH_upper.txt")
cropsH_upper = np.column_stack((time, healthyCropsUpper))
np.savetxt(savePath_cropsH_upper, cropsH_upper, delimiter=",")
savePath_cropsH_lower = os.path.join(outputFolder, "cropsH_lower.txt")
cropsH_lower = np.column_stack((time, healthyCropsLower))
np.savetxt(savePath_cropsH_lower, cropsH_lower, delimiter=",")

#Undetectable Crops
undetectableCropsMeanPath = os.path.join(rootFolder, "outputs", "cropU_mean.txt")
undetectableCropsSTDPath = os.path.join(rootFolder, "outputs", "cropU_std.txt")
undetectableCropsMean = np.genfromtxt(undetectableCropsMeanPath, delimiter=',', skip_header=1)[:,1]
undetectableCropsSTD = np.genfromtxt(undetectableCropsSTDPath, delimiter=',', skip_header=1)[:,1]
undetectableCropsUpper = undetectableCropsMean + undetectableCropsSTD
print(undetectableCropsUpper)
undetectableCropsLower = undetectableCropsMean - undetectableCropsSTD
savePath_cropsU_upper = os.path.join(outputFolder, "cropsU_upper.txt")
cropsU_upper = np.column_stack((time, undetectableCropsUpper))
np.savetxt(savePath_cropsU_upper, cropsU_upper, delimiter=",")
savePath_cropsU_lower = os.path.join(outputFolder, "cropsU_lower.txt")
cropsU_lower = np.column_stack((time, undetectableCropsLower))
np.savetxt(savePath_cropsU_lower, cropsU_lower, delimiter=",")

#Detectable Crops
detectableCropsMeanPath = os.path.join(rootFolder, "outputs", "cropD_mean.txt")
detectableCropsSTDPath = os.path.join(rootFolder, "outputs", "cropD_std.txt")
detectableCropsMean = np.genfromtxt(detectableCropsMeanPath, delimiter=',', skip_header=1)[:,1]
detectableCropsSTD = np.genfromtxt(detectableCropsSTDPath, delimiter=',', skip_header=1)[:,1]
detectableCropsUpper = detectableCropsMean + detectableCropsSTD
detectableCropsLower = detectableCropsMean - detectableCropsSTD
savePath_cropsD_upper = os.path.join(outputFolder, "cropsD_upper.txt")
cropsD_upper = np.column_stack((time, detectableCropsUpper))
np.savetxt(savePath_cropsD_upper, cropsD_upper, delimiter=",")
savePath_cropsD_lower = os.path.join(outputFolder, "cropsD_lower.txt")
cropsD_lower = np.column_stack((time, detectableCropsLower))
np.savetxt(savePath_cropsD_lower, cropsD_lower, delimiter=",")

#Healthy Sentinels
healthySentinelsMeanPath = os.path.join(rootFolder, "outputs", "sentinelH_mean.txt")
healthySentinelsSTDPath = os.path.join(rootFolder, "outputs", "sentinelH_std.txt")
healthySentinelsMean = np.genfromtxt(healthySentinelsMeanPath, delimiter=',', skip_header=1)[:,1]
healthySentinelsSTD = np.genfromtxt(healthySentinelsSTDPath, delimiter=',', skip_header=1)[:,1]
healthySentinelsUpper = healthySentinelsMean + healthySentinelsSTD
healthySentinelsLower = healthySentinelsMean - healthySentinelsSTD
savePath_sentinelH_upper = os.path.join(outputFolder, "sentinelH_upper.txt")
sentinelH_upper = np.column_stack((time, healthySentinelsUpper))
np.savetxt(savePath_sentinelH_upper, sentinelH_upper, delimiter=",")
savePath_sentinelH_lower = os.path.join(outputFolder, "sentinelH_lower.txt")
sentinelH_lower = np.column_stack((time, healthySentinelsLower))
np.savetxt(savePath_sentinelH_lower, sentinelH_lower, delimiter=",")

#Undetectable Sentinels
undetectableSentinelsMeanPath = os.path.join(rootFolder, "outputs", "sentinelU_mean.txt")
undetectableSentinelsSTDPath = os.path.join(rootFolder, "outputs", "sentinelU_std.txt")
undetectableSentinelsMean = np.genfromtxt(undetectableSentinelsMeanPath, delimiter=',', skip_header=1)[:,1]
undetectableSentinelsSTD = np.genfromtxt(undetectableSentinelsSTDPath, delimiter=',', skip_header=1)[:,1]
undetectableSentinelsUpper = undetectableSentinelsMean + undetectableSentinelsSTD
undetectableSentinelsLower = undetectableSentinelsMean - undetectableSentinelsSTD
savePath_sentinelU_upper = os.path.join(outputFolder, "sentinelU_upper.txt")
sentinelU_upper = np.column_stack((time, undetectableSentinelsUpper))
np.savetxt(savePath_sentinelU_upper, sentinelU_upper, delimiter=",")
savePath_sentinelU_lower = os.path.join(outputFolder, "sentinelU_lower.txt")
sentinelU_lower = np.column_stack((time, undetectableSentinelsLower))
np.savetxt(savePath_sentinelU_lower, sentinelU_lower, delimiter=",")

#Detectable Sentinels
detectableSentinelsMeanPath = os.path.join(rootFolder, "outputs", "sentinelD_mean.txt")
detectableSentinelsSTDPath = os.path.join(rootFolder, "outputs", "sentinelD_std.txt")
detectableSentinelsMean = np.genfromtxt(detectableSentinelsMeanPath, delimiter=',', skip_header=1)[:,1]
detectableSentinelsSTD = np.genfromtxt(detectableSentinelsSTDPath, delimiter=',', skip_header=1)[:,1]
detectableSentinelsUpper = detectableSentinelsMean + detectableSentinelsSTD
detectableSentinelsLower = detectableSentinelsMean - detectableSentinelsSTD
savePath_sentinelD_upper = os.path.join(outputFolder, "sentinelD_upper.txt")
sentinelD_upper = np.column_stack((time, detectableSentinelsUpper))
np.savetxt(savePath_sentinelD_upper, sentinelD_upper, delimiter=",")
savePath_sentinelD_lower = os.path.join(outputFolder, "sentinelD_lower.txt")
sentinelD_lower = np.column_stack((time, detectableSentinelsLower))
np.savetxt(savePath_sentinelD_lower, sentinelD_lower, delimiter=",")

fig, axs = plt.subplots(2)
# subplot titles
axs[0].set_title('Crops')
axs[1].set_title('Sentinels')

# plot crops
# healthy
axs[0].plot(time, healthyCropsMean, label = 'healthy', color = 'tab:blue')
axs[0].plot(time, healthyCropsMean+healthyCropsSTD, color = 'tab:blue', alpha = 0.5)
axs[0].plot(time, healthyCropsMean-healthyCropsSTD, color = 'tab:blue', alpha = 0.5)
axs[0].fill_between(time, healthyCropsMean, healthyCropsMean+healthyCropsSTD, color = 'tab:blue', alpha = 0.1)
axs[0].fill_between(time, healthyCropsMean, healthyCropsMean-healthyCropsSTD, color = 'tab:blue', alpha = 0.1)

# undetectable
axs[0].plot(time, undetectableCropsMean, label='Undetectable', color = 'tab:orange')
axs[0].plot(time, undetectableCropsMean+undetectableCropsSTD, color = 'tab:orange', alpha = 0.5)
axs[0].plot(time, undetectableCropsMean-undetectableCropsSTD, color = 'tab:orange', alpha = 0.5)
axs[0].fill_between(time, undetectableCropsMean, undetectableCropsMean+undetectableCropsSTD, color = 'tab:orange', alpha = 0.1)
axs[0].fill_between(time, undetectableCropsMean, undetectableCropsMean-undetectableCropsSTD, color = 'tab:orange', alpha = 0.1)

# undetectable
axs[0].plot(time, detectableCropsMean, label='Detectable', color = 'tab:green')
axs[0].plot(time, detectableCropsMean+detectableCropsSTD, color = 'tab:green', alpha = 0.5)
axs[0].plot(time, detectableCropsMean-detectableCropsSTD, color = 'tab:green', alpha = 0.5)
axs[0].fill_between(time, detectableCropsMean, detectableCropsMean+detectableCropsSTD, color = 'tab:green', alpha = 0.1)
axs[0].fill_between(time, detectableCropsMean, detectableCropsMean-detectableCropsSTD, color = 'tab:green', alpha = 0.1)

# plot sentinels
# healthy
axs[1].plot(time, healthySentinelsMean, color = 'tab:blue')
axs[1].plot(time, healthySentinelsMean+healthySentinelsSTD, color = 'tab:blue', alpha = 0.5)
axs[1].plot(time, healthySentinelsMean-healthySentinelsSTD, color = 'tab:blue', alpha = 0.5)
axs[1].fill_between(time, healthySentinelsMean, healthySentinelsMean+healthySentinelsSTD, color = 'tab:blue', alpha = 0.1)
axs[1].fill_between(time, healthySentinelsMean, healthySentinelsMean-healthySentinelsSTD, color = 'tab:blue', alpha = 0.1)

# undetectable
axs[1].plot(time, undetectableSentinelsMean, color = 'tab:orange')
axs[1].plot(time, undetectableSentinelsMean+undetectableSentinelsSTD, color = 'tab:orange', alpha = 0.5)
axs[1].plot(time, undetectableSentinelsMean-undetectableSentinelsSTD, color = 'tab:orange', alpha = 0.5)
axs[1].fill_between(time, undetectableSentinelsMean, undetectableSentinelsMean+undetectableSentinelsSTD, color = 'tab:orange', alpha = 0.1)
axs[1].fill_between(time, undetectableSentinelsMean, undetectableSentinelsMean-undetectableSentinelsSTD, color = 'tab:orange', alpha = 0.1)

# undetectable
axs[1].plot(time, detectableSentinelsMean, color = 'tab:green')
axs[1].plot(time, detectableSentinelsMean+detectableSentinelsSTD, color = 'tab:green', alpha = 0.5)
axs[1].plot(time, detectableSentinelsMean-detectableSentinelsSTD, color = 'tab:green', alpha = 0.5)
axs[1].fill_between(time, detectableSentinelsMean, detectableSentinelsMean+detectableSentinelsSTD, color = 'tab:green', alpha = 0.1)
axs[1].fill_between(time, detectableSentinelsMean, detectableSentinelsMean-detectableSentinelsSTD, color = 'tab:green', alpha = 0.1)

for ax in axs.flat:
    ax.set(xlabel='time (days)', ylabel='no. plants')

for ax in axs.flat:
    ax.label_outer()
    
fig.legend(loc='upper center', ncol=3)
fig.savefig(savePathHUD)
plt.show()
