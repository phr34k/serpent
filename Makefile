
PYTHON=C:\Python27\ 

TARGET=bin\release\denv.exe
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
        $(CC) -I$(PYTHON)\include -DWINDOWS /Fointermediate\ /Fe$@ $** $(PYTHON)\libs\python27.lib Shell32.lib
        copy $(PYTHON)python27.dll bin\release\python27.dll
        $(TARGET) rebuild /t:serpent_project --python-sdk=$(PYTHON)
        del $(TARGET)      
