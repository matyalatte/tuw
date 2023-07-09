# Check Boxes

You can use check boxes.  

![check box](https://github.com/matyalatte/Simple-Command-Runner/assets/69258547/79a362fc-e09e-4b94-97b6-eafbf622b407)

```json
"components": [
    {
        "type": "check",
        "label": "checkbox",
        "value": "checked!"
    },
    {
        "type": "check_array",
        "label": "options",
        "items": [
            {
                "label": "flag1",
                "value": " -f1"
            },
            {
                "label": "flag2",
                "value": " -f2"
            },
            {
                "label": "flag3",
                "value": " -f3"
            }
        ]
    }
]
```

-   `check` is a check box. `value` will be inputted into commands if you check the check box.
-   `check_array` is an array of the `check` components.
