# Cross-platform Support

There are optional keys to make platfrom specific GUIs.  
No need to make JSON files for each platforms.  

## "command_'os'"

There are optional keys to overwrite the command.  
`command_win` is for Windows, `command_mac` is for macOS, and `command_linux` is for Linux.  
Simple Command Runner will use the platform specific commands instead of the value of `command`.  

```json
{
    "label": "Platform Specific Commands",
    "command_win": "echo Windows!",
    "command_mac": "echo macOS!",
    "command_linux": "echo Linux!",
    "show_last_line": true,
    "components": []
}
```

## "platforms"

`platforms` is an optional key for components.  
Simple Command Runner will ignore the component if the running OS is not in the values of `platforms`.  
`win` for Windows components, `mac` for macOS, and `linux` for Linux.  

```json
{
    "label": "Platform Specific Components",
    "command": "echo %os%",
    "show_last_line": true,
    "components": [
        {
            "type": "check",
            "label": "Windows!",
            "id": "os",
            "default": true,
            "platforms": [ "win" ]
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
