#ifndef ZBOR_BASE_H
#define ZBOR_BASE_H

#include <cstddef>
#include <cstdint>

namespace zbor {

// enum MT {
//     MT_UINT     = 0 << 5,
//     MT_NINT     = 1 << 5,
//     MT_DATA     = 2 << 5,
//     MT_TEXT     = 3 << 5,
//     MT_ARRAY    = 4 << 5,
//     MT_MAP      = 5 << 5,
//     MT_TAG      = 6 << 5,
//     MT_SIMPLE   = 7 << 5,
// };

// enum AI {
//     AI_0        = 23,
//     AI_1        = 24,
//     AI_2        = 25,
//     AI_4        = 26,
//     AI_8        = 27,
//     AI_INDEF    = 31,
// };

/**
 * @brief Enum for primitive (simple) values. Includes FLOAT 
 * markers which are used during encoding.
 * 
 */
enum Prim {
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
    TYPE_INVALID,
};

/**
 * @brief General zbor errors.
 * 
 */
enum Err {
    NO_ERR,
    ERR_NULLPTR,
    ERR_EMPTY,
    ERR_NOT_FOUND,
    ERR_NO_VALUE_FOR_KEY,
//     ERR_INVALID_PARAM,
//     ERR_INVALID_DATA,
//     ERR_INVALID_TYPE,
//     ERR_INVALID_SIMPLE,
//     ERR_INVALID_FLOAT_TYPE,
//     ERR_OUT_OF_MEM,
//     ERR_OUT_OF_DATA,
};

// inline const char* err_str(Err err)
// {
//     constexpr const char *str[] = {
//         "NO_ERR",
//         "ERR_NULLPTR",
//         "ERR_INVALID_PARAM",
//         "ERR_INVALID_DATA",
//         "ERR_INVALID_TYPE",
//         "ERR_INVALID_SIMPLE",
//         "ERR_INVALID_FLOAT_TYPE",
//         "ERR_OUT_OF_MEM",
//         "ERR_OUT_OF_DATA",
//         "ERR_ALREADY_EMPTY",
//         "ERR_NOT_FOUND",
//         "ERR_NO_VALUE_FOR_KEY",
//     };
//     return str[err];
// }

struct CBOR;

// struct Tag {
//     uint64_t val;
//     CBOR *content;
// };

struct Pair {
    CBOR *key;
    CBOR *val;
};

/**
 * @brief Linked list iterator, used in Array and CBOR sequence.
 * 
 */
struct Iter {

    Iter(CBOR *p) : p(p) {}

    bool    operator!=(const Iter &other)   { return p != other.p; }
    CBOR&   operator*()                     { return *p; }
    void    operator++();
private:
    CBOR *p;
};

struct MapIter {

    MapIter(CBOR *p) : p(p) {}

    bool    operator!=(const MapIter &other)   { return p != other.p; }
    Pair    operator*();
    void    operator++();
private:
    CBOR *p;
};

/**
 * @brief Array for storing CBOR elements. Actually uses linked list
 * implementation under the hood, to efficiently add and remove elements.
 * Hence, provides range-based for loop support and doesn't provide operator[],
 * to not confuse users about its access time.
 * 
 */
struct Array {
    size_t size() const { return len; }
    Iter begin()        { return head; }
    Iter end()          { return NULL; }
    CBOR* front()       { return head; }
    CBOR* back()        { return tail; }
    Err push(CBOR *val);
    Err pop(CBOR *val);
protected:
    CBOR *head = nullptr;
    CBOR *tail = nullptr;
    size_t len = 0;
};

struct Map : Array {
    MapIter begin()    { return head; }
    MapIter end()      { return NULL; }
    CBOR* front() = delete;
    CBOR* back() = delete;
    Err push(CBOR *key, CBOR *val);
    Err pop(CBOR *key);
};

#define ZBOR_STRING true

#if ZBOR_STRING
struct String {
    String() = default;
    String(const uint8_t *dat, size_t len) : dat{dat}, len{len} {}
    String(const char *txt, size_t len) : txt{txt}, len{len} {}
    union {
        const uint8_t *dat;
        const char *txt;
    };
    size_t len;
};
#endif

struct CBOR {

    CBOR() {}
    CBOR(int val);
    CBOR(int64_t val);
    CBOR(uint64_t val);
    CBOR(const uint8_t *data, size_t len);
    CBOR(const char *text, size_t len);
    CBOR(Array arr);
    // CBOR(Map map);
    // CBOR(Tag tag);
    CBOR(Prim val);
    CBOR(bool val);
    CBOR(double val);

    // Iter begin()    { return this; }
    // Iter end()      { return NULL; }

    CBOR *next  = nullptr;
    CBOR *prev  = nullptr;
    Type type   = TYPE_INVALID;
    union {
        uint64_t uint;
        int64_t sint;
#if ZBOR_STRING
        String str;
#else
        const uint8_t *data;
        const char *text;
#endif
        Array arr;
        // Map map;
        // Tag tag;
        Prim prim;
        double dbl;
    };
#if !ZBOR_STRING
    size_t len  = 0;
#endif
};

/**
 * @brief Object pool for CBOR elements with static allocation.
 * Zero-size partial specialization is reserved for dynamic 
 * allocation (currently not implemented).
 * 
 * @tparam N Number of elements, 0 is reserved for dynamic allocation
 */
template<size_t N>
struct Pool {

    template<typename... Args>
    CBOR* make(Args... args)
    {
        return idx < N ? &(buf[idx++] = CBOR(args...)) : nullptr;
    }

    void free(CBOR *p)
    {
        if (p < buf || p >= buf + N || !idx || p == &buf[--idx])
            return;
        *p = buf[idx];
    }

    void clear() 
    { 
        idx = 0;
    }

    size_t size() const
    {
        return idx;
    }
private:
    CBOR    buf[N];
    size_t  idx = 0;
};

};

#endif