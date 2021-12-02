#include <iostream>
#include <cstring>
#include "cbor.h"
#include "cbor_make.h"
#include "cbor_pool.h"
#include "cbor_encoder.h"

#define SIZE(x) (sizeof(x) / sizeof(x[0]))

using namespace cbor;

void pretty_cbor(const CBOR *cbor)
{
    printf("[%p] \n", (void*) cbor);

    int i = 0;

    for (const CBOR *obj = cbor; obj;) {

        printf("%d) ", ++i);

        switch (obj->type) {

            case TYPE_UINT:
                printf("%lu", obj->uint);
                break;

            case TYPE_SINT:
                printf("%ld", obj->sint);
                break;

            case TYPE_DATA:
                for (size_t i = 0; i < obj->str.len; ++i)
                    printf("%02x", obj->str.data[i]);
                break;

            case TYPE_TEXT:
                printf("\"%.*s\"", (int) obj->str.len, obj->str.text);
                break;

            case TYPE_ARRAY:
            case TYPE_MAP:
            case TYPE_TAG:
            case TYPE_SIMPLE:
                printf("simple(%u)", obj->prim);
                break;

            case TYPE_FLOAT_16:
            case TYPE_FLOAT_32:
            case TYPE_FLOAT_64:
                break;

            default:
                printf("unknown type");
        }

        printf("\n");

        obj = obj->next;
    }
}

int main(int, char**) 
{
    // cbor::Pool<4> pool;
    // cbor::Buf<128> buf;

    // printf("CBOR: %lu bytes \n", sizeof(CBOR));

    const uint8_t data[]    = { 0xde, 0xad, 0xbe, 0xef };
    const char *text        = "test";

    CBOR cbors[] = {
        make_uint(666),
        make_sint(-44),
        make_data(data, sizeof(data)),
        make_text(text, strlen(text)),
    };

    for (size_t i = 0; i < SIZE(cbors) - 1; ++i) // Imitate decoding process, when all saved sequentialy
        cbors[i].next = &cbors[i + 1];

    pretty_cbor(&cbors[0]);
}
