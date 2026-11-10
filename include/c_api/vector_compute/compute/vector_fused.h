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
#error "vector_compute/compute/vector_fused.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_FUSED_H
#endif

#ifndef INCLUDE_C_API_VECTOR_COMPUTE_COMPUTE_VECTOR_FUSED_H
#define INCLUDE_C_API_VECTOR_COMPUTE_COMPUTE_VECTOR_FUSED_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/memory_base_impl/vector_fused_intf_impl.h"
#endif
/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Builds one DEQSCALE register value directly from a float scale, a 9-bit effective int16_t offset and a
 * signedness flag.
 *
 * @override_id asc_set_deq_scale__f32_i16_b
 * @archs 2201
 * @param [in] scale Float scale factor of the quantization; the doc records no numeric restriction for this overload.
 * @param [in] offset int16_t quantization offset; only the low 9 bits are effective per the doc table.
 * @param [in] sign_mode bool flag selecting whether the quantized result is signed (true) or not (false).
 */
__aicore__ inline void asc_set_deq_scale(float scale, int16_t offset, bool sign_mode);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Packs 16 groups of scale, offset and sign-mode quantization parameters into the caller-supplied tmp buffer and
 * points the DEQSCALE register at it.
 *
 * @override_id asc_set_deq_scale__PUu64_Avdeqf32_Avdeqi16_Avdeqb
 * @archs 2201
 * @param [out] tmp Caller-supplied UB scratch buffer of at least 16 uint64_t entries; the impl writes one packed
 * DEQSCALE config word per entry and then programs the register with its address.
 * @param [in] scale_arr Array of ASC_VDEQ_SIZE (16) float scale factors, one per quantization group.
 * @param [in] offset_arr Array of ASC_VDEQ_SIZE (16) int16_t offsets, one per quantization group.
 * @param [in] sign_mode_arr Array of ASC_VDEQ_SIZE (16) bool flags telling whether each quantized result is signed.
 */
__aicore__ inline void asc_set_deq_scale(
    __ubuf__ uint64_t* tmp, float scale_arr[ASC_VDEQ_SIZE], int16_t offset_arr[ASC_VDEQ_SIZE],
    bool sign_mode_arr[ASC_VDEQ_SIZE]);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Loads a half-precision scale into the DEQSCALE register for int32_t-to-half dequantization casts.
 *
 * @override_id asc_set_deq_scale__f16
 * @archs 2201
 * @param [in] scale half quantization scale written to the DEQSCALE register; used by int32_t to half cast scenarios
 * such as cast_deq.
 */
__aicore__ inline void asc_set_deq_scale(half scale);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Multiplies half vectors src0 and src1 element-wise over repeat iterations, rounding with RINT and saturating
 * into int8_t values in dst.
 *
 * @override_id asc_mul_cast_half2int8__PUi8_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the int8_t results; must be 32-byte aligned.
 * @param [in] src0 First source vector start address of half elements; must be 32-byte aligned.
 * @param [in] src1 Second source vector start address of half elements; must be 32-byte aligned.
 * @param [in] repeat Number of iterations of the fused multiply-cast.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src0_block_stride Address stride between DataBlocks inside one iteration of source 0.
 * @param [in] src1_block_stride Address stride between DataBlocks inside one iteration of source 1.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of adjacent source-0 iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of adjacent source-1 iterations.
 */
__aicore__ inline void asc_mul_cast_half2int8(
    __ubuf__ int8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Multiplies half vectors src0 and src1 element-wise, rounds the products with RINT and saturates them into
 * int8_t values stored in dst.
 *
 * @override_id asc_mul_cast_half2int8__PUi8_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the int8_t results; must be 32-byte aligned.
 * @param [in] src0 First source vector start address of half elements; must be 32-byte aligned.
 * @param [in] src1 Second source vector start address of half elements; must be 32-byte aligned.
 * @param [in] count Number of elements participating in the contiguous computation.
 */
__aicore__ inline void asc_mul_cast_half2int8(
    __ubuf__ int8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Multiplies half vectors src0 and src1 element-wise over repeat iterations, rounding with RINT and saturating
 * into uint8_t values in dst.
 *
 * @override_id asc_mul_cast_half2uint8__PUu8_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the uint8_t results; must be 32-byte aligned.
 * @param [in] src0 First source vector start address of half elements; must be 32-byte aligned.
 * @param [in] src1 Second source vector start address of half elements; must be 32-byte aligned.
 * @param [in] repeat Number of iterations of the fused multiply-cast.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src0_block_stride Address stride between DataBlocks inside one iteration of source 0.
 * @param [in] src1_block_stride Address stride between DataBlocks inside one iteration of source 1.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of adjacent source-0 iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of adjacent source-1 iterations.
 */
__aicore__ inline void asc_mul_cast_half2uint8(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Multiplies half vectors src0 and src1 element-wise, rounds the products with RINT and saturates them into
 * uint8_t values stored in dst.
 *
 * @override_id asc_mul_cast_half2uint8__PUu8_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the uint8_t results; must be 32-byte aligned.
 * @param [in] src0 First source vector start address of half elements; must be 32-byte aligned.
 * @param [in] src1 Second source vector start address of half elements; must be 32-byte aligned.
 * @param [in] count Number of elements participating in the contiguous computation.
 */
__aicore__ inline void asc_mul_cast_half2uint8(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Dequantizes int32_t elements of src to half over repeat iterations using the asc_set_deq_scale scale and
 * per-block/per-repeat strides.
 *
 * @override_id asc_deq_int322half__PUf16_PUi32_u8_u16_u16_u8_u8
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the dequantized half values; must be 32-byte aligned.
 * @param [in] src Source vector start address of int32_t elements; must be 32-byte aligned.
 * @param [in] repeat Number of iterations of the dequantization.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of the source.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations; carried
 * as uint8_t.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations; carried as
 * uint8_t.
 */
__aicore__ inline void asc_deq_int322half(
    __ubuf__ half* dst, __ubuf__ int32_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint8_t dst_repeat_stride, uint8_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Dequantizes count int32_t elements of src to half using the scale set by asc_set_deq_scale and stores the
 * results in dst.
 *
 * @override_id asc_deq_int322half__PUf16_PUi32_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the dequantized half values; must be 32-byte aligned.
 * @param [in] src Source vector start address of int32_t elements to be scaled and cast; must be 32-byte aligned.
 * @param [in] count Number of elements participating in the contiguous dequantization.
 */
__aicore__ inline void asc_deq_int322half(__ubuf__ half* dst, __ubuf__ int32_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Applies ReLU element-wise, writing max(src[i], 0) into dst for repeat iterations with explicit DataBlock
 * strides.
 *
 * @override_id asc_relu__PUf16_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the half (fp16) destination vector in Unified Buffer that receives the rectified
 * results.
 * @param [in] src Start address of the half (fp16) source vector in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations of the high-dimensional slice performed by the vector instruction.
 * @param [in] dst_block_stride Address stride in DataBlock units between the DataBlocks of the destination accessed
 * within one iteration.
 * @param [in] src_block_stride Address stride in DataBlock units between the DataBlocks of the source accessed within
 * one iteration.
 * @param [in] dst_repeat_stride Address stride in DataBlock units between the same DataBlock of the destination in two
 * adjacent iterations.
 * @param [in] src_repeat_stride Address stride in DataBlock units between the same DataBlock of the source in two
 * adjacent iterations.
 */
__aicore__ inline void asc_relu(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Applies ReLU element-wise, writing max(src[i], 0) into dst for the first count elements.
 *
 * @override_id asc_relu__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the half (fp16) destination vector in Unified Buffer that receives the rectified
 * results.
 * @param [in] src Start address of the half (fp16) source vector in Unified Buffer, read element-wise.
 * @param [in] count Number of consecutive half elements processed from the start addresses; the impl passes it to
 * asc_set_mask_count_begin before the vector instruction.
 */
__aicore__ inline void asc_relu(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Applies ReLU element-wise, writing max(src[i], 0) into dst for repeat iterations with explicit DataBlock
 * strides.
 *
 * @override_id asc_relu__PUf32_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the float (fp32) destination vector in Unified Buffer that receives the rectified
 * results.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations of the high-dimensional slice performed by the vector instruction.
 * @param [in] dst_block_stride Address stride in DataBlock units between the DataBlocks of the destination accessed
 * within one iteration.
 * @param [in] src_block_stride Address stride in DataBlock units between the DataBlocks of the source accessed within
 * one iteration.
 * @param [in] dst_repeat_stride Address stride in DataBlock units between the same DataBlock of the destination in two
 * adjacent iterations.
 * @param [in] src_repeat_stride Address stride in DataBlock units between the same DataBlock of the source in two
 * adjacent iterations.
 */
__aicore__ inline void asc_relu(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Applies ReLU element-wise, writing max(src[i], 0) into dst for the first count elements.
 *
 * @override_id asc_relu__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the float (fp32) destination vector in Unified Buffer that receives the rectified
 * results.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer, read element-wise.
 * @param [in] count Number of consecutive float elements processed from the start addresses; the impl passes it to
 * asc_set_mask_count_begin before the vector instruction.
 */
__aicore__ inline void asc_relu(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Applies ReLU element-wise, writing max(src[i], 0) into dst for repeat iterations with explicit DataBlock
 * strides.
 *
 * @override_id asc_relu__PUi32_PUi32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the 32-bit signed integer (int32_t) destination vector in Unified Buffer that
 * receives the rectified results.
 * @param [in] src Start address of the 32-bit signed integer (int32_t) source vector in Unified Buffer, read
 * element-wise.
 * @param [in] repeat Number of iterations of the high-dimensional slice performed by the vector instruction.
 * @param [in] dst_block_stride Address stride in DataBlock units between the DataBlocks of the destination accessed
 * within one iteration.
 * @param [in] src_block_stride Address stride in DataBlock units between the DataBlocks of the source accessed within
 * one iteration.
 * @param [in] dst_repeat_stride Address stride in DataBlock units between the same DataBlock of the destination in two
 * adjacent iterations.
 * @param [in] src_repeat_stride Address stride in DataBlock units between the same DataBlock of the source in two
 * adjacent iterations.
 */
__aicore__ inline void asc_relu(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Applies ReLU element-wise, writing max(src[i], 0) into dst for the first count elements.
 *
 * @override_id asc_relu__PUi32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the 32-bit signed integer (int32_t) destination vector in Unified Buffer that
 * receives the rectified results.
 * @param [in] src Start address of the 32-bit signed integer (int32_t) source vector in Unified Buffer, read
 * element-wise.
 * @param [in] count Number of consecutive int32_t elements processed from the start addresses; the impl passes it to
 * asc_set_mask_count_begin before the vector instruction.
 */
__aicore__ inline void asc_relu(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes dst[i] = (src0[i] * src1[i]) + dst[i] element-wise for the first count elements.
 *
 * @override_id asc_fma__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [in,out] dst Start address of the half (fp16) destination vector in Unified Buffer, read as the addend and
 * overwritten with the fused result.
 * @param [in] src0 Start address of the first half (fp16) source vector in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second half (fp16) source vector in Unified Buffer, read element-wise.
 * @param [in] count Number of consecutive half elements processed from the start addresses; the impl passes it to
 * asc_set_mask_count_begin before the vector instruction.
 */
__aicore__ inline void asc_fma(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes dst[i] = (src0[i] * src1[i]) + dst[i] element-wise for repeat iterations with explicit DataBlock
 * strides.
 *
 * @override_id asc_fma__PUf16_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in,out] dst Start address of the half (fp16) destination vector in Unified Buffer, read as the addend and
 * overwritten with the fused result.
 * @param [in] src0 Start address of the first half (fp16) source vector in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second half (fp16) source vector in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations of the high-dimensional slice performed by the vector instruction.
 * @param [in] dst_block_stride Address stride in DataBlock units between the DataBlocks of the destination accessed
 * within one iteration.
 * @param [in] src0_block_stride Address stride in DataBlock units between the DataBlocks of the first source accessed
 * within one iteration.
 * @param [in] src1_block_stride Address stride in DataBlock units between the DataBlocks of the second source accessed
 * within one iteration.
 * @param [in] dst_repeat_stride Address stride in DataBlock units between the same DataBlock of the destination in two
 * adjacent iterations.
 * @param [in] src0_repeat_stride Address stride in DataBlock units between the same DataBlock of the first source in
 * two adjacent iterations.
 * @param [in] src1_repeat_stride Address stride in DataBlock units between the same DataBlock of the second source in
 * two adjacent iterations.
 */
__aicore__ inline void asc_fma(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes dst[i] = (src0[i] * src1[i]) + dst[i] element-wise for the first count elements.
 *
 * @override_id asc_fma__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [in,out] dst Start address of the float (fp32) destination vector in Unified Buffer, read as the addend and
 * overwritten with the fused result.
 * @param [in] src0 Start address of the first float (fp32) source vector in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second float (fp32) source vector in Unified Buffer, read element-wise.
 * @param [in] count Number of consecutive float elements processed from the start addresses; the impl passes it to
 * asc_set_mask_count_begin before the vector instruction.
 */
__aicore__ inline void asc_fma(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes dst[i] = (src0[i] * src1[i]) + dst[i] element-wise for repeat iterations with explicit DataBlock
 * strides.
 *
 * @override_id asc_fma__PUf32_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in,out] dst Start address of the float (fp32) destination vector in Unified Buffer, read as the addend and
 * overwritten with the fused result.
 * @param [in] src0 Start address of the first float (fp32) source vector in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second float (fp32) source vector in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations of the high-dimensional slice performed by the vector instruction.
 * @param [in] dst_block_stride Address stride in DataBlock units between the DataBlocks of the destination accessed
 * within one iteration.
 * @param [in] src0_block_stride Address stride in DataBlock units between the DataBlocks of the first source accessed
 * within one iteration.
 * @param [in] src1_block_stride Address stride in DataBlock units between the DataBlocks of the second source accessed
 * within one iteration.
 * @param [in] dst_repeat_stride Address stride in DataBlock units between the same DataBlock of the destination in two
 * adjacent iterations.
 * @param [in] src0_repeat_stride Address stride in DataBlock units between the same DataBlock of the first source in
 * two adjacent iterations.
 * @param [in] src1_repeat_stride Address stride in DataBlock units between the same DataBlock of the second source in
 * two adjacent iterations.
 */
__aicore__ inline void asc_fma(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes dst[i] = (src0[i] * src1[i]) + dst[i] element-wise with half source elements widened into the float
 * destination for the first count elements.
 *
 * @override_id asc_fma__PUf32_PUf16_PUf16_u32
 * @archs 2201
 * @param [in,out] dst Start address of the float (fp32) destination vector in Unified Buffer, read as the addend and
 * overwritten with the fused result.
 * @param [in] src0 Start address of the first half (fp16) source vector in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second half (fp16) source vector in Unified Buffer, read element-wise.
 * @param [in] count Number of consecutive float elements processed from the start addresses; the impl passes it to
 * asc_set_mask_count_begin before the vector instruction.
 */
__aicore__ inline void asc_fma(__ubuf__ float* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes dst[i] = (src0[i] * src1[i]) + dst[i] element-wise with half source elements widened into the float
 * destination for repeat iterations with explicit DataBlock strides.
 *
 * @override_id asc_fma__PUf32_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in,out] dst Start address of the float (fp32) destination vector in Unified Buffer, read as the addend and
 * overwritten with the fused result.
 * @param [in] src0 Start address of the first half (fp16) source vector in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second half (fp16) source vector in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations of the high-dimensional slice performed by the vector instruction.
 * @param [in] dst_block_stride Address stride in DataBlock units between the DataBlocks of the destination accessed
 * within one iteration.
 * @param [in] src0_block_stride Address stride in DataBlock units between the DataBlocks of the first source accessed
 * within one iteration.
 * @param [in] src1_block_stride Address stride in DataBlock units between the DataBlocks of the second source accessed
 * within one iteration.
 * @param [in] dst_repeat_stride Address stride in DataBlock units between the same DataBlock of the destination in two
 * adjacent iterations.
 * @param [in] src0_repeat_stride Address stride in DataBlock units between the same DataBlock of the first source in
 * two adjacent iterations.
 * @param [in] src1_repeat_stride Address stride in DataBlock units between the same DataBlock of the second source in
 * two adjacent iterations.
 */
__aicore__ inline void asc_fma(
    __ubuf__ float* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Multiplies the first count half elements addressed by src0 with the current values at dst, adds src1
 * element-wise, and writes the results back into dst.
 *
 * @override_id asc_mul_add__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [in,out] dst Start address in Unified Buffer of the half destination vector; the page formula dst_i = (src0_i
 * * dst_i) + src1_i reads dst as a multiplicand and writes the accumulated result back to it.
 * @param [in] src0 Start address in Unified Buffer of the first half source vector (multiplicand); read only.
 * @param [in] src1 Start address in Unified Buffer of the second half source vector (addend); read only.
 * @param [in] count Number of leading half elements to process, counted in elements rather than bytes; the impl turns
 * it into a mask count.
 */
__aicore__ inline void asc_mul_add(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Multiplies the half elements addressed by src0 with the current values at dst, adds src1 element-wise
 * following the repeat and DataBlock stride pattern, and writes the results back into dst.
 *
 * @override_id asc_mul_add__PUf16_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in,out] dst Start address in Unified Buffer of the half destination vector; the page formula dst_i = (src0_i
 * * dst_i) + src1_i reads dst as a multiplicand and writes the accumulated result back to it.
 * @param [in] src0 Start address in Unified Buffer of the first half source vector (multiplicand); read only.
 * @param [in] src1 Start address in Unified Buffer of the second half source vector (addend); read only.
 * @param [in] repeat Iteration count of the high-dimensional form; each iteration covers 8 DataBlocks of 32 Byte, so
 * repeat must not exceed the hardware limit of 255.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for dst, counted in
 * DataBlocks of 32 Byte (declared uint8_t in this overload).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t in this overload).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t in this overload).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for dst, counted in
 * DataBlocks of 32 Byte (declared uint8_t in this overload).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t in this overload).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t in this overload).
 */
__aicore__ inline void asc_mul_add(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Multiplies the first count float elements addressed by src0 with the current values at dst, adds src1
 * element-wise, and writes the results back into dst.
 *
 * @override_id asc_mul_add__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [in,out] dst Start address in Unified Buffer of the float destination vector; the page formula dst_i = (src0_i
 * * dst_i) + src1_i reads dst as a multiplicand and writes the accumulated result back to it.
 * @param [in] src0 Start address in Unified Buffer of the first float source vector (multiplicand); read only.
 * @param [in] src1 Start address in Unified Buffer of the second float source vector (addend); read only.
 * @param [in] count Number of leading float elements to process, counted in elements rather than bytes; the impl turns
 * it into a mask count.
 */
__aicore__ inline void asc_mul_add(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Multiplies the float elements addressed by src0 with the current values at dst, adds src1 element-wise
 * following the repeat and DataBlock stride pattern, and writes the results back into dst.
 *
 * @override_id asc_mul_add__PUf32_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in,out] dst Start address in Unified Buffer of the float destination vector; the page formula dst_i = (src0_i
 * * dst_i) + src1_i reads dst as a multiplicand and writes the accumulated result back to it.
 * @param [in] src0 Start address in Unified Buffer of the first float source vector (multiplicand); read only.
 * @param [in] src1 Start address in Unified Buffer of the second float source vector (addend); read only.
 * @param [in] repeat Iteration count of the high-dimensional form; each iteration covers 8 DataBlocks of 32 Byte, so
 * repeat must not exceed the hardware limit of 255.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for dst, counted in
 * DataBlocks of 32 Byte (declared uint8_t in this overload).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t in this overload).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t in this overload).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for dst, counted in
 * DataBlocks of 32 Byte (declared uint8_t in this overload).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t in this overload).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t in this overload).
 */
__aicore__ inline void asc_mul_add(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Multiplies the half elements addressed by src0 with the current values at dst, adds src1 element-wise, applies
 * ReLU and writes the results back into dst over the repeat and DataBlock stride pattern.
 *
 * @override_id asc_mul_add_relu__PUf16_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in,out] dst Start address in Unified Buffer of the half destination vector; the page formula dst_i =
 * relu((src0_i * dst_i) + src1_i) reads the old dst as a multiplicand and writes the activated result back.
 * @param [in] src0 Start address in Unified Buffer of the first half source vector (multiplicand); read only.
 * @param [in] src1 Start address in Unified Buffer of the second half source vector (addend); read only.
 * @param [in] repeat Iteration count of the high-dimensional form; each iteration covers 8 DataBlocks of 32 Byte, so
 * repeat must not exceed the hardware limit of 255.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for dst, counted in
 * DataBlocks of 32 Byte (declared uint8_t in this overload).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t in this overload).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t in this overload).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for dst, counted in
 * DataBlocks of 32 Byte (declared uint8_t in this overload).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t in this overload).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t in this overload).
 */
__aicore__ inline void asc_mul_add_relu(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Multiplies the first count half elements addressed by src0 with the current values at dst, adds src1
 * element-wise, applies ReLU and writes the results back into dst.
 *
 * @override_id asc_mul_add_relu__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [in,out] dst Start address in Unified Buffer of the half destination vector; the page formula dst_i =
 * relu((src0_i * dst_i) + src1_i) reads the old dst as a multiplicand and writes the activated result back.
 * @param [in] src0 Start address in Unified Buffer of the first half source vector (multiplicand); read only.
 * @param [in] src1 Start address in Unified Buffer of the second half source vector (addend); read only.
 * @param [in] count Number of leading half elements to process, counted in elements rather than bytes; the impl turns
 * it into a mask count.
 */
__aicore__ inline void asc_mul_add_relu(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Multiplies the float elements addressed by src0 with the current values at dst, adds src1 element-wise,
 * applies ReLU and writes the results back into dst over the repeat and DataBlock stride pattern.
 *
 * @override_id asc_mul_add_relu__PUf32_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in,out] dst Start address in Unified Buffer of the float destination vector; the page formula dst_i =
 * relu((src0_i * dst_i) + src1_i) reads the old dst as a multiplicand and writes the activated result back.
 * @param [in] src0 Start address in Unified Buffer of the first float source vector (multiplicand); read only.
 * @param [in] src1 Start address in Unified Buffer of the second float source vector (addend); read only.
 * @param [in] repeat Iteration count of the high-dimensional form; each iteration covers 8 DataBlocks of 32 Byte, so
 * repeat must not exceed the hardware limit of 255.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for dst, counted in
 * DataBlocks of 32 Byte (declared uint8_t in this overload).
 * @param [in] src0_block_stride Address distance between consecutive DataBlocks inside one iteration for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t in this overload).
 * @param [in] src1_block_stride Address distance between consecutive DataBlocks inside one iteration for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t in this overload).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for dst, counted in
 * DataBlocks of 32 Byte (declared uint8_t in this overload).
 * @param [in] src0_repeat_stride Address distance between the same DataBlock of adjacent iterations for src0, counted
 * in DataBlocks of 32 Byte (declared uint8_t in this overload).
 * @param [in] src1_repeat_stride Address distance between the same DataBlock of adjacent iterations for src1, counted
 * in DataBlocks of 32 Byte (declared uint8_t in this overload).
 */
__aicore__ inline void asc_mul_add_relu(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Multiplies the first count float elements addressed by src0 with the current values at dst, adds src1
 * element-wise, applies ReLU and writes the results back into dst.
 *
 * @override_id asc_mul_add_relu__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [in,out] dst Start address in Unified Buffer of the float destination vector; the page formula dst_i =
 * relu((src0_i * dst_i) + src1_i) reads the old dst as a multiplicand and writes the activated result back.
 * @param [in] src0 Start address in Unified Buffer of the first float source vector (multiplicand); read only.
 * @param [in] src1 Start address in Unified Buffer of the second float source vector (addend); read only.
 * @param [in] count Number of leading float elements to process, counted in elements rather than bytes; the impl turns
 * it into a mask count.
 */
__aicore__ inline void asc_mul_add_relu(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Dequantizes the first count int16_t elements of src into int8_t values with the DEQSCALE scale and offset and
 * stores them in the high half block of each DataBlock of dst.
 *
 * @override_id asc_deq_int162b8_h__PUi8_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int8_t elements that receives the
 * quantized values placed in the high half block of each DataBlock.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_deq_int162b8_h(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Dequantizes int16_t elements of src into int8_t values with the DEQSCALE scale and offset over repeat vector
 * iterations, storing them in the high half block of each DataBlock of dst with configurable DataBlock and repeat
 * strides.
 *
 * @override_id asc_deq_int162b8_h__PUi8_PUi16_u8_u16_u16_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int8_t elements that receives the
 * quantized values placed in the high half block of each DataBlock.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration for the source, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src_repeat_stride Address stride between the same DataBlock of consecutive iterations for the source,
 * counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_deq_int162b8_h(
    __ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint8_t dst_repeat_stride, uint8_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Dequantizes the first count int16_t elements of src into uint8_t values with the DEQSCALE scale and offset and
 * stores them in the high half block of each DataBlock of dst.
 *
 * @override_id asc_deq_int162b8_h__PUu8_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of uint8_t elements that receives the
 * quantized values placed in the high half block of each DataBlock.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_deq_int162b8_h(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Dequantizes int16_t elements of src into uint8_t values with the DEQSCALE scale and offset over repeat vector
 * iterations, storing them in the high half block of each DataBlock of dst with configurable DataBlock and repeat
 * strides.
 *
 * @override_id asc_deq_int162b8_h__PUu8_PUi16_u8_u16_u16_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of uint8_t elements that receives the
 * quantized values placed in the high half block of each DataBlock.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration for the source, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src_repeat_stride Address stride between the same DataBlock of consecutive iterations for the source,
 * counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_deq_int162b8_h(
    __ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint8_t dst_repeat_stride, uint8_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Dequantizes the first count int16_t elements of src into int8_t values with the DEQSCALE scale and offset and
 * stores them in the low half block of each DataBlock of dst.
 *
 * @override_id asc_deq_int162b8_l__PUi8_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int8_t elements that receives the
 * quantized values placed in the low half block of each DataBlock.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_deq_int162b8_l(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Dequantizes int16_t elements of src into int8_t values with the DEQSCALE scale and offset over repeat vector
 * iterations, storing them in the low half block of each DataBlock of dst with configurable DataBlock and repeat
 * strides.
 *
 * @override_id asc_deq_int162b8_l__PUi8_PUi16_u8_u16_u16_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int8_t elements that receives the
 * quantized values placed in the low half block of each DataBlock.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration for the source, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src_repeat_stride Address stride between the same DataBlock of consecutive iterations for the source,
 * counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_deq_int162b8_l(
    __ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint8_t dst_repeat_stride, uint8_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Dequantizes the first count int16_t elements of src into uint8_t values with the DEQSCALE scale and offset and
 * stores them in the low half block of each DataBlock of dst.
 *
 * @override_id asc_deq_int162b8_l__PUu8_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of uint8_t elements that receives the
 * quantized values placed in the low half block of each DataBlock.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_deq_int162b8_l(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Dequantizes int16_t elements of src into uint8_t values with the DEQSCALE scale and offset over repeat vector
 * iterations, storing them in the low half block of each DataBlock of dst with configurable DataBlock and repeat
 * strides.
 *
 * @override_id asc_deq_int162b8_l__PUu8_PUi16_u8_u16_u16_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of uint8_t elements that receives the
 * quantized values placed in the low half block of each DataBlock.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration for the source, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src_repeat_stride Address stride between the same DataBlock of consecutive iterations for the source,
 * counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_deq_int162b8_l(
    __ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint8_t dst_repeat_stride, uint8_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Applies the Leaky ReLU with scalar slope value to half elements addressed by src following the repeat and
 * DataBlock stride pattern and writes the per-iteration results into dst.
 *
 * @override_id asc_leakyrelu__PUf16_PUf16_f16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the half destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the half source vector holding the values to activate; read only.
 * @param [in] value Scalar half slope applied to elements that are less than or equal to 0; it is the alpha of the
 * Leaky ReLU and is never written.
 * @param [in] repeat Iteration count of the high-dimensional form; each iteration covers 8 DataBlocks of 32 Byte, so
 * repeat must not exceed the hardware limit of 255.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for dst, counted in
 * DataBlocks of 32 Byte (declared uint16_t in this overload).
 * @param [in] src_block_stride Address distance between consecutive DataBlocks inside one iteration for src, counted in
 * DataBlocks of 32 Byte (declared uint16_t in this overload).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for dst, counted in
 * DataBlocks of 32 Byte (declared uint16_t in this overload).
 * @param [in] src_repeat_stride Address distance between the same DataBlock of adjacent iterations for src, counted in
 * DataBlocks of 32 Byte (declared uint16_t in this overload).
 */
__aicore__ inline void asc_leakyrelu(
    __ubuf__ half* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Applies the Leaky ReLU with scalar slope value to the first count half elements addressed by src and writes
 * the results into dst.
 *
 * @override_id asc_leakyrelu__PUf16_PUf16_f16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the half destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the half source vector holding the values to activate; read only.
 * @param [in] value Scalar half slope applied to elements that are less than or equal to 0; it is the alpha of the
 * Leaky ReLU and is never written.
 * @param [in] count Number of leading half elements to process, counted in elements rather than bytes; the impl turns
 * it into a mask count.
 */
__aicore__ inline void asc_leakyrelu(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Applies the Leaky ReLU with scalar slope value to float elements addressed by src following the repeat and
 * DataBlock stride pattern and writes the per-iteration results into dst.
 *
 * @override_id asc_leakyrelu__PUf32_PUf32_f32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the float destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the float source vector holding the values to activate; read only.
 * @param [in] value Scalar float slope applied to elements that are less than or equal to 0; it is the alpha of the
 * Leaky ReLU and is never written.
 * @param [in] repeat Iteration count of the high-dimensional form; each iteration covers 8 DataBlocks of 32 Byte, so
 * repeat must not exceed the hardware limit of 255.
 * @param [in] dst_block_stride Address distance between consecutive DataBlocks inside one iteration for dst, counted in
 * DataBlocks of 32 Byte (declared uint16_t in this overload).
 * @param [in] src_block_stride Address distance between consecutive DataBlocks inside one iteration for src, counted in
 * DataBlocks of 32 Byte (declared uint16_t in this overload).
 * @param [in] dst_repeat_stride Address distance between the same DataBlock of adjacent iterations for dst, counted in
 * DataBlocks of 32 Byte (declared uint16_t in this overload).
 * @param [in] src_repeat_stride Address distance between the same DataBlock of adjacent iterations for src, counted in
 * DataBlocks of 32 Byte (declared uint16_t in this overload).
 */
__aicore__ inline void asc_leakyrelu(
    __ubuf__ float* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Applies the Leaky ReLU with scalar slope value to the first count float elements addressed by src and writes
 * the results into dst.
 *
 * @override_id asc_leakyrelu__PUf32_PUf32_f32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the float destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the float source vector holding the values to activate; read only.
 * @param [in] value Scalar float slope applied to elements that are less than or equal to 0; it is the alpha of the
 * Leaky ReLU and is never written.
 * @param [in] count Number of leading float elements to process, counted in elements rather than bytes; the impl turns
 * it into a mask count.
 */
__aicore__ inline void asc_leakyrelu(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i - src1_i) element-wise for count elements, writing half results.
 *
 * @override_id asc_sub_relu__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the half destination vector that receives relu(src0_i - src1_i);
 * it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first half source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second half source vector; it must be 32-byte aligned.
 * @param [in] count Number of half element pairs to process, counted in source elements.
 */
__aicore__ inline void asc_sub_relu(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i - src1_i) element-wise over a configurable repeat and DataBlock stride pattern, writing
 * half results.
 *
 * @override_id asc_sub_relu__PUf16_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the half destination vector that receives relu(src0_i - src1_i);
 * it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first half source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second half source vector; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride of src0 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride of src1 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_repeat_stride Address stride of src0 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_repeat_stride Address stride of src1 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_sub_relu(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i - src1_i) element-wise for count elements, writing float results.
 *
 * @override_id asc_sub_relu__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the float destination vector that receives relu(src0_i - src1_i);
 * it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first float source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second float source vector; it must be 32-byte aligned.
 * @param [in] count Number of float element pairs to process, counted in source elements.
 */
__aicore__ inline void asc_sub_relu(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i - src1_i) element-wise over a configurable repeat and DataBlock stride pattern, writing
 * float results.
 *
 * @override_id asc_sub_relu__PUf32_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the float destination vector that receives relu(src0_i - src1_i);
 * it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first float source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second float source vector; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride of src0 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride of src1 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_repeat_stride Address stride of src0 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_repeat_stride Address stride of src1 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_sub_relu(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i - src1_i) element-wise for count elements, writing int16_t results.
 *
 * @override_id asc_sub_relu__PUi16_PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the int16_t destination vector that receives relu(src0_i -
 * src1_i); it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first int16_t source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second int16_t source vector; it must be 32-byte aligned.
 * @param [in] count Number of int16_t element pairs to process, counted in source elements.
 */
__aicore__ inline void asc_sub_relu(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i - src1_i) element-wise over a configurable repeat and DataBlock stride pattern, writing
 * int16_t results.
 *
 * @override_id asc_sub_relu__PUi16_PUi16_PUi16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the int16_t destination vector that receives relu(src0_i -
 * src1_i); it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first int16_t source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second int16_t source vector; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride of src0 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride of src1 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_repeat_stride Address stride of src0 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_repeat_stride Address stride of src1 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_sub_relu(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i - src1_i) element-wise for count elements, writing int8_t results after the f162s8
 * conversion of the final result.
 *
 * @override_id asc_sub_relu__PUi8_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the int8_t destination vector that receives relu(src0_i -
 * src1_i); it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first half source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second half source vector; it must be 32-byte aligned.
 * @param [in] count Number of half element pairs to process, counted in source elements.
 */
__aicore__ inline void asc_sub_relu(__ubuf__ int8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i - src1_i) element-wise over a configurable repeat and DataBlock stride pattern, writing
 * int8_t results after the f162s8 conversion of the final result.
 *
 * @override_id asc_sub_relu__PUi8_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the int8_t destination vector that receives relu(src0_i -
 * src1_i); it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first half source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second half source vector; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride of src0 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride of src1 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_repeat_stride Address stride of src0 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_repeat_stride Address stride of src1 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_sub_relu(
    __ubuf__ int8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i - src1_i) element-wise for count elements, writing half results after the f322f16
 * conversion of the final result.
 *
 * @override_id asc_sub_relu__PUf16_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the half destination vector that receives relu(src0_i - src1_i);
 * it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first float source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second float source vector; it must be 32-byte aligned.
 * @param [in] count Number of float element pairs to process, counted in source elements.
 */
__aicore__ inline void asc_sub_relu(__ubuf__ half* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i - src1_i) element-wise over a configurable repeat and DataBlock stride pattern, writing
 * half results after the f322f16 conversion of the final result.
 *
 * @override_id asc_sub_relu__PUf16_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the half destination vector that receives relu(src0_i - src1_i);
 * it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first float source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second float source vector; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride of src0 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride of src1 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_repeat_stride Address stride of src0 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_repeat_stride Address stride of src1 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_sub_relu(
    __ubuf__ half* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i - src1_i) element-wise for count elements, writing int8_t results after the s162s8
 * conversion of the final result.
 *
 * @override_id asc_sub_relu__PUi8_PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the int8_t destination vector that receives relu(src0_i -
 * src1_i); it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first int16_t source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second int16_t source vector; it must be 32-byte aligned.
 * @param [in] count Number of int16_t element pairs to process, counted in source elements.
 */
__aicore__ inline void asc_sub_relu(
    __ubuf__ int8_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i - src1_i) element-wise over a configurable repeat and DataBlock stride pattern, writing
 * int8_t results after the s162s8 conversion of the final result.
 *
 * @override_id asc_sub_relu__PUi8_PUi16_PUi16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the int8_t destination vector that receives relu(src0_i -
 * src1_i); it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first int16_t source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second int16_t source vector; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride of src0 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride of src1 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_repeat_stride Address stride of src0 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_repeat_stride Address stride of src1 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_sub_relu(
    __ubuf__ int8_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i + src1_i) element-wise over a configurable repeat and DataBlock stride pattern, writing
 * half results.
 *
 * @override_id asc_add_relu__PUf16_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the half destination vector that receives relu(src0_i + src1_i);
 * it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first half source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second half source vector; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride of src0 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride of src1 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_repeat_stride Address stride of src0 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_repeat_stride Address stride of src1 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_add_relu(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i + src1_i) element-wise for count elements, writing half results.
 *
 * @override_id asc_add_relu__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the half destination vector that receives relu(src0_i + src1_i);
 * it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first half source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second half source vector; it must be 32-byte aligned.
 * @param [in] count Number of half element pairs to process, counted in source elements.
 */
__aicore__ inline void asc_add_relu(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i + src1_i) element-wise over a configurable repeat and DataBlock stride pattern, writing
 * float results.
 *
 * @override_id asc_add_relu__PUf32_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the float destination vector that receives relu(src0_i + src1_i);
 * it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first float source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second float source vector; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride of src0 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride of src1 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_repeat_stride Address stride of src0 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_repeat_stride Address stride of src1 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_add_relu(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i + src1_i) element-wise for count elements, writing float results.
 *
 * @override_id asc_add_relu__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the float destination vector that receives relu(src0_i + src1_i);
 * it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first float source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second float source vector; it must be 32-byte aligned.
 * @param [in] count Number of float element pairs to process, counted in source elements.
 */
__aicore__ inline void asc_add_relu(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i + src1_i) element-wise over a configurable repeat and DataBlock stride pattern, writing
 * int16_t results.
 *
 * @override_id asc_add_relu__PUi16_PUi16_PUi16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the int16_t destination vector that receives relu(src0_i +
 * src1_i); it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first int16_t source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second int16_t source vector; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride of src0 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride of src1 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_repeat_stride Address stride of src0 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_repeat_stride Address stride of src1 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_add_relu(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i + src1_i) element-wise for count elements, writing int16_t results.
 *
 * @override_id asc_add_relu__PUi16_PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the int16_t destination vector that receives relu(src0_i +
 * src1_i); it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first int16_t source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second int16_t source vector; it must be 32-byte aligned.
 * @param [in] count Number of int16_t element pairs to process, counted in source elements.
 */
__aicore__ inline void asc_add_relu(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Vector-dequantizes the first count int16_t elements of src into int8_t values with the per-group scale and
 * offset loaded into DEQSCALE and stores them in the high half block of each DataBlock of dst.
 *
 * @override_id asc_vdeq_int162b8_h__PUi8_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int8_t elements that receives the
 * quantized values placed in the high half block of each DataBlock.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_vdeq_int162b8_h(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Vector-dequantizes int16_t elements of src into int8_t values with the per-group scale and offset loaded into
 * DEQSCALE over repeat vector iterations, storing them in the high half block of each DataBlock of dst with
 * configurable DataBlock and repeat strides.
 *
 * @override_id asc_vdeq_int162b8_h__PUi8_PUi16_u8_u16_u16_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int8_t elements that receives the
 * quantized values placed in the high half block of each DataBlock.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration for the source, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src_repeat_stride Address stride between the same DataBlock of consecutive iterations for the source,
 * counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_vdeq_int162b8_h(
    __ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint8_t dst_repeat_stride, uint8_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Vector-dequantizes the first count int16_t elements of src into uint8_t values with the per-group scale and
 * offset loaded into DEQSCALE and stores them in the high half block of each DataBlock of dst.
 *
 * @override_id asc_vdeq_int162b8_h__PUu8_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of uint8_t elements that receives the
 * quantized values placed in the high half block of each DataBlock.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_vdeq_int162b8_h(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Vector-dequantizes int16_t elements of src into uint8_t values with the per-group scale and offset loaded into
 * DEQSCALE over repeat vector iterations, storing them in the high half block of each DataBlock of dst with
 * configurable DataBlock and repeat strides.
 *
 * @override_id asc_vdeq_int162b8_h__PUu8_PUi16_u8_u16_u16_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of uint8_t elements that receives the
 * quantized values placed in the high half block of each DataBlock.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration for the source, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src_repeat_stride Address stride between the same DataBlock of consecutive iterations for the source,
 * counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_vdeq_int162b8_h(
    __ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint8_t dst_repeat_stride, uint8_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Vector-dequantizes int16_t elements of src into int8_t values with the per-group scale and offset loaded into
 * DEQSCALE over repeat vector iterations, storing them in the low half block of each DataBlock of dst with configurable
 * DataBlock and repeat strides.
 *
 * @override_id asc_vdeq_int162b8_l__PUi8_PUi16_u8_u16_u16_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int8_t elements that receives the
 * quantized values placed in the low half block of each DataBlock.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration for the source, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src_repeat_stride Address stride between the same DataBlock of consecutive iterations for the source,
 * counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_vdeq_int162b8_l(
    __ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint8_t dst_repeat_stride, uint8_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Vector-dequantizes the first count int16_t elements of src into int8_t values with the per-group scale and
 * offset loaded into DEQSCALE and stores them in the low half block of each DataBlock of dst.
 *
 * @override_id asc_vdeq_int162b8_l__PUi8_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int8_t elements that receives the
 * quantized values placed in the low half block of each DataBlock.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_vdeq_int162b8_l(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Vector-dequantizes int16_t elements of src into uint8_t values with the per-group scale and offset loaded into
 * DEQSCALE over repeat vector iterations, storing them in the low half block of each DataBlock of dst with configurable
 * DataBlock and repeat strides.
 *
 * @override_id asc_vdeq_int162b8_l__PUu8_PUi16_u8_u16_u16_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of uint8_t elements that receives the
 * quantized values placed in the low half block of each DataBlock.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration for the source, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src_repeat_stride Address stride between the same DataBlock of consecutive iterations for the source,
 * counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_vdeq_int162b8_l(
    __ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint8_t dst_repeat_stride, uint8_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Vector-dequantizes the first count int16_t elements of src into uint8_t values with the per-group scale and
 * offset loaded into DEQSCALE and stores them in the low half block of each DataBlock of dst.
 *
 * @override_id asc_vdeq_int162b8_l__PUu8_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of uint8_t elements that receives the
 * quantized values placed in the low half block of each DataBlock.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_vdeq_int162b8_l(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes dst[i] = src[i] * value + dst[i] element-wise for the first count elements.
 *
 * @override_id asc_axpy__PUf16_PUf16_f16_u32
 * @archs 2201
 * @param [in,out] dst Start address of the half (fp16) destination vector in Unified Buffer, read as the addend and
 * overwritten with the fused result.
 * @param [in] src Start address of the half (fp16) source vector in Unified Buffer, read element-wise.
 * @param [in] value Scalar multiplier of type half (fp16) applied to every source element.
 * @param [in] count Number of consecutive half elements processed from the start addresses; the impl passes it to
 * asc_set_mask_count_begin before the vector instruction.
 */
__aicore__ inline void asc_axpy(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes dst[i] = src[i] * value + dst[i] element-wise for repeat iterations with explicit DataBlock strides.
 *
 * @override_id asc_axpy__PUf16_PUf16_f16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [in,out] dst Start address of the half (fp16) destination vector in Unified Buffer, read as the addend and
 * overwritten with the fused result.
 * @param [in] src Start address of the half (fp16) source vector in Unified Buffer, read element-wise.
 * @param [in] value Scalar multiplier of type half (fp16) applied to every source element.
 * @param [in] repeat Number of iterations of the high-dimensional slice performed by the vector instruction.
 * @param [in] dst_block_stride Address stride in DataBlock units between the DataBlocks of the destination accessed
 * within one iteration.
 * @param [in] src_block_stride Address stride in DataBlock units between the DataBlocks of the source accessed within
 * one iteration.
 * @param [in] dst_repeat_stride Address stride in DataBlock units between the same DataBlock of the destination in two
 * adjacent iterations.
 * @param [in] src_repeat_stride Address stride in DataBlock units between the same DataBlock of the source in two
 * adjacent iterations.
 */
__aicore__ inline void asc_axpy(
    __ubuf__ half* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes dst[i] = src[i] * value + dst[i] element-wise for the first count elements.
 *
 * @override_id asc_axpy__PUf32_PUf32_f32_u32
 * @archs 2201
 * @param [in,out] dst Start address of the float (fp32) destination vector in Unified Buffer, read as the addend and
 * overwritten with the fused result.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer, read element-wise.
 * @param [in] value Scalar multiplier of type float (fp32) applied to every source element.
 * @param [in] count Number of consecutive float elements processed from the start addresses; the impl passes it to
 * asc_set_mask_count_begin before the vector instruction.
 */
__aicore__ inline void asc_axpy(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes dst[i] = src[i] * value + dst[i] element-wise for repeat iterations with explicit DataBlock strides.
 *
 * @override_id asc_axpy__PUf32_PUf32_f32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [in,out] dst Start address of the float (fp32) destination vector in Unified Buffer, read as the addend and
 * overwritten with the fused result.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer, read element-wise.
 * @param [in] value Scalar multiplier of type float (fp32) applied to every source element.
 * @param [in] repeat Number of iterations of the high-dimensional slice performed by the vector instruction.
 * @param [in] dst_block_stride Address stride in DataBlock units between the DataBlocks of the destination accessed
 * within one iteration.
 * @param [in] src_block_stride Address stride in DataBlock units between the DataBlocks of the source accessed within
 * one iteration.
 * @param [in] dst_repeat_stride Address stride in DataBlock units between the same DataBlock of the destination in two
 * adjacent iterations.
 * @param [in] src_repeat_stride Address stride in DataBlock units between the same DataBlock of the source in two
 * adjacent iterations.
 */
__aicore__ inline void asc_axpy(
    __ubuf__ float* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes dst[i] = src[i] * value + dst[i] element-wise with half source elements widened into the float
 * destination for the first count elements.
 *
 * @override_id asc_axpy__PUf32_PUf16_f16_u32
 * @archs 2201
 * @param [in,out] dst Start address of the float (fp32) destination vector in Unified Buffer, read as the addend and
 * overwritten with the fused result.
 * @param [in] src Start address of the half (fp16) source vector in Unified Buffer, read element-wise.
 * @param [in] value Scalar multiplier of type half (fp16) applied to every source element.
 * @param [in] count Number of consecutive float elements processed from the start addresses; the impl passes it to
 * asc_set_mask_count_begin before the vector instruction.
 */
__aicore__ inline void asc_axpy(__ubuf__ float* dst, __ubuf__ half* src, half value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes dst[i] = src[i] * value + dst[i] element-wise with half source elements widened into the float
 * destination for repeat iterations with explicit DataBlock strides.
 *
 * @override_id asc_axpy__PUf32_PUf16_f16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [in,out] dst Start address of the float (fp32) destination vector in Unified Buffer, read as the addend and
 * overwritten with the fused result.
 * @param [in] src Start address of the half (fp16) source vector in Unified Buffer, read element-wise.
 * @param [in] value Scalar multiplier of type half (fp16) applied to every source element.
 * @param [in] repeat Number of iterations of the high-dimensional slice performed by the vector instruction.
 * @param [in] dst_block_stride Address stride in DataBlock units between the DataBlocks of the destination accessed
 * within one iteration.
 * @param [in] src_block_stride Address stride in DataBlock units between the DataBlocks of the source accessed within
 * one iteration.
 * @param [in] dst_repeat_stride Address stride in DataBlock units between the same DataBlock of the destination in two
 * adjacent iterations.
 * @param [in] src_repeat_stride Address stride in DataBlock units between the same DataBlock of the source in two
 * adjacent iterations.
 */
__aicore__ inline void asc_axpy(
    __ubuf__ float* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i + src1_i) element-wise for count elements, writing int8_t results after the f162s8
 * conversion of the final result.
 *
 * @override_id asc_add_relu__PUi8_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the int8_t destination vector that receives relu(src0_i +
 * src1_i); it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first half source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second half source vector; it must be 32-byte aligned.
 * @param [in] count Number of half element pairs to process, counted in source elements.
 */
__aicore__ inline void asc_add_relu(__ubuf__ int8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i + src1_i) element-wise over a configurable repeat and DataBlock stride pattern, writing
 * int8_t results after the f162s8 conversion of the final result.
 *
 * @override_id asc_add_relu__PUi8_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the int8_t destination vector that receives relu(src0_i +
 * src1_i); it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first half source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second half source vector; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride of src0 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride of src1 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_repeat_stride Address stride of src0 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_repeat_stride Address stride of src1 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_add_relu(
    __ubuf__ int8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i + src1_i) element-wise for count elements, writing half results after the f322f16
 * conversion of the final result.
 *
 * @override_id asc_add_relu__PUf16_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the half destination vector that receives relu(src0_i + src1_i);
 * it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first float source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second float source vector; it must be 32-byte aligned.
 * @param [in] count Number of float element pairs to process, counted in source elements.
 */
__aicore__ inline void asc_add_relu(__ubuf__ half* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i + src1_i) element-wise over a configurable repeat and DataBlock stride pattern, writing
 * half results after the f322f16 conversion of the final result.
 *
 * @override_id asc_add_relu__PUf16_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the half destination vector that receives relu(src0_i + src1_i);
 * it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first float source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second float source vector; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride of src0 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride of src1 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_repeat_stride Address stride of src0 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_repeat_stride Address stride of src1 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_add_relu(
    __ubuf__ half* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i + src1_i) element-wise for count elements, writing int8_t results after the s162s8
 * conversion of the final result.
 *
 * @override_id asc_add_relu__PUi8_PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the int8_t destination vector that receives relu(src0_i +
 * src1_i); it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first int16_t source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second int16_t source vector; it must be 32-byte aligned.
 * @param [in] count Number of int16_t element pairs to process, counted in source elements.
 */
__aicore__ inline void asc_add_relu(
    __ubuf__ int8_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_fused
 * @brief Computes relu(src0_i + src1_i) element-wise over a configurable repeat and DataBlock stride pattern, writing
 * int8_t results after the s162s8 conversion of the final result.
 *
 * @override_id asc_add_relu__PUi8_PUi16_PUi16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the int8_t destination vector that receives relu(src0_i +
 * src1_i); it must be 32-byte aligned.
 * @param [in] src0 Start address in Unified Buffer of the first int16_t source vector; it must be 32-byte aligned.
 * @param [in] src1 Start address in Unified Buffer of the second int16_t source vector; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride of src0 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride of src1 between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src0_repeat_stride Address stride of src0 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 * @param [in] src1_repeat_stride Address stride of src1 between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_add_relu(
    __ubuf__ int8_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_FUSED_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_FUSED_H
#endif
