# JSON with Comments

> [!WARNING]
> Released builds do not support this feature yet.

Tuw supports the [JSON with Comments](https://code.visualstudio.com/docs/languages/json#_json-with-comments) format. It allows C-style comments (`//` and `/**/`) and trailing commas in addition to the standard JSON format. You can also use `.jsonc` as a file extension to let code editors know that it uses the extended JSON format.  

```jsonc
{
    // Single-line comment
    /*
     * Multi-line comment
     */
    "gui": [
        {
            "label": "JSON with Comments",
            "command": "echo Hello!",
            "components": [],
            // You can put a trailing comma after the last element.
            "": "",
        }
    ]
}
```
