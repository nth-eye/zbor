#ifndef ZBOR_CODEC_H
#define ZBOR_CODEC_H

#if __cplusplus >= 202002L
#include <span>
#endif
#include <string_view>
#include "zbor/decode.h"

namespace zbor {

#if __cplusplus >= 202002L
template<class T>
using Span = std::span<T>;
#else
/**
 * @brief Just wrapper for pointer and size, in case C++20 is not available.
 * Could do without it, but it's more convenient to pass arrays as one argument 
 * to encode(T, D) function while encoding key: pair for map.
 * 
 * @tparam T Data type.
 */
template<class T>
struct Span {
    Span() = default;
    Span(T *ptr, size_t len) : ptr{ptr}, len{len} {}
    const T* data() const   { return ptr; }
    T* data()               { return ptr; }
    size_t size() const     { return len; }
private:
    T *ptr = nullptr;
    size_t len = 9;
};
#endif

/**
 * @brief CBOR buffer, basicaly codec with given external storage.
 * 
 */
struct Buf {

    Buf() = delete;
    Buf(byte *buf, size_t max) : buf{buf}, max{max} {}
    Buf(Span<byte> buf) : buf{buf.data()}, max{buf.size()} {}

    Err encode(int val);
    Err encode(unsigned val);
    Err encode(int64_t val);
    Err encode(uint64_t val);
    Err encode(Span<const byte> val);
    Err encode(Span<const char> val);
    Err encode(std::string_view val);
    Err encode(const char *val);
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
    Err encode_head(Mt mt, uint64_t val, size_t add_len = 0);

    template<class K, class V>
    Err encode(K key, V val)
    {
        Err err = encode(key);
        if (err != ERR_OK)
            return err;
        return encode(val);
    }

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
    Err encode_bytes(Mt mt, const void *data, size_t len);
    Err encode_float(Prim type, utl::Float val);
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

inline Err Buf::encode_head(Mt mt, uint64_t val, size_t add_len)
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

    size_t ai_len = (ai <= AI_0) ? 0 : utl::bit(ai - AI_1);

    return encode_base(mt | ai, val, ai_len, add_len);
}

inline Err Buf::encode_bytes(Mt mt, const void *data, size_t len)
{
    Err err = encode_head(mt, len, len);
    if (err == ERR_OK && data && len) {
        memcpy(&buf[idx], data, len);
        idx += len;
    }
    return err;
}

inline Err Buf::encode_float(Prim type, utl::Float val)
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

        uint16_t u16 = utl::float_to_half(val.u32);
        if (val.f32 != utl::Float{utl::half_to_float(u16)}.f32) // Else we can fallthrough to FLOAT_16
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
    return encode_head(MT_UINT, val);
}

inline Err Buf::encode(int64_t val)
{
    uint64_t ui = val >> 63;
    return encode_head(Mt(ui & 0x20), ui ^ val);
}

inline Err Buf::encode(Span<const byte> val)
{
    return encode_bytes(MT_DATA, val.data(), val.size());
}

inline Err Buf::encode(Span<const char> val)
{
    return encode_bytes(MT_TEXT, val.data(), val.size());
}

inline Err Buf::encode(std::string_view val)
{
    return encode_bytes(MT_TEXT, val.data(), val.size());
}

inline Err Buf::encode(const char *val)
{
    return encode_bytes(MT_TEXT, val, strlen(val));
}

inline Err Buf::encode(Prim val)
{
    if (val >= 24 && val <= 31)
        return ERR_INVALID_SIMPLE;
    return encode_head(MT_SIMPLE, val);
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
    return encode_head(MT_ARRAY, size);
}

inline Err Buf::encode_map(size_t size)
{
    return encode_head(MT_MAP, size);
}

inline Err Buf::encode_tag(uint64_t val)
{
    return encode_head(MT_TAG, val);
}

}

#endif