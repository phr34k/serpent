
PYTHON=C:\Python27\ 

PYTHON_DLL=$(PYTHON)python27.dll
TARGET=bin\release\denv.exe
TARGET2=bin\release\env.exe
TOOLSET=v140
CC=cl

SOURCES = \
	serpent/embed.cpp \
        serpent/FileGlobBase.cpp \
        serpent/main.cpp

all: $(TARGET)

$(TARGET):$(SOURCES)
	-mkdir bin
	-mkdir bin\release
	-mkdir intermediate
        cd serpent
        $(PYTHON)python.exe package.py > embed.cpp
        cd ..
        $(CC) -I$(PYTHON)include -DWINDOWS /Fointermediate\ /Fe$@ $** $(PYTHON)libs\python27.lib Shell32.lib Rpcrt4.lib Ole32.lib
        copy $(PYTHON_DLL) bin\release\python27.dll
        -$(TARGET) rebuild /t:serpent_project /f:BUILDENV~ --python-sdk=$(PYTHON) --toolset=$(TOOLSET) /nolog
        del $(TARGET)      
        -$(TARGET2) rebuild /t:* /f:BUILDENV --python-sdk=$(PYTHON) --toolset=$(TOOLSET) /nolog
