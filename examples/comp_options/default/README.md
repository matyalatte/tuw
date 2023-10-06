# Default Values

`default` is an optional key for components to set default values.  

![default](https://github.com/matyalatte/Simple-Command-Runner/assets/69258547/e7daec5f-398b-4ec5-9c91-32c2df049432)  

```json
{
    "type": "file",
    "label": "Some file path",
    "extension": "any files (*)|*",
    "default": "./foo/bar"
},
{
    "type": "check_array",
    "label": "options",
    "items": [
        {
            "label": "falg1",
            "default": true
        },
        {
            "label": "falg2",
            "default": false
        },
        {
            "label": "falg3",
            "default": true
        }
    ]
}
```
