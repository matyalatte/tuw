# Simple-Command-Runner ver 0.4.0

![build](https://github.com/matyalatte/Simple-Command-Runner/actions/workflows/build_all.yml/badge.svg)
![test](https://github.com/matyalatte/Simple-Command-Runner/actions/workflows/test.yml/badge.svg)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/4aee3ee5172e4c38915d07f9c62725d3)](https://www.codacy.com/gh/matyalatte/Simple-Command-Runner/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=matyalatte/Simple-Command-Runner&amp;utm_campaign=Badge_Grade)
<a href="https://www.buymeacoffee.com/matyalatteQ" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/default-orange.png" alt="Buy Me A Coffee" height="41" width="174"></a>  

Small and simple GUI wrapper for command-line tools.  

## Only 1MB for a portable GUI!

Simple Command Runner will give a very simple GUI to your scripts.  
All you need is write a `.json` file and run a small executable.  
It can make a GUI to run commands of your scripts.  
**No need coding, no need browsers, and no need stupidly large executables**!  

![sample](https://user-images.githubusercontent.com/69258547/192090786-11a3f5ef-988e-442f-8ba9-fd1636b9f350.png)
<img src=https://user-images.githubusercontent.com/69258547/192090797-f5e5b52d-59aa-4942-a361-2c8b5c7bd746.png width=387></img>  

## Features

-   Define GUI in .json
-   Save arguments
-   Input paths by drag and drop
-   Cross-platform
-   Native look and feel
-   Portable
-   Small size (under 1MB)

## Downloads

You can download executables from [the release page](https://github.com/matyalatte/Simple-Command-Runner/releases)

-   `SimpleCommandRunner*-Windows*.zip` is for Windows.  
-   `SimpleCommandRunner*-macOS*.tar.bz2` is for macOS.  
-   `SimpleCommandRunner*-Linux*.tar.bz2` is for Ubuntu (20.04 or later).  

> `*-packed.*` are the 1MB executables that were compressed by [UPX](https://github.com/upx/upx).  
> But they might cause some problems (e.g. false positives by anti-viruses) on your machine.  
> Use the uncompressed ones if they won't work.  

> The linux builds only support Ubuntu due to the glibc dependences.  
> Build the executable by yourself if you want to use it on other linux distros.  

## Examples

There are some [json files](../examples/README.md) to learn how to define GUIs.  

## FAQ

[Frequently Asked Questions](./FAQ.md)

## Platforms

Simple Command Runner uses cross-platform framework.  
I made sure I can build it with the following platforms and compilers.

-   Windows10 + MSVC 19.31
-   MacOS 11 + AppleClang 13.0
-   Ubuntu 20.04 + GCC 9.4
-   Alpine Linux 3.16 + GCC 11.2

And it should support your platform as well if [wxWidgets library](https://github.com/wxWidgets/wxWidgets) supports.  

## Building

### CMake

There are platform-specific documents for building the executable with CMake.  

-   [Building Workflow for Windows](./Build-on-Windows.md)  
-   [Building Workflow for macOS](./Build-on-Mac.md)  
-   [Building Workflow for Linux](./Build-on-Linux.md)  

You can also see [batch files](../batch_files/), [shell scripts](../shell_scripts/), and [CMakePresets.json](../CMakePresets.json) to understand the workflow.  

### Docker

Linux users can use dockerfiles to build the executable.  
See the dockerfiles for the details.

-   [Dockerfile_Ubuntu](../Dockerfile_Ubuntu): Builds SimpleCommandRunner on Ubuntu20.04  
-   [Dockerfile_Alpine](../Dockerfile_Alpine): Builds SimpleCommandRunner on Alpine3.16  

### Github Actions

There are some building workflows for Github Actions.  
You can make Github build and upload the executable for your platform.  

-   [`build_windows`](../.github/workflows/build_windows.yml) is for Windows.
-   [`build_mac`](../.github/workflows/build_mac.yml) is for MacOS.
-   [`build_ubuntu`](../.github/workflows/build_ubuntu.yml) is for Ubuntu (20.04 or later).

See here for the details.  
[How to Build With Github Actions](./Github-Actions.md)

## License

Files in this repository are available under the [GPL2+](../license.txt).  

## External Projects

| Project | Used for | License |
| -- | -- | -- |
| [wxWidgets library](https://github.com/wxWidgets/wxWidgets) | GUI framework | [Modified LGPL](https://github.com/wxWidgets/wxWidgets/blob/master/docs/licence.txt) | 
| [nlohmann's json library](https://github.com/nlohmann/json) | JSON handling | [MIT](https://github.com/nlohmann/json/blob/develop/LICENSE.MIT) |
| [UPX](https://github.com/upx/upx) | EXE compression | [Modified GPL2+](https://github.com/upx/upx/blob/devel/LICENSE) |
