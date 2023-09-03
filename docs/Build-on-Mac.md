# Building Workflow for macOS

## Requirements

-   Xcode
-   Ninja
-   [Meson](https://github.com/mesonbuild/meson) (**0.58** or later)
-   Shell scripts in [`./Simple-Command-Runner/shell_scripts`](../shell_scripts)

> :warning: GCC is not supported.  
> (It might work, but if you find issues, you need to solve them by yourself.)  

## Install Meson and Ninja

You can install meson and ninja via Homebrew. (`brew install meson ninja`)

> If you are a Python user, you can also get them via pip. (`pip3 install meson ninja`)

## Build

Run `bash shell_scripts/build.sh`.  
The executable will be generated in `build\Release\`.  

## Debug

If you want a debug build, run `bash shell_scripts/build.sh Debug` on the terminal.  

## Unversal Binaries

If you want to build universal binaries, run `bash shell_scripts/build.sh Release Universal` on the terminal.  

## Compression

You can use [UPX](https://github.com/upx/upx) if you want smaller exe.  

```bash
brew install upx
upx SimpleCommandRunner --best
```

## Test

If you want to build tests, type `bash test.sh` or `bash test.sh Debug` on the terminal.  
