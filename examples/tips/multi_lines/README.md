# Multiple Lines

Tuw executes single-line commands on the command prompt (or terminal.)  
If you need to run multiple commands, you should merge them into a single line command.  
I won't delve into command-line details because it's specific to the command prompt or terminal environment, not Tuw itself.  
But you can see some examples for it.  

## Windows

On windows, you can join the commands with ` && `.  
And some commands like `for` loop require parentheses `()`.

```json
{
    "label": "Search json in a folder (Windows)",
    "command": "@echo off && (for %%f in (\"%dir%\\*.json\") do (echo %%f)) && echo Done!",
    "button": "Echo!",
    "components": [
        {
            "type": "folder",
            "label": "a folder"
        }
    ]
}
```

## Unix/Linux

On non-Windows platforms, you can join the commands with `;`.

```json
{
    "label": "Search json in a folder (Unix/Linux)",
    "command": "for f in %dir%/*.json; do echo \"${f}\"; done; echo Done!",
    "button": "Echo!",
    "components": [
        {
            "type": "folder",
            "label": "a folder"
        }
    ]
}
```
