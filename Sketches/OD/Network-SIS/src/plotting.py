import os
import numpy as np
import matplotlib.pyplot as plt 
import networkx as nx

root_folder = os.path.dirname(os.path.dirname(__file__))
data_path_SEIR = os.path.join(root_folder, "outputs", "Network-SIR.txt")
data_path_Edges = os.path.join(root_folder, "outputs", "Network-Edges.txt")

SIR_data = np.genfromtxt(data_path_SEIR, delimiter = ',', skip_header = 0)
Edges_data = open(data_path_Edges)

"""
row = Edges_data.read()

edges = np.genfromtxt(data_path_Edges,delimiter=';',dtype=None)
edges2 = []
for i,x in enumerate(edges):
    if edges[i] != 'False':
        edges2.append(tuple(int(x) for x in edges[i].split(',')))
print(edges2)
"""

figSIR, axSIR = plt.subplots()
axSIR.plot(SIR_data[:,0],SIR_data[:,1],label='Susceptible') 
axSIR.plot(SIR_data[:,0],SIR_data[:,2],label='Infected')
axSIR.set_xlabel("Time")
axSIR.set_ylabel("No. People")
figSIR.legend(loc='upper center', ncol=4) 

"""
figNetwork, axNetwork = plt.subplots()
G = nx.Graph()
G.add_edges_from(edges2)
nx.draw_spring(G, node_size=30 ,ax=axNetwork)
"""

figSIR.savefig("Network-SIR")
# figNetwork.savefig("Network")

plt.show()