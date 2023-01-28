# Building Workflow for Ubuntu and MacOS

Building workflow for Ubuntu and MacOS.  
If your OS is Ubuntu, note that the workflow will only work on 20.04 or later.  
And the built executable won't work on old versions like 18.04.

## 0. Setup for Ubuntu

If your OS is Ubuntu, type `sudo apt install -y wget ca-certificates build-essential libgtk-3-dev git cmake` on the Terminal.

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
