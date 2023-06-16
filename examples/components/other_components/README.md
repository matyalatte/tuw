# Other Components

There are more components you can use.

![others](https://user-images.githubusercontent.com/69258547/171445898-7b696c95-a081-4a3a-aa66-71b8265f64d8.png)

```json
"components": [
    {
        "type": "static_text",
        "label": "This is a sample GUI. Edit 'gui_definition.json' for your scripts."
    },
    {
        "type": "choice",
        "label": "num",
        "items": [
            {
                "label": "one",
                "value": "1"
            },
            {
                "label": "two",
                "value": "2"
            },
            {
                "label": "three",
                "value": "3"
            }
        ],
        "default": 1
    },
    {
        "type": "check",
        "label": "checkbox",
        "value": "checked!",
        "default": true
    },
    {
        "type": "text",
        "label": "Some text"
    }
]
```

-   `static_text` is a string component. You don't need to put `%*%` in command for this component.
-   `choice` is a combo box. `value` will be inputted to command.
-   `check` is a check box. `value` will be inputted to command if you check the check box.
-   `text` is a text box.
