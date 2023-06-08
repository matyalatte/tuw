@echo off
REM Downloads wxWidget.
REM Run it as admin. Or fail to download.

REM wxWidgets version is defined in ./Simple-Command-Runner/WX_VERSION.txt
set /p WX_VERSION=< %~dp0\..\WX_VERSION.txt

REM Download source codes
@pushd %USERPROFILE%
    curl -OL https://github.com/wxWidgets/wxWidgets/releases/download/v%WX_VERSION%/wxWidgets-%WX_VERSION%.zip
    powershell Expand-Archive -Path wxWidgets-%WX_VERSION%.zip
    del wxWidgets-%WX_VERSION%.zip
@popd

REM Fix src\msw\dib.cpp
set SOURCE_DIR=%USERPROFILE%\wxWidgets-%WX_VERSION%\src\
@pushd %~dp0
    copy /Y %SOURCE_DIR%\msw\dib.cpp dib.cpp
    copy /Y %SOURCE_DIR%\common\url.cpp url.cpp
    powershell -ExecutionPolicy Unrestricted ./fix_wxWidgets.ps1
    move /Y dib.cpp %SOURCE_DIR%\msw\dib.cpp
    move /Y url.cpp %SOURCE_DIR%\common\url.cpp
@popd

PAUSE
