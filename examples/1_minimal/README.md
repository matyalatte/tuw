# Minimal GUI

This is a minimal GUI you can make.  
It has only a button to echo `Hello!`.  

![minimal](https://user-images.githubusercontent.com/69258547/192091288-72b69003-eafa-41ef-8167-5d0e0fe010d9.png)

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

You can write definition of your GUI in `"gui": [{}]`.  

-   `label` is a label of your definition. You can type here as you like.
-   `command` is a command you want to execute when clicking the execution button.
-   `compoents` is an array of GUI components (e.g. file pickers). `[]` means no components.
