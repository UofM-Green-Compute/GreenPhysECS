import os
import numpy as np
import matplotlib.pyplot as plt 

root_folder = os.path.dirname(os.path.dirname(__file__))
file = os.path.join(root_folder,"outputs","Lattice-Grid.txt")
Grid_File = open(file)

grid_points = []

for i,line in enumerate(Grid_File):
    grid_points.append([])
    line = line.split('|')
    for point in line:
        if(point != '\n'): grid_points[i].append(point)

x_vals = []
y_vals = []
for line in grid_points:
    for p in line: 
        p = p.split(',')
        x_vals.append(float(p[0]))
        y_vals.append(float(p[1]))

plt.scatter(x_vals,y_vals)
plt.show()
