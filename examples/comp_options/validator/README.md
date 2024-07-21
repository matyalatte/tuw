# Validators

`validator` is an optional key for components used to validate input strings before execution.
It allows you to specify constraints and custom error messages about string validation.

![validator](https://github.com/matyalatte/tuw/assets/69258547/7231512d-f68e-4908-bf7f-d7a4f0a72213)  

## Constraints

You can use four kinds of constraints.

-   `exist`: Ensures the input path exists.
-   `not_empty`: Requires the input string to be non-empty.
-   `regex`: Validates the input against a regex pattern.
-   `wildcard`: Validates the input against a wildcard pattern.

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
> The regex validator does NOT support some operators like the group `()` operator.
> See [here](https://github.com/matyalatte/tiny-str-match?tab=readme-ov-file#supported-regex-operators
) for a list of supported operators.

## Custom error messages

You can overwrite default error messages using `*_error` options.  

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
