# Choices

You can use a combo box or radio buttons to let users select one of the choices.  

![choices](https://github.com/matyalatte/tuw/assets/69258547/7dc186dc-685f-4258-8f74-a78f75e1ae26)  

```json
"components": [
    {
        "type": "combo",
        "id": "cb",
        "label": "Combo box",
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
                "label": "3"
            }
        ]
    },
    {
        "type": "radio",
        "id": "rb",
        "label": "Radio buttons",
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
                "label": "3"
            }
        ]
    }
]
```

`combo` is the type for a combo box, and `radio` is for radio buttons.
They require `items` to specify choices.
You can also use `value` to specify the actual value that is injected into commands.  
