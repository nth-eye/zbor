#ifndef ZBOR_BASE_H
#define ZBOR_BASE_H

#include "zbor/util.h"
#include <cstring>

namespace zbor {

struct Obj;

/**
 * @brief Obj major type (3 bits).
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
 * @brief Obj additional info (5 bits).
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
 * @brief Type for Obj objects (not major type).
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
    ERR_OK,
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
 * @brief Pair of pointers to Obj key and value.
 * 
 */
struct Pair {
    Obj *key;
    Obj *val;
};

/**
 * @brief String wrapper for length and data / text pointers.
 * 
 */
struct String {
    String() = default;
    String(const uint8_t *dat, size_t len) : dat{dat}, len{len} {}
    String(const char *txt, size_t len) : txt{txt}, len{len} {}
    String(const char *txt) : txt{txt}, len{strlen(txt)} {}
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

    Iter(Obj *p) : p(p) {}

    bool    operator!=(const Iter &other) const { return p != other.p; }
    bool    operator==(const Iter &other) const { return !operator!=(other); }
    Obj*    operator*()                         { return p; }
    Iter&   operator++();
private:
    Obj *p;
};

/**
 * @brief Iterator for map. Basically same as Iter, but 
 * returns Pair and is incremented two times in a row.
 * 
 */
struct MapIter {

    MapIter(Obj *p) : p{p} {}

    bool     operator!=(const MapIter &other) const  { return p != other.p; }
    bool     operator==(const MapIter &other) const  { return !operator!=(other);  }
    Pair     operator*();
    MapIter& operator++();
private:
    Obj *p;
};

/**
 * @brief Array for storing Obj elements. Actually uses linked list
 * implementation under the hood, to efficiently add and remove elements 
 * on the fly and to not rely on memory allocation. Provides range-based 
 * for loop support and doesn't provide operator[], to not confuse users 
 * about its access time.
 * 
 */
struct Array {
    void init()         { head = tail = 0; len = 0; }
    size_t size() const { return len; }
    Obj* front()        { return head; }
    Obj* back()         { return tail; }
    Iter begin()        { return head; }
    Iter end()          { return NULL; }
    const Iter begin() const    { return head; }
    const Iter end() const      { return NULL; }
    Err push(Obj *val);
    Err pop(Obj *val);
protected:
    Obj *head = nullptr;
    Obj *tail = nullptr;
    size_t len = 0;
};

/**
 * @brief Map for storing Obj key-value pairs. Same as Array, but
 * uses MapIter and doesn't have front() and back().
 * 
 */
struct Map : Array {
    size_t size() const { return len >> 1; }
    MapIter begin()     { return head; }
    MapIter end()       { return NULL; }
    const MapIter begin() const { return head; }
    const MapIter end() const   { return NULL; }
    Obj* front() = delete;
    Obj* back() = delete;
    Err push(Obj *key, Obj *val);
    Err pop(Obj *key);
};

/**
 * @brief Strong type to provide Obj object constructor for 
 * chunks of data and text.
 * 
 */
template<Type T>
struct Chunks : Array {
    static_assert(T == TYPE_DATA || T == TYPE_TEXT, "only data or text");
    Err push(Obj *val);
};
using ChunkData = Chunks<TYPE_DATA>;
using ChunkText = Chunks<TYPE_TEXT>;

/**
 * @brief Obj tag, stores integer tag value and pointer 
 * to Obj content object. 
 * 
 */
struct Tag {
    uint64_t val;
    Obj *content;
};

/**
 * @brief Used as decoding result, which provides all necessary
 * info, including fail reason. Has begin and end iterators.
 * 
 */
struct Sequence {
    Iter begin()    { return root; }
    Iter end()      { return NULL; }
    Obj *root;
    size_t size;
    Err err;
};

/**
 * @brief General Obj element, which can store any type. 
 * Includes appropriate constructors.
 * 
 */
struct Obj {

    Obj() {}
    Obj(int val);
    Obj(int64_t val);
    Obj(uint64_t val);
    Obj(const uint8_t *data, size_t len);
    Obj(const char *text, size_t len);
    Obj(const char *text);
    Obj(Array arr);
    Obj(Map map);
    Obj(Tag tag);
    Obj(Prim val);
    Obj(bool val);
    Obj(double val);
    Obj(ChunkData val);
    Obj(ChunkText val);

    Obj *next = nullptr;
    Obj *prev = nullptr;
    Type type = TYPE_INVALID;
    union {
        uint64_t uint;
        int64_t sint;
        String str;
        Array arr;
        Map map;
        Tag tag;
        Prim prim;
        double dbl;
        ChunkData chunk_dat;
        ChunkText chunk_txt;
    };
};

/**
 * @brief Object pool for Obj elements with static allocation.
 * 
 * @tparam N Number of elements
 */
template<size_t N>
using Pool = StaticPool<Obj, N>;

inline Obj::Obj(int val) : Obj(int64_t(val))
{}

inline Obj::Obj(int64_t val)
{
    if (val < 0) {
        type = TYPE_SINT;
        sint = val;
    } else {
        type = TYPE_UINT;
        uint = val;
    }
}

inline Obj::Obj(uint64_t val) : type{TYPE_UINT}, uint{val}
{}

inline Obj::Obj(const uint8_t *data, size_t len) : type{TYPE_DATA}, str{data, len}
{}

inline Obj::Obj(const char *text, size_t len) : type{TYPE_TEXT}, str{text, len}
{}

inline Obj::Obj(const char *text) : type{TYPE_TEXT}, str{text}
{}

inline Obj::Obj(Array arr) : type{TYPE_ARRAY}, arr{arr}
{}

inline Obj::Obj(Map map) : type{TYPE_MAP}, map{map}
{}

inline Obj::Obj(Tag tag) : type{TYPE_TAG}, tag{tag}
{}

inline Obj::Obj(Prim val) 
{
    if (val < 24 || (val > 31 && val < 256)) {
        type = TYPE_PRIM;
        prim = val;
    }
}

inline Obj::Obj(bool val) : Obj(val ? PRIM_TRUE : PRIM_FALSE)
{}

inline Obj::Obj(double val) : type{TYPE_DOUBLE}, dbl{val}
{}

inline Obj::Obj(ChunkData val) : type{TYPE_DATA_CHUNKS}, chunk_dat{val}
{}

inline Obj::Obj(ChunkText val) : type{TYPE_TEXT_CHUNKS}, chunk_txt{val}
{}

inline Iter& Iter::operator++()
{ 
    p = p->next;
    return *this;
}

inline MapIter& MapIter::operator++()
{
    p = p->next ? p->next->next : nullptr;
    return *this;
}

inline Pair MapIter::operator*()
{
    return {p, p ? p->next : nullptr};
}

inline Err Array::push(Obj *val)
{
    if (!val)
        return ERR_NULLPTR;
    
    val->prev = tail;

    if (tail)
        tail->next = val;
    else
        head = val;

    tail = val;
    tail->next = nullptr;

    ++len;

    return ERR_OK;
}

inline Err Array::pop(Obj *val)
{
    if (!val)
        return ERR_NULLPTR;

    if (!len)
        return ERR_EMPTY;

    for (auto it = head; it;) {

        auto next = it->next;
        auto prev = it->prev;

        if (it == val) {

            --len;

            if (prev)
                prev->next = next;
            else
                head = next;

            if (next)
                next->prev = prev;
            else
                tail = prev;

            return ERR_OK;
        }
        it = next;
    }
    return ERR_NOT_FOUND;
}

inline Err Map::push(Obj *key, Obj *val)
{
    if (!key || !val)
        return ERR_NULLPTR;

    key->next = val;
    key->prev = tail;
    val->prev = key;

    if (tail)
        tail->next = key;
    else
        head = key;

    tail = val;
    tail->next = nullptr;
    
    len += 2;

    return ERR_OK;
}

inline Err Map::pop(Obj *key)
{
    if (!key)
        return ERR_NULLPTR;

    if (!len)
        return ERR_EMPTY;

    for (auto it = head; it;) {

        if (!it->next)
            return ERR_NO_VALUE_FOR_KEY;

        auto next = it->next->next;
        auto prev = it->prev;

        if (it == key) {

            len -= 2;

            if (prev)
                prev->next = next;
            else
                head = next;

            if (next)
                next->prev = prev;
            else
                tail = prev;

            return ERR_OK;
        }
        it = next;
    }
    return ERR_NOT_FOUND;
}

template<Type T>
Err Chunks<T>::push(Obj *val)
{
    if (val && val->type != T)
        return ERR_INVALID_TYPE;
    return Array::push(val);
}

};

#endif
