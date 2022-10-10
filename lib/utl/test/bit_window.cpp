#include <gtest/gtest.h>
#include "utl/bit_window.h"
#include "utl/log.h"

using namespace utl;

// TEST(BitWindow, Default)
// {
    // bit_window bw;

    // ASSERT_EQ(bw.check_and_update(0), true);
    // ASSERT_EQ(bw.check_and_update(0), false);
    // ASSERT_EQ(bw.check_and_update(31), true);
    // ASSERT_EQ(bw.check_and_update(31), false);
    // ASSERT_EQ(bw.check_and_update(1), true);
    // ASSERT_EQ(bw.check_and_update(30), true);
    // ASSERT_EQ(bw.check_and_update(32), true);
    // ASSERT_EQ(bw.check_and_update(0), false);
    // ASSERT_EQ(bw.check_and_update(1), false);
    // ASSERT_EQ(bw.check_and_update(2), true);

    // bit_window bw2;

    // ASSERT_EQ(bw2.check_and_update(32), true);
    // ASSERT_EQ(bw2.check_and_update(32), false);
    // ASSERT_EQ(bw2.check_and_update(0), false);
    // ASSERT_EQ(bw2.check_and_update(1), true);
    // ASSERT_EQ(bw2.check_and_update(2), true);

    // uint32_t a = 0 - uint32_t(-1);

    // printf("a %lu 0x%08x \n", a, a);
// }

TEST(BitWindow, Default)
{
    bit_window bw;
    EXPECT_EQ(bw.latest(), 0);
    EXPECT_EQ(bw.check(0), false);
    EXPECT_EQ(bw.check(1), true);
    EXPECT_EQ(bw.check(32), true);
    bw.update(33);
    EXPECT_EQ(bw.check(0), false);
    EXPECT_EQ(bw.check(1), true);
    EXPECT_EQ(bw.check(32), true);

    // bit_window bw;
    // EXPECT_EQ(bw.latest(), -1);
    // EXPECT_EQ(bw.check(0), true);
    // EXPECT_EQ(bw.check(32), true);
    // EXPECT_EQ(bw.check(512), true);
    // EXPECT_EQ(bw.check(-1), false);
    // EXPECT_EQ(bw.check(-32), false);
    // EXPECT_EQ(bw.check(-33), true);
}