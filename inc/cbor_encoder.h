#ifndef CBOR_ENCODER_H
#define CBOR_ENCODER_H

#include "cbor.h"

namespace cbor {

template<size_t N>
struct Buf {

    bool encode(CBOR *val);
    bool encode_sint(int64_t val);
    bool encode_uint(uint64_t val);
    bool encode_nint(uint64_t val); // If you need -2^64 (-18446744073709551616)
    bool encode_data(const void *data, size_t len);
    bool encode_text(const char *text, size_t len);
    bool encode_simple(Prim val);
    bool encode_fp16(half val);
    bool encode_fp32(float val);
    bool encode_fp64(double val);

    uint8_t buf[N];
    size_t  cnt = 0;
};

}

#endif