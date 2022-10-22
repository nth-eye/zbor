#ifndef ZBOR_ENC_H
#define ZBOR_ENC_H

#include "zbor/dec.h"
#include <algorithm>

namespace zbor {
namespace enc {

enum arr : size_t   {};
enum map : size_t   {};
enum tag : uint64_t {};
struct indef_dat    {};
struct indef_txt    {};
struct indef_arr    {};
struct indef_map    {};
struct breaker      {};
template<size_t N>
struct txt { 
    byte buf[N - 1]{};
    constexpr txt(const char (&str)[N]) { std::copy_n(str, N - 1, buf); }
    constexpr auto data() const { return &buf[0]; }
    constexpr auto size() const { return N - 1; }
};
template<class T>
concept boolean = std::is_same<T, bool>::value;

/**
 * @brief CBOR base codec implementation with CRTP interface.
 * 
 * @tparam T User class
 */
template<class T>
struct interface {

    // ANCHOR Helpers

    constexpr operator seq() const                      { return {buf(), buf() + idx()}; }
    constexpr seq_iter begin() const                    { return {buf(), buf() + idx()}; }
    constexpr seq_iter end() const                      { return {}; }
    constexpr const byte& operator[](size_t i) const    { return buf()[i]; }
    constexpr byte& operator[](size_t i)                { return buf()[i]; }
    constexpr const byte* data() const                  { return buf(); }
    constexpr byte* data()                              { return buf(); }
    constexpr size_t capacity() const                   { return max(); }
    constexpr size_t size() const                       { return idx(); }
    constexpr size_t resize(size_t len)                 { return len <= max() ? idx() = len : idx(); }    
    constexpr void clear()                              { idx() = 0; }

    // ANCHOR Variadic interface

    constexpr err encode_(auto... args)
    {
        err e = err_ok;
        ((e = encode(args)) || ...);
        return e;
    }

    // ANCHOR Implicit interface

    constexpr err encode(unsigned val)
    { 
        return encode_uint(val);
    }
    constexpr err encode(int val)
    { 
        return encode_sint(val);
    }
    constexpr err encode(uint64_t val)
    { 
        return encode_uint(val);
    }
    constexpr err encode(int64_t val)
    {
        return encode_sint(val);
    }
    constexpr err encode(prim val)
    {
        return encode_prim(val);
    }
    constexpr err encode(boolean auto val)
    { 
        return encode_bool(val); 
    }
    constexpr err encode(float val)
    { 
        return encode_float(val);
    }
    constexpr err encode(double val)
    { 
        return encode_double(val);
    }
    constexpr err encode(span val)
    { 
        return encode_data(val); 
    }
    constexpr err encode(list val)
    { 
        return encode_data(val); 
    }
    err encode(std::string_view val)
    { 
        return encode_text(val);
    }
    err encode(const char* val)
    { 
        return encode_text(val); 
    }
    template<size_t N>
    constexpr err encode(const enc::txt<N>& val)
    {
        return encode_text(val); 
    }
    constexpr err encode(enc::arr val)
    {
        return encode_arr(val);
    }
    constexpr err encode(enc::map val)
    {
        return encode_map(val);
    }
    constexpr err encode(enc::tag val)
    {
        return encode_tag(val);
    }
    constexpr err encode(enc::indef_dat)
    {
        return encode_indef_dat();
    }
    constexpr err encode(enc::indef_txt)
    {
        return encode_indef_txt();
    }
    constexpr err encode(enc::indef_arr)
    {
        return encode_indef_arr();
    }
    constexpr err encode(enc::indef_map)
    {
        return encode_indef_map();
    }
    constexpr err encode(enc::breaker)
    {
        return encode_break();
    }

    // ANCHOR: Explicit interface

    constexpr err encode_uint(uint64_t val)
    {
        return encode_head(mt_uint, val);
    }
    constexpr err encode_sint(int64_t val)
    {
        uint64_t ui = val >> 63;
        return encode_head(mt_t(ui & 0x20), ui ^ val);
    }
    constexpr err encode_prim(prim val)
    {
        return val < 24 || val > 31 ? encode_head(mt_simple, val) : err_invalid_simple;
    }
    constexpr err encode_bool(bool val)
    {
        return encode_byte(mt_simple | (prim_false + val));
    }
    constexpr err encode_float(float val)
    {
        if (val != val)
            return encode_nan();

        auto u32 = std::bit_cast<uint32_t>(val);
        auto u16 = utl::float_to_half(u32);

        if (val != std::bit_cast<float>(utl::half_to_float(u16)))
            return encode_base(mt_simple | byte(prim_float_32), u32, 4);
        
        if ((u16 & 0x7fff) <= 0x7c00)
            return encode_base(mt_simple | byte(prim_float_16), u16, 2);

        return encode_nan();
    }
    constexpr err encode_double(double val)
    {
        if (val != val)
            return encode_nan();
        if (val == float(val))
            return encode_float(val);
        return encode_base(mt_simple | byte(prim_float_64), std::bit_cast<uint64_t>(val), 8);
    }
    constexpr err encode_data(span val)
    { 
        return encode_string(mt_data, val.data(), val.size()); 
    }
    constexpr err encode_data(list val)
    { 
        return encode_string(mt_data, val.begin(), val.size());  
    }
    constexpr err encode_text(span val)
    {
        return encode_string(mt_text, val.data(), val.size());
    }
    constexpr err encode_text(list val)
    { 
        return encode_string(mt_text, val.begin(), val.size()); 
    }
    err encode_text(std::string_view val)
    { 
        return encode_string(mt_text, val.data(), val.size());
    }
    err encode_text(const char* val)
    { 
        return encode_string(mt_text, val, strlen(val)); 
    }
    template<size_t N>
    constexpr err encode_text(const enc::txt<N>& val)
    {
        return encode_string(mt_text, val.data(), val.size());
    }
    constexpr err encode_arr(size_t size)
    { 
        return encode_head(mt_array, size);
    }
    constexpr err encode_map(size_t size)
    { 
        return encode_head(mt_map, size);
    }
    constexpr err encode_tag(uint64_t val)
    {
        return encode_head(mt_tag, val);
    }
    constexpr err encode_indef_dat()
    { 
        return encode_byte(mt_data | byte(ai_indef));
    }
    constexpr err encode_indef_txt()
    { 
        return encode_byte(mt_text | byte(ai_indef));
    }
    constexpr err encode_indef_arr()
    {
        return encode_byte(mt_array | byte(ai_indef));
    }
    constexpr err encode_indef_map()
    {
        return encode_byte(mt_map | byte(ai_indef));
    }
    constexpr err encode_break()
    {
        return encode_byte(0xff); 
    }
private:
    constexpr err encode_nan()
    { 
        return encode_base(mt_simple | byte(prim_float_16), 0x7e00, 2); 
    }
    constexpr err encode_byte(byte b)
    { 
        return idx() < max() ? buf()[idx()++] = b, err_ok : err_no_memory; 
    }
    constexpr err encode_base(byte start, uint64_t val, size_t ai_len, size_t add_len = 0)
    {
        if (idx() + ai_len + add_len + 1 > max())
            return err_no_memory;
        buf()[idx()++] = start;
        for (int i = 8 * ai_len - 8; i >= 0; i -= 8)
            buf()[idx()++] = val >> i;
        return err_ok;
    }
    constexpr err encode_head(mt_t mt, uint64_t val, size_t add_len = 0)
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
    constexpr err encode_string(mt_t mt, const void* data, size_t len)
    {
        err e = encode_head(mt, len, len);
        if (e == err_ok && len) {
            std::copy_n(static_cast<pointer>(data), len, buf() + idx());
            idx() += len;
        }
        return e;
    }
private:
    constexpr auto buf() const  { return static_cast<const T*>(this)->buf; }
    constexpr auto buf()        { return static_cast<T*>(this)->buf; }
    constexpr auto max() const  { return static_cast<const T*>(this)->max; }
    constexpr auto& idx()       { return static_cast<T*>(this)->idx; }
    constexpr auto& idx() const { return static_cast<const T*>(this)->idx; }
};

/**
 * @brief Same as zbor::ref but const.
 * 
 */
struct cref : enc::interface<cref> {
    friend enc::interface<cref>;
    constexpr cref() = delete;
    constexpr cref(std::span<const byte> buf, const size_t& len) : idx{len}, max{buf.size()}, buf{buf.data()} {}
private:
    const size_t& idx;
    const size_t max;
    const byte* const buf;
};

}

/**
 * @brief Reference to CBOR codec with actual storage, either zbor::view 
 * or zbor::codec<>. Use it to pass those around to common non-templated
 * functions to read and/or modify source. Unlike zbor::view, stores a 
 * reference to both memory and current size of a storage.
 * 
 */
struct ref : enc::interface<ref> {
    friend enc::interface<ref>;
    constexpr ref() = delete;
    constexpr ref(std::span<byte> buf, size_t& len) : idx{len}, max{buf.size()}, buf{buf.data()} {}
private:
    size_t& idx;
    const size_t max;
    byte* const buf;
};

/**
 * @brief Same as zbor::ref but const.
 * 
 */
using cref = const enc::cref;

/**
 * @brief CBOR codec with external storage. Basically a view which allows 
 * to use writable referenced memory with codec interface. Unlike zbor::ref, 
 * stores reference only to memory, while current size is part of an instance. 
 * So passing it by value into a function and modifying there, will modify 
 * memory content but leave size unchanged.
 * 
 */
struct view : enc::interface<view> {
    friend enc::interface<view>;
    constexpr operator ref()        { return {{buf, max}, idx}; }
    constexpr operator cref() const { return {{buf, max}, idx}; }
    constexpr view() = delete;
    constexpr view(std::span<byte> buf, size_t len = 0) : idx{len}, max{buf.size()}, buf{buf.data()} {}
private:
    size_t idx;
    const size_t max;
    byte* const buf;
};

/**
 * @brief CBOR codec with internal storage. Similar to zbor::view, but 
 * doesn't have overhead of an additional pointer to external storage, 
 * while still uses same CRTP codec interface. Like zbor::view, it can 
 * be cheaply passed around as zbor::ref. 
 * 
 * @tparam N Buffer size in bytes
 */
template<size_t N>
struct codec : enc::interface<codec<N>> {
    friend enc::interface<codec<N>>;
    constexpr operator ref()            { return {{buf}, idx}; }
    constexpr operator cref() const     { return {{buf}, idx}; }
    static constexpr size_t capacity()  { return N; }
private:
    size_t idx = 0;
    static constexpr size_t max = N;
    byte buf[N]{};
};

namespace literals {

constexpr auto operator"" _arr(unsigned long long x)    { return enc::arr(x); }
constexpr auto operator"" _map(unsigned long long x)    { return enc::map(x); }
constexpr auto operator"" _tag(unsigned long long x)    { return enc::tag(x); }
constexpr auto operator"" _prim(unsigned long long x)   { return prim(x); }
template<enc::txt T>
constexpr auto operator"" _txt()                        { return T; }

}

inline constexpr enc::indef_dat indef_dat;
inline constexpr enc::indef_txt indef_txt;
inline constexpr enc::indef_arr indef_arr;
inline constexpr enc::indef_map indef_map;
inline constexpr enc::breaker breaker;

}

#endif