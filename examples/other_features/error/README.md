# Error Handling

## Error Dialogue

When the executed command outputs something to `stderr`, Tuw will display an error dialogue.  

![Error](https://github.com/matyalatte/tuw/assets/69258547/a023c839-aafc-4beb-a702-596e2ea3a454)  

```json
{
    "gui": [
        {
            "label": "Error Sample",
            "command": "notcommand",
            "components": []
        }
    ]
}
```

## Exit Code Checking

If your command does not use `stderr` for error handling, you can use the `check_exit_code` option.  
This option displays an error dialogue when the exit code is not 0.  
You can also customize the success code using the `exit_success` option.  

```json
{
    "label": "check_exit_code Sample",
    "command": "echo exit code is zero.",
    "check_exit_code": true,
    "exit_success": 10,
    "components": []
}
```

## `show_last_line` Option

If your command neither uses `stderr` nor exit codes for error handling, you can use the `show_last_line` option.  
It displays the last line from `stdout`.  
When `check_exit_code` is enabled, it can also show the last line in the error dialogue.  

![ShowLastLine](https://github.com/matyalatte/tuw/assets/69258547/3915740e-37b3-496c-9e51-9f80334725ae)  

```json
{
    "label": "show_last_line Sample",
    "command": "echo Fake Error!",
    "show_last_line": true,
    "components": []
}
```
