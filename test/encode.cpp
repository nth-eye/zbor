#include <gtest/gtest.h>
#include "zbor/encode.h"
#include "zbor/log.h"

static void check(zbor::ref res, std::initializer_list<uint8_t> exp)
{
    ASSERT_EQ(res.size(), exp.size());
    for (size_t i = 0; auto it : exp)
        ASSERT_EQ(res[i++], it) << "at index " << i;
}

class Encode : public ::testing::Test {
protected:
    void SetUp() override 
    {

    }
    void TearDown() override 
    {
        codec.clear();
    }
    zbor::codec<77> codec;
};

TEST_F(Encode, ExplicitUnsigned)
{
    codec.encode_uint(0);
    codec.encode_uint(1);
    codec.encode_uint(10);
    codec.encode_uint(23);
    codec.encode_uint(24);
    codec.encode_uint(25);
    codec.encode_uint(100);
    codec.encode_uint(1000);
    codec.encode_uint(1000000);
    codec.encode_uint(1000000000000);
    codec.encode_uint(18446744073709551615ul);

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

TEST_F(Encode, ImplicitUnsigned)
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

TEST_F(Encode, ExplicitSigned)
{
    codec.encode_sint(-1);
    codec.encode_sint(-10);
    codec.encode_sint(-100);
    codec.encode_sint(-1000);
    codec.encode_sint(int64_t(-9223372036854775807 - 1));

    check(codec, {
        0x20, // -1
        0x29, // -10
        0x38, 0x63, // -100
        0x39, 0x03, 0xe7, // -1000
        0x3b, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // -9223372036854775808
    });
}

TEST_F(Encode, ImplicitSigned)
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

TEST_F(Encode, ExplicitFloating)
{
    codec.encode_float(0.0);
    codec.encode_float(-0.0);
    codec.encode_float(1.0);
    codec.encode_double(1.1);
    codec.encode_float(1.5);
    codec.encode_float(65504.0);
    codec.encode_float(100000.0);
    codec.encode_float(3.4028234663852886e+38);
    codec.encode_double(1.0e+300);
    codec.encode_float(5.960464477539063e-8);
    codec.encode_float(0.00006103515625);
    codec.encode_float(-4.0);
    codec.encode_double(-4.1);
    codec.encode_float(INFINITY);
    codec.encode_float(NAN);
    codec.encode_float(-INFINITY);

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

TEST_F(Encode, ImplicitFloating)
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

TEST_F(Encode, ExplicitSimple)
{
    codec.encode_prim(zbor::prim_false);
    codec.encode_prim(zbor::prim_true);
    codec.encode_prim(zbor::prim_null);
    codec.encode_prim(zbor::prim_undefined);
    codec.encode_prim(zbor::prim(16));
    codec.encode_prim(zbor::prim(255));
    codec.encode_bool(false);
    codec.encode_bool(true);

    check(codec, {
        0xf4, // false
        0xf5, // true
        0xf6, // null
        0xf7, // undefined
        0xf0, // simple(16)
        0xf8, 0xff, // simple(255)
        0xf4, // false
        0xf5, // true
    });
}

TEST_F(Encode, ImplicitSimple)
{
    codec.encode(zbor::prim_false);
    codec.encode(zbor::prim_true);
    codec.encode(zbor::prim_null);
    codec.encode(zbor::prim_undefined);
    codec.encode(zbor::prim(16));
    codec.encode(zbor::prim(255));
    codec.encode(false);
    codec.encode(true);

    check(codec, {
        0xf4, // false
        0xf5, // true
        0xf6, // null
        0xf7, // undefined
        0xf0, // simple(16)
        0xf8, 0xff, // simple(255)
        0xf4, // false
        0xf5, // true
    });
}

TEST_F(Encode, ExplicitData)
{
    const uint8_t pld[] = {0xb1, 0x6b, 0x00, 0xb5};

    codec.encode_data({});
    codec.encode_data({0x01, 0x02, 0x03, 0x04});
    codec.encode_data(zbor::span{});
    codec.encode_data(zbor::span{pld});
    codec.encode_data(std::span<uint8_t>{});
    codec.encode_data(std::span<const uint8_t>{pld});
    codec.encode_data(pld);

    check(codec, {
        0x40, // h''
        0x44, 0x01, 0x02, 0x03, 0x04, // h'01020304'
        0x40, // h''
        0x44, 0xb1, 0x6b, 0x00, 0xb5, // h'b16b00b5'
        0x40, // h''
        0x44, 0xb1, 0x6b, 0x00, 0xb5, // h'b16b00b5'
        0x44, 0xb1, 0x6b, 0x00, 0xb5, // h'b16b00b5'
    });
}

TEST_F(Encode, ImplicitData)
{
    const uint8_t pld[] = {0xb1, 0x6b, 0x00, 0xb5};

    codec.encode({});
    codec.encode({0x01, 0x02, 0x03, 0x04});
    codec.encode(zbor::span{});
    codec.encode(zbor::span{pld});
    codec.encode(std::span<uint8_t>{});
    codec.encode(std::span<const uint8_t>{pld});
    codec.encode(pld);

    check(codec, {
        0x40, // h''
        0x44, 0x01, 0x02, 0x03, 0x04, // h'01020304'
        0x40, // h''
        0x44, 0xb1, 0x6b, 0x00, 0xb5, // h'b16b00b5'
        0x40, // h''
        0x44, 0xb1, 0x6b, 0x00, 0xb5, // h'b16b00b5'
        0x44, 0xb1, 0x6b, 0x00, 0xb5, // h'b16b00b5'
    });
}

TEST_F(Encode, ExplicitText)
{
    const uint8_t pld[] = {0xf0, 0x90, 0x85, 0x91};

    codec.encode_text("");
    codec.encode_text("a");
    codec.encode_text("IETF");
    codec.encode_text(std::string_view{"\"\\"});
    codec.encode_text(std::string_view{"\u00fc"});
    codec.encode_text(std::string_view{"\u6c34"});
    codec.encode_text({});
    codec.encode_text({0x77, 0x77, 0x77});
    codec.encode_text(pld);

    check(codec, {
        0x60, // ""
        0x61, 0x61, // "a"
        0x64, 0x49, 0x45, 0x54, 0x46, // "IETF"
        0x62, 0x22, 0x5c, // "\"\\"
        0x62, 0xc3, 0xbc, // "\u00fc"
        0x63, 0xe6, 0xb0, 0xb4, // "\u6c34"
        0x60, // ""
        0x63, 0x77, 0x77, 0x77, // "\x77\x77\x77"
        0x64, 0xf0, 0x90, 0x85, 0x91, // "\ud800\udd51"
    });
}

TEST_F(Encode, ImplicitText)
{
    // const uint8_t pld[] = {0xf0, 0x90, 0x85, 0x91}; // NOTE: bytes can be encoded as text only explicitly

    codec.encode("");
    codec.encode("a");
    codec.encode("IETF");
    codec.encode(std::string_view{"\"\\"});
    codec.encode(std::string_view{"\u00fc"});
    codec.encode(std::string_view{"\u6c34"});

    check(codec, {
        0x60, // ""
        0x61, 0x61, // "a"
        0x64, 0x49, 0x45, 0x54, 0x46, // "IETF"
        0x62, 0x22, 0x5c, // "\"\\"
        0x62, 0xc3, 0xbc, // "\u00fc"
        0x63, 0xe6, 0xb0, 0xb4, // "\u6c34"
    });
}

TEST_F(Encode, ExplicitTag)
{
    codec.encode_tag(0);
    codec.encode("2013-03-21T20:04:00Z");
    codec.encode_tag(1);
    codec.encode(1363896240);
    codec.encode_tag(1);
    codec.encode(1363896240.5);
    codec.encode_tag(23);
    codec.encode({0x01, 0x02, 0x03, 0x04});
    codec.encode_tag(24);
    codec.encode({0x64, 0x49, 0x45, 0x54, 0x46});
    codec.encode_tag(32);
    codec.encode("http://www.example.com");

    check(codec, {
        0xc0, 0x74, 0x32, 0x30, 0x31, 0x33, 0x2d, 0x30, 0x33, 0x2d, 0x32, 0x31, 0x54, 0x32, 0x30, 0x3a, 0x30, 0x34, 0x3a, 0x30, 0x30, 0x5a, // 0("2013-03-21T20:04:00Z")
        0xc1, 0x1a, 0x51, 0x4b, 0x67, 0xb0, // 1(1363896240)
        0xc1, 0xfb, 0x41, 0xd4, 0x52, 0xd9, 0xec, 0x20, 0x00, 0x00, // 1(1363896240.5)
        0xd7, 0x44, 0x01, 0x02, 0x03, 0x04, // 23(h'01020304')
        0xd8, 0x18, 0x45, 0x64, 0x49, 0x45, 0x54, 0x46, // 24(h'6449455446')
        0xd8, 0x20, 0x76, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x77, 0x77, 0x77, 0x2e, 0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x2e, 0x63, 0x6f, 0x6d, // 32("http://www.example.com")
    });
}

TEST_F(Encode, ImplicitTag)
{
    using namespace zbor::literals;

    codec.encode(0_tag);
    codec.encode("2013-03-21T20:04:00Z");
    codec.encode(1_tag);
    codec.encode(1363896240);
    codec.encode(1_tag);
    codec.encode(1363896240.5);
    codec.encode(23_tag);
    codec.encode({0x01, 0x02, 0x03, 0x04});
    codec.encode(24_tag);
    codec.encode({0x64, 0x49, 0x45, 0x54, 0x46});
    codec.encode(32_tag);
    codec.encode("http://www.example.com");

    check(codec, {
        0xc0, 0x74, 0x32, 0x30, 0x31, 0x33, 0x2d, 0x30, 0x33, 0x2d, 0x32, 0x31, 0x54, 0x32, 0x30, 0x3a, 0x30, 0x34, 0x3a, 0x30, 0x30, 0x5a, // 0("2013-03-21T20:04:00Z")
        0xc1, 0x1a, 0x51, 0x4b, 0x67, 0xb0, // 1(1363896240)
        0xc1, 0xfb, 0x41, 0xd4, 0x52, 0xd9, 0xec, 0x20, 0x00, 0x00, // 1(1363896240.5)
        0xd7, 0x44, 0x01, 0x02, 0x03, 0x04, // 23(h'01020304')
        0xd8, 0x18, 0x45, 0x64, 0x49, 0x45, 0x54, 0x46, // 24(h'6449455446')
        0xd8, 0x20, 0x76, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x77, 0x77, 0x77, 0x2e, 0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x2e, 0x63, 0x6f, 0x6d, // 32("http://www.example.com")
    });
}

TEST_F(Encode, ExplicitArray)
{
    codec.encode_arr(0);
    codec.encode_arr(3);
    codec.encode(1);
    codec.encode(2);
    codec.encode(3);
    codec.encode_arr(3);
    codec.encode(1);
    codec.encode_arr(2);
    codec.encode(2);
    codec.encode(3);
    codec.encode_arr(2);
    codec.encode(4);
    codec.encode(5);
    codec.encode_arr(25);
    for (int i = 1; i <= 25; ++i)
        codec.encode(i);

    check(codec, {
        0x80, // []
        0x83, 0x01, 0x02, 0x03, // [1, 2, 3]
        0x83, 0x01, 0x82, 0x02, 0x03, 0x82, 0x04, 0x05, // [1, [2, 3], [4, 5]]
        0x98, 0x19, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x18, 0x18, 0x19, // [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]
    });
}

TEST_F(Encode, ImplicitArray)
{
    using namespace zbor::literals;

    codec.encode(0_arr);
    codec.encode(3_arr);
    codec.encode(1);
    codec.encode(2);
    codec.encode(3);
    codec.encode(3_arr);
    codec.encode(1);
    codec.encode(2_arr);
    codec.encode(2);
    codec.encode(3);
    codec.encode(2_arr);
    codec.encode(4);
    codec.encode(5);
    codec.encode(25_arr);
    for (int i = 1; i <= 25; ++i)
        codec.encode(i);

    check(codec, {
        0x80, // []
        0x83, 0x01, 0x02, 0x03, // [1, 2, 3]
        0x83, 0x01, 0x82, 0x02, 0x03, 0x82, 0x04, 0x05, // [1, [2, 3], [4, 5]]
        0x98, 0x19, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x18, 0x18, 0x19, // [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]
    });
}

TEST_F(Encode, ExplicitMap)
{
    codec.encode_map(0);
    codec.encode_map(2);
    codec.encode(1);
    codec.encode(2);
    codec.encode(3);
    codec.encode(4);
    codec.encode_map(2);
    codec.encode("a");
    codec.encode(1);
    codec.encode("b");
    codec.encode_arr(2);
    codec.encode(2);
    codec.encode(3);
    codec.encode_map(5);
    codec.encode("a");
    codec.encode("A");
    codec.encode("b");
    codec.encode("B");
    codec.encode("c");
    codec.encode("C");
    codec.encode("d");
    codec.encode("D");
    codec.encode("e");
    codec.encode("E");

    check(codec, {
        0xa0, // {}
        0xa2, 0x01, 0x02, 0x03, 0x04, // {1: 2, 3: 4}
        0xa2, 0x61, 0x61, 0x01, 0x61, 0x62, 0x82, 0x02, 0x03, // {"a": 1, "b": [2, 3]}
        0xa5, 0x61, 0x61, 0x61, 0x41, 0x61, 0x62, 0x61, 0x42, 0x61, 0x63, 0x61, 0x43, 0x61, 0x64, 0x61, 0x44, 0x61, 0x65, 0x61, 0x45, // {"a": "A", "b": "B", "c": "C", "d": "D", "e": "E"}
    });
}

TEST_F(Encode, ImplicitMap)
{
    using namespace zbor::literals;

    codec.encode(0_map);
    codec.encode(2_map);
    codec.encode(1);
    codec.encode(2);
    codec.encode(3);
    codec.encode(4);
    codec.encode(2_map);
    codec.encode("a");
    codec.encode(1);
    codec.encode("b");
    codec.encode(2_arr);
    codec.encode(2);
    codec.encode(3);
    codec.encode(5_map);
    codec.encode("a");
    codec.encode("A");
    codec.encode("b");
    codec.encode("B");
    codec.encode("c");
    codec.encode("C");
    codec.encode("d");
    codec.encode("D");
    codec.encode("e");
    codec.encode("E");

    check(codec, {
        0xa0, // {}
        0xa2, 0x01, 0x02, 0x03, 0x04, // {1: 2, 3: 4}
        0xa2, 0x61, 0x61, 0x01, 0x61, 0x62, 0x82, 0x02, 0x03, // {"a": 1, "b": [2, 3]}
        0xa5, 0x61, 0x61, 0x61, 0x41, 0x61, 0x62, 0x61, 0x42, 0x61, 0x63, 0x61, 0x43, 0x61, 0x64, 0x61, 0x44, 0x61, 0x65, 0x61, 0x45, // {"a": "A", "b": "B", "c": "C", "d": "D", "e": "E"}
    });
}

TEST_F(Encode, ExplicitIndefData)
{
    codec.encode_indef_dat();
    codec.encode({0x01, 0x02});
    codec.encode({0x03, 0x04, 0x05});
    codec.encode_break();

    check(codec, {
        0x5f, 0x42, 0x01, 0x02, 0x43, 0x03, 0x04, 0x05, 0xff, // (_ h'0102', h'030405')
    });
}

TEST_F(Encode, ImplicitIndefData)
{
    codec.encode(zbor::indef_dat);
    codec.encode({0x01, 0x02});
    codec.encode({0x03, 0x04, 0x05});
    codec.encode(zbor::breaker);

    check(codec, {
        0x5f, 0x42, 0x01, 0x02, 0x43, 0x03, 0x04, 0x05, 0xff, // (_ h'0102', h'030405')
    });
}

TEST_F(Encode, ExplicitIndefText)
{
    codec.encode_indef_txt();
    codec.encode("strea");
    codec.encode("ming");
    codec.encode_break();

    check(codec, {
        0x7f, 0x65, 0x73, 0x74, 0x72, 0x65, 0x61, 0x64, 0x6d, 0x69, 0x6e, 0x67, 0xff, // (_ "strea", "ming")
    });
}

TEST_F(Encode, ImplicitIndefText)
{
    codec.encode(zbor::indef_txt);
    codec.encode("strea");
    codec.encode("ming");
    codec.encode(zbor::breaker);

    check(codec, {
        0x7f, 0x65, 0x73, 0x74, 0x72, 0x65, 0x61, 0x64, 0x6d, 0x69, 0x6e, 0x67, 0xff, // (_ "strea", "ming")
    });
}

TEST_F(Encode, ExplicitIndefArray)
{
    codec.encode_indef_arr();
    codec.encode_break();

    codec.encode_indef_arr();
    codec.encode(1);
    codec.encode_arr(2);
    codec.encode(2);
    codec.encode(3);
    codec.encode_indef_arr();
    codec.encode(4);
    codec.encode(5);
    codec.encode_break();
    codec.encode_break();

    codec.encode_indef_arr();
    codec.encode(1);
    codec.encode_arr(2);
    codec.encode(2);
    codec.encode(3);
    codec.encode_arr(2);
    codec.encode(4);
    codec.encode(5);
    codec.encode_break();

    codec.encode_arr(3);
    codec.encode(1);
    codec.encode_arr(2);
    codec.encode(2);
    codec.encode(3);
    codec.encode_indef_arr();
    codec.encode(4);
    codec.encode(5);
    codec.encode_break();

    codec.encode_arr(3);
    codec.encode(1);
    codec.encode_indef_arr();
    codec.encode(2);
    codec.encode(3);
    codec.encode_break();
    codec.encode_arr(2);
    codec.encode(4);
    codec.encode(5);

    codec.encode_indef_arr();
    for (int i = 1; i <= 25; ++i)
        codec.encode(i);
    codec.encode_break();

    check(codec, {
        0x9f, 0xff, // [_ ]
        0x9f, 0x01, 0x82, 0x02, 0x03, 0x9f, 0x04, 0x05, 0xff, 0xff, // [_ 1, [2, 3], [_ 4, 5]]
        0x9f, 0x01, 0x82, 0x02, 0x03, 0x82, 0x04, 0x05, 0xff, // [_ 1, [2, 3], [4, 5]]
        0x83, 0x01, 0x82, 0x02, 0x03, 0x9f, 0x04, 0x05, 0xff, // [1, [2, 3], [_ 4, 5]]
        0x83, 0x01, 0x9f, 0x02, 0x03, 0xff, 0x82, 0x04, 0x05, // [1, [_ 2, 3], [4, 5]]
        0x9f, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x18, 0x18, 0x19, 0xff, // [_ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]
    });
}

TEST_F(Encode, ImplicitIndefArray)
{
    using namespace zbor::literals;

    codec.encode(zbor::indef_arr);
    codec.encode(zbor::breaker);

    codec.encode(zbor::indef_arr);
    codec.encode(1);
    codec.encode(2_arr);
    codec.encode(2);
    codec.encode(3);
    codec.encode(zbor::indef_arr);
    codec.encode(4);
    codec.encode(5);
    codec.encode(zbor::breaker);
    codec.encode(zbor::breaker);

    codec.encode(zbor::indef_arr);
    codec.encode(1);
    codec.encode(2_arr);
    codec.encode(2);
    codec.encode(3);
    codec.encode(2_arr);
    codec.encode(4);
    codec.encode(5);
    codec.encode(zbor::breaker);

    codec.encode(3_arr);
    codec.encode(1);
    codec.encode(2_arr);
    codec.encode(2);
    codec.encode(3);
    codec.encode(zbor::indef_arr);
    codec.encode(4);
    codec.encode(5);
    codec.encode(zbor::breaker);

    codec.encode_arr(3);
    codec.encode(1);
    codec.encode(zbor::indef_arr);
    codec.encode(2);
    codec.encode(3);
    codec.encode(zbor::breaker);
    codec.encode(2_arr);
    codec.encode(4);
    codec.encode(5);

    codec.encode(zbor::indef_arr);
    for (int i = 1; i <= 25; ++i)
        codec.encode(i);
    codec.encode(zbor::breaker);

    check(codec, {
        0x9f, 0xff, // [_ ]
        0x9f, 0x01, 0x82, 0x02, 0x03, 0x9f, 0x04, 0x05, 0xff, 0xff, // [_ 1, [2, 3], [_ 4, 5]]
        0x9f, 0x01, 0x82, 0x02, 0x03, 0x82, 0x04, 0x05, 0xff, // [_ 1, [2, 3], [4, 5]]
        0x83, 0x01, 0x82, 0x02, 0x03, 0x9f, 0x04, 0x05, 0xff, // [1, [2, 3], [_ 4, 5]]
        0x83, 0x01, 0x9f, 0x02, 0x03, 0xff, 0x82, 0x04, 0x05, // [1, [_ 2, 3], [4, 5]]
        0x9f, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x18, 0x18, 0x19, 0xff, // [_ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25]
    });
}

TEST_F(Encode, ExplicitIndefMap)
{
    codec.encode_indef_map();
    codec.encode("a");
    codec.encode(1);
    codec.encode("b");
    codec.encode_indef_arr();
    codec.encode(2);
    codec.encode(3);
    codec.encode_break();
    codec.encode_break();

    codec.encode_indef_map();
    codec.encode("Fun");
    codec.encode(true);
    codec.encode("Amt");
    codec.encode(-2);
    codec.encode_break();

    check(codec, {
        0xbf, 0x61, 0x61, 0x01, 0x61, 0x62, 0x9f, 0x02, 0x03, 0xff, 0xff, // {_ "a": 1, "b": [_ 2, 3]}
        0xbf, 0x63, 0x46, 0x75, 0x6e, 0xf5, 0x63, 0x41, 0x6d, 0x74, 0x21, 0xff, // {_ "Fun": true, "Amt": -2}
    });
}

TEST_F(Encode, ImplicitIndefMap)
{
    using namespace zbor::literals;

    codec.encode(zbor::indef_map);
    codec.encode("a");
    codec.encode(1);
    codec.encode("b");
    codec.encode(zbor::indef_arr);
    codec.encode(2);
    codec.encode(3);
    codec.encode(zbor::breaker);
    codec.encode(zbor::breaker);

    codec.encode(zbor::indef_map);
    codec.encode("Fun");
    codec.encode(true);
    codec.encode("Amt");
    codec.encode(-2);
    codec.encode(zbor::breaker);

    check(codec, {
        0xbf, 0x61, 0x61, 0x01, 0x61, 0x62, 0x9f, 0x02, 0x03, 0xff, 0xff, // {_ "a": 1, "b": [_ 2, 3]}
        0xbf, 0x63, 0x46, 0x75, 0x6e, 0xf5, 0x63, 0x41, 0x6d, 0x74, 0x21, 0xff, // {_ "Fun": true, "Amt": -2}
    });
}

TEST_F(Encode, ExplicitMixed)
{
    // TODO
}

TEST_F(Encode, ImplicitMixed)
{
    // TODO
}

TEST_F(Encode, Variadic)
{
    using namespace zbor::literals;
    using namespace std::literals;

    const uint8_t data[2] = {0x55};

    codec.encode_(
        2_arr,
            42u,
            -666,
        zbor::indef_arr,
            2_map,
                0.0,
                7.62e39,
                NAN,
                13_prim, 
            4_tag,
                true,               
            zbor::prim_false,
            zbor::prim_null,
            zbor::prim_undefined,
        zbor::breaker,
        zbor::indef_map,
            "",
            zbor::indef_txt,
                "zbor"sv,
                "really_long_string_view_"sv,
            zbor::breaker,
            zbor::span{},
            zbor::indef_dat,
                zbor::list{0xde, 0xad, 0xc0, 0xde},
                zbor::span{data},
            zbor::breaker,
        zbor::breaker
    );

    check(codec, {
        0x82, 0x18, 0x2a, 0x39, 0x02, 0x99, 0x9f, 0xa2, 0xf9, 0x00, 0x00, 0xfb, 0x48, 0x36, 0x64, 0xa6,
        0x9e, 0x1c, 0x8b, 0xb7, 0xf9, 0x7e, 0x00, 0xed, 0xc4, 0xf5, 0xf4, 0xf6, 0xf7, 0xff, 0xbf, 0x60,
        0x7f, 0x64, 0x7a, 0x62, 0x6f, 0x72, 0x78, 0x18, 0x72, 0x65, 0x61, 0x6c, 0x6c, 0x79, 0x5f, 0x6c,
        0x6f, 0x6e, 0x67, 0x5f, 0x73, 0x74, 0x72, 0x69, 0x6e, 0x67, 0x5f, 0x76, 0x69, 0x65, 0x77, 0x5f,
        0xff, 0x40, 0x5f, 0x44, 0xde, 0xad, 0xc0, 0xde, 0x42, 0x55, 0x00, 0xff, 0xff,
    });
}

TEST_F(Encode, Errors)
{
    const uint8_t dummy[codec.capacity() + 10]{};

    ASSERT_EQ(zbor::err_no_memory, codec.encode(dummy));

    for (int i = 24; i <= 31; ++i)
        ASSERT_EQ(zbor::err_invalid_simple, codec.encode(zbor::prim(i)));

    check(codec, {});
}

TEST_F(Encode, Constexpr)
{
    // TODO
}