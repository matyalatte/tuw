# Put Components

You can put a text box in the GUI.  

![text_box](https://github.com/matyalatte/tuw/assets/69258547/0fffa15a-2bae-48b3-be07-947e42175be5)  

```json
{
    "gui": {
        "window_name": "Title here!",
        "command": "echo %-%",
        "button": "Hello!",
        "components": [
            {
                "type": "text",
                "label": "Type 'Hello!'"
            }
        ]
    }
}
```

## Components

`components` is an array of GUI components (e.g., file pickers).  
Each component should be defined as a dictionary.  

-   `type` is the component type. `text` is for a text box.
-   `label` is the string written above the text box.

You can see [more examples](../../#2-components) for other component types.

## Command

Inputs of components will be injected into the value of `command` when executing the command.  
You can specify where they should be injected with `%*%`.  
In the example, the value of the text box will be injected at `%-%`.  

## IDs

You can also use the [`id`](../../comp_options/id) option to name the components like variables.  

```json
"gui": {
    "label": "Text Box Sample",
    "window_name": "Title here!",
    "command": "echo %foo%, %foo%",
    "button": "Hello!",
    "components": [
        {
            "type": "text",
            "label": "Type 'Hello!'",
            "id": "foo"
        }
    ]
}
```
