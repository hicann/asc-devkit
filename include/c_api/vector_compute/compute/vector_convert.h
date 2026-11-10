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
#error "vector_compute/compute/vector_convert.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_CONVERT_H
#endif

#ifndef INCLUDE_C_API_VECTOR_COMPUTE_COMPUTE_VECTOR_CONVERT_H
#define INCLUDE_C_API_VECTOR_COMPUTE_COMPUTE_VECTOR_CONVERT_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/memory_base_impl/vector_convert_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts bfloat16_t elements of src to float without a rounding mode over repeat iterations, using per-block
 * and per-repeat strides.
 *
 * @override_id asc_bfloat162float__PUf32_PUbf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the converted float values; must be 32-byte aligned.
 * @param [in] src Source vector start address of bfloat16_t elements; must be 32-byte aligned.
 * @param [in] repeat Number of iterations of the conversion.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of the source.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations.
 */
__aicore__ inline void asc_bfloat162float(
    __ubuf__ float* dst, __ubuf__ bfloat16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts count bfloat16_t elements of src to float without a rounding mode and stores the results in dst.
 *
 * @override_id asc_bfloat162float__PUf32_PUbf16_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the converted float values; must be 32-byte aligned.
 * @param [in] src Source vector start address of bfloat16_t elements; must be 32-byte aligned.
 * @param [in] count Number of elements converted by this contiguous-data prototype.
 */
__aicore__ inline void asc_bfloat162float(__ubuf__ float* dst, __ubuf__ bfloat16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts bfloat16_t elements of src to int32_t with ROUND rounding (half away from zero) in repeat iterations
 * using the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_bfloat162int32_rna__PUi32_PUbf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source bfloat16_t buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_bfloat162int32_rna(
    __ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count bfloat16_t elements of src to int32_t with ROUND rounding (half away from zero) and
 * writes the results to dst.
 *
 * @override_id asc_bfloat162int32_rna__PUi32_PUbf16_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source bfloat16_t buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_bfloat162int32_rna(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts bfloat16_t elements of src to int32_t with CEIL rounding (toward plus infinity) in repeat iterations
 * using the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_bfloat162int32_ru__PUi32_PUbf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source bfloat16_t buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_bfloat162int32_ru(
    __ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count bfloat16_t elements of src to int32_t with CEIL rounding (toward plus infinity) and
 * writes the results to dst.
 *
 * @override_id asc_bfloat162int32_ru__PUi32_PUbf16_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source bfloat16_t buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_bfloat162int32_ru(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts bfloat16_t elements of src to int32_t with FLOOR rounding (toward minus infinity) in repeat
 * iterations using the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_bfloat162int32_rd__PUi32_PUbf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source bfloat16_t buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_bfloat162int32_rd(
    __ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count bfloat16_t elements of src to int32_t with FLOOR rounding (toward minus infinity) and
 * writes the results to dst.
 *
 * @override_id asc_bfloat162int32_rd__PUi32_PUbf16_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source bfloat16_t buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_bfloat162int32_rd(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts bfloat16_t elements of src to int32_t with RINT rounding (round half to even) in repeat iterations
 * using the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_bfloat162int32_rn__PUi32_PUbf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source bfloat16_t buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_bfloat162int32_rn(
    __ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count bfloat16_t elements of src to int32_t with RINT rounding (round half to even) and
 * writes the results to dst.
 *
 * @override_id asc_bfloat162int32_rn__PUi32_PUbf16_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source bfloat16_t buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_bfloat162int32_rn(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts bfloat16_t elements of src to int32_t with TRUNC rounding (toward zero) in repeat iterations using
 * the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_bfloat162int32_rz__PUi32_PUbf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source bfloat16_t buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_bfloat162int32_rz(
    __ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count bfloat16_t elements of src to int32_t with TRUNC rounding (toward zero) and writes
 * the results to dst.
 *
 * @override_id asc_bfloat162int32_rz__PUi32_PUbf16_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source bfloat16_t buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_bfloat162int32_rz(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to bfloat16_t with RINT rounding (round half to even) in repeat iterations
 * using the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2bfloat16_rn__PUbf16_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination bfloat16_t buffer that receives the converted elements; must
 * be 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2bfloat16_rn(
    __ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count float elements of src to bfloat16_t with RINT rounding (round half to even) and
 * writes the results to dst.
 *
 * @override_id asc_float2bfloat16_rn__PUbf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination bfloat16_t buffer that receives the converted elements; must
 * be 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2bfloat16_rn(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to bfloat16_t with ROUND rounding (half away from zero) in repeat iterations
 * using the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2bfloat16_rna__PUbf16_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination bfloat16_t buffer that receives the converted elements; must
 * be 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2bfloat16_rna(
    __ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count float elements of src to bfloat16_t with ROUND rounding (half away from zero) and
 * writes the results to dst.
 *
 * @override_id asc_float2bfloat16_rna__PUbf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination bfloat16_t buffer that receives the converted elements; must
 * be 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2bfloat16_rna(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to bfloat16_t with FLOOR rounding (toward minus infinity) in repeat iterations
 * using the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2bfloat16_rd__PUbf16_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination bfloat16_t buffer that receives the converted elements; must
 * be 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2bfloat16_rd(
    __ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count float elements of src to bfloat16_t with FLOOR rounding (toward minus infinity) and
 * writes the results to dst.
 *
 * @override_id asc_float2bfloat16_rd__PUbf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination bfloat16_t buffer that receives the converted elements; must
 * be 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2bfloat16_rd(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to bfloat16_t with CEIL rounding (toward plus infinity) in repeat iterations
 * using the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2bfloat16_ru__PUbf16_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination bfloat16_t buffer that receives the converted elements; must
 * be 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2bfloat16_ru(
    __ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count float elements of src to bfloat16_t with CEIL rounding (toward plus infinity) and
 * writes the results to dst.
 *
 * @override_id asc_float2bfloat16_ru__PUbf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination bfloat16_t buffer that receives the converted elements; must
 * be 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2bfloat16_ru(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to bfloat16_t with TRUNC rounding (toward zero) in repeat iterations using the
 * given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2bfloat16_rz__PUbf16_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination bfloat16_t buffer that receives the converted elements; must
 * be 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2bfloat16_rz(
    __ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count float elements of src to bfloat16_t with TRUNC rounding (toward zero) and writes the
 * results to dst.
 *
 * @override_id asc_float2bfloat16_rz__PUbf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination bfloat16_t buffer that receives the converted elements; must
 * be 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2bfloat16_rz(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Applies the RINT rounding float-to-float precision conversion to the src elements in repeat iterations with
 * the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2float_rn__PUf32_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination float buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2float_rn(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Applies the RINT rounding float-to-float precision conversion to the first count float elements of src and
 * writes the results to dst.
 *
 * @override_id asc_float2float_rn__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination float buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2float_rn(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Applies the FLOOR rounding float-to-float precision conversion to the src elements in repeat iterations with
 * the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2float_rd__PUf32_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination float buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2float_rd(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Applies the FLOOR rounding float-to-float precision conversion to the first count float elements of src and
 * writes the results to dst.
 *
 * @override_id asc_float2float_rd__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination float buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2float_rd(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Applies the CEIL rounding float-to-float precision conversion to the src elements in repeat iterations with
 * the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2float_ru__PUf32_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination float buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2float_ru(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Applies the CEIL rounding float-to-float precision conversion to the first count float elements of src and
 * writes the results to dst.
 *
 * @override_id asc_float2float_ru__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination float buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2float_ru(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Applies the ROUND rounding float-to-float precision conversion to the src elements in repeat iterations with
 * the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2float_rna__PUf32_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination float buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2float_rna(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Applies the ROUND rounding float-to-float precision conversion to the first count float elements of src and
 * writes the results to dst.
 *
 * @override_id asc_float2float_rna__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination float buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2float_rna(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Applies the TRUNC rounding float-to-float precision conversion to the src elements in repeat iterations with
 * the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2float_rz__PUf32_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination float buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2float_rz(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Applies the TRUNC rounding float-to-float precision conversion to the first count float elements of src and
 * writes the results to dst.
 *
 * @override_id asc_float2float_rz__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination float buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2float_rz(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to half with RINT rounding when the conversion loses precision and no rounding
 * otherwise over repeat iterations, honouring the source and destination block and repeat strides.
 *
 * @override_id asc_float2half__PUf16_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned and receives
 * the converted values.
 * @param [in] src Start address of the source float vector in Unified Buffer; must be 32-byte aligned and is only read.
 * @param [in] repeat Iteration count passed straight to the conversion builtin; each iteration processes a fixed number
 * of 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between different DataBlocks inside one destination iteration, counted in
 * DataBlocks of 32 bytes.
 * @param [in] src_block_stride Address stride between different DataBlocks inside one source iteration, counted in
 * DataBlocks of 32 bytes.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in DataBlocks of 32 bytes.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_float2half(
    __ubuf__ half* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts count float elements of src to half with RINT rounding when the conversion loses precision and no
 * rounding otherwise and writes them to dst in Unified Buffer.
 *
 * @override_id asc_float2half__PUf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned and receives
 * the converted values.
 * @param [in] src Start address of the source float vector in Unified Buffer; must be 32-byte aligned and is only read.
 * @param [in] count Number of float elements converted; the implementation feeds it to the mask count register.
 */
__aicore__ inline void asc_float2half(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to half with RINT round-to-nearest-even rounding over repeat iterations,
 * honouring the source and destination block and repeat strides.
 *
 * @override_id asc_float2half_rn__PUf16_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned and receives
 * the converted values.
 * @param [in] src Start address of the source float vector in Unified Buffer; must be 32-byte aligned and is only read.
 * @param [in] repeat Iteration count passed straight to the conversion builtin; each iteration processes a fixed number
 * of 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between different DataBlocks inside one destination iteration, counted in
 * DataBlocks of 32 bytes.
 * @param [in] src_block_stride Address stride between different DataBlocks inside one source iteration, counted in
 * DataBlocks of 32 bytes.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in DataBlocks of 32 bytes.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_float2half_rn(
    __ubuf__ half* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts count float elements of src to half with RINT round-to-nearest-even rounding and writes them to dst
 * in Unified Buffer.
 *
 * @override_id asc_float2half_rn__PUf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned and receives
 * the converted values.
 * @param [in] src Start address of the source float vector in Unified Buffer; must be 32-byte aligned and is only read.
 * @param [in] count Number of float elements converted; the implementation feeds it to the mask count register.
 */
__aicore__ inline void asc_float2half_rn(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to half with ROUND round-half-away-from-zero rounding over repeat iterations,
 * honouring the source and destination block and repeat strides.
 *
 * @override_id asc_float2half_rna__PUf16_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned and receives
 * the converted values.
 * @param [in] src Start address of the source float vector in Unified Buffer; must be 32-byte aligned and is only read.
 * @param [in] repeat Iteration count passed straight to the conversion builtin; each iteration processes a fixed number
 * of 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between different DataBlocks inside one destination iteration, counted in
 * DataBlocks of 32 bytes.
 * @param [in] src_block_stride Address stride between different DataBlocks inside one source iteration, counted in
 * DataBlocks of 32 bytes.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in DataBlocks of 32 bytes.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_float2half_rna(
    __ubuf__ half* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts count float elements of src to half with ROUND round-half-away-from-zero rounding and writes them to
 * dst in Unified Buffer.
 *
 * @override_id asc_float2half_rna__PUf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned and receives
 * the converted values.
 * @param [in] src Start address of the source float vector in Unified Buffer; must be 32-byte aligned and is only read.
 * @param [in] count Number of float elements converted; the implementation feeds it to the mask count register.
 */
__aicore__ inline void asc_float2half_rna(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to half with FLOOR rounding toward minus infinity over repeat iterations,
 * honouring the source and destination block and repeat strides.
 *
 * @override_id asc_float2half_rd__PUf16_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned and receives
 * the converted values.
 * @param [in] src Start address of the source float vector in Unified Buffer; must be 32-byte aligned and is only read.
 * @param [in] repeat Iteration count passed straight to the conversion builtin; each iteration processes a fixed number
 * of 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between different DataBlocks inside one destination iteration, counted in
 * DataBlocks of 32 bytes.
 * @param [in] src_block_stride Address stride between different DataBlocks inside one source iteration, counted in
 * DataBlocks of 32 bytes.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in DataBlocks of 32 bytes.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_float2half_rd(
    __ubuf__ half* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts count float elements of src to half with FLOOR rounding toward minus infinity and writes them to dst
 * in Unified Buffer.
 *
 * @override_id asc_float2half_rd__PUf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned and receives
 * the converted values.
 * @param [in] src Start address of the source float vector in Unified Buffer; must be 32-byte aligned and is only read.
 * @param [in] count Number of float elements converted; the implementation feeds it to the mask count register.
 */
__aicore__ inline void asc_float2half_rd(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to half with CEIL rounding toward plus infinity over repeat iterations,
 * honouring the source and destination block and repeat strides.
 *
 * @override_id asc_float2half_ru__PUf16_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned and receives
 * the converted values.
 * @param [in] src Start address of the source float vector in Unified Buffer; must be 32-byte aligned and is only read.
 * @param [in] repeat Iteration count passed straight to the conversion builtin; each iteration processes a fixed number
 * of 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between different DataBlocks inside one destination iteration, counted in
 * DataBlocks of 32 bytes.
 * @param [in] src_block_stride Address stride between different DataBlocks inside one source iteration, counted in
 * DataBlocks of 32 bytes.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in DataBlocks of 32 bytes.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_float2half_ru(
    __ubuf__ half* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts count float elements of src to half with CEIL rounding toward plus infinity and writes them to dst in
 * Unified Buffer.
 *
 * @override_id asc_float2half_ru__PUf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned and receives
 * the converted values.
 * @param [in] src Start address of the source float vector in Unified Buffer; must be 32-byte aligned and is only read.
 * @param [in] count Number of float elements converted; the implementation feeds it to the mask count register.
 */
__aicore__ inline void asc_float2half_ru(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to half with TRUNC rounding toward zero over repeat iterations, honouring the
 * source and destination block and repeat strides.
 *
 * @override_id asc_float2half_rz__PUf16_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned and receives
 * the converted values.
 * @param [in] src Start address of the source float vector in Unified Buffer; must be 32-byte aligned and is only read.
 * @param [in] repeat Iteration count passed straight to the conversion builtin; each iteration processes a fixed number
 * of 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between different DataBlocks inside one destination iteration, counted in
 * DataBlocks of 32 bytes.
 * @param [in] src_block_stride Address stride between different DataBlocks inside one source iteration, counted in
 * DataBlocks of 32 bytes.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in DataBlocks of 32 bytes.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_float2half_rz(
    __ubuf__ half* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts count float elements of src to half with TRUNC rounding toward zero and writes them to dst in Unified
 * Buffer.
 *
 * @override_id asc_float2half_rz__PUf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned and receives
 * the converted values.
 * @param [in] src Start address of the source float vector in Unified Buffer; must be 32-byte aligned and is only read.
 * @param [in] count Number of float elements converted; the implementation feeds it to the mask count register.
 */
__aicore__ inline void asc_float2half_rz(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to half with ODD Von Neumann round-to-nearest-odd rounding over repeat
 * iterations, honouring the source and destination block and repeat strides.
 *
 * @override_id asc_float2half_ro__PUf16_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned and receives
 * the converted values.
 * @param [in] src Start address of the source float vector in Unified Buffer; must be 32-byte aligned and is only read.
 * @param [in] repeat Iteration count passed straight to the conversion builtin; each iteration processes a fixed number
 * of 32-byte DataBlocks.
 * @param [in] dst_block_stride Address stride between different DataBlocks inside one destination iteration, counted in
 * DataBlocks of 32 bytes.
 * @param [in] src_block_stride Address stride between different DataBlocks inside one source iteration, counted in
 * DataBlocks of 32 bytes.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in DataBlocks of 32 bytes.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_float2half_ro(
    __ubuf__ half* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts count float elements of src to half with ODD Von Neumann round-to-nearest-odd rounding and writes
 * them to dst in Unified Buffer.
 *
 * @override_id asc_float2half_ro__PUf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned and receives
 * the converted values.
 * @param [in] src Start address of the source float vector in Unified Buffer; must be 32-byte aligned and is only read.
 * @param [in] count Number of float elements converted; the implementation feeds it to the mask count register.
 */
__aicore__ inline void asc_float2half_ro(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to int32_t with ROUND rounding (half away from zero) in repeat iterations using
 * the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2int32_rna__PUi32_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2int32_rna(
    __ubuf__ int32_t* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count float elements of src to int32_t with ROUND rounding (half away from zero) and writes
 * the results to dst.
 *
 * @override_id asc_float2int32_rna__PUi32_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2int32_rna(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to int32_t with CEIL rounding (toward plus infinity) in repeat iterations using
 * the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2int32_ru__PUi32_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2int32_ru(
    __ubuf__ int32_t* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count float elements of src to int32_t with CEIL rounding (toward plus infinity) and writes
 * the results to dst.
 *
 * @override_id asc_float2int32_ru__PUi32_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2int32_ru(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to int32_t with FLOOR rounding (toward minus infinity) in repeat iterations
 * using the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2int32_rd__PUi32_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2int32_rd(
    __ubuf__ int32_t* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count float elements of src to int32_t with FLOOR rounding (toward minus infinity) and
 * writes the results to dst.
 *
 * @override_id asc_float2int32_rd__PUi32_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2int32_rd(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to int32_t with RINT rounding (round half to even) in repeat iterations using
 * the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2int32_rn__PUi32_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2int32_rn(
    __ubuf__ int32_t* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count float elements of src to int32_t with RINT rounding (round half to even) and writes
 * the results to dst.
 *
 * @override_id asc_float2int32_rn__PUi32_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2int32_rn(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to int32_t with TRUNC rounding (toward zero) in repeat iterations using the
 * given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2int32_rz__PUi32_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2int32_rz(
    __ubuf__ int32_t* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count float elements of src to int32_t with TRUNC rounding (toward zero) and writes the
 * results to dst.
 *
 * @override_id asc_float2int32_rz__PUi32_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2int32_rz(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to int64_t with ROUND rounding (half away from zero) in repeat iterations using
 * the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2int64_rna__PUi64_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int64_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2int64_rna(
    __ubuf__ int64_t* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count float elements of src to int64_t with ROUND rounding (half away from zero) and writes
 * the results to dst.
 *
 * @override_id asc_float2int64_rna__PUi64_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int64_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2int64_rna(__ubuf__ int64_t* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to int64_t with CEIL rounding (toward plus infinity) in repeat iterations using
 * the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2int64_ru__PUi64_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int64_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2int64_ru(
    __ubuf__ int64_t* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count float elements of src to int64_t with CEIL rounding (toward plus infinity) and writes
 * the results to dst.
 *
 * @override_id asc_float2int64_ru__PUi64_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int64_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2int64_ru(__ubuf__ int64_t* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to int64_t with FLOOR rounding (toward minus infinity) in repeat iterations
 * using the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2int64_rd__PUi64_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int64_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2int64_rd(
    __ubuf__ int64_t* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count float elements of src to int64_t with FLOOR rounding (toward minus infinity) and
 * writes the results to dst.
 *
 * @override_id asc_float2int64_rd__PUi64_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int64_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2int64_rd(__ubuf__ int64_t* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to int64_t with RINT rounding (round half to even) in repeat iterations using
 * the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2int64_rn__PUi64_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int64_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2int64_rn(
    __ubuf__ int64_t* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count float elements of src to int64_t with RINT rounding (round half to even) and writes
 * the results to dst.
 *
 * @override_id asc_float2int64_rn__PUi64_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int64_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2int64_rn(__ubuf__ int64_t* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to int64_t with TRUNC rounding (toward zero) in repeat iterations using the
 * given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2int64_rz__PUi64_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int64_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2int64_rz(
    __ubuf__ int64_t* dst, __ubuf__ float* src, const uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count float elements of src to int64_t with TRUNC rounding (toward zero) and writes the
 * results to dst.
 *
 * @override_id asc_float2int64_rz__PUi64_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int64_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2int64_rz(__ubuf__ int64_t* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts half elements of src to float over repeat iterations with per-block and per-repeat strides, applying
 * RINT rounding when precision is lost.
 *
 * @override_id asc_half2float__PUf32_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the converted float values; must be 32-byte aligned.
 * @param [in] src Source vector start address of half elements; must be 32-byte aligned.
 * @param [in] repeat Number of iterations of the conversion.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of the source.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations.
 */
__aicore__ inline void asc_half2float(
    __ubuf__ float* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts count half elements of src to float using RINT rounding when precision is lost and stores the results
 * in dst.
 *
 * @override_id asc_half2float__PUf32_PUf16_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the converted float values; must be 32-byte aligned.
 * @param [in] src Source vector start address of half elements; must be 32-byte aligned.
 * @param [in] count Number of elements participating in the contiguous conversion.
 */
__aicore__ inline void asc_half2float(__ubuf__ float* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts up to repeat iterations of half data into packed int4 values using per-DataBlock and per-iteration
 * strides and RINT rounding only when inexact.
 *
 * @override_id asc_half2int4__PUi4x2_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer for packed int4b_t values; must be 32-byte aligned and
 * written by the conversion.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] repeat Number of iterations executed by the instruction, given as uint8_t and bounded by the documented
 * hardware limit of 255.
 * @param [in] dst_block_stride Address stride between DataBlocks within one iteration for the destination; the default
 * unit is one DataBlock of 32 bytes.
 * @param [in] src_block_stride Address stride between DataBlocks within one iteration for the source, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent iterations for the destination,
 * in units of 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent iterations for the source, in
 * units of 32-byte DataBlocks.
 */
__aicore__ inline void asc_half2int4(
    __ubuf__ int4b_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of a UB buffer into packed int4 values, rounding to nearest even only
 * when the conversion is inexact.
 *
 * @override_id asc_half2int4__PUi4x2_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer receiving packed int4b_t values, two 4-bit results per
 * byte; must be 32-byte aligned.
 * @param [in] src Start address of the source UB buffer of half elements to convert; must be 32-byte aligned and is
 * read only.
 * @param [in] count Number of elements converted starting from the first one; it must be even because the destination
 * packs two int4 values per byte.
 */
__aicore__ inline void asc_half2int4(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts up to repeat iterations of half data into packed int4 values with per-DataBlock and per-iteration
 * strides using ROUND rounding away from zero.
 *
 * @override_id asc_half2int4_rna__PUi4x2_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer for packed int4b_t values; requires 32-byte alignment and
 * is written.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] repeat Iteration count of the instruction as uint8_t, bounded by the documented hardware limit of 255
 * iterations.
 * @param [in] dst_block_stride Destination stride between DataBlocks inside one iteration, measured in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Source stride between DataBlocks inside one iteration, measured in 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination stride between the same DataBlock of adjacent iterations, measured in
 * 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source stride between the same DataBlock of adjacent iterations, measured in 32-byte
 * DataBlocks.
 */
__aicore__ inline void asc_half2int4_rna(
    __ubuf__ int4b_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of a UB buffer into packed int4 values using ROUND rounding with ties
 * away from zero.
 *
 * @override_id asc_half2int4_rna__PUi4x2_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer receiving packed int4b_t values, two per byte; 32-byte
 * alignment required.
 * @param [in] src Start address of the source buffer of half elements; 32-byte aligned and read only.
 * @param [in] count Number of half elements converted starting from the first element; must be even for the packed int4
 * destination.
 */
__aicore__ inline void asc_half2int4_rna(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts up to repeat iterations of half data into packed int4 values with per-DataBlock and per-iteration
 * strides using CEIL rounding.
 *
 * @override_id asc_half2int4_ru__PUi4x2_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer receiving packed int4b_t values; 32-byte aligned and
 * written by the conversion.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] repeat Number of instruction iterations as uint8_t, limited by the documented maximum of 255 iterations.
 * @param [in] dst_block_stride Destination stride between DataBlocks within one iteration, in units of 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Source stride between DataBlocks within one iteration, in units of 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source stride between the same DataBlock of adjacent iterations, in units of 32-byte
 * DataBlocks.
 */
__aicore__ inline void asc_half2int4_ru(
    __ubuf__ int4b_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of a UB buffer into packed int4 values using CEIL rounding toward
 * positive infinity.
 *
 * @override_id asc_half2int4_ru__PUi4x2_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer for packed int4b_t values, two per byte; requires 32-byte
 * alignment.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] count Number of half elements converted from the beginning of the buffer; must be even for a packed int4
 * destination.
 */
__aicore__ inline void asc_half2int4_ru(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts up to repeat iterations of half data into packed int4 values with per-DataBlock and per-iteration
 * strides using FLOOR rounding.
 *
 * @override_id asc_half2int4_rd__PUi4x2_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer receiving packed int4b_t values; must be 32-byte aligned
 * and is written.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] repeat Number of iterations performed by the instruction, given as uint8_t and capped by the documented
 * 255-iteration limit.
 * @param [in] dst_block_stride Stride between DataBlocks within one iteration for the destination, in units of 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Stride between DataBlocks within one iteration for the source, in units of 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Stride between the same DataBlock of adjacent iterations for the destination, in units
 * of 32-byte DataBlocks.
 * @param [in] src_repeat_stride Stride between the same DataBlock of adjacent iterations for the source, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_half2int4_rd(
    __ubuf__ int4b_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of a UB buffer into packed int4 values using FLOOR rounding toward
 * negative infinity.
 *
 * @override_id asc_half2int4_rd__PUi4x2_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer for packed int4b_t values, two per byte; needs 32-byte
 * alignment.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] count Number of half elements converted from the start of the buffer; must be even because two int4
 * values share a byte.
 */
__aicore__ inline void asc_half2int4_rd(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts up to repeat iterations of half data into packed int4 values with per-DataBlock and per-iteration
 * strides using RINT rounding to nearest even.
 *
 * @override_id asc_half2int4_rn__PUi4x2_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer for packed int4b_t values; 32-byte aligned and written by
 * the conversion.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] repeat Iteration count of the instruction as uint8_t, limited by the documented hardware maximum of 255
 * iterations.
 * @param [in] dst_block_stride Destination stride between DataBlocks inside one iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Source stride between DataBlocks inside one iteration, counted in 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination stride between the same DataBlock of adjacent iterations, counted in
 * 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source stride between the same DataBlock of adjacent iterations, counted in 32-byte
 * DataBlocks.
 */
__aicore__ inline void asc_half2int4_rn(
    __ubuf__ int4b_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of a UB buffer into packed int4 values using RINT rounding to nearest
 * even.
 *
 * @override_id asc_half2int4_rn__PUi4x2_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer receiving packed int4b_t values, two per byte; requires
 * 32-byte alignment.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] count Number of half elements converted from the first position, which must be an even number for the
 * int4 destination.
 */
__aicore__ inline void asc_half2int4_rn(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts up to repeat iterations of half data into packed int4 values with per-DataBlock and per-iteration
 * strides using TRUNC rounding toward zero.
 *
 * @override_id asc_half2int4_rz__PUi4x2_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer for packed int4b_t values; must be 32-byte aligned and is
 * written.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] repeat Iteration count of the instruction as uint8_t, capped by the documented hardware limit of 255
 * iterations.
 * @param [in] dst_block_stride Destination stride between DataBlocks inside one iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Source stride between DataBlocks inside one iteration, counted in 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination stride between the same DataBlock of adjacent iterations, counted in
 * 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source stride between the same DataBlock of adjacent iterations, counted in 32-byte
 * DataBlocks.
 */
__aicore__ inline void asc_half2int4_rz(
    __ubuf__ int4b_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of a UB buffer into packed int4 values using TRUNC rounding toward
 * zero.
 *
 * @override_id asc_half2int4_rz__PUi4x2_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer receiving packed int4b_t values, two per byte; 32-byte
 * alignment required.
 * @param [in] src Start address of the source UB buffer of half elements to convert; 32-byte aligned and read only.
 * @param [in] count Number of half elements converted from the start of the buffer; must be even because the
 * destination packs two int4 values per byte.
 */
__aicore__ inline void asc_half2int4_rz(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts up to repeat iterations of half data into int8_t values using per-DataBlock and per-iteration strides
 * without a specified rounding mode.
 *
 * @override_id asc_half2int8__PUi8_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer for int8_t results; must be 32-byte aligned and is
 * written.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] repeat Iteration count of the instruction as uint8_t, bounded by the documented hardware limit of 255
 * iterations.
 * @param [in] dst_block_stride Destination stride between DataBlocks within one iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Source stride between DataBlocks within one iteration, counted in 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination stride between the same DataBlock of adjacent iterations, counted in
 * 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source stride between the same DataBlock of adjacent iterations, counted in 32-byte
 * DataBlocks.
 */
__aicore__ inline void asc_half2int8(
    __ubuf__ int8_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of a UB buffer into int8_t values without specifying a rounding mode.
 *
 * @override_id asc_half2int8__PUi8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer receiving converted int8_t elements; must be 32-byte
 * aligned and is written.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] count Number of elements converted from the first position, consumed as a mask count by the underlying
 * instruction.
 */
__aicore__ inline void asc_half2int8(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts up to repeat iterations of half data into int8_t values with per-DataBlock and per-iteration strides
 * using ROUND rounding away from zero.
 *
 * @override_id asc_half2int8_rna__PUi8_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer for int8_t results; requires 32-byte alignment and is
 * written.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] repeat Iteration count of the instruction, typed uint8_t and capped by the documented 255-iteration
 * limit.
 * @param [in] dst_block_stride Destination stride between DataBlocks within one iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Source stride between DataBlocks within one iteration, counted in 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination stride between the same DataBlock of adjacent iterations, counted in
 * 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source stride between the same DataBlock of adjacent iterations, counted in 32-byte
 * DataBlocks.
 */
__aicore__ inline void asc_half2int8_rna(
    __ubuf__ int8_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of a UB buffer into int8_t values using ROUND rounding with ties away
 * from zero.
 *
 * @override_id asc_half2int8_rna__PUi8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer receiving int8_t results; requires 32-byte alignment and
 * is written.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] count Number of elements converted from the beginning of the buffer, applied as the instruction mask
 * count.
 */
__aicore__ inline void asc_half2int8_rna(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts up to repeat iterations of half data into int8_t values with per-DataBlock and per-iteration strides
 * using CEIL rounding.
 *
 * @override_id asc_half2int8_ru__PUi8_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer for int8_t results; requires 32-byte alignment and is
 * written.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] repeat Iteration count of the instruction as uint8_t, bounded by the documented 255-iteration limit.
 * @param [in] dst_block_stride Destination stride between DataBlocks within one iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Source stride between DataBlocks within one iteration, counted in 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination stride between the same DataBlock of adjacent iterations, counted in
 * 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source stride between the same DataBlock of adjacent iterations, counted in 32-byte
 * DataBlocks.
 */
__aicore__ inline void asc_half2int8_ru(
    __ubuf__ int8_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of a UB buffer into int8_t values using CEIL rounding toward positive
 * infinity.
 *
 * @override_id asc_half2int8_ru__PUi8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer receiving int8_t results; requires 32-byte alignment and
 * is written.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] count Number of elements converted from the first position, used as the mask count of the underlying
 * instruction.
 */
__aicore__ inline void asc_half2int8_ru(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts up to repeat iterations of half data into int8_t values with per-DataBlock and per-iteration strides
 * using FLOOR rounding.
 *
 * @override_id asc_half2int8_rd__PUi8_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer receiving int8_t results; must be 32-byte aligned and is
 * written.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] repeat Number of instruction iterations, typed uint8_t and bounded by the documented maximum of 255.
 * @param [in] dst_block_stride Destination stride between DataBlocks inside one iteration, in units of 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Source stride between DataBlocks inside one iteration, in units of 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source stride between the same DataBlock of adjacent iterations, in units of 32-byte
 * DataBlocks.
 */
__aicore__ inline void asc_half2int8_rd(
    __ubuf__ int8_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of a UB buffer into int8_t values using FLOOR rounding toward negative
 * infinity.
 *
 * @override_id asc_half2int8_rd__PUi8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer for int8_t results; 32-byte aligned and written by the
 * conversion.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] count Number of elements converted from the start of the buffer, consumed as the instruction mask count.
 */
__aicore__ inline void asc_half2int8_rd(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts up to repeat iterations of half data into int8_t values with per-DataBlock and per-iteration strides
 * using RINT rounding to nearest even.
 *
 * @override_id asc_half2int8_rn__PUi8_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer receiving int8_t results; 32-byte aligned and written by
 * the conversion.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] repeat Iteration count of the instruction as uint8_t, limited by the documented maximum of 255
 * iterations.
 * @param [in] dst_block_stride Destination stride between DataBlocks inside one iteration, in units of 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Source stride between DataBlocks inside one iteration, in units of 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source stride between the same DataBlock of adjacent iterations, in units of 32-byte
 * DataBlocks.
 */
__aicore__ inline void asc_half2int8_rn(
    __ubuf__ int8_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of a UB buffer into int8_t values using RINT rounding to nearest even.
 *
 * @override_id asc_half2int8_rn__PUi8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer for converted int8_t elements; 32-byte aligned and
 * written by the conversion.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] count Number of elements converted from the first position, used as the instruction mask count.
 */
__aicore__ inline void asc_half2int8_rn(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts up to repeat iterations of half data into int8_t values with per-DataBlock and per-iteration strides
 * using TRUNC rounding toward zero.
 *
 * @override_id asc_half2int8_rz__PUi8_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer receiving int8_t results; 32-byte aligned and written by
 * the conversion.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] repeat Iteration count of the instruction as uint8_t, limited by the documented hardware maximum of 255.
 * @param [in] dst_block_stride Destination stride between DataBlocks inside one iteration, in units of 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Source stride between DataBlocks inside one iteration, in units of 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source stride between the same DataBlock of adjacent iterations, in units of 32-byte
 * DataBlocks.
 */
__aicore__ inline void asc_half2int8_rz(
    __ubuf__ int8_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of a UB buffer into int8_t values using TRUNC rounding toward zero.
 *
 * @override_id asc_half2int8_rz__PUi8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer for int8_t results; 32-byte aligned and written by the
 * conversion.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] count Number of elements converted from the start of the buffer, used as the instruction mask count.
 */
__aicore__ inline void asc_half2int8_rz(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts half elements of src to int16_t with ROUND rounding (half away from zero) in repeat iterations using
 * the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_half2int16_rna__PUi16_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int16_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source half buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_half2int16_rna(
    __ubuf__ int16_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of src to int16_t with ROUND rounding (half away from zero) and writes
 * the results to dst.
 *
 * @override_id asc_half2int16_rna__PUi16_PUf16_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int16_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source half buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_half2int16_rna(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts half elements of src to int16_t with CEIL rounding (toward plus infinity) in repeat iterations using
 * the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_half2int16_ru__PUi16_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int16_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source half buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_half2int16_ru(
    __ubuf__ int16_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of src to int16_t with CEIL rounding (toward plus infinity) and writes
 * the results to dst.
 *
 * @override_id asc_half2int16_ru__PUi16_PUf16_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int16_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source half buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_half2int16_ru(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts half elements of src to int16_t with FLOOR rounding (toward minus infinity) in repeat iterations
 * using the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_half2int16_rd__PUi16_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int16_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source half buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_half2int16_rd(
    __ubuf__ int16_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of src to int16_t with FLOOR rounding (toward minus infinity) and
 * writes the results to dst.
 *
 * @override_id asc_half2int16_rd__PUi16_PUf16_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int16_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source half buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_half2int16_rd(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts half elements of src to int16_t with RINT rounding (round half to even) in repeat iterations using
 * the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_half2int16_rn__PUi16_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int16_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source half buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_half2int16_rn(
    __ubuf__ int16_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of src to int16_t with RINT rounding (round half to even) and writes
 * the results to dst.
 *
 * @override_id asc_half2int16_rn__PUi16_PUf16_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int16_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source half buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_half2int16_rn(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts half elements of src to int16_t with TRUNC rounding (toward zero) in repeat iterations using the
 * given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_half2int16_rz__PUi16_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int16_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source half buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_half2int16_rz(
    __ubuf__ int16_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of src to int16_t with TRUNC rounding (toward zero) and writes the
 * results to dst.
 *
 * @override_id asc_half2int16_rz__PUi16_PUf16_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int16_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source half buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_half2int16_rz(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts half elements of src to int32_t with ROUND rounding (half away from zero) in repeat iterations using
 * the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_half2int32_rna__PUi32_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source half buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_half2int32_rna(
    __ubuf__ int32_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of src to int32_t with ROUND rounding (half away from zero) and writes
 * the results to dst.
 *
 * @override_id asc_half2int32_rna__PUi32_PUf16_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source half buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_half2int32_rna(__ubuf__ int32_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts half elements of src to int32_t with CEIL rounding (toward plus infinity) in repeat iterations using
 * the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_half2int32_ru__PUi32_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source half buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_half2int32_ru(
    __ubuf__ int32_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of src to int32_t with CEIL rounding (toward plus infinity) and writes
 * the results to dst.
 *
 * @override_id asc_half2int32_ru__PUi32_PUf16_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source half buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_half2int32_ru(__ubuf__ int32_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts half elements of src to int32_t with FLOOR rounding (toward minus infinity) in repeat iterations
 * using the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_half2int32_rd__PUi32_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source half buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_half2int32_rd(
    __ubuf__ int32_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of src to int32_t with FLOOR rounding (toward minus infinity) and
 * writes the results to dst.
 *
 * @override_id asc_half2int32_rd__PUi32_PUf16_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source half buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_half2int32_rd(__ubuf__ int32_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts half elements of src to int32_t with RINT rounding (round half to even) in repeat iterations using
 * the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_half2int32_rn__PUi32_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source half buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_half2int32_rn(
    __ubuf__ int32_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of src to int32_t with RINT rounding (round half to even) and writes
 * the results to dst.
 *
 * @override_id asc_half2int32_rn__PUi32_PUf16_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source half buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_half2int32_rn(__ubuf__ int32_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts half elements of src to int32_t with TRUNC rounding (toward zero) in repeat iterations using the
 * given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_half2int32_rz__PUi32_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source half buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_half2int32_rz(
    __ubuf__ int32_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of src to int32_t with TRUNC rounding (toward zero) and writes the
 * results to dst.
 *
 * @override_id asc_half2int32_rz__PUi32_PUf16_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int32_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source half buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_half2int32_rz(__ubuf__ int32_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int4_t elements of src to half over repeat iterations using per-block and per-repeat strides.
 *
 * @override_id asc_int42half__PUf16_PUi4x2_u8_u16_u16_u8_u8
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the half values; must be 32-byte aligned.
 * @param [in] src Source vector start address of int4b_t elements; must be 32-byte aligned.
 * @param [in] repeat Number of iterations of the conversion.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of the source.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations; carried
 * as uint8_t for this 4-bit source type.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations; carried as
 * uint8_t for this 4-bit source type.
 */
__aicore__ inline void asc_int42half(
    __ubuf__ half* dst, __ubuf__ int4b_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint8_t dst_repeat_stride, uint8_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts count int4_t elements of src to half and stores the results in dst; count must be even.
 *
 * @override_id asc_int42half__PUf16_PUi4x2_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the half values; must be 32-byte aligned.
 * @param [in] src Source vector start address of int4b_t elements; must be 32-byte aligned.
 * @param [in] count Number of elements converted by this contiguous-data prototype; must be even for int4b_t input.
 */
__aicore__ inline void asc_int42half(__ubuf__ half* dst, __ubuf__ int4b_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int8_t elements of src to half over repeat iterations using per-block and per-repeat strides.
 *
 * @override_id asc_int82half__PUf16_PUi8_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the half values; must be 32-byte aligned.
 * @param [in] src Source vector start address of int8_t elements; must be 32-byte aligned.
 * @param [in] repeat Number of iterations of the conversion.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of the source.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations.
 */
__aicore__ inline void asc_int82half(
    __ubuf__ half* dst, __ubuf__ int8_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts count int8_t elements of src to half and stores the results in dst.
 *
 * @override_id asc_int82half__PUf16_PUi8_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the half values; must be 32-byte aligned.
 * @param [in] src Source vector start address of int8_t elements; must be 32-byte aligned.
 * @param [in] count Number of elements participating in the contiguous conversion.
 */
__aicore__ inline void asc_int82half(__ubuf__ half* dst, __ubuf__ int8_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts uint8_t elements of src to half over repeat iterations using per-block and per-repeat strides.
 *
 * @override_id asc_uint82half__PUf16_PUu8_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the half values; must be 32-byte aligned.
 * @param [in] src Source vector start address of uint8_t elements; must be 32-byte aligned.
 * @param [in] repeat Number of iterations of the conversion.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of the source.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations.
 */
__aicore__ inline void asc_uint82half(
    __ubuf__ half* dst, __ubuf__ uint8_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts count uint8_t elements of src to half and stores the results in dst.
 *
 * @override_id asc_uint82half__PUf16_PUu8_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the half values; must be 32-byte aligned.
 * @param [in] src Source vector start address of uint8_t elements; must be 32-byte aligned.
 * @param [in] count Number of elements participating in the contiguous conversion.
 */
__aicore__ inline void asc_uint82half(__ubuf__ half* dst, __ubuf__ uint8_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int16_t elements of src to float over repeat iterations using per-block and per-repeat strides.
 *
 * @override_id asc_int162float__PUf32_PUi16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the converted float values; must be 32-byte aligned.
 * @param [in] src Source vector start address of int16_t elements; must be 32-byte aligned.
 * @param [in] repeat Number of iterations of the conversion.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of the source.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations.
 */
__aicore__ inline void asc_int162float(
    __ubuf__ float* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts count int16_t elements of src to float and stores the results in dst.
 *
 * @override_id asc_int162float__PUf32_PUi16_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the converted float values; must be 32-byte aligned.
 * @param [in] src Source vector start address of int16_t elements; must be 32-byte aligned.
 * @param [in] count Number of elements participating in the contiguous conversion.
 */
__aicore__ inline void asc_int162float(__ubuf__ float* dst, __ubuf__ int16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count int32_t elements of src to float, uses RINT rounding when the conversion loses
 * precision and performs no rounding otherwise, as noted in the prototype comment.
 *
 * @override_id asc_int322float__PUf32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination float vector that receives the converted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int32_t vector to convert; it must be 32-byte aligned.
 * @param [in] count Number of elements to convert, counted in int32_t elements of the source vector.
 */
__aicore__ inline void asc_int322float(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int32_t elements to float over a configurable repeat and DataBlock stride pattern, uses RINT rounding
 * when the conversion loses precision and performs no rounding otherwise, as noted in the prototype comment.
 *
 * @override_id asc_int322float__PUf32_PUi32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination float vector that receives the converted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int32_t vector to convert; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks, and the count form of this
 * API instead uses the framework default repeat value.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src_block_stride Source address stride between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source address stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_int322float(
    __ubuf__ float* dst, __ubuf__ int32_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count int32_t elements of src to float, applies ROUND, that is round-half-away-from-zero,
 * as its rounding mode.
 *
 * @override_id asc_int322float_rna__PUf32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination float vector that receives the converted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int32_t vector to convert; it must be 32-byte aligned.
 * @param [in] count Number of elements to convert, counted in int32_t elements of the source vector.
 */
__aicore__ inline void asc_int322float_rna(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int32_t elements to float over a configurable repeat and DataBlock stride pattern, applies ROUND,
 * that is round-half-away-from-zero, as its rounding mode.
 *
 * @override_id asc_int322float_rna__PUf32_PUi32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination float vector that receives the converted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int32_t vector to convert; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks, and the count form of this
 * API instead uses the framework default repeat value.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src_block_stride Source address stride between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source address stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_int322float_rna(
    __ubuf__ float* dst, __ubuf__ int32_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count int32_t elements of src to float, applies CEIL, that is rounding toward positive
 * infinity, as its rounding mode.
 *
 * @override_id asc_int322float_ru__PUf32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination float vector that receives the converted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int32_t vector to convert; it must be 32-byte aligned.
 * @param [in] count Number of elements to convert, counted in int32_t elements of the source vector.
 */
__aicore__ inline void asc_int322float_ru(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int32_t elements to float over a configurable repeat and DataBlock stride pattern, applies CEIL, that
 * is rounding toward positive infinity, as its rounding mode.
 *
 * @override_id asc_int322float_ru__PUf32_PUi32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination float vector that receives the converted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int32_t vector to convert; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks, and the count form of this
 * API instead uses the framework default repeat value.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src_block_stride Source address stride between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source address stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_int322float_ru(
    __ubuf__ float* dst, __ubuf__ int32_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count int32_t elements of src to float, applies FLOOR, that is rounding toward negative
 * infinity, as its rounding mode.
 *
 * @override_id asc_int322float_rd__PUf32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination float vector that receives the converted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int32_t vector to convert; it must be 32-byte aligned.
 * @param [in] count Number of elements to convert, counted in int32_t elements of the source vector.
 */
__aicore__ inline void asc_int322float_rd(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int32_t elements to float over a configurable repeat and DataBlock stride pattern, applies FLOOR,
 * that is rounding toward negative infinity, as its rounding mode.
 *
 * @override_id asc_int322float_rd__PUf32_PUi32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination float vector that receives the converted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int32_t vector to convert; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks, and the count form of this
 * API instead uses the framework default repeat value.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src_block_stride Source address stride between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source address stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_int322float_rd(
    __ubuf__ float* dst, __ubuf__ int32_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count int32_t elements of src to float, applies RINT, that is round-half-to-even, as its
 * rounding mode.
 *
 * @override_id asc_int322float_rn__PUf32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination float vector that receives the converted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int32_t vector to convert; it must be 32-byte aligned.
 * @param [in] count Number of elements to convert, counted in int32_t elements of the source vector.
 */
__aicore__ inline void asc_int322float_rn(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int32_t elements to float over a configurable repeat and DataBlock stride pattern, applies RINT, that
 * is round-half-to-even, as its rounding mode.
 *
 * @override_id asc_int322float_rn__PUf32_PUi32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination float vector that receives the converted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int32_t vector to convert; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks, and the count form of this
 * API instead uses the framework default repeat value.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src_block_stride Source address stride between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source address stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_int322float_rn(
    __ubuf__ float* dst, __ubuf__ int32_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count int32_t elements of src to float, applies TRUNC, that is truncation toward zero, as
 * its rounding mode.
 *
 * @override_id asc_int322float_rz__PUf32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination float vector that receives the converted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int32_t vector to convert; it must be 32-byte aligned.
 * @param [in] count Number of elements to convert, counted in int32_t elements of the source vector.
 */
__aicore__ inline void asc_int322float_rz(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int32_t elements to float over a configurable repeat and DataBlock stride pattern, applies TRUNC,
 * that is truncation toward zero, as its rounding mode.
 *
 * @override_id asc_int322float_rz__PUf32_PUi32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination float vector that receives the converted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int32_t vector to convert; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks, and the count form of this
 * API instead uses the framework default repeat value.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src_block_stride Source address stride between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source address stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_int322float_rz(
    __ubuf__ float* dst, __ubuf__ int32_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int32_t elements of src to int16_t over repeat iterations using per-block and per-repeat strides.
 *
 * @override_id asc_int322int16__PUi16_PUi32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the narrowed int16_t values; must be 32-byte aligned.
 * @param [in] src Source vector start address of int32_t elements; must be 32-byte aligned.
 * @param [in] repeat Number of iterations of the conversion.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of the source.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations.
 */
__aicore__ inline void asc_int322int16(
    __ubuf__ int16_t* dst, __ubuf__ int32_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts count int32_t elements of src to int16_t and stores the results in dst.
 *
 * @override_id asc_int322int16__PUi16_PUi32_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the narrowed int16_t values; must be 32-byte aligned.
 * @param [in] src Source vector start address of int32_t elements; must be 32-byte aligned.
 * @param [in] count Number of elements participating in the contiguous conversion.
 */
__aicore__ inline void asc_int322int16(__ubuf__ int16_t* dst, __ubuf__ int32_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int32_t elements of src to int64_t over repeat iterations using per-block and per-repeat strides.
 *
 * @override_id asc_int322int64__PUi64_PUi32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the widened int64_t values; must be 32-byte aligned.
 * @param [in] src Source vector start address of int32_t elements; must be 32-byte aligned.
 * @param [in] repeat Number of iterations of the conversion.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of the source.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations.
 */
__aicore__ inline void asc_int322int64(
    __ubuf__ int64_t* dst, __ubuf__ int32_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts count int32_t elements of src to int64_t and stores the results in dst.
 *
 * @override_id asc_int322int64__PUi64_PUi32_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the widened int64_t values; must be 32-byte aligned.
 * @param [in] src Source vector start address of int32_t elements; must be 32-byte aligned.
 * @param [in] count Number of elements participating in the contiguous conversion.
 */
__aicore__ inline void asc_int322int64(__ubuf__ int64_t* dst, __ubuf__ int32_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int64_t elements of src to int32_t over repeat iterations using per-block and per-repeat strides.
 *
 * @override_id asc_int642int32__PUi32_PUi64_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the narrowed int32_t values; must be 32-byte aligned.
 * @param [in] src Source vector start address of int64_t elements; must be 32-byte aligned.
 * @param [in] repeat Number of iterations of the conversion.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of the source.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations.
 */
__aicore__ inline void asc_int642int32(
    __ubuf__ int32_t* dst, __ubuf__ int64_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts count int64_t elements of src to int32_t and stores the results in dst.
 *
 * @override_id asc_int642int32__PUi32_PUi64_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the narrowed int32_t values; must be 32-byte aligned.
 * @param [in] src Source vector start address of int64_t elements; must be 32-byte aligned.
 * @param [in] count Number of elements participating in the contiguous conversion.
 */
__aicore__ inline void asc_int642int32(__ubuf__ int32_t* dst, __ubuf__ int64_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to int16_t with ROUND rounding (half away from zero) in repeat iterations using
 * the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2int16_rna__PUi16_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int16_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2int16_rna(
    __ubuf__ int16_t* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count float elements of src to int16_t with ROUND rounding (half away from zero) and writes
 * the results to dst.
 *
 * @override_id asc_float2int16_rna__PUi16_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int16_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2int16_rna(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to int16_t with CEIL rounding (toward plus infinity) in repeat iterations using
 * the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2int16_ru__PUi16_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int16_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2int16_ru(
    __ubuf__ int16_t* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count float elements of src to int16_t with CEIL rounding (toward plus infinity) and writes
 * the results to dst.
 *
 * @override_id asc_float2int16_ru__PUi16_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int16_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2int16_ru(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to int16_t with FLOOR rounding (toward minus infinity) in repeat iterations
 * using the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2int16_rd__PUi16_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int16_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2int16_rd(
    __ubuf__ int16_t* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count float elements of src to int16_t with FLOOR rounding (toward minus infinity) and
 * writes the results to dst.
 *
 * @override_id asc_float2int16_rd__PUi16_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int16_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2int16_rd(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to int16_t with RINT rounding (round half to even) in repeat iterations using
 * the given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2int16_rn__PUi16_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int16_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2int16_rn(
    __ubuf__ int16_t* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count float elements of src to int16_t with RINT rounding (round half to even) and writes
 * the results to dst.
 *
 * @override_id asc_float2int16_rn__PUi16_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int16_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2int16_rn(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts float elements of src to int16_t with TRUNC rounding (toward zero) in repeat iterations using the
 * given DataBlock strides and writes the results to dst.
 *
 * @override_id asc_float2int16_rz__PUi16_PUf32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int16_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] repeat Number of iterations performed, each covering 8 DataBlocks of 32 bytes; the hardware limit is 255.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one destination iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Address stride between DataBlocks inside one source iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations, counted
 * in 32-byte DataBlocks.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations, counted in
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_float2int16_rz(
    __ubuf__ int16_t* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count float elements of src to int16_t with TRUNC rounding (toward zero) and writes the
 * results to dst.
 *
 * @override_id asc_float2int16_rz__PUi16_PUf32_u32
 * @archs 2201
 * @param [out] dst Starting UB address of the destination int16_t buffer that receives the converted elements; must be
 * 32-byte aligned.
 * @param [in] src Starting UB address of the source float buffer to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted, counted in elements from the start of src and dst.
 */
__aicore__ inline void asc_float2int16_rz(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int64_t elements from src to float with ROUND (round to nearest with ties away from zero) rounding
 * over repeat iterations using explicit block and repeat strides.
 *
 * @override_id asc_int642float_rna__PUf32_PUi64_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination UB start address of the converted float data; must be 32-byte aligned.
 * @param [in] src Source UB start address of the int64_t data; must be 32-byte aligned.
 * @param [in] repeat Number of iterations of the high-dimensional split conversion, each iteration processing one
 * vector-length group of elements.
 * @param [in] dst_block_stride Address stride between different DataBlocks inside one iteration of the destination
 * operand.
 * @param [in] src_block_stride Address stride between different DataBlocks inside one iteration of the source operand.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent iterations for the destination
 * operand.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent iterations for the source
 * operand.
 */
__aicore__ inline void asc_int642float_rna(
    __ubuf__ float* dst, __ubuf__ int64_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count int64_t elements from src to float with ROUND (round to nearest with ties away from
 * zero) rounding and writes them to dst.
 *
 * @override_id asc_int642float_rna__PUf32_PUi64_u32
 * @archs 2201
 * @param [out] dst Destination UB start address of the float results; must be 32-byte aligned.
 * @param [in] src Source UB start address of the int64_t elements to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted; it is loaded into the element mask by
 * asc_set_mask_count_begin before the vector instruction runs.
 */
__aicore__ inline void asc_int642float_rna(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int64_t elements from src to float with CEIL (round toward positive infinity) rounding over repeat
 * iterations using explicit block and repeat strides.
 *
 * @override_id asc_int642float_ru__PUf32_PUi64_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination UB start address of the converted float data; must be 32-byte aligned.
 * @param [in] src Source UB start address of the int64_t data; must be 32-byte aligned.
 * @param [in] repeat Number of iterations of the high-dimensional split conversion, each iteration processing one
 * vector-length group of elements.
 * @param [in] dst_block_stride Address stride between different DataBlocks inside one iteration of the destination
 * operand.
 * @param [in] src_block_stride Address stride between different DataBlocks inside one iteration of the source operand.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent iterations for the destination
 * operand.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent iterations for the source
 * operand.
 */
__aicore__ inline void asc_int642float_ru(
    __ubuf__ float* dst, __ubuf__ int64_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count int64_t elements from src to float with CEIL (round toward positive infinity)
 * rounding and writes them to dst.
 *
 * @override_id asc_int642float_ru__PUf32_PUi64_u32
 * @archs 2201
 * @param [out] dst Destination UB start address of the float results; must be 32-byte aligned.
 * @param [in] src Source UB start address of the int64_t elements to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted; it is loaded into the element mask by
 * asc_set_mask_count_begin before the vector instruction runs.
 */
__aicore__ inline void asc_int642float_ru(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int64_t elements from src to float with FLOOR (round toward negative infinity) rounding over repeat
 * iterations using explicit block and repeat strides.
 *
 * @override_id asc_int642float_rd__PUf32_PUi64_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination UB start address of the converted float data; must be 32-byte aligned.
 * @param [in] src Source UB start address of the int64_t data; must be 32-byte aligned.
 * @param [in] repeat Number of iterations of the high-dimensional split conversion, each iteration processing one
 * vector-length group of elements.
 * @param [in] dst_block_stride Address stride between different DataBlocks inside one iteration of the destination
 * operand.
 * @param [in] src_block_stride Address stride between different DataBlocks inside one iteration of the source operand.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent iterations for the destination
 * operand.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent iterations for the source
 * operand.
 */
__aicore__ inline void asc_int642float_rd(
    __ubuf__ float* dst, __ubuf__ int64_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count int64_t elements from src to float with FLOOR (round toward negative infinity)
 * rounding and writes them to dst.
 *
 * @override_id asc_int642float_rd__PUf32_PUi64_u32
 * @archs 2201
 * @param [out] dst Destination UB start address of the float results; must be 32-byte aligned.
 * @param [in] src Source UB start address of the int64_t elements to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted; it is loaded into the element mask by
 * asc_set_mask_count_begin before the vector instruction runs.
 */
__aicore__ inline void asc_int642float_rd(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int64_t elements from src to float with RINT (round to nearest with ties to even) rounding over
 * repeat iterations using explicit block and repeat strides.
 *
 * @override_id asc_int642float_rn__PUf32_PUi64_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination UB start address of the converted float data; must be 32-byte aligned.
 * @param [in] src Source UB start address of the int64_t data; must be 32-byte aligned.
 * @param [in] repeat Number of iterations of the high-dimensional split conversion, each iteration processing one
 * vector-length group of elements.
 * @param [in] dst_block_stride Address stride between different DataBlocks inside one iteration of the destination
 * operand.
 * @param [in] src_block_stride Address stride between different DataBlocks inside one iteration of the source operand.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent iterations for the destination
 * operand.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent iterations for the source
 * operand.
 */
__aicore__ inline void asc_int642float_rn(
    __ubuf__ float* dst, __ubuf__ int64_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count int64_t elements from src to float with RINT (round to nearest with ties to even)
 * rounding and writes them to dst.
 *
 * @override_id asc_int642float_rn__PUf32_PUi64_u32
 * @archs 2201
 * @param [out] dst Destination UB start address of the float results; must be 32-byte aligned.
 * @param [in] src Source UB start address of the int64_t elements to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted; it is loaded into the element mask by
 * asc_set_mask_count_begin before the vector instruction runs.
 */
__aicore__ inline void asc_int642float_rn(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int64_t elements from src to float with TRUNC (round toward zero) rounding over repeat iterations
 * using explicit block and repeat strides.
 *
 * @override_id asc_int642float_rz__PUf32_PUi64_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination UB start address of the converted float data; must be 32-byte aligned.
 * @param [in] src Source UB start address of the int64_t data; must be 32-byte aligned.
 * @param [in] repeat Number of iterations of the high-dimensional split conversion, each iteration processing one
 * vector-length group of elements.
 * @param [in] dst_block_stride Address stride between different DataBlocks inside one iteration of the destination
 * operand.
 * @param [in] src_block_stride Address stride between different DataBlocks inside one iteration of the source operand.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent iterations for the destination
 * operand.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent iterations for the source
 * operand.
 */
__aicore__ inline void asc_int642float_rz(
    __ubuf__ float* dst, __ubuf__ int64_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count int64_t elements from src to float with TRUNC (round toward zero) rounding and writes
 * them to dst.
 *
 * @override_id asc_int642float_rz__PUf32_PUi64_u32
 * @archs 2201
 * @param [out] dst Destination UB start address of the float results; must be 32-byte aligned.
 * @param [in] src Source UB start address of the int64_t elements to convert; must be 32-byte aligned.
 * @param [in] count Number of consecutive elements converted; it is loaded into the element mask by
 * asc_set_mask_count_begin before the vector instruction runs.
 */
__aicore__ inline void asc_int642float_rz(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int16_t elements to half over a configurable repeat and DataBlock stride pattern, uses RINT rounding
 * when the conversion loses precision and performs no rounding otherwise, as noted in the prototype comment.
 *
 * @override_id asc_int162half__PUf16_PUi16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination half vector that receives the converted elements;
 * it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int16_t vector to convert; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks, and the count form of this
 * API instead uses the framework default repeat value.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src_block_stride Source address stride between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source address stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_int162half(
    __ubuf__ half* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count int16_t elements of src to half, uses RINT rounding when the conversion loses
 * precision and performs no rounding otherwise, as noted in the prototype comment.
 *
 * @override_id asc_int162half__PUf16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination half vector that receives the converted elements;
 * it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int16_t vector to convert; it must be 32-byte aligned.
 * @param [in] count Number of elements to convert, counted in int16_t elements of the source vector.
 */
__aicore__ inline void asc_int162half(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int16_t elements to half over a configurable repeat and DataBlock stride pattern, applies ROUND, that
 * is round-half-away-from-zero, as its rounding mode.
 *
 * @override_id asc_int162half_rna__PUf16_PUi16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination half vector that receives the converted elements;
 * it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int16_t vector to convert; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks, and the count form of this
 * API instead uses the framework default repeat value.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src_block_stride Source address stride between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source address stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_int162half_rna(
    __ubuf__ half* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count int16_t elements of src to half, applies ROUND, that is round-half-away-from-zero, as
 * its rounding mode.
 *
 * @override_id asc_int162half_rna__PUf16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination half vector that receives the converted elements;
 * it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int16_t vector to convert; it must be 32-byte aligned.
 * @param [in] count Number of elements to convert, counted in int16_t elements of the source vector.
 */
__aicore__ inline void asc_int162half_rna(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int16_t elements to half over a configurable repeat and DataBlock stride pattern, applies CEIL, that
 * is rounding toward positive infinity, as its rounding mode.
 *
 * @override_id asc_int162half_ru__PUf16_PUi16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination half vector that receives the converted elements;
 * it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int16_t vector to convert; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks, and the count form of this
 * API instead uses the framework default repeat value.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src_block_stride Source address stride between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source address stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_int162half_ru(
    __ubuf__ half* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count int16_t elements of src to half, applies CEIL, that is rounding toward positive
 * infinity, as its rounding mode.
 *
 * @override_id asc_int162half_ru__PUf16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination half vector that receives the converted elements;
 * it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int16_t vector to convert; it must be 32-byte aligned.
 * @param [in] count Number of elements to convert, counted in int16_t elements of the source vector.
 */
__aicore__ inline void asc_int162half_ru(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int16_t elements to half over a configurable repeat and DataBlock stride pattern, applies FLOOR, that
 * is rounding toward negative infinity, as its rounding mode.
 *
 * @override_id asc_int162half_rd__PUf16_PUi16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination half vector that receives the converted elements;
 * it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int16_t vector to convert; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks, and the count form of this
 * API instead uses the framework default repeat value.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src_block_stride Source address stride between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source address stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_int162half_rd(
    __ubuf__ half* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count int16_t elements of src to half, applies FLOOR, that is rounding toward negative
 * infinity, as its rounding mode.
 *
 * @override_id asc_int162half_rd__PUf16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination half vector that receives the converted elements;
 * it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int16_t vector to convert; it must be 32-byte aligned.
 * @param [in] count Number of elements to convert, counted in int16_t elements of the source vector.
 */
__aicore__ inline void asc_int162half_rd(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int16_t elements to half over a configurable repeat and DataBlock stride pattern, applies RINT, that
 * is round-half-to-even, as its rounding mode.
 *
 * @override_id asc_int162half_rn__PUf16_PUi16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination half vector that receives the converted elements;
 * it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int16_t vector to convert; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks, and the count form of this
 * API instead uses the framework default repeat value.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src_block_stride Source address stride between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source address stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_int162half_rn(
    __ubuf__ half* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count int16_t elements of src to half, applies RINT, that is round-half-to-even, as its
 * rounding mode.
 *
 * @override_id asc_int162half_rn__PUf16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination half vector that receives the converted elements;
 * it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int16_t vector to convert; it must be 32-byte aligned.
 * @param [in] count Number of elements to convert, counted in int16_t elements of the source vector.
 */
__aicore__ inline void asc_int162half_rn(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts int16_t elements to half over a configurable repeat and DataBlock stride pattern, applies TRUNC, that
 * is truncation toward zero, as its rounding mode.
 *
 * @override_id asc_int162half_rz__PUf16_PUi16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination half vector that receives the converted elements;
 * it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int16_t vector to convert; it must be 32-byte aligned.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks, and the count form of this
 * API instead uses the framework default repeat value.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src_block_stride Source address stride between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source address stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_int162half_rz(
    __ubuf__ half* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count int16_t elements of src to half, applies TRUNC, that is truncation toward zero, as
 * its rounding mode.
 *
 * @override_id asc_int162half_rz__PUf16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination half vector that receives the converted elements;
 * it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int16_t vector to convert; it must be 32-byte aligned.
 * @param [in] count Number of elements to convert, counted in int16_t elements of the source vector.
 */
__aicore__ inline void asc_int162half_rz(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts up to repeat iterations of half data into uint8_t values with per-DataBlock and per-iteration
 * strides, rounding only inexact values.
 *
 * @override_id asc_half2uint8__PUu8_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer for uint8_t results; must be 32-byte aligned and is
 * written.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] repeat Iteration count of the instruction as uint8_t, bounded by the documented hardware limit of 255
 * iterations.
 * @param [in] dst_block_stride Destination stride between DataBlocks within one iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Source stride between DataBlocks within one iteration, counted in 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination stride between the same DataBlock of adjacent iterations, counted in
 * 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source stride between the same DataBlock of adjacent iterations, counted in 32-byte
 * DataBlocks.
 */
__aicore__ inline void asc_half2uint8(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of a UB buffer into uint8_t values, rounding to nearest even only when
 * the conversion is inexact.
 *
 * @override_id asc_half2uint8__PUu8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer receiving converted uint8_t elements; must be 32-byte
 * aligned and is written by the conversion.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] count Number of elements converted from the first position, applied as the mask count of the underlying
 * instruction.
 */
__aicore__ inline void asc_half2uint8(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts up to repeat iterations of half data into uint8_t values with per-DataBlock and per-iteration strides
 * using ROUND rounding away from zero.
 *
 * @override_id asc_half2uint8_rna__PUu8_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer receiving uint8_t results; must be 32-byte aligned and is
 * written.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] repeat Number of instruction iterations, typed uint8_t and bounded by the documented maximum of 255.
 * @param [in] dst_block_stride Destination stride between DataBlocks inside one iteration, in units of 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Source stride between DataBlocks inside one iteration, in units of 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source stride between the same DataBlock of adjacent iterations, in units of 32-byte
 * DataBlocks.
 */
__aicore__ inline void asc_half2uint8_rna(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of a UB buffer into uint8_t values using ROUND rounding with ties away
 * from zero.
 *
 * @override_id asc_half2uint8_rna__PUu8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer for uint8_t results; 32-byte aligned and written by the
 * conversion.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] count Number of elements converted from the start of the buffer, consumed as the instruction mask count.
 */
__aicore__ inline void asc_half2uint8_rna(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts up to repeat iterations of half data into uint8_t values with per-DataBlock and per-iteration strides
 * using CEIL rounding.
 *
 * @override_id asc_half2uint8_ru__PUu8_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer for uint8_t results; requires 32-byte alignment and is
 * written.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] repeat Iteration count of the instruction as uint8_t, bounded by the documented 255-iteration limit.
 * @param [in] dst_block_stride Destination stride between DataBlocks within one iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Source stride between DataBlocks within one iteration, counted in 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination stride between the same DataBlock of adjacent iterations, counted in
 * 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source stride between the same DataBlock of adjacent iterations, counted in 32-byte
 * DataBlocks.
 */
__aicore__ inline void asc_half2uint8_ru(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of a UB buffer into uint8_t values using CEIL rounding toward positive
 * infinity.
 *
 * @override_id asc_half2uint8_ru__PUu8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer receiving uint8_t results; requires 32-byte alignment and
 * is written.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] count Number of elements converted from the first position, applied as the mask count of the underlying
 * instruction.
 */
__aicore__ inline void asc_half2uint8_ru(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts up to repeat iterations of half data into uint8_t values with per-DataBlock and per-iteration strides
 * using FLOOR rounding.
 *
 * @override_id asc_half2uint8_rd__PUu8_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer for uint8_t results; requires 32-byte alignment and is
 * written.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] repeat Iteration count of the instruction, typed uint8_t and capped by the documented 255-iteration
 * limit.
 * @param [in] dst_block_stride Destination stride between DataBlocks within one iteration, counted in 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Source stride between DataBlocks within one iteration, counted in 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination stride between the same DataBlock of adjacent iterations, counted in
 * 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source stride between the same DataBlock of adjacent iterations, counted in 32-byte
 * DataBlocks.
 */
__aicore__ inline void asc_half2uint8_rd(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of a UB buffer into uint8_t values using FLOOR rounding toward negative
 * infinity.
 *
 * @override_id asc_half2uint8_rd__PUu8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer receiving uint8_t results; requires 32-byte alignment and
 * is written.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] count Number of elements converted from the beginning of the buffer, applied as the instruction mask
 * count.
 */
__aicore__ inline void asc_half2uint8_rd(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts up to repeat iterations of half data into uint8_t values with per-DataBlock and per-iteration strides
 * using RINT rounding to nearest even.
 *
 * @override_id asc_half2uint8_rn__PUu8_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer receiving uint8_t results; 32-byte aligned and written by
 * the conversion.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] repeat Iteration count of the instruction as uint8_t, limited by the documented maximum of 255
 * iterations.
 * @param [in] dst_block_stride Destination stride between DataBlocks inside one iteration, in units of 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Source stride between DataBlocks inside one iteration, in units of 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source stride between the same DataBlock of adjacent iterations, in units of 32-byte
 * DataBlocks.
 */
__aicore__ inline void asc_half2uint8_rn(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of a UB buffer into uint8_t values using RINT rounding to nearest even.
 *
 * @override_id asc_half2uint8_rn__PUu8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer for uint8_t results; 32-byte aligned and written by the
 * conversion.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] count Number of elements converted from the first position, used as the instruction mask count.
 */
__aicore__ inline void asc_half2uint8_rn(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts up to repeat iterations of half data into uint8_t values with per-DataBlock and per-iteration strides
 * using TRUNC rounding toward zero.
 *
 * @override_id asc_half2uint8_rz__PUu8_PUf16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer receiving uint8_t results; 32-byte aligned and written by
 * the conversion.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] repeat Iteration count of the instruction as uint8_t, limited by the documented hardware maximum of 255.
 * @param [in] dst_block_stride Destination stride between DataBlocks inside one iteration, in units of 32-byte
 * DataBlocks.
 * @param [in] src_block_stride Source stride between DataBlocks inside one iteration, in units of 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source stride between the same DataBlock of adjacent iterations, in units of 32-byte
 * DataBlocks.
 */
__aicore__ inline void asc_half2uint8_rz(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_convert
 * @brief Converts the first count half elements of a UB buffer into uint8_t values using TRUNC rounding toward zero.
 *
 * @override_id asc_half2uint8_rz__PUu8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination UB buffer for uint8_t results; 32-byte aligned and written by the
 * conversion.
 * @param [in] src Start address of the source UB buffer of half elements; 32-byte aligned and read only.
 * @param [in] count Number of elements converted from the start of the buffer, used as the instruction mask count.
 */
__aicore__ inline void asc_half2uint8_rz(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_CONVERT_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_CONVERT_H
#endif
