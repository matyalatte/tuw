# Placeholder

`empty_message` is an optional key for path pickers and text boxes.  
It will show a message when the text box is empty.  

![Placeholder](https://github.com/matyalatte/Simple-Command-Runner/assets/69258547/7694fdec-c739-45b9-980c-64e2f81203d7)

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
