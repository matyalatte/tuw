#pragma once
#include <string>

int StartsWith(const char *str, const char* pattern);

#ifdef _WIN32
std::string UTF16toUTF8(const wchar_t* str);
std::wstring UTF8toUTF16(const char* str);
void PrintFmt(const char* fmt, ...);
#elif defined(__linux__)
void SetLogEntry(void* log_entry);
void PrintFmt(const char* fmt, ...);
#else
#define PrintFmt(...) printf(__VA_ARGS__)
#endif
