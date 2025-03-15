# Tool Tips

`tooltip` is an option for components that displays a message when the mouse cursor hovers over the component.  

![tooltip](https://github.com/matyalatte/tuw/assets/69258547/8c7244ee-48ce-4492-973c-a3e6c628b8ed)  

```json
{
    "type": "file",
    "id": "x",
    "label": "Some file path",
    "extension": "any files (*)|*",
    "tooltip": "tooltip for file path!"
},
{
    "type": "folder",
    "id": "y",
    "label": "Some folder path",
    "tooltip": "tooltip for folder path!"
},
{
    "type": "check_array",
    "id": "z",
    "label": "options",
    "items": [
        {
            "label": "falg1",
            "tooltip": "tooltip for flag1!"
        },
        {
            "label": "falg2",
            "tooltip": "flag2!"
        },
        {
            "label": "falg3",
            "tooltip": "flag3..."
        }
    ]
}
```
