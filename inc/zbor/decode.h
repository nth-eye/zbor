#ifndef ZBOR_DECODE_H
#define ZBOR_DECODE_H

#include "zbor/base.h"
#include "zbor/float.h"

namespace zbor {

/**
 * @brief Decode data into Obj objects, allocated from
 * static pool. IMPORTANT: Indefinite string (data/text) 
 * support is limited. Although their presence doesn't 
 * break decoding, chunks are parsed as separate objects, 
 * stored in special ChunkData/ChunkText members and not 
 * concatenated afterwards. Special zbor::Type are made 
 * for these: TYPE_DATA_CHUNKS and TYPE_TEXT_CHUNKS.
 * 
 * @tparam N Pool size
 * @tparam D Maximum depth, by default equals N
 * @param pool Obj object pool
 * @param buf Input data to parse
 * @param len Data length
 * @return Top level object sequence if ok, if not - error is stored
 */
template<size_t N, size_t D = N>
Sequence decode(Pool<N> &pool, const uint8_t *buf, size_t len)
{
    struct Item {
        Obj *item;
        size_t cnt;
    };
    Obj *root   = nullptr;
    Obj *prev   = nullptr;
    Obj *item   = nullptr;
    size_t cnt  = 0;
    bool indef  = false;

    auto ret = [&](Err err) { return Sequence{root, cnt, err}; };

    if (!buf || !len)
        return ret(ERR_INVALID_PARAM);

    Float fp;
    Stack<Item, D> stack;
    Item parent = {nullptr, 0};
    auto p      = buf;
    auto end    = buf + len;

    while (p < end) {

        if (*p == 0xff) {
            ++p;
            indef = false;
            if (!parent.item)
                return ret(ERR_INVALID_DATA);
            item = parent.item;
            stack.pop(parent);
            prev = item;
            continue;
        }

        item = pool.make();

        if (!item)
            return ret(ERR_OUT_OF_MEM);

        if (!root)
            root = item;

        uint8_t mt = *p & 0xe0;
        uint8_t ai = *p++ & 0x1f;
        uint64_t val = ai;
        
        if (indef) {
            if (ai == AI_INDEF)
                return ret(ERR_INVALID_DATA);
            if ((mt >> 5) != parent.item->type - TYPE_DATA_CHUNKS + TYPE_DATA) {
                pool.free(item);
                return ret(ERR_INVALID_DATA);
            }
        }

        if (parent.item) {
            switch (parent.item->type) {
                case TYPE_DATA_CHUNKS:
                case TYPE_TEXT_CHUNKS:
                case TYPE_ARRAY:
                case TYPE_MAP: parent.item->arr.push(item); break;
                case TYPE_TAG: parent.item->tag.content = item; break;
                default:
                    pool.free(item); 
                    return ret(ERR_INVALID_DATA);
            }
            parent.cnt--;
        } else {
            cnt++;
        }

        if (prev && !parent.item) {
            prev->next = item;
            item->prev = prev;
        }
        item->type = Type(mt >> 5);

        switch (ai) {
        case AI_1:
        case AI_2:
        case AI_4:
        case AI_8: 
        {
            size_t len = bit(ai - AI_1);
            if (p + len > end)
                return ret(ERR_OUT_OF_DATA);
            val = 0;
            for (int i = 8 * len - 8; i >= 0; i -= 8)
                val |= ((uint64_t) *p++) << i;
        }
        break;
        case 28:
        case 29:
        case 30:
            return ret(ERR_INVALID_DATA);
        break;
        case AI_INDEF:
            if (mt == MT_DATA || mt == MT_TEXT) {
                if (!stack.push(parent))
                    ret(ERR_DEPTH_EXCEEDED);
                item->type = Type(item->type + TYPE_DATA_CHUNKS - TYPE_DATA);
                item->arr.init();
                val = -1;
                parent = {item, val};
                indef = true;
                continue;
            } else if (mt == MT_ARRAY || mt == MT_MAP) {
                val = -1;
            } else {
                return ret(ERR_INVALID_DATA);
            }
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
            if (!stack.push(parent))
                ret(ERR_DEPTH_EXCEEDED);
            item->arr.init();
            parent = {item, val};
        break;
        case MT_TAG:
            if (!stack.push(parent))
                ret(ERR_DEPTH_EXCEEDED);
            item->tag.val = val;
            parent = {item, 1};
        break;
        case MT_SIMPLE:
            switch (ai) {
            case PRIM_FLOAT_16:
                fp.u32      = half_to_float(val);
                item->dbl   = fp.f32;
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
    if (parent.item || indef)
        return ret(ERR_INVALID_DATA);
    return ret(ERR_OK);
}

}

#endif
