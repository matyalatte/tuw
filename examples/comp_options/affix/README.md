# Affixes

`prefix` and `suffix` can append strings to user inputs.

![Affix](https://github.com/user-attachments/assets/15910d56-26b5-4f90-8294-602bec9383d3)

```json
{
    "gui": [
        {
            "label": "Affix example",
            "command": "echo %-%",
            "components": [
                {
                    "type": "text",
                    "label": "Text box",
                    "prefix": "-pre=",
                    "suffix": " -suf"
                }
            ]
        }
    ]
}
```
