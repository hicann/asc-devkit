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
#error "vector_compute/compute/vector_logic.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_LOGIC_H
#endif

#ifndef INCLUDE_C_API_VECTOR_COMPUTE_COMPUTE_VECTOR_LOGIC_H
#define INCLUDE_C_API_VECTOR_COMPUTE_COMPUTE_VECTOR_LOGIC_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/memory_base_impl/vector_logic_intf_impl.h"
#endif
/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Shifts int16_t elements of src left by distance bits over repeat vector iterations using per-operand DataBlock
 * and repeat strides, writing the results to dst.
 *
 * @override_id asc_shiftleft__PUi16_PUi16_u32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int16_t elements that receives the shifted
 * results.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] distance Bit count by which every source element is shifted left; it must not be negative and must not
 * exceed the bit width of int16_t.
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
__aicore__ inline void asc_shiftleft(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t distance, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Shifts each of the first count int16_t elements of src left by distance bits and writes the results to dst.
 *
 * @override_id asc_shiftleft__PUi16_PUi16_u32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int16_t elements that receives the shifted
 * results.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] distance Bit count by which every source element is shifted left; it must not be negative and must not
 * exceed the bit width of int16_t.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_shiftleft(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t distance, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Shifts uint16_t elements of src left by distance bits over repeat vector iterations using per-operand
 * DataBlock and repeat strides, writing the results to dst.
 *
 * @override_id asc_shiftleft__PUu16_PUu16_u32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of uint16_t elements that receives the
 * shifted results.
 * @param [in] src Start address of the source vector of uint16_t elements in Unified Buffer, read element-wise.
 * @param [in] distance Bit count by which every source element is shifted left; it must not be negative and must not
 * exceed the bit width of uint16_t.
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
__aicore__ inline void asc_shiftleft(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t distance, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Shifts each of the first count uint16_t elements of src left by distance bits and writes the results to dst.
 *
 * @override_id asc_shiftleft__PUu16_PUu16_u32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of uint16_t elements that receives the
 * shifted results.
 * @param [in] src Start address of the source vector of uint16_t elements in Unified Buffer, read element-wise.
 * @param [in] distance Bit count by which every source element is shifted left; it must not be negative and must not
 * exceed the bit width of uint16_t.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_shiftleft(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t distance, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Shifts int32_t elements of src left by distance bits over repeat vector iterations using per-operand DataBlock
 * and repeat strides, writing the results to dst.
 *
 * @override_id asc_shiftleft__PUi32_PUi32_u32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int32_t elements that receives the shifted
 * results.
 * @param [in] src Start address of the source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] distance Bit count by which every source element is shifted left; it must not be negative and must not
 * exceed the bit width of int32_t.
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
__aicore__ inline void asc_shiftleft(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t distance, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Shifts each of the first count int32_t elements of src left by distance bits and writes the results to dst.
 *
 * @override_id asc_shiftleft__PUi32_PUi32_u32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int32_t elements that receives the shifted
 * results.
 * @param [in] src Start address of the source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] distance Bit count by which every source element is shifted left; it must not be negative and must not
 * exceed the bit width of int32_t.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_shiftleft(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t distance, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Shifts uint32_t elements of src left by distance bits over repeat vector iterations using per-operand
 * DataBlock and repeat strides, writing the results to dst.
 *
 * @override_id asc_shiftleft__PUu32_PUu32_u32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of uint32_t elements that receives the
 * shifted results.
 * @param [in] src Start address of the source vector of uint32_t elements in Unified Buffer, read element-wise.
 * @param [in] distance Bit count by which every source element is shifted left; it must not be negative and must not
 * exceed the bit width of uint32_t.
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
__aicore__ inline void asc_shiftleft(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t distance, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Shifts each of the first count uint32_t elements of src left by distance bits and writes the results to dst.
 *
 * @override_id asc_shiftleft__PUu32_PUu32_u32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of uint32_t elements that receives the
 * shifted results.
 * @param [in] src Start address of the source vector of uint32_t elements in Unified Buffer, read element-wise.
 * @param [in] distance Bit count by which every source element is shifted left; it must not be negative and must not
 * exceed the bit width of uint32_t.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_shiftleft(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t distance, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Computes the bitwise OR of the two operands over the first count int16_t elements addressed by src0 and src1
 * and writes the results into dst.
 *
 * @override_id asc_or__PUi16_PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the int16_t destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src0 Start address in Unified Buffer of the int16_t vector holding the first OR operand; read only.
 * @param [in] src1 Start address in Unified Buffer of the int16_t vector holding the second OR operand; read only.
 * @param [in] count Number of leading int16_t elements to process, counted in elements rather than bytes; the impl
 * turns it into a mask count.
 */
__aicore__ inline void asc_or(__ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Computes the bitwise OR of the two operands over int16_t elements addressed by src0 and src1 following the
 * repeat and DataBlock stride pattern and writes the per-iteration results into dst.
 *
 * @override_id asc_or__PUi16_PUi16_PUi16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the int16_t destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src0 Start address in Unified Buffer of the int16_t vector holding the first OR operand; read only.
 * @param [in] src1 Start address in Unified Buffer of the int16_t vector holding the second OR operand; read only.
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
__aicore__ inline void asc_or(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Computes the bitwise OR of the two operands over the first count uint16_t elements addressed by src0 and src1
 * and writes the results into dst.
 *
 * @override_id asc_or__PUu16_PUu16_PUu16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the uint16_t destination vector; results are written through
 * this pointer and it is never read.
 * @param [in] src0 Start address in Unified Buffer of the uint16_t vector holding the first OR operand; read only.
 * @param [in] src1 Start address in Unified Buffer of the uint16_t vector holding the second OR operand; read only.
 * @param [in] count Number of leading uint16_t elements to process, counted in elements rather than bytes; the impl
 * turns it into a mask count.
 */
__aicore__ inline void asc_or(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Computes the bitwise OR of the two operands over uint16_t elements addressed by src0 and src1 following the
 * repeat and DataBlock stride pattern and writes the per-iteration results into dst.
 *
 * @override_id asc_or__PUu16_PUu16_PUu16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the uint16_t destination vector; results are written through
 * this pointer and it is never read.
 * @param [in] src0 Start address in Unified Buffer of the uint16_t vector holding the first OR operand; read only.
 * @param [in] src1 Start address in Unified Buffer of the uint16_t vector holding the second OR operand; read only.
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
__aicore__ inline void asc_or(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Complements every bit of the first count int16_t elements addressed by src and writes the results into dst.
 *
 * @override_id asc_not__PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the int16_t destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the int16_t source vector; read only.
 * @param [in] count Number of leading int16_t elements to process, counted in elements rather than bytes; the impl
 * turns it into a mask count.
 */
__aicore__ inline void asc_not(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Complements every bit of the int16_t elements addressed by src following the repeat and DataBlock stride
 * pattern and writes the per-iteration results into dst.
 *
 * @override_id asc_not__PUi16_PUi16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the int16_t destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the int16_t source vector; read only.
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
__aicore__ inline void asc_not(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Complements every bit of the first count uint16_t elements addressed by src and writes the results into dst.
 *
 * @override_id asc_not__PUu16_PUu16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the uint16_t destination vector; results are written through
 * this pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the uint16_t source vector; read only.
 * @param [in] count Number of leading uint16_t elements to process, counted in elements rather than bytes; the impl
 * turns it into a mask count.
 */
__aicore__ inline void asc_not(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Complements every bit of the uint16_t elements addressed by src following the repeat and DataBlock stride
 * pattern and writes the per-iteration results into dst.
 *
 * @override_id asc_not__PUu16_PUu16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the uint16_t destination vector; results are written through
 * this pointer and it is never read.
 * @param [in] src Start address in Unified Buffer of the uint16_t source vector; read only.
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
__aicore__ inline void asc_not(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Shifts every int16_t element of src right by value bits using arithmetic shift, writing the results to dst.
 *
 * @override_id asc_shiftright__PUi16_PUi16_i32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination int16_t vector that receives the shifted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int16_t vector whose elements are shifted; it must be
 * 32-byte aligned.
 * @param [in] value Number of bit positions to shift right; the page forbids negative values and requires value not to
 * exceed the int16_t bit width (16), and the parameter type is int32_t.
 * @param [in] count Number of elements of src that participate in the shift, counted in int16_t elements.
 */
__aicore__ inline void asc_shiftright(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int32_t value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Shifts every int16_t element of src right by value bits using arithmetic shift, writing the results to dst.
 *
 * @override_id asc_shiftright__PUi16_PUi16_i32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination int16_t vector that receives the shifted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int16_t vector whose elements are shifted; it must be
 * 32-byte aligned.
 * @param [in] value Number of bit positions to shift right; the page forbids negative values and requires value not to
 * exceed the int16_t bit width (16), and the parameter type is int32_t.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src_block_stride Source address stride between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source address stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_shiftright(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Shifts every int32_t element of src right by value bits using arithmetic shift, writing the results to dst.
 *
 * @override_id asc_shiftright__PUi32_PUi32_i32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination int32_t vector that receives the shifted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int32_t vector whose elements are shifted; it must be
 * 32-byte aligned.
 * @param [in] value Number of bit positions to shift right; the page forbids negative values and requires value not to
 * exceed the int32_t bit width (32), and the parameter type is int32_t.
 * @param [in] count Number of elements of src that participate in the shift, counted in int32_t elements.
 */
__aicore__ inline void asc_shiftright(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Shifts every int32_t element of src right by value bits using arithmetic shift, writing the results to dst.
 *
 * @override_id asc_shiftright__PUi32_PUi32_i32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination int32_t vector that receives the shifted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int32_t vector whose elements are shifted; it must be
 * 32-byte aligned.
 * @param [in] value Number of bit positions to shift right; the page forbids negative values and requires value not to
 * exceed the int32_t bit width (32), and the parameter type is int32_t.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src_block_stride Source address stride between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source address stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_shiftright(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Shifts every uint16_t element of src right by value bits using logical shift, writing the results to dst.
 *
 * @override_id asc_shiftright__PUu16_PUu16_u32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination uint16_t vector that receives the shifted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source uint16_t vector whose elements are shifted; it must be
 * 32-byte aligned.
 * @param [in] value Number of bit positions to shift right; the page forbids negative values and requires value not to
 * exceed the uint16_t bit width (16), and the parameter type is uint32_t.
 * @param [in] count Number of elements of src that participate in the shift, counted in uint16_t elements.
 */
__aicore__ inline void asc_shiftright(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Shifts every uint16_t element of src right by value bits using logical shift, writing the results to dst.
 *
 * @override_id asc_shiftright__PUu16_PUu16_u32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination uint16_t vector that receives the shifted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source uint16_t vector whose elements are shifted; it must be
 * 32-byte aligned.
 * @param [in] value Number of bit positions to shift right; the page forbids negative values and requires value not to
 * exceed the uint16_t bit width (16), and the parameter type is uint32_t.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src_block_stride Source address stride between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source address stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_shiftright(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Shifts every uint32_t element of src right by value bits using logical shift, writing the results to dst.
 *
 * @override_id asc_shiftright__PUu32_PUu32_u32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination uint32_t vector that receives the shifted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source uint32_t vector whose elements are shifted; it must be
 * 32-byte aligned.
 * @param [in] value Number of bit positions to shift right; the page forbids negative values and requires value not to
 * exceed the uint32_t bit width (32), and the parameter type is uint32_t.
 * @param [in] count Number of elements of src that participate in the shift, counted in uint32_t elements.
 */
__aicore__ inline void asc_shiftright(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Shifts every uint32_t element of src right by value bits using logical shift, writing the results to dst.
 *
 * @override_id asc_shiftright__PUu32_PUu32_u32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination uint32_t vector that receives the shifted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source uint32_t vector whose elements are shifted; it must be
 * 32-byte aligned.
 * @param [in] value Number of bit positions to shift right; the page forbids negative values and requires value not to
 * exceed the uint32_t bit width (32), and the parameter type is uint32_t.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src_block_stride Source address stride between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source address stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_shiftright(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Shifts every int16_t element of src right by value bits using arithmetic shift, rounding the shifted-out bit
 * into the result, writing the results to dst.
 *
 * @override_id asc_shiftright_round__PUi16_PUi16_i32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination int16_t vector that receives the shifted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int16_t vector whose elements are shifted; it must be
 * 32-byte aligned.
 * @param [in] value Number of bit positions to shift right; the page forbids negative values and requires value not to
 * exceed the int16_t bit width (16), and the parameter type is int32_t.
 * @param [in] count Number of elements of src that participate in the shift, counted in int16_t elements.
 */
__aicore__ inline void asc_shiftright_round(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, int32_t value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Shifts every int16_t element of src right by value bits using arithmetic shift, rounding the shifted-out bit
 * into the result, writing the results to dst.
 *
 * @override_id asc_shiftright_round__PUi16_PUi16_i32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination int16_t vector that receives the shifted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int16_t vector whose elements are shifted; it must be
 * 32-byte aligned.
 * @param [in] value Number of bit positions to shift right; the page forbids negative values and requires value not to
 * exceed the int16_t bit width (16), and the parameter type is int32_t.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src_block_stride Source address stride between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source address stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_shiftright_round(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Shifts every int32_t element of src right by value bits using arithmetic shift, rounding the shifted-out bit
 * into the result, writing the results to dst.
 *
 * @override_id asc_shiftright_round__PUi32_PUi32_i32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination int32_t vector that receives the shifted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int32_t vector whose elements are shifted; it must be
 * 32-byte aligned.
 * @param [in] value Number of bit positions to shift right; the page forbids negative values and requires value not to
 * exceed the int32_t bit width (32), and the parameter type is int32_t.
 * @param [in] count Number of elements of src that participate in the shift, counted in int32_t elements.
 */
__aicore__ inline void asc_shiftright_round(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Shifts every int32_t element of src right by value bits using arithmetic shift, rounding the shifted-out bit
 * into the result, writing the results to dst.
 *
 * @override_id asc_shiftright_round__PUi32_PUi32_i32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer of the destination int32_t vector that receives the shifted
 * elements; it must be 32-byte aligned.
 * @param [in] src Start address in Unified Buffer of the source int32_t vector whose elements are shifted; it must be
 * 32-byte aligned.
 * @param [in] value Number of bit positions to shift right; the page forbids negative values and requires value not to
 * exceed the int32_t bit width (32), and the parameter type is int32_t.
 * @param [in] repeat Number of iterations; one iteration covers eight 32-byte DataBlocks.
 * @param [in] dst_block_stride Destination address stride between different DataBlocks inside one iteration, in units
 * of 32-byte DataBlocks.
 * @param [in] src_block_stride Source address stride between different DataBlocks inside one iteration, in units of
 * 32-byte DataBlocks.
 * @param [in] dst_repeat_stride Destination address stride between the same DataBlock of adjacent iterations, in units
 * of 32-byte DataBlocks.
 * @param [in] src_repeat_stride Source address stride between the same DataBlock of adjacent iterations, in units of
 * 32-byte DataBlocks.
 */
__aicore__ inline void asc_shiftright_round(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Computes the bitwise AND of the two operands over int16_t elements addressed by src0 and src1 following the
 * repeat and DataBlock stride pattern and writes the per-iteration results into dst.
 *
 * @override_id asc_and__PUi16_PUi16_PUi16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the int16_t destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src0 Start address in Unified Buffer of the int16_t vector holding the first AND operand; read only.
 * @param [in] src1 Start address in Unified Buffer of the int16_t vector holding the second AND operand; read only.
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
__aicore__ inline void asc_and(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Computes the bitwise AND of the two operands over the first count int16_t elements addressed by src0 and src1
 * and writes the results into dst.
 *
 * @override_id asc_and__PUi16_PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the int16_t destination vector; results are written through this
 * pointer and it is never read.
 * @param [in] src0 Start address in Unified Buffer of the int16_t vector holding the first AND operand; read only.
 * @param [in] src1 Start address in Unified Buffer of the int16_t vector holding the second AND operand; read only.
 * @param [in] count Number of leading int16_t elements to process, counted in elements rather than bytes; the impl
 * turns it into a mask count.
 */
__aicore__ inline void asc_and(__ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Computes the bitwise AND of the two operands over uint16_t elements addressed by src0 and src1 following the
 * repeat and DataBlock stride pattern and writes the per-iteration results into dst.
 *
 * @override_id asc_and__PUu16_PUu16_PUu16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the uint16_t destination vector; results are written through
 * this pointer and it is never read.
 * @param [in] src0 Start address in Unified Buffer of the uint16_t vector holding the first AND operand; read only.
 * @param [in] src1 Start address in Unified Buffer of the uint16_t vector holding the second AND operand; read only.
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
__aicore__ inline void asc_and(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_logic
 * @brief Computes the bitwise AND of the two operands over the first count uint16_t elements addressed by src0 and src1
 * and writes the results into dst.
 *
 * @override_id asc_and__PUu16_PUu16_PUu16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the uint16_t destination vector; results are written through
 * this pointer and it is never read.
 * @param [in] src0 Start address in Unified Buffer of the uint16_t vector holding the first AND operand; read only.
 * @param [in] src1 Start address in Unified Buffer of the uint16_t vector holding the second AND operand; read only.
 * @param [in] count Number of leading uint16_t elements to process, counted in elements rather than bytes; the impl
 * turns it into a mask count.
 */
__aicore__ inline void asc_and(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1, uint32_t count);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_LOGIC_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_LOGIC_H
#endif
