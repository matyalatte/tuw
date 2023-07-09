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

    REM Remove unnecessary headers
    cd wxWidgets-%WX_VERSION%\include\wx
    rmdir /s /q android dfb gtk gtk1 motif osx qt univ unix x11
@popd

@pushd %~dp0\..
    REM Apply some bug fixes from the latest branch
    set SOURCE_DIR=%USERPROFILE%\wxWidgets-%WX_VERSION%\
    xcopy /s /y wx_fix %SOURCE_DIR%
popd

PAUSE
