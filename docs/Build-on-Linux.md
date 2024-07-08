# Building Workflow for Linux

## Requirements

-   Build tools (e.g. `build-essential` for Ubuntu)
-   GTK+ 3.10 or later (e.g. `libgtk-3-dev` for Ubuntu)
-   Ninja
-   [Meson](https://github.com/mesonbuild/meson) (**0.58** or later)
-   bash
-   Shell scripts in [`./Tuw/shell_scripts`](../shell_scripts)

## Install Meson and Ninja

You can install meson and ninja via apt. (`sudo apt install meson ninja`)  

> [!Note]
> If you are a Python user, you can also get them via pip. (`sudo pip3 install meson ninja`)

## Build

Run `bash shell_scripts/build.sh`.  
The executable will be generated in `build/Release/`.  

## Debug

If you want a debug build, run `bash shell_scripts/build.sh Debug` on the terminal.  

## Test

To build tests, type `bash shell_scripts/test.sh` or `bash shell_scripts/test.sh Debug` on the terminal.

## Coverage

If you use GCC, you can get coverage reports.  
Install lcov with `sudo apt install lcov` and run `bash shell_scripts/coverage.sh` or `bash shell_scripts/coverage.sh Debug` on the terminal.  
It'll generate html files in `./Tuw/coverage-report/`.

## GLIBC Dependencies

If you have built the binary with GLIBC, you can use `check_glibc_compatibility.sh` to see the required versions of GLIBC and GLIBCXX.  

```console
$ bash shell_scripts/check_libc_compatibility.sh build/Release/Tuw
Required GLIBC versions
2.2.5
2.3
2.3.4
2.4
2.7
2.15
Required GLIBCXX versions
3.4
3.4.21
```

## Dockerfiles

You can see some dockerfiles to understand the workflow.  

-   [docker/ubuntu.dockerfile](../docker/ubuntu.dockerfile): Builds Tuw on Ubuntu20.04  
-   [docker/alpine.dockerfile](../docker/alpine.dockerfile): Builds Tuw on Alpine3.16  
