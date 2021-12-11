#ifndef CBOR_BUF_H
#define CBOR_BUF_H

#include "cbor.h"
#include "cbor_float.h"

namespace cbor {

constexpr int bit(int x)    { return 1 << x; }
// constexpr int get_mt(int x) { return x & 0xe0; }
// constexpr int get_ai(int x) { return x & 0x1f; }

template<size_t N>
struct Codec {

    Err encode(CBOR *val);
    Err encode(int val);
    Err encode(int64_t val);
    Err encode(uint64_t val);
    Err encode(const uint8_t *data, size_t len);
    Err encode(const char *text, size_t len);
    Err encode(Array arr);
    Err encode(Map map);
    Err encode(Tag tag);
    Err encode(Prim val);
    Err encode(bool val);
    Err encode(float val);
    Err encode(double val);
    Err encode_n18446744073709551616();

    template<class Pool>
    Err decode(Pool &pool, uint8_t *buf, size_t buf_len);

    size_t size() const { return cnt; }

    uint8_t buf[N];
private:
    Err encode_base(uint8_t start, uint64_t val, size_t ai_len, size_t add_len = 0);
    Err encode_int(MT mt, uint64_t val, size_t add_len = 0);
    Err encode_bytes(MT mt, const void *data, size_t len);
    Err encode_float(Prim type, Float val);

    size_t cnt = 0;
};

// SECTION: Private

template<size_t N>
Err Codec<N>::encode_base(uint8_t start, uint64_t val, size_t ai_len, size_t add_len)
{
    if (cnt + ai_len + add_len + 1 > N)
        return ERR_OUT_OF_MEM;

    buf[cnt++] = start;
    for (int i = 8 * ai_len - 8; i >= 0; i -= 8)
        buf[cnt++] = val >> i;

    return NO_ERR;
}

template<size_t N>
Err Codec<N>::encode_int(MT mt, uint64_t val, size_t add_len)
{
    AI ai;

    if (val <= AI_0)
        ai = AI(val);
    else if (val <= 0xff)
        ai = AI_1;
    else if (val <= 0xffff)
        ai = AI_2;
    else if (val <= 0xffffffff)
        ai = AI_4;
    else
        ai = AI_8;

    size_t ai_len = (ai <= AI_0) ? 0 : bit(ai - AI_1);

    return encode_base(mt | ai, val, ai_len, add_len);
}

template<size_t N>
Err Codec<N>::encode_bytes(MT mt, const void *data, size_t len)
{
    Err err = encode_int(mt, len, len);
    if (err == NO_ERR && data && len) {
        memcpy(&buf[cnt], data, len);
        cnt += len;
    }
    return err;
}

template<size_t N>
Err Codec<N>::encode_float(Prim type, Float val)
{
    switch (type) {

        case PRIM_FLOAT_64:
        {
            if (val.f64 != val.f64)
                goto if_nan;

            float f32 = val.f64;
            if (val.f64 != f32) // Else we can fallthrough to FLOAT_32
                return encode_base(MT_SIMPLE | PRIM_FLOAT_64, val.u64, 8);
            val.f32 = f32;
        }

        case PRIM_FLOAT_32:
        {
            if (val.f32 != val.f32)
                goto if_nan;
#if CBOR_USE_FLOAT16
            half f16 = val.f32;
            if (val.f32 != f16) // Else we can fallthrough to FLOAT_16
                return encode_base(MT_SIMPLE | PRIM_FLOAT_32, val.u32, 4);
            val.f16 = f16;
#else
            return encode_base(MT_SIMPLE | PRIM_FLOAT_32, val.u32, 4);
        if_nan: 
            return encode_base(MT_SIMPLE | PRIM_FLOAT_32, 0x7fc00000, 4);
#endif
        }
#if CBOR_USE_FLOAT16
        case PRIM_FLOAT_16:
            if (val.f16 != val.f16)
                goto if_nan;
            return encode_base(MT_SIMPLE | PRIM_FLOAT_16, val.u16, 2);
        if_nan: 
            return encode_base(MT_SIMPLE | PRIM_FLOAT_16, 0x7e00, 2);
#endif
        default:
            return ERR_INVALID_PARAM;
    }
}

// !SECTION: Private

template<size_t N>
Err Codec<N>::encode(CBOR *val)
{
    if (!val)
        return ERR_NULL_PTR;

    switch (val->type) {
        case TYPE_UINT: return encode(val->uint);
        case TYPE_SINT: return encode(val->sint);
        case TYPE_DATA: return encode(val->str.data, val->str.len);
        case TYPE_TEXT: return encode(val->str.text, val->str.len);
        case TYPE_ARRAY: return encode(val->arr);
        case TYPE_MAP: return encode(val->map);
        case TYPE_TAG: return encode(val->tag);
        case TYPE_SIMPLE: return encode(val->prim);
        case TYPE_FLOAT: return encode(val->f);
        case TYPE_DOUBLE: return encode(val->d);
        default: return ERR_INVALID_TYPE;
    }
}

template<size_t N>
Err Codec<N>::encode(int val)
{
    return encode(int64_t(val));
}

template<size_t N>
Err Codec<N>::encode(uint64_t val)
{
    return encode_int(MT_UINT, val);
}

template<size_t N>
Err Codec<N>::encode(int64_t val)
{
    uint64_t ui = val >> 63;
    return encode_int(MT(ui & 0x20), ui ^ val);
}

template<size_t N>
Err Codec<N>::encode(const uint8_t *data, size_t len)
{
    return encode_bytes(MT_DATA, data, len);
}

template<size_t N>
Err Codec<N>::encode(const char *text, size_t len)
{
    return encode_bytes(MT_TEXT, text, len);
}

template<size_t N>
Err Codec<N>::encode(Array arr)
{
    Err err = encode_int(MT_ARRAY, arr.len);
    if (err != NO_ERR)
        return err;

    for (auto it : arr) {
        if ((err = encode(&it)) != NO_ERR)
            return err;
    }
    return NO_ERR;
}

template<size_t N>
Err Codec<N>::encode(Map map)
{
    Err err = encode_int(MT_MAP, map.len);
    if (err != NO_ERR)
        return err;

    for (auto it : map) {
        if ((err = encode(it.key)) != NO_ERR ||
            (err = encode(it.val)) != NO_ERR)
            return err;
    }
    return NO_ERR;
}

template<size_t N>
Err Codec<N>::encode(Tag tag)
{
    Err err = encode_int(MT_TAG, tag.val, 0);
    if (err != NO_ERR)
        return err; 
    return encode(tag.content);
}

template<size_t N>
Err Codec<N>::encode(Prim val)
{
    if (val >= 24 && val <= 31)
        return ERR_INVALID_PARAM;

    return encode_int(MT_SIMPLE, val);
}

template<size_t N>
Err Codec<N>::encode(bool val)
{
    return encode(val ? PRIM_TRUE : PRIM_FALSE);
}

template<size_t N>
Err Codec<N>::encode(float val)
{
    Float tmp;
    tmp.f32 = val;
    return encode_float(PRIM_FLOAT_32, tmp);
}

template<size_t N>
Err Codec<N>::encode(double val)
{
    Float tmp;
    tmp.f64 = val;
    return encode_float(PRIM_FLOAT_64, tmp);
}

template<size_t N>
Err Codec<N>::encode_n18446744073709551616()
{
    return encode_int(MT_NINT, 0xffffffffffffffff);
}

template<size_t N>
template<class Pool>
Err Codec<N>::decode(Pool &pool, uint8_t *buf, size_t buf_len)
{
    if (!buf || !buf_len)
        return ERR_INVALID_PARAM;

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
    return NO_ERR;
}

}

#endif