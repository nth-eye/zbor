#ifndef ZBOR_CODEC_H
#define ZBOR_CODEC_H

#include <span>
#include <string_view>
#include <cstring>
#include "zbor/decode.h"

namespace zbor {

/**
 * @brief CBOR buffer, basicaly codec with given external storage.
 * 
 */
struct Buf {

    constexpr Buf() = delete;
    constexpr Buf(byte *buf, size_t max) : buf{buf}, max{max} {}
    constexpr Buf(std::span<byte> buf) : buf{buf.data()}, max{buf.size()} {}

    operator seq_t() const                    { return {buf, idx}; }
    seq_iter begin() const                  { return {buf, buf + idx}; }
    seq_iter end() const                    { return {}; }
    constexpr const byte& operator[](size_t i) const    { return buf[i]; }
    constexpr const byte* data() const                  { return buf; }
    constexpr byte* data()                              { return buf; }
    constexpr size_t capacity() const                   { return max; }
    constexpr size_t size() const                       { return idx; }
    constexpr size_t resize(size_t size)                { return size <= max ? idx = size : idx; }    
    constexpr void clear()                              { idx = 0; }

    err_t encode(int val);
    err_t encode(unsigned val);
    err_t encode(int64_t val);
    err_t encode(uint64_t val);
    err_t encode(std::span<const byte> val);
    err_t encode(std::string_view val);
    err_t encode(const char *val);
    err_t encode(prim_t val);
    err_t encode(bool val);
    err_t encode(float val);
    err_t encode(double val);

    err_t encode_indef_dat();
    err_t encode_indef_txt();
    err_t encode_indef_arr();
    err_t encode_indef_map();
    err_t encode_break();
    err_t encode_arr(size_t size);
    err_t encode_map(size_t size);
    err_t encode_tag(uint64_t val);
    err_t encode_head(mt_t mt, uint64_t val, size_t add_len = 0);

    template<class K, class V>
    err_t encode(K key, V val)
    {
        err_t err = encode(key);
        if (err != err_ok)
            return err;
        return encode(val);
    }
private:
    err_t encode_byte(byte b);
    err_t encode_base(byte start, uint64_t val, size_t ai_len, size_t add_len = 0);
    err_t encode_bytes(mt_t mt, const void* data, size_t len);
    err_t encode_float(prim_t type, utl::fp_bits val);
private:
    byte *buf;
    size_t max;
    size_t idx = 0;
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

inline err_t Buf::encode_byte(byte b)
{
    return idx < max ? buf[idx++] = b, err_ok : err_no_memory;
}

inline err_t Buf::encode_base(byte start, uint64_t val, size_t ai_len, size_t add_len)
{
    if (idx + ai_len + add_len + 1 > max)
        return err_no_memory;

    buf[idx++] = start;
    for (int i = 8 * ai_len - 8; i >= 0; i -= 8)
        buf[idx++] = val >> i;

    return err_ok;
}

inline err_t Buf::encode_head(mt_t mt, uint64_t val, size_t add_len)
{
    byte ai;

    if (val <= ai_0)
        ai = val;
    else if (val <= 0xff)
        ai = ai_1;
    else if (val <= 0xffff)
        ai = ai_2;
    else if (val <= 0xffffffff)
        ai = ai_4;
    else
        ai = ai_8;

    size_t ai_len = (ai <= ai_0) ? 0 : utl::bit(ai - ai_1);

    return encode_base(mt | ai, val, ai_len, add_len);
}

inline err_t Buf::encode_bytes(mt_t mt, const void* data, size_t len)
{
    err_t err = encode_head(mt, len, len);
    if (err == err_ok && data && len) {
        memcpy(&buf[idx], data, len);
        idx += len;
    }
    return err;
}

inline err_t Buf::encode_float(prim_t type, utl::fp_bits val)
{
    switch (type) 
    {
    case prim_float_64:
    {
        if (val.f64 != val.f64)
            goto if_nan;

        float f32 = val.f64;
        if (val.f64 != f32) // Else we can fallthrough to float_32
            return encode_base(mt_simple | byte(prim_float_64), val.u64, 8);
        val.f32 = f32;
        [[fallthrough]];
    }
    case prim_float_32:
    {
        if (val.f32 != val.f32)
            goto if_nan;

        uint16_t u16 = utl::float_to_half(val.u32);
        if (val.f32 != utl::fp_bits{utl::half_to_float(u16)}.f32) // Else we can fallthrough to float_16
            return encode_base(mt_simple | byte(prim_float_32), val.u32, 4);
        val.u16 = u16;
        [[fallthrough]];
    }
    case prim_float_16:
        if ((val.u16 & 0x7fff) > 0x7c00)
            goto if_nan;
        return encode_base(mt_simple | byte(prim_float_16), val.u16, 2);
    if_nan: 
        return encode_base(mt_simple | byte(prim_float_16), 0x7e00, 2);

    default:
        return err_invalid_float_type;
    }
}

// !SECTION: Private

inline err_t Buf::encode(int val)
{
    return encode(int64_t(val));
}

inline err_t Buf::encode(unsigned val)
{
    return encode(uint64_t(val));
}

inline err_t Buf::encode(uint64_t val)
{
    return encode_head(mt_uint, val);
}

inline err_t Buf::encode(int64_t val)
{
    uint64_t ui = val >> 63;
    return encode_head(mt_t(ui & 0x20), ui ^ val);
}

inline err_t Buf::encode(std::span<const byte> val)
{
    return encode_bytes(mt_data, val.data(), val.size());
}

inline err_t Buf::encode(std::string_view val)
{
    return encode_bytes(mt_text, val.data(), val.size());
}

inline err_t Buf::encode(const char *val)
{
    return encode_bytes(mt_text, val, strlen(val));
}

inline err_t Buf::encode(prim_t val)
{
    if (val >= 24 && val <= 31)
        return err_invalid_simple;
    return encode_head(mt_simple, val);
}

inline err_t Buf::encode(bool val)
{
    return encode_byte(mt_simple | (prim_false + val));
}

inline err_t Buf::encode(float val)
{
    return encode_float(prim_float_32, val);
}

inline err_t Buf::encode(double val)
{
    return encode_float(prim_float_64, val);
}

inline err_t Buf::encode_indef_dat()
{
    return encode_byte(mt_data | byte(ai_indef));
}

inline err_t Buf::encode_indef_txt()
{
    return encode_byte(mt_text | byte(ai_indef));
}

inline err_t Buf::encode_indef_arr()
{
    return encode_byte(mt_array | byte(ai_indef));
}

inline err_t Buf::encode_indef_map()
{
    return encode_byte(mt_map | byte(ai_indef));
}

inline err_t Buf::encode_break()
{
    return encode_byte(0xff);
}

inline err_t Buf::encode_arr(size_t size)
{
    return encode_head(mt_array, size);
}

inline err_t Buf::encode_map(size_t size)
{
    return encode_head(mt_map, size);
}

inline err_t Buf::encode_tag(uint64_t val)
{
    return encode_head(mt_tag, val);
}

}

#endif