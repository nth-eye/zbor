#include <gtest/gtest.h>
#include "utl/bit_vector.h"

using namespace utl;

TEST(BitVector, Depth)
{
    ASSERT_EQ((bit_vector<uint8_t, 1, 1>::depth()), 1);
    ASSERT_EQ((bit_vector<uint8_t, 8, 1>::depth()), 1);
    ASSERT_EQ((bit_vector<uint8_t, 9, 1>::depth()), 2);
    ASSERT_EQ((bit_vector<uint8_t, 64, 1>::depth()), 2);
    ASSERT_EQ((bit_vector<uint8_t, 65, 1>::depth()), 3);
    ASSERT_EQ((bit_vector<uint8_t, 512, 1>::depth()), 3);
    ASSERT_EQ((bit_vector<uint8_t, 513, 1>::depth()), 4);

    ASSERT_EQ((bit_vector<uint8_t, 1, 4>::depth()), 1);
    ASSERT_EQ((bit_vector<uint8_t, 32, 4>::depth()), 1);
    ASSERT_EQ((bit_vector<uint8_t, 33, 4>::depth()), 2);
    ASSERT_EQ((bit_vector<uint8_t, 256, 4>::depth()), 2);
    ASSERT_EQ((bit_vector<uint8_t, 257, 4>::depth()), 3);
    ASSERT_EQ((bit_vector<uint8_t, 2048, 4>::depth()), 3);
    ASSERT_EQ((bit_vector<uint8_t, 2049, 4>::depth()), 4);

    ASSERT_EQ((bit_vector<uint8_t, 1, 7>::depth()), 1);
    ASSERT_EQ((bit_vector<uint8_t, 56, 7>::depth()), 1);
    ASSERT_EQ((bit_vector<uint8_t, 57, 7>::depth()), 2);
    ASSERT_EQ((bit_vector<uint8_t, 448, 7>::depth()), 2);
    ASSERT_EQ((bit_vector<uint8_t, 449, 7>::depth()), 3);
    ASSERT_EQ((bit_vector<uint8_t, 3584, 7>::depth()), 3);
    ASSERT_EQ((bit_vector<uint8_t, 3585, 7>::depth()), 4);

    ASSERT_EQ((bit_vector<uint32_t, 1, 1>::depth()), 1);
    ASSERT_EQ((bit_vector<uint32_t, 32, 1>::depth()), 1);
    ASSERT_EQ((bit_vector<uint32_t, 33, 1>::depth()), 2);
    ASSERT_EQ((bit_vector<uint32_t, 1024, 1>::depth()), 2);
    ASSERT_EQ((bit_vector<uint32_t, 1025, 1>::depth()), 3);
    ASSERT_EQ((bit_vector<uint32_t, 32768, 1>::depth()), 3);
    ASSERT_EQ((bit_vector<uint32_t, 32769, 1>::depth()), 4);

    ASSERT_EQ((bit_vector<uint32_t, 1, 4>::depth()), 1);
    ASSERT_EQ((bit_vector<uint32_t, 128, 4>::depth()), 1);
    ASSERT_EQ((bit_vector<uint32_t, 129, 4>::depth()), 2);
    ASSERT_EQ((bit_vector<uint32_t, 4096, 4>::depth()), 2);
    ASSERT_EQ((bit_vector<uint32_t, 4097, 4>::depth()), 3);
    ASSERT_EQ((bit_vector<uint32_t, 131072, 4>::depth()), 3);
    ASSERT_EQ((bit_vector<uint32_t, 131073, 4>::depth()), 4);

    ASSERT_EQ((bit_vector<uint32_t, 1, 31>::depth()), 1);
    ASSERT_EQ((bit_vector<uint32_t, 992, 31>::depth()), 1);
    ASSERT_EQ((bit_vector<uint32_t, 993, 31>::depth()), 2);
    ASSERT_EQ((bit_vector<uint32_t, 31744, 31>::depth()), 2);
    ASSERT_EQ((bit_vector<uint32_t, 31745, 31>::depth()), 3);
    ASSERT_EQ((bit_vector<uint32_t, 1015808, 31>::depth()), 3);
    ASSERT_EQ((bit_vector<uint32_t, 1015809, 31>::depth()), 4);
}

TEST(BitVector, Words)
{

}