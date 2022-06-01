# GUI for Texconv
[Texconv](https://github.com/microsoft/DirectXTex/wiki/Texconv) is a texture converter made by Microsoft.<br>
But it doesn't support GUI.<br>
So I made my own GUI for Texconv. <br>
Download texconv.exe form [here](https://github.com/microsoft/DirectXTex/releases) and put in the same directory as SimpleCommandRunner.exe and gui_definition.json.

![texconv](https://user-images.githubusercontent.com/69258547/166093778-90ded69c-8db3-4531-b2d6-baf075f9c5f1.png)

It will add help urls to menu bar.<br>

```
"help": [
    {
        "type": "url",
        "label": "About Texconv",
        "url": "https://github.com/microsoft/DirectXTex/wiki/Texconv"
    },
    {
        "type": "url",
        "label": "About Simple Command Runner",
        "url": "https://github.com/matyalatte/Simple-Command-Runner"
    }
]
```

