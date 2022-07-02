#include <gtest/gtest.h>
#include "zbor/base.h"

using namespace zbor;

TEST(Object, Default)
{
    ASSERT_EQ(Obj{}.type, TYPE_INVALID);
    ASSERT_EQ(Obj{}.valid(), false);
}
