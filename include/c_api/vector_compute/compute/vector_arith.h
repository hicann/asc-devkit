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
#error "vector_compute/compute/vector_arith.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_ARITH_H
#endif

#ifndef INCLUDE_C_API_VECTOR_COMPUTE_COMPUTE_VECTOR_ARITH_H
#define INCLUDE_C_API_VECTOR_COMPUTE_COMPUTE_VECTOR_ARITH_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/memory_base_impl/vector_arith_intf_impl.h"
#endif
/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Adds the first count elements of src0 and src1 element-wise and writes the half sums to dst.
 *
 * @override_id asc_add__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer (__ubuf__ half*) holding dst_i = src0_i + src1_i for the
 * first count elements.
 * @param [in] src0 Start address of the first half source buffer in UB; read only.
 * @param [in] src1 Start address of the second half source buffer in UB; read only.
 * @param [in] count Number of elements taking part in the calculation (uint32_t); the impl passes it to
 * asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_add(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Adds src0 and src1 element-wise over repeat iterations using per-block and per-repeat address strides and
 * writes the half sums to dst.
 *
 * @override_id asc_add__PUf16_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer (__ubuf__ half*) holding dst_i = src0_i + src1_i for
 * every element covered by repeat and the strides.
 * @param [in] src0 Start address of the first half source buffer in UB; read only.
 * @param [in] src1 Start address of the second half source buffer in UB; read only.
 * @param [in] repeat Number of iterations (uint8_t).
 * @param [in] dst_block_stride Address stride between DataBlocks of dst within one iteration (uint8_t).
 * @param [in] src0_block_stride Address stride between DataBlocks of src0 within one iteration (uint8_t).
 * @param [in] src1_block_stride Address stride between DataBlocks of src1 within one iteration (uint8_t).
 * @param [in] dst_repeat_stride Address stride between corresponding DataBlocks of consecutive dst iterations
 * (uint8_t).
 * @param [in] src0_repeat_stride Address stride between corresponding DataBlocks of consecutive src0 iterations
 * (uint8_t).
 * @param [in] src1_repeat_stride Address stride between corresponding DataBlocks of consecutive src1 iterations
 * (uint8_t).
 */
__aicore__ inline void asc_add(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Adds the first count elements of src0 and src1 element-wise and writes the float sums to dst.
 *
 * @override_id asc_add__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer (__ubuf__ float*) holding dst_i = src0_i + src1_i for the
 * first count elements.
 * @param [in] src0 Start address of the first float source buffer in UB; read only.
 * @param [in] src1 Start address of the second float source buffer in UB; read only.
 * @param [in] count Number of elements taking part in the calculation (uint32_t); the impl passes it to
 * asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_add(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Adds src0 and src1 element-wise over repeat iterations using per-block and per-repeat address strides and
 * writes the float sums to dst.
 *
 * @override_id asc_add__PUf32_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer (__ubuf__ float*) holding dst_i = src0_i + src1_i for
 * every element covered by repeat and the strides.
 * @param [in] src0 Start address of the first float source buffer in UB; read only.
 * @param [in] src1 Start address of the second float source buffer in UB; read only.
 * @param [in] repeat Number of iterations (uint8_t).
 * @param [in] dst_block_stride Address stride between DataBlocks of dst within one iteration (uint8_t).
 * @param [in] src0_block_stride Address stride between DataBlocks of src0 within one iteration (uint8_t).
 * @param [in] src1_block_stride Address stride between DataBlocks of src1 within one iteration (uint8_t).
 * @param [in] dst_repeat_stride Address stride between corresponding DataBlocks of consecutive dst iterations
 * (uint8_t).
 * @param [in] src0_repeat_stride Address stride between corresponding DataBlocks of consecutive src0 iterations
 * (uint8_t).
 * @param [in] src1_repeat_stride Address stride between corresponding DataBlocks of consecutive src1 iterations
 * (uint8_t).
 */
__aicore__ inline void asc_add(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Adds the first count elements of src0 and src1 element-wise and writes the int16_t sums to dst.
 *
 * @override_id asc_add__PUi16_PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer (__ubuf__ int16_t*) holding dst_i = src0_i + src1_i for
 * the first count elements.
 * @param [in] src0 Start address of the first int16_t source buffer in UB; read only.
 * @param [in] src1 Start address of the second int16_t source buffer in UB; read only.
 * @param [in] count Number of elements taking part in the calculation (uint32_t); the impl passes it to
 * asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_add(__ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Adds src0 and src1 element-wise over repeat iterations using per-block and per-repeat address strides and
 * writes the int16_t sums to dst.
 *
 * @override_id asc_add__PUi16_PUi16_PUi16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer (__ubuf__ int16_t*) holding dst_i = src0_i + src1_i for
 * every element covered by repeat and the strides.
 * @param [in] src0 Start address of the first int16_t source buffer in UB; read only.
 * @param [in] src1 Start address of the second int16_t source buffer in UB; read only.
 * @param [in] repeat Number of iterations (uint8_t).
 * @param [in] dst_block_stride Address stride between DataBlocks of dst within one iteration (uint8_t).
 * @param [in] src0_block_stride Address stride between DataBlocks of src0 within one iteration (uint8_t).
 * @param [in] src1_block_stride Address stride between DataBlocks of src1 within one iteration (uint8_t).
 * @param [in] dst_repeat_stride Address stride between corresponding DataBlocks of consecutive dst iterations
 * (uint8_t).
 * @param [in] src0_repeat_stride Address stride between corresponding DataBlocks of consecutive src0 iterations
 * (uint8_t).
 * @param [in] src1_repeat_stride Address stride between corresponding DataBlocks of consecutive src1 iterations
 * (uint8_t).
 */
__aicore__ inline void asc_add(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Adds the first count elements of src0 and src1 element-wise and writes the int32_t sums to dst.
 *
 * @override_id asc_add__PUi32_PUi32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer (__ubuf__ int32_t*) holding dst_i = src0_i + src1_i for
 * the first count elements.
 * @param [in] src0 Start address of the first int32_t source buffer in UB; read only.
 * @param [in] src1 Start address of the second int32_t source buffer in UB; read only.
 * @param [in] count Number of elements taking part in the calculation (uint32_t); the impl passes it to
 * asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_add(__ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Adds src0 and src1 element-wise over repeat iterations using per-block and per-repeat address strides and
 * writes the int32_t sums to dst.
 *
 * @override_id asc_add__PUi32_PUi32_PUi32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer (__ubuf__ int32_t*) holding dst_i = src0_i + src1_i for
 * every element covered by repeat and the strides.
 * @param [in] src0 Start address of the first int32_t source buffer in UB; read only.
 * @param [in] src1 Start address of the second int32_t source buffer in UB; read only.
 * @param [in] repeat Number of iterations (uint8_t).
 * @param [in] dst_block_stride Address stride between DataBlocks of dst within one iteration (uint8_t).
 * @param [in] src0_block_stride Address stride between DataBlocks of src0 within one iteration (uint8_t).
 * @param [in] src1_block_stride Address stride between DataBlocks of src1 within one iteration (uint8_t).
 * @param [in] dst_repeat_stride Address stride between corresponding DataBlocks of consecutive dst iterations
 * (uint8_t).
 * @param [in] src0_repeat_stride Address stride between corresponding DataBlocks of consecutive src0 iterations
 * (uint8_t).
 * @param [in] src1_repeat_stride Address stride between corresponding DataBlocks of consecutive src1 iterations
 * (uint8_t).
 */
__aicore__ inline void asc_add(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Adds the scalar value to each of the first count elements of src and writes the half sums to dst.
 *
 * @override_id asc_add_scalar__PUf16_PUf16_f16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer (__ubuf__ half*) holding dst_i = src_i + value for the
 * first count elements.
 * @param [in] src Start address of the half source buffer in UB; read only.
 * @param [in] value Scalar half operand added to every processed element.
 * @param [in] count Number of elements taking part in the continuous calculation (uint32_t); the impl passes it to
 * asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_add_scalar(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Adds the scalar value to src element-wise over repeat iterations with explicit block and repeat strides and
 * writes the half sums to dst.
 *
 * @override_id asc_add_scalar__PUf16_PUf16_f16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer (__ubuf__ half*) holding dst_i = src_i + value for every
 * element addressed by repeat and the strides.
 * @param [in] src Start address of the half source buffer in UB; read only.
 * @param [in] value Scalar half operand added to every processed element.
 * @param [in] repeat Number of iterations (uint8_t).
 * @param [in] dst_block_stride Address stride between DataBlocks of dst within one iteration (uint16_t).
 * @param [in] src_block_stride Address stride between DataBlocks of src within one iteration (uint16_t).
 * @param [in] dst_repeat_stride Address stride between corresponding DataBlocks of consecutive dst iterations
 * (uint16_t).
 * @param [in] src_repeat_stride Address stride between corresponding DataBlocks of consecutive src iterations
 * (uint16_t).
 */
__aicore__ inline void asc_add_scalar(
    __ubuf__ half* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Adds the scalar value to each of the first count elements of src and writes the float sums to dst.
 *
 * @override_id asc_add_scalar__PUf32_PUf32_f32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer (__ubuf__ float*) holding dst_i = src_i + value for the
 * first count elements.
 * @param [in] src Start address of the float source buffer in UB; read only.
 * @param [in] value Scalar float operand added to every processed element.
 * @param [in] count Number of elements taking part in the continuous calculation (uint32_t); the impl passes it to
 * asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_add_scalar(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Adds the scalar value to src element-wise over repeat iterations with explicit block and repeat strides and
 * writes the float sums to dst.
 *
 * @override_id asc_add_scalar__PUf32_PUf32_f32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer (__ubuf__ float*) holding dst_i = src_i + value for every
 * element addressed by repeat and the strides.
 * @param [in] src Start address of the float source buffer in UB; read only.
 * @param [in] value Scalar float operand added to every processed element.
 * @param [in] repeat Number of iterations (uint8_t).
 * @param [in] dst_block_stride Address stride between DataBlocks of dst within one iteration (uint16_t).
 * @param [in] src_block_stride Address stride between DataBlocks of src within one iteration (uint16_t).
 * @param [in] dst_repeat_stride Address stride between corresponding DataBlocks of consecutive dst iterations
 * (uint16_t).
 * @param [in] src_repeat_stride Address stride between corresponding DataBlocks of consecutive src iterations
 * (uint16_t).
 */
__aicore__ inline void asc_add_scalar(
    __ubuf__ float* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Adds the scalar value to each of the first count elements of src and writes the int16_t sums to dst.
 *
 * @override_id asc_add_scalar__PUi16_PUi16_i16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer (__ubuf__ int16_t*) holding dst_i = src_i + value for the
 * first count elements.
 * @param [in] src Start address of the int16_t source buffer in UB; read only.
 * @param [in] value Scalar int16_t operand added to every processed element.
 * @param [in] count Number of elements taking part in the continuous calculation (uint32_t); the impl passes it to
 * asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_add_scalar(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Adds the scalar value to src element-wise over repeat iterations with explicit block and repeat strides and
 * writes the int16_t sums to dst.
 *
 * @override_id asc_add_scalar__PUi16_PUi16_i16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer (__ubuf__ int16_t*) holding dst_i = src_i + value for
 * every element addressed by repeat and the strides.
 * @param [in] src Start address of the int16_t source buffer in UB; read only.
 * @param [in] value Scalar int16_t operand added to every processed element.
 * @param [in] repeat Number of iterations (uint8_t).
 * @param [in] dst_block_stride Address stride between DataBlocks of dst within one iteration (uint16_t).
 * @param [in] src_block_stride Address stride between DataBlocks of src within one iteration (uint16_t).
 * @param [in] dst_repeat_stride Address stride between corresponding DataBlocks of consecutive dst iterations
 * (uint16_t).
 * @param [in] src_repeat_stride Address stride between corresponding DataBlocks of consecutive src iterations
 * (uint16_t).
 */
__aicore__ inline void asc_add_scalar(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Adds the scalar value to each of the first count elements of src and writes the int32_t sums to dst.
 *
 * @override_id asc_add_scalar__PUi32_PUi32_i32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer (__ubuf__ int32_t*) holding dst_i = src_i + value for the
 * first count elements.
 * @param [in] src Start address of the int32_t source buffer in UB; read only.
 * @param [in] value Scalar int32_t operand added to every processed element.
 * @param [in] count Number of elements taking part in the continuous calculation (uint32_t); the impl passes it to
 * asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_add_scalar(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Adds the scalar value to src element-wise over repeat iterations with explicit block and repeat strides and
 * writes the int32_t sums to dst.
 *
 * @override_id asc_add_scalar__PUi32_PUi32_i32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer (__ubuf__ int32_t*) holding dst_i = src_i + value for
 * every element addressed by repeat and the strides.
 * @param [in] src Start address of the int32_t source buffer in UB; read only.
 * @param [in] value Scalar int32_t operand added to every processed element.
 * @param [in] repeat Number of iterations (uint8_t).
 * @param [in] dst_block_stride Address stride between DataBlocks of dst within one iteration (uint16_t).
 * @param [in] src_block_stride Address stride between DataBlocks of src within one iteration (uint16_t).
 * @param [in] dst_repeat_stride Address stride between corresponding DataBlocks of consecutive dst iterations
 * (uint16_t).
 * @param [in] src_repeat_stride Address stride between corresponding DataBlocks of consecutive src iterations
 * (uint16_t).
 */
__aicore__ inline void asc_add_scalar(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise quotient of the first count half elements addressed by src0 and src1 and writes the
 * results into dst.
 *
 * @override_id asc_div__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the half destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src0 Start address in Unified Buffer of the first half source vector (the dividend of the element-wise
 * division); read only.
 * @param [in] src1 Start address in Unified Buffer of the second half source vector (the divisor of the element-wise
 * division); read only.
 * @param [in] count Number of leading half elements to process, counted in elements rather than bytes; the impl turns
 * it into a mask count for the call.
 */
__aicore__ inline void asc_div(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise quotient of the half elements addressed by src0 and src1 using the repeat and
 * DataBlock stride pattern and writes the per-iteration results into dst.
 *
 * @override_id asc_div__PUf16_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the half destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src0 Start address in Unified Buffer of the first half source vector (the dividend of the element-wise
 * division); read only.
 * @param [in] src1 Start address in Unified Buffer of the second half source vector (the divisor of the element-wise
 * division); read only.
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
__aicore__ inline void asc_div(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise quotient of the first count float elements addressed by src0 and src1 and writes the
 * results into dst.
 *
 * @override_id asc_div__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the float destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src0 Start address in Unified Buffer of the first float source vector (the dividend of the element-wise
 * division); read only.
 * @param [in] src1 Start address in Unified Buffer of the second float source vector (the divisor of the element-wise
 * division); read only.
 * @param [in] count Number of leading float elements to process, counted in elements rather than bytes; the impl turns
 * it into a mask count for the call.
 */
__aicore__ inline void asc_div(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise quotient of the float elements addressed by src0 and src1 using the repeat and
 * DataBlock stride pattern and writes the per-iteration results into dst.
 *
 * @override_id asc_div__PUf32_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the float destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src0 Start address in Unified Buffer of the first float source vector (the dividend of the element-wise
 * division); read only.
 * @param [in] src1 Start address in Unified Buffer of the second float source vector (the divisor of the element-wise
 * division); read only.
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
__aicore__ inline void asc_div(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the natural exponential of the half elements addressed by src using the repeat and DataBlock stride
 * pattern and writes the per-iteration results into dst.
 *
 * @override_id asc_exp__PUf16_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the half destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the half source vector; read only.
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
__aicore__ inline void asc_exp(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the natural exponential of the first count half elements addressed by src and writes the results into
 * dst.
 *
 * @override_id asc_exp__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the half destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the half source vector; read only.
 * @param [in] count Number of leading half elements to process, counted in elements rather than bytes; the impl turns
 * it into a mask count for the call.
 */
__aicore__ inline void asc_exp(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the natural exponential of the float elements addressed by src using the repeat and DataBlock stride
 * pattern and writes the per-iteration results into dst.
 *
 * @override_id asc_exp__PUf32_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the float destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the float source vector; read only.
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
__aicore__ inline void asc_exp(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the natural exponential of the first count float elements addressed by src and writes the results
 * into dst.
 *
 * @override_id asc_exp__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the float destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the float source vector; read only.
 * @param [in] count Number of leading float elements to process, counted in elements rather than bytes; the impl turns
 * it into a mask count for the call.
 */
__aicore__ inline void asc_exp(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the natural logarithm of the half elements addressed by src using the repeat and DataBlock stride
 * pattern and writes the per-iteration results into dst.
 *
 * @override_id asc_log__PUf16_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the half destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the half source vector; read only.
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
__aicore__ inline void asc_log(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the natural logarithm of the first count half elements addressed by src and writes the results into
 * dst.
 *
 * @override_id asc_log__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the half destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the half source vector; read only.
 * @param [in] count Number of leading half elements to process, counted in elements rather than bytes; the impl turns
 * it into a mask count for the call.
 */
__aicore__ inline void asc_log(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the natural logarithm of the float elements addressed by src using the repeat and DataBlock stride
 * pattern and writes the per-iteration results into dst.
 *
 * @override_id asc_log__PUf32_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the float destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the float source vector; read only.
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
__aicore__ inline void asc_log(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the natural logarithm of the first count float elements addressed by src and writes the results into
 * dst.
 *
 * @override_id asc_log__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the float destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the float source vector; read only.
 * @param [in] count Number of leading float elements to process, counted in elements rather than bytes; the impl turns
 * it into a mask count for the call.
 */
__aicore__ inline void asc_log(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the absolute value of the half elements addressed by src using the repeat and DataBlock stride
 * pattern and writes the per-iteration results into dst.
 *
 * @override_id asc_abs__PUf16_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the half destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the half source vector; read only.
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
__aicore__ inline void asc_abs(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the absolute value of the first count half elements addressed by src and writes the results into dst.
 *
 * @override_id asc_abs__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the half destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the half source vector; read only.
 * @param [in] count Number of leading half elements to process, counted in elements rather than bytes; the impl turns
 * it into a mask count for the call.
 */
__aicore__ inline void asc_abs(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the absolute value of the float elements addressed by src using the repeat and DataBlock stride
 * pattern and writes the per-iteration results into dst.
 *
 * @override_id asc_abs__PUf32_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the float destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the float source vector; read only.
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
__aicore__ inline void asc_abs(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the absolute value of the first count float elements addressed by src and writes the results into
 * dst.
 *
 * @override_id asc_abs__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the float destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the float source vector; read only.
 * @param [in] count Number of leading float elements to process, counted in elements rather than bytes; the impl turns
 * it into a mask count for the call.
 */
__aicore__ inline void asc_abs(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise maximum of src0 and src1 for the first count half elements and writes the result to
 * dst.
 *
 * @override_id asc_max__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of half elements that receives the
 * element-wise maximum.
 * @param [in] src0 Start address of the first source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_max(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise maximum of src0 and src1 over repeat vector iterations of half elements, using
 * per-operand DataBlock and repeat strides, and writes the result to dst.
 *
 * @override_id asc_max__PUf16_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of half elements that receives the
 * element-wise maximum.
 * @param [in] src0 Start address of the first source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride between DataBlocks inside one iteration for source operand 0, counted in
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride between DataBlocks inside one iteration for source operand 1, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 0, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 1, counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_max(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise maximum of src0 and src1 for the first count float elements and writes the result to
 * dst.
 *
 * @override_id asc_max__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of float elements that receives the
 * element-wise maximum.
 * @param [in] src0 Start address of the first source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_max(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise maximum of src0 and src1 over repeat vector iterations of float elements, using
 * per-operand DataBlock and repeat strides, and writes the result to dst.
 *
 * @override_id asc_max__PUf32_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of float elements that receives the
 * element-wise maximum.
 * @param [in] src0 Start address of the first source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride between DataBlocks inside one iteration for source operand 0, counted in
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride between DataBlocks inside one iteration for source operand 1, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 0, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 1, counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_max(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise maximum of src0 and src1 for the first count int16_t elements and writes the result
 * to dst.
 *
 * @override_id asc_max__PUi16_PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int16_t elements that receives the
 * element-wise maximum.
 * @param [in] src0 Start address of the first source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_max(__ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise maximum of src0 and src1 over repeat vector iterations of int16_t elements, using
 * per-operand DataBlock and repeat strides, and writes the result to dst.
 *
 * @override_id asc_max__PUi16_PUi16_PUi16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int16_t elements that receives the
 * element-wise maximum.
 * @param [in] src0 Start address of the first source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride between DataBlocks inside one iteration for source operand 0, counted in
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride between DataBlocks inside one iteration for source operand 1, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 0, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 1, counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_max(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise maximum of src0 and src1 for the first count int32_t elements and writes the result
 * to dst.
 *
 * @override_id asc_max__PUi32_PUi32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int32_t elements that receives the
 * element-wise maximum.
 * @param [in] src0 Start address of the first source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_max(__ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise maximum of src0 and src1 over repeat vector iterations of int32_t elements, using
 * per-operand DataBlock and repeat strides, and writes the result to dst.
 *
 * @override_id asc_max__PUi32_PUi32_PUi32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int32_t elements that receives the
 * element-wise maximum.
 * @param [in] src0 Start address of the first source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride between DataBlocks inside one iteration for source operand 0, counted in
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride between DataBlocks inside one iteration for source operand 1, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 0, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 1, counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_max(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Compares each of the first count half elements of src with the scalar value and writes the larger value to
 * dst.
 *
 * @override_id asc_max_scalar__PUf16_PUf16_f16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of half elements that receives the
 * element-wise maximum against the scalar.
 * @param [in] src Start address of the source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_max_scalar(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Compares half elements of src with the scalar value over repeat vector iterations using per-operand DataBlock
 * and repeat strides, writing the larger value to dst.
 *
 * @override_id asc_max_scalar__PUf16_PUf16_f16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of half elements that receives the
 * element-wise maximum against the scalar.
 * @param [in] src Start address of the source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
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
__aicore__ inline void asc_max_scalar(
    __ubuf__ half* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Compares each of the first count float elements of src with the scalar value and writes the larger value to
 * dst.
 *
 * @override_id asc_max_scalar__PUf32_PUf32_f32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of float elements that receives the
 * element-wise maximum against the scalar.
 * @param [in] src Start address of the source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_max_scalar(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Compares float elements of src with the scalar value over repeat vector iterations using per-operand DataBlock
 * and repeat strides, writing the larger value to dst.
 *
 * @override_id asc_max_scalar__PUf32_PUf32_f32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of float elements that receives the
 * element-wise maximum against the scalar.
 * @param [in] src Start address of the source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
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
__aicore__ inline void asc_max_scalar(
    __ubuf__ float* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Compares each of the first count int16_t elements of src with the scalar value and writes the larger value to
 * dst.
 *
 * @override_id asc_max_scalar__PUi16_PUi16_i16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int16_t elements that receives the
 * element-wise maximum against the scalar.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_max_scalar(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Compares int16_t elements of src with the scalar value over repeat vector iterations using per-operand
 * DataBlock and repeat strides, writing the larger value to dst.
 *
 * @override_id asc_max_scalar__PUi16_PUi16_i16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int16_t elements that receives the
 * element-wise maximum against the scalar.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
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
__aicore__ inline void asc_max_scalar(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Compares each of the first count int32_t elements of src with the scalar value and writes the larger value to
 * dst.
 *
 * @override_id asc_max_scalar__PUi32_PUi32_i32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int32_t elements that receives the
 * element-wise maximum against the scalar.
 * @param [in] src Start address of the source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_max_scalar(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Compares int32_t elements of src with the scalar value over repeat vector iterations using per-operand
 * DataBlock and repeat strides, writing the larger value to dst.
 *
 * @override_id asc_max_scalar__PUi32_PUi32_i32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int32_t elements that receives the
 * element-wise maximum against the scalar.
 * @param [in] src Start address of the source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
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
__aicore__ inline void asc_max_scalar(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise minimum of src0 and src1 for the first count half elements and writes the result to
 * dst.
 *
 * @override_id asc_min__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of half elements that receives the
 * element-wise minimum.
 * @param [in] src0 Start address of the first source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_min(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise minimum of src0 and src1 over repeat vector iterations of half elements, using
 * per-operand DataBlock and repeat strides, and writes the result to dst.
 *
 * @override_id asc_min__PUf16_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of half elements that receives the
 * element-wise minimum.
 * @param [in] src0 Start address of the first source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride between DataBlocks inside one iteration for source operand 0, counted in
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride between DataBlocks inside one iteration for source operand 1, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 0, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 1, counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_min(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise minimum of src0 and src1 for the first count float elements and writes the result to
 * dst.
 *
 * @override_id asc_min__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of float elements that receives the
 * element-wise minimum.
 * @param [in] src0 Start address of the first source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_min(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise minimum of src0 and src1 over repeat vector iterations of float elements, using
 * per-operand DataBlock and repeat strides, and writes the result to dst.
 *
 * @override_id asc_min__PUf32_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of float elements that receives the
 * element-wise minimum.
 * @param [in] src0 Start address of the first source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride between DataBlocks inside one iteration for source operand 0, counted in
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride between DataBlocks inside one iteration for source operand 1, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 0, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 1, counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_min(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise minimum of src0 and src1 for the first count int16_t elements and writes the result
 * to dst.
 *
 * @override_id asc_min__PUi16_PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int16_t elements that receives the
 * element-wise minimum.
 * @param [in] src0 Start address of the first source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_min(__ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise minimum of src0 and src1 over repeat vector iterations of int16_t elements, using
 * per-operand DataBlock and repeat strides, and writes the result to dst.
 *
 * @override_id asc_min__PUi16_PUi16_PUi16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int16_t elements that receives the
 * element-wise minimum.
 * @param [in] src0 Start address of the first source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride between DataBlocks inside one iteration for source operand 0, counted in
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride between DataBlocks inside one iteration for source operand 1, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 0, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 1, counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_min(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise minimum of src0 and src1 for the first count int32_t elements and writes the result
 * to dst.
 *
 * @override_id asc_min__PUi32_PUi32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int32_t elements that receives the
 * element-wise minimum.
 * @param [in] src0 Start address of the first source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_min(__ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise minimum of src0 and src1 over repeat vector iterations of int32_t elements, using
 * per-operand DataBlock and repeat strides, and writes the result to dst.
 *
 * @override_id asc_min__PUi32_PUi32_PUi32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int32_t elements that receives the
 * element-wise minimum.
 * @param [in] src0 Start address of the first source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride between DataBlocks inside one iteration for source operand 0, counted in
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride between DataBlocks inside one iteration for source operand 1, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 0, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 1, counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_min(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Multiplies src0 and src1 element-wise for the first count half elements and writes the products to dst.
 *
 * @override_id asc_mul__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of half elements that receives the
 * element-wise products.
 * @param [in] src0 Start address of the first source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_mul(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Multiplies src0 and src1 element-wise over repeat vector iterations of half elements, using per-operand
 * DataBlock and repeat strides, and writes the products to dst.
 *
 * @override_id asc_mul__PUf16_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of half elements that receives the
 * element-wise products.
 * @param [in] src0 Start address of the first source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride between DataBlocks inside one iteration for source operand 0, counted in
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride between DataBlocks inside one iteration for source operand 1, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 0, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 1, counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_mul(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Multiplies src0 and src1 element-wise for the first count float elements and writes the products to dst.
 *
 * @override_id asc_mul__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of float elements that receives the
 * element-wise products.
 * @param [in] src0 Start address of the first source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_mul(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Multiplies src0 and src1 element-wise over repeat vector iterations of float elements, using per-operand
 * DataBlock and repeat strides, and writes the products to dst.
 *
 * @override_id asc_mul__PUf32_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of float elements that receives the
 * element-wise products.
 * @param [in] src0 Start address of the first source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride between DataBlocks inside one iteration for source operand 0, counted in
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride between DataBlocks inside one iteration for source operand 1, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 0, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 1, counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_mul(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Multiplies src0 and src1 element-wise for the first count int16_t elements and writes the products to dst.
 *
 * @override_id asc_mul__PUi16_PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int16_t elements that receives the
 * element-wise products.
 * @param [in] src0 Start address of the first source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_mul(__ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Multiplies src0 and src1 element-wise over repeat vector iterations of int16_t elements, using per-operand
 * DataBlock and repeat strides, and writes the products to dst.
 *
 * @override_id asc_mul__PUi16_PUi16_PUi16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int16_t elements that receives the
 * element-wise products.
 * @param [in] src0 Start address of the first source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride between DataBlocks inside one iteration for source operand 0, counted in
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride between DataBlocks inside one iteration for source operand 1, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 0, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 1, counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_mul(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Multiplies src0 and src1 element-wise for the first count int32_t elements and writes the products to dst.
 *
 * @override_id asc_mul__PUi32_PUi32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int32_t elements that receives the
 * element-wise products.
 * @param [in] src0 Start address of the first source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_mul(__ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Multiplies src0 and src1 element-wise over repeat vector iterations of int32_t elements, using per-operand
 * DataBlock and repeat strides, and writes the products to dst.
 *
 * @override_id asc_mul__PUi32_PUi32_PUi32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int32_t elements that receives the
 * element-wise products.
 * @param [in] src0 Start address of the first source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride between DataBlocks inside one iteration for source operand 0, counted in
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride between DataBlocks inside one iteration for source operand 1, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 0, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 1, counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_mul(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Multiplies each of the first count half elements of src by the scalar value and writes the products to dst.
 *
 * @override_id asc_mul_scalar__PUf16_PUf16_f16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of half elements that receives the
 * element-wise product with the scalar.
 * @param [in] src Start address of the source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_mul_scalar(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Multiplies half elements of src by the scalar value over repeat vector iterations using per-operand DataBlock
 * and repeat strides, writing the products to dst.
 *
 * @override_id asc_mul_scalar__PUf16_PUf16_f16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of half elements that receives the
 * element-wise product with the scalar.
 * @param [in] src Start address of the source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
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
__aicore__ inline void asc_mul_scalar(
    __ubuf__ half* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Multiplies each of the first count float elements of src by the scalar value and writes the products to dst.
 *
 * @override_id asc_mul_scalar__PUf32_PUf32_f32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of float elements that receives the
 * element-wise product with the scalar.
 * @param [in] src Start address of the source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_mul_scalar(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Multiplies float elements of src by the scalar value over repeat vector iterations using per-operand DataBlock
 * and repeat strides, writing the products to dst.
 *
 * @override_id asc_mul_scalar__PUf32_PUf32_f32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of float elements that receives the
 * element-wise product with the scalar.
 * @param [in] src Start address of the source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
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
__aicore__ inline void asc_mul_scalar(
    __ubuf__ float* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Multiplies each of the first count int16_t elements of src by the scalar value and writes the products to dst.
 *
 * @override_id asc_mul_scalar__PUi16_PUi16_i16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int16_t elements that receives the
 * element-wise product with the scalar.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_mul_scalar(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Multiplies int16_t elements of src by the scalar value over repeat vector iterations using per-operand
 * DataBlock and repeat strides, writing the products to dst.
 *
 * @override_id asc_mul_scalar__PUi16_PUi16_i16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int16_t elements that receives the
 * element-wise product with the scalar.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
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
__aicore__ inline void asc_mul_scalar(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Multiplies each of the first count int32_t elements of src by the scalar value and writes the products to dst.
 *
 * @override_id asc_mul_scalar__PUi32_PUi32_i32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int32_t elements that receives the
 * element-wise product with the scalar.
 * @param [in] src Start address of the source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_mul_scalar(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Multiplies int32_t elements of src by the scalar value over repeat vector iterations using per-operand
 * DataBlock and repeat strides, writing the products to dst.
 *
 * @override_id asc_mul_scalar__PUi32_PUi32_i32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int32_t elements that receives the
 * element-wise product with the scalar.
 * @param [in] src Start address of the source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
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
__aicore__ inline void asc_mul_scalar(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Subtracts src1 from src0 element-wise for the first count half elements and writes the differences to dst.
 *
 * @override_id asc_sub__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of half elements that receives the
 * element-wise differences.
 * @param [in] src0 Start address of the first source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_sub(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Subtracts src1 from src0 element-wise over repeat vector iterations of half elements, using per-operand
 * DataBlock and repeat strides, and writes the differences to dst.
 *
 * @override_id asc_sub__PUf16_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of half elements that receives the
 * element-wise differences.
 * @param [in] src0 Start address of the first source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride between DataBlocks inside one iteration for source operand 0, counted in
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride between DataBlocks inside one iteration for source operand 1, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 0, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 1, counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_sub(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Subtracts src1 from src0 element-wise for the first count float elements and writes the differences to dst.
 *
 * @override_id asc_sub__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of float elements that receives the
 * element-wise differences.
 * @param [in] src0 Start address of the first source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_sub(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Subtracts src1 from src0 element-wise over repeat vector iterations of float elements, using per-operand
 * DataBlock and repeat strides, and writes the differences to dst.
 *
 * @override_id asc_sub__PUf32_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of float elements that receives the
 * element-wise differences.
 * @param [in] src0 Start address of the first source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride between DataBlocks inside one iteration for source operand 0, counted in
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride between DataBlocks inside one iteration for source operand 1, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 0, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 1, counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_sub(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Subtracts src1 from src0 element-wise for the first count int16_t elements and writes the differences to dst.
 *
 * @override_id asc_sub__PUi16_PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int16_t elements that receives the
 * element-wise differences.
 * @param [in] src0 Start address of the first source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_sub(__ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Subtracts src1 from src0 element-wise over repeat vector iterations of int16_t elements, using per-operand
 * DataBlock and repeat strides, and writes the differences to dst.
 *
 * @override_id asc_sub__PUi16_PUi16_PUi16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int16_t elements that receives the
 * element-wise differences.
 * @param [in] src0 Start address of the first source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride between DataBlocks inside one iteration for source operand 0, counted in
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride between DataBlocks inside one iteration for source operand 1, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 0, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 1, counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_sub(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Subtracts src1 from src0 element-wise for the first count int32_t elements and writes the differences to dst.
 *
 * @override_id asc_sub__PUi32_PUi32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int32_t elements that receives the
 * element-wise differences.
 * @param [in] src0 Start address of the first source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_sub(__ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Subtracts src1 from src0 element-wise over repeat vector iterations of int32_t elements, using per-operand
 * DataBlock and repeat strides, and writes the differences to dst.
 *
 * @override_id asc_sub__PUi32_PUi32_PUi32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int32_t elements that receives the
 * element-wise differences.
 * @param [in] src0 Start address of the first source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] src1 Start address of the second source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] repeat Number of iterations to execute; each iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration for the destination, counted in
 * 32-byte DataBlocks.
 * @param [in] src0_block_stride Address stride between DataBlocks inside one iteration for source operand 0, counted in
 * 32-byte DataBlocks.
 * @param [in] src1_block_stride Address stride between DataBlocks inside one iteration for source operand 1, counted in
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of consecutive iterations for the
 * destination, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 0, counted in 32-byte DataBlocks (0 repeats the first iteration).
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of consecutive iterations for source operand
 * 1, counted in 32-byte DataBlocks (0 repeats the first iteration).
 */
__aicore__ inline void asc_sub(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Subtracts the scalar value from each of the first count half elements of src and writes the differences to
 * dst.
 *
 * @override_id asc_sub_scalar__PUf16_PUf16_f16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of half elements that receives the
 * element-wise difference from the scalar.
 * @param [in] src Start address of the source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_sub_scalar(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Subtracts the scalar value from half elements of src over repeat vector iterations using per-operand DataBlock
 * and repeat strides, writing the differences to dst.
 *
 * @override_id asc_sub_scalar__PUf16_PUf16_f16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of half elements that receives the
 * element-wise difference from the scalar.
 * @param [in] src Start address of the source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
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
__aicore__ inline void asc_sub_scalar(
    __ubuf__ half* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Subtracts the scalar value from each of the first count float elements of src and writes the differences to
 * dst.
 *
 * @override_id asc_sub_scalar__PUf32_PUf32_f32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of float elements that receives the
 * element-wise difference from the scalar.
 * @param [in] src Start address of the source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_sub_scalar(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Subtracts the scalar value from float elements of src over repeat vector iterations using per-operand
 * DataBlock and repeat strides, writing the differences to dst.
 *
 * @override_id asc_sub_scalar__PUf32_PUf32_f32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of float elements that receives the
 * element-wise difference from the scalar.
 * @param [in] src Start address of the source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
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
__aicore__ inline void asc_sub_scalar(
    __ubuf__ float* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Subtracts the scalar value from each of the first count int16_t elements of src and writes the differences to
 * dst.
 *
 * @override_id asc_sub_scalar__PUi16_PUi16_i16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int16_t elements that receives the
 * element-wise difference from the scalar.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_sub_scalar(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Subtracts the scalar value from int16_t elements of src over repeat vector iterations using per-operand
 * DataBlock and repeat strides, writing the differences to dst.
 *
 * @override_id asc_sub_scalar__PUi16_PUi16_i16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int16_t elements that receives the
 * element-wise difference from the scalar.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
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
__aicore__ inline void asc_sub_scalar(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Subtracts the scalar value from each of the first count int32_t elements of src and writes the differences to
 * dst.
 *
 * @override_id asc_sub_scalar__PUi32_PUi32_i32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int32_t elements that receives the
 * element-wise difference from the scalar.
 * @param [in] src Start address of the source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_sub_scalar(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Subtracts the scalar value from int32_t elements of src over repeat vector iterations using per-operand
 * DataBlock and repeat strides, writing the differences to dst.
 *
 * @override_id asc_sub_scalar__PUi32_PUi32_i32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int32_t elements that receives the
 * element-wise difference from the scalar.
 * @param [in] src Start address of the source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
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
__aicore__ inline void asc_sub_scalar(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise reciprocal of the first count half elements addressed by src and writes the results
 * into dst.
 *
 * @override_id asc_rcp__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the half destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the half source vector; read only.
 * @param [in] count Number of leading half elements to process, counted in elements rather than bytes; the impl turns
 * it into a mask count for the call.
 */
__aicore__ inline void asc_rcp(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise reciprocal of the half elements addressed by src using the repeat and DataBlock
 * stride pattern and writes the per-iteration results into dst.
 *
 * @override_id asc_rcp__PUf16_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the half destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the half source vector; read only.
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
__aicore__ inline void asc_rcp(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise reciprocal of the first count float elements addressed by src and writes the results
 * into dst.
 *
 * @override_id asc_rcp__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the float destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the float source vector; read only.
 * @param [in] count Number of leading float elements to process, counted in elements rather than bytes; the impl turns
 * it into a mask count for the call.
 */
__aicore__ inline void asc_rcp(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise reciprocal of the float elements addressed by src using the repeat and DataBlock
 * stride pattern and writes the per-iteration results into dst.
 *
 * @override_id asc_rcp__PUf32_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the float destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the float source vector; read only.
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
__aicore__ inline void asc_rcp(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise reciprocal square root of the first count half elements addressed by src and writes
 * the results into dst.
 *
 * @override_id asc_rsqrt__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the half destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the half source vector; read only.
 * @param [in] count Number of leading half elements to process, counted in elements rather than bytes; the impl turns
 * it into a mask count for the call.
 */
__aicore__ inline void asc_rsqrt(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise reciprocal square root of the half elements addressed by src using the repeat and
 * DataBlock stride pattern and writes the per-iteration results into dst.
 *
 * @override_id asc_rsqrt__PUf16_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the half destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the half source vector; read only.
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
__aicore__ inline void asc_rsqrt(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise reciprocal square root of the first count float elements addressed by src and writes
 * the results into dst.
 *
 * @override_id asc_rsqrt__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the float destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the float source vector; read only.
 * @param [in] count Number of leading float elements to process, counted in elements rather than bytes; the impl turns
 * it into a mask count for the call.
 */
__aicore__ inline void asc_rsqrt(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise reciprocal square root of the float elements addressed by src using the repeat and
 * DataBlock stride pattern and writes the per-iteration results into dst.
 *
 * @override_id asc_rsqrt__PUf32_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the float destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the float source vector; read only.
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
__aicore__ inline void asc_rsqrt(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise square root of the half elements addressed by src using the repeat and DataBlock
 * stride pattern and writes the per-iteration results into dst.
 *
 * @override_id asc_sqrt__PUf16_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the half destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the half source vector; read only.
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
__aicore__ inline void asc_sqrt(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise square root of the first count half elements addressed by src and writes the results
 * into dst.
 *
 * @override_id asc_sqrt__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the half destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the half source vector; read only.
 * @param [in] count Number of leading half elements to process, counted in elements rather than bytes; the impl turns
 * it into a mask count for the call.
 */
__aicore__ inline void asc_sqrt(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise square root of the float elements addressed by src using the repeat and DataBlock
 * stride pattern and writes the per-iteration results into dst.
 *
 * @override_id asc_sqrt__PUf32_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the float destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the float source vector; read only.
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
__aicore__ inline void asc_sqrt(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Computes the element-wise square root of the first count float elements addressed by src and writes the
 * results into dst.
 *
 * @override_id asc_sqrt__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the float destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the float source vector; read only.
 * @param [in] count Number of leading float elements to process, counted in elements rather than bytes; the impl turns
 * it into a mask count for the call.
 */
__aicore__ inline void asc_sqrt(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Compares each of the first count half elements of src with the scalar value and writes the smaller value to
 * dst.
 *
 * @override_id asc_min_scalar__PUf16_PUf16_f16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of half elements that receives the
 * element-wise minimum against the scalar.
 * @param [in] src Start address of the source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_min_scalar(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Compares half elements of src with the scalar value over repeat vector iterations using per-operand DataBlock
 * and repeat strides, writing the smaller value to dst.
 *
 * @override_id asc_min_scalar__PUf16_PUf16_f16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of half elements that receives the
 * element-wise minimum against the scalar.
 * @param [in] src Start address of the source vector of half elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
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
__aicore__ inline void asc_min_scalar(
    __ubuf__ half* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Compares each of the first count float elements of src with the scalar value and writes the smaller value to
 * dst.
 *
 * @override_id asc_min_scalar__PUf32_PUf32_f32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of float elements that receives the
 * element-wise minimum against the scalar.
 * @param [in] src Start address of the source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_min_scalar(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Compares float elements of src with the scalar value over repeat vector iterations using per-operand DataBlock
 * and repeat strides, writing the smaller value to dst.
 *
 * @override_id asc_min_scalar__PUf32_PUf32_f32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of float elements that receives the
 * element-wise minimum against the scalar.
 * @param [in] src Start address of the source vector of float elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
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
__aicore__ inline void asc_min_scalar(
    __ubuf__ float* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Compares each of the first count int16_t elements of src with the scalar value and writes the smaller value to
 * dst.
 *
 * @override_id asc_min_scalar__PUi16_PUi16_i16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int16_t elements that receives the
 * element-wise minimum against the scalar.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_min_scalar(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Compares int16_t elements of src with the scalar value over repeat vector iterations using per-operand
 * DataBlock and repeat strides, writing the smaller value to dst.
 *
 * @override_id asc_min_scalar__PUi16_PUi16_i16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int16_t elements that receives the
 * element-wise minimum against the scalar.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
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
__aicore__ inline void asc_min_scalar(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Compares each of the first count int32_t elements of src with the scalar value and writes the smaller value to
 * dst.
 *
 * @override_id asc_min_scalar__PUi32_PUi32_i32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int32_t elements that receives the
 * element-wise minimum against the scalar.
 * @param [in] src Start address of the source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_min_scalar(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_arith
 * @brief Compares int32_t elements of src with the scalar value over repeat vector iterations using per-operand
 * DataBlock and repeat strides, writing the smaller value to dst.
 *
 * @override_id asc_min_scalar__PUi32_PUi32_i32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int32_t elements that receives the
 * element-wise minimum against the scalar.
 * @param [in] src Start address of the source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] value Scalar operand applied to every source element, with the same type as the elements.
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
__aicore__ inline void asc_min_scalar(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_ARITH_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_ARITH_H
#endif
