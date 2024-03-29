#!/usr/bin/python3
from brian2 import *
import matplotlib.pyplot as plt
import time
#BrianLogger.log_level_diagnostic()
set_device('cpp_standalone')

eqs = '''
dv/dt = (I-v)/tau : 1
I : 1
tau : second
'''
N = 2
G = NeuronGroup(2, eqs, threshold='v>1', reset='v = 0', method='exact')
G.I = [2, 0]
G.tau = [10, 100]*ms

# Comment these two lines out to see what happens without Synapses
S = Synapses(G, G, on_pre='v_post += 0.2')
S.connect(i=0, j=1)

#M = StateMonitor(G, 'v', record=True)
SpM = SpikeMonitor(G)

s=time.time()
run(200*ms)
e=time.time()
print(e-s)
'''
plt.plot(M.t/ms, M.v[0], label='Neuron 0')
plt.plot(M.t/ms, M.v[1], label='Neuron 1')
plt.xlabel('Time (ms)')
plt.ylabel('v')
plt.legend();
plt.show()
'''
plt.plot(SpM.t/ms, SpM.i, '.k')
plt.xlabel('Time (ms)')
plt.ylabel('Neuron index')
plt.show()
