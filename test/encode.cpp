#include <gtest/gtest.h>
#include "zbor/encode.h"
#include "zbor/log.h"

static void check(zbor::ref res, std::initializer_list<uint8_t> exp)
{
    ASSERT_EQ(res.size(), exp.size());
    for (size_t i = 0; auto it : exp)
        ASSERT_EQ(res[i++], it) << "at index " << i;
}

class Codec : public ::testing::Test {
protected:
    void SetUp() override 
    {

    }
    void TearDown() override 
    {
        codec.clear();
    }
    zbor::codec<70> codec;
};

TEST_F(Codec, EncodeUnsigned)
{
    codec.encode(0);
    codec.encode(1);
    codec.encode(10);
    codec.encode(23);
    codec.encode(24);
    codec.encode(25);
    codec.encode(100);
    codec.encode(1000);
    codec.encode(1000000);
    codec.encode(1000000000000);
    codec.encode(18446744073709551615ul);

    check(codec, {
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
    });
}

TEST_F(Codec, EncodeSigned)
{
    codec.encode(-1);
    codec.encode(-10);
    codec.encode(-100);
    codec.encode(-1000);
    codec.encode(int64_t(-9223372036854775807 - 1));

    check(codec, {
        0x20, // -1
        0x29, // -10
        0x38, 0x63, // -100
        0x39, 0x03, 0xe7, // -1000
        0x3b, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // -9223372036854775808
    });
}

TEST_F(Codec, EncodeFloat)
{
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

    check(codec, {
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
    });
}

TEST_F(Codec, EncodeSimple)
{
    codec.encode(zbor::prim_false);
    codec.encode(zbor::prim_true);
    codec.encode(zbor::prim_null);
    codec.encode(zbor::prim_undefined);
    codec.encode(zbor::prim_t(16));
    codec.encode(zbor::prim_t(255));

    check(codec, {
        0xf4, // false
        0xf5, // true
        0xf6, // null
        0xf7, // undefined
        0xf0, // simple(16)
        0xf8, 0xff, // simple(255)
    });
}

TEST_F(Codec, EncodeIllegalSimple)
{
    ASSERT_EQ(zbor::err_invalid_simple, codec.encode(zbor::prim_t(24)));
    ASSERT_EQ(zbor::err_invalid_simple, codec.encode(zbor::prim_t(25)));
    ASSERT_EQ(zbor::err_invalid_simple, codec.encode(zbor::prim_t(26)));
    ASSERT_EQ(zbor::err_invalid_simple, codec.encode(zbor::prim_t(27)));
    ASSERT_EQ(zbor::err_invalid_simple, codec.encode(zbor::prim_t(28)));
    ASSERT_EQ(zbor::err_invalid_simple, codec.encode(zbor::prim_t(29)));
    ASSERT_EQ(zbor::err_invalid_simple, codec.encode(zbor::prim_t(30)));
    ASSERT_EQ(zbor::err_invalid_simple, codec.encode(zbor::prim_t(31)));

    check(codec, {});
}

TEST_F(Codec, EncodeBool)
{
    codec.encode(false);
    codec.encode(true);

    check(codec, {
        0xf4, // false
        0xf5, // true
    });
}

TEST_F(Codec, EncodeData)
{
    // codec.encode(std::span<uint8_t>{});
    // codec.encode(std::span<uint8_t>{});

    check(codec, {
        0x40, // h''
        0x44, 0x01, 0x02, 0x03, 0x04, // h'01020304'
    });
}