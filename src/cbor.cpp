#include "cbor.h"

namespace cbor {

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

};