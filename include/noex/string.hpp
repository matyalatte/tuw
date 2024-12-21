#pragma once

#include <wchar.h>
#include <cinttypes>

#include "noex/error.hpp"

namespace noex {

// String class that doesn't raise std errors.
// It works without any crashes even when it got a memory allocation error.
// But you have to check get_error_no() after string allocations
// or it might have an unexpected value (an empty string).
template <typename charT>
class basic_string {
 private:
    charT* m_str;
    size_t m_size;
    size_t m_capacity;

    void reserve(size_t capacity) noexcept;
    void assign(const charT* str, size_t size, size_t capacity) noexcept;
    void append(const charT* str, size_t size) noexcept;

 public:
    basic_string() noexcept : m_str(nullptr), m_size(0), m_capacity(0) {}
    basic_string(const charT* str) noexcept;  // NOLINT(runtime/explicit)
    basic_string(const charT* str, size_t size) noexcept;
    basic_string(const basic_string& str) noexcept;
    basic_string(basic_string&& str) noexcept;

    // This allocates null bytes to use them as a buffer.
    explicit basic_string(size_t size) noexcept;

    ~basic_string() noexcept { clear(); }
    inline size_t length() const noexcept { return m_size; }
    inline size_t size() const noexcept { return m_size; }
    inline size_t capacity() const noexcept { return m_capacity; }

    inline bool empty() const noexcept {
        return !m_str || m_size == 0;
    }

    // Returns an immutable C string. This can't be null.
    const charT* c_str() const noexcept;

    // Returns a pointer to the actual buffer.
    inline charT* data() const noexcept { return m_str; }

    void clear() noexcept;

    const charT& operator[](size_t id) const noexcept;

    basic_string& operator=(const charT* str) noexcept;
    basic_string& operator=(const basic_string& str) noexcept;
    basic_string& operator=(basic_string&& str) noexcept;

    basic_string& operator+=(const charT* str) noexcept;
    basic_string& operator+=(const basic_string& str) noexcept;

    basic_string operator+(const charT* str) const noexcept;
    basic_string operator+(const basic_string& str) const noexcept;

    // You can append numbers as strings
    // Note: wstring does not support operator+ for numbers
    basic_string operator+(int num) const noexcept;
    basic_string operator+(size_t num) const noexcept;
    basic_string operator+(uint32_t num) const noexcept;

    static basic_string to_string(int num) noexcept;
    static basic_string to_string(size_t num) noexcept;
    static basic_string to_string(uint32_t num) noexcept;

    bool operator==(const charT* str) const noexcept;
    bool operator==(const basic_string& str) const noexcept;
    inline bool operator!=(const charT* str) const noexcept {
        return !(*this == str);
    }
    inline bool operator!=(const basic_string& str) const noexcept {
        return !(*this == str);
    }

    inline const charT* begin() const noexcept {
        return c_str();
    }

    inline const charT* end() const noexcept {
        return c_str() + m_size;
    }

    static const size_t npos;
    size_t find(charT c) const noexcept;
    size_t find(const charT* str) const noexcept;
    inline size_t find(const basic_string& str) const noexcept {
        return find(str.c_str());
    }

    inline bool contains(charT c) const noexcept {
        return find(c) != npos;
    }
    inline bool contains(const charT* str) const noexcept {
        return find(str) != npos;
    }
    inline bool contains(const basic_string& str) const noexcept {
        return find(str) != npos;
    }

    inline void push_back(charT c) noexcept {
        this->append(&c, 1);
    }

    void erase(size_t pos, size_t n = npos) noexcept;
    basic_string substr(size_t start, size_t size) const noexcept;
};

template <typename charT>
basic_string<charT> operator+(const charT* str1, const basic_string<charT>& str2) noexcept;

template <typename charT>
inline bool operator==(const charT* str1, const basic_string<charT>& str2) noexcept {
    return str2 == str1;
}

template <typename charT>
inline bool operator!=(const charT* str1, const basic_string<charT>& str2) noexcept {
    return str2 != str1;
}

extern template class basic_string<char>;
extern template class basic_string<wchar_t>;
typedef basic_string<char> string;
typedef basic_string<wchar_t> wstring;

// to_string functions
template <typename numT>
inline string to_string(numT num) {
    return string::to_string(num);
}

template <typename numT>
inline wstring to_wstring(numT num) {
    return wstring::to_string(num);
}

}  // namespace noex
