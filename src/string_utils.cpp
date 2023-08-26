#include "string_utils.h"

#ifdef _WIN32
#include "windows/uipriv_windows.hpp"

std::string UTF16toUTF8(const wchar_t* str) {
    char* uchar = toUTF8(str);
    std::string ustr = uchar;
    uiprivFree(uchar);
    return ustr;
}

std::wstring UTF8toUTF16(const char* str) {
    wchar_t* widechar = toUTF16(str);
    std::wstring wstr = widechar;
    uiprivFree(widechar);
    return wstr;
}

void PrintFmt(const char* fmt, ...) {

    va_list va;
    va_start(va, fmt);

    va_list va2;
    va_copy(va2, va);
    size_t n = _vscprintf(fmt, va2);
    va_end(va2);
    n++;

    char* buf = (char *) uiprivAlloc(n * sizeof (char), "char[]");
    vsprintf_s(buf, n, fmt, va);
    va_end(va);

    WCHAR* wfmt = toUTF16(buf);
    wprintf(wfmt);

    uiprivFree(buf);
    uiprivFree(wfmt);
}
#endif

