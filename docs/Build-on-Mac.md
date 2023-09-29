# Building Workflow for macOS

## Requirements

-   Xcode
-   Ninja
-   [Meson](https://github.com/mesonbuild/meson) (**0.58** or later)
-   Shell scripts in [`./Tuw/shell_scripts`](../shell_scripts)

> :warning: GCC is not supported.  
> (It might work, but if you find issues, you need to solve them by yourself.)  

## Install Meson and Ninja

You can install meson and ninja via Homebrew. (`brew install meson ninja`)

> If you are a Python user, you can also get them via pip. (`pip3 install meson ninja`)

## Build

Run `bash shell_scripts/build.sh`.  
The executable will be generated in `build/Release/`.  

## Debug

If you want a debug build, run `bash shell_scripts/build.sh Debug` on the terminal.  

## Test

If you want to build tests, type `bash shell_scripts/test.sh` or `bash shell_scripts/test.sh Debug` on the terminal.  
