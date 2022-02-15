#include <gtest/gtest.h>
#include "zbor_encode.h"
#include <cmath>

using namespace zbor;

template<class E, size_t N>
void encode_check(E &enc, const uint8_t (&exp)[N])
{
    ASSERT_EQ(enc.size(), N);
    for (size_t i = 0; i < N; ++i)
        ASSERT_EQ(enc[i], exp[i]) << "differ at index " << i;
}

TEST(Encode, Uint)
{
    const uint8_t exp[] = {
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
    Encoder<sizeof(exp)> enc;

    enc.encode(0);
    enc.encode(1);
    enc.encode(10);
    enc.encode(23);
    enc.encode(24);
    enc.encode(100);
    enc.encode(1000);
    enc.encode(1000000);
    enc.encode(1000000000000);
    enc.encode(18446744073709551615ul);

    encode_check(enc, exp);
}

TEST(Encode, Sint)
{
    const uint8_t exp[] = {
        0x20,
        0x29,
        0x38, 0x63,
        0x39, 0x03, 0xe7
    };
    Encoder<sizeof(exp)> enc;

    enc.encode(-1);
    enc.encode(-10);
    enc.encode(-100);
    enc.encode(-1000);

    encode_check(enc, exp);
}

TEST(Encode, Data)
{
    const uint8_t exp[] = {
        0x40,
        0x44, 0x01, 0x02, 0x03, 0x04,
    };
    Encoder<sizeof(exp)> enc;

    enc.encode((uint8_t*) nullptr, 0);
    enc.encode((uint8_t*) "\x01\x02\x03\x04", 4);

    encode_check(enc, exp);
}

TEST(Encode, Text)
{
    const uint8_t exp[] = {
        0x60,
        0x61, 0x61,
        0x64, 0x49, 0x45, 0x54, 0x46,
        0x62, 0x22, 0x5c,
        0x62, 0xc3, 0xbc,
        0x63, 0xe6, 0xb0, 0xb4,
        0x64, 0xf0, 0x90, 0x85, 0x91,
    };
    Encoder<sizeof(exp)> enc;

    const unsigned char test[] = { 0xf0, 0x90, 0x85, 0x91 };

    enc.encode((char*) nullptr, 0);
    enc.encode("a", strlen("a"));
    enc.encode("IETF", strlen("IETF"));
    enc.encode("\"\\", strlen("\"\\"));
    enc.encode("\u00fc", strlen("\u00fc"));
    enc.encode("\u6c34", strlen("\u6c34"));
    enc.encode((char*) test, sizeof(test));   

    encode_check(enc, exp);
}

TEST(Encode, Array)
{
    const uint8_t exp[] = {
        0x80,
        0x83, 0x01, 0x02, 0x03,
        0x83, 0x01, 0x82, 0x02, 0x03, 0x82, 0x04, 0x05,
        0x98, 0x19, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x18, 0x18, 0x19,
        0x82, 0x61, 0x61, 0xa1, 0x61, 0x62, 0x61, 0x63,
    };
    Encoder<sizeof(exp)> enc;
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

    enc.encode(arr_0);
    enc.encode(arr_1);
    enc.encode(arr_2);
    enc.encode(arr_3);
    enc.encode(arr_4);

    encode_check(enc, exp);
}

TEST(Encode, Map)
{
    const uint8_t exp[] = {
        0xa0,
        0xa2, 0x01, 0x02, 0x03, 0x04,
        0xa2, 0x61, 0x61, 0x01, 0x61, 0x62, 0x82, 0x02, 0x03,
        0xa5, 0x61, 0x61, 0x61, 0x41, 0x61, 0x62, 0x61, 0x42, 0x61, 0x63, 0x61, 0x43, 0x61, 0x64, 0x61, 0x44, 0x61, 0x65, 0x61, 0x45,
    };
    Encoder<sizeof(exp)> enc;
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

    enc.encode(map_0);
    enc.encode(map_1);
    enc.encode(map_2);
    enc.encode(map_3);

    encode_check(enc, exp);
}

TEST(Encode, Tag)
{
    const uint8_t exp[] = {
        0xc0, 0x74, 0x32, 0x30, 0x31, 0x33, 0x2d, 0x30, 0x33, 0x2d, 0x32, 0x31, 0x54, 0x32, 0x30, 0x3a, 0x30, 0x34, 0x3a, 0x30, 0x30, 0x5a,
        0xc1, 0x1a, 0x51, 0x4b, 0x67, 0xb0,
        0xc1, 0xfb, 0x41, 0xd4, 0x52, 0xd9, 0xec, 0x20, 0x00, 0x00,
        0xd7, 0x44, 0x01, 0x02, 0x03, 0x04,
        0xd8, 0x18, 0x45, 0x64, 0x49, 0x45, 0x54, 0x46,
        0xd8, 0x20, 0x76, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x77, 0x77, 0x77, 0x2e, 0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x2e, 0x63, 0x6f, 0x6d,
    };
    Encoder<sizeof(exp)> enc;
    Pool<8> pool;
    
    enc.encode({0, pool.make("2013-03-21T20:04:00Z", strlen("2013-03-21T20:04:00Z"))});
    enc.encode({1, pool.make(1363896240)});
    enc.encode({1, pool.make(1363896240.5)});
    enc.encode({23, pool.make((uint8_t*) "\x01\x02\x03\x04", 4)});
    enc.encode({24, pool.make((uint8_t*) "\x64\x49\x45\x54\x46", 5)});
    enc.encode({32, pool.make("http://www.example.com", strlen("http://www.example.com"))});

    encode_check(enc, exp);
}

TEST(Encode, Simple)
{
    const uint8_t exp[] = {
        0xf4,
        0xf5,
        0xf4,
        0xf5,
        0xf6,
        0xf7,
        0xf0,
        0xf8, 0xff,
    };
    Encoder<sizeof(exp)> enc;

    enc.encode(false);
    enc.encode(true);
    enc.encode(PRIM_FALSE);
    enc.encode(PRIM_TRUE);
    enc.encode(PRIM_NULL);
    enc.encode(PRIM_UNDEFINED);
    enc.encode(Prim(16));
    enc.encode(Prim(255));

    encode_check(enc, exp);
}

TEST(Encode, Float)
{
    const uint8_t exp[] = {
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
    Encoder<sizeof(exp)> enc;

    enc.encode(0.0);
    enc.encode(-0.0);
    enc.encode(1.0);
    enc.encode(1.1);
    enc.encode(1.5);
    enc.encode(65504.0);
    enc.encode(100000.0);
    enc.encode(3.4028234663852886e+38);
    enc.encode(1.0e+300);
    enc.encode(5.960464477539063e-8);
    enc.encode(0.00006103515625);
    enc.encode(-4.0);
    enc.encode(-4.1);
    enc.encode(INFINITY);
    enc.encode(NAN);
    enc.encode(-INFINITY);

    encode_check(enc, exp);
}

TEST(Encode, Indef)
{
    const uint8_t exp[] = {
        0x5f, 0x42, 0x01, 0x02, 0x43, 0x03, 0x04, 0x05, 0xff,
        0x7f, 0x65, 0x73, 0x74, 0x72, 0x65, 0x61, 0x64, 0x6d, 0x69, 0x6e, 0x67, 0xff, 
        0x9f, 0xff,
        // 0x9f, 0x01, 0x82, 0x02, 0x03, 0x9f, 0x04, 0x05, 0xff, 0xff,
    };
    Encoder<sizeof(exp)> enc;

    enc.encode_start_indef(MT_DATA);
    enc.encode((uint8_t*) "\x01\x02", 2);
    enc.encode((uint8_t*) "\x03\x04\x05", 3);
    enc.encode_break();

    enc.encode_start_indef(MT_TEXT);
    enc.encode("strea", strlen("strea"));
    enc.encode("ming", strlen("ming"));
    enc.encode_break();

    enc.encode_start_indef(MT_ARRAY);
    enc.encode_break();

    // enc.encode_start_indef(MT_ARRAY);
    // enc.encode_break();

    encode_check(enc, exp);
}