#pragma once
#include <cstdio>
#include "env_utils.h"
#include "noex/string.hpp"

// Returns only the last line and removes trailing line feeds (\n and \r.)
noex::string GetLastLine(const noex::string& str) noexcept;

// Convert allocated string with env_utils.h into noex::string
noex::string envuStr(char *cstr) noexcept;

uint32_t Fnv1Hash32(const noex::string& str) noexcept;

#ifdef _WIN32
noex::string ANSItoUTF8(const noex::string& str) noexcept;
#else
#define ANSItoUTF8(str) str
#endif

#ifdef _WIN32
noex::string UTF16toUTF8(const wchar_t* str) noexcept;
noex::wstring UTF8toUTF16(const char* str) noexcept;
void FprintFmt(FILE* out, const char* fmt, ...) noexcept;
void EnableCSI() noexcept;
#elif defined(__TUW_UNIX__)
void SetLogEntry(void* log_entry) noexcept;
void Log(const char* str) noexcept;
void FprintFmt(FILE* out, const char* fmt, ...) noexcept;
#else
#define FprintFmt(...) fprintf(__VA_ARGS__)
#endif

// Note: PrintFmt() and FprintFmt() only support UTF-8 strings.
//       Some localized versions of Windows might require ANSItoUTF8()
//       before printing strings.
#define PrintFmt(...) FprintFmt(stdout, __VA_ARGS__)
