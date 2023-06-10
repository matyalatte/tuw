@echo off
REM Builds SimpleCommandRunner with CMake and Visual Studio.
REM SimpleCommandRunner.exe will be generated in Simple-Command-Runner/build/%BUILD_TYPE%

REM You can specify build type as an argument like "build_exe.bat Release"
if /I "%~1"=="Debug" (
    set BUILD_TYPE=Debug
) else (
    set BUILD_TYPE=Release
)
echo Build type: %BUILD_TYPE%

@pushd %~dp0\..
    cmake --preset %BUILD_TYPE%-Windows
    cmake --build --preset %BUILD_TYPE%-Windows
    cd build/%BUILD_TYPE%/Release
    copy /Y SimpleCommandRunner.exe ..\
@popd

pause
