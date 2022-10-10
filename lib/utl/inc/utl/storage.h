#ifndef UTL_STORAGE_H
#define UTL_STORAGE_H

#include "utl/base.h"
#include <memory>
#include <type_traits>

namespace utl {

/**
 * @brief Uninitialized union container for any type. Uses regular C 
 * array as underlying storage. Can be used in constexpr context and
 * converts implicitly to pointer of underlying type.
 * 
 * @tparam T Type of elements
 * @tparam N Number of elements
 */
template<class T, size_t N>
union storage {
    using type = T;
    static constexpr size_t size = N;
    static_assert(N);
    constexpr storage() noexcept {}
    constexpr ~storage() noexcept {}
    constexpr operator auto()       { return buf; }
    constexpr operator auto() const { return buf; }
private:
    T buf[N];
};

/**
 * @brief Properly destoy object of any given type inplace.
 * 
 * @tparam T Object type
 * @param p Pointer to object
 */
template<class T>
constexpr void dtor(T* p) 
{
    std::destroy_at(p);
}

/**
 * @brief Properly destroy range of objects.
 * 
 * @tparam It Iterator type
 * @param head Iterator of first element
 * @param tail Iterator after last element
 */
template<class It>
constexpr void dtor(It head, It tail)
{
    std::destroy(head, tail);
}

/**
 * @brief Properly destroy N objects.
 * 
 * @tparam It Iterator type
 * @param p Iterator of first element
 * @param n Number of elements
 */
template<class It>
constexpr void dtor_n(It p, size_t n)
{
    std::destroy_n(p, n);
}

/**
 * @brief Create a T object initialized with arguments at address.
 * 
 * @tparam T Object type
 * @tparam Args Arguments used for initialization
 * @param p Pointer to the uninitialized storage
 * @param args Arguments used for initialization
 * @return Pointer to constructed object
 */
template<class T, class... Args>
constexpr T* ctor(T* p, Args&&... args)
{
    return std::construct_at(p, std::forward<Args>(args)...);
}

/**
 * @brief Create N number of T objects all initialized with arguments.
 * 
 * @tparam T Object type
 * @tparam Args Arguments used for initialization
 * @param p Pointer to the uninitialized storage for first object
 * @param n Number of objects to construct
 * @param args Arguments used for initialization
 */
template<class T, class... Args>
constexpr void ctor_n(T* p, size_t n, Args&&... args)
{
    for (size_t i = 0; i < n; ++i)
        ctor(p + i, std::forward<Args>(args)...);
}

template<class InIt, class OutIt>
constexpr OutIt move_create_backward(InIt first, InIt last, OutIt d_last)
{
    while (first != last)
        ctor(--d_last, std::move(*--last));
    return d_last;
}

template<class InIt, class OutIt>
constexpr OutIt move_create_forward(InIt first, InIt last, OutIt d_first)
{
    while (first != last)
        ctor(d_first++, std::move(*first++));
    return d_first;
}

template<class InIt, class OutIt>
constexpr OutIt move_assign_backward(InIt first, InIt last, OutIt d_last)
{
    while (first != last)
        *--d_last = std::move(*--last);
    return d_last;
}

template<class InIt, class OutIt>
constexpr OutIt move_assign_forward(InIt first, InIt last, OutIt d_first)
{
    while (first != last)
        *d_first++ = std::move(*first++);
    return d_first;
}

template<class InIt, class OutIt>
constexpr OutIt copy_create_forward(InIt first, InIt last, OutIt d_first)
{
    while (first != last)
        ctor(d_first++, *first++);
    return d_first;
}

template<class InIt, class OutIt>
constexpr OutIt copy_assign_forward(InIt first, InIt last, OutIt d_first)
{
    while (first != last)
        *d_first++ = *first++;
    return d_first;
}

}

#endif