import os
import numpy as np
import matplotlib.pyplot as plt

root_folder = os.path.dirname(os.path.dirname(__file__))
data_path_SIR = os.path.join(root_folder, "outputs", "position.txt")
save_path_SIR = os.path.join(root_folder, "outputs", "position.pdf")

positionFile = np.genfromtxt(data_path_SIR, delimiter = ',', skip_header = 1)
timeData = positionFile[:, 0]
xPositionData = positionFile[:, 1]
yPositionData = positionFile[:, 2]
print(timeData)
fig, ax = plt.subplots()
ax.plot(xPositionData,yPositionData,label='Person 1')
ax.set_xlim(0, 50)
ax.set_ylim(0, 50)
fig.legend(loc='upper center', ncol=3)
fig.savefig(save_path_SIR)
plt.show()