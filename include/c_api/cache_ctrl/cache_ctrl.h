/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "cache_ctrl.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_CACHE_CTRL_CACHE_CTRL_H
#endif

#ifndef INCLUDE_C_API_CACHE_CTRL_CACHE_CTRL_H
#define INCLUDE_C_API_CACHE_CTRL_CACHE_CTRL_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/memory_base_impl/cache_ctrl_intf_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/cache_ctrl_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.cache_ctrl
 * @brief Preloads instruction data starting at a Global Memory address into the ICache in units of 2 KByte to avoid
 * later instruction-cache misses.
 *
 * @override_id asc_icache_preload__PCv_i64
 * @archs 2201 3510
 * @param [in] addr Global Memory start address of the instructions to preload; hardware uses the 128-byte-aligned
 * effective bits of this address.
 * @param [in] prefetch_len Length of the instruction data to preload, counted in 2-KByte instruction units; documented
 * range [0, 15] on AIC (32 KB ICache) and [0, 7] on AIV (16 KB ICache).
 */
__aicore__ inline void asc_icache_preload(const void* addr, int64_t prefetch_len);

namespace __asc_aicore {
/**
 * @ingroup simd_c_api.cache_ctrl
 * @brief Cleans and invalidates the single DCache line that contains the GM address dst, so the next scalar access to
 * that address reads fresh GM data.
 *
 * @override_id asc_dcci_single__PGv
 * @archs 2201 3510
 * @param [in] dst Address of the cache line to clean and invalidate, passed as __gm__ void*; the doc states the address
 * need not be Cache Line aligned because hardware aligns to the line boundary.
 */
__aicore__ inline void asc_dcci_single(__gm__ void* dst);
} // namespace __asc_aicore

/**
 * @ingroup simd_c_api.cache_ctrl
 * @brief Cleans and invalidates the single DCache line that contains the Unified Buffer address dst (AIV only).
 *
 * @override_id asc_ub_dcci_single__PUv
 * @archs 2201 3510
 * @param [in] dst Address of the cache line to clean and invalidate, passed as __ubuf__ void*; the doc states the
 * address need not be Cache Line aligned because hardware aligns to the line boundary.
 */
__aicore__ inline void asc_ub_dcci_single(__ubuf__ void* dst);

/**
 * @ingroup simd_c_api.cache_ctrl
 * @brief Cleans and invalidates every valid DCache line whose address belongs to the Unified Buffer address range.
 *
 * @override_id asc_dcci_entire_ub__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_dcci_entire_ub();

/**
 * @ingroup simd_c_api.cache_ctrl
 * @brief Cleans and invalidates every valid DCache line whose address belongs to GM, writing dirty data back to GM.
 *
 * @override_id asc_dcci_entire_out__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_dcci_entire_out();

/**
 * @ingroup simd_c_api.cache_ctrl
 * @brief Cleans and invalidates every valid DCache line of the current AI Core, covering both Unified Buffer and GM
 * addresses.
 *
 * @override_id asc_dcci_entire_all__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_dcci_entire_all();

/**
 * @ingroup simd_c_api.cache_ctrl
 * @brief Cleans and invalidates every valid DCache line associated with GM addresses used by scalar atomic operations.
 *
 * @override_id asc_dcci_entire_atomic__z
 * @archs 2201 3510
 */
__aicore__ inline void asc_dcci_entire_atomic();

/**
 * @ingroup simd_c_api.cache_ctrl
 * @brief Returns the ICache preload status: 0 when the previous preload has finished and 1 while a preload is still
 * running.
 *
 * @override_id asc_get_icache_preload_status__z
 * @archs 2201 3510
 */
__aicore__ inline int64_t asc_get_icache_preload_status();

/**
 * @ingroup simd_c_api.cache_ctrl
 * @brief Preloads the 64-byte cache line containing address plus offset from Global Memory into the DCache for later
 * scalar reads.
 *
 * @override_id asc_datacache_preload__PGu64_i64
 * @archs 2201 3510
 * @param [in] address Source address in Global Memory, expressed in bytes; the prefetch base.
 * @param [in] offset Signed byte offset added to address; the prefetch starts at address + offset.
 */
__aicore__ inline void asc_datacache_preload(__gm__ uint64_t* address, int64_t offset);

/**
 * @ingroup simd_c_api.cache_ctrl
 * @brief Invalidates the whole DCache by marking every cache line invalid without writing back dirty data, so later
 * accesses reload from Global Memory.
 *
 * @override_id asc_dci__z
 * @archs 3510
 */
__aicore__ inline void asc_dci();

/**
 * @ingroup simd_c_api.cache_ctrl
 * @brief Sets the L2 cache management policy applied by subsequent scalar GM load operations until the mode is changed
 * again.
 *
 * @override_id asc_set_scalar_cache_mode__loadl2
 * @archs 3510
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enumerator selecting the L2 policy for the scalar load path; values
 * NORMAL_FIRST_VICTIM=0, NORMAL_LAST_VICTIM=1, NORMAL_PERSISTENT=2, NOTALLOC_KEEP=4, NOTALLOC_CLEAN=5, NOTALLOC_DROP=6.
 */
__aicore__ inline void asc_set_scalar_cache_mode(asc_load_l2_cache_mode l2_cache_mode);

/**
 * @ingroup simd_c_api.cache_ctrl
 * @brief Sets the L2 cache management policy applied by subsequent scalar GM store operations until the mode is changed
 * again.
 *
 * @override_id asc_set_scalar_cache_mode__storel2
 * @archs 3510
 * @param [in] l2_cache_mode asc_store_l2_cache_mode enumerator selecting the L2 policy for the scalar store path;
 * values NORMAL_FIRST_VICTIM=0, NORMAL_LAST_VICTIM=1, NORMAL_PERSISTENT=2, NOTALLOC_CLEAN=4.
 */
__aicore__ inline void asc_set_scalar_cache_mode(asc_store_l2_cache_mode l2_cache_mode);

/**
 * @ingroup simd_c_api.cache_ctrl
 * @brief Returns the current L2 Cache management policy of the scalar load path as an asc_load_l2_cache_mode value.
 *
 * @override_id asc_get_scalar_load_cache_mode__z
 * @archs 3510
 */
__aicore__ inline asc_load_l2_cache_mode asc_get_scalar_load_cache_mode();

/**
 * @ingroup simd_c_api.cache_ctrl
 * @brief Returns the current L2 Cache management policy of the scalar store path as an asc_store_l2_cache_mode value.
 *
 * @override_id asc_get_scalar_store_cache_mode__z
 * @archs 3510
 */
__aicore__ inline asc_store_l2_cache_mode asc_get_scalar_store_cache_mode();

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_CACHE_CTRL_CACHE_CTRL_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_CACHE_CTRL_CACHE_CTRL_H
#endif
