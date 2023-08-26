#pragma once
#include <string>

#ifdef _WIN32
#include "windows/uipriv_windows.hpp"
#endif

#ifdef _WIN32
//template <typename ... Args>
inline void PrintFmt(const char* fmt, ...) {

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
#else
#define PrintFmt(fmt, ...) printf(fmt, __VA_ARGS__)
#endif
