# Building Workflow for Ubuntu and MacOS
Building workflow for Ubuntu and MacOS.<br>
I don't know if it works for other linux distributions.

## 0. Setup for Ubuntu
If your OS is Ubuntu, type `sudo apt -y install build-essential libgtk-3-dev` on the Terminal.

## 1. Build wxWidgets
wxWidgets is a GUI framework.<br>
You can install it with the following steps.

1. Run `download_wxWidgets.sh`.
2. Run `build_wxWidgets.sh`.
3. Run `install_wxWidgets.sh`.

## 2. Build an executable with Shell Scripts
You can build Simple Command Runner with shell scripts.<br>
The steps are as follows.

1. Run `build_exe.sh`.
2. An executable file `SimpleCommandRunner` will be generated in `./Simple-Command-Runner/build`.

## Uninstall wxWidgets
`uninstall_wxWidgets.sh` will uninstall the libs and remove all resource files.