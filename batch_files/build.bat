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

set OPTIONS=
if /I "%~2"=="UCRT" (
    set OPTIONS= -Duse_ucrt=true
)

echo Build type: %BUILD_TYPE%

REM Check if MSBuild exists
WHERE MSBuild >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo MSBuild: Not found
) else (
    echo MSBuild: Found
    if not exist %~dp0\..\build\%BUILD_TYPE%%~2 (
        set OPTIONS=%OPTIONS% --backend=vs
    )
)

@pushd %~dp0\..
    meson setup build\%BUILD_TYPE%%~2 %PRESET%%OPTIONS% -Dbuild_test=false
    if %ERRORLEVEL% neq 0 goto :buildend
    cd build\%BUILD_TYPE%%~2
    meson compile -v
    :buildend
popd
