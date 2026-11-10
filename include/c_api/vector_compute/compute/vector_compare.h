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
#error "vector_compute/compute/vector_compare.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_COMPARE_H
#endif

#ifndef INCLUDE_C_API_VECTOR_COMPUTE_COMPUTE_VECTOR_COMPARE_H
#define INCLUDE_C_API_VECTOR_COMPUTE_COMPUTE_VECTOR_COMPARE_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/memory_base_impl/vector_compare_intf_impl.h"
#endif
/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Retrieves the comparison result of a preceding Compare operation into a buffer.
 *
 * @override_id asc_get_cmp_mask__PUv
 * @archs 2201
 * @param [out] dst Address in UB that receives the result of the comparison operation.
 */
__aicore__ inline void asc_get_cmp_mask(__ubuf__ void* dst);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Sets the mask that asc_select uses to choose elements.
 *
 * @override_id asc_set_cmp_mask__PUv
 * @archs 2201
 * @param [in] sel_mask Start address of the mask used for the selection.
 */
__aicore__ inline void asc_set_cmp_mask(__ubuf__ void* sel_mask);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares src0 and src1 for equality element-wise and leaves one result bit per element in the comparison mask
 * register.
 *
 * @override_id asc_eq__PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in] src0 Start address of the half (fp16) source vector in Unified Buffer holding the left-hand comparison
 * operands.
 * @param [in] src1 Start address of the half (fp16) source vector in Unified Buffer holding the right-hand comparison
 * operands.
 * @param [in] repeat Number of iterations of the high-dimensional slice; the page notes that in the register form a
 * fixed 256 bytes are processed and this field has no effect.
 * @param [in] dst_block_stride Address stride in DataBlock units for the destination side within one iteration (the
 * comparison result register in this form).
 * @param [in] src0_block_stride Address stride in DataBlock units between the DataBlocks of source src0 within one
 * iteration.
 * @param [in] src1_block_stride Address stride in DataBlock units between the DataBlocks of source src1 within one
 * iteration.
 * @param [in] dst_repeat_stride Address stride in DataBlock units between the same destination DataBlock in adjacent
 * iterations (result register layout).
 * @param [in] src0_repeat_stride Address stride in DataBlock units between the same DataBlock of source src0 in
 * adjacent iterations.
 * @param [in] src1_repeat_stride Address stride in DataBlock units between the same DataBlock of source src1 in
 * adjacent iterations.
 */
__aicore__ inline void asc_eq(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares src0 and src1 for equality element-wise and leaves one result bit per element in the comparison mask
 * register.
 *
 * @override_id asc_eq__PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in] src0 Start address of the float (fp32) source vector in Unified Buffer holding the left-hand comparison
 * operands.
 * @param [in] src1 Start address of the float (fp32) source vector in Unified Buffer holding the right-hand comparison
 * operands.
 * @param [in] repeat Number of iterations of the high-dimensional slice; the page notes that in the register form a
 * fixed 256 bytes are processed and this field has no effect.
 * @param [in] dst_block_stride Address stride in DataBlock units for the destination side within one iteration (the
 * comparison result register in this form).
 * @param [in] src0_block_stride Address stride in DataBlock units between the DataBlocks of source src0 within one
 * iteration.
 * @param [in] src1_block_stride Address stride in DataBlock units between the DataBlocks of source src1 within one
 * iteration.
 * @param [in] dst_repeat_stride Address stride in DataBlock units between the same destination DataBlock in adjacent
 * iterations (result register layout).
 * @param [in] src0_repeat_stride Address stride in DataBlock units between the same DataBlock of source src0 in
 * adjacent iterations.
 * @param [in] src1_repeat_stride Address stride in DataBlock units between the same DataBlock of source src1 in
 * adjacent iterations.
 */
__aicore__ inline void asc_eq(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares src0 and src1 for equality element-wise and writes one result bit per element into dst.
 *
 * @override_id asc_eq__PUu8_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the byte-packed result buffer in Unified Buffer; each element comparison
 * contributes one bit and the bits are ordered little-endian.
 * @param [in] src0 Start address of the half (fp16) source vector in Unified Buffer holding the left-hand comparison
 * operands.
 * @param [in] src1 Start address of the half (fp16) source vector in Unified Buffer holding the right-hand comparison
 * operands.
 * @param [in] repeat Number of iterations of the high-dimensional slice performed by the comparison instruction.
 * @param [in] dst_block_stride Address stride in DataBlock units for the destination side within one iteration.
 * @param [in] src0_block_stride Address stride in DataBlock units between the DataBlocks of source src0 within one
 * iteration.
 * @param [in] src1_block_stride Address stride in DataBlock units between the DataBlocks of source src1 within one
 * iteration.
 * @param [in] dst_repeat_stride Address stride in DataBlock units between the same destination DataBlock in adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride in DataBlock units between the same DataBlock of source src0 in
 * adjacent iterations.
 * @param [in] src1_repeat_stride Address stride in DataBlock units between the same DataBlock of source src1 in
 * adjacent iterations.
 */
__aicore__ inline void asc_eq(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares src0 and src1 for equality element-wise and writes one result bit per element into dst.
 *
 * @override_id asc_eq__PUu8_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the byte-packed result buffer in Unified Buffer; each element comparison
 * contributes one bit and the bits are ordered little-endian.
 * @param [in] src0 Start address of the float (fp32) source vector in Unified Buffer holding the left-hand comparison
 * operands.
 * @param [in] src1 Start address of the float (fp32) source vector in Unified Buffer holding the right-hand comparison
 * operands.
 * @param [in] repeat Number of iterations of the high-dimensional slice performed by the comparison instruction.
 * @param [in] dst_block_stride Address stride in DataBlock units for the destination side within one iteration.
 * @param [in] src0_block_stride Address stride in DataBlock units between the DataBlocks of source src0 within one
 * iteration.
 * @param [in] src1_block_stride Address stride in DataBlock units between the DataBlocks of source src1 within one
 * iteration.
 * @param [in] dst_repeat_stride Address stride in DataBlock units between the same destination DataBlock in adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride in DataBlock units between the same DataBlock of source src0 in
 * adjacent iterations.
 * @param [in] src1_repeat_stride Address stride in DataBlock units between the same DataBlock of source src1 in
 * adjacent iterations.
 */
__aicore__ inline void asc_eq(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares src0 and src1 for equality element-wise and writes one result bit per element into dst.
 *
 * @override_id asc_eq__PUu8_PUi32_PUi32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the byte-packed result buffer in Unified Buffer; each element comparison
 * contributes one bit and the bits are ordered little-endian.
 * @param [in] src0 Start address of the 32-bit signed integer (int32_t) source vector in Unified Buffer holding the
 * left-hand comparison operands.
 * @param [in] src1 Start address of the 32-bit signed integer (int32_t) source vector in Unified Buffer holding the
 * right-hand comparison operands.
 * @param [in] repeat Number of iterations of the high-dimensional slice performed by the comparison instruction.
 * @param [in] dst_block_stride Address stride in DataBlock units for the destination side within one iteration.
 * @param [in] src0_block_stride Address stride in DataBlock units between the DataBlocks of source src0 within one
 * iteration.
 * @param [in] src1_block_stride Address stride in DataBlock units between the DataBlocks of source src1 within one
 * iteration.
 * @param [in] dst_repeat_stride Address stride in DataBlock units between the same destination DataBlock in adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride in DataBlock units between the same DataBlock of source src0 in
 * adjacent iterations.
 * @param [in] src1_repeat_stride Address stride in DataBlock units between the same DataBlock of source src1 in
 * adjacent iterations.
 */
__aicore__ inline void asc_eq(
    __ubuf__ uint8_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares every half element of src with scalar value using > and packs one result bit per element into dst,
 * with results ordered little-endian.
 *
 * @override_id asc_gt_scalar__PUu8_PUf16_f16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the packed comparison bits (one bit per element,
 * little-endian).
 * @param [in] src Source vector start address of half elements compared against value.
 * @param [in] value Scalar value used as the right-hand side of the > comparison.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of the source.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations.
 */
__aicore__ inline void asc_gt_scalar(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares every float element of src with scalar value using > and packs one result bit per element into dst,
 * with results ordered little-endian.
 *
 * @override_id asc_gt_scalar__PUu8_PUf32_f32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the packed comparison bits (one bit per element,
 * little-endian).
 * @param [in] src Source vector start address of float elements compared against value.
 * @param [in] value Scalar value used as the right-hand side of the > comparison.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of the source.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations.
 */
__aicore__ inline void asc_gt_scalar(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares the half elements of src0 and src1 for not equal to (src0 != src1) and leaves one result bit per
 * element in the comparison mask register, to be read back with asc_get_cmp_mask.
 *
 * @override_id asc_ne__PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in] src0 Start address in Unified Buffer of the first half operand of the comparison; read only.
 * @param [in] src1 Start address in Unified Buffer of the second half operand of the comparison; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; in the register form the page states that it has no
 * effect because exactly 256 Byte are processed.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for the
 * destination, counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for the destination,
 * counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 */
__aicore__ inline void asc_ne(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares the float elements of src0 and src1 for not equal to (src0 != src1) and leaves one result bit per
 * element in the comparison mask register, to be read back with asc_get_cmp_mask.
 *
 * @override_id asc_ne__PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in] src0 Start address in Unified Buffer of the first float operand of the comparison; read only.
 * @param [in] src1 Start address in Unified Buffer of the second float operand of the comparison; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; in the register form the page states that it has no
 * effect because exactly 256 Byte are processed.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for the
 * destination, counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for the destination,
 * counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 */
__aicore__ inline void asc_ne(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares the half elements of src0 and src1 for not equal to (src0 != src1) and writes one result bit per
 * element into the packed uint8_t destination at dst.
 *
 * @override_id asc_ne__PUu8_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the uint8_t destination that receives the packed comparison bits
 * in little-endian element order; written and never read.
 * @param [in] src0 Start address in Unified Buffer of the first half operand of the comparison; read only.
 * @param [in] src1 Start address in Unified Buffer of the second half operand of the comparison; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; in the register form the page states that it has no
 * effect because exactly 256 Byte are processed.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for the
 * destination, counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for the destination,
 * counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 */
__aicore__ inline void asc_ne(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares the float elements of src0 and src1 for not equal to (src0 != src1) and writes one result bit per
 * element into the packed uint8_t destination at dst.
 *
 * @override_id asc_ne__PUu8_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the uint8_t destination that receives the packed comparison bits
 * in little-endian element order; written and never read.
 * @param [in] src0 Start address in Unified Buffer of the first float operand of the comparison; read only.
 * @param [in] src1 Start address in Unified Buffer of the second float operand of the comparison; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; in the register form the page states that it has no
 * effect because exactly 256 Byte are processed.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for the
 * destination, counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for the destination,
 * counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 */
__aicore__ inline void asc_ne(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares every half element of src with scalar value using >= and packs one result bit per element into dst,
 * with results ordered little-endian.
 *
 * @override_id asc_ge_scalar__PUu8_PUf16_f16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the packed comparison bits (one bit per element,
 * little-endian).
 * @param [in] src Source vector start address of half elements compared against value.
 * @param [in] value Scalar value used as the right-hand side of the >= comparison.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of the source.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations.
 */
__aicore__ inline void asc_ge_scalar(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares every float element of src with scalar value using >= and packs one result bit per element into dst,
 * with results ordered little-endian.
 *
 * @override_id asc_ge_scalar__PUu8_PUf32_f32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the packed comparison bits (one bit per element,
 * little-endian).
 * @param [in] src Source vector start address of float elements compared against value.
 * @param [in] value Scalar value used as the right-hand side of the >= comparison.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of the source.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations.
 */
__aicore__ inline void asc_ge_scalar(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares every element of src with the scalar value for equality and packs the resulting predicate bits into
 * dst for half sources.
 *
 * @override_id asc_eq_scalar__PUu8_PUf16_f16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand holding the packed comparison bits (one bit per source
 * element, little-endian byte order); must be 32-byte aligned.
 * @param [in] src Start address of the source half vector operand compared element-wise with value; must be 32-byte
 * aligned.
 * @param [in] value Scalar half value that each element of src is tested against.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of dst.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of src.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of dst in adjacent iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of src in adjacent iterations.
 */
__aicore__ inline void asc_eq_scalar(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares every element of src with the scalar value for equality and packs the resulting predicate bits into
 * dst for float sources.
 *
 * @override_id asc_eq_scalar__PUu8_PUf32_f32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand holding the packed comparison bits (one bit per source
 * element, little-endian byte order); must be 32-byte aligned.
 * @param [in] src Start address of the source float vector operand compared element-wise with value; must be 32-byte
 * aligned.
 * @param [in] value Scalar float value that each element of src is tested against.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of dst.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of src.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of dst in adjacent iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of src in adjacent iterations.
 */
__aicore__ inline void asc_eq_scalar(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares every element of src with the scalar value for equality and packs the resulting predicate bits into
 * dst for int32_t sources.
 *
 * @override_id asc_eq_scalar__PUu8_PUi32_i32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand holding the packed comparison bits (one bit per source
 * element, little-endian byte order); must be 32-byte aligned.
 * @param [in] src Start address of the source int32_t vector operand compared element-wise with value; must be 32-byte
 * aligned.
 * @param [in] value Scalar int32_t value that each element of src is tested against.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of dst.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of src.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of dst in adjacent iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of src in adjacent iterations.
 */
__aicore__ inline void asc_eq_scalar(
    __ubuf__ uint8_t* dst, __ubuf__ int32_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares every half element of src with scalar value using < and packs one result bit per element into dst in
 * little-endian order.
 *
 * @override_id asc_lt_scalar__PUu8_PUf16_f16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the packed comparison bits (one bit per element,
 * little-endian).
 * @param [in] src Source vector start address of half elements compared against value.
 * @param [in] value Scalar value used as the right-hand side of the < comparison.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of the source.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations.
 */
__aicore__ inline void asc_lt_scalar(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares every float element of src with scalar value using < and packs one result bit per element into dst in
 * little-endian order.
 *
 * @override_id asc_lt_scalar__PUu8_PUf32_f32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the packed comparison bits (one bit per element,
 * little-endian).
 * @param [in] src Source vector start address of float elements compared against value.
 * @param [in] value Scalar value used as the right-hand side of the < comparison.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of the source.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations.
 */
__aicore__ inline void asc_lt_scalar(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares the half elements of src0 and src1 for greater than or equal to (src0 >= src1) and leaves one result
 * bit per element in the comparison mask register, to be read back with asc_get_cmp_mask.
 *
 * @override_id asc_ge__PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in] src0 Start address in Unified Buffer of the first half operand of the comparison; read only.
 * @param [in] src1 Start address in Unified Buffer of the second half operand of the comparison; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; in the register form the page states that it has no
 * effect because exactly 256 Byte are processed.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for the
 * destination, counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for the destination,
 * counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 */
__aicore__ inline void asc_ge(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares the float elements of src0 and src1 for greater than or equal to (src0 >= src1) and leaves one result
 * bit per element in the comparison mask register, to be read back with asc_get_cmp_mask.
 *
 * @override_id asc_ge__PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in] src0 Start address in Unified Buffer of the first float operand of the comparison; read only.
 * @param [in] src1 Start address in Unified Buffer of the second float operand of the comparison; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; in the register form the page states that it has no
 * effect because exactly 256 Byte are processed.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for the
 * destination, counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for the destination,
 * counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 */
__aicore__ inline void asc_ge(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares the half elements of src0 and src1 for greater than or equal to (src0 >= src1) and writes one result
 * bit per element into the packed uint8_t destination at dst.
 *
 * @override_id asc_ge__PUu8_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the uint8_t destination that receives the packed comparison bits
 * in little-endian element order; written, never read.
 * @param [in] src0 Start address in Unified Buffer of the first half operand of the comparison; read only.
 * @param [in] src1 Start address in Unified Buffer of the second half operand of the comparison; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; in the register form the page states that it has no
 * effect because exactly 256 Byte are processed.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for the
 * destination, counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for the destination,
 * counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 */
__aicore__ inline void asc_ge(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares the float elements of src0 and src1 for greater than or equal to (src0 >= src1) and writes one result
 * bit per element into the packed uint8_t destination at dst.
 *
 * @override_id asc_ge__PUu8_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the uint8_t destination that receives the packed comparison bits
 * in little-endian element order; written, never read.
 * @param [in] src0 Start address in Unified Buffer of the first float operand of the comparison; read only.
 * @param [in] src1 Start address in Unified Buffer of the second float operand of the comparison; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; in the register form the page states that it has no
 * effect because exactly 256 Byte are processed.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for the
 * destination, counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for the destination,
 * counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 */
__aicore__ inline void asc_ge(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares the half elements of src0 and src1 for less than (src0 < src1) and leaves one result bit per element
 * in the comparison mask register, to be read back with asc_get_cmp_mask.
 *
 * @override_id asc_lt__PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in] src0 Start address in Unified Buffer of the first half operand of the comparison; read only.
 * @param [in] src1 Start address in Unified Buffer of the second half operand of the comparison; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; in the register form the page states that it has no
 * effect because exactly 256 Byte are processed.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for the
 * destination, counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for the destination,
 * counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 */
__aicore__ inline void asc_lt(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares the float elements of src0 and src1 for less than (src0 < src1) and leaves one result bit per element
 * in the comparison mask register, to be read back with asc_get_cmp_mask.
 *
 * @override_id asc_lt__PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in] src0 Start address in Unified Buffer of the first float operand of the comparison; read only.
 * @param [in] src1 Start address in Unified Buffer of the second float operand of the comparison; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; in the register form the page states that it has no
 * effect because exactly 256 Byte are processed.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for the
 * destination, counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for the destination,
 * counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 */
__aicore__ inline void asc_lt(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares the half elements of src0 and src1 for less than (src0 < src1) and writes one result bit per element
 * into the packed uint8_t destination at dst.
 *
 * @override_id asc_lt__PUu8_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the uint8_t destination that receives the packed comparison bits
 * in little-endian element order; written and never read.
 * @param [in] src0 Start address in Unified Buffer of the first half operand of the comparison; read only.
 * @param [in] src1 Start address in Unified Buffer of the second half operand of the comparison; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; in the register form the page states that it has no
 * effect because exactly 256 Byte are processed.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for the
 * destination, counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for the destination,
 * counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 */
__aicore__ inline void asc_lt(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares the float elements of src0 and src1 for less than (src0 < src1) and writes one result bit per element
 * into the packed uint8_t destination at dst.
 *
 * @override_id asc_lt__PUu8_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the uint8_t destination that receives the packed comparison bits
 * in little-endian element order; written and never read.
 * @param [in] src0 Start address in Unified Buffer of the first float operand of the comparison; read only.
 * @param [in] src1 Start address in Unified Buffer of the second float operand of the comparison; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; in the register form the page states that it has no
 * effect because exactly 256 Byte are processed.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for the
 * destination, counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for the destination,
 * counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 */
__aicore__ inline void asc_lt(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares every half element of src with scalar value for inequality and packs one result bit per element into
 * dst.
 *
 * @override_id asc_ne_scalar__PUu8_PUf16_f16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the packed comparison bits, one bit per element in
 * little-endian order.
 * @param [in] src Source vector start address of half elements compared against value.
 * @param [in] value Scalar value used as the right-hand side of the != comparison.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of the source.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations.
 */
__aicore__ inline void asc_ne_scalar(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares every float element of src with scalar value for inequality and packs one result bit per element into
 * dst.
 *
 * @override_id asc_ne_scalar__PUu8_PUf32_f32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the packed comparison bits, one bit per element in
 * little-endian order.
 * @param [in] src Source vector start address of float elements compared against value.
 * @param [in] value Scalar value used as the right-hand side of the != comparison.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of the source.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations.
 */
__aicore__ inline void asc_ne_scalar(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares the half elements of src0 and src1 for greater than (src0 > src1) and leaves one result bit per
 * element in the comparison mask register, to be read back with asc_get_cmp_mask.
 *
 * @override_id asc_gt__PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in] src0 Start address in Unified Buffer of the first half operand of the comparison; read only.
 * @param [in] src1 Start address in Unified Buffer of the second half operand of the comparison; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; in the register form the page states that it has no
 * effect because exactly 256 Byte are processed.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for the
 * destination, counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for the destination,
 * counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 */
__aicore__ inline void asc_gt(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares the float elements of src0 and src1 for greater than (src0 > src1) and leaves one result bit per
 * element in the comparison mask register, to be read back with asc_get_cmp_mask.
 *
 * @override_id asc_gt__PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in] src0 Start address in Unified Buffer of the first float operand of the comparison; read only.
 * @param [in] src1 Start address in Unified Buffer of the second float operand of the comparison; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; in the register form the page states that it has no
 * effect because exactly 256 Byte are processed.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for the
 * destination, counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for the destination,
 * counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 */
__aicore__ inline void asc_gt(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares the half elements of src0 and src1 for greater than (src0 > src1) and writes one result bit per
 * element into the packed uint8_t destination at dst.
 *
 * @override_id asc_gt__PUu8_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the uint8_t destination that receives the packed comparison bits
 * in little-endian element order; written, never read.
 * @param [in] src0 Start address in Unified Buffer of the first half operand of the comparison; read only.
 * @param [in] src1 Start address in Unified Buffer of the second half operand of the comparison; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; in the register form the page states that it has no
 * effect because exactly 256 Byte are processed.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for the
 * destination, counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for the destination,
 * counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 */
__aicore__ inline void asc_gt(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares the float elements of src0 and src1 for greater than (src0 > src1) and writes one result bit per
 * element into the packed uint8_t destination at dst.
 *
 * @override_id asc_gt__PUu8_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the uint8_t destination that receives the packed comparison bits
 * in little-endian element order; written, never read.
 * @param [in] src0 Start address in Unified Buffer of the first float operand of the comparison; read only.
 * @param [in] src1 Start address in Unified Buffer of the second float operand of the comparison; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; in the register form the page states that it has no
 * effect because exactly 256 Byte are processed.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for the
 * destination, counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for the destination,
 * counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 */
__aicore__ inline void asc_gt(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares the half elements of src0 and src1 for less than or equal to (src0 <= src1) and leaves one result bit
 * per element in the comparison mask register, to be read back with asc_get_cmp_mask.
 *
 * @override_id asc_le__PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in] src0 Start address in Unified Buffer of the first half operand of the comparison; read only.
 * @param [in] src1 Start address in Unified Buffer of the second half operand of the comparison; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; in the register form the page states that it has no
 * effect because exactly 256 Byte are processed.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for the
 * destination, counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for the destination,
 * counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 */
__aicore__ inline void asc_le(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares the float elements of src0 and src1 for less than or equal to (src0 <= src1) and leaves one result
 * bit per element in the comparison mask register, to be read back with asc_get_cmp_mask.
 *
 * @override_id asc_le__PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in] src0 Start address in Unified Buffer of the first float operand of the comparison; read only.
 * @param [in] src1 Start address in Unified Buffer of the second float operand of the comparison; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; in the register form the page states that it has no
 * effect because exactly 256 Byte are processed.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for the
 * destination, counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for the destination,
 * counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 */
__aicore__ inline void asc_le(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares the half elements of src0 and src1 for less than or equal to (src0 <= src1) and writes one result bit
 * per element into the packed uint8_t destination at dst.
 *
 * @override_id asc_le__PUu8_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the uint8_t destination that receives the packed comparison bits
 * in little-endian element order; written and never read.
 * @param [in] src0 Start address in Unified Buffer of the first half operand of the comparison; read only.
 * @param [in] src1 Start address in Unified Buffer of the second half operand of the comparison; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; in the register form the page states that it has no
 * effect because exactly 256 Byte are processed.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for the
 * destination, counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for the destination,
 * counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 */
__aicore__ inline void asc_le(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares the float elements of src0 and src1 for less than or equal to (src0 <= src1) and writes one result
 * bit per element into the packed uint8_t destination at dst.
 *
 * @override_id asc_le__PUu8_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the uint8_t destination that receives the packed comparison bits
 * in little-endian element order; written and never read.
 * @param [in] src0 Start address in Unified Buffer of the first float operand of the comparison; read only.
 * @param [in] src1 Start address in Unified Buffer of the second float operand of the comparison; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; in the register form the page states that it has no
 * effect because exactly 256 Byte are processed.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for the
 * destination, counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for the destination,
 * counted in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t here).
 */
__aicore__ inline void asc_le(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares every half element of src with scalar value using <= and packs one result bit per element into dst,
 * with results ordered little-endian.
 *
 * @override_id asc_le_scalar__PUu8_PUf16_f16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the packed comparison bits (one bit per element,
 * little-endian).
 * @param [in] src Source vector start address of half elements compared against value.
 * @param [in] value Scalar value used as the right-hand side of the <= comparison.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of the source.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations.
 */
__aicore__ inline void asc_le_scalar(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_compare
 * @brief Compares every float element of src with scalar value using <= and packs one result bit per element into dst,
 * with results ordered little-endian.
 *
 * @override_id asc_le_scalar__PUu8_PUf32_f32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the packed comparison bits (one bit per element,
 * little-endian).
 * @param [in] src Source vector start address of float elements compared against value.
 * @param [in] value Scalar value used as the right-hand side of the <= comparison.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of the source.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations.
 */
__aicore__ inline void asc_le_scalar(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_COMPARE_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_COMPARE_H
#endif
