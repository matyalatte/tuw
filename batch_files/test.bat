@echo off

REM Builds SimpleCommandRunner with msbuild
REM SimpleCommandRunner.exe will be generated in Simple-Command-Runner/build/Release

REM You need Visual Studio to use this batch file.
REM Run it with "x64 Ntive Tools Command Prompt for VS 2022". Or fail to build.

set VS_VERSION=Visual Studio 17 2022

mkdir %~dp0\..\Debug
@pushd %~dp0\..\Debug

cmake -G "%VS_VERSION%"^
    -A x64^
    -D CMAKE_BUILD_TYPE=Debug^
    -D BUILD_TESTS=ON^
    -D BUILD_EXE=OFF^
    ../

cmake --build . --config Debug

ctest --verbose -C Debug
@popd
