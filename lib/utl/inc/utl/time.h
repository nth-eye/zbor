#ifndef UTL_TIME_H
#define UTL_TIME_H

#include "utl/str.h"
#include <ctime>

namespace utl {

/**
 * @brief Measure execution time of a function.
 * 
 * @tparam N Number of calls
 * @tparam Fn Function pointer
 * @tparam Args Arguments of the function
 * @param fn Function pointer
 * @param args Arguments of the function
 * @return Total clock ticks for N executions
 */
template<size_t N = 1, class Fn, class ...Args>
clock_t exec_time(Fn &&fn, Args &&...args)
{
    clock_t begin = clock();
    for (size_t i = 0; i < N; ++i) 
        fn(args...);
    clock_t end = clock();
    return (end - begin);
}

/**
 * @brief Measure average execution time of a function for N calls.
 * 
 * @tparam N Number of calls
 * @tparam Args Function pointer followed by arguments
 * @param args Function pointer followed by arguments
 * @return Average clock ticks for each call
 */
template<size_t N = 1, class ...Args>
clock_t exec_time_avg(Args &&...args)
{
    return exec_time<N>(args...) / N;
}

/**
 * @brief Measure average execution time of a member function for N calls.
 * 
 * @tparam N Number of calls
 * @tparam Fn Member function pointer
 * @tparam Ptr Object pointer
 * @tparam Args Arguments of the member function
 * @param fn Member function pointer
 * @param ptr Object pointer
 * @param args Arguments of the member function
 * @return Total clock ticks for N executions 
 */
template<size_t N = 1, class Fn, class Ptr, class ...Args>
clock_t m_exec_time(Fn &&fn, Ptr *ptr, Args &&...args)
{
    clock_t begin = clock();
    for (size_t i = 0; i < N; ++i) 
        (ptr->*fn)(args...);
    clock_t end = clock();
    return (end - begin);
}

/**
 * @brief Measure average execution time of a member function for N calls.
 * 
 * @tparam N Number of calls
 * @tparam Args Member function pointer followed by object pointer and arguments
 * @param args Member function pointer followed by object pointer and arguments
 * @return Average clock ticks for each call 
 */
template<size_t N = 1, class ...Args>
clock_t m_exec_time_avg(Args &&...args)
{
    return m_exec_time<N>(args...) / N;
}

/**
 * @brief Calculate seconds since epoch without timezone correction, 
 * using days since January 1 instead of month and month day.
 * 
 * @param year Years since 1900
 * @param yday Days since January 1 - [0, 365]
 * @param hour Hours since midnight – [0, 23]
 * @param min Minutes after the hour – [0, 59]
 * @param sec Seconds after the minute - [0, 60]
 * @return UTC timestamp
 */
constexpr int64_t timeutc(int64_t year, int64_t yday, int64_t hour, int64_t min, int64_t sec)
{
    return sec + min * 60 + hour * 3600 + yday * 86400 +
    (year - 70) * 31536000 + ((year - 69) / 4) * 86400 -
    ((year - 1) / 100) * 86400 + ((year + 299) / 400) * 86400;
}

/**
 * @brief Calculate seconds since epoch without timezone correction,
 * using month and month day instead of days since January 1.
 * 
 * @param year Years since 1900
 * @param month Months since January – [0, 11]
 * @param mday Day of the month – [1, 31]
 * @param hour Hours since midnight – [0, 23]
 * @param min Minutes after the hour – [0, 59]
 * @param sec Seconds after the minute - [0, 60]
 * @return UTC timestamp
 */
constexpr int64_t timeutc(int64_t year, int64_t month, int64_t mday, int64_t hour, int64_t min, int64_t sec)
{
    if (month < 0 || month > 11)
        return 0;
    constexpr int64_t days[] = {
        31, 28, 31, 30, 
        31, 30, 31, 31, 
        30, 31, 30, 31
    };
    int64_t yday = mday - 1;
    for (int i = 0; i < month; ++i)
        yday += days[i];
    return timeutc(year, yday, hour, min, sec);
}

/**
 * @brief Calculate seconds since epoch without timezone correction,
 * using tm struct.
 * 
 * @param utc Standard date-time structure
 * @return UTC timestamp
 */
constexpr int64_t timeutc(const tm &utc)
{
    return utc.tm_mday ? 
        timeutc(utc.tm_year, utc.tm_mon, utc.tm_mday, utc.tm_hour, utc.tm_min, utc.tm_sec) :
        timeutc(utc.tm_year, utc.tm_yday, utc.tm_hour, utc.tm_min, utc.tm_sec);
}

/**
 * @brief Get date and time when the code was compiled as tm struct.
 * 
 * @return Standard date-time structure
 */
constexpr tm compilation_tm()
{
    constexpr auto timestr = __DATE__ " " __TIME__;
    constexpr auto spacebar = timestr[4] == ' ';
    constexpr auto tokens = split<6 + spacebar>(timestr, " :");
    constexpr std::string_view months[] = {
        "Jan", "Feb", "Mar", "Apr",
        "May", "Jun", "Jul", "Aug",
        "Sep", "Oct", "Nov", "Dec",
    };
    static_assert(tokens.size() == 6 + spacebar);

    tm utc = {};

    for (size_t i = 0; i < countof(months); ++i) {
        if (tokens[0] == months[i]) {
            utc.tm_mon = i;
            break;
        }
    }
    utc.tm_mday = str_to_int(tokens[spacebar + 1]);
    utc.tm_year = str_to_int(tokens[spacebar + 2]) - 1900;
    utc.tm_hour = str_to_int(tokens[spacebar + 3]);
    utc.tm_min = str_to_int(tokens[spacebar + 4]);
    utc.tm_sec = str_to_int(tokens[spacebar + 5]);

    return utc;
}

/**
 * @brief Get seconds since epoch when the code was compiled, 
 * without timezone correction. Subtract timezone offset to 
 * aquire actual local time of compilaiton. Without correction 
 * it will be same as GMT timestamp.
 * 
 * @return UTC timestamp
 */
constexpr int64_t compilation_utc()
{
    return timeutc(compilation_tm());
}

}

#endif