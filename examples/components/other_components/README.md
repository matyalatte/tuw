# Other Components

There are more components you can use.

![others](https://github.com/matyalatte/Simple-Command-Runner/assets/69258547/3e178f69-3db7-4c43-b08f-d962912f096a)

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
        ]
    },
    {
        "type": "text",
        "label": "Some text"
    }
]
```

-   `static_text` is a string component. You don't need to put `%*%` in command for this component.
-   `choice` is a combo box. `value` will be inputted to command.
-   `text` is a text box.
