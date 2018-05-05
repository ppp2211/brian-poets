#!/usr/bin/python3

from graph.core import *

from graph.load_xml import load_graph_types_and_instances
from graph.save_xml import save_graph
import sys
import os
import math
import random

urand=random.random

import os
appBase=os.path.dirname(os.path.realpath(__file__))

src=appBase+"/snn_always_spike_graph_type.xml"
(graphTypes,graphInstances)=load_graph_types_and_instances(src,src)

'''
if len(sys.argv)>1:
    Ne=int(sys.argv[1])
if len(sys.argv)>2:
    Ni=int(sys.argv[2])
if len(sys.argv)>3:
    K=int(sys.argv[3])

N=Ne+Ni
K=min(N,K)
'''
N = 2 #two neurons for now

graphType=graphTypes["snn_always_spike"]
neuronType=graphType.device_types["neuron"]
clockType=graphType.device_types["clock"]

instName="snn_always_spike_twonode"

properties={}
res=GraphInstance(instName, graphType, properties)

clock=DeviceInstance(res, "clock", clockType, {"neuronCount":N})
res.add_device_instance(clock)

dt = 0.125

nodes=[None]*N
for i in range(N):
    if i==0:
        I = 2
        tau = 10
        thr = 1 
        rst = 0
    else:
        I = 0
        tau = 100
        thr = 1 
        rst = 0
    props={
        "I":I, "tau":tau, "thr":thr, "rst":rst, "dt":dt
    }
    nodes[i]=DeviceInstance(res, "n_{}".format(i), neuronType, props)
    res.add_device_instance(nodes[i])

    res.add_edge_instance(EdgeInstance(res,nodes[i],"tick",clock,"tick",None))
    res.add_edge_instance(EdgeInstance(res,clock,"tock",nodes[i],"tock",None))

    #single synapse between neuron 0 and 1 with weight v+=0.2    
ei=EdgeInstance(res, nodes[1], "input", nodes[0], "fire", {"weight":0.2} )
res.add_edge_instance(ei)
'''
for dst in range(N):
    free=list(range(N))
    random.shuffle(free)
    for i in range(K):
        src=free[i]
        
        if src<Ne:
            S=0.5*urand()
        else:
            S=-urand()
        ei=EdgeInstance(res, nodes[dst], "input", nodes[src], "fire", {"weight":S} )
        res.add_edge_instance(ei)
'''

save_graph(res,sys.stdout)
