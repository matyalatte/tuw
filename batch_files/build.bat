@echo off

if /I "%~1"=="Debug" (
    set BUILD_TYPE=Debug
    set PRESET=--native-file presets\debug.ini
) else (
    set BUILD_TYPE=Release
    set PRESET=--native-file presets\release.ini
)

if /I "%~2"=="ARM" (
    if /I "%~1"=="Debug" (
        set PRESET=--cross-file presets\windows_arm64.ini --cross-file presets\debug.ini
    ) else (
        set PRESET=--cross-file presets\windows_arm64.ini --cross-file presets\release.ini
    )
)

echo Build type: %BUILD_TYPE%

@pushd %~dp0\..
    meson setup build\%BUILD_TYPE%%~2 --backend=vs %PRESET%
    if %ERRORLEVEL% neq 0 goto :buildend
    cd build\%BUILD_TYPE%%~2
    meson compile -v
    :buildend
popd
pause
