# Building Executables

## Platforms

Simple Command Runner uses cross-platform libraries.  
I made sure I can build it with the following platforms and compilers.

-   Windows10 + MSVC 19.31
-   MacOS 11 + AppleClang 13.0
-   Ubuntu 20.04 + GCC 9.4
-   Alpine Linux 3.16 + GCC 11.2

And it should support your platform as well if [libui](https://github.com/libui-ng/libui-ng) supports.  

## CMake

There are platform-specific documents for building the executable with CMake.  

-   [Building Workflow for Windows](./Build-on-Windows.md)  
-   [Building Workflow for macOS](./Build-on-Mac.md)  
-   [Building Workflow for Linux](./Build-on-Linux.md)  

You can also see [batch files](../batch_files/), [shell scripts](../shell_scripts/), and [CMakePresets.json](../CMakePresets.json) to understand the workflow.  

## Docker

Linux users can use dockerfiles to build the executable.  
See the dockerfiles for the details.

-   [Dockerfile_Ubuntu](../Dockerfile_Ubuntu): Builds SimpleCommandRunner on Ubuntu20.04  
-   [Dockerfile_Alpine](../Dockerfile_Alpine): Builds SimpleCommandRunner on Alpine3.16  

## Github Actions

There are some building workflows for Github Actions.  
You can make Github build and upload the executable for your platform.  

-   [`build_windows`](../.github/workflows/build_windows.yml) is for Windows.
-   [`build_mac`](../.github/workflows/build_mac.yml) is for MacOS.
-   [`build_ubuntu`](../.github/workflows/build_ubuntu.yml) is for Ubuntu (20.04 or later).

See here for the details.  
[How to Build With Github Actions](./Github-Actions.md)
