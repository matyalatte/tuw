#!/bin/bash
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
