import numpy as np
import pandas as pd
import os

numberOfSamples = 100
dataPath = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', 
                                          'outputs'))

def saveData(header, dataColumn1, dataColumn2, filename):
    SavePath = os.path.join(dataPath, filename)
    saveData = np.column_stack((dataColumn1, dataColumn2))
    save_df = pd.DataFrame(saveData, columns=header)
    save_df.to_csv(SavePath, index=False, sep=",")
    return

load_path1 = os.path.join(dataPath, f"HUDcrops_{1}.txt")
data1 = np.genfromtxt(load_path1, delimiter = ',', skip_header=1)
time = data1[:,0]
dataLength = len(data1[:,0])
healthyCropsData = np.zeros(numberOfSamples, dtype=object)
undetectableCropsData = np.zeros(numberOfSamples, dtype=object)
detectableCropsData = np.zeros(numberOfSamples, dtype=object)
healthySentinelsData = np.zeros(numberOfSamples, dtype=object)
undetectableSentinelsData = np.zeros(numberOfSamples, dtype=object)
detectableSentinelsData = np.zeros(numberOfSamples, dtype=object)
for sampleIndex in range(numberOfSamples):
    cropPath = os.path.join(dataPath, f"HUDcrops_{sampleIndex+1}.txt")
    cropData = np.genfromtxt(cropPath, delimiter=',', skip_header=1)
    sentinelPath = os.path.join(dataPath, f"HUDsentinels_{sampleIndex+1}.txt")
    sentinelData = np.genfromtxt(sentinelPath, delimiter=',', skip_header=1)
    healthyCropsData[sampleIndex] = cropData[:,1]
    undetectableCropsData[sampleIndex] = cropData[:,2]
    detectableCropsData[sampleIndex] = cropData[:,3]
    healthySentinelsData[sampleIndex] = sentinelData[:,1]
    undetectableSentinelsData[sampleIndex] = sentinelData[:,2]
    detectableSentinelsData[sampleIndex] = sentinelData[:,3]

healthyCropsMean = np.mean(healthyCropsData, axis=0)
healthyCropsSTD = np.std(healthyCropsData, axis=0)
undetectableCropsMean = np.mean(undetectableCropsData, axis=0)
undetectableCropsSTD = np.std(undetectableCropsData, axis=0)
detectableCropsMean = np.mean(detectableCropsData, axis=0)
detectableCropsSTD = np.std(detectableCropsData, axis=0)

healthySentinelsMean = np.mean(healthySentinelsData, axis=0)
healthySentinelsSTD = np.std(healthySentinelsData, axis=0)
undetectableSentinelsMean = np.mean(undetectableSentinelsData, axis=0)
undetectableSentinelsSTD = np.std(undetectableSentinelsData, axis=0)
detectableSentinelsMean = np.mean(detectableSentinelsData, axis=0)
detectableSentinelsSTD = np.std(detectableSentinelsData, axis=0)

saveData(['time', 'healthy crops mean'], time, healthyCropsMean, 'HealthyCropsMean.txt')
saveData(['time', 'healthy crops STD'], time, healthyCropsSTD, 'HealthyCropsSTD.txt')
saveData(['time', 'healthy crops upper'], time, healthyCropsMean+healthyCropsSTD, 'HealthyCropsUpper.txt')
saveData(['time', 'healthy crops lower'], time, healthyCropsMean-healthyCropsSTD, 'HealthyCropsLower.txt')

saveData(['time', 'detectable crops mean'], time, detectableCropsMean, 'DetectableCropsMean.txt')
saveData(['time', 'detectable crops STD'], time, detectableCropsSTD, 'DetectableCropsSTD.txt')
saveData(['time', 'detectable crops upper'], time, detectableCropsMean+detectableCropsSTD, 'DetectableCropsUpper.txt')
saveData(['time', 'detectable crops lower'], time, detectableCropsMean-detectableCropsSTD, 'DetectableCropsLower.txt')

saveData(['time', 'undetectable crops mean'], time, undetectableCropsMean, 'UndetectableCropsMean.txt')
saveData(['time', 'undetectable crops STD'], time, undetectableCropsSTD, 'UndetectableCropsSTD.txt')
saveData(['time', 'undetectable crops upper'], time, undetectableCropsMean+undetectableCropsSTD, 'UndetectableCropsUpper.txt')
saveData(['time', 'undetectable crops lower'], time, undetectableCropsMean-undetectableCropsSTD, 'UndetectableCropsLower.txt')

saveData(['time', 'healthy sentinels mean'], time, healthySentinelsMean, 'HealthySentinelsMean.txt')
saveData(['time', 'healthy sentinels STD'], time, healthySentinelsSTD, 'HealthySentinelsSTD.txt')
saveData(['time', 'healthy sentinels upper'], time, healthySentinelsMean+healthySentinelsSTD, 'HealthySentinelsUpper.txt')
saveData(['time', 'healthy sentinels lower'], time, healthySentinelsMean-healthySentinelsSTD, 'HealthySentinelsLower.txt')

saveData(['time', 'detectable sentinels mean'], time, detectableSentinelsMean, 'DetectableSentinelsMean.txt')
saveData(['time', 'detectable sentinels STD'], time, detectableSentinelsSTD, 'DetectableSentinelsSTD.txt')
saveData(['time', 'detectable sentinels upper'], time, detectableSentinelsMean+detectableSentinelsSTD, 'DetectableSentinelsUpper.txt')
saveData(['time', 'detectable sentinels lower'], time, detectableSentinelsMean-detectableSentinelsSTD, 'DetectableSentinelsLower.txt')

saveData(['time', 'undetectable sentinels mean'], time, undetectableSentinelsMean, 'UndetectableSentinelsMean.txt')
saveData(['time', 'undetectable sentinels STD'], time, undetectableSentinelsSTD, 'UndetectableSentinelsSTD.txt')
saveData(['time', 'undetectable sentinels upper'], time, undetectableSentinelsMean+undetectableSentinelsSTD, 'UndetectableSentinelsUpper.txt')
saveData(['time', 'undetectable sentinels lower'], time, undetectableSentinelsMean-undetectableSentinelsSTD, 'UndetectableSentinelsLower.txt')
