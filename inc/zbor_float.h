#ifndef ZBOR_FLOAT_H
#define ZBOR_FLOAT_H

#include <cstdint>
#include <cmath>
#include <cfloat>

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

double half_to_double(uint16_t x);
uint32_t half_to_float(uint16_t h);
uint16_t half_from_float(uint32_t f);

};

#endif