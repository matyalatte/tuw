# Building Workflow for Windows

## 0. Requirements

-   Visual Studio 2022
-   CMake (**3.25** or later)
-   Batch files in [`./Simple-Command-Runner/batch_files`](../batch_files)

> The batch files support only Visual Studio 2022.  
> If you don't want to use it, edit some variables in `build_wxWidgets.bat` and `build_exe.bat`.  

> CMake should be 3.25 or later.  
> Or use [v0.2.3](https://github.com/matyalatte/Simple-Command-Runner/tree/v0.2.3) that supports old cmake versions.  

## 1. Build wxWidgets

wxWidgets is a GUI framework.  
You can build it with the following steps.

1.  Run `download_wxWidgets.bat` as admin.
2.  Open the command prompt.
3.  Run `build_wxWidgets.bat` on the command prompt.

## 2-a. Build .exe with Batch Files

You can build Simple Command Runner with batch files.  
The steps are as follows.

1.  Run `build_exe.bat` on the command prompt.
2.  `SimpleCommandRunner.exe` will be generated in `./Simple-Command-Runner/Release/Release`.

## 2-b. Build .exe with Visual Studio

Of course, you can build the exe with Visual Studio.  
The steps are as follows.

1.  Open `./Simple-Command-Runner` with Visual Studio.  
2.  Select `Manage Configurations` from configuration.  
3.  Add `-D wxWidgets_ROOT_DIR=C:/wxWidgets-3.2.2/Release/Installed -D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded` to `CMake command arguments` for `x64-Release`.  
4.  Build SimpleCommandRunner.exe.  

## Debug build

If you want a debug build, you need to use `Debug` as an argument for batch files.  
So, you should type `build_wxWidgets.bat Debug` and `build_exe.bat Debug` on the command prompt.  
If you will build the exe with Visual Studio, you should add `-D wxWidgets_ROOT_DIR=C:/wxWidgets-3.2.2/Debug/Installed` to `CMake command arguments` for `x64-Debug`.

## Test

If you want to build tests, type `test.bat` or `test.bat Debug` on the command prompt.

## Coverage

`test.bat Debug` will make coverage report if [OpenCppCoverage](https://github.com/OpenCppCoverage/OpenCppCoverage/releases) is installed.  

## Uninstall wxWidgets

If you want to uninstall wxWidgets, remove `C:/wxWidgets-*`.
