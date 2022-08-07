#ifndef UTL_FLOAT_H
#define UTL_FLOAT_H

#include "utl/base.h"

namespace utl {
namespace impl {

constexpr uint32_t u32_dec(uint32_t a)              { return a - 1; }
constexpr uint32_t u32_inc(uint32_t a)              { return a + 1; }
constexpr uint32_t u32_not(uint32_t a)              { return ~a; }
constexpr uint32_t u32_neg(uint32_t a)              { return -a; }
constexpr uint32_t u32_ext(uint32_t a)              { return ((int32_t) a) >> 31; }
constexpr uint32_t u32_or(uint32_t a, uint32_t b)   { return a | b; }
constexpr uint32_t u32_add(uint32_t a, uint32_t b)  { return a + b; }
constexpr uint32_t u32_sub(uint32_t a, uint32_t b)  { return a - b; }
constexpr uint32_t u32_and(uint32_t a, uint32_t b)  { return a & b; }
constexpr uint32_t u32_andc(uint32_t a, uint32_t b) { return a & ~b; }
constexpr uint32_t u32_srl(uint32_t a, int sa)      { return a >> sa; }
constexpr uint32_t u32_sll(uint32_t a, int sa)      { return a << sa; }

/**
 * @brief Select on Sign bit
 * 
 * @param test Value to check sign bit
 * @param a Select on negative
 * @param b Select on positive
 * @return Selected value 
 */
constexpr uint32_t u32_sels(uint32_t test, uint32_t a, uint32_t b)
{
    const uint32_t mask     = u32_ext(test);
    const uint32_t sel_a    = u32_and(a, mask);
    const uint32_t sel_b    = u32_andc(b, mask);
    const uint32_t result   = u32_or(sel_a, sel_b);

    return result;
}

/**
 * @brief Count leading zeros
 * 
 * @param x Argument
 * @return Number of leading zeros
 */
constexpr uint32_t u32_cntlz(uint32_t x)
{
    if (!x) return 32;
#ifdef __GNUC__
    return __builtin_clz(x);
#else
    constexpr char debruijn32[32] = {
        0, 31, 9, 30, 3, 8, 13, 29, 2, 5, 7, 21, 12, 24, 28, 19,
        1, 10, 4, 14, 6, 22, 25, 20, 11, 15, 23, 26, 16, 27, 17, 18
    };
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return debruijn32[++x * 0x076be629 >> 27];
#endif
}

}

/**
 * @brief Union to access bit patterns of floating point numbers.
 * 
 */
union fp_bits {
    constexpr fp_bits() : u64{0} {};
    constexpr fp_bits(float f) : f32{f} {}
    constexpr fp_bits(double f) : f64{f} {}
    constexpr fp_bits(uint16_t u) : u16{u} {}
    constexpr fp_bits(uint32_t u) : u32{u} {}
    constexpr fp_bits(uint64_t u) : u64{u} {} 
    uint16_t    u16;
    uint32_t    u32;
    float       f32;
    uint64_t    u64;
    double      f64;
};

/**
 * @brief Convert single precision floating point to half precision.
 * 
 * @param f Single stored as uint32_t
 * @return Half stored as uint16_t
 */
constexpr uint16_t float_to_half(uint32_t f)
{
    using namespace impl;

    constexpr uint32_t one                      = 0x00000001;
    constexpr uint32_t f_s_mask                 = 0x80000000;
    constexpr uint32_t f_e_mask                 = 0x7f800000;
    constexpr uint32_t f_m_mask                 = 0x007fffff;
    constexpr uint32_t f_m_hidden_bit           = 0x00800000;
    constexpr uint32_t f_m_round_bit            = 0x00001000;
    constexpr uint32_t f_snan_mask              = 0x7fc00000;
    constexpr uint32_t f_e_pos                  = 0x00000017;
    constexpr uint32_t h_e_pos                  = 0x0000000a;
    constexpr uint32_t h_e_mask                 = 0x00007c00;
    constexpr uint32_t h_snan_mask              = 0x00007e00;
    constexpr uint32_t h_e_mask_value           = 0x0000001f;
    constexpr uint32_t f_h_s_pos_offset         = 0x00000010;
    constexpr uint32_t f_h_bias_offset          = 0x00000070;
    constexpr uint32_t f_h_m_pos_offset         = 0x0000000d;
    constexpr uint32_t h_nan_min                = 0x00007c01;
    constexpr uint32_t f_h_e_biased_flag        = 0x0000008f;
    const uint32_t f_s                          = u32_and(f, f_s_mask);
    const uint32_t f_e                          = u32_and(f, f_e_mask);
    const uint16_t h_s                          = u32_srl(f_s, f_h_s_pos_offset);
    const uint32_t f_m                          = u32_and(f, f_m_mask);
    const uint16_t f_e_amount                   = u32_srl(f_e, f_e_pos);
    const uint32_t f_e_half_bias                = u32_sub(f_e_amount, f_h_bias_offset);
    const uint32_t f_snan                       = u32_and(f, f_snan_mask);
    const uint32_t f_m_round_mask               = u32_and(f_m, f_m_round_bit);
    const uint32_t f_m_round_offset             = u32_sll(f_m_round_mask, one);
    const uint32_t f_m_rounded                  = u32_add(f_m, f_m_round_offset);
    const uint32_t f_m_denorm_sa                = u32_sub(one, f_e_half_bias);
    const uint32_t f_m_with_hidden              = u32_or(f_m_rounded, f_m_hidden_bit);
    const uint32_t f_m_denorm                   = u32_srl(f_m_with_hidden, f_m_denorm_sa);
    const uint32_t h_m_denorm                   = u32_srl(f_m_denorm, f_h_m_pos_offset);
    const uint32_t f_m_rounded_overflow         = u32_and(f_m_rounded, f_m_hidden_bit);
    const uint32_t m_nan                        = u32_srl(f_m, f_h_m_pos_offset);
    const uint32_t h_em_nan                     = u32_or(h_e_mask, m_nan);
    const uint32_t h_e_norm_overflow_offset     = u32_inc(f_e_half_bias);
    const uint32_t h_e_norm_overflow            = u32_sll(h_e_norm_overflow_offset, h_e_pos);
    const uint32_t h_e_norm                     = u32_sll(f_e_half_bias, h_e_pos);
    const uint32_t h_m_norm                     = u32_srl(f_m_rounded, f_h_m_pos_offset);
    const uint32_t h_em_norm                    = u32_or(h_e_norm, h_m_norm);
    const uint32_t is_h_ndenorm_msb             = u32_sub(f_h_bias_offset, f_e_amount);
    const uint32_t is_f_e_flagged_msb           = u32_sub(f_h_e_biased_flag, f_e_half_bias);
    const uint32_t is_h_denorm_msb              = u32_not(is_h_ndenorm_msb);
    const uint32_t is_f_m_eqz_msb               = u32_dec(f_m);
    const uint32_t is_h_nan_eqz_msb             = u32_dec(m_nan);
    const uint32_t is_f_inf_msb                 = u32_and(is_f_e_flagged_msb, is_f_m_eqz_msb);
    const uint32_t is_f_nan_underflow_msb       = u32_and(is_f_e_flagged_msb, is_h_nan_eqz_msb);
    const uint32_t is_e_overflow_msb            = u32_sub(h_e_mask_value, f_e_half_bias);
    const uint32_t is_h_inf_msb                 = u32_or(is_e_overflow_msb, is_f_inf_msb);
    const uint32_t is_f_nsnan_msb               = u32_sub(f_snan, f_snan_mask);
    const uint32_t is_m_norm_overflow_msb       = u32_neg(f_m_rounded_overflow);
    const uint32_t is_f_snan_msb                = u32_not(is_f_nsnan_msb);
    const uint32_t h_em_overflow_result         = u32_sels(is_m_norm_overflow_msb, h_e_norm_overflow, h_em_norm);
    const uint32_t h_em_nan_result              = u32_sels(is_f_e_flagged_msb, h_em_nan, h_em_overflow_result);
    const uint32_t h_em_nan_underflow_result    = u32_sels(is_f_nan_underflow_msb, h_nan_min, h_em_nan_result);
    const uint32_t h_em_inf_result              = u32_sels(is_h_inf_msb, h_e_mask, h_em_nan_underflow_result);
    const uint32_t h_em_denorm_result           = u32_sels(is_h_denorm_msb, h_m_denorm, h_em_inf_result);
    const uint32_t h_em_snan_result             = u32_sels(is_f_snan_msb, h_snan_mask, h_em_denorm_result);
    const uint32_t h_result                     = u32_or(h_s, h_em_snan_result);

    return h_result;
}

/**
 * @brief Convert half precision floating point to single precision.
 * 
 * @param h Half stored as uint16_t
 * @return Single stored as uint32_t
 */
constexpr uint32_t half_to_float(uint16_t h)
{
    using namespace impl;

    constexpr uint32_t h_e_mask             = 0x00007c00;
    constexpr uint32_t h_m_mask             = 0x000003ff;
    constexpr uint32_t h_s_mask             = 0x00008000;
    constexpr uint32_t h_f_s_pos_offset     = 0x00000010;
    constexpr uint32_t h_f_e_pos_offset     = 0x0000000d;
    constexpr uint32_t h_f_bias_offset      = 0x0001c000;
    constexpr uint32_t f_e_mask             = 0x7f800000;
    constexpr uint32_t f_m_mask             = 0x007fffff;
    constexpr uint32_t h_f_e_denorm_bias    = 0x0000007e;
    constexpr uint32_t h_f_m_denorm_sa_bias = 0x00000008;
    constexpr uint32_t f_e_pos              = 0x00000017;
    constexpr uint32_t h_e_mask_minus_one   = 0x00007bff;
    const uint32_t h_e                      = u32_and(h, h_e_mask);
    const uint32_t h_m                      = u32_and(h, h_m_mask);
    const uint32_t h_s                      = u32_and(h, h_s_mask);
    const uint32_t h_e_f_bias               = u32_add(h_e, h_f_bias_offset);
    const uint32_t h_m_nlz                  = u32_cntlz(h_m);
    const uint32_t f_s                      = u32_sll(h_s, h_f_s_pos_offset);
    const uint32_t f_e                      = u32_sll(h_e_f_bias, h_f_e_pos_offset);
    const uint32_t f_m                      = u32_sll(h_m, h_f_e_pos_offset);
    const uint32_t f_em                     = u32_or(f_e, f_m);
    const uint32_t h_f_m_sa                 = u32_sub(h_m_nlz, h_f_m_denorm_sa_bias);
    const uint32_t f_e_denorm_unpacked      = u32_sub(h_f_e_denorm_bias, h_f_m_sa);
    const uint32_t h_f_m                    = u32_sll(h_m, h_f_m_sa);
    const uint32_t f_m_denorm               = u32_and(h_f_m, f_m_mask);
    const uint32_t f_e_denorm               = u32_sll(f_e_denorm_unpacked, f_e_pos);
    const uint32_t f_em_denorm              = u32_or(f_e_denorm, f_m_denorm);
    const uint32_t f_em_nan                 = u32_or(f_e_mask, f_m);
    const uint32_t is_e_eqz_msb             = u32_dec(h_e);
    const uint32_t is_m_nez_msb             = u32_neg(h_m);
    const uint32_t is_e_flagged_msb         = u32_sub(h_e_mask_minus_one, h_e);
    const uint32_t is_zero_msb              = u32_andc(is_e_eqz_msb, is_m_nez_msb);
    const uint32_t is_inf_msb               = u32_andc(is_e_flagged_msb, is_m_nez_msb);
    const uint32_t is_denorm_msb            = u32_and(is_m_nez_msb, is_e_eqz_msb);
    const uint32_t is_nan_msb               = u32_and(is_e_flagged_msb, is_m_nez_msb); 
    const uint32_t is_zero                  = u32_ext(is_zero_msb);
    const uint32_t f_zero_result            = u32_andc(f_em, is_zero);
    const uint32_t f_denorm_result          = u32_sels(is_denorm_msb, f_em_denorm, f_zero_result);
    const uint32_t f_inf_result             = u32_sels(is_inf_msb, f_e_mask, f_denorm_result);
    const uint32_t f_nan_result             = u32_sels(is_nan_msb, f_em_nan, f_inf_result);
    const uint32_t f_result                 = u32_or(f_s, f_nan_result);

    return f_result;
}

/**
 * @brief Convert double precision floating point to half precision.
 * 
 * @param d Double stored as uint64_t
 * @return Half stored as uint16_t
 */
constexpr uint16_t double_to_half(uint64_t d)
{
    fp_bits fp = d;
    fp.f32 = fp.f64;
    return float_to_half(fp.u32);
}

/**
 * @brief Convert half precision floating point to double precision.
 * 
 * @param h Half stored as uint16_t
 * @return Double stored as uint64_t
 */
constexpr uint64_t half_to_double(uint16_t h)
{
    fp_bits fp = half_to_float(h);
    fp.f64 = fp.f32;
    return fp.u64;
}

}

#endif