# Building Workflow for Windows

## Requirements

-   Visual Studio
-   [Meson](https://github.com/mesonbuild/meson/releases) (**0.58** or later)
-   Batch files in [`./Simple-Command-Runner/batch_files`](../batch_files)

> :warning: MinGW-w64 is not supported.  
> (It might work, but if you find issues, you need to solve them by yourself.)  

## Install Meson

Get the installer (*.msi) from [the release page](https://github.com/mesonbuild/meson/releases).  
And launch it on your machine.  

> If you are a Python user, you can also get meson via pip. (`pip3 install meson`)

## Build

Run `batch_files/build.bat`.  
The executable will be generated in `build/Release/`.  

## Debug

If you want a debug build, run `batch_files/build_debug.bat`.  
If you want to debug the exe on Visual Studio, you can open `build/Debug/SimpleCommandRunner.sln` with it.  
Then, set `SimpleCommandRunner` as the startup project.  

## Cross Compile for ARM64

If you installed ARM64 components in Visual Studio, you can build ARM64 binary on x64 machine.  
Run `batch_files/build_arm.bat` on the command prompt.  
The ARM64 version will be generated in `build/ReleaseARM/`.  

## Test

If you want to run tests, run [`batch_files/test.bat`](../batch_files/test.bat) or `batch_files/test_debug.bat`.  

## Coverage

`batch_files/test_debug.bat` will make coverage report if [OpenCppCoverage](https://github.com/OpenCppCoverage/OpenCppCoverage/releases) is installed.  
