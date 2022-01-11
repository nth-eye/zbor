#include <gtest/gtest.h>
#include "zbor_base.h"

using namespace zbor;

// class DefaultCBOR : public ::testing::Test {
// protected:
//     void SetUp() override 
//     {
//     }
//     CBOR cbor;
// };

TEST(CBOR, Default)
{
    CBOR cbor;

    EXPECT_EQ(cbor.next, nullptr);
    EXPECT_EQ(cbor.prev, nullptr);
    EXPECT_EQ(cbor.type, TYPE_INVALID);
}

TEST(CBOR, Unsigned)
{
    CBOR cbor = 42;

    EXPECT_EQ(cbor.type, TYPE_UINT);
    EXPECT_EQ(cbor.uint, 42);
}

TEST(CBOR, Signed)
{
    CBOR cbor = -42;

    EXPECT_EQ(cbor.type, TYPE_SINT);
    EXPECT_EQ(cbor.sint, -42);
}

TEST(CBOR, MaxUnsigned)
{
    uint64_t max_uint = 18446744073709551615ul;
    CBOR cbor = max_uint;

    EXPECT_EQ(cbor.type, TYPE_UINT);
    EXPECT_EQ(cbor.uint, max_uint);
}

TEST(CBOR, MinSigned)
{
    int64_t min_sint = int64_t(-9223372036854775807 - 1);
    CBOR cbor = min_sint;

    EXPECT_EQ(cbor.type, TYPE_SINT);
    EXPECT_EQ(cbor.uint, min_sint);
}

TEST(CBOR, DataString)
{
    const uint8_t data[] = {0x00, 0x11, 0x22, 0x33};
    CBOR cbor = {data, sizeof(data)};

    EXPECT_EQ(cbor.type, TYPE_DATA);
    EXPECT_EQ(cbor.str.dat, data);
    EXPECT_EQ(cbor.str.len, sizeof(data));
}

TEST(CBOR, TextString)
{
    const char *text = "hello";
    CBOR cbor = {text, strlen(text)};

    EXPECT_EQ(cbor.type, TYPE_TEXT);
    EXPECT_EQ(cbor.str.txt, text);
    EXPECT_EQ(cbor.str.len, strlen(text));
}

TEST(CBOR, ArrayEmpty)
{
    CBOR cbor = Array();

    EXPECT_EQ(cbor.type, TYPE_ARRAY);
    EXPECT_EQ(cbor.arr.size(), 0);
}

TEST(CBOR, ArrayCreateThenPush)
{
    CBOR cbor = Array();
    CBOR obj;

    EXPECT_EQ(cbor.arr.push(&obj), NO_ERR);
    EXPECT_EQ(cbor.arr.size(), 1);
    EXPECT_EQ(cbor.arr.front(), &obj);
}

TEST(CBOR, ArrayPushThenCreate)
{
    auto arr = Array();
    CBOR obj;

    EXPECT_EQ(arr.push(&obj), NO_ERR);

    CBOR cbor = arr;

    EXPECT_EQ(cbor.type, TYPE_ARRAY);
    EXPECT_EQ(cbor.arr.size(), 1);
    EXPECT_EQ(cbor.arr.front(), &obj);
}

TEST(CBOR, MapEmpty)
{
    CBOR cbor = Map();

    EXPECT_EQ(cbor.type, TYPE_MAP);
    EXPECT_EQ(cbor.map.size(), 0);
}

TEST(CBOR, MapCreateThenPush)
{
    CBOR cbor = Map();
    CBOR key;
    CBOR val;

    EXPECT_EQ(cbor.map.push(&key, &val), NO_ERR);
    EXPECT_EQ(cbor.map.size(), 1);

    auto it = cbor.map.begin();

    EXPECT_EQ((*it).key, &key);
    EXPECT_EQ((*it).val, &val);
}

TEST(CBOR, MapPushThenCreate)
{
    auto map = Map();
    CBOR key;
    CBOR val;

    EXPECT_EQ(map.push(&key, &val), NO_ERR);

    CBOR cbor = map;

    EXPECT_EQ(cbor.type, TYPE_MAP);
    EXPECT_EQ(cbor.map.size(), 1);

    auto it = cbor.map.begin();

    EXPECT_EQ((*it).key, &key);
    EXPECT_EQ((*it).val, &val);   
}

// TODO: Tag

TEST(CBOR, BoolFalse)
{
    CBOR cbor = false;

    EXPECT_EQ(cbor.type, TYPE_PRIM);
    EXPECT_EQ(cbor.prim, PRIM_FALSE);
}

TEST(CBOR, BoolTrue)
{
    CBOR cbor = true;

    EXPECT_EQ(cbor.type, TYPE_PRIM);
    EXPECT_EQ(cbor.prim, PRIM_TRUE);
}

TEST(CBOR, PrimitiveFalse)
{
    CBOR cbor = PRIM_FALSE;

    EXPECT_EQ(cbor.type, TYPE_PRIM);
    EXPECT_EQ(cbor.prim, PRIM_FALSE);
}

TEST(CBOR, PrimitiveTrue)
{
    CBOR cbor = PRIM_TRUE;

    EXPECT_EQ(cbor.type, TYPE_PRIM);
    EXPECT_EQ(cbor.prim, PRIM_TRUE);
}

TEST(CBOR, PrimitiveNull)
{
    CBOR cbor = PRIM_NULL;

    EXPECT_EQ(cbor.type, TYPE_PRIM);
    EXPECT_EQ(cbor.prim, PRIM_NULL);
}

TEST(CBOR, PrimitiveUndefined)
{
    CBOR cbor = PRIM_UNDEFINED;

    EXPECT_EQ(cbor.type, TYPE_PRIM);
    EXPECT_EQ(cbor.prim, PRIM_UNDEFINED);
}

TEST(CBOR, PrimitiveReserved)
{
    CBOR cbor = PRIM_FLOAT_16;

    EXPECT_EQ(cbor.type, TYPE_INVALID);
}

TEST(CBOR, PrimitiveInvalid)
{
    CBOR cbor = Prim(31);

    EXPECT_EQ(cbor.type, TYPE_INVALID);
}

TEST(CBOR, PrimitiveValid)
{
    CBOR cbor = Prim(32);

    EXPECT_EQ(cbor.type, TYPE_PRIM);
    EXPECT_EQ(cbor.prim, Prim(32));
}

TEST(CBOR, PrimitiveGreaterThanMax)
{
    CBOR cbor = Prim(512);

    EXPECT_EQ(cbor.type, TYPE_INVALID);
}

TEST(CBOR, Float)
{
    CBOR cbor = 1.0f;

    EXPECT_EQ(cbor.type, TYPE_DOUBLE);
    EXPECT_EQ(cbor.dbl, 1.0f);
}

TEST(CBOR, Double)
{
    CBOR cbor = 1.0;

    EXPECT_EQ(cbor.type, TYPE_DOUBLE);
    EXPECT_EQ(cbor.dbl, 1.0);
}

TEST(CBOR, Sizeof)
{
    std::cout << "sizeof CBOR: " << sizeof(CBOR) << std::endl;
}