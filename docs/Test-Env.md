# Test Environments

Here is the list of test environments.

-   :heavy_check_mark: : Tested
-   :warning: : Tested once, but not with the latest build
-   :x: : Not tested

| OS | Arch | Compiler | libc | Build | Manual Test | CI Test |
| :--: | :--: | :--: | :--: | :--: | :--: | :--: |
| Windows10 | x64 | msvc |  | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| Windows10 | arm64 | msvc |  | :heavy_check_mark: | :x: | :heavy_check_mark: |
| Windows8 | x64 |  |  | :x: | :warning: | :x: |
| macOS11 | x64 | clang |  | :heavy_check_mark: | :x: | :heavy_check_mark: |
| macOS10.15 | x64 | clang |  | :heavy_check_mark: | :heavy_check_mark: | :x: |
| macOS10.12 | x64 |  |  | :x: | :warning: | :x: |
| Ubuntu20.04 | x64 | gcc | glibc | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| Ubuntu20.04 | arm64 | gcc | glibc | :heavy_check_mark: | :x: | :heavy_check_mark: |
| Ubuntu18.04 | x64 |  | glibc | :x: | :warning: | :x: |
| Fedora32 | x64 |  | glibc | :x: | :warning: | :x: |
| ZorinOS17 | x64 |  | glibc | :x: | :warning: | :x: |
| Alpine3.16 | x64 | gcc | musl | :heavy_check_mark: | :x: | :heavy_check_mark: |
| Alpine3.16 | arm64 | gcc | musl | :heavy_check_mark: | :x: | :heavy_check_mark: |
| Alpine3.18 | x64 |  | musl | :x: | :warning: | :x: |
| FreeBSD14.0 | x64 | gcc | | :warning: | :warning: | :x: |
| OpenBSD7.4 | x64 | gcc | | :warning: | :warning: | :warning: |
| Haiku R1/beta4 | x64 | gcc | | :warning: | :warning: | :x: |
