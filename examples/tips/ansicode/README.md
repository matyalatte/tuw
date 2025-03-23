# ANSI Codes

Tuw supports many of the ANSI escape sequences. You can use colors, font styles, and carriage returns to output strings.  
You can also see the list of supported escape sequences here.  
[gtk-ansi-parser: Supported Escape Sequences](https://matyalatte.github.io/gtk-ansi-parser/md_docs__a_n_s_icodes.html)  

![ansicode](https://github.com/user-attachments/assets/270f7236-2665-46f0-ba55-a4f570f6378a)

```json
{
    "gui": {
        "label": "ANSI escape sequences",
        "command": "bash ansicode.sh",
        "command_win": "ansicode.bat",
        "components": []
    }
}
```

```bash
#!/bin/bash
# ansicode.sh

for y in {0..15}; do
    for x in {0..15}; do
        color=$((y * 16 + x))
        printf "\033[48;5;%dm  \033[0m" "$color"
    done
    echo ""
done

echo -e "\033[1mBold\033[0m\033[3mItalic\033[0m\033[4mUnderline\033[0m"
echo -e "\033[31;5mRedBlink\033[0m\033[31;2mRedFaint\033[0m"
echo -e "\033[9mStrikethrough\033[0m\033[7mReverse\033[0m\033[8mConceal\033[0m"
echo -e "0123456789abcdefghjklmn\rCarriage returns!"
```

```bat
@echo off
REM ansicode.bat

setlocal EnableDelayedExpansion
for /f %%i in ('cmd /k prompt $e^<nul') do set ESC=%%i

for /L %%y in (0,1,15) do (
    for /L %%x in (0,1,15) do (
        set /a color=%%y * 16 + %%x
        <nul set /p=%ESC%[48;5;!color!m  %ESC%[0m
    )
    echo.
)

echo %ESC%[1mBold%ESC%[0m%ESC%[3mItalic%ESC%[0m%ESC%[4mUnderline%ESC%[0m
echo %ESC%[31;5mRedBlink%ESC%[0m%ESC%[31;2mRedFaint%ESC%[0m
echo %ESC%[9mStrikethrough%ESC%[0m%ESC%[7mReverse%ESC%[0m%ESC%[8mConceal%ESC%[0m
echo 0123456789abcdefghjklmn%ESC%[1K%ESC%[0GCarriage returns!
```
