# Tool Tips

`tooltip` is an optional key for components to show a message when the mouse cursor is on a component.  

![tooltip](https://user-images.githubusercontent.com/69258547/223138605-9a9aa6a7-a5c9-4aa6-b0af-dd674b46160a.png)  

```json
{
    "type": "file",
    "label": "Some file path",
    "extension": "any files (*)|*",
    "tooltip": "tooltip for file path!"
},
{
    "type": "folder",
    "label": "Some folder path",
    "tooltip": "tooltip for folder path!"
},
{
    "type": "check_array",
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
