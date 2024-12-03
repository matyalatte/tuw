#include <wchar.h>
#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "noex/string.hpp"

namespace noex {

static ErrorNo g_error_status = OK;

ErrorNo GetErrorNo() noexcept {
    return g_error_status;
}

void ClearErrorNo() noexcept {
    g_error_status = noex::OK;
}

void SetErrorNo(ErrorNo err) noexcept {
    g_error_status = err;
}

template <typename charT>
void basic_string<charT>::reserve(size_t capacity) noexcept {
    // do nothing when it has enough size already.
    if (capacity <= m_capacity) return;

    // allocate a new buffer
    charT* new_str = static_cast<charT*>(calloc(capacity + 1, sizeof(charT)));
    if (!new_str) {
        clear();
        SetErrorNo(STR_ALLOCATION_ERROR);
        return;
    }

    // copy the old buffer to the new one.
    if (m_str) {
        memcpy(new_str, m_str, (m_size + 1) * sizeof(charT));
        free(m_str);
    }
    m_str = new_str;
    m_capacity = capacity;
}

template <typename charT>
void basic_string<charT>::assign(const charT* str, size_t size, size_t capacity) noexcept {
    reserve(capacity);
    if (!m_str || !str)
        return;

    memcpy(m_str, str, size * sizeof(charT));
    m_str[size] = 0;
    m_size = size;
}

inline size_t get_strlen(const char* str) noexcept {
    return (str) ? strlen(str) : 0;
}

inline size_t get_strlen(const wchar_t* str) noexcept {
    return (str) ? wcslen(str) : 0;
}

template <typename charT>
basic_string<charT>::basic_string(const charT* str) noexcept :
        m_str(nullptr), m_size(0), m_capacity(0) {
    size_t size = get_strlen(str);
    assign(str, size, size);
}

template <typename charT>
basic_string<charT>::basic_string(const charT* str, size_t size) noexcept :
        m_str(nullptr), m_size(0), m_capacity(0) {
    assign(str, size, size);
}

template <typename charT>
basic_string<charT>::basic_string(const basic_string<charT>& str) noexcept :
        m_str(nullptr), m_size(0), m_capacity(0) {
    assign(str.c_str(), str.m_size, str.m_capacity);
}

template <typename charT>
basic_string<charT>::basic_string(basic_string<charT>&& str) noexcept :
        m_str(str.m_str), m_size(str.m_size), m_capacity(str.m_capacity) {
    str.m_str = nullptr;
    str.m_size = 0;
    str.m_capacity = 0;
}

template <typename charT>
basic_string<charT>::basic_string(size_t size) noexcept : m_size(size), m_capacity(size) {
    m_str = reinterpret_cast<charT*>(calloc(size + 1, sizeof(charT)));
    if (!m_str) {
        m_size = 0;
        m_capacity = 0;
        SetErrorNo(STR_ALLOCATION_ERROR);
    }
}

static const char dummy[2] = { 0, 0 };

template <typename charT>
const charT* basic_string<charT>::c_str() const noexcept {
    if (m_str)
        return m_str;
    return reinterpret_cast<const charT*>(dummy);
}

template <typename charT>
void basic_string<charT>::clear() noexcept {
    if (m_str)
        free(m_str);
    m_str = nullptr;
    m_size = 0;
    m_capacity = 0;
}

template <typename charT>
basic_string<charT>& basic_string<charT>::operator=(const charT* str) noexcept {
    size_t size = get_strlen(str);
    assign(str, size, size);
    return *this;
}

template <typename charT>
basic_string<charT>& basic_string<charT>::operator=(const basic_string<charT>& str) noexcept {
    if (this == &str) return *this;
    assign(str.c_str(), str.m_size, str.m_capacity);
    return *this;
}

template <typename charT>
basic_string<charT>& basic_string<charT>::operator=(basic_string<charT>&& str) noexcept {
    if (this == &str) return *this;
    m_str = str.m_str;
    m_size = str.m_size;
    m_capacity = str.m_capacity;
    str.m_str = nullptr;
    str.m_size = 0;
    str.m_capacity = 0;
    return *this;
}

template <typename charT>
void basic_string<charT>::append(const charT* str, size_t size) noexcept {
    size_t new_size = m_size + size;
    reserve(new_size);
    if (!m_str || !str)
        return;

    memcpy(m_str + m_size, str, size * sizeof(charT));
    m_str[new_size] = 0;
    m_size = new_size;
}

template <typename charT>
basic_string<charT>& basic_string<charT>::operator+=(const charT* str) noexcept {
    append(str, get_strlen(str));
    return *this;
}

template <typename charT>
basic_string<charT>& basic_string<charT>::operator+=(const basic_string<charT>& str) noexcept {
    append(str.c_str(), str.size());
    return *this;
}

template <typename charT>
basic_string<charT> basic_string<charT>::operator+(const charT* str) const noexcept {
    basic_string<charT> new_str(*this);
    new_str += str;
    return new_str;
}

template <typename charT>
basic_string<charT> basic_string<charT>::operator+(const basic_string<charT>& str) const noexcept {
    basic_string<charT> new_str(*this);
    new_str += str;
    return new_str;
}

// Convert number to c string, and append it to string.
# define DEFINE_TO_STRING(num_type, num_fmt) \
inline int snprintf_wrap(char* buf, size_t size, const char* fmt, const wchar_t* wfmt, num_type num) { \
    (void) (wfmt); \
    return snprintf(buf, size, fmt, num); \
} \
inline int snprintf_wrap(wchar_t* buf, size_t size, const char* fmt, const wchar_t* wfmt, num_type num) { \
    (void) (fmt); \
    return swprintf(buf, size, wfmt, num); \
} \
template <typename charT> \
basic_string<charT> basic_string<charT>::to_string(num_type num) noexcept { \
    charT buf[21]; /* assume that the max value of num has 20 digits (2^64). */ \
    buf[20] = 0; \
    int num_size = snprintf_wrap(buf, 21, "%" num_fmt, L"%" num_fmt, num); \
    if (num_size <= 0 || num_size > 20) { \
        SetErrorNo(STR_FORMAT_ERROR); \
        return basic_string<charT>(); \
    } \
    return basic_string<charT>(buf, num_size); \
} \
template <typename charT> \
basic_string<charT> basic_string<charT>::operator+(num_type num) const noexcept { \
    basic_string<charT> new_str(*this); \
    return new_str + basic_string<charT>::to_string(num); \
}

DEFINE_TO_STRING(int, "d")
DEFINE_TO_STRING(size_t, "zu")
DEFINE_TO_STRING(uint32_t, PRIu32)

inline bool streq(const char* str1, const char* str2) noexcept {
    return strcmp(str1, str2) == 0;
}

inline bool streq(const wchar_t* str1, const wchar_t* str2) noexcept {
    return wcscmp(str1, str2) == 0;
}

template <typename charT>
bool basic_string<charT>::operator==(const charT* str) const noexcept {
    return streq(c_str(), (str) ? str : reinterpret_cast<const charT*>(dummy));
}

template <typename charT>
bool basic_string<charT>::operator==(const basic_string<charT>& str) const noexcept {
    return streq(c_str(), str.c_str());
}

template <typename charT>
const size_t basic_string<charT>::npos = static_cast<size_t>(-1);

template <typename charT>
size_t basic_string<charT>::find(const charT c) const noexcept {
    if (empty()) return npos;
    const charT* p = begin();
    for (; p < end(); p++) {
        if (*p == c)
            return static_cast<size_t>(p - m_str);
    }
    return npos;
}

template <typename charT>
size_t basic_string<charT>::find(const charT* str) const noexcept {
    if (empty() || !str) return npos;

    // Note: This function uses a slow algorithm.
    const charT* p = begin();
    for (; p < end(); p++) {
        const charT* tmp_p = p;
        const charT* str_p = str;
        while (tmp_p < end() && *str_p != 0 && *tmp_p == *str_p) {
            tmp_p++;
            str_p++;
        }
        if (*str_p == 0)
            return static_cast<size_t>(p - m_str);
    }
    return npos;
}

template <typename charT>
basic_string<charT> basic_string<charT>::substr(size_t start, size_t size) const noexcept {
    if (start + size > m_size) {
        SetErrorNo(STR_BOUNDARY_ERROR);
        return basic_string<charT>();
    }
    basic_string<charT> new_str(m_str + start, size);
    return new_str;
}

template <typename charT>
void basic_string<charT>::erase(size_t pos, size_t n) noexcept {
    if (m_size < pos) {
        SetErrorNo(STR_BOUNDARY_ERROR);
        return;
    }
    if (n == npos) {
        n = m_size - pos;
    } else if (m_size < pos + n) {
        SetErrorNo(STR_BOUNDARY_ERROR);
        return;
    }
    memcpy(m_str + pos, m_str + pos + n, m_size - pos - n);
    m_size -= n;
    m_str[m_size] = 0;
}

template <typename charT>
const charT& basic_string<charT>::operator[](size_t id) const noexcept {
    if (id > m_size) {
        SetErrorNo(STR_BOUNDARY_ERROR);
        return reinterpret_cast<const charT*>(dummy)[0];
    }
    return c_str()[id];
}

template <typename charT>
basic_string<charT> operator+(const charT* str1, const basic_string<charT>& str2) noexcept {
    basic_string<charT> new_str(str1);
    new_str += str2;
    return new_str;
}
template
basic_string<char> operator+(const char* str1, const basic_string<char>& str2) noexcept;
template
basic_string<wchar_t> operator+(const wchar_t* str1, const basic_string<wchar_t>& str2) noexcept;

template class basic_string<char>;
template class basic_string<wchar_t>;

}  // namespace noex
