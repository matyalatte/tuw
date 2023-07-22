# Putting Components

You can put a text box in the GUI.  

![text_box](https://github.com/matyalatte/Simple-Command-Runner/assets/69258547/4e42ae56-1525-4dea-8062-47f8e4c0f8de)  

```json
{
    "gui": [
        {
            "label": "Text Box Sample",
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
    ]
}
```

## Components

`compoents` is an array of GUI components (e.g. file pickers).  
Each component should be defined as a dictionary.  

-   `type` is component type. `text` is for a text box.
-   `label` is a string written above the text box.

You can see [more examples](../../#2-components) for the other component types.

## "command"

Inputs of components will be injected into `command` when executing the command.  
You can specify where they should be injected with `%*%`.  
In the example, the value of the text box will be injected in `%-%`.  

## IDs

You can also use the [`id`](../../comp_options/id) option to name the components like variables.  

```json
"gui": [
    {
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
]
```
