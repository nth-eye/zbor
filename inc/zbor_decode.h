#ifndef ZBOR_DECODE_H
#define ZBOR_DECODE_H

#include "zbor_base.h"
#include "zbor_float.h"

namespace zbor {

template<size_t N, size_t D = N>
Sequence decode(Pool<N> &pool, const uint8_t *buf, size_t len)
{
    struct Item {
        CBOR *item;
        size_t cnt;
    };
    CBOR *root  = nullptr;
    CBOR *prev  = nullptr;
    CBOR *item  = nullptr;
    size_t cnt  = 0;

    auto ret = [&](Err err) { return Sequence{root, cnt, err}; };

    if (!buf || !len)
        return ret(ERR_INVALID_PARAM);

    Float fp;
    Stack<Item, D> stack;
    Item parent = {nullptr, 0};
    auto p      = buf;
    auto end    = buf + len;

    while (p < end) {

        item = pool.make();

        if (!item)
            return ret(ERR_OUT_OF_MEM);

        if (!root)
            root = item;

        if (parent.item) {
            switch (parent.item->type) {
                case TYPE_ARRAY:
                case TYPE_MAP: parent.item->arr.push(item); break;
                case TYPE_TAG: parent.item->tag.content = item; break;
                default: return ret(ERR_INVALID_DATA);
            }
            parent.cnt--;
        } else {
            cnt++;
        }

        if (prev && !parent.item) {
            prev->next = item;
            item->prev = prev;
        }

        uint8_t mt = *p & 0xe0;
        uint8_t ai = *p++ & 0x1f;
        uint64_t val = ai;

        item->type = Type(mt >> 5);

        switch (ai) {
        case AI_1:
        case AI_2:
        case AI_4:
        case AI_8: {
            size_t len = bit(ai - AI_1);
            if (p + len > end)
                return ret(ERR_OUT_OF_DATA);
            val = 0;
            for (int i = 8 * len - 8; i >= 0; i -= 8)
                val |= ((uint64_t) *p++) << i;
            break;
        }
        case 28:
        case 29:
        case 30:
            return ret(ERR_INVALID_DATA);
            break;
        case AI_INDEF:
            return ret(ERR_INVALID_DATA);
            break;
        }

        switch (mt) {
        case MT_UINT:
            item->uint = val;
            break;
        case MT_NINT:
            item->sint = ~val;
            break;
        case MT_DATA:
        case MT_TEXT:
            if (p + val > end)
                return ret(ERR_OUT_OF_DATA);
            item->str.dat = p;
            item->str.len = val;
            p += val;
            break;
        case MT_MAP:
            val <<= 1;
        case MT_ARRAY:
            stack.push(parent);
            item->arr.init();
            parent = {item, val};
            break;
        case MT_TAG:
            stack.push(parent);
            item->tag.val = val;
            parent = {item, 1};
            break;
        case MT_SIMPLE:
            switch (ai) {
            case PRIM_FLOAT_16:
                fp.u32      = half_to_float(val);
                item->dbl   = fp.f32; // half_to_double_direct(val);
                item->type  = TYPE_DOUBLE;
                break;
            case PRIM_FLOAT_32:
                fp.u32      = val;
                item->dbl   = fp.f32;
                item->type  = TYPE_DOUBLE;
                break;
            case PRIM_FLOAT_64:
                fp.u64      = val;
                item->dbl   = fp.f64;
                item->type  = TYPE_DOUBLE;
                break;
            default:
                item->prim = Prim(val);
                if (val >= 24 &&
                    val <= 31)
                    return ret(ERR_INVALID_DATA);
                break;
            }
            break;
        default:
            return ret(ERR_INVALID_DATA);
        }

        while (parent.item && !parent.cnt) {
            item = parent.item;
            stack.pop(parent);
        }
        prev = item;
    }
    return ret(NO_ERR);
}

}

#endif