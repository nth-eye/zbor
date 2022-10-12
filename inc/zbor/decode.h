#ifndef ZBOR_DECODE_H
#define ZBOR_DECODE_H

#include "zbor/base.h"
#include <tuple>

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
constexpr std::tuple<item, err, const byte*> decode(const byte* p, const byte* const end)
{
    if (p >= end)
        return {{}, err_out_of_bounds, end};

    size_t nest = 0;
    size_t skip = 0;
    size_t len;

    byte mt = *p   & 0xe0;
    byte ai = *p++ & 0x1f;
    item obj = type_t(mt >> 5);
    uint64_t val = ai;
    uint64_t size = 0;
    decltype(p) head = nullptr;

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
            obj.type = type_indef_data;
        break;
        case mt_text:
            obj.type = type_indef_text;
        break;
        case mt_array:
        case mt_map:
            size = size_t(-1);
        break;
        default:
            return {{}, err_invalid_indef_mt, p};
        }
        head = p;
        nest = 1;
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
            obj.data = {p, size_t(val)};
            p += val;
        break;
        case mt_text:
            obj.text = {p, size_t(val)};
            p += val;
        break;
        case mt_map:
            head = p;
            size = val;
            skip = val << 1;
        break;
        case mt_array:
            head = p;
            size = val;
            skip = val;
        break;
        case mt_tag:
            head = p;
            size = val;
            skip = 1;
        break;
        case mt_simple:
            switch (ai) 
            {
            case prim_float_16:
                obj.dbl     = std::bit_cast<float>(utl::half_to_float(val));
                obj.type    = type_double;
            break;
            case prim_float_32:
                obj.dbl     = std::bit_cast<float>(uint32_t(val));
                obj.type    = type_double;
            break;
            case prim_float_64:
                obj.dbl     = std::bit_cast<double>(val);
                obj.type    = type_double;
            break;
            default:
                obj.prim    = prim_t(val);
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

        if ((obj.type == type_indef_data || obj.type == type_indef_text) && *p != 0xff) {
            if (obj.type != (mt >> 5) + 7 || val == ai_indef)
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
    case type_array:        obj.arr  = {head, p, size}; break;
    case type_map:          obj.map  = {head, p, size}; break;
    case type_tag:          obj.tag  = {head, p, size}; break;
    case type_indef_data:   obj.istr = {head, p}; break;
    case type_indef_text:   obj.istr = {head, p}; break;
    default:;
    }
    return {obj, err_ok, p};
}

/**
 * @brief Sequence iterator which holds range (begin and end pointers). Used 
 * to traverse CBOR sequence (RFC-8742), which is just series of adjacent 
 * objects. Used to traverse arr_t, istr_t or any series of bytes 
 * as item one by one. Only exception is map_t.
 * 
 */
struct seq_iter {
    constexpr seq_iter() = default;
    constexpr seq_iter(const byte* head, const byte* tail) : head{head}, tail{tail}
    {
        step(key);
    }
    constexpr bool operator!=(const seq_iter&) const 
    { 
        return key.valid();
    }
    constexpr auto& operator*() const 
    { 
        return key; 
    }
    constexpr auto& operator++()
    {
        step(key);
        return *this;
    }
    constexpr auto operator++(int) 
    { 
        auto tmp = *this; 
        ++(*this); 
        return tmp; 
    }
protected:
    constexpr void step(item& o) 
    {
        std::tie(o, std::ignore, head) = decode(head, tail); 
    }
protected:
    const byte* head = nullptr;
    const byte* tail = nullptr;
    item key;
};

/**
 * @brief Map iterator, same as seq_iter, but parses two objects 
 * in a row and returns them as pair.
 * 
 */
struct map_iter : seq_iter {
    constexpr map_iter() = default;
    constexpr map_iter(const byte* head, const byte* tail) : seq_iter{head, tail}
    {
        if (key.valid()) 
            step(val);
    }
    constexpr bool operator!=(const map_iter&) const 
    { 
        return key.valid() && val.valid();
    }
    constexpr auto operator*() const 
    { 
        return std::pair<const item&, const item&>{key, val}; 
    }
    constexpr auto& operator++()
    {
        step(key);
        if (key.valid()) 
            step(val);
        return *this;
    }
    constexpr auto operator++(int) 
    { 
        auto tmp = *this; 
        ++(*this); 
        return tmp; 
    }
private:
    item val;
};

constexpr seq_iter seq::begin() const   { return {data(), data() + size()}; }
constexpr seq_iter seq::end() const     { return {}; }
constexpr map_iter map_t::begin() const { return {data(), data() + seq::size()}; }
constexpr map_iter map_t::end() const   { return {}; }
constexpr item tag_t::content() const   { return std::get<item>(decode(data(), data() + size())); }

}

#endif