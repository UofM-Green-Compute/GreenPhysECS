import os
import numpy as np
import matplotlib.pyplot as plt

root_folder = os.path.dirname(os.path.dirname(__file__))
data_path1 = os.path.join(root_folder, "outputs", "person1.txt")
data_path2 = os.path.join(root_folder, "outputs", "person2.txt")
save_path = os.path.join(root_folder, "outputs", "position.pdf")

# Person 1 Data
positionFile1 = np.genfromtxt(data_path1, delimiter = ',', skip_header = 1)
timeData = positionFile1[:, 0]
xPositionData1 = positionFile1[:, 1]
yPositionData1 = positionFile1[:, 2]

# Person 2 Data
positionFile2 = np.genfromtxt(data_path2, delimiter = ',', skip_header = 1)
xPositionData2 = positionFile2[:, 1]
yPositionData2 = positionFile2[:, 2]

fig, ax = plt.subplots()
ax.plot(xPositionData1,yPositionData1,label='Person 1')
ax.plot(xPositionData2,yPositionData2,label='Person 2')
ax.set_xlim(0, 50)
ax.set_ylim(0, 50)
ax.set_aspect('equal')
fig.legend(loc='upper center', ncol=3)
fig.savefig(save_path)
plt.show()
