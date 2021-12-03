import sys, os;
base_dir = os.path.dirname(os.path.realpath(__file__))
if sys.maxsize > 2**32:
	sys.path.append( os.path.join(base_dir, "x64"))
else:	
	sys.path.append( os.path.join(base_dir, "x86"))