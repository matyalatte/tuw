@echo off
REM Builds tests for SimpleCommandRunner.
REM You can also get coverage report if OpenCppCoverage is installed.

if /I "%~1"=="Debug" (
    set BUILD_TYPE=Debug
    set OPTIONS=-Dbuildtype=debug -Dlibui:buildtype=debug
) else (
    set BUILD_TYPE=Release
    set OPTIONS=-Dbuildtype=release  -Dlibui:buildtype=release -Db_vscrt=mt -Db_ndebug=true -Dcpp_rtti=false -Db_lto=true -Dcpp_eh=none -Dlibui:cpp_eh=none
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

set COMMON_OPT=-Ddefault_library=static -Dlibui:default_library=static^
 -Dbuild_exe=false -Dbuild_test=true^
 -Dlibui:tests=false -Dlibui:examples=false

@pushd %~dp0\..
    meson setup build\%BUILD_TYPE%-Test --backend=vs %COMMON_OPT% %OPTIONS%
    if %ERRORLEVEL% neq 0 goto :testend
    meson compile -v -C build\%BUILD_TYPE%-Test
    if %ERRORLEVEL% neq 0 goto :testend

    if "%BUILD_TYPE%"=="Release" goto nocoverage
    if %GET_COVERAGE% equ 0 goto nocoverage

    REM Test and get coverage report from tests.
    set MODULES=--modules %cd%\build\%BUILD_TYPE%-Test\tests
    set SOURCES=--sources %cd%\src
    set EXPORT_TYPE=--export_type html:%cd%\coverage-report
    set WORKDIR=--working_dir %cd%
    OpenCppCoverage --cover_children %WORKDIR% %EXPORT_TYPE% %MODULES% %SOURCES% -- meson test -v -C build\%BUILD_TYPE%-Test
    goto testend

    :nocoverage
    REM Test without OpenCppCoverage
    meson test -v -C build\%BUILD_TYPE%-Test
    if %GET_COVERAGE% equ 0 (
        echo INFO: Install OpenCppCoverage if you want coverage report.
        echo       https://github.com/OpenCppCoverage/OpenCppCoverage/releases
    ) else if "%BUILD_TYPE%"=="Release" echo INFO: Coverage report is available with Debug build.
    :testend
@popd

pause
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
