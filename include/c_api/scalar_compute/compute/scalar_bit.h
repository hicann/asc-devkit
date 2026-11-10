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
#error "scalar_bit.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_COMPUTE_COMPUTE_SCALAR_BIT_H
#endif

#ifndef INCLUDE_C_API_SCALAR_COMPUTE_COMPUTE_SCALAR_BIT_H
#define INCLUDE_C_API_SCALAR_COMPUTE_COMPUTE_SCALAR_BIT_H

#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/memory_base_impl/scalar_bit_intf_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/scalar_bit_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.scalar_compute.scalar_bit
 * @brief Counts the consecutive bits equal to the sign bit, starting from the highest value bit, of a 64-bit signed
 * integer.
 *
 * @override_id asc_sflbits__i64
 * @archs 2201 3510
 * @param [in] value 64-bit signed integer to inspect; range [-2^63, 2^63 - 1].
 */
__aicore__ inline int64_t asc_sflbits(int64_t value);
/**
 * @ingroup simd_c_api.scalar_compute.scalar_bit
 * @brief Finds the index of the first zero bit (from the least significant bit) in a 64-bit unsigned integer.
 *
 * @override_id asc_ffz__u64
 * @archs 2201 3510
 * @param [in] value Input 64-bit unsigned integer to scan; range [0, 2^64 - 1].
 */
__aicore__ inline int64_t asc_ffz(uint64_t value);
/**
 * @ingroup simd_c_api.scalar_compute.scalar_bit
 * @brief Finds the index of the first set bit (from the least significant bit) in a 64-bit unsigned integer.
 *
 * @override_id asc_ffs__u64
 * @archs 2201 3510
 * @param [in] value Input 64-bit unsigned integer to scan; range [0, 2^64 - 1].
 */
__aicore__ inline int64_t asc_ffs(uint64_t value);
/**
 * @ingroup simd_c_api.scalar_compute.scalar_bit
 * @brief Counts the number of set bits in a 64-bit unsigned integer.
 *
 * @override_id asc_popc__u64
 * @archs 2201 3510
 * @param [in] value 64-bit unsigned integer whose set bits are counted; range [0, 2^64 - 1].
 */
__aicore__ inline int64_t asc_popc(uint64_t value);
/**
 * @ingroup simd_c_api.scalar_compute.scalar_bit
 * @brief Counts the leading zero bits of a 64-bit unsigned integer.
 *
 * @override_id asc_clz__u64
 * @archs 2201 3510
 * @param [in] value_in 64-bit unsigned integer whose leading zeros are counted; range [0, 2^64 - 1].
 */
__aicore__ inline int64_t asc_clz(uint64_t value_in);
/**
 * @ingroup simd_c_api.scalar_compute.scalar_bit
 * @brief Counts the number of zero bits in a 64-bit unsigned integer.
 *
 * @override_id asc_zero_bits_cnt__u64
 * @archs 2201 3510
 * @param [in] value 64-bit unsigned integer whose zero bits are counted; range [0, 2^64 - 1].
 */
__aicore__ inline int64_t asc_zero_bits_cnt(uint64_t value);
/**
 * @ingroup simd_c_api.scalar_compute.scalar_bit
 * @brief Sets bit idx of a 64-bit unsigned integer to 1 and returns the modified value.
 *
 * @override_id asc_set_nthbit__u64_i64
 * @archs 2201 3510
 * @param [in] bits 64-bit unsigned integer to modify; range [0, 2^64 - 1], and the page states the input value itself
 * stays unchanged after the computation.
 * @param [in] idx Bit index to set; the effective index is computed as (idx % 64 + 64) % 64, giving an index range of
 * [0, 63].
 */
__aicore__ inline uint64_t asc_set_nthbit(uint64_t bits, int64_t idx);
/**
 * @ingroup simd_c_api.scalar_compute.scalar_bit
 * @brief Clears bit idx of a 64-bit unsigned integer and returns the modified value.
 *
 * @override_id asc_clear_nthbit__u64_i64
 * @archs 2201 3510
 * @param [in] bits 64-bit unsigned integer whose bits are modified; range [0, 2^64 - 1].
 * @param [in] idx Bit index to clear; the effective index is computed as (idx % 64 + 64) % 64, giving a range of [0,
 * 63].
 */
__aicore__ inline uint64_t asc_clear_nthbit(uint64_t bits, int64_t idx);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_COMPUTE_COMPUTE_SCALAR_BIT_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_COMPUTE_COMPUTE_SCALAR_BIT_H
#endif
