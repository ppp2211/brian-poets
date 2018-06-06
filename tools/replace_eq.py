import sys
import os

def purge(line):
	l = line.replace("const double", "float")
	l = l.replace(" [brian2.devices.device]", '')
	l = l.replace("dt", 'deviceProperties->dt')
	l = l.replace("tau", 'deviceProperties->tau')
	l = l.replace("I", ' deviceProperties->I ')
	return l

eq = ""

with open(sys.argv[3], 'r') as brianLog:
	for line in brianLog:
		if "const double _v" in line:
			eq += purge(line)
			break
		elif "_lio_1" in line:
			eq += purge(line)
		

print(eq)

with open(sys.argv[1], 'r') as xmlSrc:
	with open(sys.argv[2], 'w') as xmlDst:
		for line in xmlSrc:
			if line.strip() == "//##INSERT_EQUATIONS_HERE##":
				xmlDst.write(eq)
			else:
				xmlDst.write(line)

