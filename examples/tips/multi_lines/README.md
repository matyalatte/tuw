# Multiple Lines

Simple Command Runner will execute a single line command on the command prompt (or the terminal.)  
If you want to executle multiple commands, you should merge them into a single line command.  
I won't explain the details because it's the thing about the command-line, not the Simple Command Runner.  
But you can see some examples about it.  

## Windows

For windows, you can join the commands with ` && `.  
And some commands (e.g. for loop) require `()`.

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

For non-Windows platforms, you can join the commands with `;`.

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
