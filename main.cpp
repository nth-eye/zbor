#include <cstdio>
#include <cstring>
#include <ctime>
#include "zbor.h"

// #define SIZE(x) (sizeof(x) / sizeof(x[0]))

// void pretty_cbor(CBOR &obj)
// {
//     switch (obj.type) {

//         case TYPE_UINT: printf("%lu", obj.uint); break;
//         case TYPE_SINT: printf("%ld", obj.sint); break;
//         case TYPE_DATA:
//             printf("h'");
//             for (size_t i = 0; i < obj.str.len; ++i)
//                 printf("%02x", obj.str.data[i]);
//             printf("'");
//             break;
//         case TYPE_TEXT:
//             printf("\"%.*s\"", (int) obj.str.len, obj.str.text);
//             break;
//         case TYPE_ARRAY:
//             printf("[");
//             for (auto &child : obj.arr) {
//                 pretty_cbor(child);
//                 if (child.next)
//                     printf(", ");
//             }
//             printf("]");
//             break;
//         case TYPE_MAP:
//             printf("{");
//             printf("}");
//             break;
//         case TYPE_TAG: 
//             printf("%lu(", obj.tag.val);
//             if (obj.tag.content)
//                 pretty_cbor(*obj.tag.content);
//             printf(")");
//             break;
//         case TYPE_SIMPLE:
//             switch (obj.prim) {
//                 case PRIM_FALSE: printf("false"); break;
//                 case PRIM_TRUE: printf("true"); break;
//                 case PRIM_NULL: printf("null"); break;
//                 case PRIM_UNDEFINED: printf("undefined"); break;
//                 default:
//                     if (obj.prim < 24 || obj.prim > 31)
//                         printf("simple(%u)", obj.prim);
//                     else
//                         printf("<illegal>");
//                     break;
//             }
//             break;
//         case TYPE_HALF: printf("NOT IMPLEMENTED"); break;
//         case TYPE_FLOAT: printf("%ff", obj.f); break;
//         case TYPE_DOUBLE: printf("%f", obj.d); break;
//         default: printf("<unknown>");
//     }
// }

template<size_t N = 1, class Fn, class Ptr, class ...Args>
clock_t measure_time(Fn &&fn, Ptr *ptr, Args &&...args)
{
    clock_t begin = clock();
    for (size_t i = 0; i < N; ++i) 
        (ptr->*fn)(args...);
    clock_t end = clock();

    return (end - begin);
}

// template<size_t N = 1, class Fn, class ...Args>
// clock_t measure_time(Fn &&fn, Args &&...args)
// {
//     clock_t begin = clock();
//     for (size_t i = 0; i < N; ++i) 
//         fn(args...);
//     clock_t end = clock();

//     return (end - begin); // / N;
// }

using namespace zbor;

int main(int, char**) 
{
    printf("sizeof Pair: %lu \n", sizeof(zbor::Pair));
    printf("sizeof String: %lu \n", sizeof(zbor::String));
    printf("sizeof Array: %lu \n", sizeof(zbor::Array));
    printf("sizeof Map: %lu \n", sizeof(zbor::Map));
    printf("sizeof Tag: %lu \n", sizeof(zbor::Tag));
    printf("sizeof Sequence: %lu \n", sizeof(zbor::Sequence));
    printf("sizeof CBOR: %lu \n", sizeof(zbor::CBOR));

    // using PoolType = StaticPool<int, 32>;
    // PoolType pool;

    // int *ints[32] = {};

    // for (int i = 0; i < 32; ++i)
    //     ints[i] = pool.make();
    // pool.free(ints[1]);

    // printf("1: %lu clock_t \n", measure_time<100000000>(&PoolType::get_free_idx, &pool));
    // printf("2: %lu clock_t \n", measure_time<100000000>(&PoolType::get_free_idx_bit, &pool));
    // printf("1: %lu clock_t \n", measure_time<100000000>(&PoolType::get_free_idx, &pool));
    // printf("2: %lu clock_t \n", measure_time<100000000>(&PoolType::get_free_idx_bit, &pool));
    // printf("1: %lu clock_t \n", measure_time<100000000>(&PoolType::get_free_idx, &pool));

    // // printf("CBOR: %lu bytes \n", sizeof(CBOR));

    // // Pool<64> pool;

    // const uint8_t buf[] = { 
    //     // 0x0a, 
    //     // 0x17, 
    //     // 0x18, 0x64,
    //     // 0x39, 0x03, 0xe7,
    //     // 0x44, 0x01, 0x02, 0x03, 0x04,
    //     // 0x64, 0x49, 0x45, 0x54, 0x46,

    //     // 0xf4,
    //     // 0xf5,
    //     // 0xf6,
    //     // 0xf7,
    //     // 0xf0,
    //     // 0xf8, 0xff,

    //     // 0xf9, 0x00, 0x00,
    //     // 0xf9, 0x80, 0x00,
    //     // 0xf9, 0x3c, 0x00,
    //     // 0xfa, 0x47, 0xc3, 0x50, 0x00,
    //     // 0xfb, 0x3f, 0xf1, 0x99, 0x99, 0x99, 0x99, 0x99, 0x9a,
    //     // 0xf9, 0x7c, 0x00,
    //     // 0xf9, 0x7e, 0x00,
    //     // 0xf9, 0xfc, 0x00,

    //     0x80,
    //     0x83, 0x01, 0x02, 0x03,
    //     0x83, 0x01, 0x82, 0x02, 0x03, 0x82, 0x04, 0x05,

    //     0x82, 0x01, 0x82, 0x02, 0x03,
    //     0x04,
    //     0x98, 0x19, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x18, 0x18, 0x19,
    // };

    // printf("1: %lu clock_t \n", measure_time<10000000>(decode<64>, pool, buf, sizeof(buf)));
    // // printf("2: %lu clock_t \n", measure_time<10000000>(decode_2<64>, pool, buf, sizeof(buf)));
    // printf("1: %lu clock_t \n", measure_time<10000000>(decode<64>, pool, buf, sizeof(buf)));
    // // printf("2: %lu clock_t \n", measure_time<10000000>(decode_2<64>, pool, buf, sizeof(buf)));
    // printf("1: %lu clock_t \n", measure_time<10000000>(decode<64>, pool, buf, sizeof(buf)));

    // auto ret = decode(pool, buf, sizeof(buf));

    // printf("Pool: %lu tokens \n", pool.size());
    // printf("        \n\
    //     root:   %p  \n\
    //     size:   %lu \n\
    //     err:    %s  \n",
    //     (void*) ret.root,
    //     ret.len,
    //     err_str(ret.err));

    // if (ret.root) {
    //     int i = 0;
    //     for (auto it : *ret.root) {
    //         printf("%d) ", ++i);
    //         pretty_cbor(it);
    //         printf("\n");
    //     }
    // }
    // // const uint8_t data[] = { 0xde, 0xad, 0xbe, 0xef };
    // // const char *text = "test";

    // // Array arr;
    // // CBOR elements[] = { 0, -99, { text, strlen(text) } };

    // // arr.push(&elements[0]);
    // // arr.push(&elements[1]);
    // // arr.push(&elements[2]);

    // // CBOR tag_content = false;

    // // CBOR cbors[] = {
    // //     666,
    // //     -44,
    // //     { data, sizeof(data) },
    // //     { text, strlen(text) },
    // //     arr,
    // //     Map{ },
    // //     Tag{ 2, &tag_content },
    // //     true,
    // //     PRIM_NULL,
    // //     0.0f,
    // //     0.0,
    // // };

    // // int i = 0;

    // // for (auto &obj : cbors) {
    // //     printf("%d) ", ++i);
    // //     pretty_cbor(obj);
    // //     printf("\n");
    // }
}
