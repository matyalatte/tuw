#include <type_traits>
#include <cstdlib>
#include <cstring>
#include <utility>

#include "noex/vector.hpp"
#include "noex/string.hpp"

namespace noex {

basic_point_vector::basic_point_vector(const basic_point_vector& vec) noexcept :
        m_data(nullptr), m_size(0), m_capacity(0) {
    assign(vec);
}

basic_point_vector::basic_point_vector(basic_point_vector&& vec) noexcept :
        m_data(nullptr), m_size(0), m_capacity(0) {
    assign(vec);
}

void basic_point_vector::assign(const basic_point_vector& vec) noexcept {
    if (this == &vec) return;
    reserve(vec.capacity());
    if (m_capacity != vec.m_size) return;
    memcpy(m_data, vec.m_data, vec.m_size * sizeof(void*));
    m_size = vec.size();
    return;
}

void basic_point_vector::assign(basic_point_vector&& vec) noexcept {
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

// basic_point_vector::
void basic_point_vector::reserve(size_t capacity) noexcept {
    if (capacity <= m_capacity) return;

    void** data = static_cast<void**>(calloc(capacity, sizeof(void*)));
    if (!data) {
        SetErrorNo(VEC_ALLOCATION_ERROR);
        clear();
        return;
    }

    memcpy(data, m_data, m_size * sizeof(void*));

    free(m_data);
    m_data = data;
    m_capacity = capacity;
}

void basic_point_vector::clear() noexcept {
    if (m_data)
        free(m_data);
    m_data = nullptr;
    m_size = 0;
    m_capacity = 0;
}

void** basic_point_vector::at_base(size_t id) const noexcept {
    if (empty() || id >= m_size) {
        // boundary error
        SetErrorNo(VEC_BOUNDARY_ERROR);
        static void* dummy = nullptr;
        return &dummy;
    }
    return m_data + id;
}

basic_point_vector& basic_point_vector::operator=(const basic_point_vector& vec) noexcept {
    assign(vec);
    return *this;
}

basic_point_vector& basic_point_vector::operator=(basic_point_vector&& vec) noexcept {
    assign(vec);
    return *this;
}

bool basic_point_vector::operator==(const basic_point_vector& vec) const noexcept {
    if (vec.m_size != m_size) return false;
    for (size_t i = 0; i < vec.m_size; ++i)
        if (m_data[i] != vec.m_data[i]) return false;
    return true;
}

void basic_point_vector::push_back(const void* val) noexcept {
    reserve(m_size + 1);
    if (m_capacity < m_size + 1) return;
    memcpy(m_data + m_size, &val, sizeof(void*));
    m_size++;
}

void basic_point_vector::shrink_to_fit() noexcept {
    if (m_size >= m_capacity) return;

    void** data = static_cast<void**>(calloc(m_size, sizeof(void*)));
    if (!data) {
        SetErrorNo(VEC_ALLOCATION_ERROR);
        return;
    }

    // Move or copy existing elements to the new memory
    memcpy(data, m_data, m_size * sizeof(void*));

    free(m_data);
    m_data = data;
    m_capacity = m_size;
}

}  // namespace noex
