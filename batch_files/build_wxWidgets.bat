@echo off

REM Builds wxWidgets with nmake.
REM You need Visual Studio to use this batch file.
REM Run it with "x64 Ntive Tools Command Prompt for VS 2022". Or fail to build.

set /p WX_VERSION=< %~dp0\..\WX_VERSION.txt
set VS_VERSION=Visual Studio 17 2022

REM You can specify build type as an argument like "build_wxWidgets.bat Release"
REM The built library will be installed in C:\wxWidgets-%WX_VERSION%\Installed\%BUILD_TYPE%
if /I "%~1"=="Release" (
    set BUILD_TYPE=Release
) else if /I "%~1"=="Debug" (
    set BUILD_TYPE=Debug
) else (
    set BUILD_TYPE=Release
)
echo BUILD_TYPE:%BUILD_TYPE%

@pushd C:\wxWidgets-%WX_VERSION%-%BUILD_TYPE%\build\msw
    if "%BUILD_TYPE%"=="Release" (
        REM Release build
        nmake /f makefile.vc BUILD=release TARGET_CPU=X64 RUNTIME_LIBS=static
    ) else (
        REM Debug build
        nmake /f makefile.vc BUILD=debug TARGET_CPU=X64 
    )
@popd
