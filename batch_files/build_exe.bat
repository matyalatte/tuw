@echo off

REM Builds SimpleCommandRunner with msbuild
REM SimpleCommandRunner.exe will be generated in Simple-Command-Runner/build/Release

REM You need Visual Studio to use this batch file.
REM Run it with "x64 Ntive Tools Command Prompt for VS 2022". Or fail to build.

set VS_VERSION=Visual Studio 17 2022

REM You can specify build type as an argument like "build_exe.bat Release"
if /I "%~1"=="Release" (
    set BUILD_TYPE=Release
) else if /I "%~1"=="Debug" (
    set BUILD_TYPE=Debug
) else (
    set BUILD_TYPE=Release
)
echo BUILD_TYPE:%BUILD_TYPE%

mkdir %~dp0\..\%BUILD_TYPE%
@pushd %~dp0\..\%BUILD_TYPE%

if "%BUILD_TYPE%"=="Release" (
    REM Release build
    cmake -G "%VS_VERSION%"^
        -A x64^
        -D CMAKE_CONFIGURATION_TYPES=%BUILD_TYPE%^
        -D CMAKE_BUILD_TYPE=%BUILD_TYPE%^
        -D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded^
        ../
) else (
    REM Debug build
    cmake -G "%VS_VERSION%"^
        -A x64^
        -D CMAKE_BUILD_TYPE=%BUILD_TYPE%^
        -D CMAKE_CONFIGURATION_TYPES=%BUILD_TYPE%^
        ../
)

cmake --build . --config %BUILD_TYPE%
@popd
