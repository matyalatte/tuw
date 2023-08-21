# Building Workflow for Windows

## Requirements

-   Visual Studio
-   CMake (**3.17** or later)
-   [Meson](https://github.com/mesonbuild/meson/releases) (**0.58** or later)
-   Batch files in [`./Simple-Command-Runner/batch_files`](../batch_files)

## Build

Run `batch_files/build.bat`.

## Compression

You can use [UPX](https://github.com/upx/upx/releases/latest) if you want smaller exe.  
  
`upx SimpleCommandRunner.exe --best`  

## Test

Not yet

## Coverage

Not yet
