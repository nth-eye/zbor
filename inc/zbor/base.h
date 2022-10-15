#ifndef ZBOR_BASE_H
#define ZBOR_BASE_H

#include <span>
#include <string_view>
#include <cstring>
#include "utl/str.h"

namespace zbor {

using byte = unsigned char;
using span_t = std::span<const byte>;

/**
 * @brief String view with <zbor::byte> as underlying character type. 
 * May be expanded with additional "text"-type specific functionality, 
 * e.g comparison with std::string_view. This workwaround is necessary 
 * to make decode() constexpr, because reinterpret_cast is not allowed 
 * and it's not possible to create regular std::string_view from <byte*> 
 * during parsing. 
 * 
 */
struct text_t : std::basic_string_view<byte> {
    using base = std::basic_string_view<byte>;
    using base::base;

    text_t(const char* str) noexcept
        : base{reinterpret_cast<const byte*>(str), strlen(str)} 
    {}

    friend constexpr bool operator==(const text_t& lhs, const std::string_view& rhs)
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
    friend constexpr bool operator==(const std::string_view& lhs, const text_t& rhs)
    {
        return operator==(rhs, lhs);
    }
};

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
enum prim_t : byte {
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
    type_double,
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
    err_invalid_simple,
    err_invalid_indef_mt,
    err_invalid_indef_item,
    err_reserved_ai,
    err_break_without_start,
};

/**
 * @brief Wrapper for uint64_t to allow encode() function overload
 * for CBOR tag type. Content must be encoded separately.
 * 
 */
struct tag_num {
    constexpr tag_num() = delete;
    constexpr tag_num(uint64_t num) : num{num} {}
    constexpr operator uint64_t() const { return num; }
private:
    uint64_t num;
};

/**
 * @brief CBOR sequence, read-only wrapper for traversal on-the-fly.
 * 
 */
struct seq : span_t {
    using span_t::span_t;
    constexpr seq_iter begin() const;
    constexpr seq_iter end() const;
};

/**
 * @brief Sequence wrapper for CBOR indefinite byte and text strings.
 * 
 */
struct istr_t : seq {
    using seq::seq;
};

/**
 * @brief Sequence wrapper for CBOR array.
 * 
 */
struct arr_t : seq {
    constexpr arr_t(const byte* head, const byte* tail, size_t len) : seq{head, tail}, len{len} {}
    constexpr auto size() const     { return len; }
    constexpr bool indef() const    { return len == size_t(-1); } 
private:
    size_t len;
};

/**
 * @brief Sequence wrapper for CBOR map.
 * 
 */
struct map_t : arr_t {
    using arr_t::arr_t;
    constexpr map_iter begin() const;
    constexpr map_iter end() const;
};

/**
 * @brief CBOR tag with number (stored) and content (decoded on-the-fly).
 * 
 */
struct tag_t : seq {
    constexpr tag_t(const byte* head, const byte* tail, uint64_t number) : seq{head, tail}, number{number} {}
    constexpr uint64_t num() const { return number; }
    constexpr item content() const;
private:
    uint64_t number;
};

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
        text_t text;
        span_t data;
        istr_t istr;
        arr_t arr;
        map_t map;
        tag_t tag;
        prim_t prim;
        double dbl;
    };
};

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
        case type_double: return "float";
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
        case err_invalid_simple: return "invalid_simple";
        case err_invalid_indef_mt: return "invalid_indef_mt";
        case err_invalid_indef_item: return "invalid_indef_item";
        case err_reserved_ai: return "reserved_ai";
        case err_break_without_start: return "break_without_start";
        default: return "<unknown>";
    }
}

}

#endif