#include "zbor_base.h"

namespace zbor {

CBOR::CBOR(int val) : CBOR(int64_t(val))
{}

CBOR::CBOR(int64_t val)
{
    if (val < 0) {
        type = TYPE_SINT;
        sint = val;
    } else {
        type = TYPE_UINT;
        uint = val;
    }
}

CBOR::CBOR(uint64_t val) : type(TYPE_UINT), uint(val)
{}

CBOR::CBOR(const uint8_t *data, size_t len) : type(TYPE_DATA), str{data, len}
{}

CBOR::CBOR(const char *text, size_t len) : type(TYPE_TEXT), str{text, len}
{}

CBOR::CBOR(Array arr) : type(TYPE_ARRAY), arr(arr)
{}

CBOR::CBOR(Map map) : type(TYPE_MAP), map(map)
{}

CBOR::CBOR(Tag tag) : type(TYPE_TAG), tag(tag)
{}

CBOR::CBOR(Prim val)
{
    if (val < 24 || val > 31) { // Otherwise invalid
        type = TYPE_SIMPLE;
        prim = val;
    }
}

CBOR::CBOR(bool val) : CBOR(val ? PRIM_TRUE : PRIM_FALSE)
{}

CBOR::CBOR(float val) : type(TYPE_FLOAT), f(val)
{}

CBOR::CBOR(double val) : type(TYPE_DOUBLE), d(val)
{}

// SECTION: Array and map

void iter::operator++()
{ 
    p = p->next; 
}

void map_iter::operator++()
{
    p = p->next->next;
}

Pair map_iter::operator*()
{
    return {p, p->next};
}

Err Array::push(CBOR *val)
{
    if (!val)
        return ERR_NULL_PTR;

    val->next = NULL;

    if (tail)
        tail->next = val;
    else
        head = val;

    tail = val;
    ++len;

    return NO_ERR;
}

Err Array::pop(CBOR *val)
{
    if (!val)
        return ERR_NULL_PTR;

    if (!len)
        return ERR_ALREADY_EMPTY;

    for (auto it = head; it;) {

        CBOR *next = it->next;
        CBOR *prev = it->prev;

        if (it == val) {
            if (prev)
                prev->next = next;
            if (next)
                next = NULL;
            --len;
            return NO_ERR;
        }
        it = it->next;
    }
    return ERR_NOT_FOUND;
}

Err Map::push(CBOR *key, CBOR *val)
{
    if (!key || !val)
        return ERR_NULL_PTR;

    key->next = val;
    val->next = NULL;

    if (tail)
        tail->next = key;
    else
        head = key;

    tail = val;
    ++len;

    return NO_ERR;
}

Err Map::pop(CBOR *key)
{
    if (!key)
        return ERR_NULL_PTR;

    if (!len)
        return ERR_ALREADY_EMPTY;

    for (auto it = head; it;) {

        CBOR *next = it->next->next;
        CBOR *prev = it->prev;

        if (it == key) {
            if (prev)
                prev->next = next;
            if (next)
                next = NULL;
            --len;
            return NO_ERR;
        }
        if (next)
            it = next;
        else
            return ERR_NO_VAL_FOR_KEY;
    }
    return ERR_NOT_FOUND;
}

// !SECTION: Array and map

};