# Compress with UPX

You can compress the built executables with [UPX](https://github.com/upx/upx).  
The binary size will be under 500KB.  

## Install UPX

There are [releases](https://github.com/upx/upx/releases) for Linux and Windows users.  
And macOS users can get it via Homebrew.  

```bash
brew install upx
```

## Compress Executables

You can compress executables with UPX via command-line.  

```bash
upx SimpleCommandRunner --best
```
