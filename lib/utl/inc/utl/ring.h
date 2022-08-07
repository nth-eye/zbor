#ifndef UTL_RING_H
#define UTL_RING_H

#include "utl/storage.h"

namespace utl {

/**
 * @brief Ring buffer implementation with unmasked indices logic.
 * Provides storage capacity of N, which must be power of 2.
 * 
 * @tparam T Type of elements
 * @tparam N Maximum number of elements, must be power of 2
 * @tparam Discard Discard old elements when buffer is full, default is false
 */
template<class T, size_t N, bool Discard = false>
struct ring {
private:
    static constexpr size_t mask = N - 1;
    static_assert(N > 1 && !(mask & N), "ring_buf size must be > 1 and power of 2"); 
    struct iter {
        iter(T *buf, size_t pos) : buf{buf}, pos{pos} {}
        void operator++()                       { ++pos; }
        bool operator!=(const iter &rhs) const  { return pos != rhs.pos; }
        T&   operator*()                        { return buf[pos & mask]; }
    private:
        T *buf;
        size_t pos;
    };
public:
    const iter begin() const            { return {buf, head}; }
    const iter end() const              { return {buf, tail}; }
    iter begin()                        { return {buf, head}; }
    iter end()                          { return {buf, tail}; }
    const T& front() const              { return buf[head & mask]; }
    const T& back() const               { return buf[tail & mask]; }
    T& front()                          { return buf[head & mask]; }
    T& back()                           { return buf[tail & mask]; }
    size_t constexpr capacity() const   { return N; }
    size_t size() const                 { return tail - head; }
    bool empty() const                  { return tail == head; }
    bool full() const                   { return size() == N; }
    void clear()                        { head = tail = 0; }
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

}

#endif