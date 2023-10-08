# Renaming Buttons

`button` is an optional key for path pickers.  
It'll rename the button of the picker.  

![Button](https://github.com/matyalatte/tuw/assets/69258547/8505cd7c-5045-4be5-8bba-0e2bd000523a)

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
