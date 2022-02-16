#ifndef ZBOR_FLOAT_H
#define ZBOR_FLOAT_H

#include <cstdint>

namespace zbor {

#define ZBOR_USE_FP16 true

union Float {
    Float() {}
    Float(float f) : f32(f) {}
    Float(double f) : f64(f) {}
    Float(uint16_t u) : u16(u) {}
    Float(uint32_t u) : u32(u) {}
    Float(uint64_t u) : u64(u) {} 
    uint16_t    u16;
    uint32_t    u32;
    float       f32;
    uint64_t    u64;
    double      f64;
};

uint32_t half_to_float(uint16_t h);
uint16_t half_from_float(uint32_t f);
uint64_t half_to_double(uint16_t h);
uint16_t half_from_double(uint64_t d);
double half_to_double_direct(uint16_t h);

};

#endif