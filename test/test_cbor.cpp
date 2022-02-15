#include <gtest/gtest.h>
#include "zbor_base.h"

using namespace zbor;

TEST(Cbor, Default)
{
    CBOR cbor;

    EXPECT_EQ(cbor.next, nullptr);
    EXPECT_EQ(cbor.prev, nullptr);
    EXPECT_EQ(cbor.type, TYPE_INVALID);
}

TEST(Cbor, Unsigned)
{
    CBOR cbor = 42;

    EXPECT_EQ(cbor.type, TYPE_UINT);
    EXPECT_EQ(cbor.uint, 42);
}

TEST(Cbor, Signed)
{
    CBOR cbor = -42;

    EXPECT_EQ(cbor.type, TYPE_SINT);
    EXPECT_EQ(cbor.sint, -42);
}

TEST(Cbor, MaxUnsigned)
{
    uint64_t max_uint = 18446744073709551615ul;
    CBOR cbor = max_uint;

    EXPECT_EQ(cbor.type, TYPE_UINT);
    EXPECT_EQ(cbor.uint, max_uint);
}

TEST(Cbor, MinSigned)
{
    int64_t min_sint = int64_t(-9223372036854775807 - 1);
    CBOR cbor = min_sint;

    EXPECT_EQ(cbor.type, TYPE_SINT);
    EXPECT_EQ(cbor.uint, min_sint);
}

TEST(Cbor, DataString)
{
    const uint8_t data[] = {0x00, 0x11, 0x22, 0x33};
    CBOR cbor = {data, sizeof(data)};

    EXPECT_EQ(cbor.type, TYPE_DATA);
    EXPECT_EQ(cbor.str.dat, data);
    EXPECT_EQ(cbor.str.len, sizeof(data));
}

TEST(Cbor, TextString)
{
    const char *text = "hello";
    CBOR cbor = {text, strlen(text)};

    EXPECT_EQ(cbor.type, TYPE_TEXT);
    EXPECT_EQ(cbor.str.txt, text);
    EXPECT_EQ(cbor.str.len, strlen(text));
}

TEST(Cbor, ArrayEmpty)
{
    CBOR cbor = Array();

    EXPECT_EQ(cbor.type, TYPE_ARRAY);
    EXPECT_EQ(cbor.arr.size(), 0);
}

TEST(Cbor, ArrayCreateThenPush)
{
    CBOR cbor = Array();
    CBOR obj;

    EXPECT_EQ(cbor.arr.push(&obj), NO_ERR);
    EXPECT_EQ(cbor.arr.size(), 1);
    EXPECT_EQ(cbor.arr.front(), &obj);
}

TEST(Cbor, ArrayPushThenCreate)
{
    auto arr = Array();
    CBOR obj;

    EXPECT_EQ(arr.push(&obj), NO_ERR);

    CBOR cbor = arr;

    EXPECT_EQ(cbor.type, TYPE_ARRAY);
    EXPECT_EQ(cbor.arr.size(), 1);
    EXPECT_EQ(cbor.arr.front(), &obj);
}

TEST(Cbor, MapEmpty)
{
    CBOR cbor = Map();

    EXPECT_EQ(cbor.type, TYPE_MAP);
    EXPECT_EQ(cbor.map.size(), 0);
}

TEST(Cbor, MapCreateThenPush)
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

TEST(Cbor, MapPushThenCreate)
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

TEST(Cbor, Tag)
{
    CBOR content = 42;
    CBOR cbor = Tag{777, &content};

    EXPECT_EQ(cbor.tag.val, 777);
    EXPECT_EQ(cbor.tag.content, &content);
}

TEST(Cbor, BoolFalse)
{
    CBOR cbor = false;

    EXPECT_EQ(cbor.type, TYPE_PRIM);
    EXPECT_EQ(cbor.prim, PRIM_FALSE);
}

TEST(Cbor, BoolTrue)
{
    CBOR cbor = true;

    EXPECT_EQ(cbor.type, TYPE_PRIM);
    EXPECT_EQ(cbor.prim, PRIM_TRUE);
}

TEST(Cbor, PrimitiveFalse)
{
    CBOR cbor = PRIM_FALSE;

    EXPECT_EQ(cbor.type, TYPE_PRIM);
    EXPECT_EQ(cbor.prim, PRIM_FALSE);
}

TEST(Cbor, PrimitiveTrue)
{
    CBOR cbor = PRIM_TRUE;

    EXPECT_EQ(cbor.type, TYPE_PRIM);
    EXPECT_EQ(cbor.prim, PRIM_TRUE);
}

TEST(Cbor, PrimitiveNull)
{
    CBOR cbor = PRIM_NULL;

    EXPECT_EQ(cbor.type, TYPE_PRIM);
    EXPECT_EQ(cbor.prim, PRIM_NULL);
}

TEST(Cbor, PrimitiveUndefined)
{
    CBOR cbor = PRIM_UNDEFINED;

    EXPECT_EQ(cbor.type, TYPE_PRIM);
    EXPECT_EQ(cbor.prim, PRIM_UNDEFINED);
}

TEST(Cbor, PrimitiveReserved)
{
    CBOR cbor = PRIM_FLOAT_16;

    EXPECT_EQ(cbor.type, TYPE_INVALID);
}

TEST(Cbor, PrimitiveInvalid)
{
    CBOR cbor = Prim(31);

    EXPECT_EQ(cbor.type, TYPE_INVALID);
}

TEST(Cbor, PrimitiveValid)
{
    CBOR cbor = Prim(32);

    EXPECT_EQ(cbor.type, TYPE_PRIM);
    EXPECT_EQ(cbor.prim, Prim(32));
}

TEST(Cbor, PrimitiveGreaterThanMax)
{
    CBOR cbor = Prim(512);

    EXPECT_EQ(cbor.type, TYPE_INVALID);
}

TEST(Cbor, Float)
{
    CBOR cbor = 1.0f;

    EXPECT_EQ(cbor.type, TYPE_DOUBLE);
    EXPECT_EQ(cbor.dbl, 1.0f);
}

TEST(Cbor, Double)
{
    CBOR cbor = 1.0;

    EXPECT_EQ(cbor.type, TYPE_DOUBLE);
    EXPECT_EQ(cbor.dbl, 1.0);
}
