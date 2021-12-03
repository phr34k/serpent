# Serpent

A python based project/solution generation system and build system, and a collection of usefull maintenance scripts to help assist maintenaning / integrating codebases. This project was created out of the need of the desire to manage a larger codebase programatically in similair fasion but with a clear and conside language, inspired by Google's Bazel and there latent support for Windows serpent was created.

[![Build status](https://ci.appveyor.com/api/projects/status/xd4wejmpfw8oauv3?svg=true)](https://ci.appveyor.com/project/phr34k/serpent)

## Command-line reference

```
srp help
srp build /t:serpent_project
srp build /t:serpent_project /f:BUILDENV
srp rebuild /t:serpent_project
srp rebuild /t:serpent_project /f:BUILDENV
srp clean /t:serpent_project
srp clean /t:serpent_project /f:BUILDENV
srp configure /t:serpent_project && env rebuild @BUILD_RESP
srp install //rserved
srp package //rserved
```

## A Quick look at the language 

For a complete language reference see [Language Reference](LANGUAGE.md)

```
# Declares an option that can be set on the command line
serpent.option(
   trigger     = "python-sdk",
   value       = "C:\\Python27-Clean",
   description = "Python sdk location"
)

# Loads an extension module in which freeform python can be executed 
msbuild_ext = serpent.load('https://raw.githubusercontent.com/phr34k/serpent/master/serpent.msbuild.srp') or serpent.load('serpent.msbuild.srp');

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

# Generates the .user file which contain the debug options.
msbuild_ext.visual_studio_debug_options(
	project = "msvc/env.vcxproj",
	executable = 'Bin/Release/env.exe', 
	arguments = 'graph.proj', 
	workingdir = '.'
)

# Generates a solution file with a specific target and add earlier defined Visual C++ Projects to them.
msbuild_ext.visual_studio_solution(
	solution = "Workspace.sln",
	format = 12,
	version = 2012,
	projects = ['serpent_project']
)

# Loads the partial build script from a subfolder, the triggers and optiones declared will be listed.
serpent.include('samples/cpp/BUILDENV')
```

## Build packages & Additional Python Packages

By default serpent creates a directory under the users home directory i.e. `%USERPROFILE%/.srp/` following the same conventions of populair tools. The .srp directory is used to user configured properties as well as a repository where build definitions can be downloaded. In the future this mechanism will be used to resolve definitions from the internet as illustrated in the above example.

By default python searches for additional python modules/packages in `%PYTHONHOME%`, however within serpent a different variable is used `%SERPENTHOME%`. You may define SERPENTHOME as PYTHONHOME, if you wish to use the same packages for both serpent and python.

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

This project currently is only available for windows, it uses python 2.7, so we are going to assume you have it installed somewhere. To build the environment from source we use the command `nmake PYTHON=C:\Python27\\ TOOLSET=v110` this will cause the command to compile a bootstap version of the environment and then use the real environment to furter compile itself with the command `env rebuild /t:serpent_project --python-sdk=C:\Python27`. 

From that point on the build environment can be used to compile the sample projects using `env build /t:sample_cpp && env run /t:sample_cpp`. To build with a different toolset you can use the option `--toolset=v110` or any of the visual studio compatible 
toolset v110 (2012), v120 (2013), v140 (2015)

To help you get started to migrate your projects from traditional visual studio projects you can use the command-line `python msbuild/convert_project.py myproject.vcxproj` which will transform your project into serpents project definition. Likewise we also facilate the use of referencing externally managed projects as-is.

## Maintenance Scripts

A collection of maintenance scripts can be installed with a python package, you can perform `pip install -I pip install git+git://github.com/phr34k/serpent.git#subdirectory=packages` [Maintenance Script Reference](MAINTENANCE.md)

## Feedback

Feedback and contributions are always appreciated, just drop a note, or reach out. Please do note that in the current stage the project isn't mature enough to be used in production environments compared to other alternatives such as psake or premake. 
