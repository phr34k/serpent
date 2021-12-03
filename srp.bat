@echo off
if exist "%~dp0\.srp-workarea\\Release\env.exe" (
    "%~dp0\.srp-workarea\Bin\Release\env.exe" %*
) else (
    rem "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
    nmake PYTHON=C:\ProgramData\Python\Python310\ PYTHONVERSION=python310 TOOLSET=v143
    if exist "%~dp0\.srp-workarea\Bin\Release\env.exe" (
	"%~dp0\.srp-workarea\Bin\Release\env.exe" %*
    ) else (
	DEL /F/Q/S %~dp0\.srp-workarea\Bin\*.* > NUL
	DEL /F/Q/S %~dp0\.srp-workarea\intermediate\*.* > NUL
        RMDIR /Q/S %~dp0\.srp-workarea\Bin
        RMDIR /Q/S %~dp0\.srp-workarea\intermediate
	echo Failed to run srp because there were compile errors
    )
)

