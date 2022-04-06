#include <gtest/gtest.h>
#include "zbor_base.h"

using namespace zbor;

class MapTest : public ::testing::Test {
protected:
    void SetUp() override 
    {
    }
    Map map;
};

TEST_F(MapTest, Default)
{
    ASSERT_EQ(map.size(), 0);
    ASSERT_EQ(map.begin(), map.end());
}

TEST_F(MapTest, PushOne)
{
    CBOR key = 1;
    CBOR val = 2;

    ASSERT_EQ(map.push(&key, &val), NO_ERR);
    ASSERT_EQ(map.size(), 1);
    ASSERT_NE(map.begin(), map.end());

    auto it = map.begin();

    ASSERT_EQ((*it).key, &key);
    ASSERT_EQ((*it).val, &val);
    ASSERT_EQ(++it, map.end());
}

TEST_F(MapTest, PushNullptrValue)
{
    CBOR key;

    ASSERT_EQ(map.push(&key, nullptr), ERR_NULLPTR);
    ASSERT_EQ(map.size(), 0);
    ASSERT_EQ(map.begin(), map.end());
}

TEST_F(MapTest, PushNullptrKey)
{
    CBOR val;

    ASSERT_EQ(map.push(nullptr, &val), ERR_NULLPTR);
    ASSERT_EQ(map.size(), 0);
    ASSERT_EQ(map.begin(), map.end());
}

TEST_F(MapTest, PushNullptrKeyAndValue)
{
    ASSERT_EQ(map.push(nullptr, nullptr), ERR_NULLPTR);
    ASSERT_EQ(map.size(), 0);
    ASSERT_EQ(map.begin(), map.end());
}

TEST_F(MapTest, PushAndPop)
{
    CBOR key = 42;
    CBOR val = 44;

    ASSERT_EQ(map.push(&key, &val), NO_ERR);
    ASSERT_EQ(map.pop(&key), NO_ERR);
    ASSERT_EQ(map.size(), 0);
    ASSERT_EQ(map.begin(), map.end());
}

TEST_F(MapTest, PushAndPopNullptr)
{
    CBOR key = 42;
    CBOR val = 44;

    ASSERT_EQ(map.push(&key, &val), NO_ERR);
    ASSERT_EQ(map.pop(nullptr), ERR_NULLPTR);
    ASSERT_EQ(map.size(), 1);
    ASSERT_NE(map.begin(), map.end());

    auto it = map.begin();

    ASSERT_EQ((*it).key, &key);
    ASSERT_EQ((*it).val, &val);
    ASSERT_EQ(++it, map.end());
}

TEST_F(MapTest, PushAndPopWrong)
{
    CBOR key = 42;
    CBOR val = 44;
    CBOR wrong = 43;
    
    ASSERT_EQ(map.push(&key, &val), NO_ERR);
    ASSERT_EQ(map.pop(&wrong), ERR_NOT_FOUND);
    ASSERT_EQ(map.size(), 1);
    ASSERT_NE(map.begin(), map.end());

    auto it = map.begin();

    ASSERT_EQ((*it).key, &key);
    ASSERT_EQ((*it).val, &val);
    ASSERT_EQ(++it, map.end());
}

TEST_F(MapTest, PopWhenAlreadyEmpty)
{
    CBOR wrong = 43;
    
    ASSERT_EQ(map.pop(&wrong), ERR_EMPTY);
    ASSERT_EQ(map.size(), 0);
    ASSERT_EQ(map.begin(), map.end());
}
