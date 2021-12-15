#ifndef ZBOR_BASE_H
#define ZBOR_BASE_H

#include <cstddef>
#include <cstdint>

namespace zbor {

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

enum Prim : uint8_t {
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
    TYPE_HALF,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_INVALID,
};

enum Err {
    NO_ERR,
    ERR_NULL_PTR,
    ERR_INVALID_PARAM,
    ERR_INVALID_DATA,
    ERR_INVALID_TYPE,
    ERR_OUT_OF_MEM,
    ERR_OUT_OF_DATA,
    ERR_ALREADY_EMPTY,
    ERR_NOT_FOUND,
    ERR_NO_VAL_FOR_KEY,
};

struct String {
    String(const uint8_t *data, size_t len) : data(data), len(len) {}
    String(const char *text, size_t len) : text(text), len(len) {}
    union {
        const uint8_t *data;
        const char *text;
    };
    size_t len;
};

struct CBOR;

struct Tag {
    uint64_t val;
    CBOR *content;
};

struct Pair {
    CBOR *key;
    CBOR *val;
};

struct iter {

    iter(CBOR *p) : p(p) {}

    bool    operator!=(const iter &other)   { return p != other.p; }
    CBOR&   operator*()                     { return *p; }
    void    operator++();
private:
    CBOR *p;
};

struct map_iter {

    map_iter(CBOR *p) : p(p) {}

    bool    operator!=(const map_iter &other)   { return p != other.p; }
    Pair    operator*();
    void    operator++();
private:
    CBOR *p;
};

struct List {

    void clear()
    {
        head = NULL;
        tail = NULL;
        len = 0;
    }

    CBOR *head = NULL;
    CBOR *tail = NULL;
    size_t len = 0;
};

struct Array : List {

    iter begin()    { return iter(head); }
    iter end()      { return iter(NULL); }

    Err push(CBOR *val);
    Err pop(CBOR *val);
};

struct Map : List {

    map_iter begin()    { return map_iter(head); }
    map_iter end()      { return map_iter(NULL); }

    Err push(CBOR *key, CBOR *val);
    Err pop(CBOR *key);
};

struct CBOR {

    CBOR() {}
    CBOR(int val);
    CBOR(int64_t val);
    CBOR(uint64_t val);
    CBOR(const uint8_t *data, size_t len);
    CBOR(const char *text, size_t len);
    CBOR(Array arr);
    CBOR(Map map);
    CBOR(Tag tag);
    CBOR(Prim val);
    CBOR(bool val);
    CBOR(float val);
    CBOR(double val);

    iter begin()    { return iter(this); }
    iter end()      { return iter(NULL); }

    Type type = TYPE_INVALID;
    union {
        uint64_t uint;
        int64_t sint;
        String str;
        Array arr;
        Map map;
        Tag tag;
        Prim prim;
        float f;
        double d;
    };
    CBOR *next = NULL;
    CBOR *prev = NULL;
};

template<size_t N>
struct Pool {

    template<typename... Args>
    CBOR* make(Args... args)
    {
        return cnt < N ? &(buf[cnt++] = CBOR(args...)) : nullptr;
    }

    void free(CBOR *p)
    {
        if (p <   buf       || 
            p >=  buf + N   ||
            p == &buf[--cnt])
            return;
        *p = buf[cnt];
    }

    void clear() 
    { 
        cnt = 0;
    }

    size_t size() const
    {
        return cnt;
    }
private:
    CBOR    buf[N];
    size_t  cnt = 0;
};

};

#endif