#ifndef CBOR_MAKE_H
#define CBOR_MAKE_H

#include "cbor.h"

namespace cbor {

CBOR make_sint(int64_t val);
CBOR make_uint(uint64_t val);
CBOR make_data(const uint8_t *data, size_t len);
CBOR make_text(const char *text, size_t len);
CBOR make_array();
CBOR make_map();
CBOR make_tag();
CBOR make_bool(bool val);
CBOR make_prim(Prim val);
CBOR make_float(float val);
CBOR make_double(double val);

};

#endif