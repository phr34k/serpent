# Serpent

A python based project/solution generation system and build system, and a collection of usefull maintenance scripts to help assist maintenaning / integrating codebases. This project was created out of the need of the desire to manage a larger codebase programatically in similair fasion but with a clear and conside language, inspired by Google's Bazel and there latent support for Windows serpent was created.

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
msbuild_ext = serpent.load('BUILDENV.MSBUILD.SRP');

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

This project currently is only available for windows, it uses python 2.7, so we are going to assume you have it installed somewhere. To build the environment from source we use the command `nmake PYTHON=C:\Python27\\` this will cause the command to compile a bootstap version of the environment and then use the real environment to furter compile itself with the command `env rebuild /t:serpent_project --python-sdk=C:\Python27`

From that point on the build environment can be used to compile the sample projects using `env build /t:sample_cpp && env run /t:sample_cpp`.

To help you get started to migrate your projects from traditional visual studio projects you can use the command-line `python msbuild/convert_project.py myproject.vcxproj` which will transform your project into serpents project definition. Likewise we also facilate the use of referencing externally managed projects as-is.

## Maintenance Scripts

For a reference and sample output see [Maintenance Script Reference](MAINTENANCE.md)

- [msbuild/compact.py](msbuild/compact.py) scans a directory recursively for .ncb .sdf and .opensdf files and removes them.
- [msbuild/validate.py](msbuild/validate.py) scans a directory recursively for .vcxproj .csproj .vbproj files and performs sanatizing checks such as ensure project files don't reference $(SolutionDir) directly.
- [msbuild/modify-project.py](msbuild/modify-project.py) uses pythonnet and msbuild api to programtically modify projects.
- [msbuild/convert_project.py](msbuild/convert_project.py) uses pythonnet and msbuild api to convert the project.
- [msbuild/change_project.py](msbuild/change_project.py) uses pythonnet and msbuild api to make modifications to the project.
- [msbuild/format_project.py](msbuild/format_project.py) uses pythonnet and msbuild api to reformat the project.
- [msbuild/refs_project.py](msbuild/refs_project.py) uses pythonnet to recurse into a project and list all project references.

## Feedback

Feedback and contributions are always appreciated, just drop a note, or reach out. Please do note that in the current stage the project isn't mature enough to be used in production environments compared to other alternatives such as psake or premake. 
