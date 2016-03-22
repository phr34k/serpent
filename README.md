# Serpent
a python based project generation system and build system.

## Command-line reference

```
env help
env build /t:serpent_project
env build /t:serpent_project /f:BUILDENV
env rebuild /t:serpent_project
env rebuild /t:serpent_project /f:BUILDENV
env clean /t:serpent_project
env clean /t:serpent_project /f:BUILDENV
env configure /t:serpent_project && env rebuild @BUILD_RESP
```

## Language reference

```
# Declares an option that can be set on the command line
serpent.option(
   trigger     = "python-sdk",
   value       = "C:\\Python27-Clean",
   description = "Python sdk location"
)

# Loads an extension module in which freeform python can be executed 
msbuild_ext = serpent.load('BUILDENV.MSBUILD.SRP');

# Generates a Visual C++ project
msbuild_ext.visual_studio(
	project = "msvc/env.project",
	files = serpent.glob(['Serpent/*.cpp']),
	resources = serpent.glob(['Serpent/*.rc']),
	name = 'serpent_project',
	defines = ['AAAA'],
	outdir = 'Bin/Release/',
	intdir = 'Intermediate/env/',
	includes = [os.path.join(serpent.triggers['python-sdk'], 'include')],
	libs = [os.path.join(serpent.triggers['python-sdk'], 'libs/python27.lib')]
)

# Flags a function as a prebuild step which gets evaluated before compilation of targets
@serpent.prebuild
def run():
	print "Error: 1"
	print "Error: 1"
	print "Error: 1"
	print "Warning: 1"
	print "Warning: 1"

# Flags a function as a postbuild step which gets evaluated after compilation of targets
@serpent.postbuild
def run():
	print "Error: 1"
	print "Error: 1"
	print "Error: 1"
	print "Warning: 1"
	print "Warning: 1"


# Language built-in for dictionaries
d = {"a": 2, "b": 5}
e = d["a"]

# Language built-in for lists
x = [1, 2, 3]
x = [1, 2] + [3, 4]

# Language built-in for sets
s = set([1, 2])
s = s | set([4, 5])

```

## Internals

```
serpent.action					Maps to the first argument of serpent i.e. build, help, deploy
serpent.triggers				Maps to options passed with --name=value
serpent.targets					Maps to options passed with /t:name
serpent.platforms				Maps to options passed with /p:name
serpent._SERPENT_COMMAND		The serpent executable used to interpret the script
serpent._SERPENT_VERSION		The version of the executable
serpent._SERPENT_SCRIPT			The current script location
serpent._WORKING_DIR			The current working directory
```

## Getting started

This project currently is only available for windows, it uses python 2.7, so we are going to assume you have it installed somewhere.  Adjust the location in the BUILDENV file to change the location. From that point on you should be able to call env rebuild /t:serpent_project.
