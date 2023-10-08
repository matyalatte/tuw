# Path Pickers

You can use file pickers and folder pickers.  
Of course, you can drop files on the pickers to specifiy the paths.  

![PathPikcers](https://github.com/matyalatte/tuw/assets/69258547/47bf541f-7ac4-465b-8bff-512c48d9d2a9)

```json
{
    "gui": [
        {
            "label": "Copy file",
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
    ]
}
```

## Components

`compoents` is an array of GUI components (e.g. file pickers).  
Each component should be defined as a dictionary.  

-   `type` is component type. `file` for a file picker, and `folder` for a dir picker.
-   `label` is a string written above the text box of the picker.
-   `extension` is an optional key for file pickers. It's wildcard for file extensions. Use the same syntax as for [wxWidget's wildcards](https://docs.wxwidgets.org/3.0/classwx_file_dialog.html).
-   `add_quotes` is an optional key for components. It will add quotes (`""`) to the input strings.

## Command

Inputs of components will be injected into `command` when executing the command.  
You can specify where they should be injected with `%*%`.  
In the example, the file path will be injected in `%foo%`, and the folder path will be in `%bar%`.  
You can also use [`id`](../../comp_options/id) option to name the components like variables.
