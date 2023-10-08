# Minimal GUI

Tuw will read `./gui_definition.json` when launching the executable.  
You can define a GUI in the JSON file.  
  
The following JSON is for a minimal GUI that you can make.  
It has only a button to echo `Hello!`.  

![minimal](https://github.com/matyalatte/tuw/assets/69258547/a47047d4-0b7c-48cf-bf6b-18b62476e71c)

```json
{
    "gui": [
        {
            "label": "Minimal Sample",
            "command": "echo Hello!",
            "components": []
        }
    ]
}
```

You can write a definition of your GUI in `"gui": [{}]`.  

-   `label` is a label of your definition. You can type here as you like.
-   `command` is a command you want to execute when clicking the execution button.
-   `components` is an array of GUI components (e.g. file pickers). `[]` means no components.
