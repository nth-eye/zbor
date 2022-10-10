#ifndef UTL_RING_H
#define UTL_RING_H

#include "utl/storage.h"

#define RING_ITER_SEPARATE  true

namespace utl {
namespace imp {

#if (RING_ITER_SEPARATE)
template<class T, size_t N>
struct ring_iter {

    // ANCHOR Member types

    using size_type         = size_t;
    using difference_type   = ptrdiff_t;
    using value_type        = T;
    using pointer           = value_type*;
    using reference         = value_type&;

    // ANCHOR Core

    ring_iter(pointer buf, size_type pos) : buf{buf}, pos{pos} {}
    ring_iter&  operator++()    { ++pos; return *this; }
    ring_iter&  operator--()    { --pos; return *this; }
    ring_iter   operator++(int) { auto tmp = *this; ++(*this); return tmp; }
    ring_iter   operator--(int) { auto tmp = *this; --(*this); return tmp; }
    reference   operator*()     { return buf[pos & (N - 1)]; }
    pointer     operator->()    { return &buf[pos & (N - 1)]; }
    friend bool operator==(const ring_iter& lhs, const ring_iter& rhs)  { return lhs.pos == rhs.pos; }
    friend bool operator!=(const ring_iter& lhs, const ring_iter& rhs)  { return lhs.pos != rhs.pos; }
private:
    pointer buf;
    size_type pos;
};
#endif

}

/**
 * @brief Ring buffer implementation with unmasked indices logic.
 * Provides storage capacity of N, which must be power of 2.
 * 
 * @tparam T Type of elements
 * @tparam N Maximum number of elements, must be power of 2
 * @tparam Discard Discard old elements when buffer is full
 */
template<class T, size_t N, bool Discard = false>
struct ring {

    // ANCHOR Member types

    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using universal_reference = value_type&&;
#if (RING_ITER_SEPARATE)
    using iterator = imp::ring_iter<value_type, N>;
#else
    struct iterator {
        iterator(pointer buf, size_type pos) : buf{buf}, pos{pos} {}
        iterator&   operator++()    { ++pos; return *this; }
        iterator&   operator--()    { --pos; return *this; }
        iterator    operator++(int) { auto tmp = *this; ++(*this); return tmp; }
        iterator    operator--(int) { auto tmp = *this; --(*this); return tmp; }
        reference   operator*()     { return access(buf, pos); }
        pointer     operator->()    { return &access(buf, pos); }
        friend bool operator==(const iterator& lhs, const iterator& rhs)    { return lhs.pos == rhs.pos; }
        friend bool operator!=(const iterator& lhs, const iterator& rhs)    { return lhs.pos != rhs.pos; }
    private:
        pointer buf;
        size_type pos;
    };
#endif
    using const_iterator = const iterator;

    // TODO Constructors

    // TODO Assingment operator

    // TODO Assign

    // ANCHOR Capacity

    constexpr static size_type capacity()       { return N; }
    constexpr static size_type max_size()       { return N; }
    constexpr size_type size() const noexcept   { return tail - head; }
    constexpr bool empty() const noexcept       { return tail == head; }
    constexpr bool full() const noexcept        { return size() == capacity(); }

    // ANCHOR Iterators

    constexpr iterator begin() noexcept                 { return {buf, head}; }
    constexpr const_iterator begin() const noexcept     { return {buf, head}; }
    constexpr const_iterator cbegin() const noexcept    { return begin(); }
    constexpr iterator end() noexcept                   { return {buf, tail}; }
    constexpr const_iterator end() const noexcept       { return {buf, tail}; }
    constexpr const_iterator cend() const noexcept      { return end(); }

    // ANCHOR Access

    constexpr reference operator[](size_type i)             { return at(head + i); }
    constexpr const_reference operator[](size_type i) const { return at(head + i); }
    constexpr reference front()                             { return at(head); }
    constexpr const_reference front() const                 { return at(head); }
    constexpr reference back()                              { return at(tail); }
    constexpr const_reference back() const                  { return at(tail); }

    // TODO Modifiers

    constexpr void resize(size_type n)
    {
        impl_resize(n);
    }
    
    constexpr void resize(size_type n, const_reference x)
    {
        impl_resize(n, x);
    }

    constexpr void clear() noexcept
    {
        for (auto &it : *this)
            dtor(&it);
        head = tail = 0;
    }

    constexpr pointer linearize()
    {
        // if ()
        return &at(head);
    }

    constexpr bool is_linearized() const
    {
        return (head & mask) <= (tail & mask); 
    }
private:
    static constexpr auto mask = N - 1;
    static_assert(N > 1 && !(mask & N), "ring_buf size must be > 1 and power of 2");
#if (RING_ITER_SEPARATE)
    constexpr reference at(size_type idx)
    {
        return buf[idx & mask];
    }
#else
    static constexpr reference access(pointer ptr, size_type idx)
    {
        return ptr[idx & mask];
    }
    constexpr reference at(size_type idx)
    {
        return access(buf, idx); // buf[idx & mask];
    }
#endif
    template<class... Args>
    constexpr void impl_resize(size_type n, Args&&... args)
    {
        assert(n <= capacity());
        // if (n > len)
        //     ctor_n(begin() + len, n - len, std::forward<Args>(args)...);
        // else
        //     dtor_n(begin() + n, len - n);
        // len = n;
    }
private:
    storage<T, N> buf;
    size_type head = 0; // First item index / beginning of the buffer
    size_type tail = 0; // Last item index
};

#if (false)
template<class T, size_t N, bool Discard = false>
struct ring {
    void pop()                          { head += !empty(); }
    void pop(size_t len)                { head += len < size() ? len : size(); }
    void put(const T &item)	
    {
        if (full()) {
            if constexpr (Discard) 
                pop();
            else
                return;
        }
        buf[tail++ & mask] = item;
    }
    template<class... Args>
    void emplace(Args... args)
    {
        put(T{args...});
    }
    bool get(T &item)
    { 
        if (empty()) return false;
        item = buf[head & mask];
        pop(); 
        return true; 
    }
private:
    storage<T, N> buf;
    size_t head = 0;   // First item index / beginning of the buffer.
    size_t tail = 0;   // Last item index.
};
#endif 

}

#undef RING_ITER_SEPARATE

#endif