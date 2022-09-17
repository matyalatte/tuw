# Building Workflow for Windows

## 0. Requirements
- Visual Studio 2022 (need cmake tools)
- Batch files in [`./Simple-Command-Runner/batch_files`](../batch_files)

## 1. Build wxWidgets
wxWidgets is a GUI framework.<br>
You can install it with the following steps.

1. Run `download_wxWidgets.bat` as admin.
2. Run `open_x64_native_command_prompt_for_VS2022.bat` to open the command prompt.
3. Type `build_wxWidgets.bat` on the command prompt.

## 2-a. Build .exe with Batch Files
You can build Simple Command Runner with batch files.<br>
The steps are as follows.

1. Run `open_x64_native_command_prompt_for_VS2022.bat` to open the command prompt.
2. Run `build_exe.bat` on the command prompt.
3. `SimpleCommandRunner.exe` will be generated in `./Simple-Command-Runner/build/Release`.

## 2-b. Build .exe with Visual Studio
Of course, you can build the exe with Visual Studio.<br>
The steps are as follows.

1. Open `./Simple-Command-Runner` with Visual Studio.
2. Select `Manage Configurations` from configuration.
3. Add `-D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded` to `CMake command arguments` in `x64-Release`.
4. Build SimpleCommandRunner.exe.

## Debug build
If you want a debug build, you need to use `Debug` as an argument for batch files.<br>
So, you should type `download_wxWidgets.bat Debug`, `build_wxWidgets.bat Debug`, and `build_exe.bat Debug` on the command prompt.<br>
And no need `-D CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded` for cmake arguments.


## Uninstall wxWidgets
If you want to uninstall wxWidgets, remove `C:/wxWidgets-*`.