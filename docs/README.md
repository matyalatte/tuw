# Simple-Command-Runner ver 0.3.0

![build](https://github.com/matyalatte/Simple-Command-Runner/actions/workflows/build_all.yml/badge.svg)
![test](https://github.com/matyalatte/Simple-Command-Runner/actions/workflows/test.yml/badge.svg)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/4aee3ee5172e4c38915d07f9c62725d3)](https://www.codacy.com/gh/matyalatte/Simple-Command-Runner/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=matyalatte/Simple-Command-Runner&amp;utm_campaign=Badge_Grade)

Simple GUI wrapper to execute commands.  

## About

Simple Command Runner will give very simple GUI to your scripts.  
All you need is write a `.json` file and run an `.exe` file.  
It can make GUI to run commands of your scripts.  
No need to use IDE and manage scripts for GUI.  

![sample](https://user-images.githubusercontent.com/69258547/192090786-11a3f5ef-988e-442f-8ba9-fd1636b9f350.png)
<img src=https://user-images.githubusercontent.com/69258547/192090797-f5e5b52d-59aa-4942-a361-2c8b5c7bd746.png width=387></img>  

## Features

-   Cross-platform
-   Portable
-   Define GUI in .json
-   Save arguments
-   Able to input paths by drag and drop

## Downloads

You can download executables from [the release page](https://github.com/matyalatte/Simple-Command-Runner/releases)

-   `SimpleCommandRunner*-Windows.zip` is for Windows.  
-   `SimpleCommandRunner*-macOS.tar.bz2` is for Mac.  
-   `SimpleCommandRunner*-Linux.tar.bz2` is for Ubuntu (20.04 or later).  

> The linux build only supports Ubuntu due to the glibc dependences.  
> If you want to use it on other linux distributions, you should get the lib or build the executable by yourself.  

## Examples

There are some [json files](../examples) to learn how to use.  

## FAQ

[Frequently Asked Questions](./FAQ.md)

## Platforms

Simple Command Runner uses cross-platform framework.  
I made sure I can build it with the following platforms and compilers.

-   Windows10 + MSVC 19.31
-   Ubuntu 20.04 + GCC 9.4
-   MacOS 11 + AppleClang 13.0

And I think it supports your environment as well if [wxWidgets library](https://github.com/wxWidgets/wxWidgets) supports.  

## How to Build

### Github Actions

There are some building workflows for Github Actions.  
You can make Github build and upload the executable for your platform.  

-   [`build_windows`](../.github/workflows/build_windows.yml) is for Windows.
-   [`build_mac`](../.github/workflows/build_mac.yml) is for MacOS.
-   [`build_ubuntu`](../.github/workflows/build_ubuntu.yml) is for Ubuntu (20.04 or later).

See here for the details.  
[How to Build With Github Actions](./Github-Actions.md)

### Windows

There is a document for Windows users.  
[Building Workflow for Windows](./Build-on-Windows.md)  

It only supports Visual Studio 2022, but you can see [the batch files](../batch_files/) to find a way to build it with your environment.  

### Mac and Ubuntu

There is a document for Mac and Ubuntu users.  
[Building Workflow for Ubuntu and MacOS](./Build-on-Unix.md)  

And you can see [the shell scripts](../shell_scripts/) to understand the workflow.  

### Docker

Ubuntu users can also use a docker file to build the executable.  
See [the docker file](../Dockerfile) for the instruction.  

## License

### Simple Command Runner

Files in this repository are licensed under [wxWindows Library Licence](../license.txt).  
It is a modified version of LGPL explicitly allowing not distributing
the sources of an application using the library even in the case of static linking.  

### wxWidgets

Simple Command Runner uses [wxWidgets library](https://github.com/wxWidgets/wxWidgets) for the GUI framework.  
It is licensed under [wxWindows Library Licence](https://github.com/wxWidgets/wxWidgets/blob/master/docs/licence.txt).  

### nlohmann-json

Simple Command Runner uses [nlohmann's json library](https://github.com/nlohmann/json).  
It is licensed under [MIT license](https://github.com/nlohmann/json/blob/develop/LICENSE.MIT).
