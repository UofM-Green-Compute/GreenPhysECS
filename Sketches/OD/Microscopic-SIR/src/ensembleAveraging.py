import numpy as np
import pandas as pd
import os

threshold = 50

numberOfSamples = 100
data_path = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', 
                                          'outputs'))

def saveData(header, dataColumn1, dataColumn2, filename):
    SavePath = os.path.join(data_path, filename)
    saveData = np.column_stack((dataColumn1, dataColumn2))
    save_df = pd.DataFrame(saveData, columns=header)
    save_df.to_csv(SavePath, index=False, sep=",")

filename1 = f"SIR_{1}.txt"
load_path1 = os.path.join(data_path, f"SIR_{1}.txt")
time = np.genfromtxt(load_path1, delimiter = ',', skip_header=1)[:,0]

# Cases where there has been an epidemic or endemic
susceptibleDataE = []
infectedDataE = []
recoveredDataE = []

# Cases where there has not been an epidemic or endemic
susceptibleDataNE = []
infectedDataNE = []
recoveredDataNE = []

i = 0
j = 0
for sampleIndex in range(numberOfSamples):
    load_path = os.path.join(data_path, f"SIR_{sampleIndex+1}.txt")
    data = np.genfromtxt(load_path, delimiter=',', skip_header=1)

    if (data[-1,3] > threshold):
        susceptibleDataE.append(data[:,1])
        infectedDataE.append(data[:,2])
        recoveredDataE.append(data[:,3])
        i += 1
    else: 
        susceptibleDataNE.append(data[:,1])
        infectedDataNE.append(data[:,2])
        recoveredDataNE.append(data[:,3])
        j += 1

susceptibleDataE = np.array(susceptibleDataE)
infectedDataE = np.array(infectedDataE)
recoveredDataE = np.array(recoveredDataE)

susceptibleMeanE = np.mean(susceptibleDataE, axis=0)
susceptibleSTD_E = np.std(susceptibleDataE, axis=0)
infectedMeanE = np.mean(infectedDataE, axis=0)
infectedSTD_E = np.std(infectedDataE, axis=0) 
recoveredMeanE = np.mean(recoveredDataE, axis=0)
recoveredSTD_E = np.std(recoveredDataE, axis=0)

susceptibleMeanNE = np.mean(susceptibleDataNE, axis=0)
susceptibleSTD_NE = np.std(susceptibleDataNE, axis=0)
infectedMeanNE = np.mean(infectedDataNE, axis=0)
infectedSTD_NE = np.std(infectedDataNE, axis=0)
recoveredMeanNE = np.mean(recoveredDataNE, axis=0)
recoveredSTD_NE = np.std(recoveredDataNE, axis=0)

"""
Empty out the previous contents of the files
files = ['SusceptibleMeanE.txt','SusceptibleSTD_E.txt','SusceptibleUpperE.txt','SusceptibleLowerE.txt','InfectedMeanE.txt','InfectedSTD_E.txt',
         'InfectedUpperE.txt','InfectedLowerE.txt','RecoveredMeanE.txt','RecoveredSTD_E.txt','RecoveredUpperE.txt','RecoveredLowerE.txt',
         'SusceptibleMeanNE.txt','SusceptibleSTD_NE.txt','SusceptibleUpperNE.txt','SusceptibleLowerNE.txt','InfectedMeanNE.txt','InfectedSTD_NE.txt',
         'InfectedUpperNE.txt','InfectedLowerNE.txt','RecoveredMeanNE.txt','RecoveredSTD_NE.txt','RecoveredUpperNE.txt','RecoveredLowerNE.txt']
for file in files:
    load_path = os.path.join(data_path, file)
    f = open(load_path,'r+')
    f.truncate(0)
"""

# Significant outbreak occured 
if(len(susceptibleDataE) != 0):
    saveData(['time', 'susceptible mean'], time, susceptibleMeanE, 'SusceptibleMeanE.txt')
    saveData(['time', 'susceptible STD'], time, susceptibleSTD_E, 'SusceptibleSTD_E.txt')
    saveData(['time', 'susceptible upper'], time, susceptibleMeanE+susceptibleSTD_E, 'SusceptibleUpperE.txt')
    saveData(['time', 'susceptible lower'], time, susceptibleMeanE-susceptibleSTD_E, 'SusceptibleLowerE.txt')

    saveData(['time', 'infected mean'], time, infectedMeanE, 'InfectedMeanE.txt')
    saveData(['time', 'infected STD'], time, infectedSTD_E, 'InfectedSTD_E.txt')
    saveData(['time', 'infected upper'], time, infectedMeanE+infectedSTD_E, 'InfectedUpperE.txt')
    saveData(['time', 'infected lower'], time, infectedMeanE-infectedSTD_E, 'InfectedLowerE.txt')

    saveData(['time', 'recovered mean'], time, recoveredMeanE, 'RecoveredMeanE.txt')
    saveData(['time', 'recovered STD'], time, recoveredSTD_E, 'RecoveredSTD_E.txt')
    saveData(['time', 'recovered upper'], time, recoveredMeanE+recoveredSTD_E, 'RecoveredUpperE.txt')
    saveData(['time', 'recovered lower'], time, recoveredMeanE-recoveredSTD_E, 'RecoveredLowerE.txt') 

# No significant outbreak occured 
if(len(susceptibleDataNE) != 0):
    saveData(['time', 'susceptible mean'], time, susceptibleMeanNE, 'SusceptibleMeanNE.txt')
    saveData(['time', 'susceptible STD'], time, susceptibleSTD_NE, 'SusceptibleSTD_NE.txt')
    saveData(['time', 'susceptible upper'], time, susceptibleMeanNE+susceptibleSTD_NE, 'SusceptibleUpperNE.txt')
    saveData(['time', 'susceptible lower'], time, susceptibleMeanNE-susceptibleSTD_NE, 'SusceptibleLowerNE.txt')

    saveData(['time', 'infected mean'], time, infectedMeanNE, 'InfectedMeanNE.txt')
    saveData(['time', 'infected STD'], time, infectedSTD_NE, 'InfectedSTD_NE.txt')
    saveData(['time', 'infected upper'], time, infectedMeanNE+infectedSTD_NE, 'InfectedUpperNE.txt')
    saveData(['time', 'infected lower'], time, infectedMeanNE-infectedSTD_NE, 'InfectedLowerNE.txt')

    saveData(['time', 'recovered mean'], time, recoveredMeanNE, 'RecoveredMeanNE.txt')
    saveData(['time', 'recovered STD'], time, recoveredSTD_NE, 'RecoveredSTD_NE.txt')
    saveData(['time', 'recovered upper'], time, recoveredMeanNE+recoveredSTD_NE, 'RecoveredUpperNE.txt')
    saveData(['time', 'recovered lower'], time, recoveredMeanNE-recoveredSTD_NE, 'RecoveredLowerNE.txt') 