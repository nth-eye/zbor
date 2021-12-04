#ifndef CBOR_TYPES_H
#define CBOR_TYPES_H

#include <cstddef>
#include <cstdint>

namespace cbor {

using half = uint16_t;

enum MT {
    MT_UINT     = 0 << 5,
    MT_NINT     = 1 << 5,
    MT_DATA     = 2 << 5,
    MT_TEXT     = 3 << 5,
    MT_ARRAY    = 4 << 5,
    MT_MAP      = 5 << 5,
    MT_TAG      = 6 << 5,
    MT_SIMPLE   = 7 << 5,
};

enum AI {
    AI_0        = 23,
    AI_1        = 24,
    AI_2        = 25,
    AI_4        = 26,
    AI_8        = 27,
    AI_INDEF    = 31,
};

enum Prim {
    PRIM_FALSE      = 20,
    PRIM_TRUE       = 21,
    PRIM_NULL       = 22,
    PRIM_UNDEFINED  = 23,
    PRIM_FLOAT_16   = 25,
    PRIM_FLOAT_32   = 26,
    PRIM_FLOAT_64   = 27,
};

enum Type {
    TYPE_UINT,
    TYPE_SINT,
    TYPE_DATA,
    TYPE_TEXT,
    TYPE_ARRAY,
    TYPE_MAP,
    TYPE_TAG,
    TYPE_SIMPLE,
    TYPE_FLOAT_16,
    TYPE_FLOAT_32,
    TYPE_FLOAT_64,
    TYPE_INVALID,
};

enum Err {
    NO_ERR,
    ERR_NULL_PTR,
    ERR_INVALID_PARAM,
    ERR_INVALID_DATA,
    ERR_OUT_OF_MEM,
    ERR_OUT_OF_DATA,
    ERR_ALREADY_EMPTY,
    ERR_NOT_FOUND,
    ERR_NO_VAL_FOR_KEY,
};

union Float {
    uint16_t    u16;
    half        f16;
    uint32_t    u32;
    float       f32;
    uint64_t    u64;
    double      f64;
};

struct Bytes {
    union {
        const uint8_t *data;
        const char *text;
    };
    size_t len;
};

struct CBOR;

struct Array {

    struct iter {

        iter(CBOR *p) : p(p) {}

        void    operator++();
        bool    operator!=(const iter &other)   { return p != other.p; }
        CBOR&   operator*()                     { return *p; }
    private:
        CBOR *p;
    };

    iter begin()    { return iter(head); }
    iter end()      { return iter(NULL); }

    void    reset();
    Err     push(CBOR *val);
    Err     pop(CBOR *val);

    CBOR *head;
    CBOR *tail;
    size_t len;
};

struct Map {

    struct iter {

        iter(CBOR *p) : p(p) {}

        void    operator++();
        bool    operator!=(const iter &other)   { return p != other.p; }
        CBOR&   operator*()                     { return *p; }
    private:
        CBOR *p;
    };

    iter begin()    { return iter(head); }
    iter end()      { return iter(NULL); }

    void    reset();
    Err     insert(CBOR *key, CBOR *val);
    Err     erase(CBOR *key);

    CBOR *head;
    CBOR *tail;
    size_t len;
};

};

#endif