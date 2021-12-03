import os
path=os.path
del os

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

def download(url, hash = None):
   import urllib2
   url = url
   file_name = url.split('/')[-1]
   u = urllib2.urlopen(url)
   f = open(file_name, 'wb')
   meta = u.info()
   file_size = int(meta.getheaders("Content-Length")[0])
   file_size_dl = 0
   block_sz = 8192
   while True:
       buffer = u.read(block_sz)
       if not buffer:
           break
       file_size_dl += len(buffer)
       f.write(buffer)
       status = r"%10d  [%3.2f%%]" % (file_size_dl, file_size_dl * 100. / file_size)
       status = status + chr(8)*(len(status)+1)
       print status,
   f.close()   
