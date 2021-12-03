@echo off
if exist "%~dp0Bin\Release\env.exe" (
    "%~dp0Bin\Release\env.exe" %*
) else (
    "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
    nmake PYTHON=D:\SDK\pyhton27\\ TOOLSET=v140
    if exist "%~dp0Bin\Release\env.exe" (
	"%~dp0Bin\Release\env.exe" %*
    ) else (
	DEL /F/Q/S %~dp0Bin\*.* > NUL
	DEL /F/Q/S %~dp0intermediate\*.* > NUL
        RMDIR /Q/S %~dp0Bin
        RMDIR /Q/S %~dp0intermediate
	echo Failed to run srp because there were compile errors
    )
)

