# Simple Command Runner ver 0.6.0

![build](https://github.com/matyalatte/Simple-Command-Runner/actions/workflows/build_all.yml/badge.svg)
![test](https://github.com/matyalatte/Simple-Command-Runner/actions/workflows/test.yml/badge.svg)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/4aee3ee5172e4c38915d07f9c62725d3)](https://www.codacy.com/gh/matyalatte/Simple-Command-Runner/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=matyalatte/Simple-Command-Runner&amp;utm_campaign=Badge_Grade)
<a href="https://www.buymeacoffee.com/matyalatteQ" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/default-orange.png" alt="Buy Me A Coffee" height="41" width="174"></a>  

Small and simple GUI wrapper for command-line tools.  

## About this branch

This branch will use [libui](https://github.com/libui-ng/libui-ng) as a GUI library.  
It still has some issues and lacks some features I want.  
But it'll be the best GUI library for Simple Command Runner in the future.  
  
Here is the list of changes from the main branch.

- EXE became smaller. (600KB for the windows build and 300KB for the unix builds!)  
- Compilation time became faster.  
- Support older build systems. (No need CMake 3.25!)
- User-friendly workflows. (Just by running meson via command-line!)
- Float pickers are unavaiable.
- The `tooltip` option is unavailable.
- The `placeholder` option is unavailable.
- The layout is a little bit ugly.
- Haven't beed tested yet.

## Only 600KB for a portable GUI!

Simple Command Runner will give a very simple GUI to your scripts.  
All you need is write a `.json` file and run a small executable.  
It can make a GUI to run commands of your scripts.  
**No need coding, no need browsers, and no need stupidly large executables**!  

![sample](https://user-images.githubusercontent.com/69258547/192090786-11a3f5ef-988e-442f-8ba9-fd1636b9f350.png)
<img src=https://user-images.githubusercontent.com/69258547/192090797-f5e5b52d-59aa-4942-a361-2c8b5c7bd746.png width=387></img>  

## Features

-   [Define GUI in JSON](../examples/)
-   [Embed JSON into exe](../examples/get_start/json_embed/)
-   Save arguments
-   Input paths by drag and drop
-   Cross-platform
-   Native look and feel
-   Portable
-   Small size
-   UTF-8 support

## Downloads

You can download executables from [the release page](https://github.com/matyalatte/Simple-Command-Runner/releases)

-   `SimpleCommandRunner*-Windows*.zip` is for Windows (7 or later).  
-   `SimpleCommandRunner*-macOS*.tar.bz2` is for macOS (10.9 or later).  
-   `SimpleCommandRunner*-Linux*.tar.bz2` is for Ubuntu (20.04 or later).  

> The linux builds only support Ubuntu due to the glibc dependences.  
> Build the executable by yourself if you want to use it on other linux distros.  

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
        "url": "https://raw.githubusercontent.com/matyalatte/Simple-Command-Runner/main/schema/schema.json"
    }
]
```

### react-jsonschema-form

You can also make `gui_definition.json` with [web forms](https://rjsf-team.github.io/react-jsonschema-form/#eyJmb3JtRGF0YSI6eyJndWkiOltdLCJoZWxwIjpbXX0sInNjaGVtYSI6eyIkc2NoZW1hIjoiaHR0cDovL2pzb24tc2NoZW1hLm9yZy9kcmFmdC0wNy9zY2hlbWEjIiwidHlwZSI6Im9iamVjdCIsInByb3BlcnRpZXMiOnsiZ3VpIjp7InR5cGUiOiJhcnJheSIsIml0ZW1zIjp7InR5cGUiOiJvYmplY3QiLCJwcm9wZXJ0aWVzIjp7ImxhYmVsIjp7InR5cGUiOiJzdHJpbmcifSwiY29tbWFuZCI6eyJ0eXBlIjoic3RyaW5nIn0sIndpbmRvd19uYW1lIjp7InR5cGUiOiJzdHJpbmcifSwiYnV0dG9uIjp7InR5cGUiOiJzdHJpbmcifSwic2hvd19sYXN0X2xpbmUiOnsidHlwZSI6ImJvb2xlYW4ifSwiY29tcG9uZW50cyI6eyJ0eXBlIjoiYXJyYXkiLCJpdGVtcyI6eyJ0eXBlIjoib2JqZWN0IiwicHJvcGVydGllcyI6eyJ0eXBlIjp7InR5cGUiOiJzdHJpbmciLCJlbnVtIjpbInN0YXRpY190ZXh0IiwiZmlsZSIsImZvbGRlciIsImNob2ljZSIsImNoZWNrIiwiY2hlY2tfYXJyYXkiLCJ0ZXh0IiwiaW50IiwiZmxvYXQiXX0sImxhYmVsIjp7InR5cGUiOiJzdHJpbmcifSwiaWQiOnsidHlwZSI6InN0cmluZyJ9LCJhZGRfcXVvdGVzIjp7InR5cGUiOiJib29sZWFuIn19LCJhbGxPZiI6W3siJHJlZiI6IiMvZGVmaW5pdGlvbnMvY29tcG9uZW50cy9maWxlIn0seyIkcmVmIjoiIy9kZWZpbml0aW9ucy9jb21wb25lbnRzL2ZvbGRlcl9vcl90ZXh0In0seyIkcmVmIjoiIy9kZWZpbml0aW9ucy9jb21wb25lbnRzL2NoZWNrIn0seyIkcmVmIjoiIy9kZWZpbml0aW9ucy9jb21wb25lbnRzL2ludCJ9LHsiJHJlZiI6IiMvZGVmaW5pdGlvbnMvY29tcG9uZW50cy9mbG9hdCJ9LHsiJHJlZiI6IiMvZGVmaW5pdGlvbnMvY29tcG9uZW50cy9jaG9pY2UifSx7IiRyZWYiOiIjL2RlZmluaXRpb25zL2NvbXBvbmVudHMvY2hlY2tfYXJyYXkifV0sInJlcXVpcmVkIjpbInR5cGUiLCJsYWJlbCJdfX19LCJyZXF1aXJlZCI6WyJsYWJlbCIsImNvbW1hbmQiLCJjb21wb25lbnRzIl19fSwiaGVscCI6eyJ0eXBlIjoiYXJyYXkiLCJpdGVtcyI6eyJ0eXBlIjoib2JqZWN0IiwicHJvcGVydGllcyI6eyJ0eXBlIjp7InR5cGUiOiJzdHJpbmciLCJlbnVtIjpbInVybCIsImZpbGUiXX0sImxhYmVsIjp7InR5cGUiOiJzdHJpbmcifX0sInJlcXVpcmVkIjpbInR5cGUiLCJsYWJlbCJdLCJhbGxPZiI6W3siaWYiOnsicHJvcGVydGllcyI6eyJ0eXBlIjp7ImNvbnN0IjoidXJsIn19fSwidGhlbiI6eyJwcm9wZXJ0aWVzIjp7InVybCI6eyJ0eXBlIjoic3RyaW5nIn0sInBhdGgiOnsidHlwZSI6Im51bGwifX19fSx7ImlmIjp7InByb3BlcnRpZXMiOnsidHlwZSI6eyJjb25zdCI6ImZpbGUifX19LCJ0aGVuIjp7InByb3BlcnRpZXMiOnsidXJsIjp7InR5cGUiOiJudWxsIn0sInBhdGgiOnsidHlwZSI6InN0cmluZyJ9fX19XX19fSwicmVxdWlyZWQiOlsiZ3VpIl0sImRlZmluaXRpb25zIjp7ImNvbXBvbmVudHMiOnsiZmlsZSI6eyJpZiI6eyJwcm9wZXJ0aWVzIjp7InR5cGUiOnsiY29uc3QiOiJmaWxlIn19fSwidGhlbiI6eyJwcm9wZXJ0aWVzIjp7ImV4dGVuc2lvbiI6eyJ0eXBlIjoic3RyaW5nIn0sImVtcHR5X21lc3NhZ2UiOnsidHlwZSI6InN0cmluZyJ9LCJkZWZhdWx0Ijp7InR5cGUiOiJzdHJpbmcifSwidG9vbHRpcCI6eyJ0eXBlIjoic3RyaW5nIn19fX0sImZvbGRlcl9vcl90ZXh0Ijp7ImlmIjp7InByb3BlcnRpZXMiOnsidHlwZSI6eyJlbnVtIjpbImZvbGRlciIsInRleHQiXX19fSwidGhlbiI6eyJwcm9wZXJ0aWVzIjp7ImVtcHR5X21lc3NhZ2UiOnsidHlwZSI6InN0cmluZyJ9LCJkZWZhdWx0Ijp7InR5cGUiOiJzdHJpbmcifSwidG9vbHRpcCI6eyJ0eXBlIjoic3RyaW5nIn19fX0sImNoZWNrIjp7ImlmIjp7InByb3BlcnRpZXMiOnsidHlwZSI6eyJjb25zdCI6ImNoZWNrIn19fSwidGhlbiI6eyJwcm9wZXJ0aWVzIjp7ImRlZmF1bHQiOnsidHlwZSI6ImJvb2xlYW4ifSwidG9vbHRpcCI6eyJ0eXBlIjoic3RyaW5nIn0sInZhbHVlIjp7InR5cGUiOiJzdHJpbmcifX19fSwiaW50Ijp7ImlmIjp7InByb3BlcnRpZXMiOnsidHlwZSI6eyJjb25zdCI6ImludCJ9fX0sInRoZW4iOnsicHJvcGVydGllcyI6eyJtaW4iOnsidHlwZSI6ImludGVnZXIifSwibWF4Ijp7InR5cGUiOiJpbnRlZ2VyIn0sImluYyI6eyJ0eXBlIjoiaW50ZWdlciJ9LCJ3cmFwIjp7InR5cGUiOiJib29sZWFuIn0sImRlZmF1bHQiOnsidHlwZSI6ImludGVnZXIifSwidG9vbHRpcCI6eyJ0eXBlIjoic3RyaW5nIn19fX0sImZsb2F0Ijp7ImlmIjp7InByb3BlcnRpZXMiOnsidHlwZSI6eyJjb25zdCI6ImZsb2F0In19fSwidGhlbiI6eyJwcm9wZXJ0aWVzIjp7Im1pbiI6eyJ0eXBlIjoibnVtYmVyIn0sIm1heCI6eyJ0eXBlIjoibnVtYmVyIn0sImluYyI6eyJ0eXBlIjoibnVtYmVyIn0sIndyYXAiOnsidHlwZSI6ImJvb2xlYW4ifSwiZGlnaXRzIjp7InR5cGUiOiJpbnRlZ2VyIn0sImRlZmF1bHQiOnsidHlwZSI6Im51bWJlciJ9LCJ0b29sdGlwIjp7InR5cGUiOiJzdHJpbmcifX19fSwiY2hvaWNlIjp7ImlmIjp7InByb3BlcnRpZXMiOnsidHlwZSI6eyJjb25zdCI6ImNob2ljZSJ9fX0sInRoZW4iOnsicHJvcGVydGllcyI6eyJkZWZhdWx0Ijp7InR5cGUiOiJpbnRlZ2VyIn0sInRvb2x0aXAiOnsidHlwZSI6InN0cmluZyJ9LCJpdGVtcyI6eyJ0eXBlIjoiYXJyYXkiLCJpdGVtcyI6eyJ0eXBlIjoib2JqZWN0IiwicHJvcGVydGllcyI6eyJsYWJlbCI6eyJ0eXBlIjoic3RyaW5nIn0sInZhbHVlIjp7InR5cGUiOiJzdHJpbmcifX0sInJlcXVpcmVkIjpbImxhYmVsIl19fX19fSwiY2hlY2tfYXJyYXkiOnsiaWYiOnsicHJvcGVydGllcyI6eyJ0eXBlIjp7ImNvbnN0IjoiY2hlY2tfYXJyYXkifX19LCJ0aGVuIjp7InByb3BlcnRpZXMiOnsiaXRlbXMiOnsidHlwZSI6ImFycmF5IiwiaXRlbXMiOnsidHlwZSI6Im9iamVjdCIsInByb3BlcnRpZXMiOnsibGFiZWwiOnsidHlwZSI6InN0cmluZyJ9LCJ2YWx1ZSI6eyJ0eXBlIjoic3RyaW5nIn0sInRvb2x0aXAiOnsidHlwZSI6InN0cmluZyJ9LCJkZWZhdWx0Ijp7InR5cGUiOiJib29sZWFuIn19LCJyZXF1aXJlZCI6WyJsYWJlbCJdfX19fX19fX0sInVpU2NoZW1hIjp7fSwidGhlbWUiOiJkZWZhdWx0IiwibGl2ZVNldHRpbmdzIjp7InNob3dFcnJvckxpc3QiOiJ0b3AiLCJleHBlcmltZW50YWxfZGVmYXVsdEZvcm1TdGF0ZUJlaGF2aW9yIjp7ImFycmF5TWluSXRlbXMiOiJwb3B1bGF0ZSIsImVtcHR5T2JqZWN0RmllbGRzIjoicG9wdWxhdGVBbGxEZWZhdWx0cyJ9LCJvbWl0RXh0cmFEYXRhIjp0cnVlLCJsaXZlT21pdCI6dHJ1ZX19)

## FAQ

[Frequently Asked Questions](./FAQ.md)

## Building

[Building Executables](./Building.md)

## Want Smaller Executables?

You can use [UPX to compress the executables](./UPX.md).  
They will be **under 300KB**!  

## License

Files in this repository are available under the [GPL2+](../license.txt).  

## External Projects

| Project | Used for | License |
| -- | -- | -- |
| [libui-ng](https://github.com/libui-ng/libui-ng) | GUI framework | [MIT](http://opensource.org/licenses/MIT) | 
| [subprocess.h](https://github.com/sheredom/subprocess.h) | Command processing | [Unlicense](https://github.com/sheredom/subprocess.h/blob/master/LICENSE) | 
| [RapidJSON](https://github.com/Tencent/rapidjson) | JSON handling | [MIT](http://opensource.org/licenses/MIT) |
| [wxWidgets library](https://github.com/wxWidgets/wxWidgets) | Some utils | [Modified LGPL](https://github.com/wxWidgets/wxWidgets/blob/master/docs/licence.txt) | 
