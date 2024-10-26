# Optional Components

`optional` is an option to ignore `"add_quotes"`, `"prefix"`, `"suffix"`, and `"validator"` when a text box is empty.

![optional](https://github.com/user-attachments/assets/a7c94cc7-cb84-4ca9-a1a1-ed1e6babc18e)

```json
{
    "gui": {
        "command": "echo %-%",
        "components": [
            {
                "type": "text",
                "label": "Text box",
                "optional": true,
                "prefix": "-pre=",
                "suffix": " -suf",
                "validator": {
                    "regex": ".+"
                }
            }
        ]
    }
}
```
