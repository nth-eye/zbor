#ifndef ZBOR_BASE_H
#define ZBOR_BASE_H

#define ZBOR_SEQ_SPAN           false

#include "utl/bit.h"
#include "utl/float.h"
#include <span>

namespace zbor {

using byte = uint8_t;

struct seq_iter;
struct map_iter;
struct Obj;

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

#if (ZBOR_SEQ_SPAN)

struct seq : std::span<const byte> {
    seq(const byte* p, size_t len) : std::span<const byte>{p, len} {}
    seq_iter begin() const;
    seq_iter end() const;
    void set_end(const byte* end)   { *this = {data(), size_t(end - data())}; }
};

struct IndefString : seq {
    IndefString(const byte* head) : seq{head, 0} {}
};

struct Arr : seq {
    Arr(const byte* head, size_t len) : seq{head, 0}, len{len} {}
    size_t size() const { return len; }
    bool indef() const  { return len == size_t(-1); } 
protected:
    size_t len;
};

struct Map : Arr {
    Map(const byte* head, size_t len) : Arr{head, len} {}
    map_iter begin() const;
    map_iter end() const;
};

struct Tag : seq {
    Tag(const byte* head, uint64_t number) : seq{head, 0}, number{number} {}
    uint64_t num() const    { return number; }
    Obj content() const;
private:
    uint64_t number;
};

#else

/**
 * @brief CBOR sequence, read-only wrapper for traversal on-the-fly.
 * 
 */
struct seq {
    seq(const byte* p) : head{p} {}
    seq(const byte* p, size_t len) : head{p}, tail{p + len} {}
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
struct IndefString : seq {
    IndefString(const byte* head) : seq{head} {}
};

/**
 * @brief Sequence wrapper for generic array.
 * 
 */
struct Arr : seq {
    Arr(const byte* head, size_t len) : seq{head}, len{len} {}
    size_t size() const { return len; }
    bool indef() const  { return len == size_t(-1); } 
protected:
    size_t len;
};

/**
 * @brief Sequence wrapper for generic map.
 * 
 */
struct Map : Arr {
    Map(const byte* head, size_t len) : Arr{head, len} {}
    map_iter begin() const;
    map_iter end() const;
};

/**
 * @brief Tag with number (stored) and content (decoded on-the-fly).
 * 
 */
struct Tag : seq {
    Tag(const byte* head, uint64_t number) : seq{head}, number{number} {}
    uint64_t num() const    { return number; }
    Obj content() const;
private:
    uint64_t number;
};
#endif

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
        std::string_view text;
        std::span<const byte> data;
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