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
totalPopulation = data1[1,1]+data1[1,2]+data1[1,3]

dataLength = len(data1[:,0])
EDParray = np.zeros(numberOfSamples, dtype=object)
EDTarray = np.zeros(numberOfSamples, dtype=object)
for sampleIndex in range(numberOfSamples):
    cropPath = os.path.join(dataPath, f"HUDcrops_{sampleIndex+1}.txt")
    cropData = np.genfromtxt(cropPath, delimiter=',', skip_header=1)
    sentinelPath = os.path.join(dataPath, f"HUDsentinels_{sampleIndex+1}.txt")
    sentinelData = np.genfromtxt(sentinelPath, delimiter=',', skip_header=1)
    infectedPopulation = (cropData[-1,2]+cropData[-1,3]
                          +sentinelData[-1,2]+sentinelData[-1,3])
    EDParray[sampleIndex] = infectedPopulation/totalPopulation
    EDTarray[sampleIndex] = cropData[-1,0]

EDPmean = np.mean(EDParray)
EDP_STD = np.std(EDParray)

EDTmean = np.mean(EDTarray)
EDT_STD = np.std(EDTarray)

print("Mean EDP = ",EDPmean,", sigma = ",EDP_STD)
print("Mean EDT = ",EDTmean,", sigma = ",EDT_STD)
