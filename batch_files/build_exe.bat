@echo off

REM Builds SimpleCommandRunner with msbuild
REM SimpleCommandRunner.exe will be generated in Simple-Command-Runner/build/Release

REM You need Visual Studio to use this batch file.
REM Run it with "x64 Ntive Tools Command Prompt for VS 2022". Or fail to build.

set VS_VERSION=Visual Studio 17 2022

mkdir ..\build
@pushd ..\build

cmake -G "%VS_VERSION%"^
 -A x64^
 -D CMAKE_BUILD_TYPE=Release^
 -D CMAKE_BUILD_SHARED=OFF^
 -D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded^
 ../

msbuild SimpleCommandRunner.vcxproj /t:build /p:configuration=Release /p:platform=x64 -maxcpucount

@popd
