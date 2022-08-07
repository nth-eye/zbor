#ifndef UTL_SVECTOR_H
#define UTL_SVECTOR_H

#include "utl/storage.h"

namespace utl {
namespace impl {

/**
 * @brief Common static vector implementation independent of the 
 * underlying storage container.
 * 
 * @tparam T Type of elements
 * @tparam N Maximum number of elements
 * @tparam Storage Underlying storage
 */
template<class T, size_t N, class Storage>
struct svector_base {

    // Get capacity of the vector.
    constexpr size_t capacity() const               { return N; }
    // Get size of the vector.
    constexpr size_t size() const                   { return len; }
    // Check if the vector is empty.
    constexpr bool empty() const                    { return len == 0; }
    // Check if the vector is full.
    constexpr bool full() const                     { return len == N; }
    // Get element at index.
    constexpr const T& operator[](size_t i) const   { return buf[i]; }
    // Get element at index without const.
    constexpr T& operator[](size_t i)               { return buf[i]; }
    // Get pointer to the beginning of the vector.
    constexpr const T* begin() const                { return buf; }
    // Get pointer to the beginning of the vector.
    constexpr T* begin()                            { return buf; }
    // Get pointer to the end of the vector.
    constexpr const T* end() const                  { return buf + len; }
    // Get pointer to the end of the vector.
    constexpr T* end()                              { return buf + len; }
    // Get pointer to the beginning of the vector.
    constexpr const T* data() const                 { return buf; }
    // Get pointer to the beginning of the vector.
    constexpr T* data()                             { return buf; }
    // Clear the vector.
    constexpr void clear()                          { len = 0; }
    // Resize the vector.
    constexpr void resize(size_t size)              { if (size <= N) len = size; }
    // Push element to the end of the vector.
    constexpr void push_back(const T &x)            { if (len < N) buf[len++] = x; }
    // Pop element from the end of the vector.
    constexpr void pop_back()                       { if (len > 0) --len; }
    // Erase element at iterator.
    constexpr void erase(T *p)
    {
        if (!len || p == &buf[--len])
            return;
        *p = buf[len];
    }
private:
    Storage buf;
    size_t len = 0;
};

}

/**
 * @brief Resizable static vector with pre-allocated memory on stack.
 * Uses utl::storage, so no constexpr support, but doesn't default 
 * construct elements. More efficient than utl::ce_svector at run-time.
 * 
 * @tparam T Type of elements
 * @tparam N Maximum number of elements
 */
template<class T, size_t N>
struct svector : impl::svector_base<T, N, storage<T, N>> {};

/**
 * @brief Resizable static vector with pre-allocated memory on stack.
 * Uses utl::ce_storage, so has constexpr support at the cost of default 
 * object construction. Less efficient than utl::svector at run-time.
 * 
 * @tparam T Type of elements
 * @tparam N Maximum number of elements
 */
template<class T, size_t N>
struct ce_svector : impl::svector_base<T, N, ce_storage<T, N>> {};

}

#endif