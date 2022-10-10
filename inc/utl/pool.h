#ifndef UTL_POOL_H
#define UTL_POOL_H

#include "utl/bit_vector.h"
#include "utl/storage.h"

namespace utl {

/**
 * @brief Static object pool with pre-allocated memory on stack.
 * Uses utl::bit_vector to indicate positions of available elements.
 * Cannot be used in constexpr context because of std::unique_ptr, 
 * even though custom deleter doesn't deallocate any actual memory.
 * 
 * @tparam T Type of elements
 * @tparam N Number of elements in pool
 * @tparam G Grow point of utl::bit_vector, look its description
 * @tparam W Word type of utl::bit_vector, look its description
 */
template<class T, size_t N, size_t G = 4, class W = uint32_t>
struct pool {

    // ANCHOR Member types

    using value_type = T;
    using size_type = size_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    struct releaser {
        void operator()(pointer obj) {
            if (obj) {
                dtor(obj);
                ptr->bits.clr(obj - ptr->buf);
                ptr->available++;
            }
        }
        pool* ptr;
    };
    using return_type = std::unique_ptr<value_type, releaser>;

    // ANCHOR Constructors

    pool() = default;
    pool(const pool&) = delete;
    pool(pool&&) = delete; 
    pool& operator=(const pool&) = delete; 
    pool& operator=(pool&&) = delete; 

    // ANCHOR Capacity

    constexpr static size_type capacity()       { return N; }
    constexpr size_type left() const noexcept   { return available; }
    constexpr bool empty() const noexcept       { return available == 0; }

    // ANCHOR Modifiers

    template<typename... Args>
    constexpr auto get(Args&&... args)
    {
        auto acquire = [this] (pointer ptr) {
            return return_type(ptr, releaser{this}); 
        };
        if (empty()) return acquire(nullptr);
        --available; 
        return acquire(ctor(&buf[bits.acquire_any()], std::forward<Args>(args)...));
    }
private:
    storage<T, N> buf;
    size_type available = N;
    bit_vector<W, N, G> bits;
};

}

#endif