# Building Workflow for Ubuntu and MacOS
Building workflow for Ubuntu and MacOS.<br>
I don't know if it works for other linux distributions.

## 0. Setup for Ubuntu
If your OS is Ubuntu, type `sudo apt -y install build-essential libgtk-3-dev` on the Terminal.

## 1. Build wxWidgets
wxWidgets is a GUI framework.<br>
You can install it with the following steps.

1. Open the Terminal.
2. Type `bash download_wxWidgets.sh` (Use `bash`. `sh` won't work.)
3. Type `bash build_wxWidgets.sh`.
4. Type `bash install_wxWidgets.sh`.

## 2. Build an executable with Shell Scripts
You can build Simple Command Runner with shell scripts.<br>
The steps are as follows.

1. Open the Terminal.
2. Type `bush build_exe.sh`.
3. An executable file `SimpleCommandRunner` will be generated in `./Simple-Command-Runner/build`.

## Uninstall wxWidgets
`uninstall_wxWidgets.sh` will uninstall the libs and remove all resource files.
