#ifndef ZBOR_DECODE_H
#define ZBOR_DECODE_H

#include "utl/float.h"
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <span>
#include <tuple>
#include <string_view>

namespace zbor {

using byte = uint8_t;
using span = std::span<const byte>;
using list = std::initializer_list<byte>;
using pointer = const byte*;

struct seq_iter;
struct map_iter;
struct item;

/**
 * @brief CBOR major type (3 bits).
 * 
 */
enum mt_t {
    mt_uint     = 0 << 5,
    mt_nint     = 1 << 5,
    mt_data     = 2 << 5,
    mt_text     = 3 << 5,
    mt_array    = 4 << 5,
    mt_map      = 5 << 5,
    mt_tag      = 6 << 5,
    mt_simple   = 7 << 5,
};

/**
 * @brief CBOR additional info (5 bits).
 * 
 */
enum ai_t {
    ai_0        = 23,
    ai_1        = 24,
    ai_2        = 25,
    ai_4        = 26,
    ai_8        = 27,
    ai_indef    = 31,
};

/**
 * @brief Enum for primitive (simple) values. Includes float 
 * markers which are used during en/decoding.
 * 
 */
enum prim : byte {
    prim_false      = 20,
    prim_true       = 21,
    prim_null       = 22,
    prim_undefined  = 23,
    prim_float_16   = 25,
    prim_float_32   = 26,
    prim_float_64   = 27,
};

/**
 * @brief Type for CBOR objects (not major type).
 * 
 */
enum type_t {
    type_uint,
    type_sint,
    type_data,
    type_text,
    type_array,
    type_map,
    type_tag,
    type_prim,
    type_floating,
    type_indef_data,
    type_indef_text,
    type_invalid,
};

/**
 * @brief General codec errors.
 * 
 */
enum err {
    err_ok,
    err_no_memory,
    err_out_of_bounds,
    err_reserved_ai,
    err_invalid_break,
    err_invalid_simple,
    err_invalid_indef_mt,
    err_invalid_indef_string,
};

/**
 * @brief CBOR sequence, read-only wrapper for traversal on-the-fly.
 * 
 */
struct seq : span {
    using span::span;
    constexpr seq_iter begin() const;
    constexpr seq_iter end() const;
};

namespace dec {

/**
 * @brief String view with <zbor::byte> as underlying character type. 
 * May be expanded with additional "text"-type specific functionality, 
 * e.g comparison with std::string_view. This workwaround is necessary 
 * to make decode() constexpr, because reinterpret_cast is not allowed 
 * and it's not possible to create regular std::string_view from <byte*> 
 * during parsing. 
 * 
 */
struct txt : std::basic_string_view<byte> {
    using base = std::basic_string_view<byte>;
    using base::base;
    friend constexpr bool operator==(const txt& lhs, const std::string_view& rhs)
    {
        if (lhs.size() != rhs.size())
            return false;
        if (std::is_constant_evaluated()) {
            for (size_t i = 0; i < lhs.size(); ++i) {
                if (lhs[i] != rhs[i])
                    return false;
            }
            return true;
        }
        return !memcmp(lhs.data(), rhs.data(), lhs.size());
    }
    friend constexpr bool operator==(const std::string_view& lhs, const txt& rhs)
    {
        return operator==(rhs, lhs);
    }
};

/**
 * @brief Sequence wrapper for CBOR indefinite byte and text strings.
 * 
 */
struct istr : seq {
    using seq::seq;
};

/**
 * @brief Sequence wrapper for CBOR array.
 * 
 */
struct arr : seq {
    constexpr arr(pointer head, pointer tail, size_t len) : seq{head, tail}, len{len} {}
    constexpr auto size() const     { return len; }
    constexpr bool indef() const    { return len == size_t(-1); } 
private:
    size_t len;
};

/**
 * @brief Sequence wrapper for CBOR map.
 * 
 */
struct map : arr {
    using arr::arr;
    constexpr map_iter begin() const;
    constexpr map_iter end() const;
};

/**
 * @brief CBOR tag with number (stored) and content (decoded on-the-fly).
 * 
 */
struct tag : seq {
    constexpr tag(pointer head, pointer tail, uint64_t number) : seq{head, tail}, number{number} {}
    constexpr uint64_t num() const { return number; }
    constexpr item content() const;
private:
    uint64_t number;
};

constexpr std::tuple<err, uint64_t, pointer> ai_check(byte ai, pointer p, const pointer end)
{
    switch (ai) 
    {
    case ai_1:
    case ai_2:
    case ai_4:
    case ai_8: {
        size_t len = utl::bit(ai - ai_1);
        if (p + len > end)
            return {err_out_of_bounds, 0, p};
        uint64_t val = 0;
        for (int i = 8 * len - 8; i >= 0; i -= 8)
            val |= uint64_t(*p++) << i;
        return {err_ok, val, p};
    }
    break;
    case 28:
    case 29:
    case 30: return {err_reserved_ai, 0, p};
    }
    return {err_ok, ai, p};
}

}

/**
 * @brief Generic decoded CBOR object which can hold any type.
 * 
 */
struct item {
    constexpr item(type_t t = type_invalid) : type{t} {}
    constexpr bool valid() const { return type != type_invalid; }
    type_t type;
    union {
        uint64_t uint;
        int64_t sint;
        prim prim;
        span data;
        dec::txt text;
        dec::istr istr;
        dec::arr arr;
        dec::map map;
        dec::tag tag;
        double fp;
    };
};

/**
 * @brief Decode next adjacent CBOR item. Almost all validity checks always performed 
 * throughout decoding process: out-of-bounds, reserved AI, invalid indef MT, nested 
 * indefinite strings, break without start. What isn't checked is number of elements 
 * within nested containers (if at least one of them is indefinite). For example, 0x9f82ff 
 * will be first parsed as valid indefinite array, and then, only if user starts to traverse 
 * over its elements, decoding of malformed nested array will report err_out_of_bounds.
 * 
 * @param p Begin pointer, must be valid pointer
 * @param end End pointer, must be valid pointer
 * @return Tuple with decoded object, error status and pointer past last character interpreted
 */
constexpr std::tuple<item, err, pointer> decode(pointer p, const pointer end)
{
    if (p >= end)
        return {{}, err_out_of_bounds, end};

    byte mt             = *p   & 0xe0;
    byte ai             = *p++ & 0x1f;
    item obj            = type_t(mt >> 5);
    uint64_t val        = ai;
    uint64_t size       = 0;
    size_t nest         = 0;
    size_t skip         = 0;
    decltype(p) head    = nullptr;
    err e;

    if (ai == ai_indef) {
        switch (mt)
        {
        case mt_data: obj.type = type_indef_data; break;
        case mt_text: obj.type = type_indef_text; break;
        case mt_array:
        case mt_map: size = size_t(-1); break;
        case mt_simple: return {{}, err_invalid_break, p};
        default: return {{}, err_invalid_indef_mt, p};
        }
        head = p;
        nest = 1;
    } else {
        std::tie(e, val, p) = dec::ai_check(ai, p, end);

        if (e != err_ok)
            return {{}, e, p};

        switch (mt) 
        {
        case mt_uint: obj.uint = val; break;
        case mt_nint: obj.sint = ~val; break;
        case mt_data:
            obj.data = {p, size_t(val)};
            p += val;
        break;
        case mt_text:
            obj.text = {p, size_t(val)};
            p += val;
        break;
        case mt_map:
            head = p;
            size = val;
            skip = val << 1;
        break;
        case mt_array:
            head = p;
            size = val;
            skip = val;
        break;
        case mt_tag:
            head = p;
            size = val;
            skip = 1;
        break;
        case mt_simple:
            switch (ai) 
            {
            case prim_float_16:
                obj.fp      = std::bit_cast<float>(utl::half_to_float(val));
                obj.type    = type_floating;
            break;
            case prim_float_32:
                obj.fp      = std::bit_cast<float>(uint32_t(val));
                obj.type    = type_floating;
            break;
            case prim_float_64:
                obj.fp      = std::bit_cast<double>(val);
                obj.type    = type_floating;
            break;
            default:
                obj.prim    = prim(val);
            break;
            }
        break;
        }
    }

    if (obj.type == type_indef_data || 
        obj.type == type_indef_text) 
    {
        while (true) {
            
            if (p >= end)
                return {{}, err_out_of_bounds, end};

            if (*p == 0xff) {
                ++p;
                break;
            }
            val = *p & 0x1f;

            if (obj.type != ((*p++ & 0xe0) >> 5) + 7 || val == ai_indef)
                return {{}, err_invalid_indef_string, p};

            std::tie(e, val, p) = dec::ai_check(val, p, end);

            if (e != err_ok)
                return {{}, e, p};

            p += val;
        }
    } else {
        while (skip || nest) {

            if (p >= end)
                return {{}, err_out_of_bounds, end};

            mt  = *p   & 0xe0;
            val = *p++ & 0x1f;

            if (val == ai_indef) {
                switch (mt) {
                case mt_data:
                case mt_text:
                case mt_array:
                case mt_map: ++nest; 
                break;
                case mt_simple:
                    if (!nest)
                        return {{}, err_invalid_break, p};
                    --nest;
                break;
                default: return {{}, err_invalid_indef_mt, p};
                }
            } else {
                std::tie(e, val, p) = dec::ai_check(val, p, end);

                if (e != err_ok) 
                    return {{}, e, p};
                    
                switch (mt) {
                case mt_data:
                case mt_text:   p += val; break;
                case mt_array:  if (!nest) skip += val;         break;
                case mt_map:    if (!nest) skip += val << 1;    break;
                case mt_tag:    if (!nest) skip += 1;           break;
                }
            }
            if (skip && !nest)
                skip--;
        }
    }
    switch (obj.type) 
    {
    case type_array:        obj.arr  = {head, p, size}; break;
    case type_map:          obj.map  = {head, p, size}; break;
    case type_tag:          obj.tag  = {head, p, size}; break;
    case type_indef_data:   obj.istr = {head, p}; break;
    case type_indef_text:   obj.istr = {head, p}; break;
    default:;
    }
    return {obj, err_ok, p};
}

/**
 * @brief Sequence iterator which holds range (begin and end pointers). Used 
 * to traverse CBOR sequence (RFC-8742), which is just series of adjacent 
 * objects. Used to traverse arr_t, istr_t or any series of bytes as item 
 * one by one. Only exception is map_t.
 * 
 */
struct seq_iter {
    constexpr seq_iter() = default;
    constexpr seq_iter(pointer head, pointer tail) : head{head}, tail{tail}
    {
        step(key);
    }
    constexpr bool operator!=(const seq_iter&) const 
    { 
        return key.valid();
    }
    constexpr auto& operator*() const 
    { 
        return key; 
    }
    constexpr auto& operator++()
    {
        step(key);
        return *this;
    }
    constexpr auto operator++(int) 
    { 
        auto tmp = *this; 
        ++(*this); 
        return tmp; 
    }
protected:
    constexpr void step(item& o) 
    {
        std::tie(o, std::ignore, head) = decode(head, tail); 
    }
protected:
    pointer head = nullptr;
    pointer tail = nullptr;
    item key;
};

/**
 * @brief Map iterator, same as seq_iter, but parses two objects in a 
 * row and returns them as pair.
 * 
 */
struct map_iter : seq_iter {
    constexpr map_iter() = default;
    constexpr map_iter(pointer head, pointer tail) : seq_iter{head, tail}
    {
        if (key.valid()) 
            step(val);
    }
    constexpr bool operator!=(const map_iter&) const 
    { 
        return key.valid() && val.valid();
    }
    constexpr auto operator*() const 
    { 
        return std::pair<const item&, const item&>{key, val}; 
    }
    constexpr auto& operator++()
    {
        step(key);
        if (key.valid()) 
            step(val);
        return *this;
    }
    constexpr auto operator++(int) 
    { 
        auto tmp = *this; 
        ++(*this); 
        return tmp; 
    }
private:
    item val;
};

constexpr seq_iter seq::begin() const       { return {data(), data() + size()}; }
constexpr seq_iter seq::end() const         { return {}; }
constexpr map_iter dec::map::begin() const  { return {data(), data() + seq::size()}; }
constexpr map_iter dec::map::end() const    { return {}; }
constexpr item dec::tag::content() const    { return std::get<item>(decode(data(), data() + size())); }

/**
 * @brief Get human readable name for type enum.
 * 
 * @param t Type enumeration
 * @return String with type name 
 */
constexpr auto str_type(type_t t)
{
    switch (t) {
        case type_uint: return "unsigned";
        case type_sint: return "negative";
        case type_data: return "data";
        case type_text: return "text";
        case type_array: return "array";
        case type_map: return "map";
        case type_tag: return "tag";
        case type_prim: return "simple";
        case type_floating: return "float";
        case type_indef_data: return "indefinite data";
        case type_indef_text: return "indefinite text";
        case type_invalid: return "<invalid>";
        default: return "<unknown>";
    }
}

/**
 * @brief Get human readable name for error enum.
 * 
 * @param e Error enumeration
 * @return String with error name
 */
constexpr auto str_err(err e)
{
    switch (e) {
        case err_ok: return "ok";
        case err_no_memory: return "no_memory";
        case err_out_of_bounds: return "out_of_bounds";
        case err_reserved_ai: return "reserved_ai";
        case err_invalid_break: return "invalid_break";
        case err_invalid_simple: return "invalid_simple";
        case err_invalid_indef_mt: return "invalid_indef_mt";
        case err_invalid_indef_string: return "invalid_indef_string";
        default: return "<unknown>";
    }
}

}

#endif