#include <gtest/gtest.h>
#include "zbor/base.h"

using namespace zbor;

TEST(Object, Default)
{
    ASSERT_EQ(obj_t{}.type, type_invalid);
    ASSERT_EQ(obj_t{}.valid(), false);
}
