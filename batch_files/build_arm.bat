REM Cross compile for ARM64 build

@pushd %~dp0
call build.bat Release ARM
popd
