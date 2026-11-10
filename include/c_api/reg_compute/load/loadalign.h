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
#error "reg_compute/load/loadalign.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_LOAD_LOADALIGN_H
#endif

#ifndef INCLUDE_C_API_REG_COMPUTE_LOAD_LOADALIGN_H
#define INCLUDE_C_API_REG_COMPUTE_LOAD_LOADALIGN_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/loadalign_intf_impl.h"
#endif

// ========== return-value load APIs ==========
// ========== return vector data register APIs ==========
// ========== asc_loadalign ==========
/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads int4b_t data starting at the 32-byte-aligned UB address src and returns it in a vector_int4x2_t register
 * without changing the data format or content.
 *
 * @override_id asc_loadalign__PUi4x2
 * @archs 3510
 * @param [in] src Source UB address of the int4b_t data to load; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline vector_int4x2_t asc_loadalign(__ubuf__ int4b_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads int8_t data starting at the 32-byte-aligned UB address src and returns it in a vector_int8_t register
 * without changing the data format or content.
 *
 * @override_id asc_loadalign__PUi8
 * @archs 3510
 * @param [in] src Source UB address of the int8_t data to load; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline vector_int8_t asc_loadalign(__ubuf__ int8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads uint8_t data starting at the 32-byte-aligned UB address src and returns it in a vector_uint8_t register
 * without changing the data format or content.
 *
 * @override_id asc_loadalign__PUu8
 * @archs 3510
 * @param [in] src Source UB address of the uint8_t data to load; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline vector_uint8_t asc_loadalign(__ubuf__ uint8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads fp4x2_e2m1_t data starting at the 32-byte-aligned UB address src and returns it in a vector_fp4x2_e2m1_t
 * register without changing the data format or content.
 *
 * @override_id asc_loadalign__PUf4e2m1x2
 * @archs 3510
 * @param [in] src Source UB address of the fp4x2_e2m1_t data to load; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline vector_fp4x2_e2m1_t asc_loadalign(__ubuf__ fp4x2_e2m1_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads fp4x2_e1m2_t data starting at the 32-byte-aligned UB address src and returns it in a vector_fp4x2_e1m2_t
 * register without changing the data format or content.
 *
 * @override_id asc_loadalign__PUf4e1m2x2
 * @archs 3510
 * @param [in] src Source UB address of the fp4x2_e1m2_t data to load; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline vector_fp4x2_e1m2_t asc_loadalign(__ubuf__ fp4x2_e1m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads hifloat8_t data starting at the 32-byte-aligned UB address src and returns it in a vector_hifloat8_t
 * register without changing the data format or content.
 *
 * @override_id asc_loadalign__PUhf8
 * @archs 3510
 * @param [in] src Source UB address of the hifloat8_t data to load; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline vector_hifloat8_t asc_loadalign(__ubuf__ hifloat8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads fp8_e8m0_t data starting at the 32-byte-aligned UB address src and returns it in a vector_fp8_e8m0_t
 * register without changing the data format or content.
 *
 * @override_id asc_loadalign__PUf8e8m0
 * @archs 3510
 * @param [in] src Source UB address of the fp8_e8m0_t data to load; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline vector_fp8_e8m0_t asc_loadalign(__ubuf__ fp8_e8m0_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads fp8_e5m2_t data starting at the 32-byte-aligned UB address src and returns it in a vector_fp8_e5m2_t
 * register without changing the data format or content.
 *
 * @override_id asc_loadalign__PUf8e5m2
 * @archs 3510
 * @param [in] src Source UB address of the fp8_e5m2_t data to load; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline vector_fp8_e5m2_t asc_loadalign(__ubuf__ fp8_e5m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads fp8_e4m3fn_t data starting at the 32-byte-aligned UB address src and returns it in a vector_fp8_e4m3fn_t
 * register without changing the data format or content.
 *
 * @override_id asc_loadalign__PUf8e4m3
 * @archs 3510
 * @param [in] src Source UB address of the fp8_e4m3fn_t data to load; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline vector_fp8_e4m3fn_t asc_loadalign(__ubuf__ fp8_e4m3fn_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads int16_t data starting at the 32-byte-aligned UB address src and returns it in a vector_int16_t register
 * without changing the data format or content.
 *
 * @override_id asc_loadalign__PUi16
 * @archs 3510
 * @param [in] src Source UB address of the int16_t data to load; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline vector_int16_t asc_loadalign(__ubuf__ int16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads uint16_t data starting at the 32-byte-aligned UB address src and returns it in a vector_uint16_t
 * register without changing the data format or content.
 *
 * @override_id asc_loadalign__PUu16
 * @archs 3510
 * @param [in] src Source UB address of the uint16_t data to load; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline vector_uint16_t asc_loadalign(__ubuf__ uint16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads half data starting at the 32-byte-aligned UB address src and returns it in a vector_half register
 * without changing the data format or content.
 *
 * @override_id asc_loadalign__PUf16
 * @archs 3510
 * @param [in] src Source UB address of the half data to load; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline vector_half asc_loadalign(__ubuf__ half* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads bfloat16_t data starting at the 32-byte-aligned UB address src and returns it in a vector_bfloat16_t
 * register without changing the data format or content.
 *
 * @override_id asc_loadalign__PUbf16
 * @archs 3510
 * @param [in] src Source UB address of the bfloat16_t data to load; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline vector_bfloat16_t asc_loadalign(__ubuf__ bfloat16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads int32_t data starting at the 32-byte-aligned UB address src and returns it in a vector_int32_t register
 * without changing the data format or content.
 *
 * @override_id asc_loadalign__PUi32
 * @archs 3510
 * @param [in] src Source UB address of the int32_t data to load; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline vector_int32_t asc_loadalign(__ubuf__ int32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads uint32_t data starting at the 32-byte-aligned UB address src and returns it in a vector_uint32_t
 * register without changing the data format or content.
 *
 * @override_id asc_loadalign__PUu32
 * @archs 3510
 * @param [in] src Source UB address of the uint32_t data to load; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline vector_uint32_t asc_loadalign(__ubuf__ uint32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads float data starting at the 32-byte-aligned UB address src and returns it in a vector_float register
 * without changing the data format or content.
 *
 * @override_id asc_loadalign__PUf32
 * @archs 3510
 * @param [in] src Source UB address of the float data to load; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline vector_float asc_loadalign(__ubuf__ float* src);

// ========== asc_loadalign_brc_datablock ==========
/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector register filled by broadcasting the 32-byte DataBlock of packed int4b_t element pairs read
 * from the 32-byte-aligned src address.
 *
 * @override_id asc_loadalign_brc_datablock__PUi4x2
 * @archs 3510
 * @param [in] src Unified Buffer base address of the int4b_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 */
__simd_callee__ inline vector_int4x2_t asc_loadalign_brc_datablock(__ubuf__ int4b_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector register filled by broadcasting the 32-byte DataBlock of int8_t elements read from the
 * 32-byte-aligned src address.
 *
 * @override_id asc_loadalign_brc_datablock__PUi8
 * @archs 3510
 * @param [in] src Unified Buffer base address of the int8_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 */
__simd_callee__ inline vector_int8_t asc_loadalign_brc_datablock(__ubuf__ int8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector register filled by broadcasting the 32-byte DataBlock of uint8_t elements read from the
 * 32-byte-aligned src address.
 *
 * @override_id asc_loadalign_brc_datablock__PUu8
 * @archs 3510
 * @param [in] src Unified Buffer base address of the uint8_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 */
__simd_callee__ inline vector_uint8_t asc_loadalign_brc_datablock(__ubuf__ uint8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector register filled by broadcasting the 32-byte DataBlock of fp4x2_e2m1_t elements read from the
 * 32-byte-aligned src address.
 *
 * @override_id asc_loadalign_brc_datablock__PUf4e2m1x2
 * @archs 3510
 * @param [in] src Unified Buffer base address of the fp4x2_e2m1_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 */
__simd_callee__ inline vector_fp4x2_e2m1_t asc_loadalign_brc_datablock(__ubuf__ fp4x2_e2m1_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector register filled by broadcasting the 32-byte DataBlock of fp4x2_e1m2_t elements read from the
 * 32-byte-aligned src address.
 *
 * @override_id asc_loadalign_brc_datablock__PUf4e1m2x2
 * @archs 3510
 * @param [in] src Unified Buffer base address of the fp4x2_e1m2_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 */
__simd_callee__ inline vector_fp4x2_e1m2_t asc_loadalign_brc_datablock(__ubuf__ fp4x2_e1m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector register filled by broadcasting the 32-byte DataBlock of hifloat8_t elements read from the
 * 32-byte-aligned src address.
 *
 * @override_id asc_loadalign_brc_datablock__PUhf8
 * @archs 3510
 * @param [in] src Unified Buffer base address of the hifloat8_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 */
__simd_callee__ inline vector_hifloat8_t asc_loadalign_brc_datablock(__ubuf__ hifloat8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector register filled by broadcasting the 32-byte DataBlock of fp8_e8m0_t elements read from the
 * 32-byte-aligned src address.
 *
 * @override_id asc_loadalign_brc_datablock__PUf8e8m0
 * @archs 3510
 * @param [in] src Unified Buffer base address of the fp8_e8m0_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 */
__simd_callee__ inline vector_fp8_e8m0_t asc_loadalign_brc_datablock(__ubuf__ fp8_e8m0_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector register filled by broadcasting the 32-byte DataBlock of fp8_e5m2_t elements read from the
 * 32-byte-aligned src address.
 *
 * @override_id asc_loadalign_brc_datablock__PUf8e5m2
 * @archs 3510
 * @param [in] src Unified Buffer base address of the fp8_e5m2_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 */
__simd_callee__ inline vector_fp8_e5m2_t asc_loadalign_brc_datablock(__ubuf__ fp8_e5m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector register filled by broadcasting the 32-byte DataBlock of fp8_e4m3fn_t elements read from the
 * 32-byte-aligned src address.
 *
 * @override_id asc_loadalign_brc_datablock__PUf8e4m3
 * @archs 3510
 * @param [in] src Unified Buffer base address of the fp8_e4m3fn_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 */
__simd_callee__ inline vector_fp8_e4m3fn_t asc_loadalign_brc_datablock(__ubuf__ fp8_e4m3fn_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector register filled by broadcasting the 32-byte DataBlock of int16_t elements read from the
 * 32-byte-aligned src address.
 *
 * @override_id asc_loadalign_brc_datablock__PUi16
 * @archs 3510
 * @param [in] src Unified Buffer base address of the int16_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 */
__simd_callee__ inline vector_int16_t asc_loadalign_brc_datablock(__ubuf__ int16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector register filled by broadcasting the 32-byte DataBlock of uint16_t elements read from the
 * 32-byte-aligned src address.
 *
 * @override_id asc_loadalign_brc_datablock__PUu16
 * @archs 3510
 * @param [in] src Unified Buffer base address of the uint16_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 */
__simd_callee__ inline vector_uint16_t asc_loadalign_brc_datablock(__ubuf__ uint16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector register filled by broadcasting the 32-byte DataBlock of half elements read from the
 * 32-byte-aligned src address.
 *
 * @override_id asc_loadalign_brc_datablock__PUf16
 * @archs 3510
 * @param [in] src Unified Buffer base address of the half source; the actual read address of the 32-byte DataBlock must
 * be 32-byte aligned.
 */
__simd_callee__ inline vector_half asc_loadalign_brc_datablock(__ubuf__ half* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector register filled by broadcasting the 32-byte DataBlock of bfloat16_t elements read from the
 * 32-byte-aligned src address.
 *
 * @override_id asc_loadalign_brc_datablock__PUbf16
 * @archs 3510
 * @param [in] src Unified Buffer base address of the bfloat16_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 */
__simd_callee__ inline vector_bfloat16_t asc_loadalign_brc_datablock(__ubuf__ bfloat16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector register filled by broadcasting the 32-byte DataBlock of int32_t elements read from the
 * 32-byte-aligned src address.
 *
 * @override_id asc_loadalign_brc_datablock__PUi32
 * @archs 3510
 * @param [in] src Unified Buffer base address of the int32_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 */
__simd_callee__ inline vector_int32_t asc_loadalign_brc_datablock(__ubuf__ int32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector register filled by broadcasting the 32-byte DataBlock of uint32_t elements read from the
 * 32-byte-aligned src address.
 *
 * @override_id asc_loadalign_brc_datablock__PUu32
 * @archs 3510
 * @param [in] src Unified Buffer base address of the uint32_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 */
__simd_callee__ inline vector_uint32_t asc_loadalign_brc_datablock(__ubuf__ uint32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector register filled by broadcasting the 32-byte DataBlock of float elements read from the
 * 32-byte-aligned src address.
 *
 * @override_id asc_loadalign_brc_datablock__PUf32
 * @archs 3510
 * @param [in] src Unified Buffer base address of the float source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 */
__simd_callee__ inline vector_float asc_loadalign_brc_datablock(__ubuf__ float* src);

// ========== asc_loadalign_brc_elem ==========
/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the int4b_t element at an address aligned to the dtype size and broadcasts it across the whole vector
 * data register, returning that register.
 *
 * @override_id asc_loadalign_brc_elem__PUi4x2
 * @archs 3510
 * @param [in] src Source UB address of the int4b_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 */
__simd_callee__ inline vector_int4x2_t asc_loadalign_brc_elem(__ubuf__ int4b_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the int8_t element at an address aligned to the dtype size and broadcasts it across the whole vector
 * data register, returning that register.
 *
 * @override_id asc_loadalign_brc_elem__PUi8
 * @archs 3510
 * @param [in] src Source UB address of the int8_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 */
__simd_callee__ inline vector_int8_t asc_loadalign_brc_elem(__ubuf__ int8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the uint8_t element at an address aligned to the dtype size and broadcasts it across the whole vector
 * data register, returning that register.
 *
 * @override_id asc_loadalign_brc_elem__PUu8
 * @archs 3510
 * @param [in] src Source UB address of the uint8_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 */
__simd_callee__ inline vector_uint8_t asc_loadalign_brc_elem(__ubuf__ uint8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the fp4x2_e2m1_t element at an address aligned to the dtype size and broadcasts it across the whole
 * vector data register, returning that register.
 *
 * @override_id asc_loadalign_brc_elem__PUf4e2m1x2
 * @archs 3510
 * @param [in] src Source UB address of the fp4x2_e2m1_t element to broadcast; the effective read address must be
 * aligned to the dtype size and stay inside UB.
 */
__simd_callee__ inline vector_fp4x2_e2m1_t asc_loadalign_brc_elem(__ubuf__ fp4x2_e2m1_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the fp4x2_e1m2_t element at an address aligned to the dtype size and broadcasts it across the whole
 * vector data register, returning that register.
 *
 * @override_id asc_loadalign_brc_elem__PUf4e1m2x2
 * @archs 3510
 * @param [in] src Source UB address of the fp4x2_e1m2_t element to broadcast; the effective read address must be
 * aligned to the dtype size and stay inside UB.
 */
__simd_callee__ inline vector_fp4x2_e1m2_t asc_loadalign_brc_elem(__ubuf__ fp4x2_e1m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the hifloat8_t element at an address aligned to the dtype size and broadcasts it across the whole vector
 * data register, returning that register.
 *
 * @override_id asc_loadalign_brc_elem__PUhf8
 * @archs 3510
 * @param [in] src Source UB address of the hifloat8_t element to broadcast; the effective read address must be aligned
 * to the dtype size and stay inside UB.
 */
__simd_callee__ inline vector_hifloat8_t asc_loadalign_brc_elem(__ubuf__ hifloat8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the fp8_e8m0_t element at an address aligned to the dtype size and broadcasts it across the whole vector
 * data register, returning that register.
 *
 * @override_id asc_loadalign_brc_elem__PUf8e8m0
 * @archs 3510
 * @param [in] src Source UB address of the fp8_e8m0_t element to broadcast; the effective read address must be aligned
 * to the dtype size and stay inside UB.
 */
__simd_callee__ inline vector_fp8_e8m0_t asc_loadalign_brc_elem(__ubuf__ fp8_e8m0_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the fp8_e5m2_t element at an address aligned to the dtype size and broadcasts it across the whole vector
 * data register, returning that register.
 *
 * @override_id asc_loadalign_brc_elem__PUf8e5m2
 * @archs 3510
 * @param [in] src Source UB address of the fp8_e5m2_t element to broadcast; the effective read address must be aligned
 * to the dtype size and stay inside UB.
 */
__simd_callee__ inline vector_fp8_e5m2_t asc_loadalign_brc_elem(__ubuf__ fp8_e5m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the fp8_e4m3fn_t element at an address aligned to the dtype size and broadcasts it across the whole
 * vector data register, returning that register.
 *
 * @override_id asc_loadalign_brc_elem__PUf8e4m3
 * @archs 3510
 * @param [in] src Source UB address of the fp8_e4m3fn_t element to broadcast; the effective read address must be
 * aligned to the dtype size and stay inside UB.
 */
__simd_callee__ inline vector_fp8_e4m3fn_t asc_loadalign_brc_elem(__ubuf__ fp8_e4m3fn_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the int16_t element at an address aligned to the dtype size and broadcasts it across the whole vector
 * data register, returning that register.
 *
 * @override_id asc_loadalign_brc_elem__PUi16
 * @archs 3510
 * @param [in] src Source UB address of the int16_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 */
__simd_callee__ inline vector_int16_t asc_loadalign_brc_elem(__ubuf__ int16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the uint16_t element at an address aligned to the dtype size and broadcasts it across the whole vector
 * data register, returning that register.
 *
 * @override_id asc_loadalign_brc_elem__PUu16
 * @archs 3510
 * @param [in] src Source UB address of the uint16_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 */
__simd_callee__ inline vector_uint16_t asc_loadalign_brc_elem(__ubuf__ uint16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the half element at an address aligned to the dtype size and broadcasts it across the whole vector data
 * register, returning that register.
 *
 * @override_id asc_loadalign_brc_elem__PUf16
 * @archs 3510
 * @param [in] src Source UB address of the half element to broadcast; the effective read address must be aligned to the
 * dtype size and stay inside UB.
 */
__simd_callee__ inline vector_half asc_loadalign_brc_elem(__ubuf__ half* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the bfloat16_t element at an address aligned to the dtype size and broadcasts it across the whole vector
 * data register, returning that register.
 *
 * @override_id asc_loadalign_brc_elem__PUbf16
 * @archs 3510
 * @param [in] src Source UB address of the bfloat16_t element to broadcast; the effective read address must be aligned
 * to the dtype size and stay inside UB.
 */
__simd_callee__ inline vector_bfloat16_t asc_loadalign_brc_elem(__ubuf__ bfloat16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the int32_t element at an address aligned to the dtype size and broadcasts it across the whole vector
 * data register, returning that register.
 *
 * @override_id asc_loadalign_brc_elem__PUi32
 * @archs 3510
 * @param [in] src Source UB address of the int32_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 */
__simd_callee__ inline vector_int32_t asc_loadalign_brc_elem(__ubuf__ int32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the uint32_t element at an address aligned to the dtype size and broadcasts it across the whole vector
 * data register, returning that register.
 *
 * @override_id asc_loadalign_brc_elem__PUu32
 * @archs 3510
 * @param [in] src Source UB address of the uint32_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 */
__simd_callee__ inline vector_uint32_t asc_loadalign_brc_elem(__ubuf__ uint32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the float element at an address aligned to the dtype size and broadcasts it across the whole vector data
 * register, returning that register.
 *
 * @override_id asc_loadalign_brc_elem__PUf32
 * @archs 3510
 * @param [in] src Source UB address of the float element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 */
__simd_callee__ inline vector_float asc_loadalign_brc_elem(__ubuf__ float* src);

// ========== asc_loadalign_brc_elem2datablock ==========
/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 8 consecutive int16_t elements from a 16-byte aligned UB address, broadcasts each element into one
 * 32-byte DataBlock, and returns the result as a vector register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__PUi16
 * @archs 3510
 * @param [in] src Source UB address of int16_t elements; the effective read address must be 16-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 */
__simd_callee__ inline vector_int16_t asc_loadalign_brc_elem2datablock(__ubuf__ int16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 8 consecutive uint16_t elements from a 16-byte aligned UB address, broadcasts each element into one
 * 32-byte DataBlock, and returns the result as a vector register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__PUu16
 * @archs 3510
 * @param [in] src Source UB address of uint16_t elements; the effective read address must be 16-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 */
__simd_callee__ inline vector_uint16_t asc_loadalign_brc_elem2datablock(__ubuf__ uint16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 8 consecutive half elements from a 16-byte aligned UB address, broadcasts each element into one 32-byte
 * DataBlock, and returns the result as a vector register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__PUf16
 * @archs 3510
 * @param [in] src Source UB address of half elements; the effective read address must be 16-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 */
__simd_callee__ inline vector_half asc_loadalign_brc_elem2datablock(__ubuf__ half* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 8 consecutive bfloat16_t elements from a 16-byte aligned UB address, broadcasts each element into one
 * 32-byte DataBlock, and returns the result as a vector register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__PUbf16
 * @archs 3510
 * @param [in] src Source UB address of bfloat16_t elements; the effective read address must be 16-byte aligned, and the
 * 8 consecutive elements read from it are broadcast unchanged.
 */
__simd_callee__ inline vector_bfloat16_t asc_loadalign_brc_elem2datablock(__ubuf__ bfloat16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 8 consecutive int32_t elements from a 32-byte aligned UB address, broadcasts each element into one
 * 32-byte DataBlock, and returns the result as a vector register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__PUi32
 * @archs 3510
 * @param [in] src Source UB address of int32_t elements; the effective read address must be 32-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 */
__simd_callee__ inline vector_int32_t asc_loadalign_brc_elem2datablock(__ubuf__ int32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 8 consecutive uint32_t elements from a 32-byte aligned UB address, broadcasts each element into one
 * 32-byte DataBlock, and returns the result as a vector register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__PUu32
 * @archs 3510
 * @param [in] src Source UB address of uint32_t elements; the effective read address must be 32-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 */
__simd_callee__ inline vector_uint32_t asc_loadalign_brc_elem2datablock(__ubuf__ uint32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 8 consecutive float elements from a 32-byte aligned UB address, broadcasts each element into one 32-byte
 * DataBlock, and returns the result as a vector register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__PUf32
 * @archs 3510
 * @param [in] src Source UB address of float elements; the effective read address must be 32-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 */
__simd_callee__ inline vector_float asc_loadalign_brc_elem2datablock(__ubuf__ float* src);

// ========== asc_loadalign_downsample ==========
/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of packed 4-bit int4b_t elements from a 32-byte-aligned UB address, keeps only
 * the even-indexed elements, and returns the vector_int4x2_t holding the downsampled result of VL bytes (256 bytes).
 *
 * @override_id asc_loadalign_downsample__PUi4x2
 * @archs 3510
 * @param [in] src UB source address of the packed 4-bit int4b_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline vector_int4x2_t asc_loadalign_downsample(__ubuf__ int4b_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit int8_t elements from a 32-byte-aligned UB address, keeps only the
 * even-indexed elements, and returns the vector_int8_t holding the downsampled result of VL bytes (256 bytes).
 *
 * @override_id asc_loadalign_downsample__PUi8
 * @archs 3510
 * @param [in] src UB source address of the 8-bit int8_t elements to read; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline vector_int8_t asc_loadalign_downsample(__ubuf__ int8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit uint8_t elements from a 32-byte-aligned UB address, keeps only the
 * even-indexed elements, and returns the vector_uint8_t holding the downsampled result of VL bytes (256 bytes).
 *
 * @override_id asc_loadalign_downsample__PUu8
 * @archs 3510
 * @param [in] src UB source address of the 8-bit uint8_t elements to read; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline vector_uint8_t asc_loadalign_downsample(__ubuf__ uint8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of packed 4-bit fp4x2_e2m1_t elements from a 32-byte-aligned UB address, keeps
 * only the even-indexed elements, and returns the vector_fp4x2_e2m1_t holding the downsampled result of VL bytes (256
 * bytes).
 *
 * @override_id asc_loadalign_downsample__PUf4e2m1x2
 * @archs 3510
 * @param [in] src UB source address of the packed 4-bit fp4x2_e2m1_t elements to read; the effective read address must
 * be 32-byte aligned.
 */
__simd_callee__ inline vector_fp4x2_e2m1_t asc_loadalign_downsample(__ubuf__ fp4x2_e2m1_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of packed 4-bit fp4x2_e1m2_t elements from a 32-byte-aligned UB address, keeps
 * only the even-indexed elements, and returns the vector_fp4x2_e1m2_t holding the downsampled result of VL bytes (256
 * bytes).
 *
 * @override_id asc_loadalign_downsample__PUf4e1m2x2
 * @archs 3510
 * @param [in] src UB source address of the packed 4-bit fp4x2_e1m2_t elements to read; the effective read address must
 * be 32-byte aligned.
 */
__simd_callee__ inline vector_fp4x2_e1m2_t asc_loadalign_downsample(__ubuf__ fp4x2_e1m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit hifloat8_t elements from a 32-byte-aligned UB address, keeps only the
 * even-indexed elements, and returns the vector_hifloat8_t holding the downsampled result of VL bytes (256 bytes).
 *
 * @override_id asc_loadalign_downsample__PUhf8
 * @archs 3510
 * @param [in] src UB source address of the 8-bit hifloat8_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline vector_hifloat8_t asc_loadalign_downsample(__ubuf__ hifloat8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit fp8_e8m0_t elements from a 32-byte-aligned UB address, keeps only the
 * even-indexed elements, and returns the vector_fp8_e8m0_t holding the downsampled result of VL bytes (256 bytes).
 *
 * @override_id asc_loadalign_downsample__PUf8e8m0
 * @archs 3510
 * @param [in] src UB source address of the 8-bit fp8_e8m0_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline vector_fp8_e8m0_t asc_loadalign_downsample(__ubuf__ fp8_e8m0_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit fp8_e5m2_t elements from a 32-byte-aligned UB address, keeps only the
 * even-indexed elements, and returns the vector_fp8_e5m2_t holding the downsampled result of VL bytes (256 bytes).
 *
 * @override_id asc_loadalign_downsample__PUf8e5m2
 * @archs 3510
 * @param [in] src UB source address of the 8-bit fp8_e5m2_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline vector_fp8_e5m2_t asc_loadalign_downsample(__ubuf__ fp8_e5m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit fp8_e4m3fn_t elements from a 32-byte-aligned UB address, keeps only
 * the even-indexed elements, and returns the vector_fp8_e4m3fn_t holding the downsampled result of VL bytes (256
 * bytes).
 *
 * @override_id asc_loadalign_downsample__PUf8e4m3
 * @archs 3510
 * @param [in] src UB source address of the 8-bit fp8_e4m3fn_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline vector_fp8_e4m3fn_t asc_loadalign_downsample(__ubuf__ fp8_e4m3fn_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 16-bit int16_t elements from a 32-byte-aligned UB address, keeps only the
 * even-indexed elements, and returns the vector_int16_t holding the downsampled result of VL bytes (256 bytes).
 *
 * @override_id asc_loadalign_downsample__PUi16
 * @archs 3510
 * @param [in] src UB source address of the 16-bit int16_t elements to read; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline vector_int16_t asc_loadalign_downsample(__ubuf__ int16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 16-bit uint16_t elements from a 32-byte-aligned UB address, keeps only the
 * even-indexed elements, and returns the vector_uint16_t holding the downsampled result of VL bytes (256 bytes).
 *
 * @override_id asc_loadalign_downsample__PUu16
 * @archs 3510
 * @param [in] src UB source address of the 16-bit uint16_t elements to read; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline vector_uint16_t asc_loadalign_downsample(__ubuf__ uint16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 16-bit half elements from a 32-byte-aligned UB address, keeps only the
 * even-indexed elements, and returns the vector_half holding the downsampled result of VL bytes (256 bytes).
 *
 * @override_id asc_loadalign_downsample__PUf16
 * @archs 3510
 * @param [in] src UB source address of the 16-bit half elements to read; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline vector_half asc_loadalign_downsample(__ubuf__ half* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 16-bit bfloat16_t elements from a 32-byte-aligned UB address, keeps only the
 * even-indexed elements, and returns the vector_bfloat16_t holding the downsampled result of VL bytes (256 bytes).
 *
 * @override_id asc_loadalign_downsample__PUbf16
 * @archs 3510
 * @param [in] src UB source address of the 16-bit bfloat16_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline vector_bfloat16_t asc_loadalign_downsample(__ubuf__ bfloat16_t* src);

// ========== asc_loadalign_unpack ==========
/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous int4b_t elements from a 32-byte aligned address with one zero element inserted after
 * each and returns the resulting VL-length vector data register.
 *
 * @override_id asc_loadalign_unpack__PUi4x2
 * @archs 3510
 * @param [in] src Source UB address of the int4b_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 */
__simd_callee__ inline vector_int4x2_t asc_loadalign_unpack(__ubuf__ int4b_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous int8_t elements from a 32-byte aligned address with one zero element inserted after each
 * and returns the resulting VL-length vector data register.
 *
 * @override_id asc_loadalign_unpack__PUi8
 * @archs 3510
 * @param [in] src Source UB address of the int8_t run to unpack; the effective read address must be 32-byte aligned and
 * stay inside UB.
 */
__simd_callee__ inline vector_int8_t asc_loadalign_unpack(__ubuf__ int8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous uint8_t elements from a 32-byte aligned address with one zero element inserted after
 * each and returns the resulting VL-length vector data register.
 *
 * @override_id asc_loadalign_unpack__PUu8
 * @archs 3510
 * @param [in] src Source UB address of the uint8_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 */
__simd_callee__ inline vector_uint8_t asc_loadalign_unpack(__ubuf__ uint8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous fp4x2_e2m1_t elements from a 32-byte aligned address with one zero element inserted
 * after each and returns the resulting VL-length vector data register.
 *
 * @override_id asc_loadalign_unpack__PUf4e2m1x2
 * @archs 3510
 * @param [in] src Source UB address of the fp4x2_e2m1_t run to unpack; the effective read address must be 32-byte
 * aligned and stay inside UB.
 */
__simd_callee__ inline vector_fp4x2_e2m1_t asc_loadalign_unpack(__ubuf__ fp4x2_e2m1_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous fp4x2_e1m2_t elements from a 32-byte aligned address with one zero element inserted
 * after each and returns the resulting VL-length vector data register.
 *
 * @override_id asc_loadalign_unpack__PUf4e1m2x2
 * @archs 3510
 * @param [in] src Source UB address of the fp4x2_e1m2_t run to unpack; the effective read address must be 32-byte
 * aligned and stay inside UB.
 */
__simd_callee__ inline vector_fp4x2_e1m2_t asc_loadalign_unpack(__ubuf__ fp4x2_e1m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous hifloat8_t elements from a 32-byte aligned address with one zero element inserted after
 * each and returns the resulting VL-length vector data register.
 *
 * @override_id asc_loadalign_unpack__PUhf8
 * @archs 3510
 * @param [in] src Source UB address of the hifloat8_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 */
__simd_callee__ inline vector_hifloat8_t asc_loadalign_unpack(__ubuf__ hifloat8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous fp8_e8m0_t elements from a 32-byte aligned address with one zero element inserted after
 * each and returns the resulting VL-length vector data register.
 *
 * @override_id asc_loadalign_unpack__PUf8e8m0
 * @archs 3510
 * @param [in] src Source UB address of the fp8_e8m0_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 */
__simd_callee__ inline vector_fp8_e8m0_t asc_loadalign_unpack(__ubuf__ fp8_e8m0_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous fp8_e5m2_t elements from a 32-byte aligned address with one zero element inserted after
 * each and returns the resulting VL-length vector data register.
 *
 * @override_id asc_loadalign_unpack__PUf8e5m2
 * @archs 3510
 * @param [in] src Source UB address of the fp8_e5m2_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 */
__simd_callee__ inline vector_fp8_e5m2_t asc_loadalign_unpack(__ubuf__ fp8_e5m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous fp8_e4m3fn_t elements from a 32-byte aligned address with one zero element inserted
 * after each and returns the resulting VL-length vector data register.
 *
 * @override_id asc_loadalign_unpack__PUf8e4m3
 * @archs 3510
 * @param [in] src Source UB address of the fp8_e4m3fn_t run to unpack; the effective read address must be 32-byte
 * aligned and stay inside UB.
 */
__simd_callee__ inline vector_fp8_e4m3fn_t asc_loadalign_unpack(__ubuf__ fp8_e4m3fn_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous int16_t elements from a 32-byte aligned address with one zero element inserted after
 * each and returns the resulting VL-length vector data register.
 *
 * @override_id asc_loadalign_unpack__PUi16
 * @archs 3510
 * @param [in] src Source UB address of the int16_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 */
__simd_callee__ inline vector_int16_t asc_loadalign_unpack(__ubuf__ int16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous uint16_t elements from a 32-byte aligned address with one zero element inserted after
 * each and returns the resulting VL-length vector data register.
 *
 * @override_id asc_loadalign_unpack__PUu16
 * @archs 3510
 * @param [in] src Source UB address of the uint16_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 */
__simd_callee__ inline vector_uint16_t asc_loadalign_unpack(__ubuf__ uint16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous half elements from a 32-byte aligned address with one zero element inserted after each
 * and returns the resulting VL-length vector data register.
 *
 * @override_id asc_loadalign_unpack__PUf16
 * @archs 3510
 * @param [in] src Source UB address of the half run to unpack; the effective read address must be 32-byte aligned and
 * stay inside UB.
 */
__simd_callee__ inline vector_half asc_loadalign_unpack(__ubuf__ half* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous bfloat16_t elements from a 32-byte aligned address with one zero element inserted after
 * each and returns the resulting VL-length vector data register.
 *
 * @override_id asc_loadalign_unpack__PUbf16
 * @archs 3510
 * @param [in] src Source UB address of the bfloat16_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 */
__simd_callee__ inline vector_bfloat16_t asc_loadalign_unpack(__ubuf__ bfloat16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous int32_t elements from a 32-byte aligned address with one zero element inserted after
 * each and returns the resulting VL-length vector data register.
 *
 * @override_id asc_loadalign_unpack__PUi32
 * @archs 3510
 * @param [in] src Source UB address of the int32_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 */
__simd_callee__ inline vector_int32_t asc_loadalign_unpack(__ubuf__ int32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous uint32_t elements from a 32-byte aligned address with one zero element inserted after
 * each and returns the resulting VL-length vector data register.
 *
 * @override_id asc_loadalign_unpack__PUu32
 * @archs 3510
 * @param [in] src Source UB address of the uint32_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 */
__simd_callee__ inline vector_uint32_t asc_loadalign_unpack(__ubuf__ uint32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous float elements from a 32-byte aligned address with one zero element inserted after each
 * and returns the resulting VL-length vector data register.
 *
 * @override_id asc_loadalign_unpack__PUf32
 * @archs 3510
 * @param [in] src Source UB address of the float run to unpack; the effective read address must be 32-byte aligned and
 * stay inside UB.
 */
__simd_callee__ inline vector_float asc_loadalign_unpack(__ubuf__ float* src);

// ========== asc_loadalign_unpack4 ==========
/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous int4b_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and returns the VL-length expanded register.
 *
 * @override_id asc_loadalign_unpack4__PUi4x2
 * @archs 3510
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 */
__simd_callee__ inline vector_int4x2_t asc_loadalign_unpack4(__ubuf__ int4b_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous int8_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and returns the VL-length expanded register.
 *
 * @override_id asc_loadalign_unpack4__PUi8
 * @archs 3510
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 */
__simd_callee__ inline vector_int8_t asc_loadalign_unpack4(__ubuf__ int8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous uint8_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and returns the VL-length expanded register.
 *
 * @override_id asc_loadalign_unpack4__PUu8
 * @archs 3510
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 */
__simd_callee__ inline vector_uint8_t asc_loadalign_unpack4(__ubuf__ uint8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous fp4x2_e2m1_t elements from the 32-byte-aligned UB address src, inserts three zero
 * elements after each source element, and returns the VL-length expanded register.
 *
 * @override_id asc_loadalign_unpack4__PUf4e2m1x2
 * @archs 3510
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 */
__simd_callee__ inline vector_fp4x2_e2m1_t asc_loadalign_unpack4(__ubuf__ fp4x2_e2m1_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous fp4x2_e1m2_t elements from the 32-byte-aligned UB address src, inserts three zero
 * elements after each source element, and returns the VL-length expanded register.
 *
 * @override_id asc_loadalign_unpack4__PUf4e1m2x2
 * @archs 3510
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 */
__simd_callee__ inline vector_fp4x2_e1m2_t asc_loadalign_unpack4(__ubuf__ fp4x2_e1m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous hifloat8_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and returns the VL-length expanded register.
 *
 * @override_id asc_loadalign_unpack4__PUhf8
 * @archs 3510
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 */
__simd_callee__ inline vector_hifloat8_t asc_loadalign_unpack4(__ubuf__ hifloat8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous fp8_e8m0_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and returns the VL-length expanded register.
 *
 * @override_id asc_loadalign_unpack4__PUf8e8m0
 * @archs 3510
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 */
__simd_callee__ inline vector_fp8_e8m0_t asc_loadalign_unpack4(__ubuf__ fp8_e8m0_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous fp8_e5m2_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and returns the VL-length expanded register.
 *
 * @override_id asc_loadalign_unpack4__PUf8e5m2
 * @archs 3510
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 */
__simd_callee__ inline vector_fp8_e5m2_t asc_loadalign_unpack4(__ubuf__ fp8_e5m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous fp8_e4m3fn_t elements from the 32-byte-aligned UB address src, inserts three zero
 * elements after each source element, and returns the VL-length expanded register.
 *
 * @override_id asc_loadalign_unpack4__PUf8e4m3
 * @archs 3510
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 */
__simd_callee__ inline vector_fp8_e4m3fn_t asc_loadalign_unpack4(__ubuf__ fp8_e4m3fn_t* src);

// ========== asc_loadalign_upsample ==========
/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of packed 4-bit int4b_t elements from a 32-byte-aligned UB address, duplicates
 * every source element, and returns the vector_int4x2_t holding the upsampled result of VL bytes (256 bytes).
 *
 * @override_id asc_loadalign_upsample__PUi4x2
 * @archs 3510
 * @param [in] src UB source address of the packed 4-bit int4b_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline vector_int4x2_t asc_loadalign_upsample(__ubuf__ int4b_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit int8_t elements from a 32-byte-aligned UB address, duplicates every
 * source element, and returns the vector_int8_t holding the upsampled result of VL bytes (256 bytes).
 *
 * @override_id asc_loadalign_upsample__PUi8
 * @archs 3510
 * @param [in] src UB source address of the 8-bit int8_t elements to read; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline vector_int8_t asc_loadalign_upsample(__ubuf__ int8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit uint8_t elements from a 32-byte-aligned UB address, duplicates every
 * source element, and returns the vector_uint8_t holding the upsampled result of VL bytes (256 bytes).
 *
 * @override_id asc_loadalign_upsample__PUu8
 * @archs 3510
 * @param [in] src UB source address of the 8-bit uint8_t elements to read; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline vector_uint8_t asc_loadalign_upsample(__ubuf__ uint8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of packed 4-bit fp4x2_e2m1_t elements from a 32-byte-aligned UB address,
 * duplicates every source element, and returns the vector_fp4x2_e2m1_t holding the upsampled result of VL bytes (256
 * bytes).
 *
 * @override_id asc_loadalign_upsample__PUf4e2m1x2
 * @archs 3510
 * @param [in] src UB source address of the packed 4-bit fp4x2_e2m1_t elements to read; the effective read address must
 * be 32-byte aligned.
 */
__simd_callee__ inline vector_fp4x2_e2m1_t asc_loadalign_upsample(__ubuf__ fp4x2_e2m1_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of packed 4-bit fp4x2_e1m2_t elements from a 32-byte-aligned UB address,
 * duplicates every source element, and returns the vector_fp4x2_e1m2_t holding the upsampled result of VL bytes (256
 * bytes).
 *
 * @override_id asc_loadalign_upsample__PUf4e1m2x2
 * @archs 3510
 * @param [in] src UB source address of the packed 4-bit fp4x2_e1m2_t elements to read; the effective read address must
 * be 32-byte aligned.
 */
__simd_callee__ inline vector_fp4x2_e1m2_t asc_loadalign_upsample(__ubuf__ fp4x2_e1m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit hifloat8_t elements from a 32-byte-aligned UB address, duplicates every
 * source element, and returns the vector_hifloat8_t holding the upsampled result of VL bytes (256 bytes).
 *
 * @override_id asc_loadalign_upsample__PUhf8
 * @archs 3510
 * @param [in] src UB source address of the 8-bit hifloat8_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline vector_hifloat8_t asc_loadalign_upsample(__ubuf__ hifloat8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit fp8_e8m0_t elements from a 32-byte-aligned UB address, duplicates every
 * source element, and returns the vector_fp8_e8m0_t holding the upsampled result of VL bytes (256 bytes).
 *
 * @override_id asc_loadalign_upsample__PUf8e8m0
 * @archs 3510
 * @param [in] src UB source address of the 8-bit fp8_e8m0_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline vector_fp8_e8m0_t asc_loadalign_upsample(__ubuf__ fp8_e8m0_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit fp8_e5m2_t elements from a 32-byte-aligned UB address, duplicates every
 * source element, and returns the vector_fp8_e5m2_t holding the upsampled result of VL bytes (256 bytes).
 *
 * @override_id asc_loadalign_upsample__PUf8e5m2
 * @archs 3510
 * @param [in] src UB source address of the 8-bit fp8_e5m2_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline vector_fp8_e5m2_t asc_loadalign_upsample(__ubuf__ fp8_e5m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit fp8_e4m3fn_t elements from a 32-byte-aligned UB address, duplicates
 * every source element, and returns the vector_fp8_e4m3fn_t holding the upsampled result of VL bytes (256 bytes).
 *
 * @override_id asc_loadalign_upsample__PUf8e4m3
 * @archs 3510
 * @param [in] src UB source address of the 8-bit fp8_e4m3fn_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline vector_fp8_e4m3fn_t asc_loadalign_upsample(__ubuf__ fp8_e4m3fn_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 16-bit int16_t elements from a 32-byte-aligned UB address, duplicates every
 * source element, and returns the vector_int16_t holding the upsampled result of VL bytes (256 bytes).
 *
 * @override_id asc_loadalign_upsample__PUi16
 * @archs 3510
 * @param [in] src UB source address of the 16-bit int16_t elements to read; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline vector_int16_t asc_loadalign_upsample(__ubuf__ int16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 16-bit uint16_t elements from a 32-byte-aligned UB address, duplicates every
 * source element, and returns the vector_uint16_t holding the upsampled result of VL bytes (256 bytes).
 *
 * @override_id asc_loadalign_upsample__PUu16
 * @archs 3510
 * @param [in] src UB source address of the 16-bit uint16_t elements to read; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline vector_uint16_t asc_loadalign_upsample(__ubuf__ uint16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 16-bit half elements from a 32-byte-aligned UB address, duplicates every
 * source element, and returns the vector_half holding the upsampled result of VL bytes (256 bytes).
 *
 * @override_id asc_loadalign_upsample__PUf16
 * @archs 3510
 * @param [in] src UB source address of the 16-bit half elements to read; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline vector_half asc_loadalign_upsample(__ubuf__ half* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 16-bit bfloat16_t elements from a 32-byte-aligned UB address, duplicates every
 * source element, and returns the vector_bfloat16_t holding the upsampled result of VL bytes (256 bytes).
 *
 * @override_id asc_loadalign_upsample__PUbf16
 * @archs 3510
 * @param [in] src UB source address of the 16-bit bfloat16_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline vector_bfloat16_t asc_loadalign_upsample(__ubuf__ bfloat16_t* src);

// ========== asc_loadalign_datablock_strided ==========
/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector_int4x2_t register holding the eight 32-byte DataBlocks gathered from the int4b_t UB source
 * src with the configured block_stride and repeat_stride, where mask decides which DataBlocks are loaded.
 *
 * @override_id asc_loadalign_datablock_strided__PUi4x2_u16_u16_vb
 * @archs 3510
 * @param [in] src Base Unified Buffer address of the int4b_t source data; the effective read addresses must be 32-byte
 * aligned and the valid DataBlocks must stay inside the UB address space.
 * @param [in] block_stride Step between the start addresses of adjacent DataBlocks, counted in units of 32 bytes, so a
 * value of 2 means a 64-byte step between consecutive DataBlocks.
 * @param [in] repeat_stride Offset of the first DataBlock read relative to src, counted in units of 32 bytes, so the
 * first effective address is src plus repeat_stride * 32 bytes.
 * @param [in] mask Mask register evaluated at DataBlock granularity for int4b_t elements: if at least one element of a
 * DataBlock is enabled the whole 32-byte block is loaded, and if every element of the block is disabled the block is
 * skipped, the matching destination elements are set to 0 and an out-of-range address does not raise an error.
 */
__simd_callee__ inline vector_int4x2_t asc_loadalign_datablock_strided(
    __ubuf__ int4b_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector_int8_t register holding the eight 32-byte DataBlocks gathered from the int8_t UB source src
 * with the configured block_stride and repeat_stride, where mask decides which DataBlocks are loaded.
 *
 * @override_id asc_loadalign_datablock_strided__PUi8_u16_u16_vb
 * @archs 3510
 * @param [in] src Base Unified Buffer address of the int8_t source data; the effective read addresses must be 32-byte
 * aligned and the valid DataBlocks must stay inside the UB address space.
 * @param [in] block_stride Step between the start addresses of adjacent DataBlocks, counted in units of 32 bytes, so a
 * value of 2 means a 64-byte step between consecutive DataBlocks.
 * @param [in] repeat_stride Offset of the first DataBlock read relative to src, counted in units of 32 bytes, so the
 * first effective address is src plus repeat_stride * 32 bytes.
 * @param [in] mask Mask register evaluated at DataBlock granularity for int8_t elements: if at least one element of a
 * DataBlock is enabled the whole 32-byte block is loaded, and if every element of the block is disabled the block is
 * skipped, the matching destination elements are set to 0 and an out-of-range address does not raise an error.
 */
__simd_callee__ inline vector_int8_t asc_loadalign_datablock_strided(
    __ubuf__ int8_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector_uint8_t register holding the eight 32-byte DataBlocks gathered from the uint8_t UB source src
 * with the configured block_stride and repeat_stride, where mask decides which DataBlocks are loaded.
 *
 * @override_id asc_loadalign_datablock_strided__PUu8_u16_u16_vb
 * @archs 3510
 * @param [in] src Base Unified Buffer address of the uint8_t source data; the effective read addresses must be 32-byte
 * aligned and the valid DataBlocks must stay inside the UB address space.
 * @param [in] block_stride Step between the start addresses of adjacent DataBlocks, counted in units of 32 bytes, so a
 * value of 2 means a 64-byte step between consecutive DataBlocks.
 * @param [in] repeat_stride Offset of the first DataBlock read relative to src, counted in units of 32 bytes, so the
 * first effective address is src plus repeat_stride * 32 bytes.
 * @param [in] mask Mask register evaluated at DataBlock granularity for uint8_t elements: if at least one element of a
 * DataBlock is enabled the whole 32-byte block is loaded, and if every element of the block is disabled the block is
 * skipped, the matching destination elements are set to 0 and an out-of-range address does not raise an error.
 */
__simd_callee__ inline vector_uint8_t asc_loadalign_datablock_strided(
    __ubuf__ uint8_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector_fp4x2_e2m1_t register holding the eight 32-byte DataBlocks gathered from the fp4x2_e2m1_t UB
 * source src with the configured block_stride and repeat_stride, where mask decides which DataBlocks are loaded.
 *
 * @override_id asc_loadalign_datablock_strided__PUf4e2m1x2_u16_u16_vb
 * @archs 3510
 * @param [in] src Base Unified Buffer address of the fp4x2_e2m1_t source data; the effective read addresses must be
 * 32-byte aligned and the valid DataBlocks must stay inside the UB address space.
 * @param [in] block_stride Step between the start addresses of adjacent DataBlocks, counted in units of 32 bytes, so a
 * value of 2 means a 64-byte step between consecutive DataBlocks.
 * @param [in] repeat_stride Offset of the first DataBlock read relative to src, counted in units of 32 bytes, so the
 * first effective address is src plus repeat_stride * 32 bytes.
 * @param [in] mask Mask register evaluated at DataBlock granularity for fp4x2_e2m1_t elements: if at least one element
 * of a DataBlock is enabled the whole 32-byte block is loaded, and if every element of the block is disabled the block
 * is skipped, the matching destination elements are set to 0 and an out-of-range address does not raise an error.
 */
__simd_callee__ inline vector_fp4x2_e2m1_t asc_loadalign_datablock_strided(
    __ubuf__ fp4x2_e2m1_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector_fp4x2_e1m2_t register holding the eight 32-byte DataBlocks gathered from the fp4x2_e1m2_t UB
 * source src with the configured block_stride and repeat_stride, where mask decides which DataBlocks are loaded.
 *
 * @override_id asc_loadalign_datablock_strided__PUf4e1m2x2_u16_u16_vb
 * @archs 3510
 * @param [in] src Base Unified Buffer address of the fp4x2_e1m2_t source data; the effective read addresses must be
 * 32-byte aligned and the valid DataBlocks must stay inside the UB address space.
 * @param [in] block_stride Step between the start addresses of adjacent DataBlocks, counted in units of 32 bytes, so a
 * value of 2 means a 64-byte step between consecutive DataBlocks.
 * @param [in] repeat_stride Offset of the first DataBlock read relative to src, counted in units of 32 bytes, so the
 * first effective address is src plus repeat_stride * 32 bytes.
 * @param [in] mask Mask register evaluated at DataBlock granularity for fp4x2_e1m2_t elements: if at least one element
 * of a DataBlock is enabled the whole 32-byte block is loaded, and if every element of the block is disabled the block
 * is skipped, the matching destination elements are set to 0 and an out-of-range address does not raise an error.
 */
__simd_callee__ inline vector_fp4x2_e1m2_t asc_loadalign_datablock_strided(
    __ubuf__ fp4x2_e1m2_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector_hifloat8_t register holding the eight 32-byte DataBlocks gathered from the hifloat8_t UB
 * source src with the configured block_stride and repeat_stride, where mask decides which DataBlocks are loaded.
 *
 * @override_id asc_loadalign_datablock_strided__PUhf8_u16_u16_vb
 * @archs 3510
 * @param [in] src Base Unified Buffer address of the hifloat8_t source data; the effective read addresses must be
 * 32-byte aligned and the valid DataBlocks must stay inside the UB address space.
 * @param [in] block_stride Step between the start addresses of adjacent DataBlocks, counted in units of 32 bytes, so a
 * value of 2 means a 64-byte step between consecutive DataBlocks.
 * @param [in] repeat_stride Offset of the first DataBlock read relative to src, counted in units of 32 bytes, so the
 * first effective address is src plus repeat_stride * 32 bytes.
 * @param [in] mask Mask register evaluated at DataBlock granularity for hifloat8_t elements: if at least one element of
 * a DataBlock is enabled the whole 32-byte block is loaded, and if every element of the block is disabled the block is
 * skipped, the matching destination elements are set to 0 and an out-of-range address does not raise an error.
 */
__simd_callee__ inline vector_hifloat8_t asc_loadalign_datablock_strided(
    __ubuf__ hifloat8_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector_fp8_e8m0_t register holding the eight 32-byte DataBlocks gathered from the fp8_e8m0_t UB
 * source src with the configured block_stride and repeat_stride, where mask decides which DataBlocks are loaded.
 *
 * @override_id asc_loadalign_datablock_strided__PUf8e8m0_u16_u16_vb
 * @archs 3510
 * @param [in] src Base Unified Buffer address of the fp8_e8m0_t source data; the effective read addresses must be
 * 32-byte aligned and the valid DataBlocks must stay inside the UB address space.
 * @param [in] block_stride Step between the start addresses of adjacent DataBlocks, counted in units of 32 bytes, so a
 * value of 2 means a 64-byte step between consecutive DataBlocks.
 * @param [in] repeat_stride Offset of the first DataBlock read relative to src, counted in units of 32 bytes, so the
 * first effective address is src plus repeat_stride * 32 bytes.
 * @param [in] mask Mask register evaluated at DataBlock granularity for fp8_e8m0_t elements: if at least one element of
 * a DataBlock is enabled the whole 32-byte block is loaded, and if every element of the block is disabled the block is
 * skipped, the matching destination elements are set to 0 and an out-of-range address does not raise an error.
 */
__simd_callee__ inline vector_fp8_e8m0_t asc_loadalign_datablock_strided(
    __ubuf__ fp8_e8m0_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector_fp8_e5m2_t register holding the eight 32-byte DataBlocks gathered from the fp8_e5m2_t UB
 * source src with the configured block_stride and repeat_stride, where mask decides which DataBlocks are loaded.
 *
 * @override_id asc_loadalign_datablock_strided__PUf8e5m2_u16_u16_vb
 * @archs 3510
 * @param [in] src Base Unified Buffer address of the fp8_e5m2_t source data; the effective read addresses must be
 * 32-byte aligned and the valid DataBlocks must stay inside the UB address space.
 * @param [in] block_stride Step between the start addresses of adjacent DataBlocks, counted in units of 32 bytes, so a
 * value of 2 means a 64-byte step between consecutive DataBlocks.
 * @param [in] repeat_stride Offset of the first DataBlock read relative to src, counted in units of 32 bytes, so the
 * first effective address is src plus repeat_stride * 32 bytes.
 * @param [in] mask Mask register evaluated at DataBlock granularity for fp8_e5m2_t elements: if at least one element of
 * a DataBlock is enabled the whole 32-byte block is loaded, and if every element of the block is disabled the block is
 * skipped, the matching destination elements are set to 0 and an out-of-range address does not raise an error.
 */
__simd_callee__ inline vector_fp8_e5m2_t asc_loadalign_datablock_strided(
    __ubuf__ fp8_e5m2_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector_fp8_e4m3fn_t register holding the eight 32-byte DataBlocks gathered from the fp8_e4m3fn_t UB
 * source src with the configured block_stride and repeat_stride, where mask decides which DataBlocks are loaded.
 *
 * @override_id asc_loadalign_datablock_strided__PUf8e4m3_u16_u16_vb
 * @archs 3510
 * @param [in] src Base Unified Buffer address of the fp8_e4m3fn_t source data; the effective read addresses must be
 * 32-byte aligned and the valid DataBlocks must stay inside the UB address space.
 * @param [in] block_stride Step between the start addresses of adjacent DataBlocks, counted in units of 32 bytes, so a
 * value of 2 means a 64-byte step between consecutive DataBlocks.
 * @param [in] repeat_stride Offset of the first DataBlock read relative to src, counted in units of 32 bytes, so the
 * first effective address is src plus repeat_stride * 32 bytes.
 * @param [in] mask Mask register evaluated at DataBlock granularity for fp8_e4m3fn_t elements: if at least one element
 * of a DataBlock is enabled the whole 32-byte block is loaded, and if every element of the block is disabled the block
 * is skipped, the matching destination elements are set to 0 and an out-of-range address does not raise an error.
 */
__simd_callee__ inline vector_fp8_e4m3fn_t asc_loadalign_datablock_strided(
    __ubuf__ fp8_e4m3fn_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector_int16_t register holding the eight 32-byte DataBlocks gathered from the int16_t UB source src
 * with the configured block_stride and repeat_stride, where mask decides which DataBlocks are loaded.
 *
 * @override_id asc_loadalign_datablock_strided__PUi16_u16_u16_vb
 * @archs 3510
 * @param [in] src Base Unified Buffer address of the int16_t source data; the effective read addresses must be 32-byte
 * aligned and the valid DataBlocks must stay inside the UB address space.
 * @param [in] block_stride Step between the start addresses of adjacent DataBlocks, counted in units of 32 bytes, so a
 * value of 2 means a 64-byte step between consecutive DataBlocks.
 * @param [in] repeat_stride Offset of the first DataBlock read relative to src, counted in units of 32 bytes, so the
 * first effective address is src plus repeat_stride * 32 bytes.
 * @param [in] mask Mask register evaluated at DataBlock granularity for int16_t elements: if at least one element of a
 * DataBlock is enabled the whole 32-byte block is loaded, and if every element of the block is disabled the block is
 * skipped, the matching destination elements are set to 0 and an out-of-range address does not raise an error.
 */
__simd_callee__ inline vector_int16_t asc_loadalign_datablock_strided(
    __ubuf__ int16_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector_uint16_t register holding the eight 32-byte DataBlocks gathered from the uint16_t UB source
 * src with the configured block_stride and repeat_stride, where mask decides which DataBlocks are loaded.
 *
 * @override_id asc_loadalign_datablock_strided__PUu16_u16_u16_vb
 * @archs 3510
 * @param [in] src Base Unified Buffer address of the uint16_t source data; the effective read addresses must be 32-byte
 * aligned and the valid DataBlocks must stay inside the UB address space.
 * @param [in] block_stride Step between the start addresses of adjacent DataBlocks, counted in units of 32 bytes, so a
 * value of 2 means a 64-byte step between consecutive DataBlocks.
 * @param [in] repeat_stride Offset of the first DataBlock read relative to src, counted in units of 32 bytes, so the
 * first effective address is src plus repeat_stride * 32 bytes.
 * @param [in] mask Mask register evaluated at DataBlock granularity for uint16_t elements: if at least one element of a
 * DataBlock is enabled the whole 32-byte block is loaded, and if every element of the block is disabled the block is
 * skipped, the matching destination elements are set to 0 and an out-of-range address does not raise an error.
 */
__simd_callee__ inline vector_uint16_t asc_loadalign_datablock_strided(
    __ubuf__ uint16_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector_half register holding the eight 32-byte DataBlocks gathered from the half UB source src with
 * the configured block_stride and repeat_stride, where mask decides which DataBlocks are loaded.
 *
 * @override_id asc_loadalign_datablock_strided__PUf16_u16_u16_vb
 * @archs 3510
 * @param [in] src Base Unified Buffer address of the half source data; the effective read addresses must be 32-byte
 * aligned and the valid DataBlocks must stay inside the UB address space.
 * @param [in] block_stride Step between the start addresses of adjacent DataBlocks, counted in units of 32 bytes, so a
 * value of 2 means a 64-byte step between consecutive DataBlocks.
 * @param [in] repeat_stride Offset of the first DataBlock read relative to src, counted in units of 32 bytes, so the
 * first effective address is src plus repeat_stride * 32 bytes.
 * @param [in] mask Mask register evaluated at DataBlock granularity for half elements: if at least one element of a
 * DataBlock is enabled the whole 32-byte block is loaded, and if every element of the block is disabled the block is
 * skipped, the matching destination elements are set to 0 and an out-of-range address does not raise an error.
 */
__simd_callee__ inline vector_half asc_loadalign_datablock_strided(
    __ubuf__ half* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector_bfloat16_t register holding the eight 32-byte DataBlocks gathered from the bfloat16_t UB
 * source src with the configured block_stride and repeat_stride, where mask decides which DataBlocks are loaded.
 *
 * @override_id asc_loadalign_datablock_strided__PUbf16_u16_u16_vb
 * @archs 3510
 * @param [in] src Base Unified Buffer address of the bfloat16_t source data; the effective read addresses must be
 * 32-byte aligned and the valid DataBlocks must stay inside the UB address space.
 * @param [in] block_stride Step between the start addresses of adjacent DataBlocks, counted in units of 32 bytes, so a
 * value of 2 means a 64-byte step between consecutive DataBlocks.
 * @param [in] repeat_stride Offset of the first DataBlock read relative to src, counted in units of 32 bytes, so the
 * first effective address is src plus repeat_stride * 32 bytes.
 * @param [in] mask Mask register evaluated at DataBlock granularity for bfloat16_t elements: if at least one element of
 * a DataBlock is enabled the whole 32-byte block is loaded, and if every element of the block is disabled the block is
 * skipped, the matching destination elements are set to 0 and an out-of-range address does not raise an error.
 */
__simd_callee__ inline vector_bfloat16_t asc_loadalign_datablock_strided(
    __ubuf__ bfloat16_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector_int32_t register holding the eight 32-byte DataBlocks gathered from the int32_t UB source src
 * with the configured block_stride and repeat_stride, where mask decides which DataBlocks are loaded.
 *
 * @override_id asc_loadalign_datablock_strided__PUi32_u16_u16_vb
 * @archs 3510
 * @param [in] src Base Unified Buffer address of the int32_t source data; the effective read addresses must be 32-byte
 * aligned and the valid DataBlocks must stay inside the UB address space.
 * @param [in] block_stride Step between the start addresses of adjacent DataBlocks, counted in units of 32 bytes, so a
 * value of 2 means a 64-byte step between consecutive DataBlocks.
 * @param [in] repeat_stride Offset of the first DataBlock read relative to src, counted in units of 32 bytes, so the
 * first effective address is src plus repeat_stride * 32 bytes.
 * @param [in] mask Mask register evaluated at DataBlock granularity for int32_t elements: if at least one element of a
 * DataBlock is enabled the whole 32-byte block is loaded, and if every element of the block is disabled the block is
 * skipped, the matching destination elements are set to 0 and an out-of-range address does not raise an error.
 */
__simd_callee__ inline vector_int32_t asc_loadalign_datablock_strided(
    __ubuf__ int32_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector_uint32_t register holding the eight 32-byte DataBlocks gathered from the uint32_t UB source
 * src with the configured block_stride and repeat_stride, where mask decides which DataBlocks are loaded.
 *
 * @override_id asc_loadalign_datablock_strided__PUu32_u16_u16_vb
 * @archs 3510
 * @param [in] src Base Unified Buffer address of the uint32_t source data; the effective read addresses must be 32-byte
 * aligned and the valid DataBlocks must stay inside the UB address space.
 * @param [in] block_stride Step between the start addresses of adjacent DataBlocks, counted in units of 32 bytes, so a
 * value of 2 means a 64-byte step between consecutive DataBlocks.
 * @param [in] repeat_stride Offset of the first DataBlock read relative to src, counted in units of 32 bytes, so the
 * first effective address is src plus repeat_stride * 32 bytes.
 * @param [in] mask Mask register evaluated at DataBlock granularity for uint32_t elements: if at least one element of a
 * DataBlock is enabled the whole 32-byte block is loaded, and if every element of the block is disabled the block is
 * skipped, the matching destination elements are set to 0 and an out-of-range address does not raise an error.
 */
__simd_callee__ inline vector_uint32_t asc_loadalign_datablock_strided(
    __ubuf__ uint32_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Returns a vector_float register holding the eight 32-byte DataBlocks gathered from the float UB source src
 * with the configured block_stride and repeat_stride, where mask decides which DataBlocks are loaded.
 *
 * @override_id asc_loadalign_datablock_strided__PUf32_u16_u16_vb
 * @archs 3510
 * @param [in] src Base Unified Buffer address of the float source data; the effective read addresses must be 32-byte
 * aligned and the valid DataBlocks must stay inside the UB address space.
 * @param [in] block_stride Step between the start addresses of adjacent DataBlocks, counted in units of 32 bytes, so a
 * value of 2 means a 64-byte step between consecutive DataBlocks.
 * @param [in] repeat_stride Offset of the first DataBlock read relative to src, counted in units of 32 bytes, so the
 * first effective address is src plus repeat_stride * 32 bytes.
 * @param [in] mask Mask register evaluated at DataBlock granularity for float elements: if at least one element of a
 * DataBlock is enabled the whole 32-byte block is loaded, and if every element of the block is disabled the block is
 * skipped, the matching destination elements are set to 0 and an out-of-range address does not raise an error.
 */
__simd_callee__ inline vector_float asc_loadalign_datablock_strided(
    __ubuf__ float* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

// ========== return mask register APIs ==========
/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/8 of data from a 32-byte-aligned UB address and returns it as a mask register.
 *
 * @override_id asc_loadalign_mask__PUu32
 * @archs 3510
 * @param [in] src Source UB address whose effective read address must be 32-byte aligned and whose VL/8-length read
 * must stay inside the UB address space; the address has to be updated manually before each consecutive load.
 */
__simd_callee__ inline vector_bool asc_loadalign_mask(__ubuf__ uint32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 of data from a 32-byte-aligned UB address, keeps the even-index bits and returns VL/8 as a 2x
 * downsampled mask register.
 *
 * @override_id asc_loadalign_mask_downsample__PUu32
 * @archs 3510
 * @param [in] src Source UB address whose effective read address must be 32-byte aligned and from which VL/4 of data is
 * read, with the read not exceeding the UB address space.
 */
__simd_callee__ inline vector_bool asc_loadalign_mask_downsample(__ubuf__ uint32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/16 of data from a 16-byte-aligned UB address, duplicates each bit and returns VL/8 as a 2x upsampled
 * mask register.
 *
 * @override_id asc_loadalign_mask_upsample__PUu32
 * @archs 3510
 * @param [in] src Source UB address whose effective read address must be 16-byte aligned and from which VL/16 of data
 * is read, with the read not exceeding the UB address space.
 */
__simd_callee__ inline vector_bool asc_loadalign_mask_upsample(__ubuf__ uint32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL int8_t elements from the 32-byte-aligned UB address src into the destination vector data register
 * dst.
 *
 * @override_id asc_loadalign__Rvi8_PUi8
 * @archs 3510
 * @param [out] dst Destination vector_int8_t register that receives VL int8_t elements read from src; its dtype must
 * match src.
 * @param [in] src Source UB address of the int8_t data; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign(vector_int8_t& dst, __ubuf__ int8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL uint8_t elements from the 32-byte-aligned UB address src into the destination vector data register
 * dst.
 *
 * @override_id asc_loadalign__Rvu8_PUu8
 * @archs 3510
 * @param [out] dst Destination vector_uint8_t register that receives VL uint8_t elements read from src; its dtype must
 * match src.
 * @param [in] src Source UB address of the uint8_t data; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign(vector_uint8_t& dst, __ubuf__ uint8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL fp4x2_e2m1_t elements from the 32-byte-aligned UB address src into the destination vector data
 * register dst.
 *
 * @override_id asc_loadalign__Rvf4e2m1x2_PUf4e2m1x2
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e2m1_t register that receives VL fp4x2_e2m1_t elements read from src; its
 * dtype must match src.
 * @param [in] src Source UB address of the fp4x2_e2m1_t data; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL fp4x2_e1m2_t elements from the 32-byte-aligned UB address src into the destination vector data
 * register dst.
 *
 * @override_id asc_loadalign__Rvf4e1m2x2_PUf4e1m2x2
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e1m2_t register that receives VL fp4x2_e1m2_t elements read from src; its
 * dtype must match src.
 * @param [in] src Source UB address of the fp4x2_e1m2_t data; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL int4b_t elements from the 32-byte-aligned UB address src into the destination vector data register
 * dst, whose actual register type is vector_int4x2_t.
 *
 * @override_id asc_loadalign__Rvi4x2_PUi4x2
 * @archs 3510
 * @param [out] dst Destination register that receives VL int4b_t elements read from src; its actual register type is
 * vector_int4x2_t and its element type must match src.
 * @param [in] src Source UB address of the int4b_t data; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign(vector_int4x2_t& dst, __ubuf__ int4b_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL fp8_e8m0_t elements from the 32-byte-aligned UB address src into the destination vector data register
 * dst.
 *
 * @override_id asc_loadalign__Rvf8e8m0_PUf8e8m0
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e8m0_t register that receives VL fp8_e8m0_t elements read from src; its dtype
 * must match src.
 * @param [in] src Source UB address of the fp8_e8m0_t data; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL fp8_e5m2_t elements from the 32-byte-aligned UB address src into the destination vector data register
 * dst.
 *
 * @override_id asc_loadalign__Rvf8e5m2_PUf8e5m2
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e5m2_t register that receives VL fp8_e5m2_t elements read from src; its dtype
 * must match src.
 * @param [in] src Source UB address of the fp8_e5m2_t data; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL fp8_e4m3fn_t elements from the 32-byte-aligned UB address src into the destination vector data
 * register dst.
 *
 * @override_id asc_loadalign__Rvf8e4m3_PUf8e4m3
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e4m3fn_t register that receives VL fp8_e4m3fn_t elements read from src; its
 * dtype must match src.
 * @param [in] src Source UB address of the fp8_e4m3fn_t data; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL hifloat8_t elements from the 32-byte-aligned UB address src into the destination vector data register
 * dst.
 *
 * @override_id asc_loadalign__Rvhf8_PUhf8
 * @archs 3510
 * @param [out] dst Destination vector_hifloat8_t register that receives VL hifloat8_t elements read from src; its dtype
 * must match src.
 * @param [in] src Source UB address of the hifloat8_t data; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL int16_t elements from the 32-byte-aligned UB address src into the destination vector data register
 * dst.
 *
 * @override_id asc_loadalign__Rvi16_PUi16
 * @archs 3510
 * @param [out] dst Destination vector_int16_t register that receives VL int16_t elements read from src; its dtype must
 * match src.
 * @param [in] src Source UB address of the int16_t data; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign(vector_int16_t& dst, __ubuf__ int16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL uint16_t elements from the 32-byte-aligned UB address src into the destination vector data register
 * dst.
 *
 * @override_id asc_loadalign__Rvu16_PUu16
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t register that receives VL uint16_t elements read from src; its dtype
 * must match src.
 * @param [in] src Source UB address of the uint16_t data; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign(vector_uint16_t& dst, __ubuf__ uint16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL half elements from the 32-byte-aligned UB address src into the destination vector data register dst.
 *
 * @override_id asc_loadalign__Rvf16_PUf16
 * @archs 3510
 * @param [out] dst Destination vector_half register that receives VL half elements read from src; its dtype must match
 * src.
 * @param [in] src Source UB address of the half data; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign(vector_half& dst, __ubuf__ half* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL bfloat16_t elements from the 32-byte-aligned UB address src into the destination vector data register
 * dst.
 *
 * @override_id asc_loadalign__Rvbf16_PUbf16
 * @archs 3510
 * @param [out] dst Destination vector_bfloat16_t register that receives VL bfloat16_t elements read from src; its dtype
 * must match src.
 * @param [in] src Source UB address of the bfloat16_t data; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL int32_t elements from the 32-byte-aligned UB address src into the destination vector data register
 * dst.
 *
 * @override_id asc_loadalign__Rvi32_PUi32
 * @archs 3510
 * @param [out] dst Destination vector_int32_t register that receives VL int32_t elements read from src; its dtype must
 * match src.
 * @param [in] src Source UB address of the int32_t data; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign(vector_int32_t& dst, __ubuf__ int32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL uint32_t elements from the 32-byte-aligned UB address src into the destination vector data register
 * dst.
 *
 * @override_id asc_loadalign__Rvu32_PUu32
 * @archs 3510
 * @param [out] dst Destination vector_uint32_t register that receives VL uint32_t elements read from src; its dtype
 * must match src.
 * @param [in] src Source UB address of the uint32_t data; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign(vector_uint32_t& dst, __ubuf__ uint32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL float elements from the 32-byte-aligned UB address src into the destination vector data register dst.
 *
 * @override_id asc_loadalign__Rvf32_PUf32
 * @archs 3510
 * @param [out] dst Destination vector_float register that receives VL float elements read from src; its dtype must
 * match src.
 * @param [in] src Source UB address of the float data; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign(vector_float& dst, __ubuf__ float* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL int64_t elements from the 32-byte-aligned UB address src into the destination vector data register
 * dst.
 *
 * @override_id asc_loadalign__Rvi64_PUi64
 * @archs 3510
 * @param [out] dst Destination vector_int64_t register that receives VL int64_t elements read from src; its dtype must
 * match src.
 * @param [in] src Source UB address of the int64_t data; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign(vector_int64_t& dst, __ubuf__ int64_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL uint64_t elements from the 32-byte-aligned UB address src into the destination vector data register
 * dst.
 *
 * @override_id asc_loadalign__Rvu64_PUu64
 * @archs 3510
 * @param [out] dst Destination vector_uint64_t register that receives VL uint64_t elements read from src; its dtype
 * must match src.
 * @param [in] src Source UB address of the uint64_t data; the effective read address must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign(vector_uint64_t& dst, __ubuf__ uint64_t* src);

__simd_callee__ inline void asc_loadalign_brc(vector_int8_t& dst, __ubuf__ int8_t* src);

__simd_callee__ inline void asc_loadalign_brc(vector_uint8_t& dst, __ubuf__ uint8_t* src);

__simd_callee__ inline void asc_loadalign_brc(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src);

__simd_callee__ inline void asc_loadalign_brc(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src);

__simd_callee__ inline void asc_loadalign_brc(vector_int4x2_t& dst, __ubuf__ int4b_t* src);

__simd_callee__ inline void asc_loadalign_brc(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src);

__simd_callee__ inline void asc_loadalign_brc(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src);

__simd_callee__ inline void asc_loadalign_brc(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src);

__simd_callee__ inline void asc_loadalign_brc(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src);

__simd_callee__ inline void asc_loadalign_brc(vector_int16_t& dst, __ubuf__ int16_t* src);

__simd_callee__ inline void asc_loadalign_brc(vector_uint16_t& dst, __ubuf__ uint16_t* src);

__simd_callee__ inline void asc_loadalign_brc(vector_half& dst, __ubuf__ half* src);

__simd_callee__ inline void asc_loadalign_brc(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src);

__simd_callee__ inline void asc_loadalign_brc(vector_int32_t& dst, __ubuf__ int32_t* src);

__simd_callee__ inline void asc_loadalign_brc(vector_uint32_t& dst, __ubuf__ uint32_t* src);

__simd_callee__ inline void asc_loadalign_brc(vector_float& dst, __ubuf__ float* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit int8_t elements from a 32-byte-aligned UB address, duplicates every
 * source element, and writes the upsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvi8_PUi8
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit int8_t elements to read; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_int8_t& dst, __ubuf__ int8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit uint8_t elements from a 32-byte-aligned UB address, duplicates every
 * source element, and writes the upsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvu8_PUu8
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit uint8_t elements to read; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_uint8_t& dst, __ubuf__ uint8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of packed 4-bit fp4x2_e2m1_t elements from a 32-byte-aligned UB address,
 * duplicates every source element, and writes the upsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvf4e2m1x2_PUf4e2m1x2
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the packed 4-bit fp4x2_e2m1_t elements to read; the effective read address must
 * be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of packed 4-bit fp4x2_e1m2_t elements from a 32-byte-aligned UB address,
 * duplicates every source element, and writes the upsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvf4e1m2x2_PUf4e1m2x2
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the packed 4-bit fp4x2_e1m2_t elements to read; the effective read address must
 * be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of packed 4-bit int4b_t elements from a 32-byte-aligned UB address, duplicates
 * every source element, and writes the upsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvi4x2_PUi4x2
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the packed 4-bit int4b_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_int4x2_t& dst, __ubuf__ int4b_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit fp8_e8m0_t elements from a 32-byte-aligned UB address, duplicates every
 * source element, and writes the upsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvf8e8m0_PUf8e8m0
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit fp8_e8m0_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit fp8_e5m2_t elements from a 32-byte-aligned UB address, duplicates every
 * source element, and writes the upsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvf8e5m2_PUf8e5m2
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit fp8_e5m2_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit fp8_e4m3fn_t elements from a 32-byte-aligned UB address, duplicates
 * every source element, and writes the upsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvf8e4m3_PUf8e4m3
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit fp8_e4m3fn_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit hifloat8_t elements from a 32-byte-aligned UB address, duplicates every
 * source element, and writes the upsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvhf8_PUhf8
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit hifloat8_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 16-bit int16_t elements from a 32-byte-aligned UB address, duplicates every
 * source element, and writes the upsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvi16_PUi16
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit int16_t elements to read; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_int16_t& dst, __ubuf__ int16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 16-bit uint16_t elements from a 32-byte-aligned UB address, duplicates every
 * source element, and writes the upsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvu16_PUu16
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit uint16_t elements to read; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_uint16_t& dst, __ubuf__ uint16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 16-bit half elements from a 32-byte-aligned UB address, duplicates every
 * source element, and writes the upsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvf16_PUf16
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit half elements to read; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_half& dst, __ubuf__ half* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 16-bit bfloat16_t elements from a 32-byte-aligned UB address, duplicates every
 * source element, and writes the upsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvbf16_PUbf16
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit bfloat16_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit int8_t elements from a 32-byte-aligned UB address, keeps only the
 * even-indexed elements, and writes the downsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvi8_PUi8
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit int8_t elements to read; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_int8_t& dst, __ubuf__ int8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit uint8_t elements from a 32-byte-aligned UB address, keeps only the
 * even-indexed elements, and writes the downsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvu8_PUu8
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit uint8_t elements to read; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_uint8_t& dst, __ubuf__ uint8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of packed 4-bit fp4x2_e2m1_t elements from a 32-byte-aligned UB address, keeps
 * only the even-indexed elements, and writes the downsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvf4e2m1x2_PUf4e2m1x2
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the packed 4-bit fp4x2_e2m1_t elements to read; the effective read address must
 * be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of packed 4-bit fp4x2_e1m2_t elements from a 32-byte-aligned UB address, keeps
 * only the even-indexed elements, and writes the downsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvf4e1m2x2_PUf4e1m2x2
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the packed 4-bit fp4x2_e1m2_t elements to read; the effective read address must
 * be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of packed 4-bit int4b_t elements from a 32-byte-aligned UB address, keeps only
 * the even-indexed elements, and writes the downsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvi4x2_PUi4x2
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the packed 4-bit int4b_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_int4x2_t& dst, __ubuf__ int4b_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit fp8_e8m0_t elements from a 32-byte-aligned UB address, keeps only the
 * even-indexed elements, and writes the downsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvf8e8m0_PUf8e8m0
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit fp8_e8m0_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit fp8_e5m2_t elements from a 32-byte-aligned UB address, keeps only the
 * even-indexed elements, and writes the downsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvf8e5m2_PUf8e5m2
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit fp8_e5m2_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit fp8_e4m3fn_t elements from a 32-byte-aligned UB address, keeps only
 * the even-indexed elements, and writes the downsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvf8e4m3_PUf8e4m3
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit fp8_e4m3fn_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit hifloat8_t elements from a 32-byte-aligned UB address, keeps only the
 * even-indexed elements, and writes the downsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvhf8_PUhf8
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit hifloat8_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 16-bit int16_t elements from a 32-byte-aligned UB address, keeps only the
 * even-indexed elements, and writes the downsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvi16_PUi16
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit int16_t elements to read; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_int16_t& dst, __ubuf__ int16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 16-bit uint16_t elements from a 32-byte-aligned UB address, keeps only the
 * even-indexed elements, and writes the downsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvu16_PUu16
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit uint16_t elements to read; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_uint16_t& dst, __ubuf__ uint16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 16-bit half elements from a 32-byte-aligned UB address, keeps only the
 * even-indexed elements, and writes the downsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvf16_PUf16
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit half elements to read; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_half& dst, __ubuf__ half* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 16-bit bfloat16_t elements from a 32-byte-aligned UB address, keeps only the
 * even-indexed elements, and writes the downsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvbf16_PUbf16
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit bfloat16_t elements to read; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous int8_t elements from a 32-byte aligned address with one zero element inserted after each
 * and writes the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvi8_PUi8
 * @archs 3510
 * @param [out] dst Destination vector_int8_t vector data register written with the VL-length unpacked result, one zero
 * element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the int8_t run to unpack; the effective read address must be 32-byte aligned and
 * stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_int8_t& dst, __ubuf__ int8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous uint8_t elements from a 32-byte aligned address with one zero element inserted after
 * each and writes the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvu8_PUu8
 * @archs 3510
 * @param [out] dst Destination vector_uint8_t vector data register written with the VL-length unpacked result, one zero
 * element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the uint8_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_uint8_t& dst, __ubuf__ uint8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous fp4x2_e2m1_t elements from a 32-byte aligned address with one zero element inserted
 * after each and writes the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvf4e2m1x2_PUf4e2m1x2
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e2m1_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the fp4x2_e2m1_t run to unpack; the effective read address must be 32-byte
 * aligned and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous fp4x2_e1m2_t elements from a 32-byte aligned address with one zero element inserted
 * after each and writes the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvf4e1m2x2_PUf4e1m2x2
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e1m2_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the fp4x2_e1m2_t run to unpack; the effective read address must be 32-byte
 * aligned and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous int4b_t elements from a 32-byte aligned address with one zero element inserted after
 * each and writes the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvi4x2_PUi4x2
 * @archs 3510
 * @param [out] dst Destination vector_int4x2_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the int4b_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_int4x2_t& dst, __ubuf__ int4b_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous fp8_e8m0_t elements from a 32-byte aligned address with one zero element inserted after
 * each and writes the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvf8e8m0_PUf8e8m0
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e8m0_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the fp8_e8m0_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous fp8_e5m2_t elements from a 32-byte aligned address with one zero element inserted after
 * each and writes the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvf8e5m2_PUf8e5m2
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e5m2_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the fp8_e5m2_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous fp8_e4m3fn_t elements from a 32-byte aligned address with one zero element inserted
 * after each and writes the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvf8e4m3_PUf8e4m3
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e4m3fn_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the fp8_e4m3fn_t run to unpack; the effective read address must be 32-byte
 * aligned and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous hifloat8_t elements from a 32-byte aligned address with one zero element inserted after
 * each and writes the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvhf8_PUhf8
 * @archs 3510
 * @param [out] dst Destination vector_hifloat8_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the hifloat8_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous int16_t elements from a 32-byte aligned address with one zero element inserted after
 * each and writes the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvi16_PUi16
 * @archs 3510
 * @param [out] dst Destination vector_int16_t vector data register written with the VL-length unpacked result, one zero
 * element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the int16_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_int16_t& dst, __ubuf__ int16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous uint16_t elements from a 32-byte aligned address with one zero element inserted after
 * each and writes the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvu16_PUu16
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the uint16_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_uint16_t& dst, __ubuf__ uint16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous half elements from a 32-byte aligned address with one zero element inserted after each
 * and writes the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvf16_PUf16
 * @archs 3510
 * @param [out] dst Destination vector_half vector data register written with the VL-length unpacked result, one zero
 * element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the half run to unpack; the effective read address must be 32-byte aligned and
 * stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_half& dst, __ubuf__ half* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous bfloat16_t elements from a 32-byte aligned address with one zero element inserted after
 * each and writes the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvbf16_PUbf16
 * @archs 3510
 * @param [out] dst Destination vector_bfloat16_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the bfloat16_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous int32_t elements from a 32-byte aligned address with one zero element inserted after
 * each and writes the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvi32_PUi32
 * @archs 3510
 * @param [out] dst Destination vector_int32_t vector data register written with the VL-length unpacked result, one zero
 * element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the int32_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_int32_t& dst, __ubuf__ int32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous uint32_t elements from a 32-byte aligned address with one zero element inserted after
 * each and writes the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvu32_PUu32
 * @archs 3510
 * @param [out] dst Destination vector_uint32_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the uint32_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_uint32_t& dst, __ubuf__ uint32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous float elements from a 32-byte aligned address with one zero element inserted after each
 * and writes the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvf32_PUf32
 * @archs 3510
 * @param [out] dst Destination vector_float vector data register written with the VL-length unpacked result, one zero
 * element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the float run to unpack; the effective read address must be 32-byte aligned and
 * stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_float& dst, __ubuf__ float* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element window at the 32-byte aligned UB address src into two vector data registers: the
 * even-index int8_t elements go to vector_int8_t dst0 and the odd-index elements to dst1, VL elements each, and the
 * caller has to advance src itself.
 *
 * @override_id asc_loadalign_deintlv__Rvi8_Rvi8_PUi8
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index int8_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index int8_t elements of the 2*VL window (VL
 * elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the address actually read must be 32-byte aligned and the whole
 * 2*VL read must stay inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_deintlv(vector_int8_t& dst0, vector_int8_t& dst1, __ubuf__ int8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element window at the 32-byte aligned UB address src into two vector data registers: the
 * even-index uint8_t elements go to vector_uint8_t dst0 and the odd-index elements to dst1, VL elements each, and the
 * caller has to advance src itself.
 *
 * @override_id asc_loadalign_deintlv__Rvu8_Rvu8_PUu8
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index uint8_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index uint8_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the address actually read must be 32-byte aligned and the whole
 * 2*VL read must stay inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint8_t& dst0, vector_uint8_t& dst1, __ubuf__ uint8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element window at the 32-byte aligned UB address src into two vector data registers: the
 * even-index fp4x2_e2m1_t elements go to vector_fp4x2_e2m1_t dst0 and the odd-index elements to dst1, VL elements each,
 * and the caller has to advance src itself.
 *
 * @override_id asc_loadalign_deintlv__Rvf4e2m1x2_Rvf4e2m1x2_PUf4e2m1x2
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index fp4x2_e2m1_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index fp4x2_e2m1_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the address actually read must be 32-byte aligned and the whole
 * 2*VL read must stay inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_fp4x2_e2m1_t& dst0, vector_fp4x2_e2m1_t& dst1, __ubuf__ fp4x2_e2m1_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element window at the 32-byte aligned UB address src into two vector data registers: the
 * even-index fp4x2_e1m2_t elements go to vector_fp4x2_e1m2_t dst0 and the odd-index elements to dst1, VL elements each,
 * and the caller has to advance src itself.
 *
 * @override_id asc_loadalign_deintlv__Rvf4e1m2x2_Rvf4e1m2x2_PUf4e1m2x2
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index fp4x2_e1m2_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index fp4x2_e1m2_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the address actually read must be 32-byte aligned and the whole
 * 2*VL read must stay inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_fp4x2_e1m2_t& dst0, vector_fp4x2_e1m2_t& dst1, __ubuf__ fp4x2_e1m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element window at the 32-byte aligned UB address src into two vector data registers: the
 * even-index int4b_t elements go to vector_int4x2_t dst0 and the odd-index elements to dst1, VL elements each, and the
 * caller has to advance src itself.
 *
 * @override_id asc_loadalign_deintlv__Rvi4x2_Rvi4x2_PUi4x2
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index int4b_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index int4b_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the address actually read must be 32-byte aligned and the whole
 * 2*VL read must stay inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_deintlv(vector_int4x2_t& dst0, vector_int4x2_t& dst1, __ubuf__ int4b_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element window at the 32-byte aligned UB address src into two vector data registers: the
 * even-index fp8_e8m0_t elements go to vector_fp8_e8m0_t dst0 and the odd-index elements to dst1, VL elements each, and
 * the caller has to advance src itself.
 *
 * @override_id asc_loadalign_deintlv__Rvf8e8m0_Rvf8e8m0_PUf8e8m0
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index fp8_e8m0_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index fp8_e8m0_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the address actually read must be 32-byte aligned and the whole
 * 2*VL read must stay inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, __ubuf__ fp8_e8m0_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element window at the 32-byte aligned UB address src into two vector data registers: the
 * even-index fp8_e5m2_t elements go to vector_fp8_e5m2_t dst0 and the odd-index elements to dst1, VL elements each, and
 * the caller has to advance src itself.
 *
 * @override_id asc_loadalign_deintlv__Rvf8e5m2_Rvf8e5m2_PUf8e5m2
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index fp8_e5m2_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index fp8_e5m2_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the address actually read must be 32-byte aligned and the whole
 * 2*VL read must stay inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, __ubuf__ fp8_e5m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element window at the 32-byte aligned UB address src into two vector data registers: the
 * even-index fp8_e4m3fn_t elements go to vector_fp8_e4m3fn_t dst0 and the odd-index elements to dst1, VL elements each,
 * and the caller has to advance src itself.
 *
 * @override_id asc_loadalign_deintlv__Rvf8e4m3_Rvf8e4m3_PUf8e4m3
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index fp8_e4m3fn_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index fp8_e4m3fn_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the address actually read must be 32-byte aligned and the whole
 * 2*VL read must stay inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, __ubuf__ fp8_e4m3fn_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element window at the 32-byte aligned UB address src into two vector data registers: the
 * even-index hifloat8_t elements go to vector_hifloat8_t dst0 and the odd-index elements to dst1, VL elements each, and
 * the caller has to advance src itself.
 *
 * @override_id asc_loadalign_deintlv__Rvhf8_Rvhf8_PUhf8
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index hifloat8_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index hifloat8_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the address actually read must be 32-byte aligned and the whole
 * 2*VL read must stay inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, __ubuf__ hifloat8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element window at the 32-byte aligned UB address src into two vector data registers: the
 * even-index int16_t elements go to vector_int16_t dst0 and the odd-index elements to dst1, VL elements each, and the
 * caller has to advance src itself.
 *
 * @override_id asc_loadalign_deintlv__Rvi16_Rvi16_PUi16
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index int16_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index int16_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the address actually read must be 32-byte aligned and the whole
 * 2*VL read must stay inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_deintlv(vector_int16_t& dst0, vector_int16_t& dst1, __ubuf__ int16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element window at the 32-byte aligned UB address src into two vector data registers: the
 * even-index uint16_t elements go to vector_uint16_t dst0 and the odd-index elements to dst1, VL elements each, and the
 * caller has to advance src itself.
 *
 * @override_id asc_loadalign_deintlv__Rvu16_Rvu16_PUu16
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index uint16_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index uint16_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the address actually read must be 32-byte aligned and the whole
 * 2*VL read must stay inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint16_t& dst0, vector_uint16_t& dst1, __ubuf__ uint16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element window at the 32-byte aligned UB address src into two vector data registers: the
 * even-index half elements go to vector_half dst0 and the odd-index elements to dst1, VL elements each, and the caller
 * has to advance src itself.
 *
 * @override_id asc_loadalign_deintlv__Rvf16_Rvf16_PUf16
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index half elements of the 2*VL window (VL
 * elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index half elements of the 2*VL window (VL
 * elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the address actually read must be 32-byte aligned and the whole
 * 2*VL read must stay inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_deintlv(vector_half& dst0, vector_half& dst1, __ubuf__ half* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element window at the 32-byte aligned UB address src into two vector data registers: the
 * even-index bfloat16_t elements go to vector_bfloat16_t dst0 and the odd-index elements to dst1, VL elements each, and
 * the caller has to advance src itself.
 *
 * @override_id asc_loadalign_deintlv__Rvbf16_Rvbf16_PUbf16
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index bfloat16_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index bfloat16_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the address actually read must be 32-byte aligned and the whole
 * 2*VL read must stay inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, __ubuf__ bfloat16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element window at the 32-byte aligned UB address src into two vector data registers: the
 * even-index int32_t elements go to vector_int32_t dst0 and the odd-index elements to dst1, VL elements each, and the
 * caller has to advance src itself.
 *
 * @override_id asc_loadalign_deintlv__Rvi32_Rvi32_PUi32
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index int32_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index int32_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the address actually read must be 32-byte aligned and the whole
 * 2*VL read must stay inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_deintlv(vector_int32_t& dst0, vector_int32_t& dst1, __ubuf__ int32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element window at the 32-byte aligned UB address src into two vector data registers: the
 * even-index uint32_t elements go to vector_uint32_t dst0 and the odd-index elements to dst1, VL elements each, and the
 * caller has to advance src itself.
 *
 * @override_id asc_loadalign_deintlv__Rvu32_Rvu32_PUu32
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index uint32_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index uint32_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the address actually read must be 32-byte aligned and the whole
 * 2*VL read must stay inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_deintlv(vector_uint32_t& dst0, vector_uint32_t& dst1, __ubuf__ uint32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element window at the 32-byte aligned UB address src into two vector data registers: the
 * even-index float elements go to vector_float dst0 and the odd-index elements to dst1, VL elements each, and the
 * caller has to advance src itself.
 *
 * @override_id asc_loadalign_deintlv__Rvf32_Rvf32_PUf32
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index float elements of the 2*VL window (VL
 * elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index float elements of the 2*VL window (VL
 * elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the address actually read must be 32-byte aligned and the whole
 * 2*VL read must stay inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_deintlv(vector_float& dst0, vector_float& dst1, __ubuf__ float* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL int8_t elements from src advanced by offset elements into dst, leaving src itself unmodified.
 *
 * @override_id asc_loadalign__Rvi8_PUi8_i32
 * @archs 3510
 * @param [out] dst Destination vector_int8_t register that receives VL int8_t elements read from src; its dtype must
 * match src.
 * @param [in] src Source UB address of the int8_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Signed element-count offset relative to the src start address; the resulting address src+offset
 * must remain 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign(vector_int8_t& dst, __ubuf__ int8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL uint8_t elements from src advanced by offset elements into dst, leaving src itself unmodified.
 *
 * @override_id asc_loadalign__Rvu8_PUu8_i32
 * @archs 3510
 * @param [out] dst Destination vector_uint8_t register that receives VL uint8_t elements read from src; its dtype must
 * match src.
 * @param [in] src Source UB address of the uint8_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Signed element-count offset relative to the src start address; the resulting address src+offset
 * must remain 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign(vector_uint8_t& dst, __ubuf__ uint8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL fp4x2_e2m1_t elements from src advanced by offset elements into dst, leaving src itself unmodified.
 *
 * @override_id asc_loadalign__Rvf4e2m1x2_PUf4e2m1x2_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e2m1_t register that receives VL fp4x2_e2m1_t elements read from src; its
 * dtype must match src.
 * @param [in] src Source UB address of the fp4x2_e2m1_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Signed element-count offset relative to the src start address; the resulting address src+offset
 * must remain 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL fp4x2_e1m2_t elements from src advanced by offset elements into dst, leaving src itself unmodified.
 *
 * @override_id asc_loadalign__Rvf4e1m2x2_PUf4e1m2x2_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e1m2_t register that receives VL fp4x2_e1m2_t elements read from src; its
 * dtype must match src.
 * @param [in] src Source UB address of the fp4x2_e1m2_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Signed element-count offset relative to the src start address; the resulting address src+offset
 * must remain 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL int4b_t elements from src advanced by offset elements into dst, leaving src itself unmodified.
 *
 * @override_id asc_loadalign__Rvi4x2_PUi4x2_i32
 * @archs 3510
 * @param [out] dst Destination register that receives VL int4b_t elements read from src; its actual register type is
 * vector_int4x2_t and its element type must match src.
 * @param [in] src Source UB address of the int4b_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Signed element-count offset relative to the src start address; the resulting address src+offset
 * must remain 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign(vector_int4x2_t& dst, __ubuf__ int4b_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL fp8_e8m0_t elements from src advanced by offset elements into dst, leaving src itself unmodified.
 *
 * @override_id asc_loadalign__Rvf8e8m0_PUf8e8m0_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e8m0_t register that receives VL fp8_e8m0_t elements read from src; its dtype
 * must match src.
 * @param [in] src Source UB address of the fp8_e8m0_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Signed element-count offset relative to the src start address; the resulting address src+offset
 * must remain 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL fp8_e5m2_t elements from src advanced by offset elements into dst, leaving src itself unmodified.
 *
 * @override_id asc_loadalign__Rvf8e5m2_PUf8e5m2_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e5m2_t register that receives VL fp8_e5m2_t elements read from src; its dtype
 * must match src.
 * @param [in] src Source UB address of the fp8_e5m2_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Signed element-count offset relative to the src start address; the resulting address src+offset
 * must remain 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL fp8_e4m3fn_t elements from src advanced by offset elements into dst, leaving src itself unmodified.
 *
 * @override_id asc_loadalign__Rvf8e4m3_PUf8e4m3_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e4m3fn_t register that receives VL fp8_e4m3fn_t elements read from src; its
 * dtype must match src.
 * @param [in] src Source UB address of the fp8_e4m3fn_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Signed element-count offset relative to the src start address; the resulting address src+offset
 * must remain 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL hifloat8_t elements from src advanced by offset elements into dst, leaving src itself unmodified.
 *
 * @override_id asc_loadalign__Rvhf8_PUhf8_i32
 * @archs 3510
 * @param [out] dst Destination vector_hifloat8_t register that receives VL hifloat8_t elements read from src; its dtype
 * must match src.
 * @param [in] src Source UB address of the hifloat8_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Signed element-count offset relative to the src start address; the resulting address src+offset
 * must remain 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL int16_t elements from src advanced by offset elements into dst, leaving src itself unmodified.
 *
 * @override_id asc_loadalign__Rvi16_PUi16_i32
 * @archs 3510
 * @param [out] dst Destination vector_int16_t register that receives VL int16_t elements read from src; its dtype must
 * match src.
 * @param [in] src Source UB address of the int16_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Signed element-count offset relative to the src start address; the resulting address src+offset
 * must remain 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign(vector_int16_t& dst, __ubuf__ int16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL uint16_t elements from src advanced by offset elements into dst, leaving src itself unmodified.
 *
 * @override_id asc_loadalign__Rvu16_PUu16_i32
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t register that receives VL uint16_t elements read from src; its dtype
 * must match src.
 * @param [in] src Source UB address of the uint16_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Signed element-count offset relative to the src start address; the resulting address src+offset
 * must remain 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign(vector_uint16_t& dst, __ubuf__ uint16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL half elements from src advanced by offset elements into dst, leaving src itself unmodified.
 *
 * @override_id asc_loadalign__Rvf16_PUf16_i32
 * @archs 3510
 * @param [out] dst Destination vector_half register that receives VL half elements read from src; its dtype must match
 * src.
 * @param [in] src Source UB address of the half data; the effective read address must be 32-byte aligned.
 * @param [in] offset Signed element-count offset relative to the src start address; the resulting address src+offset
 * must remain 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign(vector_half& dst, __ubuf__ half* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL bfloat16_t elements from src advanced by offset elements into dst, leaving src itself unmodified.
 *
 * @override_id asc_loadalign__Rvbf16_PUbf16_i32
 * @archs 3510
 * @param [out] dst Destination vector_bfloat16_t register that receives VL bfloat16_t elements read from src; its dtype
 * must match src.
 * @param [in] src Source UB address of the bfloat16_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Signed element-count offset relative to the src start address; the resulting address src+offset
 * must remain 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL int32_t elements from src advanced by offset elements into dst, leaving src itself unmodified.
 *
 * @override_id asc_loadalign__Rvi32_PUi32_i32
 * @archs 3510
 * @param [out] dst Destination vector_int32_t register that receives VL int32_t elements read from src; its dtype must
 * match src.
 * @param [in] src Source UB address of the int32_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Signed element-count offset relative to the src start address; the resulting address src+offset
 * must remain 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign(vector_int32_t& dst, __ubuf__ int32_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL uint32_t elements from src advanced by offset elements into dst, leaving src itself unmodified.
 *
 * @override_id asc_loadalign__Rvu32_PUu32_i32
 * @archs 3510
 * @param [out] dst Destination vector_uint32_t register that receives VL uint32_t elements read from src; its dtype
 * must match src.
 * @param [in] src Source UB address of the uint32_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Signed element-count offset relative to the src start address; the resulting address src+offset
 * must remain 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign(vector_uint32_t& dst, __ubuf__ uint32_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL float elements from src advanced by offset elements into dst, leaving src itself unmodified.
 *
 * @override_id asc_loadalign__Rvf32_PUf32_i32
 * @archs 3510
 * @param [out] dst Destination vector_float register that receives VL float elements read from src; its dtype must
 * match src.
 * @param [in] src Source UB address of the float data; the effective read address must be 32-byte aligned.
 * @param [in] offset Signed element-count offset relative to the src start address; the resulting address src+offset
 * must remain 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign(vector_float& dst, __ubuf__ float* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL int64_t elements from src advanced by offset elements into dst, leaving src itself unmodified.
 *
 * @override_id asc_loadalign__Rvi64_PUi64_i32
 * @archs 3510
 * @param [out] dst Destination vector_int64_t register that receives VL int64_t elements read from src; its dtype must
 * match src.
 * @param [in] src Source UB address of the int64_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Signed element-count offset relative to the src start address; the resulting address src+offset
 * must remain 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign(vector_int64_t& dst, __ubuf__ int64_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL uint64_t elements from src advanced by offset elements into dst, leaving src itself unmodified.
 *
 * @override_id asc_loadalign__Rvu64_PUu64_i32
 * @archs 3510
 * @param [out] dst Destination vector_uint64_t register that receives VL uint64_t elements read from src; its dtype
 * must match src.
 * @param [in] src Source UB address of the uint64_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Signed element-count offset relative to the src start address; the resulting address src+offset
 * must remain 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign(vector_uint64_t& dst, __ubuf__ uint64_t* src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc(vector_int8_t& dst, __ubuf__ int8_t* src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc(vector_uint8_t& dst, __ubuf__ uint8_t* src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc(vector_int4x2_t& dst, __ubuf__ int4b_t* src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc(vector_int16_t& dst, __ubuf__ int16_t* src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc(vector_uint16_t& dst, __ubuf__ uint16_t* src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc(vector_half& dst, __ubuf__ half* src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc(vector_int32_t& dst, __ubuf__ int32_t* src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc(vector_uint32_t& dst, __ubuf__ uint32_t* src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc(vector_float& dst, __ubuf__ float* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit int8_t elements starting at the 32-byte-aligned UB address src plus
 * offset elements, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvi8_PUi8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit int8_t elements to read; the read starts at src plus offset elements
 * and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_int8_t& dst, __ubuf__ int8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit uint8_t elements starting at the 32-byte-aligned UB address src plus
 * offset elements, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvu8_PUu8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit uint8_t elements to read; the read starts at src plus offset elements
 * and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_uint8_t& dst, __ubuf__ uint8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of packed 4-bit fp4x2_e2m1_t elements starting at the 32-byte-aligned UB address
 * src plus offset elements, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes)
 * into dst.
 *
 * @override_id asc_loadalign_upsample__Rvf4e2m1x2_PUf4e2m1x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the packed 4-bit fp4x2_e2m1_t elements to read; the read starts at src plus
 * offset elements and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_upsample(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of packed 4-bit fp4x2_e1m2_t elements starting at the 32-byte-aligned UB address
 * src plus offset elements, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes)
 * into dst.
 *
 * @override_id asc_loadalign_upsample__Rvf4e1m2x2_PUf4e1m2x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the packed 4-bit fp4x2_e1m2_t elements to read; the read starts at src plus
 * offset elements and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_upsample(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of packed 4-bit int4b_t elements starting at the 32-byte-aligned UB address src
 * plus offset elements, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes) into
 * dst.
 *
 * @override_id asc_loadalign_upsample__Rvi4x2_PUi4x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the packed 4-bit int4b_t elements to read; the read starts at src plus offset
 * elements and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_int4x2_t& dst, __ubuf__ int4b_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit fp8_e8m0_t elements starting at the 32-byte-aligned UB address src plus
 * offset elements, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvf8e8m0_PUf8e8m0_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit fp8_e8m0_t elements to read; the read starts at src plus offset
 * elements and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit fp8_e5m2_t elements starting at the 32-byte-aligned UB address src plus
 * offset elements, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvf8e5m2_PUf8e5m2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit fp8_e5m2_t elements to read; the read starts at src plus offset
 * elements and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit fp8_e4m3fn_t elements starting at the 32-byte-aligned UB address src
 * plus offset elements, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes) into
 * dst.
 *
 * @override_id asc_loadalign_upsample__Rvf8e4m3_PUf8e4m3_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit fp8_e4m3fn_t elements to read; the read starts at src plus offset
 * elements and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_upsample(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit hifloat8_t elements starting at the 32-byte-aligned UB address src plus
 * offset elements, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvhf8_PUhf8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit hifloat8_t elements to read; the read starts at src plus offset
 * elements and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 16-bit int16_t elements starting at the 32-byte-aligned UB address src plus
 * offset elements, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvi16_PUi16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit int16_t elements to read; the read starts at src plus offset elements
 * and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_int16_t& dst, __ubuf__ int16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 16-bit uint16_t elements starting at the 32-byte-aligned UB address src plus
 * offset elements, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvu16_PUu16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit uint16_t elements to read; the read starts at src plus offset
 * elements and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_uint16_t& dst, __ubuf__ uint16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 16-bit half elements starting at the 32-byte-aligned UB address src plus
 * offset elements, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvf16_PUf16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit half elements to read; the read starts at src plus offset elements
 * and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_half& dst, __ubuf__ half* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 16-bit bfloat16_t elements starting at the 32-byte-aligned UB address src plus
 * offset elements, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvbf16_PUbf16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit bfloat16_t elements to read; the read starts at src plus offset
 * elements and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit int8_t elements starting at the 32-byte-aligned UB address src plus
 * offset elements, keeps only the even-indexed elements, and writes the downsampled result of VL bytes (256 bytes) into
 * dst.
 *
 * @override_id asc_loadalign_downsample__Rvi8_PUi8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit int8_t elements to read; the read starts at src plus offset elements
 * and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_int8_t& dst, __ubuf__ int8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit uint8_t elements starting at the 32-byte-aligned UB address src plus
 * offset elements, keeps only the even-indexed elements, and writes the downsampled result of VL bytes (256 bytes) into
 * dst.
 *
 * @override_id asc_loadalign_downsample__Rvu8_PUu8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit uint8_t elements to read; the read starts at src plus offset elements
 * and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_uint8_t& dst, __ubuf__ uint8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of packed 4-bit fp4x2_e2m1_t elements starting at the 32-byte-aligned UB
 * address src plus offset elements, keeps only the even-indexed elements, and writes the downsampled result of VL bytes
 * (256 bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvf4e2m1x2_PUf4e2m1x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the packed 4-bit fp4x2_e2m1_t elements to read; the read starts at src plus
 * offset elements and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_downsample(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of packed 4-bit fp4x2_e1m2_t elements starting at the 32-byte-aligned UB
 * address src plus offset elements, keeps only the even-indexed elements, and writes the downsampled result of VL bytes
 * (256 bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvf4e1m2x2_PUf4e1m2x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the packed 4-bit fp4x2_e1m2_t elements to read; the read starts at src plus
 * offset elements and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_downsample(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of packed 4-bit int4b_t elements starting at the 32-byte-aligned UB address src
 * plus offset elements, keeps only the even-indexed elements, and writes the downsampled result of VL bytes (256 bytes)
 * into dst.
 *
 * @override_id asc_loadalign_downsample__Rvi4x2_PUi4x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the packed 4-bit int4b_t elements to read; the read starts at src plus offset
 * elements and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_int4x2_t& dst, __ubuf__ int4b_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit fp8_e8m0_t elements starting at the 32-byte-aligned UB address src
 * plus offset elements, keeps only the even-indexed elements, and writes the downsampled result of VL bytes (256 bytes)
 * into dst.
 *
 * @override_id asc_loadalign_downsample__Rvf8e8m0_PUf8e8m0_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit fp8_e8m0_t elements to read; the read starts at src plus offset
 * elements and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit fp8_e5m2_t elements starting at the 32-byte-aligned UB address src
 * plus offset elements, keeps only the even-indexed elements, and writes the downsampled result of VL bytes (256 bytes)
 * into dst.
 *
 * @override_id asc_loadalign_downsample__Rvf8e5m2_PUf8e5m2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit fp8_e5m2_t elements to read; the read starts at src plus offset
 * elements and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit fp8_e4m3fn_t elements starting at the 32-byte-aligned UB address src
 * plus offset elements, keeps only the even-indexed elements, and writes the downsampled result of VL bytes (256 bytes)
 * into dst.
 *
 * @override_id asc_loadalign_downsample__Rvf8e4m3_PUf8e4m3_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit fp8_e4m3fn_t elements to read; the read starts at src plus offset
 * elements and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_downsample(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit hifloat8_t elements starting at the 32-byte-aligned UB address src
 * plus offset elements, keeps only the even-indexed elements, and writes the downsampled result of VL bytes (256 bytes)
 * into dst.
 *
 * @override_id asc_loadalign_downsample__Rvhf8_PUhf8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit hifloat8_t elements to read; the read starts at src plus offset
 * elements and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 16-bit int16_t elements starting at the 32-byte-aligned UB address src plus
 * offset elements, keeps only the even-indexed elements, and writes the downsampled result of VL bytes (256 bytes) into
 * dst.
 *
 * @override_id asc_loadalign_downsample__Rvi16_PUi16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit int16_t elements to read; the read starts at src plus offset elements
 * and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_int16_t& dst, __ubuf__ int16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 16-bit uint16_t elements starting at the 32-byte-aligned UB address src plus
 * offset elements, keeps only the even-indexed elements, and writes the downsampled result of VL bytes (256 bytes) into
 * dst.
 *
 * @override_id asc_loadalign_downsample__Rvu16_PUu16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit uint16_t elements to read; the read starts at src plus offset
 * elements and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_uint16_t& dst, __ubuf__ uint16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 16-bit half elements starting at the 32-byte-aligned UB address src plus
 * offset elements, keeps only the even-indexed elements, and writes the downsampled result of VL bytes (256 bytes) into
 * dst.
 *
 * @override_id asc_loadalign_downsample__Rvf16_PUf16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit half elements to read; the read starts at src plus offset elements
 * and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_half& dst, __ubuf__ half* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 16-bit bfloat16_t elements starting at the 32-byte-aligned UB address src
 * plus offset elements, keeps only the even-indexed elements, and writes the downsampled result of VL bytes (256 bytes)
 * into dst.
 *
 * @override_id asc_loadalign_downsample__Rvbf16_PUbf16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit bfloat16_t elements to read; the read starts at src plus offset
 * elements and must stay 32-byte aligned.
 * @param [in] offset Signed element offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous int8_t elements starting offset elements past a 32-byte aligned address with one zero
 * element inserted after each and writes the VL-length result to dst, without updating src.
 *
 * @override_id asc_loadalign_unpack__Rvi8_PUi8_i32
 * @archs 3510
 * @param [out] dst Destination vector_int8_t vector data register written with the VL-length unpacked result, one zero
 * element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the int8_t run to unpack; the effective read address must be 32-byte aligned and
 * stay inside UB.
 * @param [in] offset Signed element-count offset added to src before reading the VL/2 elements; the API does not update
 * src for the next call.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_int8_t& dst, __ubuf__ int8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous uint8_t elements starting offset elements past a 32-byte aligned address with one zero
 * element inserted after each and writes the VL-length result to dst, without updating src.
 *
 * @override_id asc_loadalign_unpack__Rvu8_PUu8_i32
 * @archs 3510
 * @param [out] dst Destination vector_uint8_t vector data register written with the VL-length unpacked result, one zero
 * element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the uint8_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before reading the VL/2 elements; the API does not update
 * src for the next call.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_uint8_t& dst, __ubuf__ uint8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous fp4x2_e2m1_t elements starting offset elements past a 32-byte aligned address with one
 * zero element inserted after each and writes the VL-length result to dst, without updating src.
 *
 * @override_id asc_loadalign_unpack__Rvf4e2m1x2_PUf4e2m1x2_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e2m1_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the fp4x2_e2m1_t run to unpack; the effective read address must be 32-byte
 * aligned and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before reading the VL/2 elements; the API does not update
 * src for the next call.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous fp4x2_e1m2_t elements starting offset elements past a 32-byte aligned address with one
 * zero element inserted after each and writes the VL-length result to dst, without updating src.
 *
 * @override_id asc_loadalign_unpack__Rvf4e1m2x2_PUf4e1m2x2_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e1m2_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the fp4x2_e1m2_t run to unpack; the effective read address must be 32-byte
 * aligned and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before reading the VL/2 elements; the API does not update
 * src for the next call.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous int4b_t elements starting offset elements past a 32-byte aligned address with one zero
 * element inserted after each and writes the VL-length result to dst, without updating src.
 *
 * @override_id asc_loadalign_unpack__Rvi4x2_PUi4x2_i32
 * @archs 3510
 * @param [out] dst Destination vector_int4x2_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the int4b_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before reading the VL/2 elements; the API does not update
 * src for the next call.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_int4x2_t& dst, __ubuf__ int4b_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous fp8_e8m0_t elements starting offset elements past a 32-byte aligned address with one
 * zero element inserted after each and writes the VL-length result to dst, without updating src.
 *
 * @override_id asc_loadalign_unpack__Rvf8e8m0_PUf8e8m0_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e8m0_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the fp8_e8m0_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before reading the VL/2 elements; the API does not update
 * src for the next call.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous fp8_e5m2_t elements starting offset elements past a 32-byte aligned address with one
 * zero element inserted after each and writes the VL-length result to dst, without updating src.
 *
 * @override_id asc_loadalign_unpack__Rvf8e5m2_PUf8e5m2_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e5m2_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the fp8_e5m2_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before reading the VL/2 elements; the API does not update
 * src for the next call.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous fp8_e4m3fn_t elements starting offset elements past a 32-byte aligned address with one
 * zero element inserted after each and writes the VL-length result to dst, without updating src.
 *
 * @override_id asc_loadalign_unpack__Rvf8e4m3_PUf8e4m3_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e4m3fn_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the fp8_e4m3fn_t run to unpack; the effective read address must be 32-byte
 * aligned and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before reading the VL/2 elements; the API does not update
 * src for the next call.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous hifloat8_t elements starting offset elements past a 32-byte aligned address with one
 * zero element inserted after each and writes the VL-length result to dst, without updating src.
 *
 * @override_id asc_loadalign_unpack__Rvhf8_PUhf8_i32
 * @archs 3510
 * @param [out] dst Destination vector_hifloat8_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the hifloat8_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before reading the VL/2 elements; the API does not update
 * src for the next call.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous int16_t elements starting offset elements past a 32-byte aligned address with one zero
 * element inserted after each and writes the VL-length result to dst, without updating src.
 *
 * @override_id asc_loadalign_unpack__Rvi16_PUi16_i32
 * @archs 3510
 * @param [out] dst Destination vector_int16_t vector data register written with the VL-length unpacked result, one zero
 * element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the int16_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before reading the VL/2 elements; the API does not update
 * src for the next call.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_int16_t& dst, __ubuf__ int16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous uint16_t elements starting offset elements past a 32-byte aligned address with one zero
 * element inserted after each and writes the VL-length result to dst, without updating src.
 *
 * @override_id asc_loadalign_unpack__Rvu16_PUu16_i32
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the uint16_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before reading the VL/2 elements; the API does not update
 * src for the next call.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_uint16_t& dst, __ubuf__ uint16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous half elements starting offset elements past a 32-byte aligned address with one zero
 * element inserted after each and writes the VL-length result to dst, without updating src.
 *
 * @override_id asc_loadalign_unpack__Rvf16_PUf16_i32
 * @archs 3510
 * @param [out] dst Destination vector_half vector data register written with the VL-length unpacked result, one zero
 * element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the half run to unpack; the effective read address must be 32-byte aligned and
 * stay inside UB.
 * @param [in] offset Signed element-count offset added to src before reading the VL/2 elements; the API does not update
 * src for the next call.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_half& dst, __ubuf__ half* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous bfloat16_t elements starting offset elements past a 32-byte aligned address with one
 * zero element inserted after each and writes the VL-length result to dst, without updating src.
 *
 * @override_id asc_loadalign_unpack__Rvbf16_PUbf16_i32
 * @archs 3510
 * @param [out] dst Destination vector_bfloat16_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the bfloat16_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before reading the VL/2 elements; the API does not update
 * src for the next call.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous int32_t elements starting offset elements past a 32-byte aligned address with one zero
 * element inserted after each and writes the VL-length result to dst, without updating src.
 *
 * @override_id asc_loadalign_unpack__Rvi32_PUi32_i32
 * @archs 3510
 * @param [out] dst Destination vector_int32_t vector data register written with the VL-length unpacked result, one zero
 * element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the int32_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before reading the VL/2 elements; the API does not update
 * src for the next call.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_int32_t& dst, __ubuf__ int32_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous uint32_t elements starting offset elements past a 32-byte aligned address with one zero
 * element inserted after each and writes the VL-length result to dst, without updating src.
 *
 * @override_id asc_loadalign_unpack__Rvu32_PUu32_i32
 * @archs 3510
 * @param [out] dst Destination vector_uint32_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the uint32_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before reading the VL/2 elements; the API does not update
 * src for the next call.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_uint32_t& dst, __ubuf__ uint32_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous float elements starting offset elements past a 32-byte aligned address with one zero
 * element inserted after each and writes the VL-length result to dst, without updating src.
 *
 * @override_id asc_loadalign_unpack__Rvf32_PUf32_i32
 * @archs 3510
 * @param [out] dst Destination vector_float vector data register written with the VL-length unpacked result, one zero
 * element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the float run to unpack; the effective read address must be 32-byte aligned and
 * stay inside UB.
 * @param [in] offset Signed element-count offset added to src before reading the VL/2 elements; the API does not update
 * src for the next call.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_float& dst, __ubuf__ float* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element int8_t window read at src plus an element-count offset into two vector data
 * registers, writing the even-index elements to vector_int8_t dst0 and the odd-index elements to dst1, VL elements
 * each; src itself is not updated.
 *
 * @override_id asc_loadalign_deintlv__Rvi8_Rvi8_PUi8_i32
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index int8_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index int8_t elements of the 2*VL window (VL
 * elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus offset) must be 32-byte aligned
 * and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Signed offset added to src before the read, counted in int8_t elements rather than bytes; src plus
 * offset has to keep the 32-byte alignment.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_int8_t& dst0, vector_int8_t& dst1, __ubuf__ int8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element uint8_t window read at src plus an element-count offset into two vector data
 * registers, writing the even-index elements to vector_uint8_t dst0 and the odd-index elements to dst1, VL elements
 * each; src itself is not updated.
 *
 * @override_id asc_loadalign_deintlv__Rvu8_Rvu8_PUu8_i32
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index uint8_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index uint8_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus offset) must be 32-byte aligned
 * and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Signed offset added to src before the read, counted in uint8_t elements rather than bytes; src
 * plus offset has to keep the 32-byte alignment.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_uint8_t& dst0, vector_uint8_t& dst1, __ubuf__ uint8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element fp4x2_e2m1_t window read at src plus an element-count offset into two vector data
 * registers, writing the even-index elements to vector_fp4x2_e2m1_t dst0 and the odd-index elements to dst1, VL
 * elements each; src itself is not updated.
 *
 * @override_id asc_loadalign_deintlv__Rvf4e2m1x2_Rvf4e2m1x2_PUf4e2m1x2_i32
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index fp4x2_e2m1_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index fp4x2_e2m1_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus offset) must be 32-byte aligned
 * and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Signed offset added to src before the read, counted in fp4x2_e2m1_t elements rather than bytes;
 * src plus offset has to keep the 32-byte alignment.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_fp4x2_e2m1_t& dst0, vector_fp4x2_e2m1_t& dst1, __ubuf__ fp4x2_e2m1_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element fp4x2_e1m2_t window read at src plus an element-count offset into two vector data
 * registers, writing the even-index elements to vector_fp4x2_e1m2_t dst0 and the odd-index elements to dst1, VL
 * elements each; src itself is not updated.
 *
 * @override_id asc_loadalign_deintlv__Rvf4e1m2x2_Rvf4e1m2x2_PUf4e1m2x2_i32
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index fp4x2_e1m2_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index fp4x2_e1m2_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus offset) must be 32-byte aligned
 * and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Signed offset added to src before the read, counted in fp4x2_e1m2_t elements rather than bytes;
 * src plus offset has to keep the 32-byte alignment.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_fp4x2_e1m2_t& dst0, vector_fp4x2_e1m2_t& dst1, __ubuf__ fp4x2_e1m2_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element int4b_t window read at src plus an element-count offset into two vector data
 * registers, writing the even-index elements to vector_int4x2_t dst0 and the odd-index elements to dst1, VL elements
 * each; src itself is not updated.
 *
 * @override_id asc_loadalign_deintlv__Rvi4x2_Rvi4x2_PUi4x2_i32
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index int4b_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index int4b_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus offset) must be 32-byte aligned
 * and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Signed offset added to src before the read, counted in int4b_t elements rather than bytes; src
 * plus offset has to keep the 32-byte alignment.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_int4x2_t& dst0, vector_int4x2_t& dst1, __ubuf__ int4b_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element fp8_e8m0_t window read at src plus an element-count offset into two vector data
 * registers, writing the even-index elements to vector_fp8_e8m0_t dst0 and the odd-index elements to dst1, VL elements
 * each; src itself is not updated.
 *
 * @override_id asc_loadalign_deintlv__Rvf8e8m0_Rvf8e8m0_PUf8e8m0_i32
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index fp8_e8m0_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index fp8_e8m0_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus offset) must be 32-byte aligned
 * and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Signed offset added to src before the read, counted in fp8_e8m0_t elements rather than bytes; src
 * plus offset has to keep the 32-byte alignment.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, __ubuf__ fp8_e8m0_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element fp8_e5m2_t window read at src plus an element-count offset into two vector data
 * registers, writing the even-index elements to vector_fp8_e5m2_t dst0 and the odd-index elements to dst1, VL elements
 * each; src itself is not updated.
 *
 * @override_id asc_loadalign_deintlv__Rvf8e5m2_Rvf8e5m2_PUf8e5m2_i32
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index fp8_e5m2_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index fp8_e5m2_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus offset) must be 32-byte aligned
 * and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Signed offset added to src before the read, counted in fp8_e5m2_t elements rather than bytes; src
 * plus offset has to keep the 32-byte alignment.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, __ubuf__ fp8_e5m2_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element fp8_e4m3fn_t window read at src plus an element-count offset into two vector data
 * registers, writing the even-index elements to vector_fp8_e4m3fn_t dst0 and the odd-index elements to dst1, VL
 * elements each; src itself is not updated.
 *
 * @override_id asc_loadalign_deintlv__Rvf8e4m3_Rvf8e4m3_PUf8e4m3_i32
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index fp8_e4m3fn_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index fp8_e4m3fn_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus offset) must be 32-byte aligned
 * and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Signed offset added to src before the read, counted in fp8_e4m3fn_t elements rather than bytes;
 * src plus offset has to keep the 32-byte alignment.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, __ubuf__ fp8_e4m3fn_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element hifloat8_t window read at src plus an element-count offset into two vector data
 * registers, writing the even-index elements to vector_hifloat8_t dst0 and the odd-index elements to dst1, VL elements
 * each; src itself is not updated.
 *
 * @override_id asc_loadalign_deintlv__Rvhf8_Rvhf8_PUhf8_i32
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index hifloat8_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index hifloat8_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus offset) must be 32-byte aligned
 * and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Signed offset added to src before the read, counted in hifloat8_t elements rather than bytes; src
 * plus offset has to keep the 32-byte alignment.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, __ubuf__ hifloat8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element int16_t window read at src plus an element-count offset into two vector data
 * registers, writing the even-index elements to vector_int16_t dst0 and the odd-index elements to dst1, VL elements
 * each; src itself is not updated.
 *
 * @override_id asc_loadalign_deintlv__Rvi16_Rvi16_PUi16_i32
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index int16_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index int16_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus offset) must be 32-byte aligned
 * and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Signed offset added to src before the read, counted in int16_t elements rather than bytes; src
 * plus offset has to keep the 32-byte alignment.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_int16_t& dst0, vector_int16_t& dst1, __ubuf__ int16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element uint16_t window read at src plus an element-count offset into two vector data
 * registers, writing the even-index elements to vector_uint16_t dst0 and the odd-index elements to dst1, VL elements
 * each; src itself is not updated.
 *
 * @override_id asc_loadalign_deintlv__Rvu16_Rvu16_PUu16_i32
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index uint16_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index uint16_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus offset) must be 32-byte aligned
 * and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Signed offset added to src before the read, counted in uint16_t elements rather than bytes; src
 * plus offset has to keep the 32-byte alignment.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_uint16_t& dst0, vector_uint16_t& dst1, __ubuf__ uint16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element half window read at src plus an element-count offset into two vector data
 * registers, writing the even-index elements to vector_half dst0 and the odd-index elements to dst1, VL elements each;
 * src itself is not updated.
 *
 * @override_id asc_loadalign_deintlv__Rvf16_Rvf16_PUf16_i32
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index half elements of the 2*VL window (VL
 * elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index half elements of the 2*VL window (VL
 * elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus offset) must be 32-byte aligned
 * and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Signed offset added to src before the read, counted in half elements rather than bytes; src plus
 * offset has to keep the 32-byte alignment.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_half& dst0, vector_half& dst1, __ubuf__ half* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element bfloat16_t window read at src plus an element-count offset into two vector data
 * registers, writing the even-index elements to vector_bfloat16_t dst0 and the odd-index elements to dst1, VL elements
 * each; src itself is not updated.
 *
 * @override_id asc_loadalign_deintlv__Rvbf16_Rvbf16_PUbf16_i32
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index bfloat16_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index bfloat16_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus offset) must be 32-byte aligned
 * and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Signed offset added to src before the read, counted in bfloat16_t elements rather than bytes; src
 * plus offset has to keep the 32-byte alignment.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, __ubuf__ bfloat16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element int32_t window read at src plus an element-count offset into two vector data
 * registers, writing the even-index elements to vector_int32_t dst0 and the odd-index elements to dst1, VL elements
 * each; src itself is not updated.
 *
 * @override_id asc_loadalign_deintlv__Rvi32_Rvi32_PUi32_i32
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index int32_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index int32_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus offset) must be 32-byte aligned
 * and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Signed offset added to src before the read, counted in int32_t elements rather than bytes; src
 * plus offset has to keep the 32-byte alignment.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_int32_t& dst0, vector_int32_t& dst1, __ubuf__ int32_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element uint32_t window read at src plus an element-count offset into two vector data
 * registers, writing the even-index elements to vector_uint32_t dst0 and the odd-index elements to dst1, VL elements
 * each; src itself is not updated.
 *
 * @override_id asc_loadalign_deintlv__Rvu32_Rvu32_PUu32_i32
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index uint32_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index uint32_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus offset) must be 32-byte aligned
 * and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Signed offset added to src before the read, counted in uint32_t elements rather than bytes; src
 * plus offset has to keep the 32-byte alignment.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_uint32_t& dst0, vector_uint32_t& dst1, __ubuf__ uint32_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element float window read at src plus an element-count offset into two vector data
 * registers, writing the even-index elements to vector_float dst0 and the odd-index elements to dst1, VL elements each;
 * src itself is not updated.
 *
 * @override_id asc_loadalign_deintlv__Rvf32_Rvf32_PUf32_i32
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index float elements of the 2*VL window (VL
 * elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index float elements of the 2*VL window (VL
 * elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus offset) must be 32-byte aligned
 * and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Signed offset added to src before the read, counted in float elements rather than bytes; src plus
 * offset has to keep the 32-byte alignment.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_float& dst0, vector_float& dst1, __ubuf__ float* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL elements of int8_t from the 32-byte-aligned UB address src into dst and then advances src by offset
 * elements.
 *
 * @override_id asc_loadalign_postupdate__Rvi8_RPUi8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) that receives VL int8_t elements read from UB; the
 * element type must match the src element type.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which the hardware
 * post-update advances afterwards.
 * @param [in] offset Post-update step in element counts: after VL int8_t elements are loaded the src pointer increases
 * by offset x sizeof(int8_t) bytes.
 */
__simd_callee__ inline void asc_loadalign_postupdate(vector_int8_t& dst, __ubuf__ int8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL elements of uint8_t from the 32-byte-aligned UB address src into dst and then advances src by offset
 * elements.
 *
 * @override_id asc_loadalign_postupdate__Rvu8_RPUu8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) that receives VL uint8_t elements read from UB;
 * the element type must match the src element type.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which the hardware
 * post-update advances afterwards.
 * @param [in] offset Post-update step in element counts: after VL uint8_t elements are loaded the src pointer increases
 * by offset x sizeof(uint8_t) bytes.
 */
__simd_callee__ inline void asc_loadalign_postupdate(vector_uint8_t& dst, __ubuf__ uint8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL elements of fp4x2_e2m1_t from the 32-byte-aligned UB address src into dst and then advances src by
 * offset elements.
 *
 * @override_id asc_loadalign_postupdate__Rvf4e2m1x2_RPUf4e2m1x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) that receives VL fp4x2_e2m1_t elements read
 * from UB; the element type must match the src element type.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which the hardware
 * post-update advances afterwards.
 * @param [in] offset Post-update step in element counts: after VL fp4x2_e2m1_t elements are loaded the src pointer
 * increases by offset x sizeof(fp4x2_e2m1_t) bytes.
 */
__simd_callee__ inline void asc_loadalign_postupdate(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL elements of fp4x2_e1m2_t from the 32-byte-aligned UB address src into dst and then advances src by
 * offset elements.
 *
 * @override_id asc_loadalign_postupdate__Rvf4e1m2x2_RPUf4e1m2x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) that receives VL fp4x2_e1m2_t elements read
 * from UB; the element type must match the src element type.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which the hardware
 * post-update advances afterwards.
 * @param [in] offset Post-update step in element counts: after VL fp4x2_e1m2_t elements are loaded the src pointer
 * increases by offset x sizeof(fp4x2_e1m2_t) bytes.
 */
__simd_callee__ inline void asc_loadalign_postupdate(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL elements of fp8_e8m0_t from the 32-byte-aligned UB address src into dst and then advances src by
 * offset elements.
 *
 * @override_id asc_loadalign_postupdate__Rvf8e8m0_RPUf8e8m0_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) that receives VL fp8_e8m0_t elements read from
 * UB; the element type must match the src element type.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which the hardware
 * post-update advances afterwards.
 * @param [in] offset Post-update step in element counts: after VL fp8_e8m0_t elements are loaded the src pointer
 * increases by offset x sizeof(fp8_e8m0_t) bytes.
 */
__simd_callee__ inline void asc_loadalign_postupdate(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL elements of int4b_t from the 32-byte-aligned UB address src into dst and then advances src by offset
 * elements.
 *
 * @override_id asc_loadalign_postupdate__Rvi4x2_RPUi4x2_i32
 * @archs 3510
 * @param [out] dst Destination data register (vector_int4x2_t, the real type the doc assigns to dst for dtype int4b_t)
 * that receives the VL int4b_t elements read from UB.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which the hardware
 * post-update advances afterwards.
 * @param [in] offset Post-update step in element counts: after VL int4b_t elements are loaded the src pointer increases
 * by offset x sizeof(int4b_t) bytes.
 */
__simd_callee__ inline void asc_loadalign_postupdate(vector_int4x2_t& dst, __ubuf__ int4b_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL elements of fp8_e5m2_t from the 32-byte-aligned UB address src into dst and then advances src by
 * offset elements.
 *
 * @override_id asc_loadalign_postupdate__Rvf8e5m2_RPUf8e5m2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) that receives VL fp8_e5m2_t elements read from
 * UB; the element type must match the src element type.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which the hardware
 * post-update advances afterwards.
 * @param [in] offset Post-update step in element counts: after VL fp8_e5m2_t elements are loaded the src pointer
 * increases by offset x sizeof(fp8_e5m2_t) bytes.
 */
__simd_callee__ inline void asc_loadalign_postupdate(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL elements of fp8_e4m3fn_t from the 32-byte-aligned UB address src into dst and then advances src by
 * offset elements.
 *
 * @override_id asc_loadalign_postupdate__Rvf8e4m3_RPUf8e4m3_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) that receives VL fp8_e4m3fn_t elements read
 * from UB; the element type must match the src element type.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which the hardware
 * post-update advances afterwards.
 * @param [in] offset Post-update step in element counts: after VL fp8_e4m3fn_t elements are loaded the src pointer
 * increases by offset x sizeof(fp8_e4m3fn_t) bytes.
 */
__simd_callee__ inline void asc_loadalign_postupdate(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL elements of hifloat8_t from the 32-byte-aligned UB address src into dst and then advances src by
 * offset elements.
 *
 * @override_id asc_loadalign_postupdate__Rvhf8_RPUhf8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) that receives VL hifloat8_t elements read from
 * UB; the element type must match the src element type.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which the hardware
 * post-update advances afterwards.
 * @param [in] offset Post-update step in element counts: after VL hifloat8_t elements are loaded the src pointer
 * increases by offset x sizeof(hifloat8_t) bytes.
 */
__simd_callee__ inline void asc_loadalign_postupdate(vector_hifloat8_t& dst, __ubuf__ hifloat8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL elements of int16_t from the 32-byte-aligned UB address src into dst and then advances src by offset
 * elements.
 *
 * @override_id asc_loadalign_postupdate__Rvi16_RPUi16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) that receives VL int16_t elements read from UB;
 * the element type must match the src element type.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which the hardware
 * post-update advances afterwards.
 * @param [in] offset Post-update step in element counts: after VL int16_t elements are loaded the src pointer increases
 * by offset x sizeof(int16_t) bytes.
 */
__simd_callee__ inline void asc_loadalign_postupdate(vector_int16_t& dst, __ubuf__ int16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL elements of uint16_t from the 32-byte-aligned UB address src into dst and then advances src by offset
 * elements.
 *
 * @override_id asc_loadalign_postupdate__Rvu16_RPUu16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) that receives VL uint16_t elements read from UB;
 * the element type must match the src element type.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which the hardware
 * post-update advances afterwards.
 * @param [in] offset Post-update step in element counts: after VL uint16_t elements are loaded the src pointer
 * increases by offset x sizeof(uint16_t) bytes.
 */
__simd_callee__ inline void asc_loadalign_postupdate(vector_uint16_t& dst, __ubuf__ uint16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL elements of half from the 32-byte-aligned UB address src into dst and then advances src by offset
 * elements.
 *
 * @override_id asc_loadalign_postupdate__Rvf16_RPUf16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) that receives VL half elements read from UB; the
 * element type must match the src element type.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which the hardware
 * post-update advances afterwards.
 * @param [in] offset Post-update step in element counts: after VL half elements are loaded the src pointer increases by
 * offset x sizeof(half) bytes.
 */
__simd_callee__ inline void asc_loadalign_postupdate(vector_half& dst, __ubuf__ half*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL elements of bfloat16_t from the 32-byte-aligned UB address src into dst and then advances src by
 * offset elements.
 *
 * @override_id asc_loadalign_postupdate__Rvbf16_RPUbf16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) that receives VL bfloat16_t elements read from
 * UB; the element type must match the src element type.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which the hardware
 * post-update advances afterwards.
 * @param [in] offset Post-update step in element counts: after VL bfloat16_t elements are loaded the src pointer
 * increases by offset x sizeof(bfloat16_t) bytes.
 */
__simd_callee__ inline void asc_loadalign_postupdate(vector_bfloat16_t& dst, __ubuf__ bfloat16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL elements of int32_t from the 32-byte-aligned UB address src into dst and then advances src by offset
 * elements.
 *
 * @override_id asc_loadalign_postupdate__Rvi32_RPUi32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int32_t) that receives VL int32_t elements read from UB;
 * the element type must match the src element type.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which the hardware
 * post-update advances afterwards.
 * @param [in] offset Post-update step in element counts: after VL int32_t elements are loaded the src pointer increases
 * by offset x sizeof(int32_t) bytes.
 */
__simd_callee__ inline void asc_loadalign_postupdate(vector_int32_t& dst, __ubuf__ int32_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL elements of uint32_t from the 32-byte-aligned UB address src into dst and then advances src by offset
 * elements.
 *
 * @override_id asc_loadalign_postupdate__Rvu32_RPUu32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint32_t) that receives VL uint32_t elements read from UB;
 * the element type must match the src element type.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which the hardware
 * post-update advances afterwards.
 * @param [in] offset Post-update step in element counts: after VL uint32_t elements are loaded the src pointer
 * increases by offset x sizeof(uint32_t) bytes.
 */
__simd_callee__ inline void asc_loadalign_postupdate(vector_uint32_t& dst, __ubuf__ uint32_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL elements of float from the 32-byte-aligned UB address src into dst and then advances src by offset
 * elements.
 *
 * @override_id asc_loadalign_postupdate__Rvf32_RPUf32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_float) that receives VL float elements read from UB; the
 * element type must match the src element type.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which the hardware
 * post-update advances afterwards.
 * @param [in] offset Post-update step in element counts: after VL float elements are loaded the src pointer increases
 * by offset x sizeof(float) bytes.
 */
__simd_callee__ inline void asc_loadalign_postupdate(vector_float& dst, __ubuf__ float*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL elements of int64_t from the 32-byte-aligned UB address src into dst and then advances src by offset
 * elements.
 *
 * @override_id asc_loadalign_postupdate__Rvi64_RPUi64_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int64_t) that receives VL int64_t elements read from UB;
 * the element type must match the src element type.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which the hardware
 * post-update advances afterwards.
 * @param [in] offset Post-update step in element counts: after VL int64_t elements are loaded the src pointer increases
 * by offset x sizeof(int64_t) bytes.
 */
__simd_callee__ inline void asc_loadalign_postupdate(vector_int64_t& dst, __ubuf__ int64_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL elements of uint64_t from the 32-byte-aligned UB address src into dst and then advances src by offset
 * elements.
 *
 * @override_id asc_loadalign_postupdate__Rvu64_RPUu64_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint64_t) that receives VL uint64_t elements read from UB;
 * the element type must match the src element type.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which the hardware
 * post-update advances afterwards.
 * @param [in] offset Post-update step in element counts: after VL uint64_t elements are loaded the src pointer
 * increases by offset x sizeof(uint64_t) bytes.
 */
__simd_callee__ inline void asc_loadalign_postupdate(vector_uint64_t& dst, __ubuf__ uint64_t*& src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_int8_t& dst, __ubuf__ int8_t*& src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_uint8_t& dst, __ubuf__ uint8_t*& src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc_postupdate(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc_postupdate(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc_postupdate(
    vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t*& src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_int4x2_t& dst, __ubuf__ int4b_t*& src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc_postupdate(
    vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t*& src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc_postupdate(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc_postupdate(
    vector_hifloat8_t& dst, __ubuf__ hifloat8_t*& src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_int16_t& dst, __ubuf__ int16_t*& src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_uint16_t& dst, __ubuf__ uint16_t*& src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_half& dst, __ubuf__ half*& src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc_postupdate(
    vector_bfloat16_t& dst, __ubuf__ bfloat16_t*& src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_int32_t& dst, __ubuf__ int32_t*& src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_uint32_t& dst, __ubuf__ uint32_t*& src, int32_t offset);

__simd_callee__ inline void asc_loadalign_brc_postupdate(vector_float& dst, __ubuf__ float*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads int8_t data from a 32-byte aligned UB address into vector_int8_t, duplicating each source element, and
 * post-updates src.
 *
 * @override_id asc_loadalign_upsample_postupdate__Rvi8_RPUi8_i32
 * @archs 3510
 * @param [out] dst Destination vector_int8_t register that receives the VL-length 2x upsampled result; the load writes
 * the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ int8_t* that is read at the current address and then advanced
 * in post-update mode by offset elements of int8_t (the pointer advances by offset x sizeof(int8_t) bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(int8_t) bytes, and the
 * next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample_postupdate(
    vector_int8_t& dst, __ubuf__ int8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads uint8_t data from a 32-byte aligned UB address into vector_uint8_t, duplicating each source element, and
 * post-updates src.
 *
 * @override_id asc_loadalign_upsample_postupdate__Rvu8_RPUu8_i32
 * @archs 3510
 * @param [out] dst Destination vector_uint8_t register that receives the VL-length 2x upsampled result; the load writes
 * the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ uint8_t* that is read at the current address and then advanced
 * in post-update mode by offset elements of uint8_t (the pointer advances by offset x sizeof(uint8_t) bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(uint8_t) bytes, and the
 * next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample_postupdate(
    vector_uint8_t& dst, __ubuf__ uint8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads fp4x2_e2m1_t data from a 32-byte aligned UB address into vector_fp4x2_e2m1_t, duplicating each source
 * element, and post-updates src.
 *
 * @override_id asc_loadalign_upsample_postupdate__Rvf4e2m1x2_RPUf4e2m1x2_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e2m1_t register that receives the VL-length 2x upsampled result; the load
 * writes the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ fp4x2_e2m1_t* that is read at the current address and then
 * advanced in post-update mode by offset elements of fp4x2_e2m1_t (the pointer advances by offset x
 * sizeof(fp4x2_e2m1_t) bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(fp4x2_e2m1_t) bytes, and
 * the next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample_postupdate(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads fp4x2_e1m2_t data from a 32-byte aligned UB address into vector_fp4x2_e1m2_t, duplicating each source
 * element, and post-updates src.
 *
 * @override_id asc_loadalign_upsample_postupdate__Rvf4e1m2x2_RPUf4e1m2x2_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e1m2_t register that receives the VL-length 2x upsampled result; the load
 * writes the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ fp4x2_e1m2_t* that is read at the current address and then
 * advanced in post-update mode by offset elements of fp4x2_e1m2_t (the pointer advances by offset x
 * sizeof(fp4x2_e1m2_t) bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(fp4x2_e1m2_t) bytes, and
 * the next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample_postupdate(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads fp8_e8m0_t data from a 32-byte aligned UB address into vector_fp8_e8m0_t, duplicating each source
 * element, and post-updates src.
 *
 * @override_id asc_loadalign_upsample_postupdate__Rvf8e8m0_RPUf8e8m0_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e8m0_t register that receives the VL-length 2x upsampled result; the load
 * writes the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ fp8_e8m0_t* that is read at the current address and then
 * advanced in post-update mode by offset elements of fp8_e8m0_t (the pointer advances by offset x sizeof(fp8_e8m0_t)
 * bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(fp8_e8m0_t) bytes, and
 * the next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample_postupdate(
    vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads int4b_t data from a 32-byte aligned UB address into vector_int4x2_t, duplicating each source element,
 * and post-updates src.
 *
 * @override_id asc_loadalign_upsample_postupdate__Rvi4x2_RPUi4x2_i32
 * @archs 3510
 * @param [out] dst Destination vector_int4x2_t register that receives the VL-length 2x upsampled result; the load
 * writes the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ int4b_t* that is read at the current address and then advanced
 * in post-update mode by offset elements of int4b_t (the pointer advances by offset x sizeof(int4b_t) bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(int4b_t) bytes, and the
 * next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample_postupdate(
    vector_int4x2_t& dst, __ubuf__ int4b_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads fp8_e5m2_t data from a 32-byte aligned UB address into vector_fp8_e5m2_t, duplicating each source
 * element, and post-updates src.
 *
 * @override_id asc_loadalign_upsample_postupdate__Rvf8e5m2_RPUf8e5m2_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e5m2_t register that receives the VL-length 2x upsampled result; the load
 * writes the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ fp8_e5m2_t* that is read at the current address and then
 * advanced in post-update mode by offset elements of fp8_e5m2_t (the pointer advances by offset x sizeof(fp8_e5m2_t)
 * bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(fp8_e5m2_t) bytes, and
 * the next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample_postupdate(
    vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads fp8_e4m3fn_t data from a 32-byte aligned UB address into vector_fp8_e4m3fn_t, duplicating each source
 * element, and post-updates src.
 *
 * @override_id asc_loadalign_upsample_postupdate__Rvf8e4m3_RPUf8e4m3_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e4m3fn_t register that receives the VL-length 2x upsampled result; the load
 * writes the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ fp8_e4m3fn_t* that is read at the current address and then
 * advanced in post-update mode by offset elements of fp8_e4m3fn_t (the pointer advances by offset x
 * sizeof(fp8_e4m3fn_t) bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(fp8_e4m3fn_t) bytes, and
 * the next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample_postupdate(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads hifloat8_t data from a 32-byte aligned UB address into vector_hifloat8_t, duplicating each source
 * element, and post-updates src.
 *
 * @override_id asc_loadalign_upsample_postupdate__Rvhf8_RPUhf8_i32
 * @archs 3510
 * @param [out] dst Destination vector_hifloat8_t register that receives the VL-length 2x upsampled result; the load
 * writes the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ hifloat8_t* that is read at the current address and then
 * advanced in post-update mode by offset elements of hifloat8_t (the pointer advances by offset x sizeof(hifloat8_t)
 * bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(hifloat8_t) bytes, and
 * the next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample_postupdate(
    vector_hifloat8_t& dst, __ubuf__ hifloat8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads int16_t data from a 32-byte aligned UB address into vector_int16_t, duplicating each source element, and
 * post-updates src.
 *
 * @override_id asc_loadalign_upsample_postupdate__Rvi16_RPUi16_i32
 * @archs 3510
 * @param [out] dst Destination vector_int16_t register that receives the VL-length 2x upsampled result; the load writes
 * the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ int16_t* that is read at the current address and then advanced
 * in post-update mode by offset elements of int16_t (the pointer advances by offset x sizeof(int16_t) bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(int16_t) bytes, and the
 * next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample_postupdate(
    vector_int16_t& dst, __ubuf__ int16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads uint16_t data from a 32-byte aligned UB address into vector_uint16_t, duplicating each source element,
 * and post-updates src.
 *
 * @override_id asc_loadalign_upsample_postupdate__Rvu16_RPUu16_i32
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t register that receives the VL-length 2x upsampled result; the load
 * writes the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ uint16_t* that is read at the current address and then
 * advanced in post-update mode by offset elements of uint16_t (the pointer advances by offset x sizeof(uint16_t)
 * bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(uint16_t) bytes, and the
 * next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample_postupdate(
    vector_uint16_t& dst, __ubuf__ uint16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads half data from a 32-byte aligned UB address into vector_half, duplicating each source element, and
 * post-updates src.
 *
 * @override_id asc_loadalign_upsample_postupdate__Rvf16_RPUf16_i32
 * @archs 3510
 * @param [out] dst Destination vector_half register that receives the VL-length 2x upsampled result; the load writes
 * the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ half* that is read at the current address and then advanced in
 * post-update mode by offset elements of half (the pointer advances by offset x sizeof(half) bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(half) bytes, and the
 * next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample_postupdate(vector_half& dst, __ubuf__ half*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads bfloat16_t data from a 32-byte aligned UB address into vector_bfloat16_t, duplicating each source
 * element, and post-updates src.
 *
 * @override_id asc_loadalign_upsample_postupdate__Rvbf16_RPUbf16_i32
 * @archs 3510
 * @param [out] dst Destination vector_bfloat16_t register that receives the VL-length 2x upsampled result; the load
 * writes the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ bfloat16_t* that is read at the current address and then
 * advanced in post-update mode by offset elements of bfloat16_t (the pointer advances by offset x sizeof(bfloat16_t)
 * bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(bfloat16_t) bytes, and
 * the next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample_postupdate(
    vector_bfloat16_t& dst, __ubuf__ bfloat16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads 2x-downsampled int8_t data from a 32-byte aligned UB address into vector_int8_t and post-updates src.
 *
 * @override_id asc_loadalign_downsample_postupdate__Rvi8_RPUi8_i32
 * @archs 3510
 * @param [out] dst Destination vector_int8_t register that receives the VL-length 2x downsampled result; the load
 * writes the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ int8_t* that is read at the current address and then advanced
 * in post-update mode by offset elements of int8_t (the pointer advances by offset x sizeof(int8_t) bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(int8_t) bytes, and the
 * next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample_postupdate(
    vector_int8_t& dst, __ubuf__ int8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads 2x-downsampled uint8_t data from a 32-byte aligned UB address into vector_uint8_t and post-updates src.
 *
 * @override_id asc_loadalign_downsample_postupdate__Rvu8_RPUu8_i32
 * @archs 3510
 * @param [out] dst Destination vector_uint8_t register that receives the VL-length 2x downsampled result; the load
 * writes the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ uint8_t* that is read at the current address and then advanced
 * in post-update mode by offset elements of uint8_t (the pointer advances by offset x sizeof(uint8_t) bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(uint8_t) bytes, and the
 * next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample_postupdate(
    vector_uint8_t& dst, __ubuf__ uint8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads 2x-downsampled fp4x2_e2m1_t data from a 32-byte aligned UB address into vector_fp4x2_e2m1_t and
 * post-updates src.
 *
 * @override_id asc_loadalign_downsample_postupdate__Rvf4e2m1x2_RPUf4e2m1x2_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e2m1_t register that receives the VL-length 2x downsampled result; the load
 * writes the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ fp4x2_e2m1_t* that is read at the current address and then
 * advanced in post-update mode by offset elements of fp4x2_e2m1_t (the pointer advances by offset x
 * sizeof(fp4x2_e2m1_t) bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(fp4x2_e2m1_t) bytes, and
 * the next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample_postupdate(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads 2x-downsampled fp4x2_e1m2_t data from a 32-byte aligned UB address into vector_fp4x2_e1m2_t and
 * post-updates src.
 *
 * @override_id asc_loadalign_downsample_postupdate__Rvf4e1m2x2_RPUf4e1m2x2_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e1m2_t register that receives the VL-length 2x downsampled result; the load
 * writes the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ fp4x2_e1m2_t* that is read at the current address and then
 * advanced in post-update mode by offset elements of fp4x2_e1m2_t (the pointer advances by offset x
 * sizeof(fp4x2_e1m2_t) bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(fp4x2_e1m2_t) bytes, and
 * the next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample_postupdate(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads 2x-downsampled fp8_e8m0_t data from a 32-byte aligned UB address into vector_fp8_e8m0_t and post-updates
 * src.
 *
 * @override_id asc_loadalign_downsample_postupdate__Rvf8e8m0_RPUf8e8m0_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e8m0_t register that receives the VL-length 2x downsampled result; the load
 * writes the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ fp8_e8m0_t* that is read at the current address and then
 * advanced in post-update mode by offset elements of fp8_e8m0_t (the pointer advances by offset x sizeof(fp8_e8m0_t)
 * bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(fp8_e8m0_t) bytes, and
 * the next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample_postupdate(
    vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads 2x-downsampled int4b_t data from a 32-byte aligned UB address into vector_int4x2_t and post-updates src.
 *
 * @override_id asc_loadalign_downsample_postupdate__Rvi4x2_RPUi4x2_i32
 * @archs 3510
 * @param [out] dst Destination vector_int4x2_t register that receives the VL-length 2x downsampled result; the load
 * writes the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ int4b_t* that is read at the current address and then advanced
 * in post-update mode by offset elements of int4b_t (the pointer advances by offset x sizeof(int4b_t) bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(int4b_t) bytes, and the
 * next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample_postupdate(
    vector_int4x2_t& dst, __ubuf__ int4b_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads 2x-downsampled fp8_e5m2_t data from a 32-byte aligned UB address into vector_fp8_e5m2_t and post-updates
 * src.
 *
 * @override_id asc_loadalign_downsample_postupdate__Rvf8e5m2_RPUf8e5m2_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e5m2_t register that receives the VL-length 2x downsampled result; the load
 * writes the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ fp8_e5m2_t* that is read at the current address and then
 * advanced in post-update mode by offset elements of fp8_e5m2_t (the pointer advances by offset x sizeof(fp8_e5m2_t)
 * bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(fp8_e5m2_t) bytes, and
 * the next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample_postupdate(
    vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads 2x-downsampled fp8_e4m3fn_t data from a 32-byte aligned UB address into vector_fp8_e4m3fn_t and
 * post-updates src.
 *
 * @override_id asc_loadalign_downsample_postupdate__Rvf8e4m3_RPUf8e4m3_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e4m3fn_t register that receives the VL-length 2x downsampled result; the load
 * writes the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ fp8_e4m3fn_t* that is read at the current address and then
 * advanced in post-update mode by offset elements of fp8_e4m3fn_t (the pointer advances by offset x
 * sizeof(fp8_e4m3fn_t) bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(fp8_e4m3fn_t) bytes, and
 * the next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample_postupdate(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads 2x-downsampled hifloat8_t data from a 32-byte aligned UB address into vector_hifloat8_t and post-updates
 * src.
 *
 * @override_id asc_loadalign_downsample_postupdate__Rvhf8_RPUhf8_i32
 * @archs 3510
 * @param [out] dst Destination vector_hifloat8_t register that receives the VL-length 2x downsampled result; the load
 * writes the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ hifloat8_t* that is read at the current address and then
 * advanced in post-update mode by offset elements of hifloat8_t (the pointer advances by offset x sizeof(hifloat8_t)
 * bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(hifloat8_t) bytes, and
 * the next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample_postupdate(
    vector_hifloat8_t& dst, __ubuf__ hifloat8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads 2x-downsampled int16_t data from a 32-byte aligned UB address into vector_int16_t and post-updates src.
 *
 * @override_id asc_loadalign_downsample_postupdate__Rvi16_RPUi16_i32
 * @archs 3510
 * @param [out] dst Destination vector_int16_t register that receives the VL-length 2x downsampled result; the load
 * writes the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ int16_t* that is read at the current address and then advanced
 * in post-update mode by offset elements of int16_t (the pointer advances by offset x sizeof(int16_t) bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(int16_t) bytes, and the
 * next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample_postupdate(
    vector_int16_t& dst, __ubuf__ int16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads 2x-downsampled uint16_t data from a 32-byte aligned UB address into vector_uint16_t and post-updates
 * src.
 *
 * @override_id asc_loadalign_downsample_postupdate__Rvu16_RPUu16_i32
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t register that receives the VL-length 2x downsampled result; the load
 * writes the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ uint16_t* that is read at the current address and then
 * advanced in post-update mode by offset elements of uint16_t (the pointer advances by offset x sizeof(uint16_t)
 * bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(uint16_t) bytes, and the
 * next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample_postupdate(
    vector_uint16_t& dst, __ubuf__ uint16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads 2x-downsampled half data from a 32-byte aligned UB address into vector_half and post-updates src.
 *
 * @override_id asc_loadalign_downsample_postupdate__Rvf16_RPUf16_i32
 * @archs 3510
 * @param [out] dst Destination vector_half register that receives the VL-length 2x downsampled result; the load writes
 * the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ half* that is read at the current address and then advanced in
 * post-update mode by offset elements of half (the pointer advances by offset x sizeof(half) bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(half) bytes, and the
 * next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample_postupdate(vector_half& dst, __ubuf__ half*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads 2x-downsampled bfloat16_t data from a 32-byte aligned UB address into vector_bfloat16_t and post-updates
 * src.
 *
 * @override_id asc_loadalign_downsample_postupdate__Rvbf16_RPUbf16_i32
 * @archs 3510
 * @param [out] dst Destination vector_bfloat16_t register that receives the VL-length 2x downsampled result; the load
 * writes the whole register.
 * @param [in,out] src UB source pointer of type __ubuf__ bfloat16_t* that is read at the current address and then
 * advanced in post-update mode by offset elements of bfloat16_t (the pointer advances by offset x sizeof(bfloat16_t)
 * bytes).
 * @param [in] offset Post-update step in elements: the source pointer moves by offset x sizeof(bfloat16_t) bytes, and
 * the next read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample_postupdate(
    vector_bfloat16_t& dst, __ubuf__ bfloat16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 consecutive int8_t elements from the 32-byte aligned UB address src, inserts one zero element after
 * each source element to fill VL elements in dst, then advances src by offset int8_t elements.
 *
 * @override_id asc_loadalign_unpack_postupdate__Rvi8_RPUi8_i32
 * @archs 3510
 * @param [out] dst Destination register (vector_int8_t) filled with the VL-element unpacked pattern in which every one
 * of the VL/2 source elements is followed by one element of value 0.
 * @param [in,out] src Source UB address of the int8_t data; VL/2 consecutive elements are read from an address that
 * must be 32-byte aligned and the pointer is advanced by offset * sizeof(int8_t) bytes afterwards.
 * @param [in] offset Post Update step counted in int8_t elements; after the transfer src grows by offset *
 * sizeof(int8_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate(vector_int8_t& dst, __ubuf__ int8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 consecutive uint8_t elements from the 32-byte aligned UB address src, inserts one zero element
 * after each source element to fill VL elements in dst, then advances src by offset uint8_t elements.
 *
 * @override_id asc_loadalign_unpack_postupdate__Rvu8_RPUu8_i32
 * @archs 3510
 * @param [out] dst Destination register (vector_uint8_t) filled with the VL-element unpacked pattern in which every one
 * of the VL/2 source elements is followed by one element of value 0.
 * @param [in,out] src Source UB address of the uint8_t data; VL/2 consecutive elements are read from an address that
 * must be 32-byte aligned and the pointer is advanced by offset * sizeof(uint8_t) bytes afterwards.
 * @param [in] offset Post Update step counted in uint8_t elements; after the transfer src grows by offset *
 * sizeof(uint8_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate(
    vector_uint8_t& dst, __ubuf__ uint8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 consecutive fp4x2_e2m1_t elements from the 32-byte aligned UB address src, inserts one zero element
 * after each source element to fill VL elements in dst, then advances src by offset fp4x2_e2m1_t elements.
 *
 * @override_id asc_loadalign_unpack_postupdate__Rvf4e2m1x2_RPUf4e2m1x2_i32
 * @archs 3510
 * @param [out] dst Destination register (vector_fp4x2_e2m1_t) filled with the VL-element unpacked pattern in which
 * every one of the VL/2 source elements is followed by one element of value 0.
 * @param [in,out] src Source UB address of the fp4x2_e2m1_t data; VL/2 consecutive elements are read from an address
 * that must be 32-byte aligned and the pointer is advanced by offset * sizeof(fp4x2_e2m1_t) bytes afterwards.
 * @param [in] offset Post Update step counted in fp4x2_e2m1_t elements; after the transfer src grows by offset *
 * sizeof(fp4x2_e2m1_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 consecutive fp4x2_e1m2_t elements from the 32-byte aligned UB address src, inserts one zero element
 * after each source element to fill VL elements in dst, then advances src by offset fp4x2_e1m2_t elements.
 *
 * @override_id asc_loadalign_unpack_postupdate__Rvf4e1m2x2_RPUf4e1m2x2_i32
 * @archs 3510
 * @param [out] dst Destination register (vector_fp4x2_e1m2_t) filled with the VL-element unpacked pattern in which
 * every one of the VL/2 source elements is followed by one element of value 0.
 * @param [in,out] src Source UB address of the fp4x2_e1m2_t data; VL/2 consecutive elements are read from an address
 * that must be 32-byte aligned and the pointer is advanced by offset * sizeof(fp4x2_e1m2_t) bytes afterwards.
 * @param [in] offset Post Update step counted in fp4x2_e1m2_t elements; after the transfer src grows by offset *
 * sizeof(fp4x2_e1m2_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 consecutive fp8_e8m0_t elements from the 32-byte aligned UB address src, inserts one zero element
 * after each source element to fill VL elements in dst, then advances src by offset fp8_e8m0_t elements.
 *
 * @override_id asc_loadalign_unpack_postupdate__Rvf8e8m0_RPUf8e8m0_i32
 * @archs 3510
 * @param [out] dst Destination register (vector_fp8_e8m0_t) filled with the VL-element unpacked pattern in which every
 * one of the VL/2 source elements is followed by one element of value 0.
 * @param [in,out] src Source UB address of the fp8_e8m0_t data; VL/2 consecutive elements are read from an address that
 * must be 32-byte aligned and the pointer is advanced by offset * sizeof(fp8_e8m0_t) bytes afterwards.
 * @param [in] offset Post Update step counted in fp8_e8m0_t elements; after the transfer src grows by offset *
 * sizeof(fp8_e8m0_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate(
    vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 consecutive int4b_t elements from the 32-byte aligned UB address src, inserts one zero element
 * after each source element to fill VL elements in dst, then advances src by offset int4b_t elements.
 *
 * @override_id asc_loadalign_unpack_postupdate__Rvi4x2_RPUi4x2_i32
 * @archs 3510
 * @param [out] dst Destination register (vector_int4x2_t) filled with the VL-element unpacked pattern in which every
 * one of the VL/2 source elements is followed by one element of value 0.
 * @param [in,out] src Source UB address of the int4b_t data; VL/2 consecutive elements are read from an address that
 * must be 32-byte aligned and the pointer is advanced by offset * sizeof(int4b_t) bytes afterwards.
 * @param [in] offset Post Update step counted in int4b_t elements; after the transfer src grows by offset *
 * sizeof(int4b_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate(
    vector_int4x2_t& dst, __ubuf__ int4b_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 consecutive fp8_e5m2_t elements from the 32-byte aligned UB address src, inserts one zero element
 * after each source element to fill VL elements in dst, then advances src by offset fp8_e5m2_t elements.
 *
 * @override_id asc_loadalign_unpack_postupdate__Rvf8e5m2_RPUf8e5m2_i32
 * @archs 3510
 * @param [out] dst Destination register (vector_fp8_e5m2_t) filled with the VL-element unpacked pattern in which every
 * one of the VL/2 source elements is followed by one element of value 0.
 * @param [in,out] src Source UB address of the fp8_e5m2_t data; VL/2 consecutive elements are read from an address that
 * must be 32-byte aligned and the pointer is advanced by offset * sizeof(fp8_e5m2_t) bytes afterwards.
 * @param [in] offset Post Update step counted in fp8_e5m2_t elements; after the transfer src grows by offset *
 * sizeof(fp8_e5m2_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate(
    vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 consecutive fp8_e4m3fn_t elements from the 32-byte aligned UB address src, inserts one zero element
 * after each source element to fill VL elements in dst, then advances src by offset fp8_e4m3fn_t elements.
 *
 * @override_id asc_loadalign_unpack_postupdate__Rvf8e4m3_RPUf8e4m3_i32
 * @archs 3510
 * @param [out] dst Destination register (vector_fp8_e4m3fn_t) filled with the VL-element unpacked pattern in which
 * every one of the VL/2 source elements is followed by one element of value 0.
 * @param [in,out] src Source UB address of the fp8_e4m3fn_t data; VL/2 consecutive elements are read from an address
 * that must be 32-byte aligned and the pointer is advanced by offset * sizeof(fp8_e4m3fn_t) bytes afterwards.
 * @param [in] offset Post Update step counted in fp8_e4m3fn_t elements; after the transfer src grows by offset *
 * sizeof(fp8_e4m3fn_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 consecutive hifloat8_t elements from the 32-byte aligned UB address src, inserts one zero element
 * after each source element to fill VL elements in dst, then advances src by offset hifloat8_t elements.
 *
 * @override_id asc_loadalign_unpack_postupdate__Rvhf8_RPUhf8_i32
 * @archs 3510
 * @param [out] dst Destination register (vector_hifloat8_t) filled with the VL-element unpacked pattern in which every
 * one of the VL/2 source elements is followed by one element of value 0.
 * @param [in,out] src Source UB address of the hifloat8_t data; VL/2 consecutive elements are read from an address that
 * must be 32-byte aligned and the pointer is advanced by offset * sizeof(hifloat8_t) bytes afterwards.
 * @param [in] offset Post Update step counted in hifloat8_t elements; after the transfer src grows by offset *
 * sizeof(hifloat8_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate(
    vector_hifloat8_t& dst, __ubuf__ hifloat8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 consecutive int16_t elements from the 32-byte aligned UB address src, inserts one zero element
 * after each source element to fill VL elements in dst, then advances src by offset int16_t elements.
 *
 * @override_id asc_loadalign_unpack_postupdate__Rvi16_RPUi16_i32
 * @archs 3510
 * @param [out] dst Destination register (vector_int16_t) filled with the VL-element unpacked pattern in which every one
 * of the VL/2 source elements is followed by one element of value 0.
 * @param [in,out] src Source UB address of the int16_t data; VL/2 consecutive elements are read from an address that
 * must be 32-byte aligned and the pointer is advanced by offset * sizeof(int16_t) bytes afterwards.
 * @param [in] offset Post Update step counted in int16_t elements; after the transfer src grows by offset *
 * sizeof(int16_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate(
    vector_int16_t& dst, __ubuf__ int16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 consecutive uint16_t elements from the 32-byte aligned UB address src, inserts one zero element
 * after each source element to fill VL elements in dst, then advances src by offset uint16_t elements.
 *
 * @override_id asc_loadalign_unpack_postupdate__Rvu16_RPUu16_i32
 * @archs 3510
 * @param [out] dst Destination register (vector_uint16_t) filled with the VL-element unpacked pattern in which every
 * one of the VL/2 source elements is followed by one element of value 0.
 * @param [in,out] src Source UB address of the uint16_t data; VL/2 consecutive elements are read from an address that
 * must be 32-byte aligned and the pointer is advanced by offset * sizeof(uint16_t) bytes afterwards.
 * @param [in] offset Post Update step counted in uint16_t elements; after the transfer src grows by offset *
 * sizeof(uint16_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate(
    vector_uint16_t& dst, __ubuf__ uint16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 consecutive half elements from the 32-byte aligned UB address src, inserts one zero element after
 * each source element to fill VL elements in dst, then advances src by offset half elements.
 *
 * @override_id asc_loadalign_unpack_postupdate__Rvf16_RPUf16_i32
 * @archs 3510
 * @param [out] dst Destination register (vector_half) filled with the VL-element unpacked pattern in which every one of
 * the VL/2 source elements is followed by one element of value 0.
 * @param [in,out] src Source UB address of the half data; VL/2 consecutive elements are read from an address that must
 * be 32-byte aligned and the pointer is advanced by offset * sizeof(half) bytes afterwards.
 * @param [in] offset Post Update step counted in half elements; after the transfer src grows by offset * sizeof(half)
 * bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate(vector_half& dst, __ubuf__ half*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 consecutive bfloat16_t elements from the 32-byte aligned UB address src, inserts one zero element
 * after each source element to fill VL elements in dst, then advances src by offset bfloat16_t elements.
 *
 * @override_id asc_loadalign_unpack_postupdate__Rvbf16_RPUbf16_i32
 * @archs 3510
 * @param [out] dst Destination register (vector_bfloat16_t) filled with the VL-element unpacked pattern in which every
 * one of the VL/2 source elements is followed by one element of value 0.
 * @param [in,out] src Source UB address of the bfloat16_t data; VL/2 consecutive elements are read from an address that
 * must be 32-byte aligned and the pointer is advanced by offset * sizeof(bfloat16_t) bytes afterwards.
 * @param [in] offset Post Update step counted in bfloat16_t elements; after the transfer src grows by offset *
 * sizeof(bfloat16_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate(
    vector_bfloat16_t& dst, __ubuf__ bfloat16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 consecutive int32_t elements from the 32-byte aligned UB address src, inserts one zero element
 * after each source element to fill VL elements in dst, then advances src by offset int32_t elements.
 *
 * @override_id asc_loadalign_unpack_postupdate__Rvi32_RPUi32_i32
 * @archs 3510
 * @param [out] dst Destination register (vector_int32_t) filled with the VL-element unpacked pattern in which every one
 * of the VL/2 source elements is followed by one element of value 0.
 * @param [in,out] src Source UB address of the int32_t data; VL/2 consecutive elements are read from an address that
 * must be 32-byte aligned and the pointer is advanced by offset * sizeof(int32_t) bytes afterwards.
 * @param [in] offset Post Update step counted in int32_t elements; after the transfer src grows by offset *
 * sizeof(int32_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate(
    vector_int32_t& dst, __ubuf__ int32_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 consecutive uint32_t elements from the 32-byte aligned UB address src, inserts one zero element
 * after each source element to fill VL elements in dst, then advances src by offset uint32_t elements.
 *
 * @override_id asc_loadalign_unpack_postupdate__Rvu32_RPUu32_i32
 * @archs 3510
 * @param [out] dst Destination register (vector_uint32_t) filled with the VL-element unpacked pattern in which every
 * one of the VL/2 source elements is followed by one element of value 0.
 * @param [in,out] src Source UB address of the uint32_t data; VL/2 consecutive elements are read from an address that
 * must be 32-byte aligned and the pointer is advanced by offset * sizeof(uint32_t) bytes afterwards.
 * @param [in] offset Post Update step counted in uint32_t elements; after the transfer src grows by offset *
 * sizeof(uint32_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate(
    vector_uint32_t& dst, __ubuf__ uint32_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 consecutive float elements from the 32-byte aligned UB address src, inserts one zero element after
 * each source element to fill VL elements in dst, then advances src by offset float elements.
 *
 * @override_id asc_loadalign_unpack_postupdate__Rvf32_RPUf32_i32
 * @archs 3510
 * @param [out] dst Destination register (vector_float) filled with the VL-element unpacked pattern in which every one
 * of the VL/2 source elements is followed by one element of value 0.
 * @param [in,out] src Source UB address of the float data; VL/2 consecutive elements are read from an address that must
 * be 32-byte aligned and the pointer is advanced by offset * sizeof(float) bytes afterwards.
 * @param [in] offset Post Update step counted in float elements; after the transfer src grows by offset * sizeof(float)
 * bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate(vector_float& dst, __ubuf__ float*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL int8_t elements from the 32-byte aligned UB address src, writes the even-index elements into dst0
 * and the odd-index elements into dst1, then advances src by offset int8_t elements.
 *
 * @override_id asc_loadalign_deintlv_postupdate__Rvi8_Rvi8_RPUi8_i32
 * @archs 3510
 * @param [out] dst0 Destination register (vector_int8_t) that receives the even-index elements of the source data, VL
 * elements in total; it must be a different register from dst1.
 * @param [out] dst1 Destination register (vector_int8_t) that receives the odd-index elements of the source data, VL
 * elements in total; it must be a different register from dst0.
 * @param [in,out] src Source UB address of the int8_t data; 2 x VL elements are read from an address that must be
 * 32-byte aligned and the pointer is advanced by offset * sizeof(int8_t) bytes afterwards.
 * @param [in] offset Post Update step counted in int8_t elements; after the transfer src grows by offset *
 * sizeof(int8_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(
    vector_int8_t& dst0, vector_int8_t& dst1, __ubuf__ int8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL uint8_t elements from the 32-byte aligned UB address src, writes the even-index elements into
 * dst0 and the odd-index elements into dst1, then advances src by offset uint8_t elements.
 *
 * @override_id asc_loadalign_deintlv_postupdate__Rvu8_Rvu8_RPUu8_i32
 * @archs 3510
 * @param [out] dst0 Destination register (vector_uint8_t) that receives the even-index elements of the source data, VL
 * elements in total; it must be a different register from dst1.
 * @param [out] dst1 Destination register (vector_uint8_t) that receives the odd-index elements of the source data, VL
 * elements in total; it must be a different register from dst0.
 * @param [in,out] src Source UB address of the uint8_t data; 2 x VL elements are read from an address that must be
 * 32-byte aligned and the pointer is advanced by offset * sizeof(uint8_t) bytes afterwards.
 * @param [in] offset Post Update step counted in uint8_t elements; after the transfer src grows by offset *
 * sizeof(uint8_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(
    vector_uint8_t& dst0, vector_uint8_t& dst1, __ubuf__ uint8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL fp4x2_e2m1_t elements from the 32-byte aligned UB address src, writes the even-index elements
 * into dst0 and the odd-index elements into dst1, then advances src by offset fp4x2_e2m1_t elements.
 *
 * @override_id asc_loadalign_deintlv_postupdate__Rvf4e2m1x2_Rvf4e2m1x2_RPUf4e2m1x2_i32
 * @archs 3510
 * @param [out] dst0 Destination register (vector_fp4x2_e2m1_t) that receives the even-index elements of the source
 * data, VL elements in total; it must be a different register from dst1.
 * @param [out] dst1 Destination register (vector_fp4x2_e2m1_t) that receives the odd-index elements of the source data,
 * VL elements in total; it must be a different register from dst0.
 * @param [in,out] src Source UB address of the fp4x2_e2m1_t data; 2 x VL elements are read from an address that must be
 * 32-byte aligned and the pointer is advanced by offset * sizeof(fp4x2_e2m1_t) bytes afterwards.
 * @param [in] offset Post Update step counted in fp4x2_e2m1_t elements; after the transfer src grows by offset *
 * sizeof(fp4x2_e2m1_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(
    vector_fp4x2_e2m1_t& dst0, vector_fp4x2_e2m1_t& dst1, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL fp4x2_e1m2_t elements from the 32-byte aligned UB address src, writes the even-index elements
 * into dst0 and the odd-index elements into dst1, then advances src by offset fp4x2_e1m2_t elements.
 *
 * @override_id asc_loadalign_deintlv_postupdate__Rvf4e1m2x2_Rvf4e1m2x2_RPUf4e1m2x2_i32
 * @archs 3510
 * @param [out] dst0 Destination register (vector_fp4x2_e1m2_t) that receives the even-index elements of the source
 * data, VL elements in total; it must be a different register from dst1.
 * @param [out] dst1 Destination register (vector_fp4x2_e1m2_t) that receives the odd-index elements of the source data,
 * VL elements in total; it must be a different register from dst0.
 * @param [in,out] src Source UB address of the fp4x2_e1m2_t data; 2 x VL elements are read from an address that must be
 * 32-byte aligned and the pointer is advanced by offset * sizeof(fp4x2_e1m2_t) bytes afterwards.
 * @param [in] offset Post Update step counted in fp4x2_e1m2_t elements; after the transfer src grows by offset *
 * sizeof(fp4x2_e1m2_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(
    vector_fp4x2_e1m2_t& dst0, vector_fp4x2_e1m2_t& dst1, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL fp8_e8m0_t elements from the 32-byte aligned UB address src, writes the even-index elements into
 * dst0 and the odd-index elements into dst1, then advances src by offset fp8_e8m0_t elements.
 *
 * @override_id asc_loadalign_deintlv_postupdate__Rvf8e8m0_Rvf8e8m0_RPUf8e8m0_i32
 * @archs 3510
 * @param [out] dst0 Destination register (vector_fp8_e8m0_t) that receives the even-index elements of the source data,
 * VL elements in total; it must be a different register from dst1.
 * @param [out] dst1 Destination register (vector_fp8_e8m0_t) that receives the odd-index elements of the source data,
 * VL elements in total; it must be a different register from dst0.
 * @param [in,out] src Source UB address of the fp8_e8m0_t data; 2 x VL elements are read from an address that must be
 * 32-byte aligned and the pointer is advanced by offset * sizeof(fp8_e8m0_t) bytes afterwards.
 * @param [in] offset Post Update step counted in fp8_e8m0_t elements; after the transfer src grows by offset *
 * sizeof(fp8_e8m0_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(
    vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, __ubuf__ fp8_e8m0_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL int4b_t elements from the 32-byte aligned UB address src, writes the even-index elements into
 * dst0 and the odd-index elements into dst1, then advances src by offset int4b_t elements.
 *
 * @override_id asc_loadalign_deintlv_postupdate__Rvi4x2_Rvi4x2_RPUi4x2_i32
 * @archs 3510
 * @param [out] dst0 Destination register (vector_int4x2_t) that receives the even-index elements of the source data, VL
 * elements in total; it must be a different register from dst1.
 * @param [out] dst1 Destination register (vector_int4x2_t) that receives the odd-index elements of the source data, VL
 * elements in total; it must be a different register from dst0.
 * @param [in,out] src Source UB address of the int4b_t data; 2 x VL elements are read from an address that must be
 * 32-byte aligned and the pointer is advanced by offset * sizeof(int4b_t) bytes afterwards.
 * @param [in] offset Post Update step counted in int4b_t elements; after the transfer src grows by offset *
 * sizeof(int4b_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(
    vector_int4x2_t& dst0, vector_int4x2_t& dst1, __ubuf__ int4b_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL fp8_e5m2_t elements from the 32-byte aligned UB address src, writes the even-index elements into
 * dst0 and the odd-index elements into dst1, then advances src by offset fp8_e5m2_t elements.
 *
 * @override_id asc_loadalign_deintlv_postupdate__Rvf8e5m2_Rvf8e5m2_RPUf8e5m2_i32
 * @archs 3510
 * @param [out] dst0 Destination register (vector_fp8_e5m2_t) that receives the even-index elements of the source data,
 * VL elements in total; it must be a different register from dst1.
 * @param [out] dst1 Destination register (vector_fp8_e5m2_t) that receives the odd-index elements of the source data,
 * VL elements in total; it must be a different register from dst0.
 * @param [in,out] src Source UB address of the fp8_e5m2_t data; 2 x VL elements are read from an address that must be
 * 32-byte aligned and the pointer is advanced by offset * sizeof(fp8_e5m2_t) bytes afterwards.
 * @param [in] offset Post Update step counted in fp8_e5m2_t elements; after the transfer src grows by offset *
 * sizeof(fp8_e5m2_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(
    vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, __ubuf__ fp8_e5m2_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL fp8_e4m3fn_t elements from the 32-byte aligned UB address src, writes the even-index elements
 * into dst0 and the odd-index elements into dst1, then advances src by offset fp8_e4m3fn_t elements.
 *
 * @override_id asc_loadalign_deintlv_postupdate__Rvf8e4m3_Rvf8e4m3_RPUf8e4m3_i32
 * @archs 3510
 * @param [out] dst0 Destination register (vector_fp8_e4m3fn_t) that receives the even-index elements of the source
 * data, VL elements in total; it must be a different register from dst1.
 * @param [out] dst1 Destination register (vector_fp8_e4m3fn_t) that receives the odd-index elements of the source data,
 * VL elements in total; it must be a different register from dst0.
 * @param [in,out] src Source UB address of the fp8_e4m3fn_t data; 2 x VL elements are read from an address that must be
 * 32-byte aligned and the pointer is advanced by offset * sizeof(fp8_e4m3fn_t) bytes afterwards.
 * @param [in] offset Post Update step counted in fp8_e4m3fn_t elements; after the transfer src grows by offset *
 * sizeof(fp8_e4m3fn_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(
    vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL hifloat8_t elements from the 32-byte aligned UB address src, writes the even-index elements into
 * dst0 and the odd-index elements into dst1, then advances src by offset hifloat8_t elements.
 *
 * @override_id asc_loadalign_deintlv_postupdate__Rvhf8_Rvhf8_RPUhf8_i32
 * @archs 3510
 * @param [out] dst0 Destination register (vector_hifloat8_t) that receives the even-index elements of the source data,
 * VL elements in total; it must be a different register from dst1.
 * @param [out] dst1 Destination register (vector_hifloat8_t) that receives the odd-index elements of the source data,
 * VL elements in total; it must be a different register from dst0.
 * @param [in,out] src Source UB address of the hifloat8_t data; 2 x VL elements are read from an address that must be
 * 32-byte aligned and the pointer is advanced by offset * sizeof(hifloat8_t) bytes afterwards.
 * @param [in] offset Post Update step counted in hifloat8_t elements; after the transfer src grows by offset *
 * sizeof(hifloat8_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(
    vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, __ubuf__ hifloat8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL int16_t elements from the 32-byte aligned UB address src, writes the even-index elements into
 * dst0 and the odd-index elements into dst1, then advances src by offset int16_t elements.
 *
 * @override_id asc_loadalign_deintlv_postupdate__Rvi16_Rvi16_RPUi16_i32
 * @archs 3510
 * @param [out] dst0 Destination register (vector_int16_t) that receives the even-index elements of the source data, VL
 * elements in total; it must be a different register from dst1.
 * @param [out] dst1 Destination register (vector_int16_t) that receives the odd-index elements of the source data, VL
 * elements in total; it must be a different register from dst0.
 * @param [in,out] src Source UB address of the int16_t data; 2 x VL elements are read from an address that must be
 * 32-byte aligned and the pointer is advanced by offset * sizeof(int16_t) bytes afterwards.
 * @param [in] offset Post Update step counted in int16_t elements; after the transfer src grows by offset *
 * sizeof(int16_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(
    vector_int16_t& dst0, vector_int16_t& dst1, __ubuf__ int16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL uint16_t elements from the 32-byte aligned UB address src, writes the even-index elements into
 * dst0 and the odd-index elements into dst1, then advances src by offset uint16_t elements.
 *
 * @override_id asc_loadalign_deintlv_postupdate__Rvu16_Rvu16_RPUu16_i32
 * @archs 3510
 * @param [out] dst0 Destination register (vector_uint16_t) that receives the even-index elements of the source data, VL
 * elements in total; it must be a different register from dst1.
 * @param [out] dst1 Destination register (vector_uint16_t) that receives the odd-index elements of the source data, VL
 * elements in total; it must be a different register from dst0.
 * @param [in,out] src Source UB address of the uint16_t data; 2 x VL elements are read from an address that must be
 * 32-byte aligned and the pointer is advanced by offset * sizeof(uint16_t) bytes afterwards.
 * @param [in] offset Post Update step counted in uint16_t elements; after the transfer src grows by offset *
 * sizeof(uint16_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(
    vector_uint16_t& dst0, vector_uint16_t& dst1, __ubuf__ uint16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL half elements from the 32-byte aligned UB address src, writes the even-index elements into dst0
 * and the odd-index elements into dst1, then advances src by offset half elements.
 *
 * @override_id asc_loadalign_deintlv_postupdate__Rvf16_Rvf16_RPUf16_i32
 * @archs 3510
 * @param [out] dst0 Destination register (vector_half) that receives the even-index elements of the source data, VL
 * elements in total; it must be a different register from dst1.
 * @param [out] dst1 Destination register (vector_half) that receives the odd-index elements of the source data, VL
 * elements in total; it must be a different register from dst0.
 * @param [in,out] src Source UB address of the half data; 2 x VL elements are read from an address that must be 32-byte
 * aligned and the pointer is advanced by offset * sizeof(half) bytes afterwards.
 * @param [in] offset Post Update step counted in half elements; after the transfer src grows by offset * sizeof(half)
 * bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(
    vector_half& dst0, vector_half& dst1, __ubuf__ half*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bfloat16_t elements from the 32-byte aligned UB address src, writes the even-index elements into
 * dst0 and the odd-index elements into dst1, then advances src by offset bfloat16_t elements.
 *
 * @override_id asc_loadalign_deintlv_postupdate__Rvbf16_Rvbf16_RPUbf16_i32
 * @archs 3510
 * @param [out] dst0 Destination register (vector_bfloat16_t) that receives the even-index elements of the source data,
 * VL elements in total; it must be a different register from dst1.
 * @param [out] dst1 Destination register (vector_bfloat16_t) that receives the odd-index elements of the source data,
 * VL elements in total; it must be a different register from dst0.
 * @param [in,out] src Source UB address of the bfloat16_t data; 2 x VL elements are read from an address that must be
 * 32-byte aligned and the pointer is advanced by offset * sizeof(bfloat16_t) bytes afterwards.
 * @param [in] offset Post Update step counted in bfloat16_t elements; after the transfer src grows by offset *
 * sizeof(bfloat16_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(
    vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, __ubuf__ bfloat16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL int32_t elements from the 32-byte aligned UB address src, writes the even-index elements into
 * dst0 and the odd-index elements into dst1, then advances src by offset int32_t elements.
 *
 * @override_id asc_loadalign_deintlv_postupdate__Rvi32_Rvi32_RPUi32_i32
 * @archs 3510
 * @param [out] dst0 Destination register (vector_int32_t) that receives the even-index elements of the source data, VL
 * elements in total; it must be a different register from dst1.
 * @param [out] dst1 Destination register (vector_int32_t) that receives the odd-index elements of the source data, VL
 * elements in total; it must be a different register from dst0.
 * @param [in,out] src Source UB address of the int32_t data; 2 x VL elements are read from an address that must be
 * 32-byte aligned and the pointer is advanced by offset * sizeof(int32_t) bytes afterwards.
 * @param [in] offset Post Update step counted in int32_t elements; after the transfer src grows by offset *
 * sizeof(int32_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(
    vector_int32_t& dst0, vector_int32_t& dst1, __ubuf__ int32_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL uint32_t elements from the 32-byte aligned UB address src, writes the even-index elements into
 * dst0 and the odd-index elements into dst1, then advances src by offset uint32_t elements.
 *
 * @override_id asc_loadalign_deintlv_postupdate__Rvu32_Rvu32_RPUu32_i32
 * @archs 3510
 * @param [out] dst0 Destination register (vector_uint32_t) that receives the even-index elements of the source data, VL
 * elements in total; it must be a different register from dst1.
 * @param [out] dst1 Destination register (vector_uint32_t) that receives the odd-index elements of the source data, VL
 * elements in total; it must be a different register from dst0.
 * @param [in,out] src Source UB address of the uint32_t data; 2 x VL elements are read from an address that must be
 * 32-byte aligned and the pointer is advanced by offset * sizeof(uint32_t) bytes afterwards.
 * @param [in] offset Post Update step counted in uint32_t elements; after the transfer src grows by offset *
 * sizeof(uint32_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(
    vector_uint32_t& dst0, vector_uint32_t& dst1, __ubuf__ uint32_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL float elements from the 32-byte aligned UB address src, writes the even-index elements into dst0
 * and the odd-index elements into dst1, then advances src by offset float elements.
 *
 * @override_id asc_loadalign_deintlv_postupdate__Rvf32_Rvf32_RPUf32_i32
 * @archs 3510
 * @param [out] dst0 Destination register (vector_float) that receives the even-index elements of the source data, VL
 * elements in total; it must be a different register from dst1.
 * @param [out] dst1 Destination register (vector_float) that receives the odd-index elements of the source data, VL
 * elements in total; it must be a different register from dst0.
 * @param [in,out] src Source UB address of the float data; 2 x VL elements are read from an address that must be
 * 32-byte aligned and the pointer is advanced by offset * sizeof(float) bytes afterwards.
 * @param [in] offset Post Update step counted in float elements; after the transfer src grows by offset * sizeof(float)
 * bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_deintlv_postupdate(
    vector_float& dst0, vector_float& dst1, __ubuf__ float*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks of int8_t data from src into the vector_int8_t register dst, stepping
 * block_stride DataBlocks between adjacent blocks and starting repeat_stride DataBlocks past src, keeping only the
 * DataBlocks that mask selects.
 *
 * @override_id asc_loadalign__Rvi8_PUi8_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector_int8_t register that receives the selected 32-byte DataBlocks; its dtype must
 * match src and a mask register is not allowed in this mode.
 * @param [in] src Source UB address of the int8_t data; the effective read address must be 32-byte aligned.
 * @param [in] block_stride Start-address stride between adjacent source DataBlocks, expressed in units of 32 bytes.
 * @param [in] repeat_stride Offset of the first DataBlock from src, expressed in units of 32 bytes, so the first read
 * address is src + repeat_stride * 32 bytes.
 * @param [in] mask Mask register selecting which DataBlocks are loaded: a block is loaded whole when any of its
 * elements is enabled, and is skipped and zero-filled without an out-of-bounds fault when all of its elements are
 * disabled.
 */
__simd_callee__ inline void asc_loadalign(
    vector_int8_t& dst, __ubuf__ int8_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks of uint8_t data from src into the vector_uint8_t register dst, stepping
 * block_stride DataBlocks between adjacent blocks and starting repeat_stride DataBlocks past src, keeping only the
 * DataBlocks that mask selects.
 *
 * @override_id asc_loadalign__Rvu8_PUu8_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint8_t register that receives the selected 32-byte DataBlocks; its dtype must
 * match src and a mask register is not allowed in this mode.
 * @param [in] src Source UB address of the uint8_t data; the effective read address must be 32-byte aligned.
 * @param [in] block_stride Start-address stride between adjacent source DataBlocks, expressed in units of 32 bytes.
 * @param [in] repeat_stride Offset of the first DataBlock from src, expressed in units of 32 bytes, so the first read
 * address is src + repeat_stride * 32 bytes.
 * @param [in] mask Mask register selecting which DataBlocks are loaded: a block is loaded whole when any of its
 * elements is enabled, and is skipped and zero-filled without an out-of-bounds fault when all of its elements are
 * disabled.
 */
__simd_callee__ inline void asc_loadalign(
    vector_uint8_t& dst, __ubuf__ uint8_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks of int16_t data from src into the vector_int16_t register dst, stepping
 * block_stride DataBlocks between adjacent blocks and starting repeat_stride DataBlocks past src, keeping only the
 * DataBlocks that mask selects.
 *
 * @override_id asc_loadalign__Rvi16_PUi16_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector_int16_t register that receives the selected 32-byte DataBlocks; its dtype must
 * match src and a mask register is not allowed in this mode.
 * @param [in] src Source UB address of the int16_t data; the effective read address must be 32-byte aligned.
 * @param [in] block_stride Start-address stride between adjacent source DataBlocks, expressed in units of 32 bytes.
 * @param [in] repeat_stride Offset of the first DataBlock from src, expressed in units of 32 bytes, so the first read
 * address is src + repeat_stride * 32 bytes.
 * @param [in] mask Mask register selecting which DataBlocks are loaded: a block is loaded whole when any of its
 * elements is enabled, and is skipped and zero-filled without an out-of-bounds fault when all of its elements are
 * disabled.
 */
__simd_callee__ inline void asc_loadalign(
    vector_int16_t& dst, __ubuf__ int16_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks of uint16_t data from src into the vector_uint16_t register dst, stepping
 * block_stride DataBlocks between adjacent blocks and starting repeat_stride DataBlocks past src, keeping only the
 * DataBlocks that mask selects.
 *
 * @override_id asc_loadalign__Rvu16_PUu16_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t register that receives the selected 32-byte DataBlocks; its dtype must
 * match src and a mask register is not allowed in this mode.
 * @param [in] src Source UB address of the uint16_t data; the effective read address must be 32-byte aligned.
 * @param [in] block_stride Start-address stride between adjacent source DataBlocks, expressed in units of 32 bytes.
 * @param [in] repeat_stride Offset of the first DataBlock from src, expressed in units of 32 bytes, so the first read
 * address is src + repeat_stride * 32 bytes.
 * @param [in] mask Mask register selecting which DataBlocks are loaded: a block is loaded whole when any of its
 * elements is enabled, and is skipped and zero-filled without an out-of-bounds fault when all of its elements are
 * disabled.
 */
__simd_callee__ inline void asc_loadalign(
    vector_uint16_t& dst, __ubuf__ uint16_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks of int32_t data from src into the vector_int32_t register dst, stepping
 * block_stride DataBlocks between adjacent blocks and starting repeat_stride DataBlocks past src, keeping only the
 * DataBlocks that mask selects.
 *
 * @override_id asc_loadalign__Rvi32_PUi32_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector_int32_t register that receives the selected 32-byte DataBlocks; its dtype must
 * match src and a mask register is not allowed in this mode.
 * @param [in] src Source UB address of the int32_t data; the effective read address must be 32-byte aligned.
 * @param [in] block_stride Start-address stride between adjacent source DataBlocks, expressed in units of 32 bytes.
 * @param [in] repeat_stride Offset of the first DataBlock from src, expressed in units of 32 bytes, so the first read
 * address is src + repeat_stride * 32 bytes.
 * @param [in] mask Mask register selecting which DataBlocks are loaded: a block is loaded whole when any of its
 * elements is enabled, and is skipped and zero-filled without an out-of-bounds fault when all of its elements are
 * disabled.
 */
__simd_callee__ inline void asc_loadalign(
    vector_int32_t& dst, __ubuf__ int32_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks of uint32_t data from src into the vector_uint32_t register dst, stepping
 * block_stride DataBlocks between adjacent blocks and starting repeat_stride DataBlocks past src, keeping only the
 * DataBlocks that mask selects.
 *
 * @override_id asc_loadalign__Rvu32_PUu32_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector_uint32_t register that receives the selected 32-byte DataBlocks; its dtype must
 * match src and a mask register is not allowed in this mode.
 * @param [in] src Source UB address of the uint32_t data; the effective read address must be 32-byte aligned.
 * @param [in] block_stride Start-address stride between adjacent source DataBlocks, expressed in units of 32 bytes.
 * @param [in] repeat_stride Offset of the first DataBlock from src, expressed in units of 32 bytes, so the first read
 * address is src + repeat_stride * 32 bytes.
 * @param [in] mask Mask register selecting which DataBlocks are loaded: a block is loaded whole when any of its
 * elements is enabled, and is skipped and zero-filled without an out-of-bounds fault when all of its elements are
 * disabled.
 */
__simd_callee__ inline void asc_loadalign(
    vector_uint32_t& dst, __ubuf__ uint32_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks of int64_t data from src into the vector_int64_t register dst, stepping
 * block_stride DataBlocks between adjacent blocks and starting repeat_stride DataBlocks past src, keeping only the
 * DataBlocks that mask selects.
 *
 * @override_id asc_loadalign__Rvi64_PUi64_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector_int64_t register that receives the selected 32-byte DataBlocks; its dtype must
 * match src and a mask register is not allowed in this mode.
 * @param [in] src Source UB address of the int64_t data; the effective read address must be 32-byte aligned.
 * @param [in] block_stride Start-address stride between adjacent source DataBlocks, expressed in units of 32 bytes.
 * @param [in] repeat_stride Offset of the first DataBlock from src, expressed in units of 32 bytes, so the first read
 * address is src + repeat_stride * 32 bytes.
 * @param [in] mask Mask register selecting which DataBlocks are loaded: a block is loaded whole when any of its
 * elements is enabled, and is skipped and zero-filled without an out-of-bounds fault when all of its elements are
 * disabled.
 */
__simd_callee__ inline void asc_loadalign(
    vector_int64_t& dst, __ubuf__ int64_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks of bfloat16_t data from src into the vector_bfloat16_t register dst, stepping
 * block_stride DataBlocks between adjacent blocks and starting repeat_stride DataBlocks past src, keeping only the
 * DataBlocks that mask selects.
 *
 * @override_id asc_loadalign__Rvbf16_PUbf16_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector_bfloat16_t register that receives the selected 32-byte DataBlocks; its dtype must
 * match src and a mask register is not allowed in this mode.
 * @param [in] src Source UB address of the bfloat16_t data; the effective read address must be 32-byte aligned.
 * @param [in] block_stride Start-address stride between adjacent source DataBlocks, expressed in units of 32 bytes.
 * @param [in] repeat_stride Offset of the first DataBlock from src, expressed in units of 32 bytes, so the first read
 * address is src + repeat_stride * 32 bytes.
 * @param [in] mask Mask register selecting which DataBlocks are loaded: a block is loaded whole when any of its
 * elements is enabled, and is skipped and zero-filled without an out-of-bounds fault when all of its elements are
 * disabled.
 */
__simd_callee__ inline void asc_loadalign(
    vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks of fp8_e4m3fn_t data from src into the vector_fp8_e4m3fn_t register dst,
 * stepping block_stride DataBlocks between adjacent blocks and starting repeat_stride DataBlocks past src, keeping only
 * the DataBlocks that mask selects.
 *
 * @override_id asc_loadalign__Rvf8e4m3_PUf8e4m3_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e4m3fn_t register that receives the selected 32-byte DataBlocks; its dtype
 * must match src and a mask register is not allowed in this mode.
 * @param [in] src Source UB address of the fp8_e4m3fn_t data; the effective read address must be 32-byte aligned.
 * @param [in] block_stride Start-address stride between adjacent source DataBlocks, expressed in units of 32 bytes.
 * @param [in] repeat_stride Offset of the first DataBlock from src, expressed in units of 32 bytes, so the first read
 * address is src + repeat_stride * 32 bytes.
 * @param [in] mask Mask register selecting which DataBlocks are loaded: a block is loaded whole when any of its
 * elements is enabled, and is skipped and zero-filled without an out-of-bounds fault when all of its elements are
 * disabled.
 */
__simd_callee__ inline void asc_loadalign(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, uint16_t block_stride, uint16_t repeat_stride,
    vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks of hifloat8_t data from src into the vector_hifloat8_t register dst, stepping
 * block_stride DataBlocks between adjacent blocks and starting repeat_stride DataBlocks past src, keeping only the
 * DataBlocks that mask selects.
 *
 * @override_id asc_loadalign__Rvhf8_PUhf8_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector_hifloat8_t register that receives the selected 32-byte DataBlocks; its dtype must
 * match src and a mask register is not allowed in this mode.
 * @param [in] src Source UB address of the hifloat8_t data; the effective read address must be 32-byte aligned.
 * @param [in] block_stride Start-address stride between adjacent source DataBlocks, expressed in units of 32 bytes.
 * @param [in] repeat_stride Offset of the first DataBlock from src, expressed in units of 32 bytes, so the first read
 * address is src + repeat_stride * 32 bytes.
 * @param [in] mask Mask register selecting which DataBlocks are loaded: a block is loaded whole when any of its
 * elements is enabled, and is skipped and zero-filled without an out-of-bounds fault when all of its elements are
 * disabled.
 */
__simd_callee__ inline void asc_loadalign(
    vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks of fp8_e5m2_t data from src into the vector_fp8_e5m2_t register dst, stepping
 * block_stride DataBlocks between adjacent blocks and starting repeat_stride DataBlocks past src, keeping only the
 * DataBlocks that mask selects.
 *
 * @override_id asc_loadalign__Rvf8e5m2_PUf8e5m2_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e5m2_t register that receives the selected 32-byte DataBlocks; its dtype must
 * match src and a mask register is not allowed in this mode.
 * @param [in] src Source UB address of the fp8_e5m2_t data; the effective read address must be 32-byte aligned.
 * @param [in] block_stride Start-address stride between adjacent source DataBlocks, expressed in units of 32 bytes.
 * @param [in] repeat_stride Offset of the first DataBlock from src, expressed in units of 32 bytes, so the first read
 * address is src + repeat_stride * 32 bytes.
 * @param [in] mask Mask register selecting which DataBlocks are loaded: a block is loaded whole when any of its
 * elements is enabled, and is skipped and zero-filled without an out-of-bounds fault when all of its elements are
 * disabled.
 */
__simd_callee__ inline void asc_loadalign(
    vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks of fp8_e8m0_t data from src into the vector_fp8_e8m0_t register dst, stepping
 * block_stride DataBlocks between adjacent blocks and starting repeat_stride DataBlocks past src, keeping only the
 * DataBlocks that mask selects.
 *
 * @override_id asc_loadalign__Rvf8e8m0_PUf8e8m0_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e8m0_t register that receives the selected 32-byte DataBlocks; its dtype must
 * match src and a mask register is not allowed in this mode.
 * @param [in] src Source UB address of the fp8_e8m0_t data; the effective read address must be 32-byte aligned.
 * @param [in] block_stride Start-address stride between adjacent source DataBlocks, expressed in units of 32 bytes.
 * @param [in] repeat_stride Offset of the first DataBlock from src, expressed in units of 32 bytes, so the first read
 * address is src + repeat_stride * 32 bytes.
 * @param [in] mask Mask register selecting which DataBlocks are loaded: a block is loaded whole when any of its
 * elements is enabled, and is skipped and zero-filled without an out-of-bounds fault when all of its elements are
 * disabled.
 */
__simd_callee__ inline void asc_loadalign(
    vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks of fp4x2_e1m2_t data from src into the vector_fp4x2_e1m2_t register dst,
 * stepping block_stride DataBlocks between adjacent blocks and starting repeat_stride DataBlocks past src, keeping only
 * the DataBlocks that mask selects.
 *
 * @override_id asc_loadalign__Rvf4e1m2x2_PUf4e1m2x2_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e1m2_t register that receives the selected 32-byte DataBlocks; its dtype
 * must match src and a mask register is not allowed in this mode.
 * @param [in] src Source UB address of the fp4x2_e1m2_t data; the effective read address must be 32-byte aligned.
 * @param [in] block_stride Start-address stride between adjacent source DataBlocks, expressed in units of 32 bytes.
 * @param [in] repeat_stride Offset of the first DataBlock from src, expressed in units of 32 bytes, so the first read
 * address is src + repeat_stride * 32 bytes.
 * @param [in] mask Mask register selecting which DataBlocks are loaded: a block is loaded whole when any of its
 * elements is enabled, and is skipped and zero-filled without an out-of-bounds fault when all of its elements are
 * disabled.
 */
__simd_callee__ inline void asc_loadalign(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, uint16_t block_stride, uint16_t repeat_stride,
    vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks of fp4x2_e2m1_t data from src into the vector_fp4x2_e2m1_t register dst,
 * stepping block_stride DataBlocks between adjacent blocks and starting repeat_stride DataBlocks past src, keeping only
 * the DataBlocks that mask selects.
 *
 * @override_id asc_loadalign__Rvf4e2m1x2_PUf4e2m1x2_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e2m1_t register that receives the selected 32-byte DataBlocks; its dtype
 * must match src and a mask register is not allowed in this mode.
 * @param [in] src Source UB address of the fp4x2_e2m1_t data; the effective read address must be 32-byte aligned.
 * @param [in] block_stride Start-address stride between adjacent source DataBlocks, expressed in units of 32 bytes.
 * @param [in] repeat_stride Offset of the first DataBlock from src, expressed in units of 32 bytes, so the first read
 * address is src + repeat_stride * 32 bytes.
 * @param [in] mask Mask register selecting which DataBlocks are loaded: a block is loaded whole when any of its
 * elements is enabled, and is skipped and zero-filled without an out-of-bounds fault when all of its elements are
 * disabled.
 */
__simd_callee__ inline void asc_loadalign(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, uint16_t block_stride, uint16_t repeat_stride,
    vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks of int4b_t data from src into the vector_int4x2_t register dst, stepping
 * block_stride DataBlocks between adjacent blocks and starting repeat_stride DataBlocks past src, keeping only the
 * DataBlocks that mask selects.
 *
 * @override_id asc_loadalign__Rvi4x2_PUi4x2_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector_int4x2_t register that receives the selected 32-byte DataBlocks; its dtype must
 * match src and a mask register is not allowed in this mode.
 * @param [in] src Source UB address of the int4b_t data; the effective read address must be 32-byte aligned.
 * @param [in] block_stride Start-address stride between adjacent source DataBlocks, expressed in units of 32 bytes.
 * @param [in] repeat_stride Offset of the first DataBlock from src, expressed in units of 32 bytes, so the first read
 * address is src + repeat_stride * 32 bytes.
 * @param [in] mask Mask register selecting which DataBlocks are loaded: a block is loaded whole when any of its
 * elements is enabled, and is skipped and zero-filled without an out-of-bounds fault when all of its elements are
 * disabled.
 */
__simd_callee__ inline void asc_loadalign(
    vector_int4x2_t& dst, __ubuf__ int4b_t* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks of half data from src into the vector_half register dst, stepping block_stride
 * DataBlocks between adjacent blocks and starting repeat_stride DataBlocks past src, keeping only the DataBlocks that
 * mask selects.
 *
 * @override_id asc_loadalign__Rvf16_PUf16_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector_half register that receives the selected 32-byte DataBlocks; its dtype must match
 * src and a mask register is not allowed in this mode.
 * @param [in] src Source UB address of the half data; the effective read address must be 32-byte aligned.
 * @param [in] block_stride Start-address stride between adjacent source DataBlocks, expressed in units of 32 bytes.
 * @param [in] repeat_stride Offset of the first DataBlock from src, expressed in units of 32 bytes, so the first read
 * address is src + repeat_stride * 32 bytes.
 * @param [in] mask Mask register selecting which DataBlocks are loaded: a block is loaded whole when any of its
 * elements is enabled, and is skipped and zero-filled without an out-of-bounds fault when all of its elements are
 * disabled.
 */
__simd_callee__ inline void asc_loadalign(
    vector_half& dst, __ubuf__ half* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks of float data from src into the vector_float register dst, stepping
 * block_stride DataBlocks between adjacent blocks and starting repeat_stride DataBlocks past src, keeping only the
 * DataBlocks that mask selects.
 *
 * @override_id asc_loadalign__Rvf32_PUf32_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector_float register that receives the selected 32-byte DataBlocks; its dtype must
 * match src and a mask register is not allowed in this mode.
 * @param [in] src Source UB address of the float data; the effective read address must be 32-byte aligned.
 * @param [in] block_stride Start-address stride between adjacent source DataBlocks, expressed in units of 32 bytes.
 * @param [in] repeat_stride Offset of the first DataBlock from src, expressed in units of 32 bytes, so the first read
 * address is src + repeat_stride * 32 bytes.
 * @param [in] mask Mask register selecting which DataBlocks are loaded: a block is loaded whole when any of its
 * elements is enabled, and is skipped and zero-filled without an out-of-bounds fault when all of its elements are
 * disabled.
 */
__simd_callee__ inline void asc_loadalign(
    vector_float& dst, __ubuf__ float* src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks spaced block_stride apart from src into dst and then advances src by
 * repeat_stride x 32 bytes.
 *
 * @override_id asc_loadalign_postupdate__Rvi8_RPUi8_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) that receives the VL int8_t elements assembled from
 * the selected DataBlocks; a mask register is not allowed in this mode.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which is advanced by
 * repeat_stride x 32 bytes after the eight DataBlocks are transferred.
 * @param [in] block_stride Distance in 32-byte units between the start addresses of two adjacent DataBlocks in the
 * source (uint16_t, 0-65535).
 * @param [in] repeat_stride Post-update step in 32-byte units: src increases by repeat_stride x 32 bytes once this
 * non-contiguous transfer finishes (uint16_t, 0-65535).
 * @param [in] mask Mask register evaluated per DataBlock: a DataBlock is transferred when any of its elements is
 * enabled, otherwise its destination elements are set to 0 and even an out-of-range DataBlock raises no error.
 */
__simd_callee__ inline void asc_loadalign_postupdate(
    vector_int8_t& dst, __ubuf__ int8_t*& src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks spaced block_stride apart from src into dst and then advances src by
 * repeat_stride x 32 bytes.
 *
 * @override_id asc_loadalign_postupdate__Rvu8_RPUu8_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) that receives the VL uint8_t elements assembled
 * from the selected DataBlocks; a mask register is not allowed in this mode.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which is advanced by
 * repeat_stride x 32 bytes after the eight DataBlocks are transferred.
 * @param [in] block_stride Distance in 32-byte units between the start addresses of two adjacent DataBlocks in the
 * source (uint16_t, 0-65535).
 * @param [in] repeat_stride Post-update step in 32-byte units: src increases by repeat_stride x 32 bytes once this
 * non-contiguous transfer finishes (uint16_t, 0-65535).
 * @param [in] mask Mask register evaluated per DataBlock: a DataBlock is transferred when any of its elements is
 * enabled, otherwise its destination elements are set to 0 and even an out-of-range DataBlock raises no error.
 */
__simd_callee__ inline void asc_loadalign_postupdate(
    vector_uint8_t& dst, __ubuf__ uint8_t*& src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks spaced block_stride apart from src into dst and then advances src by
 * repeat_stride x 32 bytes.
 *
 * @override_id asc_loadalign_postupdate__Rvi16_RPUi16_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) that receives the VL int16_t elements assembled
 * from the selected DataBlocks; a mask register is not allowed in this mode.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which is advanced by
 * repeat_stride x 32 bytes after the eight DataBlocks are transferred.
 * @param [in] block_stride Distance in 32-byte units between the start addresses of two adjacent DataBlocks in the
 * source (uint16_t, 0-65535).
 * @param [in] repeat_stride Post-update step in 32-byte units: src increases by repeat_stride x 32 bytes once this
 * non-contiguous transfer finishes (uint16_t, 0-65535).
 * @param [in] mask Mask register evaluated per DataBlock: a DataBlock is transferred when any of its elements is
 * enabled, otherwise its destination elements are set to 0 and even an out-of-range DataBlock raises no error.
 */
__simd_callee__ inline void asc_loadalign_postupdate(
    vector_int16_t& dst, __ubuf__ int16_t*& src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks spaced block_stride apart from src into dst and then advances src by
 * repeat_stride x 32 bytes.
 *
 * @override_id asc_loadalign_postupdate__Rvu16_RPUu16_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) that receives the VL uint16_t elements assembled
 * from the selected DataBlocks; a mask register is not allowed in this mode.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which is advanced by
 * repeat_stride x 32 bytes after the eight DataBlocks are transferred.
 * @param [in] block_stride Distance in 32-byte units between the start addresses of two adjacent DataBlocks in the
 * source (uint16_t, 0-65535).
 * @param [in] repeat_stride Post-update step in 32-byte units: src increases by repeat_stride x 32 bytes once this
 * non-contiguous transfer finishes (uint16_t, 0-65535).
 * @param [in] mask Mask register evaluated per DataBlock: a DataBlock is transferred when any of its elements is
 * enabled, otherwise its destination elements are set to 0 and even an out-of-range DataBlock raises no error.
 */
__simd_callee__ inline void asc_loadalign_postupdate(
    vector_uint16_t& dst, __ubuf__ uint16_t*& src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks spaced block_stride apart from src into dst and then advances src by
 * repeat_stride x 32 bytes.
 *
 * @override_id asc_loadalign_postupdate__Rvi32_RPUi32_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int32_t) that receives the VL int32_t elements assembled
 * from the selected DataBlocks; a mask register is not allowed in this mode.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which is advanced by
 * repeat_stride x 32 bytes after the eight DataBlocks are transferred.
 * @param [in] block_stride Distance in 32-byte units between the start addresses of two adjacent DataBlocks in the
 * source (uint16_t, 0-65535).
 * @param [in] repeat_stride Post-update step in 32-byte units: src increases by repeat_stride x 32 bytes once this
 * non-contiguous transfer finishes (uint16_t, 0-65535).
 * @param [in] mask Mask register evaluated per DataBlock: a DataBlock is transferred when any of its elements is
 * enabled, otherwise its destination elements are set to 0 and even an out-of-range DataBlock raises no error.
 */
__simd_callee__ inline void asc_loadalign_postupdate(
    vector_int32_t& dst, __ubuf__ int32_t*& src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks spaced block_stride apart from src into dst and then advances src by
 * repeat_stride x 32 bytes.
 *
 * @override_id asc_loadalign_postupdate__Rvu32_RPUu32_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint32_t) that receives the VL uint32_t elements assembled
 * from the selected DataBlocks; a mask register is not allowed in this mode.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which is advanced by
 * repeat_stride x 32 bytes after the eight DataBlocks are transferred.
 * @param [in] block_stride Distance in 32-byte units between the start addresses of two adjacent DataBlocks in the
 * source (uint16_t, 0-65535).
 * @param [in] repeat_stride Post-update step in 32-byte units: src increases by repeat_stride x 32 bytes once this
 * non-contiguous transfer finishes (uint16_t, 0-65535).
 * @param [in] mask Mask register evaluated per DataBlock: a DataBlock is transferred when any of its elements is
 * enabled, otherwise its destination elements are set to 0 and even an out-of-range DataBlock raises no error.
 */
__simd_callee__ inline void asc_loadalign_postupdate(
    vector_uint32_t& dst, __ubuf__ uint32_t*& src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks spaced block_stride apart from src into dst and then advances src by
 * repeat_stride x 32 bytes.
 *
 * @override_id asc_loadalign_postupdate__Rvi64_RPUi64_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int64_t) that receives the VL int64_t elements assembled
 * from the selected DataBlocks; a mask register is not allowed in this mode.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which is advanced by
 * repeat_stride x 32 bytes after the eight DataBlocks are transferred.
 * @param [in] block_stride Distance in 32-byte units between the start addresses of two adjacent DataBlocks in the
 * source (uint16_t, 0-65535).
 * @param [in] repeat_stride Post-update step in 32-byte units: src increases by repeat_stride x 32 bytes once this
 * non-contiguous transfer finishes (uint16_t, 0-65535).
 * @param [in] mask Mask register evaluated per DataBlock: a DataBlock is transferred when any of its elements is
 * enabled, otherwise its destination elements are set to 0 and even an out-of-range DataBlock raises no error.
 */
__simd_callee__ inline void asc_loadalign_postupdate(
    vector_int64_t& dst, __ubuf__ int64_t*& src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks spaced block_stride apart from src into dst and then advances src by
 * repeat_stride x 32 bytes.
 *
 * @override_id asc_loadalign_postupdate__Rvbf16_RPUbf16_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) that receives the VL bfloat16_t elements
 * assembled from the selected DataBlocks; a mask register is not allowed in this mode.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which is advanced by
 * repeat_stride x 32 bytes after the eight DataBlocks are transferred.
 * @param [in] block_stride Distance in 32-byte units between the start addresses of two adjacent DataBlocks in the
 * source (uint16_t, 0-65535).
 * @param [in] repeat_stride Post-update step in 32-byte units: src increases by repeat_stride x 32 bytes once this
 * non-contiguous transfer finishes (uint16_t, 0-65535).
 * @param [in] mask Mask register evaluated per DataBlock: a DataBlock is transferred when any of its elements is
 * enabled, otherwise its destination elements are set to 0 and even an out-of-range DataBlock raises no error.
 */
__simd_callee__ inline void asc_loadalign_postupdate(
    vector_bfloat16_t& dst, __ubuf__ bfloat16_t*& src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks spaced block_stride apart from src into dst and then advances src by
 * repeat_stride x 32 bytes.
 *
 * @override_id asc_loadalign_postupdate__Rvf8e4m3_RPUf8e4m3_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) that receives the VL fp8_e4m3fn_t elements
 * assembled from the selected DataBlocks; a mask register is not allowed in this mode.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which is advanced by
 * repeat_stride x 32 bytes after the eight DataBlocks are transferred.
 * @param [in] block_stride Distance in 32-byte units between the start addresses of two adjacent DataBlocks in the
 * source (uint16_t, 0-65535).
 * @param [in] repeat_stride Post-update step in 32-byte units: src increases by repeat_stride x 32 bytes once this
 * non-contiguous transfer finishes (uint16_t, 0-65535).
 * @param [in] mask Mask register evaluated per DataBlock: a DataBlock is transferred when any of its elements is
 * enabled, otherwise its destination elements are set to 0 and even an out-of-range DataBlock raises no error.
 */
__simd_callee__ inline void asc_loadalign_postupdate(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t*& src, uint16_t block_stride, uint16_t repeat_stride,
    vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks spaced block_stride apart from src into dst and then advances src by
 * repeat_stride x 32 bytes.
 *
 * @override_id asc_loadalign_postupdate__Rvhf8_RPUhf8_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) that receives the VL hifloat8_t elements
 * assembled from the selected DataBlocks; a mask register is not allowed in this mode.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which is advanced by
 * repeat_stride x 32 bytes after the eight DataBlocks are transferred.
 * @param [in] block_stride Distance in 32-byte units between the start addresses of two adjacent DataBlocks in the
 * source (uint16_t, 0-65535).
 * @param [in] repeat_stride Post-update step in 32-byte units: src increases by repeat_stride x 32 bytes once this
 * non-contiguous transfer finishes (uint16_t, 0-65535).
 * @param [in] mask Mask register evaluated per DataBlock: a DataBlock is transferred when any of its elements is
 * enabled, otherwise its destination elements are set to 0 and even an out-of-range DataBlock raises no error.
 */
__simd_callee__ inline void asc_loadalign_postupdate(
    vector_hifloat8_t& dst, __ubuf__ hifloat8_t*& src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks spaced block_stride apart from src into dst and then advances src by
 * repeat_stride x 32 bytes.
 *
 * @override_id asc_loadalign_postupdate__Rvf8e5m2_RPUf8e5m2_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) that receives the VL fp8_e5m2_t elements
 * assembled from the selected DataBlocks; a mask register is not allowed in this mode.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which is advanced by
 * repeat_stride x 32 bytes after the eight DataBlocks are transferred.
 * @param [in] block_stride Distance in 32-byte units between the start addresses of two adjacent DataBlocks in the
 * source (uint16_t, 0-65535).
 * @param [in] repeat_stride Post-update step in 32-byte units: src increases by repeat_stride x 32 bytes once this
 * non-contiguous transfer finishes (uint16_t, 0-65535).
 * @param [in] mask Mask register evaluated per DataBlock: a DataBlock is transferred when any of its elements is
 * enabled, otherwise its destination elements are set to 0 and even an out-of-range DataBlock raises no error.
 */
__simd_callee__ inline void asc_loadalign_postupdate(
    vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t*& src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks spaced block_stride apart from src into dst and then advances src by
 * repeat_stride x 32 bytes.
 *
 * @override_id asc_loadalign_postupdate__Rvf8e8m0_RPUf8e8m0_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) that receives the VL fp8_e8m0_t elements
 * assembled from the selected DataBlocks; a mask register is not allowed in this mode.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which is advanced by
 * repeat_stride x 32 bytes after the eight DataBlocks are transferred.
 * @param [in] block_stride Distance in 32-byte units between the start addresses of two adjacent DataBlocks in the
 * source (uint16_t, 0-65535).
 * @param [in] repeat_stride Post-update step in 32-byte units: src increases by repeat_stride x 32 bytes once this
 * non-contiguous transfer finishes (uint16_t, 0-65535).
 * @param [in] mask Mask register evaluated per DataBlock: a DataBlock is transferred when any of its elements is
 * enabled, otherwise its destination elements are set to 0 and even an out-of-range DataBlock raises no error.
 */
__simd_callee__ inline void asc_loadalign_postupdate(
    vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t*& src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks spaced block_stride apart from src into dst and then advances src by
 * repeat_stride x 32 bytes.
 *
 * @override_id asc_loadalign_postupdate__Rvf4e1m2x2_RPUf4e1m2x2_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) that receives the VL fp4x2_e1m2_t elements
 * assembled from the selected DataBlocks; a mask register is not allowed in this mode.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which is advanced by
 * repeat_stride x 32 bytes after the eight DataBlocks are transferred.
 * @param [in] block_stride Distance in 32-byte units between the start addresses of two adjacent DataBlocks in the
 * source (uint16_t, 0-65535).
 * @param [in] repeat_stride Post-update step in 32-byte units: src increases by repeat_stride x 32 bytes once this
 * non-contiguous transfer finishes (uint16_t, 0-65535).
 * @param [in] mask Mask register evaluated per DataBlock: a DataBlock is transferred when any of its elements is
 * enabled, otherwise its destination elements are set to 0 and even an out-of-range DataBlock raises no error.
 */
__simd_callee__ inline void asc_loadalign_postupdate(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t*& src, uint16_t block_stride, uint16_t repeat_stride,
    vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks spaced block_stride apart from src into dst and then advances src by
 * repeat_stride x 32 bytes.
 *
 * @override_id asc_loadalign_postupdate__Rvf4e2m1x2_RPUf4e2m1x2_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) that receives the VL fp4x2_e2m1_t elements
 * assembled from the selected DataBlocks; a mask register is not allowed in this mode.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which is advanced by
 * repeat_stride x 32 bytes after the eight DataBlocks are transferred.
 * @param [in] block_stride Distance in 32-byte units between the start addresses of two adjacent DataBlocks in the
 * source (uint16_t, 0-65535).
 * @param [in] repeat_stride Post-update step in 32-byte units: src increases by repeat_stride x 32 bytes once this
 * non-contiguous transfer finishes (uint16_t, 0-65535).
 * @param [in] mask Mask register evaluated per DataBlock: a DataBlock is transferred when any of its elements is
 * enabled, otherwise its destination elements are set to 0 and even an out-of-range DataBlock raises no error.
 */
__simd_callee__ inline void asc_loadalign_postupdate(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t*& src, uint16_t block_stride, uint16_t repeat_stride,
    vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks spaced block_stride apart from src into dst and then advances src by
 * repeat_stride x 32 bytes.
 *
 * @override_id asc_loadalign_postupdate__Rvi4x2_RPUi4x2_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) that receives the VL int4b_t elements assembled
 * from the selected DataBlocks; a mask register is not allowed in this mode.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which is advanced by
 * repeat_stride x 32 bytes after the eight DataBlocks are transferred.
 * @param [in] block_stride Distance in 32-byte units between the start addresses of two adjacent DataBlocks in the
 * source (uint16_t, 0-65535).
 * @param [in] repeat_stride Post-update step in 32-byte units: src increases by repeat_stride x 32 bytes once this
 * non-contiguous transfer finishes (uint16_t, 0-65535).
 * @param [in] mask Mask register evaluated per DataBlock: a DataBlock is transferred when any of its elements is
 * enabled, otherwise its destination elements are set to 0 and even an out-of-range DataBlock raises no error.
 */
__simd_callee__ inline void asc_loadalign_postupdate(
    vector_int4x2_t& dst, __ubuf__ int4b_t*& src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks spaced block_stride apart from src into dst and then advances src by
 * repeat_stride x 32 bytes.
 *
 * @override_id asc_loadalign_postupdate__Rvf16_RPUf16_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) that receives the VL half elements assembled from the
 * selected DataBlocks; a mask register is not allowed in this mode.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which is advanced by
 * repeat_stride x 32 bytes after the eight DataBlocks are transferred.
 * @param [in] block_stride Distance in 32-byte units between the start addresses of two adjacent DataBlocks in the
 * source (uint16_t, 0-65535).
 * @param [in] repeat_stride Post-update step in 32-byte units: src increases by repeat_stride x 32 bytes once this
 * non-contiguous transfer finishes (uint16_t, 0-65535).
 * @param [in] mask Mask register evaluated per DataBlock: a DataBlock is transferred when any of its elements is
 * enabled, otherwise its destination elements are set to 0 and even an out-of-range DataBlock raises no error.
 */
__simd_callee__ inline void asc_loadalign_postupdate(
    vector_half& dst, __ubuf__ half*& src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads eight 32-byte DataBlocks spaced block_stride apart from src into dst and then advances src by
 * repeat_stride x 32 bytes.
 *
 * @override_id asc_loadalign_postupdate__Rvf32_RPUf32_u16_u16_vb
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_float) that receives the VL float elements assembled from
 * the selected DataBlocks; a mask register is not allowed in this mode.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which is advanced by
 * repeat_stride x 32 bytes after the eight DataBlocks are transferred.
 * @param [in] block_stride Distance in 32-byte units between the start addresses of two adjacent DataBlocks in the
 * source (uint16_t, 0-65535).
 * @param [in] repeat_stride Post-update step in 32-byte units: src increases by repeat_stride x 32 bytes once this
 * non-contiguous transfer finishes (uint16_t, 0-65535).
 * @param [in] mask Mask register evaluated per DataBlock: a DataBlock is transferred when any of its elements is
 * enabled, otherwise its destination elements are set to 0 and even an out-of-range DataBlock raises no error.
 */
__simd_callee__ inline void asc_loadalign_postupdate(
    vector_float& dst, __ubuf__ float*& src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL fp4x2_e1m2_t elements into dst from src plus the element offset held in the addr_reg, so the offset
 * may change per Hardware Loop iteration.
 *
 * @override_id asc_loadalign__Rvf4e1m2x2_PUf4e1m2x2_ar
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e1m2_t register that receives VL fp4x2_e1m2_t elements read from src; its
 * dtype must match src.
 * @param [in] src Source UB address of the fp4x2_e1m2_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Address register holding the element-count offset from src; it must be generated by
 * asc_update_addr_reg beforehand, whose b8/b16/b32 modes address 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL fp4x2_e2m1_t elements into dst from src plus the element offset held in the addr_reg, so the offset
 * may change per Hardware Loop iteration.
 *
 * @override_id asc_loadalign__Rvf4e2m1x2_PUf4e2m1x2_ar
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e2m1_t register that receives VL fp4x2_e2m1_t elements read from src; its
 * dtype must match src.
 * @param [in] src Source UB address of the fp4x2_e2m1_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Address register holding the element-count offset from src; it must be generated by
 * asc_update_addr_reg beforehand, whose b8/b16/b32 modes address 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL int4b_t elements into dst from src plus the element offset held in the addr_reg, so the offset may
 * change per Hardware Loop iteration.
 *
 * @override_id asc_loadalign__Rvi4x2_PUi4x2_ar
 * @archs 3510
 * @param [out] dst Destination register that receives VL int4b_t elements read from src; its actual register type is
 * vector_int4x2_t and its element type must match src.
 * @param [in] src Source UB address of the int4b_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Address register holding the element-count offset from src; it must be generated by
 * asc_update_addr_reg beforehand, whose b8/b16/b32 modes address 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign(vector_int4x2_t& dst, __ubuf__ int4b_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL uint8_t elements into dst from src plus the element offset held in the addr_reg, so the offset may
 * change per Hardware Loop iteration.
 *
 * @override_id asc_loadalign__Rvu8_PUu8_ar
 * @archs 3510
 * @param [out] dst Destination vector_uint8_t register that receives VL uint8_t elements read from src; its dtype must
 * match src.
 * @param [in] src Source UB address of the uint8_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Address register holding the element-count offset from src; it must be generated by
 * asc_update_addr_reg beforehand, whose b8/b16/b32 modes address 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign(vector_uint8_t& dst, __ubuf__ uint8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL int8_t elements into dst from src plus the element offset held in the addr_reg, so the offset may
 * change per Hardware Loop iteration.
 *
 * @override_id asc_loadalign__Rvi8_PUi8_ar
 * @archs 3510
 * @param [out] dst Destination vector_int8_t register that receives VL int8_t elements read from src; its dtype must
 * match src.
 * @param [in] src Source UB address of the int8_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Address register holding the element-count offset from src; it must be generated by
 * asc_update_addr_reg beforehand, whose b8/b16/b32 modes address 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign(vector_int8_t& dst, __ubuf__ int8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL fp8_e4m3fn_t elements into dst from src plus the element offset held in the addr_reg, so the offset
 * may change per Hardware Loop iteration.
 *
 * @override_id asc_loadalign__Rvf8e4m3_PUf8e4m3_ar
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e4m3fn_t register that receives VL fp8_e4m3fn_t elements read from src; its
 * dtype must match src.
 * @param [in] src Source UB address of the fp8_e4m3fn_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Address register holding the element-count offset from src; it must be generated by
 * asc_update_addr_reg beforehand, whose b8/b16/b32 modes address 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL hifloat8_t elements into dst from src plus the element offset held in the addr_reg, so the offset may
 * change per Hardware Loop iteration.
 *
 * @override_id asc_loadalign__Rvhf8_PUhf8_ar
 * @archs 3510
 * @param [out] dst Destination vector_hifloat8_t register that receives VL hifloat8_t elements read from src; its dtype
 * must match src.
 * @param [in] src Source UB address of the hifloat8_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Address register holding the element-count offset from src; it must be generated by
 * asc_update_addr_reg beforehand, whose b8/b16/b32 modes address 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL fp8_e5m2_t elements into dst from src plus the element offset held in the addr_reg, so the offset may
 * change per Hardware Loop iteration.
 *
 * @override_id asc_loadalign__Rvf8e5m2_PUf8e5m2_ar
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e5m2_t register that receives VL fp8_e5m2_t elements read from src; its dtype
 * must match src.
 * @param [in] src Source UB address of the fp8_e5m2_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Address register holding the element-count offset from src; it must be generated by
 * asc_update_addr_reg beforehand, whose b8/b16/b32 modes address 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL fp8_e8m0_t elements into dst from src plus the element offset held in the addr_reg, so the offset may
 * change per Hardware Loop iteration.
 *
 * @override_id asc_loadalign__Rvf8e8m0_PUf8e8m0_ar
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e8m0_t register that receives VL fp8_e8m0_t elements read from src; its dtype
 * must match src.
 * @param [in] src Source UB address of the fp8_e8m0_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Address register holding the element-count offset from src; it must be generated by
 * asc_update_addr_reg beforehand, whose b8/b16/b32 modes address 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL uint16_t elements into dst from src plus the element offset held in the addr_reg, so the offset may
 * change per Hardware Loop iteration.
 *
 * @override_id asc_loadalign__Rvu16_PUu16_ar
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t register that receives VL uint16_t elements read from src; its dtype
 * must match src.
 * @param [in] src Source UB address of the uint16_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Address register holding the element-count offset from src; it must be generated by
 * asc_update_addr_reg beforehand, whose b8/b16/b32 modes address 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign(vector_uint16_t& dst, __ubuf__ uint16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL int16_t elements into dst from src plus the element offset held in the addr_reg, so the offset may
 * change per Hardware Loop iteration.
 *
 * @override_id asc_loadalign__Rvi16_PUi16_ar
 * @archs 3510
 * @param [out] dst Destination vector_int16_t register that receives VL int16_t elements read from src; its dtype must
 * match src.
 * @param [in] src Source UB address of the int16_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Address register holding the element-count offset from src; it must be generated by
 * asc_update_addr_reg beforehand, whose b8/b16/b32 modes address 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign(vector_int16_t& dst, __ubuf__ int16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL bfloat16_t elements into dst from src plus the element offset held in the addr_reg, so the offset may
 * change per Hardware Loop iteration.
 *
 * @override_id asc_loadalign__Rvbf16_PUbf16_ar
 * @archs 3510
 * @param [out] dst Destination vector_bfloat16_t register that receives VL bfloat16_t elements read from src; its dtype
 * must match src.
 * @param [in] src Source UB address of the bfloat16_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Address register holding the element-count offset from src; it must be generated by
 * asc_update_addr_reg beforehand, whose b8/b16/b32 modes address 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL uint32_t elements into dst from src plus the element offset held in the addr_reg, so the offset may
 * change per Hardware Loop iteration.
 *
 * @override_id asc_loadalign__Rvu32_PUu32_ar
 * @archs 3510
 * @param [out] dst Destination vector_uint32_t register that receives VL uint32_t elements read from src; its dtype
 * must match src.
 * @param [in] src Source UB address of the uint32_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Address register holding the element-count offset from src; it must be generated by
 * asc_update_addr_reg beforehand, whose b8/b16/b32 modes address 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign(vector_uint32_t& dst, __ubuf__ uint32_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL int32_t elements into dst from src plus the element offset held in the addr_reg, so the offset may
 * change per Hardware Loop iteration.
 *
 * @override_id asc_loadalign__Rvi32_PUi32_ar
 * @archs 3510
 * @param [out] dst Destination vector_int32_t register that receives VL int32_t elements read from src; its dtype must
 * match src.
 * @param [in] src Source UB address of the int32_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Address register holding the element-count offset from src; it must be generated by
 * asc_update_addr_reg beforehand, whose b8/b16/b32 modes address 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign(vector_int32_t& dst, __ubuf__ int32_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL int64_t elements into dst from src plus the element offset held in the addr_reg, so the offset may
 * change per Hardware Loop iteration.
 *
 * @override_id asc_loadalign__Rvi64_PUi64_ar
 * @archs 3510
 * @param [out] dst Destination vector_int64_t register that receives VL int64_t elements read from src; its dtype must
 * match src.
 * @param [in] src Source UB address of the int64_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Address register holding the element-count offset from src; it must be generated by
 * asc_update_addr_reg beforehand, whose b8/b16/b32 modes address 1/2/4-byte elements. For int64_t use
 * asc_update_addr_reg_b32 with each dimension set to twice the desired b64 element offset, because b64 mode is
 * unsupported.
 */
__simd_callee__ inline void asc_loadalign(vector_int64_t& dst, __ubuf__ int64_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL uint64_t elements into dst from src plus the element offset held in the addr_reg, so the offset may
 * change per Hardware Loop iteration.
 *
 * @override_id asc_loadalign__Rvu64_PUu64_ar
 * @archs 3510
 * @param [out] dst Destination vector_uint64_t register that receives VL uint64_t elements read from src; its dtype
 * must match src.
 * @param [in] src Source UB address of the uint64_t data; the effective read address must be 32-byte aligned.
 * @param [in] offset Address register holding the element-count offset from src; it must be generated by
 * asc_update_addr_reg beforehand, whose b8/b16/b32 modes address 1/2/4-byte elements. For uint64_t use
 * asc_update_addr_reg_b32 with each dimension set to twice the desired b64 element offset, because b64 mode is
 * unsupported.
 */
__simd_callee__ inline void asc_loadalign(vector_uint64_t& dst, __ubuf__ uint64_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL half elements into dst from src plus the element offset held in the addr_reg, so the offset may
 * change per Hardware Loop iteration.
 *
 * @override_id asc_loadalign__Rvf16_PUf16_ar
 * @archs 3510
 * @param [out] dst Destination vector_half register that receives VL half elements read from src; its dtype must match
 * src.
 * @param [in] src Source UB address of the half data; the effective read address must be 32-byte aligned.
 * @param [in] offset Address register holding the element-count offset from src; it must be generated by
 * asc_update_addr_reg beforehand, whose b8/b16/b32 modes address 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign(vector_half& dst, __ubuf__ half* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL float elements into dst from src plus the element offset held in the addr_reg, so the offset may
 * change per Hardware Loop iteration.
 *
 * @override_id asc_loadalign__Rvf32_PUf32_ar
 * @archs 3510
 * @param [out] dst Destination vector_float register that receives VL float elements read from src; its dtype must
 * match src.
 * @param [in] src Source UB address of the float data; the effective read address must be 32-byte aligned.
 * @param [in] offset Address register holding the element-count offset from src; it must be generated by
 * asc_update_addr_reg beforehand, whose b8/b16/b32 modes address 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign(vector_float& dst, __ubuf__ float* src, addr_reg offset);

__simd_callee__ inline void asc_loadalign_brc(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, addr_reg offset);

__simd_callee__ inline void asc_loadalign_brc(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, addr_reg offset);

__simd_callee__ inline void asc_loadalign_brc(vector_int4x2_t& dst, __ubuf__ int4b_t* src, addr_reg offset);

__simd_callee__ inline void asc_loadalign_brc(vector_uint8_t& dst, __ubuf__ uint8_t* src, addr_reg offset);

__simd_callee__ inline void asc_loadalign_brc(vector_int8_t& dst, __ubuf__ int8_t* src, addr_reg offset);

__simd_callee__ inline void asc_loadalign_brc(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, addr_reg offset);

__simd_callee__ inline void asc_loadalign_brc(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, addr_reg offset);

__simd_callee__ inline void asc_loadalign_brc(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, addr_reg offset);

__simd_callee__ inline void asc_loadalign_brc(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, addr_reg offset);

__simd_callee__ inline void asc_loadalign_brc(vector_uint16_t& dst, __ubuf__ uint16_t* src, addr_reg offset);

__simd_callee__ inline void asc_loadalign_brc(vector_int16_t& dst, __ubuf__ int16_t* src, addr_reg offset);

__simd_callee__ inline void asc_loadalign_brc(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, addr_reg offset);

__simd_callee__ inline void asc_loadalign_brc(vector_half& dst, __ubuf__ half* src, addr_reg offset);

__simd_callee__ inline void asc_loadalign_brc(vector_uint32_t& dst, __ubuf__ uint32_t* src, addr_reg offset);

__simd_callee__ inline void asc_loadalign_brc(vector_int32_t& dst, __ubuf__ int32_t* src, addr_reg offset);

__simd_callee__ inline void asc_loadalign_brc(vector_float& dst, __ubuf__ float* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of packed 4-bit fp4x2_e1m2_t elements starting at the UB address given by src
 * plus the offset held in the addr_reg, duplicates every source element, and writes the upsampled result of VL bytes
 * (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvf4e1m2x2_PUf4e1m2x2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the packed 4-bit fp4x2_e1m2_t elements to read; the read starts at src plus the
 * offset recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_upsample(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of packed 4-bit fp4x2_e2m1_t elements starting at the UB address given by src
 * plus the offset held in the addr_reg, duplicates every source element, and writes the upsampled result of VL bytes
 * (256 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvf4e2m1x2_PUf4e2m1x2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the packed 4-bit fp4x2_e2m1_t elements to read; the read starts at src plus the
 * offset recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_upsample(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of packed 4-bit int4b_t elements starting at the UB address given by src plus the
 * offset held in the addr_reg, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes)
 * into dst.
 *
 * @override_id asc_loadalign_upsample__Rvi4x2_PUi4x2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the packed 4-bit int4b_t elements to read; the read starts at src plus the
 * offset recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_int4x2_t& dst, __ubuf__ int4b_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit uint8_t elements starting at the UB address given by src plus the offset
 * held in the addr_reg, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes) into
 * dst.
 *
 * @override_id asc_loadalign_upsample__Rvu8_PUu8_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit uint8_t elements to read; the read starts at src plus the offset
 * recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_uint8_t& dst, __ubuf__ uint8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit int8_t elements starting at the UB address given by src plus the offset
 * held in the addr_reg, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes) into
 * dst.
 *
 * @override_id asc_loadalign_upsample__Rvi8_PUi8_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit int8_t elements to read; the read starts at src plus the offset
 * recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_int8_t& dst, __ubuf__ int8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit fp8_e4m3fn_t elements starting at the UB address given by src plus the
 * offset held in the addr_reg, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes)
 * into dst.
 *
 * @override_id asc_loadalign_upsample__Rvf8e4m3_PUf8e4m3_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit fp8_e4m3fn_t elements to read; the read starts at src plus the offset
 * recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_upsample(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit hifloat8_t elements starting at the UB address given by src plus the
 * offset held in the addr_reg, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes)
 * into dst.
 *
 * @override_id asc_loadalign_upsample__Rvhf8_PUhf8_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit hifloat8_t elements to read; the read starts at src plus the offset
 * recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit fp8_e5m2_t elements starting at the UB address given by src plus the
 * offset held in the addr_reg, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes)
 * into dst.
 *
 * @override_id asc_loadalign_upsample__Rvf8e5m2_PUf8e5m2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit fp8_e5m2_t elements to read; the read starts at src plus the offset
 * recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 8-bit fp8_e8m0_t elements starting at the UB address given by src plus the
 * offset held in the addr_reg, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes)
 * into dst.
 *
 * @override_id asc_loadalign_upsample__Rvf8e8m0_PUf8e8m0_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit fp8_e8m0_t elements to read; the read starts at src plus the offset
 * recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 16-bit uint16_t elements starting at the UB address given by src plus the
 * offset held in the addr_reg, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes)
 * into dst.
 *
 * @override_id asc_loadalign_upsample__Rvu16_PUu16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit uint16_t elements to read; the read starts at src plus the offset
 * recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_uint16_t& dst, __ubuf__ uint16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 16-bit int16_t elements starting at the UB address given by src plus the
 * offset held in the addr_reg, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes)
 * into dst.
 *
 * @override_id asc_loadalign_upsample__Rvi16_PUi16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit int16_t elements to read; the read starts at src plus the offset
 * recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_int16_t& dst, __ubuf__ int16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 16-bit bfloat16_t elements starting at the UB address given by src plus the
 * offset held in the addr_reg, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes)
 * into dst.
 *
 * @override_id asc_loadalign_upsample__Rvbf16_PUbf16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit bfloat16_t elements to read; the read starts at src plus the offset
 * recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/2 bytes (128 bytes) of 16-bit half elements starting at the UB address given by src plus the offset
 * held in the addr_reg, duplicates every source element, and writes the upsampled result of VL bytes (256 bytes) into
 * dst.
 *
 * @override_id asc_loadalign_upsample__Rvf16_PUf16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the upsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit half elements to read; the read starts at src plus the offset
 * recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_half& dst, __ubuf__ half* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of packed 4-bit fp4x2_e1m2_t elements starting at the UB address given by src
 * plus the offset held in the addr_reg, keeps only the even-indexed elements, and writes the downsampled result of VL
 * bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvf4e1m2x2_PUf4e1m2x2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the packed 4-bit fp4x2_e1m2_t elements to read; the read starts at src plus the
 * offset recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_downsample(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of packed 4-bit fp4x2_e2m1_t elements starting at the UB address given by src
 * plus the offset held in the addr_reg, keeps only the even-indexed elements, and writes the downsampled result of VL
 * bytes (256 bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvf4e2m1x2_PUf4e2m1x2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the packed 4-bit fp4x2_e2m1_t elements to read; the read starts at src plus the
 * offset recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_downsample(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of packed 4-bit int4b_t elements starting at the UB address given by src plus
 * the offset held in the addr_reg, keeps only the even-indexed elements, and writes the downsampled result of VL bytes
 * (256 bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvi4x2_PUi4x2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the packed 4-bit int4b_t elements to read; the read starts at src plus the
 * offset recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_int4x2_t& dst, __ubuf__ int4b_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit uint8_t elements starting at the UB address given by src plus the
 * offset held in the addr_reg, keeps only the even-indexed elements, and writes the downsampled result of VL bytes (256
 * bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvu8_PUu8_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit uint8_t elements to read; the read starts at src plus the offset
 * recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_uint8_t& dst, __ubuf__ uint8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit int8_t elements starting at the UB address given by src plus the
 * offset held in the addr_reg, keeps only the even-indexed elements, and writes the downsampled result of VL bytes (256
 * bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvi8_PUi8_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit int8_t elements to read; the read starts at src plus the offset
 * recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_int8_t& dst, __ubuf__ int8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit fp8_e4m3fn_t elements starting at the UB address given by src plus the
 * offset held in the addr_reg, keeps only the even-indexed elements, and writes the downsampled result of VL bytes (256
 * bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvf8e4m3_PUf8e4m3_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit fp8_e4m3fn_t elements to read; the read starts at src plus the offset
 * recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_downsample(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit hifloat8_t elements starting at the UB address given by src plus the
 * offset held in the addr_reg, keeps only the even-indexed elements, and writes the downsampled result of VL bytes (256
 * bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvhf8_PUhf8_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit hifloat8_t elements to read; the read starts at src plus the offset
 * recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit fp8_e5m2_t elements starting at the UB address given by src plus the
 * offset held in the addr_reg, keeps only the even-indexed elements, and writes the downsampled result of VL bytes (256
 * bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvf8e5m2_PUf8e5m2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit fp8_e5m2_t elements to read; the read starts at src plus the offset
 * recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 8-bit fp8_e8m0_t elements starting at the UB address given by src plus the
 * offset held in the addr_reg, keeps only the even-indexed elements, and writes the downsampled result of VL bytes (256
 * bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvf8e8m0_PUf8e8m0_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 8-bit fp8_e8m0_t elements to read; the read starts at src plus the offset
 * recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 16-bit uint16_t elements starting at the UB address given by src plus the
 * offset held in the addr_reg, keeps only the even-indexed elements, and writes the downsampled result of VL bytes (256
 * bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvu16_PUu16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit uint16_t elements to read; the read starts at src plus the offset
 * recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_uint16_t& dst, __ubuf__ uint16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 16-bit int16_t elements starting at the UB address given by src plus the
 * offset held in the addr_reg, keeps only the even-indexed elements, and writes the downsampled result of VL bytes (256
 * bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvi16_PUi16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit int16_t elements to read; the read starts at src plus the offset
 * recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_int16_t& dst, __ubuf__ int16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 16-bit bfloat16_t elements starting at the UB address given by src plus the
 * offset held in the addr_reg, keeps only the even-indexed elements, and writes the downsampled result of VL bytes (256
 * bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvbf16_PUbf16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit bfloat16_t elements to read; the read starts at src plus the offset
 * recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads 2 x VL bytes (512 bytes) of 16-bit half elements starting at the UB address given by src plus the offset
 * held in the addr_reg, keeps only the even-indexed elements, and writes the downsampled result of VL bytes (256 bytes)
 * into dst.
 *
 * @override_id asc_loadalign_downsample__Rvf16_PUf16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register that receives the downsampled result of VL bytes (256 bytes); its
 * dtype must match src.
 * @param [in] src UB source address of the 16-bit half elements to read; the read starts at src plus the offset
 * recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address in element units;
 * it must be produced by asc_update_addr_reg, and b8/b16/b32 select 1/2/4-byte elements.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_half& dst, __ubuf__ half* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element fp4x2_e1m2_t window read at src plus an addr_reg element offset into two vector
 * data registers, writing the even-index elements to vector_fp4x2_e1m2_t dst0 and the odd-index elements to dst1, VL
 * elements each; the register must be produced by asc_update_addr_reg.
 *
 * @override_id asc_loadalign_deintlv__Rvf4e1m2x2_Rvf4e1m2x2_PUf4e1m2x2_ar
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index fp4x2_e1m2_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index fp4x2_e1m2_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus the offset held in the address
 * register) must be 32-byte aligned and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Address register holding the element-count offset relative to src, generated by
 * asc_update_addr_reg; its b8, b16 or b32 mode means 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_fp4x2_e1m2_t& dst0, vector_fp4x2_e1m2_t& dst1, __ubuf__ fp4x2_e1m2_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element fp4x2_e2m1_t window read at src plus an addr_reg element offset into two vector
 * data registers, writing the even-index elements to vector_fp4x2_e2m1_t dst0 and the odd-index elements to dst1, VL
 * elements each; the register must be produced by asc_update_addr_reg.
 *
 * @override_id asc_loadalign_deintlv__Rvf4e2m1x2_Rvf4e2m1x2_PUf4e2m1x2_ar
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index fp4x2_e2m1_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index fp4x2_e2m1_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus the offset held in the address
 * register) must be 32-byte aligned and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Address register holding the element-count offset relative to src, generated by
 * asc_update_addr_reg; its b8, b16 or b32 mode means 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_fp4x2_e2m1_t& dst0, vector_fp4x2_e2m1_t& dst1, __ubuf__ fp4x2_e2m1_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element int4b_t window read at src plus an addr_reg element offset into two vector data
 * registers, writing the even-index elements to vector_int4x2_t dst0 and the odd-index elements to dst1, VL elements
 * each; the register must be produced by asc_update_addr_reg.
 *
 * @override_id asc_loadalign_deintlv__Rvi4x2_Rvi4x2_PUi4x2_ar
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index int4b_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index int4b_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus the offset held in the address
 * register) must be 32-byte aligned and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Address register holding the element-count offset relative to src, generated by
 * asc_update_addr_reg; its b8, b16 or b32 mode means 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_int4x2_t& dst0, vector_int4x2_t& dst1, __ubuf__ int4b_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element uint8_t window read at src plus an addr_reg element offset into two vector data
 * registers, writing the even-index elements to vector_uint8_t dst0 and the odd-index elements to dst1, VL elements
 * each; the register must be produced by asc_update_addr_reg.
 *
 * @override_id asc_loadalign_deintlv__Rvu8_Rvu8_PUu8_ar
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index uint8_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index uint8_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus the offset held in the address
 * register) must be 32-byte aligned and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Address register holding the element-count offset relative to src, generated by
 * asc_update_addr_reg; its b8, b16 or b32 mode means 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_uint8_t& dst0, vector_uint8_t& dst1, __ubuf__ uint8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element int8_t window read at src plus an addr_reg element offset into two vector data
 * registers, writing the even-index elements to vector_int8_t dst0 and the odd-index elements to dst1, VL elements
 * each; the register must be produced by asc_update_addr_reg.
 *
 * @override_id asc_loadalign_deintlv__Rvi8_Rvi8_PUi8_ar
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index int8_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index int8_t elements of the 2*VL window (VL
 * elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus the offset held in the address
 * register) must be 32-byte aligned and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Address register holding the element-count offset relative to src, generated by
 * asc_update_addr_reg; its b8, b16 or b32 mode means 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_int8_t& dst0, vector_int8_t& dst1, __ubuf__ int8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element fp8_e4m3fn_t window read at src plus an addr_reg element offset into two vector
 * data registers, writing the even-index elements to vector_fp8_e4m3fn_t dst0 and the odd-index elements to dst1, VL
 * elements each; the register must be produced by asc_update_addr_reg.
 *
 * @override_id asc_loadalign_deintlv__Rvf8e4m3_Rvf8e4m3_PUf8e4m3_ar
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index fp8_e4m3fn_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index fp8_e4m3fn_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus the offset held in the address
 * register) must be 32-byte aligned and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Address register holding the element-count offset relative to src, generated by
 * asc_update_addr_reg; its b8, b16 or b32 mode means 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_fp8_e4m3fn_t& dst0, vector_fp8_e4m3fn_t& dst1, __ubuf__ fp8_e4m3fn_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element hifloat8_t window read at src plus an addr_reg element offset into two vector
 * data registers, writing the even-index elements to vector_hifloat8_t dst0 and the odd-index elements to dst1, VL
 * elements each; the register must be produced by asc_update_addr_reg.
 *
 * @override_id asc_loadalign_deintlv__Rvhf8_Rvhf8_PUhf8_ar
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index hifloat8_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index hifloat8_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus the offset held in the address
 * register) must be 32-byte aligned and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Address register holding the element-count offset relative to src, generated by
 * asc_update_addr_reg; its b8, b16 or b32 mode means 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_hifloat8_t& dst0, vector_hifloat8_t& dst1, __ubuf__ hifloat8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element fp8_e5m2_t window read at src plus an addr_reg element offset into two vector
 * data registers, writing the even-index elements to vector_fp8_e5m2_t dst0 and the odd-index elements to dst1, VL
 * elements each; the register must be produced by asc_update_addr_reg.
 *
 * @override_id asc_loadalign_deintlv__Rvf8e5m2_Rvf8e5m2_PUf8e5m2_ar
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index fp8_e5m2_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index fp8_e5m2_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus the offset held in the address
 * register) must be 32-byte aligned and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Address register holding the element-count offset relative to src, generated by
 * asc_update_addr_reg; its b8, b16 or b32 mode means 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_fp8_e5m2_t& dst0, vector_fp8_e5m2_t& dst1, __ubuf__ fp8_e5m2_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element fp8_e8m0_t window read at src plus an addr_reg element offset into two vector
 * data registers, writing the even-index elements to vector_fp8_e8m0_t dst0 and the odd-index elements to dst1, VL
 * elements each; the register must be produced by asc_update_addr_reg.
 *
 * @override_id asc_loadalign_deintlv__Rvf8e8m0_Rvf8e8m0_PUf8e8m0_ar
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index fp8_e8m0_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index fp8_e8m0_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus the offset held in the address
 * register) must be 32-byte aligned and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Address register holding the element-count offset relative to src, generated by
 * asc_update_addr_reg; its b8, b16 or b32 mode means 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_fp8_e8m0_t& dst0, vector_fp8_e8m0_t& dst1, __ubuf__ fp8_e8m0_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element uint16_t window read at src plus an addr_reg element offset into two vector data
 * registers, writing the even-index elements to vector_uint16_t dst0 and the odd-index elements to dst1, VL elements
 * each; the register must be produced by asc_update_addr_reg.
 *
 * @override_id asc_loadalign_deintlv__Rvu16_Rvu16_PUu16_ar
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index uint16_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index uint16_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus the offset held in the address
 * register) must be 32-byte aligned and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Address register holding the element-count offset relative to src, generated by
 * asc_update_addr_reg; its b8, b16 or b32 mode means 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_uint16_t& dst0, vector_uint16_t& dst1, __ubuf__ uint16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element int16_t window read at src plus an addr_reg element offset into two vector data
 * registers, writing the even-index elements to vector_int16_t dst0 and the odd-index elements to dst1, VL elements
 * each; the register must be produced by asc_update_addr_reg.
 *
 * @override_id asc_loadalign_deintlv__Rvi16_Rvi16_PUi16_ar
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index int16_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index int16_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus the offset held in the address
 * register) must be 32-byte aligned and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Address register holding the element-count offset relative to src, generated by
 * asc_update_addr_reg; its b8, b16 or b32 mode means 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_int16_t& dst0, vector_int16_t& dst1, __ubuf__ int16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element bfloat16_t window read at src plus an addr_reg element offset into two vector
 * data registers, writing the even-index elements to vector_bfloat16_t dst0 and the odd-index elements to dst1, VL
 * elements each; the register must be produced by asc_update_addr_reg.
 *
 * @override_id asc_loadalign_deintlv__Rvbf16_Rvbf16_PUbf16_ar
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index bfloat16_t elements of the 2*VL
 * window (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index bfloat16_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus the offset held in the address
 * register) must be 32-byte aligned and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Address register holding the element-count offset relative to src, generated by
 * asc_update_addr_reg; its b8, b16 or b32 mode means 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_bfloat16_t& dst0, vector_bfloat16_t& dst1, __ubuf__ bfloat16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element half window read at src plus an addr_reg element offset into two vector data
 * registers, writing the even-index elements to vector_half dst0 and the odd-index elements to dst1, VL elements each;
 * the register must be produced by asc_update_addr_reg.
 *
 * @override_id asc_loadalign_deintlv__Rvf16_Rvf16_PUf16_ar
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index half elements of the 2*VL window (VL
 * elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index half elements of the 2*VL window (VL
 * elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus the offset held in the address
 * register) must be 32-byte aligned and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Address register holding the element-count offset relative to src, generated by
 * asc_update_addr_reg; its b8, b16 or b32 mode means 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_half& dst0, vector_half& dst1, __ubuf__ half* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element uint32_t window read at src plus an addr_reg element offset into two vector data
 * registers, writing the even-index elements to vector_uint32_t dst0 and the odd-index elements to dst1, VL elements
 * each; the register must be produced by asc_update_addr_reg.
 *
 * @override_id asc_loadalign_deintlv__Rvu32_Rvu32_PUu32_ar
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index uint32_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index uint32_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus the offset held in the address
 * register) must be 32-byte aligned and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Address register holding the element-count offset relative to src, generated by
 * asc_update_addr_reg; its b8, b16 or b32 mode means 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_uint32_t& dst0, vector_uint32_t& dst1, __ubuf__ uint32_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element int32_t window read at src plus an addr_reg element offset into two vector data
 * registers, writing the even-index elements to vector_int32_t dst0 and the odd-index elements to dst1, VL elements
 * each; the register must be produced by asc_update_addr_reg.
 *
 * @override_id asc_loadalign_deintlv__Rvi32_Rvi32_PUi32_ar
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index int32_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index int32_t elements of the 2*VL window
 * (VL elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus the offset held in the address
 * register) must be 32-byte aligned and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Address register holding the element-count offset relative to src, generated by
 * asc_update_addr_reg; its b8, b16 or b32 mode means 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_int32_t& dst0, vector_int32_t& dst1, __ubuf__ int32_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Deinterleaves a 2*VL-element float window read at src plus an addr_reg element offset into two vector data
 * registers, writing the even-index elements to vector_float dst0 and the odd-index elements to dst1, VL elements each;
 * the register must be produced by asc_update_addr_reg.
 *
 * @override_id asc_loadalign_deintlv__Rvf32_Rvf32_PUf32_ar
 * @archs 3510
 * @param [out] dst0 Destination vector data register that receives the even-index float elements of the 2*VL window (VL
 * elements); its element type must match src and it must be a different register from dst1.
 * @param [out] dst1 Destination vector data register that receives the odd-index float elements of the 2*VL window (VL
 * elements); its element type must match src and it must be a different register from dst0.
 * @param [in] src Base UB address of the source window; the effective address (src plus the offset held in the address
 * register) must be 32-byte aligned and the whole 2*VL read must stay inside the UB address space.
 * @param [in] offset Address register holding the element-count offset relative to src, generated by
 * asc_update_addr_reg; its b8, b16 or b32 mode means 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_deintlv(
    vector_float& dst0, vector_float& dst1, __ubuf__ float* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous fp4x2_e1m2_t elements starting at the element offset held in an addr_reg past a 32-byte
 * aligned address with one zero element inserted after each, writing the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvf4e1m2x2_PUf4e1m2x2_ar
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e1m2_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the fp4x2_e1m2_t run to unpack; the effective read address must be 32-byte
 * aligned and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src for
 * the VL/2-element read; its b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous fp4x2_e2m1_t elements starting at the element offset held in an addr_reg past a 32-byte
 * aligned address with one zero element inserted after each, writing the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvf4e2m1x2_PUf4e2m1x2_ar
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e2m1_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the fp4x2_e2m1_t run to unpack; the effective read address must be 32-byte
 * aligned and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src for
 * the VL/2-element read; its b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous int4b_t elements starting at the element offset held in an addr_reg past a 32-byte
 * aligned address with one zero element inserted after each, writing the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvi4x2_PUi4x2_ar
 * @archs 3510
 * @param [out] dst Destination vector_int4x2_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the int4b_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src for
 * the VL/2-element read; its b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_int4x2_t& dst, __ubuf__ int4b_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous uint8_t elements starting at the element offset held in an addr_reg past a 32-byte
 * aligned address with one zero element inserted after each, writing the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvu8_PUu8_ar
 * @archs 3510
 * @param [out] dst Destination vector_uint8_t vector data register written with the VL-length unpacked result, one zero
 * element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the uint8_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src for
 * the VL/2-element read; its b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_uint8_t& dst, __ubuf__ uint8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous int8_t elements starting at the element offset held in an addr_reg past a 32-byte
 * aligned address with one zero element inserted after each, writing the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvi8_PUi8_ar
 * @archs 3510
 * @param [out] dst Destination vector_int8_t vector data register written with the VL-length unpacked result, one zero
 * element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the int8_t run to unpack; the effective read address must be 32-byte aligned and
 * stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src for
 * the VL/2-element read; its b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_int8_t& dst, __ubuf__ int8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous fp8_e4m3fn_t elements starting at the element offset held in an addr_reg past a 32-byte
 * aligned address with one zero element inserted after each, writing the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvf8e4m3_PUf8e4m3_ar
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e4m3fn_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the fp8_e4m3fn_t run to unpack; the effective read address must be 32-byte
 * aligned and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src for
 * the VL/2-element read; its b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous hifloat8_t elements starting at the element offset held in an addr_reg past a 32-byte
 * aligned address with one zero element inserted after each, writing the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvhf8_PUhf8_ar
 * @archs 3510
 * @param [out] dst Destination vector_hifloat8_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the hifloat8_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src for
 * the VL/2-element read; its b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous fp8_e5m2_t elements starting at the element offset held in an addr_reg past a 32-byte
 * aligned address with one zero element inserted after each, writing the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvf8e5m2_PUf8e5m2_ar
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e5m2_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the fp8_e5m2_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src for
 * the VL/2-element read; its b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous fp8_e8m0_t elements starting at the element offset held in an addr_reg past a 32-byte
 * aligned address with one zero element inserted after each, writing the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvf8e8m0_PUf8e8m0_ar
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e8m0_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the fp8_e8m0_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src for
 * the VL/2-element read; its b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous uint16_t elements starting at the element offset held in an addr_reg past a 32-byte
 * aligned address with one zero element inserted after each, writing the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvu16_PUu16_ar
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the uint16_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src for
 * the VL/2-element read; its b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_uint16_t& dst, __ubuf__ uint16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous int16_t elements starting at the element offset held in an addr_reg past a 32-byte
 * aligned address with one zero element inserted after each, writing the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvi16_PUi16_ar
 * @archs 3510
 * @param [out] dst Destination vector_int16_t vector data register written with the VL-length unpacked result, one zero
 * element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the int16_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src for
 * the VL/2-element read; its b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_int16_t& dst, __ubuf__ int16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous half elements starting at the element offset held in an addr_reg past a 32-byte aligned
 * address with one zero element inserted after each, writing the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvf16_PUf16_ar
 * @archs 3510
 * @param [out] dst Destination vector_half vector data register written with the VL-length unpacked result, one zero
 * element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the half run to unpack; the effective read address must be 32-byte aligned and
 * stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src for
 * the VL/2-element read; its b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_half& dst, __ubuf__ half* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous bfloat16_t elements starting at the element offset held in an addr_reg past a 32-byte
 * aligned address with one zero element inserted after each, writing the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvbf16_PUbf16_ar
 * @archs 3510
 * @param [out] dst Destination vector_bfloat16_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the bfloat16_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src for
 * the VL/2-element read; its b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous uint32_t elements starting at the element offset held in an addr_reg past a 32-byte
 * aligned address with one zero element inserted after each, writing the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvu32_PUu32_ar
 * @archs 3510
 * @param [out] dst Destination vector_uint32_t vector data register written with the VL-length unpacked result, one
 * zero element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the uint32_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src for
 * the VL/2-element read; its b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_uint32_t& dst, __ubuf__ uint32_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous int32_t elements starting at the element offset held in an addr_reg past a 32-byte
 * aligned address with one zero element inserted after each, writing the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvi32_PUi32_ar
 * @archs 3510
 * @param [out] dst Destination vector_int32_t vector data register written with the VL-length unpacked result, one zero
 * element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the int32_t run to unpack; the effective read address must be 32-byte aligned
 * and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src for
 * the VL/2-element read; its b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_int32_t& dst, __ubuf__ int32_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/2 contiguous float elements starting at the element offset held in an addr_reg past a 32-byte aligned
 * address with one zero element inserted after each, writing the VL-length result to dst.
 *
 * @override_id asc_loadalign_unpack__Rvf32_PUf32_ar
 * @archs 3510
 * @param [out] dst Destination vector_float vector data register written with the VL-length unpacked result, one zero
 * element after each source element; its dtype must match src.
 * @param [in] src Source UB address of the float run to unpack; the effective read address must be 32-byte aligned and
 * stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src for
 * the VL/2-element read; its b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack(vector_float& dst, __ubuf__ float* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/8 bytes of mask data from the 32-byte-aligned UB address src into the mask register dst.
 *
 * @override_id asc_loadalign__Rvb_PUu32
 * @archs 3510
 * @param [out] dst Destination mask register (vector_bool) that receives VL/8 bytes of mask data read from src.
 * @param [in] src Source UB address holding the mask words, read as uint32_t; the effective read address must be
 * 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign(vector_bool& dst, __ubuf__ uint32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/8 bytes of mask data from src advanced by offset bytes into the mask register dst, leaving src itself
 * unmodified.
 *
 * @override_id asc_loadalign__Rvb_PUu32_i32
 * @archs 3510
 * @param [out] dst Destination mask register (vector_bool) that receives VL/8 bytes of mask data read from src.
 * @param [in] src Source UB address holding the mask words, read as uint32_t; the effective read address must be
 * 32-byte aligned.
 * @param [in] offset Signed offset in bytes relative to the src start address, because a mask-register destination
 * counts the offset in bytes; the resulting address src+offset must stay 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign(vector_bool& dst, __ubuf__ uint32_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/8 bytes of mask data into the mask register dst from src plus the offset held in the addr_reg,
 * applied in b32 mode.
 *
 * @override_id asc_loadalign__Rvb_PUu32_ar
 * @archs 3510
 * @param [out] dst Destination mask register (vector_bool) that receives VL/8 bytes of mask data read from src.
 * @param [in] src Source UB address holding the mask words, read as uint32_t; the effective read address must be
 * 32-byte aligned.
 * @param [in] offset Address register holding the offset from src; because dst is a vector_bool mask register the
 * offset must be applied in b32 mode, and the register must be produced by asc_update_addr_reg before this call.
 */
__simd_callee__ inline void asc_loadalign(vector_bool& dst, __ubuf__ uint32_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/16 bytes (16 bytes) of packed 32-bit mask words from a 16-byte-aligned UB address, duplicates every
 * mask bit, and writes the upsampled mask of VL/8 bytes (32 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvb_PUu32
 * @archs 3510
 * @param [out] dst Destination mask register (vector_bool) that receives the upsampled mask of VL/8 bytes (32 bytes).
 * @param [in] src UB source address of the packed 32-bit mask words to read; the effective read address must be 16-byte
 * aligned.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_bool& dst, __ubuf__ uint32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/16 bytes (16 bytes) of packed 32-bit mask words starting at the 16-byte-aligned UB address src plus
 * offset bytes, duplicates every mask bit, and writes the upsampled mask of VL/8 bytes (32 bytes) into dst.
 *
 * @override_id asc_loadalign_upsample__Rvb_PUu32_i32
 * @archs 3510
 * @param [out] dst Destination mask register (vector_bool) that receives the upsampled mask of VL/8 bytes (32 bytes).
 * @param [in] src UB source address of the packed 32-bit mask words to read; the read starts at src plus offset bytes
 * and must stay 16-byte aligned.
 * @param [in] offset Signed byte offset relative to the src start address; the resulting read address must remain
 * 16-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_bool& dst, __ubuf__ uint32_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/16 bytes (16 bytes) of packed 32-bit mask words starting at the UB address given by src plus the
 * offset held in the addr_reg, duplicates every mask bit, and writes the upsampled mask of VL/8 bytes (32 bytes) into
 * dst.
 *
 * @override_id asc_loadalign_upsample__Rvb_PUu32_ar
 * @archs 3510
 * @param [out] dst Destination mask register (vector_bool) that receives the upsampled mask of VL/8 bytes (32 bytes).
 * @param [in] src UB source address of the packed 32-bit mask words to read; the read starts at src plus the offset
 * recorded in the addr_reg and must stay 16-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address; it must be
 * produced by asc_update_addr_reg, and b32 mode (4-byte elements) is required when dst is vector_bool.
 */
__simd_callee__ inline void asc_loadalign_upsample(vector_bool& dst, __ubuf__ uint32_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 bytes (64 bytes) of packed 32-bit mask words from a 32-byte-aligned UB address, keeps only the
 * even-indexed bits, and writes the downsampled mask of VL/8 bytes (32 bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvb_PUu32
 * @archs 3510
 * @param [out] dst Destination mask register (vector_bool) that receives the downsampled mask of VL/8 bytes (32 bytes).
 * @param [in] src UB source address of the packed 32-bit mask words to read; the effective read address must be 32-byte
 * aligned.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_bool& dst, __ubuf__ uint32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 bytes (64 bytes) of packed 32-bit mask words starting at the 32-byte-aligned UB address src plus
 * offset bytes, keeps only the even-indexed bits, and writes the downsampled mask of VL/8 bytes (32 bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvb_PUu32_i32
 * @archs 3510
 * @param [out] dst Destination mask register (vector_bool) that receives the downsampled mask of VL/8 bytes (32 bytes).
 * @param [in] src UB source address of the packed 32-bit mask words to read; the read starts at src plus offset bytes
 * and must stay 32-byte aligned.
 * @param [in] offset Signed byte offset relative to the src start address; the resulting read address must remain
 * 32-byte aligned and inside UB.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_bool& dst, __ubuf__ uint32_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 bytes (64 bytes) of packed 32-bit mask words starting at the UB address given by src plus the
 * offset held in the addr_reg, keeps only the even-indexed bits, and writes the downsampled mask of VL/8 bytes (32
 * bytes) into dst.
 *
 * @override_id asc_loadalign_downsample__Rvb_PUu32_ar
 * @archs 3510
 * @param [out] dst Destination mask register (vector_bool) that receives the downsampled mask of VL/8 bytes (32 bytes).
 * @param [in] src UB source address of the packed 32-bit mask words to read; the read starts at src plus the offset
 * recorded in the addr_reg and must stay 32-byte aligned.
 * @param [in] offset Address register (addr_reg) holding the offset relative to the src start address; it must be
 * produced by asc_update_addr_reg, and b32 mode (4-byte elements) is required when dst is vector_bool.
 */
__simd_callee__ inline void asc_loadalign_downsample(vector_bool& dst, __ubuf__ uint32_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/8 bytes from the 32-byte-aligned UB address src into the mask register dst and then advances src by
 * offset bytes.
 *
 * @override_id asc_loadalign_postupdate__Rvb_RPUu32_i32
 * @archs 3510
 * @param [out] dst Destination mask register (vector_bool) that receives VL/8 bytes of mask bits read from UB; its
 * source element type is uint32_t.
 * @param [in,out] src UB source pointer whose 32-byte-aligned current address is read and which the hardware
 * post-update advances afterwards.
 * @param [in] offset Post-update step in bytes for the mask-register overload: after VL/8 bytes are loaded the src
 * pointer increases by offset bytes.
 */
__simd_callee__ inline void asc_loadalign_postupdate(vector_bool& dst, __ubuf__ uint32_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads bits from a 16-byte aligned UB address into the mask register duplicating every bit, then post-updates
 * src.
 *
 * @override_id asc_loadalign_upsample_postupdate__Rvb_RPUu32_i32
 * @archs 3510
 * @param [out] dst Destination mask register (vector_bool) that receives the VL/8 (32-byte) result of the 2x upsampling
 * operation through a plds mask load; the whole register is written.
 * @param [in,out] src UB source pointer of type __ubuf__ uint32_t* whose bits are read from the current address and
 * whose value is then advanced in post-update mode by offset bytes (the pointer advances by exactly offset bytes).
 * @param [in] offset Post-update step in bytes: the source pointer moves by exactly offset bytes, and the next read
 * address must stay aligned to the mask form's documented boundary.
 */
__simd_callee__ inline void asc_loadalign_upsample_postupdate(
    vector_bool& dst, __ubuf__ uint32_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads bits from a 32-byte aligned UB address into the mask register keeping every even bit, then post-updates
 * src.
 *
 * @override_id asc_loadalign_downsample_postupdate__Rvb_RPUu32_i32
 * @archs 3510
 * @param [out] dst Destination mask register (vector_bool) that receives the VL/8 (32-byte) result of the 2x
 * downsampling operation through a plds mask load; the whole register is written.
 * @param [in,out] src UB source pointer of type __ubuf__ uint32_t* whose bits are read from the current address and
 * whose value is then advanced in post-update mode by offset bytes (the pointer advances by exactly offset bytes).
 * @param [in] offset Post-update step in bytes: the source pointer moves by exactly offset bytes, and the next read
 * address must stay aligned to the mask form's documented boundary.
 */
__simd_callee__ inline void asc_loadalign_downsample_postupdate(
    vector_bool& dst, __ubuf__ uint32_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the int8_t element at an address aligned to the dtype size and broadcasts it across the whole vector
 * data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvi8_PUi8
 * @archs 3510
 * @param [out] dst Destination vector_int8_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the int8_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_int8_t& dst, __ubuf__ int8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the uint8_t element at an address aligned to the dtype size and broadcasts it across the whole vector
 * data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvu8_PUu8
 * @archs 3510
 * @param [out] dst Destination vector_uint8_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the uint8_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_uint8_t& dst, __ubuf__ uint8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the fp4x2_e2m1_t element at an address aligned to the dtype size and broadcasts it across the whole
 * vector data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvf4e2m1x2_PUf4e2m1x2
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e2m1_t vector data register written with the VL-length broadcast result;
 * its dtype must match src.
 * @param [in] src Source UB address of the fp4x2_e2m1_t element to broadcast; the effective read address must be
 * aligned to the dtype size and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the fp4x2_e1m2_t element at an address aligned to the dtype size and broadcasts it across the whole
 * vector data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvf4e1m2x2_PUf4e1m2x2
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e1m2_t vector data register written with the VL-length broadcast result;
 * its dtype must match src.
 * @param [in] src Source UB address of the fp4x2_e1m2_t element to broadcast; the effective read address must be
 * aligned to the dtype size and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the int4b_t element at an address aligned to the dtype size and broadcasts it across the whole vector
 * data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvi4x2_PUi4x2
 * @archs 3510
 * @param [out] dst Destination vector_int4x2_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the int4b_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_int4x2_t& dst, __ubuf__ int4b_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the fp8_e8m0_t element at an address aligned to the dtype size and broadcasts it across the whole vector
 * data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvf8e8m0_PUf8e8m0
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e8m0_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the fp8_e8m0_t element to broadcast; the effective read address must be aligned
 * to the dtype size and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the fp8_e5m2_t element at an address aligned to the dtype size and broadcasts it across the whole vector
 * data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvf8e5m2_PUf8e5m2
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e5m2_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the fp8_e5m2_t element to broadcast; the effective read address must be aligned
 * to the dtype size and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the fp8_e4m3fn_t element at an address aligned to the dtype size and broadcasts it across the whole
 * vector data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvf8e4m3_PUf8e4m3
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e4m3fn_t vector data register written with the VL-length broadcast result;
 * its dtype must match src.
 * @param [in] src Source UB address of the fp8_e4m3fn_t element to broadcast; the effective read address must be
 * aligned to the dtype size and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the hifloat8_t element at an address aligned to the dtype size and broadcasts it across the whole vector
 * data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvhf8_PUhf8
 * @archs 3510
 * @param [out] dst Destination vector_hifloat8_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the hifloat8_t element to broadcast; the effective read address must be aligned
 * to the dtype size and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the int16_t element at an address aligned to the dtype size and broadcasts it across the whole vector
 * data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvi16_PUi16
 * @archs 3510
 * @param [out] dst Destination vector_int16_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the int16_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_int16_t& dst, __ubuf__ int16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the uint16_t element at an address aligned to the dtype size and broadcasts it across the whole vector
 * data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvu16_PUu16
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the uint16_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_uint16_t& dst, __ubuf__ uint16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the half element at an address aligned to the dtype size and broadcasts it across the whole vector data
 * register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvf16_PUf16
 * @archs 3510
 * @param [out] dst Destination vector_half vector data register written with the VL-length broadcast result; its dtype
 * must match src.
 * @param [in] src Source UB address of the half element to broadcast; the effective read address must be aligned to the
 * dtype size and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_half& dst, __ubuf__ half* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the bfloat16_t element at an address aligned to the dtype size and broadcasts it across the whole vector
 * data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvbf16_PUbf16
 * @archs 3510
 * @param [out] dst Destination vector_bfloat16_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the bfloat16_t element to broadcast; the effective read address must be aligned
 * to the dtype size and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the int32_t element at an address aligned to the dtype size and broadcasts it across the whole vector
 * data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvi32_PUi32
 * @archs 3510
 * @param [out] dst Destination vector_int32_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the int32_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_int32_t& dst, __ubuf__ int32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the uint32_t element at an address aligned to the dtype size and broadcasts it across the whole vector
 * data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvu32_PUu32
 * @archs 3510
 * @param [out] dst Destination vector_uint32_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the uint32_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_uint32_t& dst, __ubuf__ uint32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the float element at an address aligned to the dtype size and broadcasts it across the whole vector data
 * register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvf32_PUf32
 * @archs 3510
 * @param [out] dst Destination vector_float vector data register written with the VL-length broadcast result; its dtype
 * must match src.
 * @param [in] src Source UB address of the float element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_float& dst, __ubuf__ float* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the int8_t element at offset elements past an address aligned to the dtype size and broadcasts it across
 * the whole vector data register written to dst, without updating src.
 *
 * @override_id asc_loadalign_brc_elem__Rvi8_PUi8_i32
 * @archs 3510
 * @param [out] dst Destination vector_int8_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the int8_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before the load; the API does not update src for the next
 * call.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_int8_t& dst, __ubuf__ int8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the uint8_t element at offset elements past an address aligned to the dtype size and broadcasts it
 * across the whole vector data register written to dst, without updating src.
 *
 * @override_id asc_loadalign_brc_elem__Rvu8_PUu8_i32
 * @archs 3510
 * @param [out] dst Destination vector_uint8_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the uint8_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before the load; the API does not update src for the next
 * call.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_uint8_t& dst, __ubuf__ uint8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the fp4x2_e2m1_t element at offset elements past an address aligned to the dtype size and broadcasts it
 * across the whole vector data register written to dst, without updating src.
 *
 * @override_id asc_loadalign_brc_elem__Rvf4e2m1x2_PUf4e2m1x2_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e2m1_t vector data register written with the VL-length broadcast result;
 * its dtype must match src.
 * @param [in] src Source UB address of the fp4x2_e2m1_t element to broadcast; the effective read address must be
 * aligned to the dtype size and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before the load; the API does not update src for the next
 * call.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the fp4x2_e1m2_t element at offset elements past an address aligned to the dtype size and broadcasts it
 * across the whole vector data register written to dst, without updating src.
 *
 * @override_id asc_loadalign_brc_elem__Rvf4e1m2x2_PUf4e1m2x2_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e1m2_t vector data register written with the VL-length broadcast result;
 * its dtype must match src.
 * @param [in] src Source UB address of the fp4x2_e1m2_t element to broadcast; the effective read address must be
 * aligned to the dtype size and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before the load; the API does not update src for the next
 * call.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the int4b_t element at offset elements past an address aligned to the dtype size and broadcasts it
 * across the whole vector data register written to dst, without updating src.
 *
 * @override_id asc_loadalign_brc_elem__Rvi4x2_PUi4x2_i32
 * @archs 3510
 * @param [out] dst Destination vector_int4x2_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the int4b_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before the load; the API does not update src for the next
 * call.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_int4x2_t& dst, __ubuf__ int4b_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the fp8_e8m0_t element at offset elements past an address aligned to the dtype size and broadcasts it
 * across the whole vector data register written to dst, without updating src.
 *
 * @override_id asc_loadalign_brc_elem__Rvf8e8m0_PUf8e8m0_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e8m0_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the fp8_e8m0_t element to broadcast; the effective read address must be aligned
 * to the dtype size and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before the load; the API does not update src for the next
 * call.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the fp8_e5m2_t element at offset elements past an address aligned to the dtype size and broadcasts it
 * across the whole vector data register written to dst, without updating src.
 *
 * @override_id asc_loadalign_brc_elem__Rvf8e5m2_PUf8e5m2_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e5m2_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the fp8_e5m2_t element to broadcast; the effective read address must be aligned
 * to the dtype size and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before the load; the API does not update src for the next
 * call.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the fp8_e4m3fn_t element at offset elements past an address aligned to the dtype size and broadcasts it
 * across the whole vector data register written to dst, without updating src.
 *
 * @override_id asc_loadalign_brc_elem__Rvf8e4m3_PUf8e4m3_i32
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e4m3fn_t vector data register written with the VL-length broadcast result;
 * its dtype must match src.
 * @param [in] src Source UB address of the fp8_e4m3fn_t element to broadcast; the effective read address must be
 * aligned to the dtype size and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before the load; the API does not update src for the next
 * call.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the hifloat8_t element at offset elements past an address aligned to the dtype size and broadcasts it
 * across the whole vector data register written to dst, without updating src.
 *
 * @override_id asc_loadalign_brc_elem__Rvhf8_PUhf8_i32
 * @archs 3510
 * @param [out] dst Destination vector_hifloat8_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the hifloat8_t element to broadcast; the effective read address must be aligned
 * to the dtype size and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before the load; the API does not update src for the next
 * call.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the int16_t element at offset elements past an address aligned to the dtype size and broadcasts it
 * across the whole vector data register written to dst, without updating src.
 *
 * @override_id asc_loadalign_brc_elem__Rvi16_PUi16_i32
 * @archs 3510
 * @param [out] dst Destination vector_int16_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the int16_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before the load; the API does not update src for the next
 * call.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_int16_t& dst, __ubuf__ int16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the uint16_t element at offset elements past an address aligned to the dtype size and broadcasts it
 * across the whole vector data register written to dst, without updating src.
 *
 * @override_id asc_loadalign_brc_elem__Rvu16_PUu16_i32
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the uint16_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before the load; the API does not update src for the next
 * call.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_uint16_t& dst, __ubuf__ uint16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the half element at offset elements past an address aligned to the dtype size and broadcasts it across
 * the whole vector data register written to dst, without updating src.
 *
 * @override_id asc_loadalign_brc_elem__Rvf16_PUf16_i32
 * @archs 3510
 * @param [out] dst Destination vector_half vector data register written with the VL-length broadcast result; its dtype
 * must match src.
 * @param [in] src Source UB address of the half element to broadcast; the effective read address must be aligned to the
 * dtype size and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before the load; the API does not update src for the next
 * call.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_half& dst, __ubuf__ half* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the bfloat16_t element at offset elements past an address aligned to the dtype size and broadcasts it
 * across the whole vector data register written to dst, without updating src.
 *
 * @override_id asc_loadalign_brc_elem__Rvbf16_PUbf16_i32
 * @archs 3510
 * @param [out] dst Destination vector_bfloat16_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the bfloat16_t element to broadcast; the effective read address must be aligned
 * to the dtype size and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before the load; the API does not update src for the next
 * call.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the int32_t element at offset elements past an address aligned to the dtype size and broadcasts it
 * across the whole vector data register written to dst, without updating src.
 *
 * @override_id asc_loadalign_brc_elem__Rvi32_PUi32_i32
 * @archs 3510
 * @param [out] dst Destination vector_int32_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the int32_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before the load; the API does not update src for the next
 * call.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_int32_t& dst, __ubuf__ int32_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the uint32_t element at offset elements past an address aligned to the dtype size and broadcasts it
 * across the whole vector data register written to dst, without updating src.
 *
 * @override_id asc_loadalign_brc_elem__Rvu32_PUu32_i32
 * @archs 3510
 * @param [out] dst Destination vector_uint32_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the uint32_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before the load; the API does not update src for the next
 * call.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_uint32_t& dst, __ubuf__ uint32_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the float element at offset elements past an address aligned to the dtype size and broadcasts it across
 * the whole vector data register written to dst, without updating src.
 *
 * @override_id asc_loadalign_brc_elem__Rvf32_PUf32_i32
 * @archs 3510
 * @param [out] dst Destination vector_float vector data register written with the VL-length broadcast result; its dtype
 * must match src.
 * @param [in] src Source UB address of the float element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 * @param [in] offset Signed element-count offset added to src before the load; the API does not update src for the next
 * call.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_float& dst, __ubuf__ float* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the fp4x2_e1m2_t element at the element offset held in an addr_reg past an address aligned to the dtype
 * size and broadcasts it across the whole vector data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvf4e1m2x2_PUf4e1m2x2_ar
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e1m2_t vector data register written with the VL-length broadcast result;
 * its dtype must match src.
 * @param [in] src Source UB address of the fp4x2_e1m2_t element to broadcast; the effective read address must be
 * aligned to the dtype size and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src; its
 * b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the fp4x2_e2m1_t element at the element offset held in an addr_reg past an address aligned to the dtype
 * size and broadcasts it across the whole vector data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvf4e2m1x2_PUf4e2m1x2_ar
 * @archs 3510
 * @param [out] dst Destination vector_fp4x2_e2m1_t vector data register written with the VL-length broadcast result;
 * its dtype must match src.
 * @param [in] src Source UB address of the fp4x2_e2m1_t element to broadcast; the effective read address must be
 * aligned to the dtype size and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src; its
 * b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the int4b_t element at the element offset held in an addr_reg past an address aligned to the dtype size
 * and broadcasts it across the whole vector data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvi4x2_PUi4x2_ar
 * @archs 3510
 * @param [out] dst Destination vector_int4x2_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the int4b_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src; its
 * b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_int4x2_t& dst, __ubuf__ int4b_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the uint8_t element at the element offset held in an addr_reg past an address aligned to the dtype size
 * and broadcasts it across the whole vector data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvu8_PUu8_ar
 * @archs 3510
 * @param [out] dst Destination vector_uint8_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the uint8_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src; its
 * b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_uint8_t& dst, __ubuf__ uint8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the int8_t element at the element offset held in an addr_reg past an address aligned to the dtype size
 * and broadcasts it across the whole vector data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvi8_PUi8_ar
 * @archs 3510
 * @param [out] dst Destination vector_int8_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the int8_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src; its
 * b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_int8_t& dst, __ubuf__ int8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the fp8_e4m3fn_t element at the element offset held in an addr_reg past an address aligned to the dtype
 * size and broadcasts it across the whole vector data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvf8e4m3_PUf8e4m3_ar
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e4m3fn_t vector data register written with the VL-length broadcast result;
 * its dtype must match src.
 * @param [in] src Source UB address of the fp8_e4m3fn_t element to broadcast; the effective read address must be
 * aligned to the dtype size and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src; its
 * b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the hifloat8_t element at the element offset held in an addr_reg past an address aligned to the dtype
 * size and broadcasts it across the whole vector data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvhf8_PUhf8_ar
 * @archs 3510
 * @param [out] dst Destination vector_hifloat8_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the hifloat8_t element to broadcast; the effective read address must be aligned
 * to the dtype size and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src; its
 * b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the fp8_e5m2_t element at the element offset held in an addr_reg past an address aligned to the dtype
 * size and broadcasts it across the whole vector data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvf8e5m2_PUf8e5m2_ar
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e5m2_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the fp8_e5m2_t element to broadcast; the effective read address must be aligned
 * to the dtype size and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src; its
 * b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the fp8_e8m0_t element at the element offset held in an addr_reg past an address aligned to the dtype
 * size and broadcasts it across the whole vector data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvf8e8m0_PUf8e8m0_ar
 * @archs 3510
 * @param [out] dst Destination vector_fp8_e8m0_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the fp8_e8m0_t element to broadcast; the effective read address must be aligned
 * to the dtype size and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src; its
 * b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the uint16_t element at the element offset held in an addr_reg past an address aligned to the dtype size
 * and broadcasts it across the whole vector data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvu16_PUu16_ar
 * @archs 3510
 * @param [out] dst Destination vector_uint16_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the uint16_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src; its
 * b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_uint16_t& dst, __ubuf__ uint16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the int16_t element at the element offset held in an addr_reg past an address aligned to the dtype size
 * and broadcasts it across the whole vector data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvi16_PUi16_ar
 * @archs 3510
 * @param [out] dst Destination vector_int16_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the int16_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src; its
 * b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_int16_t& dst, __ubuf__ int16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the bfloat16_t element at the element offset held in an addr_reg past an address aligned to the dtype
 * size and broadcasts it across the whole vector data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvbf16_PUbf16_ar
 * @archs 3510
 * @param [out] dst Destination vector_bfloat16_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the bfloat16_t element to broadcast; the effective read address must be aligned
 * to the dtype size and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src; its
 * b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the half element at the element offset held in an addr_reg past an address aligned to the dtype size and
 * broadcasts it across the whole vector data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvf16_PUf16_ar
 * @archs 3510
 * @param [out] dst Destination vector_half vector data register written with the VL-length broadcast result; its dtype
 * must match src.
 * @param [in] src Source UB address of the half element to broadcast; the effective read address must be aligned to the
 * dtype size and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src; its
 * b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_half& dst, __ubuf__ half* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the uint32_t element at the element offset held in an addr_reg past an address aligned to the dtype size
 * and broadcasts it across the whole vector data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvu32_PUu32_ar
 * @archs 3510
 * @param [out] dst Destination vector_uint32_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the uint32_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src; its
 * b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_uint32_t& dst, __ubuf__ uint32_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the int32_t element at the element offset held in an addr_reg past an address aligned to the dtype size
 * and broadcasts it across the whole vector data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvi32_PUi32_ar
 * @archs 3510
 * @param [out] dst Destination vector_int32_t vector data register written with the VL-length broadcast result; its
 * dtype must match src.
 * @param [in] src Source UB address of the int32_t element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src; its
 * b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_int32_t& dst, __ubuf__ int32_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads the float element at the element offset held in an addr_reg past an address aligned to the dtype size
 * and broadcasts it across the whole vector data register written to dst.
 *
 * @override_id asc_loadalign_brc_elem__Rvf32_PUf32_ar
 * @archs 3510
 * @param [out] dst Destination vector_float vector data register written with the VL-length broadcast result; its dtype
 * must match src.
 * @param [in] src Source UB address of the float element to broadcast; the effective read address must be aligned to
 * the dtype size and stay inside UB.
 * @param [in] offset Address register produced by asc_update_addr_reg that holds the element-count offset from src; its
 * b8/b16/b32 mode selects 1, 2 or 4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_brc_elem(vector_float& dst, __ubuf__ float* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous int8_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and writes the VL-length expanded data into dst, leaving src unchanged.
 *
 * @override_id asc_loadalign_unpack4__Rvi8_PUi8
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) receiving the VL-length expanded data, one source
 * element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_int8_t& dst, __ubuf__ int8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous uint8_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and writes the VL-length expanded data into dst, leaving src unchanged.
 *
 * @override_id asc_loadalign_unpack4__Rvu8_PUu8
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) receiving the VL-length expanded data, one source
 * element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_uint8_t& dst, __ubuf__ uint8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous fp4x2_e2m1_t elements from the 32-byte-aligned UB address src, inserts three zero
 * elements after each source element, and writes the VL-length expanded data into dst, leaving src unchanged.
 *
 * @override_id asc_loadalign_unpack4__Rvf4e2m1x2_PUf4e2m1x2
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) receiving the VL-length expanded data, one
 * source element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous fp4x2_e1m2_t elements from the 32-byte-aligned UB address src, inserts three zero
 * elements after each source element, and writes the VL-length expanded data into dst, leaving src unchanged.
 *
 * @override_id asc_loadalign_unpack4__Rvf4e1m2x2_PUf4e1m2x2
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) receiving the VL-length expanded data, one
 * source element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous int4b_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and writes the VL-length expanded data into dst, leaving src unchanged.
 *
 * @override_id asc_loadalign_unpack4__Rvi4x2_PUi4x2
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) receiving the VL-length expanded data, one source
 * element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_int4x2_t& dst, __ubuf__ int4b_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous fp8_e8m0_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and writes the VL-length expanded data into dst, leaving src unchanged.
 *
 * @override_id asc_loadalign_unpack4__Rvf8e8m0_PUf8e8m0
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) receiving the VL-length expanded data, one
 * source element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous fp8_e5m2_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and writes the VL-length expanded data into dst, leaving src unchanged.
 *
 * @override_id asc_loadalign_unpack4__Rvf8e5m2_PUf8e5m2
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) receiving the VL-length expanded data, one
 * source element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous fp8_e4m3fn_t elements from the 32-byte-aligned UB address src, inserts three zero
 * elements after each source element, and writes the VL-length expanded data into dst, leaving src unchanged.
 *
 * @override_id asc_loadalign_unpack4__Rvf8e4m3_PUf8e4m3
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) receiving the VL-length expanded data, one
 * source element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous hifloat8_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and writes the VL-length expanded data into dst, leaving src unchanged.
 *
 * @override_id asc_loadalign_unpack4__Rvhf8_PUhf8
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) receiving the VL-length expanded data, one
 * source element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous int8_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and writes the VL-length expanded data into dst starting offset elements after src,
 * without updating src.
 *
 * @override_id asc_loadalign_unpack4__Rvi8_PUi8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) receiving the VL-length expanded data, one source
 * element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 * @param [in] offset Element offset relative to src: the read begins offset int8_t elements after src and src itself is
 * not updated.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_int8_t& dst, __ubuf__ int8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous uint8_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and writes the VL-length expanded data into dst starting offset elements after src,
 * without updating src.
 *
 * @override_id asc_loadalign_unpack4__Rvu8_PUu8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) receiving the VL-length expanded data, one source
 * element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 * @param [in] offset Element offset relative to src: the read begins offset uint8_t elements after src and src itself
 * is not updated.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_uint8_t& dst, __ubuf__ uint8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous fp4x2_e2m1_t elements from the 32-byte-aligned UB address src, inserts three zero
 * elements after each source element, and writes the VL-length expanded data into dst starting offset elements after
 * src, without updating src.
 *
 * @override_id asc_loadalign_unpack4__Rvf4e2m1x2_PUf4e2m1x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) receiving the VL-length expanded data, one
 * source element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 * @param [in] offset Element offset relative to src: the read begins offset fp4x2_e2m1_t elements after src and src
 * itself is not updated.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous fp4x2_e1m2_t elements from the 32-byte-aligned UB address src, inserts three zero
 * elements after each source element, and writes the VL-length expanded data into dst starting offset elements after
 * src, without updating src.
 *
 * @override_id asc_loadalign_unpack4__Rvf4e1m2x2_PUf4e1m2x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) receiving the VL-length expanded data, one
 * source element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 * @param [in] offset Element offset relative to src: the read begins offset fp4x2_e1m2_t elements after src and src
 * itself is not updated.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous int4b_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and writes the VL-length expanded data into dst starting offset elements after src,
 * without updating src.
 *
 * @override_id asc_loadalign_unpack4__Rvi4x2_PUi4x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) receiving the VL-length expanded data, one source
 * element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 * @param [in] offset Element offset relative to src: the read begins offset int4b_t elements after src and src itself
 * is not updated.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_int4x2_t& dst, __ubuf__ int4b_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous fp8_e8m0_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and writes the VL-length expanded data into dst starting offset elements after src,
 * without updating src.
 *
 * @override_id asc_loadalign_unpack4__Rvf8e8m0_PUf8e8m0_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) receiving the VL-length expanded data, one
 * source element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 * @param [in] offset Element offset relative to src: the read begins offset fp8_e8m0_t elements after src and src
 * itself is not updated.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous fp8_e5m2_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and writes the VL-length expanded data into dst starting offset elements after src,
 * without updating src.
 *
 * @override_id asc_loadalign_unpack4__Rvf8e5m2_PUf8e5m2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) receiving the VL-length expanded data, one
 * source element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 * @param [in] offset Element offset relative to src: the read begins offset fp8_e5m2_t elements after src and src
 * itself is not updated.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous fp8_e4m3fn_t elements from the 32-byte-aligned UB address src, inserts three zero
 * elements after each source element, and writes the VL-length expanded data into dst starting offset elements after
 * src, without updating src.
 *
 * @override_id asc_loadalign_unpack4__Rvf8e4m3_PUf8e4m3_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) receiving the VL-length expanded data, one
 * source element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 * @param [in] offset Element offset relative to src: the read begins offset fp8_e4m3fn_t elements after src and src
 * itself is not updated.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous hifloat8_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and writes the VL-length expanded data into dst starting offset elements after src,
 * without updating src.
 *
 * @override_id asc_loadalign_unpack4__Rvhf8_PUhf8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) receiving the VL-length expanded data, one
 * source element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 * @param [in] offset Element offset relative to src: the read begins offset hifloat8_t elements after src and src
 * itself is not updated.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous fp4x2_e1m2_t elements from the 32-byte-aligned UB address src, inserts three zero
 * elements after each source element, and writes the VL-length expanded data into dst using the element offset held in
 * the addr_reg offset, without updating src.
 *
 * @override_id asc_loadalign_unpack4__Rvf4e1m2x2_PUf4e1m2x2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) receiving the VL-length expanded data, one
 * source element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 * @param [in] offset Address register holding the element offset relative to src; it must be produced by
 * asc_update_addr_reg, and its b8/b16/b32 mode means 1/2/4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack4(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous fp4x2_e2m1_t elements from the 32-byte-aligned UB address src, inserts three zero
 * elements after each source element, and writes the VL-length expanded data into dst using the element offset held in
 * the addr_reg offset, without updating src.
 *
 * @override_id asc_loadalign_unpack4__Rvf4e2m1x2_PUf4e2m1x2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) receiving the VL-length expanded data, one
 * source element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 * @param [in] offset Address register holding the element offset relative to src; it must be produced by
 * asc_update_addr_reg, and its b8/b16/b32 mode means 1/2/4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack4(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous int4b_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and writes the VL-length expanded data into dst using the element offset held in the
 * addr_reg offset, without updating src.
 *
 * @override_id asc_loadalign_unpack4__Rvi4x2_PUi4x2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) receiving the VL-length expanded data, one source
 * element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 * @param [in] offset Address register holding the element offset relative to src; it must be produced by
 * asc_update_addr_reg, and its b8/b16/b32 mode means 1/2/4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_int4x2_t& dst, __ubuf__ int4b_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous uint8_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and writes the VL-length expanded data into dst using the element offset held in the
 * addr_reg offset, without updating src.
 *
 * @override_id asc_loadalign_unpack4__Rvu8_PUu8_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) receiving the VL-length expanded data, one source
 * element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 * @param [in] offset Address register holding the element offset relative to src; it must be produced by
 * asc_update_addr_reg, and its b8/b16/b32 mode means 1/2/4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_uint8_t& dst, __ubuf__ uint8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous int8_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and writes the VL-length expanded data into dst using the element offset held in the
 * addr_reg offset, without updating src.
 *
 * @override_id asc_loadalign_unpack4__Rvi8_PUi8_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) receiving the VL-length expanded data, one source
 * element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 * @param [in] offset Address register holding the element offset relative to src; it must be produced by
 * asc_update_addr_reg, and its b8/b16/b32 mode means 1/2/4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_int8_t& dst, __ubuf__ int8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous fp8_e4m3fn_t elements from the 32-byte-aligned UB address src, inserts three zero
 * elements after each source element, and writes the VL-length expanded data into dst using the element offset held in
 * the addr_reg offset, without updating src.
 *
 * @override_id asc_loadalign_unpack4__Rvf8e4m3_PUf8e4m3_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) receiving the VL-length expanded data, one
 * source element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 * @param [in] offset Address register holding the element offset relative to src; it must be produced by
 * asc_update_addr_reg, and its b8/b16/b32 mode means 1/2/4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack4(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous hifloat8_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and writes the VL-length expanded data into dst using the element offset held in the
 * addr_reg offset, without updating src.
 *
 * @override_id asc_loadalign_unpack4__Rvhf8_PUhf8_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) receiving the VL-length expanded data, one
 * source element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 * @param [in] offset Address register holding the element offset relative to src; it must be produced by
 * asc_update_addr_reg, and its b8/b16/b32 mode means 1/2/4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous fp8_e5m2_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and writes the VL-length expanded data into dst using the element offset held in the
 * addr_reg offset, without updating src.
 *
 * @override_id asc_loadalign_unpack4__Rvf8e5m2_PUf8e5m2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) receiving the VL-length expanded data, one
 * source element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 * @param [in] offset Address register holding the element offset relative to src; it must be produced by
 * asc_update_addr_reg, and its b8/b16/b32 mode means 1/2/4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads VL/4 contiguous fp8_e8m0_t elements from the 32-byte-aligned UB address src, inserts three zero elements
 * after each source element, and writes the VL-length expanded data into dst using the element offset held in the
 * addr_reg offset, without updating src.
 *
 * @override_id asc_loadalign_unpack4__Rvf8e8m0_PUf8e8m0_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) receiving the VL-length expanded data, one
 * source element followed by three zeros per group.
 * @param [in] src Start address in UB of the source data; the actual read address must be 32-byte aligned and must stay
 * inside the UB address space.
 * @param [in] offset Address register holding the element offset relative to src; it must be produced by
 * asc_update_addr_reg, and its b8/b16/b32 mode means 1/2/4 bytes per element.
 */
__simd_callee__ inline void asc_loadalign_unpack4(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of int8_t elements from the 32-byte-aligned src address and broadcasts it across
 * the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvi8_PUi8
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the int8_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_int8_t& dst, __ubuf__ int8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of uint8_t elements from the 32-byte-aligned src address and broadcasts it across
 * the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvu8_PUu8
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the uint8_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_uint8_t& dst, __ubuf__ uint8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of fp4x2_e2m1_t elements from the 32-byte-aligned src address and broadcasts it
 * across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvf4e2m1x2_PUf4e2m1x2
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) that receives the DataBlock broadcast across
 * its full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the fp4x2_e2m1_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of fp4x2_e1m2_t elements from the 32-byte-aligned src address and broadcasts it
 * across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvf4e1m2x2_PUf4e1m2x2
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) that receives the DataBlock broadcast across
 * its full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the fp4x2_e1m2_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of packed int4b_t element pairs from the 32-byte-aligned src address and
 * broadcasts it across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvi4x2_PUi4x2
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the int4b_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_int4x2_t& dst, __ubuf__ int4b_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of fp8_e8m0_t elements from the 32-byte-aligned src address and broadcasts it
 * across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvf8e8m0_PUf8e8m0
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) that receives the DataBlock broadcast across
 * its full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the fp8_e8m0_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of fp8_e5m2_t elements from the 32-byte-aligned src address and broadcasts it
 * across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvf8e5m2_PUf8e5m2
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) that receives the DataBlock broadcast across
 * its full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the fp8_e5m2_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of fp8_e4m3fn_t elements from the 32-byte-aligned src address and broadcasts it
 * across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvf8e4m3_PUf8e4m3
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) that receives the DataBlock broadcast across
 * its full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the fp8_e4m3fn_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of hifloat8_t elements from the 32-byte-aligned src address and broadcasts it
 * across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvhf8_PUhf8
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) that receives the DataBlock broadcast across
 * its full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the hifloat8_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of int16_t elements from the 32-byte-aligned src address and broadcasts it across
 * the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvi16_PUi16
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the int16_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_int16_t& dst, __ubuf__ int16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of uint16_t elements from the 32-byte-aligned src address and broadcasts it across
 * the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvu16_PUu16
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the uint16_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_uint16_t& dst, __ubuf__ uint16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of half elements from the 32-byte-aligned src address and broadcasts it across the
 * whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvf16_PUf16
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) that receives the DataBlock broadcast across its full
 * length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the half source; the actual read address of the 32-byte DataBlock must
 * be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_half& dst, __ubuf__ half* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of bfloat16_t elements from the 32-byte-aligned src address and broadcasts it
 * across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvbf16_PUbf16
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) that receives the DataBlock broadcast across
 * its full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the bfloat16_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of int32_t elements from the 32-byte-aligned src address and broadcasts it across
 * the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvi32_PUi32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int32_t) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the int32_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_int32_t& dst, __ubuf__ int32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of uint32_t elements from the 32-byte-aligned src address and broadcasts it across
 * the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvu32_PUu32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint32_t) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the uint32_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_uint32_t& dst, __ubuf__ uint32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of float elements from the 32-byte-aligned src address and broadcasts it across
 * the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvf32_PUf32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_float) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the float source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_float& dst, __ubuf__ float* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of int8_t elements from src plus offset elements and broadcasts it across the
 * whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvi8_PUi8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the int8_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 * @param [in] offset Element offset relative to the src start address, counted in elements of int8_t; this prototype
 * does not update src and the resulting read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_int8_t& dst, __ubuf__ int8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of uint8_t elements from src plus offset elements and broadcasts it across the
 * whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvu8_PUu8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the uint8_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 * @param [in] offset Element offset relative to the src start address, counted in elements of uint8_t; this prototype
 * does not update src and the resulting read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_uint8_t& dst, __ubuf__ uint8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of fp4x2_e2m1_t elements from src plus offset elements and broadcasts it across
 * the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvf4e2m1x2_PUf4e2m1x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) that receives the DataBlock broadcast across
 * its full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the fp4x2_e2m1_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 * @param [in] offset Element offset relative to the src start address, counted in elements of fp4x2_e2m1_t; this
 * prototype does not update src and the resulting read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of fp4x2_e1m2_t elements from src plus offset elements and broadcasts it across
 * the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvf4e1m2x2_PUf4e1m2x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) that receives the DataBlock broadcast across
 * its full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the fp4x2_e1m2_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 * @param [in] offset Element offset relative to the src start address, counted in elements of fp4x2_e1m2_t; this
 * prototype does not update src and the resulting read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of packed int4b_t element pairs from src plus offset elements and broadcasts it
 * across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvi4x2_PUi4x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the int4b_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 * @param [in] offset Element offset relative to the src start address, counted in elements of int4b_t; this prototype
 * does not update src and the resulting read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_int4x2_t& dst, __ubuf__ int4b_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of fp8_e8m0_t elements from src plus offset elements and broadcasts it across the
 * whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvf8e8m0_PUf8e8m0_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) that receives the DataBlock broadcast across
 * its full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the fp8_e8m0_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 * @param [in] offset Element offset relative to the src start address, counted in elements of fp8_e8m0_t; this
 * prototype does not update src and the resulting read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(
    vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of fp8_e5m2_t elements from src plus offset elements and broadcasts it across the
 * whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvf8e5m2_PUf8e5m2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) that receives the DataBlock broadcast across
 * its full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the fp8_e5m2_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 * @param [in] offset Element offset relative to the src start address, counted in elements of fp8_e5m2_t; this
 * prototype does not update src and the resulting read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(
    vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of fp8_e4m3fn_t elements from src plus offset elements and broadcasts it across
 * the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvf8e4m3_PUf8e4m3_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) that receives the DataBlock broadcast across
 * its full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the fp8_e4m3fn_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 * @param [in] offset Element offset relative to the src start address, counted in elements of fp8_e4m3fn_t; this
 * prototype does not update src and the resulting read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of hifloat8_t elements from src plus offset elements and broadcasts it across the
 * whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvhf8_PUhf8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) that receives the DataBlock broadcast across
 * its full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the hifloat8_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 * @param [in] offset Element offset relative to the src start address, counted in elements of hifloat8_t; this
 * prototype does not update src and the resulting read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(
    vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of int16_t elements from src plus offset elements and broadcasts it across the
 * whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvi16_PUi16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the int16_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 * @param [in] offset Element offset relative to the src start address, counted in elements of int16_t; this prototype
 * does not update src and the resulting read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_int16_t& dst, __ubuf__ int16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of uint16_t elements from src plus offset elements and broadcasts it across the
 * whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvu16_PUu16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the uint16_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 * @param [in] offset Element offset relative to the src start address, counted in elements of uint16_t; this prototype
 * does not update src and the resulting read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_uint16_t& dst, __ubuf__ uint16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of half elements from src plus offset elements and broadcasts it across the whole
 * destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvf16_PUf16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) that receives the DataBlock broadcast across its full
 * length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the half source; the actual read address of the 32-byte DataBlock must
 * be 32-byte aligned.
 * @param [in] offset Element offset relative to the src start address, counted in elements of half; this prototype does
 * not update src and the resulting read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_half& dst, __ubuf__ half* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of bfloat16_t elements from src plus offset elements and broadcasts it across the
 * whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvbf16_PUbf16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) that receives the DataBlock broadcast across
 * its full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the bfloat16_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 * @param [in] offset Element offset relative to the src start address, counted in elements of bfloat16_t; this
 * prototype does not update src and the resulting read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(
    vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of int32_t elements from src plus offset elements and broadcasts it across the
 * whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvi32_PUi32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int32_t) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the int32_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 * @param [in] offset Element offset relative to the src start address, counted in elements of int32_t; this prototype
 * does not update src and the resulting read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_int32_t& dst, __ubuf__ int32_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of uint32_t elements from src plus offset elements and broadcasts it across the
 * whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvu32_PUu32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint32_t) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the uint32_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 * @param [in] offset Element offset relative to the src start address, counted in elements of uint32_t; this prototype
 * does not update src and the resulting read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_uint32_t& dst, __ubuf__ uint32_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of float elements from src plus offset elements and broadcasts it across the whole
 * destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvf32_PUf32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_float) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the float source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 * @param [in] offset Element offset relative to the src start address, counted in elements of float; this prototype
 * does not update src and the resulting read address must stay 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_float& dst, __ubuf__ float* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of fp4x2_e1m2_t elements from src plus the element offset held in the addr_reg and
 * broadcasts it across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvf4e1m2x2_PUf4e1m2x2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) that receives the DataBlock broadcast across
 * its full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the fp4x2_e1m2_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 * @param [in] offset Address register produced by asc_update_addr_reg holding the element offset relative to src; its
 * b8, b16 or b32 mode fixes the element size at 1, 2 or 4 bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of fp4x2_e2m1_t elements from src plus the element offset held in the addr_reg and
 * broadcasts it across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvf4e2m1x2_PUf4e2m1x2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) that receives the DataBlock broadcast across
 * its full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the fp4x2_e2m1_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 * @param [in] offset Address register produced by asc_update_addr_reg holding the element offset relative to src; its
 * b8, b16 or b32 mode fixes the element size at 1, 2 or 4 bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of packed int4b_t element pairs from src plus the element offset held in the
 * addr_reg and broadcasts it across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvi4x2_PUi4x2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the int4b_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 * @param [in] offset Address register produced by asc_update_addr_reg holding the element offset relative to src; its
 * b8, b16 or b32 mode fixes the element size at 1, 2 or 4 bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_int4x2_t& dst, __ubuf__ int4b_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of uint8_t elements from src plus the element offset held in the addr_reg and
 * broadcasts it across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvu8_PUu8_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the uint8_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 * @param [in] offset Address register produced by asc_update_addr_reg holding the element offset relative to src; its
 * b8, b16 or b32 mode fixes the element size at 1, 2 or 4 bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_uint8_t& dst, __ubuf__ uint8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of int8_t elements from src plus the element offset held in the addr_reg and
 * broadcasts it across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvi8_PUi8_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the int8_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 * @param [in] offset Address register produced by asc_update_addr_reg holding the element offset relative to src; its
 * b8, b16 or b32 mode fixes the element size at 1, 2 or 4 bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_int8_t& dst, __ubuf__ int8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of fp8_e4m3fn_t elements from src plus the element offset held in the addr_reg and
 * broadcasts it across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvf8e4m3_PUf8e4m3_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) that receives the DataBlock broadcast across
 * its full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the fp8_e4m3fn_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 * @param [in] offset Address register produced by asc_update_addr_reg holding the element offset relative to src; its
 * b8, b16 or b32 mode fixes the element size at 1, 2 or 4 bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of hifloat8_t elements from src plus the element offset held in the addr_reg and
 * broadcasts it across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvhf8_PUhf8_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) that receives the DataBlock broadcast across
 * its full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the hifloat8_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 * @param [in] offset Address register produced by asc_update_addr_reg holding the element offset relative to src; its
 * b8, b16 or b32 mode fixes the element size at 1, 2 or 4 bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(
    vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of fp8_e5m2_t elements from src plus the element offset held in the addr_reg and
 * broadcasts it across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvf8e5m2_PUf8e5m2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) that receives the DataBlock broadcast across
 * its full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the fp8_e5m2_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 * @param [in] offset Address register produced by asc_update_addr_reg holding the element offset relative to src; its
 * b8, b16 or b32 mode fixes the element size at 1, 2 or 4 bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(
    vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of fp8_e8m0_t elements from src plus the element offset held in the addr_reg and
 * broadcasts it across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvf8e8m0_PUf8e8m0_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) that receives the DataBlock broadcast across
 * its full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the fp8_e8m0_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 * @param [in] offset Address register produced by asc_update_addr_reg holding the element offset relative to src; its
 * b8, b16 or b32 mode fixes the element size at 1, 2 or 4 bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(
    vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of uint16_t elements from src plus the element offset held in the addr_reg and
 * broadcasts it across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvu16_PUu16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the uint16_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 * @param [in] offset Address register produced by asc_update_addr_reg holding the element offset relative to src; its
 * b8, b16 or b32 mode fixes the element size at 1, 2 or 4 bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_uint16_t& dst, __ubuf__ uint16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of int16_t elements from src plus the element offset held in the addr_reg and
 * broadcasts it across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvi16_PUi16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the int16_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 * @param [in] offset Address register produced by asc_update_addr_reg holding the element offset relative to src; its
 * b8, b16 or b32 mode fixes the element size at 1, 2 or 4 bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_int16_t& dst, __ubuf__ int16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of bfloat16_t elements from src plus the element offset held in the addr_reg and
 * broadcasts it across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvbf16_PUbf16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) that receives the DataBlock broadcast across
 * its full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the bfloat16_t source; the actual read address of the 32-byte
 * DataBlock must be 32-byte aligned.
 * @param [in] offset Address register produced by asc_update_addr_reg holding the element offset relative to src; its
 * b8, b16 or b32 mode fixes the element size at 1, 2 or 4 bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(
    vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of half elements from src plus the element offset held in the addr_reg and
 * broadcasts it across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvf16_PUf16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) that receives the DataBlock broadcast across its full
 * length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the half source; the actual read address of the 32-byte DataBlock must
 * be 32-byte aligned.
 * @param [in] offset Address register produced by asc_update_addr_reg holding the element offset relative to src; its
 * b8, b16 or b32 mode fixes the element size at 1, 2 or 4 bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_half& dst, __ubuf__ half* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of uint32_t elements from src plus the element offset held in the addr_reg and
 * broadcasts it across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvu32_PUu32_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint32_t) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the uint32_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 * @param [in] offset Address register produced by asc_update_addr_reg holding the element offset relative to src; its
 * b8, b16 or b32 mode fixes the element size at 1, 2 or 4 bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_uint32_t& dst, __ubuf__ uint32_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of int32_t elements from src plus the element offset held in the addr_reg and
 * broadcasts it across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvi32_PUi32_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int32_t) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the int32_t source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 * @param [in] offset Address register produced by asc_update_addr_reg holding the element offset relative to src; its
 * b8, b16 or b32 mode fixes the element size at 1, 2 or 4 bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_int32_t& dst, __ubuf__ int32_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock of float elements from src plus the element offset held in the addr_reg and
 * broadcasts it across the whole destination vector register dst.
 *
 * @override_id asc_loadalign_brc_datablock__Rvf32_PUf32_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_float) that receives the DataBlock broadcast across its
 * full length; its dtype must match src.
 * @param [in] src Unified Buffer base address of the float source; the actual read address of the 32-byte DataBlock
 * must be 32-byte aligned.
 * @param [in] offset Address register produced by asc_update_addr_reg holding the element offset relative to src; its
 * b8, b16 or b32 mode fixes the element size at 1, 2 or 4 bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock(vector_float& dst, __ubuf__ float* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Broadcasts 8 consecutive int16_t elements read from a 16-byte aligned UB address into one 32-byte DataBlock
 * each and writes the result into a vector data register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__Rvi16_PUi16
 * @archs 3510
 * @param [out] dst Destination vector data register of type int16_t that receives the broadcast values; its dtype must
 * match src and a full VL of data is written.
 * @param [in] src Source UB address of int16_t elements; the effective read address must be 16-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(vector_int16_t& dst, __ubuf__ int16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Broadcasts 8 consecutive uint16_t elements read from a 16-byte aligned UB address into one 32-byte DataBlock
 * each and writes the result into a vector data register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__Rvu16_PUu16
 * @archs 3510
 * @param [out] dst Destination vector data register of type uint16_t that receives the broadcast values; its dtype must
 * match src and a full VL of data is written.
 * @param [in] src Source UB address of uint16_t elements; the effective read address must be 16-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(vector_uint16_t& dst, __ubuf__ uint16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Broadcasts 8 consecutive half elements read from a 16-byte aligned UB address into one 32-byte DataBlock each
 * and writes the result into a vector data register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__Rvf16_PUf16
 * @archs 3510
 * @param [out] dst Destination vector data register of type half that receives the broadcast values; its dtype must
 * match src and a full VL of data is written.
 * @param [in] src Source UB address of half elements; the effective read address must be 16-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(vector_half& dst, __ubuf__ half* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Broadcasts 8 consecutive bfloat16_t elements read from a 16-byte aligned UB address into one 32-byte DataBlock
 * each and writes the result into a vector data register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__Rvbf16_PUbf16
 * @archs 3510
 * @param [out] dst Destination vector data register of type bfloat16_t that receives the broadcast values; its dtype
 * must match src and a full VL of data is written.
 * @param [in] src Source UB address of bfloat16_t elements; the effective read address must be 16-byte aligned, and the
 * 8 consecutive elements read from it are broadcast unchanged.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Broadcasts 8 consecutive int32_t elements read from a 32-byte aligned UB address into one 32-byte DataBlock
 * each and writes the result into a vector data register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__Rvi32_PUi32
 * @archs 3510
 * @param [out] dst Destination vector data register of type int32_t that receives the broadcast values; its dtype must
 * match src and a full VL of data is written.
 * @param [in] src Source UB address of int32_t elements; the effective read address must be 32-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(vector_int32_t& dst, __ubuf__ int32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Broadcasts 8 consecutive uint32_t elements read from a 32-byte aligned UB address into one 32-byte DataBlock
 * each and writes the result into a vector data register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__Rvu32_PUu32
 * @archs 3510
 * @param [out] dst Destination vector data register of type uint32_t that receives the broadcast values; its dtype must
 * match src and a full VL of data is written.
 * @param [in] src Source UB address of uint32_t elements; the effective read address must be 32-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(vector_uint32_t& dst, __ubuf__ uint32_t* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Broadcasts 8 consecutive float elements read from a 32-byte aligned UB address into one 32-byte DataBlock each
 * and writes the result into a vector data register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__Rvf32_PUf32
 * @archs 3510
 * @param [out] dst Destination vector data register of type float that receives the broadcast values; its dtype must
 * match src and a full VL of data is written.
 * @param [in] src Source UB address of float elements; the effective read address must be 32-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(vector_float& dst, __ubuf__ float* src);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Broadcasts 8 consecutive int16_t elements read from a UB address plus a constant element offset into one
 * 32-byte DataBlock each, writing the result into a vector data register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__Rvi16_PUi16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register of type int16_t that receives the broadcast values; its dtype must
 * match src and a full VL of data is written.
 * @param [in] src Source UB address of int16_t elements; the effective read address must be 16-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 * @param [in] offset Constant element offset relative to the src start address; the interface does not update src
 * automatically.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(
    vector_int16_t& dst, __ubuf__ int16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Broadcasts 8 consecutive uint16_t elements read from a UB address plus a constant element offset into one
 * 32-byte DataBlock each, writing the result into a vector data register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__Rvu16_PUu16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register of type uint16_t that receives the broadcast values; its dtype must
 * match src and a full VL of data is written.
 * @param [in] src Source UB address of uint16_t elements; the effective read address must be 16-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 * @param [in] offset Constant element offset relative to the src start address; the interface does not update src
 * automatically.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(
    vector_uint16_t& dst, __ubuf__ uint16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Broadcasts 8 consecutive half elements read from a UB address plus a constant element offset into one 32-byte
 * DataBlock each, writing the result into a vector data register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__Rvf16_PUf16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register of type half that receives the broadcast values; its dtype must
 * match src and a full VL of data is written.
 * @param [in] src Source UB address of half elements; the effective read address must be 16-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 * @param [in] offset Constant element offset relative to the src start address; the interface does not update src
 * automatically.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(vector_half& dst, __ubuf__ half* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Broadcasts 8 consecutive bfloat16_t elements read from a UB address plus a constant element offset into one
 * 32-byte DataBlock each, writing the result into a vector data register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__Rvbf16_PUbf16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register of type bfloat16_t that receives the broadcast values; its dtype
 * must match src and a full VL of data is written.
 * @param [in] src Source UB address of bfloat16_t elements; the effective read address must be 16-byte aligned, and the
 * 8 consecutive elements read from it are broadcast unchanged.
 * @param [in] offset Constant element offset relative to the src start address; the interface does not update src
 * automatically.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(
    vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Broadcasts 8 consecutive int32_t elements read from a UB address plus a constant element offset into one
 * 32-byte DataBlock each, writing the result into a vector data register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__Rvi32_PUi32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register of type int32_t that receives the broadcast values; its dtype must
 * match src and a full VL of data is written.
 * @param [in] src Source UB address of int32_t elements; the effective read address must be 32-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 * @param [in] offset Constant element offset relative to the src start address; the interface does not update src
 * automatically.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(
    vector_int32_t& dst, __ubuf__ int32_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Broadcasts 8 consecutive uint32_t elements read from a UB address plus a constant element offset into one
 * 32-byte DataBlock each, writing the result into a vector data register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__Rvu32_PUu32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register of type uint32_t that receives the broadcast values; its dtype must
 * match src and a full VL of data is written.
 * @param [in] src Source UB address of uint32_t elements; the effective read address must be 32-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 * @param [in] offset Constant element offset relative to the src start address; the interface does not update src
 * automatically.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(
    vector_uint32_t& dst, __ubuf__ uint32_t* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Broadcasts 8 consecutive float elements read from a UB address plus a constant element offset into one 32-byte
 * DataBlock each, writing the result into a vector data register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__Rvf32_PUf32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register of type float that receives the broadcast values; its dtype must
 * match src and a full VL of data is written.
 * @param [in] src Source UB address of float elements; the effective read address must be 32-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 * @param [in] offset Constant element offset relative to the src start address; the interface does not update src
 * automatically.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(vector_float& dst, __ubuf__ float* src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Broadcasts 8 consecutive uint16_t elements read from a UB address plus an addr_reg element offset into one
 * 32-byte DataBlock each, writing the result into a vector data register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__Rvu16_PUu16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register of type uint16_t that receives the broadcast values; its dtype must
 * match src and a full VL of data is written.
 * @param [in] src Source UB address of uint16_t elements; the effective read address must be 16-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 * @param [in] offset Address register holding the element offset relative to src that was produced by
 * asc_update_addr_reg; 2-byte (b16) or 4-byte (b32) elements are addressed, and it is meant for offsets that change
 * with each hardware-loop iteration.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(
    vector_uint16_t& dst, __ubuf__ uint16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Broadcasts 8 consecutive int16_t elements read from a UB address plus an addr_reg element offset into one
 * 32-byte DataBlock each, writing the result into a vector data register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__Rvi16_PUi16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register of type int16_t that receives the broadcast values; its dtype must
 * match src and a full VL of data is written.
 * @param [in] src Source UB address of int16_t elements; the effective read address must be 16-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 * @param [in] offset Address register holding the element offset relative to src that was produced by
 * asc_update_addr_reg; 2-byte (b16) or 4-byte (b32) elements are addressed, and it is meant for offsets that change
 * with each hardware-loop iteration.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(
    vector_int16_t& dst, __ubuf__ int16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Broadcasts 8 consecutive bfloat16_t elements read from a UB address plus an addr_reg element offset into one
 * 32-byte DataBlock each, writing the result into a vector data register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__Rvbf16_PUbf16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register of type bfloat16_t that receives the broadcast values; its dtype
 * must match src and a full VL of data is written.
 * @param [in] src Source UB address of bfloat16_t elements; the effective read address must be 16-byte aligned, and the
 * 8 consecutive elements read from it are broadcast unchanged.
 * @param [in] offset Address register holding the element offset relative to src that was produced by
 * asc_update_addr_reg; 2-byte (b16) or 4-byte (b32) elements are addressed, and it is meant for offsets that change
 * with each hardware-loop iteration.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(
    vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Broadcasts 8 consecutive half elements read from a UB address plus an addr_reg element offset into one 32-byte
 * DataBlock each, writing the result into a vector data register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__Rvf16_PUf16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register of type half that receives the broadcast values; its dtype must
 * match src and a full VL of data is written.
 * @param [in] src Source UB address of half elements; the effective read address must be 16-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 * @param [in] offset Address register holding the element offset relative to src that was produced by
 * asc_update_addr_reg; 2-byte (b16) or 4-byte (b32) elements are addressed, and it is meant for offsets that change
 * with each hardware-loop iteration.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(vector_half& dst, __ubuf__ half* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Broadcasts 8 consecutive uint32_t elements read from a UB address plus an addr_reg element offset into one
 * 32-byte DataBlock each, writing the result into a vector data register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__Rvu32_PUu32_ar
 * @archs 3510
 * @param [out] dst Destination vector data register of type uint32_t that receives the broadcast values; its dtype must
 * match src and a full VL of data is written.
 * @param [in] src Source UB address of uint32_t elements; the effective read address must be 32-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 * @param [in] offset Address register holding the element offset relative to src that was produced by
 * asc_update_addr_reg; 2-byte (b16) or 4-byte (b32) elements are addressed, and it is meant for offsets that change
 * with each hardware-loop iteration.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(
    vector_uint32_t& dst, __ubuf__ uint32_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Broadcasts 8 consecutive int32_t elements read from a UB address plus an addr_reg element offset into one
 * 32-byte DataBlock each, writing the result into a vector data register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__Rvi32_PUi32_ar
 * @archs 3510
 * @param [out] dst Destination vector data register of type int32_t that receives the broadcast values; its dtype must
 * match src and a full VL of data is written.
 * @param [in] src Source UB address of int32_t elements; the effective read address must be 32-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 * @param [in] offset Address register holding the element offset relative to src that was produced by
 * asc_update_addr_reg; 2-byte (b16) or 4-byte (b32) elements are addressed, and it is meant for offsets that change
 * with each hardware-loop iteration.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(
    vector_int32_t& dst, __ubuf__ int32_t* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Broadcasts 8 consecutive float elements read from a UB address plus an addr_reg element offset into one
 * 32-byte DataBlock each, writing the result into a vector data register.
 *
 * @override_id asc_loadalign_brc_elem2datablock__Rvf32_PUf32_ar
 * @archs 3510
 * @param [out] dst Destination vector data register of type float that receives the broadcast values; its dtype must
 * match src and a full VL of data is written.
 * @param [in] src Source UB address of float elements; the effective read address must be 32-byte aligned, and the 8
 * consecutive elements read from it are broadcast unchanged.
 * @param [in] offset Address register holding the element offset relative to src that was produced by
 * asc_update_addr_reg; 2-byte (b16) or 4-byte (b32) elements are addressed, and it is meant for offsets that change
 * with each hardware-loop iteration.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock(vector_float& dst, __ubuf__ float* src, addr_reg offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads a single int8_t element from the dtype aligned UB address src, broadcasts it across every lane of the
 * vector_int8_t register dst, and then advances src by offset int8_t elements.
 *
 * @override_id asc_loadalign_brc_elem_postupdate__Rvi8_RPUi8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) whose lanes are all filled with the broadcast copy
 * of the single int8_t element read from src.
 * @param [in,out] src Source UB address holding the int8_t element; the address actually read must be aligned to
 * sizeof(int8_t) bytes and after the transfer the pointer is advanced by offset * sizeof(int8_t) bytes.
 * @param [in] offset Post Update step counted in int8_t elements; src grows by offset * sizeof(int8_t) bytes and the
 * updated address must still satisfy int8_t alignment.
 */
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(
    vector_int8_t& dst, __ubuf__ int8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads a single uint8_t element from the dtype aligned UB address src, broadcasts it across every lane of the
 * vector_uint8_t register dst, and then advances src by offset uint8_t elements.
 *
 * @override_id asc_loadalign_brc_elem_postupdate__Rvu8_RPUu8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) whose lanes are all filled with the broadcast copy
 * of the single uint8_t element read from src.
 * @param [in,out] src Source UB address holding the uint8_t element; the address actually read must be aligned to
 * sizeof(uint8_t) bytes and after the transfer the pointer is advanced by offset * sizeof(uint8_t) bytes.
 * @param [in] offset Post Update step counted in uint8_t elements; src grows by offset * sizeof(uint8_t) bytes and the
 * updated address must still satisfy uint8_t alignment.
 */
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(
    vector_uint8_t& dst, __ubuf__ uint8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads a single fp4x2_e2m1_t element from the dtype aligned UB address src, broadcasts it across every lane of
 * the vector_fp4x2_e2m1_t register dst, and then advances src by offset fp4x2_e2m1_t elements.
 *
 * @override_id asc_loadalign_brc_elem_postupdate__Rvf4e2m1x2_RPUf4e2m1x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) whose lanes are all filled with the broadcast
 * copy of the single fp4x2_e2m1_t element read from src.
 * @param [in,out] src Source UB address holding the fp4x2_e2m1_t element; the address actually read must be aligned to
 * sizeof(fp4x2_e2m1_t) bytes and after the transfer the pointer is advanced by offset * sizeof(fp4x2_e2m1_t) bytes.
 * @param [in] offset Post Update step counted in fp4x2_e2m1_t elements; src grows by offset * sizeof(fp4x2_e2m1_t)
 * bytes and the updated address must still satisfy fp4x2_e2m1_t alignment.
 */
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads a single fp4x2_e1m2_t element from the dtype aligned UB address src, broadcasts it across every lane of
 * the vector_fp4x2_e1m2_t register dst, and then advances src by offset fp4x2_e1m2_t elements.
 *
 * @override_id asc_loadalign_brc_elem_postupdate__Rvf4e1m2x2_RPUf4e1m2x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) whose lanes are all filled with the broadcast
 * copy of the single fp4x2_e1m2_t element read from src.
 * @param [in,out] src Source UB address holding the fp4x2_e1m2_t element; the address actually read must be aligned to
 * sizeof(fp4x2_e1m2_t) bytes and after the transfer the pointer is advanced by offset * sizeof(fp4x2_e1m2_t) bytes.
 * @param [in] offset Post Update step counted in fp4x2_e1m2_t elements; src grows by offset * sizeof(fp4x2_e1m2_t)
 * bytes and the updated address must still satisfy fp4x2_e1m2_t alignment.
 */
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads a single fp8_e8m0_t element from the dtype aligned UB address src, broadcasts it across every lane of
 * the vector_fp8_e8m0_t register dst, and then advances src by offset fp8_e8m0_t elements.
 *
 * @override_id asc_loadalign_brc_elem_postupdate__Rvf8e8m0_RPUf8e8m0_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) whose lanes are all filled with the broadcast
 * copy of the single fp8_e8m0_t element read from src.
 * @param [in,out] src Source UB address holding the fp8_e8m0_t element; the address actually read must be aligned to
 * sizeof(fp8_e8m0_t) bytes and after the transfer the pointer is advanced by offset * sizeof(fp8_e8m0_t) bytes.
 * @param [in] offset Post Update step counted in fp8_e8m0_t elements; src grows by offset * sizeof(fp8_e8m0_t) bytes
 * and the updated address must still satisfy fp8_e8m0_t alignment.
 */
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(
    vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads a single int4b_t element from the dtype aligned UB address src, broadcasts it across every lane of the
 * vector_int4x2_t register dst, and then advances src by offset int4b_t elements.
 *
 * @override_id asc_loadalign_brc_elem_postupdate__Rvi4x2_RPUi4x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) whose lanes are all filled with the broadcast
 * copy of the single int4b_t element read from src.
 * @param [in,out] src Source UB address holding the int4b_t element; the address actually read must be aligned to
 * sizeof(int4b_t) bytes and after the transfer the pointer is advanced by offset * sizeof(int4b_t) bytes.
 * @param [in] offset Post Update step counted in int4b_t elements; src grows by offset * sizeof(int4b_t) bytes and the
 * updated address must still satisfy int4b_t alignment.
 */
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(
    vector_int4x2_t& dst, __ubuf__ int4b_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads a single fp8_e5m2_t element from the dtype aligned UB address src, broadcasts it across every lane of
 * the vector_fp8_e5m2_t register dst, and then advances src by offset fp8_e5m2_t elements.
 *
 * @override_id asc_loadalign_brc_elem_postupdate__Rvf8e5m2_RPUf8e5m2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) whose lanes are all filled with the broadcast
 * copy of the single fp8_e5m2_t element read from src.
 * @param [in,out] src Source UB address holding the fp8_e5m2_t element; the address actually read must be aligned to
 * sizeof(fp8_e5m2_t) bytes and after the transfer the pointer is advanced by offset * sizeof(fp8_e5m2_t) bytes.
 * @param [in] offset Post Update step counted in fp8_e5m2_t elements; src grows by offset * sizeof(fp8_e5m2_t) bytes
 * and the updated address must still satisfy fp8_e5m2_t alignment.
 */
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(
    vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads a single fp8_e4m3fn_t element from the dtype aligned UB address src, broadcasts it across every lane of
 * the vector_fp8_e4m3fn_t register dst, and then advances src by offset fp8_e4m3fn_t elements.
 *
 * @override_id asc_loadalign_brc_elem_postupdate__Rvf8e4m3_RPUf8e4m3_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) whose lanes are all filled with the broadcast
 * copy of the single fp8_e4m3fn_t element read from src.
 * @param [in,out] src Source UB address holding the fp8_e4m3fn_t element; the address actually read must be aligned to
 * sizeof(fp8_e4m3fn_t) bytes and after the transfer the pointer is advanced by offset * sizeof(fp8_e4m3fn_t) bytes.
 * @param [in] offset Post Update step counted in fp8_e4m3fn_t elements; src grows by offset * sizeof(fp8_e4m3fn_t)
 * bytes and the updated address must still satisfy fp8_e4m3fn_t alignment.
 */
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads a single hifloat8_t element from the dtype aligned UB address src, broadcasts it across every lane of
 * the vector_hifloat8_t register dst, and then advances src by offset hifloat8_t elements.
 *
 * @override_id asc_loadalign_brc_elem_postupdate__Rvhf8_RPUhf8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) whose lanes are all filled with the broadcast
 * copy of the single hifloat8_t element read from src.
 * @param [in,out] src Source UB address holding the hifloat8_t element; the address actually read must be aligned to
 * sizeof(hifloat8_t) bytes and after the transfer the pointer is advanced by offset * sizeof(hifloat8_t) bytes.
 * @param [in] offset Post Update step counted in hifloat8_t elements; src grows by offset * sizeof(hifloat8_t) bytes
 * and the updated address must still satisfy hifloat8_t alignment.
 */
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(
    vector_hifloat8_t& dst, __ubuf__ hifloat8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads a single int16_t element from the dtype aligned UB address src, broadcasts it across every lane of the
 * vector_int16_t register dst, and then advances src by offset int16_t elements.
 *
 * @override_id asc_loadalign_brc_elem_postupdate__Rvi16_RPUi16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) whose lanes are all filled with the broadcast copy
 * of the single int16_t element read from src.
 * @param [in,out] src Source UB address holding the int16_t element; the address actually read must be aligned to
 * sizeof(int16_t) bytes and after the transfer the pointer is advanced by offset * sizeof(int16_t) bytes.
 * @param [in] offset Post Update step counted in int16_t elements; src grows by offset * sizeof(int16_t) bytes and the
 * updated address must still satisfy int16_t alignment.
 */
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(
    vector_int16_t& dst, __ubuf__ int16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads a single uint16_t element from the dtype aligned UB address src, broadcasts it across every lane of the
 * vector_uint16_t register dst, and then advances src by offset uint16_t elements.
 *
 * @override_id asc_loadalign_brc_elem_postupdate__Rvu16_RPUu16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) whose lanes are all filled with the broadcast
 * copy of the single uint16_t element read from src.
 * @param [in,out] src Source UB address holding the uint16_t element; the address actually read must be aligned to
 * sizeof(uint16_t) bytes and after the transfer the pointer is advanced by offset * sizeof(uint16_t) bytes.
 * @param [in] offset Post Update step counted in uint16_t elements; src grows by offset * sizeof(uint16_t) bytes and
 * the updated address must still satisfy uint16_t alignment.
 */
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(
    vector_uint16_t& dst, __ubuf__ uint16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads a single half element from the dtype aligned UB address src, broadcasts it across every lane of the
 * vector_half register dst, and then advances src by offset half elements.
 *
 * @override_id asc_loadalign_brc_elem_postupdate__Rvf16_RPUf16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) whose lanes are all filled with the broadcast copy of
 * the single half element read from src.
 * @param [in,out] src Source UB address holding the half element; the address actually read must be aligned to
 * sizeof(half) bytes and after the transfer the pointer is advanced by offset * sizeof(half) bytes.
 * @param [in] offset Post Update step counted in half elements; src grows by offset * sizeof(half) bytes and the
 * updated address must still satisfy half alignment.
 */
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(vector_half& dst, __ubuf__ half*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads a single bfloat16_t element from the dtype aligned UB address src, broadcasts it across every lane of
 * the vector_bfloat16_t register dst, and then advances src by offset bfloat16_t elements.
 *
 * @override_id asc_loadalign_brc_elem_postupdate__Rvbf16_RPUbf16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) whose lanes are all filled with the broadcast
 * copy of the single bfloat16_t element read from src.
 * @param [in,out] src Source UB address holding the bfloat16_t element; the address actually read must be aligned to
 * sizeof(bfloat16_t) bytes and after the transfer the pointer is advanced by offset * sizeof(bfloat16_t) bytes.
 * @param [in] offset Post Update step counted in bfloat16_t elements; src grows by offset * sizeof(bfloat16_t) bytes
 * and the updated address must still satisfy bfloat16_t alignment.
 */
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(
    vector_bfloat16_t& dst, __ubuf__ bfloat16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads a single int32_t element from the dtype aligned UB address src, broadcasts it across every lane of the
 * vector_int32_t register dst, and then advances src by offset int32_t elements.
 *
 * @override_id asc_loadalign_brc_elem_postupdate__Rvi32_RPUi32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int32_t) whose lanes are all filled with the broadcast copy
 * of the single int32_t element read from src.
 * @param [in,out] src Source UB address holding the int32_t element; the address actually read must be aligned to
 * sizeof(int32_t) bytes and after the transfer the pointer is advanced by offset * sizeof(int32_t) bytes.
 * @param [in] offset Post Update step counted in int32_t elements; src grows by offset * sizeof(int32_t) bytes and the
 * updated address must still satisfy int32_t alignment.
 */
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(
    vector_int32_t& dst, __ubuf__ int32_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads a single uint32_t element from the dtype aligned UB address src, broadcasts it across every lane of the
 * vector_uint32_t register dst, and then advances src by offset uint32_t elements.
 *
 * @override_id asc_loadalign_brc_elem_postupdate__Rvu32_RPUu32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint32_t) whose lanes are all filled with the broadcast
 * copy of the single uint32_t element read from src.
 * @param [in,out] src Source UB address holding the uint32_t element; the address actually read must be aligned to
 * sizeof(uint32_t) bytes and after the transfer the pointer is advanced by offset * sizeof(uint32_t) bytes.
 * @param [in] offset Post Update step counted in uint32_t elements; src grows by offset * sizeof(uint32_t) bytes and
 * the updated address must still satisfy uint32_t alignment.
 */
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(
    vector_uint32_t& dst, __ubuf__ uint32_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads a single float element from the dtype aligned UB address src, broadcasts it across every lane of the
 * vector_float register dst, and then advances src by offset float elements.
 *
 * @override_id asc_loadalign_brc_elem_postupdate__Rvf32_RPUf32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_float) whose lanes are all filled with the broadcast copy
 * of the single float element read from src.
 * @param [in,out] src Source UB address holding the float element; the address actually read must be aligned to
 * sizeof(float) bytes and after the transfer the pointer is advanced by offset * sizeof(float) bytes.
 * @param [in] offset Post Update step counted in float elements; src grows by offset * sizeof(float) bytes and the
 * updated address must still satisfy float alignment.
 */
__simd_callee__ inline void asc_loadalign_brc_elem_postupdate(vector_float& dst, __ubuf__ float*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/4 consecutive int8_t elements from a 32-byte-aligned UB address, expands each source element into
 * four with three zero elements appended, writes the resulting VL data to the destination vector register and
 * post-updates the source pointer.
 *
 * @override_id asc_loadalign_unpack4_postupdate__Rvi8_RPUi8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) that receives the VL-length unpacked data; its
 * element type must match src and each source element is followed by three zero elements.
 * @param [in,out] src Source UB pointer to the int8_t data, passed by reference: the load reads at the current address,
 * which must be 32-byte aligned, and the pointer is advanced afterwards by offset elements.
 * @param [in] offset Post-update step in elements: after the load, src advances by offset * sizeof(dtype) bytes, and
 * the next read address must still be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack4_postupdate(vector_int8_t& dst, __ubuf__ int8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/4 consecutive uint8_t elements from a 32-byte-aligned UB address, expands each source element into
 * four with three zero elements appended, writes the resulting VL data to the destination vector register and
 * post-updates the source pointer.
 *
 * @override_id asc_loadalign_unpack4_postupdate__Rvu8_RPUu8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) that receives the VL-length unpacked data; its
 * element type must match src and each source element is followed by three zero elements.
 * @param [in,out] src Source UB pointer to the uint8_t data, passed by reference: the load reads at the current
 * address, which must be 32-byte aligned, and the pointer is advanced afterwards by offset elements.
 * @param [in] offset Post-update step in elements: after the load, src advances by offset * sizeof(dtype) bytes, and
 * the next read address must still be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack4_postupdate(
    vector_uint8_t& dst, __ubuf__ uint8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/4 consecutive fp4x2_e2m1_t elements from a 32-byte-aligned UB address, expands each source element
 * into four with three zero elements appended, writes the resulting VL data to the destination vector register and
 * post-updates the source pointer.
 *
 * @override_id asc_loadalign_unpack4_postupdate__Rvf4e2m1x2_RPUf4e2m1x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) that receives the VL-length unpacked data;
 * its element type must match src and each source element is followed by three zero elements.
 * @param [in,out] src Source UB pointer to the fp4x2_e2m1_t data, passed by reference: the load reads at the current
 * address, which must be 32-byte aligned, and the pointer is advanced afterwards by offset elements.
 * @param [in] offset Post-update step in elements: after the load, src advances by offset * sizeof(dtype) bytes, and
 * the next read address must still be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack4_postupdate(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/4 consecutive fp4x2_e1m2_t elements from a 32-byte-aligned UB address, expands each source element
 * into four with three zero elements appended, writes the resulting VL data to the destination vector register and
 * post-updates the source pointer.
 *
 * @override_id asc_loadalign_unpack4_postupdate__Rvf4e1m2x2_RPUf4e1m2x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) that receives the VL-length unpacked data;
 * its element type must match src and each source element is followed by three zero elements.
 * @param [in,out] src Source UB pointer to the fp4x2_e1m2_t data, passed by reference: the load reads at the current
 * address, which must be 32-byte aligned, and the pointer is advanced afterwards by offset elements.
 * @param [in] offset Post-update step in elements: after the load, src advances by offset * sizeof(dtype) bytes, and
 * the next read address must still be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack4_postupdate(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/4 consecutive fp8_e8m0_t elements from a 32-byte-aligned UB address, expands each source element into
 * four with three zero elements appended, writes the resulting VL data to the destination vector register and
 * post-updates the source pointer.
 *
 * @override_id asc_loadalign_unpack4_postupdate__Rvf8e8m0_RPUf8e8m0_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) that receives the VL-length unpacked data; its
 * element type must match src and each source element is followed by three zero elements.
 * @param [in,out] src Source UB pointer to the fp8_e8m0_t data, passed by reference: the load reads at the current
 * address, which must be 32-byte aligned, and the pointer is advanced afterwards by offset elements.
 * @param [in] offset Post-update step in elements: after the load, src advances by offset * sizeof(dtype) bytes, and
 * the next read address must still be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack4_postupdate(
    vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/4 consecutive int4b_t elements from a 32-byte-aligned UB address, expands each source element into
 * four with three zero elements appended, writes the resulting VL data to the destination vector register and
 * post-updates the source pointer.
 *
 * @override_id asc_loadalign_unpack4_postupdate__Rvi4x2_RPUi4x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) that receives the VL-length unpacked data; its
 * element type must match src and each source element is followed by three zero elements.
 * @param [in,out] src Source UB pointer to the int4b_t data, passed by reference: the load reads at the current
 * address, which must be 32-byte aligned, and the pointer is advanced afterwards by offset elements.
 * @param [in] offset Post-update step in elements: after the load, src advances by offset * sizeof(dtype) bytes, and
 * the next read address must still be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack4_postupdate(
    vector_int4x2_t& dst, __ubuf__ int4b_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/4 consecutive fp8_e5m2_t elements from a 32-byte-aligned UB address, expands each source element into
 * four with three zero elements appended, writes the resulting VL data to the destination vector register and
 * post-updates the source pointer.
 *
 * @override_id asc_loadalign_unpack4_postupdate__Rvf8e5m2_RPUf8e5m2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) that receives the VL-length unpacked data; its
 * element type must match src and each source element is followed by three zero elements.
 * @param [in,out] src Source UB pointer to the fp8_e5m2_t data, passed by reference: the load reads at the current
 * address, which must be 32-byte aligned, and the pointer is advanced afterwards by offset elements.
 * @param [in] offset Post-update step in elements: after the load, src advances by offset * sizeof(dtype) bytes, and
 * the next read address must still be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack4_postupdate(
    vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/4 consecutive fp8_e4m3fn_t elements from a 32-byte-aligned UB address, expands each source element
 * into four with three zero elements appended, writes the resulting VL data to the destination vector register and
 * post-updates the source pointer.
 *
 * @override_id asc_loadalign_unpack4_postupdate__Rvf8e4m3_RPUf8e4m3_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) that receives the VL-length unpacked data;
 * its element type must match src and each source element is followed by three zero elements.
 * @param [in,out] src Source UB pointer to the fp8_e4m3fn_t data, passed by reference: the load reads at the current
 * address, which must be 32-byte aligned, and the pointer is advanced afterwards by offset elements.
 * @param [in] offset Post-update step in elements: after the load, src advances by offset * sizeof(dtype) bytes, and
 * the next read address must still be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack4_postupdate(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads VL/4 consecutive hifloat8_t elements from a 32-byte-aligned UB address, expands each source element into
 * four with three zero elements appended, writes the resulting VL data to the destination vector register and
 * post-updates the source pointer.
 *
 * @override_id asc_loadalign_unpack4_postupdate__Rvhf8_RPUhf8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) that receives the VL-length unpacked data; its
 * element type must match src and each source element is followed by three zero elements.
 * @param [in,out] src Source UB pointer to the hifloat8_t data, passed by reference: the load reads at the current
 * address, which must be 32-byte aligned, and the pointer is advanced afterwards by offset elements.
 * @param [in] offset Post-update step in elements: after the load, src advances by offset * sizeof(dtype) bytes, and
 * the next read address must still be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_unpack4_postupdate(
    vector_hifloat8_t& dst, __ubuf__ hifloat8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock from the 32-byte aligned UB address src, broadcasts it across every lane of the
 * vector_int8_t register dst, and then advances src by offset int8_t elements.
 *
 * @override_id asc_loadalign_brc_datablock_postupdate__Rvi8_RPUi8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) whose lanes all receive the broadcast copy of the
 * 32-byte DataBlock read from src; the element type must match src (int8_t).
 * @param [in,out] src Source UB address of the int8_t DataBlock; the address actually read must be 32-byte aligned, and
 * after the transfer the pointer is advanced by offset * sizeof(int8_t) bytes.
 * @param [in] offset Post Update step counted in int8_t elements; after the load src grows by offset * sizeof(int8_t)
 * bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock_postupdate(
    vector_int8_t& dst, __ubuf__ int8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock from the 32-byte aligned UB address src, broadcasts it across every lane of the
 * vector_uint8_t register dst, and then advances src by offset uint8_t elements.
 *
 * @override_id asc_loadalign_brc_datablock_postupdate__Rvu8_RPUu8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) whose lanes all receive the broadcast copy of the
 * 32-byte DataBlock read from src; the element type must match src (uint8_t).
 * @param [in,out] src Source UB address of the uint8_t DataBlock; the address actually read must be 32-byte aligned,
 * and after the transfer the pointer is advanced by offset * sizeof(uint8_t) bytes.
 * @param [in] offset Post Update step counted in uint8_t elements; after the load src grows by offset * sizeof(uint8_t)
 * bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock_postupdate(
    vector_uint8_t& dst, __ubuf__ uint8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock from the 32-byte aligned UB address src, broadcasts it across every lane of the
 * vector_fp4x2_e2m1_t register dst, and then advances src by offset fp4x2_e2m1_t elements.
 *
 * @override_id asc_loadalign_brc_datablock_postupdate__Rvf4e2m1x2_RPUf4e2m1x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) whose lanes all receive the broadcast copy of
 * the 32-byte DataBlock read from src; the element type must match src (fp4x2_e2m1_t).
 * @param [in,out] src Source UB address of the fp4x2_e2m1_t DataBlock; the address actually read must be 32-byte
 * aligned, and after the transfer the pointer is advanced by offset * sizeof(fp4x2_e2m1_t) bytes.
 * @param [in] offset Post Update step counted in fp4x2_e2m1_t elements; after the load src grows by offset *
 * sizeof(fp4x2_e2m1_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock_postupdate(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock from the 32-byte aligned UB address src, broadcasts it across every lane of the
 * vector_fp4x2_e1m2_t register dst, and then advances src by offset fp4x2_e1m2_t elements.
 *
 * @override_id asc_loadalign_brc_datablock_postupdate__Rvf4e1m2x2_RPUf4e1m2x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) whose lanes all receive the broadcast copy of
 * the 32-byte DataBlock read from src; the element type must match src (fp4x2_e1m2_t).
 * @param [in,out] src Source UB address of the fp4x2_e1m2_t DataBlock; the address actually read must be 32-byte
 * aligned, and after the transfer the pointer is advanced by offset * sizeof(fp4x2_e1m2_t) bytes.
 * @param [in] offset Post Update step counted in fp4x2_e1m2_t elements; after the load src grows by offset *
 * sizeof(fp4x2_e1m2_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock_postupdate(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock from the 32-byte aligned UB address src, broadcasts it across every lane of the
 * vector_fp8_e8m0_t register dst, and then advances src by offset fp8_e8m0_t elements.
 *
 * @override_id asc_loadalign_brc_datablock_postupdate__Rvf8e8m0_RPUf8e8m0_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) whose lanes all receive the broadcast copy of
 * the 32-byte DataBlock read from src; the element type must match src (fp8_e8m0_t).
 * @param [in,out] src Source UB address of the fp8_e8m0_t DataBlock; the address actually read must be 32-byte aligned,
 * and after the transfer the pointer is advanced by offset * sizeof(fp8_e8m0_t) bytes.
 * @param [in] offset Post Update step counted in fp8_e8m0_t elements; after the load src grows by offset *
 * sizeof(fp8_e8m0_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock_postupdate(
    vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock from the 32-byte aligned UB address src, broadcasts it across every lane of the
 * vector_int4x2_t register dst, and then advances src by offset int4b_t elements.
 *
 * @override_id asc_loadalign_brc_datablock_postupdate__Rvi4x2_RPUi4x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) whose lanes all receive the broadcast copy of the
 * 32-byte DataBlock read from src; the element type must match src (int4b_t).
 * @param [in,out] src Source UB address of the int4b_t DataBlock; the address actually read must be 32-byte aligned,
 * and after the transfer the pointer is advanced by offset * sizeof(int4b_t) bytes.
 * @param [in] offset Post Update step counted in int4b_t elements; after the load src grows by offset * sizeof(int4b_t)
 * bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock_postupdate(
    vector_int4x2_t& dst, __ubuf__ int4b_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock from the 32-byte aligned UB address src, broadcasts it across every lane of the
 * vector_fp8_e5m2_t register dst, and then advances src by offset fp8_e5m2_t elements.
 *
 * @override_id asc_loadalign_brc_datablock_postupdate__Rvf8e5m2_RPUf8e5m2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) whose lanes all receive the broadcast copy of
 * the 32-byte DataBlock read from src; the element type must match src (fp8_e5m2_t).
 * @param [in,out] src Source UB address of the fp8_e5m2_t DataBlock; the address actually read must be 32-byte aligned,
 * and after the transfer the pointer is advanced by offset * sizeof(fp8_e5m2_t) bytes.
 * @param [in] offset Post Update step counted in fp8_e5m2_t elements; after the load src grows by offset *
 * sizeof(fp8_e5m2_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock_postupdate(
    vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock from the 32-byte aligned UB address src, broadcasts it across every lane of the
 * vector_fp8_e4m3fn_t register dst, and then advances src by offset fp8_e4m3fn_t elements.
 *
 * @override_id asc_loadalign_brc_datablock_postupdate__Rvf8e4m3_RPUf8e4m3_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) whose lanes all receive the broadcast copy of
 * the 32-byte DataBlock read from src; the element type must match src (fp8_e4m3fn_t).
 * @param [in,out] src Source UB address of the fp8_e4m3fn_t DataBlock; the address actually read must be 32-byte
 * aligned, and after the transfer the pointer is advanced by offset * sizeof(fp8_e4m3fn_t) bytes.
 * @param [in] offset Post Update step counted in fp8_e4m3fn_t elements; after the load src grows by offset *
 * sizeof(fp8_e4m3fn_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock_postupdate(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock from the 32-byte aligned UB address src, broadcasts it across every lane of the
 * vector_hifloat8_t register dst, and then advances src by offset hifloat8_t elements.
 *
 * @override_id asc_loadalign_brc_datablock_postupdate__Rvhf8_RPUhf8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) whose lanes all receive the broadcast copy of
 * the 32-byte DataBlock read from src; the element type must match src (hifloat8_t).
 * @param [in,out] src Source UB address of the hifloat8_t DataBlock; the address actually read must be 32-byte aligned,
 * and after the transfer the pointer is advanced by offset * sizeof(hifloat8_t) bytes.
 * @param [in] offset Post Update step counted in hifloat8_t elements; after the load src grows by offset *
 * sizeof(hifloat8_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock_postupdate(
    vector_hifloat8_t& dst, __ubuf__ hifloat8_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock from the 32-byte aligned UB address src, broadcasts it across every lane of the
 * vector_int16_t register dst, and then advances src by offset int16_t elements.
 *
 * @override_id asc_loadalign_brc_datablock_postupdate__Rvi16_RPUi16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) whose lanes all receive the broadcast copy of the
 * 32-byte DataBlock read from src; the element type must match src (int16_t).
 * @param [in,out] src Source UB address of the int16_t DataBlock; the address actually read must be 32-byte aligned,
 * and after the transfer the pointer is advanced by offset * sizeof(int16_t) bytes.
 * @param [in] offset Post Update step counted in int16_t elements; after the load src grows by offset * sizeof(int16_t)
 * bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock_postupdate(
    vector_int16_t& dst, __ubuf__ int16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock from the 32-byte aligned UB address src, broadcasts it across every lane of the
 * vector_uint16_t register dst, and then advances src by offset uint16_t elements.
 *
 * @override_id asc_loadalign_brc_datablock_postupdate__Rvu16_RPUu16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) whose lanes all receive the broadcast copy of the
 * 32-byte DataBlock read from src; the element type must match src (uint16_t).
 * @param [in,out] src Source UB address of the uint16_t DataBlock; the address actually read must be 32-byte aligned,
 * and after the transfer the pointer is advanced by offset * sizeof(uint16_t) bytes.
 * @param [in] offset Post Update step counted in uint16_t elements; after the load src grows by offset *
 * sizeof(uint16_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock_postupdate(
    vector_uint16_t& dst, __ubuf__ uint16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock from the 32-byte aligned UB address src, broadcasts it across every lane of the
 * vector_half register dst, and then advances src by offset half elements.
 *
 * @override_id asc_loadalign_brc_datablock_postupdate__Rvf16_RPUf16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) whose lanes all receive the broadcast copy of the
 * 32-byte DataBlock read from src; the element type must match src (half).
 * @param [in,out] src Source UB address of the half DataBlock; the address actually read must be 32-byte aligned, and
 * after the transfer the pointer is advanced by offset * sizeof(half) bytes.
 * @param [in] offset Post Update step counted in half elements; after the load src grows by offset * sizeof(half) bytes
 * and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock_postupdate(
    vector_half& dst, __ubuf__ half*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock from the 32-byte aligned UB address src, broadcasts it across every lane of the
 * vector_bfloat16_t register dst, and then advances src by offset bfloat16_t elements.
 *
 * @override_id asc_loadalign_brc_datablock_postupdate__Rvbf16_RPUbf16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) whose lanes all receive the broadcast copy of
 * the 32-byte DataBlock read from src; the element type must match src (bfloat16_t).
 * @param [in,out] src Source UB address of the bfloat16_t DataBlock; the address actually read must be 32-byte aligned,
 * and after the transfer the pointer is advanced by offset * sizeof(bfloat16_t) bytes.
 * @param [in] offset Post Update step counted in bfloat16_t elements; after the load src grows by offset *
 * sizeof(bfloat16_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock_postupdate(
    vector_bfloat16_t& dst, __ubuf__ bfloat16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock from the 32-byte aligned UB address src, broadcasts it across every lane of the
 * vector_int32_t register dst, and then advances src by offset int32_t elements.
 *
 * @override_id asc_loadalign_brc_datablock_postupdate__Rvi32_RPUi32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int32_t) whose lanes all receive the broadcast copy of the
 * 32-byte DataBlock read from src; the element type must match src (int32_t).
 * @param [in,out] src Source UB address of the int32_t DataBlock; the address actually read must be 32-byte aligned,
 * and after the transfer the pointer is advanced by offset * sizeof(int32_t) bytes.
 * @param [in] offset Post Update step counted in int32_t elements; after the load src grows by offset * sizeof(int32_t)
 * bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock_postupdate(
    vector_int32_t& dst, __ubuf__ int32_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock from the 32-byte aligned UB address src, broadcasts it across every lane of the
 * vector_uint32_t register dst, and then advances src by offset uint32_t elements.
 *
 * @override_id asc_loadalign_brc_datablock_postupdate__Rvu32_RPUu32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint32_t) whose lanes all receive the broadcast copy of the
 * 32-byte DataBlock read from src; the element type must match src (uint32_t).
 * @param [in,out] src Source UB address of the uint32_t DataBlock; the address actually read must be 32-byte aligned,
 * and after the transfer the pointer is advanced by offset * sizeof(uint32_t) bytes.
 * @param [in] offset Post Update step counted in uint32_t elements; after the load src grows by offset *
 * sizeof(uint32_t) bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock_postupdate(
    vector_uint32_t& dst, __ubuf__ uint32_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Reads one 32-byte DataBlock from the 32-byte aligned UB address src, broadcasts it across every lane of the
 * vector_float register dst, and then advances src by offset float elements.
 *
 * @override_id asc_loadalign_brc_datablock_postupdate__Rvf32_RPUf32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_float) whose lanes all receive the broadcast copy of the
 * 32-byte DataBlock read from src; the element type must match src (float).
 * @param [in,out] src Source UB address of the float DataBlock; the address actually read must be 32-byte aligned, and
 * after the transfer the pointer is advanced by offset * sizeof(float) bytes.
 * @param [in] offset Post Update step counted in float elements; after the load src grows by offset * sizeof(float)
 * bytes and the updated address must again be 32-byte aligned.
 */
__simd_callee__ inline void asc_loadalign_brc_datablock_postupdate(
    vector_float& dst, __ubuf__ float*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads 8 consecutive int16_t elements from the aligned source address src, broadcasts each into one 32-byte
 * DataBlock of the dst register, then advances src by offset elements (post-update).
 *
 * @override_id asc_loadalign_brc_elem2datablock_postupdate__Rvi16_RPUi16_i32
 * @archs 3510
 * @param [out] dst Destination vector register of type vector_int16_t; it receives a full vector length of broadcast
 * data, one int16_t element per 32-byte DataBlock.
 * @param [in,out] src Source Unified Buffer pointer to int16_t data, read from the current address and then updated in
 * place to the post-update address; the b16 type requires 16-byte alignment before and after the update.
 * @param [in] offset Post-update step in elements: after the load, src advances by offset * sizeof(int16_t) = offset *
 * 2 bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock_postupdate(
    vector_int16_t& dst, __ubuf__ int16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads 8 consecutive uint16_t elements from the aligned source address src, broadcasts each into one 32-byte
 * DataBlock of the dst register, then advances src by offset elements (post-update).
 *
 * @override_id asc_loadalign_brc_elem2datablock_postupdate__Rvu16_RPUu16_i32
 * @archs 3510
 * @param [out] dst Destination vector register of type vector_uint16_t; it receives a full vector length of broadcast
 * data, one uint16_t element per 32-byte DataBlock.
 * @param [in,out] src Source Unified Buffer pointer to uint16_t data, read from the current address and then updated in
 * place to the post-update address; the b16 type requires 16-byte alignment before and after the update.
 * @param [in] offset Post-update step in elements: after the load, src advances by offset * sizeof(uint16_t) = offset *
 * 2 bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock_postupdate(
    vector_uint16_t& dst, __ubuf__ uint16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads 8 consecutive half elements from the aligned source address src, broadcasts each into one 32-byte
 * DataBlock of the dst register, then advances src by offset elements (post-update).
 *
 * @override_id asc_loadalign_brc_elem2datablock_postupdate__Rvf16_RPUf16_i32
 * @archs 3510
 * @param [out] dst Destination vector register of type vector_half; it receives a full vector length of broadcast data,
 * one half element per 32-byte DataBlock.
 * @param [in,out] src Source Unified Buffer pointer to half data, read from the current address and then updated in
 * place to the post-update address; the b16 type requires 16-byte alignment before and after the update.
 * @param [in] offset Post-update step in elements: after the load, src advances by offset * sizeof(half) = offset * 2
 * bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock_postupdate(
    vector_half& dst, __ubuf__ half*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads 8 consecutive bfloat16_t elements from the aligned source address src, broadcasts each into one 32-byte
 * DataBlock of the dst register, then advances src by offset elements (post-update).
 *
 * @override_id asc_loadalign_brc_elem2datablock_postupdate__Rvbf16_RPUbf16_i32
 * @archs 3510
 * @param [out] dst Destination vector register of type vector_bfloat16_t; it receives a full vector length of broadcast
 * data, one bfloat16_t element per 32-byte DataBlock.
 * @param [in,out] src Source Unified Buffer pointer to bfloat16_t data, read from the current address and then updated
 * in place to the post-update address; the b16 type requires 16-byte alignment before and after the update.
 * @param [in] offset Post-update step in elements: after the load, src advances by offset * sizeof(bfloat16_t) = offset
 * * 2 bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock_postupdate(
    vector_bfloat16_t& dst, __ubuf__ bfloat16_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads 8 consecutive int32_t elements from the aligned source address src, broadcasts each into one 32-byte
 * DataBlock of the dst register, then advances src by offset elements (post-update).
 *
 * @override_id asc_loadalign_brc_elem2datablock_postupdate__Rvi32_RPUi32_i32
 * @archs 3510
 * @param [out] dst Destination vector register of type vector_int32_t; it receives a full vector length of broadcast
 * data, one int32_t element per 32-byte DataBlock.
 * @param [in,out] src Source Unified Buffer pointer to int32_t data, read from the current address and then updated in
 * place to the post-update address; the b32 type requires 32-byte alignment before and after the update.
 * @param [in] offset Post-update step in elements: after the load, src advances by offset * sizeof(int32_t) = offset *
 * 4 bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock_postupdate(
    vector_int32_t& dst, __ubuf__ int32_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads 8 consecutive uint32_t elements from the aligned source address src, broadcasts each into one 32-byte
 * DataBlock of the dst register, then advances src by offset elements (post-update).
 *
 * @override_id asc_loadalign_brc_elem2datablock_postupdate__Rvu32_RPUu32_i32
 * @archs 3510
 * @param [out] dst Destination vector register of type vector_uint32_t; it receives a full vector length of broadcast
 * data, one uint32_t element per 32-byte DataBlock.
 * @param [in,out] src Source Unified Buffer pointer to uint32_t data, read from the current address and then updated in
 * place to the post-update address; the b32 type requires 32-byte alignment before and after the update.
 * @param [in] offset Post-update step in elements: after the load, src advances by offset * sizeof(uint32_t) = offset *
 * 4 bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock_postupdate(
    vector_uint32_t& dst, __ubuf__ uint32_t*& src, int32_t offset);

/**
 * @ingroup simd_c_api.reg_compute.load
 * @brief Loads 8 consecutive float elements from the aligned source address src, broadcasts each into one 32-byte
 * DataBlock of the dst register, then advances src by offset elements (post-update).
 *
 * @override_id asc_loadalign_brc_elem2datablock_postupdate__Rvf32_RPUf32_i32
 * @archs 3510
 * @param [out] dst Destination vector register of type vector_float; it receives a full vector length of broadcast
 * data, one float element per 32-byte DataBlock.
 * @param [in,out] src Source Unified Buffer pointer to float data, read from the current address and then updated in
 * place to the post-update address; the b32 type requires 32-byte alignment before and after the update.
 * @param [in] offset Post-update step in elements: after the load, src advances by offset * sizeof(float) = offset * 4
 * bytes.
 */
__simd_callee__ inline void asc_loadalign_brc_elem2datablock_postupdate(
    vector_float& dst, __ubuf__ float*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_int8_t register dst, with no offset argument, so the caller must advance the source
 * address itself; deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvi8_PUi8
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the int8_t source operand; it is only read, and because this overload passes
 * no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_int8_t& dst, __ubuf__ int8_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_uint8_t register dst, with no offset argument, so the caller must advance the
 * source address itself; deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvu8_PUu8
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the uint8_t source operand; it is only read, and because this overload passes
 * no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_uint8_t& dst, __ubuf__ uint8_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_fp4x2_e2m1_t register dst, with no offset argument, so the caller must advance the
 * source address itself; deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvf4e2m1x2_PUf4e2m1x2
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp4x2_e2m1_t source operand; it is only read, and because this overload
 * passes no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_fp4x2_e1m2_t register dst, with no offset argument, so the caller must advance the
 * source address itself; deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvf4e1m2x2_PUf4e1m2x2
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp4x2_e1m2_t source operand; it is only read, and because this overload
 * passes no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_int4x2_t register dst, with no offset argument, so the caller must advance the
 * source address itself; deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvi4x2_PUi4x2
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the int4b_t source operand; it is only read, and because this overload passes
 * no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_int4x2_t& dst, __ubuf__ int4b_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_fp8_e8m0_t register dst, with no offset argument, so the caller must advance the
 * source address itself; deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvf8e8m0_PUf8e8m0
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp8_e8m0_t source operand; it is only read, and because this overload
 * passes no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_fp8_e5m2_t register dst, with no offset argument, so the caller must advance the
 * source address itself; deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvf8e5m2_PUf8e5m2
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp8_e5m2_t source operand; it is only read, and because this overload
 * passes no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_fp8_e4m3fn_t register dst, with no offset argument, so the caller must advance the
 * source address itself; deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvf8e4m3_PUf8e4m3
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp8_e4m3fn_t source operand; it is only read, and because this overload
 * passes no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_hifloat8_t register dst, with no offset argument, so the caller must advance the
 * source address itself; deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvhf8_PUhf8
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the hifloat8_t source operand; it is only read, and because this overload
 * passes no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_int8_t destination
 * register dst, with no offset argument, so the caller must advance the source address itself; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvi8_PUi8
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the int8_t source operand; it is only read, and because this overload passes
 * no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int8_t& dst, __ubuf__ int8_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_uint8_t destination
 * register dst, with no offset argument, so the caller must advance the source address itself; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvu8_PUu8
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the uint8_t source operand; it is only read, and because this overload passes
 * no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint8_t& dst, __ubuf__ uint8_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_fp4x2_e2m1_t
 * destination register dst, with no offset argument, so the caller must advance the source address itself; deprecated
 * in favor of asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvf4e2m1x2_PUf4e2m1x2
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp4x2_e2m1_t source operand; it is only read, and because this overload
 * passes no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_fp4x2_e1m2_t
 * destination register dst, with no offset argument, so the caller must advance the source address itself; deprecated
 * in favor of asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvf4e1m2x2_PUf4e1m2x2
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp4x2_e1m2_t source operand; it is only read, and because this overload
 * passes no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_int4x2_t destination
 * register dst, with no offset argument, so the caller must advance the source address itself; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvi4x2_PUi4x2
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the int4b_t source operand; it is only read, and because this overload passes
 * no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int4x2_t& dst, __ubuf__ int4b_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_fp8_e8m0_t destination
 * register dst, with no offset argument, so the caller must advance the source address itself; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvf8e8m0_PUf8e8m0
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp8_e8m0_t source operand; it is only read, and because this overload
 * passes no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_fp8_e5m2_t destination
 * register dst, with no offset argument, so the caller must advance the source address itself; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvf8e5m2_PUf8e5m2
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp8_e5m2_t source operand; it is only read, and because this overload
 * passes no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_fp8_e4m3fn_t
 * destination register dst, with no offset argument, so the caller must advance the source address itself; deprecated
 * in favor of asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvf8e4m3_PUf8e4m3
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp8_e4m3fn_t source operand; it is only read, and because this overload
 * passes no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_hifloat8_t destination
 * register dst, with no offset argument, so the caller must advance the source address itself; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvhf8_PUhf8
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the hifloat8_t source operand; it is only read, and because this overload
 * passes no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_int16_t destination
 * register dst, with no offset argument, so the caller must advance the source address itself; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvi16_PUi16
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the int16_t source operand; it is only read, and because this overload passes
 * no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int16_t& dst, __ubuf__ int16_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_uint16_t destination
 * register dst, with no offset argument, so the caller must advance the source address itself; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvu16_PUu16
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the uint16_t source operand; it is only read, and because this overload passes
 * no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint16_t& dst, __ubuf__ uint16_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_half destination
 * register dst, with no offset argument, so the caller must advance the source address itself; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvf16_PUf16
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) that receives the VL-length result; the page lists it
 * as an output.
 * @param [in] src Start address in UB of the half source operand; it is only read, and because this overload passes no
 * offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_half& dst, __ubuf__ half* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_bfloat16_t destination
 * register dst, with no offset argument, so the caller must advance the source address itself; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvbf16_PUbf16
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the bfloat16_t source operand; it is only read, and because this overload
 * passes no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_int32_t destination
 * register dst, with no offset argument, so the caller must advance the source address itself; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvi32_PUi32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int32_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the int32_t source operand; it is only read, and because this overload passes
 * no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int32_t& dst, __ubuf__ int32_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_uint32_t destination
 * register dst, with no offset argument, so the caller must advance the source address itself; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvu32_PUu32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint32_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the uint32_t source operand; it is only read, and because this overload passes
 * no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint32_t& dst, __ubuf__ uint32_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_float destination
 * register dst, with no offset argument, so the caller must advance the source address itself; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvf32_PUf32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_float) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the float source operand; it is only read, and because this overload passes no
 * offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_float& dst, __ubuf__ float* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads 16-bit source elements covering one vector length and expands each element into one 32-byte DataBlock of
 * the vector_int16_t destination register dst, with no offset argument, so the caller must advance the source address
 * itself; deprecated in favor of asc_loadalign_brc_elem2datablock.
 *
 * @override_id asc_loadalign_brc_v3__Rvi16_PUi16
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the int16_t source operand; it is only read, and because this overload passes
 * no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v3(vector_int16_t& dst, __ubuf__ int16_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads 16-bit source elements covering one vector length and expands each element into one 32-byte DataBlock of
 * the vector_uint16_t destination register dst, with no offset argument, so the caller must advance the source address
 * itself; deprecated in favor of asc_loadalign_brc_elem2datablock.
 *
 * @override_id asc_loadalign_brc_v3__Rvu16_PUu16
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the uint16_t source operand; it is only read, and because this overload passes
 * no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v3(vector_uint16_t& dst, __ubuf__ uint16_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads 16-bit source elements covering one vector length and expands each element into one 32-byte DataBlock of
 * the vector_half destination register dst, with no offset argument, so the caller must advance the source address
 * itself; deprecated in favor of asc_loadalign_brc_elem2datablock.
 *
 * @override_id asc_loadalign_brc_v3__Rvf16_PUf16
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) that receives the VL-length result; the page lists it
 * as an output.
 * @param [in] src Start address in UB of the half source operand; it is only read, and because this overload passes no
 * offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v3(vector_half& dst, __ubuf__ half* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads 16-bit source elements covering one vector length and expands each element into one 32-byte DataBlock of
 * the vector_bfloat16_t destination register dst, with no offset argument, so the caller must advance the source
 * address itself; deprecated in favor of asc_loadalign_brc_elem2datablock.
 *
 * @override_id asc_loadalign_brc_v3__Rvbf16_PUbf16
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the bfloat16_t source operand; it is only read, and because this overload
 * passes no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v3(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads 32-bit source elements covering one vector length and expands each element into one 32-byte DataBlock of
 * the vector_int32_t destination register dst, with no offset argument, so the caller must advance the source address
 * itself; deprecated in favor of asc_loadalign_brc_elem2datablock.
 *
 * @override_id asc_loadalign_brc_v3__Rvi32_PUi32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int32_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the int32_t source operand; it is only read, and because this overload passes
 * no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v3(vector_int32_t& dst, __ubuf__ int32_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads 32-bit source elements covering one vector length and expands each element into one 32-byte DataBlock of
 * the vector_uint32_t destination register dst, with no offset argument, so the caller must advance the source address
 * itself; deprecated in favor of asc_loadalign_brc_elem2datablock.
 *
 * @override_id asc_loadalign_brc_v3__Rvu32_PUu32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint32_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the uint32_t source operand; it is only read, and because this overload passes
 * no offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v3(vector_uint32_t& dst, __ubuf__ uint32_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads 32-bit source elements covering one vector length and expands each element into one 32-byte DataBlock of
 * the vector_float destination register dst, with no offset argument, so the caller must advance the source address
 * itself; deprecated in favor of asc_loadalign_brc_elem2datablock.
 *
 * @override_id asc_loadalign_brc_v3__Rvf32_PUf32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_float) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the float source operand; it is only read, and because this overload passes no
 * offset the caller must advance it between consecutive calls.
 */
__simd_callee__ inline void asc_loadalign_brc_v3(vector_float& dst, __ubuf__ float* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_int8_t register dst, taking the offset as an int32_t value relative to src;
 * deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvi8_PUi8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the int8_t source operand; it is only read and is not modified by this call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_int8_t& dst, __ubuf__ int8_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_uint8_t register dst, taking the offset as an int32_t value relative to src;
 * deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvu8_PUu8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the uint8_t source operand; it is only read and is not modified by this call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_uint8_t& dst, __ubuf__ uint8_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_fp4x2_e2m1_t register dst, taking the offset as an int32_t value relative to src;
 * deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvf4e2m1x2_PUf4e2m1x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp4x2_e2m1_t source operand; it is only read and is not modified by this
 * call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_fp4x2_e1m2_t register dst, taking the offset as an int32_t value relative to src;
 * deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvf4e1m2x2_PUf4e1m2x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp4x2_e1m2_t source operand; it is only read and is not modified by this
 * call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_int4x2_t register dst, taking the offset as an int32_t value relative to src;
 * deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvi4x2_PUi4x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the int4b_t source operand; it is only read and is not modified by this call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_int4x2_t& dst, __ubuf__ int4b_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_fp8_e8m0_t register dst, taking the offset as an int32_t value relative to src;
 * deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvf8e8m0_PUf8e8m0_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp8_e8m0_t source operand; it is only read and is not modified by this
 * call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_fp8_e5m2_t register dst, taking the offset as an int32_t value relative to src;
 * deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvf8e5m2_PUf8e5m2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp8_e5m2_t source operand; it is only read and is not modified by this
 * call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_fp8_e4m3fn_t register dst, taking the offset as an int32_t value relative to src;
 * deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvf8e4m3_PUf8e4m3_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp8_e4m3fn_t source operand; it is only read and is not modified by this
 * call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_hifloat8_t register dst, taking the offset as an int32_t value relative to src;
 * deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvhf8_PUhf8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the hifloat8_t source operand; it is only read and is not modified by this
 * call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_int8_t destination
 * register dst, taking the offset as an int32_t value relative to src; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvi8_PUi8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the int8_t source operand; it is only read and is not modified by this call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int8_t& dst, __ubuf__ int8_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_uint8_t destination
 * register dst, taking the offset as an int32_t value relative to src; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvu8_PUu8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the uint8_t source operand; it is only read and is not modified by this call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint8_t& dst, __ubuf__ uint8_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_fp4x2_e2m1_t
 * destination register dst, taking the offset as an int32_t value relative to src; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvf4e2m1x2_PUf4e2m1x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp4x2_e2m1_t source operand; it is only read and is not modified by this
 * call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_fp4x2_e1m2_t
 * destination register dst, taking the offset as an int32_t value relative to src; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvf4e1m2x2_PUf4e1m2x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp4x2_e1m2_t source operand; it is only read and is not modified by this
 * call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_int4x2_t destination
 * register dst, taking the offset as an int32_t value relative to src; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvi4x2_PUi4x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the int4b_t source operand; it is only read and is not modified by this call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int4x2_t& dst, __ubuf__ int4b_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_fp8_e8m0_t destination
 * register dst, taking the offset as an int32_t value relative to src; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvf8e8m0_PUf8e8m0_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp8_e8m0_t source operand; it is only read and is not modified by this
 * call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_fp8_e5m2_t destination
 * register dst, taking the offset as an int32_t value relative to src; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvf8e5m2_PUf8e5m2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp8_e5m2_t source operand; it is only read and is not modified by this
 * call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_fp8_e4m3fn_t
 * destination register dst, taking the offset as an int32_t value relative to src; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvf8e4m3_PUf8e4m3_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp8_e4m3fn_t source operand; it is only read and is not modified by this
 * call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_hifloat8_t destination
 * register dst, taking the offset as an int32_t value relative to src; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvhf8_PUhf8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the hifloat8_t source operand; it is only read and is not modified by this
 * call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_int16_t destination
 * register dst, taking the offset as an int32_t value relative to src; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvi16_PUi16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the int16_t source operand; it is only read and is not modified by this call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int16_t& dst, __ubuf__ int16_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_uint16_t destination
 * register dst, taking the offset as an int32_t value relative to src; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvu16_PUu16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the uint16_t source operand; it is only read and is not modified by this call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint16_t& dst, __ubuf__ uint16_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_half destination
 * register dst, taking the offset as an int32_t value relative to src; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvf16_PUf16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) that receives the VL-length result; the page lists it
 * as an output.
 * @param [in] src Start address in UB of the half source operand; it is only read and is not modified by this call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_half& dst, __ubuf__ half* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_bfloat16_t destination
 * register dst, taking the offset as an int32_t value relative to src; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvbf16_PUbf16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the bfloat16_t source operand; it is only read and is not modified by this
 * call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_int32_t destination
 * register dst, taking the offset as an int32_t value relative to src; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvi32_PUi32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int32_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the int32_t source operand; it is only read and is not modified by this call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int32_t& dst, __ubuf__ int32_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_uint32_t destination
 * register dst, taking the offset as an int32_t value relative to src; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvu32_PUu32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint32_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the uint32_t source operand; it is only read and is not modified by this call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint32_t& dst, __ubuf__ uint32_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_float destination
 * register dst, taking the offset as an int32_t value relative to src; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvf32_PUf32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_float) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the float source operand; it is only read and is not modified by this call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_float& dst, __ubuf__ float* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads 16-bit source elements covering one vector length and expands each element into one 32-byte DataBlock of
 * the vector_int16_t destination register dst, taking the offset as an int32_t value relative to src; deprecated in
 * favor of asc_loadalign_brc_elem2datablock.
 *
 * @override_id asc_loadalign_brc_v3__Rvi16_PUi16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the int16_t source operand; it is only read and is not modified by this call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v3(vector_int16_t& dst, __ubuf__ int16_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads 16-bit source elements covering one vector length and expands each element into one 32-byte DataBlock of
 * the vector_uint16_t destination register dst, taking the offset as an int32_t value relative to src; deprecated in
 * favor of asc_loadalign_brc_elem2datablock.
 *
 * @override_id asc_loadalign_brc_v3__Rvu16_PUu16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the uint16_t source operand; it is only read and is not modified by this call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v3(vector_uint16_t& dst, __ubuf__ uint16_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads 16-bit source elements covering one vector length and expands each element into one 32-byte DataBlock of
 * the vector_half destination register dst, taking the offset as an int32_t value relative to src; deprecated in favor
 * of asc_loadalign_brc_elem2datablock.
 *
 * @override_id asc_loadalign_brc_v3__Rvf16_PUf16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) that receives the VL-length result; the page lists it
 * as an output.
 * @param [in] src Start address in UB of the half source operand; it is only read and is not modified by this call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v3(vector_half& dst, __ubuf__ half* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads 16-bit source elements covering one vector length and expands each element into one 32-byte DataBlock of
 * the vector_bfloat16_t destination register dst, taking the offset as an int32_t value relative to src; deprecated in
 * favor of asc_loadalign_brc_elem2datablock.
 *
 * @override_id asc_loadalign_brc_v3__Rvbf16_PUbf16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the bfloat16_t source operand; it is only read and is not modified by this
 * call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v3(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads 32-bit source elements covering one vector length and expands each element into one 32-byte DataBlock of
 * the vector_int32_t destination register dst, taking the offset as an int32_t value relative to src; deprecated in
 * favor of asc_loadalign_brc_elem2datablock.
 *
 * @override_id asc_loadalign_brc_v3__Rvi32_PUi32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int32_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the int32_t source operand; it is only read and is not modified by this call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v3(vector_int32_t& dst, __ubuf__ int32_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads 32-bit source elements covering one vector length and expands each element into one 32-byte DataBlock of
 * the vector_uint32_t destination register dst, taking the offset as an int32_t value relative to src; deprecated in
 * favor of asc_loadalign_brc_elem2datablock.
 *
 * @override_id asc_loadalign_brc_v3__Rvu32_PUu32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint32_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the uint32_t source operand; it is only read and is not modified by this call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v3(vector_uint32_t& dst, __ubuf__ uint32_t* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads 32-bit source elements covering one vector length and expands each element into one 32-byte DataBlock of
 * the vector_float destination register dst, taking the offset as an int32_t value relative to src; deprecated in favor
 * of asc_loadalign_brc_elem2datablock.
 *
 * @override_id asc_loadalign_brc_v3__Rvf32_PUf32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_float) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the float source operand; it is only read and is not modified by this call.
 * @param [in] offset Offset relative to the src start address, passed as an int32_t element offset; the caller decides
 * whether to advance this offset or the source pointer.
 */
__simd_callee__ inline void asc_loadalign_brc_v3(vector_float& dst, __ubuf__ float* src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads VL/4 int8_t elements from the UB address in src in UNPACKV2 mode, expands them into the full VL of dst
 * with three zero lanes after each element, and post-updates src by offset.
 *
 * @override_id asc_loadalign_unpack_postupdate_v2__Rvi8_RPUi8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) that receives the VL-wide int8_t data produced by
 * the UNPACKV2 expansion.
 * @param [in,out] src UB address of the packed int8_t block read by the instruction (VL/4 elements); passed by
 * reference because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(
    vector_int8_t& dst, __ubuf__ int8_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads VL/4 uint8_t elements from the UB address in src in UNPACKV2 mode, expands them into the full VL of dst
 * with three zero lanes after each element, and post-updates src by offset.
 *
 * @override_id asc_loadalign_unpack_postupdate_v2__Rvu8_RPUu8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) that receives the VL-wide uint8_t data produced by
 * the UNPACKV2 expansion.
 * @param [in,out] src UB address of the packed uint8_t block read by the instruction (VL/4 elements); passed by
 * reference because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(
    vector_uint8_t& dst, __ubuf__ uint8_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads VL/4 fp4x2_e2m1_t elements from the UB address in src in UNPACKV2 mode, expands them into the full VL of
 * dst with three zero lanes after each element, and post-updates src by offset.
 *
 * @override_id asc_loadalign_unpack_postupdate_v2__Rvf4e2m1x2_RPUf4e2m1x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) that receives the VL-wide fp4x2_e2m1_t data
 * produced by the UNPACKV2 expansion.
 * @param [in,out] src UB address of the packed fp4x2_e2m1_t block read by the instruction (VL/4 elements); passed by
 * reference because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads VL/4 fp4x2_e1m2_t elements from the UB address in src in UNPACKV2 mode, expands them into the full VL of
 * dst with three zero lanes after each element, and post-updates src by offset.
 *
 * @override_id asc_loadalign_unpack_postupdate_v2__Rvf4e1m2x2_RPUf4e1m2x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) that receives the VL-wide fp4x2_e1m2_t data
 * produced by the UNPACKV2 expansion.
 * @param [in,out] src UB address of the packed fp4x2_e1m2_t block read by the instruction (VL/4 elements); passed by
 * reference because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads VL/4 fp8_e8m0_t elements from the UB address in src in UNPACKV2 mode, expands them into the full VL of
 * dst with three zero lanes after each element, and post-updates src by offset.
 *
 * @override_id asc_loadalign_unpack_postupdate_v2__Rvf8e8m0_RPUf8e8m0_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) that receives the VL-wide fp8_e8m0_t data
 * produced by the UNPACKV2 expansion.
 * @param [in,out] src UB address of the packed fp8_e8m0_t block read by the instruction (VL/4 elements); passed by
 * reference because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(
    vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads VL/4 int4b_t elements from the UB address in src in UNPACKV2 mode, expands them into the full VL of dst
 * with three zero lanes after each element, and post-updates src by offset.
 *
 * @override_id asc_loadalign_unpack_postupdate_v2__Rvi4x2_RPUi4x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) that receives the VL-wide int4b_t data produced
 * by the UNPACKV2 expansion.
 * @param [in,out] src UB address of the packed int4b_t block read by the instruction (VL/4 elements); passed by
 * reference because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(
    vector_int4x2_t& dst, __ubuf__ int4b_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads VL/4 fp8_e5m2_t elements from the UB address in src in UNPACKV2 mode, expands them into the full VL of
 * dst with three zero lanes after each element, and post-updates src by offset.
 *
 * @override_id asc_loadalign_unpack_postupdate_v2__Rvf8e5m2_RPUf8e5m2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) that receives the VL-wide fp8_e5m2_t data
 * produced by the UNPACKV2 expansion.
 * @param [in,out] src UB address of the packed fp8_e5m2_t block read by the instruction (VL/4 elements); passed by
 * reference because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(
    vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads VL/4 fp8_e4m3fn_t elements from the UB address in src in UNPACKV2 mode, expands them into the full VL of
 * dst with three zero lanes after each element, and post-updates src by offset.
 *
 * @override_id asc_loadalign_unpack_postupdate_v2__Rvf8e4m3_RPUf8e4m3_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) that receives the VL-wide fp8_e4m3fn_t data
 * produced by the UNPACKV2 expansion.
 * @param [in,out] src UB address of the packed fp8_e4m3fn_t block read by the instruction (VL/4 elements); passed by
 * reference because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads VL/4 hifloat8_t elements from the UB address in src in UNPACKV2 mode, expands them into the full VL of
 * dst with three zero lanes after each element, and post-updates src by offset.
 *
 * @override_id asc_loadalign_unpack_postupdate_v2__Rvhf8_RPUhf8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) that receives the VL-wide hifloat8_t data
 * produced by the UNPACKV2 expansion.
 * @param [in,out] src UB address of the packed hifloat8_t block read by the instruction (VL/4 elements); passed by
 * reference because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_unpack_postupdate_v2(
    vector_hifloat8_t& dst, __ubuf__ hifloat8_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the int8_t DataBlock of 32 bytes read from the UB address in src across the whole VL of dst in
 * BRCV2 mode, then post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v2__Rvi8_RPUi8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) that receives the int8_t DataBlock replicated over
 * the full VL.
 * @param [in,out] src UB address of the single int8_t DataBlock read by the instruction; passed by reference because
 * the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_int8_t& dst, __ubuf__ int8_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the uint8_t DataBlock of 32 bytes read from the UB address in src across the whole VL of dst in
 * BRCV2 mode, then post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v2__Rvu8_RPUu8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) that receives the uint8_t DataBlock replicated
 * over the full VL.
 * @param [in,out] src UB address of the single uint8_t DataBlock read by the instruction; passed by reference because
 * the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_uint8_t& dst, __ubuf__ uint8_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the fp4x2_e2m1_t DataBlock of 32 bytes read from the UB address in src across the whole VL of dst
 * in BRCV2 mode, then post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v2__Rvf4e2m1x2_RPUf4e2m1x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) that receives the fp4x2_e2m1_t DataBlock
 * replicated over the full VL.
 * @param [in,out] src UB address of the single fp4x2_e2m1_t DataBlock read by the instruction; passed by reference
 * because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the fp4x2_e1m2_t DataBlock of 32 bytes read from the UB address in src across the whole VL of dst
 * in BRCV2 mode, then post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v2__Rvf4e1m2x2_RPUf4e1m2x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) that receives the fp4x2_e1m2_t DataBlock
 * replicated over the full VL.
 * @param [in,out] src UB address of the single fp4x2_e1m2_t DataBlock read by the instruction; passed by reference
 * because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the fp8_e8m0_t DataBlock of 32 bytes read from the UB address in src across the whole VL of dst in
 * BRCV2 mode, then post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v2__Rvf8e8m0_RPUf8e8m0_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) that receives the fp8_e8m0_t DataBlock
 * replicated over the full VL.
 * @param [in,out] src UB address of the single fp8_e8m0_t DataBlock read by the instruction; passed by reference
 * because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the int4b_t DataBlock of 32 bytes read from the UB address in src across the whole VL of dst in
 * BRCV2 mode, then post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v2__Rvi4x2_RPUi4x2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) that receives the int4b_t DataBlock replicated
 * over the full VL.
 * @param [in,out] src UB address of the single int4b_t DataBlock read by the instruction; passed by reference because
 * the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_int4x2_t& dst, __ubuf__ int4b_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the fp8_e5m2_t DataBlock of 32 bytes read from the UB address in src across the whole VL of dst in
 * BRCV2 mode, then post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v2__Rvf8e5m2_RPUf8e5m2_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) that receives the fp8_e5m2_t DataBlock
 * replicated over the full VL.
 * @param [in,out] src UB address of the single fp8_e5m2_t DataBlock read by the instruction; passed by reference
 * because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the fp8_e4m3fn_t DataBlock of 32 bytes read from the UB address in src across the whole VL of dst
 * in BRCV2 mode, then post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v2__Rvf8e4m3_RPUf8e4m3_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) that receives the fp8_e4m3fn_t DataBlock
 * replicated over the full VL.
 * @param [in,out] src UB address of the single fp8_e4m3fn_t DataBlock read by the instruction; passed by reference
 * because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the hifloat8_t DataBlock of 32 bytes read from the UB address in src across the whole VL of dst in
 * BRCV2 mode, then post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v2__Rvhf8_RPUhf8_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) that receives the hifloat8_t DataBlock
 * replicated over the full VL.
 * @param [in,out] src UB address of the single hifloat8_t DataBlock read by the instruction; passed by reference
 * because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_hifloat8_t& dst, __ubuf__ hifloat8_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the int16_t DataBlock of 32 bytes read from the UB address in src across the whole VL of dst in
 * BRCV2 mode, then post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v2__Rvi16_RPUi16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) that receives the int16_t DataBlock replicated
 * over the full VL.
 * @param [in,out] src UB address of the single int16_t DataBlock read by the instruction; passed by reference because
 * the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_int16_t& dst, __ubuf__ int16_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the uint16_t DataBlock of 32 bytes read from the UB address in src across the whole VL of dst in
 * BRCV2 mode, then post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v2__Rvu16_RPUu16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) that receives the uint16_t DataBlock replicated
 * over the full VL.
 * @param [in,out] src UB address of the single uint16_t DataBlock read by the instruction; passed by reference because
 * the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_uint16_t& dst, __ubuf__ uint16_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the half DataBlock of 32 bytes read from the UB address in src across the whole VL of dst in BRCV2
 * mode, then post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v2__Rvf16_RPUf16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) that receives the half DataBlock replicated over the
 * full VL.
 * @param [in,out] src UB address of the single half DataBlock read by the instruction; passed by reference because the
 * hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_half& dst, __ubuf__ half*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the bfloat16_t DataBlock of 32 bytes read from the UB address in src across the whole VL of dst in
 * BRCV2 mode, then post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v2__Rvbf16_RPUbf16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) that receives the bfloat16_t DataBlock
 * replicated over the full VL.
 * @param [in,out] src UB address of the single bfloat16_t DataBlock read by the instruction; passed by reference
 * because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_bfloat16_t& dst, __ubuf__ bfloat16_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the int32_t DataBlock of 32 bytes read from the UB address in src across the whole VL of dst in
 * BRCV2 mode, then post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v2__Rvi32_RPUi32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int32_t) that receives the int32_t DataBlock replicated
 * over the full VL.
 * @param [in,out] src UB address of the single int32_t DataBlock read by the instruction; passed by reference because
 * the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_int32_t& dst, __ubuf__ int32_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the uint32_t DataBlock of 32 bytes read from the UB address in src across the whole VL of dst in
 * BRCV2 mode, then post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v2__Rvu32_RPUu32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint32_t) that receives the uint32_t DataBlock replicated
 * over the full VL.
 * @param [in,out] src UB address of the single uint32_t DataBlock read by the instruction; passed by reference because
 * the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(
    vector_uint32_t& dst, __ubuf__ uint32_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the float DataBlock of 32 bytes read from the UB address in src across the whole VL of dst in BRCV2
 * mode, then post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v2__Rvf32_RPUf32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_float) that receives the float DataBlock replicated over
 * the full VL.
 * @param [in,out] src UB address of the single float DataBlock read by the instruction; passed by reference because the
 * hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v2(vector_float& dst, __ubuf__ float*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads VL/DataBlock int16_t elements from the UB address in src in BRCV3 mode, broadcasts each element into one
 * 32-byte DataBlock of dst, and post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v3__Rvi16_RPUi16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) in which every 32-byte DataBlock receives one
 * broadcast int16_t element.
 * @param [in,out] src UB address of the int16_t elements read by the instruction (one element per destination
 * DataBlock); passed by reference because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v3(
    vector_int16_t& dst, __ubuf__ int16_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads VL/DataBlock uint16_t elements from the UB address in src in BRCV3 mode, broadcasts each element into
 * one 32-byte DataBlock of dst, and post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v3__Rvu16_RPUu16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) in which every 32-byte DataBlock receives one
 * broadcast uint16_t element.
 * @param [in,out] src UB address of the uint16_t elements read by the instruction (one element per destination
 * DataBlock); passed by reference because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v3(
    vector_uint16_t& dst, __ubuf__ uint16_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads VL/DataBlock half elements from the UB address in src in BRCV3 mode, broadcasts each element into one
 * 32-byte DataBlock of dst, and post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v3__Rvf16_RPUf16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) in which every 32-byte DataBlock receives one
 * broadcast half element.
 * @param [in,out] src UB address of the half elements read by the instruction (one element per destination DataBlock);
 * passed by reference because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v3(vector_half& dst, __ubuf__ half*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads VL/DataBlock bfloat16_t elements from the UB address in src in BRCV3 mode, broadcasts each element into
 * one 32-byte DataBlock of dst, and post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v3__Rvbf16_RPUbf16_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) in which every 32-byte DataBlock receives one
 * broadcast bfloat16_t element.
 * @param [in,out] src UB address of the bfloat16_t elements read by the instruction (one element per destination
 * DataBlock); passed by reference because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v3(
    vector_bfloat16_t& dst, __ubuf__ bfloat16_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads VL/DataBlock int32_t elements from the UB address in src in BRCV3 mode, broadcasts each element into one
 * 32-byte DataBlock of dst, and post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v3__Rvi32_RPUi32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int32_t) in which every 32-byte DataBlock receives one
 * broadcast int32_t element.
 * @param [in,out] src UB address of the int32_t elements read by the instruction (one element per destination
 * DataBlock); passed by reference because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v3(
    vector_int32_t& dst, __ubuf__ int32_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads VL/DataBlock uint32_t elements from the UB address in src in BRCV3 mode, broadcasts each element into
 * one 32-byte DataBlock of dst, and post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v3__Rvu32_RPUu32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint32_t) in which every 32-byte DataBlock receives one
 * broadcast uint32_t element.
 * @param [in,out] src UB address of the uint32_t elements read by the instruction (one element per destination
 * DataBlock); passed by reference because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v3(
    vector_uint32_t& dst, __ubuf__ uint32_t*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock_postupdate)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads VL/DataBlock float elements from the UB address in src in BRCV3 mode, broadcasts each element into one
 * 32-byte DataBlock of dst, and post-updates src by offset.
 *
 * @override_id asc_loadalign_brc_postupdate_v3__Rvf32_RPUf32_i32
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_float) in which every 32-byte DataBlock receives one
 * broadcast float element.
 * @param [in,out] src UB address of the float elements read by the instruction (one element per destination DataBlock);
 * passed by reference because the hardware post update advances the pointer.
 * @param [in] offset Post-update step in elements applied to src after the move, so the caller does not have to update
 * the source address itself.
 */
__simd_callee__ inline void asc_loadalign_brc_postupdate_v3(vector_float& dst, __ubuf__ float*& src, int32_t offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_fp4x2_e1m2_t register dst, taking the offset from an addr_reg relative to the
 * 32-byte-aligned src start; deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvf4e1m2x2_PUf4e1m2x2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp4x2_e1m2_t source operand; the page requires it to be 32-byte aligned
 * and it is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(
    vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_fp4x2_e2m1_t register dst, taking the offset from an addr_reg relative to the
 * 32-byte-aligned src start; deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvf4e2m1x2_PUf4e2m1x2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp4x2_e2m1_t source operand; the page requires it to be 32-byte aligned
 * and it is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(
    vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_int4x2_t register dst, taking the offset from an addr_reg relative to the
 * 32-byte-aligned src start; deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvi4x2_PUi4x2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the int4b_t source operand; the page requires it to be 32-byte aligned and it
 * is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_int4x2_t& dst, __ubuf__ int4b_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_uint8_t register dst, taking the offset from an addr_reg relative to the
 * 32-byte-aligned src start; deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvu8_PUu8_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the uint8_t source operand; the page requires it to be 32-byte aligned and it
 * is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_uint8_t& dst, __ubuf__ uint8_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_int8_t register dst, taking the offset from an addr_reg relative to the
 * 32-byte-aligned src start; deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvi8_PUi8_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the int8_t source operand; the page requires it to be 32-byte aligned and it
 * is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_int8_t& dst, __ubuf__ int8_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_fp8_e4m3fn_t register dst, taking the offset from an addr_reg relative to the
 * 32-byte-aligned src start; deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvf8e4m3_PUf8e4m3_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp8_e4m3fn_t source operand; the page requires it to be 32-byte aligned
 * and it is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(
    vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_hifloat8_t register dst, taking the offset from an addr_reg relative to the
 * 32-byte-aligned src start; deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvhf8_PUhf8_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the hifloat8_t source operand; the page requires it to be 32-byte aligned and
 * it is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_fp8_e5m2_t register dst, taking the offset from an addr_reg relative to the
 * 32-byte-aligned src start; deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvf8e5m2_PUf8e5m2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp8_e5m2_t source operand; the page requires it to be 32-byte aligned and
 * it is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_unpack4)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Unpacks VL/4 bytes read from src in UB as unsigned u8 into VL-length 32-bit lanes with zeros between them and
 * writes the result into the vector_fp8_e8m0_t register dst, taking the offset from an addr_reg relative to the
 * 32-byte-aligned src start; deprecated in favor of asc_loadalign_unpack4.
 *
 * @override_id asc_loadalign_unpack_v2__Rvf8e8m0_PUf8e8m0_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp8_e8m0_t source operand; the page requires it to be 32-byte aligned and
 * it is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_unpack_v2(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_fp4x2_e1m2_t
 * destination register dst, taking the offset from an addr_reg relative to the 32-byte-aligned src start; deprecated in
 * favor of asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvf4e1m2x2_PUf4e1m2x2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e1m2_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp4x2_e1m2_t source operand; the page requires it to be 32-byte aligned
 * and it is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp4x2_e1m2_t& dst, __ubuf__ fp4x2_e1m2_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_fp4x2_e2m1_t
 * destination register dst, taking the offset from an addr_reg relative to the 32-byte-aligned src start; deprecated in
 * favor of asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvf4e2m1x2_PUf4e2m1x2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp4x2_e2m1_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp4x2_e2m1_t source operand; the page requires it to be 32-byte aligned
 * and it is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp4x2_e2m1_t& dst, __ubuf__ fp4x2_e2m1_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_int4x2_t destination
 * register dst, taking the offset from an addr_reg relative to the 32-byte-aligned src start; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvi4x2_PUi4x2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int4x2_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the int4b_t source operand; the page requires it to be 32-byte aligned and it
 * is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int4x2_t& dst, __ubuf__ int4b_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_uint8_t destination
 * register dst, taking the offset from an addr_reg relative to the 32-byte-aligned src start; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvu8_PUu8_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint8_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the uint8_t source operand; the page requires it to be 32-byte aligned and it
 * is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint8_t& dst, __ubuf__ uint8_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_int8_t destination
 * register dst, taking the offset from an addr_reg relative to the 32-byte-aligned src start; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvi8_PUi8_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int8_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the int8_t source operand; the page requires it to be 32-byte aligned and it
 * is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int8_t& dst, __ubuf__ int8_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_fp8_e4m3fn_t
 * destination register dst, taking the offset from an addr_reg relative to the 32-byte-aligned src start; deprecated in
 * favor of asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvf8e4m3_PUf8e4m3_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e4m3fn_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp8_e4m3fn_t source operand; the page requires it to be 32-byte aligned
 * and it is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e4m3fn_t& dst, __ubuf__ fp8_e4m3fn_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_hifloat8_t destination
 * register dst, taking the offset from an addr_reg relative to the 32-byte-aligned src start; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvhf8_PUhf8_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_hifloat8_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the hifloat8_t source operand; the page requires it to be 32-byte aligned and
 * it is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_hifloat8_t& dst, __ubuf__ hifloat8_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_fp8_e5m2_t destination
 * register dst, taking the offset from an addr_reg relative to the 32-byte-aligned src start; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvf8e5m2_PUf8e5m2_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e5m2_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp8_e5m2_t source operand; the page requires it to be 32-byte aligned and
 * it is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e5m2_t& dst, __ubuf__ fp8_e5m2_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_fp8_e8m0_t destination
 * register dst, taking the offset from an addr_reg relative to the 32-byte-aligned src start; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvf8e8m0_PUf8e8m0_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_fp8_e8m0_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the fp8_e8m0_t source operand; the page requires it to be 32-byte aligned and
 * it is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_fp8_e8m0_t& dst, __ubuf__ fp8_e8m0_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_uint16_t destination
 * register dst, taking the offset from an addr_reg relative to the 32-byte-aligned src start; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvu16_PUu16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the uint16_t source operand; the page requires it to be 32-byte aligned and it
 * is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint16_t& dst, __ubuf__ uint16_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_int16_t destination
 * register dst, taking the offset from an addr_reg relative to the 32-byte-aligned src start; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvi16_PUi16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the int16_t source operand; the page requires it to be 32-byte aligned and it
 * is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int16_t& dst, __ubuf__ int16_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_bfloat16_t destination
 * register dst, taking the offset from an addr_reg relative to the 32-byte-aligned src start; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvbf16_PUbf16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the bfloat16_t source operand; the page requires it to be 32-byte aligned and
 * it is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_half destination
 * register dst, taking the offset from an addr_reg relative to the 32-byte-aligned src start; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvf16_PUf16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) that receives the VL-length result; the page lists it
 * as an output.
 * @param [in] src Start address in UB of the half source operand; the page requires it to be 32-byte aligned and it is
 * only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_half& dst, __ubuf__ half* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_uint32_t destination
 * register dst, taking the offset from an addr_reg relative to the 32-byte-aligned src start; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvu32_PUu32_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint32_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the uint32_t source operand; the page requires it to be 32-byte aligned and it
 * is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_uint32_t& dst, __ubuf__ uint32_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_int32_t destination
 * register dst, taking the offset from an addr_reg relative to the 32-byte-aligned src start; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvi32_PUi32_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int32_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the int32_t source operand; the page requires it to be 32-byte aligned and it
 * is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_int32_t& dst, __ubuf__ int32_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Reads one 32-byte DataBlock from src and broadcasts it across the full VL-length vector_float destination
 * register dst, taking the offset from an addr_reg relative to the 32-byte-aligned src start; deprecated in favor of
 * asc_loadalign_brc_datablock.
 *
 * @override_id asc_loadalign_brc_v2__Rvf32_PUf32_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_float) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the float source operand; the page requires it to be 32-byte aligned and it is
 * only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v2(vector_float& dst, __ubuf__ float* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads 16-bit source elements covering one vector length and expands each element into one 32-byte DataBlock of
 * the vector_uint16_t destination register dst, taking the offset from an addr_reg relative to the 32-byte-aligned src
 * start; deprecated in favor of asc_loadalign_brc_elem2datablock.
 *
 * @override_id asc_loadalign_brc_v3__Rvu16_PUu16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint16_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the uint16_t source operand; the page requires it to be 32-byte aligned and it
 * is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v3(vector_uint16_t& dst, __ubuf__ uint16_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads 16-bit source elements covering one vector length and expands each element into one 32-byte DataBlock of
 * the vector_int16_t destination register dst, taking the offset from an addr_reg relative to the 32-byte-aligned src
 * start; deprecated in favor of asc_loadalign_brc_elem2datablock.
 *
 * @override_id asc_loadalign_brc_v3__Rvi16_PUi16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int16_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the int16_t source operand; the page requires it to be 32-byte aligned and it
 * is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v3(vector_int16_t& dst, __ubuf__ int16_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads 16-bit source elements covering one vector length and expands each element into one 32-byte DataBlock of
 * the vector_bfloat16_t destination register dst, taking the offset from an addr_reg relative to the 32-byte-aligned
 * src start; deprecated in favor of asc_loadalign_brc_elem2datablock.
 *
 * @override_id asc_loadalign_brc_v3__Rvbf16_PUbf16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_bfloat16_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the bfloat16_t source operand; the page requires it to be 32-byte aligned and
 * it is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v3(vector_bfloat16_t& dst, __ubuf__ bfloat16_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads 16-bit source elements covering one vector length and expands each element into one 32-byte DataBlock of
 * the vector_half destination register dst, taking the offset from an addr_reg relative to the 32-byte-aligned src
 * start; deprecated in favor of asc_loadalign_brc_elem2datablock.
 *
 * @override_id asc_loadalign_brc_v3__Rvf16_PUf16_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_half) that receives the VL-length result; the page lists it
 * as an output.
 * @param [in] src Start address in UB of the half source operand; the page requires it to be 32-byte aligned and it is
 * only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v3(vector_half& dst, __ubuf__ half* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads 32-bit source elements covering one vector length and expands each element into one 32-byte DataBlock of
 * the vector_uint32_t destination register dst, taking the offset from an addr_reg relative to the 32-byte-aligned src
 * start; deprecated in favor of asc_loadalign_brc_elem2datablock.
 *
 * @override_id asc_loadalign_brc_v3__Rvu32_PUu32_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_uint32_t) that receives the VL-length result; the page
 * lists it as an output.
 * @param [in] src Start address in UB of the uint32_t source operand; the page requires it to be 32-byte aligned and it
 * is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v3(vector_uint32_t& dst, __ubuf__ uint32_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads 32-bit source elements covering one vector length and expands each element into one 32-byte DataBlock of
 * the vector_int32_t destination register dst, taking the offset from an addr_reg relative to the 32-byte-aligned src
 * start; deprecated in favor of asc_loadalign_brc_elem2datablock.
 *
 * @override_id asc_loadalign_brc_v3__Rvi32_PUi32_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_int32_t) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the int32_t source operand; the page requires it to be 32-byte aligned and it
 * is only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v3(vector_int32_t& dst, __ubuf__ int32_t* src, addr_reg offset);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_loadalign_brc_elem2datablock)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Loads 32-bit source elements covering one vector length and expands each element into one 32-byte DataBlock of
 * the vector_float destination register dst, taking the offset from an addr_reg relative to the 32-byte-aligned src
 * start; deprecated in favor of asc_loadalign_brc_elem2datablock.
 *
 * @override_id asc_loadalign_brc_v3__Rvf32_PUf32_ar
 * @archs 3510
 * @param [out] dst Destination vector data register (vector_float) that receives the VL-length result; the page lists
 * it as an output.
 * @param [in] src Start address in UB of the float source operand; the page requires it to be 32-byte aligned and it is
 * only read.
 * @param [in] offset Address register holding the offset relative to the src start in element units; it must be created
 * with asc_update_addr_reg and is consumed without being written back.
 */
__simd_callee__ inline void asc_loadalign_brc_v3(vector_float& dst, __ubuf__ float* src, addr_reg offset);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_LOAD_LOADALIGN_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_COMPUTE_LOAD_LOADALIGN_H
#endif
