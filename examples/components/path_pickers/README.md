# Path Pickers

You can use file pickers and folder pickers.  
Of course, you can drop files on the pickers to specify the paths.  

![PathPikcers](https://github.com/matyalatte/tuw/assets/69258547/47bf541f-7ac4-465b-8bff-512c48d9d2a9)

```json
{
    "gui": {
        "window_name": "Picker sample",
        "command": "copy %img% %out%",
        "button": "copy",
        "components": [
            {
                "type": "file",
                "id": "img",
                "label": "Image file you want to copy",
                "extension": "BMP and GIF files (*.bmp;*.gif)|*.bmp;*.gif|PNG files (*.png)|*.png",
                "add_quotes": true
            },
            {
                "type": "folder",
                "id": "out",
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
-   `extension` is an option for file pickers. It specifies the wildcard for file extensions. Use the same syntax as for [WinAPI's filter string](https://learn.microsoft.com/en-us/dotnet/api/microsoft.win32.filedialog.filter).
-   `add_quotes` is an option for components. It adds quotes (`""`) to the input strings.

## Command

Tuw injects inputs of components into `command` when executing the command.
You can specify where they should be injected with `%*%`.
In the example, the file path will be injected at `%img%`, and the folder path will be at `%out%`.  

## Save Dialog

File pickers include a `use_save_dialog` option that allows users to select a new file through a save dialog.

```json
{
    "gui": {
        "window_name": "Save dialog sample",
        "command": "echo test > %out%",
        "components": [
            {
                "type": "file",
                "id": "out",
                "label": "Output file",
                "add_quotes": true,
                "use_save_dialog": true
            }
        ]
    }
}
```
