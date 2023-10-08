# Error Handling

## Error Dialogue

When the executed command output something to `stderr`, Tuw will show an error dialogue.  

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

Your command might not use `stderr` for error handling.  
If so, you can use `check_exit_code` option.  
It's an option to show an error dialogue when the exit code is not 0.  
Also, you can change the success code with `exit_success` option.  

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

Your command might not use `stderr` nor exit codes.  
If so, you can use `show_last_line` option.  
It's an option to show the last line from `stdout`.  
When `check_exit_code` is on, it can also show the last line on the error dialogue.  

![ShowLastLine](https://github.com/matyalatte/tuw/assets/69258547/3915740e-37b3-496c-9e51-9f80334725ae)  

```json
{
    "label": "show_last_line Sample",
    "command": "echo Fake Error!",
    "show_last_line": true,
    "components": []
}
```
