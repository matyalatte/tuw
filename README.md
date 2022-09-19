![build](https://github.com/matyalatte/Simple-Command-Runner/actions/workflows/build_all.yml/badge.svg)
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
- Save arguments
- Able to input paths by drag and drop

## Downloads
Download the executable from [here](https://github.com/matyalatte/Simple-Command-Runner/releases)
- `SimpleCommandRunner*-Windows.zip` is for Windows.<br>
- `SimpleCommandRunner*-macOS.tar.bz2` is for Mac.<br>
- `SimpleCommandRunner*-Linux.tar.bz2` is for Ubuntu (20.04 or later).<br>

(The Linux one won't work on old Ubuntu versions and other linux distributions due to glibc dependances.)<br>

## Examples
There are some [json files](./examples) to learn how to use.<br>

## FAQ
[Frequently Asked Questions](./docs/FAQ.md)

## Platforms
Simple Command Runner uses cross-platform framework.<br>
I made sure we can build it with the following platforms and compilers.
- Windows10 + MSVC 19.31
- Ubuntu 20.04 + GCC 9.4
- MacOS 10.15 + AppleClang 12.0

But I haven't tested enough yet.<br>
There may be some bugs.

## How to Build
### Github Actions
There are some building workflows for Github Actions.<br>
You can make Github build and upload the executable for your platform.<br>
- [`build_windows`](./.github/workflows/build_windows.yml) is for Windows.
- [`build_mac`](./.github/workflows/build_mac.yml) is for MacOS.
- [`build_ubuntu`](./.github/workflows/build_ubuntu.yml) is for Ubuntu (20.04 or later).

See here for the details.<br>
[How to Build With Github Actions](./docs/Github_Actions.md)

### Windows
There is a document for Windows users.<br>
[Building Workflow for Windows](./docs/Build_With_Windows.md)<br>

It only supports Visual Studio 2022, but you can see [the batch files](./batch_files/) to find a way to build it with your environment.<br>

### Mac and Ubuntu
There is a document for Mac and Ubuntu users.<br>
[Building Workflow for Ubuntu and MacOS](./docs/Build_With_Unix.md)<br>

And you can see [the shell scripts](./shell_scripts/) to understand the workflow.<br>

### Docker
Ubuntu users can also use a docker file to build the executable.<br>
See [the docker file](./Dockerfile) for the instruction.<br>

## License
### Simple Command Runner
Files in this repository are licensed under [wxWindows Library Licence](license.txt).<br>
It is a modified version of LGPL explicitly allowing not distributing
the sources of an application using the library even in the case of static linking.<br>

### wxWidget
Simple Command Runner uses [wxWidgets library](https://github.com/wxWidgets/wxWidgets) for the GUI framework.<br>
It is licensed under [wxWindows Library Licence](https://github.com/wxWidgets/wxWidgets/blob/master/docs/licence.txt).<br>

### nlohmann-json
Simple Command Runner uses [nlohmann's json library](https://github.com/nlohmann/json).<br>
It is licensed under [MIT license](https://github.com/nlohmann/json/blob/develop/LICENSE.MIT).
