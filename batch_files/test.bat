@echo off

REM Builds tests for SimpleCommandRunner.
REM You can also get coverage report if OpenCppCoverage is installed.

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

REM Check if OpenCppCoverage exists
WHERE OpenCppCoverage >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo OpenCppCoverage: Not found
    set GET_COVERAGE=0
) else (
    echo OpenCppCoverage: Found
    set GET_COVERAGE=1
)

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

    if "%BUILD_TYPE%"=="Release" goto nocoverage
    if %GET_COVERAGE% equ 0 goto nocoverage

    REM Test and get coverage report from tests.
    cd ..
    set MODULES=--modules %cd%\%BUILD_TYPE%Test\tests
    set SOURCES=--sources %cd%\src
    set EXPORT_TYPE=--export_type html:%cd%\coverage-report
    set CMD=ctest -C %BUILD_TYPE%
    set WORKDIR=--working_dir %cd%\%BUILD_TYPE%Test
    OpenCppCoverage --cover_children %WORKDIR% %EXPORT_TYPE% %MODULES% %SOURCES% -- %CMD%
    goto testend

    :nocoverage
    ctest --verbose --output-on-failure -C %BUILD_TYPE%
    if %GET_COVERAGE% equ 0 (
        echo INFO: Install OpenCppCoverage if you want coverage report.
        echo       https://github.com/OpenCppCoverage/OpenCppCoverage/releases
    ) else if "%BUILD_TYPE%"=="Release" echo INFO: Coverage report is available with Debug build.
    :testend
@popd
