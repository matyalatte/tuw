@echo off

REM Builds wxWidgets with nmake.
REM You need Visual Studio to use this batch file.
REM Run it with "x64 Ntive Tools Command Prompt for VS 2022". Or fail to build.

set /p WX_VERSION=< ../WX_VERSION.txt

@pushd C:\wxWidgets-%WX_VERSION%\build\msw
nmake /f makefile.vc BUILD=release TARGET_CPU=X64 RUNTIME_LIBS=static
@popd
