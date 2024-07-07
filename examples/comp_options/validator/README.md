# Validator

`validator` is an optional key for components to validate input strings before execution.
You can specify some constraints in the value of the `validator` option.
Then, each component can show error messages before executing commands.  

![validator](https://github.com/matyalatte/tuw/assets/69258547/7231512d-f68e-4908-bf7f-d7a4f0a72213)  


## Constraints

There are four kinds of constraints you can use.

-   `exist`: Input path should exist.
-   `not_empty`: Input string should not be empty.
-   `regex`: Input should match a regex pattern.
-   `wildcard`: Input should match a wildcard pattern.

```json
{
    "type": "file",
    "label": "Some file path",
    "validator": {
        "exist": true,
        "not_empty": true,
        "regex": "^[\\w]+\\.png$",
        "wildcard": "*.png"
    }
}
```

> [!WARNING]
> Regex validator does NOT support some operators like the group `()` operator.
> See [here](https://github.com/matyalatte/tiny-str-match?tab=readme-ov-file#supported-regex-operators
) for the list of supported operators.

## Custom error messages

You can overwrite the error messages with `*_error` options.  

![custom_error](https://github.com/matyalatte/tuw/assets/69258547/87244bbd-9b3a-4205-a1d3-ff1ee899c8d6)


```json
{
    "type": "file",
    "label": "Some file path",
    "validator": {
        "exist": true,
        "exist_error": "File does not exist!!!",
        "not_empty": true,
        "not_empty_error": "Type something...",
        "regex": "^[\\w]+\\.png$",
        "regex_error": "Only alphanumeric characters are supported.",
        "wildcard": "*.png",
        "wildcard_error": "Not PNG!"
    }
}
```
