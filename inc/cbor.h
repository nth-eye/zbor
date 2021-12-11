#ifndef CBOR_H
#define CBOR_H

#include "cbor_misc.h"

namespace cbor {

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
private:
    CBOR    buf[N];
    size_t  cnt = 0;
};

};

#endif
