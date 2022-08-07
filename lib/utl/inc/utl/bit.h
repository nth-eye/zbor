#ifndef UTL_BIT_H
#define UTL_BIT_H

#include "utl/base.h"

namespace utl {

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
 * @brief Make integer with bit at given position.
 * 
 * @tparam T Inetegr type
 * @param n Bit position
 * @return Integer with set bit
 */
template<class T = unsigned> 
constexpr T bit(int n)  
{  
    return T(1) << n;  
}

/**
 * @brief Get n-th bit of an integer.
 * 
 * @tparam T Integer type
 * @param x Integer
 * @param n Bit position from LSB
 * @return true if set
 */
template<class T>
constexpr bool get_bit(T x, int n)
{ 
    return (x >> n) & 1; 
}

/**
 * @brief Set n-th bit of an integer.
 * 
 * @tparam T Integer type
 * @param x Integer
 * @param n Bit position from LSB
 */
template<class T>
constexpr void set_bit(T &x, int n)
{ 
    x |= 1 << n; 
}

/**
 * @brief Clear n-th bit of an integer.
 * 
 * @tparam T Integer type
 * @param x Integer
 * @param n Bit position from LSB
 */
template<class T>
constexpr void clr_bit(T &x, int n)
{ 
    x &= ~(1 << n); 
}

/**
 * @brief Get n-th bit of an array (starting from LSB).
 * 
 * @param arr Array of bytes
 * @param n Index of bit to get
 * @return true if set
 */
constexpr bool get_arr_bit(const uint8_t *arr, int n)
{
    return get_bit(arr[n >> 3], n & 7);
}

/**
 * @brief Set n-th bit in an array of bytes (starting from LSB).
 * 
 * @param arr Array of bytes
 * @param n Index of bit to set
 */
constexpr void set_arr_bit(uint8_t *arr, int n)
{
    set_bit(arr[n >> 3], n & 7);
}

/**
 * @brief Clear n-th bit in an array of bytes (starting from LSB).
 * 
 * @param arr Array of bytes
 * @param n Index of bit to clear
 */
constexpr void clr_arr_bit(uint8_t *arr, int n)
{
    clr_bit(arr[n >> 3], n & 7);
}

/**
 * @brief Shift bits left in array of integer elements from least significant bit 
 * and considering 0-th byte as the right most.
 * uint16_t example: 0b10000000'11100001 ==> 0b00000001'11000010. 
 * 
 * @tparam T Integer type
 * @tparam L Length of array
 * @param x Array of integers, nullptr not acceptable!
 * @param len Number of elements
 */
template<class T, size_t L>
constexpr void shift_left(T (&x)[L])
{
    for (int i = L - 1; i > 0; --i) {
        x[i] <<= 1;
        x[i] |= x[i - 1] >> (sizeof(T) * 8 - 1);
    }
    x[0] <<= 1;
}

}

#endif