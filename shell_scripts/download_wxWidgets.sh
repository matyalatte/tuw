#!/bin/bash
# Downloads wxWidgets.

# wxWidgets version is defined in ./Simple-Command-Runner/WX_VERSION.txt
wx_version="$(cat $(dirname "$0")/../WX_VERSION.txt)"

pushd ~/
    wget https://github.com/wxWidgets/wxWidgets/releases/download/v"$wx_version"/wxWidgets-"$wx_version".tar.bz2
    tar -xvjof wxWidgets-"$wx_version".tar.bz2
    rm -f wxWidgets-"$wx_version".tar.bz2
    cd wxWidgets-"$wx_version"/src/common
    old_code="USE_PROTOCOL(wxFileProto)"
    new_code="#if wxUSE_PROTOCOL_FILE"\\$'\n'"USE_PROTOCOL(wxFileProto)"\\$'\n'"#endif"
    if [[ "$OSTYPE" == "darwin"* ]]; then
        sed -i '' -e "s/${old_code}/${new_code}/g" url.cpp
    else
        sed -i -e "s/${old_code}/${new_code}/g" url.cpp
    fi
popd
