@echo off
REM Builds tests for SimpleCommandRunner.
REM You can also get coverage report if OpenCppCoverage is installed.

if /I "%~1"=="Debug" (
    set BUILD_TYPE=Debug
    set PRESET=--native-file presets\debug.ini --native-file presets\test.ini
) else (
    set BUILD_TYPE=Release
    set PRESET=--native-file presets\release.ini --native-file presets\test.ini
)
echo Build type: %BUILD_TYPE%

if /I "%~2"=="ARM" (
    if /I "%~1"=="Debug" (
        set PRESET=--cross-file presets\windows_arm64.ini --cross-file presets\debug.ini --cross-file presets\test.ini
    ) else (
        set PRESET=--cross-file presets\windows_arm64.ini --cross-file presets\release.ini --cross-file presets\test.ini
    )
)

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
    meson setup build\%BUILD_TYPE%%~2-Test --backend=vs %PRESET%
    if %ERRORLEVEL% neq 0 goto :testend
    meson compile -v -C build\%BUILD_TYPE%%~2-Test
    if %ERRORLEVEL% neq 0 goto :testend

    if "%BUILD_TYPE%"=="Release" goto nocoverage
    if %GET_COVERAGE% equ 0 goto nocoverage

    REM Test and get coverage report from tests.
    set MODULES=--modules %cd%\build\%BUILD_TYPE%%~2-Test\tests
    set SOURCES=--sources %cd%\src
    set EXPORT_TYPE=--export_type html:%cd%\coverage-report
    set WORKDIR=--working_dir %cd%
    OpenCppCoverage --cover_children %WORKDIR% %EXPORT_TYPE% %MODULES% %SOURCES% -- meson test -v -C build\%BUILD_TYPE%%~2-Test
    goto testend

    :nocoverage
    REM Test without OpenCppCoverage
    meson test -v -C build\%BUILD_TYPE%%~2-Test
    if %GET_COVERAGE% equ 0 (
        echo INFO: Install OpenCppCoverage if you want coverage report.
        echo       https://github.com/OpenCppCoverage/OpenCppCoverage/releases
    ) else if "%BUILD_TYPE%"=="Release" echo INFO: Coverage report is available with Debug build.
    :testend
@popd

pause
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
