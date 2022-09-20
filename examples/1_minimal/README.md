# Minimal GUI
This is a minimal GUI you can make.<br>
It has only a button to echo `Hello!`.<br>

![minimal](https://user-images.githubusercontent.com/69258547/166092954-21dac225-07f8-43d2-b1d7-6df7c59625e6.png)

```
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

You can write definition of your GUI in `"gui": [{}]`.<br>
- `label` is a label of your definition. You can type here as you like.
- `command` is a command you want to execute when clicking the execution button.
- `compoents` is an array of GUI components (e.g. file pickers). `[]` means no components.
