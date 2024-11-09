# Path Pickers

You can use file pickers and folder pickers.  
Of course, you can drop files on the pickers to specify the paths.  

![PathPikcers](https://github.com/matyalatte/tuw/assets/69258547/47bf541f-7ac4-465b-8bff-512c48d9d2a9)

```json
{
    "gui": {
        "window_name": "Picker sample",
        "command": "copy %foo% %bar%",
        "button": "copy",
        "components": [
            {
                "type": "file",
                "label": "PNG file you want to copy",
                "extension": "PNG files (*.png)|*.png",
                "add_quotes": true
            },
            {
                "type": "folder",
                "label": "Output path",
                "add_quotes": true
            }
        ]
    }
}
```

## Components

`components` is an array of GUI components (e.g., file pickers).  
Each component should be defined as a dictionary.  

-   `type` is the component type. `file` for a file picker, and `folder` for a directory picker.
-   `label` is the string written above the text box of the picker.
-   `extension` is an optional key for file pickers. It specifies the wildcard for file extensions. Use the same syntax as for [wxWidget's wildcards](https://docs.wxwidgets.org/3.0/classwx_file_dialog.html).
-   `add_quotes` is an optional key for components. It will add quotes (`""`) to the input strings.

## Command

Inputs of components will be injected into `command` when executing the command.  
You can specify where they should be injected with `%*%`.  
In the example, the file path will be injected at `%foo%`, and the folder path will be at `%bar%`.  
You can also use the [`id`](../../comp_options/id) option to name the components like variables.
