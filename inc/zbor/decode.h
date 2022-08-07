#ifndef ZBOR_DECODE_H
#define ZBOR_DECODE_H

#include <tuple>
#include "zbor/base.h"

namespace zbor {

/**
 * @brief Decode next adjacent CBOR item. Almost all validity checks always performed 
 * throughout decoding process: out-of-bounds, reserved AI, invalid indef MT, nested 
 * indefinite strings, break without start. What isn't checked is number of elements 
 * within nested containers (if at least one of them is indefinite). For example, 0x9f82ff 
 * will be first parsed as valid indefinite array, and then, only if user starts to traverse 
 * over its elements, decoding of malformed nested array will report err_out_of_bounds.
 * 
 * @param p Begin pointer, must be valid pointer
 * @param end End pointer, must be valid pointer
 * @return Tuple with decoded object, error status and pointer to character past the last character interpreted
 */
inline std::tuple<Obj, Err, const byte*> decode(const byte *p, const byte * const end)
{
    if (p >= end)
        return {{}, err_out_of_bounds, end};

    size_t nest = 0;
    size_t skip = 0;
    size_t len;

    byte mt = *p   & 0xe0;
    byte ai = *p++ & 0x1f;

    uint64_t val = ai;

    Obj obj;
    obj.type = Type(mt >> 5);

    switch (ai) 
    {
    case ai_1:
    case ai_2:
    case ai_4:
    case ai_8: 
        len = utl::bit(ai - ai_1);
        if (p + len > end)
            return {{}, err_out_of_bounds, p};
        val = 0;
        for (int i = 8 * len - 8; i >= 0; i -= 8)
            val |= uint64_t(*p++) << i;
    break;
    case 28:
    case 29:
    case 30:
        return {{}, err_reserved_ai, p};
    case ai_indef:
        switch (mt)
        {
        case mt_data: 
            obj.istr = {p};
            obj.type = type_indef_data;
            nest = 1;
        break;
        case mt_text:
            obj.istr = {p};
            obj.type = type_indef_text;
            nest = 1;
        break;
        case mt_array:
            obj.arr = {p, size_t(-1)};
            nest = 1;
        break;
        case mt_map:
            obj.map = {p, size_t(-1)};
            nest = 1;
        break;
        default:
            return {{}, err_invalid_indef_mt, p};
        }
    }

    if (ai != ai_indef) {
        switch (mt) 
        {
        case mt_uint:
            obj.uint = val;
        break;
        case mt_nint:
            obj.sint = ~val;
        break;
        case mt_data:
        case mt_text:
            obj.str.dat = p;
            obj.str.len = val;
            p += val;
        break;
        case mt_map:
            obj.map = {p, size_t(val)};
            skip = val << 1;
        break;
        case mt_array:
            obj.arr = {p, size_t(val)};
            skip = val;
        break;
        case mt_tag:
            obj.tag = {p, val};
            skip = 1;
        break;
        case mt_simple:
            switch (ai) 
            {
            case prim_float_16:
                obj.dbl     = utl::fp_bits(utl::half_to_float(val)).f32;
                obj.type    = type_double;
            break;
            case prim_float_32:
                obj.dbl     = utl::fp_bits(uint32_t(val)).f32;
                obj.type    = type_double;
            break;
            case prim_float_64:
                obj.dbl     = utl::fp_bits(val).f64;
                obj.type    = type_double;
            break;
            default:
                obj.prim    = Prim(val);
            break;
            }
        break;
        }
    }

    while (skip || nest) {

        if (p >= end)
            return {{}, err_out_of_bounds, end};

        mt  = *p & 0xe0;
        val = *p & 0x1f;
        
        if (obj.type == type_indef_data) {
            if (!(mt == mt_data && val != ai_indef) && *p != 0xff)
                return {{}, err_invalid_indef_item, p};
        } else if (obj.type == type_indef_text) {
            if (!(mt == mt_text && val != ai_indef) && *p != 0xff)
                return {{}, err_invalid_indef_item, p};
        }
        ++p;

        switch (val)
        {
        case ai_1:
        case ai_2:
        case ai_4:
        case ai_8: 
            len = utl::bit(val - ai_1);
            if (p + len > end)
                return {{}, err_out_of_bounds, p};
            val = 0;
            for (int i = 8 * len - 8; i >= 0; i -= 8)
                val |= uint64_t(*p++) << i;
        break;
        case 28:
        case 29:
        case 30:
            return {{}, err_reserved_ai, p};
        case ai_indef:
            switch (mt)
            {
            case mt_array:
            case mt_map:
                ++nest;
            break;
            case mt_simple:
                if (nest)
                    nest--;
                else
                    return {{}, err_break_without_start, p};
            break;
            default:
                return {{}, err_invalid_indef_mt, p};
            }
        }

        switch (mt)
        {
        case mt_uint:
        case mt_nint:
        case mt_simple: 
        break;
        case mt_data:
        case mt_text:
            p += val;
        break;
        case mt_array:
            if (!nest)
                skip += val;
        break;
        case mt_map:
            if (!nest)
                skip += val << 1;
        break;
        case mt_tag:
            if (!nest)
                skip += 1;
        break;
        }

        if (skip && !nest)
            skip--;
    }

    switch (obj.type) 
    {
    case type_array:
    case type_map:
    case type_tag:
    case type_indef_data:
    case type_indef_text: obj.istr.set_end(p); break;
    default:;
    }
    return {obj, err_ok, p};
}

/**
 * @brief Generator which holds range (begin and end pointers) for byte sequence.
 * Used as base for other iterators.
 * 
 */
struct Gen {

    Gen() = default;
    Gen(const byte *head, const byte *tail) : head{head}, tail{tail} {}

    void step(Obj &o) 
    {
        std::tie(o, std::ignore, head) = decode(head, tail); 
    }
protected:
    const byte *head;
    const byte *tail;
};

/**
 * @brief Sequence iterator, used to traverse CBOR sequence (RFC-8742), which
 * is just series of adjacent objects. Used to traverse Array, IndefString or 
 * any series of bytes as Objects one by one. Only exception is Map.
 * 
 */
struct SeqIter : Gen {
    
    SeqIter() = default;
    SeqIter(const byte *head, const byte *tail) : Gen{head, tail}
    {
        step(o);
    }

    bool operator!=(const SeqIter&) const 
    { 
        return o.valid();
    }
    auto& operator*() const 
    { 
        return o; 
    }
    auto& operator++()
    {
        step(o);
        return *this;
    }
    auto operator++(int) 
    { 
        SeqIter tmp = *this; 
        ++(*this); 
        return tmp; 
    }
private:
    Obj o;
};

/**
 * @brief Map iterator, same as SeqIter but parses two objects in a row and 
 * returns them as pair.
 * 
 */
struct MapIter : Gen {

    MapIter() = default;
    MapIter(const byte *head, const byte *tail) : Gen{head, tail}
    {
        step(key);
        if (key.valid()) step(val);
    }

    bool operator!=(const MapIter&) const 
    { 
        return key.valid() && val.valid();
    }
    auto operator*() const 
    { 
        return std::pair<const Obj&, const Obj&>{key, val}; 
    }
    auto& operator++()
    {
        step(key);
        if (key.valid()) step(val);
        return *this;
    }
    auto operator++(int) 
    { 
        MapIter tmp = *this; 
        ++(*this); 
        return tmp; 
    }
private:
    Obj key;
    Obj val;
};

/**
 * @brief CBOR sequence, read-only wrapper for traversal on-the-fly.
 * 
 */
struct Seq {
    Seq(const byte *p, size_t len) : head{p}, tail{p + len} {}
    SeqIter begin() const   { return {head, tail}; }
    SeqIter end() const     { return {}; }
    size_t size() const     { return tail - head; }
    auto data() const       { return head; }
private:
    const byte *head;
    const byte *tail;
};

inline SeqIter Range::begin() const { return {head, tail}; }
inline SeqIter Range::end() const   { return {}; }
inline MapIter Map::begin() const   { return {head, tail}; }
inline MapIter Map::end() const     { return {}; }
inline Obj Tag::content() const     { return std::get<Obj>(decode(head, tail)); }

}

#endif