#ifndef ZBOR_ENCODE_H
#define ZBOR_ENCODE_H

#include "zbor_base.h"
#include "zbor_float.h"

namespace zbor {

template<size_t N>
struct Encoder {

    static_assert(N, "encoder size must be > 0");

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
    Err encode_start_indef(MT mt);
    Err encode_break();

    const uint8_t& operator[](size_t i) const { return buf[i]; }
    const uint8_t* data() const { return buf; }
    size_t size() const         { return idx; }
private:
    uint8_t buf[N];
    size_t idx = 0;

    Err encode_byte(uint8_t byte);
    Err encode_base(uint8_t start, uint64_t val, size_t ai_len, size_t add_len = 0);
    Err encode_int(MT mt, uint64_t val, size_t add_len = 0);
    Err encode_bytes(MT mt, const void *data, size_t len);
    Err encode_float(Prim type, Float val);
};

// SECTION: Private

template<size_t N>
Err Encoder<N>::encode_byte(uint8_t byte)
{
    if (idx >= N)
        return ERR_OUT_OF_MEM;
    buf[idx++] = byte;
    return NO_ERR;
}

template<size_t N>
Err Encoder<N>::encode_base(uint8_t start, uint64_t val, size_t ai_len, size_t add_len)
{
    if (idx + ai_len + add_len + 1 > N)
        return ERR_OUT_OF_MEM;

    buf[idx++] = start;
    for (int i = 8 * ai_len - 8; i >= 0; i -= 8)
        buf[idx++] = val >> i;

    return NO_ERR;
}

template<size_t N>
Err Encoder<N>::encode_int(MT mt, uint64_t val, size_t add_len)
{
    uint8_t ai;

    if (val <= AI_0)
        ai = val;
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
Err Encoder<N>::encode_bytes(MT mt, const void *data, size_t len)
{
    Err err = encode_int(mt, len, len);
    if (err == NO_ERR && data && len) {
        memcpy(&buf[idx], data, len);
        idx += len;
    }
    return err;
}

template<size_t N>
Err Encoder<N>::encode_float(Prim type, Float val)
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
#if (ZBOR_USE_FP16)
            uint16_t u16 = half_from_float(val.u32);
            Float tmp = half_to_float(u16);
            if (val.f32 != tmp.f32)
                return encode_base(MT_SIMPLE | PRIM_FLOAT_32, val.u32, 4);
            val.u16 = u16;
#else
            return encode_base(MT_SIMPLE | PRIM_FLOAT_32, val.u32, 4);
        if_nan: 
            return encode_base(MT_SIMPLE | PRIM_FLOAT_32, 0x7fc00000, 4);
#endif
        }
#if (ZBOR_USE_FP16)
        case PRIM_FLOAT_16:
            if ((val.u16 & 0x7fff) > 0x7c00)
                goto if_nan;
            return encode_base(MT_SIMPLE | PRIM_FLOAT_16, val.u16, 2);
        if_nan: 
            return encode_base(MT_SIMPLE | PRIM_FLOAT_16, 0x7e00, 2);
#endif
        default:
            return ERR_INVALID_FLOAT_TYPE;
    }
}

// !SECTION: Private

template<size_t N>
Err Encoder<N>::encode(CBOR *val)
{
    if (!val)
        return ERR_NULLPTR;

    switch (val->type) {
        case TYPE_UINT:     return encode(val->uint);
        case TYPE_SINT:     return encode(val->sint);
        case TYPE_DATA:     return encode(val->str.dat, val->str.len);
        case TYPE_TEXT:     return encode(val->str.txt, val->str.len);
        case TYPE_ARRAY:    return encode(val->arr);
        case TYPE_MAP:      return encode(val->map);
        case TYPE_TAG:      return encode(val->tag);
        case TYPE_PRIM:     return encode(val->prim);
        case TYPE_DOUBLE:   return encode(val->dbl);
        default: return ERR_INVALID_TYPE;
    }
}

template<size_t N>
Err Encoder<N>::encode(int val)
{
    return encode(int64_t(val));
}

template<size_t N>
Err Encoder<N>::encode(uint64_t val)
{
    return encode_int(MT_UINT, val);
}

template<size_t N>
Err Encoder<N>::encode(int64_t val)
{
    uint64_t ui = val >> 63;
    return encode_int(MT(ui & 0x20), ui ^ val);
}

template<size_t N>
Err Encoder<N>::encode(const uint8_t *data, size_t len)
{
    return encode_bytes(MT_DATA, data, len);
}

template<size_t N>
Err Encoder<N>::encode(const char *text, size_t len)
{
    return encode_bytes(MT_TEXT, text, len);
}

template<size_t N>
Err Encoder<N>::encode(Array arr)
{
    Err err = encode_int(MT_ARRAY, arr.size());
    if (err != NO_ERR)
        return err;

    for (auto it : arr) {
        if ((err = encode(&it)) != NO_ERR)
            return err;
    }
    return NO_ERR;
}

template<size_t N>
Err Encoder<N>::encode(Map map)
{
    Err err = encode_int(MT_MAP, map.size());
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
Err Encoder<N>::encode(Tag tag)
{
    Err err = encode_int(MT_TAG, tag.val, 0);
    if (err != NO_ERR)
        return err; 
    return encode(tag.content);
}

template<size_t N>
Err Encoder<N>::encode(Prim val)
{
    if (val >= 24 && val <= 31)
        return ERR_INVALID_SIMPLE;
    return encode_int(MT_SIMPLE, val);
}

template<size_t N>
Err Encoder<N>::encode(bool val)
{
    return encode(val ? PRIM_TRUE : PRIM_FALSE);
}

template<size_t N>
Err Encoder<N>::encode(float val)
{
    return encode_float(PRIM_FLOAT_32, val);
}

template<size_t N>
Err Encoder<N>::encode(double val)
{
    return encode_float(PRIM_FLOAT_64, val);
}

template<size_t N>
Err Encoder<N>::encode_start_indef(MT mt)
{
    if (mt != MT_MAP    &&
        mt != MT_ARRAY  &&
        mt != MT_TEXT   &&
        mt != MT_DATA)
        return ERR_INVALID_TYPE;
    return encode_byte(mt | AI_INDEF);
}

template<size_t N>
Err Encoder<N>::encode_break()
{
    return encode_byte(0xff);
}

}

#endif