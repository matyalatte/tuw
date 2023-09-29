# Building Executables

## Platforms

Tuw uses cross-platform libraries.  
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

You can also see [batch files](../batch_files/) and [shell scripts](../shell_scripts/) to understand the workflow.  

## Docker

Linux users can use dockerfiles to build the executable.  
See the dockerfiles for the details.

-   [Dockerfile_Ubuntu](../Dockerfile_Ubuntu): Builds Tuw on Ubuntu20.04  
-   [Dockerfile_Alpine](../Dockerfile_Alpine): Builds Tuw on Alpine3.16  

## Github Actions

There is a building workflow for Github Actions.  
You can make Github build and upload the executables.  

See here for the details.  
[How to Build With Github Actions](./Github-Actions.md)
