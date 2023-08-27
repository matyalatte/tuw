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
The executable will be generated in `build\Release\`.  

## Debug

If you want a debug build, run `batch_files/build.bat Debug` on the command prompt.  
If you want to debug the exe on Visual Studio, you can open `build/Debug/SimpleCommandRunner.sln` with it.  
Then, set `SimpleCommandRunner` as the startup project.  

## Compression

You can use [UPX](https://github.com/upx/upx/releases/latest) if you want smaller exe.  
  
`upx SimpleCommandRunner.exe --best`  

## Test

Not yet

## Coverage

Not yet
