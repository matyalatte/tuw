# Pickers
You can use file pickers and folder pickers.

![Pikcers](https://user-images.githubusercontent.com/69258547/171440880-5948a7f0-5e26-4c38-ab95-8c6daaf67f93.png)

```
{
    "gui": [
        {
            "label": "Copy file",
            "window_name": "Picker sample",
            "command": "copy %file% %out_path%",
            "button": "copy",
            "components": [
                {
                    "type": "file",
                    "label": "A file you want to copy",
                    "extension": "any files | *",
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

`window_name` is an optional key to rename the window title of `SimpleCommandRunner.exe`.<br>
`compoents` is an array of GUI components.<br>
Each component will be defined as a dictionary.<br>
`type` is component type. `file` is for file picker and `folder` is for dir picker.<br>
`label` is a string written above the textbox of picker.<br>
`extension` is an optional key for file picker. It's wildcard for file pickers. Use the same syntax as for [wxWidget's wildcards](https://docs.wxwidgets.org/3.0/classwx_file_dialog.html).<br>
`add_quotes` is an optional key for components. It will add quotes to the input strings.<br>
<br>
Inputs of components will be injected into `command` when executing the command.<br>
You can specifiy where you inject the inputs with `%*%`.<br>
In the sample, file path will be injected in `%file%`, and folder path will be in %out_path%.
