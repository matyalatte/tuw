#pragma once

#include <cstring>
#include <utility>

#include "noex/error.hpp"

namespace noex {

// Vector class that doesn't raise std exceptions.
// It works without any crashes even when it got a memory allocation error.
// But you have to check GetErrorNo() after vector allocations
// or it might have unexpected values.
template <typename T>
class vector {
 private:
    T* m_data;
    size_t m_size;
    size_t m_capacity;

    static T& get_default() noexcept {
        static char buf[sizeof(T)];
        memset(buf, '\0', sizeof(T));
        return *reinterpret_cast<T*>(buf);
    }

 public:
    vector() noexcept : m_data(nullptr), m_size(0), m_capacity(0) {}
    vector(const vector& vec) noexcept :
            m_data(nullptr), m_size(0), m_capacity(0) {
        *this = vec;
    }
    vector(vector&& vec) noexcept :
            m_data(nullptr), m_size(0), m_capacity(0) {
        *this = vec;
    }

    ~vector() noexcept { clear(); }

    void reserve(size_t capacity) noexcept {
        if (capacity <= m_capacity) return;

        T* data = reinterpret_cast<T*>(calloc(capacity, sizeof(T)));
        if (!data) {
            SetErrorNo(VEC_ALLOCATION_ERROR);
            clear();
            return;
        }

        // Move or copy existing elements to the new memory
        for (size_t i = 0; i < m_size; ++i) {
            new (data + i) T(std::move(m_data[i]));
            m_data[i].~T();
        }

        free(m_data);
        m_data = data;
        m_capacity = capacity;
    }

    size_t length() const noexcept { return m_size; }
    size_t size() const noexcept { return m_size; }
    size_t capacity() const noexcept { return m_capacity; }

    bool empty() const noexcept {
        return m_data == nullptr || m_size == 0;
    }

    // Returns a pointer to the actual buffer.
    T* data() const noexcept { return m_data; }

    void clear() noexcept {
        if (m_data) {
            for (size_t i = 0; i < m_size; ++i)
                m_data[i].~T();
            free(m_data);
        }
        m_data = nullptr;
        m_size = 0;
        m_capacity = 0;
    }

    T& at(size_t id) const noexcept {
        if (empty() || id >= m_size) {
            // boundary error
            SetErrorNo(VEC_BOUNDARY_ERROR);
            return get_default();
        }
        return m_data[id];
    }

    T& operator[](size_t id) const noexcept {
        return at(id);
    }

    vector& operator=(const vector& vec) noexcept {
        if (this == &vec) return *this;
        reserve(vec.capacity());
        if (m_capacity != vec.size()) return *this;
        for (size_t i = 0; i < vec.size(); ++i) {
            m_data[i] = vec[i];
        }
        m_size = vec.size();
        return *this;
    }

    vector& operator=(vector&& vec) noexcept {
        if (this == &vec) return *this;
        reserve(vec.capacity());
        if (m_capacity != vec.size()) return *this;
        for (size_t i = 0; i < vec.size(); ++i) {
            *(m_data + i) = std::move(vec[i]);
        }
        m_size = vec.size();
        vec.clear();
        return *this;
    }

    bool operator==(const vector& vec) const noexcept {
        if (vec.size() != m_size) return false;
        for (size_t i = 0; i < vec.size(); ++i)
            if (m_data[i] != vec[i]) return false;
        return true;
    }
    inline bool operator!=(const vector& str) const noexcept {
        return !(*this == str);
    }

    T* begin() const noexcept {
        if (empty()) return &get_default();
        return m_data;
    }

    T* end() const noexcept {
        if (empty()) return &get_default();
        return m_data + m_size;
    }

    T& back() const noexcept {
        if (empty()) {
            SetErrorNo(VEC_BOUNDARY_ERROR);
            return get_default();
        }
        return *(m_data + m_size - 1);
    }

    void push_back(const T& val) noexcept {
        reserve(m_size + 1);
        if (m_capacity < m_size + 1) return;
        T* new_val = m_data + m_size;
        *new_val = val;
        m_size++;
    }

    void push_back(T&& val) noexcept {
        reserve(m_size + 1);
        if (m_capacity < m_size + 1) return;
        T* new_val = m_data + m_size;
        *new_val = std::move(val);
        m_size++;
    }

    template <typename... Args>
    void emplace_back(Args&&... args) noexcept {
        reserve(m_size + 1);
        if (m_capacity < m_size + 1) return;

        // Use placement new to construct the object in-place
        new (m_data + m_size) T(std::forward<Args>(args)...);
        m_size++;
    }

    void shrink_to_fit() noexcept {
        if (m_size >= m_capacity) return;

        T* data = static_cast<T*>(calloc(m_size, sizeof(T)));
        if (!data) {
            SetErrorNo(VEC_ALLOCATION_ERROR);
            return;
        }

        // Move or copy existing elements to the new memory
        for (size_t i = 0; i < m_size; ++i) {
            new (data + i) T(std::move(m_data[i]));
            m_data[i].~T();
        }

        free(m_data);
        m_data = data;
        m_capacity = m_size;
    }
};

}  // namespace noex
