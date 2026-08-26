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
 * \file asc_fp8_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_FP8_IMPL__
#warning "impl/simt_api/asc_fp8_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "simt_api/asc_fp8.h" and use public functions or variables defined in interface header files."
#endif

#ifndef IMPL_SIMT_API_ASC_FP8_IMPL_H
#define IMPL_SIMT_API_ASC_FP8_IMPL_H

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#include "__clang_cce_simt_fp8.h"
#endif

#include "simt_api/device_types.h"
#include "impl/simt_api/internal_functions_impl.h"

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)

__SIMT_DEVICE_FUNCTIONS_DECL__ inline hifloat8x2_t __float22hif82_rna(const float2 x)
{
    return __cvt_hifloat8x2_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline hifloat8x2_t __float22hif82_rna_sat(const float2 x)
{
    return __cvt_hifloat8x2_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline hifloat8x2_t __float22hif82_rh(const float2 x)
{
    return __cvt_hifloat8x2_t<__internal_get_round<__RoundMode::CAST_HYBRID>(), RoundingSaturation::RS_DISABLE_VALUE>(
        x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline hifloat8x2_t __float22hif82_rh_sat(const float2 x)
{
    return __cvt_hifloat8x2_t<__internal_get_round<__RoundMode::CAST_HYBRID>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline hifloat8x2_t __half22hif82_rna(const half2 x)
{
    return __cvt_hifloat8x2_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline hifloat8x2_t __half22hif82_rna_sat(const half2 x)
{
    return __cvt_hifloat8x2_t<__internal_get_round<__RoundMode::CAST_ROUND>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline hifloat8x2_t __half22hif82_rh(const half2 x)
{
    return __cvt_hifloat8x2_t<__internal_get_round<__RoundMode::CAST_HYBRID>(), RoundingSaturation::RS_DISABLE_VALUE>(
        x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline hifloat8x2_t __half22hif82_rh_sat(const half2 x)
{
    return __cvt_hifloat8x2_t<__internal_get_round<__RoundMode::CAST_HYBRID>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float2 __hif822float2(const hifloat8x2_t x)
{
    return __cvt_float2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 __hif822half2(const hifloat8x2_t x)
{
    return __cvt_half2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float2 __e4m3x22float2(const float8_e4m3x2_t x)
{
    return __cvt_float2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float2 __e5m2x22float2(const float8_e5m2x2_t x)
{
    return __cvt_float2<__internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline __asc_fp8x2_storage_t __asc_cvt_float2_to_fp8x2(
    const float2 x, const __asc_saturation_t saturate, const __asc_fp8_interpretation_t fp8_interpretation)
{
    __asc_fp8x2_storage_t res = 0;
    if (saturate == __ASC_NOSAT) {
        if (fp8_interpretation == __ASC_E4M3) {
            float8_e4m3x2_t tmp = __cvt_float8_e4m3x2_t<
                __internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
            res = *reinterpret_cast<__asc_fp8x2_storage_t*>(&tmp);
        } else {
            float8_e5m2x2_t tmp = __cvt_float8_e5m2x2_t<
                __internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_DISABLE_VALUE>(x);
            res = *reinterpret_cast<__asc_fp8x2_storage_t*>(&tmp);
        }
    } else {
        if (fp8_interpretation == __ASC_E4M3) {
            float8_e4m3x2_t tmp = __cvt_float8_e4m3x2_t<
                __internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
            res = *reinterpret_cast<__asc_fp8x2_storage_t*>(&tmp);
        } else {
            float8_e5m2x2_t tmp = __cvt_float8_e5m2x2_t<
                __internal_get_round<__RoundMode::CAST_RINT>(), RoundingSaturation::RS_ENABLE_VALUE>(x);
            res = *reinterpret_cast<__asc_fp8x2_storage_t*>(&tmp);
        }
    }
    return res;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline __asc_fp8_storage_t __asc_cvt_float_to_fp8(
    const float x, const __asc_saturation_t saturation, const __asc_fp8_interpretation_t fp8_interpretation)
{
    float2 f{x, 0.0f};
    __asc_fp8x2_storage_t tmp = __asc_cvt_float2_to_fp8x2(f, saturation, fp8_interpretation);
    __asc_fp8_storage_t res = (__asc_fp8_storage_t)(tmp);
    return res;
}

#endif
#endif // IMPL_SIMT_API_ASC_FP8_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_FP8_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_FP8_IMPL__
#endif
