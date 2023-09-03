#include "string_utils.h"

#ifdef _WIN32
#include "windows/uipriv_windows.hpp"
#else
#include <cstring>
#endif

int StartsWith(const char *str, const char* pattern) {
    return strncmp(str, pattern, strlen(pattern));
}

#ifdef _WIN32
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

    char* buf = reinterpret_cast<char *>(uiprivAlloc(n * sizeof (char), "char[]"));
    vsprintf_s(buf, n, fmt, va);
    va_end(va);

    WCHAR* wfmt = toUTF16(buf);
    wprintf(wfmt);

    uiprivFree(buf);
    uiprivFree(wfmt);
}

#elif defined(__linux__)
#include <stdarg.h>
#include "ui.h"

class Logger {
 private:
    uiMultilineEntry* m_log_entry;
    std::string m_log_buffer;

 public:
    Logger() {
        m_log_entry = NULL;
        m_log_buffer = "";
    }
    ~Logger() {}

    void SetLogEntry(void* log_entry) {
        m_log_entry = static_cast<uiMultilineEntry*>(log_entry);
        if (m_log_entry != NULL) {
            uiMultilineEntrySetText(m_log_entry, m_log_buffer.c_str());
            m_log_buffer = "";
        }
    }

    void Log(const char* str) {
        if (m_log_entry == NULL) {
            m_log_buffer += str;
        } else {
            uiMultilineEntryAppend(m_log_entry, str);
        }
    }
};

Logger g_logger = Logger();

void SetLogEntry(void* log_entry) {
    g_logger.SetLogEntry(log_entry);
}

void PrintFmt(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    va_list va2;
    va_copy(va2, va);
    size_t size = vsnprintf(NULL, 0, fmt, va2);
    va_end(va2);
    char* buf = new char[size + 1];
    buf[size] = 0;
    vsnprintf(buf, size + 1, fmt, va);
    printf("%s", buf);
    g_logger.Log(buf);
    delete[] buf;
    va_end(va);
}
#endif

