#ifndef ZBOR_LOG_H
#define ZBOR_LOG_H

#include "zbor/decode.h"
#include <cstdio>
#include <cctype>
#include <cmath>

namespace zbor {

constexpr const char* str_type(Type t)
{
    switch (t) {
        case TYPE_UINT: return "unsigned ";
        case TYPE_SINT: return "negative ";
        case TYPE_DATA: return "byte string";
        case TYPE_TEXT: return "text string ";
        case TYPE_ARRAY: return "array";
        case TYPE_MAP: return "map";
        case TYPE_TAG: return "tag";
        case TYPE_PRIM: return "simple";
        case TYPE_DOUBLE: return "float";
        case TYPE_INDEF_DATA: return "indefinite byte string";
        case TYPE_INDEF_TEXT: return "indefinite text string";
        case TYPE_INVALID: return "<invalid>";
        default: return "<unknown>";
    }
}

constexpr const char* str_err(Err e)
{
    switch (e) {
        case ERR_OK: return "ERR_OK";
        case ERR_NO_MEMORY: return "ERR_NO_MEMORY";
        case ERR_OUT_OF_BOUNDS: return "ERR_OUT_OF_BOUNDS";
        case ERR_INVALID_SIMPLE: return "ERR_INVALID_SIMPLE";
        case ERR_INVALID_FLOAT_TYPE: return "ERR_INVALID_FLOAT_TYPE";
        case ERR_INVALID_INDEF_MT: return "ERR_INVALID_INDEF_MT";
        case ERR_INVALID_INDEF_ITEM: return "ERR_INVALID_INDEF_ITEM";
        case ERR_RESERVED_AI: return "ERR_RESERVED_AI";
        case ERR_BREAK_WITHOUT_START: return "ERR_BREAK_WITHOUT_START";
        default: return "<unknown>";
    }
}

/**
 * @brief Print hex nicely with relevant ASCII representation.
 * 
 * @param data Data to print
 * @param len Length in bytes
 */
inline void log_hex(const void *data, size_t len)
{
    if (!data)
        return;
    const uint8_t *p = static_cast<const uint8_t*>(data);

    for (size_t i = 0; i < len; ++i) {

        if (!(i & 15))
            printf("| ");
        printf("%02x ", p[i]);
        
        if ((i & 7) == 7)
            printf(" ");

        if ((i & 15) == 15) {
            printf("|");
            for (int j = 15; j >= 0; --j) {
                char c = p[i - j];
                printf("%c", isprint(c) ? c : '.');
            }
            printf("|\n");
        }
    }
    int rem = len - ((len >> 4) << 4);

    if (rem) {
        printf("%*c |", (16 - rem) * 3 + ((~rem & 8) >> 3), ' ');
        for (int j = rem; j; --j) {
            char c = p[len - j];
            printf("%c", isprint(c) ? c : '.');
        }
        for (int j = 0; j < 16 - rem; ++j)
            printf(".");
        printf("|\n");
    }
}

/**
 * @brief Print CBOR object in diagnostic notation.
 * 
 * @param obj CBOR item
 */
inline void log_obj(const Obj &obj)
{
    switch (obj.type) 
    {
    case TYPE_UINT: 
        printf("%lu", obj.uint); 
    break;
    case TYPE_SINT: 
        printf("%ld", obj.sint); 
    break;
    case TYPE_DATA:
        printf("h'");
        for (size_t i = 0; i < obj.str.len; ++i)
            printf("%02x", obj.str.dat[i]);
        printf("'");
    break;
    case TYPE_TEXT:
        printf("\"%.*s\"", int(obj.str.len), obj.str.txt);
    break;
    case TYPE_ARRAY:
    {
        printf("[");

        if (obj.arr.indef())
            printf("_ ");

        auto arr_itr = obj.arr.begin();
        auto arr_end = obj.arr.end();

        while (arr_itr != arr_end) {
            log_obj(*arr_itr);
            if (++arr_itr != arr_end)
                printf(", ");
        }
        printf("]");
    }
    break;
    case TYPE_MAP:
    {
        printf("{");

        if (obj.map.indef())
            printf("_ ");

        auto map_itr = obj.map.begin();
        auto map_end = obj.map.end();

        while (map_itr != map_end) {

            auto [key, val] = *map_itr;

            log_obj(key);
            printf(": ");
            log_obj(val);

            if (++map_itr != map_end)
                printf(", ");
        }
        printf("}");
    }
    break;
    case TYPE_TAG: 
        printf("%lu(", obj.tag.num());
        log_obj(obj.tag.content());
        printf(")");
    break;
    case TYPE_PRIM:
        switch (obj.prim) 
        {
        case PRIM_FALSE: 
            printf("false"); 
        break;
        case PRIM_TRUE: 
            printf("true"); 
        break;
        case PRIM_NULL: 
            printf("null"); 
        break;
        case PRIM_UNDEFINED: 
            printf("undefined"); 
        break;
        default:
            if (obj.prim < 24 || obj.prim > 31)
                printf("simple(%u)", obj.prim);
            else
                printf("<illegal>");
        break;
        }
    break;
    case TYPE_DOUBLE: 
        if (int(obj.dbl * 10) % 10 == 0) {
            if (obj.dbl == 0 && std::signbit(obj.dbl))
                printf("-0.0");
            else
                printf("%.1f", obj.dbl); 
        } else {
            printf("%g", obj.dbl); 
        }
    break;
    case TYPE_INDEF_DATA:
    case TYPE_INDEF_TEXT:
    {
        printf("(_ ");

        auto str_itr = obj.istr.begin();
        auto str_end = obj.istr.end();

        while (str_itr != str_end) {
            log_obj(*str_itr);
            if (++str_itr != str_end)
                printf(", ");
        }
        printf(")");
    } 
    break;
    case TYPE_INVALID:
        printf("<invalid>");
    break;
    default: 
        printf("<unknown>");
    }
}

/**
 * @brief Print CBOR sequence as raw bytes and diagnostic notation.
 * 
 * @param seq CBOR sequence
 */
inline void log_seq(const Seq &seq)
{
    printf("+-----------HEX-----------+\n");
    log_hex(seq.data(), seq.size());
    printf("+--------DIAGNOSTIC-------+\n");
    int i = 0; 
    for (auto it : seq) {
        printf("| %d) ", ++i);
        log_obj(it);
        printf("\n");
    }
    printf("+-------------------------+\n");
}

/**
 * @brief Print CBOR object in diagnostic notation with padding for maps
 * and arrays.
 * 
 * @param obj CBOR item
 */
inline void log_obj_with_pad(const Obj &obj, int first_pad = 0, int pad = 0)
{
    for (int i = 0; i < first_pad; ++i)
        printf(" ");
    switch (obj.type) 
    {
    case TYPE_UINT: 
        printf("%lu", obj.uint); 
    break;
    case TYPE_SINT: 
        printf("%ld", obj.sint); 
    break;
    case TYPE_DATA:
        printf("h'");
        for (size_t i = 0; i < obj.str.len; ++i)
            printf("%02x", obj.str.dat[i]);
        printf("'");
    break;
    case TYPE_TEXT:
        printf("\"%.*s\"", int(obj.str.len), obj.str.txt);
    break;
    case TYPE_ARRAY:
    {
        printf("[");

        if (obj.arr.indef())
            printf("_ ");

        auto arr_itr = obj.arr.begin();
        auto arr_end = obj.arr.end();

        while (arr_itr != arr_end) {
            printf("\n");
            log_obj_with_pad(*arr_itr, pad + 2, pad + 2);
            printf(", ");
            if (!(++arr_itr != arr_end))
                printf("\n");
        }
        for (int i = 0; i < pad; ++i)
            printf(" ");
        printf("]");
    }
    break;
    case TYPE_MAP:
    {
        printf("{");

        if (obj.map.indef())
            printf("_ ");

        auto map_itr = obj.map.begin();
        auto map_end = obj.map.end();

        while (map_itr != map_end) {

            auto [key, val] = *map_itr;

            printf("\n");
            log_obj_with_pad(key, pad + 1, pad + 1);
            printf(": ");
            log_obj_with_pad(val, 0, pad + 1);
            printf(", ");

            if (!(++map_itr != map_end))
                printf("\n");
        }
        for (int i = 0; i < pad; ++i)
            printf(" ");
        printf("}");
    }
    break;
    case TYPE_TAG: 
        printf("%lu(", obj.tag.num());
        log_obj_with_pad(obj.tag.content(), 0, 0);
        printf(")");
    break;
    case TYPE_PRIM:
        switch (obj.prim) 
        {
        case PRIM_FALSE: 
            printf("false"); 
        break;
        case PRIM_TRUE: 
            printf("true"); 
        break;
        case PRIM_NULL: 
            printf("null"); 
        break;
        case PRIM_UNDEFINED: 
            printf("undefined"); 
        break;
        default:
            if (obj.prim < 24 || obj.prim > 31)
                printf("simple(%u)", obj.prim);
            else
                printf("<illegal>");
        break;
        }
    break;
    case TYPE_DOUBLE: 
        if (int(obj.dbl * 10) % 10 == 0) {
            if (obj.dbl == 0 && std::signbit(obj.dbl))
                printf("-0.0");
            else
                printf("%.1f", obj.dbl); 
        } else {
            printf("%g", obj.dbl); 
        }
    break;
    case TYPE_INDEF_DATA:
    case TYPE_INDEF_TEXT:
    {
        printf("(_ ");

        auto str_itr = obj.istr.begin();
        auto str_end = obj.istr.end();

        while (str_itr != str_end) {
            log_obj_with_pad(*str_itr, 0, 0);
            if (++str_itr != str_end)
                printf(", ");
        }
        printf(")");
    } 
    break;
    case TYPE_INVALID:
        printf("<invalid>");
    break;
    default: 
        printf("<unknown>");
    }
}

}

#endif