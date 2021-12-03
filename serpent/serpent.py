import os
path=os.path
del os

_prebuild = []
_postbuild = []
_targets = targets;
_premake = []
_install = []
_artifacts = []

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
    def _download(url, hash = None):
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
    def _call():
      _download(url=url, hash=hash)
    _install.append(_call);  

def artifact(id, version, files = []):
  def _artifact(id, version, files = []):
    print "publishing artifacts"
  def _call():
    _artifact(id=id, version=version, files=files)    
  _artifacts.append(_call);

def build():
  if action == "install":
     target_package()
  if action == "package":
     target_install()
  if action == "run":
     target_run()
  if action == "prebuild":
     for x in _prebuild: x()
  if action == "postbuild":
     for x in _postbuild: x()     
     
def target_package():
  for x in _artifacts:
    x()

def target_install():
  for x in _install:
    x()

def target_run():  
  print "run"
