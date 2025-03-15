# Renaming Buttons

`button` is an option to customize buttons in path pickers.
It allows you to rename the button associated with the picker component.  

![Button](https://github.com/matyalatte/tuw/assets/69258547/8505cd7c-5045-4be5-8bba-0e2bd000523a)

```json
{
    "gui": {
        "window_name": "Button sample",
        "command": "echo %f% %dir%",
        "components": [
            {
                "type": "file",
                "id": "f",
                "label": "Some file",
                "button": "..."
            },
            {
                "type": "folder",
                "id": "dir",
                "label": "Some folder",
                "button": "Open"
            }
        ]
    }
}
```
