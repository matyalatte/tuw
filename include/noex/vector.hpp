#pragma once

#include <type_traits>
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

    // if T is C-style struct or not
    static const bool is_trivial = std::is_trivial<T>::value;

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
        if (is_trivial) {
            memcpy(data, m_data, m_size * sizeof(T));
        } else {
            for (size_t i = 0; i < m_size; ++i) {
                new (data + i) T(std::move(m_data[i]));
                m_data[i].~T();
            }
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
            if (!is_trivial) {
                for (size_t i = 0; i < m_size; ++i)
                    m_data[i].~T();
            }
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
            static T dummy{};
            return dummy;
        }
        return m_data[id];
    }

    T& operator[](size_t id) const noexcept {
        return at(id);
    }

    vector& operator=(const vector& vec) noexcept {
        if (this == &vec) return *this;
        reserve(vec.capacity());
        if (m_capacity != vec.m_size) return *this;
        if (is_trivial) {
            memcpy(m_data, vec.m_data, vec.m_size * sizeof(T));
        } else {
            for (size_t i = 0; i < vec.m_size; ++i)
                new (m_data + i) T(vec.m_data[i]);
        }
        m_size = vec.size();
        return *this;
    }

    vector& operator=(vector&& vec) noexcept {
        if (this == &vec) return *this;

        if (is_trivial) {
            clear();
            m_data = vec.m_data;
            m_size = vec.m_size;
            m_capacity = vec.m_capacity;
            vec.m_data = nullptr;
            vec.m_size = 0;
            vec.m_capacity = 0;
        } else {
            reserve(vec.capacity());
            if (m_capacity != vec.m_size) return *this;
            for (size_t i = 0; i < vec.m_size; ++i) {
                new (m_data + i) T(std::move(vec.m_data[i]));
            }
            m_size = vec.size();
            vec.clear();
        }
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
        return m_data;
    }

    T* end() const noexcept {
        return m_data + m_size;
    }

    T& back() const noexcept {
        return at(m_size - 1);
    }

    void push_back(const T& val) noexcept {
        reserve(m_size + 1);
        if (m_capacity < m_size + 1) return;
        if (is_trivial) {
            memcpy(m_data + m_size, &val, sizeof(T));
        } else {
            new (m_data + m_size) T(val);
        }
        m_size++;
    }

    void push_back(T&& val) noexcept {
        reserve(m_size + 1);
        if (m_capacity < m_size + 1) return;
        if (is_trivial) {
            memcpy(m_data + m_size, &val, sizeof(T));
        } else {
            new (m_data + m_size) T(std::move(val));
        }
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
        if (is_trivial) {
            memcpy(data, m_data, m_size * sizeof(T));
        } else {
            for (size_t i = 0; i < m_size; ++i) {
                new (data + i) T(std::move(m_data[i]));
                m_data[i].~T();
            }
        }

        free(m_data);
        m_data = data;
        m_capacity = m_size;
    }
};

}  // namespace noex
