# Building Workflow for Windows

## Requirements

-   Visual Studio or MinGW-w64
-   [Meson](https://github.com/mesonbuild/meson/releases) (**0.58** or later)
-   Batch files in [`./Tuw/batch_files`](../batch_files)

> [!Note]
> It is recommended to use Visual Studio because MinGW-w64 makes exe larger.

## Install Meson

Get the installer (*.msi) from [the release page](https://github.com/mesonbuild/meson/releases) and launch it on your machine.  

> [!Note]
> If you are a Python user, you can also get meson via pip. (`pip3 install meson`)

## Build

Run `batch_files/build.bat`.  
The executable will be generated in `build/Release/`.  

## Debug

If you want a debug build, run `batch_files/build_debug.bat`.  
To debug the executable in Visual Studio, you can open `build/Debug/Tuw.sln` with VS and set `Tuw` as the startup project.  

## Cross Compile for ARM64

If you have installed ARM64 components in Visual Studio, you can build ARM64 binary on your x64 machine.  
Run `batch_files/build_arm.bat` on the command prompt.  
The ARM64 build will be generated in `build/ReleaseARM/`.  

## Test

To run tests, run [`batch_files/test.bat`](../batch_files/test.bat) or `batch_files/test_debug.bat`.  

## Coverage

`batch_files/test_debug.bat` will generate a coverage report if [OpenCppCoverage](https://github.com/OpenCppCoverage/OpenCppCoverage/releases) is installed.  
