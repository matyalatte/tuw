@echo off

if /I "%~1"=="Debug" (
    set BUILD_TYPE=Debug
    set OPTIONS=-Dbuildtype=debug -Dlibui:buildtype=debug
) else (
    set BUILD_TYPE=Release
    set OPTIONS=-Dbuildtype=custom -Dlibui:buildtype=custom -Db_vscrt=mt^
                -Db_ndebug=true -Dcpp_rtti=false -Db_lto=true -Dcpp_eh=none -Dlibui:cpp_eh=none^
                -Ddebug=false -Doptimization=s
)
echo Build type: %BUILD_TYPE%

set COMMON_OPT=-Ddefault_library=static -Dlibui:default_library=static^
 -Dlibui:tests=false -Dlibui:examples=false

@pushd %~dp0\..
    meson setup build\%BUILD_TYPE% --backend=vs %COMMON_OPT% %OPTIONS%
    if %ERRORLEVEL% neq 0 goto :buildend
    cd build\%BUILD_TYPE%
    meson compile -v
    :buildend
popd
pause
