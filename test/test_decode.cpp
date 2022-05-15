#include <gtest/gtest.h>
#include <cmath>
#include "zbor/decode.h"

using namespace zbor;

TEST(Decode, OutOfBounds)
{
    const byte test[] = { 0x00 };

    auto [obj, err, ptr] = decode(test + sizeof(test), test + sizeof(test));

    ASSERT_EQ(err, ERR_OUT_OF_BOUNDS);
    ASSERT_EQ(ptr, test + sizeof(test));
    ASSERT_EQ(obj.type, TYPE_INVALID);
}

TEST(Decode, Uint)
{
    const byte test[] = { 
        0x00, // 0
        0x01, // 1
        0x0a, // 10
        0x17, // 23
        0x18, 0x18, // 24
        0x18, 0x19, // 25
        0x18, 0x64, // 100
        0x19, 0x03, 0xe8, // 1000
        0x1a, 0x00, 0x0f, 0x42, 0x40, // 1000000
        0x1b, 0x00, 0x00, 0x00, 0xe8, 0xd4, 0xa5, 0x10, 0x00, // 1000000000000
        0x1b, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // 18446744073709551615
    };

    Obj obj;
    Err err;
    auto ptr = test;
    auto end = test + sizeof(test);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 1);
    ASSERT_EQ(obj.type, TYPE_UINT);
    ASSERT_EQ(obj.uint, 0);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 2);
    ASSERT_EQ(obj.type, TYPE_UINT);
    ASSERT_EQ(obj.uint, 1);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 3);
    ASSERT_EQ(obj.type, TYPE_UINT);
    ASSERT_EQ(obj.uint, 10);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 4);
    ASSERT_EQ(obj.type, TYPE_UINT);
    ASSERT_EQ(obj.uint, 23);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 6);
    ASSERT_EQ(obj.type, TYPE_UINT);
    ASSERT_EQ(obj.uint, 24);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 8);
    ASSERT_EQ(obj.type, TYPE_UINT);
    ASSERT_EQ(obj.uint, 25);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 10);
    ASSERT_EQ(obj.type, TYPE_UINT);
    ASSERT_EQ(obj.uint, 100);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 13);
    ASSERT_EQ(obj.type, TYPE_UINT);
    ASSERT_EQ(obj.uint, 1000);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 18);
    ASSERT_EQ(obj.type, TYPE_UINT);
    ASSERT_EQ(obj.uint, 1000000);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 27);
    ASSERT_EQ(obj.type, TYPE_UINT);
    ASSERT_EQ(obj.uint, 1000000000000);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 36);
    ASSERT_EQ(obj.type, TYPE_UINT);
    ASSERT_EQ(obj.uint, 18446744073709551615u);

    ASSERT_EQ(ptr, end);
}

TEST(Decode, Sint)
{
    const byte test[] = {
        0x20, // -1
        0x29, // -10
        0x38, 0x63, // -100
        0x39, 0x03, 0xe7, // -1000
        0x3b, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // -18446744073709551616
    };

    Obj obj;
    Err err;
    auto ptr = test;
    auto end = test + sizeof(test);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 1);
    ASSERT_EQ(obj.type, TYPE_SINT);
    ASSERT_EQ(obj.sint, -1);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 2);
    ASSERT_EQ(obj.type, TYPE_SINT);
    ASSERT_EQ(obj.sint, -10);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 4);
    ASSERT_EQ(obj.type, TYPE_SINT);
    ASSERT_EQ(obj.sint, -100);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 7);
    ASSERT_EQ(obj.type, TYPE_SINT);
    ASSERT_EQ(obj.sint, -1000);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 16);
    ASSERT_EQ(obj.type, TYPE_SINT);
    ASSERT_EQ(obj.sint, -18446744073709551616);

    ASSERT_EQ(ptr, end);
}

TEST(Decode, Float)
{
    const byte test[] = {
        0xf9, 0x00, 0x00, // 0.0
        0xf9, 0x80, 0x00, // -0.0
        0xf9, 0x3c, 0x00, // 1.0
        0xfb, 0x3f, 0xf1, 0x99, 0x99, 0x99, 0x99, 0x99, 0x9a, // 1.1
        0xf9, 0x3e, 0x00, // 1.5
        0xf9, 0x7b, 0xff, // 65504.0
        0xfa, 0x47, 0xc3, 0x50, 0x00, // 100000.0
        0xfa, 0x7f, 0x7f, 0xff, 0xff, // 3.4028234663852886e+38
        0xfb, 0x7e, 0x37, 0xe4, 0x3c, 0x88, 0x00, 0x75, 0x9c, // 1.0e+300
        0xf9, 0x00, 0x01, // 5.960464477539063e-8
        0xf9, 0x04, 0x00, // 0.00006103515625
        0xf9, 0xc4, 0x00, // -4.0
        0xfb, 0xc0, 0x10, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, // -4.1
        0xf9, 0x7c, 0x00, // Infinity
        0xf9, 0x7e, 0x00, // NaN
        0xf9, 0xfc, 0x00, // -Infinity
    };

    Obj obj;
    Err err;
    auto ptr = test;
    auto end = test + sizeof(test);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 3);
    ASSERT_EQ(obj.type, TYPE_DOUBLE);
    ASSERT_DOUBLE_EQ(obj.dbl, 0.0);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 6);
    ASSERT_EQ(obj.type, TYPE_DOUBLE);
    ASSERT_DOUBLE_EQ(obj.dbl, -0.0);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 9);
    ASSERT_EQ(obj.type, TYPE_DOUBLE);
    ASSERT_DOUBLE_EQ(obj.dbl, 1.0);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 18);
    ASSERT_EQ(obj.type, TYPE_DOUBLE);
    ASSERT_DOUBLE_EQ(obj.dbl, 1.1);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 21);
    ASSERT_EQ(obj.type, TYPE_DOUBLE);
    ASSERT_DOUBLE_EQ(obj.dbl, 1.5);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 24);
    ASSERT_EQ(obj.type, TYPE_DOUBLE);
    ASSERT_DOUBLE_EQ(obj.dbl, 65504.0);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 29);
    ASSERT_EQ(obj.type, TYPE_DOUBLE);
    ASSERT_DOUBLE_EQ(obj.dbl, 100000.0);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 34);
    ASSERT_EQ(obj.type, TYPE_DOUBLE);
    ASSERT_DOUBLE_EQ(obj.dbl, 3.4028234663852886e+38);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 43);
    ASSERT_EQ(obj.type, TYPE_DOUBLE);
    ASSERT_DOUBLE_EQ(obj.dbl, 1.0e+300);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 46);
    ASSERT_EQ(obj.type, TYPE_DOUBLE);
    ASSERT_DOUBLE_EQ(obj.dbl, 5.960464477539063e-8);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 49);
    ASSERT_EQ(obj.type, TYPE_DOUBLE);
    ASSERT_DOUBLE_EQ(obj.dbl, 0.00006103515625);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 52);
    ASSERT_EQ(obj.type, TYPE_DOUBLE);
    ASSERT_DOUBLE_EQ(obj.dbl, -4.0);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 61);
    ASSERT_EQ(obj.type, TYPE_DOUBLE);
    ASSERT_DOUBLE_EQ(obj.dbl, -4.1);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 64);
    ASSERT_EQ(obj.type, TYPE_DOUBLE);
    ASSERT_DOUBLE_EQ(obj.dbl, double(INFINITY));

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 67);
    ASSERT_EQ(obj.type, TYPE_DOUBLE);
    ASSERT_TRUE(std::isnan(obj.dbl));

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 70);
    ASSERT_EQ(obj.type, TYPE_DOUBLE);
    ASSERT_DOUBLE_EQ(obj.dbl, double(-INFINITY));

    ASSERT_EQ(ptr, end);
}

TEST(Decode, Simple)
{
    const byte test[] = {
        0xf4, // false
        0xf5, // true
        0xf6, // null
        0xf7, // undefined
        0xf0, // simple(16)
        0xf8, 0xff, // simple(255)
    };

    Obj obj;
    Err err;
    auto ptr = test;
    auto end = test + sizeof(test);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 1);
    ASSERT_EQ(obj.type, TYPE_PRIM);
    ASSERT_EQ(obj.prim, zbor::PRIM_FALSE);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 2);
    ASSERT_EQ(obj.type, TYPE_PRIM);
    ASSERT_EQ(obj.prim, zbor::PRIM_TRUE);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 3);
    ASSERT_EQ(obj.type, TYPE_PRIM);
    ASSERT_EQ(obj.prim, zbor::PRIM_NULL);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 4);
    ASSERT_EQ(obj.type, TYPE_PRIM);
    ASSERT_EQ(obj.prim, zbor::PRIM_UNDEFINED);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 5);
    ASSERT_EQ(obj.type, TYPE_PRIM);
    ASSERT_EQ(obj.prim, Prim(16));

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 7);
    ASSERT_EQ(obj.type, TYPE_PRIM);
    ASSERT_EQ(obj.prim, Prim(255));

    ASSERT_EQ(ptr, end);
}

TEST(Decode, Data)
{
    const byte test[] = {
        0x40, // h''
        0x44, 0x01, 0x02, 0x03, 0x04, // h'01020304'
    };
    const byte first[1] = {};
    const byte second[] = {0x01, 0x02, 0x03, 0x04};

    Obj obj;
    Err err;
    auto ptr = test;
    auto end = test + sizeof(test);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 1);
    ASSERT_EQ(obj.type, TYPE_DATA);
    ASSERT_EQ(obj.str.len, 0);
    ASSERT_EQ(memcmp(first, obj.str.dat, obj.str.len), 0);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 6);
    ASSERT_EQ(obj.type, TYPE_DATA);
    ASSERT_EQ(obj.str.len, 4);
    ASSERT_EQ(memcmp(second, obj.str.dat, obj.str.len), 0);

    ASSERT_EQ(ptr, end);
}

TEST(Decode, Text)
{
    const byte test[] = {
        0x60, // ""
        0x61, 0x61, // "a"
        0x64, 0x49, 0x45, 0x54, 0x46, // "IETF"
        0x62, 0x22, 0x5c, // "\"\\"
        0x62, 0xc3, 0xbc, // "\u00fc"
        0x63, 0xe6, 0xb0, 0xb4, // "\u6c34"
        0x64, 0xf0, 0x90, 0x85, 0x91, // "\ud800\udd51"
    };
    const byte garbage[] = { 0xf0, 0x90, 0x85, 0x91 };

    Obj obj;
    Err err;
    auto ptr = test;
    auto end = test + sizeof(test);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 1);
    ASSERT_EQ(obj.type, TYPE_TEXT);
    ASSERT_EQ(obj.str.len, strlen(""));
    ASSERT_EQ(strncmp("", obj.str.txt, obj.str.len), 0);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 3);
    ASSERT_EQ(obj.type, TYPE_TEXT);
    ASSERT_EQ(obj.str.len, strlen("a"));
    ASSERT_EQ(strncmp("a", obj.str.txt, obj.str.len), 0);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 8);
    ASSERT_EQ(obj.type, TYPE_TEXT);
    ASSERT_EQ(obj.str.len, strlen("IETF"));
    ASSERT_EQ(strncmp("IETF", obj.str.txt, obj.str.len), 0);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 11);
    ASSERT_EQ(obj.type, TYPE_TEXT);
    ASSERT_EQ(obj.str.len, strlen("\"\\"));
    ASSERT_EQ(strncmp("\"\\", obj.str.txt, obj.str.len), 0);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 14);
    ASSERT_EQ(obj.type, TYPE_TEXT);
    ASSERT_EQ(obj.str.len, strlen("\u00fc"));
    ASSERT_EQ(strncmp("\u00fc", obj.str.txt, obj.str.len), 0);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 18);
    ASSERT_EQ(obj.type, TYPE_TEXT);
    ASSERT_EQ(obj.str.len, strlen("\u6c34"));
    ASSERT_EQ(strncmp("\u6c34", obj.str.txt, obj.str.len), 0);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 23);
    ASSERT_EQ(obj.type, TYPE_TEXT);
    ASSERT_EQ(obj.str.len, sizeof(garbage));
    ASSERT_EQ(strncmp((char*) garbage, obj.str.txt, obj.str.len), 0);

    ASSERT_EQ(ptr, end);
}

TEST(Decode, Tag)
{
    const byte test[] = {
        0xc0, 0x74, 0x32, 0x30, 0x31, 0x33, 0x2d, 0x30, 0x33, 0x2d, 0x32, 0x31, 0x54, 0x32, 0x30, 0x3a, 0x30, 0x34, 0x3a, 0x30, 0x30, 0x5a, // 0("2013-03-21T20:04:00Z")
        0xc1, 0x1a, 0x51, 0x4b, 0x67, 0xb0, // 1(1363896240)
        0xc1, 0xfb, 0x41, 0xd4, 0x52, 0xd9, 0xec, 0x20, 0x00, 0x00, // 1(1363896240.5)
        0xd7, 0x44, 0x01, 0x02, 0x03, 0x04, // 23(h'01020304')
        0xd8, 0x18, 0x45, 0x64, 0x49, 0x45, 0x54, 0x46, // 24(h'6449455446')
        0xd8, 0x20, 0x76, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x77, 0x77, 0x77, 0x2e, 0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x2e, 0x63, 0x6f, 0x6d, // 32("http://www.example.com")
    };
    const byte data_1[] = { 0x01, 0x02, 0x03, 0x04 };
    const byte data_2[] = { 0x64, 0x49, 0x45, 0x54, 0x46 };

    Obj obj;
    Obj content;
    Err err;
    auto ptr = test;
    auto end = test + sizeof(test);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 22);
    ASSERT_EQ(obj.type, TYPE_TAG);
    ASSERT_EQ(obj.tag.num(), 0);

    content = obj.tag.content();
    ASSERT_EQ(content.type, TYPE_TEXT);
    ASSERT_EQ(content.str.len, strlen("2013-03-21T20:04:00Z"));
    ASSERT_EQ(strncmp("2013-03-21T20:04:00Z", content.str.txt, content.str.len), 0);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 28);
    ASSERT_EQ(obj.type, TYPE_TAG);
    ASSERT_EQ(obj.tag.num(), 1);

    content = obj.tag.content();
    ASSERT_EQ(content.type, TYPE_UINT);
    ASSERT_EQ(content.uint, 1363896240);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 38);
    ASSERT_EQ(obj.type, TYPE_TAG);
    ASSERT_EQ(obj.tag.num(), 1);

    content = obj.tag.content();
    ASSERT_EQ(content.type, TYPE_DOUBLE);
    ASSERT_DOUBLE_EQ(content.dbl, 1363896240.5);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 44);
    ASSERT_EQ(obj.type, TYPE_TAG);
    ASSERT_EQ(obj.tag.num(), 23);

    content = obj.tag.content();
    ASSERT_EQ(content.type, TYPE_DATA);
    ASSERT_EQ(content.str.len, sizeof(data_1));
    ASSERT_EQ(memcmp(data_1, content.str.dat, content.str.len), 0);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 52);
    ASSERT_EQ(obj.type, TYPE_TAG);
    ASSERT_EQ(obj.tag.num(), 24);

    content = obj.tag.content();
    ASSERT_EQ(content.type, TYPE_DATA);
    ASSERT_EQ(content.str.len, sizeof(data_2));
    ASSERT_EQ(memcmp(data_2, content.str.dat, content.str.len), 0);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 77);
    ASSERT_EQ(obj.type, TYPE_TAG);
    ASSERT_EQ(obj.tag.num(), 32);

    content = obj.tag.content();
    ASSERT_EQ(content.type, TYPE_TEXT);
    ASSERT_EQ(content.str.len, strlen("http://www.example.com"));
    ASSERT_EQ(strncmp("http://www.example.com", content.str.txt, content.str.len), 0);

    ASSERT_EQ(ptr, end);
}

TEST(Decode, Array)
{
    const byte test[] = {
        0x80, // []
        0x83, 0x01, 0x02, 0x03, // [1, 2, 3]
        0x83, 0x01, 0x82, 0x02, 0x03, 0x82, 0x04, 0x05, // [1, [2, 3], [4, 5]]
        0x98, 0x19, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x18, 0x18, 0x19, // [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]
    };

    Obj obj;
    Err err;
    auto ptr = test;
    auto end = test + sizeof(test);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 1);
    ASSERT_EQ(obj.type, TYPE_ARRAY);
    ASSERT_EQ(obj.arr.size(), 0);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 5);
    ASSERT_EQ(obj.type, TYPE_ARRAY);
    ASSERT_EQ(obj.arr.size(), 3);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 13);
    ASSERT_EQ(obj.type, TYPE_ARRAY);
    ASSERT_EQ(obj.arr.size(), 3);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 42);
    ASSERT_EQ(obj.type, TYPE_ARRAY);
    ASSERT_EQ(obj.arr.size(), 25);

    ASSERT_EQ(ptr, end);
}

TEST(Decode, Map)
{
    const byte test[] = {
        0xa0, // {}
        0xa2, 0x01, 0x02, 0x03, 0x04, // {1: 2, 3: 4}
        0xa2, 0x61, 0x61, 0x01, 0x61, 0x62, 0x82, 0x02, 0x03, // {"a": 1, "b": [2, 3]}
        0xa5, 0x61, 0x61, 0x61, 0x41, 0x61, 0x62, 0x61, 0x42, 0x61, 0x63, 0x61, 0x43, 0x61, 0x64, 0x61, 0x44, 0x61, 0x65, 0x61, 0x45, // {"a": "A", "b": "B", "c": "C", "d": "D", "e": "E"}
    };

    Obj obj;
    Err err;
    auto ptr = test;
    auto end = test + sizeof(test);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 1);
    ASSERT_EQ(obj.type, TYPE_MAP);
    ASSERT_EQ(obj.map.size(), 0);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 6);
    ASSERT_EQ(obj.type, TYPE_MAP);
    ASSERT_EQ(obj.map.size(), 2);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 15);
    ASSERT_EQ(obj.type, TYPE_MAP);
    ASSERT_EQ(obj.map.size(), 2);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 36);
    ASSERT_EQ(obj.type, TYPE_MAP);
    ASSERT_EQ(obj.map.size(), 5);

    ASSERT_EQ(ptr, end);
}

TEST(Decode, IndefData)
{
    const byte test[] = {
        0x5f, 0x42, 0x01, 0x02, 0x43, 0x03, 0x04, 0x05, 0xff, // (_ h'0102', h'030405')
    };

    Obj obj;
    Err err;
    auto ptr = test;
    auto end = test + sizeof(test);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 9);
    ASSERT_EQ(obj.type, TYPE_INDEF_DATA);

    ASSERT_EQ(ptr, end);
}

TEST(Decode, IndefText)
{
    const byte test[] = {
        0x7f, 0x65, 0x73, 0x74, 0x72, 0x65, 0x61, 0x64, 0x6d, 0x69, 0x6e, 0x67, 0xff, // (_ "strea", "ming")
    };

    Obj obj;
    Err err;
    auto ptr = test;
    auto end = test + sizeof(test);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 13);
    ASSERT_EQ(obj.type, TYPE_INDEF_TEXT);

    ASSERT_EQ(ptr, end);
}

TEST(Decode, IndefArray)
{
    const byte test[] = {
        0x9f, 0xff, // [_ ]
        0x9f, 0x01, 0x82, 0x02, 0x03, 0x9f, 0x04, 0x05, 0xff, 0xff, // [_ 1, [2, 3], [_ 4, 5]]
        0x9f, 0x01, 0x82, 0x02, 0x03, 0x82, 0x04, 0x05, 0xff, // [_ 1, [2, 3], [4, 5]]
        0x83, 0x01, 0x82, 0x02, 0x03, 0x9f, 0x04, 0x05, 0xff, // [1, [2, 3], [_ 4, 5]]
        0x83, 0x01, 0x9f, 0x02, 0x03, 0xff, 0x82, 0x04, 0x05, // [1, [_ 2, 3], [4, 5]]
        0x9f, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x18, 0x18, 0x19, 0xff, // [_ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]
    };

    Obj obj;
    Err err;
    auto ptr = test;
    auto end = test + sizeof(test);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 2);
    ASSERT_EQ(obj.type, TYPE_ARRAY);
    ASSERT_EQ(obj.arr.indef(), true);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 12);
    ASSERT_EQ(obj.type, TYPE_ARRAY);
    ASSERT_EQ(obj.arr.indef(), true);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 21);
    ASSERT_EQ(obj.type, TYPE_ARRAY);
    ASSERT_EQ(obj.arr.indef(), true);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 30);
    ASSERT_EQ(obj.type, TYPE_ARRAY);
    ASSERT_EQ(obj.arr.indef(), false);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 39);
    ASSERT_EQ(obj.type, TYPE_ARRAY);
    ASSERT_EQ(obj.arr.indef(), false);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 68);
    ASSERT_EQ(obj.type, TYPE_ARRAY);
    ASSERT_EQ(obj.arr.indef(), true);

    ASSERT_EQ(ptr, end);
}

TEST(Decode, IndefMap)
{
    const byte test[] = {
        0xbf, 0x61, 0x61, 0x01, 0x61, 0x62, 0x9f, 0x02, 0x03, 0xff, 0xff, // {_ "a": 1, "b": [_ 2, 3]}
        0xbf, 0x63, 0x46, 0x75, 0x6e, 0xf5, 0x63, 0x41, 0x6d, 0x74, 0x21, 0xff, // {_ "Fun": true, "Amt": -2}
    };

    Obj obj;
    Err err;
    auto ptr = test;
    auto end = test + sizeof(test);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 11);
    ASSERT_EQ(obj.type, TYPE_MAP);
    ASSERT_EQ(obj.map.indef(), true);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 23);
    ASSERT_EQ(obj.type, TYPE_MAP);
    ASSERT_EQ(obj.map.indef(), true);

    ASSERT_EQ(ptr, end);
}

TEST(Decode, Mixed)
{
    const byte test[] = {
        0x82, 0x61, 0x61, 0xa1, 0x61, 0x62, 0x61, 0x63, // ["a", {"b": "c"}]
        0x82, 0x61, 0x61, 0xbf, 0x61, 0x62, 0x61, 0x63, 0xff, // ["a", {_ "b": "c"}]
    };

    Obj obj;
    Err err;
    auto ptr = test;
    auto end = test + sizeof(test);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 8);
    ASSERT_EQ(obj.type, TYPE_ARRAY);
    ASSERT_EQ(obj.arr.size(), 2);

    std::tie(obj, err, ptr) = decode(ptr, end);

    ASSERT_EQ(err, ERR_OK);
    ASSERT_EQ(ptr, test + 17);
    ASSERT_EQ(obj.type, TYPE_ARRAY);
    ASSERT_EQ(obj.arr.size(), 2);

    ASSERT_EQ(ptr, end);
}