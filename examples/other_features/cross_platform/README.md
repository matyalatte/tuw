# Cross-platform Support

There are optional keys to create platform-specific GUIs without needing separate JSON files for each platform.  

## "command_'os'"

You can use options to override commands based on the operating system:

-   `command_win` for Windows
-   `command_mac` for macOS
-   `command_linux` for Linux (and other unix variants)

Tuw will use the platform-specific command instead of the default `command` value.  

```json
{
    "command_win": "echo Windows!",
    "command_mac": "echo macOS!",
    "command_linux": "echo Linux!",
    "show_last_line": true,
    "components": []
}
```

## "platforms"

You can specify the operating systems on which each component should be active using the `platforms` key:

-   `win` for Windows
-   `mac` for macOS
-   `linux` for Linux (and other unix variants)

Tuw will ignore the component if the current OS does not match the specified platform(s).  

```json
{
    "command": "echo %os%",
    "show_last_line": true,
    "components": [
        {
            "type": "check",
            "label": "Windows!",
            "id": "os",
            "default": true,
            "platforms": "win"
        },
        {
            "type": "check",
            "label": "Not Windows!",
            "id": "os",
            "default": true,
            "platforms": [ "mac", "linux" ]
        }
    ]
}
```
