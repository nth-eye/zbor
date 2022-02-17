#ifndef ZBOR_BASE_H
#define ZBOR_BASE_H

#include "zbor_util.h"

namespace zbor {

struct CBOR;

/**
 * @brief CBOR major type (3 bits).
 * 
 */
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

/**
 * @brief CBOR additional info (5 bits).
 * 
 */
enum AI {
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
    TYPE_DATA_CHUNKS,
    TYPE_TEXT_CHUNKS,
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
    ERR_INVALID_PARAM,
    ERR_INVALID_DATA,
    ERR_INVALID_TYPE,
    ERR_INVALID_SIMPLE,
    ERR_INVALID_FLOAT_TYPE,
    ERR_OUT_OF_MEM,
    ERR_OUT_OF_DATA,
    ERR_DEPTH_EXCEEDED,
};

/**
 * @brief Pair of pointers to CBOR key and value.
 * 
 */
struct Pair {
    CBOR *key;
    CBOR *val;
};

/**
 * @brief String wrapper for length and data / text pointers.
 * 
 */
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

/**
 * @brief Linked list iterator, used in Array and Sequence.
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

/**
 * @brief Iterator for map. Basically same as Iter, but 
 * returns Pair and is incremented two times in a row.
 * 
 */
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
 * implementation under the hood, to efficiently add and remove elements 
 * on the fly and to not rely on memory allocation. Provides range-based 
 * for loop support and doesn't provide operator[], to not confuse users 
 * about its access time.
 * 
 */
struct Array {
    void init()         { head = tail = 0; len = 0; }
    size_t size() const { return len; }
    CBOR* front()       { return head; }
    CBOR* back()        { return tail; }
    Iter begin()        { return head; }
    Iter end()          { return NULL; }
    const Iter begin() const    { return head; }
    const Iter end() const      { return NULL; }
    Err push(CBOR *val);
    Err pop(CBOR *val);
protected:
    CBOR *head = nullptr;
    CBOR *tail = nullptr;
    size_t len = 0;
};

/**
 * @brief Map for storing CBOR key-value pairs. Same as Array, but
 * uses MapIter and doesn't have front() and back().
 * 
 */
struct Map : Array {
    size_t size() const { return len >> 1; }
    MapIter begin()     { return head; }
    MapIter end()       { return NULL; }
    const MapIter begin() const { return head; }
    const MapIter end() const   { return NULL; }
    CBOR* front() = delete;
    CBOR* back() = delete;
    Err push(CBOR *key, CBOR *val);
    Err pop(CBOR *key);
};

/**
 * @brief Strong type to provide CBOR object constructor for 
 * chunks of data and text.
 * 
 */
template<Type T>
struct Chunks : Array {
    static_assert(T == TYPE_DATA || T == TYPE_TEXT, "only data or text");
};

/**
 * @brief CBOR tag, stores integer tag value and pointer 
 * to CBOR content object. 
 * 
 */
struct Tag {
    uint64_t val;
    CBOR *content;
};

/**
 * @brief Used as decoding result, which provides all necessary
 * info, including fail reason. Has begin and end iterators.
 * 
 */
struct Sequence {
    Iter begin()    { return root; }
    Iter end()      { return NULL; }
    CBOR *root;
    size_t size;
    Err err;
};

/**
 * @brief General CBOR element, which can store any type. 
 * Includes appropriate constructors.
 * 
 */
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
    CBOR(double val);
    CBOR(Chunks<TYPE_DATA> val);
    CBOR(Chunks<TYPE_TEXT> val);

    CBOR *next  = nullptr;
    CBOR *prev  = nullptr;
    Type type   = TYPE_INVALID;
    union {
        uint64_t uint;
        int64_t sint;
        String str;
        Array arr;
        Map map;
        Tag tag;
        Prim prim;
        double dbl;
        Chunks<TYPE_DATA> chunk_dat;
        Chunks<TYPE_TEXT> chunk_txt;
    };
};

/**
 * @brief Object pool for CBOR elements with static allocation.
 * 
 * @tparam N Number of elements
 */
template<size_t N>
using Pool = StaticPool<CBOR, N>;

};

#endif
