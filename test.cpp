#include <gtest/gtest.h>
#include "zbor.h"

using namespace zbor;

TEST(Decode, Uint)
{
    CBOR expected[] = {
        0,
        1,
        10,
        23,
        24,
        100,
        1000,
        1000000,
        1000000000000,
        18446744073709551615ul,
    };

    const uint8_t encoded[] = {
        0x00,
        0x01,
        0x0a,
        0x17,
        0x18, 0x18,
        0x18, 0x64,
        0x19, 0x03, 0xe8,
        0x1a, 0x00, 0x0f, 0x42, 0x40,
        0x1b, 0x00, 0x00, 0x00, 0xe8, 0xd4, 0xa5, 0x10, 0x00,
        0x1b, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    };

    constexpr auto size = sizeof(expected) / sizeof(expected[0]);

    Pool<size> pool; 

    auto ret = decode(pool, encoded, sizeof(encoded));

    ASSERT_EQ(ret.err, NO_ERR);
    ASSERT_EQ(ret.len, size);
    ASSERT_NE(ret.root, nullptr);

    int i = 0;

    for (auto it : *ret.root) {
        ASSERT_EQ(expected[i].type, it.type);
        ASSERT_EQ(expected[i].uint, it.uint);
        ++i;
    }
}

TEST(Decode, Sint)
{
    CBOR expected[] = {
        -1,
        -10,
        -100,
        -1000,
    };

    const uint8_t encoded[] = {
        0x20,
        0x29,
        0x38, 0x63,
        0x39, 0x03, 0xe7,
    };

    constexpr auto size = sizeof(expected) / sizeof(expected[0]);

    Pool<size> pool; 

    auto ret = decode(pool, encoded, sizeof(encoded));

    ASSERT_EQ(ret.err, NO_ERR);
    ASSERT_EQ(ret.len, size);
    ASSERT_NE(ret.root, nullptr);
    
    int i = 0;

    for (auto it : *ret.root) {
        ASSERT_EQ(expected[i].type, it.type);
        ASSERT_EQ(expected[i].sint, it.sint);
        ++i;
    }
}

TEST(Decode, Data)
{
    CBOR expected[] = {
        {(uint8_t*) nullptr, 0},
        {(uint8_t*) "\x01\x02\x03\x04", 4},
    };

    const uint8_t encoded[] = {
        0x40,
        0x44, 0x01, 0x02, 0x03, 0x04,
    };

    constexpr auto size = sizeof(expected) / sizeof(expected[0]);

    Pool<size> pool; 

    auto ret = decode(pool, encoded, sizeof(encoded));

    ASSERT_EQ(ret.err, NO_ERR);
    ASSERT_EQ(ret.len, size);
    ASSERT_NE(ret.root, nullptr);
    
    int i = 0;

    for (auto it : *ret.root) {
        ASSERT_EQ(expected[i].type, it.type);
        ASSERT_EQ(expected[i].str.len, it.str.len);
        for (size_t j = 0; j < it.str.len; ++j)
            ASSERT_EQ(expected[i].str.data[j], it.str.data[j]);
        ++i;
    }
}

TEST(Decode, Text)
{
    const unsigned char test[] = { 0xf0, 0x90, 0x85, 0x91 };

    CBOR expected[] = {
        {(char*) nullptr, 0},
        {"a", strlen("a")},
        {"IETF", strlen("IETF")},
        {"\"\\", strlen("\"\\")},
        {"\u00fc", strlen("\u00fc")},
        {"\u6c34", strlen("\u6c34")},
        {(char*) test, sizeof(test)},  
    };

    const uint8_t encoded[] = {
        0x60,
        0x61, 0x61,
        0x64, 0x49, 0x45, 0x54, 0x46,
        0x62, 0x22, 0x5c,
        0x62, 0xc3, 0xbc,
        0x63, 0xe6, 0xb0, 0xb4,
        0x64, 0xf0, 0x90, 0x85, 0x91,
    };

    constexpr auto size = sizeof(expected) / sizeof(expected[0]);

    Pool<size> pool; 

    auto ret = decode(pool, encoded, sizeof(encoded));

    ASSERT_EQ(ret.err, NO_ERR);
    ASSERT_EQ(ret.len, size);
    ASSERT_NE(ret.root, nullptr);
    
    int i = 0;

    for (auto it : *ret.root) {
        ASSERT_EQ(expected[i].type, it.type);
        ASSERT_EQ(expected[i].str.len, it.str.len);
        for (size_t j = 0; j < it.str.len; ++j)
            ASSERT_EQ(expected[i].str.text[j], it.str.text[j]);
        ++i;
    }
}

TEST(Decode, Array)
{
    // CBOR expected[] = {
    //     -1,
    //     -10,
    //     -100,
    //     -1000,
    // };

    // TODO

    const uint8_t encoded[] = {
        0x80,
        0x83, 0x01, 0x02, 0x03,
        0x83, 0x01, 0x82, 0x02, 0x03, 0x82, 0x04, 0x05,
        0x98, 0x19, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x18, 0x18, 0x19,
        0x82, 0x61, 0x61, 0xa1, 0x61, 0x62, 0x61, 0x63,
    };

    // constexpr auto size = sizeof(expected) / sizeof(expected[0]);

    // Pool<size> pool; 

    // auto ret = decode(pool, encoded, sizeof(encoded));

    // ASSERT_EQ(ret.err, NO_ERR);
    // ASSERT_EQ(ret.len, size);
    // ASSERT_NE(ret.root, nullptr);
    
    // int i = 0;

    // for (auto it : *ret.root) {
    //     ASSERT_EQ(expected[i].type, it.type);
    //     ASSERT_EQ(expected[i].sint, it.sint);
    //     ++i;
    // }
}

TEST(Decode, Simple)
{
    CBOR expected[] = {
        false,
        true,
        PRIM_FALSE,
        PRIM_TRUE,
        PRIM_NULL,
        PRIM_UNDEFINED,
        Prim(16),
        Prim(255),
    };

    const uint8_t encoded[] = {
        0xf4,
        0xf5,
        0xf4,
        0xf5,
        0xf6,
        0xf7,
        0xf0,
        0xf8, 0xff,
    };

    constexpr auto size = sizeof(expected) / sizeof(expected[0]);

    Pool<size> pool; 

    auto ret = decode(pool, encoded, sizeof(encoded));

    ASSERT_EQ(ret.err, NO_ERR);
    ASSERT_EQ(ret.len, size);
    ASSERT_NE(ret.root, nullptr);
    
    int i = 0;

    for (auto it : *ret.root) {
        ASSERT_EQ(expected[i].type, it.type);
        ASSERT_EQ(expected[i].prim, it.prim);
        ++i;
    }
}

TEST(Decode, Float)
{
    CBOR expected[] = {
        0.0,
        -0.0,
        1.0,
        1.1,
        1.5,
        65504.0,
        100000.0,
        3.4028234663852886e+38,
        1.0e+300,
        5.960464477539063e-8,
        0.00006103515625,
        -4.0,
        -4.1,
        (double) INFINITY,
        (double) NAN,
        (double) -INFINITY,
    };

    const uint8_t encoded[] = {
        0xf9, 0x00, 0x00,
        0xf9, 0x80, 0x00,
        0xf9, 0x3c, 0x00,
        0xfb, 0x3f, 0xf1, 0x99, 0x99, 0x99, 0x99, 0x99, 0x9a,
        0xf9, 0x3e, 0x00,
        0xf9, 0x7b, 0xff,
        0xfa, 0x47, 0xc3, 0x50, 0x00,
        0xfa, 0x7f, 0x7f, 0xff, 0xff,
        0xfb, 0x7e, 0x37, 0xe4, 0x3c, 0x88, 0x00, 0x75, 0x9c,
        0xf9, 0x00, 0x01,
        0xf9, 0x04, 0x00,
        0xf9, 0xc4, 0x00,
        0xfb, 0xc0, 0x10, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
        0xf9, 0x7c, 0x00,
        0xf9, 0x7e, 0x00,
        0xf9, 0xfc, 0x00,
    };

    constexpr auto size = sizeof(expected) / sizeof(expected[0]);

    Pool<size> pool; 

    auto ret = decode(pool, encoded, sizeof(encoded));

    ASSERT_EQ(ret.err, NO_ERR);
    ASSERT_EQ(ret.len, size);
    ASSERT_NE(ret.root, nullptr);
    
    int i = 0;

    for (auto it : *ret.root) {
        ASSERT_EQ(expected[i].type, it.type);
        ASSERT_EQ(expected[i].d, it.d);
        ++i;
    }
}