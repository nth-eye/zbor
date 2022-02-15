#ifndef ZBOR_UTIL_H
#define ZBOR_UTIL_H

#include <cstdint>
#include <cstddef>

namespace zbor {

/**
 * @brief Get number of bytes required to store N bits.
 * 
 * @param n Number of bits to store
 * @return Number of bytes
 */
constexpr size_t bytes_in_bits(size_t n)
{ 
    return (n >> 3) + !!(n & 7); 
}

/**
 * @brief Integer with bit set at given position.
 * 
 * @param n Bit position
 * @return Integer with set bit
 */
constexpr unsigned bit(unsigned n) 
{ 
    return 1ul << n; 
}

/**
 * @brief Get n-th bit of a byte.
 * 
 * @param b Byte
 * @param n Bit position from LSB
 * @return Bit value
 */
constexpr bool get_bit(uint8_t b, unsigned n)
{ 
    return (b >> n) & 1; 
}

/**
 * @brief Set n-th bit of a byte.
 * 
 * @param b Byte
 * @param n Bit position from LSB
 */
inline void set_bit(uint8_t &b, unsigned n)
{ 
    b |= 1 << n; 
}

/**
 * @brief Clear n-th bit of a byte.
 * 
 * @param b Byte
 * @param n Bit position from LSB
 */
inline void clr_bit(uint8_t &b, unsigned n)
{ 
    b &= ~(1 << n); 
}

/**
 * @brief Static object pool. Uses individual bits to indicate if element is free.
 * 
 * @tparam T Type of elements
 * @tparam N Number of elements
 */
template<class T, size_t N>
struct StaticPool {

    static_assert(N, "pool size must be > 0");
    static constexpr size_t Bytes = bytes_in_bits(N);

    template<typename... Args>
    T* make(Args... args)
    {
        constexpr auto last_byte = Bytes - 1;
        constexpr auto last_bit = N & 7 ? N & 7 : 8;

        size_t byte = last_byte;

        for (size_t i = 0; i < last_byte; ++i) {
            if (taken[i] != 0xff) {
                byte = i;
                break;
            }
        }
        for (unsigned bit = 0; bit < last_bit; ++bit) {
            if (get_bit(taken[byte], bit) == false) {
                set_bit(taken[byte], bit);
                return &(arr[(byte << 3) + bit] = T(args...));
            }
        }
        return nullptr;
    }

    void free(const T *p)
    {
        const size_t n = p - arr;
        if (n >= N)
            return;
        clr_bit(taken[n >> 3], n & 7);
    }
private:
    T       arr[N];
    uint8_t taken[Bytes] = {};
};

}

#endif