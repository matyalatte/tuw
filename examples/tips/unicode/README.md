# Unicode Characters

Tuw supports UTF-8 strings.  

![unicode](https://github.com/matyalatte/tuw/assets/69258547/b260277f-9a81-4eed-b6d4-3d9b0bf1924d)

```json
{
    "window_name": "Unicode Sample",
    "command": "echo file: %รหัส% & echo folder: %dir% & echo checkbox: %check%",
    "button": "こんにちは！",
    "components": [
        {
            "type": "file",
            "id": "รหัส",
            "label": "文件",
            "extension": "any files | *",
            "default": "ああああ"
        },
        {
            "type": "folder",
            "id": "dir",
            "label": "폴더",
            "default": "いいいい",
        },
        {
            "type": "check",
            "id": "check",
            "label": "вариант",
            "default": true
        }
    ]
}
```
