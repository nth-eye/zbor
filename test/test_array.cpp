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
    EXPECT_EQ(arr.front(), nullptr);
    EXPECT_EQ(arr.back(), nullptr);
    EXPECT_EQ(arr.size(), 0);
}

TEST_F(ArrayTest, Push)
{
    CBOR obj;

    auto err = arr.push(&obj);

    ASSERT_EQ(err, NO_ERR);
    EXPECT_EQ(arr.front(), &obj);
    EXPECT_EQ(arr.back(), &obj);
    EXPECT_EQ(arr.size(), 1);
}

TEST_F(ArrayTest, PushAndPop)
{
    CBOR obj;

    arr.push(&obj);
    auto err = arr.pop(&obj);

    ASSERT_EQ(err, NO_ERR);
    EXPECT_EQ(arr.front(), nullptr);
    EXPECT_EQ(arr.back(), nullptr);
    EXPECT_EQ(arr.size(), 0);
}

TEST_F(ArrayTest, PushAndPopWrongObject)
{
    CBOR obj;
    CBOR wrong;

    arr.push(&obj);
    auto err = arr.pop(&wrong);

    ASSERT_EQ(err, ERR_NOT_FOUND);
    EXPECT_EQ(arr.front(), &obj);
    EXPECT_EQ(arr.back(), &obj);
    EXPECT_EQ(arr.size(), 1);
}

TEST_F(ArrayTest, PushNullptr)
{
    auto err = arr.push(nullptr);

    ASSERT_EQ(err, ERR_NULLPTR);
    EXPECT_EQ(arr.front(), nullptr);
    EXPECT_EQ(arr.back(), nullptr);
    EXPECT_EQ(arr.size(), 0);
}

TEST_F(ArrayTest, PopNullptr)
{
    auto err = arr.pop(nullptr);

    ASSERT_EQ(err, ERR_NULLPTR);
    EXPECT_EQ(arr.front(), nullptr);
    EXPECT_EQ(arr.back(), nullptr);
    EXPECT_EQ(arr.size(), 0);
}

TEST_F(ArrayTest, PopAlreadyEmpty)
{
    CBOR obj;

    auto err = arr.pop(&obj);

    ASSERT_EQ(err, ERR_EMPTY);
    EXPECT_EQ(arr.front(), nullptr);
    EXPECT_EQ(arr.back(), nullptr);
    EXPECT_EQ(arr.size(), 0);
}

TEST_F(ArrayTest, DoublePop)
{
    CBOR obj;

    arr.push(&obj);
    arr.pop(&obj);
    auto err = arr.pop(&obj);

    ASSERT_EQ(err, ERR_EMPTY);
    EXPECT_EQ(arr.front(), nullptr);
    EXPECT_EQ(arr.back(), nullptr);
    EXPECT_EQ(arr.size(), 0);
}

TEST_F(ArrayTest, Iterator)
{
    CBOR obj_1;
    CBOR obj_2 = 42;

    arr.push(&obj_1);
    arr.push(&obj_2);

    auto it = arr.begin();

    EXPECT_EQ((*it).type, TYPE_INVALID);

    ++it;

    EXPECT_EQ((*it).type, TYPE_UINT);
    EXPECT_EQ((*it).uint, 42);
}
