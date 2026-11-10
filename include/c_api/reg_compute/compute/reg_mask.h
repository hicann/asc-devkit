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
#error "compute/reg_mask.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_MASK_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_MASK_H
#define INCLUDE_C_API_REG_COMPUTE_COMPUTE_REG_MASK_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/reg_mask_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.reg_compute.reg_mask
 * @brief Generates a b8 mask from the element count value and subtracts the 256-element vector processing granularity
 * from value in place.
 *
 * @override_id asc_update_mask_b8__Ru32
 * @archs 3510
 * @param [in,out] value Number of remaining elements; after the call it becomes value = (value < 256) ? 0 : (value -
 * 256), so the count is consumed by one b8 vector processing unit.
 */
__simd_callee__ inline vector_bool asc_update_mask_b8(uint32_t& value);

/**
 * @ingroup simd_c_api.reg_compute.reg_mask
 * @brief Generates a b16 mask from the element count value and subtracts the 128-element vector processing granularity
 * from value in place.
 *
 * @override_id asc_update_mask_b16__Ru32
 * @archs 3510
 * @param [in,out] value Number of remaining elements; after the call it becomes value = (value < 128) ? 0 : (value -
 * 128), so the count is consumed by one b16 vector processing unit.
 */
__simd_callee__ inline vector_bool asc_update_mask_b16(uint32_t& value);

/**
 * @ingroup simd_c_api.reg_compute.reg_mask
 * @brief Generates a b32 mask from the element count value and subtracts the 64-element vector processing granularity
 * from value in place.
 *
 * @override_id asc_update_mask_b32__Ru32
 * @archs 3510
 * @param [in,out] value Number of remaining elements; after the call it becomes value = (value < 64) ? 0 : (value -
 * 64), so the count is consumed by one b32 vector processing unit.
 */
__simd_callee__ inline vector_bool asc_update_mask_b32(uint32_t& value);

/**
 * @ingroup simd_c_api.reg_compute.reg_mask
 * @brief Reads the b16 mask value from the SPR and returns it as a 256-bit mask register with every SPR bit expanded to
 * two bits.
 *
 * @override_id asc_get_mask_spr_b16__z
 * @archs 3510
 */
__simd_callee__ inline vector_bool asc_get_mask_spr_b16();

/**
 * @ingroup simd_c_api.reg_compute.reg_mask
 * @brief Reads the b32 mask value from the SPR and returns it as a 256-bit mask register with every SPR bit expanded to
 * four bits.
 *
 * @override_id asc_get_mask_spr_b32__z
 * @archs 3510
 */
__simd_callee__ inline vector_bool asc_get_mask_spr_b32();

#define asc_create_mask_b8 pset_b8

#define asc_create_mask_b16 pset_b16

#define asc_create_mask_b32 pset_b32

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_MASK_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_COMPUTE_REG_MASK_H
#endif
