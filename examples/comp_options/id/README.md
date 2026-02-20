# ID

`id` is an option for components.
You can use the defined ids as variable names in commands.  

```json
{
    "command": "echo x: %x% & echo y: %y% & echo x: %x%",
    "button": "Echo!",
    "components": [
        {
            "type": "text",
            "label": "Value of y",
            "id": "y"
        },
        {
            "type": "text",
            "label": "Value of x",
            "id": "x"
        }
    ]
}
```

## Predefined IDs

There are some predefined ids.  

-   `%%` will be replaced with `%` at runtime.  
-   `%__CWD__%` will be replaced with the executable directory.  
-   `%__HOME__%` will be replaced with the user's home directory.  
-   All ids that start with `_` are reserved.  

```json
{
    "command": "echo percent: %% & echo cwd: %__CWD__% & echo home: %__HOME__%",
    "button": "Echo!",
    "components": []
}
```
