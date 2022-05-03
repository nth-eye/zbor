#include <gtest/gtest.h>
#include "zbor/decode.h"
#include <cmath>

using namespace zbor;

void decode_compare(const Obj &exp, const Obj &res)
{
    ASSERT_EQ(exp.type, res.type);
    switch (exp.type) {
        case TYPE_UINT:
            ASSERT_EQ(exp.uint, res.uint);
        break;
        case TYPE_SINT:
            ASSERT_EQ(exp.sint, res.sint);
        break;
        case TYPE_DATA:
            ASSERT_EQ(exp.str.len, res.str.len);
            for (size_t j = 0; j < res.str.len; ++j)
                ASSERT_EQ(exp.str.dat[j], res.str.dat[j]) << "at index " << j;
        break;
        case TYPE_TEXT:
            ASSERT_EQ(exp.str.len, res.str.len);
            for (size_t j = 0; j < res.str.len; ++j)
                ASSERT_EQ(exp.str.txt[j], res.str.txt[j]) << "at index " << j;
        break;
        case TYPE_ARRAY:
        {
            ASSERT_EQ(exp.arr.size(), res.arr.size());
            auto exp_iter = exp.arr.begin();
            for (auto it : res.arr) {
                decode_compare(**exp_iter, *it);
                ++exp_iter;
            }
        }
        break;
        case TYPE_MAP:
        {
            ASSERT_EQ(exp.map.size(), res.map.size());
            auto exp_iter = exp.map.begin();
            for (auto it : res.map) {
                decode_compare(*(*exp_iter).key, *it.key);
                decode_compare(*(*exp_iter).val, *it.val);
                ++exp_iter;
            }
        }
        break;
        case TYPE_TAG:
            ASSERT_EQ(exp.tag.val, res.tag.val);
            ASSERT_NE(exp.tag.content, nullptr);
            ASSERT_NE(res.tag.content, nullptr);
            decode_compare(*exp.tag.content, *res.tag.content);
        break;
        case TYPE_PRIM:
            ASSERT_EQ(exp.prim, res.prim);
        break;
        case TYPE_DOUBLE:
            if ((std::isnan(exp.dbl) == false) || 
                (std::isnan(res.dbl) == false))
            {
                ASSERT_FLOAT_EQ(exp.dbl, res.dbl);
            }
        break;
        case TYPE_DATA_CHUNKS:
        {
            ASSERT_EQ(exp.arr.size(), res.arr.size());
            auto exp_iter = exp.arr.begin();
            for (auto it : res.arr) {
                ASSERT_EQ(it->type, TYPE_DATA);
                ASSERT_EQ((*exp_iter)->type, TYPE_DATA);
                decode_compare(**exp_iter, *it);
                ++exp_iter;
            }
        }
        break;
        case TYPE_TEXT_CHUNKS:
        {
            ASSERT_EQ(exp.arr.size(), res.arr.size());
            auto exp_iter = exp.arr.begin();
            for (auto it : res.arr) {
                ASSERT_EQ(it->type, TYPE_TEXT);
                ASSERT_EQ((*exp_iter)->type, TYPE_TEXT);
                decode_compare(**exp_iter, *it);
                ++exp_iter;
            }
        }
        break;
        default:
            ASSERT_TRUE(false);
    }
}

template<size_t N, size_t M, size_t P = M>
void decode_check(const uint8_t (&enc)[N], const Obj (&exp)[M])
{
    Pool<P> pool; 

    auto ret = decode(pool, enc, sizeof(enc));

    ASSERT_EQ(ret.err, ERR_OK);
    ASSERT_EQ(ret.size, M);
    ASSERT_NE(ret.root, nullptr);

    int i = 0;

    for (auto it : ret) {
        decode_compare(exp[i], *it);
        ++i;
    }
}

TEST(Decode, Nullptr)
{
    Pool<4> pool;
    Sequence seq;
    uint8_t dummy;

    seq = decode(pool, nullptr, 20);

    EXPECT_EQ(seq.root, nullptr);
    EXPECT_EQ(seq.size, 0);
    EXPECT_EQ(seq.err, ERR_INVALID_PARAM);

    seq = decode(pool, &dummy, 0);

    EXPECT_EQ(seq.root, nullptr);
    EXPECT_EQ(seq.size, 0);
    EXPECT_EQ(seq.err, ERR_INVALID_PARAM);
}

TEST(Decode, Uint)
{
    const uint8_t enc[] = {
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
    const Obj exp[] = {
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
    decode_check(enc, exp);
}

TEST(Decode, Sint)
{
    const Obj exp[] = {
        -1,
        -10,
        -100,
        -1000,
    };
    const uint8_t enc[] = {
        0x20,
        0x29,
        0x38, 0x63,
        0x39, 0x03, 0xe7,
    };
    decode_check(enc, exp);
}

TEST(Decode, Data)
{
    const Obj exp[] = {
        {(uint8_t*) nullptr, 0},
        {(uint8_t*) "\x01\x02\x03\x04", 4},
    };
    const uint8_t enc[] = {
        0x40,
        0x44, 0x01, 0x02, 0x03, 0x04,
    };
    decode_check(enc, exp);
}

TEST(Decode, Text)
{
    const unsigned char test[] = { 0xf0, 0x90, 0x85, 0x91 };
    const Obj exp[] = {
        {(char*) nullptr, 0},
        {"a", strlen("a")},
        {"IETF", strlen("IETF")},
        {"\"\\", strlen("\"\\")},
        {"\u00fc", strlen("\u00fc")},
        {"\u6c34", strlen("\u6c34")},
        {(char*) test, sizeof(test)},  
    };
    const uint8_t enc[] = {
        0x60,
        0x61, 0x61,
        0x64, 0x49, 0x45, 0x54, 0x46,
        0x62, 0x22, 0x5c,
        0x62, 0xc3, 0xbc,
        0x63, 0xe6, 0xb0, 0xb4,
        0x64, 0xf0, 0x90, 0x85, 0x91,
    };
    decode_check(enc, exp);
}

TEST(Decode, Array)
{
    Pool<10> pool;

    Array arr_0, arr_1, arr_2;

    arr_1.push(pool.make(1));
    arr_1.push(pool.make(2));
    arr_1.push(pool.make(3));

    arr_2.push(pool.make(1));

    Array arr_2_1;

    arr_2_1.push(pool.make(2));
    arr_2_1.push(pool.make(3));

    Array arr_2_2;

    arr_2_2.push(pool.make(4));
    arr_2_2.push(pool.make(5));

    arr_2.push(pool.make(arr_2_1));
    arr_2.push(pool.make(arr_2_2));

    const Obj exp[] = {
        arr_0,
        arr_1,
        arr_2,
    };
    const uint8_t enc[] = {
        0x80,
        0x83, 0x01, 0x02, 0x03,
        0x83, 0x01, 0x82, 0x02, 0x03, 0x82, 0x04, 0x05,
    };
    decode_check<sizeof(enc), 3, 13>(enc, exp);
}

TEST(Decode, Map)
{
    Pool<20> pool;

    Map map_0, map_1, map_2, map_3;

    map_1.push(pool.make(1), pool.make(2));
    map_1.push(pool.make(3), pool.make(4));

    Array arr;

    arr.push(pool.make(2));
    arr.push(pool.make(3));

    map_2.push(pool.make("a", 1), pool.make(1));
    map_2.push(pool.make("b", 1), pool.make(arr));

    map_3.push(pool.make("a", 1), pool.make("A", 1));
    map_3.push(pool.make("b", 1), pool.make("B", 1));
    map_3.push(pool.make("c", 1), pool.make("C", 1));
    map_3.push(pool.make("d", 1), pool.make("D", 1));
    map_3.push(pool.make("e", 1), pool.make("E", 1));

    const Obj exp[] = {
        map_0,
        map_1,
        map_2,
        map_3,
    };
    const uint8_t enc[] = {
        0xa0,
        0xa2, 0x01, 0x02, 0x03, 0x04,
        0xa2, 0x61, 0x61, 0x01, 0x61, 0x62, 0x82, 0x02, 0x03,
        0xa5, 0x61, 0x61, 0x61, 0x41, 0x61, 0x62, 0x61, 0x42, 0x61, 0x63, 0x61, 0x43, 0x61, 0x64, 0x61, 0x44, 0x61, 0x65, 0x61, 0x45,
    };
    decode_check<sizeof(enc), 4, 24>(enc, exp);
}

TEST(Decode, Tag)
{
    Pool<6> pool;

    const Obj exp[] = {
        Tag{0, pool.make("2013-03-21T20:04:00Z", strlen("2013-03-21T20:04:00Z"))},
        Tag{1, pool.make(1363896240)},
        Tag{1, pool.make(1363896240.5)},
        Tag{23, pool.make((uint8_t*) "\x01\x02\x03\x04", 4)},
        Tag{24, pool.make((uint8_t*) "\x64\x49\x45\x54\x46", 5)},
        Tag{32, pool.make("http://www.example.com", strlen("http://www.example.com"))},
    };
    const uint8_t enc[] = {
        0xc0, 0x74, 0x32, 0x30, 0x31, 0x33, 0x2d, 0x30, 0x33, 0x2d, 0x32, 0x31, 0x54, 0x32, 0x30, 0x3a, 0x30, 0x34, 0x3a, 0x30, 0x30, 0x5a,
        0xc1, 0x1a, 0x51, 0x4b, 0x67, 0xb0,
        0xc1, 0xfb, 0x41, 0xd4, 0x52, 0xd9, 0xec, 0x20, 0x00, 0x00,
        0xd7, 0x44, 0x01, 0x02, 0x03, 0x04,
        0xd8, 0x18, 0x45, 0x64, 0x49, 0x45, 0x54, 0x46,
        0xd8, 0x20, 0x76, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x77, 0x77, 0x77, 0x2e, 0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x2e, 0x63, 0x6f, 0x6d,
    };
    decode_check<sizeof(enc), 6, 12>(enc, exp);
}

TEST(Decode, Simple)
{
    const Obj exp[] = {
        false,
        true,
        PRIM_FALSE,
        PRIM_TRUE,
        PRIM_NULL,
        PRIM_UNDEFINED,
        Prim(16),
        Prim(255),
    };
    const uint8_t enc[] = {
        0xf4,
        0xf5,
        0xf4,
        0xf5,
        0xf6,
        0xf7,
        0xf0,
        0xf8, 0xff,
    };
    decode_check(enc, exp);
}

TEST(Decode, Float)
{
    const Obj exp[] = {
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
    const uint8_t enc[] = {
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
    decode_check(enc, exp);
}

TEST(Decode, IndefArray)
{
    Pool<7> pool;

    Array arr_0, arr_1;

    arr_1.push(pool.make(1));
    Array arr_1_1;
    arr_1_1.push(pool.make(2));
    arr_1_1.push(pool.make(3));
    arr_1.push(pool.make(arr_1_1));
    Array arr_1_2;
    arr_1_2.push(pool.make(4));
    arr_1_2.push(pool.make(5));
    arr_1.push(pool.make(arr_1_2));

    const Obj exp[] = {
        arr_0,
        arr_1,
    };
    const uint8_t enc[] = {
        0x9f, 0xff,
        0x9f, 0x01, 0x82, 0x02, 0x03, 0x9f, 0x04, 0x05, 0xff, 0xff,
    };
    decode_check<sizeof(enc), 2, 16>(enc, exp);
}

TEST(Decode, IndefMap)
{
    Pool<4> pool;

    Map map;

    map.push(pool.make("Fun", 3), pool.make(true));
    map.push(pool.make("Amt", 3), pool.make(-2));

    const Obj exp[] = {
        map,
    };
    const uint8_t enc[] = {
        0xbf, 0x63, 0x46, 0x75, 0x6e, 0xf5, 0x63, 0x41, 0x6d, 0x74, 0x21, 0xff,
    };
    decode_check<sizeof(enc), 1, 5>(enc, exp);
}

TEST(Decode, IndefString)
{
    Pool<4> pool;

    ChunkData chunks_0;
    ChunkText chunks_1;

    chunks_0.push(pool.make((uint8_t*) "\x01\x02", 2));
    chunks_0.push(pool.make((uint8_t*) "\x03\x04\x05", 3));

    chunks_1.push(pool.make("strea", strlen("strea")));
    chunks_1.push(pool.make("ming", strlen("ming")));

    const uint8_t enc[] = {
        0x5f, 0x42, 0x01, 0x02, 0x43, 0x03, 0x04, 0x05, 0xff,
        0x7f, 0x65, 0x73, 0x74, 0x72, 0x65, 0x61, 0x64, 0x6d, 0x69, 0x6e, 0x67, 0xff, 
    };
    const Obj exp[] = {
        chunks_0,
        chunks_1,
    };
    decode_check<sizeof(enc), 2, 6>(enc, exp);
}
