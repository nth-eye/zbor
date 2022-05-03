#include <cstdio>
#include <ctime>
#include "zbor/encode.h"
#include "zbor/decode.h"

using namespace zbor;

void pretty_cbor(Obj &obj)
{
    switch (obj.type) {

        case TYPE_UINT: printf("%lu", obj.uint); break;
        case TYPE_SINT: printf("%ld", obj.sint); break;
        case TYPE_DATA:
            printf("h'");
            for (size_t i = 0; i < obj.str.len; ++i)
                printf("%02x", obj.str.dat[i]);
            printf("'");
            break;
        case TYPE_TEXT:
            printf("\"%.*s\"", (int) obj.str.len, obj.str.txt);
            break;
        case TYPE_ARRAY:
            printf("[");
            for (auto child : obj.arr) {
                pretty_cbor(*child);
                if (child->next)
                    printf(", ");
            }
            printf("]");
            break;
        case TYPE_MAP:
            printf("{");
            for (auto child : obj.map) {
                pretty_cbor(*child.key);
                printf(": ");
                pretty_cbor(*child.val);
                if (child.val->next)
                    printf(", ");
            }
            printf("}");
            break;
        case TYPE_TAG: 
            printf("%lu(", obj.tag.val);
            if (obj.tag.content)
                pretty_cbor(*obj.tag.content);
            printf(")");
            break;
        case TYPE_PRIM:
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
        case TYPE_DOUBLE: printf("%f", obj.dbl); break;
        default: printf("<unknown>");
    }
}

template<size_t N = 1, class Fn, class Ptr, class ...Args>
clock_t measure_time(Fn &&fn, Ptr *ptr, Args &&...args)
{
    clock_t begin = clock();
    for (size_t i = 0; i < N; ++i) 
        (ptr->*fn)(args...);
    clock_t end = clock();

    return (end - begin);
}

int main(int, char**) 
{
    printf("sizeof Pair: %lu \n", sizeof(zbor::Pair));
    printf("sizeof String: %lu \n", sizeof(zbor::String));
    printf("sizeof Array: %lu \n", sizeof(zbor::Array));
    printf("sizeof Map: %lu \n", sizeof(zbor::Map));
    printf("sizeof Tag: %lu \n", sizeof(zbor::Tag));
    printf("sizeof Sequence: %lu \n", sizeof(zbor::Sequence));
    printf("sizeof Obj: %lu \n", sizeof(zbor::Obj));

    // ANCHOR: Example pool

    // zbor::Pool<4> pool;
    // zbor::Array arr;
    // zbor::Obj *ptr;

    // arr.push(pool.make(true));
    // arr.push(pool.make(false));
    // arr.push(pool.make(Prim(69)));

    // ptr = pool.make(PRIM_NULL);
    // pool.free(ptr);

    // ptr = pool.make(arr);
    // pool.free(ptr); // NOTE: Doesn't automatically free elements

    // ptr = pool.make(arr);
    // for (auto it : ptr->arr) // Free elements (non recursive)
    //     pool.free(it);
    // pool.free(ptr);

    // ANCHOR: Example Encoding

    // zbor::Pool<1> pool;
    // zbor::Encoder<9> enc;

    // enc.encode(pool.make(0xfffffffffffffffful));

    // for (size_t i = 0; i < enc.size(); ++i)
    //     printf("%02x ", enc[i]);
    // printf("\n");

    // ANCHOR: Example General
    
    // Pool<32> pool;
    // Array arr;
    // Map map;

    // const uint8_t data[] = { 0xde, 0xad, 0xbe, 0xef };
    // const char *text = "test";

    // arr.push(pool.make(0));
    // arr.push(pool.make(-99));
    // arr.push(pool.make(text, strlen(text)));
    // map.push(pool.make("arr", 3), pool.make(arr));
    // map.push(pool.make("err", 3), pool.make(777));

    // Obj cbors[] = {
    //     666,
    //     -44,
    //     { data, sizeof(data) },
    //     { text, strlen(text) },
    //     arr,
    //     map,
    //     Tag{ 2, pool.make(false) },
    //     true,
    //     PRIM_NULL,
    //     0.0f,
    //     0.0,
    //     13.37,
    // };

    // int i = 0;

    // for (auto &obj : cbors) {
    //     printf("%d) ", ++i);
    //     pretty_cbor(obj);
    //     printf("\n");
    // }
}
