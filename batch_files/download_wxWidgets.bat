@echo off

REM Downloads wxWidget.
REM Run it as admin. Or fail to install.

set /p WX_VERSION=< %~dp0\..\WX_VERSION.txt

@pushd C:\
curl -OL https://github.com/wxWidgets/wxWidgets/releases/download/v%WX_VERSION%/wxWidgets-%WX_VERSION%.zip
powershell Expand-Archive -Path wxWidgets-%WX_VERSION%.zip
del wxWidgets-%WX_VERSION%.zip
@popd

PAUSE
