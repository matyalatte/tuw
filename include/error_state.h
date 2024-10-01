#pragma once

#include <cstdarg>
#include <cstddef>

class ErrorState {
 private:
    bool m_has_error;
    char* m_error_buf;
    size_t m_error_buf_size;
    bool ReallocErrorBuf(size_t len);
    void ThrowBase(const char* fmt, va_list va);

 public:
    ErrorState() : m_has_error(false),
                   m_error_buf(nullptr),
                   m_error_buf_size(0) {}
    ~ErrorState();
    void Reset();
    inline bool HasError() const { return m_has_error; }
    inline bool HasWarning() const { return !m_has_error && m_error_buf; }
    inline bool Ok() const { return !HasError(); }
    const char* GetErrorMsg() const { return (HasError() && m_error_buf) ? m_error_buf : ""; }
    const char* GetWarningMsg() const { return HasWarning() ? m_error_buf : ""; }
    void Throw(const char* fmt, ...);
    void Warn(const char* fmt, ...);
};
