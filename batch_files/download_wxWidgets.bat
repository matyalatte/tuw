@echo off

REM Downloads wxWidget.
REM Run it as admin. Or fail to install.

set /p WX_VERSION=< %~dp0\..\WX_VERSION.txt

REM You can specify build type as an argument like "build_exe.bat Release"
if /I "%~1"=="Release" (
    set BUILD_TYPE=Release
) else if /I "%~1"=="Debug" (
    set BUILD_TYPE=Debug
) else (
    set BUILD_TYPE=Release
)
echo BUILD_TYPE:%BUILD_TYPE%

@pushd C:\
    curl -OL https://github.com/wxWidgets/wxWidgets/releases/download/v%WX_VERSION%/wxWidgets-%WX_VERSION%.zip
    powershell Expand-Archive -Path wxWidgets-%WX_VERSION%.zip
    move wxWidgets-%WX_VERSION% wxWidgets-%WX_VERSION%-%BUILD_TYPE%
    del wxWidgets-%WX_VERSION%.zip
@popd

PAUSE
