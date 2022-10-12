#include <gtest/gtest.h>
#include "zbor/base.h"

using namespace zbor;

TEST(Object, Default)
{
    ASSERT_EQ(item{}.type, type_invalid);
    ASSERT_EQ(item{}.valid(), false);
}
