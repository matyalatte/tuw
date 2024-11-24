#include <cinttypes>
#include <wchar.h>
#include <cstdio>
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

void string::assign(const char *str, size_t size) noexcept {
    clear();
    if (!str || size == 0)
        return;

    m_size = size;
    m_str = reinterpret_cast<char*>(malloc(size + 1));
    if (!m_str) {
        m_size = 0;
        SetErrorNo(STR_ALLOCATION_ERROR);
        return;
    }
    memcpy(m_str, str, size);
    m_str[size] = '\0';
}

#define get_strlen(str) ((str) ? strlen(str) : 0)

string::string(const char* str) noexcept :
        m_str(nullptr), m_size(0) {
    assign(str, get_strlen(str));
}

string::string(const char* str, size_t size) noexcept :
        m_str(nullptr), m_size(0) {
    assign(str, size);
}

string::string(const noex::string& str) noexcept :
        m_str(nullptr), m_size(0) {
    assign(str.c_str(), str.size());
}

string::string(noex::string&& str) noexcept :
        m_str(str.m_str), m_size(str.m_size) {
    str.m_str = nullptr;
    str.m_size = 0;
}

string::string(size_t size) noexcept : m_size(size) {
    m_str = reinterpret_cast<char*>(calloc(size + 1, sizeof(char)));
    if (!m_str) {
        m_size = 0;
        SetErrorNo(STR_ALLOCATION_ERROR);
    }
}

const char* string::c_str() const noexcept {
    if (m_str)
        return m_str;
    return "";
}

void string::clear() noexcept {
    if (m_str)
        free(m_str);
    m_str = nullptr;
    m_size = 0;
}

string& string::operator=(const char* str) noexcept {
    assign(str, get_strlen(str));
    return *this;
}

string& string::operator=(const string& str) noexcept {
    if (this == &str) return *this;
    assign(str.c_str(), str.size());
    return *this;
}

string& string::operator=(string&& str) noexcept {
    if (this == &str) return *this;
    clear();
    m_str = str.m_str;
    m_size = str.m_size;
    str.m_str = nullptr;
    str.m_size = 0;
    return *this;
}

void string::append(const char* str, size_t size) noexcept {
    if (!str || size == 0)
        return;

    size_t new_size = m_size + size;
    char* new_cstr = reinterpret_cast<char*>(malloc(new_size + 1));
    if (!new_cstr) {
        SetErrorNo(STR_ALLOCATION_ERROR);
        return;
    }

    if (!empty())
        memcpy(new_cstr, m_str, m_size);
    memcpy(new_cstr + m_size, str, size);
    new_cstr[new_size] = '\0';

    clear();
    m_str = new_cstr;
    m_size = new_size;
}

string& string::operator+=(const char* str) noexcept {
    append(str, get_strlen(str));
    return *this;
}

string& string::operator+=(const string& str) noexcept {
    append(str.c_str(), str.size());
    return *this;
}

string string::operator+(const char* str) const noexcept {
    string new_str(*this);
    new_str += str;
    return new_str;
}

string string::operator+(const string& str) const noexcept {
    string new_str(*this);
    new_str += str;
    return new_str;
}

// Convert number to c string, and append it to string.
# define DEFINE_PLUS_FOR_NUM(num_type, fmt) \
string string::operator+(num_type num) const noexcept { \
    string new_str(*this); \
    \
    int num_size = snprintf(nullptr, 0, "%" fmt, num); \
    if (num_size <= 0) { \
        SetErrorNo(STR_FORMAT_ERROR); \
        return new_str; \
    } \
    \
    char* num_str = reinterpret_cast<char*>(malloc(num_size + 1)); \
    if (!num_str) { \
        SetErrorNo(STR_ALLOCATION_ERROR); \
        return new_str; \
    } \
    \
    int ret = snprintf(num_str, num_size + 1, "%" fmt, num); \
    if (ret == num_size) { \
        new_str.append(num_str, num_size); \
    } else { \
        SetErrorNo(STR_FORMAT_ERROR); \
    } \
    free(num_str); \
    return new_str; \
}

DEFINE_PLUS_FOR_NUM(int, "d")
DEFINE_PLUS_FOR_NUM(size_t, "zu")
DEFINE_PLUS_FOR_NUM(uint32_t, PRIu32)

#define null_to_empty(str) ((str) ? str : "")

bool string::operator==(const char* str) const noexcept {
    return strcmp(c_str(), null_to_empty(str)) == 0;
}

bool string::operator==(const string& str) const noexcept {
    return strcmp(c_str(), str.c_str()) == 0;
}

const size_t string::npos = static_cast<size_t>(-1);

size_t string::find(const char c) const noexcept {
    if (empty()) return npos;
    const char* p = begin();
    for (; p < end(); p++) {
        if (*p == c)
            return static_cast<size_t>(p - m_str);
    }
    return npos;
}

size_t string::find(const char* str) const noexcept {
    if (empty() || !str) return npos;

    // Note: This function uses a slow algorithm.
    const char* p = begin();
    for (; p < end(); p++) {
        const char* tmp_p = p;
        const char* str_p = str;
        while (tmp_p < end() && *str_p != '\0' && *tmp_p == *str_p) {
            tmp_p++;
            str_p++;
        }
        if (*str_p == '\0')
            return static_cast<size_t>(p - m_str);
    }
    return npos;
}

string string::substr(size_t start, size_t size) const noexcept {
    if (start + size > m_size) {
        SetErrorNo(STR_BOUNDARY_ERROR);
        return string();
    }
    string new_str(m_str + start, size);
    return new_str;
}

const char& string::operator[](size_t id) const noexcept {
    if (id > m_size) {
        SetErrorNo(STR_BOUNDARY_ERROR);
        return ""[0];
    }
    return c_str()[id];
}

string operator+(const char* str1, const string& str2) noexcept {
    string new_str(str1);
    new_str += str2;
    return new_str;
}

wstring::wstring(const wchar_t* str) noexcept :
        m_str(nullptr), m_size(0) {
    if (!str) return;

    size_t size = wcslen(str);
    if (size == 0)
        return;

    m_str = reinterpret_cast<wchar_t*>(malloc((size + 1) * sizeof(wchar_t)));
    if (!m_str) {
        noex::SetErrorNo(STR_ALLOCATION_ERROR);
        return;
    }

    m_size = size;
    memcpy(m_str, str, size * sizeof(wchar_t));
    m_str[size] = L'\0';
}

const wchar_t* wstring::c_str() const noexcept {
    if (m_str)
        return m_str;
    return L"";
}

void wstring::clear() noexcept {
    if (m_str)
        free(m_str);
    m_str = nullptr;
    m_size = 0;
}

}  // namespace noex
