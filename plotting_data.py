import matplotlib.pyplot as plt
import numpy as np
import os

path = os.path.abspath(__file__)
dir = os.path.dirname(path)
os.chdir(dir)

lines = []

graphs_per_column = 2

with open("graphs.txt", "r") as f:
    lines = [line.strip().split(" ") for line in f]

figure, axis = plt.subplots(graphs_per_column, 2, figsize=(14, 8))

number = 0
for i in lines:
    name = i.pop(0)

    x = [float(val) for val in i[:len(i) // 2]]
    y = [float(val) for val in i[len(i) // 2:]]
    xpoints = np.array(x)
    ypoints = np.array(y)
    markers = [0]

    a = axis[number % graphs_per_column, number // graphs_per_column]

    a.plot(xpoints, ypoints, '-gD', markevery=markers)
    a.set_title(name)

    z = list(zip(xpoints, ypoints))
    xy = z[0]
    a.annotate('(%.3f, %.3f)' % xy, xy=xy)
    xy = z[-1]
    a.annotate('(%.3f, %.3f)' % xy, xy=xy)

    number += 1

#plt.xlabel('x')
#plt.ylabel('y')
#plt.legend()

plt.tight_layout()
plt.show()