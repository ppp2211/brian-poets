#!/usr/bin/python3

from graph.core import *

from graph.load_xml import load_graph_types_and_instances
from graph.save_xml import save_graph
import sys
import os
import math
import random
import re

scriptPath = sys.argv[1]

tauArr = []
IArr = []
neuronSrc = []
neuronDst = []
N = 0
endTime = 0
v_init_str = "v_init=0"

with open(scriptPath, 'r') as brianSrc:
    for line in brianSrc:
        lineArr = re.split('[\[\]\(\)=<>,*]', line.strip().replace(' ',''))
        if (lineArr[0] == "N"):
            N = int(lineArr[1])
        elif (len(lineArr) > 1):
            if (lineArr[1] == "NeuronGroup"):
                thr = float(lineArr[6].strip("'")) #todo process this
                rst = float(lineArr[9].strip("'"))
            elif (lineArr[0] == "G.tau"):
                for i in range(0, N):
                    tauArr.append(float(lineArr[2+i]))    
            elif (lineArr[0] == "G.I"):
                for i in range(0, N):
                    IArr.append(float(lineArr[2+i]))    
            elif (lineArr[1] == "Synapses"):
                 weight = float(lineArr[6].strip("'"))
            elif (lineArr[0] == "S.connect"):
                ij = 0 
                for i in range (2, len(lineArr)):
                     if lineArr[i] is "j":
                         ij = 1
                     elif lineArr[i] is not '':
                         if not ij:
                             neuronSrc.append(int(lineArr[i]))
                         else:
                             neuronDst.append(int(lineArr[i]))
            elif (lineArr[0] == "run"):
                endTime = lineArr[1]
                if lineArr[2] == "s":
                    endTime = endTime * 1000
            elif (lineArr[0] == "G.v"):
                v_init_str = "v_init=" + line.split('=')[1].replace("'","")
            elif (lineArr[0] == "I"):
                I = eval(line.split('=')[1])
            elif (lineArr[0] == "tau"):
                tau = eval(line.split('=')[1].replace("*ms",""))
appBase=os.path.dirname(os.path.realpath(__file__))
src=appBase+"/snn_always_spike_graph_type.xml"
(graphTypes,graphInstances)=load_graph_types_and_instances(src,src)
                
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
    if IArr is not empty:
        I = IArr[i]
    if tauArr is not empty:
        tau = tauArr[i]
    exec(v_init_str)
    props={
        "I":I, "tau":tau, "thr":thr, "rst":rst, "dt":dt, "endTime":endTime, "index":i, "v_init":v_init
    }
    nodes[i]=DeviceInstance(res, "n_{}".format(i), neuronType, props)
    res.add_device_instance(nodes[i])

    res.add_edge_instance(EdgeInstance(res,nodes[i],"tick",clock,"tick",None))
    res.add_edge_instance(EdgeInstance(res,clock,"tock",nodes[i],"tock",None))

for src in neuronSrc:
    for dst in neuronDst:
        ei=EdgeInstance(res, nodes[dst], "input", nodes[src], "fire", {"weight":weight} )
        res.add_edge_instance(ei)

save_graph(res,sys.stdout)
