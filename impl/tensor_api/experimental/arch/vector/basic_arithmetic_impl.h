/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/tensor_api/experimental/arch/vector/basic_arithmetic_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"tensor_api/experimental/arch/vector/basic_arithmetic.h\"\" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_BASIC_ARITHMETIC_IMPL_H
#endif

#ifndef IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_BASIC_ARITHMETIC_IMPL_H
#define IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_BASIC_ARITHMETIC_IMPL_H

#include "impl/tensor_api/experimental/arch/utils/reg_utils.h"
#include "tensor_api/experimental/arch/vector/mask_reg_compute.h"

namespace asc {
namespace te {
namespace experimental {
namespace detail {

constexpr uint32_t float_sign_bit = 0x80000000u;
constexpr uint32_t float_exponent_mask = 0x7F800000u;
constexpr int32_t float_exponent_shift = 23;
constexpr int32_t float_exponent_bias = 127;
constexpr int32_t float_min_normal_exponent = -126;
constexpr int32_t float_max_normal_exponent = 127;
constexpr int32_t float_normalization_scale_exponent = 23;
constexpr int32_t float_mantissa_drop_width = 24;

constexpr int16_t half_exponent_shift = 10;
constexpr int16_t half_exponent_bias = 15;
constexpr int16_t half_scale_underflow_bound = -9;
constexpr int16_t half_scale_overflow_bound = 31;

__simd_callee__ inline vector_float div_flush_subnormal(vector_float src, vector_bool mask)
{
    vector_uint32_t bits;
    vector_uint32_t exponent;
    vector_uint32_t exponent_mask;
    vector_uint32_t sign;
    vector_uint32_t zero;
    vector_bool exponent_is_zero;
    vector_float dst;
    asc_duplicate_scalar(exponent_mask, float_exponent_mask, mask);
    asc_duplicate_scalar(sign, float_sign_bit, mask);
    asc_duplicate_scalar(zero, 0u, mask);
    asc_and(exponent, (vector_uint32_t&)src, exponent_mask, mask);
    asc_eq(exponent_is_zero, exponent, zero, mask);
    asc_and(bits, (vector_uint32_t&)src, sign, mask);
    asc_select(dst, (vector_float&)bits, src, exponent_is_zero);
    return dst;
}

__simd_callee__ inline void div_flush_subnormal_half(vector_half& dst, vector_bool mask)
{
    constexpr uint16_t sign_bit = 0x8000u;
    constexpr uint16_t exponent_mask = 0x7C00u;

    vector_uint16_t exponent;
    vector_uint16_t sign;
    vector_uint16_t zero;
    vector_bool exponent_is_zero;

    asc_duplicate_scalar(exponent, exponent_mask, mask);
    asc_duplicate_scalar(sign, sign_bit, mask);
    asc_duplicate_scalar(zero, 0u, mask);
    asc_and(exponent, (vector_uint16_t&)dst, exponent, mask);
    asc_eq(exponent_is_zero, exponent, zero, mask);
    asc_and(sign, (vector_uint16_t&)dst, sign, mask);
    asc_select(dst, (vector_half&)sign, dst, exponent_is_zero);
}

__simd_callee__ inline void div_precision(
    vector_float& dst, vector_float src0, vector_float src1, vector_bool mask, bool force_ftz)
{
    constexpr uint32_t inf_nan_bound = 0xff800000u;
    constexpr int32_t precision_threshold = -64;

    if (force_ftz) {
        src0 = div_flush_subnormal(src0, mask);
        src1 = div_flush_subnormal(src1, mask);
    }

    vector_float z;
    vector_float tmp_dst;
    vector_float r;
    vector_float y;
    vector_float r_pre;
    vector_float r_next;
    vector_float z_pre;
    vector_float z_next;
    vector_float sign_vector;
    vector_uint32_t inf_nan;
    vector_uint32_t src0_reg;
    vector_uint32_t src0_exp_bits;
    vector_uint32_t scale_bits;
    vector_int32_t src0_exp;
    vector_int32_t k;
    vector_int32_t threshold_vec;
    vector_int32_t new_exp;
    vector_bool cmp_mask;
    vector_bool inf_nan_mask;
    vector_bool zero_mask;
    vector_bool need_scale_mask;

    asc_duplicate_scalar((vector_uint32_t&)sign_vector, float_sign_bit, mask);
    asc_div(z, src0, src1, mask);
    asc_or(inf_nan, (vector_uint32_t&)z, (vector_uint32_t&)sign_vector, mask);
    tmp_dst = z;
    asc_eq_scalar(zero_mask, z, 0.0f, mask);
    asc_ge_scalar(inf_nan_mask, (vector_uint32_t&)inf_nan, inf_nan_bound, mask);
    asc_or(inf_nan_mask, inf_nan_mask, zero_mask, mask);

    asc_duplicate_scalar(src0_reg, float_exponent_mask, mask);
    asc_and(src0_exp_bits, (vector_uint32_t&)src0, src0_reg, mask);
    asc_shiftright_scalar(src0_exp_bits, src0_exp_bits, float_exponent_shift, mask);
    asc_add_scalar(src0_exp, (vector_int32_t&)src0_exp_bits, -float_exponent_bias, mask);
    asc_lt_scalar(need_scale_mask, src0_exp, precision_threshold, mask);

    asc_duplicate_scalar(threshold_vec, precision_threshold, mask);
    asc_sub(k, threshold_vec, src0_exp, need_scale_mask);
    asc_add_scalar(new_exp, k, float_exponent_bias, mask);
    asc_shiftleft_scalar(scale_bits, (vector_uint32_t&)new_exp, float_exponent_shift, mask);

    asc_mul((vector_float&)src0_reg, src0, (vector_float&)scale_bits, mask);
    asc_mul((vector_float&)src0_exp_bits, src1, (vector_float&)scale_bits, mask);
    vector_float a_scaled = (vector_float&)src0_reg;
    vector_float b_scaled = (vector_float&)src0_exp_bits;

    asc_mul_scalar(y, b_scaled, -1.0f, mask);
    r = a_scaled;
    asc_mula(r, z, y, mask);
    r_pre = a_scaled;
    r_next = a_scaled;
    asc_add_scalar((vector_int32_t&)z_pre, (vector_int32_t&)z, -1, mask);
    asc_add_scalar((vector_int32_t&)z_next, (vector_int32_t&)z, 1, mask);
    asc_mula(r_pre, z_pre, y, mask);
    asc_mula(r_next, z_next, y, mask);
    asc_abs(r, r, mask);
    asc_abs(r_pre, r_pre, mask);
    asc_abs(r_next, r_next, mask);
    asc_lt(cmp_mask, r, r_pre, mask);
    asc_select(r, r, r_pre, cmp_mask);
    asc_select(z, z, z_pre, cmp_mask);
    asc_lt(cmp_mask, r_next, r, mask);
    asc_select(z, z_next, z, cmp_mask);
    asc_select(dst, tmp_dst, z, inf_nan_mask);
}

__simd_callee__ inline void div_sticky(
    vector_bool& sticky_positive, vector_bool& sticky_zero, vector_float q, vector_float src0, vector_float src1,
    vector_bool mask)
{
    vector_float neg_src1;
    vector_float residual;
    vector_uint32_t sign_mask;
    vector_uint32_t src0_sign;
    vector_uint32_t zero_bits;
    vector_bool residual_gt_zero;
    vector_bool residual_lt_zero;
    vector_bool src0_positive;
    vector_bool src0_negative;
    vector_bool positive_by_positive_src;
    vector_bool positive_by_negative_src;

    asc_mul_scalar(neg_src1, src1, -1.0f, mask);
    residual = src0;
    asc_mula(residual, q, neg_src1, mask);
    asc_gt_scalar(residual_gt_zero, residual, 0.0f, mask);
    asc_lt_scalar(residual_lt_zero, residual, 0.0f, mask);
    asc_eq_scalar(sticky_zero, residual, 0.0f, mask);
    asc_duplicate_scalar(sign_mask, float_sign_bit, mask);
    asc_and(src0_sign, (vector_uint32_t&)src0, sign_mask, mask);
    asc_duplicate_scalar(zero_bits, 0u, mask);
    asc_eq(src0_positive, src0_sign, zero_bits, mask);
    asc_not(src0_negative, src0_positive, mask);
    asc_and(positive_by_positive_src, residual_gt_zero, src0_positive, mask);
    asc_and(positive_by_negative_src, residual_lt_zero, src0_negative, mask);
    asc_or(sticky_positive, positive_by_positive_src, positive_by_negative_src, mask);
}

template <bool use_sticky>
__simd_callee__ inline void div_scale_round(
    vector_float& dst, vector_int32_t scale, vector_bool sticky_positive, vector_bool sticky_zero, vector_bool mask)
{
    constexpr uint32_t sign_extractor = 0x80000000u;
    constexpr uint32_t exponent_extractor = 0x7F800000u;
    constexpr uint32_t mantissa_mask = 0x007FFFFFu;
    constexpr uint32_t implicit_bit = 0x00800000u;
    constexpr uint32_t inf = 0x7F800000u;

    vector_uint32_t q_sign;
    vector_uint32_t q_exponent;
    vector_uint32_t q_mantissa;
    vector_uint32_t m24;
    vector_uint32_t result_bits;
    vector_uint32_t tmp_u0;
    vector_uint32_t tmp_u1;
    vector_uint32_t tmp_u2;
    vector_uint32_t one_u;
    vector_uint32_t zero_u;
    vector_int32_t e;
    vector_int32_t drop;
    vector_int32_t drop_minus_one;
    vector_int32_t tmp_i0;
    vector_int32_t tmp_i1;
    vector_bool overflow_mask;
    vector_bool not_overflow_mask;
    vector_bool normal_mask;
    vector_bool subnormal_mask;
    vector_bool zero_mask;
    vector_bool subnormal_round_mask;
    vector_bool rem_gt_half_mask;
    vector_bool rem_eq_half_mask;
    vector_bool kept_odd_mask;
    vector_bool tie_up_mask;
    vector_bool round_up_mask;

    asc_duplicate_scalar(tmp_u0, sign_extractor, mask);
    asc_and(q_sign, (vector_uint32_t&)dst, tmp_u0, mask);
    asc_duplicate_scalar(tmp_u0, exponent_extractor, mask);
    asc_and(q_exponent, (vector_uint32_t&)dst, tmp_u0, mask);
    asc_shiftright_scalar(q_exponent, q_exponent, float_exponent_shift, mask);
    asc_duplicate_scalar(tmp_i0, float_exponent_bias, mask);
    asc_sub(e, (vector_int32_t&)q_exponent, tmp_i0, mask);
    asc_add(e, e, scale, mask);
    asc_duplicate_scalar(tmp_u0, mantissa_mask, mask);
    asc_and(q_mantissa, (vector_uint32_t&)dst, tmp_u0, mask);
    asc_duplicate_scalar(tmp_u0, implicit_bit, mask);
    asc_or(m24, q_mantissa, tmp_u0, mask);

    asc_gt_scalar(overflow_mask, e, float_max_normal_exponent, mask);
    asc_duplicate_scalar(tmp_u0, inf, overflow_mask);
    asc_or(result_bits, q_sign, tmp_u0, overflow_mask);
    asc_select((vector_uint32_t&)dst, result_bits, (vector_uint32_t&)dst, overflow_mask);
    asc_not(not_overflow_mask, overflow_mask, mask);
    asc_ge_scalar(normal_mask, e, float_min_normal_exponent, not_overflow_mask);
    asc_add_scalar(tmp_i1, e, float_exponent_bias, normal_mask);
    asc_shiftleft_scalar(tmp_u0, (vector_uint32_t&)tmp_i1, float_exponent_shift, normal_mask);
    asc_or(result_bits, q_sign, tmp_u0, normal_mask);
    asc_or(result_bits, result_bits, q_mantissa, normal_mask);
    asc_select((vector_uint32_t&)dst, result_bits, (vector_uint32_t&)dst, normal_mask);

    asc_not(subnormal_mask, normal_mask, not_overflow_mask);
    asc_mul_scalar(tmp_i0, (vector_int32_t&)e, -1, subnormal_mask);
    asc_add_scalar(tmp_i0, tmp_i0, float_min_normal_exponent, subnormal_mask);
    drop = tmp_i0;
    asc_gt_scalar(zero_mask, drop, float_mantissa_drop_width, subnormal_mask);
    asc_select((vector_uint32_t&)dst, q_sign, (vector_uint32_t&)dst, zero_mask);
    asc_not(subnormal_round_mask, zero_mask, subnormal_mask);
    asc_duplicate_scalar(one_u, 1u, subnormal_round_mask);
    asc_duplicate_scalar(zero_u, 0u, subnormal_round_mask);
    asc_shiftright(tmp_u0, m24, (vector_int32_t&)drop, subnormal_round_mask);
    asc_shiftleft(tmp_u1, one_u, (vector_int32_t&)drop, subnormal_round_mask);
    asc_sub(tmp_u1, tmp_u1, one_u, subnormal_round_mask);
    asc_and(tmp_u2, m24, tmp_u1, subnormal_round_mask);
    asc_add_scalar(drop_minus_one, drop, -1, subnormal_round_mask);
    asc_shiftleft(tmp_u1, one_u, (vector_int32_t&)drop_minus_one, subnormal_round_mask);
    asc_gt(rem_gt_half_mask, tmp_u2, tmp_u1, subnormal_round_mask);
    asc_eq(rem_eq_half_mask, tmp_u2, tmp_u1, subnormal_round_mask);
    asc_and(tmp_u2, tmp_u0, one_u, subnormal_round_mask);
    asc_ne(kept_odd_mask, tmp_u2, zero_u, subnormal_round_mask);
    if constexpr (use_sticky) {
        vector_bool sticky_zero_odd_mask;
        asc_and(sticky_zero_odd_mask, sticky_zero, kept_odd_mask, subnormal_round_mask);
        asc_or(tie_up_mask, sticky_positive, sticky_zero_odd_mask, subnormal_round_mask);
        asc_and(tie_up_mask, tie_up_mask, rem_eq_half_mask, subnormal_round_mask);
    } else {
        asc_and(tie_up_mask, rem_eq_half_mask, kept_odd_mask, subnormal_round_mask);
    }
    asc_or(round_up_mask, rem_gt_half_mask, tie_up_mask, subnormal_round_mask);
    asc_add(tmp_u2, tmp_u0, one_u, round_up_mask);
    asc_select(tmp_u0, tmp_u2, tmp_u0, round_up_mask);
    asc_or(result_bits, q_sign, tmp_u0, subnormal_round_mask);
    asc_select((vector_uint32_t&)dst, result_bits, (vector_uint32_t&)dst, subnormal_round_mask);
}

template <bool use_sticky>
__simd_callee__ inline void div_subnormal(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    constexpr uint32_t exponent_extractor = 0x807FFFFF;
    constexpr uint32_t exponent_normalizer = 0x3F800000;
    constexpr uint32_t inf = 0x7F800000;
    constexpr uint32_t nan = 0x7FC00000;
    constexpr float min_normal = 0x1p-126f;
    constexpr float normalize_scale = 0x1p23f;

    vector_float src0_abs;
    vector_float src1_abs;
    vector_float src0_subnormal;
    vector_float src1_subnormal;
    vector_float src0_all;
    vector_float src1_all;
    vector_float src0_norm;
    vector_float src1_norm;
    vector_float min_normal_reg;
    vector_uint32_t tmp0;
    vector_uint32_t src0_exponent;
    vector_uint32_t src1_exponent;
    vector_int32_t scale;
    vector_int32_t tmp1;
    vector_bool mask_src0_normal;
    vector_bool mask_src0_subnormal;
    vector_bool mask_src1_normal;
    vector_bool mask_src1_subnormal;
    vector_bool mask_tmp;
    vector_bool mask_nan;
    vector_bool mask_inf;
    vector_bool mask_src0_zero;
    vector_bool mask_src1_zero;
    vector_bool mask_valid;
    vector_bool sticky_positive;
    vector_bool sticky_zero;

    asc_duplicate_scalar(min_normal_reg, min_normal, mask);
    asc_abs(src0_abs, src0, mask);
    asc_abs(src1_abs, src1, mask);
    asc_duplicate_scalar(tmp0, inf, mask);
    asc_eq(mask_inf, (vector_uint32_t&)src0_abs, tmp0, mask);
    asc_eq(mask_tmp, (vector_uint32_t&)src1_abs, tmp0, mask);
    asc_or(mask_valid, mask_inf, mask_tmp, mask);
    asc_duplicate_scalar(tmp0, 0u, mask);
    asc_eq(mask_src0_zero, (vector_uint32_t&)src0_abs, tmp0, mask);
    asc_or(mask_valid, mask_valid, mask_src0_zero, mask);
    asc_eq(mask_src1_zero, (vector_uint32_t&)src1_abs, tmp0, mask);
    asc_or(mask_valid, mask_valid, mask_src1_zero, mask);
    asc_not(mask_valid, mask_valid, mask);

    asc_lt(mask_src0_subnormal, src0_abs, min_normal_reg, mask);
    asc_not(mask_src0_normal, mask_src0_subnormal, mask);
    asc_mul_scalar(src0_subnormal, src0, normalize_scale, mask_src0_subnormal);
    asc_lt(mask_src1_subnormal, src1_abs, min_normal_reg, mask);
    asc_not(mask_src1_normal, mask_src1_subnormal, mask);
    asc_mul_scalar(src1_subnormal, src1, normalize_scale, mask_src1_subnormal);
    asc_select(src0_all, src0, src0_subnormal, mask_src0_normal);
    asc_select(src1_all, src1, src1_subnormal, mask_src1_normal);

    asc_duplicate_scalar(tmp0, exponent_extractor, mask);
    asc_and((vector_uint32_t&)src0_norm, (vector_uint32_t&)src0_all, tmp0, mask_valid);
    asc_and((vector_uint32_t&)src1_norm, (vector_uint32_t&)src1_all, tmp0, mask_valid);
    asc_duplicate_scalar(tmp0, exponent_normalizer, mask);
    asc_add((vector_uint32_t&)src0_norm, (vector_uint32_t&)src0_norm, tmp0, mask_valid);
    asc_add((vector_uint32_t&)src1_norm, (vector_uint32_t&)src1_norm, tmp0, mask_valid);
    asc_select(src0_norm, src0_norm, src0_all, mask_valid);
    asc_select(src1_norm, src1_norm, src1_all, mask_valid);

    if constexpr (use_sticky) {
        div_precision(dst, src0_norm, src1_norm, mask, false);
        div_sticky(sticky_positive, sticky_zero, dst, src0_norm, src1_norm, mask_valid);
    } else {
        asc_div(dst, src0_norm, src1_norm, mask);
    }

    asc_duplicate_scalar(tmp0, inf, mask);
    asc_and(src0_exponent, (vector_uint32_t&)src0_all, tmp0, mask);
    asc_and(src1_exponent, (vector_uint32_t&)src1_all, tmp0, mask);
    asc_shiftright_scalar(src0_exponent, src0_exponent, float_exponent_shift, mask);
    asc_shiftright_scalar(src1_exponent, src1_exponent, float_exponent_shift, mask);
    asc_sub(scale, (vector_int32_t&)src0_exponent, (vector_int32_t&)src1_exponent, mask);
    asc_add_scalar(tmp1, scale, -float_normalization_scale_exponent, mask_src0_subnormal);
    asc_select(scale, tmp1, scale, mask_src0_subnormal);
    asc_add_scalar(tmp1, scale, float_normalization_scale_exponent, mask_src1_subnormal);
    asc_select(scale, tmp1, scale, mask_src1_subnormal);
    div_scale_round<use_sticky>(dst, scale, sticky_positive, sticky_zero, mask_valid);
    asc_ne(mask_nan, src0_abs, src0_abs, mask);
    asc_ne(mask_tmp, src1_abs, src1_abs, mask);
    asc_or(mask_nan, mask_nan, mask_tmp, mask);
    asc_duplicate_scalar((vector_uint32_t&)tmp0, nan, mask);
    asc_select(dst, (vector_float&)tmp0, dst, mask_nan);
}

} // namespace detail

template <typename T>
__simd_callee__ inline reg_tensor<T> ln(const reg_tensor<T>& src)
{
    static_assert(detail::supports_float_math_v<T>, "ln does not support this element type");
    reg_tensor<T> dst;
    asc_ln(dst.reg, src.reg, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> log(const reg_tensor<T>& src)
{
    return ln(src);
}

template <typename T>
__simd_callee__ inline reg_tensor<T> operator+(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_add_sub_v<T>, "operator+ does not support this element type");
    reg_tensor<T> dst;
    asc_add(dst.reg, src0.reg, src1.reg, src0.mask);
    dst.mask = src0.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> operator+(const reg_tensor<T>& src, const T& scalar)
{
    static_assert(detail::supports_add_sub_v<T>, "scalar operator+ does not support this element type");
    reg_tensor<T> dst;
    asc_add_scalar(dst.reg, src.reg, scalar, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> operator+(const T& scalar, const reg_tensor<T>& src)
{
    static_assert(detail::supports_add_sub_v<T>, "scalar operator+ does not support this element type");
    reg_tensor<T> dst;
    asc_add_scalar(dst.reg, src.reg, scalar, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> neg(const reg_tensor<T>& src)
{
    static_assert(detail::supports_neg_v<T>, "neg does not support this element type");
    reg_tensor<T> dst;
    asc_neg(dst.reg, src.reg, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> operator-(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_add_sub_v<T>, "operator- does not support this element type");
    reg_tensor<T> dst;
    asc_sub(dst.reg, src0.reg, src1.reg, src0.mask);
    dst.mask = src0.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> operator-(const reg_tensor<T>& src, const T& scalar)
{
    static_assert(detail::supports_add_sub_v<T>, "scalar operator- does not support this element type");
    auto scalar_reg = detail::make_reg_operand<T>(scalar, src.mask);
    reg_tensor<T> dst;
    asc_sub(dst.reg, src.reg, scalar_reg.reg, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> operator-(const T& scalar, const reg_tensor<T>& src)
{
    static_assert(detail::supports_add_sub_v<T>, "scalar operator- does not support this element type");
    auto scalar_reg = detail::make_reg_operand<T>(scalar, src.mask);
    reg_tensor<T> dst;
    asc_sub(dst.reg, scalar_reg.reg, src.reg, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> operator*(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_mul_v<T>, "operator* does not support this element type");
    reg_tensor<T> dst;
    asc_mul(dst.reg, src0.reg, src1.reg, src0.mask);
    dst.mask = src0.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> operator*(const reg_tensor<T>& src, const T& scalar)
{
    static_assert(detail::supports_mul_scalar_v<T>, "scalar operator* does not support this element type");
    reg_tensor<T> dst;
    asc_mul_scalar(dst.reg, src.reg, scalar, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> operator*(const T& scalar, const reg_tensor<T>& src)
{
    static_assert(detail::supports_mul_scalar_v<T>, "scalar operator* does not support this element type");
    reg_tensor<T> dst;
    asc_mul_scalar(dst.reg, src.reg, scalar, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> max(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_min_max_v<T>, "max does not support this element type");
    reg_tensor<T> dst;
    asc_max(dst.reg, src0.reg, src1.reg, src0.mask);
    dst.mask = src0.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> max(const reg_tensor<T>& src, const T& scalar)
{
    static_assert(detail::supports_min_max_v<T>, "max does not support this element type");
    reg_tensor<T> dst;
    asc_max_scalar(dst.reg, src.reg, scalar, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> max(const T& scalar, const reg_tensor<T>& src)
{
    return max(src, scalar);
}

template <typename T>
__simd_callee__ inline reg_tensor<T> abs(const reg_tensor<T>& src)
{
    static_assert(detail::supports_abs_v<T>, "abs does not support this element type");
    reg_tensor<T> dst;
    asc_abs(dst.reg, src.reg, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> exp(const reg_tensor<T>& src)
{
    static_assert(detail::supports_float_math_v<T>, "exp supports half and float");
    reg_tensor<T> dst;
    asc_exp(dst.reg, src.reg, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> sqrt(const reg_tensor<T>& src)
{
    static_assert(detail::supports_float_math_v<T>, "sqrt supports half and float");
    reg_tensor<T> dst;
    asc_sqrt(dst.reg, src.reg, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> log2(const reg_tensor<T>& src)
{
    static_assert(detail::supports_float_math_v<T>, "log2 does not support this element type");
    constexpr float ln2_reciprocal = 1.4426950408889634;
    if constexpr (AscendC::Std::is_same_v<T, float>) {
        reg_tensor<T> natural_log;
        asc_ln(natural_log.reg, src.reg, src.mask);
        reg_tensor<T> dst;
        asc_mul_scalar(dst.reg, natural_log.reg, ln2_reciprocal, src.mask);
        dst.mask = src.mask;
        return dst;
    } else {
        vector_half f16_reg_low;
        vector_half f16_reg_high;
        vector_float f32_reg_low;
        vector_float f32_reg_high;
        vector_half tmp_reg;
        vector_bool mask_low;
        vector_bool mask_high;
        const auto mask_all = all_mask<float>();

        asc_unpack_lower(mask_low, src.mask);
        asc_unpack_upper(mask_high, src.mask);

        asc_intlv(f16_reg_low, f16_reg_high, src.reg, src.reg);

        f32_reg_low = asc_half2float(f16_reg_low, mask_all.reg, ASC_POSITION_EVEN);
        f32_reg_high = asc_half2float(f16_reg_high, mask_all.reg, ASC_POSITION_EVEN);

        asc_ln(f32_reg_low, f32_reg_low, mask_all.reg);
        asc_ln(f32_reg_high, f32_reg_high, mask_all.reg);
        asc_mul_scalar(f32_reg_low, f32_reg_low, ln2_reciprocal, mask_all.reg);
        asc_mul_scalar(f32_reg_high, f32_reg_high, ln2_reciprocal, mask_all.reg);

        f16_reg_low = asc_float2half_rn(f32_reg_low, mask_low, ASC_POSITION_EVEN);
        f16_reg_high = asc_float2half_rn(f32_reg_high, mask_high, ASC_POSITION_EVEN);

        asc_deintlv(f16_reg_low, tmp_reg, f16_reg_low, tmp_reg);
        asc_deintlv(f16_reg_high, tmp_reg, tmp_reg, f16_reg_high);

        reg_tensor<T> dst;
        const auto select_mask = make_mask<mask_pattern::half, half>();
        asc_select(dst.reg, f16_reg_low, f16_reg_high, select_mask.reg);
        dst.mask = src.mask;
        return dst;
    }
}

template <typename T>
__simd_callee__ inline reg_tensor<T> log10(const reg_tensor<T>& src)
{
    static_assert(detail::supports_float_math_v<T>, "log10 does not support this element type");
    const T ln10_reciprocal = static_cast<T>(0.43429448190325176);
    reg_tensor<T> natural_log;
    asc_ln(natural_log.reg, src.reg, src.mask);
    reg_tensor<T> dst;
    asc_mul_scalar(dst.reg, natural_log.reg, ln10_reciprocal, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> relu(const reg_tensor<T>& src)
{
    static_assert(detail::supports_relu_v<T>, "relu supports half, int32_t, and float");
    reg_tensor<T> dst;
    asc_relu(dst.reg, src.reg, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> prelu(const reg_tensor<T>& src, const reg_tensor<T>& slope)
{
    static_assert(detail::supports_float_math_v<T>, "prelu supports half and float");
    reg_tensor<T> dst;
    asc_prelu(dst.reg, src.reg, slope.reg, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> leaky_relu(const reg_tensor<T>& src, const T& slope)
{
    static_assert(detail::supports_float_math_v<T>, "leaky_relu does not support this element type");
    reg_tensor<T> dst;
    asc_leakyrelu(dst.reg, src.reg, slope, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_pair<T, bool> add_carry(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_carry_v<T>, "add_carry supports int32_t and uint32_t");
    reg_pair<T, bool> result;
    asc_add(result.second.reg, result.first.reg, src0.reg, src1.reg, src0.mask);
    result.first.mask = src0.mask;
    result.second.mask = src0.mask;
    return result;
}

template <typename T>
__simd_callee__ inline reg_pair<T, bool> add_carry(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1, const reg_tensor<bool>& carry_src)
{
    static_assert(detail::supports_carry_v<T>, "add_carry supports int32_t and uint32_t");
    reg_pair<T, bool> result;
    asc_addc(result.second.reg, result.first.reg, src0.reg, src1.reg, carry_src.reg, src0.mask);
    result.first.mask = src0.mask;
    result.second.mask = src0.mask;
    return result;
}

template <typename T>
__simd_callee__ inline reg_pair<T, bool> sub_carry(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_carry_v<T>, "sub_carry supports int32_t and uint32_t");
    reg_pair<T, bool> result;
    asc_sub(result.second.reg, result.first.reg, src0.reg, src1.reg, src0.mask);
    result.first.mask = src0.mask;
    result.second.mask = src0.mask;
    return result;
}

template <typename T>
__simd_callee__ inline reg_pair<T, bool> sub_carry(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1, const reg_tensor<bool>& carry_src)
{
    static_assert(detail::supports_carry_v<T>, "sub_carry supports int32_t and uint32_t");
    reg_pair<T, bool> result;
    asc_subc(result.second.reg, result.first.reg, src0.reg, src1.reg, carry_src.reg, src0.mask);
    result.first.mask = src0.mask;
    result.second.mask = src0.mask;
    return result;
}

template <typename T>
__simd_callee__ inline reg_pair<T> mull(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_mull_v<T>, "mull does not support this element type");
    reg_pair<T> result;
    asc_mull(result.first.reg, result.second.reg, src0.reg, src1.reg, src0.mask);
    result.first.mask = src0.mask;
    result.second.mask = src0.mask;
    return result;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> operator/(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_div_v<T>, "operator/ does not support this element type");
    reg_tensor<T> dst;
    asc_div(dst.reg, src0.reg, src1.reg, src0.mask);
    dst.mask = src0.mask;
    return dst;
}

template <bool precisionMode, div_algo algo, typename T>
__simd_callee__ inline reg_tensor<T> div(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_div_v<T>, "div does not support this element type");
    static_assert(
        algo == div_algo::INTRINSIC || algo == div_algo::DIFF_COMPENSATION ||
            algo == div_algo::PRECISION_1ULP_FTZ_TRUE || algo == div_algo::PRECISION_0ULP_FTZ_TRUE ||
            algo == div_algo::PRECISION_0ULP_FTZ_FALSE || algo == div_algo::PRECISION_1ULP_FTZ_FALSE,
        "div does not support this algorithm");
    static_assert(!precisionMode || AscendC::Std::is_same_v<T, float>, "div precision modes only support float");
    static_assert(
        (algo != div_algo::DIFF_COMPENSATION && algo != div_algo::PRECISION_0ULP_FTZ_TRUE &&
         algo != div_algo::PRECISION_0ULP_FTZ_FALSE) ||
            AscendC::Std::is_same_v<T, float>,
        "div 0-ULP algorithms only support float");
    static_assert(
        algo != div_algo::PRECISION_1ULP_FTZ_FALSE || AscendC::Std::is_same_v<T, half> ||
            AscendC::Std::is_same_v<T, float>,
        "div PRECISION_1ULP_FTZ_FALSE only supports half/float");

    constexpr bool usePrecision = precisionMode || algo == div_algo::DIFF_COMPENSATION ||
                                  algo == div_algo::PRECISION_0ULP_FTZ_TRUE ||
                                  algo == div_algo::PRECISION_0ULP_FTZ_FALSE;
    constexpr bool preserveSubnormal =
        algo == div_algo::PRECISION_0ULP_FTZ_FALSE || (!precisionMode && algo == div_algo::PRECISION_1ULP_FTZ_FALSE)
#if !defined(__ASC_FTZ__)
        || (algo != div_algo::PRECISION_0ULP_FTZ_TRUE && (precisionMode || algo != div_algo::PRECISION_1ULP_FTZ_TRUE))
#endif
        ;

    auto mask = src0.mask;
    reg_tensor<T> dst;
    if constexpr (usePrecision) {
        if constexpr (preserveSubnormal) {
            detail::div_subnormal<true>(dst.reg, src0.reg, src1.reg, mask);
        } else {
            detail::div_precision(dst.reg, src0.reg, src1.reg, mask, false);
        }
    } else if constexpr (preserveSubnormal) {
        if constexpr (AscendC::Std::is_same_v<T, float>) {
            detail::div_subnormal<false>(dst.reg, src0.reg, src1.reg, mask);
        } else {
            asc_div(dst.reg, src0.reg, src1.reg, mask);
        }
    } else {
        asc_div(dst.reg, src0.reg, src1.reg, mask);
        if constexpr (AscendC::Std::is_same_v<T, half>) {
            detail::div_flush_subnormal_half(dst.reg, mask);
        }
    }
    dst.mask = src0.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> min(const reg_tensor<T>& src0, const reg_tensor<T>& src1)
{
    static_assert(detail::supports_min_max_v<T>, "min does not support this element type");
    reg_tensor<T> dst;
    asc_min(dst.reg, src0.reg, src1.reg, src0.mask);
    dst.mask = src0.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> min(const reg_tensor<T>& src, const T& scalar)
{
    static_assert(detail::supports_min_max_v<T>, "min does not support this element type");
    reg_tensor<T> dst;
    asc_min_scalar(dst.reg, src.reg, scalar, src.mask);
    dst.mask = src.mask;
    return dst;
}

template <typename T>
__simd_callee__ inline reg_tensor<T> min(const T& scalar, const reg_tensor<T>& src)
{
    static_assert(detail::supports_min_max_v<T>, "min does not support this element type");
    reg_tensor<T> dst;
    asc_min_scalar(dst.reg, src.reg, scalar, src.mask);
    dst.mask = src.mask;
    return dst;
}

} // namespace experimental
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_BASIC_ARITHMETIC_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_BASIC_ARITHMETIC_IMPL_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_BASIC_ARITHMETIC_IMPL_H
#endif
