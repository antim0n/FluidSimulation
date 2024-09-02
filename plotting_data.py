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

""" single function """
x = np.linspace(0, 20) # v max
y = 0.025 / x

plt.plot(x, y)

plt.xlabel("v max")
plt.ylabel("delta t")
plt.title("Maximum time step with the CFL-condition.")
plt.show()

""" single points """
# for i in lines:
#     name = i.pop(0)

#     x = [float(val) for val in i[:len(i) // 2]]
#     y = [float(val) for val in i[len(i) // 2:]]
#     xpoints = np.array(x)
#     ypoints = np.array(y)

#     plt.plot(xpoints, ypoints, 'o',color=(np.random.random(), np.random.random(), np.random.random()), label=name, markersize=5)
#     for xy in zip(xpoints, ypoints):
#         plt.annotate('%s' % xy[1], xy=xy, textcoords='data', fontsize=6)

# plt.xlabel("time step")
# plt.ylabel("")
# plt.title("Maximum time step and lambda with varying time step.")
# plt.legend()
# plt.show()

""" multiple plots in one """
# for i in lines:
#     name = i.pop(0)

#     x = [float(val) for val in i[:len(i) // 2]]
#     y = [float(val) for val in i[len(i) // 2:]]
#     xpoints = np.array(x)
#     ypoints = np.array(y)

#     plt.plot(xpoints, ypoints, color=(np.random.random(), np.random.random(), np.random.random()), label=name)

# plt.xlabel("time")
# plt.ylabel("density error")
# plt.title("Density error with varying time step.")
# plt.legend()
# plt.show()

""" many singular plots """
# figure, axis = plt.subplots(graphs_per_column, 2, figsize=(14, 8))

# number = 0
# for i in lines:
#     name = i.pop(0)

#     x = [float(val) for val in i[:len(i) // 2]]
#     y = [float(val) for val in i[len(i) // 2:]]
#     xpoints = np.array(x)
#     ypoints = np.array(y)
#     markers = [0]

#     a = axis[number % graphs_per_column, number // graphs_per_column]

#     a.plot(xpoints, ypoints, '-gD', markevery=markers)
#     a.set_title(name)

#     z = list(zip(xpoints, ypoints))
#     xy = z[0]
#     a.annotate('(%.3f, %.3f)' % xy, xy=xy)
#     xy = z[-1]
#     a.annotate('(%.3f, %.3f)' % xy, xy=xy)

#     number += 1

# #plt.xlabel('x')
# #plt.ylabel('y')
# #plt.legend()

# plt.tight_layout()
# plt.show()