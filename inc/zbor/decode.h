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
 * over its elements, decoding of malformed nested array will report ERR_OUT_OF_BOUNDS.
 * 
 * @param p Begin pointer, must be valid pointer
 * @param end End pointer, must be valid pointer
 * @return Tuple with decoded object, error status and pointer to character past the last character interpreted
 */
inline std::tuple<Obj, Err, const byte*> decode(const byte *p, const byte * const end)
{
    if (p >= end)
        return {{}, ERR_OUT_OF_BOUNDS, end};

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
    case AI_1:
    case AI_2:
    case AI_4:
    case AI_8: 
        len = utl::bit(ai - AI_1);
        if (p + len > end)
            return {{}, ERR_OUT_OF_BOUNDS, p};
        val = 0;
        for (int i = 8 * len - 8; i >= 0; i -= 8)
            val |= uint64_t(*p++) << i;
    break;
    case 28:
    case 29:
    case 30:
        return {{}, ERR_RESERVED_AI, p};
    case AI_INDEF:
        switch (mt)
        {
        case MT_DATA: 
            obj.istr = {p};
            obj.type = TYPE_INDEF_DATA;
            nest = 1;
        break;
        case MT_TEXT:
            obj.istr = {p};
            obj.type = TYPE_INDEF_TEXT;
            nest = 1;
        break;
        case MT_ARRAY:
            obj.arr = {p, size_t(-1)};
            nest = 1;
        break;
        case MT_MAP:
            obj.map = {p, size_t(-1)};
            nest = 1;
        break;
        default:
            return {{}, ERR_INVALID_INDEF_MT, p};
        }
    }

    if (ai != AI_INDEF) {
        switch (mt) 
        {
        case MT_UINT:
            obj.uint = val;
        break;
        case MT_NINT:
            obj.sint = ~val;
        break;
        case MT_DATA:
        case MT_TEXT:
            obj.str.dat = p;
            obj.str.len = val;
            p += val;
        break;
        case MT_MAP:
            obj.map = {p, val};
            skip = val << 1;
        break;
        case MT_ARRAY:
            obj.arr = {p, val};
            skip = val;
        break;
        case MT_TAG:
            obj.tag = {p, val};
            skip = 1;
        break;
        case MT_SIMPLE:
            switch (ai) 
            {
            case PRIM_FLOAT_16:
                obj.dbl     = utl::Float(utl::half_to_float(val)).f32;
                obj.type    = TYPE_DOUBLE;
            break;
            case PRIM_FLOAT_32:
                obj.dbl     = utl::Float(uint32_t(val)).f32;
                obj.type    = TYPE_DOUBLE;
            break;
            case PRIM_FLOAT_64:
                obj.dbl     = utl::Float(val).f64;
                obj.type    = TYPE_DOUBLE;
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
            return {{}, ERR_OUT_OF_BOUNDS, end};

        mt  = *p & 0xe0;
        val = *p & 0x1f;
        
        if (obj.type == TYPE_INDEF_DATA) {
            if (!(mt == MT_DATA && val != AI_INDEF) && *p != 0xff)
                return {{}, ERR_INVALID_INDEF_ITEM, p};
        } else if (obj.type == TYPE_INDEF_TEXT) {
            if (!(mt == MT_TEXT && val != AI_INDEF) && *p != 0xff)
                return {{}, ERR_INVALID_INDEF_ITEM, p};
        }
        ++p;

        switch (val)
        {
        case AI_1:
        case AI_2:
        case AI_4:
        case AI_8: 
            len = utl::bit(val - AI_1);
            if (p + len > end)
                return {{}, ERR_OUT_OF_BOUNDS, p};
            val = 0;
            for (int i = 8 * len - 8; i >= 0; i -= 8)
                val |= uint64_t(*p++) << i;
        break;
        case 28:
        case 29:
        case 30:
            return {{}, ERR_RESERVED_AI, p};
        case AI_INDEF:
            switch (mt)
            {
            case MT_ARRAY:
            case MT_MAP:
                ++nest;
            break;
            case MT_SIMPLE:
                if (nest)
                    nest--;
                else
                    return {{}, ERR_BREAK_WITHOUT_START, p};
            break;
            default:
                return {{}, ERR_INVALID_INDEF_MT, p};
            }
        }

        switch (mt)
        {
        case MT_UINT:
        case MT_NINT:
        case MT_SIMPLE: 
        break;
        case MT_DATA:
        case MT_TEXT:
            p += val;
        break;
        case MT_ARRAY:
            if (!nest)
                skip += val;
        break;
        case MT_MAP:
            if (!nest)
                skip += val << 1;
        break;
        case MT_TAG:
            if (!nest)
                skip += 1;
        break;
        }

        if (skip && !nest)
            skip--;
    }

    switch (obj.type) 
    {
    case TYPE_ARRAY:        obj.arr.set_end(p); break;
    case TYPE_MAP:          obj.map.set_end(p); break;
    case TYPE_TAG:          obj.tag.set_end(p); break;
    case TYPE_INDEF_DATA:
    case TYPE_INDEF_TEXT:   obj.istr.set_end(p); break;
    default:;
    }
    return {obj, ERR_OK, p};
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

inline SeqIter IndefString::begin() const   { return {head, tail}; }
inline SeqIter IndefString::end() const     { return {}; }
inline MapIter Map::begin() const           { return {head, tail}; }
inline MapIter Map::end() const             { return {}; }
inline Obj Tag::content() const             { return std::get<Obj>(decode(head, tail)); }

}

#endif