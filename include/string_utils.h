#pragma once
#include <string>
#include "env_utils.h"

inline std::string envuStr(char *cstr) {
    if (cstr == NULL)
        return "";
    std::string str = cstr;
    envuFree(cstr);
    return str;
}

uint32_t Fnv1Hash32(const std::string& str);

// Efficient way to concat two or three char* strings.
// str2 can be int, size_t, or uint32_t as well. It will be converted to string internally.
template<typename T>
std::string ConcatCStrings(const char* str1, T str2, const char* str3);

template<typename T>
inline std::string ConcatCStrings(const char* str1, T str2) {
    return ConcatCStrings(str1, str2, nullptr);
}

#ifdef _WIN32
std::string UTF16toUTF8(const wchar_t* str);
std::wstring UTF8toUTF16(const char* str);
void PrintFmt(const char* fmt, ...);
void EnableCSI();
#elif defined(__TUW_UNIX__)
void SetLogEntry(void* log_entry);
void PrintFmt(const char* fmt, ...);
#else
#define PrintFmt(...) printf(__VA_ARGS__)
#endif
