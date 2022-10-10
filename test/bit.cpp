#include <gtest/gtest.h>
#include "utl/bit.h"

using namespace utl;

TEST(Bit, BitSize)
{
    ASSERT_EQ(bit_size<uint8_t>(), 8);
    ASSERT_EQ(bit_size<uint16_t>(), 16);
    ASSERT_EQ(bit_size<uint32_t>(), 32);
    ASSERT_EQ(bit_size<uint64_t>(), 64);
}

TEST(Bit, BitFull)
{
    ASSERT_EQ(bit_full<uint8_t>(), 0xff);
    ASSERT_EQ(bit_full<uint16_t>(), 0xffff);
    ASSERT_EQ(bit_full<uint32_t>(), 0xffffffff);
    ASSERT_EQ(bit_full<uint64_t>(), 0xffffffffffffffff);
}

TEST(Bit, BitWrap)
{
    ASSERT_EQ(bit_wrap<uint8_t>(), 0b111);
    ASSERT_EQ(bit_wrap<uint16_t>(), 0b1111);
    ASSERT_EQ(bit_wrap<uint32_t>(), 0b11111);
    ASSERT_EQ(bit_wrap<uint64_t>(), 0b111111);
}

TEST(Bit, BitShift)
{
    ASSERT_EQ(bit_shft<uint8_t>(), 3);
    ASSERT_EQ(bit_shft<uint16_t>(), 4);
    ASSERT_EQ(bit_shft<uint32_t>(), 5);
    ASSERT_EQ(bit_shft<uint64_t>(), 6);
}

TEST(Bit, BitCeil)
{
    auto bc8 = [](int x) {return bit_ceil<uint8_t>(x);};
    ASSERT_EQ(bc8(0), 0);
    ASSERT_EQ(bc8(1), 1);
    ASSERT_EQ(bc8(7), 1);
    ASSERT_EQ(bc8(8), 1);
    ASSERT_EQ(bc8(42), 6);
    ASSERT_EQ(bc8(63), 8);
    ASSERT_EQ(bc8(64), 8);
    ASSERT_EQ(bc8(69), 9);
    ASSERT_EQ(bc8(1337), 168);

    auto bc16 = [](int x) {return bit_ceil<uint16_t>(x);};
    ASSERT_EQ(bc16(0), 0);
    ASSERT_EQ(bc16(1), 1);
    ASSERT_EQ(bc16(15), 1);
    ASSERT_EQ(bc16(16), 1);
    ASSERT_EQ(bc16(42), 3);
    ASSERT_EQ(bc16(63), 4);
    ASSERT_EQ(bc16(64), 4);
    ASSERT_EQ(bc16(69), 5);
    ASSERT_EQ(bc16(1337), 84);

    auto bc32 = [](int x) {return bit_ceil<uint32_t>(x);};
    ASSERT_EQ(bc32(0), 0);
    ASSERT_EQ(bc32(1), 1);
    ASSERT_EQ(bc32(31), 1);
    ASSERT_EQ(bc32(32), 1);
    ASSERT_EQ(bc32(42), 2);
    ASSERT_EQ(bc32(63), 2);
    ASSERT_EQ(bc32(64), 2);
    ASSERT_EQ(bc32(69), 3);
    ASSERT_EQ(bc32(1337), 42);

    auto bc64 = [](int x) {return bit_ceil<uint64_t>(x);};
    ASSERT_EQ(bc64(0), 0);
    ASSERT_EQ(bc64(1), 1);
    ASSERT_EQ(bc64(42), 1);
    ASSERT_EQ(bc64(63), 1);
    ASSERT_EQ(bc64(64), 1);
    ASSERT_EQ(bc64(69), 2);
    ASSERT_EQ(bc64(1337), 21);
    ASSERT_EQ(bc64(4096), 64);
}

TEST(Bit, CountLeadingZeros)
{
    ASSERT_EQ(cntlz(0x80000000), 0);
    ASSERT_EQ(cntlz(0x01000000), 7);
    ASSERT_EQ(cntlz(0x00800000), 8);
    ASSERT_EQ(cntlz(0x00010000), 15);
    ASSERT_EQ(cntlz(0x00008000), 16);
    ASSERT_EQ(cntlz(0x00000100), 23);
    ASSERT_EQ(cntlz(0x00000080), 24);
    ASSERT_EQ(cntlz(0x00000001), 31);
    ASSERT_EQ(cntlz(0x00000000), 32);

    ASSERT_EQ(cntlz(0xfb5f2e2c), 0);
    ASSERT_EQ(cntlz(0x58835269), 1);
    ASSERT_EQ(cntlz(0xf9a63895), 0);
    ASSERT_EQ(cntlz(0xeb157265), 0);
    ASSERT_EQ(cntlz(0x25d613b2), 2);
    ASSERT_EQ(cntlz(0x7a0aa4ee), 1);
    ASSERT_EQ(cntlz(0x2e744e63), 2);
    ASSERT_EQ(cntlz(0x8bb413d2), 0);
    ASSERT_EQ(cntlz(0x54fe413f), 1);
}

TEST(Bit, CountTrailingZeros)
{
    ASSERT_EQ(cnttz(0x00000000), 32);
    ASSERT_EQ(cnttz(0x80000000), 31);
    ASSERT_EQ(cnttz(0x01000000), 24);
    ASSERT_EQ(cnttz(0x00800000), 23);
    ASSERT_EQ(cnttz(0x00010000), 16);
    ASSERT_EQ(cnttz(0x00008000), 15);
    ASSERT_EQ(cnttz(0x00000100), 8);
    ASSERT_EQ(cnttz(0x00000080), 7);
    ASSERT_EQ(cnttz(0x00000001), 0);

    ASSERT_EQ(cnttz(0xfb5f2e2c), 2);
    ASSERT_EQ(cnttz(0x58835269), 0);
    ASSERT_EQ(cnttz(0xf9a63895), 0);
    ASSERT_EQ(cnttz(0xeb157265), 0);
    ASSERT_EQ(cnttz(0x25d613b2), 1);
    ASSERT_EQ(cnttz(0x7a0aa4ee), 1);
    ASSERT_EQ(cnttz(0x2e744e63), 0);
    ASSERT_EQ(cnttz(0x8bb413d2), 1);
    ASSERT_EQ(cnttz(0x54fe413f), 0);
}

TEST(Bit, WordsIntBits)
{
    auto wib8 = [](size_t x) {return words_in_bits<uint8_t>(x);};
    ASSERT_EQ(wib8(0), 0);
    ASSERT_EQ(wib8(1), 1);
    ASSERT_EQ(wib8(7), 1);
    ASSERT_EQ(wib8(8), 1);
    ASSERT_EQ(wib8(9), 2);
    ASSERT_EQ(wib8(42), 6);
    ASSERT_EQ(wib8(64), 8);
    ASSERT_EQ(wib8(69), 9);
    ASSERT_EQ(wib8(228), 29);
    ASSERT_EQ(wib8(9999), 1250);

    auto wib16 = [](size_t x) {return words_in_bits<uint16_t>(x);};
    ASSERT_EQ(wib16(0), 0);
    ASSERT_EQ(wib16(1), 1);
    ASSERT_EQ(wib16(15), 1);
    ASSERT_EQ(wib16(16), 1);
    ASSERT_EQ(wib16(17), 2);
    ASSERT_EQ(wib16(42), 3);
    ASSERT_EQ(wib16(64), 4);
    ASSERT_EQ(wib16(69), 5);
    ASSERT_EQ(wib16(228), 15);
    ASSERT_EQ(wib16(9999), 625);

    auto wib32 = [](size_t x) {return words_in_bits<uint32_t>(x);};
    ASSERT_EQ(wib32(0), 0);
    ASSERT_EQ(wib32(1), 1);
    ASSERT_EQ(wib32(31), 1);
    ASSERT_EQ(wib32(32), 1);
    ASSERT_EQ(wib32(33), 2);
    ASSERT_EQ(wib32(42), 2);
    ASSERT_EQ(wib32(64), 2);
    ASSERT_EQ(wib32(69), 3);
    ASSERT_EQ(wib32(228), 8);
    ASSERT_EQ(wib32(9999), 313);

    auto wib64 = [](size_t x) {return words_in_bits<uint64_t>(x);};
    ASSERT_EQ(wib64(0), 0);
    ASSERT_EQ(wib64(1), 1);
    ASSERT_EQ(wib64(63), 1);
    ASSERT_EQ(wib64(64), 1);
    ASSERT_EQ(wib64(65), 2);
    ASSERT_EQ(wib64(42), 1);
    ASSERT_EQ(wib64(64), 1);
    ASSERT_EQ(wib64(69), 2);
    ASSERT_EQ(wib64(228), 4);
    ASSERT_EQ(wib64(9999), 157);
}

TEST(Bit, BytesInBits)
{
    ASSERT_EQ(bytes_in_bits(0), 0);
    ASSERT_EQ(bytes_in_bits(1), 1);
    ASSERT_EQ(bytes_in_bits(7), 1);
    ASSERT_EQ(bytes_in_bits(8), 1);
    ASSERT_EQ(bytes_in_bits(9), 2);
    ASSERT_EQ(bytes_in_bits(42), 6);
    ASSERT_EQ(bytes_in_bits(64), 8);
    ASSERT_EQ(bytes_in_bits(69), 9);
    ASSERT_EQ(bytes_in_bits(228), 29);
    ASSERT_EQ(bytes_in_bits(9999), 1250);
}

TEST(Bit, MakeBit)
{
    ASSERT_EQ(bit(0), 1);
    ASSERT_EQ(bit(1), 2);
    ASSERT_EQ(bit(7), 128);
    ASSERT_EQ(bit(31), 2147483648);
    ASSERT_EQ(bit<uint64_t>(63), uint64_t(9223372036854775808ull));
}

TEST(Bit, GetBit)
{
    ASSERT_EQ(get_bit(0, 0), 0);
    ASSERT_EQ(get_bit(0, 1), 0);
    ASSERT_EQ(get_bit(0, 2), 0);
    ASSERT_EQ(get_bit(0, 3), 0);
    ASSERT_EQ(get_bit(0, 4), 0);
    ASSERT_EQ(get_bit(0, 5), 0);
    ASSERT_EQ(get_bit(0, 6), 0);
    ASSERT_EQ(get_bit(0, 7), 0);
    ASSERT_EQ(get_bit(42, 0), 0);
    ASSERT_EQ(get_bit(42, 1), 1);
    ASSERT_EQ(get_bit(42, 2), 0);
    ASSERT_EQ(get_bit(42, 3), 1);
    ASSERT_EQ(get_bit(42, 4), 0);
    ASSERT_EQ(get_bit(42, 5), 1);
    ASSERT_EQ(get_bit(42, 6), 0);
    ASSERT_EQ(get_bit(42, 7), 0);
    ASSERT_EQ(get_bit(255, 0), 1);
    ASSERT_EQ(get_bit(255, 1), 1);
    ASSERT_EQ(get_bit(255, 2), 1);
    ASSERT_EQ(get_bit(255, 3), 1);
    ASSERT_EQ(get_bit(255, 4), 1);
    ASSERT_EQ(get_bit(255, 5), 1);
    ASSERT_EQ(get_bit(255, 6), 1);
    ASSERT_EQ(get_bit(255, 7), 1);
}

TEST(Bit, SetBit)
{
    uint8_t b = 0;

    set_bit(b, 0);
    set_bit(b, 1);
    set_bit(b, 5);

    ASSERT_EQ(b, 0x23);

    set_bit(b, 9);

    ASSERT_EQ(b, 0x23);
}

TEST(Bit, ClearBit)
{
    uint8_t b = 0x23;

    clr_bit(b, 0);
    clr_bit(b, 1);
    clr_bit(b, 6);

    ASSERT_EQ(b, 0x20);

    clr_bit(b, 9);

    ASSERT_EQ(b, 0x20);

    clr_bit(b, 5);

    ASSERT_EQ(b, 0x00);
}

TEST(Bit, GetArrayBit)
{
    const uint8_t arr[] = { 0x02, 0x78 };

    ASSERT_EQ(get_arr_bit(arr, 0), 0);
    ASSERT_EQ(get_arr_bit(arr, 1), 1);
    ASSERT_EQ(get_arr_bit(arr, 2), 0);
    ASSERT_EQ(get_arr_bit(arr, 3), 0);
    ASSERT_EQ(get_arr_bit(arr, 4), 0);
    ASSERT_EQ(get_arr_bit(arr, 5), 0);
    ASSERT_EQ(get_arr_bit(arr, 6), 0);
    ASSERT_EQ(get_arr_bit(arr, 7), 0);
    ASSERT_EQ(get_arr_bit(arr, 8), 0);
    ASSERT_EQ(get_arr_bit(arr, 9), 0);
    ASSERT_EQ(get_arr_bit(arr, 10), 0);
    ASSERT_EQ(get_arr_bit(arr, 11), 1);
    ASSERT_EQ(get_arr_bit(arr, 12), 1);
    ASSERT_EQ(get_arr_bit(arr, 13), 1);
    ASSERT_EQ(get_arr_bit(arr, 14), 1);
    ASSERT_EQ(get_arr_bit(arr, 15), 0);
}

TEST(Bit, SetArrayBit)
{
    uint8_t arr[] = { 0x02, 0x15 };

    set_arr_bit(arr, 0);
    set_arr_bit(arr, 1);
    set_arr_bit(arr, 5);
    set_arr_bit(arr, 14);

    ASSERT_EQ(arr[0], 0x23);
    ASSERT_EQ(arr[1], 0x55);
}

TEST(Bit, ClearArrayBit)
{
    uint8_t arr[] = { 0x23, 0x55 };

    clr_arr_bit(arr, 0);
    clr_arr_bit(arr, 1);
    clr_arr_bit(arr, 6);
    clr_arr_bit(arr, 12);
    clr_arr_bit(arr, 14);

    ASSERT_EQ(arr[0], 0x20);
    ASSERT_EQ(arr[1], 0x05);
}

TEST(Bit, ShiftArrayLeft)
{
    uint8_t arr_1[] = { 0x11, 0x88 };

    shift_left(arr_1);

    ASSERT_EQ(arr_1[0], 0x22);
    ASSERT_EQ(arr_1[1], 0x10);

    uint16_t arr_2[] = { 0x8811, 0x2233 };

    shift_left(arr_2);
    shift_left(arr_2);

    ASSERT_EQ(arr_2[0], 0x2044);
    ASSERT_EQ(arr_2[1], 0x88ce);
}
