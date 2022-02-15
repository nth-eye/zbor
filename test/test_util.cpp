#include <gtest/gtest.h>
#include "zbor_util.h"

using namespace zbor;

TEST(Util, BytesInBits)
{
    EXPECT_EQ(bytes_in_bits(0), 0);
    EXPECT_EQ(bytes_in_bits(1), 1);
    EXPECT_EQ(bytes_in_bits(8), 1);
    EXPECT_EQ(bytes_in_bits(9), 2);
    EXPECT_EQ(bytes_in_bits(42), 6);
    EXPECT_EQ(bytes_in_bits(69), 9);
    EXPECT_EQ(bytes_in_bits(1337), 168);
}

TEST(Util, Bit)
{
    EXPECT_EQ(bit(0), 1);
    EXPECT_EQ(bit(1), 2);
    EXPECT_EQ(bit(2), 4);
    EXPECT_EQ(bit(4), 0x10);
    EXPECT_EQ(bit(8), 0x100);
    EXPECT_EQ(bit(16), 0x10000);
    EXPECT_EQ(bit(31), 0x80000000);
    EXPECT_EQ(bit(32), 0x0);
}

TEST(Util, GetBit)
{
    uint8_t byte = 0x8d;

    EXPECT_EQ(get_bit(byte, 0), true);
    EXPECT_EQ(get_bit(byte, 1), false);
    EXPECT_EQ(get_bit(byte, 2), true);
    EXPECT_EQ(get_bit(byte, 3), true);
    EXPECT_EQ(get_bit(byte, 4), false);
    EXPECT_EQ(get_bit(byte, 5), false);
    EXPECT_EQ(get_bit(byte, 6), false);
    EXPECT_EQ(get_bit(byte, 7), true);
    EXPECT_EQ(get_bit(byte, 8), false);
    EXPECT_EQ(get_bit(byte, 16), false);
}

TEST(Util, SetBit)
{
    uint8_t byte = 0x00;

    set_bit(byte, 1);
    set_bit(byte, 2);
    set_bit(byte, 4);
    set_bit(byte, 4);
    set_bit(byte, 8);
    set_bit(byte, 63);

    EXPECT_EQ(byte, 0x16);
}

TEST(Util, ClearBit)
{
    uint8_t byte = 0x42;

    clr_bit(byte, 0);
    clr_bit(byte, 1);
    clr_bit(byte, 8);
    clr_bit(byte, 63);

    EXPECT_EQ(byte, 0x40);
}

TEST(Util, StaticPool)
{
    StaticPool<int, 2> pool;

    auto el_1 = pool.make(1);
    auto el_2 = pool.make(2);
    auto el_3 = pool.make(3);

    ASSERT_NE(el_1, nullptr);
    ASSERT_NE(el_2, nullptr);
    ASSERT_EQ(el_3, nullptr);

    EXPECT_EQ(*el_1, 1);
    EXPECT_EQ(*el_2, 2);

    pool.free(el_1);
    pool.free(el_1);
    pool.free(nullptr);
    pool.free(reinterpret_cast<const int*>(0x42424249));

    auto el_4 = pool.make(4);

    ASSERT_EQ(el_4, el_1);
    EXPECT_EQ(*el_4, 4);
}