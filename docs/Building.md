# Building Executables

## Supported Platforms

Tuw uses cross-platform libraries.  
You can build it on the following platforms.  

-   Windows 7 or later  
-   macOS 10.9 or later  
-   Linux with GTK+ 3.10 or later  

You can also see the list of tested platforms [here](./Test-Env.md).

## Meson

There are platform-specific documents for building the executable with [Meson](https://github.com/mesonbuild/meson).  

-   [Building Workflow for Windows](./Build-on-Windows.md)  
-   [Building Workflow for macOS](./Build-on-Mac.md)  
-   [Building Workflow for Linux](./Build-on-Linux.md)  

You can also see [batch files](../batch_files/) and [shell scripts](../shell_scripts/) to understand the workflow.  

## Docker

Linux users can use dockerfiles to build the executable.  
See the dockerfiles for the details.

-   [docker/ubuntu.dockerfile](../docker/ubuntu.dockerfile): Builds Tuw on Ubuntu20.04  
-   [docker/alpine.dockerfile](../docker/alpine.dockerfile): Builds Tuw on Alpine3.16  

## Github Actions

There is a building workflow for Github Actions.  
You can make Github build and upload the executables.  

See here for the details.  
[How to Build With Github Actions](./Github-Actions.md)
