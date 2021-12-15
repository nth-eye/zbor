#include <cstdio>
#include <cstring>
#include "zbor.h"

#define SIZE(x) (sizeof(x) / sizeof(x[0]))

using namespace zbor;

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
            printf("%lu(", obj.tag.val);
            if (obj.tag.content)
                pretty_cbor(*obj.tag.content);
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
        case TYPE_HALF: printf("NOT IMPLEMENTED"); break;
        case TYPE_FLOAT: printf("%ef", obj.f); break;
        case TYPE_DOUBLE: printf("%e", obj.d); break;
        default: printf("<unknown>");
    }
}

// #include <ctime>

// template<size_t N = 1, class Fn, class ...Args>
// clock_t measure_time(Fn &&fn, Args &&...args)
// {
//     clock_t begin = clock();
//     for (size_t i = 0; i < N; ++i) 
//         fn(args...);
//     clock_t end = clock();

//     return (end - begin); // / N;
// }

int main(int, char**) 
{
    printf("CBOR: %lu bytes \n", sizeof(CBOR));

    Pool<16> pool;

    const uint8_t buf[] = { 0x0a, 0x17, /*0x18, 0x64*/ };

    auto ret = decode(pool, buf, sizeof(buf));

    printf("pool size: %lu \n", pool.size());

    if (ret.root) {
        int i = 0;
        for (auto it : *ret.root) {
            printf("%d) ", ++i);
            pretty_cbor(it);
            printf("\n");
        }
    }
    // const uint8_t data[] = { 0xde, 0xad, 0xbe, 0xef };
    // const char *text = "test";

    // Array arr;
    // CBOR elements[] = { 0, -99, { text, strlen(text) } };

    // arr.push(&elements[0]);
    // arr.push(&elements[1]);
    // arr.push(&elements[2]);

    // CBOR tag_content = false;

    // CBOR cbors[] = {
    //     666,
    //     -44,
    //     { data, sizeof(data) },
    //     { text, strlen(text) },
    //     arr,
    //     Map{ },
    //     Tag{ 2, &tag_content },
    //     true,
    //     PRIM_NULL,
    //     0.0f,
    //     0.0,
    // };

    // int i = 0;

    // for (auto &obj : cbors) {
    //     printf("%d) ", ++i);
    //     pretty_cbor(obj);
    //     printf("\n");
    // }
}
