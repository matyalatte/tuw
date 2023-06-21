#!/bin/bash
# Downloads wxWidgets.

# wxWidgets version is defined in ./Simple-Command-Runner/WX_VERSION.txt
wx_version="$(cat $(dirname "$0")/../WX_VERSION.txt)"

pushd ~/
    wget https://github.com/wxWidgets/wxWidgets/releases/download/v"$wx_version"/wxWidgets-"$wx_version".tar.bz2
    tar -xvjof wxWidgets-"$wx_version".tar.bz2
    rm -f wxWidgets-"$wx_version".tar.bz2

    if [[ "$OSTYPE" == "darwin"* ]]; then
        cd wxWidgets-"$wx_version"/src/osx
        old_code="AsString(wxLocale::GetSystemEncoding())"
        new_code="AsString()"
        sed -i '' -e "s/${old_code}/${new_code}/g" cocoa/utils.mm
        sed -i '' -e "s/${old_code}/${new_code}/g" carbon/app.cpp
        old_code=", wxLocale::GetSystemEncoding()"
        sed -i '' -e "s/${old_code}//g" carbon/graphics.cpp
        old_code="include \"wx/scopedarray.h\""
        new_code="include \"wx/scopedarray.h\""\\$'\n'"#include \"wx/scopedptr.h\""
        sed -i '' -e "s|${old_code}|${new_code}|g" carbon/dataobj.cpp
        old_code="include \"wx/process.h\""
        new_code="include \"wx/process.h\""\\$'\n'"#include \"wx/scopedptr.h\""
        sed -i '' -e "s|${old_code}|${new_code}|g" core/utilsexc_cf.cpp

        cd ../common
        old_code="ifdef __WXOSX__"
        new_code="ifdef __WXOSX__"\\$'\n'"#if wxUSE_INTL"
        sed -i '' -e "s/${old_code}/${new_code}/g" datetimefmt.cpp
        old_code="(\"%X\", wxUILocale::GetCurrent().GetInfo(wxLOCALE_TIME_FMT));"
        new_code="(\"%X\", wxUILocale::GetCurrent().GetInfo(wxLOCALE_TIME_FMT));"\\$'\n'"#endif"
        sed -i '' -e "s/${old_code}/${new_code}/g" datetimefmt.cpp

        cd ../../include/wx
        old_code="define wxGETTEXT_IN_CONTEXT(c, s) *(s)"
        new_code="define wxGETTEXT_IN_CONTEXT(c, s) wxString(s)"
        sed -i '' -e "s/${old_code}/${new_code}/g" translation.h
    fi
popd
