#ifndef ZBOR_BASE_H
#define ZBOR_BASE_H

#include <cstdint>
#include <cstddef>
#include <span>

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
    err_invalid_simple,
    err_invalid_indef_mt,
    err_invalid_indef_item,
    err_reserved_ai,
    err_break_without_start,
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