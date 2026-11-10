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
#error "vector_compute/compute/vector_broadcast.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_BROADCAST_H
#endif

#ifndef INCLUDE_C_API_VECTOR_COMPUTE_COMPUTE_VECTOR_BROADCAST_H
#define INCLUDE_C_API_VECTOR_COMPUTE_COMPUTE_VECTOR_BROADCAST_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/memory_base_impl/vector_broadcast_intf_impl.h"
#endif
/**
 * @ingroup simd_c_api.vector_compute.vector_broadcast
 * @brief Broadcasts eight uint16 values of one input vector into eight 32-byte data blocks of the destination, one
 * value per block, for the given number of iterations.
 *
 * @override_id asc_brcb__PUu16_PUu16_u16_u16_u8
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the broadcast data blocks.
 * @param [in] src Source vector start address whose eight values per iteration are broadcast.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] repeat Number of iterations of the broadcast.
 */
__aicore__ inline void asc_brcb(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint16_t dst_block_stride, uint16_t dst_repeat_stride,
    uint8_t repeat);

/**
 * @ingroup simd_c_api.vector_compute.vector_broadcast
 * @brief Broadcasts eight uint32 values of one input vector into eight 32-byte data blocks of the destination, one
 * value per block, for the given number of iterations.
 *
 * @override_id asc_brcb__PUu32_PUu32_u16_u16_u8
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the broadcast data blocks.
 * @param [in] src Source vector start address whose eight values per iteration are broadcast.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] repeat Number of iterations of the broadcast.
 */
__aicore__ inline void asc_brcb(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint16_t dst_block_stride, uint16_t dst_repeat_stride,
    uint8_t repeat);

/**
 * @ingroup simd_c_api.vector_compute.vector_broadcast
 * @brief Replicates the scalar src into half destination elements in Unified Buffer over repeat iterations, honouring
 * dst_block_stride and dst_repeat_stride.
 *
 * @override_id asc_duplicate__PUf16_f16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned and receives
 * the replicated scalar over all iterations.
 * @param [in] src Scalar half value that is copied into every destination element.
 * @param [in] repeat Iteration count passed straight to vector_dup; each iteration covers the eight 32-byte DataBlocks
 * of one vector iteration.
 * @param [in] dst_block_stride Address stride between different DataBlocks inside a single destination iteration,
 * counted in DataBlocks of 32 bytes.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_duplicate(
    __ubuf__ half* dst, half src, uint8_t repeat, uint16_t dst_block_stride, uint16_t dst_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_broadcast
 * @brief Replicates the scalar src into the first count half elements of the destination vector in Unified Buffer.
 *
 * @override_id asc_duplicate__PUf16_f16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned and receives
 * count copies of the scalar.
 * @param [in] src Scalar half value that is copied into every destination element.
 * @param [in] count Number of consecutive half elements to fill; the implementation feeds it to the mask count register
 * so exactly this many elements are written.
 */
__aicore__ inline void asc_duplicate(__ubuf__ half* dst, half src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_broadcast
 * @brief Replicates the scalar src into int16_t destination elements in Unified Buffer over repeat iterations,
 * honouring dst_block_stride and dst_repeat_stride.
 *
 * @override_id asc_duplicate__PUi16_i16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination int16_t vector in Unified Buffer; must be 32-byte aligned and
 * receives the replicated scalar over all iterations.
 * @param [in] src Scalar int16_t value that is copied into every destination element.
 * @param [in] repeat Iteration count passed straight to vector_dup; each iteration covers the eight 32-byte DataBlocks
 * of one vector iteration.
 * @param [in] dst_block_stride Address stride between different DataBlocks inside a single destination iteration,
 * counted in DataBlocks of 32 bytes.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_duplicate(
    __ubuf__ int16_t* dst, int16_t src, uint8_t repeat, uint16_t dst_block_stride, uint16_t dst_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_broadcast
 * @brief Replicates the scalar src into the first count int16_t elements of the destination vector in Unified Buffer.
 *
 * @override_id asc_duplicate__PUi16_i16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination int16_t vector in Unified Buffer; must be 32-byte aligned and
 * receives count copies of the scalar.
 * @param [in] src Scalar int16_t value that is copied into every destination element.
 * @param [in] count Number of consecutive int16_t elements to fill; the implementation feeds it to the mask count
 * register so exactly this many elements are written.
 */
__aicore__ inline void asc_duplicate(__ubuf__ int16_t* dst, int16_t src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_broadcast
 * @brief Replicates the scalar src into uint16_t destination elements in Unified Buffer over repeat iterations,
 * honouring dst_block_stride and dst_repeat_stride.
 *
 * @override_id asc_duplicate__PUu16_u16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination uint16_t vector in Unified Buffer; must be 32-byte aligned and
 * receives the replicated scalar over all iterations.
 * @param [in] src Scalar uint16_t value that is copied into every destination element.
 * @param [in] repeat Iteration count passed straight to vector_dup; each iteration covers the eight 32-byte DataBlocks
 * of one vector iteration.
 * @param [in] dst_block_stride Address stride between different DataBlocks inside a single destination iteration,
 * counted in DataBlocks of 32 bytes.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_duplicate(
    __ubuf__ uint16_t* dst, uint16_t src, uint8_t repeat, uint16_t dst_block_stride, uint16_t dst_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_broadcast
 * @brief Replicates the scalar src into the first count uint16_t elements of the destination vector in Unified Buffer.
 *
 * @override_id asc_duplicate__PUu16_u16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination uint16_t vector in Unified Buffer; must be 32-byte aligned and
 * receives count copies of the scalar.
 * @param [in] src Scalar uint16_t value that is copied into every destination element.
 * @param [in] count Number of consecutive uint16_t elements to fill; the implementation feeds it to the mask count
 * register so exactly this many elements are written.
 */
__aicore__ inline void asc_duplicate(__ubuf__ uint16_t* dst, uint16_t src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_broadcast
 * @brief Replicates the scalar src into bfloat16_t destination elements in Unified Buffer over repeat iterations,
 * honouring dst_block_stride and dst_repeat_stride.
 *
 * @override_id asc_duplicate__PUbf16_bf16_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination bfloat16_t vector in Unified Buffer; must be 32-byte aligned and
 * receives the replicated scalar over all iterations.
 * @param [in] src Scalar bfloat16_t value that is copied into every destination element.
 * @param [in] repeat Iteration count passed straight to vector_dup; each iteration covers the eight 32-byte DataBlocks
 * of one vector iteration.
 * @param [in] dst_block_stride Address stride between different DataBlocks inside a single destination iteration,
 * counted in DataBlocks of 32 bytes.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_duplicate(
    __ubuf__ bfloat16_t* dst, bfloat16_t src, uint8_t repeat, uint16_t dst_block_stride, uint16_t dst_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_broadcast
 * @brief Replicates the scalar src into the first count bfloat16_t elements of the destination vector in Unified
 * Buffer.
 *
 * @override_id asc_duplicate__PUbf16_bf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination bfloat16_t vector in Unified Buffer; must be 32-byte aligned and
 * receives count copies of the scalar.
 * @param [in] src Scalar bfloat16_t value that is copied into every destination element.
 * @param [in] count Number of consecutive bfloat16_t elements to fill; the implementation feeds it to the mask count
 * register so exactly this many elements are written.
 */
__aicore__ inline void asc_duplicate(__ubuf__ bfloat16_t* dst, bfloat16_t src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_broadcast
 * @brief Replicates the scalar src into float destination elements in Unified Buffer over repeat iterations, honouring
 * dst_block_stride and dst_repeat_stride.
 *
 * @override_id asc_duplicate__PUf32_f32_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination float vector in Unified Buffer; must be 32-byte aligned and
 * receives the replicated scalar over all iterations.
 * @param [in] src Scalar float value that is copied into every destination element.
 * @param [in] repeat Iteration count passed straight to vector_dup; each iteration covers the eight 32-byte DataBlocks
 * of one vector iteration.
 * @param [in] dst_block_stride Address stride between different DataBlocks inside a single destination iteration,
 * counted in DataBlocks of 32 bytes.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_duplicate(
    __ubuf__ float* dst, float src, uint8_t repeat, uint16_t dst_block_stride, uint16_t dst_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_broadcast
 * @brief Replicates the scalar src into the first count float elements of the destination vector in Unified Buffer.
 *
 * @override_id asc_duplicate__PUf32_f32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector in Unified Buffer; must be 32-byte aligned and
 * receives count copies of the scalar.
 * @param [in] src Scalar float value that is copied into every destination element.
 * @param [in] count Number of consecutive float elements to fill; the implementation feeds it to the mask count
 * register so exactly this many elements are written.
 */
__aicore__ inline void asc_duplicate(__ubuf__ float* dst, float src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_broadcast
 * @brief Replicates the scalar src into int32_t destination elements in Unified Buffer over repeat iterations,
 * honouring dst_block_stride and dst_repeat_stride.
 *
 * @override_id asc_duplicate__PUi32_i32_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination int32_t vector in Unified Buffer; must be 32-byte aligned and
 * receives the replicated scalar over all iterations.
 * @param [in] src Scalar int32_t value that is copied into every destination element.
 * @param [in] repeat Iteration count passed straight to vector_dup; each iteration covers the eight 32-byte DataBlocks
 * of one vector iteration.
 * @param [in] dst_block_stride Address stride between different DataBlocks inside a single destination iteration,
 * counted in DataBlocks of 32 bytes.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_duplicate(
    __ubuf__ int32_t* dst, int32_t src, uint8_t repeat, uint16_t dst_block_stride, uint16_t dst_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_broadcast
 * @brief Replicates the scalar src into the first count int32_t elements of the destination vector in Unified Buffer.
 *
 * @override_id asc_duplicate__PUi32_i32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination int32_t vector in Unified Buffer; must be 32-byte aligned and
 * receives count copies of the scalar.
 * @param [in] src Scalar int32_t value that is copied into every destination element.
 * @param [in] count Number of consecutive int32_t elements to fill; the implementation feeds it to the mask count
 * register so exactly this many elements are written.
 */
__aicore__ inline void asc_duplicate(__ubuf__ int32_t* dst, int32_t src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_broadcast
 * @brief Replicates the scalar src into uint32_t destination elements in Unified Buffer over repeat iterations,
 * honouring dst_block_stride and dst_repeat_stride.
 *
 * @override_id asc_duplicate__PUu32_u32_u8_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination uint32_t vector in Unified Buffer; must be 32-byte aligned and
 * receives the replicated scalar over all iterations.
 * @param [in] src Scalar uint32_t value that is copied into every destination element.
 * @param [in] repeat Iteration count passed straight to vector_dup; each iteration covers the eight 32-byte DataBlocks
 * of one vector iteration.
 * @param [in] dst_block_stride Address stride between different DataBlocks inside a single destination iteration,
 * counted in DataBlocks of 32 bytes.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_duplicate(
    __ubuf__ uint32_t* dst, uint32_t src, uint8_t repeat, uint16_t dst_block_stride, uint16_t dst_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_broadcast
 * @brief Replicates the scalar src into the first count uint32_t elements of the destination vector in Unified Buffer.
 *
 * @override_id asc_duplicate__PUu32_u32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination uint32_t vector in Unified Buffer; must be 32-byte aligned and
 * receives count copies of the scalar.
 * @param [in] src Scalar uint32_t value that is copied into every destination element.
 * @param [in] count Number of consecutive uint32_t elements to fill; the implementation feeds it to the mask count
 * register so exactly this many elements are written.
 */
__aicore__ inline void asc_duplicate(__ubuf__ uint32_t* dst, uint32_t src, uint32_t count);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_BROADCAST_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_BROADCAST_H
#endif
