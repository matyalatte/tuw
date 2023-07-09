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
    if %ERRORLEVEL% neq 0 goto :buildend
    cmake --build --preset %BUILD_TYPE%-Windows
    if %ERRORLEVEL% neq 0 goto :buildend
    cd build/%BUILD_TYPE%/%BUILD_TYPE%
    copy /Y SimpleCommandRunner.exe ..\
    :buildend
@popd

pause
