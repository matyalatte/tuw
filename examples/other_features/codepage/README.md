# UTF-8 Outputs on Windows

Tuw expects user's default locale for stdout on Windows. If you want to use UTF-8 outputs on Windows, you should use `"codepage": "utf8"` (or `"codepage": "utf-8"`) as an option.  

```json
{
    "gui": [
        {
            "label": "Use UTF-8 outputs on Windows",
            "command": "powershell -ExecutionPolicy Bypass -File \"utf.ps1\"",
            "codepage": "utf8",
            "components": []
        }
    ]
}
```

```ps1
# utf.ps1
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
Write-Output "あいうえお"
```
