# Building Workflow for Windows

## 0. Requirements

-   Visual Studio 2022
-   CMake (**3.25** or later)
-   Batch files in [`./Simple-Command-Runner/batch_files`](../batch_files)

> Note that this workflow supports only Visual Studio 2022.  
> If you don't want to use it, you should edit some variables in [`build_wxWidgets.bat`](../batch_files/build_wxWidgets.bat) and [`CMakePresets.json`](../CMakePresets.json).  

## 1. Build wxWidgets

wxWidgets is a GUI framework.  
You can build it with the following steps.

1.  Run [`download_wxWidgets.bat`](../batch_files/download_wxWidgets.bat).
2.  Run [`build_wxWidgets.bat`](../batch_files/build_wxWidgets.bat).
3.  Type `build_wxWidgets.bat Debug` on the command prompt if you want a debug build.

## 2-a. Build .exe with Batch Files

You can build Simple Command Runner with batch files.  
The steps are as follows.

1.  Run [`build_exe.bat`](../batch_files/build_exe.bat).
2.  `SimpleCommandRunner.exe` will be generated in `./Simple-Command-Runner/build/Release`.
3.  Type `build_exe.bat Debug` on the command prompt if you want a debug build.

## 2-b. Build .exe with Visual Studio

Of course, you can build the exe with Visual Studio.  
The steps are as follows.

1.  Open `./Simple-Command-Runner` with Visual Studio.
2.  Select one of the configurations.
3.  Open [`CMakeLists.txt`](../CMakeLists.txt) in Visual Studio.
4.  Push `ctrl+s` to run the configuration.
5.  Select `SimpleCommandRunner.exe` as a startup item.
6.  Build `SimpleCommandRunner.exe`.

## Compression

The built binary will be 2 or 3 MB.  
You should use [UPX](https://github.com/upx/upx/releases/latest) if you want smaller exe.  
  
`upx SimpleCommandRunner.exe --best`  

## Test

If you want to run tests, run [`test.bat`](../batch_files/test.bat) or type `test.bat Debug` on the command prompt.  
In Visual Studio, you can also run the tests from `Test->Run CTests for SimpleCommandRunner`.  

## Coverage

`test.bat Debug` will make coverage report if [OpenCppCoverage](https://github.com/OpenCppCoverage/OpenCppCoverage/releases) is installed.  

## Uninstall wxWidgets

If you want to uninstall wxWidgets, remove `C:/Users/'username'/wxWidgets-*`.
