# Simple Command Runner ver 0.4.1

![build](https://github.com/matyalatte/Simple-Command-Runner/actions/workflows/build_all.yml/badge.svg)
![test](https://github.com/matyalatte/Simple-Command-Runner/actions/workflows/test.yml/badge.svg)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/4aee3ee5172e4c38915d07f9c62725d3)](https://www.codacy.com/gh/matyalatte/Simple-Command-Runner/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=matyalatte/Simple-Command-Runner&amp;utm_campaign=Badge_Grade)
<a href="https://www.buymeacoffee.com/matyalatteQ" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/default-orange.png" alt="Buy Me A Coffee" height="41" width="174"></a>  

Small and simple GUI wrapper for command-line tools.  

## Only 1MB for a portable GUI!

Simple Command Runner will give a very simple GUI to your scripts.  
All you need is write a `.json` file and run a small executable.  
It can make a GUI to run commands of your scripts.  
**No need coding, no need browsers, and no need stupidly large executables**!  

![sample](https://user-images.githubusercontent.com/69258547/192090786-11a3f5ef-988e-442f-8ba9-fd1636b9f350.png)
<img src=https://user-images.githubusercontent.com/69258547/192090797-f5e5b52d-59aa-4942-a361-2c8b5c7bd746.png width=387></img>  

## Features

-   Define GUI in .json
-   Save arguments
-   Input paths by drag and drop
-   Cross-platform
-   Native look and feel
-   Portable
-   Small size (under 1MB)

## Downloads

You can download executables from [the release page](https://github.com/matyalatte/Simple-Command-Runner/releases)

-   `SimpleCommandRunner*-Windows*.zip` is for Windows.  
-   `SimpleCommandRunner*-macOS*.tar.bz2` is for macOS.  
-   `SimpleCommandRunner*-Linux*.tar.bz2` is for Ubuntu (20.04 or later).  

> `*-packed.*` are the 1MB executables that were compressed by [UPX](https://github.com/upx/upx).  
> But they might cause some problems (e.g. false positives by anti-viruses) on your machine.  
> Use the uncompressed ones if they won't work.  

> The linux builds only support Ubuntu due to the glibc dependences.  
> Build the executable by yourself if you want to use it on other linux distros.  

## Examples

There are some [json files](../examples/README.md) to learn how to define GUIs.  

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

You can also make `gui_definition.json` with [web forms](https://rjsf-team.github.io/react-jsonschema-form/#eyJmb3JtRGF0YSI6eyJndWkiOltdLCJoZWxwIjpbXX0sInNjaGVtYSI6eyIkc2NoZW1hIjoiaHR0cDovL2pzb24tc2NoZW1hLm9yZy9kcmFmdC0wNy9zY2hlbWEjIiwidHlwZSI6Im9iamVjdCIsInByb3BlcnRpZXMiOnsicmVjb21tZW5kZWQiOnsiJHJlZiI6IiMvZGVmaW5pdGlvbnMvdHlwZXMvdmVyc2lvbiJ9LCJtaW5pbXVtX3JlcXVpcmVkIjp7IiRyZWYiOiIjL2RlZmluaXRpb25zL3R5cGVzL3ZlcnNpb24ifSwiZ3VpIjp7InR5cGUiOiJhcnJheSIsIml0ZW1zIjp7InR5cGUiOiJvYmplY3QiLCJwcm9wZXJ0aWVzIjp7ImxhYmVsIjp7InR5cGUiOiJzdHJpbmcifSwiY29tbWFuZCI6eyJ0eXBlIjoic3RyaW5nIn0sIndpbmRvd19uYW1lIjp7InR5cGUiOiJzdHJpbmcifSwiYnV0dG9uIjp7InR5cGUiOiJzdHJpbmcifSwic2hvd19sYXN0X2xpbmUiOnsidHlwZSI6ImJvb2xlYW4ifSwiY2hlY2tfZXhpdF9jb2RlIjp7InR5cGUiOiJib29sZWFuIn0sImV4aXRfc3VjY2VzcyI6eyJ0eXBlIjoiaW50ZWdlciJ9LCJjb21wb25lbnRzIjp7InR5cGUiOiJhcnJheSIsIml0ZW1zIjp7InR5cGUiOiJvYmplY3QiLCJwcm9wZXJ0aWVzIjp7InR5cGUiOnsidHlwZSI6InN0cmluZyIsImVudW0iOlsic3RhdGljX3RleHQiLCJmaWxlIiwiZm9sZGVyIiwiY2hvaWNlIiwiY2hlY2siLCJjaGVja19hcnJheSIsInRleHQiLCJpbnQiLCJmbG9hdCJdfSwibGFiZWwiOnsidHlwZSI6InN0cmluZyJ9LCJpZCI6eyJ0eXBlIjoic3RyaW5nIn0sImFkZF9xdW90ZXMiOnsidHlwZSI6ImJvb2xlYW4ifX0sImFsbE9mIjpbeyIkcmVmIjoiIy9kZWZpbml0aW9ucy9jb21wb25lbnRzL2ZpbGUifSx7IiRyZWYiOiIjL2RlZmluaXRpb25zL2NvbXBvbmVudHMvZm9sZGVyX29yX3RleHQifSx7IiRyZWYiOiIjL2RlZmluaXRpb25zL2NvbXBvbmVudHMvY2hlY2sifSx7IiRyZWYiOiIjL2RlZmluaXRpb25zL2NvbXBvbmVudHMvaW50In0seyIkcmVmIjoiIy9kZWZpbml0aW9ucy9jb21wb25lbnRzL2Zsb2F0In0seyIkcmVmIjoiIy9kZWZpbml0aW9ucy9jb21wb25lbnRzL2Nob2ljZSJ9LHsiJHJlZiI6IiMvZGVmaW5pdGlvbnMvY29tcG9uZW50cy9jaGVja19hcnJheSJ9XSwicmVxdWlyZWQiOlsidHlwZSIsImxhYmVsIl19fX0sInJlcXVpcmVkIjpbImxhYmVsIiwiY29tbWFuZCIsImNvbXBvbmVudHMiXX19LCJoZWxwIjp7InR5cGUiOiJhcnJheSIsIml0ZW1zIjp7InR5cGUiOiJvYmplY3QiLCJwcm9wZXJ0aWVzIjp7InR5cGUiOnsidHlwZSI6InN0cmluZyIsImVudW0iOlsidXJsIiwiZmlsZSJdfSwibGFiZWwiOnsidHlwZSI6InN0cmluZyJ9fSwicmVxdWlyZWQiOlsidHlwZSIsImxhYmVsIl0sImFsbE9mIjpbeyJpZiI6eyJwcm9wZXJ0aWVzIjp7InR5cGUiOnsiY29uc3QiOiJ1cmwifX19LCJ0aGVuIjp7InByb3BlcnRpZXMiOnsidXJsIjp7InR5cGUiOiJzdHJpbmcifSwicGF0aCI6eyJ0eXBlIjoibnVsbCJ9fX19LHsiaWYiOnsicHJvcGVydGllcyI6eyJ0eXBlIjp7ImNvbnN0IjoiZmlsZSJ9fX0sInRoZW4iOnsicHJvcGVydGllcyI6eyJ1cmwiOnsidHlwZSI6Im51bGwifSwicGF0aCI6eyJ0eXBlIjoic3RyaW5nIn19fX1dfX19LCJyZXF1aXJlZCI6WyJndWkiXSwiZGVmaW5pdGlvbnMiOnsidHlwZXMiOnsidmVyc2lvbiI6eyJ0eXBlIjoic3RyaW5nIiwicGF0dGVybiI6Il5bLjAtOV0rJCIsIm1pbkxlbmd0aCI6MSwibWF4TGVuZ3RoIjo4fX0sImNvbXBvbmVudHMiOnsiZmlsZSI6eyJpZiI6eyJwcm9wZXJ0aWVzIjp7InR5cGUiOnsiY29uc3QiOiJmaWxlIn19fSwidGhlbiI6eyJwcm9wZXJ0aWVzIjp7ImV4dGVuc2lvbiI6eyJ0eXBlIjoic3RyaW5nIn0sImVtcHR5X21lc3NhZ2UiOnsidHlwZSI6InN0cmluZyJ9LCJkZWZhdWx0Ijp7InR5cGUiOiJzdHJpbmcifSwidG9vbHRpcCI6eyJ0eXBlIjoic3RyaW5nIn19fX0sImZvbGRlcl9vcl90ZXh0Ijp7ImlmIjp7InByb3BlcnRpZXMiOnsidHlwZSI6eyJlbnVtIjpbImZvbGRlciIsInRleHQiXX19fSwidGhlbiI6eyJwcm9wZXJ0aWVzIjp7ImVtcHR5X21lc3NhZ2UiOnsidHlwZSI6InN0cmluZyJ9LCJkZWZhdWx0Ijp7InR5cGUiOiJzdHJpbmcifSwidG9vbHRpcCI6eyJ0eXBlIjoic3RyaW5nIn19fX0sImNoZWNrIjp7ImlmIjp7InByb3BlcnRpZXMiOnsidHlwZSI6eyJjb25zdCI6ImNoZWNrIn19fSwidGhlbiI6eyJwcm9wZXJ0aWVzIjp7ImRlZmF1bHQiOnsidHlwZSI6ImJvb2xlYW4ifSwidG9vbHRpcCI6eyJ0eXBlIjoic3RyaW5nIn0sInZhbHVlIjp7InR5cGUiOiJzdHJpbmcifX19fSwiaW50Ijp7ImlmIjp7InByb3BlcnRpZXMiOnsidHlwZSI6eyJjb25zdCI6ImludCJ9fX0sInRoZW4iOnsicHJvcGVydGllcyI6eyJtaW4iOnsidHlwZSI6ImludGVnZXIifSwibWF4Ijp7InR5cGUiOiJpbnRlZ2VyIn0sImluYyI6eyJ0eXBlIjoiaW50ZWdlciJ9LCJ3cmFwIjp7InR5cGUiOiJib29sZWFuIn0sImRlZmF1bHQiOnsidHlwZSI6ImludGVnZXIifSwidG9vbHRpcCI6eyJ0eXBlIjoic3RyaW5nIn19fX0sImZsb2F0Ijp7ImlmIjp7InByb3BlcnRpZXMiOnsidHlwZSI6eyJjb25zdCI6ImZsb2F0In19fSwidGhlbiI6eyJwcm9wZXJ0aWVzIjp7Im1pbiI6eyJ0eXBlIjoibnVtYmVyIn0sIm1heCI6eyJ0eXBlIjoibnVtYmVyIn0sImluYyI6eyJ0eXBlIjoibnVtYmVyIn0sIndyYXAiOnsidHlwZSI6ImJvb2xlYW4ifSwiZGlnaXRzIjp7InR5cGUiOiJpbnRlZ2VyIn0sImRlZmF1bHQiOnsidHlwZSI6Im51bWJlciJ9LCJ0b29sdGlwIjp7InR5cGUiOiJzdHJpbmcifX19fSwiY2hvaWNlIjp7ImlmIjp7InByb3BlcnRpZXMiOnsidHlwZSI6eyJjb25zdCI6ImNob2ljZSJ9fX0sInRoZW4iOnsicHJvcGVydGllcyI6eyJkZWZhdWx0Ijp7InR5cGUiOiJpbnRlZ2VyIn0sInRvb2x0aXAiOnsidHlwZSI6InN0cmluZyJ9LCJpdGVtcyI6eyJ0eXBlIjoiYXJyYXkiLCJpdGVtcyI6eyJ0eXBlIjoib2JqZWN0IiwicHJvcGVydGllcyI6eyJsYWJlbCI6eyJ0eXBlIjoic3RyaW5nIn0sInZhbHVlIjp7InR5cGUiOiJzdHJpbmcifX0sInJlcXVpcmVkIjpbImxhYmVsIl19fX19fSwiY2hlY2tfYXJyYXkiOnsiaWYiOnsicHJvcGVydGllcyI6eyJ0eXBlIjp7ImNvbnN0IjoiY2hlY2tfYXJyYXkifX19LCJ0aGVuIjp7InByb3BlcnRpZXMiOnsiaXRlbXMiOnsidHlwZSI6ImFycmF5IiwiaXRlbXMiOnsidHlwZSI6Im9iamVjdCIsInByb3BlcnRpZXMiOnsibGFiZWwiOnsidHlwZSI6InN0cmluZyJ9LCJ2YWx1ZSI6eyJ0eXBlIjoic3RyaW5nIn0sInRvb2x0aXAiOnsidHlwZSI6InN0cmluZyJ9LCJkZWZhdWx0Ijp7InR5cGUiOiJib29sZWFuIn19LCJyZXF1aXJlZCI6WyJsYWJlbCJdfX19fX19fX0sInVpU2NoZW1hIjp7fSwidGhlbWUiOiJkZWZhdWx0IiwibGl2ZVNldHRpbmdzIjp7InNob3dFcnJvckxpc3QiOiJ0b3AiLCJleHBlcmltZW50YWxfZGVmYXVsdEZvcm1TdGF0ZUJlaGF2aW9yIjp7ImFycmF5TWluSXRlbXMiOiJwb3B1bGF0ZSIsImVtcHR5T2JqZWN0RmllbGRzIjoicG9wdWxhdGVBbGxEZWZhdWx0cyJ9LCJvbWl0RXh0cmFEYXRhIjp0cnVlLCJsaXZlT21pdCI6dHJ1ZX19)

## FAQ

[Frequently Asked Questions](./FAQ.md)

## Building

[Building Executables](./Building.md)

## License

Files in this repository are available under the [GPL2+](../license.txt).  

## External Projects

| Project | Used for | License |
| -- | -- | -- |
| [wxWidgets library](https://github.com/wxWidgets/wxWidgets) | GUI framework | [Modified LGPL](https://github.com/wxWidgets/wxWidgets/blob/master/docs/licence.txt) | 
| [nlohmann's json library](https://github.com/nlohmann/json) | JSON handling | [MIT](https://github.com/nlohmann/json/blob/develop/LICENSE.MIT) |
| [UPX](https://github.com/upx/upx) | EXE compression | [Modified GPL2+](https://github.com/upx/upx/blob/devel/LICENSE) |
