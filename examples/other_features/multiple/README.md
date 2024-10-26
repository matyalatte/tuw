# Multiple Definitions

`"gui"` can be an array of gui definitions.
Users can select one of these definitions from the `Menu` in the executable.
You can also use `"label"` to modify strings that are displayed in the menu bar.  

![advanced](https://github.com/matyalatte/tuw/assets/69258547/956be42e-6931-4b71-ae3c-180103a93714)  

```json
{
    "gui": [
        {
            "label": "Sample GUI",
            "command": "echo file: %-%",
            "button": "Echo!",
            "components": [
                {
                    "type": "file",
                    "label": "Some file path"
                }
            ]
        },
        {
            "label": "Sample GUI2",
            "command": "echo text_box: %-%",
            "button": "Echo!",
            "components": [
                {
                    "type": "text",
                    "label": "Some text"
                }
            ]
        }
    ]
}
```
