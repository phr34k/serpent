import os, sys, re

def verify(path):
	os.remove(path)	

def scan(path):
	for subdir, dirs, files in os.walk(path):
	    for file in files:
	        filepath = subdir + os.sep + file        
	        if filepath.endswith(".ncb"):
	            verify(filepath)
	        if filepath.endswith(".sdf"):
	        	verify(filepath)
	        if filepath.endswith(".opensdf"):
	        	verify(filepath)


if __name__ == "__main__":
	if len(sys.argv) >= 1:
		scan(sys.argv[1])
