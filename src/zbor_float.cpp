#include "zbor_float.h"

namespace {

inline uint32_t u32_dec(uint32_t a) { return a - 1; }
inline uint32_t u32_inc(uint32_t a) { return a + 1; }
inline uint32_t u32_not(uint32_t a) { return ~a; }
inline uint32_t u32_neg(uint32_t a) { return -a; }
inline uint32_t u32_ext(uint32_t a) { return ((int32_t) a) >> 31; }
inline uint32_t u32_and(uint32_t a, uint32_t b) { return a & b; }
inline uint32_t u32_andc(uint32_t a, uint32_t b) { return a & ~b; }
inline uint32_t u32_or(uint32_t a, uint32_t b) { return a | b; }
inline uint32_t u32_srl(uint32_t a, int sa) { return a >> sa; }
inline uint32_t u32_sll(uint32_t a, int sa) { return a << sa; }
inline uint32_t u32_add(uint32_t a, uint32_t b) { return a + b; }
inline uint32_t u32_sub(uint32_t a, uint32_t b) { return a - b; }

// Select on Sign bit
inline uint32_t u32_sels(uint32_t test, uint32_t a, uint32_t b)
{
    const uint32_t mask     = u32_ext(test);
    const uint32_t sel_a    = u32_and(a, mask);
    const uint32_t sel_b    = u32_andc(b, mask);
    const uint32_t result   = u32_or(sel_a, sel_b);

    return result;
}

// Count Leading Zeros
inline uint32_t u32_cntlz(uint32_t x)
{
#ifdef __GNUC__
    /* NOTE: __builtin_clz is undefined for x == 0 */
    /* On PowerPC, this will map to insn: cntlzw   */
    /* On Pentium, this will map to insn: clz      */
    uint32_t is_x_nez_msb = u32_neg(x);
    uint32_t nlz          = __builtin_clz(x);
    uint32_t result       = u32_sels(is_x_nez_msb, nlz, 0x00000020);
    return result;
#else
    const uint32_t x0  = u32_srl(x, 1);
    const uint32_t x1  = u32_or(x, x0);
    const uint32_t x2  = u32_srl(x1, 2);
    const uint32_t x3  = u32_or(x1, x2);
    const uint32_t x4  = u32_srl(x3, 4);
    const uint32_t x5  = u32_or(x3, x4);
    const uint32_t x6  = u32_srl(x5, 8);
    const uint32_t x7  = u32_or(x5, x6);
    const uint32_t x8  = u32_srl(x7, 16);
    const uint32_t x9  = u32_or(x7, x8);
    const uint32_t xA  = u32_not(x9);
    const uint32_t xB  = u32_srl(xA, 1);
    const uint32_t xC  = u32_and(xB, 0x55555555);
    const uint32_t xD  = u32_sub(xA, xC);
    const uint32_t xE  = u32_and(xD, 0x33333333);
    const uint32_t xF  = u32_srl(xD, 2);
    const uint32_t x10 = u32_and(xF, 0x33333333);
    const uint32_t x11 = u32_add(xE, x10);
    const uint32_t x12 = u32_srl(x11, 4);
    const uint32_t x13 = u32_add(x11, x12);
    const uint32_t x14 = u32_and(x13, 0x0f0f0f0f);
    const uint32_t x15 = u32_srl(x14, 8);
    const uint32_t x16 = u32_add(x14, x15);
    const uint32_t x17 = u32_srl(x16, 16);
    const uint32_t x18 = u32_add(x16, x17);
    const uint32_t x19 = u32_and(x18, 0x0000003f);
    return x19;
#endif
}

}

namespace zbor {

uint16_t half_from_float(uint32_t f)
{
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

    return (uint16_t) h_result;
}

uint32_t half_to_float(uint16_t h)
{
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

double half_to_double(uint16_t x) 
{
    unsigned exp = (x >> 10) & 0x1f;
    unsigned mant = x & 0x3ff;
    double val;
    if (exp == 0) 
        val = ldexp(mant, -24);
    else if (exp != 31) 
        val = ldexp(mant + 1024, exp - 25);
    else 
        val = mant == 0 ? INFINITY : NAN;
    return x & 0x8000 ? -val : val;
}

}