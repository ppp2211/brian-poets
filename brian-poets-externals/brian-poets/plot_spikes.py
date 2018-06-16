import os
import sys
from matplotlib import pyplot as plt

index = []
time = []

with open(sys.argv[1], 'r') as infile:
    for line in infile:
        l = line.strip().split(',')
        index.append(int(l[0]))
        time.append(float(l[1]))

plt.plot(time, index, '.k')
plt.xlabel("Time (ms)")
plt.ylabel("Neuron index")
plt.savefig(sys.argv[2])
plt.show()
