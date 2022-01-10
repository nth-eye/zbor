#include <gtest/gtest.h>
#include "zbor.h"

using namespace zbor;

TEST(Encode, Uint) 
{
    const uint8_t expected[] =
        "\x00"
        "\x01"
        "\x0a"
        "\x17"
        "\x18\x18"
        "\x18\x64"
        "\x19\x03\xe8"
        "\x1a\x00\x0f\x42\x40"
        "\x1b\x00\x00\x00\xe8\xd4\xa5\x10\x00"
        "\x1b\xff\xff\xff\xff\xff\xff\xff\xff";

    Codec<sizeof(expected)> codec;

    codec.encode(0);
    codec.encode(1);
    codec.encode(10);
    codec.encode(23);
    codec.encode(24);
    codec.encode(100);
    codec.encode(1000);
    codec.encode(1000000);
    codec.encode(1000000000000);
    codec.encode(18446744073709551615ul);

    size_t exp_len = sizeof(expected) - 1;

    ASSERT_EQ(codec.size(), exp_len);
    
    for (size_t i = 0; i < exp_len; ++i)
        ASSERT_EQ(codec.buf[i], expected[i]) << "differ at index " << i;
}

TEST(Encode, Sint)
{
    const uint8_t expected[] =
        "\x20"
        "\x29"
        "\x38\x63"
        "\x39\x03\xe7";

    Codec<sizeof(expected)> codec;

    codec.encode(-1);
    codec.encode(-10);
    codec.encode(-100);
    codec.encode(-1000);

    size_t exp_len = sizeof(expected) - 1;

    ASSERT_EQ(codec.size(), exp_len);
    
    for (size_t i = 0; i < exp_len; ++i)
        ASSERT_EQ(codec.buf[i], expected[i]) << "differ at index " << i;
}

TEST(Encode, Data)
{
    const uint8_t expected[] =
        "\x40"
        "\x44\x01\x02\x03\x04";

    Codec<sizeof(expected)> codec;

    codec.encode((uint8_t*) nullptr, 0);
    codec.encode((uint8_t*) "\x01\x02\x03\x04", 4);

    size_t exp_len = sizeof(expected) - 1;

    ASSERT_EQ(codec.size(), exp_len);
    
    for (size_t i = 0; i < exp_len; ++i)
        ASSERT_EQ(codec.buf[i], expected[i]) << "differ at index " << i;
}

TEST(Encode, Text)
{
    const uint8_t expected[] =
        "\x60"
        "\x61\x61"
        "\x64\x49\x45\x54\x46"
        "\x62\x22\x5c"
        "\x62\xc3\xbc"
        "\x63\xe6\xb0\xb4"
        "\x64\xf0\x90\x85\x91";

    Codec<sizeof(expected)> codec;

    const unsigned char test[] = { 0xf0, 0x90, 0x85, 0x91 };

    codec.encode((char*) nullptr, 0);
    codec.encode("a", strlen("a"));
    codec.encode("IETF", strlen("IETF"));
    codec.encode("\"\\", strlen("\"\\"));
    codec.encode("\u00fc", strlen("\u00fc"));
    codec.encode("\u6c34", strlen("\u6c34"));
    codec.encode((char*) test, sizeof(test));   

    size_t exp_len = sizeof(expected) - 1;

    ASSERT_EQ(codec.size(), exp_len);
    
    for (size_t i = 0; i < exp_len; ++i)
        ASSERT_EQ(codec.buf[i], expected[i]) << "differ at index " << i;
}

TEST(Encode, Array)
{
    const uint8_t expected[] =
        "\x80"
        "\x83\x01\x02\x03"
        "\x83\x01\x82\x02\x03\x82\x04\x05"
        "\x98\x19\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x18\x18\x19"
        "\x82\x61\x61\xa1\x61\x62\x61\x63";

    Codec<sizeof(expected)> codec;
    Pool<64> pool;

    Array arr_0, arr_1, arr_2, arr_3, arr_4;

    arr_1.push(pool.make(1));
    arr_1.push(pool.make(2));
    arr_1.push(pool.make(3));

    arr_2.push(pool.make(1));

    CBOR *arr_2_1 = pool.make(Array());

    arr_2_1->arr.push(pool.make(2));
    arr_2_1->arr.push(pool.make(3));

    CBOR *arr_2_2 = pool.make(Array());

    arr_2_2->arr.push(pool.make(4));
    arr_2_2->arr.push(pool.make(5));

    arr_2.push(arr_2_1);
    arr_2.push(arr_2_2);

    for (int i = 1; i < 26; ++i)
        arr_3.push(pool.make(i));

    CBOR *map = pool.make(Map());

    map->map.push(pool.make("b", 1), pool.make("c", 1));
    
    arr_4.push(pool.make("a", 1));
    arr_4.push(map);

    codec.encode(arr_0);
    codec.encode(arr_1);
    codec.encode(arr_2);
    codec.encode(arr_3);
    codec.encode(arr_4);

    size_t exp_len = sizeof(expected) - 1;

    ASSERT_EQ(codec.size(), exp_len);
    
    for (size_t i = 0; i < exp_len; ++i)
        ASSERT_EQ(codec.buf[i], expected[i]) << "differ at index " << i;
}

TEST(Encode, Map)
{
    const uint8_t expected[] =
        "\xa0"
        "\xa2\x01\x02\x03\x04"
        "\xa2\x61\x61\x01\x61\x62\x82\x02\x03"
        "\xa5\x61\x61\x61\x41\x61\x62\x61\x42\x61\x63\x61\x43\x61\x64\x61\x44\x61\x65\x61\x45";

    Codec<sizeof(expected)> codec;
    Pool<32> pool;

    Map map_0;
    Map map_1;
    Map map_2;
    Map map_3;

    map_1.push(pool.make(1), pool.make(2));
    map_1.push(pool.make(3), pool.make(4));

    CBOR *arr = pool.make(Array());
    Array *arr_p = &arr->arr;

    arr_p->push(pool.make(2));
    arr_p->push(pool.make(3));

    map_2.push(pool.make("a", 1), pool.make(1));
    map_2.push(pool.make("b", 1), arr);

    map_3.push(pool.make("a", 1), pool.make("A", 1));
    map_3.push(pool.make("b", 1), pool.make("B", 1));
    map_3.push(pool.make("c", 1), pool.make("C", 1));
    map_3.push(pool.make("d", 1), pool.make("D", 1));
    map_3.push(pool.make("e", 1), pool.make("E", 1));

    codec.encode(map_0);
    codec.encode(map_1);
    codec.encode(map_2);
    codec.encode(map_3);

    size_t exp_len = sizeof(expected) - 1;

    ASSERT_EQ(codec.size(), exp_len);
    
    for (size_t i = 0; i < exp_len; ++i)
        ASSERT_EQ(codec.buf[i], expected[i]) << "differ at index " << i;
}

TEST(Encode, Tag)
{
    const uint8_t expected[] =
        "\xc0\x74\x32\x30\x31\x33\x2d\x30\x33\x2d\x32\x31\x54\x32\x30\x3a\x30\x34\x3a\x30\x30\x5a"
        "\xc1\x1a\x51\x4b\x67\xb0"
        "\xc1\xfb\x41\xd4\x52\xd9\xec\x20\x00\x00"
        "\xd7\x44\x01\x02\x03\x04"
        "\xd8\x18\x45\x64\x49\x45\x54\x46"
        "\xd8\x20\x76\x68\x74\x74\x70\x3a\x2f\x2f\x77\x77\x77\x2e\x65\x78\x61\x6d\x70\x6c\x65\x2e\x63\x6f\x6d";

    Codec<sizeof(expected)> codec;
    Pool<8> pool;
    
    codec.encode({0, pool.make("2013-03-21T20:04:00Z", strlen("2013-03-21T20:04:00Z"))});
    codec.encode({1, pool.make(1363896240)});
    codec.encode({1, pool.make(1363896240.5)});
    codec.encode({23, pool.make((uint8_t*) "\x01\x02\x03\x04", 4)});
    codec.encode({24, pool.make((uint8_t*) "\x64\x49\x45\x54\x46", 5)});
    codec.encode({32, pool.make("http://www.example.com", strlen("http://www.example.com"))});

    size_t exp_len = sizeof(expected) - 1;

    ASSERT_EQ(codec.size(), exp_len);
    
    for (size_t i = 0; i < exp_len; ++i)
        ASSERT_EQ(codec.buf[i], expected[i]) << "differ at index " << i;
}

TEST(Encode, Simple)
{
    const uint8_t expected[] =
        "\xf4"
        "\xf5"
        "\xf4"
        "\xf5"
        "\xf6"
        "\xf7"
        "\xf0"
        "\xf8\xff";

    Codec<sizeof(expected)> codec;

    codec.encode(false);
    codec.encode(true);
    codec.encode(PRIM_FALSE);
    codec.encode(PRIM_TRUE);
    codec.encode(PRIM_NULL);
    codec.encode(PRIM_UNDEFINED);
    codec.encode(Prim(16));
    codec.encode(Prim(255));

    size_t exp_len = sizeof(expected) - 1;

    ASSERT_EQ(codec.size(), exp_len);
    
    for (size_t i = 0; i < exp_len; ++i)
        ASSERT_EQ(codec.buf[i], expected[i]) << "differ at index " << i;
}

TEST(Encode, Float)
{
    const uint8_t expected[] =
        "\xf9\x00\x00"
        "\xf9\x80\x00"
        "\xf9\x3c\x00"
        "\xfb\x3f\xf1\x99\x99\x99\x99\x99\x9a"
        "\xf9\x3e\x00"
        "\xf9\x7b\xff"
        "\xfa\x47\xc3\x50\x00"
        "\xfa\x7f\x7f\xff\xff"
        "\xfb\x7e\x37\xe4\x3c\x88\x00\x75\x9c"
        "\xf9\x00\x01"
        "\xf9\x04\x00"
        "\xf9\xc4\x00"
        "\xfb\xc0\x10\x66\x66\x66\x66\x66\x66"
        "\xf9\x7c\x00"
        "\xf9\x7e\x00"
        "\xf9\xfc\x00";

    Codec<sizeof(expected)> codec;

    codec.encode(0.0);
    codec.encode(-0.0);
    codec.encode(1.0);
    codec.encode(1.1);
    codec.encode(1.5);
    codec.encode(65504.0);
    codec.encode(100000.0);
    codec.encode(3.4028234663852886e+38);
    codec.encode(1.0e+300);
    codec.encode(5.960464477539063e-8);
    codec.encode(0.00006103515625);
    codec.encode(-4.0);
    codec.encode(-4.1);
    codec.encode(INFINITY);
    codec.encode(NAN);
    codec.encode(-INFINITY);

    size_t exp_len = sizeof(expected) - 1;

    ASSERT_EQ(codec.size(), exp_len);
    
    for (size_t i = 0; i < exp_len; ++i)
        ASSERT_EQ(codec.buf[i], expected[i]) << "differ at index " << i;
}

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