#pragma once
#include <cstring>
#include "string_utils.h"

struct ExecuteResult {
    int exit_code;
    noex::string err_msg;
    noex::string last_line;
};

// When use_utf8_on_windows is true,
// Tuw converts output strings from UTF-8 to UTF-16 on Windows.
ExecuteResult Execute(const noex::string& cmd,
                      bool use_utf8_on_windows = false) noexcept;
ExecuteResult LaunchDefaultApp(const noex::string& url) noexcept;

// We use ring buffers to store outputs.
template <size_t Size>
class RingStrBuffer {
 private:
    char m_buffer[Size];
    size_t m_tail;
    bool m_is_full;

 public:
    RingStrBuffer() noexcept : m_tail(0), m_is_full(false) {}

    bool IsFull() const noexcept {
        return m_is_full;
    }

    bool IsEmpty() const noexcept {
        return (!m_is_full && m_tail == 0);
    }

    void PushBack(char value) noexcept {
        m_buffer[m_tail] = value;

        m_tail = (m_tail + 1) % Size;
        m_is_full = (m_is_full || m_tail == 0);
    }

    void PushBack(const char* buf, size_t size) noexcept {
        if (!buf)
            return;

        if (size >= Size) {
            memcpy(m_buffer, buf + size - Size, Size);
            m_tail = 0;
            m_is_full = true;
            return;
        }

        if (m_tail + size >= Size) {
            size_t first_chunk_size = Size - m_tail;
            memcpy(m_buffer + m_tail, buf, first_chunk_size);
            memcpy(m_buffer, buf + first_chunk_size, size - first_chunk_size);
            m_is_full = true;
        } else {
            memcpy(m_buffer + m_tail, buf, size);
        }
        m_tail = (m_tail + size) % Size;
    }

    // Slide tail to 0
    void SlideToInitialPos() noexcept {
        if (!m_is_full || m_tail == 0)
            return;
        char buf[Size];
        size_t first_chunk_size = Size - m_tail;
        memcpy(buf, m_buffer, m_tail);
        memcpy(m_buffer, m_buffer + m_tail, first_chunk_size);
        memcpy(m_buffer + first_chunk_size, buf, m_tail);
        m_tail = 0;
    }

    noex::string ToString() noexcept {
        if (!m_is_full)
            return noex::string(m_buffer, m_tail);
        SlideToInitialPos();
        return noex::string(m_buffer, Size);
    }
};
