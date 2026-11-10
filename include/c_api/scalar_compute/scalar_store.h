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
#error "scalar_store.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_COMPUTE_SCALAR_STORE_H
#endif

#ifndef INCLUDE_C_API_SCALAR_COMPUTE_SCALAR_STORE_H
#define INCLUDE_C_API_SCALAR_COMPUTE_SCALAR_STORE_H

#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/scalar_store_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.scalar_compute.scalar_store
 * @brief Writes the int8_t value to the GM address addr without going through the DCache.
 *
 * @override_id asc_store_dev__PGi8_i8
 * @archs 3510
 * @param [out] addr Destination GM address the int8_t value is stored to; its start address must be aligned to
 * sizeof(int8_t).
 * @param [in] value The int8_t value written to addr.
 */
__aicore__ inline void asc_store_dev(__gm__ int8_t* addr, int8_t value);
/**
 * @ingroup simd_c_api.scalar_compute.scalar_store
 * @brief Writes the uint8_t value to the GM address addr without going through the DCache.
 *
 * @override_id asc_store_dev__PGu8_u8
 * @archs 3510
 * @param [out] addr Destination GM address the uint8_t value is stored to; its start address must be aligned to
 * sizeof(uint8_t).
 * @param [in] value The uint8_t value written to addr.
 */
__aicore__ inline void asc_store_dev(__gm__ uint8_t* addr, uint8_t value);
/**
 * @ingroup simd_c_api.scalar_compute.scalar_store
 * @brief Writes the int16_t value to the GM address addr without going through the DCache.
 *
 * @override_id asc_store_dev__PGi16_i16
 * @archs 3510
 * @param [out] addr Destination GM address the int16_t value is stored to; its start address must be aligned to
 * sizeof(int16_t).
 * @param [in] value The int16_t value written to addr.
 */
__aicore__ inline void asc_store_dev(__gm__ int16_t* addr, int16_t value);
/**
 * @ingroup simd_c_api.scalar_compute.scalar_store
 * @brief Writes the uint16_t value to the GM address addr without going through the DCache.
 *
 * @override_id asc_store_dev__PGu16_u16
 * @archs 3510
 * @param [out] addr Destination GM address the uint16_t value is stored to; its start address must be aligned to
 * sizeof(uint16_t).
 * @param [in] value The uint16_t value written to addr.
 */
__aicore__ inline void asc_store_dev(__gm__ uint16_t* addr, uint16_t value);
/**
 * @ingroup simd_c_api.scalar_compute.scalar_store
 * @brief Writes the int32_t value to the GM address addr without going through the DCache.
 *
 * @override_id asc_store_dev__PGi32_i32
 * @archs 3510
 * @param [out] addr Destination GM address the int32_t value is stored to; its start address must be aligned to
 * sizeof(int32_t).
 * @param [in] value The int32_t value written to addr.
 */
__aicore__ inline void asc_store_dev(__gm__ int32_t* addr, int32_t value);
/**
 * @ingroup simd_c_api.scalar_compute.scalar_store
 * @brief Writes the uint32_t value to the GM address addr without going through the DCache.
 *
 * @override_id asc_store_dev__PGu32_u32
 * @archs 3510
 * @param [out] addr Destination GM address the uint32_t value is stored to; its start address must be aligned to
 * sizeof(uint32_t).
 * @param [in] value The uint32_t value written to addr.
 */
__aicore__ inline void asc_store_dev(__gm__ uint32_t* addr, uint32_t value);
/**
 * @ingroup simd_c_api.scalar_compute.scalar_store
 * @brief Writes the int64_t value to the GM address addr without going through the DCache.
 *
 * @override_id asc_store_dev__PGi64_i64
 * @archs 3510
 * @param [out] addr Destination GM address the int64_t value is stored to; its start address must be aligned to
 * sizeof(int64_t).
 * @param [in] value The int64_t value written to addr.
 */
__aicore__ inline void asc_store_dev(__gm__ int64_t* addr, int64_t value);
/**
 * @ingroup simd_c_api.scalar_compute.scalar_store
 * @brief Writes the uint64_t value to the GM address addr without going through the DCache.
 *
 * @override_id asc_store_dev__PGu64_u64
 * @archs 3510
 * @param [out] addr Destination GM address the uint64_t value is stored to; its start address must be aligned to
 * sizeof(uint64_t).
 * @param [in] value The uint64_t value written to addr.
 */
__aicore__ inline void asc_store_dev(__gm__ uint64_t* addr, uint64_t value);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_COMPUTE_SCALAR_STORE_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_COMPUTE_SCALAR_STORE_H
#endif
