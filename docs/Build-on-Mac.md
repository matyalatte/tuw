# Building Workflow for macOS

## Requirements

-   xcode
-   [Meson](https://github.com/mesonbuild/meson) (**0.58** or later)
-   Shell scripts in [`./Simple-Command-Runner/shell_scripts`](../shell_scripts)

## Install Meson

You can install meson via Homebrew. (`brew install meson`)

> If you are a Python user, you can also get meson via pip. (`pip3 install meson ninja`)

## Build

Run `shell_scripts/build.sh`.  
The executable will be generated in `build\Release\`.  

## Debug

If you want a debug build, run `batch_files/build.bat Debug` on the command prompt.  

## Compression

You can use [UPX](https://github.com/upx/upx) if you want smaller exe.  

```bash
brew install upx
upx SimpleCommandRunner --best
```

## Test

Not yet
