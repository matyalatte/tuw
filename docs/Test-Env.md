# Test Environments

## CI Environments

CI workflows are executed on the following platforms.

| OS | Arch | Compiler | libc |
| :--: | :--: | :--: | :--: |
| Windows10 | x64 | msvc |  |
| Windows10 | arm64 | msvc |  |
| macOS14 | arm64 | clang |  |
| Ubuntu20.04 | x64 | gcc | glibc |
| Ubuntu20.04 | arm64 | gcc | glibc |
| Alpine3.16 | x64 | gcc | musl |
| Alpine3.16 | arm64 | gcc | musl |
| FreeBSD15.0 | x64 | clang |  |
| OpenBSD7.7 | x64 | clang |  |

## Tested Environments

The following platforms have been tested in the past. However, compatibility with the current version of tuw is unknown.

-   :heavy_check_mark: : Tested
-   :question: : Not tested
-   :x: : Failed

| OS | Arch | Compiler | libc | Build | Manual Test | CI Test |
| :--: | :--: | :--: | :--: | :--: | :--: | :--: |
| Windows11 | x64 | msvc |  | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| Windows10 | x64 | gcc | | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| Windows8 | x64 |  |  | :question: | :heavy_check_mark: | :question: |
| macOS11 | x64 | clang |  | :heavy_check_mark: | :question: | :heavy_check_mark: |
| macOS10.15 | x64 | clang |  | :heavy_check_mark: | :heavy_check_mark: | :question: |
| macOS10.12 | x64 |  |  | :question: | :heavy_check_mark: | :question: |
| Ubuntu24.04 | x64 | gcc | glibc | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| Fedora32 | x64 |  | glibc | :question: | :heavy_check_mark: | :question: |
| ZorinOS17 | x64 |  | glibc | :question: | :heavy_check_mark: | :question: |
| Alpine3.18 | x64 |  | musl | :question: | :heavy_check_mark: | :question: |
| FreeBSD14.2 | x64 | gcc | | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| OpenBSD7.4 | x64 | clang | | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| NetBSD9.3 | x64 | gcc | | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| Haiku R1/beta5 | x64 | gcc | | :heavy_check_mark: | :heavy_check_mark: | :x: |
| OpenIndiana 2024.04 | x64 | gcc | | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
