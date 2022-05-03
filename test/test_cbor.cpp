#include <gtest/gtest.h>
#include "zbor/base.h"

using namespace zbor;

TEST(Cbor, Default)
{
    Obj cbor;

    EXPECT_EQ(cbor.next, nullptr);
    EXPECT_EQ(cbor.prev, nullptr);
    EXPECT_EQ(cbor.type, TYPE_INVALID);
}

TEST(Cbor, Unsigned)
{
    Obj cbor = 42;

    EXPECT_EQ(cbor.type, TYPE_UINT);
    EXPECT_EQ(cbor.uint, 42);
}

TEST(Cbor, Signed)
{
    Obj cbor = -42;

    EXPECT_EQ(cbor.type, TYPE_SINT);
    EXPECT_EQ(cbor.sint, -42);
}

TEST(Cbor, MaxUnsigned)
{
    uint64_t max_uint = 18446744073709551615ul;
    Obj cbor = max_uint;

    EXPECT_EQ(cbor.type, TYPE_UINT);
    EXPECT_EQ(cbor.uint, max_uint);
}

TEST(Cbor, MinSigned)
{
    int64_t min_sint = int64_t(-9223372036854775807 - 1);
    Obj cbor = min_sint;

    EXPECT_EQ(cbor.type, TYPE_SINT);
    EXPECT_EQ(cbor.uint, min_sint);
}

TEST(Cbor, DataString)
{
    const uint8_t data[] = {0x00, 0x11, 0x22, 0x33};
    Obj cbor = {data, sizeof(data)};

    EXPECT_EQ(cbor.type, TYPE_DATA);
    EXPECT_EQ(cbor.str.dat, data);
    EXPECT_EQ(cbor.str.len, sizeof(data));
}

TEST(Cbor, TextString)
{
    const char *text = "hello";
    Obj cbor = {text, strlen(text)};

    EXPECT_EQ(cbor.type, TYPE_TEXT);
    EXPECT_EQ(cbor.str.txt, text);
    EXPECT_EQ(cbor.str.len, strlen(text));
}

TEST(Cbor, ArrayEmpty)
{
    Obj cbor = Array();

    EXPECT_EQ(cbor.type, TYPE_ARRAY);
    EXPECT_EQ(cbor.arr.size(), 0);
}

TEST(Cbor, ArrayCreateThenPush)
{
    Obj cbor = Array();
    Obj obj;

    EXPECT_EQ(cbor.arr.push(&obj), ERR_OK);
    EXPECT_EQ(cbor.arr.size(), 1);
    EXPECT_EQ(cbor.arr.front(), &obj);
}

TEST(Cbor, ArrayPushThenCreate)
{
    auto arr = Array();
    Obj obj;

    EXPECT_EQ(arr.push(&obj), ERR_OK);

    Obj cbor = arr;

    EXPECT_EQ(cbor.type, TYPE_ARRAY);
    EXPECT_EQ(cbor.arr.size(), 1);
    EXPECT_EQ(cbor.arr.front(), &obj);
}

TEST(Cbor, MapEmpty)
{
    Obj cbor = Map();

    EXPECT_EQ(cbor.type, TYPE_MAP);
    EXPECT_EQ(cbor.map.size(), 0);
}

TEST(Cbor, MapCreateThenPush)
{
    Obj cbor = Map();
    Obj key;
    Obj val;

    EXPECT_EQ(cbor.map.push(&key, &val), ERR_OK);
    EXPECT_EQ(cbor.map.size(), 1);

    auto it = cbor.map.begin();

    EXPECT_EQ((*it).key, &key);
    EXPECT_EQ((*it).val, &val);
}

TEST(Cbor, MapPushThenCreate)
{
    auto map = Map();
    Obj key;
    Obj val;

    EXPECT_EQ(map.push(&key, &val), ERR_OK);

    Obj cbor = map;

    EXPECT_EQ(cbor.type, TYPE_MAP);
    EXPECT_EQ(cbor.map.size(), 1);

    auto it = cbor.map.begin();

    EXPECT_EQ((*it).key, &key);
    EXPECT_EQ((*it).val, &val);   
}

TEST(Cbor, Tag)
{
    Obj content = 42;
    Obj cbor = Tag{777, &content};

    EXPECT_EQ(cbor.tag.val, 777);
    EXPECT_EQ(cbor.tag.content, &content);
}

TEST(Cbor, BoolFalse)
{
    Obj cbor = false;

    EXPECT_EQ(cbor.type, TYPE_PRIM);
    EXPECT_EQ(cbor.prim, PRIM_FALSE);
}

TEST(Cbor, BoolTrue)
{
    Obj cbor = true;

    EXPECT_EQ(cbor.type, TYPE_PRIM);
    EXPECT_EQ(cbor.prim, PRIM_TRUE);
}

TEST(Cbor, PrimitiveFalse)
{
    Obj cbor = PRIM_FALSE;

    EXPECT_EQ(cbor.type, TYPE_PRIM);
    EXPECT_EQ(cbor.prim, PRIM_FALSE);
}

TEST(Cbor, PrimitiveTrue)
{
    Obj cbor = PRIM_TRUE;

    EXPECT_EQ(cbor.type, TYPE_PRIM);
    EXPECT_EQ(cbor.prim, PRIM_TRUE);
}

TEST(Cbor, PrimitiveNull)
{
    Obj cbor = PRIM_NULL;

    EXPECT_EQ(cbor.type, TYPE_PRIM);
    EXPECT_EQ(cbor.prim, PRIM_NULL);
}

TEST(Cbor, PrimitiveUndefined)
{
    Obj cbor = PRIM_UNDEFINED;

    EXPECT_EQ(cbor.type, TYPE_PRIM);
    EXPECT_EQ(cbor.prim, PRIM_UNDEFINED);
}

TEST(Cbor, PrimitiveReserved)
{
    Obj cbor = PRIM_FLOAT_16;

    EXPECT_EQ(cbor.type, TYPE_INVALID);
}

TEST(Cbor, PrimitiveInvalid)
{
    Obj cbor = Prim(31);

    EXPECT_EQ(cbor.type, TYPE_INVALID);
}

TEST(Cbor, PrimitiveValid)
{
    Obj cbor = Prim(32);

    EXPECT_EQ(cbor.type, TYPE_PRIM);
    EXPECT_EQ(cbor.prim, Prim(32));
}

TEST(Cbor, PrimitiveGreaterThanMax)
{
    Obj cbor = Prim(512);

    EXPECT_EQ(cbor.type, TYPE_INVALID);
}

TEST(Cbor, Float)
{
    Obj cbor = 1.0f;

    EXPECT_EQ(cbor.type, TYPE_DOUBLE);
    EXPECT_EQ(cbor.dbl, 1.0f);
}

TEST(Cbor, Double)
{
    Obj cbor = 1.0;

    EXPECT_EQ(cbor.type, TYPE_DOUBLE);
    EXPECT_EQ(cbor.dbl, 1.0);
}
