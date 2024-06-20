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
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/4aee3ee5172e4c38915d07f9c62725d3)](https://app.codacy.com/gh/matyalatte/tuw/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)

## Only 500KB for a portable GUI!

Tuw will give a very simple GUI to your scripts.  
All you need is a JSON file and a tiny executable.  
**No need compilers, no need browsers, and no need huge executables**!  

![sample](https://github.com/matyalatte/tuw/assets/69258547/9b3c8487-010e-497b-b66c-95af84906dd0)
<img src=https://user-images.githubusercontent.com/69258547/192090797-f5e5b52d-59aa-4942-a361-2c8b5c7bd746.png width=387></img>  

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

-   `Tuw*-Windows*.zip` is for Windows (7 or later.)  
-   `Tuw*-macOS*.tar.bz2` is for macOS (10.9 or later.)  
-   `Tuw*-Linux*.tar.bz2` is for Linux (with GTK3.14, GLIBC2.15, and GLIBCXX3.4.21, or later versions of the libraries.)  

> Linux builds only support distributions using glibc.  
> [Build the executable](./Building.md) by yourself if you want to use it on unsupported distros.  

## Examples

There are some [JSON files](../examples/README.md) to learn how to define GUIs.  

## JSON Schema

There is a [schema](../schema/schema.json) for `gui_definition.json`.  
You can use it to validate definitions while editing files.  

### VSCode

For VSCode, you can add the schema path to `settings.json` (`File > Preferences > Settings > JSON:Schemas > Edit in settings.json`.)  

```json
"json.schemas": [
    {
        "fileMatch": [ "gui_definition.json" ],
        "url": "https://raw.githubusercontent.com/matyalatte/tuw/main/schema/schema.json"
    }
]
```

## FAQ

[Frequently Asked Questions](./FAQ.md)

## Building

[Building Executables](./Building.md)

## License

Files in this repository are available under the [MIT license](../license.txt).  

## External Projects

| Project | About | License |
| -- | -- | -- |
| [libui-ng](https://github.com/libui-ng/libui-ng) | GUI framework | [MIT](http://opensource.org/licenses/MIT) |
| [RapidJSON](https://github.com/Tencent/rapidjson) | JSON parser | [MIT](http://opensource.org/licenses/MIT) |
| [subprocess.h](https://github.com/sheredom/subprocess.h) | Command processor | [Unlicense](https://github.com/sheredom/subprocess.h/blob/master/LICENSE) |
| [c-env-utils](https://github.com/matyalatte/c-env-utils) | Utilities for env info | [MIT](http://opensource.org/licenses/MIT) |
