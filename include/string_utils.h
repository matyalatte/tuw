#pragma once
#include "env_utils.h"

// String class that doesn't raise std errors.
class tuwString {
 private:
    char* m_str;
    size_t m_size;
    void alloc_cstr(const char* str, size_t size);
    void append(const char* str, size_t size);

 public:
    tuwString() : m_str(nullptr), m_size(0) {}
    tuwString(const char* str);  // NOLINT(runtime/explicit)
    tuwString(const char* str, size_t size);
    tuwString(const tuwString& str);
    tuwString(tuwString&& str);

    // This allocates null bytes to use them as a buffer.
    explicit tuwString(size_t size);

    ~tuwString() { clear(); }
    size_t length() const { return m_size; }
    size_t size() const { return m_size; }

    bool empty() const { return m_str == nullptr || m_str[0] == '\0'; }

    // Returns an immutable C string. This can't be null.
    const char* c_str() const {
        if (m_str)
            return m_str;
        return "";
    }

    // Returns a pointer to the actual buffer.
    char* data() const { return m_str; }

    void clear() {
        if (m_str)
            free(m_str);
        m_str = nullptr;
        m_size = 0;
    }

    tuwString& operator=(const char* str);
    tuwString& operator=(const tuwString& str);
    tuwString& operator=(tuwString&& str);

    tuwString& operator+=(const char* str);
    tuwString& operator+=(const tuwString& str);

    tuwString operator+(const char* str) const;
    tuwString operator+(const tuwString& str) const;

    // You can append numbers as strings
    tuwString operator+(int num) const;
    tuwString operator+(size_t num) const;
    tuwString operator+(uint32_t num) const;

    const char& operator[](size_t id) const {
        return c_str()[id];
    }

    bool operator==(const char* str) const;
    bool operator==(const tuwString& str) const;
    inline bool operator!=(const char* str) const {
        return !(*this == str);
    }
    inline bool operator!=(const tuwString& str) const {
        return !(*this == str);
    }

    const char* begin() const {
        return c_str();
    }

    const char* end() const {
        return c_str() + m_size;
    }

    static const size_t npos;
    size_t find(const char c) const;
    size_t find(const char* str) const;
    inline size_t find(const tuwString& str) const {
        return find(str.c_str());
    }

    void push_back(const char c) {
        this->append(&c, 1);
    }

    tuwString substr(size_t start, size_t size) const;
};

tuwString operator+(const char* str1, const tuwString& str2);

inline bool operator==(const char* str1, const tuwString& str2) {
    return str2 == str1;
}

inline bool operator!=(const char* str1, const tuwString& str2) {
    return str2 != str1;
}

class tuwWstring {
 private:
    wchar_t* m_str;
    size_t m_size;

 public:
    tuwWstring(const wchar_t* str);  // NOLINT(runtime/explicit)
    ~tuwWstring() { clear(); }
    size_t length() const { return m_size; }
    size_t size() const { return m_size; }
    bool empty() const { return m_str == nullptr || m_str[0] == L'\0'; }

    const wchar_t* c_str() const {
        if (m_str)
            return m_str;
        return L"";
    }

    void clear() {
        if (m_str)
            free(m_str);
        m_size = 0;
    }
};

inline tuwString envuStr(char *cstr) {
    if (cstr == NULL)
        return "";
    tuwString str = cstr;
    envuFree(cstr);
    return str;
}

uint32_t Fnv1Hash32(const tuwString& str);

#ifdef _WIN32
tuwString UTF16toUTF8(const wchar_t* str);
tuwWstring UTF8toUTF16(const char* str);
void PrintFmt(const char* fmt, ...);
void EnableCSI();
#elif defined(__TUW_UNIX__)
void SetLogEntry(void* log_entry);
void PrintFmt(const char* fmt, ...);
#else
#define PrintFmt(...) printf(__VA_ARGS__)
#endif
