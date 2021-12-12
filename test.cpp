#include <gtest/gtest.h>
#include "zbor.h"
#include "zbor_codec.h"

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