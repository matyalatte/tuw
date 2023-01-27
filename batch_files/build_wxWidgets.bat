@echo off

REM Builds wxWidgets with CMake and Visual Studio.

set /p WX_VERSION=< %~dp0\..\WX_VERSION.txt
set VS_VERSION=Visual Studio 17 2022

REM You can specify build type as an argument like "build_wxWidgets.bat Release"
REM The library will be installed in C:\wxWidgets-%WX_VERSION%\%BUILD_TYPE%\Installed
if /I "%~1"=="Debug" (
    set BUILD_TYPE=Debug
) else (
    set BUILD_TYPE=Release
)
echo BUILD_TYPE: %BUILD_TYPE%

set OPTIONS=-G "%VS_VERSION%"^
 -A x64^
 -D CMAKE_CONFIGURATION_TYPES=%BUILD_TYPE%^
 -D CMAKE_INSTALL_PREFIX=C:/wxWidgets-%WX_VERSION%/%BUILD_TYPE%/Installed^
 -D wxBUILD_SHARED=OFF

@pushd C:\wxWidgets-%WX_VERSION%
    mkdir %BUILD_TYPE%
    cd %BUILD_TYPE%
    if "%BUILD_TYPE%"=="Release" (
        set OPTIONS=%OPTIONS% -D wxBUILD_USE_STATIC_RUNTIME=ON
    )
    echo CMake arguments: %OPTIONS%
    cmake %OPTIONS% ../
    cmake --build . --config %BUILD_TYPE%
    cmake --install . --config %BUILD_TYPE%
@popd
