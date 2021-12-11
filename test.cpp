#include <gtest/gtest.h>
#include "cbor.h"
#include "cbor_codec.h"

using namespace cbor;

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
        "\x63\xe6\xb0\xb4";

    Codec<sizeof(expected)> codec;

    codec.encode((char*) nullptr, 0);
    codec.encode("a", strlen("a"));
    codec.encode("IETF", strlen("IETF"));
    codec.encode("\"\\", strlen("\"\\"));
    codec.encode("\u00fc", strlen("\u00fc"));
    codec.encode("\u6c34", strlen("\u6c34"));    

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
        "\x98\x19\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d"
        "\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x18\x18\x19";

    Codec<sizeof(expected)> codec;
    Pool<64> pool;

    Array arr_0;
    Array arr_1;
    Array arr_2;
    Array arr_3;

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

    codec.encode(arr_0);
    codec.encode(arr_1);
    codec.encode(arr_2);
    codec.encode(arr_3);

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
        "\xa2\x61\x61\x01\x61\x62\x82\x02\x03";

    Codec<sizeof(expected)> codec;
    Pool<16> pool;

    Map map_0;
    Map map_1;
    Map map_2;

    map_1.push(pool.make(1), pool.make(2));
    map_1.push(pool.make(3), pool.make(4));

    CBOR *arr = pool.make(Array());
    Array *arr_p = &arr->arr;

    arr_p->push(pool.make(2));
    arr_p->push(pool.make(3));

    map_2.push(pool.make("a", 1), pool.make(1));
    map_2.push(pool.make("b", 1), arr);

    codec.encode(map_0);
    codec.encode(map_1);
    codec.encode(map_2);

    size_t exp_len = sizeof(expected) - 1;

    ASSERT_EQ(codec.size(), exp_len);
    
    for (size_t i = 0; i < exp_len; ++i)
        ASSERT_EQ(codec.buf[i], expected[i]) << "differ at index " << i;
}

TEST(Encode, Tag)
{
    const uint8_t expected[] =
        "\xc1\x1a\x51\x4b\x67\xb0";

    Codec<sizeof(expected)> codec;
    Pool<4> pool;

    Tag tag = {1, pool.make(1363896240)};
    
    codec.encode(tag);

    size_t exp_len = sizeof(expected) - 1;

    ASSERT_EQ(codec.size(), exp_len);
    
    for (size_t i = 0; i < exp_len; ++i)
        ASSERT_EQ(codec.buf[i], expected[i]) << "differ at index " << i;
}