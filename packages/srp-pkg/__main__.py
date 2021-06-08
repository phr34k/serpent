import requests, sys, getopt, os

def install():
 	pass

def main(argv):
	inputfile = ''
	uinputfile = ''

	packages = []
	try:
		opts, args = getopt.getopt(argv,"hi:u:")
	except getopt.GetoptError:
		print 'srp-pkg -i <package>'
		print 'srp-pkg -u <package>'
		sys.exit(2)
	for opt, arg in opts:
		if opt in ("-h"):
			print 'srp-pkg -i <package>'
			print 'srp-pkg -u <package>'
		if opt in ("-i"):
			packages.append( (arg, 'install') )
		if opt in ("-u"):
			packages.append( (arg, 'uninstall') )

	if len(packages) > 0:
		for package in packages:
			if package[1] == 'install':
				print 'Install package ', package[0]
				if os.path.isfile(os.path.join(".srp", package[0])):
					print "package was already installed"
				else:
					resp = requests.get("https://raw.githubusercontent.com/phr34k/serpent/master/modules/" + package[0])					
					with open(os.path.join(".srp", package[0]), 'w') as f:
						f.write(resp.content)
			elif package[1] == 'uninstall':
				print 'Remove package ', package[0]
				if os.path.isfile(os.path.join(".srp", package[0])):
					print "package was not installed"
				else:
					os.remove(package[0])

if __name__ == "__main__":
   main(sys.argv[1:])