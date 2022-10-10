#ifndef UTL_BIT_WINDOW_H
#define UTL_BIT_WINDOW_H

#include "utl/bit.h"

#define BIT_WINDOW_EXP  true

namespace utl {

template<class T = uint32_t>
struct bit_window {

    bit_window() = default;
    bit_window(T seqn) : high{seqn} {}

    constexpr T latest() const
    {
        return high;
    }
#if (BIT_WINDOW_EXP)
    constexpr bool check(T seqn) const
    {
        if (seqn > high)
            return true;
        if (high - seqn - 1 >= bits)
            return false;
        return !get_bit(mask, high - seqn - 1);
    }
#else
    constexpr bool check(T seqn) const
    {
        if (seqn > high)
            return true;
        if (high - seqn >= bits)
            return false;
        return !get_bit(mask, high - seqn);
    }
#endif


#if (BIT_WINDOW_EXP)
    constexpr void update(T seqn)
    {
        if (seqn > high)
            update_latest(seqn);
        else if (high - seqn - 1 < bits)
            set_bit(mask, high - seqn - 1);
    }
#else
    constexpr void update(T seqn)
    {
        if (seqn > high)
            update_latest(seqn);
        else if (high - seqn >= bits)
            return;
        else 
            set_bit(mask, high - seqn);
    }
#endif

    constexpr bool check_and_update(T seqn)
    {
        if (seqn > high) {
            update_latest(seqn);
            return true;
        }
        auto idx = high - seqn;
        if (idx >= bits)
            return false;
        if (get_bit(mask, idx))
            return false;
        set_bit(mask, idx);
        return true;
    }
#if (BIT_WINDOW_EXP)
public:
    constexpr void update_latest(T seqn)
    {
        auto diff = seqn - high;
        mask = diff < bits ? mask << diff : 0;
        high = seqn;
    }
#else
private:
    constexpr void update_latest(T seqn)
    {
        auto diff = seqn - high;
        mask = 1 | (diff < bits ? mask << diff : 0);
        high = seqn;
    }
#endif
#if (BIT_WINDOW_EXP)
public:
    T mask = -1;
    T high = 0;
#else
private:
    T mask = -2;
    T high = 0;
#endif
    static constexpr T bits = sizeof(T) * 8;
    static constexpr T edge = -bits;
};

}

#endif