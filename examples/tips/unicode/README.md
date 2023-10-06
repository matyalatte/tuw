# Unicode Characters

Tuw supports UTF-8 strings.  

![unicode](https://github.com/matyalatte/Simple-Command-Runner/assets/69258547/b260277f-9a81-4eed-b6d4-3d9b0bf1924d)

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
            "default": "ああああ",
            "id": "รหัส"
        },
        {
            "type": "folder",
            "label": "폴더",
            "default": "いいいい",
        },
        {
            "type": "check",
            "label": "вариант",
            "default": true
        }
    ]
}
```
