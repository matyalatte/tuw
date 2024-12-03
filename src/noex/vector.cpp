#include <type_traits>
#include <cstdlib>
#include <cstring>
#include <utility>

#include "noex/vector.hpp"
#include "noex/string.hpp"

namespace noex {

void trivial_vector_base::assign(const trivial_vector_base& vec) noexcept {
    if (this == &vec) return;
    reserve(vec.m_capacity);
    if (m_capacity != vec.m_size) return;
    memcpy(m_data, vec.m_data, vec.m_size * vec.m_sizeof_type);
    m_size = vec.m_size;
    return;
}

void trivial_vector_base::assign(trivial_vector_base&& vec) noexcept {
    if (this == &vec) return;
    clear();
    m_data = vec.m_data;
    m_size = vec.m_size;
    m_capacity = vec.m_capacity;
    vec.m_data = nullptr;
    vec.m_size = 0;
    vec.m_capacity = 0;
    return;
}

trivial_vector_base::trivial_vector_base(size_t sizeof_type) noexcept :
        m_data(nullptr), m_size(0),
        m_capacity(0), m_sizeof_type(sizeof_type) {}

trivial_vector_base::trivial_vector_base(const trivial_vector_base& vec) noexcept :
        m_data(nullptr), m_size(0), m_capacity(0),
        m_sizeof_type(vec.m_sizeof_type) {
    assign(vec);
}

trivial_vector_base::trivial_vector_base(trivial_vector_base&& vec) noexcept :
        m_data(nullptr), m_size(0), m_capacity(0),
        m_sizeof_type(vec.m_sizeof_type) {
    assign(vec);
}

void trivial_vector_base::reserve(size_t capacity) noexcept {
    if (capacity <= m_capacity) return;

    char* data = static_cast<char*>(calloc(capacity, m_sizeof_type));
    if (!data) {
        SetErrorNo(VEC_ALLOCATION_ERROR);
        clear();
        return;
    }

    memcpy(data, m_data, m_size * m_sizeof_type);

    free(m_data);
    m_data = data;
    m_capacity = capacity;
}

void trivial_vector_base::clear() noexcept {
    if (m_data)
        free(m_data);
    m_data = nullptr;
    m_size = 0;
    m_capacity = 0;
}

void* trivial_vector_base::at_base(size_t id) const noexcept {
    if (empty() || id >= m_size) {
        // boundary error
        SetErrorNo(VEC_BOUNDARY_ERROR);
        return nullptr;
    }
    return m_data + (id * m_sizeof_type);
}

void trivial_vector_base::push_back_base(const void* val) noexcept {
    reserve(m_size + 1);
    if (m_capacity < m_size + 1) return;
    memcpy(m_data + m_size * m_sizeof_type, val, m_sizeof_type);
    m_size++;
}

void trivial_vector_base::shrink_to_fit() noexcept {
    if (m_size >= m_capacity) return;

    char* data = static_cast<char*>(calloc(m_size, m_sizeof_type));
    if (!data) {
        SetErrorNo(VEC_ALLOCATION_ERROR);
        return;
    }

    // Move or copy existing elements to the new memory
    memcpy(data, m_data, m_size * m_sizeof_type);

    free(m_data);
    m_data = data;
    m_capacity = m_size;
}

}  // namespace noex
