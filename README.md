![build](https://github.com/matyalatte/Simple-Command-Runner/actions/workflows/build_windows.yml/badge.svg)
![build](https://github.com/matyalatte/Simple-Command-Runner/actions/workflows/build_ubuntu.yml/badge.svg)
![build](https://github.com/matyalatte/Simple-Command-Runner/actions/workflows/build_mac.yml/badge.svg)
# Simple-Command-Runner ver 0.1.3
Simple GUI builder to execute commands.<br>

## About
Simple Command Runner will give very simple GUI to your scripts.<br>
All you need is write a `.json` file and run an `.exe` file.<br>
It can make GUI to run commands of your scripts.<br>
You don't need to use IDE and manage scripts for GUI.<br>

![sample](https://user-images.githubusercontent.com/69258547/171450112-8e1b40a0-36ae-4507-a4a7-2952d0bdbb52.png)![json](https://user-images.githubusercontent.com/69258547/171449741-859ca1bb-6f99-4e06-a7b9-a1873ca8b7eb.png)<br>

## Features
- Cross-platform
- Standalone
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
Download `SimpleCommandRunner*.zip` from [here](https://github.com/matyalatte/Simple-Command-Runner/releases)<br>

\* There are no releases for linux and mac.


## Samples
There are some [sample json files](./samples) to learn how to use.<br>

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
