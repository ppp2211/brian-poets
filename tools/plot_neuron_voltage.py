#!/usr/bin/python3

from graph.events import *
import sys
import os
import matplotlib.pyplot as plt
import argparse

parser=argparse.ArgumentParser("Render an event log to dot.")
parser.add_argument('graph', metavar="G", default="-", nargs="?")
parser.add_argument('--output', default="graph.dot")

args=parser.parse_args()    
 
n0 = []
n1 = []
       
def write_graph(dst, src):
    def out(string):
        #sys.stderr.write("{}\n".format(string))
        print(string,file=dst)
        
    class LogSink(LogWriter):
       def onInitEvent(self,e):
           pass
       def onRecvEvent(self,e):
           pass
       def onSendEvent(self,e):
            #eventsBySeq.setdefault(e.dev,[]).append( (e.seq,e) )
            if e.dev == "n_0" and e.pin == "tock":
                if e.S.get("v") is not None:
                    n0.append(e.S.get("v"))
                else: n0.append(0)
            elif e.dev == "n_1" and e.pin == "tock":
                if e.S.get("v") is not None:
                    n1.append(e.S.get("v"))
                else: n1.append(0)
    sink=LogSink()
    
    parseEvents(src,sink)
    
    for i in n1:
        print(i)
    plt.plot(n0)
    plt.plot(n1)
    plt.show()
dst=sys.stdout
if args.output!="-":
    sys.stderr.write("Opening output file {}\n".format(args.output))
    dst=open(args.output,"wt")
write_graph(dst,args.graph)
