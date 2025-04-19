@echo off
setlocal EnableDelayedExpansion

REM Enable ANSI escape codes
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
