# Unicode Characters

Simple Command Runner supports UTF-8 strings.  

![unicode](https://github.com/matyalatte/Simple-Command-Runner/assets/69258547/72c60172-3081-48b3-b72f-0a81bd1be397)

```json
{
    "label": "Unicode Sample",
    "command": "echo file: %รหัส% & echo folder: %-% & echo checkbox: %-%",
    "button": "こんにちは！",
    "components": [
        {
            "type": "file",
            "label": "文件",
            "extension": "any files | *",
            "id": "รหัส"
        },
        {
            "type": "folder",
            "label": "폴더"
        },
        {
            "type": "check",
            "label": "вариант"
        }
    ]
}
```
