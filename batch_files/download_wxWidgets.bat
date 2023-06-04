@echo off

REM Downloads wxWidget.
REM Run it as admin. Or fail to download.

set /p WX_VERSION=< %~dp0\..\WX_VERSION.txt

@pushd C:\
    curl -OL https://github.com/wxWidgets/wxWidgets/releases/download/v%WX_VERSION%/wxWidgets-%WX_VERSION%.zip
    powershell Expand-Archive -Path wxWidgets-%WX_VERSION%.zip
    del wxWidgets-%WX_VERSION%.zip
@popd

set SOURCE_DIR=C:\wxWidgets-%WX_VERSION%\src\msw

REM Fix src\msw\dib.cpp
@pushd %~dp0
    copy /Y %SOURCE_DIR%\dib.cpp dib.buckup.cpp
    powershell -ExecutionPolicy Unrestricted ./fix_wxWidgets.ps1
    copy /Y dib.fixed.cpp %SOURCE_DIR%\dib.cpp
@popd

PAUSE
