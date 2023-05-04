# Building Workflow for Ubuntu and MacOS

Building workflow for Ubuntu and MacOS.  

## 0. Requirements

-   xcode (for macOS)
-   build-essential (for linux)
-   libgtk-3-dev (for linux)
-   cmake (**3.25** or later)
-   wget

> CMake should be 3.25 or later.  
> Or use [v0.2.3](https://github.com/matyalatte/Simple-Command-Runner/tree/v0.2.3) that supports old cmake versions.  

## 1. Build wxWidgets

wxWidgets is a GUI framework.  
You can install it with the following steps.

1.  Open the Terminal.
2.  Move to `./Simple-Command-Runner/shell_scripts`
3.  Type `bash download_wxWidgets.sh` (Use `bash`. `sh` won't work.)
4.  Type `bash build_wxWidgets.sh`.

## 2. Build an executable with Shell Scripts

You can build Simple Command Runner with shell scripts.  
The steps are as follows.

1.  Open the Terminal.
2.  Move to `./Simple-Command-Runner/shell_scripts`
3.  Type `bash build_exe.sh`.
4.  An executable file `SimpleCommandRunner` will be generated in `./Simple-Command-Runner/Release`.

## Debug build

If you want a debug build, you need to use `Debug` as an argument for shell scripts.  
So, you should type `bash build_wxWidgets.sh Debug` and `bash build_exe.sh Debug` on the terminal.  

## Test

If you want to build tests, type `bash test.sh` or `bash test.sh Debug` on the terminal.

## Coverage

If you will use GCC, you can get coverage reports.  
Install lcov with `sudo apt install lcov`.  
Then, type `bash coverage.sh` or `bash coverage.sh Debug` on the Terminal.  
It'll generate html files in `./Simple-Command-Runner/coverage-report/`.

## Uninstall wxWidgets

If you want to uninstall wxWidgets, remove `~/wxWidgets-*`.
