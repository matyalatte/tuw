# Building Workflow for Ubuntu and MacOS

## 0. Requirements

-   build-essential (for Ubuntu)
-   libgtk-3-dev (for Ubuntu)
-   cmake (**3.25** or later)
-   wget
-   bash
-   Shell scripts in [`./Simple-Command-Runner/shell_scripts`](../shell_scripts)

> CMake should be 3.25 or later.  
> Or use [v0.2.3](https://github.com/matyalatte/Simple-Command-Runner/tree/v0.2.3) that supports old cmake versions.  

> Note that I tested only with Ubuntu.  
> The workflow and the built binary might not work on other linux distros.  

## 1. Build wxWidgets

wxWidgets is a GUI framework.  
You can build it with the following steps.

1.  Open the Terminal.
2.  Move to `./Simple-Command-Runner/shell_scripts`
3.  Type `bash download_wxWidgets.sh` (Use `bash`. `sh` won't work.)
4.  Type `bash build_wxWidgets.sh`.

> If you won't use GCC, you could need to edit `CXXFLAGS` in `build_wxWidgets.sh`.  

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

## Dockerfiles

You can see some dockerfiles to understand the workflow.  

-   [ubuntu20.04-cmake3.25](https://github.com/matyalatte/Matya-Dockerfiles/blob/main/ubuntu20.04-cmake3.25/Dockerfile): Dockerfile to install cmake3.25 on Ubuntu20.04
-   [alpine3.16-cmake3.25](https://github.com/matyalatte/Matya-Dockerfiles/blob/main/alpine3.16-cmake3.25/Dockerfile): Dockerfile to install cmake3.25 on Alpine3.16
-   [Dockerfile_Ubuntu](../Dockerfile_Ubuntu): Dockerfile to build SimpleCommandRunner on Ubuntu20.04
-   [Dockerfile_Alpine](../Dockerfile_Alpine): Dockerfile to build SimpleCommandRunner on Alpine3.16
