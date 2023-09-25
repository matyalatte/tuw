# Help Documents

You can put some documents in menu bar.  

![sample](https://user-images.githubusercontent.com/69258547/222972599-bab3ff85-2c6c-432d-91e7-8244a8a8e514.png)

```json
"help": [
    {
        "type": "url",
        "label": "About Simple Command Runner",
        "url": "https://github.com/matyalatte/Tuw"
    },
    {
        "type": "url",
        "label": "Releases",
        "url": "https://github.com/matyalatte/Tuw/releases"
    },
    {
        "type": "file",
        "label": "Open JSON File",
        "path": "./gui_definition.json"
    }
]
```

-   `type`: Document type. `url` and `file` are available.  
-   `label`: String that will be shown in menu bar.  
-   `url`: URL for `url` type document. It'll be opened in user's default browser.  
-   `path`: File path for `file` type document. It'll be opened in user's default application.  
