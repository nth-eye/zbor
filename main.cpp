#include <cstdio>
#include <cstring>
#include "cbor.h"
#include "cbor_pool.h"
#include "cbor_encoder.h"

#define SIZE(x) (sizeof(x) / sizeof(x[0]))

using namespace cbor;

void pretty_cbor(CBOR &obj)
{
    switch (obj.type) {

        case TYPE_UINT: printf("%lu", obj.uint); break;
        case TYPE_SINT: printf("%ld", obj.sint); break;
        case TYPE_DATA:
            printf("h'");
            for (size_t i = 0; i < obj.str.len; ++i)
                printf("%02x", obj.str.data[i]);
            printf("'");
            break;
        case TYPE_TEXT:
            printf("\"%.*s\"", (int) obj.str.len, obj.str.text);
            break;
        case TYPE_ARRAY:
            printf("[");
            for (auto &child : obj.arr) {
                pretty_cbor(child);
                if (child.next)
                    printf(", ");
            }
            printf("]");
            break;
        case TYPE_MAP:
            printf("{");
            printf("}");
            break;
        case TYPE_TAG: 
            printf("%lu(", obj.uint);
            if (obj.next)
                pretty_cbor(*obj.next);
            printf(")");
            break;
        case TYPE_SIMPLE:
            switch (obj.prim) {
                case PRIM_FALSE: printf("false"); break;
                case PRIM_TRUE: printf("true"); break;
                case PRIM_NULL: printf("null"); break;
                case PRIM_UNDEFINED: printf("undefined"); break;
                default:
                    if (obj.prim < 24 || obj.prim > 31)
                        printf("simple(%u)", obj.prim);
                    else
                        printf("<illegal>");
                    break;
            }
            break;
        case TYPE_FLOAT_16: printf("NOT IMPLEMENTED"); break;
        case TYPE_FLOAT_32: printf("%ef", obj.f.f32); break;
        case TYPE_FLOAT_64: printf("%e", obj.f.f64); break;
        default: printf("<unknown>");
    }
}

void pretty_cbor_sequence(CBOR &cbor)
{
    int i = 0;

    for (auto &obj : cbor) {
        printf("%d) ", ++i);
        pretty_cbor(obj);
        printf("\n");
    }
}

int main(int, char**) 
{
    // cbor::Pool<4> pool;
    // cbor::Buf<128> buf;

    // printf("CBOR: %lu bytes \n", sizeof(CBOR));

    const uint8_t data[] = { 0xde, 0xad, 0xbe, 0xef };
    const char *text = "test";

    CBOR content = false;
    CBOR arr = nullptr;
    CBOR el_0 = 0;
    CBOR el_1 = -99;
    CBOR el_2 = {text, strlen(text)};

    arr.arr.push(&el_0);
    arr.arr.push(&el_1);
    arr.arr.push(&el_2);

    CBOR cbors[] = {
        666,
        -44,
        { data, sizeof(data) },
        { text, strlen(text) },
        arr,
        { nullptr, nullptr },
        { 2, &content },
        content,
        PRIM_NULL,
        0.0f,
        0.0,
    };

    for (size_t i = 0; i < SIZE(cbors) - 1; ++i) // Imitate decoding process, when all saved sequentialy
        cbors[i].next = &cbors[i + 1];

    pretty_cbor_sequence(cbors[0]);
}
