#include "cbor_make.h"

namespace cbor {

CBOR make_sint(int64_t val)
{
    CBOR cbor;

    if (val < 0) {
        cbor.type = TYPE_SINT;
        cbor.sint = val;
    } else {
        cbor.type = TYPE_UINT;
        cbor.uint = val;
    }
    return cbor;
}

CBOR make_uint(uint64_t val)
{
    CBOR cbor;

    cbor.type = TYPE_UINT;
    cbor.uint = val;

    return cbor;
}

CBOR make_data(const uint8_t *data, size_t len)
{
    CBOR cbor;

    cbor.type       = TYPE_DATA;
    cbor.str.data   = data;
    cbor.str.len    = len;

    return cbor;
}

CBOR make_text(const char *text, size_t len)
{
    CBOR cbor;

    cbor.type       = TYPE_TEXT;
    cbor.str.text   = text;
    cbor.str.len    = len;

    return cbor;
}

// CBOR make_array()
// {

// }

// CBOR make_map()
// {

// }

// CBOR make_tag()
// {

// }

// CBOR make_bool(bool val)
// {

// }

// CBOR make_prim(Prim val)
// {

// }

// CBOR make_float(float val)
// {

// }

// CBOR make_double(double val)
// {

// }

};