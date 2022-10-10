#ifndef UTL_MATH_H
#define UTL_MATH_H

#include "utl/base.h"
#include <cmath>

namespace utl {
namespace imp {
inline constexpr double pi = 3.14159265358979323846; // M_PI isn't always defined
}

/**
 * @brief Convert decimal degrees to radians.
 * 
 * @param deg Decimal degrees
 * @return Radians
 */
constexpr double radians(double deg)
{
    return deg * imp::pi / 180.0;
}

/**
 * @brief Convert radians to decimal degrees.
 * 
 * @param rad Radians
 * @return Decimal degrees
 */
constexpr double degrees(double rad)
{
    return rad * 180.0 / imp::pi;
}

/**
 * @brief Unsigned integer division with round up.
 * 
 * @tparam T Unsigned integer type
 * @param dividend Unsigned
 * @param divisor Unsigned, can be 0
 * @return Quotient
 */
template<class T>
constexpr T uceil(T dividend, T divisor)
{
    return divisor ? (dividend + (divisor - 1)) / divisor : 0;
}

/**
 * @brief map_t integer from one range to another. 
 * Input range difference shouldn't be zero.
 * 
 * @tparam T Integer type
 * @param val Input value
 * @param in_min Input minimum
 * @param in_max Input maximum
 * @param out_min Output range minimum 
 * @param out_max Output range maximum
 * @return Result
 */
template<class T>
constexpr T imap(T val, T in_min, T in_max, T out_min, T out_max)
{
    double slope = 1.0 * (out_max - out_min) / (in_max - in_min);
    return out_min + slope * (val - in_min);
}

/**
 * @brief Integer power function.
 * 
 * @tparam T Integer type
 * @param b Base
 * @param e Exponent
 * @return 'base' to the power 'exp'
 */
template<class T>
constexpr T ipow(T b, unsigned e)
{
    T r = 1;
    while (e) {
        if (e & 1)
            r *= b;
        b *= b;
        e >>= 1;
    }
    return r;
}

/**
 * @brief Get length of integer in symbols. 10 => 2, 100 => 3, etc.
 * 
 * @tparam T Integer type
 * @param val Integer value
 * @return Length  
 */
template<class T>
constexpr T ilen(T val)
{
    T l = !val;
    while (val) { 
        ++l; 
        val /= 10; 
    }
    return l;
}

/**
 * @brief Factorial, negative input leads to undefined behavior.
 * 
 * @tparam T Integer type
 * @param x Argument
 * @return Result
 */
template<class T>
constexpr T fact(T x)
{
    T res = 1;
    for (T i = 2; i <= x; ++i)
        res *= i;
    return res;
}

/**
 * @brief Galois 2^8 field multiplication using Russian 
 * Peasant Multiplication algorithm.
 * 
 * @param x Multiplicand
 * @param y Multiplier
 * @return Product
 */
constexpr uint8_t gf_mul(uint8_t x, uint8_t y) 
{
    uint8_t r = 0; 

    while (y) {
        if (y & 1)
            r ^= x; 
        x = (x << 1) ^ ((x >> 7) * 0x11d);
        y >>= 1;
    }
    return r;
}

/**
 * @brief Calculate distance in meters on a sphere surface between 
 * two decimal degree points.
 * 
 * @param lat_1 Latitude of first point
 * @param lng_1 Longitude of first point
 * @param lat_2 Latitude of second point
 * @param lng_2 Longitude of second point
 * @param radius Sphere radius in meters
 * @return Distance in meters 
 */
inline double haversine(double lat_1, double lng_1, double lat_2, double lng_2, double radius)
{
    double a =  pow(sin(radians(lat_2 - lat_1) / 2), 2) +
                pow(sin(radians(lng_2 - lng_1) / 2), 2) *
                cos(radians(lat_1)) * 
                cos(radians(lat_2));
    double c = 2 * asin(sqrt(a));
    return c * radius;
}

/**
 * @brief Calculate distance in meters on Earth between two 
 * decimal degree points in geographic coordinate system.
 * 
 * @param lat_1 Latitude of first point
 * @param lng_1 Longitude of first point
 * @param lat_2 Latitude of second point
 * @param lng_2 Longitude of second point
 * @return Distance in meters 
 */
inline double gcs_distance(double lat_1, double lng_1, double lat_2, double lng_2)
{
    return haversine(lat_1, lng_1, lat_2, lng_2, 6371000);
}

/**
 * @brief Calculate tilt using accelerometer data.
 * 
 * @param x Axis X acceleration 
 * @param y Axis Y acceleration
 * @param z Axis Z acceleration
 * @return Inclination angle in degrees
 */
inline double inclination(double x, double y, double z)
{
    return degrees(acos(z / sqrt(x * x + y * y + z * z)));
}

/**
 * @brief Calculate Rxyz roll using accelerometer data.
 * 
 * @param y Axis Y acceleration
 * @param z Axis Z acceleration
 * @return Roll angle in degrees
 */
inline double roll(double y, double z)
{
    return degrees(atan2(y, z));
}

/**
 * @brief Calculate Rxyz pitch using accelerometer data.
 * 
 * @param x Axis X acceleration
 * @param y Axis Y acceleration
 * @param z Axis Z acceleration
 * @return Pitch angle in degrees
 */
inline double pitch(double x, double y, double z)
{
    return degrees(atan2(-x, sqrt(y * y + z * z)));
}

}

#endif