#include "cbor.h"

namespace cbor {

CBOR::CBOR(int val) : CBOR(int64_t(val))
{

}

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

CBOR::CBOR(uint64_t val)
{
    type = TYPE_UINT;
    uint = val;
}

CBOR::CBOR(const uint8_t *data, size_t len)
{
    type        = TYPE_DATA;
    str.data    = data;
    str.len     = len;
}

CBOR::CBOR(const char *text, size_t len)
{
    type        = TYPE_TEXT;
    str.text    = text;
    str.len     = len;
}

CBOR::CBOR(CBOR *element)
{
    type = TYPE_ARRAY;

    if (element)
        arr.push(element);
    else
        arr.reset();
}

CBOR::CBOR(CBOR *key, CBOR *val)
{
    type = TYPE_MAP;

    if (key && val)
        map.insert(key, val);
    else
        map.reset();
}

CBOR::CBOR(uint64_t tag_val, CBOR *tag_content)
{
    type = TYPE_TAG;
    uint = tag_val;
    next = tag_content;
}

CBOR::CBOR(Prim val)
{
    if (val < 24 || val > 31) { // Otherwise invalid
        type = TYPE_SIMPLE;
        prim = val;
    }
}

CBOR::CBOR(bool val) : CBOR(val ? PRIM_TRUE : PRIM_FALSE)
{

}

CBOR::CBOR(float val)
{
    type    = TYPE_FLOAT_32;
    f.f32   = val;
}

CBOR::CBOR(double val)
{
    type    = TYPE_FLOAT_64;
    f.f64   = val;
}

void CBOR::iter::operator++()
{ 
    p = p->next; 
}

// SECTION: Array and map

void Array::iter::operator++()
{ 
    p = p->next; 
}

void Array::reset()
{
    head = NULL;
    tail = NULL;
    len = 0;
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

    for (CBOR *it = head; it;) {
        CBOR *next = it->next;
        if (it == val) {
            it->prev->next = next;
            if (next)
                next = NULL;
            --len;
            return NO_ERR;
        }
        it = it->next;
    }
    return ERR_NOT_FOUND;
}

void Map::iter::operator++()
{ 
    p = p->next->next; 
}

void Map::reset()
{
    head = NULL;
    tail = NULL;
    len = 0;
}

Err Map::insert(CBOR *key, CBOR *val)
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

Err Map::erase(CBOR *key)
{
    if (!key)
        return ERR_NULL_PTR;

    if (!len)
        return ERR_ALREADY_EMPTY;

    for (CBOR *it = head; it;) {

        CBOR *next = it->next->next;

        if (it == key) {
            it->prev->next = next;
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