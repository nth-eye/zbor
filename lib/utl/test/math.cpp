#include <gtest/gtest.h>
#include <utl/math.h>
#include <cmath>

using namespace utl;

#define EXPECT_NEAR_PR(a, b) EXPECT_NEAR(a, b, 1e-10)

TEST(Math, Radians)
{
    EXPECT_NEAR_PR(radians(0),      0);
    EXPECT_NEAR_PR(radians(15),     M_PI/12);
    EXPECT_NEAR_PR(radians(30),     M_PI/6);
    EXPECT_NEAR_PR(radians(45),     M_PI/4);
    EXPECT_NEAR_PR(radians(90),     M_PI/2);
    EXPECT_NEAR_PR(radians(180),    M_PI);
    EXPECT_NEAR_PR(radians(360),    M_PI*2);
    EXPECT_NEAR_PR(radians(540),    M_PI*3);
    EXPECT_NEAR_PR(radians(66.6),   1.1623892818);

    EXPECT_NEAR_PR(radians(-0),     0);
    EXPECT_NEAR_PR(radians(-15),    -M_PI/12);
    EXPECT_NEAR_PR(radians(-30),    -M_PI/6);
    EXPECT_NEAR_PR(radians(-45),    -M_PI/4);
    EXPECT_NEAR_PR(radians(-90),    -M_PI/2);
    EXPECT_NEAR_PR(radians(-180),   -M_PI);
    EXPECT_NEAR_PR(radians(-360),   -M_PI*2);
    EXPECT_NEAR_PR(radians(-540),   -M_PI*3);
    EXPECT_NEAR_PR(radians(-66.6),  -1.1623892818);
}

TEST(Math, Degrees)
{
    EXPECT_NEAR_PR(degrees(0),          0);
    EXPECT_NEAR_PR(degrees(1),          57.2957795131);
    EXPECT_NEAR_PR(degrees(3.14),       179.9087476711);
    EXPECT_NEAR_PR(degrees(M_PI),       180);
    EXPECT_NEAR_PR(degrees(M_PI*2),     360);
    EXPECT_NEAR_PR(degrees(M_PI*3),     540);
    EXPECT_NEAR_PR(degrees(M_PI/4),     45);
    EXPECT_NEAR_PR(degrees(M_PI/2),     90);

    EXPECT_NEAR_PR(degrees(-0),         0);
    EXPECT_NEAR_PR(degrees(-1),         -57.2957795131);
    EXPECT_NEAR_PR(degrees(-3.14),      -179.9087476711);
    EXPECT_NEAR_PR(degrees(-M_PI),      -180);
    EXPECT_NEAR_PR(degrees(-M_PI*2),    -360);
    EXPECT_NEAR_PR(degrees(-M_PI*3),    -540);
    EXPECT_NEAR_PR(degrees(-M_PI/4),    -45);
    EXPECT_NEAR_PR(degrees(-M_PI/2),    -90);
}

TEST(Math, UnsignedCeil)
{
    EXPECT_EQ(uceil(0, 0), 0);
    EXPECT_EQ(uceil(0, 1), 0);
    EXPECT_EQ(uceil(4, 2), 2);
    EXPECT_EQ(uceil(5, 2), 3);
    EXPECT_EQ(uceil(42, 11), 4); 
}

TEST(Math, IntMap)
{
    EXPECT_EQ(imap(0, 0, 1, 0, 0), 0);
    EXPECT_EQ(imap(1, 0, 1, 0, 0), 0);
    EXPECT_EQ(imap(0, 0, 1, 0, 1), 0);
    EXPECT_EQ(imap(1, 0, 1, 0, 1), 1);
    EXPECT_EQ(imap(0xff, 0x00, 0xff, 0, 100), 100);
    EXPECT_EQ(imap(0x80, 0x00, 0xff, 0, 100), 50);
    EXPECT_EQ(imap(0x40, 0x00, 0xff, 0, 100), 25);
    EXPECT_EQ(imap(0x20, 0x00, 0xff, 0, 100), 12);
    EXPECT_EQ(imap(0x42, 0x00, 0xff, 0, 1000), 258);
    EXPECT_EQ(imap(0x69, 0x00, 0xff, 0, 1000), 411);
}

TEST(Math, IntPower)
{
    EXPECT_EQ(ipow(1, 0), 1);
    EXPECT_EQ(ipow(1, 1), 1);
    EXPECT_EQ(ipow(1, 99), 1);
    EXPECT_EQ(ipow(2, 0), 1);
    EXPECT_EQ(ipow(2, 1), 2);
    EXPECT_EQ(ipow(2, 2), 4);
    EXPECT_EQ(ipow(2, 10), 1024);
    EXPECT_EQ(ipow(3ull, 20), 3486784401);
    EXPECT_EQ(ipow(2ull, 63), 9223372036854775808ull);
}

TEST(Math, IntLength)
{
    EXPECT_EQ(ilen(0), 1);
    EXPECT_EQ(ilen(1), 1);
    EXPECT_EQ(ilen(9), 1);
    EXPECT_EQ(ilen(10), 2);
    EXPECT_EQ(ilen(99), 2);
    EXPECT_EQ(ilen(100), 3);
    EXPECT_EQ(ilen(999), 3);
    EXPECT_EQ(ilen(20220805), 8);
}

TEST(Math, Factorial)
{
    EXPECT_EQ(fact(0), 1);
    EXPECT_EQ(fact(1), 1);
    EXPECT_EQ(fact(2), 2);
    EXPECT_EQ(fact(3), 6);
    EXPECT_EQ(fact(4), 24);
    EXPECT_EQ(fact(13ul), 6227020800); 
}

TEST(Math, GaloisFieldMultiplication)
{
    
}
