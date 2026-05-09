import numpy as np
import pandas as pd
import os

numberOfSamples = 100
data_path = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', 
                                          'outputs'))

def saveData(header, dataColumn1, dataColumn2, filename):
    SavePath = os.path.join(data_path, filename)
    saveData = np.column_stack((dataColumn1, dataColumn2))
    save_df = pd.DataFrame(saveData, columns=header)
    save_df.to_csv(SavePath, index=False, sep=",")
    return

filename1 = f"SEIR_{1}.txt"
load_path1 = os.path.join(data_path, f"SEIR_{1}.txt")
data1 = np.genfromtxt(load_path1, delimiter = ',', skip_header=1)
time = data1[:,0]
dataLength = len(data1[:,0])
susceptibleData = np.zeros(numberOfSamples, dtype=object)
exposedData = np.zeros(numberOfSamples, dtype=object)
infectedData = np.zeros(numberOfSamples, dtype=object)
recoveredData = np.zeros(numberOfSamples, dtype=object)
for sampleIndex in range(numberOfSamples):
    load_path = os.path.join(data_path, f"SEIR_{sampleIndex+1}.txt")
    data = np.genfromtxt(load_path, delimiter=',', skip_header=1)
    susceptibleData[sampleIndex] = data[:,1]
    exposedData[sampleIndex] = data[:,2]
    infectedData[sampleIndex] = data[:,3]
    recoveredData[sampleIndex] = data[:,4]

susceptibleMean = np.mean(susceptibleData, axis=0)
susceptibleSTD = np.std(susceptibleData, axis=0)
exposedMean = np.mean(exposedData, axis=0)
exposedSTD = np.std(exposedData, axis=0)
infectedMean = np.mean(infectedData, axis=0)
infectedSTD = np.std(infectedData, axis=0)
recoveredMean = np.mean(recoveredData, axis = 0)
recoveredSTD = np.std(recoveredData, axis = 0)

saveData(['time', 'susceptible mean'], time, susceptibleMean, 'SusceptibleMean.txt')
saveData(['time', 'susceptible STD'], time, susceptibleSTD, 'SusceptibleSTD.txt')
saveData(['time', 'susceptible upper'], time, susceptibleMean+susceptibleSTD, 'SusceptibleUpper.txt')
saveData(['time', 'susceptible lower'], time, susceptibleMean-susceptibleSTD, 'SusceptibleLower.txt')

saveData(['time', 'exposed mean'], time, exposedMean, 'ExposedMean.txt')
saveData(['time', 'exposed STD'], time, exposedSTD, 'ExposedSTD.txt')
saveData(['time', 'exposed upper'], time, exposedMean+exposedSTD, 'ExposedUpper.txt')
saveData(['time', 'exposed lower'], time, exposedMean-exposedSTD, 'ExposedLower.txt')

saveData(['time', 'infectious mean'], time, infectedMean, 'InfectedMean.txt')
saveData(['time', 'infectious STD'], time, infectedSTD, 'InfectedSTD.txt')
saveData(['time', 'infectious upper'], time, infectedMean+infectedSTD, 'InfectedUpper.txt')
saveData(['time', 'infectious lower'], time, infectedMean-infectedSTD, 'InfectedLower.txt')

saveData(['time', 'recovered mean'], time, recoveredMean, 'RecoveredMean.txt')
saveData(['time', 'recovered STD'], time, recoveredSTD, 'RecoveredSTD.txt')
saveData(['time', 'recovered upper'], time, recoveredMean+recoveredSTD, 'RecoveredUpper.txt')
saveData(['time', 'recovered lower'], time, recoveredMean-recoveredSTD, 'RecoveredLower.txt')
