# Placeholders

`placeholder` is an optional key for path pickers and text boxes.  
It displays a message when the text box is empty.  

![Placeholder](https://github.com/matyalatte/tuw/assets/69258547/760a7df6-babf-4479-a7bc-059249e2cedb)

```json
{
    "gui": {
        "window_name": "Placeholder sample",
        "command": "echo %-% %-% %-%",
        "components": [
            {
                "type": "file",
                "label": "Some file",
                "placeholder": "Drop a file here!"
            },
            {
                "type": "folder",
                "label": "Some folder",
                "placeholder": "Drop a folder here!"
            },
            {
                "type": "text",
                "label": "Some text",
                "placeholder": "Type here!"
            }
        ]
    }
}
```
