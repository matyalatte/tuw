# Building Workflow for Windows

## 0. Requirements
- Visual Studio 2022
- CMake
- Batch files in [`./Simple-Command-Runner/batch_files`](../batch_files)

## 1. Build wxWidgets
wxWidgets is a GUI framework.<br>
You can install it with the following steps.

1. Run `download_wxWidgets.bat` as admin.
2. Open the command prompt.
3. Run `build_wxWidgets.bat` on the command prompt.

## 2-a. Build .exe with Batch Files
You can build Simple Command Runner with batch files.<br>
The steps are as follows.

1. Run `build_exe.bat` on the command prompt.
2. `SimpleCommandRunner.exe` will be generated in `./Simple-Command-Runner/Release/Release`.

## 2-b. Build .exe with Visual Studio
Of course, you can build the exe with Visual Studio.<br>
The steps are as follows.

1. Open `./Simple-Command-Runner` with Visual Studio.
2. Select `Manage Configurations` from configuration.
4. Add `-D wxWidgets_ROOT_DIR=C:/wxWidgets-3.1.5/Release/Installed -D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded -D wxWidgets_CONFIGURATION=mswu` to `CMake command arguments` for `x64-Release`.
5. Build SimpleCommandRunner.exe.

## Debug build
If you want a debug build, you need to use `Debug` as an argument for batch files.<br>
So, you should type `build_wxWidgets.bat Debug` and `build_exe.bat Debug` on the command prompt.<br>
If you will build the exe with Visual Studio, you should add `-D wxWidgets_ROOT_DIR=C:/wxWidgets-3.1.5/Debug/Installed -D wxWidgets_CONFIGURATION=mswud` to `CMake command arguments` for `x64-Debug` in the IDE.

## Test
If you want to build tests, type `test.bat` or `test.bat Debug` on the command prompt.

## Uninstall wxWidgets
If you want to uninstall wxWidgets, remove `C:/wxWidgets-*`.