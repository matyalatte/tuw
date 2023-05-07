# Error Handling

## Error Dialogue

When the executed command print something to `stderr`, Simple Command Runner will show an error dialogue.  

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

## `show_last_line` Option

Your command might not use `stderr` even if they got errors.  
If so, you can use `show_last_line` option.  
It's an option to show the last line from `stdout`.  
You can also use it to show success messages.  

![ShowLastLine](https://user-images.githubusercontent.com/69258547/236607913-31d00dbb-6180-4a99-b981-8c6dec3fb8d8.png)  

```json
{
    "label": "show_last_line Sample",
    "command": "echo Fake Error!",
    "show_last_line": true,
    "components": []
}
```
