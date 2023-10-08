# Placeholder

`empty_message` is an optional key for path pickers and text boxes.  
It will show a message when the text box is empty.  

![Placeholder](https://github.com/matyalatte/tuw/assets/69258547/760a7df6-babf-4479-a7bc-059249e2cedb)

```json
{
    "gui": [
        {
            "label": "Placeholder",
            "window_name": "Placeholder sample",
            "command": "echo %-% %-% %-%",
            "components": [
                {
                    "type": "file",
                    "label": "Some file",
                    "empty_message": "Drop a file here!"
                },
                {
                    "type": "folder",
                    "label": "Some folder",
                    "empty_message": "Drop a folder here!"
                },
                {
                    "type": "text",
                    "label": "Some text",
                    "empty_message": "Type here!"
                }
            ]
        }
    ]
}
```
