#include <gtest/gtest.h>
#include "zbor/base.h"

using namespace zbor;

TEST(Object, Default)
{
    static_assert(item{}.type == type_invalid);
    static_assert(item{}.valid() == false);
    
    ASSERT_EQ(item{}.type, type_invalid);
    ASSERT_EQ(item{}.valid(), false);
}

TEST(Object, TextType)
{
    static constexpr const byte text_1[] = "hello";
    static constexpr const byte text_2[] = "world";

    static_assert(text_t{text_1} == "hello");
    static_assert(text_t{text_2} == "world");
    static_assert(text_t{text_1} != "world");
    static_assert(text_t{text_2} != "hello");

    ASSERT_EQ(text_t{text_1}, "hello");
    ASSERT_EQ(text_t{text_2}, "world");
    ASSERT_NE(text_t{text_1}, "world");
    ASSERT_NE(text_t{text_2}, "hello");
    ASSERT_EQ("hello", text_t{text_1});
    ASSERT_EQ("world", text_t{text_2});
    ASSERT_NE("world", text_t{text_1});
    ASSERT_NE("hello", text_t{text_2});
}