import os
import numpy as np
import pandas as pd

rootPath = os.path.dirname(os.path.dirname(__file__))
outputFolder = os.path.join(rootPath, "outputs")

hexagonBoundaries = np.array([0.1, 0.1732, 0.2, 0.25])
squareBoundaries = np.array([0.1, 0.1414, 0.2, 0.2236, 0.25])

hexagonPath = os.path.join(outputFolder, "EDP(R)_Hexagon_In.txt")
squarePath = os.path.join(outputFolder, "EDP(R)_Square_In.txt")

hexagonRadius = np.genfromtxt(hexagonPath, delimiter=',', skip_header=1)[:, 0]
hexagonMean   = np.genfromtxt(hexagonPath, delimiter=',', skip_header=1)[:, 1]
squareRadius  = np.genfromtxt(squarePath,  delimiter=',', skip_header=1)[:, 0]
squareMean    = np.genfromtxt(squarePath,  delimiter=',', skip_header=1)[:, 1]

for lowerIndex in range(len(hexagonBoundaries) - 1):
    lowerBoundary = hexagonBoundaries[lowerIndex]
    upperBoundary = hexagonBoundaries[lowerIndex + 1]
    radii = np.array([])
    data  = np.array([])

    for radiusIndex, radius in enumerate(hexagonRadius):
        if lowerBoundary <= radius < upperBoundary:
            data  = np.append(data, hexagonMean[radiusIndex])
            radii = np.append(radii, radius)

    mean = np.mean(data)
    data[:] = mean

    radii = np.append(radii, lowerBoundary)
    data  = np.append(data, mean)

    radii = np.append(radii, upperBoundary)
    data  = np.append(data, mean)

    savePath = os.path.join(outputFolder, f"EDP(R)_Hexagon_Region_{lowerIndex + 1}.txt")
    saveData = np.column_stack([radii, data])
    df = pd.DataFrame(saveData, columns=['Radius', 'Mean EDP'])
    df.to_csv(savePath, index=False, sep=",")

for lowerIndex in range(len(squareBoundaries) - 1):
    lowerBoundary = squareBoundaries[lowerIndex]
    upperBoundary = squareBoundaries[lowerIndex + 1]
    radii = np.array([])
    data  = np.array([])

    for radiusIndex, radius in enumerate(squareRadius):
        if lowerBoundary <= radius < upperBoundary:
            data  = np.append(data, squareMean[radiusIndex])
            radii = np.append(radii, radius)

    mean = np.mean(data)
    data[:] = mean

    radii = np.append(radii, lowerBoundary)
    data  = np.append(data, mean)

    radii = np.append(radii, upperBoundary)
    data  = np.append(data, mean)

    savePath = os.path.join(outputFolder, f"EDP(R)_Square_Region_{lowerIndex + 1}.txt")
    saveData = np.column_stack([radii, data])
    df = pd.DataFrame(saveData, columns=['Radius', 'Mean EDP'])
    df.to_csv(savePath, index=False, sep=",")
