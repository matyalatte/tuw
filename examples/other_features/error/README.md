# Error Handling

## Error Dialogue

When the executed command output something to `stderr`, Simple Command Runner will show an error dialogue.  

![Error](https://user-images.githubusercontent.com/69258547/236607612-183a72d9-a7f3-4006-b545-8d2f5c9ca890.png)  

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

![ShowLastLine](https://user-images.githubusercontent.com/69258547/236607913-31d00dbb-6180-4a99-b981-8c6dec3fb8d8.png)  

```json
{
    "label": "show_last_line Sample",
    "command": "echo Fake Error!",
    "show_last_line": true,
    "components": []
}
```
