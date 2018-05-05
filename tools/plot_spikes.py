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
 
spikes = []
time = []
       
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
            if e.pin == "fire":
                spikes.append(int(e.dev[2:]))
                if e.S.get("t") is not None:
                    time.append(e.S.get("t"))
                else: time.append(0)
            

    sink=LogSink()
    
    parseEvents(src,sink)
    
    plt.plot(time, spikes, '.k')
    plt.xlabel("Time (ms)")
    plt.ylabel("Neuron index")
    plt.show()
dst=sys.stdout
if args.output!="-":
    sys.stderr.write("Opening output file {}\n".format(args.output))
    dst=open(args.output,"wt")
write_graph(dst,args.graph)
