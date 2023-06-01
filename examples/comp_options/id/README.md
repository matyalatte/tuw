# ID

`id` is an optional key for components.  
You can use the defined ids as variable names in commands.  

```json
{
    "label": "IDs",
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

## Undefined IDs

When you put an undefined id in `%*%`, it'll use one of the compoents that have no id.

```json
{
    "label": "Undefined IDs",
    "command": "echo x: %% & echo y: %y% & echo z: %foo%",
    "button": "Echo!",
    "components": [
        {
            "type": "text",
            "label": "Value of y",
            "id": "y"
        },
        {
            "type": "text",
            "label": "Value of x"
        },
        {
            "type": "text",
            "label": "Value of z"
        }
    ]
}
```

## Predefined IDs

There are some predefined ids.  
`%_%` will be replaced with `%` at runtime.  
`%__CWD__%` will be replaced with the executable directory at runtime.  
Also, all ids that start with `_` are reserved.  

```json
{
    "label": "Reserved IDs",
    "command": "echo percent: %_% & echo cwd: %__CWD__%",
    "button": "Echo!",
    "components": []
}
```
