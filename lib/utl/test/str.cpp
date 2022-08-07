#include <gtest/gtest.h>
#include <utl/str.h>

using namespace utl;

template<class T>
static void compare(const T *out, const T *exp, size_t len)
{
    ASSERT_TRUE(out);
    ASSERT_TRUE(exp);
    for (size_t i = 0; i < len; ++i)
        ASSERT_EQ(out[i], exp[i]) << "At index " << i;
}

TEST(Str, BinToChar)
{
    EXPECT_EQ(bin_to_char(0), '0');
    EXPECT_EQ(bin_to_char(1), '1');
    EXPECT_EQ(bin_to_char(2), '2');
    EXPECT_EQ(bin_to_char(3), '3');
    EXPECT_EQ(bin_to_char(4), '4');
    EXPECT_EQ(bin_to_char(5), '5');
    EXPECT_EQ(bin_to_char(6), '6');
    EXPECT_EQ(bin_to_char(7), '7');
    EXPECT_EQ(bin_to_char(8), '8');
    EXPECT_EQ(bin_to_char(9), '9');
    EXPECT_EQ(bin_to_char(10), 'a');
    EXPECT_EQ(bin_to_char(11), 'b');
    EXPECT_EQ(bin_to_char(12), 'c');
    EXPECT_EQ(bin_to_char(13), 'd');
    EXPECT_EQ(bin_to_char(14), 'e');
    EXPECT_EQ(bin_to_char(15), 'f');
    EXPECT_EQ(bin_to_char(16), '0');
}

TEST(Str, StringToDouble)
{
    EXPECT_EQ(str_to_dbl("0"), 0);
    EXPECT_EQ(str_to_dbl("4"), 4);
    EXPECT_EQ(str_to_dbl("5."), 5);
    EXPECT_EQ(str_to_dbl(".9"), 0.9);
    EXPECT_EQ(str_to_dbl("+5."), 5);
    EXPECT_EQ(str_to_dbl("-.9"), -0.9);
    EXPECT_EQ(str_to_dbl("0.0"), 0);
    EXPECT_EQ(str_to_dbl("0.1"), 0.1);
    EXPECT_EQ(str_to_dbl("+0.2"), 0.2);
    EXPECT_EQ(str_to_dbl("-0.3"), -0.3);
    EXPECT_EQ(str_to_dbl("666.777"), 666.777);
    EXPECT_EQ(str_to_dbl("+666.777"), 666.777);
    EXPECT_EQ(str_to_dbl("-888.999"), -888.999);
    
    EXPECT_EQ(str_to_dbl({}), 0);
    EXPECT_EQ(str_to_dbl(""), 0);
    EXPECT_EQ(str_to_dbl("hello"), 0);
    EXPECT_EQ(str_to_dbl("1.0.0"), 0);
    EXPECT_EQ(str_to_dbl("--1"), 0);
    EXPECT_EQ(str_to_dbl(" 42"), 0);
    EXPECT_EQ(str_to_dbl(" 69.0"), 0);
}

TEST(Str, StringToInt)
{
    EXPECT_EQ(str_to_int("0"), 0);
    EXPECT_EQ(str_to_int("1"), 1);
    EXPECT_EQ(str_to_int("-1"), -1);
    EXPECT_EQ(str_to_int("12"), 12);
    EXPECT_EQ(str_to_int("+12"), 12);
    EXPECT_EQ(str_to_int("-12"), -12);
    EXPECT_EQ(str_to_int("+1234"), 1234);
    EXPECT_EQ(str_to_int("-1234"), -1234);
    EXPECT_EQ(str_to_int("2147483647"), 2147483647);
    EXPECT_EQ(str_to_int("-2147483648"), -2147483648);

    EXPECT_EQ(str_to_int({}), 0);
    EXPECT_EQ(str_to_int(""), 0);
    EXPECT_EQ(str_to_int("hello"), 0);
    EXPECT_EQ(str_to_int("10 "), 0);
    EXPECT_EQ(str_to_int("--1"), 0);
    EXPECT_EQ(str_to_int(" 42"), 0);
}

TEST(Str, StringToBinary)
{
    uint8_t bin[8];

    const char *str_1 = "11";

    ASSERT_EQ(str_to_bin(nullptr, strlen(str_1), bin, sizeof(bin)), 0);
    ASSERT_EQ(str_to_bin(str_1, 0, bin, sizeof(bin)), 0);
    ASSERT_EQ(str_to_bin(str_1, strlen(str_1), bin, 0), 0);
    ASSERT_EQ(str_to_bin(str_1, strlen(str_1), nullptr, sizeof(bin)), 0);

    const char *str_2       = "0123456789abcdef";
    const uint8_t exp_2[]   = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};

    ASSERT_EQ(str_to_bin(str_2, strlen(str_2), bin, sizeof(bin)), 8);

    compare(bin, exp_2, sizeof(exp_2));

    const char *str_3       = "0123456789abcdef0011223344556677";
    const uint8_t exp_3[]   = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};

    ASSERT_EQ(str_to_bin(str_3, strlen(str_3), bin, sizeof(bin)), 8);

    compare(bin, exp_3, sizeof(exp_3));

    const char *str_4       = "4";
    const uint8_t exp_4[]   = {0x04};

    ASSERT_EQ(str_to_bin(str_4, strlen(str_4), bin, sizeof(bin)), 1);

    compare(bin, exp_4, sizeof(exp_4));

    const char *str_5       = "67b";
    const uint8_t exp_5[]   = {0x06, 0x7b};

    ASSERT_EQ(str_to_bin(str_5, strlen(str_5), bin, sizeof(bin)), 2);

    compare(bin, exp_5, sizeof(exp_5));
}

TEST(Str, BinaryToString)
{
    char str[16] = {};

    const uint8_t bin_1[] = {0x11};

    ASSERT_EQ(bin_to_str(nullptr, 100, str, sizeof(str)), 0);
    ASSERT_EQ(bin_to_str(bin_1, 0, str, sizeof(str)), 0);
    ASSERT_EQ(bin_to_str(bin_1, sizeof(bin_1), nullptr, sizeof(str)), 0);
    ASSERT_EQ(bin_to_str(bin_1, sizeof(bin_1), str, 0), 0);

    const char *exp_2       = "0123456789abcdef";
    const uint8_t bin_2[]   = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};

    ASSERT_EQ(bin_to_str(bin_2, sizeof(bin_2), str, sizeof(str)), 16);

    compare(str, exp_2, strlen(exp_2));

    const char *exp_3       = "0123456789abcdef";
    const uint8_t bin_3[]   = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};

    ASSERT_EQ(bin_to_str(bin_3, sizeof(bin_3), str, sizeof(str)), 16);

    compare(str, exp_3, strlen(exp_3));

    const char *exp_4       = "0123456789abcd";
    const uint8_t bin_4[]   = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};

    ASSERT_EQ(bin_to_str(bin_4, sizeof(bin_4), str, sizeof(str) - 1), 14);

    compare(str, exp_4, strlen(exp_4));
}

TEST(Str, BinaryToCString)
{
    char str[17] = {};

    const uint8_t bin_1[] = {0x11};

    ASSERT_EQ(bin_to_cstr(nullptr, 100, str, sizeof(str)), 0);
    ASSERT_EQ(bin_to_cstr(bin_1, 0, str, sizeof(str)), 0);
    ASSERT_EQ(bin_to_cstr(bin_1, sizeof(bin_1), nullptr, sizeof(str)), 0);
    ASSERT_EQ(bin_to_cstr(bin_1, sizeof(bin_1), str, 0), 0);

    const uint8_t bin_2[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};

    ASSERT_EQ(bin_to_cstr(bin_2, sizeof(bin_2), str, sizeof(str)), 16);
    ASSERT_STREQ(str, "0123456789abcdef");

    const uint8_t bin_3[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};

    ASSERT_EQ(bin_to_cstr(bin_3, sizeof(bin_3), str, sizeof(str)), 16);
    ASSERT_STREQ(str, "0123456789abcdef");

    const uint8_t bin_4[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};

    ASSERT_EQ(bin_to_cstr(bin_4, sizeof(bin_4), str, sizeof(str) - 1), 14);
    ASSERT_STREQ(str, "0123456789abcd");
}

TEST(Str, Split)
{
    auto str_1 = "hello,world";
    auto spl_1 = split<2>(str_1, ",");

    ASSERT_EQ(spl_1.size(), 2);
    ASSERT_EQ(spl_1[0], "hello");
    ASSERT_EQ(spl_1[1], "world");

    auto str_2 = "hello";
    auto spl_2 = split<2>(str_2, ",");

    ASSERT_EQ(spl_2.size(), 1);
    ASSERT_EQ(spl_2[0], "hello");

    auto str_3 = "hello,";
    auto spl_3 = split<2>(str_3, ",");

    ASSERT_EQ(spl_3.size(), 2);
    ASSERT_EQ(spl_3[0], "hello");
    ASSERT_EQ(spl_3[1], "");

    auto str_4 = ",hello";
    auto spl_4 = split<2>(str_4, ",");

    ASSERT_EQ(spl_4.size(), 2);
    ASSERT_EQ(spl_4[0], "");
    ASSERT_EQ(spl_4[1], "hello");

    auto str_5 = "hello,world,overflow";
    auto spl_5 = split<2>(str_5, ",");

    ASSERT_EQ(spl_5.size(), 2);
    ASSERT_EQ(spl_5[0], "hello");
    ASSERT_EQ(spl_5[1], "world");

    auto str_6 = "hello,.+world/";
    auto spl_6 = split<4>(str_6, ",.+");

    ASSERT_EQ(spl_6.size(), 4);
    ASSERT_EQ(spl_6[0], "hello");
    ASSERT_EQ(spl_6[1], "");
    ASSERT_EQ(spl_6[2], "");
    ASSERT_EQ(spl_6[3], "world/");
}
