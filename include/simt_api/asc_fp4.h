/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef INCLUDE_SIMT_API_ASC_FP4_H
#define INCLUDE_SIMT_API_ASC_FP4_H

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_FP4_H__
#endif

#if (__NPU_ARCH__ == 9201) || (__NPU_ARCH__ == 9202)

#include "simt_api/asc_bf16.h"
#include "simt_api/device_types.h"

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __fp4x2_e1m22bfloat162(const float4_e1m2x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e1m2x2_t __bfloat1622fp4x2_e1m2_rn(const bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e1m2x2_t __bfloat1622fp4x2_e1m2_rna(const bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e1m2x2_t __bfloat1622fp4x2_e1m2_rd(const bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e1m2x2_t __bfloat1622fp4x2_e1m2_ru(const bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e1m2x2_t __bfloat1622fp4x2_e1m2_rz(const bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline bfloat16x2_t __fp4x2_e2m12bfloat162(const float4_e2m1x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e2m1x2_t __bfloat1622fp4x2_e2m1_rn(const bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e2m1x2_t __bfloat1622fp4x2_e2m1_rna(const bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e2m1x2_t __bfloat1622fp4x2_e2m1_rd(const bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e2m1x2_t __bfloat1622fp4x2_e2m1_ru(const bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e2m1x2_t __bfloat1622fp4x2_e2m1_rz(const bfloat16x2_t x);

#if (__NPU_ARCH__ == 9202)

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e1m2x2_t __bfloat1622fp4x2_e1m2_rha(const bfloat16x2_t x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e2m1x2_t __hmul22fp4x2_e2m1_rna(const half2 x, const half2 y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e2m1x2_t __hmul22fp4x2_e2m1_rha(const half2 x, const half2 y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e1m2x2_t __hmul22fp4x2_e1m2_rna(const half2 x, const half2 y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e1m2x2_t __hmul22fp4x2_e1m2_rha(const half2 x, const half2 y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e2m1x2_t __hmul22fp4x2_e2m1_rna(const bfloat16x2 x, const bfloat16x2 y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e2m1x2_t __hmul22fp4x2_e2m1_rha(const bfloat16x2 x, const bfloat16x2 y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e1m2x2_t __hmul22fp4x2_e1m2_rna(const bfloat16x2 x, const bfloat16x2 y);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float4_e1m2x2_t __hmul22fp4x2_e1m2_rha(const bfloat16x2 x, const bfloat16x2 y);
#endif

#ifndef __NPU_COMPILER_INTERNAL_PURE_SIMT__
#include "impl/simt_api/asc_fp4_impl.h"
#endif

#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_FP4_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_ASC_FP4_H__
#endif

#endif // INCLUDE_SIMT_API_ASC_FP4_H
