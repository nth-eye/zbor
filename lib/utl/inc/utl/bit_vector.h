#ifndef UTL_BIT_VECTOR_H
#define UTL_BIT_VECTOR_H

#define BIT_VECTOR_DEBUG    false

#if !(BIT_VECTOR_DEBUG)
#define PRINT(...)  void()
#else
#define PRINT(...)  printf(__VA_ARGS__)
#include <cstdio>
#endif
#include "utl/bit.h"
#include <array>

namespace utl {
namespace imp {

struct bit_tree_level {
    size_t head;
    size_t size;
    size_t remainder;
};

template<class T>
constexpr auto bit_tree_depth(size_t bits, size_t grow_point)
{
    size_t depth = 0;
    while (1) {
        ++depth;
        bits = bit_ceil<T>(bits);
        if (bits < bit_size<T>()) {
            if (bits > grow_point)
                ++depth;
            break;
        }
    }
    return depth;
}

template<class T, size_t N, size_t G>
constexpr auto bit_tree_struct()
{
    static_assert(N, "bit_vector size must be > 0");
    static_assert(G && G < bit_size<T>(), "grow point G must be [0 < G < sizeof(word) * 8]");
    std::array<bit_tree_level, bit_tree_depth<T>(N, G)> arr = {};
    size_t bits = N;
    size_t head = 0;
    for (auto& it : arr) {
        it.remainder = bits & bit_wrap<T>();
        it.head = head;
        it.size = bits = bit_ceil<T>(bits);
        head += bits;
    }
    return arr;
}

}

template<class T, size_t N, size_t G>
struct bit_vector {
    constexpr bit_vector()
    {
        for (const auto& [head, size, remainder] : levels) {
            if (remainder) {
                buf[head + size - 1] = ~(bit(remainder) - 1);
            }
        }
    }
    static constexpr size_t depth()
    {
        return levels.size();
    }
    static constexpr size_t words()
    {
        return levels.back().head + levels.back().size;
    }
    constexpr bool operator[](size_t pos) const
    { 
        assert(pos < N);
        return get_arr_bit(buf, pos); 
    }
    constexpr void set(size_t pos)
    {
        assert(pos < N);
        PRINT("set %lu \n", pos);
        if constexpr (depth() == 1) {
            set_arr_bit(buf, pos); 
            return;
        }
        size_t bit;
        for (const auto &it : levels) {
            bit = pos & bit_wrap<T>();
            pos = pos >> bit_shft<T>();
            PRINT("set bit at [%lu]:%lu \n", it.head + pos, bit);
            set_bit(buf[it.head + pos], bit);
            if (buf[it.head + pos] != bit_full<T>())
                break;
        }
    }
    constexpr void clr(size_t pos)
    { 
        assert(pos < N);
        PRINT("cleared %lu \n", pos);
        if constexpr (depth() == 1) {
            clr_arr_bit(buf, pos); 
            return;
        }
        size_t bit;
        for (const auto &it : levels) {
            bit = pos & bit_wrap<T>();
            pos = pos >> bit_shft<T>();
            PRINT("clr bit at [%lu]:%lu \n", it.head + pos, bit);
            if (buf[it.head + pos] == bit_full<T>()) {
                clr_bit(buf[it.head + pos], bit);
                continue;
            } else {
                clr_bit(buf[it.head + pos], bit);
                break;
            }
        }
    }
    constexpr auto acquire_any()
    {
        const auto &end = levels.back();
        for (size_t i = 0; i < end.size; ++i) {
            if (buf[end.head + i] != bit_full<T>()) {
                if constexpr (depth() == 1) {
                    size_t pos = cnttz(~buf[i]);
                    set_bit(buf[i], pos);
                    PRINT("acquired %lu \n", (i << bit_shft<T>()) + pos);
                    return (i << bit_shft<T>()) + pos;
                } else {
                    size_t pos = (i << bit_shft<T>()) + cnttz(~buf[end.head + i]);
                    for (auto it = levels.rbegin() + 1; it != levels.rend(); ++it) {
                        pos = (pos << bit_shft<T>()) + cnttz(~buf[it->head + pos]);
                    }
                    set(pos);
                    PRINT("acquired %lu \n", pos);
                    return pos;
                }
            }
        }
        return N;
    }
private:
    static constexpr auto levels = imp::bit_tree_struct<T, N, G>();
private:
    T buf[words()] = {};
};

template<class type, size_t bits, size_t grow>
void print_bit_tree_info() 
{
    auto array = imp::bit_tree_struct<type, bits, grow>();
    auto depth = imp::bit_tree_depth<type>(bits, grow);
    auto words = sizeof(bit_vector<type, bits, grow>) / sizeof(type);
    PRINT("| b_%-10lu ", bits);
    PRINT("| w_%-9lu ", words);
    PRINT("| d_%-1lu ", depth);
    PRINT("[%lu]: ", array.size());
    size_t cnt = 0;
    for (auto [head, size, remainder] : array) {
        cnt += size;
        PRINT("{%lu:%lu:%lu} ", head, size, remainder);
    }
    if (depth != array.size() || cnt != words)
        PRINT("! NOT OKAY !");
    PRINT("\n");
}

}

#undef BIT_VECTOR_DEBUG

#endif
