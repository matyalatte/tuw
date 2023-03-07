# Version Checking

There are 2 options to check the tool version.

- `recommended`: Show a warning when the running version is not this value.  
- `minimum_required`: Stop reading json file when the running version is lower than this value.  

> The version string should be of the form `x.x.x`.

![version_error](https://user-images.githubusercontent.com/69258547/223422466-bcda85bb-90e9-4782-9d94-8aeb443e2441.png)

```json
{
    "recommended": "1.1.0",
    "minimum_required": "1.0.0",
    "gui": [
        {
            "label": "You can't see this GUI.",
            "command": "echo Hello!",
            "components": []
        }
    ]
}
```
