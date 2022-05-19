#ifndef ZBOR_CODEC_H
#define ZBOR_CODEC_H

#include "zbor/decode.h"

namespace zbor {

/**
 * @brief CBOR buffer, basicaly codec with given external storage.
 * 
 */
struct Buf {

    Buf() = delete;
    Buf(byte *buf, size_t max) : buf{buf}, max{max} {}
    
    Err encode(int val);
    Err encode(unsigned val);
    Err encode(int64_t val);
    Err encode(uint64_t val);
    Err encode(const byte *data, size_t len);
    Err encode(const char *text, size_t len);
    Err encode(const char *text);
    Err encode(Prim val);
    Err encode(bool val);
    Err encode(float val);
    Err encode(double val);

    Err encode_indef_dat();
    Err encode_indef_txt();
    Err encode_indef_arr();
    Err encode_indef_map();
    Err encode_break();
    Err encode_arr(size_t size);
    Err encode_map(size_t size);
    Err encode_tag(uint64_t val);

    operator Seq() const                    { return {buf, idx}; }
    SeqIter begin() const                   { return {buf, buf + idx}; }
    SeqIter end() const                     { return {}; }
    const byte& operator[](size_t i) const  { return buf[i]; }
    const byte* data() const                { return buf; }
    byte* data()                            { return buf; }
    size_t capacity() const                 { return max; }
    size_t size() const                     { return idx; }
    size_t resize(size_t size)              { return size <= max ? idx = size : idx; }    
    void clear()                            { idx = 0; }
private:
    byte *buf;
    size_t max;
    size_t idx = 0;

    Err encode_byte(byte b);
    Err encode_base(byte start, uint64_t val, size_t ai_len, size_t add_len = 0);
    Err encode_int(Mt mt, uint64_t val, size_t add_len = 0);
    Err encode_bytes(Mt mt, const void *data, size_t len);
    Err encode_float(Prim type, Float val);
};

/**
 * @brief Wrapper for Buf wit internal storage.
 * 
 * @tparam N Buffer size in bytes
 */
template<size_t N>
struct Codec : Buf {
    static_assert(N, "codec size must be > 0");
    Codec() : Buf{buf, N} {}
private:
    byte buf[N];
};

// SECTION: Private

inline Err Buf::encode_byte(byte b)
{
    return idx < max ? buf[idx++] = b, ERR_OK : ERR_NO_MEMORY;
}

inline Err Buf::encode_base(byte start, uint64_t val, size_t ai_len, size_t add_len)
{
    if (idx + ai_len + add_len + 1 > max)
        return ERR_NO_MEMORY;

    buf[idx++] = start;
    for (int i = 8 * ai_len - 8; i >= 0; i -= 8)
        buf[idx++] = val >> i;

    return ERR_OK;
}

inline Err Buf::encode_int(Mt mt, uint64_t val, size_t add_len)
{
    byte ai;

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

inline Err Buf::encode_bytes(Mt mt, const void *data, size_t len)
{
    Err err = encode_int(mt, len, len);
    if (err == ERR_OK && data && len) {
        memcpy(&buf[idx], data, len);
        idx += len;
    }
    return err;
}

inline Err Buf::encode_float(Prim type, Float val)
{
    switch (type) 
    {
    case PRIM_FLOAT_64:
    {
        if (val.f64 != val.f64)
            goto if_nan;

        float f32 = val.f64;
        if (val.f64 != f32) // Else we can fallthrough to FLOAT_32
            return encode_base(MT_SIMPLE | byte(PRIM_FLOAT_64), val.u64, 8);
        val.f32 = f32;
        [[fallthrough]];
    }
    case PRIM_FLOAT_32:
    {
        if (val.f32 != val.f32)
            goto if_nan;

        uint16_t u16 = half_from_float(val.u32);
        if (val.f32 != Float{half_to_float(u16)}.f32) // Else we can fallthrough to FLOAT_16
            return encode_base(MT_SIMPLE | byte(PRIM_FLOAT_32), val.u32, 4);
        val.u16 = u16;
        [[fallthrough]];
    }
    case PRIM_FLOAT_16:
        if ((val.u16 & 0x7fff) > 0x7c00)
            goto if_nan;
        return encode_base(MT_SIMPLE | byte(PRIM_FLOAT_16), val.u16, 2);
    if_nan: 
        return encode_base(MT_SIMPLE | byte(PRIM_FLOAT_16), 0x7e00, 2);

    default:
        return ERR_INVALID_FLOAT_TYPE;
    }
}

// !SECTION: Private

inline Err Buf::encode(int val)
{
    return encode(int64_t(val));
}

inline Err Buf::encode(unsigned val)
{
    return encode(uint64_t(val));
}

inline Err Buf::encode(uint64_t val)
{
    return encode_int(MT_UINT, val);
}

inline Err Buf::encode(int64_t val)
{
    uint64_t ui = val >> 63;
    return encode_int(Mt(ui & 0x20), ui ^ val);
}

inline Err Buf::encode(const byte *data, size_t len)
{
    return encode_bytes(MT_DATA, data, len);
}

inline Err Buf::encode(const char *text, size_t len)
{
    return encode_bytes(MT_TEXT, text, len);
}

inline Err Buf::encode(const char *text)
{
    return encode_bytes(MT_TEXT, text, strlen(text));
}

inline Err Buf::encode(Prim val)
{
    if (val >= 24 && val <= 31)
        return ERR_INVALID_SIMPLE;
    return encode_int(MT_SIMPLE, val);
}

inline Err Buf::encode(bool val)
{
    return encode_byte(MT_SIMPLE | (PRIM_FALSE + val));
}

inline Err Buf::encode(float val)
{
    return encode_float(PRIM_FLOAT_32, val);
}

inline Err Buf::encode(double val)
{
    return encode_float(PRIM_FLOAT_64, val);
}

inline Err Buf::encode_indef_dat()
{
    return encode_byte(MT_DATA | byte(AI_INDEF));
}

inline Err Buf::encode_indef_txt()
{
    return encode_byte(MT_TEXT | byte(AI_INDEF));
}

inline Err Buf::encode_indef_arr()
{
    return encode_byte(MT_ARRAY | byte(AI_INDEF));
}

inline Err Buf::encode_indef_map()
{
    return encode_byte(MT_MAP | byte(AI_INDEF));
}

inline Err Buf::encode_break()
{
    return encode_byte(0xff);
}

inline Err Buf::encode_arr(size_t size)
{
    return encode_int(MT_ARRAY, size);
}

inline Err Buf::encode_map(size_t size)
{
    return encode_int(MT_MAP, size);
}

inline Err Buf::encode_tag(uint64_t val)
{
    return encode_int(MT_TAG, val);
}

}

#endif