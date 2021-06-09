@echo off
rem DEL /F/Q/S .srp-workarea\Bin\*.* > NUL
if exist ".srp-workarea\Bin\Release\env.exe" (
    goto :RUN
) else (
    goto :REBUILD
)

:RUN
".srp-workarea\Bin\Release\env.exe" %*
goto :EOF 

:REBUILD
echo "rebuilding..."
if "%DevEnvDir%" == "" (
    CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
)

nmake TOOLSET=v140
if exist ".srp-workarea\Bin\Release\env.exe" (
    goto :RUN
) else (
    goto :FAILED
)

:FAILED
echo "deleting files..."
DEL /F/Q/S .srp-workarea\Bin\*.* > NUL
DEL /F/Q/S .srp-workarea\intermediate\*.* > NUL
    RMDIR /Q/S .srp-workarea\Bin
    RMDIR /Q/S .srp-workarea\intermediate
echo Failed to run srp because there were compile errors

:EOF