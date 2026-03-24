import os
import numpy as np
import matplotlib.pyplot as plt

rootFolder = os.path.dirname(os.path.dirname(__file__))
savePathHUD = os.path.join(rootFolder, "outputs", "HUDensemble.pdf")

#Healthy Crops
healthyCropsMeanPath = os.path.join(rootFolder, "outputs", "HealthyCropsMean.txt")
healthyCropsUpperPath = os.path.join(rootFolder, "outputs", "HealthyCropsUpper.txt")
healthyCropsLowerPath = os.path.join(rootFolder, "outputs", "HealthyCropsLower.txt")
time = np.genfromtxt(healthyCropsMeanPath, delimiter = ',', skip_header = 1)[:,0]
healthyCropsMean = np.genfromtxt(healthyCropsMeanPath, delimiter = ',', skip_header = 1)[:,1]
healthyCropsUpper = np.genfromtxt(healthyCropsUpperPath, delimiter = ',', skip_header = 1)[:,1]
healthyCropsLower = np.genfromtxt(healthyCropsLowerPath, delimiter = ',', skip_header = 1)[:,1]

#Undetectable Crops
undetectableCropsMeanPath = os.path.join(rootFolder, "outputs", "UndetectableCropsMean.txt")
undetectableCropsUpperPath = os.path.join(rootFolder, "outputs", "UndetectableCropsUpper.txt")
undetectableCropsLowerPath = os.path.join(rootFolder, "outputs", "UndetectableCropsLower.txt")
undetectableCropsMean = np.genfromtxt(undetectableCropsMeanPath, delimiter = ',', skip_header = 1)[:,1]
undetectableCropsUpper = np.genfromtxt(undetectableCropsUpperPath, delimiter = ',', skip_header = 1)[:,1]
undetectableCropsLower = np.genfromtxt(undetectableCropsLowerPath, delimiter = ',', skip_header = 1)[:,1]

#Detectable Crops
detectableCropsMeanPath = os.path.join(rootFolder, "outputs", "DetectableCropsMean.txt")
detectableCropsUpperPath = os.path.join(rootFolder, "outputs", "DetectableCropsUpper.txt")
detectableCropsLowerPath = os.path.join(rootFolder, "outputs", "DetectableCropsLower.txt")
detectableCropsMean = np.genfromtxt(detectableCropsMeanPath, delimiter = ',', skip_header = 1)[:,1]
detectableCropsUpper = np.genfromtxt(detectableCropsUpperPath, delimiter = ',', skip_header = 1)[:,1]
detectableCropsLower = np.genfromtxt(detectableCropsLowerPath, delimiter = ',', skip_header = 1)[:,1]

#Healthy Sentinels
healthySentinelsMeanPath = os.path.join(rootFolder, "outputs", "HealthySentinelsMean.txt")
healthySentinelsUpperPath = os.path.join(rootFolder, "outputs", "HealthySentinelsUpper.txt")
healthySentinelsLowerPath = os.path.join(rootFolder, "outputs", "HealthySentinelsLower.txt")
healthySentinelsMean = np.genfromtxt(healthySentinelsMeanPath, delimiter = ',', skip_header = 1)[:,1]
healthySentinelsUpper = np.genfromtxt(healthySentinelsUpperPath, delimiter = ',', skip_header = 1)[:,1]
healthySentinelsLower = np.genfromtxt(healthySentinelsLowerPath, delimiter = ',', skip_header = 1)[:,1]

#Undetectable Sentinels
undetectableSentinelsMeanPath = os.path.join(rootFolder, "outputs", "UndetectableSentinelsMean.txt")
undetectableSentinelsUpperPath = os.path.join(rootFolder, "outputs", "UndetectableSentinelsUpper.txt")
undetectableSentinelsLowerPath = os.path.join(rootFolder, "outputs", "UndetectableSentinelsLower.txt")
undetectableSentinelsMean = np.genfromtxt(undetectableSentinelsMeanPath, delimiter = ',', skip_header = 1)[:,1]
undetectableSentinelsUpper = np.genfromtxt(undetectableSentinelsUpperPath, delimiter = ',', skip_header = 1)[:,1]
undetectableSentinelsLower = np.genfromtxt(undetectableSentinelsLowerPath, delimiter = ',', skip_header = 1)[:,1]

#Detectable Sentinels
detectableSentinelsMeanPath = os.path.join(rootFolder, "outputs", "DetectableSentinelsMean.txt")
detectableSentinelsUpperPath = os.path.join(rootFolder, "outputs", "DetectableSentinelsUpper.txt")
detectableSentinelsLowerPath = os.path.join(rootFolder, "outputs", "DetectableSentinelsLower.txt")
detectableSentinelsMean = np.genfromtxt(detectableSentinelsMeanPath, delimiter = ',', skip_header = 1)[:,1]
detectableSentinelsUpper = np.genfromtxt(detectableSentinelsUpperPath, delimiter = ',', skip_header = 1)[:,1]
detectableSentinelsLower = np.genfromtxt(detectableSentinelsLowerPath, delimiter = ',', skip_header = 1)[:,1]

fig, axs = plt.subplots(2)
# subplot titles
axs[0].set_title('Crops')
axs[1].set_title('Sentinels')

# plot crops
# healthy
axs[0].plot(time, healthyCropsMean, label = 'healthy', color = 'tab:blue')
axs[0].plot(time, healthyCropsUpper, color = 'tab:blue', alpha = 0.5)
axs[0].plot(time, healthyCropsLower, color = 'tab:blue', alpha = 0.5)
axs[0].fill_between(time, healthyCropsMean, healthyCropsUpper, color = 'tab:blue', alpha = 0.1)
axs[0].fill_between(time, healthyCropsMean, healthyCropsLower, color = 'tab:blue', alpha = 0.1)

# undetectable
axs[0].plot(time, undetectableCropsMean, label='Undetectable', color = 'tab:orange')
axs[0].plot(time, undetectableCropsLower, color = 'tab:orange', alpha = 0.5)
axs[0].plot(time, undetectableCropsUpper, color = 'tab:orange', alpha = 0.5)
axs[0].fill_between(time, undetectableCropsMean, undetectableCropsUpper, color = 'tab:orange', alpha = 0.1)
axs[0].fill_between(time, undetectableCropsMean, undetectableCropsLower, color = 'tab:orange', alpha = 0.1)

# undetectable
axs[0].plot(time, detectableCropsMean, label='Detectable', color = 'tab:green')
axs[0].plot(time, detectableCropsLower, color = 'tab:green', alpha = 0.5)
axs[0].plot(time, detectableCropsUpper, color = 'tab:green', alpha = 0.5)
axs[0].fill_between(time, detectableCropsMean, detectableCropsUpper, color = 'tab:green', alpha = 0.1)
axs[0].fill_between(time, detectableCropsMean, detectableCropsLower, color = 'tab:green', alpha = 0.1)

# plot sentinels
# healthy
axs[1].plot(time, healthySentinelsMean, color = 'tab:blue')
axs[1].plot(time, healthySentinelsUpper, color = 'tab:blue', alpha = 0.5)
axs[1].plot(time, healthySentinelsLower, color = 'tab:blue', alpha = 0.5)
axs[1].fill_between(time, healthySentinelsMean, healthySentinelsUpper, color = 'tab:blue', alpha = 0.1)
axs[1].fill_between(time, healthySentinelsMean, healthySentinelsLower, color = 'tab:blue', alpha = 0.1)

# undetectable
axs[1].plot(time, undetectableSentinelsMean, color = 'tab:orange')
axs[1].plot(time, undetectableSentinelsLower, color = 'tab:orange', alpha = 0.5)
axs[1].plot(time, undetectableSentinelsUpper, color = 'tab:orange', alpha = 0.5)
axs[1].fill_between(time, undetectableSentinelsMean, undetectableSentinelsUpper, color = 'tab:orange', alpha = 0.1)
axs[1].fill_between(time, undetectableSentinelsMean, undetectableSentinelsLower, color = 'tab:orange', alpha = 0.1)

# undetectable
axs[1].plot(time, detectableSentinelsMean, color = 'tab:green')
axs[1].plot(time, detectableSentinelsLower, color = 'tab:green', alpha = 0.5)
axs[1].plot(time, detectableSentinelsUpper, color = 'tab:green', alpha = 0.5)
axs[1].fill_between(time, detectableSentinelsMean, detectableSentinelsUpper, color = 'tab:green', alpha = 0.1)
axs[1].fill_between(time, detectableSentinelsMean, detectableSentinelsLower, color = 'tab:green', alpha = 0.1)

for ax in axs.flat:
    ax.set(xlabel='time (days)', ylabel='no. plants')

for ax in axs.flat:
    ax.label_outer()
    
fig.legend(loc='upper center', ncol=3)
fig.savefig(savePathHUD)
plt.show()
