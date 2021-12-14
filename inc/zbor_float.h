#ifndef ZBOR_FLOAT_H
#define ZBOR_FLOAT_H

#include <cstdint>
#include <cmath>
#include <cfloat>
#include "half.h"

namespace zbor {

#define ZBOR_USE_FP16       true
#define ZBOR_USE_FP16_SW    true

#if ZBOR_USE_FP16
#if ZBOR_USE_FP16_SW
using half = uint16_t;
#else
using half = _Float16;
#endif
#endif

union Float {
    Float(float f) : f32(f) {}
    Float(double f) : f64(f) {} 
    Float(uint32_t u) : u32(u) {}
    Float(uint64_t u) : u64(u) {} 
#if ZBOR_USE_FP16
    uint16_t    u16;
    half        f16;
#endif
    uint32_t    u32;
    float       f32;
    uint64_t    u64;
    double      f64;
};

inline double half_to_double(uint16_t x) 
{
    unsigned exp = (x >> 10) & 0x1f;
    unsigned mant = x & 0x3ff;
    double val;
    if (exp == 0) 
        val = ldexp(mant, -24);
    else if (exp != 31) 
        val = ldexp(mant + 1024, exp - 25);
    else 
        val = mant == 0 ? INFINITY : NAN;
    return x & 0x8000 ? -val : val;
}

};

#endif