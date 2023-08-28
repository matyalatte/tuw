# Building Workflow for Linux

## Requirements

-   Build tools (e.g. `build-essential` for Ubuntu)
-   GTK3.0 (e.g. `libgtk-3-dev` for Ubuntu)
-   Ninja
-   [Meson](https://github.com/mesonbuild/meson) (**0.58** or later)
-   bash
-   Shell scripts in [`./Simple-Command-Runner/shell_scripts`](../shell_scripts)

## Install Meson and Ninja

You can install meson and ninja via apt. (`sudo apt install meson ninja`)

> If you are a Python user, you can also get them via pip. (`sudo pip3 install meson ninja`)

## Build

Run `shell_scripts/build.sh`.  
The executable will be generated in `build\Release\`.  

## Debug

If you want a debug build, run `batch_files/build.bat Debug` on the command prompt.  

## Compression

The built binary will be 2 or 3 MB.  
You should use [UPX](https://github.com/upx/upx/releases/latest) if you want smaller exe.  

```bash
upx SimpleCommandRunner --best
```

## Test

Not yet

## Coverage

Not yet

## Dockerfiles

Not yet
