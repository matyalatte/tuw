@echo off

REM Builds tests for SimpleCommandRunner.

set /p WX_VERSION=< %~dp0\..\WX_VERSION.txt
set VS_VERSION=Visual Studio 17 2022

if /I "%~1"=="Debug" (
    set BUILD_TYPE=Debug
) else (
    set BUILD_TYPE=Release
)
echo BUILD_TYPE: %BUILD_TYPE%

set OPTIONS=-G "%VS_VERSION%"^
 -A x64^
 -D CMAKE_CONFIGURATION_TYPES=%BUILD_TYPE%^
 -D wxWidgets_ROOT_DIR=C:/wxWidgets-%WX_VERSION%/%BUILD_TYPE%/Installed^
 -D BUILD_TESTS=ON^
 -D BUILD_EXE=OFF

set WXRC_CMD=C:/wxWidgets-%WX_VERSION%/%BUILD_TYPE%/Installed/bin/wxrc.exe

mkdir %~dp0\..\%BUILD_TYPE%Test
@pushd %~dp0\..\%BUILD_TYPE%Test
    if "%BUILD_TYPE%"=="Release" (
        set OPTIONS=%OPTIONS% -D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded -D wxWidgets_CONFIGURATION=mswu
    ) else (
        set OPTIONS=%OPTIONS% -D wxWidgets_CONFIGURATION=mswud
    )
    echo CMake arguments: %OPTIONS%

    cmake %OPTIONS% ../
    cmake --build . --config %BUILD_TYPE%
    ctest --verbose --output-on-failure -C %BUILD_TYPE%
@popd
