#ifndef ZBOR_BASE_H
#define ZBOR_BASE_H

#include "utl/float.h"
#include <span>
#include <string_view>
#include <cstring>

namespace zbor {

using byte = uint8_t;
using span_t = std::span<const byte>;

/**
 * @brief 
 * 
 */
struct text_t : std::basic_string_view<byte> {
    using base = std::basic_string_view<byte>;
    using base::base;
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
struct obj_t;

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
 * markers which are used during encoding.
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
enum err_t {
    err_ok,
    err_no_memory,
    err_out_of_bounds,
    err_invalid_simple,
    err_invalid_float_type,
    err_invalid_indef_mt,
    err_invalid_indef_item,
    err_reserved_ai,
    err_break_without_start,
};

/**
 * @brief CBOR sequence, read-only wrapper for traversal on-the-fly.
 * 
 */
struct seq_t : span_t {
    using span_t::span_t;
    constexpr seq_iter begin() const;
    constexpr seq_iter end() const;
};

/**
 * @brief Sequence wrapper for indefinite byte and text strings.
 * 
 */
struct istr_r : seq_t {
    using seq_t::seq_t;
};

/**
 * @brief Sequence wrapper for CBOR array.
 * 
 */
struct arr_t : seq_t {
    constexpr arr_t(const byte* head, const byte* tail, size_t len) : seq_t{head, tail}, len{len} {}
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
struct tag_t : seq_t {
    constexpr tag_t(const byte* head, const byte* tail, uint64_t number) : seq_t{head, tail}, number{number} {}
    constexpr uint64_t num() const  { return number; }
    constexpr obj_t content() const;
private:
    uint64_t number;
};

/**
 * @brief Generic CBOR object which can hold any type.
 * 
 */
struct obj_t {
    constexpr obj_t() : uint{}      {}
    constexpr bool valid() const    { return type != type_invalid; }
    type_t type = type_invalid;
    union {
        uint64_t uint;
        int64_t sint;
        text_t text;
        span_t data;
        istr_r istr;
        arr_t arr;
        map_t map;
        tag_t tag;
        prim_t prim;
        double dbl;
    };
};

}

#endif