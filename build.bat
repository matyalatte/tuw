REM Builds SimpleCommandRunner with cmake and VS2022
REM SimpleCommandRunner.exe will be generated in ./build/Release

set MSBUILD=C:\Program Files\Microsoft Visual Studio\2022\Community\Msbuild\Current\Bin\MSBuild.exe
set VS_VERSION=Visual Studio 17 2022

mkdir build
cd build

REM make project for VS
cmake -G "%VS_VERSION%"^
 -A x64^
 -D CMAKE_BUILD_TYPE=Release^
 -D CMAKE_BUILD_SHARED=OFF^
 ../

REM build with VS

"%MSBUILD%" SimpleCommandRunner.vcxproj /t:build /p:configuration=Release /p:platform=x64 -maxcpucount

pause
