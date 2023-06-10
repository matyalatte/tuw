@echo off
REM Builds tests for SimpleCommandRunner.
REM You can also get coverage report if OpenCppCoverage is installed.

REM You can specify build type as an argument like "test.bat Release"
if /I "%~1"=="Debug" (
    set BUILD_TYPE=Debug
) else (
    set BUILD_TYPE=Release
)
echo Build type: %BUILD_TYPE%

REM Check if OpenCppCoverage exists
WHERE OpenCppCoverage >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo OpenCppCoverage: Not found
    set GET_COVERAGE=0
) else (
    echo OpenCppCoverage: Found
    set GET_COVERAGE=1
)

@pushd %~dp0\..

    set PRESET=--preset %BUILD_TYPE%-Windows-Test
    cmake %PRESET% -D BUILD_TESTS=ON
    cmake --build %PRESET%

    if "%BUILD_TYPE%"=="Release" goto nocoverage
    if %GET_COVERAGE% equ 0 goto nocoverage

    REM Test and get coverage report from tests.
    cd ..
    set MODULES=--modules %cd%\build\%BUILD_TYPE%\tests
    set SOURCES=--sources %cd%\src
    set EXPORT_TYPE=--export_type html:%cd%\coverage-report
    set WORKDIR=--working_dir %cd%\build\%BUILD_TYPE%
    OpenCppCoverage --cover_children %WORKDIR% %EXPORT_TYPE% %MODULES% %SOURCES% -- ctest %PRESET%
    goto testend

    :nocoverage
    REM Test without OpenCppCoverage
    ctest %PRESET%
    if %GET_COVERAGE% equ 0 (
        echo INFO: Install OpenCppCoverage if you want coverage report.
        echo       https://github.com/OpenCppCoverage/OpenCppCoverage/releases
    ) else if "%BUILD_TYPE%"=="Release" echo INFO: Coverage report is available with Debug build.
    :testend
@popd

pause
