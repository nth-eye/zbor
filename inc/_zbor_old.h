// #ifndef ZBOR_H
// #define ZBOR_H

// #include "zbor_base.h"
// #include "zbor_float.h"

// namespace zbor {

// struct DecodeResult {
//     CBOR *root;
//     size_t len;
//     Err err;
// };

// struct StackItem {
//     CBOR *item;
//     size_t cnt;
// };

// template<size_t N>
// struct Stack {
//     bool push(StackItem item) 
//     { 
//         if (size >= N)
//             return false;
//         stack[size++] = item;
//         return true;
//     }
//     void pop(StackItem &item) 
//     {
//         item = stack[--size];
//     }
// private:
//     StackItem stack[N];
//     size_t size = 0;
// };

// template<size_t N>
// DecodeResult decode(Pool<N> &pool, const uint8_t *buf, size_t buf_len)
// {
//     pool.clear();

//     CBOR *root  = nullptr;
//     CBOR *prev  = nullptr;
//     CBOR *item  = nullptr;
//     size_t cnt  = 0;

//     auto ret = [&](Err err) { return DecodeResult{root, cnt, err}; };

//     if (!buf || !buf_len)
//         return ret(ERR_INVALID_PARAM);

//     Float fp;
//     Stack<N> stack;
//     StackItem parent    = {nullptr, 0};
//     const uint8_t *p    = buf;
//     const uint8_t *end  = buf + buf_len;

//     while (p < end) {

//         item = pool.make();

//         if (!item)
//             return ret(ERR_OUT_OF_MEM);

//         if (!root)
//             root = item;

//         if (parent.item) {
//             parent.item->arr.push(item);
//             parent.cnt--;
//         } else {
//             cnt++;
//         }

//         if (prev && !parent.item) {
//             prev->next = item;
//             item->prev = prev;
//         }

//         uint8_t mt = *p     & 0xe0;
//         uint8_t ai = *p++   & 0x1f;
//         uint64_t val = ai;

//         item->type = Type(mt >> 5);

//         switch (ai) {
//         case AI_1:
//         case AI_2:
//         case AI_4:
//         case AI_8: {
//             size_t len = bit(ai - AI_1);
//             if (p + len > end)
//                 return ret(ERR_OUT_OF_DATA);
//             val = 0;
//             for (int i = 8 * len - 8; i >= 0; i -= 8)
//                 val |= ((uint64_t) *p++) << i;
//             break;
//         }
//         case 28:
//         case 29:
//         case 30:
//             return ret(ERR_INVALID_DATA);
//             break;
//         case AI_INDEF:
//             return ret(ERR_INVALID_DATA);
//             break;
//         }

//         switch (mt) {
//         case MT_UINT:
//             item->uint = val;
//             break;
//         case MT_NINT:
//             item->sint = ~val;
//             break;
//         case MT_DATA:
//         case MT_TEXT:
//             if (p + val > end)
//                 return ret(ERR_OUT_OF_DATA);
//             item->str.data   = p;
//             item->str.len    = val;
//             p += val;
//             break;
//         case MT_MAP:
//             val <<= 1;
//         case MT_ARRAY:
//             item->arr.clear();
//             stack.push(parent);
//             parent = {item, val};
//             break;
//         case MT_TAG:
//             return ret(ERR_INVALID_DATA);
//         case MT_SIMPLE:
//             switch (ai) {
//             case PRIM_FLOAT_16:
//                 item->d     = half_to_double(val);
//                 item->type  = TYPE_DOUBLE;
//                 break;
//             case PRIM_FLOAT_32:
//                 fp.u32      = val;
//                 item->d     = fp.f32;
//                 item->type  = TYPE_DOUBLE;
//                 break;
//             case PRIM_FLOAT_64:
//                 fp.u64      = val;
//                 item->d     = fp.f64;
//                 item->type  = TYPE_DOUBLE;
//                 break;
//             default:
//                 item->prim = Prim(val);
//                 if (val >= 24 &&
//                     val <= 31)
//                     return ret(ERR_INVALID_DATA);
//                 break;
//             }
//             break;
//         default:
//             return ret(ERR_INVALID_DATA);
//             break;
//         }

//         while (parent.item && !parent.cnt) {
//             item = parent.item;
//             stack.pop(parent);
//         }
//         prev = item;
//     }
//     return ret(NO_ERR);
// }

// #endif
