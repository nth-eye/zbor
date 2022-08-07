#ifndef UTL_STR_H
#define UTL_STR_H

#include "utl/math.h"
#include "utl/svector.h"
#include <string_view>

namespace utl {

/**
 * @brief Convert integer to hexadecimal character.
 * 
 * @param bin Byte value
 * @return Hexadecimal character 
 */
constexpr char bin_to_char(uint8_t bin)
{
    return "0123456789abcdef"[bin & 0xf];
}

/**
 * @brief Convert string to double, malformed input returns 0.
 * 
 * @param str Input string
 * @param len String length
 * @return Result 
 */
constexpr double str_to_dbl(const char *str, size_t len)
{
    if (!str || !len)
        return 0;

    const auto end = str + len;
    double neg = 1;
    double res = 0;
    int dot = 0;

    switch (*str) {
        case '-': neg = -1; [[fallthrough]];
        case '+': str += 1;
        break;
    }
    for (; str != end; ++str) {
        if (*str >= '0' && *str <= '9') {
            res = res * 10.0 + (*str - '0');
        } else {
            if (*str != '.' || dot)
                return 0;
            dot = end - str - 1;
        }
    }
    return res * neg / ipow(10, dot);
}

/**
 * @brief Convert string view to double, malformed input returns 0.
 * 
 * @param sv Input string view
 * @return Result
 */
constexpr double str_to_dbl(std::string_view sv)
{
    return str_to_dbl(sv.data(), sv.size());
}

/**
 * @brief Convert string to integer, malformed input returns 0.
 * 
 * @param str Input string
 * @param len String length
 * @return Result 
 */
constexpr long str_to_int(const char *str, size_t len)
{
    if (!str || !len)
        return 0;

    const auto end = str + len;
    long neg = 1;
    long res = 0;

    switch (*str) {
        case '-': neg = -1; [[fallthrough]];
        case '+': str += 1;
        break;
    }
    for (; str != end; ++str) {
        if (*str >= '0' && *str <= '9')
            res = res * 10 + (*str - '0');
        else
            return 0;
    }
    return res * neg;
}

/**
 * @brief Convert string view to integer, malformed input returns 0.
 * 
 * @param sv Input string view
 * @return Result
 */
constexpr long str_to_int(std::string_view sv)
{
    return str_to_int(sv.data(), sv.size());
}

/**
 * @brief Convert string with hexadecimal characters ('0'...'F') to array of bytes.
 * All non-hex chars will be mapped as 0. String with odd length will be interpeted
 * as with prepended '0', e.g. "fff" --> "0fff". Works with both upper and lower cases.
 * If output array is too small, as much as possible will be processed.
 * 
 * @param str Input string
 * @param str_len Input string length
 * @param bin Output array
 * @param max_bin_len Output array max size 
 * @return Length of resulting array, 0 if failed
 */
constexpr size_t str_to_bin(const char *str, size_t str_len, uint8_t *bin, size_t max_bin_len)
{
    if (!str || !bin)
        return 0;

    constexpr uint8_t map[256] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  !"#$%&'()*+,-./
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0123456789:;<=>?
        0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // @ABCDEFGHIJKLMNO
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // PQRSTUVWXYZ[\]^_
        0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // `abcdefghijklmno
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // pqrstuvwxyz{|}~.
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
    };
    size_t i = 0; 
    size_t j = 0;
    size_t bin_len = (str_len + 1) >> 1; // The output array size is half the str length (rounded up)

    if (bin_len > max_bin_len) {
        bin_len = max_bin_len;
        str_len = max_bin_len << 1; // return 0;
    }
    if (str_len & 1) {
        int idx = str[0];
        bin[0] = map[idx];
        i = j = 1;
    }
    for (; i < str_len; i += 2, j++) {
        int i0 = str[i];
        int i1 = str[i + 1];
        bin[j] = (map[i0] << 4) | map[i1];
    }
    return bin_len;
}

/**
 * @brief Convert byte array to hexadecimal lowercase string, NOT null-terminated. 
 * If input is too large for output, as much bytes as possible will be processed.
 * 
 * @param bin Input array
 * @param bin_len Input array length
 * @param str Output string
 * @param max_str_len Output string maximum length
 * @return Resulting string length, 0 if failed
 */
constexpr size_t bin_to_str(const uint8_t *bin, size_t bin_len, char *str, size_t max_str_len)
{
    if (!str || !bin || !bin_len || max_str_len < 2)
        return 0;

    size_t str_len = bin_len << 1;

    if (str_len >= max_str_len) {
        str_len = max_str_len & ~1;
        bin_len = str_len >> 1;
    }
    for (size_t i = 0; i < bin_len; ++i) {
        *str++ = bin_to_char(bin[i] >> 4);
        *str++ = bin_to_char(bin[i] & 0xf);
    }
    return str_len;
}

/**
 * @brief Convert byte array to hexadecimal lowercase null-terminated C string.
 * If input is too large for output, as much bytes as possible will be processed.
 * 
 * @param bin Input array
 * @param bin_len Input array length
 * @param str Output string
 * @param max_str_len Output string maximum length, including 0-terminator
 * @return Resulting string length, 0 if failed
 */
constexpr size_t bin_to_cstr(const uint8_t *bin, size_t bin_len, char *str, size_t max_str_len)
{
    if (!str || !bin || !bin_len || max_str_len < 3)
        return 0;

    size_t str_len = bin_len << 1;

    if (str_len >= max_str_len) {
        str_len = (max_str_len - 1) & ~1;
        bin_len = str_len >> 1;
    }
    for (size_t i = 0; i < bin_len; ++i) {
        *str++ = bin_to_char(bin[i] >> 4);
        *str++ = bin_to_char(bin[i] & 0xf);
    }
    *str = 0;

    return str_len;
}

/**
 * @brief Split string into tokens by given delimiter. Returns 
 * utl::ce_vector filled with std::string_view tokens.
 * 
 * @tparam N Maximum number of tokens
 * @param str String to split
 * @param del All delimiters characters
 * @return Static vector with split string
 */
template<size_t N>
constexpr auto split(std::string_view str, std::string_view del)
{
    ce_svector<std::string_view, N> tokens = {};

    size_t head = 0; 
    size_t tail = 0; 
 
    for (size_t i = 0; i < N && tail != std::string_view::npos; ++i) { 
        tail = str.find_first_of(del, head); 
        tokens.push_back(str.substr(head, tail - head)); 
        head = tail + 1; 
    } 
    return tokens;
}

}

#endif