#ifndef ZBOR_BASE_H
#define ZBOR_BASE_H

#include "utl/bit.h"
#include "utl/float.h"
#include <span>

namespace zbor {

using byte = uint8_t;

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
struct seq_t {
    seq_t(const byte* p) : head{p} {}
    seq_t(const byte* p, size_t len) : head{p}, tail{p + len} {}
    seq_iter begin() const;
    seq_iter end() const;
    size_t size() const             { return tail - head; }
    auto data() const               { return head; }
    void set_end(const byte* end)   { tail = end; }
protected:
    const byte* head;
    const byte* tail;
};

/**
 * @brief Sequence wrapper for indefinite byte and text strings.
 * 
 */
struct istring_t : seq_t {
    istring_t(const byte* head) : seq_t{head} {}
};

/**
 * @brief Sequence wrapper for generic array.
 * 
 */
struct array_t : seq_t {
    array_t(const byte* head, size_t len) : seq_t{head}, len{len} {}
    size_t size() const { return len; }
    bool indef() const  { return len == size_t(-1); } 
protected:
    size_t len;
};

/**
 * @brief Sequence wrapper for generic map.
 * 
 */
struct map_t : array_t {
    map_t(const byte* head, size_t len) : array_t{head, len} {}
    map_iter begin() const;
    map_iter end() const;
};

/**
 * @brief tag_t with number (stored) and content (decoded on-the-fly).
 * 
 */
struct tag_t : seq_t {
    tag_t(const byte* head, uint64_t number) : seq_t{head}, number{number} {}
    uint64_t num() const    { return number; }
    obj_t content() const;
private:
    uint64_t number;
};

/**
 * @brief Generic CBOR object which can hold any type.
 * 
 */
struct obj_t {

    obj_t() : uint{} {}

    bool valid() const { return type != type_invalid; }

    union {
        uint64_t uint;
        int64_t sint;
        std::string_view text;
        std::span<const byte> data;
        istring_t istr;
        array_t arr;
        map_t map;
        tag_t tag;
        prim_t prim;
        double dbl;
    };
    type_t type = type_invalid;
};

}

#endif