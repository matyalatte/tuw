#pragma once

#include <cinttypes>
#include <wchar.h>
#include <cstdlib>
#include <cstring>

#include "noex/error.hpp"

namespace noex {

// String class that doesn't raise std errors.
// It works without any crashes even when it got a memory allocation error.
// But you have to check GetErrorNo() after string allocations
// or it might have an unexpected value (an empty string).
class string {
 private:
    char* m_str;
    size_t m_size;
    void assign(const char* str, size_t size) noexcept;
    void append(const char* str, size_t size) noexcept;

 public:
    string() noexcept : m_str(nullptr), m_size(0) {}
    string(const char* str) noexcept;  // NOLINT(runtime/explicit)
    string(const char* str, size_t size) noexcept;
    string(const string& str) noexcept;
    string(string&& str) noexcept;

    // This allocates null bytes to use them as a buffer.
    explicit string(size_t size) noexcept;

    ~string() noexcept { clear(); }
    inline size_t length() const noexcept { return m_size; }
    inline size_t size() const noexcept { return m_size; }

    inline bool empty() const noexcept {
        return !m_str || m_size == 0;
    }

    // Returns an immutable C string. This can't be null.
    const char* c_str() const noexcept;

    // Returns a pointer to the actual buffer.
    inline char* data() const noexcept { return m_str; }

    void clear() noexcept;

    const char& operator[](size_t id) const noexcept;

    string& operator=(const char* str) noexcept;
    string& operator=(const string& str) noexcept;
    string& operator=(string&& str) noexcept;

    string& operator+=(const char* str) noexcept;
    string& operator+=(const string& str) noexcept;

    string operator+(const char* str) const noexcept;
    string operator+(const string& str) const noexcept;

    // You can append numbers as strings
    string operator+(int num) const noexcept;
    string operator+(size_t num) const noexcept;
    string operator+(uint32_t num) const noexcept;

    bool operator==(const char* str) const noexcept;
    bool operator==(const string& str) const noexcept;
    inline bool operator!=(const char* str) const noexcept {
        return !(*this == str);
    }
    inline bool operator!=(const string& str) const noexcept {
        return !(*this == str);
    }

    inline const char* begin() const noexcept {
        return c_str();
    }

    inline const char* end() const noexcept {
        return c_str() + m_size;
    }

    static const size_t npos;
    size_t find(const char c) const noexcept;
    size_t find(const char* str) const noexcept;
    inline size_t find(const string& str) const noexcept {
        return find(str.c_str());
    }

    inline void push_back(const char c) noexcept {
        this->append(&c, 1);
    }

    string substr(size_t start, size_t size) const noexcept;
};

string operator+(const char* str1, const string& str2) noexcept;

inline bool operator==(const char* str1, const string& str2) noexcept {
    return str2 == str1;
}

inline bool operator!=(const char* str1, const string& str2) noexcept {
    return str2 != str1;
}

class wstring {
 private:
    wchar_t* m_str;
    size_t m_size;

 public:
    wstring(const wchar_t* str) noexcept;  // NOLINT(runtime/explicit)
    ~wstring() noexcept { clear(); }
    inline size_t length() const noexcept { return m_size; }
    inline size_t size() const noexcept { return m_size; }
    inline bool empty() const noexcept {
        return !m_str || m_size == 0;
    }

    const wchar_t* c_str() const noexcept;
    void clear() noexcept;
};

}  // namespace noex
