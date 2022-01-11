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
    EXPECT_EQ(map.size(), 0);

    auto it = map.begin();

    EXPECT_EQ((*it).key, nullptr);
    EXPECT_EQ((*it).val, nullptr);

    it = map.end();

    EXPECT_EQ((*it).key, nullptr);
    EXPECT_EQ((*it).val, nullptr);
}

TEST_F(MapTest, PushOne)
{
    CBOR key = 1;
    CBOR val = 2;

    ASSERT_EQ(map.push(&key, &val), NO_ERR);
    ASSERT_EQ(map.size(), 1);

    auto it = map.begin();

    EXPECT_EQ((*it).key, &key);
    EXPECT_EQ((*it).val, &val);

    it = map.end();

    EXPECT_EQ((*it).key, nullptr);
    EXPECT_EQ((*it).val, nullptr);
}

TEST_F(MapTest, PushNullptrValue)
{
    CBOR key;

    EXPECT_EQ(map.push(&key, nullptr), ERR_NULLPTR);
    EXPECT_EQ(map.size(), 0);

    auto it = map.begin();

    EXPECT_EQ((*it).key, nullptr);
    EXPECT_EQ((*it).val, nullptr);

    it = map.end();

    EXPECT_EQ((*it).key, nullptr);
    EXPECT_EQ((*it).val, nullptr);
}

TEST_F(MapTest, PushNullptrKey)
{
    CBOR val;

    EXPECT_EQ(map.push(nullptr, &val), ERR_NULLPTR);
    EXPECT_EQ(map.size(), 0);
    
    auto it = map.begin();

    EXPECT_EQ((*it).key, nullptr);
    EXPECT_EQ((*it).val, nullptr);

    it = map.end();

    EXPECT_EQ((*it).key, nullptr);
    EXPECT_EQ((*it).val, nullptr);
}

TEST_F(MapTest, PushNullptrKeyAndValue)
{
    EXPECT_EQ(map.push(nullptr, nullptr), ERR_NULLPTR);
    EXPECT_EQ(map.size(), 0);

    auto it = map.begin();

    EXPECT_EQ((*it).key, nullptr);
    EXPECT_EQ((*it).val, nullptr);

    it = map.end();

    EXPECT_EQ((*it).key, nullptr);
    EXPECT_EQ((*it).val, nullptr);
}

TEST_F(MapTest, PushAndPop)
{
    CBOR key = 42;
    CBOR val = 44;

    map.push(&key, &val);
    
    EXPECT_EQ(map.pop(&key), NO_ERR);
    EXPECT_EQ(map.size(), 0);

    auto it = map.begin();

    EXPECT_EQ((*it).key, nullptr);
    EXPECT_EQ((*it).val, nullptr);

    it = map.end();

    EXPECT_EQ((*it).key, nullptr);
    EXPECT_EQ((*it).val, nullptr);
}

TEST_F(MapTest, PushAndPopNullptr)
{
    CBOR key = 42;
    CBOR val = 44;

    map.push(&key, &val);
    
    EXPECT_EQ(map.pop(nullptr), ERR_NULLPTR);
    EXPECT_EQ(map.size(), 1);

    auto it = map.begin();

    EXPECT_EQ((*it).key, &key);
    EXPECT_EQ((*it).val, &val);

    it = map.end();

    EXPECT_EQ((*it).key, nullptr);
    EXPECT_EQ((*it).val, nullptr);
}

TEST_F(MapTest, PushAndPopWrong)
{
    CBOR key = 42;
    CBOR val = 44;
    CBOR wrong = 43;

    map.push(&key, &val);
    
    EXPECT_EQ(map.pop(&wrong), ERR_NOT_FOUND);
    EXPECT_EQ(map.size(), 1);

    auto it = map.begin();

    EXPECT_EQ((*it).key, &key);
    EXPECT_EQ((*it).val, &val);

    it = map.end();

    EXPECT_EQ((*it).key, nullptr);
    EXPECT_EQ((*it).val, nullptr);
}

TEST_F(MapTest, PopWhenAlreadyEmpty)
{
    CBOR wrong = 43;
    
    EXPECT_EQ(map.pop(&wrong), ERR_EMPTY);
    EXPECT_EQ(map.size(), 0);

    auto it = map.begin();

    EXPECT_EQ((*it).key, nullptr);
    EXPECT_EQ((*it).val, nullptr);

    it = map.end();

    EXPECT_EQ((*it).key, nullptr);
    EXPECT_EQ((*it).val, nullptr);
}
