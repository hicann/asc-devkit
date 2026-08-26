/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file asc_fp16_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_FP16_IMPL__
#warning "impl/simt_api/asc_fp16_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "simt_api/asc_fp16.h" and use public functions or variables defined in interface header files."
#endif

#ifndef IMPL_SIMT_API_ASC_FP16_IMPL_H
#define IMPL_SIMT_API_ASC_FP16_IMPL_H

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#include "__clang_cce_simt_fp16.h"
#endif

#include "simt_api/device_types.h"
#include "impl/simt_api/internal_functions_impl.h"
#include "impl/simt_api/math_functions_impl.h"

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)

constexpr uint32_t __INTERNAL_HALF_INF = 0x7C00;
constexpr uint32_t __INTERNAL_HALF_NEG_INF = 0xFC00;

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bool __hisnan(half x) { return __isnan(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bool __hisinf(half x) { return __isinf(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __hfma(half x, half y, half z) { return __fma(x, y, z); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __habs(half x)
{
    uint16_t bits = *reinterpret_cast<uint16_t*>(&x);
    bits &= 0x7FFF;
    return *reinterpret_cast<half*>(&bits);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bool __internal_is_positive_inf(half x)
{
    uint16_t* int_x = reinterpret_cast<uint16_t*>(&x);
    return *int_x == __INTERNAL_HALF_INF;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bool __internal_is_negative_inf(half x)
{
    uint16_t* int_x = reinterpret_cast<uint16_t*>(&x);
    return *int_x == __INTERNAL_HALF_NEG_INF;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __hmax(half x, half y)
{
    if (__hisnan(x)) {
        return y;
    } else if (__hisnan(y)) {
        return x;
    }
    return __hmax_nan(x, y);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __hmin(half x, half y)
{
    if (__hisnan(x)) {
        return y;
    } else if (__hisnan(y)) {
        return x;
    }
    return __hmin_nan(x, y);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hcos(half x)
{
    float tmp = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    tmp = cosf(tmp);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(tmp);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2cos(half2 x)
{
    float2 x_fp32 =
        __cvt_float2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    float2 res_fp32;
    res_fp32.x = cosf(x_fp32.x);
    res_fp32.y = cosf(x_fp32.y);
    return __cvt_half2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(res_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hsin(half x)
{
    float tmp = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    tmp = sinf(tmp);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(tmp);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2sin(half2 x)
{
    float2 x_fp32 =
        __cvt_float2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    float2 res_fp32;
    res_fp32.x = sinf(x_fp32.x);
    res_fp32.y = sinf(x_fp32.y);
    return __cvt_half2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(res_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half htanh(half x)
{
    float tmp = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    tmp = tanhf(tmp);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(tmp);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2tanh(half2 x)
{
    float2 x_fp32 =
        __cvt_float2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    float2 res_fp32;
    res_fp32.x = tanhf(x_fp32.x);
    res_fp32.y = tanhf(x_fp32.y);
    return __cvt_half2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(res_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hexp(half x) { return __expf(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hexp2(half x)
{
    float tmp = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    tmp = powf(2.0f, tmp);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(tmp);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2exp2(half2 x)
{
    float2 x_fp32 =
        __cvt_float2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    float2 res_fp32;
    res_fp32.x = powf(2.0f, x_fp32.x);
    res_fp32.y = powf(2.0f, x_fp32.y);
    return __cvt_half2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(res_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hexp10(half x)
{
    float tmp = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    tmp = powf(10.0f, tmp);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(tmp);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2exp10(half2 x)
{
    float2 x_fp32 =
        __cvt_float2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    float2 res_fp32;
    res_fp32.x = powf(10.0f, x_fp32.x);
    res_fp32.y = powf(10.0f, x_fp32.y);
    return __cvt_half2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(res_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hlog(half x) { return __logf(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hlog2(half x)
{
    float tmp = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    tmp = logf(x) / logf(2.0f);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(tmp);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2log2(half2 x)
{
    float2 x_fp32 =
        __cvt_float2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    float2 res_fp32;
    res_fp32.x = logf(x_fp32.x) / logf(2.0f);
    res_fp32.y = logf(x_fp32.y) / logf(2.0f);
    return __cvt_half2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(res_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hlog10(half x)
{
    float tmp = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    tmp = logf(tmp) / logf(10.0f);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(tmp);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2log10(half2 x)
{
    float2 x_fp32 =
        __cvt_float2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    float2 res_fp32;
    res_fp32.x = logf(x_fp32.x) / logf(10.0f);
    res_fp32.y = logf(x_fp32.y) / logf(10.0f);
    return __cvt_half2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(res_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hsqrt(half x) { return __sqrtf(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hrsqrt(half x) { return (half)1.0 / hsqrt(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2exp(half2 x) { return __expf(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2log(half2 x) { return __logf(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2sqrt(half2 x) { return __sqrtf(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2rsqrt(half2 x)
{
    half tmp1 = static_cast<half>(1.0) / __sqrtf(x.x);
    half tmp2 = static_cast<half>(1.0) / __sqrtf(x.y);
    return {tmp1, tmp2};
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hrcp(half x) { return static_cast<half>(1.0) / x; }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2rcp(half2 x)
{
    half tmp1 = static_cast<half>(1.0) / x.x;
    half tmp2 = static_cast<half>(1.0) / x.y;
    return {tmp1, tmp2};
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hfloor(half x) { return __floorf(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2floor(half2 x)
{
    half tmp1 = __floorf(x.x);
    half tmp2 = __floorf(x.y);
    return {tmp1, tmp2};
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hrint(half x) { return __rintf(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2rint(half2 x)
{
    half tmp1 = __rintf(x.x);
    half tmp2 = __rintf(x.y);
    return {tmp1, tmp2};
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half hceil(half x) { return __ceilf(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2ceil(half2 x)
{
    half tmp1 = __ceilf(x.x);
    half tmp2 = __ceilf(x.y);
    return {tmp1, tmp2};
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half htrunc(half x)
{
    if (x > static_cast<half>(0)) {
        return __floorf(x);
    } else {
        return __ceilf(x);
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 h2trunc(half2 x)
{
    half tmp1 = x.x;
    half tmp2 = x.y;
    if (x.x > static_cast<half>(0)) {
        tmp1 = __floorf(tmp1);
    } else {
        tmp1 = __ceilf(tmp1);
    }
    if (x.y > static_cast<half>(0)) {
        tmp2 = __floorf(tmp2);
    } else {
        tmp2 = __ceilf(tmp2);
    }
    x = {tmp1, tmp2};
    return x;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half(const float x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_rn(const float x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_rn_sat(const float x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_rz(const float x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_rz_sat(const float x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_rd(const float x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_rd_sat(const float x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_ru(const float x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_ru_sat(const float x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_rna(const float x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_rna_sat(const float x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_ro(const float x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_ODD>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __float2half_ro_sat(const float x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_ODD>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float22half2_rn_sat(const float2 x)
{
    return __cvt_half2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float22half2_rz_sat(const float2 x)
{
    return __cvt_half2<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float22half2_rd(const float2 x)
{
    return __cvt_half2<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float22half2_rd_sat(const float2 x)
{
    return __cvt_half2<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float22half2_ru(const float2 x)
{
    return __cvt_half2<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float22half2_ru_sat(const float2 x)
{
    return __cvt_half2<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float22half2_rna(const float2 x)
{
    return __cvt_half2<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float22half2_rna_sat(const float2 x)
{
    return __cvt_half2<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float22half2_ro(const float2 x)
{
    return __cvt_half2<__internal_get_round<__RoundMode::CAST_ODD>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float22half2_ro_sat(const float2 x)
{
    return __cvt_half2<__internal_get_round<__RoundMode::CAST_ODD>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __half2float(const half x)
{
    return __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline signed char __half2char_rz(const half x)
{
    int i = __cvt_int32_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __internal_int_to_char(i);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned char __half2uchar_rz(const half x)
{
    unsigned int i =
        __cvt_uint32_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __internal_uint_to_uchar(i);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short int __half2short_rn(const half x)
{
    int i = __cvt_int32_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __internal_int_to_short(i);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short int __half2short_rz(const half x)
{
    int i = __cvt_int32_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __internal_int_to_short(i);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short int __half2short_rd(const half x)
{
    int i = __cvt_int32_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __internal_int_to_short(i);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short int __half2short_ru(const half x)
{
    int i = __cvt_int32_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __internal_int_to_short(i);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned short int __half2ushort_rn(const half x)
{
    unsigned int i =
        __cvt_uint32_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __internal_uint_to_ushort(i);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned short int __half2ushort_rz(const half x)
{
    unsigned int i =
        __cvt_uint32_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __internal_uint_to_ushort(i);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned short int __half2ushort_rd(const half x)
{
    unsigned int i =
        __cvt_uint32_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __internal_uint_to_ushort(i);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned short int __half2ushort_ru(const half x)
{
    unsigned int i =
        __cvt_uint32_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __internal_uint_to_ushort(i);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __half2uint_rn(const half x)
{
    return __cvt_uint32_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __half2uint_rz(const half x)
{
    return __cvt_uint32_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __half2uint_rd(const half x)
{
    return __cvt_uint32_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __half2uint_ru(const half x)
{
    return __cvt_uint32_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __half2uint_rna(const half x)
{
    return __cvt_uint32_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __half2int_rn(const half x)
{
    return __cvt_int32_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __half2int_rz(const half x)
{
    return __cvt_int32_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __half2int_rd(const half x)
{
    return __cvt_int32_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __half2int_ru(const half x)
{
    return __cvt_int32_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __half2int_rna(const half x)
{
    return __cvt_int32_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __half2ull_rn(const half x)
{
    float x_fp32 = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __cvt_uint64_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __half2ull_rz(const half x)
{
    float x_fp32 = __cvt_float<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __cvt_uint64_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __half2ull_rd(const half x)
{
    float x_fp32 = __cvt_float<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __cvt_uint64_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __half2ull_ru(const half x)
{
    float x_fp32 = __cvt_float<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __cvt_uint64_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __half2ull_rna(const half x)
{
    float x_fp32 = __cvt_float<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __cvt_uint64_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __half2ll_rn(const half x)
{
    float x_fp32 = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __cvt_int64_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __half2ll_rz(const half x)
{
    float x_fp32 = __cvt_float<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __cvt_int64_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __half2ll_rd(const half x)
{
    float x_fp32 = __cvt_float<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __cvt_int64_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __half2ll_ru(const half x)
{
    float x_fp32 = __cvt_float<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __cvt_int64_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __half2ll_rna(const half x)
{
    float x_fp32 = __cvt_float<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __cvt_int64_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __half2half_rn(const half x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __half2half_rz(const half x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __half2half_rd(const half x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __half2half_ru(const half x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __half2half_rna(const half x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __uint2half_rn(const unsigned int x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __uint2half_rn_sat(const unsigned int x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __uint2half_rz(const unsigned int x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __uint2half_rz_sat(const unsigned int x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __uint2half_rd(const unsigned int x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __uint2half_rd_sat(const unsigned int x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __uint2half_ru(const unsigned int x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __uint2half_ru_sat(const unsigned int x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __uint2half_rna(const unsigned int x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __uint2half_rna_sat(const unsigned int x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __int2half_rn(const int x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __int2half_rn_sat(const int x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __int2half_rz(const int x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __int2half_rz_sat(const int x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __int2half_rd(const int x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __int2half_rd_sat(const int x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __int2half_ru(const int x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __int2half_ru_sat(const int x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __int2half_rna(const int x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __int2half_rna_sat(const int x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __short2half_rn(const short int x)
{
    int32_t i = static_cast<int32_t>(x);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(i);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __short2half_rz(const short int x)
{
    int32_t i = static_cast<int32_t>(x);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(i);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __short2half_rd(const short int x)
{
    int32_t i = static_cast<int32_t>(x);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(i);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __short2half_ru(const short int x)
{
    int32_t i = static_cast<int32_t>(x);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(i);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ushort2half_rn(const unsigned short int x)
{
    uint32_t i = static_cast<uint32_t>(x);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(i);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ushort2half_rz(const unsigned short int x)
{
    uint32_t i = static_cast<uint32_t>(x);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(i);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ushort2half_rd(const unsigned short int x)
{
    uint32_t i = static_cast<uint32_t>(x);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(i);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ushort2half_ru(const unsigned short int x)
{
    uint32_t i = static_cast<uint32_t>(x);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(i);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ull2half_rn(const unsigned long long int x)
{
    uint64_t y = x;
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ull2half_rz(const unsigned long long int x)
{
    uint64_t y = x;
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ull2half_rd(const unsigned long long int x)
{
    uint64_t y = x;
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ull2half_ru(const unsigned long long int x)
{
    uint64_t y = x;
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ull2half_rna(const unsigned long long int x)
{
    uint64_t y = x;
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ll2half_rn(const long long int x)
{
    int64_t y = x;
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ll2half_rz(const long long int x)
{
    int64_t y = x;
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ll2half_rd(const long long int x)
{
    int64_t y = x;
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ll2half_ru(const long long int x)
{
    int64_t y = x;
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ll2half_rna(const long long int x)
{
    int64_t y = x;
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
    return __cvt_half<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __floats2half2_rn(const float x, const float y)
{
    half2 tmp;
    tmp.x = __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    tmp.y = __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float22half2_rn(const float2 x)
{
    return __cvt_half2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __float2half2_rn(const float x)
{
    half y = __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    half2 tmp{y, y};
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __half2half2(const half x)
{
    half2 tmp{x, x};
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __low2float(const half2 x)
{
    return __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x.x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __low2half(const half2 x) { return x.x; }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __low2half2(const half2 x)
{
    half2 tmp;
    tmp.x = x.x;
    tmp.y = x.x;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __lowhigh2highlow(const half2 x)
{
    half2 tmp;
    tmp.x = x.y;
    tmp.y = x.x;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __high2float(const half2 x)
{
    return __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x.y);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __high2half(const half2 x) { return x.y; }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __high2half2(const half2 x)
{
    half2 tmp;
    tmp.x = x.y;
    tmp.y = x.y;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __highs2half2(const half2 x, const half2 y)
{
    half2 tmp;
    tmp.x = x.y;
    tmp.y = y.y;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __lows2half2(const half2 x, const half2 y)
{
    half2 tmp;
    tmp.x = x.x;
    tmp.y = y.x;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __halves2half2(const half x, const half y)
{
    half2 tmp;
    tmp.x = x;
    tmp.y = y;
    return tmp;
}
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
#ifndef ASCENDC_CPU_DEBUG
__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_add(__ubuf__ half* address, half val)
{
    atomicAdd(address, val);
    return *address;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_add(__ubuf__ half2* address, half2 val)
{
    return atomicAdd(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_sub(__ubuf__ half2* address, half2 val)
{
    return atomicSub(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_sub(__ubuf__ half* address, half val)
{
    return asc_atomic_add(address, -val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_exch(__ubuf__ half2* address, half2 val)
{
    return atomicExch(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_max(__ubuf__ half* address, half val)
{
    atomicMax(address, val);
    return *address;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_max(__ubuf__ half2* address, half2 val)
{
    return atomicMax(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_min(__ubuf__ half* address, half val)
{
    atomicMin(address, val);
    return *address;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_min(__ubuf__ half2* address, half2 val)
{
    return atomicMin(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_cas(__ubuf__ half2* address, half2 compare, half2 val)
{
    return atomicCAS(address, compare, val);
}
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_sub(__gm__ half* address, half val)
{
    return asc_atomic_add(address, -val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_add(__gm__ half* address, half val)
{
    atomicAdd(address, val);
    return *address;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_add(__gm__ half2* address, half2 val)
{
    return atomicAdd(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_sub(__gm__ half2* address, half2 val)
{
    return atomicSub(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_exch(__gm__ half2* address, half2 val)
{
    return atomicExch(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_max(__gm__ half* address, half val)
{
    atomicMax(address, val);
    return *address;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_max(__gm__ half2* address, half2 val)
{
    return atomicMax(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_min(__gm__ half* address, half val)
{
    atomicMin(address, val);
    return *address;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_min(__gm__ half2* address, half2 val)
{
    return atomicMin(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_cas(__gm__ half2* address, half2 compare, half2 val)
{
    return atomicCAS(address, compare, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_ldcg(__gm__ half* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_ldcg(__gm__ half2* address)
{
    int32_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        reinterpret_cast<__gm__ int32_t*>(address));
    return reinterpret_cast<half2&>(t);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_ldca(__gm__ half* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_ldca(__gm__ half2* address)
{
    int32_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        reinterpret_cast<__gm__ int32_t*>(address));
    return reinterpret_cast<half2&>(t);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ half* address, half val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ half2* address, half2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        reinterpret_cast<__gm__ int32_t*>(address), reinterpret_cast<int32_t&>(val));
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ half* address, half val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ half2* address, half2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        reinterpret_cast<__gm__ int32_t*>(address), reinterpret_cast<int32_t&>(val));
}

#else
#ifndef ASCENDC_CPU_DEBUG
__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_add(half* address, half val)
{
    __atomic_add(address, val);
    return *address;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_add(half2* address, half2 val)
{
    return __atomic_add(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_sub(half2* address, half2 val)
{
    return __atomic_sub(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_sub(half* address, half val)
{
    return asc_atomic_add(address, -val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_exch(half2* address, half2 val)
{
    return __atomic_exch(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_max(half* address, half val)
{
    __atomic_max(address, val);
    return *address;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_max(half2* address, half2 val)
{
    return __atomic_max(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_atomic_min(half* address, half val)
{
    __atomic_min(address, val);
    return *address;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_min(half2* address, half2 val)
{
    return __atomic_min(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_atomic_cas(half2* address, half2 compare, half2 val)
{
    return __atomic_cas(address, compare, val);
}
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_ldcg(half* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_ldcg(half2* address)
{
    int32_t t =
        __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(reinterpret_cast<int32_t*>(address));
    return reinterpret_cast<half2&>(t);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_ldca(half* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_ldca(half2* address)
{
    int32_t t =
        __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(reinterpret_cast<int32_t*>(address));
    return reinterpret_cast<half2&>(t);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(half* address, half val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(half2* address, half2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        reinterpret_cast<int32_t*>(address), reinterpret_cast<int32_t&>(val));
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(half* address, half val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(half2* address, half2 val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        reinterpret_cast<int32_t*>(address), reinterpret_cast<int32_t&>(val));
}
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_shfl(half var, int32_t src_lane, int32_t width)
{
    return __shfl(var, src_lane, width);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_shfl(half2 var, int32_t src_lane, int32_t width)
{
    return __shfl(var, src_lane, width);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_shfl_up(half var, uint32_t delta, int32_t width)
{
    return __shfl_up(var, delta, width);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_shfl_up(half2 var, uint32_t delta, int32_t width)
{
    return __shfl_up(var, delta, width);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_shfl_down(half var, uint32_t delta, int32_t width)
{
    return __shfl_down(var, delta, width);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_shfl_down(half2 var, uint32_t delta, int32_t width)
{
    return __shfl_down(var, delta, width);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_shfl_xor(half var, int32_t lane_mask, int32_t width)
{
    return __shfl_xor(var, lane_mask, width);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 asc_shfl_xor(half2 var, int32_t lane_mask, int32_t width)
{
    return __shfl_xor(var, lane_mask, width);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_reduce_add(half val) { return __reduce_add(val); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_reduce_max(half val) { return __reduce_max(val); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half asc_reduce_min(half val) { return __reduce_min(val); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 make_half2(half x, half y)
{
    half2 tmp;
    tmp.x = x;
    tmp.y = y;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __ushort_as_half(const unsigned short int x)
{
    union Data {
        unsigned short int i;
        half f;
    };
    union Data data = {.i = x};
    return data.f;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline short int __half_as_short(const half x)
{
    union Data {
        short int i;
        half f;
    };
    union Data data = {.f = x};
    return data.i;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned short int __half_as_ushort(const half x)
{
    union Data {
        unsigned short int i;
        half f;
    };
    union Data data = {.f = x};
    return data.i;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __short_as_half(const short int x)
{
    union Data {
        short int i;
        half f;
    };
    union Data data = {.i = x};
    return data.f;
}

#endif
#endif // IMPL_SIMT_API_ASC_FP16_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_FP16_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_FP16_IMPL__
#endif
