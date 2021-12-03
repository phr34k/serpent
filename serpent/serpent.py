import os, tarfile
path=os.path
del os

_prebuild = []
_postbuild = []
_targets = targets;
_premake = []
_install = []
_artifacts = []

default = ();

def select(values):
  if True in values:
     return values[True]
  if default in values:
     return values[default]
  return None

def join(*args, **kwargs):  
  workingdir= kwargs.get('workingdir', '.')
  l = []
  for x in range(len(args)):
    l.append(path.join(workingdir, args[x]))
  return l

def enum(**enums):
    return type('Enum', (), enums)

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
  def _artifact_install(id, version):
    tar = tarfile.open(".srp/%s.%s.tar.bz2" % (id, version), "r:bz2")
    for file in tar:
      data = tar.extractfile(file.name).read()
      fout = open(file.name, "w")
      fout.write(data)
      fout.close()
    tar.close()
  def _artifact_create(id, version, files = []):
    tar = tarfile.open(".srp/%s.%s.tar.bz2" % (id, version), "w:bz2")
    for name in files: tar.add(name)
    tar.close()
  def _create():
    _artifact_create(id=id, version=version, files=files)
  def _inst():
    _artifact_install(id=id, version=version)

  _artifacts.append(_create);
  _install.append(_inst);

def build():
  if action == "install":
     target_install()
  if action == "package":
     target_package()
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
