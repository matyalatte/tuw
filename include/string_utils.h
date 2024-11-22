#pragma once
#include <cstdio>
#include "env_utils.h"

enum StringError : int {
    STR_OK = 0,
    STR_ALLOCATION_ERROR,  // Failed to allocate memory for string.
    STR_BOUNDARY_ERROR,  // Accessed out-of-bounds with substr or [].
    STR_FORMAT_ERROR,  // Failed to convert number to string.
    STR_ERROR_MAX,
};

// Returns the error status for tuwString.
StringError GetStringError() noexcept;

// Set STR_OK to the error status.
void ClearStringError() noexcept;

// String class that doesn't raise std errors.
// It works without any crashes even when it got a memory allocation error.
// But you have to check GetStringError() after string allocations
// or it might have an unexpected value (an empty string).
class tuwString {
 private:
    char* m_str;
    size_t m_size;
    void alloc_cstr(const char* str, size_t size) noexcept;
    void append(const char* str, size_t size) noexcept;

 public:
    tuwString() noexcept : m_str(nullptr), m_size(0) {}
    tuwString(const char* str) noexcept;  // NOLINT(runtime/explicit)
    tuwString(const char* str, size_t size) noexcept;
    tuwString(const tuwString& str) noexcept;
    tuwString(tuwString&& str) noexcept;

    // This allocates null bytes to use them as a buffer.
    explicit tuwString(size_t size) noexcept;

    ~tuwString() noexcept { clear(); }
    size_t length() const noexcept { return m_size; }
    size_t size() const noexcept { return m_size; }

    bool empty() const noexcept {
        return !m_str || m_str[0] == '\0' || m_size == 0;
    }

    // Returns an immutable C string. This can't be null.
    const char* c_str() const noexcept {
        if (m_str)
            return m_str;
        return "";
    }

    // Returns a pointer to the actual buffer.
    char* data() const noexcept { return m_str; }

    void clear() noexcept {
        if (m_str)
            free(m_str);
        m_str = nullptr;
        m_size = 0;
    }

    const char& operator[](size_t id) const noexcept;

    tuwString& operator=(const char* str) noexcept;
    tuwString& operator=(const tuwString& str) noexcept;
    tuwString& operator=(tuwString&& str) noexcept;

    tuwString& operator+=(const char* str) noexcept;
    tuwString& operator+=(const tuwString& str) noexcept;

    tuwString operator+(const char* str) const noexcept;
    tuwString operator+(const tuwString& str) const noexcept;

    // You can append numbers as strings
    tuwString operator+(int num) const noexcept;
    tuwString operator+(size_t num) const noexcept;
    tuwString operator+(uint32_t num) const noexcept;

    bool operator==(const char* str) const noexcept;
    bool operator==(const tuwString& str) const noexcept;
    inline bool operator!=(const char* str) const noexcept {
        return !(*this == str);
    }
    inline bool operator!=(const tuwString& str) const noexcept {
        return !(*this == str);
    }

    const char* begin() const noexcept {
        return c_str();
    }

    const char* end() const noexcept {
        return c_str() + m_size;
    }

    static const size_t npos;
    size_t find(const char c) const noexcept;
    size_t find(const char* str) const noexcept;
    inline size_t find(const tuwString& str) const noexcept {
        return find(str.c_str());
    }

    void push_back(const char c) noexcept {
        this->append(&c, 1);
    }

    tuwString substr(size_t start, size_t size) const noexcept;
};

tuwString operator+(const char* str1, const tuwString& str2) noexcept;

inline bool operator==(const char* str1, const tuwString& str2) noexcept {
    return str2 == str1;
}

inline bool operator!=(const char* str1, const tuwString& str2) noexcept {
    return str2 != str1;
}

// Returns only the last line and removes trailing line feeds (\n and \r.)
tuwString GetLastLine(const tuwString& str) noexcept;

class tuwWstring {
 private:
    wchar_t* m_str;
    size_t m_size;

 public:
    tuwWstring(const wchar_t* str) noexcept;  // NOLINT(runtime/explicit)
    ~tuwWstring() noexcept { clear(); }
    size_t length() const noexcept { return m_size; }
    size_t size() const noexcept { return m_size; }
    bool empty() const noexcept {
        return !m_str || m_str[0] == L'\0' || m_size == 0;
    }

    const wchar_t* c_str() const noexcept {
        if (m_str)
            return m_str;
        return L"";
    }

    void clear() noexcept {
        if (m_str)
            free(m_str);
        m_str = nullptr;
        m_size = 0;
    }
};

inline tuwString envuStr(char *cstr) noexcept {
    if (cstr == NULL)
        return "";
    tuwString str = cstr;
    envuFree(cstr);
    return str;
}

uint32_t Fnv1Hash32(const tuwString& str) noexcept;

#ifdef _WIN32
tuwString UTF16toUTF8(const wchar_t* str) noexcept;
tuwWstring UTF8toUTF16(const char* str) noexcept;
void FprintFmt(FILE* out, const char* fmt, ...) noexcept;
void EnableCSI() noexcept;
#elif defined(__TUW_UNIX__)
void SetLogEntry(void* log_entry) noexcept;
void FprintFmt(FILE* out, const char* fmt, ...) noexcept;
#else
#define FprintFmt(...) fprintf(__VA_ARGS__)
#endif

#define PrintFmt(...) FprintFmt(stdout, __VA_ARGS__)
