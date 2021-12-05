#ifndef CBOR_BUF_H
#define CBOR_BUF_H

#include "cbor.h"

namespace cbor {

template<size_t N>
struct Buf {

    bool encode(CBOR *val);
    bool encode_sint(int64_t val);
    bool encode_uint(uint64_t val);
    bool encode_nint(uint64_t val); // If you need -2^64 (-18446744073709551616)
    bool encode_data(const void *data, size_t len);
    bool encode_text(const char *text, size_t len);
    bool encode_prim(Prim val);
    bool encode_fp16(half val);
    bool encode_fp32(float val);
    bool encode_fp64(double val);

    uint8_t buf[N];
    size_t  cnt = 0;
};

// constexpr unsigned bit(int x)       { return 1 << x; }
// constexpr uint8_t get_mt(uint8_t x) { return x & 0xe0; }
// constexpr uint8_t get_ai(uint8_t x) { return x & 0x1f; }
// constexpr double half_to_double(unsigned x) 
// {
//     unsigned exp = (x >> 10) & 0x1f;
//     unsigned mant = x & 0x3ff;
//     double val = 0;
//     if (exp == 0) 
//         val = ldexp(mant, -24);
//     else if (exp != 31) 
//         val = ldexp(mant + 1024, exp - 25);
//     else 
//         val = mant == 0 ? INFINITY : NAN;
//     return x & 0x8000 ? -val : val;
// }
// constexpr float half_to_float(unsigned x) 
// { 
//     uint32_t e = (x & 0x7C00) >> 10; // exponent
//     uint32_t m = (x & 0x03FF) << 13; // mantissa
//     float f = m;

//     uint32_t u = reinterpret_cast<uint32_t&>(f);
//     uint32_t v = u >> 23; // evil log2 bit hack to count leading zeros in denormalized format
    
//     // sign : normalized : denormalized
//     u = (x & 0x8000) << 16 | 
//         (e != 0) * ((e + 112) << 23 | m) | 
//         ((e == 0) & (m != 0)) * ((v - 37) << 23 | ((m << (150 - v)) & 0x007fe000));
    
//     return reinterpret_cast<float&>(u);
// }
// constexpr half float_to_half(uint32_t x) 
// {
//     uint32_t b = x + 0x00001000; // round-to-nearest-even: add last bit after truncated mantissa
//     uint32_t e = (b & 0x7f800000) >> 23; // exponent
//     uint32_t m = b & 0x007fffff; // mantissa; in line below: 0x007FF000 = 0x00800000-0x00001000 = decimal indicator flag - initial rounding

//     // sign : normalized : denormalized : saturate
//     return  (b & 0x80000000) >> 16 | 
//             (e > 112) * ((((e - 112) << 10) & 0x7c00) | m >> 13) | 
//             ((e < 113) & (e > 101)) * ((((0x007ff000 + m) >> (125 - e)) + 1) >> 1) | 
//             (e > 143) * 0x7fff;
// }

// template<size_t N>
// Err Pool<N>::decode(uint8_t *buf, size_t buf_len)
// {
//     if (!buf || !buf_len)
//         return ERR_INVALID_PARAM;

//     Float fp;
//     CBOR *obj;

//     uint8_t *p      = buf;
//     uint8_t *end    = buf + buf_len;

//     while (p < end) {

//         if ((obj = make()) == NULL)
//             return ERR_OUT_OF_MEM;

//         uint8_t mt = get_mt(*p);
//         uint8_t ai = get_ai(*p++);
//         uint64_t val = ai;

//         obj->type = mt >> 5;

//         switch (ai) 
//         {
//         case AI_1:
//         case AI_2:
//         case AI_4:
//         case AI_8:
//         {
//             size_t len = bit(ai - AI_1);
//             if (p + len > end)
//                 return ERR_OUT_OF_DATA;
//             val = 0;
//             for (int i = 8 * len - 8; i >= 0; i -= 8)
//                 val |= ((uint64_t) *p++) << i;
//             break;
//         }
//         case 28:
//         case 29:
//         case 30:
//             return ERR_INVALID_DATA;
//             break;

//         case AI_INDEF:
//             return ERR_INVALID_DATA;
//             break;
//         }

//         switch (obj->type) 
//         {
//         case MT_UINT:
//             obj->uint = val;
//             break;

//         case MT_NINT:
//             obj->sint = ~val;
//             break;

//         case MT_DATA:
//         case MT_TEXT:
//             if (p + val > end)
//                 return ERR_OUT_OF_DATA;
//             obj->str.data   = p;
//             obj->str.len    = val;
//             p += val;
//             break;
        
//         case MT_ARRAY:
//         case MT_MAP:
//         case MT_TAG:
//             return ERR_INVALID_DATA;

//         case MT_SIMPLE:
//             switch (ai) 
//             {
//             case PRIM_FLOAT_16:
//                 obj->f.f64  = half_to_double(val);
//                 obj->type   = TYPE_DOUBLE;
//             case PRIM_FLOAT_32:
//                 fp.u32      = val;
//                 obj->f.f64  = fp.f32;
//                 obj->type   = TYPE_DOUBLE;
//                 break;
//             case PRIM_FLOAT_64:
//                 fp.u64      = val;
//                 obj->f.f64  = fp.f64;
//                 obj->type   = TYPE_DOUBLE;
//                 break;
//             default:
//                 obj->prim = val;
//                 if (val >= 24 &
//                     val <= 31)
//                     return ERR_INVALID_DATA;
//                 break;
//             }
//             break;

//         default:
//             return ERR_INVALID_DATA;
//             break;
//         }
//     }
//     return NO_ERR;
// }

}

#endif