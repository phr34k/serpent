
PYTHON=C:\Python27\ 
PYTHON_DLL=$(PYTHON)python27.dll
PYTHONLOCATE="exec \"\"\"\nimport win32process, shutil\nfor process in win32process.EnumProcessModules(-1):\n  name = win32process.GetModuleFileNameEx(-1, process)\n  if 'python27.dll' in name:\n    shutil.copyfile(name,'bin/release/python27.dll')\n\"\"\""
TARGET=bin\release\denv.exe
TARGET2=bin\release\env.exe
TOOLSET=v140
CC=cl

SOURCES = \
	serpent/embed.cpp \
        serpent/FileGlobBase.cpp \
        serpent/main.cpp

all: $(TARGET)

clean:
        -del intermediate\*.obj /q
        -del bin\release\denv.exe /q      
        -rmdir intermediate /S /Q               
        -rmdir bin\release /S /Q        
        -rmdir bin /S /Q

install_modules:
        -copy serpent.msbuild.srp .srp\modules
        -copy bin\release\env.exe .srp\.bin\srp.exe

install2:
        -setx SERPENT ^%USERPROFILE^%%\.srp\.bin
        -test.bat	

$(TARGET):$(SOURCES)
	-mkdir bin
	-mkdir bin\release
	-mkdir intermediate
        $(PYTHON)\python.exe -c $(PYTHONLOCATE)
        cd serpent
        $(PYTHON)python.exe package.py > embed.cpp
        cd ..
        $(CC) -I$(PYTHON)include -DWINDOWS -DHAVE_JUNCTIONS /Fointermediate\ /Fe$@ $** $(PYTHON)libs\python27.lib Shell32.lib Rpcrt4.lib Ole32.lib Advapi32.lib
        nmake install_modules
        -$(TARGET) rebuild /t:serpent_project /f:BUILDENV~ --python-sdk=$(PYTHON) --toolset=$(TOOLSET) /nolog
        del $(TARGET)      
        -$(TARGET2) rebuild /t:* /f:BUILDENV --python-sdk=$(PYTHON) --toolset=$(TOOLSET) /nolog
