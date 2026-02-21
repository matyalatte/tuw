#pragma once

#include <type_traits>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <new>

#include "noex/error.hpp"

namespace noex {

template <typename T>
class trivial_vector;
template <typename T>
class non_trivial_vector;

// Vector class that doesn't raise std exceptions.
// It works without any crashes even when it got a memory allocation error.
// But you have to check get_error_no() after vector allocations
// or it might have unexpected values.
template <typename T>
class vector :
    public
        std::conditional<
            std::is_trivial<T>::value,
            trivial_vector<T>,
            non_trivial_vector<T>
        >::type {
};

// base class for non trivial classes
template <typename T>
class non_trivial_vector {
    static_assert(!std::is_trivial<T>::value,
                  "template parameter T must be a non-trivial class");

 private:
    T* m_data;
    size_t m_size;
    size_t m_capacity;

 public:
    non_trivial_vector() noexcept : m_data(nullptr), m_size(0), m_capacity(0) {}
    non_trivial_vector(const non_trivial_vector& vec) noexcept :
            m_data(nullptr), m_size(0), m_capacity(0) {
        *this = vec;
    }
    non_trivial_vector(non_trivial_vector&& vec) noexcept :
            m_data(nullptr), m_size(0), m_capacity(0) {
        *this = static_cast<non_trivial_vector&&>(vec);
    }

    ~non_trivial_vector() noexcept { clear(); }

    void reserve(size_t capacity) noexcept {
        if (capacity <= m_capacity) return;

        T* data = reinterpret_cast<T*>(calloc(capacity, sizeof(T)));
        if (!data) {
            set_error_no(VEC_ALLOCATION_ERROR);
            clear();
            return;
        }

        // Move or copy existing elements to the new memory
        for (size_t i = 0; i < m_size; ++i) {
            new (data + i) T(static_cast<T&&>(m_data[i]));
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
        return m_size == 0;
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
            set_error_no(VEC_BOUNDARY_ERROR);
            static T dummy{};
            return dummy;
        }
        return m_data[id];
    }

    T& operator[](size_t id) const noexcept {
        return at(id);
    }

    non_trivial_vector& operator=(const non_trivial_vector& vec) noexcept {
        if (this == &vec) return *this;
        reserve(vec.m_capacity);
        if (m_capacity != vec.m_size) return *this;
        for (size_t i = 0; i < vec.m_size; ++i)
            new (m_data + i) T(vec.m_data[i]);
        m_size = vec.m_size;
        return *this;
    }

    non_trivial_vector& operator=(non_trivial_vector&& vec) noexcept {
        if (this == &vec) return *this;
        reserve(vec.m_capacity);
        if (m_capacity != vec.m_size) return *this;
        for (size_t i = 0; i < vec.m_size; ++i) {
            new (m_data + i) T(static_cast<T&&>(vec.m_data[i]));
        }
        m_size = vec.m_size;
        vec.clear();
        return *this;
    }

    bool operator==(const non_trivial_vector& vec) const noexcept {
        if (vec.m_size != m_size) return false;
        for (size_t i = 0; i < vec.m_size; ++i)
            if (m_data[i] != vec[i]) return false;
        return true;
    }

    inline bool operator!=(const non_trivial_vector& str) const noexcept {
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

    void pop_back() noexcept {
        if (empty())
            return;
        back().~T();
        m_size--;
    }

    void push_back(const T& val) noexcept {
        reserve(m_size + 1);
        if (m_capacity < m_size + 1) return;
        new (m_data + m_size) T(val);
        m_size++;
    }

    void push_back(T&& val) noexcept {
        reserve(m_size + 1);
        if (m_capacity < m_size + 1) return;
        new (m_data + m_size) T(static_cast<T&&>(val));
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

        T* data = reinterpret_cast<T*>(calloc(m_size, sizeof(T)));
        if (!data) {
            set_error_no(VEC_ALLOCATION_ERROR);
            return;
        }

        // Move or copy existing elements to the new memory
        for (size_t i = 0; i < m_size; ++i) {
            new (data + i) T(static_cast<T&&>(m_data[i]));
            m_data[i].~T();
        }

        free(m_data);
        m_data = data;
        m_capacity = m_size;
    }
};

// base class for trivial_vector
class trivial_vector_base {
 protected:
    char* m_data;
    size_t m_size;
    size_t m_capacity;
    size_t m_sizeof_type;

    void assign(const trivial_vector_base& vec) noexcept;
    void assign(trivial_vector_base&& vec) noexcept;
    void* at_base(size_t id) const noexcept;
    void push_back_base(const void* val) noexcept;

 public:
    trivial_vector_base(size_t sizeof_type) noexcept;  // NOLINT(runtime/explicit)
    trivial_vector_base(const trivial_vector_base& vec) noexcept;
    trivial_vector_base(trivial_vector_base&& vec) noexcept;
    ~trivial_vector_base() noexcept { clear(); }

    void reserve(size_t capacity) noexcept;
    size_t length() const noexcept { return m_size; }
    size_t size() const noexcept { return m_size; }
    size_t capacity() const noexcept { return m_capacity; }

    bool empty() const noexcept {
        return m_size == 0;
    }

    void clear() noexcept;

    void emplace_back() noexcept {
        reserve(m_size + 1);
    }

    void shrink_to_fit() noexcept;

    void pop_back() noexcept {
        if (empty())
            return;
        m_size--;
    }
};

// base class for trivial classes
template <typename T>
class trivial_vector : public trivial_vector_base {
    static_assert(std::is_trivial<T>::value,
                  "template parameter T must be a trivial class");

 public:
    trivial_vector() noexcept :
        trivial_vector_base(sizeof(T)) {}
    trivial_vector(const trivial_vector& vec) noexcept :
        trivial_vector_base(vec) {}
    trivial_vector(trivial_vector&& vec) noexcept :
        trivial_vector_base(static_cast<trivial_vector&&>(vec)) {}

    trivial_vector& operator=(const trivial_vector& vec) noexcept {
        assign(vec);
        return *this;
    }

    trivial_vector& operator=(trivial_vector&& vec) noexcept {
        assign(static_cast<trivial_vector&&>(vec));
        return *this;
    }

    bool operator==(const trivial_vector& vec) const noexcept {
        if (vec.m_size != m_size) return false;
        T* this_data_p = static_cast<T*>(m_data);
        T* vec_data_p = static_cast<T*>(vec.m_data);
        for (size_t i = 0; i < vec.m_size; ++i) {
            if (*this_data_p != *vec_data_p) return false;
            this_data_p++;
            vec_data_p++;
        }
        return true;
    }

    bool operator!=(const trivial_vector& str) const noexcept {
        return !(*this == str);
    }

    void push_back(const T val) noexcept {
        push_back_base(&val);
    }

    // Note:
    //   emplace_back was implemented only for compatibility.
    //   We should use push_back for trivial classes to minimize binary.
    template <typename... Args>
    void emplace_back(Args&&... args) noexcept {
        reserve(m_size + 1);
        if (m_capacity < m_size + 1) return;

        // Use placement new to construct the object in-place
        new (m_data + m_size * m_sizeof_type) T(std::forward<Args>(args)...);
        m_size++;
    }

    // Returns a pointer to the actual buffer.
    T* data() const noexcept {
        return reinterpret_cast<T*>(m_data);
    }

    T& at(size_t id) const noexcept {
        T* ptr = static_cast<T*>(at_base(id));
        if (ptr == nullptr) {
            static T dummy{};
            return dummy;
        }
        return *ptr;
    }

    T& operator[](size_t id) const noexcept {
        return at(id);
    }

    T* begin() const noexcept {
        return reinterpret_cast<T*>(m_data);
    }

    T* end() const noexcept {
        return reinterpret_cast<T*>(m_data + m_size * m_sizeof_type);
    }

    T& back() const noexcept {
        return *static_cast<T*>(at_base(m_size - 1));
    }
};

}  // namespace noex
