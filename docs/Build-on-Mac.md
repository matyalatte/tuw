# Building Workflow for macOS

## 0. Requirements

-   xcode
-   cmake (**3.25** or later)
-   wget
-   Shell scripts in [`./Simple-Command-Runner/shell_scripts`](../shell_scripts)

## 1. Build wxWidgets

wxWidgets is a GUI framework.  
You can build it with the following steps.

1.  Open the Terminal.
2.  Move to `./Simple-Command-Runner/shell_scripts`.
3.  Type `bash download_wxWidgets.sh`. (Use `bash`. `sh` won't work.)
4.  Type `bash build_wxWidgets.sh`.
5.  Type `bash build_wxWidgets.sh Debug` if you want a debug build.

## 2. Build an executable with Shell Scripts

You can build Simple Command Runner with shell scripts.  
The steps are as follows.  

1.  Open the Terminal.
2.  Move to `./Simple-Command-Runner/shell_scripts`.
3.  Type `bash build_exe.sh`.
4.  An executable file `SimpleCommandRunner` will be generated in `./Simple-Command-Runner/build/Release`.
5.  Type `bash build_exe.sh Debug` if you want a debug build.

## Compression

The built binary will be 2 or 3 MB.  
You should use [UPX](https://github.com/upx/upx) if you want smaller exe.  

```bash
brew install upx
upx SimpleCommandRunner --best
```

## Test

If you want to build tests, type `bash test.sh` or `bash test.sh Debug` on the terminal.

## Uninstall wxWidgets

If you want to uninstall wxWidgets, remove `~/wxWidgets-*`.
