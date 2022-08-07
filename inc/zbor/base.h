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
    MT_UINT     = 0 << 5,
    MT_NINT     = 1 << 5,
    MT_DATA     = 2 << 5,
    MT_TEXT     = 3 << 5,
    MT_ARRAY    = 4 << 5,
    MT_MAP      = 5 << 5,
    MT_TAG      = 6 << 5,
    MT_SIMPLE   = 7 << 5,
};

/**
 * @brief CBOR additional info (5 bits).
 * 
 */
enum Ai {
    AI_0        = 23,
    AI_1        = 24,
    AI_2        = 25,
    AI_4        = 26,
    AI_8        = 27,
    AI_INDEF    = 31,
};

/**
 * @brief Enum for primitive (simple) values. Includes FLOAT 
 * markers which are used during encoding.
 * 
 */
enum Prim : byte {
    PRIM_FALSE      = 20,
    PRIM_TRUE       = 21,
    PRIM_NULL       = 22,
    PRIM_UNDEFINED  = 23,
    PRIM_FLOAT_16   = 25,
    PRIM_FLOAT_32   = 26,
    PRIM_FLOAT_64   = 27,
};

/**
 * @brief Type for CBOR objects (not major type).
 * 
 */
enum Type {
    TYPE_UINT,
    TYPE_SINT,
    TYPE_DATA,
    TYPE_TEXT,
    TYPE_ARRAY,
    TYPE_MAP,
    TYPE_TAG,
    TYPE_PRIM,
    TYPE_DOUBLE,
    TYPE_INDEF_DATA,
    TYPE_INDEF_TEXT,
    TYPE_INVALID,
};

/**
 * @brief General codec errors.
 * 
 */
enum Err {
    ERR_OK,
    ERR_NO_MEMORY,
    ERR_OUT_OF_BOUNDS,
    ERR_INVALID_SIMPLE,
    ERR_INVALID_FLOAT_TYPE,
    ERR_INVALID_INDEF_MT,
    ERR_INVALID_INDEF_ITEM,
    ERR_RESERVED_AI,
    ERR_BREAK_WITHOUT_START,
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

    bool valid() const { return type != TYPE_INVALID; }

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
    Type type = TYPE_INVALID;
};

}

#endif