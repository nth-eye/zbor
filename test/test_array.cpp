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
    Obj obj;

    ASSERT_EQ(arr.push(&obj), ERR_OK);
    ASSERT_EQ(arr.front(), &obj);
    ASSERT_EQ(arr.back(), &obj);
    ASSERT_EQ(arr.size(), 1);
}

TEST_F(ArrayTest, PushAndPop)
{
    Obj obj;

    ASSERT_EQ(arr.push(&obj), ERR_OK);
    ASSERT_EQ(arr.pop(&obj), ERR_OK);
    ASSERT_EQ(arr.front(), nullptr);
    ASSERT_EQ(arr.back(), nullptr);
    ASSERT_EQ(arr.size(), 0);
}

TEST_F(ArrayTest, PushAndPopWrongObject)
{
    Obj obj;
    Obj wrong;

    ASSERT_EQ(arr.push(&obj), ERR_OK);
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
    Obj obj;

    ASSERT_EQ(arr.pop(&obj), ERR_EMPTY);
    ASSERT_EQ(arr.front(), nullptr);
    ASSERT_EQ(arr.back(), nullptr);
    ASSERT_EQ(arr.size(), 0);
}

TEST_F(ArrayTest, DoublePop)
{
    Obj obj;

    ASSERT_EQ(arr.push(&obj), ERR_OK);
    ASSERT_EQ(arr.pop(&obj), ERR_OK);
    ASSERT_EQ(arr.pop(&obj), ERR_EMPTY);
    ASSERT_EQ(arr.front(), nullptr);
    ASSERT_EQ(arr.back(), nullptr);
    ASSERT_EQ(arr.size(), 0);
}

TEST_F(ArrayTest, Iterator)
{
    Obj obj_1;
    Obj obj_2 = 42;

    ASSERT_EQ(arr.push(&obj_1), ERR_OK);
    ASSERT_EQ(arr.push(&obj_2), ERR_OK);
    ASSERT_EQ(arr.size(), 2);
    ASSERT_NE(arr.begin(), arr.end());

    auto it = arr.begin();

    ASSERT_EQ((*it)->type, TYPE_INVALID);

    ASSERT_NE(++it, arr.end());
    ASSERT_EQ((*it)->type, TYPE_UINT);
    ASSERT_EQ((*it)->uint, 42);

    ASSERT_EQ(++it, arr.end());
}
