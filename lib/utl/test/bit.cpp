#include <gtest/gtest.h>
#include <utl/bit.h>

using namespace utl;

TEST(Bit, BytesInBits)
{
    EXPECT_EQ(bytes_in_bits(0), 0);
    EXPECT_EQ(bytes_in_bits(1), 1);
    EXPECT_EQ(bytes_in_bits(7), 1);
    EXPECT_EQ(bytes_in_bits(8), 1);
    EXPECT_EQ(bytes_in_bits(9), 2);
    EXPECT_EQ(bytes_in_bits(42), 6);
    EXPECT_EQ(bytes_in_bits(64), 8);
    EXPECT_EQ(bytes_in_bits(69), 9);
    EXPECT_EQ(bytes_in_bits(228), 29);
    EXPECT_EQ(bytes_in_bits(9999), 1250);
}

TEST(Bit, MakeBit)
{
    EXPECT_EQ(bit(0), 1);
    EXPECT_EQ(bit(1), 2);
    EXPECT_EQ(bit(7), 128);
    EXPECT_EQ(bit(31), 2147483648);
    EXPECT_EQ(bit<uint64_t>(63), uint64_t(9223372036854775808ull));
}

TEST(Bit, GetBit)
{
    EXPECT_EQ(get_bit(0, 0), 0);
    EXPECT_EQ(get_bit(0, 1), 0);
    EXPECT_EQ(get_bit(0, 2), 0);
    EXPECT_EQ(get_bit(0, 3), 0);
    EXPECT_EQ(get_bit(0, 4), 0);
    EXPECT_EQ(get_bit(0, 5), 0);
    EXPECT_EQ(get_bit(0, 6), 0);
    EXPECT_EQ(get_bit(0, 7), 0);
    EXPECT_EQ(get_bit(42, 0), 0);
    EXPECT_EQ(get_bit(42, 1), 1);
    EXPECT_EQ(get_bit(42, 2), 0);
    EXPECT_EQ(get_bit(42, 3), 1);
    EXPECT_EQ(get_bit(42, 4), 0);
    EXPECT_EQ(get_bit(42, 5), 1);
    EXPECT_EQ(get_bit(42, 6), 0);
    EXPECT_EQ(get_bit(42, 7), 0);
    EXPECT_EQ(get_bit(255, 0), 1);
    EXPECT_EQ(get_bit(255, 1), 1);
    EXPECT_EQ(get_bit(255, 2), 1);
    EXPECT_EQ(get_bit(255, 3), 1);
    EXPECT_EQ(get_bit(255, 4), 1);
    EXPECT_EQ(get_bit(255, 5), 1);
    EXPECT_EQ(get_bit(255, 6), 1);
    EXPECT_EQ(get_bit(255, 7), 1);
}

TEST(Bit, SetBit)
{
    uint8_t b = 0;

    set_bit(b, 0);
    set_bit(b, 1);
    set_bit(b, 5);

    EXPECT_EQ(b, 0x23);

    set_bit(b, 9);

    EXPECT_EQ(b, 0x23);
}

TEST(Bit, ClearBit)
{
    uint8_t b = 0x23;

    clr_bit(b, 0);
    clr_bit(b, 1);
    clr_bit(b, 6);

    EXPECT_EQ(b, 0x20);

    clr_bit(b, 9);

    EXPECT_EQ(b, 0x20);

    clr_bit(b, 5);

    EXPECT_EQ(b, 0x00);
}

TEST(Bit, GetArrayBit)
{
    const uint8_t arr[] = { 0x02, 0x78 };

    EXPECT_EQ(get_arr_bit(arr, 0), 0);
    EXPECT_EQ(get_arr_bit(arr, 1), 1);
    EXPECT_EQ(get_arr_bit(arr, 2), 0);
    EXPECT_EQ(get_arr_bit(arr, 3), 0);
    EXPECT_EQ(get_arr_bit(arr, 4), 0);
    EXPECT_EQ(get_arr_bit(arr, 5), 0);
    EXPECT_EQ(get_arr_bit(arr, 6), 0);
    EXPECT_EQ(get_arr_bit(arr, 7), 0);
    EXPECT_EQ(get_arr_bit(arr, 8), 0);
    EXPECT_EQ(get_arr_bit(arr, 9), 0);
    EXPECT_EQ(get_arr_bit(arr, 10), 0);
    EXPECT_EQ(get_arr_bit(arr, 11), 1);
    EXPECT_EQ(get_arr_bit(arr, 12), 1);
    EXPECT_EQ(get_arr_bit(arr, 13), 1);
    EXPECT_EQ(get_arr_bit(arr, 14), 1);
    EXPECT_EQ(get_arr_bit(arr, 15), 0);
}

TEST(Bit, SetArrayBit)
{
    uint8_t arr[] = { 0x02, 0x15 };

    set_arr_bit(arr, 0);
    set_arr_bit(arr, 1);
    set_arr_bit(arr, 5);
    set_arr_bit(arr, 14);

    EXPECT_EQ(arr[0], 0x23);
    EXPECT_EQ(arr[1], 0x55);
}

TEST(Bit, ClearArrayBit)
{
    uint8_t arr[] = { 0x23, 0x55 };

    clr_arr_bit(arr, 0);
    clr_arr_bit(arr, 1);
    clr_arr_bit(arr, 6);
    clr_arr_bit(arr, 12);
    clr_arr_bit(arr, 14);

    EXPECT_EQ(arr[0], 0x20);
    EXPECT_EQ(arr[1], 0x05);
}

TEST(Bit, ShiftArrayLeft)
{
    uint8_t arr_1[] = { 0x11, 0x88 };

    shift_left(arr_1);

    EXPECT_EQ(arr_1[0], 0x22);
    EXPECT_EQ(arr_1[1], 0x10);

    uint16_t arr_2[] = { 0x8811, 0x2233 };

    shift_left(arr_2);
    shift_left(arr_2);

    EXPECT_EQ(arr_2[0], 0x2044);
    EXPECT_EQ(arr_2[1], 0x88ce);
}
