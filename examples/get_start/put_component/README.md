# Put Components

You can put a text box in the GUI.  

![text_box](https://github.com/matyalatte/tuw/assets/69258547/0fffa15a-2bae-48b3-be07-947e42175be5)  

```json
{
    "gui": {
        "window_name": "Title here!",
        "command": "echo %entry%",
        "button": "Hello!",
        "components": [
            {
                "type": "text",
                "id": "entry",
                "label": "Type 'Hello!'"
            }
        ]
    }
}
```

## Components

`components` is an array of GUI components (e.g., file pickers).
Each component should have the following keys.  

-   `type`: Component type. `text` is for a text box.
-   [`id`](../../comp_options/id): Component name. You can use it as a variable name in commands.
-   `label`: String written above the text box.

You can see [more examples](../../#2-components) for other component types.

## Command

Tuw injects inputs of components into `command` when executing the command.
You can specify where they should be injected with `%*%`.
In the example, the value of the text box will be injected at `%entry%`.  
