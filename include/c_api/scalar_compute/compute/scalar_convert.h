/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "scalar_convert.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_COMPUTE_COMPUTE_SCALAR_CONVERT_H
#endif

#ifndef INCLUDE_C_API_SCALAR_COMPUTE_COMPUTE_SCALAR_CONVERT_H
#define INCLUDE_C_API_SCALAR_COMPUTE_COMPUTE_SCALAR_CONVERT_H

#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/scalar_convert_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.scalar_compute.scalar_convert
 * @brief Converts a float scalar to half with a fixed round-to-nearest-odd rounding mode.
 *
 * @override_id asc_float2half_ro__f32
 * @archs 3510
 * @param [in] value Source scalar of type float to be converted.
 */
__aicore__ inline half asc_float2half_ro(float value);

/**
 * @ingroup simd_c_api.scalar_compute.scalar_convert
 * @brief Converts the scalar float value to int32_t using FLOOR rounding (toward negative infinity) and returns the
 * int32_t result.
 *
 * @override_id asc_float2int32_rd__f32
 * @archs 3510
 * @param [in] value Source scalar of type float that is converted; passed by value and never written.
 */
__aicore__ inline int32_t asc_float2int32_rd(float value);
/**
 * @ingroup simd_c_api.scalar_compute.scalar_convert
 * @brief Converts the scalar float value to int32_t using CEIL rounding (toward positive infinity) and returns the
 * int32_t result.
 *
 * @override_id asc_float2int32_ru__f32
 * @archs 3510
 * @param [in] value Source scalar of type float that is converted; passed by value and never written.
 */
__aicore__ inline int32_t asc_float2int32_ru(float value);
/**
 * @ingroup simd_c_api.scalar_compute.scalar_convert
 * @brief Converts the scalar float value to int32_t using RINT rounding (with ties rounded to the nearest even value
 * (bankers rounding)) and returns the int32_t result.
 *
 * @override_id asc_float2int32_rn__f32
 * @archs 3510
 * @param [in] value Source scalar of type float that is converted; passed by value and never written.
 */
__aicore__ inline int32_t asc_float2int32_rn(float value);
/**
 * @ingroup simd_c_api.scalar_compute.scalar_convert
 * @brief Converts the scalar float value to int32_t using ROUND rounding (with a fractional part of 0.5 or more rounded
 * away from zero) and returns the int32_t result.
 *
 * @override_id asc_float2int32_rna__f32
 * @archs 3510
 * @param [in] value Source scalar of type float that is converted; passed by value and never written.
 */
__aicore__ inline int32_t asc_float2int32_rna(float value);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_COMPUTE_COMPUTE_SCALAR_CONVERT_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_COMPUTE_COMPUTE_SCALAR_CONVERT_H
#endif
