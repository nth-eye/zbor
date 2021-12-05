#ifndef CBOR_H
#define CBOR_H

#include "cbor_misc.h"

namespace cbor {

struct CBOR {

    CBOR() {}
    CBOR(int val);
    CBOR(int64_t val);
    CBOR(uint64_t val);
    CBOR(const void *data, size_t len);
    CBOR(const char *text, size_t len);
    CBOR(CBOR *element);
    CBOR(CBOR *key, CBOR *val);
    CBOR(uint64_t tag_val, CBOR *tag_content);
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
        Bytes str;
        Prim prim;
        float f;
        double d;
        Array arr;
        Map map;
    };
    CBOR *next = NULL;
    CBOR *prev = NULL;
};

template<size_t N>
struct Pool {

    void clear() 
    { 
        cnt = 0;
    }

    template<typename... Args>
    CBOR* make(Args... args)
    {
        if (cnt < N)
            return &(buf[cnt++] = CBOR(args...));
        return nullptr;
    }

    void free(CBOR *p)
    {
        if (p < buf || p >= buf + N)
            return;

        if (&buf[--cnt] == p)
            return;

        *p = buf[cnt];
    }
private:
    CBOR    buf[N];
    size_t  cnt = 0;
};

};

#endif
