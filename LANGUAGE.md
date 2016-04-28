
# Language Reference

## General Constructs

The general language constructs originate from python, infact standard compliant python is used. There are a few constraints namely
within a BUILDENV file builtin functions are disabled to help guarantee the portabillity to the file.

```
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

## Options

Declares an option the options can be overriden on the command-line and they can also be reflected. You can use serpent._options
to retrieve the value of a option.

```
# Declares an option that can be set on the command line
serpent.option(
   trigger     = "python-sdk",
   value       = "C:\\Python27-Clean",
   description = "Python sdk location"
)
```

## Extensions

Imports an etension this is similair to the import directive, in fact at the time of writing they are analogous, however this import 
directive allows you to refer to file by absolute or relative file path. The file will only be evaluated/imported once similiar to pythons import directive, prior to importing the directive the working directory is changed, and you can import python dependecies 
from the extension if required.

```
# Loads an extension module in which freeform python can be executed 
msbuild_ext = serpent.load('BUILDENV.MSBUILD.SRP');
```

## Referencing sub-definitions

Logically your project might be scoped out of several directories where each directory could contain a subset of the build definition. Our model allows you to consume that defintion and make it part of the 'bigger picture'.

```
# Loads the partial build script from a subfolder, the triggers and optiones declared will be listed.
serpent.include('samples/cpp/BUILDENV')
```

## Prebuild and postbuild execution

Sometimes it's usefull to have a prebuild or postbuild step that can be called when executing your build definition. For now the build steps can be used to call abritary python commands as long as logic is included from an extension module.

```
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
```

## Project generation via MSBuild Extension

Generates a Visual Studio Project, only vcxproj are supported, targetting Visual Studio 2010 and onwards.

```
# Generates a Visual C++ project
msbuild_ext.visual_studio(
    project = "msvc/env.vcxproj",
    files = serpent.glob(['Serpent/*.cpp']),
    resources = serpent.glob(['Serpent/*.rc']),
    name = 'serpent_project',
    defines = ['AAAA'],
    outdir = 'Bin/Release/',
    intdir = 'Intermediate/env/',
    includes = [os.path.join(serpent.triggers['python-sdk'], 'include')],
    libs = [os.path.join(serpent.triggers['python-sdk'], 'libs/python27.lib')]
)
```

## User properties generation via MSBuild Extension

Generates a Visual Studio Project User File, only vcxproj.user are supported, targetting Visual Studio 2010 and onwards.

```
# Generates the .user file which contain the debug options.
msbuild_ext.visual_studio_debug_options(
    project = "msvc/env.vcxproj",
    executable = 'Bin/Release/env.exe', 
    arguments = 'graph.proj', 
    workingdir = '.'
)
```

## Solution generation via MSBuild Extension

Generates a Visual Studio Solution from named projects.

```
# Generates a solution file with a specific target and add earlier defined Visual C++ Projects to them.
msbuild_ext.visual_studio_solution(
    solution = "Workspace.sln",
    format = 12,
    version = 2012,
    projects = ['serpent_project']
)
```

## Referencing projects via MSBuild Extension

```
msbuild_ext.external(
	name = 'thirdparty_libzmq2',
	project = 'Thirdparty/libzmq-master/builds/msvc/vs2012/libzmq/libzmq.vcxproj',
	depends = ['thirdparty_libzmq2'],
	configuration = ['Release', 'Debug'],
	options = ["SolutionDir=" + serpent.path.abspath("Thirdparty/libzmq-master/builds/msvc/vs2012") + "/"]
)
```

## Generating debug options via MSBuild Extension

```
msbuild_ext.visual_studio_debug_options(
	project = "Game/Snowbunt/Snowbunt.vcxproj",
	executable = 'Bin/Release/GraphIDE/snowbunt.exe', 
	arguments = 'graph.proj', 
	workingdir = '.'
)
```

# Reserved Constructs

# Downloading files

```
serpent.download(
	url 		= "https://github.com/phr34k/serpent/archive/master.zip"
)
```

# Packaging artifacts

```
serpent.artifact(
	id = 'com.serpent',
	version = '123',
	files = ['master.zip']
)
```

