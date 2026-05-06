import os
import networkx as nx

root_folder = os.path.dirname(os.path.dirname(__file__))
input_path = os.path.join(root_folder, "outputs", "Network-Edges.txt")
output_path1 = os.path.join(root_folder, "outputs", "edges.txt")
output_path2 = os.path.join(root_folder, "outputs", "nodes.txt")

data = []
with open(input_path, "r") as f:
    for i, row in enumerate(f):
        data.append([])
        row = row.split(";")
        for j, line in enumerate(row):
            if line != '\n' and j != 0:
                line = line.split(",")
                data[i].append(tuple(int(x) for x in line))

edges = data[40]
print(edges)

G = nx.Graph()
G.add_edges_from(edges)

pos = nx.spring_layout(G)

with open(output_path1, "w") as f:
    for u, v in edges:
        f.write(f"\\draw ({u}) -- ({v});\n")

with open(output_path2, "w") as f:
    for node, (x, y) in pos.items():
        f.write(f"\\node ({node}) at ({x:.4f},{y:.4f}) {{}};\n")
print(pos)
