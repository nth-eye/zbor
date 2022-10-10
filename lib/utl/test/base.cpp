#include <gtest/gtest.h>
#include "utl/base.h"

using namespace utl;

TEST(Base, Countof)
{
    const uint8_t arr_1[10] = {};
    const uint16_t arr_2[20] = {};
    const uint32_t arr_3[30] = {};

    EXPECT_EQ(countof(arr_1), 10);
    EXPECT_EQ(countof(arr_2), 20);
    EXPECT_EQ(countof(arr_3), 30);
}