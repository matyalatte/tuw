# Help Documents

You can put some documents in menu bar.  

![sample](https://github.com/matyalatte/tuw/assets/69258547/e408a179-6f9f-4769-ab3d-57f87d392a4f)

```json
"help": [
    {
        "type": "url",
        "label": "About Tuw",
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
