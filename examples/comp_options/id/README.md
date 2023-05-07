# ID

`id` is an optional key for components.  
Its value will be used as a variable name in commands.  
  
Here is an example.  

```json
{
    "label": "Sample GUI",
    "command": "echo y: %something% & echo x: %x% & echo z: %bruh% & echo x: %x%",
    "button": "Echo!",
    "components": [
        {
            "type": "static_text",
            "label": "Sample GUI for \"id\" option."
        },
        {
            "type": "text",
            "label": "Value of x",
            "id": "x"
        },
        {
            "type": "text",
            "label": "Value of y"
        },
        {
            "type": "text",
            "label": "Value of z"
        }
    ]
}
```

In this case, the value of x will be injected into both of `%x%` parts.  
Because `x` is an ID for the first text box.  
The value of y will be injected into `%something%`.  
Because `something` is not any ID for components and the second text box is the first component that have no ID.  
The value of z will be injected into `%bruh%`.  
Because the last text box is the second component that have no ID.  
