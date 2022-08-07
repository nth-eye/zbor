#ifndef UTL_PHYSICS_H
#define UTL_PHYSICS_H

#include "utl/math.h"
#include <cmath>

namespace utl {

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