# Choices

You can use a combo box or radio buttons to let users select one of choices.

![choices](https://github.com/matyalatte/tuw/assets/69258547/3e178f69-3db7-4c43-b08f-d962912f096a)

```json
"components": [
    {
        "type": "combo",
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
                "label": "three",
                "value": "3"
            }
        ]
    },
    {
        "type": "radio",
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
                "label": "three",
                "value": "3"
            }
        ]
    }
]
```

`combo` is a type for a combo box, and `radio` is for radio buttons.  
They require `items` to specify choices.  
`label` is the string that will be displayed in GUI.  
`value` is the actual value that will be injected in commands.  

