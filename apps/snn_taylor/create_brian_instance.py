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
connP = -1

with open(scriptPath, 'r') as brianSrc:
    for line in brianSrc:
        lineArr = re.split('[\[\]\(\)=<>,*]', line.strip().replace(' ',''))
        #print(lineArr)
        if (lineArr[0] == "N"):
            N = int(lineArr[1])
        elif (len(lineArr) > 1):
            if (lineArr[1] == "NeuronGroup"):
                thr = float(lineArr[6].strip("'")) #todo process this
                rst = float(lineArr[9].strip("'"))

            elif (lineArr[0] == "tau"):
                for i in range(0, N):
                    tauArr.append(float(lineArr[1]))
            elif (lineArr[0] == "I"):
                for i in range(0, N):
                    IArr.append(float(lineArr[1]))  

            elif (lineArr[0] == "G.tau"):
                for i in range(0, N):
                    tauArr.append(float(lineArr[2+i]))
            elif (lineArr[0] == "G.I"):
                for i in range(0, N):
                    IArr.append(float(lineArr[2+i])) 

            elif (lineArr[1] == "Synapses"):
                weight = float(lineArr[6].strip("'"))
            elif (lineArr[0] == "S.connect"):
                if (lineArr[1] == "condition"):
                    connP = float(lineArr[5])
                else:
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
                endTime = float(lineArr[1])
                if lineArr[2] == "s":
                    endTime = endTime * 1000
                    

'''     
print("N = ",N)
print("Tau =",tauArr)
print("Thr =",thr)
print("Res =",rst)
print("I =",IArr)
print("Src =",neuronSrc,"Dest =",neuronDst)
'''
appBase=os.path.dirname(os.path.realpath(__file__))
src=appBase+"/snn_taylor_graph_type.xml"
(graphTypes,graphInstances)=load_graph_types_and_instances(src,src)

graphType=graphTypes["snn_taylor"]
neuronType=graphType.device_types["neuron"]
clockType=graphType.device_types["clock"]

instName="snn_brian_taylor"

properties={}
res=GraphInstance(instName, graphType, properties)

clock=DeviceInstance(res, "clock", clockType, {"neuronCount":N})
res.add_device_instance(clock)

dt = 0.125

nodes=[None]*N
for i in range(N):
    I = IArr[i] 
    tau = tauArr[i]
#        thr = 1.0 
#        rst = 0.0
    props={
        "I":I, "tau":tau, "thr":thr, "rst":rst, "dt":dt, "endTime":endTime
    }
    nodes[i]=DeviceInstance(res, "n_{}".format(i), neuronType, props)
    res.add_device_instance(nodes[i])

    res.add_edge_instance(EdgeInstance(res,nodes[i],"tick",clock,"tick",None))
    res.add_edge_instance(EdgeInstance(res,clock,"tock",nodes[i],"tock",None))

    #single synapse between neuron 0 and 1 with weight v+=0.2    
#ei=EdgeInstance(res, nodes[1], "input", nodes[0], "fire", {"weight":weight} )
#res.add_edge_instance(ei)

if connP == -1:
    for src in neuronSrc:
        for dst in neuronDst:
            ei=EdgeInstance(res, nodes[dst], "input", nodes[src], "fire", {"weight":weight} )
            res.add_edge_instance(ei)
elif (connP > 0.0):
    for i in range(N):
        for j in range(N):
            x = random.uniform(0, 1)
            if (i != j) and (x <= connP):
                ei=EdgeInstance(res, nodes[j], "input", nodes[i], "fire", {"weight":weight} )
                res.add_edge_instance(ei)

save_graph(res,sys.stdout)
