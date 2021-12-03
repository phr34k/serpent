_prebuild = []
_postbuild = []
_targets = targets;
_premake = []

def prebuild(func):
   def call():
		func()
   def empty():
		pass
   _prebuild.append(call)
   return empty

def postbuild(func):
   def call():
		func()
   def empty():
		pass
   _postbuild.append(call)
   return empty

def package(name, packages = []):   
   if name in targets:
      _targets = list(set(_targets + packages))
   target( name = name )   