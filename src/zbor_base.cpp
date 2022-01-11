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

CBOR::CBOR(uint64_t val) : type{TYPE_UINT}, uint{val}
{}

CBOR::CBOR(const uint8_t *data, size_t len) : type{TYPE_DATA}, str{data, len}
{}

CBOR::CBOR(const char *text, size_t len) : type{TYPE_TEXT}, str{text, len}
{}

CBOR::CBOR(Array arr) : type{TYPE_ARRAY}, arr{arr}
{}

CBOR::CBOR(Map map) : type{TYPE_MAP}, map{map}
{}

CBOR::CBOR(Tag tag) : type{TYPE_TAG}, tag{tag}
{}

CBOR::CBOR(Prim val) 
{
    if (val < 24 || (val > 31 && val < 256)) {
        type = TYPE_PRIM;
        prim = val;
    }
}

CBOR::CBOR(bool val) : CBOR(val ? PRIM_TRUE : PRIM_FALSE)
{}

CBOR::CBOR(double val) : type{TYPE_DOUBLE}, dbl{val}
{}

// SECTION: Array and map

void Iter::operator++()
{ 
    p = p->next; 
}

void MapIter::operator++()
{
    p = p->next ? p->next->next : nullptr;
}

Pair MapIter::operator*()
{
    return {p, p ? p->next : nullptr};
}

Err Array::push(CBOR *val)
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

    return NO_ERR;
}

Err Array::pop(CBOR *val)
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

            return NO_ERR;
        }
        it = next;
    }
    return ERR_NOT_FOUND;
}

Err Map::push(CBOR *key, CBOR *val)
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
    
    ++len;

    return NO_ERR;
}

Err Map::pop(CBOR *key)
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

            --len;

            if (prev)
                prev->next = next;
            else
                head = next;

            if (next)
                next->prev = prev;
            else
                tail = prev;

            return NO_ERR;
        }
        it = next;
    }
    return ERR_NOT_FOUND;
}

// !SECTION: Array and map

};