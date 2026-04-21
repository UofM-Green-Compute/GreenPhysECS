import os
import numpy as np
import matplotlib.pyplot as plt

rootFolder = os.path.dirname(os.path.dirname(__file__))
savePath = os.path.join(rootFolder, "outputs", "HUDsample.pdf")

# Crops
cropsPath = os.path.join(rootFolder, "outputs", "HUDcrops_1.txt")
time = np.genfromtxt(cropsPath, delimiter = ',', skip_header = 1)[:,0]
healthyCrops = np.genfromtxt(cropsPath, delimiter = ',', skip_header = 1)[:,1]
undetectableCrops = np.genfromtxt(cropsPath, delimiter = ',', skip_header = 1)[:,2]
detectableCrops = np.genfromtxt(cropsPath, delimiter = ',', skip_header = 1)[:,3]

# Sentinels
sentinelsPath = os.path.join(rootFolder, "outputs", "HUDsentinels_1.txt")
healthySentinels = np.genfromtxt(sentinelsPath, delimiter = ',', skip_header = 1)[:,1]
undetectableSentinels = np.genfromtxt(sentinelsPath, delimiter = ',', skip_header = 1)[:,2]
detectableSentinels = np.genfromtxt(sentinelsPath, delimiter = ',', skip_header = 1)[:,3]

fig, axs = plt.subplots(2)
# subplot titles
axs[0].set_title('Crops')
axs[1].set_title('Sentinels')

# plot crops
axs[0].plot(time, healthyCrops, color = 'tab:blue')
axs[0].plot(time, undetectableCrops, color = 'tab:orange')
axs[0].plot(time, detectableCrops, color = 'tab:green')

# plot sentinels
axs[1].plot(time, healthySentinels, color = 'tab:blue')
axs[1].plot(time, undetectableSentinels, color = 'tab:orange')
axs[1].plot(time, detectableSentinels, color = 'tab:green')

for ax in axs.flat:
    ax.set(xlabel='time (days)', ylabel='no. plants')

for ax in axs.flat:
    ax.label_outer()
    
fig.legend(loc='upper center', ncol=3)
fig.savefig(savePath)
plt.show()
