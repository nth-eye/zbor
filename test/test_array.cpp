#include <gtest/gtest.h>
#include "zbor_base.h"

using namespace zbor;

class ArrayTest : public ::testing::Test {
protected:
    void SetUp() override 
    {
    }
    Array arr;
};

TEST_F(ArrayTest, Default)
{
    ASSERT_EQ(arr.front(), nullptr);
    ASSERT_EQ(arr.back(), nullptr);
    ASSERT_EQ(arr.size(), 0);
}

TEST_F(ArrayTest, Push)
{
    CBOR obj;

    ASSERT_EQ(arr.push(&obj), NO_ERR);
    ASSERT_EQ(arr.front(), &obj);
    ASSERT_EQ(arr.back(), &obj);
    ASSERT_EQ(arr.size(), 1);
}

TEST_F(ArrayTest, PushAndPop)
{
    CBOR obj;

    ASSERT_EQ(arr.push(&obj), NO_ERR);
    ASSERT_EQ(arr.pop(&obj), NO_ERR);
    ASSERT_EQ(arr.front(), nullptr);
    ASSERT_EQ(arr.back(), nullptr);
    ASSERT_EQ(arr.size(), 0);
}

TEST_F(ArrayTest, PushAndPopWrongObject)
{
    CBOR obj;
    CBOR wrong;

    ASSERT_EQ(arr.push(&obj), NO_ERR);
    ASSERT_EQ(arr.pop(&wrong), ERR_NOT_FOUND);
    ASSERT_EQ(arr.front(), &obj);
    ASSERT_EQ(arr.back(), &obj);
    ASSERT_EQ(arr.size(), 1);
}

TEST_F(ArrayTest, PushNullptr)
{
    ASSERT_EQ(arr.push(nullptr), ERR_NULLPTR);
    ASSERT_EQ(arr.front(), nullptr);
    ASSERT_EQ(arr.back(), nullptr);
    ASSERT_EQ(arr.size(), 0);
}

TEST_F(ArrayTest, PopNullptr)
{
    ASSERT_EQ(arr.pop(nullptr), ERR_NULLPTR);
    ASSERT_EQ(arr.front(), nullptr);
    ASSERT_EQ(arr.back(), nullptr);
    ASSERT_EQ(arr.size(), 0);
}

TEST_F(ArrayTest, PopAlreadyEmpty)
{
    CBOR obj;

    ASSERT_EQ(arr.pop(&obj), ERR_EMPTY);
    ASSERT_EQ(arr.front(), nullptr);
    ASSERT_EQ(arr.back(), nullptr);
    ASSERT_EQ(arr.size(), 0);
}

TEST_F(ArrayTest, DoublePop)
{
    CBOR obj;

    ASSERT_EQ(arr.push(&obj), NO_ERR);
    ASSERT_EQ(arr.pop(&obj), NO_ERR);
    ASSERT_EQ(arr.pop(&obj), ERR_EMPTY);
    ASSERT_EQ(arr.front(), nullptr);
    ASSERT_EQ(arr.back(), nullptr);
    ASSERT_EQ(arr.size(), 0);
}

TEST_F(ArrayTest, Iterator)
{
    CBOR obj_1;
    CBOR obj_2 = 42;

    ASSERT_EQ(arr.push(&obj_1), NO_ERR);
    ASSERT_EQ(arr.push(&obj_2), NO_ERR);
    ASSERT_EQ(arr.size(), 2);
    ASSERT_NE(arr.begin(), arr.end());

    auto it = arr.begin();

    ASSERT_EQ((*it)->type, TYPE_INVALID);

    ASSERT_NE(++it, arr.end());
    ASSERT_EQ((*it)->type, TYPE_UINT);
    ASSERT_EQ((*it)->uint, 42);

    ASSERT_EQ(++it, arr.end());
}
