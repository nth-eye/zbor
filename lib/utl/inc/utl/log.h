#ifndef UTL_LOG_H
#define UTL_LOG_H

#include "utl/bit.h"
#include "utl/str.h"
#include <cctype>
#include <cstdio>

namespace utl {

/**
 * @brief Print hex nicely with relevant ASCII representation.
 * 
 * @param dat Data to print
 * @param len Length in bytes
 */
inline void log_hex(const void *dat, size_t len)
{
    if (!dat || !len)
        return;

    auto p = static_cast<const uint8_t*>(dat);

    for (size_t i = 0; i < len; ++i) {

        if (!(i & 15)) {
            putchar('|');
            putchar(' ');
        }
        putchar(bin_to_char(p[i] >> 4));
        putchar(bin_to_char(p[i] & 0xF));
        putchar(' ');
        
        if ((i & 7) == 7)
            putchar(' ');

        if ((i & 15) == 15) {
            putchar('|');
            for (int j = 15; j >= 0; --j) {
                char c = p[i - j];
                putchar(isprint(c) ? c : '.');
            }
            putchar('|');
            putchar('\n');
        }
    }
    int rem = len - ((len >> 4) << 4);
    if (rem) {
        for (int j = (16 - rem) * 3 + ((~rem & 8) >> 3); j >= 0; --j)
            putchar(' ');
        putchar('|');
        for (int j = rem; j; --j) {
            char c = p[len - j];
            putchar(isprint(c) ? c : '.');
        }
        for (int j = 0; j < 16 - rem; ++j)
            putchar('.');
        putchar('|');
        putchar('\n');
    }
}

/**
 * @brief Print bits nicely from offset position with MSB at left and relevant ASCII.
 * 
 * @param dat Memory with bits to log
 * @param len Number of bits
 * @param pos Bit offset in initial byte, be careful to not go out of memory region
 */
inline void log_bits(const void *dat, size_t len, size_t pos)
{
    if (!dat || !len)
        return;

    auto ptr = static_cast<const uint8_t*>(dat);
    auto rem = len & 7;

    uint8_t line[8];

    for (size_t i = 0; i < len; ++i) {

        if (!(i & 63)) {
            putchar('|');
            putchar(' ');
            for (auto &c : line) c = 0;
        }

        if (get_arr_bit(ptr, pos++))
            set_arr_bit(line, i & 63);
            
        if (rem && i == len - 1) {
            uint8_t c = line[(i & 63) >> 3];
            for (int j = rem - 1; j >= 0; --j)
                putchar(get_bit(c, j) + '0');
        }

        if ((i & 7) == 7) {
            uint8_t c = line[(i & 63) >> 3];
            for (int j = 7; j >= 0; --j)
                putchar(get_bit(c, j) + '0');
            putchar(' ');
        }

        if ((i & 63) == 63) {
            putchar(' ');
            putchar('|');
            for (int j = 0; j < 8; ++j) {
                char c = line[j];
                putchar(isprint(c) ? c : '.');
            }
            putchar('|');
            putchar('\n');
        }
    }

    int rem_bits = len - ((len >> 6) << 6);
    int rem_bytes = rem_bits >> 3;
    int fill_bits = 64 - rem_bits;
    int fill_bytes = fill_bits >> 3;

    if (rem_bits) {
        for (int j = fill_bits + bytes_in_bits(fill_bits); j >= 0; --j)
            putchar(' ');
        putchar('|');
        for (int j = 0; j < rem_bytes; ++j) {
            char c = line[j];
            putchar(isprint(c) ? c : '.');
        }
        if (rem)
            putchar('.');
        for (int j = 0; j < fill_bytes; ++j)
            putchar('.');
        putchar('|');
        putchar('\n');
    }
}

}

#endif
