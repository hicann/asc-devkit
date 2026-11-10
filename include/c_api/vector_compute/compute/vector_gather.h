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
#error "vector_compute/compute/vector_gather.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_GATHER_H
#endif

#ifndef INCLUDE_C_API_VECTOR_COMPUTE_COMPUTE_VECTOR_GATHER_H
#define INCLUDE_C_API_VECTOR_COMPUTE_COMPUTE_VECTOR_GATHER_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/memory_base_impl/vector_gather_intf_impl.h"
#endif
/**
 * @ingroup simd_c_api.vector_compute.vector_gather
 * @brief Gathers int16_t elements from src into dst using the per-element byte offsets in src_offset over repeat
 * iterations with destination repeat stride dst_repeat_stride.
 *
 * @override_id asc_gather__PUi16_PUi16_PUu32_u16_u8
 * @archs 2201
 * @param [out] dst Start address of the destination int16_t vector in Unified Buffer; must be 32-byte aligned and
 * receives the gathered elements.
 * @param [in] src Base address of the source int16_t vector in Unified Buffer; must be 32-byte aligned, and each
 * gathered element is read from src plus the matching src_offset value.
 * @param [in] src_offset Per-element byte offsets relative to the src base address, one uint32_t per gathered int16_t
 * element; each offset must keep the element-width alignment and the resulting address must stay inside Unified Buffer.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in DataBlocks of 32 bytes.
 * @param [in] repeat Number of gather iterations passed to vgather.
 */
__aicore__ inline void asc_gather(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride,
    uint8_t repeat);

/**
 * @ingroup simd_c_api.vector_compute.vector_gather
 * @brief Gathers count int16_t elements from src into dst using the per-element byte offsets in src_offset.
 *
 * @override_id asc_gather__PUi16_PUi16_PUu32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination int16_t vector in Unified Buffer; must be 32-byte aligned and
 * receives the gathered elements.
 * @param [in] src Base address of the source int16_t vector in Unified Buffer; must be 32-byte aligned, and each
 * gathered element is read from src plus the matching src_offset value.
 * @param [in] src_offset Per-element byte offsets relative to the src base address, one uint32_t per gathered int16_t
 * element; each offset must keep the element-width alignment and the resulting address must stay inside Unified Buffer.
 * @param [in] count Number of int16_t elements gathered; the implementation opens the mask count register with it.
 */
__aicore__ inline void asc_gather(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_gather
 * @brief Gathers uint16_t elements from src into dst using the per-element byte offsets in src_offset over repeat
 * iterations with destination repeat stride dst_repeat_stride.
 *
 * @override_id asc_gather__PUu16_PUu16_PUu32_u16_u8
 * @archs 2201
 * @param [out] dst Start address of the destination uint16_t vector in Unified Buffer; must be 32-byte aligned and
 * receives the gathered elements.
 * @param [in] src Base address of the source uint16_t vector in Unified Buffer; must be 32-byte aligned, and each
 * gathered element is read from src plus the matching src_offset value.
 * @param [in] src_offset Per-element byte offsets relative to the src base address, one uint32_t per gathered uint16_t
 * element; each offset must keep the element-width alignment and the resulting address must stay inside Unified Buffer.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in DataBlocks of 32 bytes.
 * @param [in] repeat Number of gather iterations passed to vgather.
 */
__aicore__ inline void asc_gather(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride,
    uint8_t repeat);

/**
 * @ingroup simd_c_api.vector_compute.vector_gather
 * @brief Gathers count uint16_t elements from src into dst using the per-element byte offsets in src_offset.
 *
 * @override_id asc_gather__PUu16_PUu16_PUu32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination uint16_t vector in Unified Buffer; must be 32-byte aligned and
 * receives the gathered elements.
 * @param [in] src Base address of the source uint16_t vector in Unified Buffer; must be 32-byte aligned, and each
 * gathered element is read from src plus the matching src_offset value.
 * @param [in] src_offset Per-element byte offsets relative to the src base address, one uint32_t per gathered uint16_t
 * element; each offset must keep the element-width alignment and the resulting address must stay inside Unified Buffer.
 * @param [in] count Number of uint16_t elements gathered; the implementation opens the mask count register with it.
 */
__aicore__ inline void asc_gather(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_gather
 * @brief Gathers half elements from src into dst using the per-element byte offsets in src_offset over repeat
 * iterations with destination repeat stride dst_repeat_stride.
 *
 * @override_id asc_gather__PUf16_PUf16_PUu32_u16_u8
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned and receives
 * the gathered elements.
 * @param [in] src Base address of the source half vector in Unified Buffer; must be 32-byte aligned, and each gathered
 * element is read from src plus the matching src_offset value.
 * @param [in] src_offset Per-element byte offsets relative to the src base address, one uint32_t per gathered half
 * element; each offset must keep the element-width alignment and the resulting address must stay inside Unified Buffer.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in DataBlocks of 32 bytes.
 * @param [in] repeat Number of gather iterations passed to vgather.
 */
__aicore__ inline void asc_gather(
    __ubuf__ half* dst, __ubuf__ half* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t repeat);

/**
 * @ingroup simd_c_api.vector_compute.vector_gather
 * @brief Gathers count half elements from src into dst using the per-element byte offsets in src_offset.
 *
 * @override_id asc_gather__PUf16_PUf16_PUu32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned and receives
 * the gathered elements.
 * @param [in] src Base address of the source half vector in Unified Buffer; must be 32-byte aligned, and each gathered
 * element is read from src plus the matching src_offset value.
 * @param [in] src_offset Per-element byte offsets relative to the src base address, one uint32_t per gathered half
 * element; each offset must keep the element-width alignment and the resulting address must stay inside Unified Buffer.
 * @param [in] count Number of half elements gathered; the implementation opens the mask count register with it.
 */
__aicore__ inline void asc_gather(
    __ubuf__ half* dst, __ubuf__ half* src, __ubuf__ uint32_t* src_offset, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_gather
 * @brief Gathers bfloat16_t elements from src into dst using the per-element byte offsets in src_offset over repeat
 * iterations with destination repeat stride dst_repeat_stride.
 *
 * @override_id asc_gather__PUbf16_PUbf16_PUu32_u16_u8
 * @archs 2201
 * @param [out] dst Start address of the destination bfloat16_t vector in Unified Buffer; must be 32-byte aligned and
 * receives the gathered elements.
 * @param [in] src Base address of the source bfloat16_t vector in Unified Buffer; must be 32-byte aligned, and each
 * gathered element is read from src plus the matching src_offset value.
 * @param [in] src_offset Per-element byte offsets relative to the src base address, one uint32_t per gathered
 * bfloat16_t element; each offset must keep the element-width alignment and the resulting address must stay inside
 * Unified Buffer.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in DataBlocks of 32 bytes.
 * @param [in] repeat Number of gather iterations passed to vgather.
 */
__aicore__ inline void asc_gather(
    __ubuf__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride,
    uint8_t repeat);

/**
 * @ingroup simd_c_api.vector_compute.vector_gather
 * @brief Gathers count bfloat16_t elements from src into dst using the per-element byte offsets in src_offset.
 *
 * @override_id asc_gather__PUbf16_PUbf16_PUu32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination bfloat16_t vector in Unified Buffer; must be 32-byte aligned and
 * receives the gathered elements.
 * @param [in] src Base address of the source bfloat16_t vector in Unified Buffer; must be 32-byte aligned, and each
 * gathered element is read from src plus the matching src_offset value.
 * @param [in] src_offset Per-element byte offsets relative to the src base address, one uint32_t per gathered
 * bfloat16_t element; each offset must keep the element-width alignment and the resulting address must stay inside
 * Unified Buffer.
 * @param [in] count Number of bfloat16_t elements gathered; the implementation opens the mask count register with it.
 */
__aicore__ inline void asc_gather(
    __ubuf__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_gather
 * @brief Gathers int32_t elements from src into dst using the per-element byte offsets in src_offset over repeat
 * iterations with destination repeat stride dst_repeat_stride.
 *
 * @override_id asc_gather__PUi32_PUi32_PUu32_u16_u8
 * @archs 2201
 * @param [out] dst Start address of the destination int32_t vector in Unified Buffer; must be 32-byte aligned and
 * receives the gathered elements.
 * @param [in] src Base address of the source int32_t vector in Unified Buffer; must be 32-byte aligned, and each
 * gathered element is read from src plus the matching src_offset value.
 * @param [in] src_offset Per-element byte offsets relative to the src base address, one uint32_t per gathered int32_t
 * element; each offset must keep the element-width alignment and the resulting address must stay inside Unified Buffer.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in DataBlocks of 32 bytes.
 * @param [in] repeat Number of gather iterations passed to vgather.
 */
__aicore__ inline void asc_gather(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride,
    uint8_t repeat);

/**
 * @ingroup simd_c_api.vector_compute.vector_gather
 * @brief Gathers count int32_t elements from src into dst using the per-element byte offsets in src_offset.
 *
 * @override_id asc_gather__PUi32_PUi32_PUu32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination int32_t vector in Unified Buffer; must be 32-byte aligned and
 * receives the gathered elements.
 * @param [in] src Base address of the source int32_t vector in Unified Buffer; must be 32-byte aligned, and each
 * gathered element is read from src plus the matching src_offset value.
 * @param [in] src_offset Per-element byte offsets relative to the src base address, one uint32_t per gathered int32_t
 * element; each offset must keep the element-width alignment and the resulting address must stay inside Unified Buffer.
 * @param [in] count Number of int32_t elements gathered; the implementation opens the mask count register with it.
 */
__aicore__ inline void asc_gather(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, __ubuf__ uint32_t* src_offset, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_gather
 * @brief Gathers uint32_t elements from src into dst using the per-element byte offsets in src_offset over repeat
 * iterations with destination repeat stride dst_repeat_stride.
 *
 * @override_id asc_gather__PUu32_PUu32_PUu32_u16_u8
 * @archs 2201
 * @param [out] dst Start address of the destination uint32_t vector in Unified Buffer; must be 32-byte aligned and
 * receives the gathered elements.
 * @param [in] src Base address of the source uint32_t vector in Unified Buffer; must be 32-byte aligned, and each
 * gathered element is read from src plus the matching src_offset value.
 * @param [in] src_offset Per-element byte offsets relative to the src base address, one uint32_t per gathered uint32_t
 * element; each offset must keep the element-width alignment and the resulting address must stay inside Unified Buffer.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in DataBlocks of 32 bytes.
 * @param [in] repeat Number of gather iterations passed to vgather.
 */
__aicore__ inline void asc_gather(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride,
    uint8_t repeat);

/**
 * @ingroup simd_c_api.vector_compute.vector_gather
 * @brief Gathers count uint32_t elements from src into dst using the per-element byte offsets in src_offset.
 *
 * @override_id asc_gather__PUu32_PUu32_PUu32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination uint32_t vector in Unified Buffer; must be 32-byte aligned and
 * receives the gathered elements.
 * @param [in] src Base address of the source uint32_t vector in Unified Buffer; must be 32-byte aligned, and each
 * gathered element is read from src plus the matching src_offset value.
 * @param [in] src_offset Per-element byte offsets relative to the src base address, one uint32_t per gathered uint32_t
 * element; each offset must keep the element-width alignment and the resulting address must stay inside Unified Buffer.
 * @param [in] count Number of uint32_t elements gathered; the implementation opens the mask count register with it.
 */
__aicore__ inline void asc_gather(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, __ubuf__ uint32_t* src_offset, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_gather
 * @brief Gathers float elements from src into dst using the per-element byte offsets in src_offset over repeat
 * iterations with destination repeat stride dst_repeat_stride.
 *
 * @override_id asc_gather__PUf32_PUf32_PUu32_u16_u8
 * @archs 2201
 * @param [out] dst Start address of the destination float vector in Unified Buffer; must be 32-byte aligned and
 * receives the gathered elements.
 * @param [in] src Base address of the source float vector in Unified Buffer; must be 32-byte aligned, and each gathered
 * element is read from src plus the matching src_offset value.
 * @param [in] src_offset Per-element byte offsets relative to the src base address, one uint32_t per gathered float
 * element; each offset must keep the element-width alignment and the resulting address must stay inside Unified Buffer.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in DataBlocks of 32 bytes.
 * @param [in] repeat Number of gather iterations passed to vgather.
 */
__aicore__ inline void asc_gather(
    __ubuf__ float* dst, __ubuf__ float* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride,
    uint8_t repeat);

/**
 * @ingroup simd_c_api.vector_compute.vector_gather
 * @brief Gathers count float elements from src into dst using the per-element byte offsets in src_offset.
 *
 * @override_id asc_gather__PUf32_PUf32_PUu32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector in Unified Buffer; must be 32-byte aligned and
 * receives the gathered elements.
 * @param [in] src Base address of the source float vector in Unified Buffer; must be 32-byte aligned, and each gathered
 * element is read from src plus the matching src_offset value.
 * @param [in] src_offset Per-element byte offsets relative to the src base address, one uint32_t per gathered float
 * element; each offset must keep the element-width alignment and the resulting address must stay inside Unified Buffer.
 * @param [in] count Number of float elements gathered; the implementation opens the mask count register with it.
 */
__aicore__ inline void asc_gather(
    __ubuf__ float* dst, __ubuf__ float* src, __ubuf__ uint32_t* src_offset, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_gather
 * @brief Gathers 32-byte data blocks from the uint16 source vector into dst according to per-block offsets given by
 * src_offset.
 *
 * @override_id asc_gather_datablock__PUu16_PUu16_PUu32_u16_u8_u8
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the gathered data blocks; must be 32-byte aligned.
 * @param [in] src Source vector start address from which data blocks are collected; must be 32-byte aligned.
 * @param [in] src_offset Address offset of each DataBlock within the source vector, used to select the gathered blocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration.
 * @param [in] repeat Number of iterations of the gather.
 */
__aicore__ inline void asc_gather_datablock(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride,
    uint8_t dst_block_stride, uint8_t repeat);

/**
 * @ingroup simd_c_api.vector_compute.vector_gather
 * @brief Gathers 32-byte data blocks from the uint32 source vector into dst according to per-block offsets given by
 * src_offset.
 *
 * @override_id asc_gather_datablock__PUu32_PUu32_PUu32_u16_u8_u8
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the gathered data blocks; must be 32-byte aligned.
 * @param [in] src Source vector start address from which data blocks are collected; must be 32-byte aligned.
 * @param [in] src_offset Address offset of each DataBlock within the source vector, used to select the gathered blocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration.
 * @param [in] repeat Number of iterations of the gather.
 */
__aicore__ inline void asc_gather_datablock(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride,
    uint8_t dst_block_stride, uint8_t repeat);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_GATHER_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_GATHER_H
#endif
