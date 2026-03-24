import numpy as np
import pandas as pd
import os

# preamble for directories and the like
Pcrops = 1000
Psentinels = 50
N = 25
Delta = 30
totalPopulation = Pcrops+Psentinels
numberOfSamples = 100

# access outputs folder
outputPath = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', 
                                          'outputs'))

def saveData(path, header, dataColumn1, dataColumn2, filename):
    SavePath = os.path.join(path, filename)
    saveData = np.column_stack((dataColumn1, dataColumn2))
    save_df = pd.DataFrame(saveData, columns=header)
    save_df.to_csv(SavePath, index=False, sep=",")
    return

NsentinelArray = np.zeros(N+1, dtype=object)
EDP_meanArray=np.zeros(N+1, dtype=object)
EDP_stdArray=np.zeros(N+1, dtype=object)
EDT_meanArray=np.zeros(N+1, dtype=object)
EDT_stdArray=np.zeros(N+1, dtype=object)

for Nsentinel in range(N+1):
    dataPath1 = os.path.join(outputPath, f"Pcrops{Pcrops}_Psentinels{Psentinels}")
    dataPath2 = os.path.join(dataPath1, f"Delta{Delta}Ncrops{N-Nsentinel}_Nsentinels{Nsentinel}")
    EDParray = np.zeros(numberOfSamples, dtype=object)
    EDTarray = np.zeros(numberOfSamples, dtype=object)
    for sampleIndex in range(numberOfSamples):
        cropPath = os.path.join(dataPath2, f"HUDcrops_{sampleIndex+1}.txt")
        cropData = np.genfromtxt(cropPath, delimiter=',', skip_header=1)
        sentinelPath = os.path.join(dataPath2, f"HUDsentinels_{sampleIndex+1}.txt")
        sentinelData = np.genfromtxt(sentinelPath, delimiter=',', skip_header=1)
        infectedPopulation = (cropData[-1,2]+cropData[-1,3]
                            +sentinelData[-1,2]+sentinelData[-1,3])
        EDParray[sampleIndex] = infectedPopulation/totalPopulation
        EDTarray[sampleIndex] = cropData[-1,0]

    NsentinelArray[Nsentinel] = Nsentinel

    EDP_meanArray[Nsentinel] = 100*np.mean(EDParray)
    EDP_stdArray[Nsentinel] = 100*np.std(EDParray)

    EDT_meanArray[Nsentinel] = 100*np.mean(EDTarray)
    EDT_stdArray[Nsentinel] = 100*np.std(EDTarray)

saveData(outputPath, ['Nsentinel', 'EDP mean'], NsentinelArray, EDP_meanArray, 'EDPmean.txt')
saveData(outputPath, ['Nsentinel', 'EDP STD'], NsentinelArray, EDP_stdArray, 'EDPstd.txt')

saveData(outputPath, ['Nsentinel', 'EDT mean'], NsentinelArray, EDT_meanArray, 'EDTmean.txt')
saveData(outputPath, ['Nsentinel', 'EDT STD'], NsentinelArray, EDT_stdArray, 'EDTstd.txt')

