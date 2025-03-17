# Tuw: a tiny GUI wrapper for command-line tools

```text
  _____
 |_   _|   ___      __
   | || | | \ \ /\ / /
   | || |_| |\ V  V /
   |_| \__,_| \_/\_/
  Tiny UI wrapper for
       CLI tools
```

![build](https://github.com/matyalatte/tuw/actions/workflows/build_all.yml/badge.svg)
![test](https://github.com/matyalatte/tuw/actions/workflows/test.yml/badge.svg)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![donate](https://img.shields.io/static/v1?label=donate&message=%E2%9D%A4&logo=GitHub&color=%23fe8e86)](https://github.com/sponsors/matyalatte)  
[Examples](../examples/README.md) | [Build Instructions](./Building.md) | [FAQ](./FAQ.md)

## Only 300KB for a portable GUI

Tuw provides a very simple GUI for your scripts.  
All you need is a JSON file and a tiny executable.  
**No need for compilers, browsers, or huge executables!**  

![sample](https://github.com/user-attachments/assets/6426fc08-821f-49f5-af74-965273d45d4a)
<img src=https://github.com/user-attachments/assets/6a43c5d9-beed-4f27-8a82-5dfd5fd9c3b5 width=398></img>  

## Features

-   [Define GUI in JSON](../examples/)
-   [Embed JSON into exe](../examples/get_start/json_embed/)
-   Save arguments
-   Input paths by drag and drop
-   Cross-platform (Windows, macOS, Linux, BSD, etc.)
-   Native look and feel
-   Portable
-   Small size
-   UTF-8 support

## Downloads

You can download executables from [the release page](https://github.com/matyalatte/tuw/releases)

-   `Tuw-*-Windows-*.zip` is for Windows (7 or later.)  
-   `Tuw-*-Windows10-*.zip` requires Windows 10 or later, but it's much smaller than the standard version.  
-   `Tuw-*-macOS.tar.xz` is for macOS (10.9 or later.)  
-   `Tuw-*-Linux-*.tar.xz` is for Linux (with GTK3.14, GLIBC2.28, and GLIBCXX3.4, or later versions of the libraries.)  

> [!Note]
> Tuw also supports [Linux distributions using musl](https://github.com/matyalatte/tuw/blob/main/docs/Build-on-Linux.md) and [other Unix-like systems (BSD, Haiku, illumos, etc.)](https://github.com/matyalatte/Tuw/blob/main/docs/Build-on-Other.md). While there is no release package available for these systems, you can build Tuw from the source code.

## Examples

There are many [JSON files](../examples/README.md) to learn how to define GUIs.  

## JSON Schema

There is a [schema](../schema/schema.json) for `gui_definition.json`.  
You can use it to validate definitions while editing files.  

### VSCode

For VSCode, you can add the schema path to `settings.json` (`File > Preferences > Settings > JSON:Schemas > Edit in settings.json`.)  

```json
"files.associations": {
    "*.tuw": "jsonc"
},
"json.schemas": [
    {
        "fileMatch": [ "gui_definition.json", "gui_definition.jsonc", "*.tuw" ],
        "url": "https://raw.githubusercontent.com/matyalatte/tuw/main/schema/schema.json"
    }
]
```

## FAQ

[Frequently Asked Questions](./FAQ.md)

## Building

[Building Executables](./Building.md)

## Contributing

[Contributing to Tuw](./CONTRIBUTING.md)

## License

Files in this repository are available under the [MIT license](../license.txt).  

## External Projects

| Project | About | License |
| -- | -- | -- |
| [libui-ng](https://github.com/libui-ng/libui-ng) | GUI framework | [MIT](http://opensource.org/licenses/MIT) |
| [RapidJSON](https://github.com/Tencent/rapidjson) | JSON parser | [MIT](http://opensource.org/licenses/MIT) |
| [subprocess.h](https://github.com/sheredom/subprocess.h) | Command processor | [Unlicense](https://github.com/sheredom/subprocess.h/blob/master/LICENSE) |
| [c-env-utils](https://github.com/matyalatte/c-env-utils) | Utilities for environment info | [MIT](http://opensource.org/licenses/MIT) |
| [tiny-str-match](https://github.com/matyalatte/tiny-str-match) | String validator | [MIT](http://opensource.org/licenses/MIT) |
| [gtk-ansi-parser](https://github.com/matyalatte/gtk-ansi-parser) | ANSI parser for GtkTextBuffer | [MIT](http://opensource.org/licenses/MIT) |
