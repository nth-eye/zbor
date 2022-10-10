#ifndef ZBOR_LOG_H
#define ZBOR_LOG_H

#include "zbor/decode.h"
#include "utl/log.h"

namespace zbor {

constexpr const char* str_type(type_t t)
{
    switch (t) {
        case type_uint: return "unsigned";
        case type_sint: return "negative";
        case type_data: return "byte string";
        case type_text: return "text string ";
        case type_array: return "array";
        case type_map: return "map";
        case type_tag: return "tag";
        case type_prim: return "simple";
        case type_double: return "float";
        case type_indef_data: return "indefinite byte string";
        case type_indef_text: return "indefinite text string";
        case type_invalid: return "<invalid>";
        default: return "<unknown>";
    }
}

constexpr const char* str_err(err_t e)
{
    switch (e) {
        case err_ok: return "err_ok";
        case err_no_memory: return "err_no_memory";
        case err_out_of_bounds: return "err_out_of_bounds";
        case err_invalid_simple: return "err_invalid_simple";
        case err_invalid_float_type: return "err_invalid_float_type";
        case err_invalid_indef_mt: return "err_invalid_indef_mt";
        case err_invalid_indef_item: return "err_invalid_indef_item";
        case err_reserved_ai: return "err_reserved_ai";
        case err_break_without_start: return "err_break_without_start";
        default: return "<unknown>";
    }
}

/**
 * @brief Print CBOR object in diagnostic notation.
 * 
 * @param obj CBOR item
 */
inline void log_obj(const obj_t& obj)
{
    switch (obj.type) 
    {
    case type_uint: 
        printf("%lu", obj.uint); 
    break;
    case type_sint: 
        printf("%ld", obj.sint); 
    break;
    case type_data:
        printf("h'");
        for (auto c : obj.data)
            printf("%02x", c);
        printf("'");
    break;
    case type_text:
        printf("\"%.*s\"", int(obj.text.size()), obj.text.data());
    break;
    case type_array:
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
    case type_map:
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
    case type_tag: 
        printf("%lu(", obj.tag.num());
        log_obj(obj.tag.content());
        printf(")");
    break;
    case type_prim:
        switch (obj.prim) 
        {
        case prim_false: 
            printf("false"); 
        break;
        case prim_true: 
            printf("true"); 
        break;
        case prim_null: 
            printf("null"); 
        break;
        case prim_undefined: 
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
    case type_double: 
        if (int(obj.dbl * 10) % 10 == 0) {
            if (obj.dbl == 0 && std::signbit(obj.dbl))
                printf("-0.0");
            else
                printf("%.1f", obj.dbl); 
        } else {
            printf("%g", obj.dbl); 
        }
    break;
    case type_indef_data:
    case type_indef_text:
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
    case type_invalid:
        printf("<invalid>");
    break;
    default: 
        printf("<unknown>");
    }
}

/**
 * @brief Print CBOR sequence as raw bytes and diagnostic notation.
 * 
 * @param s CBOR sequence
 */
inline void log_seq(const seq_t& s)
{
    printf("+-----------HEX-----------+\n");
    utl::log_hex(s.data(), s.size());
    printf("+--------DIAGNOSTIC-------+\n");
    int i = 0; 
    for (auto it : s) {
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
inline void log_obj_with_pad(const obj_t& obj, int first_pad = 0, int pad = 0)
{
    for (int i = 0; i < first_pad; ++i)
        printf(" ");
    switch (obj.type) 
    {
    case type_uint: 
        printf("%lu", obj.uint); 
    break;
    case type_sint: 
        printf("%ld", obj.sint); 
    break;
    case type_data:
        printf("h'");
        for (auto c : obj.data)
            printf("%02x", c);
        printf("'");
    break;
    case type_text:
        printf("\"%.*s\"", int(obj.text.size()), obj.text.data());
    break;
    case type_array:
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
    case type_map:
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
    case type_tag: 
        printf("%lu(", obj.tag.num());
        log_obj_with_pad(obj.tag.content(), 0, 0);
        printf(")");
    break;
    case type_prim:
        switch (obj.prim) 
        {
        case prim_false: 
            printf("false"); 
        break;
        case prim_true: 
            printf("true"); 
        break;
        case prim_null: 
            printf("null"); 
        break;
        case prim_undefined: 
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
    case type_double: 
        if (int(obj.dbl * 10) % 10 == 0) {
            if (obj.dbl == 0 && std::signbit(obj.dbl))
                printf("-0.0");
            else
                printf("%.1f", obj.dbl); 
        } else {
            printf("%g", obj.dbl); 
        }
    break;
    case type_indef_data:
    case type_indef_text:
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
    case type_invalid:
        printf("<invalid>");
    break;
    default: 
        printf("<unknown>");
    }
}

}

#endif