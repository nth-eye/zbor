#ifndef CBOR_H
#define CBOR_H

#include "cbor_types.h"

namespace cbor {

struct CBOR {

    CBOR(int val);
    CBOR(int64_t val);
    CBOR(uint64_t val);
    CBOR(const uint8_t *data, size_t len);
    CBOR(const char *text, size_t len);
    CBOR(CBOR *element);
    CBOR(CBOR *key, CBOR *val);
    CBOR(uint64_t tag_val, CBOR *tag_content);
    CBOR(Prim val);
    CBOR(bool val);
    CBOR(float val);
    CBOR(double val);

    struct iter {

        iter(CBOR *p) : p(p) {}

        void    operator++();
        bool    operator!=(const iter &other)   { return p != other.p; }
        CBOR&   operator*()                     { return *p; }
    private:
        CBOR *p;
    };

    iter begin()    { return iter(this); }
    iter end()      { return iter(NULL); }

    union {
        uint64_t uint;
        int64_t sint;
        Bytes str;
        uint8_t prim;
        Float f;
        Array arr;
        Map map;
    };
    Type type = TYPE_INVALID;
    CBOR *next = NULL;
    CBOR *prev = NULL;
};

};

#endif
