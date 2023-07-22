# Renaming Buttons

`button` is an optional key for path pickers.  
It'll rename the button of the picker.  

![Button](https://github.com/matyalatte/Simple-Command-Runner/assets/69258547/979d987b-842a-4a20-a2e1-4fde85836a3e)

```json
{
    "gui": [
        {
            "label": "Button",
            "window_name": "Button sample",
            "command": "echo %-% %-%",
            "components": [
                {
                    "type": "file",
                    "label": "Some file",
                    "button": "..."
                },
                {
                    "type": "folder",
                    "label": "Some folder",
                    "button": "Open"
                }
            ]
        }
    ]
}
```
