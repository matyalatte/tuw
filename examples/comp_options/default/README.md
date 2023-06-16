# Default Values

`default` is an optional key for components to set default values.  

![default](https://user-images.githubusercontent.com/69258547/236602766-5f5394c5-c65f-4574-a772-f5720f075148.png)  

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
