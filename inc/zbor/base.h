#ifndef ZBOR_BASE_H
#define ZBOR_BASE_H

#include "utl/bit.h"
#include "utl/float.h"

namespace zbor {

using byte = uint8_t;

struct SeqIter;
struct MapIter;
struct Obj;

/**
 * @brief CBOR major type (3 bits).
 * 
 */
enum Mt {
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
enum Ai {
    ai_0        = 23,
    ai_1        = 24,
    ai_2        = 25,
    ai_4        = 26,
    ai_8        = 27,
    ai_indef    = 31,
};

/**
 * @brief Enum for primitive (simple) values. Includes FLOAT 
 * markers which are used during encoding.
 * 
 */
enum Prim : byte {
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
enum Type {
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
enum Err {
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
 * @brief String wrapper for length and data / text pointers.
 * 
 */
struct String {
    union {
        const byte *dat;
        const char *txt;
    };
    size_t len;
};

/**
 * @brief Begin and end wrapper for other CBOR iterable types.
 * 
 */
struct Range {
    Range(const byte *head) : head{head} {}
    SeqIter begin() const;
    SeqIter end() const;
    void set_end(const byte *end)   { tail = end; }
protected:
    const byte *head;
    const byte *tail;
};

/**
 * @brief Range wrapper for indefinite byte and text strings.
 * 
 */
struct IndefString : Range {
    IndefString(const byte *head) : Range{head} {}
};

/**
 * @brief Range wrapper for generic array.
 * 
 */
struct Arr : Range {
    Arr(const byte *head, size_t len) : Range{head}, len{len} {}
    size_t size() const { return len; }
    bool indef() const  { return len == size_t(-1); } 
protected:
    size_t len;
};

/**
 * @brief Range wrapper for generic map.
 * 
 */
struct Map : Arr {
    Map(const byte *head, size_t len) : Arr{head, len} {}
    MapIter begin() const;
    MapIter end() const;
};

/**
 * @brief Tag with number (stored) and content (decoded on-the-fly).
 * 
 */
struct Tag : Range {
    Tag(const byte *head, uint64_t number) : Range{head}, number{number} {}
    uint64_t num() const    { return number; }
    Obj content() const;
private:
    uint64_t number;
};

/**
 * @brief Generic CBOR object which can hold any type.
 * 
 */
struct Obj {

    Obj() : uint{} {}

    bool valid() const { return type != type_invalid; }

    union {
        uint64_t uint;
        int64_t sint;
        String str;
        IndefString istr;
        Arr arr;
        Map map;
        Tag tag;
        Prim prim;
        double dbl;
    };
    Type type = type_invalid;
};

}

#endif