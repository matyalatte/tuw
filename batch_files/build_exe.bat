@echo off

REM Builds SimpleCommandRunner with CMake and Visual Studio.
REM SimpleCommandRunner.exe will be generated in Simple-Command-Runner/%BUILD_TYPE%/Release

set /p WX_VERSION=< %~dp0\..\WX_VERSION.txt
set VS_VERSION=Visual Studio 17 2022

REM You can specify build type as an argument like "build_exe.bat Release"
if /I "%~1"=="Debug" (
    set BUILD_TYPE=Debug
) else (
    set BUILD_TYPE=Release
)
echo BUILD_TYPE: %BUILD_TYPE%

set OPTIONS=-G "%VS_VERSION%"^
 -A x64^
 -D CMAKE_CONFIGURATION_TYPES=%BUILD_TYPE%^
 -D wxWidgets_ROOT_DIR=C:/wxWidgets-%WX_VERSION%/%BUILD_TYPE%/Installed

set WXRC_CMD=C:/wxWidgets-%WX_VERSION%/%BUILD_TYPE%/Installed/bin/wxrc.exe

mkdir %~dp0\..\%BUILD_TYPE%
@pushd %~dp0\..\%BUILD_TYPE%
    if "%BUILD_TYPE%"=="Release" (
        set OPTIONS=%OPTIONS% -D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded -D wxWidgets_CONFIGURATION=mswu
    ) else (
        set OPTIONS=%OPTIONS% -D wxWidgets_CONFIGURATION=mswud
    )
    echo CMake arguments: %OPTIONS%
    cmake %OPTIONS% ../
    cmake --build . --config %BUILD_TYPE%
@popd
