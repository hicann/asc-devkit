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
 * \file asc_bf16_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_BF16_IMPL__
#warning "impl/simt_api/asc_bf16_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "simt_api/asc_bf16.h" and use public functions or variables defined in interface header files."
#endif

#ifndef IMPL_SIMT_API_ASC_BF16_IMPL_H
#define IMPL_SIMT_API_ASC_BF16_IMPL_H

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#include "__clang_cce_simt_bf16.h"
#endif

#include "simt_api/device_types.h"
#include "impl/simt_api/internal_functions_impl.h"
#include "impl/simt_api/math_functions_impl.h"

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bool __hisnan(bfloat16_t x) { return __isnan(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bool __hisinf(bfloat16_t x) { return __isinf(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __habs(bfloat16_t x)
{
    uint16_t bits = *reinterpret_cast<uint16_t*>(&x);
    bits &= 0x7FFF;
    return *reinterpret_cast<bfloat16_t*>(&bits);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __hfma(bfloat16_t x, bfloat16_t y, bfloat16_t z)
{
    return __fma(x, y, z);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __hmax(bfloat16_t x, bfloat16_t y)
{
    if (__hisnan(x)) {
        return y;
    } else if (__hisnan(y)) {
        return x;
    }
    return __max(x, y);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __hmin(bfloat16_t x, bfloat16_t y)
{
    if (__hisnan(x)) {
        return y;
    } else if (__hisnan(y)) {
        return x;
    }
    return __min(x, y);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hcos(bfloat16_t x)
{
    float tmp = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    tmp = cosf(tmp);
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(tmp);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2cos(bfloat16x2_t x)
{
    float2 x_fp32 =
        __cvt_float2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    float2 res_fp32;
    res_fp32.x = cosf(x_fp32.x);
    res_fp32.y = cosf(x_fp32.y);
    return __cvt_bfloat16x2_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(
        res_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hsin(bfloat16_t x)
{
    float tmp = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    tmp = sinf(tmp);
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(tmp);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2sin(bfloat16x2_t x)
{
    float2 x_fp32 =
        __cvt_float2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    float2 res_fp32;
    res_fp32.x = sinf(x_fp32.x);
    res_fp32.y = sinf(x_fp32.y);
    return __cvt_bfloat16x2_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(
        res_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t htanh(bfloat16_t x)
{
    float tmp = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    tmp = tanhf(tmp);
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(tmp);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2tanh(bfloat16x2_t x)
{
    float2 x_fp32 =
        __cvt_float2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    float2 res_fp32;
    res_fp32.x = tanhf(x_fp32.x);
    res_fp32.y = tanhf(x_fp32.y);
    return __cvt_bfloat16x2_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(
        res_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hexp(bfloat16_t x)
{
    float tmp = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    tmp = expf(tmp);
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(tmp);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2exp(bfloat16x2_t x)
{
    float2 x_fp32 =
        __cvt_float2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    float2 res_fp32;
    res_fp32.x = expf(x_fp32.x);
    res_fp32.y = expf(x_fp32.y);
    return __cvt_bfloat16x2_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(
        res_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hexp2(bfloat16_t x)
{
    float tmp = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    tmp = exp2f(tmp);
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(tmp);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2exp2(bfloat16x2_t x)
{
    float2 x_fp32 =
        __cvt_float2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    float2 res_fp32;
    res_fp32.x = exp2f(x_fp32.x);
    res_fp32.y = exp2f(x_fp32.y);
    return __cvt_bfloat16x2_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(
        res_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hexp10(bfloat16_t x)
{
    float tmp = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    tmp = exp10f(tmp);
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(tmp);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2exp10(bfloat16x2_t x)
{
    float2 x_fp32 =
        __cvt_float2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    float2 res_fp32;
    res_fp32.x = exp10f(x_fp32.x);
    res_fp32.y = exp10f(x_fp32.y);
    return __cvt_bfloat16x2_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(
        res_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hlog(bfloat16_t x)
{
    float tmp = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    tmp = logf(tmp);
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(tmp);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2log(bfloat16x2_t x)
{
    float2 x_fp32 =
        __cvt_float2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    float2 res_fp32;
    res_fp32.x = logf(x_fp32.x);
    res_fp32.y = logf(x_fp32.y);
    return __cvt_bfloat16x2_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(
        res_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hlog2(bfloat16_t x)
{
    float tmp = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    tmp = log2f(tmp);
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(tmp);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2log2(bfloat16x2_t x)
{
    float2 x_fp32 =
        __cvt_float2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    float2 res_fp32;
    res_fp32.x = log2f(x_fp32.x);
    res_fp32.y = log2f(x_fp32.y);
    return __cvt_bfloat16x2_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(
        res_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hlog10(bfloat16_t x)
{
    float tmp = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    tmp = log10f(tmp);
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(tmp);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2log10(bfloat16x2_t x)
{
    float2 x_fp32 =
        __cvt_float2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    float2 res_fp32;
    res_fp32.x = log10f(x_fp32.x);
    res_fp32.y = log10f(x_fp32.y);
    return __cvt_bfloat16x2_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(
        res_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hsqrt(bfloat16_t x)
{
    float tmp = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    tmp = sqrtf(tmp);
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(tmp);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2sqrt(bfloat16x2_t x)
{
    float2 x_fp32 =
        __cvt_float2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    float2 res_fp32;
    res_fp32.x = sqrtf(x_fp32.x);
    res_fp32.y = sqrtf(x_fp32.y);
    return __cvt_bfloat16x2_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(
        res_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hrsqrt(bfloat16_t x)
{
    float tmp = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    tmp = 1.0f / sqrtf(tmp);
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(tmp);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2rsqrt(bfloat16x2_t x)
{
    float2 x_fp32 =
        __cvt_float2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    float2 res_fp32;
    res_fp32.x = 1.0f / sqrtf(x_fp32.x);
    res_fp32.y = 1.0f / sqrtf(x_fp32.y);
    return __cvt_bfloat16x2_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(
        res_fp32);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hrcp(bfloat16_t x) { return static_cast<bfloat16_t>(1.0) / x; }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2rcp(bfloat16x2_t x)
{
    bfloat16_t tmp1 = static_cast<bfloat16_t>(1.0) / x.x;
    bfloat16_t tmp2 = static_cast<bfloat16_t>(1.0) / x.y;
    return {tmp1, tmp2};
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hfloor(bfloat16_t x) { return __floorf(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2floor(bfloat16x2_t x)
{
    bfloat16_t tmp1 = __floorf(x.x);
    bfloat16_t tmp2 = __floorf(x.y);
    return {tmp1, tmp2};
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hrint(bfloat16_t x) { return __rintf(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2rint(bfloat16x2_t x)
{
    bfloat16_t tmp1 = __rintf(x.x);
    bfloat16_t tmp2 = __rintf(x.y);
    return {tmp1, tmp2};
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t hceil(bfloat16_t x) { return __ceilf(x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2ceil(bfloat16x2_t x)
{
    bfloat16_t tmp1 = __ceilf(x.x);
    bfloat16_t tmp2 = __ceilf(x.y);
    return {tmp1, tmp2};
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t htrunc(bfloat16_t x)
{
    if (x > static_cast<bfloat16_t>(0)) {
        return __floorf(x);
    } else {
        return __ceilf(x);
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t h2trunc(bfloat16x2_t x)
{
    bfloat16_t tmp1 = x.x;
    bfloat16_t tmp2 = x.y;
    if (x.x > static_cast<bfloat16_t>(0)) {
        tmp1 = __floorf(tmp1);
    } else {
        tmp1 = __ceilf(tmp1);
    }
    if (x.y > static_cast<bfloat16_t>(0)) {
        tmp2 = __floorf(tmp2);
    } else {
        tmp2 = __ceilf(tmp2);
    }
    x = {tmp1, tmp2};
    return x;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __float2bfloat16(const float x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __float2bfloat16_rn(const float x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __float2bfloat16_rn_sat(const float x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __float2bfloat16_rz(const float x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __float2bfloat16_rz_sat(const float x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __float2bfloat16_rd(const float x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __float2bfloat16_rd_sat(const float x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __float2bfloat16_ru(const float x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __float2bfloat16_ru_sat(const float x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __float2bfloat16_rna(const float x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __float2bfloat16_rna_sat(const float x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __float22bfloat162_rn_sat(const float2 x)
{
    return __cvt_bfloat16x2_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __float22bfloat162_rz(const float2 x)
{
    return __cvt_bfloat16x2_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __float22bfloat162_rz_sat(const float2 x)
{
    return __cvt_bfloat16x2_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __float22bfloat162_rd(const float2 x)
{
    return __cvt_bfloat16x2_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __float22bfloat162_rd_sat(const float2 x)
{
    return __cvt_bfloat16x2_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __float22bfloat162_ru(const float2 x)
{
    return __cvt_bfloat16x2_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __float22bfloat162_ru_sat(const float2 x)
{
    return __cvt_bfloat16x2_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __float22bfloat162_rna(const float2 x)
{
    return __cvt_bfloat16x2_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __float22bfloat162_rna_sat(const float2 x)
{
    return __cvt_bfloat16x2_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __half2bfloat16_rn(const half x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __half2bfloat16_rz(const half x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __half2bfloat16_rd(const half x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __half2bfloat16_ru(const half x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __half2bfloat16_rna(const half x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __bfloat162half_rn(const bfloat16_t x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __bfloat162half_rn_sat(const bfloat16_t x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __bfloat162half_rz(const bfloat16_t x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __bfloat162half_rz_sat(const bfloat16_t x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __bfloat162half_rd(const bfloat16_t x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __bfloat162half_rd_sat(const bfloat16_t x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __bfloat162half_ru(const bfloat16_t x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __bfloat162half_ru_sat(const bfloat16_t x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __bfloat162half_rna(const bfloat16_t x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half __bfloat162half_rna_sat(const bfloat16_t x)
{
    return __cvt_half<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __bfloat162float(const bfloat16_t x)
{
    return __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __bfloat162bfloat16_rn(const bfloat16_t x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __bfloat162bfloat16_rz(const bfloat16_t x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __bfloat162bfloat16_rd(const bfloat16_t x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __bfloat162bfloat16_ru(const bfloat16_t x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __bfloat162bfloat16_rna(const bfloat16_t x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __bfloat162uint_rn(const bfloat16_t x)
{
    return __cvt_uint32_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __bfloat162uint_rz(const bfloat16_t x)
{
    return __cvt_uint32_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __bfloat162uint_rd(const bfloat16_t x)
{
    return __cvt_uint32_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __bfloat162uint_ru(const bfloat16_t x)
{
    return __cvt_uint32_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned int __bfloat162uint_rna(const bfloat16_t x)
{
    return __cvt_uint32_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __bfloat162int_rn(const bfloat16_t x)
{
    return __cvt_int32_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __bfloat162int_rz(const bfloat16_t x)
{
    return __cvt_int32_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __bfloat162int_rd(const bfloat16_t x)
{
    return __cvt_int32_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __bfloat162int_ru(const bfloat16_t x)
{
    return __cvt_int32_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline int __bfloat162int_rna(const bfloat16_t x)
{
    return __cvt_int32_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __bfloat162ull_rn(const bfloat16_t x)
{
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __cvt_uint64_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __bfloat162ull_rz(const bfloat16_t x)
{
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __cvt_uint64_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __bfloat162ull_rd(const bfloat16_t x)
{
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __cvt_uint64_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __bfloat162ull_ru(const bfloat16_t x)
{
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __cvt_uint64_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline unsigned long long int __bfloat162ull_rna(const bfloat16_t x)
{
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __cvt_uint64_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __bfloat162ll_rn(const bfloat16_t x)
{
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __cvt_int64_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __bfloat162ll_rz(const bfloat16_t x)
{
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __cvt_int64_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_ENABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __bfloat162ll_rd(const bfloat16_t x)
{
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __cvt_int64_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_ENABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __bfloat162ll_ru(const bfloat16_t x)
{
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __cvt_int64_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_ENABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline long long int __bfloat162ll_rna(const bfloat16_t x)
{
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
    return __cvt_int64_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __uint2bfloat16_rn(const unsigned int x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __uint2bfloat16_rz(const unsigned int x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __uint2bfloat16_rd(const unsigned int x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __uint2bfloat16_ru(const unsigned int x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __uint2bfloat16_rna(const unsigned int x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __int2bfloat16_rn(const int x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __int2bfloat16_rz(const int x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __int2bfloat16_rd(const int x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __int2bfloat16_ru(const int x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __int2bfloat16_rna(const int x)
{
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __ull2bfloat16_rn(const unsigned long long int x)
{
    uint64_t y = x;
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __ull2bfloat16_rz(const unsigned long long int x)
{
    uint64_t y = x;
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __ull2bfloat16_rd(const unsigned long long int x)
{
    uint64_t y = x;
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __ull2bfloat16_ru(const unsigned long long int x)
{
    uint64_t y = x;
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __ull2bfloat16_rna(const unsigned long long int x)
{
    uint64_t y = x;
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __ll2bfloat16_rn(const long long int x)
{
    int64_t y = x;
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __ll2bfloat16_rz(const long long int x)
{
    int64_t y = x;
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __ll2bfloat16_rd(const long long int x)
{
    int64_t y = x;
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __ll2bfloat16_ru(const long long int x)
{
    int64_t y = x;
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __ll2bfloat16_rna(const long long int x)
{
    int64_t y = x;
    float f = __cvt_float<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
    return __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(f);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __float2bfloat162_rn(const float x)
{
    bfloat16x2_t tmp;
    bfloat16_t bf =
        __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    tmp.x = bf;
    tmp.y = bf;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __floats2bfloat162_rn(const float x, const float y)
{
    bfloat16x2_t tmp;
    tmp.x = __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
    tmp.y = __cvt_bfloat16_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(y);
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __float22bfloat162_rn(const float2 x)
{
    return __cvt_bfloat16x2_t<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __bfloat162bfloat162(const bfloat16_t x)
{
    bfloat16x2_t tmp;
    tmp.x = x;
    tmp.y = x;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __halves2bfloat162(const bfloat16_t x, const bfloat16_t y)
{
    bfloat16x2_t tmp;
    tmp.x = x;
    tmp.y = y;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __high2bfloat16(const bfloat16x2_t x) { return x.y; }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __high2bfloat162(const bfloat16x2_t x)
{
    bfloat16x2_t tmp;
    tmp.x = x.y;
    tmp.y = x.y;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __high2float(const bfloat16x2_t x)
{
    return __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x.y);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __highs2bfloat162(const bfloat16x2_t x, const bfloat16x2_t y)
{
    bfloat16x2_t tmp;
    tmp.x = x.y;
    tmp.y = y.y;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __low2bfloat16(const bfloat16x2_t x) { return x.x; }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __low2bfloat162(const bfloat16x2_t x)
{
    bfloat16x2_t tmp;
    tmp.x = x.x;
    tmp.y = x.x;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float __low2float(const bfloat16x2_t x)
{
    return __cvt_float<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x.x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __lowhigh2highlow(const bfloat16x2_t x)
{
    bfloat16x2_t tmp;
    tmp.x = x.y;
    tmp.y = x.x;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __lows2bfloat162(const bfloat16x2_t x, const bfloat16x2_t y)
{
    bfloat16x2_t tmp;
    tmp.x = x.x;
    tmp.y = y.x;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float2 __bfloat1622float2(const bfloat16x2_t x)
{
    return __cvt_float2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}
#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
#ifndef ASCENDC_CPU_DEBUG
__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_atomic_add(__ubuf__ bfloat16_t* address, bfloat16_t val)
{
    atomicAdd(address, val);
    return *address;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_add(__ubuf__ bfloat16x2_t* address, bfloat16x2_t val)
{
    return atomicAdd(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_sub(__ubuf__ bfloat16x2_t* address, bfloat16x2_t val)
{
    return atomicSub(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_atomic_sub(__ubuf__ bfloat16_t* address, bfloat16_t val)
{
    return asc_atomic_add(address, -val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_exch(__ubuf__ bfloat16x2_t* address, bfloat16x2_t val)
{
    return atomicExch(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_atomic_max(__ubuf__ bfloat16_t* address, bfloat16_t val)
{
    atomicMax(address, val);
    return *address;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_max(__ubuf__ bfloat16x2_t* address, bfloat16x2_t val)
{
    return atomicMax(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_atomic_min(__ubuf__ bfloat16_t* address, bfloat16_t val)
{
    atomicMin(address, val);
    return *address;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_min(__ubuf__ bfloat16x2_t* address, bfloat16x2_t val)
{
    return atomicMin(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_cas(
    __ubuf__ bfloat16x2_t* address, bfloat16x2_t compare, bfloat16x2_t val)
{
    return atomicCAS(address, compare, val);
}
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_atomic_sub(__gm__ bfloat16_t* address, bfloat16_t val)
{
    return asc_atomic_add(address, -val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_atomic_add(__gm__ bfloat16_t* address, bfloat16_t val)
{
    atomicAdd(address, val);
    return *address;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_add(__gm__ bfloat16x2_t* address, bfloat16x2_t val)
{
    return atomicAdd(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_sub(__gm__ bfloat16x2_t* address, bfloat16x2_t val)
{
    return atomicSub(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_exch(__gm__ bfloat16x2_t* address, bfloat16x2_t val)
{
    return atomicExch(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_atomic_max(__gm__ bfloat16_t* address, bfloat16_t val)
{
    atomicMax(address, val);
    return *address;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_max(__gm__ bfloat16x2_t* address, bfloat16x2_t val)
{
    return atomicMax(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_atomic_min(__gm__ bfloat16_t* address, bfloat16_t val)
{
    atomicMin(address, val);
    return *address;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_min(__gm__ bfloat16x2_t* address, bfloat16x2_t val)
{
    return atomicMin(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_cas(
    __gm__ bfloat16x2_t* address, bfloat16x2_t compare, bfloat16x2_t val)
{
    return atomicCAS(address, compare, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_ldcg(__gm__ bfloat16_t* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_ldcg(__gm__ bfloat16x2_t* address)
{
    int32_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        reinterpret_cast<__gm__ int32_t*>(address));
    return reinterpret_cast<bfloat16x2_t&>(t);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_ldca(__gm__ bfloat16_t* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_ldca(__gm__ bfloat16x2_t* address)
{
    int32_t t = __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        reinterpret_cast<__gm__ int32_t*>(address));
    return reinterpret_cast<bfloat16x2_t&>(t);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ bfloat16_t* address, bfloat16_t val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(__gm__ bfloat16x2_t* address, bfloat16x2_t val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        reinterpret_cast<__gm__ int32_t*>(address), reinterpret_cast<int32_t&>(val));
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ bfloat16_t* address, bfloat16_t val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(__gm__ bfloat16x2_t* address, bfloat16x2_t val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        reinterpret_cast<__gm__ int32_t*>(address), reinterpret_cast<int32_t&>(val));
}

#else
#ifndef ASCENDC_CPU_DEBUG
__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_atomic_add(bfloat16_t* address, bfloat16_t val)
{
    __atomic_add(address, val);
    return *address;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_add(bfloat16x2_t* address, bfloat16x2_t val)
{
    return __atomic_add(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_sub(bfloat16x2_t* address, bfloat16x2_t val)
{
    return __atomic_sub(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_atomic_sub(bfloat16_t* address, bfloat16_t val)
{
    return asc_atomic_add(address, -val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_exch(bfloat16x2_t* address, bfloat16x2_t val)
{
    return __atomic_exch(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_atomic_max(bfloat16_t* address, bfloat16_t val)
{
    __atomic_max(address, val);
    return *address;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_max(bfloat16x2_t* address, bfloat16x2_t val)
{
    return __atomic_max(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_atomic_min(bfloat16_t* address, bfloat16_t val)
{
    __atomic_min(address, val);
    return *address;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_min(bfloat16x2_t* address, bfloat16x2_t val)
{
    return __atomic_min(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_atomic_cas(
    bfloat16x2_t* address, bfloat16x2_t compare, bfloat16x2_t val)
{
    return __atomic_cas(address, compare, val);
}
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_ldcg(bfloat16_t* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_ldcg(bfloat16x2_t* address)
{
    int32_t t =
        __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(reinterpret_cast<int32_t*>(address));
    return reinterpret_cast<bfloat16x2_t&>(t);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_ldca(bfloat16_t* address)
{
    return __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_ldca(bfloat16x2_t* address)
{
    int32_t t =
        __ldg<LD_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(reinterpret_cast<int32_t*>(address));
    return reinterpret_cast<bfloat16x2_t&>(t);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(bfloat16_t* address, bfloat16_t val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stcg(bfloat16x2_t* address, bfloat16x2_t val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::NON_CACHEABLE>(
        reinterpret_cast<int32_t*>(address), reinterpret_cast<int32_t&>(val));
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(bfloat16_t* address, bfloat16_t val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(address, val);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void asc_stwt(bfloat16x2_t* address, bfloat16x2_t val)
{
    __stg<ST_L2CacheType::L2_CACHE_HINT_NORMAL_FV, L1CacheType::CACHEABLE>(
        reinterpret_cast<int32_t*>(address), reinterpret_cast<int32_t&>(val));
}
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_shfl(bfloat16_t var, int32_t src_lane, int32_t width)
{
    return __shfl(var, src_lane, width);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_shfl(bfloat16x2_t var, int32_t src_lane, int32_t width)
{
    return __shfl(var, src_lane, width);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_shfl_up(bfloat16_t var, uint32_t delta, int32_t width)
{
    return __shfl_up(var, delta, width);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_shfl_up(bfloat16x2_t var, uint32_t delta, int32_t width)
{
    return __shfl_up(var, delta, width);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_shfl_down(bfloat16_t var, uint32_t delta, int32_t width)
{
    return __shfl_down(var, delta, width);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_shfl_down(bfloat16x2_t var, uint32_t delta, int32_t width)
{
    return __shfl_down(var, delta, width);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t asc_shfl_xor(bfloat16_t var, int32_t lane_mask, int32_t width)
{
    return __shfl_xor(var, lane_mask, width);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t asc_shfl_xor(bfloat16x2_t var, int32_t lane_mask, int32_t width)
{
    return __shfl_xor(var, lane_mask, width);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t make_bfloat162(bfloat16_t x, bfloat16_t y)
{
    bfloat16x2_t tmp;
    tmp.x = x;
    tmp.y = y;
    return tmp;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16_t __ushort_as_bfloat16(const unsigned short int x)
{
    union Data {
        unsigned short int i;
        bfloat16_t f;
    };
    union Data data = {.i = x};
    return data.f;
}

#endif
#endif // IMPL_SIMT_API_ASC_BF16_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_BF16_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_BF16_IMPL__
#endif
