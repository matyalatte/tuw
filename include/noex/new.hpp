#pragma once

namespace noex {

template <typename T>
T* new_ref() {
    T* obj = reinterpret_cast<T*>(calloc(1, sizeof(T)));
    if (obj) {
        if (std::is_trivial<T>::value)
            new (obj) T();
    } else {
        set_error_no(NEW_ALLOCATION_ERROR);
    }
    return obj;
}

template <typename T>
void del_ref(T* obj) {
    if (!obj)
        return;
    if (std::is_trivial<T>::value)
        obj->~T();
    free(obj);
}

}  // namespace noex
