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
#error "vector_compute/compute/vector_permute_sel.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_PERMUTE_SEL_H
#endif

#ifndef INCLUDE_C_API_VECTOR_COMPUTE_COMPUTE_VECTOR_PERMUTE_SEL_H
#define INCLUDE_C_API_VECTOR_COMPUTE_COMPUTE_VECTOR_PERMUTE_SEL_H
#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/memory_base_impl/vector_permute_sel_intf_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/vector_permute_sel_intf_impl.h"
#endif
/**
 * @ingroup simd_c_api.vector_compute.vector_permute_sel
 * @brief Loads the eight Unified Buffer addresses held in src_array into the vector address register selected by addr
 * for int8_t operands.
 *
 * @override_id asc_set_va_reg__uba8_PPUi8
 * @archs 2201 3510
 * @param [in] addr Vector address register to load, of type ub_addr8_t; valid values are VA0 to VA7 and each register
 * binds eight addresses.
 * @param [in] src_array Pointer to eight Unified Buffer addresses of int8_t elements that are read and loaded into the
 * selected address register.
 */
__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ int8_t** src_array);

/**
 * @ingroup simd_c_api.vector_compute.vector_permute_sel
 * @brief Loads the eight Unified Buffer addresses held in src_array into the vector address register selected by addr
 * for uint8_t operands.
 *
 * @override_id asc_set_va_reg__uba8_PPUu8
 * @archs 2201 3510
 * @param [in] addr Vector address register to load, of type ub_addr8_t; valid values are VA0 to VA7 and each register
 * binds eight addresses.
 * @param [in] src_array Pointer to eight Unified Buffer addresses of uint8_t elements that are read and loaded into the
 * selected address register.
 */
__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ uint8_t** src_array);

/**
 * @ingroup simd_c_api.vector_compute.vector_permute_sel
 * @brief Loads the eight Unified Buffer addresses held in src_array into the vector address register selected by addr
 * for int16_t operands.
 *
 * @override_id asc_set_va_reg__uba8_PPUi16
 * @archs 2201 3510
 * @param [in] addr Vector address register to load, of type ub_addr8_t; valid values are VA0 to VA7 and each register
 * binds eight addresses.
 * @param [in] src_array Pointer to eight Unified Buffer addresses of int16_t elements that are read and loaded into the
 * selected address register.
 */
__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ int16_t** src_array);

/**
 * @ingroup simd_c_api.vector_compute.vector_permute_sel
 * @brief Loads the eight Unified Buffer addresses held in src_array into the vector address register selected by addr
 * for uint16_t operands.
 *
 * @override_id asc_set_va_reg__uba8_PPUu16
 * @archs 2201 3510
 * @param [in] addr Vector address register to load, of type ub_addr8_t; valid values are VA0 to VA7 and each register
 * binds eight addresses.
 * @param [in] src_array Pointer to eight Unified Buffer addresses of uint16_t elements that are read and loaded into
 * the selected address register.
 */
__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ uint16_t** src_array);

/**
 * @ingroup simd_c_api.vector_compute.vector_permute_sel
 * @brief Loads the eight Unified Buffer addresses held in src_array into the vector address register selected by addr
 * for half operands.
 *
 * @override_id asc_set_va_reg__uba8_PPUf16
 * @archs 2201 3510
 * @param [in] addr Vector address register to load, of type ub_addr8_t; valid values are VA0 to VA7 and each register
 * binds eight addresses.
 * @param [in] src_array Pointer to eight Unified Buffer addresses of half elements that are read and loaded into the
 * selected address register.
 */
__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ half** src_array);

/**
 * @ingroup simd_c_api.vector_compute.vector_permute_sel
 * @brief Loads the eight Unified Buffer addresses held in src_array into the vector address register selected by addr
 * for int32_t operands.
 *
 * @override_id asc_set_va_reg__uba8_PPUi32
 * @archs 2201 3510
 * @param [in] addr Vector address register to load, of type ub_addr8_t; valid values are VA0 to VA7 and each register
 * binds eight addresses.
 * @param [in] src_array Pointer to eight Unified Buffer addresses of int32_t elements that are read and loaded into the
 * selected address register.
 */
__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ int32_t** src_array);

/**
 * @ingroup simd_c_api.vector_compute.vector_permute_sel
 * @brief Loads the eight Unified Buffer addresses held in src_array into the vector address register selected by addr
 * for uint32_t operands.
 *
 * @override_id asc_set_va_reg__uba8_PPUu32
 * @archs 2201 3510
 * @param [in] addr Vector address register to load, of type ub_addr8_t; valid values are VA0 to VA7 and each register
 * binds eight addresses.
 * @param [in] src_array Pointer to eight Unified Buffer addresses of uint32_t elements that are read and loaded into
 * the selected address register.
 */
__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ uint32_t** src_array);

/**
 * @ingroup simd_c_api.vector_compute.vector_permute_sel
 * @brief Loads the eight Unified Buffer addresses held in src_array into the vector address register selected by addr
 * for float operands.
 *
 * @override_id asc_set_va_reg__uba8_PPUf32
 * @archs 2201 3510
 * @param [in] addr Vector address register to load, of type ub_addr8_t; valid values are VA0 to VA7 and each register
 * binds eight addresses.
 * @param [in] src_array Pointer to eight Unified Buffer addresses of float elements that are read and loaded into the
 * selected address register.
 */
__aicore__ inline void asc_set_va_reg(ub_addr8_t addr, __ubuf__ float** src_array);

__aicore__ inline void asc_transpose(__ubuf__ int16_t* dst, __ubuf__ int16_t* src);

__aicore__ inline void asc_transpose(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src);

/**
 * @ingroup simd_c_api.vector_compute.vector_permute_sel
 * @brief Transposes data across the 16 addressed source DataBlocks (512 bytes per repeat) for NCHW to NC1HWC0
 * conversion, with selectable high or low DataBlock halves for 8-bit data.
 *
 * @override_id asc_transto5hd_b8__uba8_uba8_u8_u16_u16_b_b
 * @archs 2201 3510
 * @param [out] dst Destination address register (VA0, VA2, VA4 or VA6) whose 16 entries receive the transposed
 * DataBlocks.
 * @param [in] src Source address register (VA0, VA2, VA4 or VA6) holding the 16 input DataBlock addresses; it must
 * differ from dst.
 * @param [in] repeat Number of repeat iterations in the range [0, 255]; with repeat == 1 the effective start is offset
 * by dst_stride/src_stride, so pass 0 strides to start at the given addresses.
 * @param [in] dst_stride Stride between the same DataBlock of dst in adjacent iterations, in DataBlocks (32 bytes).
 * @param [in] src_stride Stride between the same DataBlock of src in adjacent iterations, in DataBlocks (32 bytes).
 * @param [in] dst_high_half bool selecting the high half (true) or low half (false) of each destination DataBlock; only
 * meaningful for int8_t/uint8_t data.
 * @param [in] src_high_half bool selecting the high half (true) or low half (false) of each source DataBlock; only
 * meaningful for int8_t/uint8_t data.
 */
__aicore__ inline void asc_transto5hd_b8(
    ub_addr8_t dst, ub_addr8_t src, uint8_t repeat, uint16_t dst_stride, uint16_t src_stride, bool dst_high_half,
    bool src_high_half);

/**
 * @ingroup simd_c_api.vector_compute.vector_permute_sel
 * @brief Transposes 16-bit data across the 16 addressed source DataBlocks, completing a 16x16 transpose per repeat for
 * NCHW to NC1HWC0 conversion.
 *
 * @override_id asc_transto5hd_b16__uba8_uba8_u8_u16_u16
 * @archs 2201 3510
 * @param [out] dst Destination address register (VA0, VA2, VA4 or VA6) whose 16 entries receive the converted
 * DataBlocks.
 * @param [in] src Source address register (VA0, VA2, VA4 or VA6) holding the 16 input DataBlock addresses; it must
 * differ from dst.
 * @param [in] repeat Number of repeat iterations in the range [0, 255]; with repeat == 1 the effective start is offset
 * by dst_stride/src_stride, so pass 0 strides to start at the given addresses.
 * @param [in] dst_stride Stride between the same DataBlock of dst in adjacent iterations, in DataBlocks (32 bytes).
 * @param [in] src_stride Stride between the same DataBlock of src in adjacent iterations, in DataBlocks (32 bytes).
 */
__aicore__ inline void asc_transto5hd_b16(
    ub_addr8_t dst, ub_addr8_t src, uint8_t repeat, uint16_t dst_stride, uint16_t src_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_permute_sel
 * @brief Transposes 32-bit data across the 16 addressed source DataBlocks (512 bytes per repeat) and converts NCHW
 * layouts to NC1HWC0.
 *
 * @override_id asc_transto5hd_b32__uba8_uba8_u8_u16_u16
 * @archs 2201 3510
 * @param [out] dst Destination address register (VA0, VA2, VA4 or VA6) whose 16 entries receive the converted
 * DataBlocks.
 * @param [in] src Source address register (VA0, VA2, VA4 or VA6) holding the 16 input DataBlock addresses; it must
 * differ from dst.
 * @param [in] repeat Number of repeat iterations in the range [0, 255]; with repeat == 1 the effective start is offset
 * by dst_stride/src_stride, so pass 0 strides to start at the given addresses.
 * @param [in] dst_stride Stride between the same DataBlock of dst in adjacent iterations, in DataBlocks (32 bytes).
 * @param [in] src_stride Stride between the same DataBlock of src in adjacent iterations, in DataBlocks (32 bytes).
 */
__aicore__ inline void asc_transto5hd_b32(
    ub_addr8_t dst, ub_addr8_t src, uint8_t repeat, uint16_t dst_stride, uint16_t src_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_permute_sel
 * @brief Selects each of the first count half elements from src0 or src1 according to the comparison mask set by
 * asc_set_cmp_mask and writes the results into dst.
 *
 * @override_id asc_select__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the half destination vector that receives the selected elements;
 * written and never read.
 * @param [in] src0 Start address in Unified Buffer of the half vector selected when the sel_mask bit is 1; read only.
 * @param [in] src1 Start address in Unified Buffer of the half vector selected when the sel_mask bit is 0; read only.
 * @param [in] count Number of leading half elements to select, counted in elements rather than bytes; the impl turns it
 * into a mask count.
 */
__aicore__ inline void asc_select(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_permute_sel
 * @brief Selects each half element from src0 or src1 according to the comparison mask set by asc_set_cmp_mask and
 * writes the per-iteration results into dst over the repeat and DataBlock stride pattern.
 *
 * @override_id asc_select__PUf16_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the half destination vector that receives the selected elements;
 * written and never read.
 * @param [in] src0 Start address in Unified Buffer of the half vector selected when the sel_mask bit is 1; read only.
 * @param [in] src1 Start address in Unified Buffer of the half vector selected when the sel_mask bit is 0; read only.
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
__aicore__ inline void asc_select(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_permute_sel
 * @brief Selects each of the first count float elements from src0 or src1 according to the comparison mask set by
 * asc_set_cmp_mask and writes the results into dst.
 *
 * @override_id asc_select__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the float destination vector that receives the selected
 * elements; written and never read.
 * @param [in] src0 Start address in Unified Buffer of the float vector selected when the sel_mask bit is 1; read only.
 * @param [in] src1 Start address in Unified Buffer of the float vector selected when the sel_mask bit is 0; read only.
 * @param [in] count Number of leading float elements to select, counted in elements rather than bytes; the impl turns
 * it into a mask count.
 */
__aicore__ inline void asc_select(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

/**
 * @ingroup simd_c_api.vector_compute.vector_permute_sel
 * @brief Selects each float element from src0 or src1 according to the comparison mask set by asc_set_cmp_mask and
 * writes the per-iteration results into dst over the repeat and DataBlock stride pattern.
 *
 * @override_id asc_select__PUf32_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer for the float destination vector that receives the selected
 * elements; written and never read.
 * @param [in] src0 Start address in Unified Buffer of the float vector selected when the sel_mask bit is 1; read only.
 * @param [in] src1 Start address in Unified Buffer of the float vector selected when the sel_mask bit is 0; read only.
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
__aicore__ inline void asc_select(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_permute_sel
 * @brief Compresses the uint16 vector src0 into a shorter vector using the comparison mask src1 and stores the
 * compacted elements in dst.
 *
 * @override_id asc_squeeze__PUu16_PUu16_PUu16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the compressed elements; must be 32-byte aligned.
 * @param [in] src0 Source vector start address holding the elements to compress; must be 32-byte aligned.
 * @param [in] src1 Source vector start address holding the comparison mask that decides which src0 elements are kept.
 * @param [in] repeat Number of iterations of the compression.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src0_block_stride Address stride between DataBlocks inside one iteration of source 0.
 * @param [in] src1_block_stride Address stride between DataBlocks inside one iteration of source 1.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of adjacent source-0 iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of adjacent source-1 iterations.
 */
__aicore__ inline void asc_squeeze(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

/**
 * @ingroup simd_c_api.vector_compute.vector_permute_sel
 * @brief Compresses the uint32 vector src0 into a shorter vector using the comparison mask src1 and stores the
 * compacted elements in dst.
 *
 * @override_id asc_squeeze__PUu32_PUu32_PUu32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the compressed elements; must be 32-byte aligned.
 * @param [in] src0 Source vector start address holding the elements to compress; must be 32-byte aligned.
 * @param [in] src1 Source vector start address holding the comparison mask that decides which src0 elements are kept.
 * @param [in] repeat Number of iterations of the compression.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src0_block_stride Address stride between DataBlocks inside one iteration of source 0.
 * @param [in] src1_block_stride Address stride between DataBlocks inside one iteration of source 1.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent destination iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of adjacent source-0 iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of adjacent source-1 iterations.
 */
__aicore__ inline void asc_squeeze(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src0, __ubuf__ uint32_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_transto5hd_b32 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous layout conversion: transposes 32-bit data across the 16 addressed source DataBlocks
 * (512 bytes per repeat) and waits for the vector pipe.
 *
 * @override_id asc_transto5hd_b32_sync__uba8_uba8_u8_u16_u16
 * @archs 2201 3510
 * @param [out] dst Destination address register (one of VA0/VA2/VA4/VA6) whose 16 entries receive the converted
 * DataBlocks.
 * @param [in] src Source address register (one of VA0/VA2/VA4/VA6) holding the 16 input DataBlock addresses; it must
 * differ from dst.
 * @param [in] repeat Number of repeat iterations in [0, 255]; with repeat == 1 the effective start is offset by
 * dst_stride/src_stride, so pass 0 strides to start at the given addresses.
 * @param [in] dst_stride Stride between the same DataBlock of dst in adjacent iterations, in DataBlocks (32 bytes).
 * @param [in] src_stride Stride between the same DataBlock of src in adjacent iterations, in DataBlocks (32 bytes).
 */
__aicore__ inline void asc_transto5hd_b32_sync(
    ub_addr8_t dst, ub_addr8_t src, uint8_t repeat, uint16_t dst_stride, uint16_t src_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_transto5hd_b16 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous layout conversion: transposes 16-bit data across the 16 addressed source DataBlocks,
 * completing a 16x16 transpose per repeat, and waits for the vector pipe.
 *
 * @override_id asc_transto5hd_b16_sync__uba8_uba8_u8_u16_u16
 * @archs 2201 3510
 * @param [out] dst Destination address register (one of VA0/VA2/VA4/VA6) whose 16 entries receive the converted
 * DataBlocks.
 * @param [in] src Source address register (one of VA0/VA2/VA4/VA6) holding the 16 input DataBlock addresses; it must
 * differ from dst.
 * @param [in] repeat Number of repeat iterations in [0, 255]; with repeat == 1 the effective start is offset by
 * dst_stride/src_stride, so pass 0 strides to start at the given addresses.
 * @param [in] dst_stride Stride between the same DataBlock of dst in adjacent iterations, in DataBlocks (32 bytes).
 * @param [in] src_stride Stride between the same DataBlock of src in adjacent iterations, in DataBlocks (32 bytes).
 */
__aicore__ inline void asc_transto5hd_b16_sync(
    ub_addr8_t dst, ub_addr8_t src, uint8_t repeat, uint16_t dst_stride, uint16_t src_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_transto5hd_b8 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous 8-bit to 5HD layout conversion: transposes 16-address data blocks with selectable
 * high/low DataBlock halves and waits for the vector pipe.
 *
 * @override_id asc_transto5hd_b8_sync__uba8_uba8_u8_u16_u16_b_b
 * @archs 2201 3510
 * @param [out] dst Destination address register (one of VA0/VA2/VA4/VA6) whose 16 entries receive the converted
 * DataBlocks.
 * @param [in] src Source address register (one of VA0/VA2/VA4/VA6) holding the 16 input DataBlock addresses; it must
 * differ from dst.
 * @param [in] repeat Number of repeat iterations in [0, 255]; with repeat == 1 the effective start is offset by
 * dst_stride/src_stride, so pass 0 strides to start at the given addresses.
 * @param [in] dst_stride Stride between the same DataBlock of dst in adjacent iterations, in DataBlocks (32 bytes).
 * @param [in] src_stride Stride between the same DataBlock of src in adjacent iterations, in DataBlocks (32 bytes).
 * @param [in] dst_high_half bool selecting the high half (true) or low half (false) of each destination DataBlock; only
 * meaningful for int8_t/uint8_t data.
 * @param [in] src_high_half bool selecting the high half (true) or low half (false) of each source DataBlock; only
 * meaningful for int8_t/uint8_t data.
 */
__aicore__ inline void asc_transto5hd_b8_sync(
    ub_addr8_t dst, ub_addr8_t src, uint8_t repeat, uint16_t dst_stride, uint16_t src_stride, bool dst_high_half,
    bool src_high_half);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_PERMUTE_SEL_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_COMPUTE_COMPUTE_VECTOR_PERMUTE_SEL_H
#endif
