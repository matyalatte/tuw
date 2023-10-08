# Check Boxes

You can use check boxes.  

![check box](https://github.com/matyalatte/tuw/assets/69258547/205ffb2e-355b-4619-a132-c2a2293c18f7)

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
