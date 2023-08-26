#pragma once
#include <string>

#ifdef _WIN32
std::string UTF16toUTF8(const wchar_t* str);
std::wstring UTF8toUTF16(const char* str);
void PrintFmt(const char* fmt, ...);
#else
#define PrintFmt(fmt, ...) printf(fmt, __VA_ARGS__)
#endif
