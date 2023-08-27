@echo off


if /I "%~1"=="Debug" (
    set BUILD_TYPE=Debug
    set OPTIONS=-Dbuildtype=debug -Dlibui:buildtype=debug
) else (
    set BUILD_TYPE=Release
    set OPTIONS=-Dbuildtype=release  -Dlibui:buildtype=release -Db_vscrt=mt -Db_ndebug=true -Dcpp_rtti=false -Db_lto=true
)
echo Build type: %BUILD_TYPE%

set COMMON_OPT=-Ddefault_library=static -Dlibui:default_library=static^
 -Dlibui:tests=false -Dlibui:examples=false

@pushd %~dp0\..
meson setup build\%BUILD_TYPE% --backend=vs %COMMON_OPT% %OPTIONS%
cd build\%BUILD_TYPE%
meson compile -v
popd
pause
