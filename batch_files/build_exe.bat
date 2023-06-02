@echo off

REM Builds SimpleCommandRunner with CMake and Visual Studio.
REM SimpleCommandRunner.exe will be generated in Simple-Command-Runner/%BUILD_TYPE%/Release

set GENERATOR=Visual Studio 17 2022
echo Generator: %GENERATOR%

set /p WX_VERSION=< %~dp0\..\WX_VERSION.txt

REM You can specify build type as an argument like "build_exe.bat Release"
if /I "%~1"=="Debug" (
    set BUILD_TYPE=Debug
) else (
    set BUILD_TYPE=Release
)
echo Build type: %BUILD_TYPE%

set wxDIR=C:/wxWidgets-%WX_VERSION%/%BUILD_TYPE%/Installed
echo wxWidgets location: %wxDIR%

set OPTIONS=-G "%GENERATOR%"^
 -A x64^
 -D CMAKE_CONFIGURATION_TYPES=%BUILD_TYPE%^
 -D wxWidgets_ROOT_DIR=%wxDIR%

set WXRC_CMD=wxDIR/bin/wxrc.exe

mkdir %~dp0\..\%BUILD_TYPE%
@pushd %~dp0\..\%BUILD_TYPE%
    if "%BUILD_TYPE%"=="Release" (
        set OPTIONS=%OPTIONS% -D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded
    )
    echo CMake arguments: %OPTIONS%
    cmake %OPTIONS% ../
    cmake --build . --config %BUILD_TYPE%
@popd
