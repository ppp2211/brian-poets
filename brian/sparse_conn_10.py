#!/usr/bin/python3
from brian2 import *
import matplotlib.pyplot as plt
import time

BrianLogger.log_level_diagnostic()
set_device('cpp_standalone')

N = 10
tau = 10*ms
I = 2 
eqs = '''
dv/dt = (I-v)/tau : 1
'''
G = NeuronGroup(N, eqs, threshold='v>1', reset='v = 0', method='exact')
G.v = '1.0*i/N'

# Comment these two lines out to see what happens without Synapses
S = Synapses(G, G, on_pre='v_post += 0.1')
S.connect(condition='i!=j', p=0.1)

spikemon = SpikeMonitor(G)
s=time.time()
run(50*ms)
e=time.time()
print(e-s)


plt.plot(spikemon.t/ms, spikemon.i, '.k')
plt.xlabel('Time (ms)')
plt.ylabel('Neuron index')
plt.legend()
plt.show()

