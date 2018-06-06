#!/bin/python3
from brian2 import *
import matplotlib.pyplot as plt

BrianLogger.log_level_diagnostic()
set_device('cpp_standalone')

N = 100
tau = 10*ms
I = 2
eqs = '''
dv/dt = (I-v)/tau : 1
I : 1
'''
G = NeuronGroup(N, eqs, threshold='v>1', reset='v = 0', method='exact')

# Comment these two lines out to see what happens without Synapses
S = Synapses(G, G, on_pre='v_post += 0.2')
S.connect(condition='i!=j', p=0.5)

spikemon = SpikeMonitor(G)

run(50*ms)
'''
plt.plot(spikemon.t/ms, spikemon.i, '.k')
plt.xlabel('Time (ms)')
plt.ylabel('Neuron index')
plt.legend();
plt.show()
'''
