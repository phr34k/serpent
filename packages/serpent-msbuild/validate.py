import os, sys, re

required_intdir = '$(ADK_INT_DIR)'
required_outputdir = '$(ADK_BIN_PATH)'
required_targets = ['Debug|Win32', 'Release|Win32']

def is_external(path):
	if 'Thirdparty' not in path and 'extern' not in path and 'msvc' not in path:
		return False
	return True

def is_properties(path):
	if path.endswith('.props'):
		return True
	return False

def verify(path):
	result = open(path).read();
	if is_external(path) == False and is_properties(path) == False:

		# Check for Solution Dir references, this will in general break consuming msbuild project in different solutions 
		if '$(SolutionDir)' in result:
			print ('Error: project %s depends on $(SolutionDir)' % path)

		# Parse the msbuild file with python to check for more detailed syntax and incompatabillity.
		import xml.etree.ElementTree as ET
		result = re.sub('\\sxmlns="[^"]+"', '', result, count=1)
		root = ET.fromstring(result)

		# Check the OutDir references in a project, we want the references to refer to a single location so that all binaries can easilly be distributed.
		for e in root.findall('PropertyGroup/OutDir'):
			if required_outputdir not in e.text:
				print ('Error: project %s does not output in %s' % (path, required_outputdir))

		# Check the OutDir references in a project, we want the references to refer to a single location so that all binaries can easilly be distributed.
		for e in root.findall('PropertyGroup/OutputPath'):
			if required_outputdir not in e.text:
				print ('Error: project %s does not output in %s' % (path, required_outputdir))

		# Check the IntDir references in a project, we want the references to refer to a single location so that all binaries can easilly be distributed.
		for e in root.findall('PropertyGroup/IntDir'):
			if required_intdir not in e.text:
				print ('Error: project %s intermediate dir does not contain %s' % (path, required_intdir))

		# Check the IntDir references in a project, we want these references to include the Platform macro
		for e in root.findall('PropertyGroup/IntDir'):
			if '$(Platform)' not in e.text:
				print ('Error: project %s intermediate dir does not contain $(Platform)' % path)

		# Check the IntDir references in a project, we want these references to include the Configuration macro
		for e in root.findall('PropertyGroup/IntDir'):
			if '$(Configuration)' not in e.text:
				print ('Error: project %s intermediate dir does not contain $(Configuration)' % path)

		# Check the IntDir references in a project, we want the references to refer to a single location so that all binaries can easilly be distributed.
		for e in root.findall('PropertyGroup/BaseIntermediateOutputPath'):
			if required_intdir not in e.text:
				print ('Error: project %s intermediate dir does not contain %s' % (path, required_intdir))						

		# Check the IntDir references in a project, we want the references to refer to a single location so that all binaries can easilly be distributed.
		for e in root.findall('PropertyGroup/IntermediateOutputPath'):
			if '$(Platform)' not in e.text:
				print ('Error: project %s intermediate dir does not contain $(Platform)' % path)
				print e.text

		# Check the IntDir references in a project, we want the references to refer to a single location so that all binaries can easilly be distributed.
		for e in root.findall('PropertyGroup/IntermediateOutputPath'):
			if '$(Configuration)' not in e.text:
				print ('Error: project %s intermediate dir does not contain $(Configuration)' % path)				
				print e.text


		# Check for the required project configurations.
		desired = {}		
		for e in root.findall('ItemGroup/ProjectConfiguration'):
			desired[e.attrib['Include']] = True
		for e in root.findall('PropertyGroup'):		
			if 'Condition' in e.attrib:
				if re.match("\s*'\$\(Configuration\)\|\$\(Platform\)'\s*==\s*'([^']+)'\s*", e.attrib['Condition']):
					desired[re.match("\s*'\$\(Configuration\)\|\$\(Platform\)'\s*==\s*'([^']+)'\s*", e.attrib['Condition']).groups(1)[0]] = True
		
		for target in required_targets:
			if target not in desired:
				print ('Error: project %s does not support %s' % (path, target))
			else:
				del desired[target]



		# Find all ClInclude
		for e in root.findall('ClInclude/*'):
			if re.match("[A-Z]:/*", e.attrib['Include']):
				print ('Error: project %s contains absolute paths to file ' % (path, e.attrib['Include']))

		# Find all ClCompile
		for e in root.findall('ClCompile/*'):
			if re.match("[A-Z]:/*", e.attrib['Include']):
				print ('Error: project %s contains absolute paths to file ' % (path, e.attrib['Include']))				

def scan(path):
	for subdir, dirs, files in os.walk(path):
	    for file in files:
	        filepath = subdir + os.sep + file        
	        if filepath.endswith(".vcxproj"):
	            verify(filepath)
	        if filepath.endswith(".csproj"):
	        	verify(filepath)
	        if filepath.endswith(".vbproj"):
	        	verify(filepath)
	        if filepath.endswith(".props"):
	        	verify(filepath)


if __name__ == "__main__":
	if len(sys.argv) >= 1:
		scan(sys.argv[1])