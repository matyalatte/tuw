REM Downloads wxWidget and builds with cmake and VS
REM You need VS2022 and cmake to use this batch file.

set MSBUILD=C:\Program Files\Microsoft Visual Studio\2022\Community\Msbuild\Current\Bin\MSBuild.exe
set VS_VERSION=Visual Studio 17 2022
set WX_VERSION=3.1.5

REM install folder
mkdir -p 3rdparty\wxWidgets-3.1.5


REM ** Download wxWidgets **
curl -OL https://github.com/wxWidgets/wxWidgets/releases/download/v%WX_VERSION%/wxWidgets-%WX_VERSION%.zip
powershell Expand-Archive -Path wxWidgets-%WX_VERSION%.zip


REM ** make project for VS **
cd wxWidgets-%WX_VERSION%
mkdir release
cd release
cmake -G "%VS_VERSION%"^
 -A x64^
 -D CMAKE_BUILD_TYPE=Release^
 -D CMAKE_INSTALL_PREFIX=../../3rdparty/wxWidgets-%WX_VERSION%^
 -D wxBUILD_SHARED=OFF^
 ../


REM ** build with VS **
"%MSBUILD%" INSTALL.vcxproj /t:build /p:configuration=Release /p:platform=x64 -maxcpucount


REM ** remove source codes **
cd ../..
del wxWidgets-%WX_VERSION%.zip
rmdir /s/q wxWidgets-%WX_VERSION%

PAUSE
