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
#error "vector_compute/compute/vector_reduce.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_REDUCE_H
#endif

#ifndef INCLUDE_C_API_VECTOR_COMPUTE_COMPUTE_VECTOR_REDUCE_H
#define INCLUDE_C_API_VECTOR_COMPUTE_COMPUTE_VECTOR_REDUCE_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/memory_base_impl/vector_reduce_intf_impl.h"
#endif

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Gathers elements of src0 into dst under either a built-in gather mask or the user-defined mask src1, iterating
 * repeat times.
 *
 * @override_id asc_reduce__PUu16_PUu16_PUu16_u16_u8_u8_u16_u8
 * @archs 2201
 * @param [out] dst Start address of the destination uint16_t vector operand; must be 32-byte aligned.
 * @param [in] src0 Start address of the source uint16_t vector operand whose elements are gathered; must be 32-byte
 * aligned.
 * @param [in] src1 Start address of the uint16_t user-defined gather mask buffer, used when pattern_mode selects the
 * user-defined mode; must be 32-byte aligned.
 * @param [in] repeat Number of iterations executed by the instruction.
 * @param [in] src0_block_stride Address stride between DataBlocks within one iteration of src0.
 * @param [in] pattern_mode Gather mask selector: 1 to 7 choose the built-in masks, 0 selects the user-defined mask
 * supplied through src1.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of src0 in adjacent iterations.
 * @param [in] src1_repeat_stride Address stride between adjacent iterations of src1 in user-defined mode.
 */
__aicore__ inline void asc_reduce(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1, uint16_t repeat,
    uint8_t src0_block_stride, uint8_t pattern_mode, uint16_t src0_repeat_stride, uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Gathers elements of src0 selected by the gather mask in src1 into dst for count elements, using the flat count
 * form.
 *
 * @override_id asc_reduce__PUu16_PUu16_PUu16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination uint16_t vector operand; must be 32-byte aligned.
 * @param [in] src0 Start address of the source uint16_t vector operand whose elements are gathered; must be 32-byte
 * aligned.
 * @param [in] src1 Start address of the uint16_t user-defined gather mask buffer, read because the flat form runs in
 * user-defined mode; must be 32-byte aligned.
 * @param [in] count Number of elements to process; the impl turns it into a count-based mask and calls the
 * high-dimension form with pattern_mode 0.
 */
__aicore__ inline void asc_reduce(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Gathers elements of src0 into dst under either a built-in gather mask or the user-defined mask src1, iterating
 * repeat times.
 *
 * @override_id asc_reduce__PUu32_PUu32_PUu32_u16_u8_u8_u16_u8
 * @archs 2201
 * @param [out] dst Start address of the destination uint32_t vector operand; must be 32-byte aligned.
 * @param [in] src0 Start address of the source uint32_t vector operand whose elements are gathered; must be 32-byte
 * aligned.
 * @param [in] src1 Start address of the uint32_t user-defined gather mask buffer, used when pattern_mode selects the
 * user-defined mode; must be 32-byte aligned.
 * @param [in] repeat Number of iterations executed by the instruction.
 * @param [in] src0_block_stride Address stride between DataBlocks within one iteration of src0.
 * @param [in] pattern_mode Gather mask selector: 1 to 7 choose the built-in masks, 0 selects the user-defined mask
 * supplied through src1.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of src0 in adjacent iterations.
 * @param [in] src1_repeat_stride Address stride between adjacent iterations of src1 in user-defined mode.
 */
__aicore__ inline void asc_reduce(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src0, __ubuf__ uint32_t* src1, uint16_t repeat,
    uint8_t src0_block_stride, uint8_t pattern_mode, uint16_t src0_repeat_stride, uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Gathers elements of src0 selected by the gather mask in src1 into dst for count elements, using the flat count
 * form.
 *
 * @override_id asc_reduce__PUu32_PUu32_PUu32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination uint32_t vector operand; must be 32-byte aligned.
 * @param [in] src0 Start address of the source uint32_t vector operand whose elements are gathered; must be 32-byte
 * aligned.
 * @param [in] src1 Start address of the uint32_t user-defined gather mask buffer, read because the flat form runs in
 * user-defined mode; must be 32-byte aligned.
 * @param [in] count Number of elements to process; the impl turns it into a count-based mask and calls the
 * high-dimension form with pattern_mode 0.
 */
__aicore__ inline void asc_reduce(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src0, __ubuf__ uint32_t* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reads back the maximum value and the index of its first occurrence produced by a preceding
 * asc_repeat_reduce_max operation on half data.
 *
 * @override_id asc_get_reduce_max_cnt__Rf16_Ru32
 * @archs 2201
 * @param [out] val Reference receiving the maximum value of the preceding reduction.
 * @param [out] index Reference receiving the index of the first occurrence of the maximum value.
 */
__aicore__ inline void asc_get_reduce_max_cnt(half& val, uint32_t& index);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reads back the maximum value and the index of its first occurrence produced by a preceding
 * asc_repeat_reduce_max operation on float data.
 *
 * @override_id asc_get_reduce_max_cnt__Rf32_Ru32
 * @archs 2201
 * @param [out] val Reference receiving the maximum value of the preceding reduction.
 * @param [out] index Reference receiving the index of the first occurrence of the maximum value.
 */
__aicore__ inline void asc_get_reduce_max_cnt(float& val, uint32_t& index);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reads back the minimum value and the index of its first occurrence produced by a preceding
 * asc_repeat_reduce_min operation on half data.
 *
 * @override_id asc_get_reduce_min_cnt__Rf16_Ru32
 * @archs 2201
 * @param [out] val Reference receiving the minimum value of the preceding reduction.
 * @param [out] index Reference receiving the index of the first occurrence of the minimum value.
 */
__aicore__ inline void asc_get_reduce_min_cnt(half& val, uint32_t& index);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reads back the minimum value and the index of its first occurrence produced by a preceding
 * asc_repeat_reduce_min operation on float data.
 *
 * @override_id asc_get_reduce_min_cnt__Rf32_Ru32
 * @archs 2201
 * @param [out] val Reference receiving the minimum value of the preceding reduction.
 * @param [out] index Reference receiving the index of the first occurrence of the minimum value.
 */
__aicore__ inline void asc_get_reduce_min_cnt(float& val, uint32_t& index);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Returns the number of elements left over after an asc_reduce operation.
 *
 * @override_id asc_get_rsvd_count__z
 * @archs 2201
 */
__aicore__ inline int64_t asc_get_rsvd_count();

__aicore__ inline int64_t asc_get_acc_val();

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Computes the per-DataBlock summation reduction over the half elements addressed by src, producing one result
 * per DataBlock per iteration into dst using the repeat and DataBlock stride pattern.
 *
 * @override_id asc_datablock_reduce_sum__PUf16_PUf16_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the reduced half results; the reduction writes here and dst is
 * never read as an operand.
 * @param [in] src Start address in Unified Buffer of the half source vector that is reduced; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; each iteration reduces 8 DataBlocks of 32 Byte, so
 * repeat must not exceed the hardware limit of 255.
 * @param [in] dst_repeat_stride Address distance between the same destination DataBlock of adjacent iterations,
 * measured in 16 Byte units when the input type is 16-bit and in 32 Byte units when it is 32-bit (so it is 16 Byte per
 * unit for half); declared uint16_t here.
 * @param [in] src_block_stride Address distance between consecutive DataBlocks inside one iteration for src, counted in
 * DataBlocks of 32 Byte (declared uint16_t here).
 * @param [in] src_repeat_stride Address distance between the same source DataBlock of adjacent iterations, counted in
 * DataBlocks of 32 Byte (declared uint16_t here).
 */
__aicore__ inline void asc_datablock_reduce_sum(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Computes the per-DataBlock summation reduction over the first count half elements addressed by src and writes
 * the reduced results into dst.
 *
 * @override_id asc_datablock_reduce_sum__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the reduced half results; the reduction writes here and dst is
 * never read as an operand.
 * @param [in] src Start address in Unified Buffer of the half source vector that is reduced; read only.
 * @param [in] count Number of leading half elements to reduce, counted in elements rather than bytes; the impl turns it
 * into a mask count.
 */
__aicore__ inline void asc_datablock_reduce_sum(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Computes the per-DataBlock summation reduction over the float elements addressed by src, producing one result
 * per DataBlock per iteration into dst using the repeat and DataBlock stride pattern.
 *
 * @override_id asc_datablock_reduce_sum__PUf32_PUf32_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the reduced float results; the reduction writes here and dst is
 * never read as an operand.
 * @param [in] src Start address in Unified Buffer of the float source vector that is reduced; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; each iteration reduces 8 DataBlocks of 32 Byte, so
 * repeat must not exceed the hardware limit of 255.
 * @param [in] dst_repeat_stride Address distance between the same destination DataBlock of adjacent iterations,
 * measured in 16 Byte units when the input type is 16-bit and in 32 Byte units when it is 32-bit (so it is 32 Byte per
 * unit for float); declared uint16_t here.
 * @param [in] src_block_stride Address distance between consecutive DataBlocks inside one iteration for src, counted in
 * DataBlocks of 32 Byte (declared uint16_t here).
 * @param [in] src_repeat_stride Address distance between the same source DataBlock of adjacent iterations, counted in
 * DataBlocks of 32 Byte (declared uint16_t here).
 */
__aicore__ inline void asc_datablock_reduce_sum(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Computes the per-DataBlock summation reduction over the first count float elements addressed by src and writes
 * the reduced results into dst.
 *
 * @override_id asc_datablock_reduce_sum__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the reduced float results; the reduction writes here and dst is
 * never read as an operand.
 * @param [in] src Start address in Unified Buffer of the float source vector that is reduced; read only.
 * @param [in] count Number of leading float elements to reduce, counted in elements rather than bytes; the impl turns
 * it into a mask count.
 */
__aicore__ inline void asc_datablock_reduce_sum(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Computes the per-DataBlock maximum reduction over the half elements addressed by src, producing one result per
 * DataBlock per iteration into dst using the repeat and DataBlock stride pattern.
 *
 * @override_id asc_datablock_reduce_max__PUf16_PUf16_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the reduced half results; the reduction writes here and dst is
 * never read as an operand.
 * @param [in] src Start address in Unified Buffer of the half source vector that is reduced; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; each iteration reduces 8 DataBlocks of 32 Byte, so
 * repeat must not exceed the hardware limit of 255.
 * @param [in] dst_repeat_stride Address distance between the same destination DataBlock of adjacent iterations,
 * measured in 16 Byte units when the input type is 16-bit and in 32 Byte units when it is 32-bit (so it is 16 Byte per
 * unit for half); declared uint16_t here.
 * @param [in] src_block_stride Address distance between consecutive DataBlocks inside one iteration for src, counted in
 * DataBlocks of 32 Byte (declared uint16_t here).
 * @param [in] src_repeat_stride Address distance between the same source DataBlock of adjacent iterations, counted in
 * DataBlocks of 32 Byte (declared uint16_t here).
 */
__aicore__ inline void asc_datablock_reduce_max(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Computes the per-DataBlock maximum reduction over the first count half elements addressed by src and writes
 * the reduced results into dst.
 *
 * @override_id asc_datablock_reduce_max__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the reduced half results; the reduction writes here and dst is
 * never read as an operand.
 * @param [in] src Start address in Unified Buffer of the half source vector that is reduced; read only.
 * @param [in] count Number of leading half elements to reduce, counted in elements rather than bytes; the impl turns it
 * into a mask count.
 */
__aicore__ inline void asc_datablock_reduce_max(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Computes the per-DataBlock maximum reduction over the float elements addressed by src, producing one result
 * per DataBlock per iteration into dst using the repeat and DataBlock stride pattern.
 *
 * @override_id asc_datablock_reduce_max__PUf32_PUf32_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the reduced float results; the reduction writes here and dst is
 * never read as an operand.
 * @param [in] src Start address in Unified Buffer of the float source vector that is reduced; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; each iteration reduces 8 DataBlocks of 32 Byte, so
 * repeat must not exceed the hardware limit of 255.
 * @param [in] dst_repeat_stride Address distance between the same destination DataBlock of adjacent iterations,
 * measured in 16 Byte units when the input type is 16-bit and in 32 Byte units when it is 32-bit (so it is 32 Byte per
 * unit for float); declared uint16_t here.
 * @param [in] src_block_stride Address distance between consecutive DataBlocks inside one iteration for src, counted in
 * DataBlocks of 32 Byte (declared uint16_t here).
 * @param [in] src_repeat_stride Address distance between the same source DataBlock of adjacent iterations, counted in
 * DataBlocks of 32 Byte (declared uint16_t here).
 */
__aicore__ inline void asc_datablock_reduce_max(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Computes the per-DataBlock maximum reduction over the first count float elements addressed by src and writes
 * the reduced results into dst.
 *
 * @override_id asc_datablock_reduce_max__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the reduced float results; the reduction writes here and dst is
 * never read as an operand.
 * @param [in] src Start address in Unified Buffer of the float source vector that is reduced; read only.
 * @param [in] count Number of leading float elements to reduce, counted in elements rather than bytes; the impl turns
 * it into a mask count.
 */
__aicore__ inline void asc_datablock_reduce_max(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Computes the per-DataBlock minimum reduction over the half elements addressed by src, producing one result per
 * DataBlock per iteration into dst using the repeat and DataBlock stride pattern.
 *
 * @override_id asc_datablock_reduce_min__PUf16_PUf16_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the reduced half results; the reduction writes here and dst is
 * never read as an operand.
 * @param [in] src Start address in Unified Buffer of the half source vector that is reduced; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; each iteration reduces 8 DataBlocks of 32 Byte, so
 * repeat must not exceed the hardware limit of 255.
 * @param [in] dst_repeat_stride Address distance between the same destination DataBlock of adjacent iterations,
 * measured in 16 Byte units when the input type is 16-bit and in 32 Byte units when it is 32-bit (so it is 16 Byte per
 * unit for half); declared uint16_t here.
 * @param [in] src_block_stride Address distance between consecutive DataBlocks inside one iteration for src, counted in
 * DataBlocks of 32 Byte (declared uint16_t here).
 * @param [in] src_repeat_stride Address distance between the same source DataBlock of adjacent iterations, counted in
 * DataBlocks of 32 Byte (declared uint16_t here).
 */
__aicore__ inline void asc_datablock_reduce_min(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Computes the per-DataBlock minimum reduction over the first count half elements addressed by src and writes
 * the reduced results into dst.
 *
 * @override_id asc_datablock_reduce_min__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the reduced half results; the reduction writes here and dst is
 * never read as an operand.
 * @param [in] src Start address in Unified Buffer of the half source vector that is reduced; read only.
 * @param [in] count Number of leading half elements to reduce, counted in elements rather than bytes; the impl turns it
 * into a mask count.
 */
__aicore__ inline void asc_datablock_reduce_min(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Computes the per-DataBlock minimum reduction over the float elements addressed by src, producing one result
 * per DataBlock per iteration into dst using the repeat and DataBlock stride pattern.
 *
 * @override_id asc_datablock_reduce_min__PUf32_PUf32_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the reduced float results; the reduction writes here and dst is
 * never read as an operand.
 * @param [in] src Start address in Unified Buffer of the float source vector that is reduced; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; each iteration reduces 8 DataBlocks of 32 Byte, so
 * repeat must not exceed the hardware limit of 255.
 * @param [in] dst_repeat_stride Address distance between the same destination DataBlock of adjacent iterations,
 * measured in 16 Byte units when the input type is 16-bit and in 32 Byte units when it is 32-bit (so it is 32 Byte per
 * unit for float); declared uint16_t here.
 * @param [in] src_block_stride Address distance between consecutive DataBlocks inside one iteration for src, counted in
 * DataBlocks of 32 Byte (declared uint16_t here).
 * @param [in] src_repeat_stride Address distance between the same source DataBlock of adjacent iterations, counted in
 * DataBlocks of 32 Byte (declared uint16_t here).
 */
__aicore__ inline void asc_datablock_reduce_min(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Computes the per-DataBlock minimum reduction over the first count float elements addressed by src and writes
 * the reduced results into dst.
 *
 * @override_id asc_datablock_reduce_min__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the reduced float results; the reduction writes here and dst is
 * never read as an operand.
 * @param [in] src Start address in Unified Buffer of the float source vector that is reduced; read only.
 * @param [in] count Number of leading float elements to reduce, counted in elements rather than bytes; the impl turns
 * it into a mask count.
 */
__aicore__ inline void asc_datablock_reduce_min(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Sums all elements inside each repeat of src in a binary tree and writes one result per repeat to dst over
 * repeat iterations.
 *
 * @override_id asc_repeat_reduce_sum__PUf16_PUf16_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand receiving one sum per repeat; 2-byte aligned for this type.
 * @param [in] src Start address of the source half vector operand; must be 32-byte aligned.
 * @param [in] repeat Number of iterations, each producing one summed result.
 * @param [in] dst_repeat_stride Address stride between adjacent dst iterations, measured in one per-repeat result
 * length (2 bytes for this type).
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of src.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of src in adjacent iterations.
 */
__aicore__ inline void asc_repeat_reduce_sum(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Sums all elements inside each repeat of src in a binary tree and writes one result per repeat to dst,
 * processing count elements in the flat form.
 *
 * @override_id asc_repeat_reduce_sum__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination operand receiving one sum per repeat; 2-byte aligned for this type.
 * @param [in] src Start address of the source half vector operand; must be 32-byte aligned.
 * @param [in] count Number of elements participating in the summation; the impl converts it into a count-based mask.
 */
__aicore__ inline void asc_repeat_reduce_sum(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Sums all elements inside each repeat of src in a binary tree and writes one result per repeat to dst over
 * repeat iterations.
 *
 * @override_id asc_repeat_reduce_sum__PUf32_PUf32_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination operand receiving one sum per repeat; 4-byte aligned for this type.
 * @param [in] src Start address of the source float vector operand; must be 32-byte aligned.
 * @param [in] repeat Number of iterations, each producing one summed result.
 * @param [in] dst_repeat_stride Address stride between adjacent dst iterations, measured in one per-repeat result
 * length (4 bytes for this type).
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of src.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of src in adjacent iterations.
 */
__aicore__ inline void asc_repeat_reduce_sum(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Sums all elements inside each repeat of src in a binary tree and writes one result per repeat to dst,
 * processing count elements in the flat form.
 *
 * @override_id asc_repeat_reduce_sum__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination operand receiving one sum per repeat; 4-byte aligned for this type.
 * @param [in] src Start address of the source float vector operand; must be 32-byte aligned.
 * @param [in] count Number of elements participating in the summation; the impl converts it into a count-based mask.
 */
__aicore__ inline void asc_repeat_reduce_sum(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces half elements from src over repeat iterations (one 256-byte Repeat each) to the first maximum of every
 * Repeat and writes the per-Repeat [index, value] pair (index first) to dst.
 *
 * @override_id asc_repeat_reduce_max_index_value__PUf16_PUf16_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [index, value] pair output, one entry per Repeat; the
 * page aligns it to 4 bytes for half input and notes the stored index must be read back with the index type (uint16),
 * not the value type.
 * @param [in] src Source UB start address of the half elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (128 elements).
 * @param [in] repeat Number of iterations; each iteration reduces one 256-byte Repeat (128 half elements) and emits one
 * index-then-value pair at dst.
 * @param [in] dst_repeat_stride Address stride between consecutive per-Repeat outputs, in units of sizeof(half) * 2 = 4
 * bytes as specified by the parameter table of the page.
 * @param [in] src_block_stride Address stride between adjacent DataBlocks inside one source iteration; the default unit
 * is one DataBlock (32 bytes) per the general vector-compute constraints.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations; the default
 * unit is one DataBlock (32 bytes), consistent with the doc example that uses 8 for a 256-byte Repeat of half elements.
 */
__aicore__ inline void asc_repeat_reduce_max_index_value(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces the first count half elements of src Repeat by Repeat (256 bytes per Repeat) to the first maximum of
 * each Repeat and writes the per-Repeat [index, value] pair (index first) to dst.
 *
 * @override_id asc_repeat_reduce_max_index_value__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [index, value] pair output, one entry per Repeat; the
 * page aligns it to 4 bytes for half input and notes the stored index must be read back with the index type (uint16),
 * not the value type.
 * @param [in] src Source UB start address of the half elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (128 elements).
 * @param [in] count Number of half elements of src participating in the reduction; the count overload programs the
 * element mask internally through asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_repeat_reduce_max_index_value(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces float elements from src over repeat iterations (one 256-byte Repeat each) to the first maximum of
 * every Repeat and writes the per-Repeat [index, value] pair (index first) to dst.
 *
 * @override_id asc_repeat_reduce_max_index_value__PUf32_PUf32_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [index, value] pair output, one entry per Repeat; the
 * page aligns it to 8 bytes for float input and notes the stored index must be read back with the index type (uint32),
 * not the value type.
 * @param [in] src Source UB start address of the float elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (64 elements).
 * @param [in] repeat Number of iterations; each iteration reduces one 256-byte Repeat (64 float elements) and emits one
 * index-then-value pair at dst.
 * @param [in] dst_repeat_stride Address stride between consecutive per-Repeat outputs, in units of sizeof(float) * 2 =
 * 8 bytes as specified by the parameter table of the page.
 * @param [in] src_block_stride Address stride between adjacent DataBlocks inside one source iteration; the default unit
 * is one DataBlock (32 bytes) per the general vector-compute constraints.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations; the default
 * unit is one DataBlock (32 bytes), consistent with the doc example that uses 8 for a 256-byte Repeat of half elements.
 */
__aicore__ inline void asc_repeat_reduce_max_index_value(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces the first count float elements of src Repeat by Repeat (256 bytes per Repeat) to the first maximum of
 * each Repeat and writes the per-Repeat [index, value] pair (index first) to dst.
 *
 * @override_id asc_repeat_reduce_max_index_value__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [index, value] pair output, one entry per Repeat; the
 * page aligns it to 8 bytes for float input and notes the stored index must be read back with the index type (uint32),
 * not the value type.
 * @param [in] src Source UB start address of the float elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (64 elements).
 * @param [in] count Number of float elements of src participating in the reduction; the count overload programs the
 * element mask internally through asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_repeat_reduce_max_index_value(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces half elements from src over repeat iterations (one 256-byte Repeat each) to the first maximum of every
 * Repeat and writes the per-Repeat [value, index] pair (value first) to dst.
 *
 * @override_id asc_repeat_reduce_max_value_index__PUf16_PUf16_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [value, index] pair output, one entry per Repeat; the
 * page aligns it to 4 bytes for half input and notes the stored index must be read back with the index type (uint16),
 * not the value type.
 * @param [in] src Source UB start address of the half elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (128 elements).
 * @param [in] repeat Number of iterations; each iteration reduces one 256-byte Repeat (128 half elements) and emits one
 * value-then-index pair at dst.
 * @param [in] dst_repeat_stride Address stride between consecutive per-Repeat outputs, in units of sizeof(half) * 2 = 4
 * bytes as specified by the parameter table of the page.
 * @param [in] src_block_stride Address stride between adjacent DataBlocks inside one source iteration; the default unit
 * is one DataBlock (32 bytes) per the general vector-compute constraints.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations; the default
 * unit is one DataBlock (32 bytes), consistent with the doc example that uses 8 for a 256-byte Repeat of half elements.
 */
__aicore__ inline void asc_repeat_reduce_max_value_index(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces the first count half elements of src Repeat by Repeat (256 bytes per Repeat) to the first maximum of
 * each Repeat and writes the per-Repeat [value, index] pair (value first) to dst.
 *
 * @override_id asc_repeat_reduce_max_value_index__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [value, index] pair output, one entry per Repeat; the
 * page aligns it to 4 bytes for half input and notes the stored index must be read back with the index type (uint16),
 * not the value type.
 * @param [in] src Source UB start address of the half elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (128 elements).
 * @param [in] count Number of half elements of src participating in the reduction; the count overload programs the
 * element mask internally through asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_repeat_reduce_max_value_index(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces float elements from src over repeat iterations (one 256-byte Repeat each) to the first maximum of
 * every Repeat and writes the per-Repeat [value, index] pair (value first) to dst.
 *
 * @override_id asc_repeat_reduce_max_value_index__PUf32_PUf32_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [value, index] pair output, one entry per Repeat; the
 * page aligns it to 8 bytes for float input and notes the stored index must be read back with the index type (uint32),
 * not the value type.
 * @param [in] src Source UB start address of the float elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (64 elements).
 * @param [in] repeat Number of iterations; each iteration reduces one 256-byte Repeat (64 float elements) and emits one
 * value-then-index pair at dst.
 * @param [in] dst_repeat_stride Address stride between consecutive per-Repeat outputs, in units of sizeof(float) * 2 =
 * 8 bytes as specified by the parameter table of the page.
 * @param [in] src_block_stride Address stride between adjacent DataBlocks inside one source iteration; the default unit
 * is one DataBlock (32 bytes) per the general vector-compute constraints.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations; the default
 * unit is one DataBlock (32 bytes), consistent with the doc example that uses 8 for a 256-byte Repeat of half elements.
 */
__aicore__ inline void asc_repeat_reduce_max_value_index(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces the first count float elements of src Repeat by Repeat (256 bytes per Repeat) to the first maximum of
 * each Repeat and writes the per-Repeat [value, index] pair (value first) to dst.
 *
 * @override_id asc_repeat_reduce_max_value_index__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [value, index] pair output, one entry per Repeat; the
 * page aligns it to 8 bytes for float input and notes the stored index must be read back with the index type (uint32),
 * not the value type.
 * @param [in] src Source UB start address of the float elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (64 elements).
 * @param [in] count Number of float elements of src participating in the reduction; the count overload programs the
 * element mask internally through asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_repeat_reduce_max_value_index(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces half elements from src over repeat iterations (one 256-byte Repeat each) to the first maximum of every
 * Repeat and writes only the per-Repeat maximum value to dst.
 *
 * @override_id asc_repeat_reduce_max_only_value__PUf16_PUf16_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination UB start address receiving the per-Repeat maximum half value; must be 4 bytes aligned
 * for half input.
 * @param [in] src Source UB start address of the half elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (128 elements).
 * @param [in] repeat Number of iterations; each iteration reduces one 256-byte Repeat (128 half elements) and emits one
 * maximum value at dst.
 * @param [in] dst_repeat_stride Address stride between consecutive per-Repeat outputs, in units of sizeof(half) = 2
 * bytes as specified by the parameter table of the page.
 * @param [in] src_block_stride Address stride between adjacent DataBlocks inside one source iteration; the default unit
 * is one DataBlock (32 bytes) per the general vector-compute constraints.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations; the default
 * unit is one DataBlock (32 bytes), consistent with the doc example that uses 8 for a 256-byte Repeat of half elements.
 */
__aicore__ inline void asc_repeat_reduce_max_only_value(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces the first count half elements of src Repeat by Repeat (256 bytes per Repeat) to the first maximum of
 * each Repeat and writes only the per-Repeat maximum value to dst.
 *
 * @override_id asc_repeat_reduce_max_only_value__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Destination UB start address receiving the per-Repeat maximum half value; must be 4 bytes aligned
 * for half input.
 * @param [in] src Source UB start address of the half elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (128 elements).
 * @param [in] count Number of half elements of src participating in the reduction; the count overload programs the
 * element mask internally through asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_repeat_reduce_max_only_value(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces float elements from src over repeat iterations (one 256-byte Repeat each) to the first maximum of
 * every Repeat and writes only the per-Repeat maximum value to dst.
 *
 * @override_id asc_repeat_reduce_max_only_value__PUf32_PUf32_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination UB start address receiving the per-Repeat maximum float value; must be 8 bytes aligned
 * for float input.
 * @param [in] src Source UB start address of the float elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (64 elements).
 * @param [in] repeat Number of iterations; each iteration reduces one 256-byte Repeat (64 float elements) and emits one
 * maximum value at dst.
 * @param [in] dst_repeat_stride Address stride between consecutive per-Repeat outputs, in units of sizeof(float) = 4
 * bytes as specified by the parameter table of the page.
 * @param [in] src_block_stride Address stride between adjacent DataBlocks inside one source iteration; the default unit
 * is one DataBlock (32 bytes) per the general vector-compute constraints.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations; the default
 * unit is one DataBlock (32 bytes), consistent with the doc example that uses 8 for a 256-byte Repeat of half elements.
 */
__aicore__ inline void asc_repeat_reduce_max_only_value(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces the first count float elements of src Repeat by Repeat (256 bytes per Repeat) to the first maximum of
 * each Repeat and writes only the per-Repeat maximum value to dst.
 *
 * @override_id asc_repeat_reduce_max_only_value__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Destination UB start address receiving the per-Repeat maximum float value; must be 8 bytes aligned
 * for float input.
 * @param [in] src Source UB start address of the float elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (64 elements).
 * @param [in] count Number of float elements of src participating in the reduction; the count overload programs the
 * element mask internally through asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_repeat_reduce_max_only_value(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces half elements from src over repeat iterations (one 256-byte Repeat each) to the first maximum of every
 * Repeat and writes only the per-Repeat index of that element to dst.
 *
 * @override_id asc_repeat_reduce_max_only_index__PUf16_PUf16_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [index] output, one entry per Repeat; the page aligns
 * it to 4 bytes for half input and notes the stored index must be read back with the index type (uint32), not the value
 * type.
 * @param [in] src Source UB start address of the half elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (128 elements).
 * @param [in] repeat Number of iterations; each iteration reduces one 256-byte Repeat (128 half elements) and emits one
 * first-maximum index at dst.
 * @param [in] dst_repeat_stride Address stride between consecutive per-Repeat outputs, in units of 4 bytes
 * (sizeof(uint32)) as specified by the parameter table of the page.
 * @param [in] src_block_stride Address stride between adjacent DataBlocks inside one source iteration; the default unit
 * is one DataBlock (32 bytes) per the general vector-compute constraints.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations; the default
 * unit is one DataBlock (32 bytes), consistent with the doc example that uses 8 for a 256-byte Repeat of half elements.
 */
__aicore__ inline void asc_repeat_reduce_max_only_index(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces the first count half elements of src Repeat by Repeat (256 bytes per Repeat) to the first maximum of
 * each Repeat and writes only the per-Repeat index of that element to dst.
 *
 * @override_id asc_repeat_reduce_max_only_index__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [index] output, one entry per Repeat; the page aligns
 * it to 4 bytes for half input and notes the stored index must be read back with the index type (uint32), not the value
 * type.
 * @param [in] src Source UB start address of the half elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (128 elements).
 * @param [in] count Number of half elements of src participating in the reduction; the count overload programs the
 * element mask internally through asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_repeat_reduce_max_only_index(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces float elements from src over repeat iterations (one 256-byte Repeat each) to the first maximum of
 * every Repeat and writes only the per-Repeat index of that element to dst.
 *
 * @override_id asc_repeat_reduce_max_only_index__PUf32_PUf32_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [index] output, one entry per Repeat; the page aligns
 * it to 8 bytes for float input and notes the stored index must be read back with the index type (uint32), not the
 * value type.
 * @param [in] src Source UB start address of the float elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (64 elements).
 * @param [in] repeat Number of iterations; each iteration reduces one 256-byte Repeat (64 float elements) and emits one
 * first-maximum index at dst.
 * @param [in] dst_repeat_stride Address stride between consecutive per-Repeat outputs, in units of 4 bytes
 * (sizeof(uint32)) as specified by the parameter table of the page.
 * @param [in] src_block_stride Address stride between adjacent DataBlocks inside one source iteration; the default unit
 * is one DataBlock (32 bytes) per the general vector-compute constraints.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations; the default
 * unit is one DataBlock (32 bytes), consistent with the doc example that uses 8 for a 256-byte Repeat of half elements.
 */
__aicore__ inline void asc_repeat_reduce_max_only_index(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces the first count float elements of src Repeat by Repeat (256 bytes per Repeat) to the first maximum of
 * each Repeat and writes only the per-Repeat index of that element to dst.
 *
 * @override_id asc_repeat_reduce_max_only_index__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [index] output, one entry per Repeat; the page aligns
 * it to 8 bytes for float input and notes the stored index must be read back with the index type (uint32), not the
 * value type.
 * @param [in] src Source UB start address of the float elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (64 elements).
 * @param [in] count Number of float elements of src participating in the reduction; the count overload programs the
 * element mask internally through asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_repeat_reduce_max_only_index(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces half elements from src over repeat iterations (one 256-byte Repeat each) to the first minimum of every
 * Repeat and writes the per-Repeat [index, value] pair (index first) to dst.
 *
 * @override_id asc_repeat_reduce_min_index_value__PUf16_PUf16_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [index, value] pair output, one entry per Repeat; the
 * page aligns it to 4 bytes for half input and notes the stored index must be read back with the index type (uint16),
 * not the value type.
 * @param [in] src Source UB start address of the half elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (128 elements).
 * @param [in] repeat Number of iterations; each iteration reduces one 256-byte Repeat (128 half elements) and emits one
 * index-then-value pair at dst.
 * @param [in] dst_repeat_stride Address stride between consecutive per-Repeat outputs, in units of sizeof(half) * 2 = 4
 * bytes as specified by the parameter table of the page.
 * @param [in] src_block_stride Address stride between adjacent DataBlocks inside one source iteration; the default unit
 * is one DataBlock (32 bytes) per the general vector-compute constraints.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations; the default
 * unit is one DataBlock (32 bytes), consistent with the doc example that uses 8 for a 256-byte Repeat of half elements.
 */
__aicore__ inline void asc_repeat_reduce_min_index_value(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces the first count half elements of src Repeat by Repeat (256 bytes per Repeat) to the first minimum of
 * each Repeat and writes the per-Repeat [index, value] pair (index first) to dst.
 *
 * @override_id asc_repeat_reduce_min_index_value__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [index, value] pair output, one entry per Repeat; the
 * page aligns it to 4 bytes for half input and notes the stored index must be read back with the index type (uint16),
 * not the value type.
 * @param [in] src Source UB start address of the half elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (128 elements).
 * @param [in] count Number of half elements of src participating in the reduction; the count overload programs the
 * element mask internally through asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_repeat_reduce_min_index_value(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces float elements from src over repeat iterations (one 256-byte Repeat each) to the first minimum of
 * every Repeat and writes the per-Repeat [index, value] pair (index first) to dst.
 *
 * @override_id asc_repeat_reduce_min_index_value__PUf32_PUf32_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [index, value] pair output, one entry per Repeat; the
 * page aligns it to 8 bytes for float input and notes the stored index must be read back with the index type (uint32),
 * not the value type.
 * @param [in] src Source UB start address of the float elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (64 elements).
 * @param [in] repeat Number of iterations; each iteration reduces one 256-byte Repeat (64 float elements) and emits one
 * index-then-value pair at dst.
 * @param [in] dst_repeat_stride Address stride between consecutive per-Repeat outputs, in units of sizeof(float) * 2 =
 * 8 bytes as specified by the parameter table of the page.
 * @param [in] src_block_stride Address stride between adjacent DataBlocks inside one source iteration; the default unit
 * is one DataBlock (32 bytes) per the general vector-compute constraints.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations; the default
 * unit is one DataBlock (32 bytes), consistent with the doc example that uses 8 for a 256-byte Repeat of half elements.
 */
__aicore__ inline void asc_repeat_reduce_min_index_value(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces the first count float elements of src Repeat by Repeat (256 bytes per Repeat) to the first minimum of
 * each Repeat and writes the per-Repeat [index, value] pair (index first) to dst.
 *
 * @override_id asc_repeat_reduce_min_index_value__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [index, value] pair output, one entry per Repeat; the
 * page aligns it to 8 bytes for float input and notes the stored index must be read back with the index type (uint32),
 * not the value type.
 * @param [in] src Source UB start address of the float elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (64 elements).
 * @param [in] count Number of float elements of src participating in the reduction; the count overload programs the
 * element mask internally through asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_repeat_reduce_min_index_value(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces half elements from src over repeat iterations (one 256-byte Repeat each) to the first minimum of every
 * Repeat and writes the per-Repeat [value, index] pair (value first) to dst.
 *
 * @override_id asc_repeat_reduce_min_value_index__PUf16_PUf16_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [value, index] pair output, one entry per Repeat; the
 * page aligns it to 4 bytes for half input and notes the stored index must be read back with the index type (uint16),
 * not the value type.
 * @param [in] src Source UB start address of the half elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (128 elements).
 * @param [in] repeat Number of iterations; each iteration reduces one 256-byte Repeat (128 half elements) and emits one
 * value-then-index pair at dst.
 * @param [in] dst_repeat_stride Address stride between consecutive per-Repeat outputs, in units of sizeof(half) * 2 = 4
 * bytes as specified by the parameter table of the page.
 * @param [in] src_block_stride Address stride between adjacent DataBlocks inside one source iteration; the default unit
 * is one DataBlock (32 bytes) per the general vector-compute constraints.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations; the default
 * unit is one DataBlock (32 bytes), consistent with the doc example that uses 8 for a 256-byte Repeat of half elements.
 */
__aicore__ inline void asc_repeat_reduce_min_value_index(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces the first count half elements of src Repeat by Repeat (256 bytes per Repeat) to the first minimum of
 * each Repeat and writes the per-Repeat [value, index] pair (value first) to dst.
 *
 * @override_id asc_repeat_reduce_min_value_index__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [value, index] pair output, one entry per Repeat; the
 * page aligns it to 4 bytes for half input and notes the stored index must be read back with the index type (uint16),
 * not the value type.
 * @param [in] src Source UB start address of the half elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (128 elements).
 * @param [in] count Number of half elements of src participating in the reduction; the count overload programs the
 * element mask internally through asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_repeat_reduce_min_value_index(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces float elements from src over repeat iterations (one 256-byte Repeat each) to the first minimum of
 * every Repeat and writes the per-Repeat [value, index] pair (value first) to dst.
 *
 * @override_id asc_repeat_reduce_min_value_index__PUf32_PUf32_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [value, index] pair output, one entry per Repeat; the
 * page aligns it to 8 bytes for float input and notes the stored index must be read back with the index type (uint32),
 * not the value type.
 * @param [in] src Source UB start address of the float elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (64 elements).
 * @param [in] repeat Number of iterations; each iteration reduces one 256-byte Repeat (64 float elements) and emits one
 * value-then-index pair at dst.
 * @param [in] dst_repeat_stride Address stride between consecutive per-Repeat outputs, in units of sizeof(float) * 2 =
 * 8 bytes as specified by the parameter table of the page.
 * @param [in] src_block_stride Address stride between adjacent DataBlocks inside one source iteration; the default unit
 * is one DataBlock (32 bytes) per the general vector-compute constraints.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations; the default
 * unit is one DataBlock (32 bytes), consistent with the doc example that uses 8 for a 256-byte Repeat of half elements.
 */
__aicore__ inline void asc_repeat_reduce_min_value_index(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces the first count float elements of src Repeat by Repeat (256 bytes per Repeat) to the first minimum of
 * each Repeat and writes the per-Repeat [value, index] pair (value first) to dst.
 *
 * @override_id asc_repeat_reduce_min_value_index__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [value, index] pair output, one entry per Repeat; the
 * page aligns it to 8 bytes for float input and notes the stored index must be read back with the index type (uint32),
 * not the value type.
 * @param [in] src Source UB start address of the float elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (64 elements).
 * @param [in] count Number of float elements of src participating in the reduction; the count overload programs the
 * element mask internally through asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_repeat_reduce_min_value_index(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces half elements from src over repeat iterations (one 256-byte Repeat each) to the first minimum of every
 * Repeat and writes only the per-Repeat minimum value to dst.
 *
 * @override_id asc_repeat_reduce_min_only_value__PUf16_PUf16_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination UB start address receiving the per-Repeat minimum half value; must be 4 bytes aligned
 * for half input.
 * @param [in] src Source UB start address of the half elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (128 elements).
 * @param [in] repeat Number of iterations; each iteration reduces one 256-byte Repeat (128 half elements) and emits one
 * minimum value at dst.
 * @param [in] dst_repeat_stride Address stride between consecutive per-Repeat outputs, in units of sizeof(half) = 2
 * bytes as specified by the parameter table of the page.
 * @param [in] src_block_stride Address stride between adjacent DataBlocks inside one source iteration; the default unit
 * is one DataBlock (32 bytes) per the general vector-compute constraints.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations; the default
 * unit is one DataBlock (32 bytes), consistent with the doc example that uses 8 for a 256-byte Repeat of half elements.
 */
__aicore__ inline void asc_repeat_reduce_min_only_value(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces the first count half elements of src Repeat by Repeat (256 bytes per Repeat) to the first minimum of
 * each Repeat and writes only the per-Repeat minimum value to dst.
 *
 * @override_id asc_repeat_reduce_min_only_value__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Destination UB start address receiving the per-Repeat minimum half value; must be 4 bytes aligned
 * for half input.
 * @param [in] src Source UB start address of the half elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (128 elements).
 * @param [in] count Number of half elements of src participating in the reduction; the count overload programs the
 * element mask internally through asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_repeat_reduce_min_only_value(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces float elements from src over repeat iterations (one 256-byte Repeat each) to the first minimum of
 * every Repeat and writes only the per-Repeat minimum value to dst.
 *
 * @override_id asc_repeat_reduce_min_only_value__PUf32_PUf32_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination UB start address receiving the per-Repeat minimum float value; must be 8 bytes aligned
 * for float input.
 * @param [in] src Source UB start address of the float elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (64 elements).
 * @param [in] repeat Number of iterations; each iteration reduces one 256-byte Repeat (64 float elements) and emits one
 * minimum value at dst.
 * @param [in] dst_repeat_stride Address stride between consecutive per-Repeat outputs, in units of sizeof(float) = 4
 * bytes as specified by the parameter table of the page.
 * @param [in] src_block_stride Address stride between adjacent DataBlocks inside one source iteration; the default unit
 * is one DataBlock (32 bytes) per the general vector-compute constraints.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations; the default
 * unit is one DataBlock (32 bytes), consistent with the doc example that uses 8 for a 256-byte Repeat of half elements.
 */
__aicore__ inline void asc_repeat_reduce_min_only_value(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces the first count float elements of src Repeat by Repeat (256 bytes per Repeat) to the first minimum of
 * each Repeat and writes only the per-Repeat minimum value to dst.
 *
 * @override_id asc_repeat_reduce_min_only_value__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Destination UB start address receiving the per-Repeat minimum float value; must be 8 bytes aligned
 * for float input.
 * @param [in] src Source UB start address of the float elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (64 elements).
 * @param [in] count Number of float elements of src participating in the reduction; the count overload programs the
 * element mask internally through asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_repeat_reduce_min_only_value(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces half elements from src over repeat iterations (one 256-byte Repeat each) to the first minimum of every
 * Repeat and writes only the per-Repeat index of that element to dst.
 *
 * @override_id asc_repeat_reduce_min_only_index__PUf16_PUf16_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [index] output, one entry per Repeat; the page aligns
 * it to 4 bytes for half input and notes the stored index must be read back with the index type (uint32), not the value
 * type.
 * @param [in] src Source UB start address of the half elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (128 elements).
 * @param [in] repeat Number of iterations; each iteration reduces one 256-byte Repeat (128 half elements) and emits one
 * first-minimum index at dst.
 * @param [in] dst_repeat_stride Address stride between consecutive per-Repeat outputs, in units of 4 bytes
 * (sizeof(uint32)) as specified by the parameter table of the page.
 * @param [in] src_block_stride Address stride between adjacent DataBlocks inside one source iteration; the default unit
 * is one DataBlock (32 bytes) per the general vector-compute constraints.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations; the default
 * unit is one DataBlock (32 bytes), consistent with the doc example that uses 8 for a 256-byte Repeat of half elements.
 */
__aicore__ inline void asc_repeat_reduce_min_only_index(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces the first count half elements of src Repeat by Repeat (256 bytes per Repeat) to the first minimum of
 * each Repeat and writes only the per-Repeat index of that element to dst.
 *
 * @override_id asc_repeat_reduce_min_only_index__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [index] output, one entry per Repeat; the page aligns
 * it to 4 bytes for half input and notes the stored index must be read back with the index type (uint32), not the value
 * type.
 * @param [in] src Source UB start address of the half elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (128 elements).
 * @param [in] count Number of half elements of src participating in the reduction; the count overload programs the
 * element mask internally through asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_repeat_reduce_min_only_index(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces float elements from src over repeat iterations (one 256-byte Repeat each) to the first minimum of
 * every Repeat and writes only the per-Repeat index of that element to dst.
 *
 * @override_id asc_repeat_reduce_min_only_index__PUf32_PUf32_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [index] output, one entry per Repeat; the page aligns
 * it to 8 bytes for float input and notes the stored index must be read back with the index type (uint32), not the
 * value type.
 * @param [in] src Source UB start address of the float elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (64 elements).
 * @param [in] repeat Number of iterations; each iteration reduces one 256-byte Repeat (64 float elements) and emits one
 * first-minimum index at dst.
 * @param [in] dst_repeat_stride Address stride between consecutive per-Repeat outputs, in units of 4 bytes
 * (sizeof(uint32)) as specified by the parameter table of the page.
 * @param [in] src_block_stride Address stride between adjacent DataBlocks inside one source iteration; the default unit
 * is one DataBlock (32 bytes) per the general vector-compute constraints.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent source iterations; the default
 * unit is one DataBlock (32 bytes), consistent with the doc example that uses 8 for a 256-byte Repeat of half elements.
 */
__aicore__ inline void asc_repeat_reduce_min_only_index(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Reduces the first count float elements of src Repeat by Repeat (256 bytes per Repeat) to the first minimum of
 * each Repeat and writes only the per-Repeat index of that element to dst.
 *
 * @override_id asc_repeat_reduce_min_only_index__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Destination UB start address of the per-Repeat [index] output, one entry per Repeat; the page aligns
 * it to 8 bytes for float input and notes the stored index must be read back with the index type (uint32), not the
 * value type.
 * @param [in] src Source UB start address of the float elements to reduce; must be 32-byte aligned, and each Repeat
 * consumes 256 bytes (64 elements).
 * @param [in] count Number of float elements of src participating in the reduction; the count overload programs the
 * element mask internally through asc_set_mask_count_begin/end.
 */
__aicore__ inline void asc_repeat_reduce_min_only_index(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Computes the adjacent-pair summation reduction over the half elements addressed by src and writes the
 * halved-length per-iteration result into dst using the repeat and DataBlock stride pattern.
 *
 * @override_id asc_pair_reduce_sum__PUf16_PUf16_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the reduced half results; the reduction writes here and dst is
 * never read as an operand.
 * @param [in] src Start address in Unified Buffer of the half source vector that is reduced; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; each iteration reduces 8 DataBlocks of 32 Byte, so
 * repeat must not exceed the hardware limit of 255.
 * @param [in] dst_repeat_stride Address distance between the same destination DataBlock of adjacent iterations,
 * measured by the page in units of one reduced repeat length (the pair reduction halves the repeat length); declared
 * uint16_t here.
 * @param [in] src_block_stride Address distance between consecutive DataBlocks inside one iteration for src, counted in
 * DataBlocks of 32 Byte (declared uint16_t here).
 * @param [in] src_repeat_stride Address distance between the same source DataBlock of adjacent iterations, counted in
 * DataBlocks of 32 Byte (declared uint16_t here).
 */
__aicore__ inline void asc_pair_reduce_sum(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Computes the adjacent-pair summation reduction over the first count half elements addressed by src and writes
 * the reduced results into dst.
 *
 * @override_id asc_pair_reduce_sum__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the reduced half results; the reduction writes here and dst is
 * never read as an operand.
 * @param [in] src Start address in Unified Buffer of the half source vector that is reduced; read only.
 * @param [in] count Number of leading half elements to reduce, counted in elements rather than bytes; the impl turns it
 * into a mask count.
 */
__aicore__ inline void asc_pair_reduce_sum(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Computes the adjacent-pair summation reduction over the float elements addressed by src and writes the
 * halved-length per-iteration result into dst using the repeat and DataBlock stride pattern.
 *
 * @override_id asc_pair_reduce_sum__PUf32_PUf32_u8_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the reduced float results; the reduction writes here and dst is
 * never read as an operand.
 * @param [in] src Start address in Unified Buffer of the float source vector that is reduced; read only.
 * @param [in] repeat Iteration count of the high-dimensional form; each iteration reduces 8 DataBlocks of 32 Byte, so
 * repeat must not exceed the hardware limit of 255.
 * @param [in] dst_repeat_stride Address distance between the same destination DataBlock of adjacent iterations,
 * measured by the page in units of one reduced repeat length (the pair reduction halves the repeat length); declared
 * uint16_t here.
 * @param [in] src_block_stride Address distance between consecutive DataBlocks inside one iteration for src, counted in
 * DataBlocks of 32 Byte (declared uint16_t here).
 * @param [in] src_repeat_stride Address distance between the same source DataBlock of adjacent iterations, counted in
 * DataBlocks of 32 Byte (declared uint16_t here).
 */
__aicore__ inline void asc_pair_reduce_sum(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_repeat_stride, uint16_t src_block_stride,
    uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_reduce
 * @brief Computes the adjacent-pair summation reduction over the first count float elements addressed by src and writes
 * the reduced results into dst.
 *
 * @override_id asc_pair_reduce_sum__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the reduced float results; the reduction writes here and dst is
 * never read as an operand.
 * @param [in] src Start address in Unified Buffer of the float source vector that is reduced; read only.
 * @param [in] count Number of leading float elements to reduce, counted in elements rather than bytes; the impl turns
 * it into a mask count.
 */
__aicore__ inline void asc_pair_reduce_sum(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_REDUCE_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_REDUCE_H
#endif
