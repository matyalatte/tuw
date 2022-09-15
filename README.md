![build](https://github.com/matyalatte/Simple-Command-Runner/actions/workflows/build_windows.yml/badge.svg)
![build](https://github.com/matyalatte/Simple-Command-Runner/actions/workflows/build_ubuntu.yml/badge.svg)
![build](https://github.com/matyalatte/Simple-Command-Runner/actions/workflows/build_mac.yml/badge.svg)
# Simple-Command-Runner ver 0.1.3
Simple GUI wrapper to execute commands.<br>

## About
Simple Command Runner will give very simple GUI to your scripts.<br>
All you need is write a `.json` file and run an `.exe` file.<br>
It can make GUI to run commands of your scripts.<br>
No need to use IDE and manage scripts for GUI.<br>

![sample](https://user-images.githubusercontent.com/69258547/171450112-8e1b40a0-36ae-4507-a4a7-2952d0bdbb52.png)
![json](https://user-images.githubusercontent.com/69258547/171449741-859ca1bb-6f99-4e06-a7b9-a1873ca8b7eb.png)<br>

## Features
- Cross-platform
- Portable
- No need IDE for GUI
- Define GUI in .json

## Platforms
Simple Command Runner uses cross-platform framework.<br>
I made sure we can build it with the following platforms and compilers.
- Windows10 + MSVC 19.31
- Ubuntu 20.04 + GCC 9.4
- MacOS 10.15 + AppleClang 12.0

But I haven't tested enough yet.<br>
There may be some bugs.

## Downloads
Download the executable from [here](https://github.com/matyalatte/Simple-Command-Runner/releases)
- `SimpleCommandRunner*-Windows.zip` is for Windows.<br>
- `SimpleCommandRunner*-macOS.tar.bz2` is for Mac.<br>
- `SimpleCommandRunner*-Linux.tar.bz2` is for Ubuntu (20.04 or later).<br>

(The Linux one won't work on old Ubuntu versions and other linux distributions due to glibc dependances.)<br>

## Examples
There are some [json files](./examples) to learn how to use.<br>

## How to Build
### Windows
There is a document for Windows users.<br>
[Simple-Command-Runner/batch_files](./batch_files)<br>

It only supports Visual Studio 2022, but you can see the batch files to find a way to build it with your environment.<br>

### Mac and Ubuntu
There is a document for Mac and Ubuntu users.<br>
[Simple-Command-Runner/shell_scripts](./shell_scripts)<br>

And you can see the shell scripts to understand the workflow.<br>

### Docker
Ubuntu users can also use a docker file to build the executable.<br>
See [the docker file](./Dockerfile) for the instruction.<br>

## FAQ
### Can I distribute the exe with my scripts?
Yes, and you can rename it to `GUI.exe`, `'project name'-GUI.exe`, or `'project name'_GUI.exe`.<br>

### Linux says `Could Not Display` when lauching the executable.
Check `Allow executing file as program.` (Properties->Permissions->Execute)<br>
You will be able to launch the executable by double-click.<br>
![Screenshot (729)](https://user-images.githubusercontent.com/69258547/189526464-cd62887b-62b1-4071-ae38-a7ab73600bbf.png)

## License
### Simple Command Runner
Simple Command Runner is licensed under [wxWindows Library Licence](license.txt).<br>
It is a modified version of LGPL explicitly allowing not distributing
the sources of an application using the library even in the case of static linking.<br>

### wxWidget
Simple Command Runner uses [wxWidgets library](https://github.com/wxWidgets/wxWidgets) for the GUI framework.<br>
It is licensed under [wxWindows Library Licence](https://github.com/wxWidgets/wxWidgets/blob/master/docs/licence.txt).<br>

### nlohmann-json
Simple Command Runner uses [nlohmann's json library](https://github.com/nlohmann/json).<br>
It is licensed under [MIT license](https://github.com/nlohmann/json/blob/develop/LICENSE.MIT).
