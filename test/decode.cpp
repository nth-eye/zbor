#include <gtest/gtest.h>
#include <cmath>
#include "zbor/decode.h"

using namespace zbor;

TEST(Decode, OutOfBounds)
{
    const byte test[] = { 0x00 };

    auto [o, e, p] = decode(test + sizeof(test), test + sizeof(test));

    ASSERT_EQ(e, err_out_of_bounds);
    ASSERT_EQ(p, test + sizeof(test));
    ASSERT_EQ(o.type, type_invalid);
}

TEST(Decode, Unsigned)
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

    item o;
    err e;
    auto p = test;
    auto end = test + sizeof(test);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 1);
    ASSERT_EQ(o.type, type_uint);
    ASSERT_EQ(o.uint, 0);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 2);
    ASSERT_EQ(o.type, type_uint);
    ASSERT_EQ(o.uint, 1);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 3);
    ASSERT_EQ(o.type, type_uint);
    ASSERT_EQ(o.uint, 10);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 4);
    ASSERT_EQ(o.type, type_uint);
    ASSERT_EQ(o.uint, 23);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 6);
    ASSERT_EQ(o.type, type_uint);
    ASSERT_EQ(o.uint, 24);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 8);
    ASSERT_EQ(o.type, type_uint);
    ASSERT_EQ(o.uint, 25);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 10);
    ASSERT_EQ(o.type, type_uint);
    ASSERT_EQ(o.uint, 100);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 13);
    ASSERT_EQ(o.type, type_uint);
    ASSERT_EQ(o.uint, 1000);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 18);
    ASSERT_EQ(o.type, type_uint);
    ASSERT_EQ(o.uint, 1000000);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 27);
    ASSERT_EQ(o.type, type_uint);
    ASSERT_EQ(o.uint, 1000000000000);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 36);
    ASSERT_EQ(o.type, type_uint);
    ASSERT_EQ(o.uint, 18446744073709551615u);

    ASSERT_EQ(p, end);
}

TEST(Decode, Signed)
{
    const byte test[] = {
        0x20, // -1
        0x29, // -10
        0x38, 0x63, // -100
        0x39, 0x03, 0xe7, // -1000
        0x3b, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // -9223372036854775808
        0x3b, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // -18446744073709551616
    };

    item o;
    err e;
    auto p = test;
    auto end = test + sizeof(test);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 1);
    ASSERT_EQ(o.type, type_sint);
    ASSERT_EQ(o.sint, -1);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 2);
    ASSERT_EQ(o.type, type_sint);
    ASSERT_EQ(o.sint, -10);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 4);
    ASSERT_EQ(o.type, type_sint);
    ASSERT_EQ(o.sint, -100);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 7);
    ASSERT_EQ(o.type, type_sint);
    ASSERT_EQ(o.sint, -1000);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 16);
    ASSERT_EQ(o.type, type_sint);
    ASSERT_EQ(o.sint, -9223372036854775807 - 1);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 25);
    ASSERT_EQ(o.type, type_sint);
    ASSERT_EQ(o.sint, 0); // NOTE: can't represent -18446744073709551616

    ASSERT_EQ(p, end);
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

    item o;
    err e;
    auto p = test;
    auto end = test + sizeof(test);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 3);
    ASSERT_EQ(o.type, type_double);
    ASSERT_DOUBLE_EQ(o.dbl, 0.0);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 6);
    ASSERT_EQ(o.type, type_double);
    ASSERT_DOUBLE_EQ(o.dbl, -0.0);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 9);
    ASSERT_EQ(o.type, type_double);
    ASSERT_DOUBLE_EQ(o.dbl, 1.0);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 18);
    ASSERT_EQ(o.type, type_double);
    ASSERT_DOUBLE_EQ(o.dbl, 1.1);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 21);
    ASSERT_EQ(o.type, type_double);
    ASSERT_DOUBLE_EQ(o.dbl, 1.5);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 24);
    ASSERT_EQ(o.type, type_double);
    ASSERT_DOUBLE_EQ(o.dbl, 65504.0);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 29);
    ASSERT_EQ(o.type, type_double);
    ASSERT_DOUBLE_EQ(o.dbl, 100000.0);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 34);
    ASSERT_EQ(o.type, type_double);
    ASSERT_DOUBLE_EQ(o.dbl, 3.4028234663852886e+38);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 43);
    ASSERT_EQ(o.type, type_double);
    ASSERT_DOUBLE_EQ(o.dbl, 1.0e+300);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 46);
    ASSERT_EQ(o.type, type_double);
    ASSERT_DOUBLE_EQ(o.dbl, 5.960464477539063e-8);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 49);
    ASSERT_EQ(o.type, type_double);
    ASSERT_DOUBLE_EQ(o.dbl, 0.00006103515625);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 52);
    ASSERT_EQ(o.type, type_double);
    ASSERT_DOUBLE_EQ(o.dbl, -4.0);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 61);
    ASSERT_EQ(o.type, type_double);
    ASSERT_DOUBLE_EQ(o.dbl, -4.1);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 64);
    ASSERT_EQ(o.type, type_double);
    ASSERT_DOUBLE_EQ(o.dbl, double(INFINITY));

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 67);
    ASSERT_EQ(o.type, type_double);
    ASSERT_TRUE(std::isnan(o.dbl));

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 70);
    ASSERT_EQ(o.type, type_double);
    ASSERT_DOUBLE_EQ(o.dbl, double(-INFINITY));

    ASSERT_EQ(p, end);
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

    item o;
    err e;
    auto p = test;
    auto end = test + sizeof(test);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 1);
    ASSERT_EQ(o.type, type_prim);
    ASSERT_EQ(o.prim, zbor::prim_false);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 2);
    ASSERT_EQ(o.type, type_prim);
    ASSERT_EQ(o.prim, zbor::prim_true);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 3);
    ASSERT_EQ(o.type, type_prim);
    ASSERT_EQ(o.prim, zbor::prim_null);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 4);
    ASSERT_EQ(o.type, type_prim);
    ASSERT_EQ(o.prim, zbor::prim_undefined);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 5);
    ASSERT_EQ(o.type, type_prim);
    ASSERT_EQ(o.prim, prim_t(16));

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 7);
    ASSERT_EQ(o.type, type_prim);
    ASSERT_EQ(o.prim, prim_t(255));

    ASSERT_EQ(p, end);
}

TEST(Decode, Data)
{
    const byte test[] = {
        0x40, // h''
        0x44, 0x01, 0x02, 0x03, 0x04, // h'01020304'
    };
    const byte first[1] = {};
    const byte second[] = {0x01, 0x02, 0x03, 0x04};

    item o;
    err e;
    auto p = test;
    auto end = test + sizeof(test);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 1);
    ASSERT_EQ(o.type, type_data);
    ASSERT_EQ(o.data.size(), 0);
    ASSERT_EQ(memcmp(first, o.data.data(), o.data.size()), 0);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 6);
    ASSERT_EQ(o.type, type_data);
    ASSERT_EQ(o.data.size(), 4);
    ASSERT_EQ(memcmp(second, o.data.data(), o.data.size()), 0);

    ASSERT_EQ(p, end);
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

    item o;
    err e;
    auto p = test;
    auto end = test + sizeof(test);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 1);
    ASSERT_EQ(o.type, type_text);
    ASSERT_EQ(o.text, "");

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 3);
    ASSERT_EQ(o.type, type_text);
    ASSERT_EQ(o.text, "a");

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 8);
    ASSERT_EQ(o.type, type_text);
    ASSERT_EQ(o.text, "IETF");

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 11);
    ASSERT_EQ(o.type, type_text);
    ASSERT_EQ(o.text, "\"\\");

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 14);
    ASSERT_EQ(o.type, type_text);
    ASSERT_EQ(o.text, "\u00fc");

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 18);
    ASSERT_EQ(o.type, type_text);
    ASSERT_EQ(o.text, "\u6c34");

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 23);
    ASSERT_EQ(o.type, type_text);
    ASSERT_EQ(o.text, text_t(garbage, sizeof(garbage)));

    ASSERT_EQ(p, end);
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

    item o;
    item content;
    err e;
    auto p = test;
    auto end = test + sizeof(test);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 22);
    ASSERT_EQ(o.type, type_tag);
    ASSERT_EQ(o.tag.num(), 0);

    content = o.tag.content();
    ASSERT_EQ(content.type, type_text);
    ASSERT_EQ(content.text, "2013-03-21T20:04:00Z");

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 28);
    ASSERT_EQ(o.type, type_tag);
    ASSERT_EQ(o.tag.num(), 1);

    content = o.tag.content();
    ASSERT_EQ(content.type, type_uint);
    ASSERT_EQ(content.uint, 1363896240);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 38);
    ASSERT_EQ(o.type, type_tag);
    ASSERT_EQ(o.tag.num(), 1);

    content = o.tag.content();
    ASSERT_EQ(content.type, type_double);
    ASSERT_DOUBLE_EQ(content.dbl, 1363896240.5);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 44);
    ASSERT_EQ(o.type, type_tag);
    ASSERT_EQ(o.tag.num(), 23);

    content = o.tag.content();
    ASSERT_EQ(content.type, type_data);
    ASSERT_EQ(content.data.size(), sizeof(data_1));
    ASSERT_EQ(memcmp(data_1, content.data.data(), content.data.size()), 0);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 52);
    ASSERT_EQ(o.type, type_tag);
    ASSERT_EQ(o.tag.num(), 24);

    content = o.tag.content();
    ASSERT_EQ(content.type, type_data);
    ASSERT_EQ(content.data.size(), sizeof(data_2));
    ASSERT_EQ(memcmp(data_2, content.data.data(), content.data.size()), 0);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 77);
    ASSERT_EQ(o.type, type_tag);
    ASSERT_EQ(o.tag.num(), 32);

    content = o.tag.content();
    ASSERT_EQ(content.type, type_text);
    ASSERT_EQ(content.text, "http://www.example.com");

    ASSERT_EQ(p, end);
}

TEST(Decode, Array)
{
    const byte test[] = {
        0x80, // []
        0x83, 0x01, 0x02, 0x03, // [1, 2, 3]
        0x83, 0x01, 0x82, 0x02, 0x03, 0x82, 0x04, 0x05, // [1, [2, 3], [4, 5]]
        0x98, 0x19, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x18, 0x18, 0x19, // [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]
    };

    item o;
    err e;
    auto p = test;
    auto end = test + sizeof(test);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 1);
    ASSERT_EQ(o.type, type_array);
    ASSERT_EQ(o.arr.size(), 0);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 5);
    ASSERT_EQ(o.type, type_array);
    ASSERT_EQ(o.arr.size(), 3);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 13);
    ASSERT_EQ(o.type, type_array);
    ASSERT_EQ(o.arr.size(), 3);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 42);
    ASSERT_EQ(o.type, type_array);
    ASSERT_EQ(o.arr.size(), 25);

    ASSERT_EQ(p, end);
}

TEST(Decode, Map)
{
    const byte test[] = {
        0xa0, // {}
        0xa2, 0x01, 0x02, 0x03, 0x04, // {1: 2, 3: 4}
        0xa2, 0x61, 0x61, 0x01, 0x61, 0x62, 0x82, 0x02, 0x03, // {"a": 1, "b": [2, 3]}
        0xa5, 0x61, 0x61, 0x61, 0x41, 0x61, 0x62, 0x61, 0x42, 0x61, 0x63, 0x61, 0x43, 0x61, 0x64, 0x61, 0x44, 0x61, 0x65, 0x61, 0x45, // {"a": "A", "b": "B", "c": "C", "d": "D", "e": "E"}
    };

    item o;
    err e;
    auto p = test;
    auto end = test + sizeof(test);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 1);
    ASSERT_EQ(o.type, type_map);
    ASSERT_EQ(o.map.size(), 0);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 6);
    ASSERT_EQ(o.type, type_map);
    ASSERT_EQ(o.map.size(), 2);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 15);
    ASSERT_EQ(o.type, type_map);
    ASSERT_EQ(o.map.size(), 2);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 36);
    ASSERT_EQ(o.type, type_map);
    ASSERT_EQ(o.map.size(), 5);

    ASSERT_EQ(p, end);
}

TEST(Decode, IndefData)
{
    const byte test[] = {
        0x5f, 0x42, 0x01, 0x02, 0x43, 0x03, 0x04, 0x05, 0xff, // (_ h'0102', h'030405')
    };

    item o;
    err e;
    auto p = test;
    auto end = test + sizeof(test);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 9);
    ASSERT_EQ(o.type, type_indef_data);

    ASSERT_EQ(p, end);
}

TEST(Decode, IndefText)
{
    const byte test[] = {
        0x7f, 0x65, 0x73, 0x74, 0x72, 0x65, 0x61, 0x64, 0x6d, 0x69, 0x6e, 0x67, 0xff, // (_ "strea", "ming")
    };

    item o;
    err e;
    auto p = test;
    auto end = test + sizeof(test);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 13);
    ASSERT_EQ(o.type, type_indef_text);

    ASSERT_EQ(p, end);
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

    item o;
    err e;
    auto p = test;
    auto end = test + sizeof(test);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 2);
    ASSERT_EQ(o.type, type_array);
    ASSERT_EQ(o.arr.indef(), true);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 12);
    ASSERT_EQ(o.type, type_array);
    ASSERT_EQ(o.arr.indef(), true);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 21);
    ASSERT_EQ(o.type, type_array);
    ASSERT_EQ(o.arr.indef(), true);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 30);
    ASSERT_EQ(o.type, type_array);
    ASSERT_EQ(o.arr.indef(), false);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 39);
    ASSERT_EQ(o.type, type_array);
    ASSERT_EQ(o.arr.indef(), false);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 68);
    ASSERT_EQ(o.type, type_array);
    ASSERT_EQ(o.arr.indef(), true);

    ASSERT_EQ(p, end);
}

TEST(Decode, IndefMap)
{
    const byte test[] = {
        0xbf, 0x61, 0x61, 0x01, 0x61, 0x62, 0x9f, 0x02, 0x03, 0xff, 0xff, // {_ "a": 1, "b": [_ 2, 3]}
        0xbf, 0x63, 0x46, 0x75, 0x6e, 0xf5, 0x63, 0x41, 0x6d, 0x74, 0x21, 0xff, // {_ "Fun": true, "Amt": -2}
    };

    item o;
    err e;
    auto p = test;
    auto end = test + sizeof(test);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 11);
    ASSERT_EQ(o.type, type_map);
    ASSERT_EQ(o.map.indef(), true);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 23);
    ASSERT_EQ(o.type, type_map);
    ASSERT_EQ(o.map.indef(), true);

    ASSERT_EQ(p, end);
}

TEST(Decode, Mixed)
{
    const byte test[] = {
        0x82, 0x61, 0x61, 0xa1, 0x61, 0x62, 0x61, 0x63, // ["a", {"b": "c"}]
        0x82, 0x61, 0x61, 0xbf, 0x61, 0x62, 0x61, 0x63, 0xff, // ["a", {_ "b": "c"}]
    };

    item o;
    err e;
    auto p = test;
    auto end = test + sizeof(test);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 8);
    ASSERT_EQ(o.type, type_array);
    ASSERT_EQ(o.arr.size(), 2);

    std::tie(o, e, p) = decode(p, end);

    ASSERT_EQ(e, err_ok);
    ASSERT_EQ(p, test + 17);
    ASSERT_EQ(o.type, type_array);
    ASSERT_EQ(o.arr.size(), 2);

    ASSERT_EQ(p, end);
}