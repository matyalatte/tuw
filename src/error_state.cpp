#include "error_state.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>

ErrorState::~ErrorState() {
    free(m_error_buf);
}

void ErrorState::Reset() {
    m_has_error = false;
    free(m_error_buf);
    m_error_buf_size = 0;
}

bool ErrorState::ReallocErrorBuf(size_t len) {
    if (m_error_buf && m_error_buf_size >= len + 1)
        return true;  // No need to re-allocate
    m_error_buf = (char*)realloc(m_error_buf, sizeof(char) * (len + 1));
    if (!m_error_buf) {
        // Failed to allocate
        m_error_buf_size = 0;
        return false;
    }
    m_error_buf_size = len + 1;
    return true;
}

void ErrorState::ThrowBase(const char* fmt, va_list va) {
    size_t len = vsnprintf(nullptr, 0, fmt, va);
    bool alloc_ok = ReallocErrorBuf(len);
    if (alloc_ok)
        vsnprintf(m_error_buf, len + 1, fmt, va);
}

void ErrorState::Throw(const char* fmt, ...) {
    m_has_error = true;
    va_list va;
    va_start(va, fmt);
    ThrowBase(fmt, va);
    va_end(va);
}

void ErrorState::Warn(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    ThrowBase(fmt, va);
    va_end(va);
}
