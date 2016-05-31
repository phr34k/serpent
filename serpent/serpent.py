import os, tarfile, StringIO
path=os.path

_prebuild = []
_postbuild = []
_targets = targets;
_premake = []
_install = []
_artifacts = []
_publish = []

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

def artifact(id, version, files = [], dependecies = []):
  workingdir = _WORKING_DIR
  for x in range(0, len(files)): files[x] = path.abspath(files[x])

  def _artifact_install(id, version):
    repository_search(id, version)
    tar = tarfile.open(".srp/%s.%s.tar.bz2" % (id, version), "r:bz2")
    f=tar.extractfile('.spkg')
    content=f.read()
    print content

    for file in tar:
      data = tar.extractfile(file.name).read()
      dirname = os.path.dirname('.srp/' + file.name)
      if os.path.exists(dirname) == False: os.makedirs(dirname)
      fout = open('.srp/' + file.name, "wb")
      fout.write(data)
      fout.close()
    tar.close()
  def _artifact_create(id, version, files = [], dependecies = []):
    tar = tarfile.open(".srp/%s.%s.tar.bz2" % (id, version), "w:bz2")

    string = StringIO.StringIO()
    string.write("""Serpent Package Information\n""");
    string.write("""Id: %s\n""" % id);
    string.write("""Version: %s\n""" % version);
    for dep in dependecies:
      string.write("""Dependecy: %s\n""" % dep);
    string.seek(0)
    info = tarfile.TarInfo(name=".spkg")
    info.size=len(string.buf)
    tar.addfile(tarinfo=info, fileobj=string)

    for name in files:
      relative_path = os.path.relpath(name, os.path.dirname(workingdir))
      tar.add(name, arcname=relative_path)
      print relative_path
    tar.close()
  def _create():
    print "create artifacts"
    _artifact_create(id=id, version=version, files=files, dependecies=dependecies)
  def _inst():
    _artifact_install(id=id, version=version)

  _artifacts.append(_create);
  _install.append(_inst);

def build():
  if action is not None:
    g = globals()
    functionName = "target_" + action
    if functionName in g:
       g[functionName]()
    else:
       print "function does not exists"

def target_build():
  print "Building..."     

def target_rebuild():
  print "Rebuilding..."
  g = globals()

def target_clean():    
  print "Clean..."

def target_package():
  for x in _publish:
    x()
     
def target_package():
  print "Packaging artifacts..."
  for x in _artifacts:
    x()

def target_install():
  print "Resolving artifacts..."
  for x in _install:
    x()

def target_prebuild():
  for x in _prebuild: x()

def target_postbuild():  
  for x in _postbuild: x()

def target_run():  
  print "run"
