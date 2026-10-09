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
 * \file asc_fp4_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_FP4_IMPL__
#warning "impl/simt_api/asc_fp4_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "simt_api/asc_fp4.h" and use public functions or variables defined in interface header files."
#endif

#ifndef IMPL_SIMT_API_ASC_FP4_IMPL_H
#define IMPL_SIMT_API_ASC_FP4_IMPL_H

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#include "__clang_cce_simt_fp4.h"
#endif

#include "simt_api/device_types.h"
#include "impl/simt_api/internal_functions_impl.h"

#if (__NPU_ARCH__ == 9201) || (__NPU_ARCH__ == 9202)

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __fp4x2_e1m22bfloat162(const float4_e1m2x2_t x)
{
    return __cvt_bfloat16x2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __fp4x2_e2m12bfloat162(const float4_e2m1x2_t x)
{
    return __cvt_bfloat16x2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e1m2x2_t __bfloat1622fp4x2_e1m2_rn(const bfloat16x2_t x)
{
    return __cvt_float4_e1m2x2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e1m2x2_t __bfloat1622fp4x2_e1m2_rna(const bfloat16x2_t x)
{
    return __cvt_float4_e1m2x2<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(
        x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e1m2x2_t __bfloat1622fp4x2_e1m2_rd(const bfloat16x2_t x)
{
    return __cvt_float4_e1m2x2<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(
        x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e1m2x2_t __bfloat1622fp4x2_e1m2_ru(const bfloat16x2_t x)
{
    return __cvt_float4_e1m2x2<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e1m2x2_t __bfloat1622fp4x2_e1m2_rz(const bfloat16x2_t x)
{
    return __cvt_float4_e1m2x2<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(
        x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e2m1x2_t __bfloat1622fp4x2_e2m1_rn(const bfloat16x2_t x)
{
    return __cvt_float4_e2m1x2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e2m1x2_t __bfloat1622fp4x2_e2m1_rna(const bfloat16x2_t x)
{
    return __cvt_float4_e2m1x2<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(
        x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e2m1x2_t __bfloat1622fp4x2_e2m1_rd(const bfloat16x2_t x)
{
    return __cvt_float4_e2m1x2<__internal_get_round<__RoundMode::CAST_FLOOR>(), RoundingSaturation::RS_DISABLE_VALUE>(
        x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e2m1x2_t __bfloat1622fp4x2_e2m1_ru(const bfloat16x2_t x)
{
    return __cvt_float4_e2m1x2<__internal_get_round<__RoundMode::CAST_CEIL>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e2m1x2_t __bfloat1622fp4x2_e2m1_rz(const bfloat16x2_t x)
{
    return __cvt_float4_e2m1x2<__internal_get_round<__RoundMode::CAST_TRUNC>(), RoundingSaturation::RS_DISABLE_VALUE>(
        x);
}

#if (__NPU_ARCH__ == 9202)

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e1m2x2_t __bfloat1622fp4x2_e1m2_rha(const bfloat16x2_t x)
{
    return __cvt_float4_e1m2x2<__internal_get_round<__RoundMode::CAST_HYBRID>(), RoundingSaturation::RS_DISABLE_VALUE>(
        x);
}
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e2m1x2_t __hmul22fp4x2_e2m1_rna(const half2 x, const half2 y)
{
    return __fmulcvt_float4_e2m1x2<__internal_get_round<__RoundMode::CAST_ROUND>()>(x, y);
}
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e2m1x2_t __hmul22fp4x2_e2m1_rha(const half2 x, const half2 y)
{
    return __fmulcvt_float4_e2m1x2<__internal_get_round<__RoundMode::CAST_HYBRID>()>(x, y);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e1m2x2_t __hmul22fp4x2_e1m2_rna(const half2 x, const half2 y)
{
    return __fmulcvt_float4_e1m2x2<__internal_get_round<__RoundMode::CAST_ROUND>()>(x, y);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e1m2x2_t __hmul22fp4x2_e1m2_rha(const half2 x, const half2 y)
{
    return __fmulcvt_float4_e1m2x2<__internal_get_round<__RoundMode::CAST_HYBRID>()>(x, y);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e2m1x2_t __hmul22fp4x2_e2m1_rna(const bfloat16x2 x, const bfloat16x2 y)
{
    return __fmulcvt_float4_e2m1x2<__internal_get_round<__RoundMode::CAST_ROUND>()>(x, y);
}
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e2m1x2_t __hmul22fp4x2_e2m1_rha(const bfloat16x2 x, const bfloat16x2 y)
{
    return __fmulcvt_float4_e2m1x2<__internal_get_round<__RoundMode::CAST_HYBRID>()>(x, y);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e1m2x2_t __hmul22fp4x2_e1m2_rna(const bfloat16x2 x, const bfloat16x2 y)
{
    return __fmulcvt_float4_e1m2x2<__internal_get_round<__RoundMode::CAST_ROUND>()>(x, y);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e1m2x2_t __hmul22fp4x2_e1m2_rha(const bfloat16x2 x, const bfloat16x2 y)
{
    return __fmulcvt_float4_e1m2x2<__internal_get_round<__RoundMode::CAST_HYBRID>()>(x, y);
}

#endif
#endif
#endif // IMPL_SIMT_API_ASC_FP4_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_FP4_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_FP4_IMPL__
#endif
