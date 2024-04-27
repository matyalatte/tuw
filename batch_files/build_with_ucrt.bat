REM Build with Universal CRT.
REM This option makes exe much smaller. But the built binary only supports Windows10 or later.

@pushd %~dp0
build.bat Release UCRT
popd
