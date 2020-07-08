
PYTHON=C:\Python27\ 
PYTHON_DLL=$(PYTHON)python27.dll
PYTHONLOCATE="exec \"\"\"\nimport win32process, shutil\nfor process in win32process.EnumProcessModules(-1):\n  name = win32process.GetModuleFileNameEx(-1, process)\n  if 'python27.dll' in name:\n    shutil.copyfile(name,'.srp-workarea/bin/release/python27.dll')\n\"\"\""
TARGET=bin\release\denv.exe
TARGET2=.srp-workarea\bin\release\env.exe
TOOLSET=v140
CC=cl

SOURCES = \
	serpent/embed.cpp \
        serpent/FileGlobBase.cpp \
        serpent/main.cpp \
        serpent/sha256.cpp

all: $(TARGET)

clean:
        -del .srp-workarea\intermediate\*.obj /q
        -del .srp-workarea\bin\release\denv.exe /q      
        -rmdir .srp-workarea\intermediate /S /Q               
        -rmdir .srp-workarea\bin\release /S /Q        
        -rmdir .srp-workarea\bin /S /Q

embed:
		cd serpent
        $(PYTHON)python.exe package.py > embed.cpp	
		cd ..        

install_modules:
	    -copy .srp\modules\serpent.msbuild.srp .
        -copy modules\serpent.msbuild.srp .srp\modules
        -copy .srp-workarea\bin\release\env.exe .srp\.bin\srp.exe

install2:
        -setx SERPENT ^%USERPROFILE^%%\.srp\.bin
        -test.bat	

$(TARGET):$(SOURCES)
	-mkdir bin
	-mkdir bin\release
	-mkdir intermediate
        -del .srp-workarea\bin\release\env.exe /q >nul 2>&1    	
        -del .srp-workarea\bin\release\denv.exe /q >nul 2>&1     	
        -del .srp-workarea\intermediate\*.obj /q >nul 2>&1	

        cd serpent
        $(PYTHON)python.exe package.py > embed.cpp
        cd ..
        $(CC) -I$(PYTHON)include -DWINDOWS -DHAVE_JUNCTIONS /Fointermediate\ /Fe$@ $** $(PYTHON)libs\python27.lib Shell32.lib Rpcrt4.lib Ole32.lib Advapi32.lib
	    -copy .srp\modules\serpent.msbuild.srp modules        
        -copy modules\serpent.msbuild.srp .srp\modules

        -$(TARGET) rebuild /t:serpent_project /f:BUILDENV~ --python-sdk=$(PYTHON) --toolset=$(TOOLSET)
        del $(TARGET)              
        $(PYTHON)\python.exe -c $(PYTHONLOCATE)        

        -del intermediate\*.obj /q >nul 2>&1         
        -del bin\release\denv.exe /q >nul 2>&1      
        -rmdir intermediate /S /Q >nul 2>&1               
        -rmdir bin\release /S /Q >nul 2>&1
        -rmdir bin /S /Q >nul 2>&1

        -$(TARGET2) rebuild /t:* /f:BUILDENV --python-sdk=$(PYTHON) --toolset=$(TOOLSET) /nolog
        -copy $(TARGET2) .srp\.bin\srp.exe
