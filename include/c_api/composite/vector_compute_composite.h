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
#error "composite/vector_compute_composite.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_VECTOR_COMPUTE_COMPOSITE_H
#endif

#ifndef INCLUDE_C_API_COMPOSITE_VECTOR_COMPUTE_COMPOSITE_H
#define INCLUDE_C_API_COMPOSITE_VECTOR_COMPUTE_COMPOSITE_H
#include "c_api/defs/defs.h"
#include "c_api/vector_compute/compute/vector_permute_sel.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/memory_base_impl/vector_compute_composite_intf_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/vector_compute_composite_intf_impl.h"
#endif
ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Adds the half vectors src0 and src1 element-wise into dst for the first count elements and then inserts a
 * PIPE_ALL barrier; deprecated in favour of asc_add plus asc_sync.
 *
 * @override_id asc_add_sync__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the half (fp16) destination vector in Unified Buffer that receives the sums.
 * @param [in] src0 Start address of the first half (fp16) addend vector in Unified Buffer.
 * @param [in] src1 Start address of the second half (fp16) addend vector in Unified Buffer.
 * @param [in] count Number of consecutive elements added; the impl passes it to asc_set_mask_count_begin before the
 * vector add.
 */
__aicore__ inline void asc_add_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Adds the float vectors src0 and src1 element-wise into dst for the first count elements and then inserts a
 * PIPE_ALL barrier; deprecated in favour of asc_add plus asc_sync.
 *
 * @override_id asc_add_sync__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the float (fp32) destination vector in Unified Buffer that receives the sums.
 * @param [in] src0 Start address of the first float (fp32) addend vector in Unified Buffer.
 * @param [in] src1 Start address of the second float (fp32) addend vector in Unified Buffer.
 * @param [in] count Number of consecutive elements added; the impl passes it to asc_set_mask_count_begin before the
 * vector add.
 */
__aicore__ inline void asc_add_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Adds the int16_t vectors src0 and src1 element-wise into dst for the first count elements and then inserts a
 * PIPE_ALL barrier; deprecated in favour of asc_add plus asc_sync.
 *
 * @override_id asc_add_sync__PUi16_PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the 16-bit signed integer (int16_t) destination vector in Unified Buffer that
 * receives the sums.
 * @param [in] src0 Start address of the first 16-bit signed integer (int16_t) addend vector in Unified Buffer.
 * @param [in] src1 Start address of the second 16-bit signed integer (int16_t) addend vector in Unified Buffer.
 * @param [in] count Number of consecutive elements added; the impl passes it to asc_set_mask_count_begin before the
 * vector add.
 */
__aicore__ inline void asc_add_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Adds the int32_t vectors src0 and src1 element-wise into dst for the first count elements and then inserts a
 * PIPE_ALL barrier; deprecated in favour of asc_add plus asc_sync.
 *
 * @override_id asc_add_sync__PUi32_PUi32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the 32-bit signed integer (int32_t) destination vector in Unified Buffer that
 * receives the sums.
 * @param [in] src0 Start address of the first 32-bit signed integer (int32_t) addend vector in Unified Buffer.
 * @param [in] src1 Start address of the second 32-bit signed integer (int32_t) addend vector in Unified Buffer.
 * @param [in] count Number of consecutive elements added; the impl passes it to asc_set_mask_count_begin before the
 * vector add.
 */
__aicore__ inline void asc_add_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Adds the scalar value to each of the first count elements of src into dst and then inserts a PIPE_ALL barrier;
 * deprecated in favour of asc_add_scalar plus asc_sync.
 *
 * @override_id asc_add_scalar_sync__PUf16_PUf16_f16_u32
 * @archs 2201
 * @param [out] dst Start address of the half (fp16) destination vector in Unified Buffer that receives the sums.
 * @param [in] src Start address of the half (fp16) source vector whose elements are added to the scalar.
 * @param [in] value Scalar addend of type half (fp16) added to every processed source element.
 * @param [in] count Number of consecutive elements added; the impl passes it to asc_set_mask_count_begin before the
 * scalar-add instruction.
 */
__aicore__ inline void asc_add_scalar_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Adds the scalar value to each of the first count elements of src into dst and then inserts a PIPE_ALL barrier;
 * deprecated in favour of asc_add_scalar plus asc_sync.
 *
 * @override_id asc_add_scalar_sync__PUf32_PUf32_f32_u32
 * @archs 2201
 * @param [out] dst Start address of the float (fp32) destination vector in Unified Buffer that receives the sums.
 * @param [in] src Start address of the float (fp32) source vector whose elements are added to the scalar.
 * @param [in] value Scalar addend of type float (fp32) added to every processed source element.
 * @param [in] count Number of consecutive elements added; the impl passes it to asc_set_mask_count_begin before the
 * scalar-add instruction.
 */
__aicore__ inline void asc_add_scalar_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Adds the scalar value to each of the first count elements of src into dst and then inserts a PIPE_ALL barrier;
 * deprecated in favour of asc_add_scalar plus asc_sync.
 *
 * @override_id asc_add_scalar_sync__PUi16_PUi16_i16_u32
 * @archs 2201
 * @param [out] dst Start address of the 16-bit signed integer (int16_t) destination vector in Unified Buffer that
 * receives the sums.
 * @param [in] src Start address of the 16-bit signed integer (int16_t) source vector whose elements are added to the
 * scalar.
 * @param [in] value Scalar addend of type 16-bit signed integer (int16_t) added to every processed source element.
 * @param [in] count Number of consecutive elements added; the impl passes it to asc_set_mask_count_begin before the
 * scalar-add instruction.
 */
__aicore__ inline void asc_add_scalar_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Adds the scalar value to each of the first count elements of src into dst and then inserts a PIPE_ALL barrier;
 * deprecated in favour of asc_add_scalar plus asc_sync.
 *
 * @override_id asc_add_scalar_sync__PUi32_PUi32_i32_u32
 * @archs 2201
 * @param [out] dst Start address of the 32-bit signed integer (int32_t) destination vector in Unified Buffer that
 * receives the sums.
 * @param [in] src Start address of the 32-bit signed integer (int32_t) source vector whose elements are added to the
 * scalar.
 * @param [in] value Scalar addend of type 32-bit signed integer (int32_t) added to every processed source element.
 * @param [in] count Number of consecutive elements added; the impl passes it to asc_set_mask_count_begin before the
 * scalar-add instruction.
 */
__aicore__ inline void asc_add_scalar_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_brcb and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous broadcast wrapper: takes 8 elements of src per iteration and fills 8 destination
 * DataBlocks of 32 bytes with one element each.
 *
 * @override_id asc_brcb_sync__PUu16_PUu16_u16_u16_u8
 * @archs 2201
 * @param [out] dst Start address of the destination uint16_t vector receiving the broadcast DataBlocks; must not
 * overlap src.
 * @param [in] src Start address of the source uint16_t vector whose 8 elements per iteration are broadcast.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of dst.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock in adjacent iterations of dst.
 * @param [in] repeat Number of iterations, each broadcasting 8 source elements into 8 DataBlocks.
 */
__aicore__ inline void asc_brcb_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint16_t dst_block_stride, uint16_t dst_repeat_stride,
    uint8_t repeat);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_brcb and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous broadcast wrapper: takes 8 elements of src per iteration and fills 8 destination
 * DataBlocks of 32 bytes with one element each.
 *
 * @override_id asc_brcb_sync__PUu32_PUu32_u16_u16_u8
 * @archs 2201
 * @param [out] dst Start address of the destination uint32_t vector receiving the broadcast DataBlocks; must not
 * overlap src.
 * @param [in] src Start address of the source uint32_t vector whose 8 elements per iteration are broadcast.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of dst.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock in adjacent iterations of dst.
 * @param [in] repeat Number of iterations, each broadcasting 8 source elements into 8 DataBlocks.
 */
__aicore__ inline void asc_brcb_sync(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint16_t dst_block_stride, uint16_t dst_repeat_stride,
    uint8_t repeat);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bitsort and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous bitmap sort wrapper: sorts 32 (Score, Index) pairs per iteration in descending Score
 * order and writes the pairs to dst.
 *
 * @override_id asc_bitsort_sync__PUf16_PUf16_PUu32_i32
 * @archs 2201 3510
 * @param [out] dst Start address of the destination buffer that receives the sorted (half Score, uint32_t Index) pairs;
 * must be 32-byte aligned.
 * @param [in] src0 Start address of the half Score vector that is sorted; must be 32-byte aligned.
 * @param [in] src1 Start address of the uint32_t Index vector holding the index belonging to each Score; must be
 * 32-byte aligned.
 * @param [in] repeat Number of iterations, range [0, 255]; each iteration sorts 32 elements, after which src0/src1
 * advance by 32 elements and dst advances by 256 bytes.
 */
__aicore__ inline void asc_bitsort_sync(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ uint32_t* src1, int32_t repeat);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bitsort and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous bitmap sort wrapper: sorts 32 (Score, Index) pairs per iteration in descending Score
 * order and writes the pairs to dst.
 *
 * @override_id asc_bitsort_sync__PUf32_PUf32_PUu32_i32
 * @archs 2201 3510
 * @param [out] dst Start address of the destination buffer that receives the sorted (float Score, uint32_t Index)
 * pairs; must be 32-byte aligned.
 * @param [in] src0 Start address of the float Score vector that is sorted; must be 32-byte aligned.
 * @param [in] src1 Start address of the uint32_t Index vector holding the index belonging to each Score; must be
 * 32-byte aligned.
 * @param [in] repeat Number of iterations, range [0, 255]; each iteration sorts 32 elements, after which src0/src1
 * advance by 32 elements and dst advances by 256 bytes.
 */
__aicore__ inline void asc_bitsort_sync(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ uint32_t* src1, int32_t repeat);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously transposes a 16x16 two-dimensional matrix data block of int16_t elements from src into dst.
 *
 * @override_id asc_transpose_sync__PUi16_PUi16
 * @archs 2201 3510
 * @param [out] dst Destination vector start address receiving the transposed 16x16 block; must be 32-byte aligned.
 * @param [in] src Source vector start address of the 16x16 matrix block to transpose; must be 32-byte aligned.
 */
__aicore__ inline void asc_transpose_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_transpose and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously transposes a 16x16 two-dimensional matrix data block of uint16_t elements from src into dst.
 *
 * @override_id asc_transpose_sync__PUu16_PUu16
 * @archs 2201 3510
 * @param [out] dst Destination vector start address receiving the transposed 16x16 block; must be 32-byte aligned.
 * @param [in] src Source vector start address of the 16x16 matrix block to transpose; must be 32-byte aligned.
 */
__aicore__ inline void asc_transpose_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_datablock_reduce_sum and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous wrapper that sums all elements inside each DataBlock of src in a binary tree and waits
 * for the vector pipe.
 *
 * @override_id asc_datablock_reduce_sum_sync__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector that receives one reduction result per DataBlock; must
 * be 32-byte aligned.
 * @param [in] src Start address of the source half vector whose DataBlocks are reduced; must be 32-byte aligned.
 * @param [in] count Number of elements participating in the computation; the implementation turns it into a count-based
 * mask.
 */
__aicore__ inline void asc_datablock_reduce_sum_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_datablock_reduce_sum and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous wrapper that sums all elements inside each DataBlock of src in a binary tree and waits
 * for the vector pipe.
 *
 * @override_id asc_datablock_reduce_sum_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector that receives one reduction result per DataBlock; must
 * be 32-byte aligned.
 * @param [in] src Start address of the source float vector whose DataBlocks are reduced; must be 32-byte aligned.
 * @param [in] count Number of elements participating in the computation; the implementation turns it into a count-based
 * mask.
 */
__aicore__ inline void asc_datablock_reduce_sum_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_datablock_reduce_max and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous wrapper that computes the maximum inside each DataBlock of src and waits for the vector
 * pipe.
 *
 * @override_id asc_datablock_reduce_max_sync__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector that receives one reduction result per DataBlock; must
 * be 32-byte aligned.
 * @param [in] src Start address of the source half vector whose DataBlocks are reduced; must be 32-byte aligned.
 * @param [in] count Number of elements participating in the computation; the implementation turns it into a count-based
 * mask.
 */
__aicore__ inline void asc_datablock_reduce_max_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_datablock_reduce_max and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous wrapper that computes the maximum inside each DataBlock of src and waits for the vector
 * pipe.
 *
 * @override_id asc_datablock_reduce_max_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector that receives one reduction result per DataBlock; must
 * be 32-byte aligned.
 * @param [in] src Start address of the source float vector whose DataBlocks are reduced; must be 32-byte aligned.
 * @param [in] count Number of elements participating in the computation; the implementation turns it into a count-based
 * mask.
 */
__aicore__ inline void asc_datablock_reduce_max_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_datablock_reduce_min and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous wrapper that computes the minimum inside each DataBlock of src and waits for the vector
 * pipe.
 *
 * @override_id asc_datablock_reduce_min_sync__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector that receives one reduction result per DataBlock; must
 * be 32-byte aligned.
 * @param [in] src Start address of the source half vector whose DataBlocks are reduced; must be 32-byte aligned.
 * @param [in] count Number of elements participating in the computation; the implementation turns it into a count-based
 * mask.
 */
__aicore__ inline void asc_datablock_reduce_min_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_datablock_reduce_min and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous wrapper that computes the minimum inside each DataBlock of src and waits for the vector
 * pipe.
 *
 * @override_id asc_datablock_reduce_min_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector that receives one reduction result per DataBlock; must
 * be 32-byte aligned.
 * @param [in] src Start address of the source float vector whose DataBlocks are reduced; must be 32-byte aligned.
 * @param [in] count Number of elements participating in the computation; the implementation turns it into a count-based
 * mask.
 */
__aicore__ inline void asc_datablock_reduce_min_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_sum and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously sums all half elements inside each repeat using pairwise binary-tree addition and writes one
 * result per repeat to dst.
 *
 * @override_id asc_repeat_reduce_sum_sync__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving one pairwise sum per repeat; 2-byte aligned for half
 * data.
 * @param [in] src Source vector start address whose elements are summed per repeat; 32-byte aligned.
 * @param [in] count Number of elements participating in the computation.
 */
__aicore__ inline void asc_repeat_reduce_sum_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_sum and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously sums all float elements inside each repeat using pairwise binary-tree addition and writes one
 * result per repeat to dst.
 *
 * @override_id asc_repeat_reduce_sum_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving one pairwise sum per repeat; 4-byte aligned for float
 * data.
 * @param [in] src Source vector start address whose elements are summed per repeat; 32-byte aligned.
 * @param [in] count Number of elements participating in the computation.
 */
__aicore__ inline void asc_repeat_reduce_sum_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_max_index_value and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous per-repeat maximum reduction of half data that stores each repeat's first maximum and
 * its index as [index, value] in dst.
 *
 * @override_id asc_repeat_reduce_max_index_value_sync__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer that receives the reduction results, stored per repeat as [index,
 * value] with the index held in the destination element type; the address must be 4-byte aligned for this half
 * overload.
 * @param [in] src Start address of the half source data in Unified Buffer; it must be 32-byte aligned.
 * @param [in] count Number of elements taking part in the computation; the synchronous form derives the mask element
 * count from it, so no mask has to be set manually.
 */
__aicore__ inline void asc_repeat_reduce_max_index_value_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_max_index_value and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous per-repeat maximum reduction of float data that stores each repeat's first maximum and
 * its index as [index, value] in dst.
 *
 * @override_id asc_repeat_reduce_max_index_value_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer that receives the reduction results, stored per repeat as [index,
 * value] with the index held in the destination element type; the address must be 8-byte aligned for this float
 * overload.
 * @param [in] src Start address of the float source data in Unified Buffer; it must be 32-byte aligned.
 * @param [in] count Number of elements taking part in the computation; the synchronous form derives the mask element
 * count from it, so no mask has to be set manually.
 */
__aicore__ inline void asc_repeat_reduce_max_index_value_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_max_value_index and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous per-repeat maximum reduction of half data that stores each repeat's first maximum and
 * its index as [value, index] in dst.
 *
 * @override_id asc_repeat_reduce_max_value_index_sync__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer that receives the reduction results, stored per repeat as [value,
 * index] with the index held in the destination element type; the address must be 4-byte aligned for this half
 * overload.
 * @param [in] src Start address of the half source data in Unified Buffer; it must be 32-byte aligned.
 * @param [in] count Number of elements taking part in the computation; the synchronous form derives the mask element
 * count from it, so no mask has to be set manually.
 */
__aicore__ inline void asc_repeat_reduce_max_value_index_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_max_value_index and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous per-repeat maximum reduction of float data that stores each repeat's first maximum and
 * its index as [value, index] in dst.
 *
 * @override_id asc_repeat_reduce_max_value_index_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer that receives the reduction results, stored per repeat as [value,
 * index] with the index held in the destination element type; the address must be 8-byte aligned for this float
 * overload.
 * @param [in] src Start address of the float source data in Unified Buffer; it must be 32-byte aligned.
 * @param [in] count Number of elements taking part in the computation; the synchronous form derives the mask element
 * count from it, so no mask has to be set manually.
 */
__aicore__ inline void asc_repeat_reduce_max_value_index_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_max_only_value and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous per-repeat maximum reduction of half data that stores each repeat's maximum as a single
 * [value] result in dst.
 *
 * @override_id asc_repeat_reduce_max_only_value_sync__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer that receives the reduction results, stored per repeat as [value];
 * the address must be 4-byte aligned for this half overload.
 * @param [in] src Start address of the half source data in Unified Buffer; it must be 32-byte aligned.
 * @param [in] count Number of elements taking part in the computation; the synchronous form derives the mask element
 * count from it, so no mask has to be set manually.
 */
__aicore__ inline void asc_repeat_reduce_max_only_value_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_max_only_value and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous per-repeat maximum reduction of float data that stores each repeat's maximum as a
 * single [value] result in dst.
 *
 * @override_id asc_repeat_reduce_max_only_value_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer that receives the reduction results, stored per repeat as [value];
 * the address must be 8-byte aligned for this float overload.
 * @param [in] src Start address of the float source data in Unified Buffer; it must be 32-byte aligned.
 * @param [in] count Number of elements taking part in the computation; the synchronous form derives the mask element
 * count from it, so no mask has to be set manually.
 */
__aicore__ inline void asc_repeat_reduce_max_only_value_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_max_only_index and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous per-repeat maximum reduction of half data that stores the index of each repeat's
 * maximum as a single [index] result in dst.
 *
 * @override_id asc_repeat_reduce_max_only_index_sync__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer that receives the reduction results, stored per repeat as [index];
 * the address must be 4-byte aligned for this half overload.
 * @param [in] src Start address of the half source data in Unified Buffer; it must be 32-byte aligned.
 * @param [in] count Number of elements taking part in the computation; the synchronous form derives the mask element
 * count from it, so no mask has to be set manually.
 */
__aicore__ inline void asc_repeat_reduce_max_only_index_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_max_only_index and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous per-repeat maximum reduction of float data that stores the index of each repeat's
 * maximum as a single [index] result in dst.
 *
 * @override_id asc_repeat_reduce_max_only_index_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer that receives the reduction results, stored per repeat as [index];
 * the address must be 8-byte aligned for this float overload.
 * @param [in] src Start address of the float source data in Unified Buffer; it must be 32-byte aligned.
 * @param [in] count Number of elements taking part in the computation; the synchronous form derives the mask element
 * count from it, so no mask has to be set manually.
 */
__aicore__ inline void asc_repeat_reduce_max_only_index_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_min_index_value and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous per-repeat minimum reduction of half data that stores each repeat's first minimum and
 * its index as [index, value] in dst.
 *
 * @override_id asc_repeat_reduce_min_index_value_sync__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer that receives the reduction results, stored per repeat as [index,
 * value]; the address must be 4-byte aligned for this half overload.
 * @param [in] src Start address of the half source data in Unified Buffer; it must be 32-byte aligned.
 * @param [in] count Number of elements taking part in the computation; the synchronous form derives the mask element
 * count from it, so no mask has to be set manually.
 */
__aicore__ inline void asc_repeat_reduce_min_index_value_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_min_index_value and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous per-repeat minimum reduction of float data that stores each repeat's first minimum and
 * its index as [index, value] in dst.
 *
 * @override_id asc_repeat_reduce_min_index_value_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer that receives the reduction results, stored per repeat as [index,
 * value]; the address must be 8-byte aligned for this float overload.
 * @param [in] src Start address of the float source data in Unified Buffer; it must be 32-byte aligned.
 * @param [in] count Number of elements taking part in the computation; the synchronous form derives the mask element
 * count from it, so no mask has to be set manually.
 */
__aicore__ inline void asc_repeat_reduce_min_index_value_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_min_value_index and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous per-repeat minimum reduction of half data that stores each repeat's first minimum and
 * its index as [value, index] in dst.
 *
 * @override_id asc_repeat_reduce_min_value_index_sync__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer that receives the reduction results, stored per repeat as [value,
 * index]; the address must be 4-byte aligned for this half overload.
 * @param [in] src Start address of the half source data in Unified Buffer; it must be 32-byte aligned.
 * @param [in] count Number of elements taking part in the computation; the synchronous form derives the mask element
 * count from it, so no mask has to be set manually.
 */
__aicore__ inline void asc_repeat_reduce_min_value_index_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_min_value_index and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous per-repeat minimum reduction of float data that stores each repeat's first minimum and
 * its index as [value, index] in dst.
 *
 * @override_id asc_repeat_reduce_min_value_index_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer that receives the reduction results, stored per repeat as [value,
 * index]; the address must be 8-byte aligned for this float overload.
 * @param [in] src Start address of the float source data in Unified Buffer; it must be 32-byte aligned.
 * @param [in] count Number of elements taking part in the computation; the synchronous form derives the mask element
 * count from it, so no mask has to be set manually.
 */
__aicore__ inline void asc_repeat_reduce_min_value_index_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_min_only_value and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous per-repeat minimum reduction of half data that stores each repeat's minimum as a single
 * [value] result in dst.
 *
 * @override_id asc_repeat_reduce_min_only_value_sync__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer that receives the reduction results, stored per repeat as [value];
 * the address must be 4-byte aligned for this half overload.
 * @param [in] src Start address of the half source data in Unified Buffer; it must be 32-byte aligned.
 * @param [in] count Number of elements taking part in the computation; the synchronous form derives the mask element
 * count from it, so no mask has to be set manually.
 */
__aicore__ inline void asc_repeat_reduce_min_only_value_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_min_only_value and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous per-repeat minimum reduction of float data that stores each repeat's minimum as a
 * single [value] result in dst.
 *
 * @override_id asc_repeat_reduce_min_only_value_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer that receives the reduction results, stored per repeat as [value];
 * the address must be 8-byte aligned for this float overload.
 * @param [in] src Start address of the float source data in Unified Buffer; it must be 32-byte aligned.
 * @param [in] count Number of elements taking part in the computation; the synchronous form derives the mask element
 * count from it, so no mask has to be set manually.
 */
__aicore__ inline void asc_repeat_reduce_min_only_value_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_min_only_index and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous per-repeat minimum reduction of half data that stores the index of each repeat's
 * minimum as a single [index] result in dst.
 *
 * @override_id asc_repeat_reduce_min_only_index_sync__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer that receives the reduction results, stored per repeat as [index];
 * the address must be 4-byte aligned for this half overload.
 * @param [in] src Start address of the half source data in Unified Buffer; it must be 32-byte aligned.
 * @param [in] count Number of elements taking part in the computation; the synchronous form derives the mask element
 * count from it, so no mask has to be set manually.
 */
__aicore__ inline void asc_repeat_reduce_min_only_index_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_min_only_index and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous per-repeat minimum reduction of float data that stores the index of each repeat's
 * minimum as a single [index] result in dst.
 *
 * @override_id asc_repeat_reduce_min_only_index_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address in Unified Buffer that receives the reduction results, stored per repeat as [index];
 * the address must be 8-byte aligned for this float overload.
 * @param [in] src Start address of the float source data in Unified Buffer; it must be 32-byte aligned.
 * @param [in] count Number of elements taking part in the computation; the synchronous form derives the mask element
 * count from it, so no mask has to be set manually.
 */
__aicore__ inline void asc_repeat_reduce_min_only_index_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_eq and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 for equality element-wise and leaves one result bit per element in the comparison mask
 * register, and then inserts a PIPE_ALL barrier; deprecated in favour of asc_eq plus asc_sync.
 *
 * @override_id asc_eq_sync__PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
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
__aicore__ inline void asc_eq_sync(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_eq and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 for equality element-wise and leaves one result bit per element in the comparison mask
 * register, and then inserts a PIPE_ALL barrier; deprecated in favour of asc_eq plus asc_sync.
 *
 * @override_id asc_eq_sync__PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
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
__aicore__ inline void asc_eq_sync(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_eq and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 for equality element-wise and writes one result bit per element into dst, and then
 * inserts a PIPE_ALL barrier; deprecated in favour of asc_eq plus asc_sync.
 *
 * @override_id asc_eq_sync__PUu8_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
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
__aicore__ inline void asc_eq_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_eq and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 for equality element-wise and writes one result bit per element into dst, and then
 * inserts a PIPE_ALL barrier; deprecated in favour of asc_eq plus asc_sync.
 *
 * @override_id asc_eq_sync__PUu8_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
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
__aicore__ inline void asc_eq_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_eq and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 for equality element-wise and writes one result bit per element into dst, and then
 * inserts a PIPE_ALL barrier; deprecated in favour of asc_eq plus asc_sync.
 *
 * @override_id asc_eq_sync__PUu8_PUi32_PUi32_u8_u8_u8_u8_u8_u8_u8
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
__aicore__ inline void asc_eq_sync(
    __ubuf__ uint8_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162float and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form: converts bfloat16_t elements of src to float in dst with no rounding mode.
 *
 * @override_id asc_bfloat162float_sync__PUf32_PUbf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector, which must be 32-byte aligned.
 * @param [in] src Start address of the source vector of bfloat16_t elements, which must be 32-byte aligned.
 * @param [in] count Number of elements participating in the conversion.
 */
__aicore__ inline void asc_bfloat162float_sync(__ubuf__ float* dst, __ubuf__ bfloat16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rna and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count bfloat16_t (bf16) elements from src into 32-bit signed integer (int32_t) in dst using ROUND
 * rounding (round half away from zero) and then inserts a PIPE_ALL barrier; deprecated in favour of
 * asc_bfloat162int32_rna plus asc_sync.
 *
 * @override_id asc_bfloat162int32_rna_sync__PUi32_PUbf16_u32
 * @archs 2201
 * @param [out] dst Start address of the 32-bit signed integer (int32_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the bfloat16_t (bf16) source vector in Unified Buffer holding the elements to
 * convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_bfloat162int32_rna_sync(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_ru and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count bfloat16_t (bf16) elements from src into 32-bit signed integer (int32_t) in dst using CEIL
 * rounding (round toward positive infinity) and then inserts a PIPE_ALL barrier; deprecated in favour of
 * asc_bfloat162int32_ru plus asc_sync.
 *
 * @override_id asc_bfloat162int32_ru_sync__PUi32_PUbf16_u32
 * @archs 2201
 * @param [out] dst Start address of the 32-bit signed integer (int32_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the bfloat16_t (bf16) source vector in Unified Buffer holding the elements to
 * convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_bfloat162int32_ru_sync(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rd and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count bfloat16_t (bf16) elements from src into 32-bit signed integer (int32_t) in dst using FLOOR
 * rounding (round toward negative infinity) and then inserts a PIPE_ALL barrier; deprecated in favour of
 * asc_bfloat162int32_rd plus asc_sync.
 *
 * @override_id asc_bfloat162int32_rd_sync__PUi32_PUbf16_u32
 * @archs 2201
 * @param [out] dst Start address of the 32-bit signed integer (int32_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the bfloat16_t (bf16) source vector in Unified Buffer holding the elements to
 * convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_bfloat162int32_rd_sync(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rn and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count bfloat16_t (bf16) elements from src into 32-bit signed integer (int32_t) in dst using RINT
 * rounding (round half to even) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_bfloat162int32_rn plus
 * asc_sync.
 *
 * @override_id asc_bfloat162int32_rn_sync__PUi32_PUbf16_u32
 * @archs 2201
 * @param [out] dst Start address of the 32-bit signed integer (int32_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the bfloat16_t (bf16) source vector in Unified Buffer holding the elements to
 * convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_bfloat162int32_rn_sync(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count bfloat16_t (bf16) elements from src into 32-bit signed integer (int32_t) in dst using TRUNC
 * rounding (round toward zero) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_bfloat162int32_rz plus
 * asc_sync.
 *
 * @override_id asc_bfloat162int32_rz_sync__PUi32_PUbf16_u32
 * @archs 2201
 * @param [out] dst Start address of the 32-bit signed integer (int32_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the bfloat16_t (bf16) source vector in Unified Buffer holding the elements to
 * convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_bfloat162int32_rz_sync(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rn and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into bfloat16_t (bf16) in dst using RINT rounding (round half to
 * even) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2bfloat16_rn plus asc_sync.
 *
 * @override_id asc_float2bfloat16_rn_sync__PUbf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the bfloat16_t (bf16) destination vector in Unified Buffer that receives the
 * converted elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2bfloat16_rn_sync(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rna and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into bfloat16_t (bf16) in dst using ROUND rounding (round half
 * away from zero) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2bfloat16_rna plus asc_sync.
 *
 * @override_id asc_float2bfloat16_rna_sync__PUbf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the bfloat16_t (bf16) destination vector in Unified Buffer that receives the
 * converted elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2bfloat16_rna_sync(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rd and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into bfloat16_t (bf16) in dst using FLOOR rounding (round toward
 * negative infinity) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2bfloat16_rd plus asc_sync.
 *
 * @override_id asc_float2bfloat16_rd_sync__PUbf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the bfloat16_t (bf16) destination vector in Unified Buffer that receives the
 * converted elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2bfloat16_rd_sync(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_ru and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into bfloat16_t (bf16) in dst using CEIL rounding (round toward
 * positive infinity) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2bfloat16_ru plus asc_sync.
 *
 * @override_id asc_float2bfloat16_ru_sync__PUbf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the bfloat16_t (bf16) destination vector in Unified Buffer that receives the
 * converted elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2bfloat16_ru_sync(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into bfloat16_t (bf16) in dst using TRUNC rounding (round toward
 * zero) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2bfloat16_rz plus asc_sync.
 *
 * @override_id asc_float2bfloat16_rz_sync__PUbf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the bfloat16_t (bf16) destination vector in Unified Buffer that receives the
 * converted elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2bfloat16_rz_sync(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2float_rn and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into float (fp32) in dst using RINT rounding (round half to
 * even) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2float_rn plus asc_sync.
 *
 * @override_id asc_float2float_rn_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the float (fp32) destination vector in Unified Buffer that receives the converted
 * elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2float_rn_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2float_rd and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into float (fp32) in dst using FLOOR rounding (round toward
 * negative infinity) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2float_rd plus asc_sync.
 *
 * @override_id asc_float2float_rd_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the float (fp32) destination vector in Unified Buffer that receives the converted
 * elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2float_rd_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2float_ru and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into float (fp32) in dst using CEIL rounding (round toward
 * positive infinity) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2float_ru plus asc_sync.
 *
 * @override_id asc_float2float_ru_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the float (fp32) destination vector in Unified Buffer that receives the converted
 * elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2float_ru_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2float_rna and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into float (fp32) in dst using ROUND rounding (round half away
 * from zero) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2float_rna plus asc_sync.
 *
 * @override_id asc_float2float_rna_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the float (fp32) destination vector in Unified Buffer that receives the converted
 * elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2float_rna_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2float_rz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into float (fp32) in dst using TRUNC rounding (round toward
 * zero) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2float_rz plus asc_sync.
 *
 * @override_id asc_float2float_rz_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the float (fp32) destination vector in Unified Buffer that receives the converted
 * elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2float_rz_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float elements from src into half elements at dst using the default mode, which the doc
 * describes as RINT when the conversion loses precision and no rounding otherwise (deprecated; use asc_float2half plus
 * asc_sync).
 *
 * @override_id asc_float2half_sync__PUf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source float vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of elements converted, counted in elements of the source and destination vectors.
 */
__aicore__ inline void asc_float2half_sync(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rn and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float elements from src into half elements at dst using RINT (round half to even) (deprecated;
 * use asc_float2half plus asc_sync).
 *
 * @override_id asc_float2half_rn_sync__PUf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source float vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of elements converted, counted in elements of the source and destination vectors.
 */
__aicore__ inline void asc_float2half_rn_sync(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rna and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float elements from src into half elements at dst using ROUND (round half away from zero)
 * (deprecated; use asc_float2half plus asc_sync).
 *
 * @override_id asc_float2half_rna_sync__PUf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source float vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of elements converted, counted in elements of the source and destination vectors.
 */
__aicore__ inline void asc_float2half_rna_sync(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rd and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float elements from src into half elements at dst using FLOOR (round toward minus infinity)
 * (deprecated; use asc_float2half plus asc_sync).
 *
 * @override_id asc_float2half_rd_sync__PUf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source float vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of elements converted, counted in elements of the source and destination vectors.
 */
__aicore__ inline void asc_float2half_rd_sync(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_ru and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float elements from src into half elements at dst using CEIL (round toward plus infinity)
 * (deprecated; use asc_float2half plus asc_sync).
 *
 * @override_id asc_float2half_ru_sync__PUf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source float vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of elements converted, counted in elements of the source and destination vectors.
 */
__aicore__ inline void asc_float2half_ru_sync(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float elements from src into half elements at dst using TRUNC (round toward zero) (deprecated;
 * use asc_float2half plus asc_sync).
 *
 * @override_id asc_float2half_rz_sync__PUf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source float vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of elements converted, counted in elements of the source and destination vectors.
 */
__aicore__ inline void asc_float2half_rz_sync(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_ro and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float elements from src into half elements at dst using ODD (Von Neumann rounding, nearest odd)
 * (deprecated; use asc_float2half plus asc_sync).
 *
 * @override_id asc_float2half_ro_sync__PUf16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source float vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of elements converted, counted in elements of the source and destination vectors.
 */
__aicore__ inline void asc_float2half_ro_sync(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int32_rna and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into 32-bit signed integer (int32_t) in dst using ROUND rounding
 * (round half away from zero) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2int32_rna plus
 * asc_sync.
 *
 * @override_id asc_float2int32_rna_sync__PUi32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the 32-bit signed integer (int32_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2int32_rna_sync(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int32_ru and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into 32-bit signed integer (int32_t) in dst using CEIL rounding
 * (round toward positive infinity) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2int32_ru plus
 * asc_sync.
 *
 * @override_id asc_float2int32_ru_sync__PUi32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the 32-bit signed integer (int32_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2int32_ru_sync(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int32_rd and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into 32-bit signed integer (int32_t) in dst using FLOOR rounding
 * (round toward negative infinity) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2int32_rd plus
 * asc_sync.
 *
 * @override_id asc_float2int32_rd_sync__PUi32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the 32-bit signed integer (int32_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2int32_rd_sync(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int32_rn and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into 32-bit signed integer (int32_t) in dst using RINT rounding
 * (round half to even) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2int32_rn plus asc_sync.
 *
 * @override_id asc_float2int32_rn_sync__PUi32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the 32-bit signed integer (int32_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2int32_rn_sync(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int32_rz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into 32-bit signed integer (int32_t) in dst using TRUNC rounding
 * (round toward zero) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2int32_rz plus asc_sync.
 *
 * @override_id asc_float2int32_rz_sync__PUi32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the 32-bit signed integer (int32_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2int32_rz_sync(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rna and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into 64-bit signed integer (int64_t) in dst using ROUND rounding
 * (round half away from zero) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2int64_rna plus
 * asc_sync.
 *
 * @override_id asc_float2int64_rna_sync__PUi64_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the 64-bit signed integer (int64_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2int64_rna_sync(__ubuf__ int64_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_ru and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into 64-bit signed integer (int64_t) in dst using CEIL rounding
 * (round toward positive infinity) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2int64_ru plus
 * asc_sync.
 *
 * @override_id asc_float2int64_ru_sync__PUi64_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the 64-bit signed integer (int64_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2int64_ru_sync(__ubuf__ int64_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rd and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into 64-bit signed integer (int64_t) in dst using FLOOR rounding
 * (round toward negative infinity) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2int64_rd plus
 * asc_sync.
 *
 * @override_id asc_float2int64_rd_sync__PUi64_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the 64-bit signed integer (int64_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2int64_rd_sync(__ubuf__ int64_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rn and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into 64-bit signed integer (int64_t) in dst using RINT rounding
 * (round half to even) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2int64_rn plus asc_sync.
 *
 * @override_id asc_float2int64_rn_sync__PUi64_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the 64-bit signed integer (int64_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2int64_rn_sync(__ubuf__ int64_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into 64-bit signed integer (int64_t) in dst using TRUNC rounding
 * (round toward zero) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2int64_rz plus asc_sync.
 *
 * @override_id asc_float2int64_rz_sync__PUi64_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the 64-bit signed integer (int64_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2int64_rz_sync(__ubuf__ int64_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2float and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous conversion of half elements to float with no rounding mode.
 *
 * @override_id asc_half2float_sync__PUf32_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector, which must be 32-byte aligned.
 * @param [in] src Start address of the source half vector, which must be 32-byte aligned.
 * @param [in] count Number of elements participating in the contiguous computation.
 */
__aicore__ inline void asc_half2float_sync(__ubuf__ float* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half elements from src to packed int4 values at dst with the default mode (RINT when precision
 * is lost, otherwise no rounding) (deprecated; use asc_half2int4 plus asc_sync).
 *
 * @override_id asc_half2int4_sync__PUi4x2_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination packed int4 vector in Unified Buffer; must be 32-byte aligned. Two
 * int4 values share one byte, so the doc requires count to be even for this count-based interface.
 * @param [in] src Start address of the source half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of half elements converted, counted in source elements; because the packed destination
 * stores two int4 values per byte, the doc requires this value to be even.
 */
__aicore__ inline void asc_half2int4_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4_rna and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half elements from src to packed int4 values at dst with ROUND rounding (round half away from
 * zero) (deprecated; use asc_half2int4 plus asc_sync).
 *
 * @override_id asc_half2int4_rna_sync__PUi4x2_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination packed int4 vector in Unified Buffer; must be 32-byte aligned. Two
 * int4 values share one byte, so the doc requires count to be even for this count-based interface.
 * @param [in] src Start address of the source half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of half elements converted, counted in source elements; because the packed destination
 * stores two int4 values per byte, the doc requires this value to be even.
 */
__aicore__ inline void asc_half2int4_rna_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4_ru and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half elements from src to packed int4 values at dst with CEIL rounding (toward plus infinity)
 * (deprecated; use asc_half2int4 plus asc_sync).
 *
 * @override_id asc_half2int4_ru_sync__PUi4x2_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination packed int4 vector in Unified Buffer; must be 32-byte aligned. Two
 * int4 values share one byte, so the doc requires count to be even for this count-based interface.
 * @param [in] src Start address of the source half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of half elements converted, counted in source elements; because the packed destination
 * stores two int4 values per byte, the doc requires this value to be even.
 */
__aicore__ inline void asc_half2int4_ru_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4_rd and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half elements from src to packed int4 values at dst with FLOOR rounding (toward minus infinity)
 * (deprecated; use asc_half2int4 plus asc_sync).
 *
 * @override_id asc_half2int4_rd_sync__PUi4x2_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination packed int4 vector in Unified Buffer; must be 32-byte aligned. Two
 * int4 values share one byte, so the doc requires count to be even for this count-based interface.
 * @param [in] src Start address of the source half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of half elements converted, counted in source elements; because the packed destination
 * stores two int4 values per byte, the doc requires this value to be even.
 */
__aicore__ inline void asc_half2int4_rd_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4_rn and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half elements from src to packed int4 values at dst with RINT rounding (round half to even)
 * (deprecated; use asc_half2int4 plus asc_sync).
 *
 * @override_id asc_half2int4_rn_sync__PUi4x2_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination packed int4 vector in Unified Buffer; must be 32-byte aligned. Two
 * int4 values share one byte, so the doc requires count to be even for this count-based interface.
 * @param [in] src Start address of the source half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of half elements converted, counted in source elements; because the packed destination
 * stores two int4 values per byte, the doc requires this value to be even.
 */
__aicore__ inline void asc_half2int4_rn_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4_rz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half elements from src to packed int4 values at dst with TRUNC rounding (toward zero)
 * (deprecated; use asc_half2int4 plus asc_sync).
 *
 * @override_id asc_half2int4_rz_sync__PUi4x2_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination packed int4 vector in Unified Buffer; must be 32-byte aligned. Two
 * int4 values share one byte, so the doc requires count to be even for this count-based interface.
 * @param [in] src Start address of the source half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of half elements converted, counted in source elements; because the packed destination
 * stores two int4 values per byte, the doc requires this value to be even.
 */
__aicore__ inline void asc_half2int4_rz_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half elements from src to int8_t values at dst with the default mode, which the doc lists as no
 * rounding mode specified (deprecated; use asc_half2int8 plus asc_sync).
 *
 * @override_id asc_half2int8_sync__PUi8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination int8_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of half elements converted, counted in source elements; it also equals the number of int8_t
 * values written to dst.
 */
__aicore__ inline void asc_half2int8_sync(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rna and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half elements from src to int8_t values at dst with ROUND rounding (round half away from zero)
 * (deprecated; use asc_half2int8 plus asc_sync).
 *
 * @override_id asc_half2int8_rna_sync__PUi8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination int8_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of half elements converted, counted in source elements; it also equals the number of int8_t
 * values written to dst.
 */
__aicore__ inline void asc_half2int8_rna_sync(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_ru and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half elements from src to int8_t values at dst with CEIL rounding (toward plus infinity)
 * (deprecated; use asc_half2int8 plus asc_sync).
 *
 * @override_id asc_half2int8_ru_sync__PUi8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination int8_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of half elements converted, counted in source elements; it also equals the number of int8_t
 * values written to dst.
 */
__aicore__ inline void asc_half2int8_ru_sync(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rd and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half elements from src to int8_t values at dst with FLOOR rounding (toward minus infinity)
 * (deprecated; use asc_half2int8 plus asc_sync).
 *
 * @override_id asc_half2int8_rd_sync__PUi8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination int8_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of half elements converted, counted in source elements; it also equals the number of int8_t
 * values written to dst.
 */
__aicore__ inline void asc_half2int8_rd_sync(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rn and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half elements from src to int8_t values at dst with RINT rounding (round half to even)
 * (deprecated; use asc_half2int8 plus asc_sync).
 *
 * @override_id asc_half2int8_rn_sync__PUi8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination int8_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of half elements converted, counted in source elements; it also equals the number of int8_t
 * values written to dst.
 */
__aicore__ inline void asc_half2int8_rn_sync(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half elements from src to int8_t values at dst with TRUNC rounding (toward zero) (deprecated;
 * use asc_half2int8 plus asc_sync).
 *
 * @override_id asc_half2int8_rz_sync__PUi8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination int8_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of half elements converted, counted in source elements; it also equals the number of int8_t
 * values written to dst.
 */
__aicore__ inline void asc_half2int8_rz_sync(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int16_rna and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half (fp16) elements from src into 16-bit signed integer (int16_t) in dst using ROUND rounding
 * (round half away from zero) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_half2int16_rna plus
 * asc_sync.
 *
 * @override_id asc_half2int16_rna_sync__PUi16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the 16-bit signed integer (int16_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the half (fp16) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_half2int16_rna_sync(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int16_ru and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half (fp16) elements from src into 16-bit signed integer (int16_t) in dst using CEIL rounding
 * (round toward positive infinity) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_half2int16_ru plus
 * asc_sync.
 *
 * @override_id asc_half2int16_ru_sync__PUi16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the 16-bit signed integer (int16_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the half (fp16) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_half2int16_ru_sync(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int16_rd and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half (fp16) elements from src into 16-bit signed integer (int16_t) in dst using FLOOR rounding
 * (round toward negative infinity) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_half2int16_rd plus
 * asc_sync.
 *
 * @override_id asc_half2int16_rd_sync__PUi16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the 16-bit signed integer (int16_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the half (fp16) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_half2int16_rd_sync(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int16_rn and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half (fp16) elements from src into 16-bit signed integer (int16_t) in dst using RINT rounding
 * (round half to even) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_half2int16_rn plus asc_sync.
 *
 * @override_id asc_half2int16_rn_sync__PUi16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the 16-bit signed integer (int16_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the half (fp16) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_half2int16_rn_sync(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int16_rz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half (fp16) elements from src into 16-bit signed integer (int16_t) in dst using TRUNC rounding
 * (round toward zero) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_half2int16_rz plus asc_sync.
 *
 * @override_id asc_half2int16_rz_sync__PUi16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the 16-bit signed integer (int16_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the half (fp16) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_half2int16_rz_sync(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int32_rna and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half (fp16) elements from src into 32-bit signed integer (int32_t) in dst using ROUND rounding
 * (round half away from zero) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_half2int32_rna plus
 * asc_sync.
 *
 * @override_id asc_half2int32_rna_sync__PUi32_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the 32-bit signed integer (int32_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the half (fp16) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_half2int32_rna_sync(__ubuf__ int32_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int32_ru and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half (fp16) elements from src into 32-bit signed integer (int32_t) in dst using CEIL rounding
 * (round toward positive infinity) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_half2int32_ru plus
 * asc_sync.
 *
 * @override_id asc_half2int32_ru_sync__PUi32_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the 32-bit signed integer (int32_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the half (fp16) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_half2int32_ru_sync(__ubuf__ int32_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int32_rd and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half (fp16) elements from src into 32-bit signed integer (int32_t) in dst using FLOOR rounding
 * (round toward negative infinity) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_half2int32_rd plus
 * asc_sync.
 *
 * @override_id asc_half2int32_rd_sync__PUi32_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the 32-bit signed integer (int32_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the half (fp16) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_half2int32_rd_sync(__ubuf__ int32_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int32_rn and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half (fp16) elements from src into 32-bit signed integer (int32_t) in dst using RINT rounding
 * (round half to even) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_half2int32_rn plus asc_sync.
 *
 * @override_id asc_half2int32_rn_sync__PUi32_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the 32-bit signed integer (int32_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the half (fp16) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_half2int32_rn_sync(__ubuf__ int32_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int32_rz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half (fp16) elements from src into 32-bit signed integer (int32_t) in dst using TRUNC rounding
 * (round toward zero) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_half2int32_rz plus asc_sync.
 *
 * @override_id asc_half2int32_rz_sync__PUi32_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the 32-bit signed integer (int32_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the half (fp16) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_half2int32_rz_sync(__ubuf__ int32_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_div and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous wrapper that divides src0 by src1 element-wise per half element and waits for the
 * vector pipe.
 *
 * @override_id asc_div_sync__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector receiving the quotient; must be 32-byte aligned.
 * @param [in] src0 Start address of the dividend half vector; must be 32-byte aligned.
 * @param [in] src1 Start address of the divisor half vector; must be 32-byte aligned.
 * @param [in] count Number of elements participating in the consecutive computation.
 */
__aicore__ inline void asc_div_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_div and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous wrapper that divides src0 by src1 element-wise per float element and waits for the
 * vector pipe.
 *
 * @override_id asc_div_sync__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector receiving the quotient; must be 32-byte aligned.
 * @param [in] src0 Start address of the dividend float vector; must be 32-byte aligned.
 * @param [in] src1 Start address of the divisor float vector; must be 32-byte aligned.
 * @param [in] count Number of elements participating in the consecutive computation.
 */
__aicore__ inline void asc_div_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_duplicate and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the scalar half src into count contiguous half elements starting at dst as a PIPE_V duplicate
 * (deprecated; superseded by asc_duplicate plus asc_sync).
 *
 * @override_id asc_duplicate_sync__PUf16_f16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer, which must be 32-byte aligned; count half
 * elements are overwritten.
 * @param [in] src Scalar half value replicated into every destination element.
 * @param [in] count Number of contiguous elements to fill, counted in elements rather than bytes.
 */
__aicore__ inline void asc_duplicate_sync(__ubuf__ half* dst, half src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_duplicate and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the scalar int16_t src into count contiguous int16_t elements starting at dst as a PIPE_V duplicate
 * (deprecated; superseded by asc_duplicate plus asc_sync).
 *
 * @override_id asc_duplicate_sync__PUi16_i16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer, which must be 32-byte aligned; count
 * int16_t elements are overwritten.
 * @param [in] src Scalar int16_t value replicated into every destination element.
 * @param [in] count Number of contiguous elements to fill, counted in elements rather than bytes.
 */
__aicore__ inline void asc_duplicate_sync(__ubuf__ int16_t* dst, int16_t src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_duplicate and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the scalar uint16_t src into count contiguous uint16_t elements starting at dst as a PIPE_V
 * duplicate (deprecated; superseded by asc_duplicate plus asc_sync).
 *
 * @override_id asc_duplicate_sync__PUu16_u16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer, which must be 32-byte aligned; count
 * uint16_t elements are overwritten.
 * @param [in] src Scalar uint16_t value replicated into every destination element.
 * @param [in] count Number of contiguous elements to fill, counted in elements rather than bytes.
 */
__aicore__ inline void asc_duplicate_sync(__ubuf__ uint16_t* dst, uint16_t src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_duplicate and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the scalar bfloat16_t src into count contiguous bfloat16_t elements starting at dst as a PIPE_V
 * duplicate (deprecated; superseded by asc_duplicate plus asc_sync).
 *
 * @override_id asc_duplicate_sync__PUbf16_bf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer, which must be 32-byte aligned; count
 * bfloat16_t elements are overwritten.
 * @param [in] src Scalar bfloat16_t value replicated into every destination element.
 * @param [in] count Number of contiguous elements to fill, counted in elements rather than bytes.
 */
__aicore__ inline void asc_duplicate_sync(__ubuf__ bfloat16_t* dst, bfloat16_t src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_duplicate and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the scalar float src into count contiguous float elements starting at dst as a PIPE_V duplicate
 * (deprecated; superseded by asc_duplicate plus asc_sync).
 *
 * @override_id asc_duplicate_sync__PUf32_f32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer, which must be 32-byte aligned; count
 * float elements are overwritten.
 * @param [in] src Scalar float value replicated into every destination element.
 * @param [in] count Number of contiguous elements to fill, counted in elements rather than bytes.
 */
__aicore__ inline void asc_duplicate_sync(__ubuf__ float* dst, float src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_duplicate and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the scalar int32_t src into count contiguous int32_t elements starting at dst as a PIPE_V duplicate
 * (deprecated; superseded by asc_duplicate plus asc_sync).
 *
 * @override_id asc_duplicate_sync__PUi32_i32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer, which must be 32-byte aligned; count
 * int32_t elements are overwritten.
 * @param [in] src Scalar int32_t value replicated into every destination element.
 * @param [in] count Number of contiguous elements to fill, counted in elements rather than bytes.
 */
__aicore__ inline void asc_duplicate_sync(__ubuf__ int32_t* dst, int32_t src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_duplicate and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Broadcasts the scalar uint32_t src into count contiguous uint32_t elements starting at dst as a PIPE_V
 * duplicate (deprecated; superseded by asc_duplicate plus asc_sync).
 *
 * @override_id asc_duplicate_sync__PUu32_u32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer, which must be 32-byte aligned; count
 * uint32_t elements are overwritten.
 * @param [in] src Scalar uint32_t value replicated into every destination element.
 * @param [in] count Number of contiguous elements to fill, counted in elements rather than bytes.
 */
__aicore__ inline void asc_duplicate_sync(__ubuf__ uint32_t* dst, uint32_t src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_exp and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous wrapper that computes the natural exponential of each element per half element and
 * waits for the vector pipe.
 *
 * @override_id asc_exp_sync__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector receiving the result; must be 32-byte aligned.
 * @param [in] src Start address of the source half vector; must be 32-byte aligned.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_exp_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_exp and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous wrapper that computes the natural exponential of each element per float element and
 * waits for the vector pipe.
 *
 * @override_id asc_exp_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector receiving the result; must be 32-byte aligned.
 * @param [in] src Start address of the source float vector; must be 32-byte aligned.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_exp_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_log and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous wrapper that computes the natural logarithm of each element per half element and waits
 * for the vector pipe.
 *
 * @override_id asc_log_sync__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector receiving the result; must be 32-byte aligned.
 * @param [in] src Start address of the source half vector; must be 32-byte aligned.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_log_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_log and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous wrapper that computes the natural logarithm of each element per float element and waits
 * for the vector pipe.
 *
 * @override_id asc_log_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector receiving the result; must be 32-byte aligned.
 * @param [in] src Start address of the source float vector; must be 32-byte aligned.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_log_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gt_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous comparison wrapper: tests src > value per half element, packs one result bit per
 * element into dst and waits for the vector pipe.
 *
 * @override_id asc_gt_scalar_sync__PUu8_PUf16_f16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination buffer holding one comparison bit per source element in
 * little-endian order; must be 32-byte aligned.
 * @param [in] src Start address of the source half vector compared element-wise with value; must be 32-byte aligned.
 * @param [in] value Scalar half value each element is compared against.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of dst.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of src.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of dst in adjacent iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of src in adjacent iterations.
 */
__aicore__ inline void asc_gt_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gt_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous comparison wrapper: tests src > value per float element, packs one result bit per
 * element into dst and waits for the vector pipe.
 *
 * @override_id asc_gt_scalar_sync__PUu8_PUf32_f32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination buffer holding one comparison bit per source element in
 * little-endian order; must be 32-byte aligned.
 * @param [in] src Start address of the source float vector compared element-wise with value; must be 32-byte aligned.
 * @param [in] value Scalar float value each element is compared against.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of dst.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of src.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of dst in adjacent iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of src in adjacent iterations.
 */
__aicore__ inline void asc_gt_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_abs and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous wrapper that computes the absolute value of each element of src and waits for the
 * vector pipe.
 *
 * @override_id asc_abs_sync__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector receiving the result; must be 32-byte aligned.
 * @param [in] src Start address of the source half vector; must be 32-byte aligned.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_abs_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_abs and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous wrapper that computes the absolute value of each element of src and waits for the
 * vector pipe.
 *
 * @override_id asc_abs_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector receiving the result; must be 32-byte aligned.
 * @param [in] src Start address of the source float vector; must be 32-byte aligned.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_abs_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_shiftleft and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Shifts every element of src left by distance bits for count elements and writes the results to dst as one
 * synchronized composite call.
 *
 * @override_id asc_shiftleft_sync__PUi16_PUi16_u32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src Start address of the source vector operand (int16_t elements).
 * @param [in] distance Number of bits each element is shifted left; it must not be negative and must not exceed the bit
 * width of the src element type.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_shiftleft_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t distance, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_shiftleft and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Shifts every element of src left by distance bits for count elements and writes the results to dst as one
 * synchronized composite call.
 *
 * @override_id asc_shiftleft_sync__PUu16_PUu16_u32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src Start address of the source vector operand (uint16_t elements).
 * @param [in] distance Number of bits each element is shifted left; it must not be negative and must not exceed the bit
 * width of the src element type.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_shiftleft_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t distance, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_shiftleft and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Shifts every element of src left by distance bits for count elements and writes the results to dst as one
 * synchronized composite call.
 *
 * @override_id asc_shiftleft_sync__PUi32_PUi32_u32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src Start address of the source vector operand (int32_t elements).
 * @param [in] distance Number of bits each element is shifted left; it must not be negative and must not exceed the bit
 * width of the src element type.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_shiftleft_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t distance, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_shiftleft and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Shifts every element of src left by distance bits for count elements and writes the results to dst as one
 * synchronized composite call.
 *
 * @override_id asc_shiftleft_sync__PUu32_PUu32_u32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src Start address of the source vector operand (uint32_t elements).
 * @param [in] distance Number of bits each element is shifted left; it must not be negative and must not exceed the bit
 * width of the src element type.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_shiftleft_sync(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t distance, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_cast_half2int8 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous element-wise product of two half vectors, cast to int8_t with RINT rounding and
 * saturation.
 *
 * @override_id asc_mul_cast_half2int8_sync__PUi8_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination int8_t vector, which must be 32-byte aligned.
 * @param [in] src0 Start address of the first source half vector, which must be 32-byte aligned.
 * @param [in] src1 Start address of the second source half vector, which must be 32-byte aligned.
 * @param [in] count Number of elements participating in the computation.
 */
__aicore__ inline void asc_mul_cast_half2int8_sync(
    __ubuf__ int8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_cast_half2uint8 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous element-wise product of two half vectors, cast to uint8_t with RINT rounding and
 * saturation.
 *
 * @override_id asc_mul_cast_half2uint8_sync__PUu8_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination uint8_t vector, which must be 32-byte aligned.
 * @param [in] src0 Start address of source vector 0 of half elements, which must be 32-byte aligned.
 * @param [in] src1 Start address of source vector 1 of half elements, which must be 32-byte aligned.
 * @param [in] count Number of elements participating in the computation.
 */
__aicore__ inline void asc_mul_cast_half2uint8_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ne and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 elementwise on the relation src0 != src1 in a synchronized composite call and leaves
 * the 1-bit-per-element result in the comparison register for asc_get_cmp_mask, processing a fixed 256 B per call.
 *
 * @override_id asc_ne_sync__PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in] src0 Start address of source operand 0 (half elements).
 * @param [in] src1 Start address of source operand 1 (half elements).
 * @param [in] repeat Number of iterations the comparison repeats; the page states that repeat has no effect in the
 * register-result form.
 * @param [in] dst_block_stride Address stride between the DataBlocks of the destination within a single iteration.
 * @param [in] src0_block_stride Address stride between the DataBlocks of source operand 0 within a single iteration.
 * @param [in] src1_block_stride Address stride between the DataBlocks of source operand 1 within a single iteration.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of the destination across adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of source operand 0 across adjacent
 * iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of source operand 1 across adjacent
 * iterations.
 */
__aicore__ inline void asc_ne_sync(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ne and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 elementwise on the relation src0 != src1 in a synchronized composite call and leaves
 * the 1-bit-per-element result in the comparison register for asc_get_cmp_mask, processing a fixed 256 B per call.
 *
 * @override_id asc_ne_sync__PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in] src0 Start address of source operand 0 (float elements).
 * @param [in] src1 Start address of source operand 1 (float elements).
 * @param [in] repeat Number of iterations the comparison repeats; the page states that repeat has no effect in the
 * register-result form.
 * @param [in] dst_block_stride Address stride between the DataBlocks of the destination within a single iteration.
 * @param [in] src0_block_stride Address stride between the DataBlocks of source operand 0 within a single iteration.
 * @param [in] src1_block_stride Address stride between the DataBlocks of source operand 1 within a single iteration.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of the destination across adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of source operand 0 across adjacent
 * iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of source operand 1 across adjacent
 * iterations.
 */
__aicore__ inline void asc_ne_sync(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ne and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 elementwise on the relation src0 != src1 in a synchronized composite call and writes
 * the packed 1-bit-per-element result to the uint8_t buffer dst in little-endian bit order.
 *
 * @override_id asc_ne_sync__PUu8_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination buffer that receives the packed 1-bit-per-element comparison result
 * in little-endian bit order.
 * @param [in] src0 Start address of source operand 0 (half elements).
 * @param [in] src1 Start address of source operand 1 (half elements).
 * @param [in] repeat Number of iterations the comparison repeats across repeat-sized blocks.
 * @param [in] dst_block_stride Address stride between the DataBlocks of the destination within a single iteration.
 * @param [in] src0_block_stride Address stride between the DataBlocks of source operand 0 within a single iteration.
 * @param [in] src1_block_stride Address stride between the DataBlocks of source operand 1 within a single iteration.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of the destination across adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of source operand 0 across adjacent
 * iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of source operand 1 across adjacent
 * iterations.
 */
__aicore__ inline void asc_ne_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ne and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 elementwise on the relation src0 != src1 in a synchronized composite call and writes
 * the packed 1-bit-per-element result to the uint8_t buffer dst in little-endian bit order.
 *
 * @override_id asc_ne_sync__PUu8_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination buffer that receives the packed 1-bit-per-element comparison result
 * in little-endian bit order.
 * @param [in] src0 Start address of source operand 0 (float elements).
 * @param [in] src1 Start address of source operand 1 (float elements).
 * @param [in] repeat Number of iterations the comparison repeats across repeat-sized blocks.
 * @param [in] dst_block_stride Address stride between the DataBlocks of the destination within a single iteration.
 * @param [in] src0_block_stride Address stride between the DataBlocks of source operand 0 within a single iteration.
 * @param [in] src1_block_stride Address stride between the DataBlocks of source operand 1 within a single iteration.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of the destination across adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of source operand 0 across adjacent
 * iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of source operand 1 across adjacent
 * iterations.
 */
__aicore__ inline void asc_ne_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ge_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous comparison wrapper: tests src >= value per half element, packs one result bit per
 * element into dst and waits for the vector pipe.
 *
 * @override_id asc_ge_scalar_sync__PUu8_PUf16_f16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination buffer holding one comparison bit per source element in
 * little-endian order; must be 32-byte aligned.
 * @param [in] src Start address of the source half vector compared element-wise with value; must be 32-byte aligned.
 * @param [in] value Scalar half value each element is compared against.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of dst.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of src.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of dst in adjacent iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of src in adjacent iterations.
 */
__aicore__ inline void asc_ge_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ge_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous comparison wrapper: tests src >= value per float element, packs one result bit per
 * element into dst and waits for the vector pipe.
 *
 * @override_id asc_ge_scalar_sync__PUu8_PUf32_f32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination buffer holding one comparison bit per source element in
 * little-endian order; must be 32-byte aligned.
 * @param [in] src Start address of the source float vector compared element-wise with value; must be 32-byte aligned.
 * @param [in] value Scalar float value each element is compared against.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of dst.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of src.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of dst in adjacent iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of src in adjacent iterations.
 */
__aicore__ inline void asc_ge_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_deq_int322half and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form: dequantizes int32_t input elements to half using the configured dequantization
 * scale.
 *
 * @override_id asc_deq_int322half_sync__PUf16_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector of half elements, which must be 32-byte aligned.
 * @param [in] src Start address of the source vector of int32_t elements, which must be 32-byte aligned.
 * @param [in] count Number of elements participating in the dequantization.
 */
__aicore__ inline void asc_deq_int322half_sync(__ubuf__ half* dst, __ubuf__ int32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mrgsort4 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously merges up to four already sorted queues of half records into one queue ordered by descending
 * score and writes the merged records to dst.
 *
 * @override_id asc_mrgsort4_sync__PUf16_AmrgPUf16_u8_u16_u16_u16_u16_b_u8
 * @archs 2201 3510
 * @param [out] dst Destination vector start address in UB; must be 32-byte aligned.
 * @param [in] src Array of four source queue start addresses (ASC_C_API_MRGSORT_ELEMENT_LEN is 4), each 8-byte aligned;
 * all four entries must be initialized even when a queue is not selected by valid_bit.
 * @param [in] repeat Number of iterations; each iteration merges one group of up to four queues and advances past the
 * queues' total length.
 * @param [in] element_length_0 Length of the queue starting at src[0], counted in 8-byte region-proposal records that
 * hold one score plus one index.
 * @param [in] element_length_1 Length of the queue starting at src[1], counted in 8-byte region-proposal records.
 * @param [in] element_length_2 Length of the queue starting at src[2], counted in 8-byte region-proposal records.
 * @param [in] element_length_3 Length of the queue starting at src[3], counted in 8-byte region-proposal records.
 * @param [in] if_exhausted_suspension Whether the instruction stops once one queue is exhausted (true) or keeps merging
 * the remaining valid queues (false).
 * @param [in] valid_bit Valid-queue mask: 3 keeps the first two queues valid, 7 the first three, and 15 all four
 * queues.
 */
__aicore__ inline void asc_mrgsort4_sync(
    __ubuf__ half* dst, __ubuf__ half* src[ASC_C_API_MRGSORT_ELEMENT_LEN], uint8_t repeat, uint16_t element_length_0,
    uint16_t element_length_1, uint16_t element_length_2, uint16_t element_length_3, bool if_exhausted_suspension,
    uint8_t valid_bit);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mrgsort4 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously merges up to four already sorted queues of float records into one queue ordered by descending
 * score and writes the merged records to dst.
 *
 * @override_id asc_mrgsort4_sync__PUf32_AmrgPUf32_u8_u16_u16_u16_u16_b_u8
 * @archs 2201 3510
 * @param [out] dst Destination vector start address in UB; must be 32-byte aligned.
 * @param [in] src Array of four source queue start addresses (ASC_C_API_MRGSORT_ELEMENT_LEN is 4), each 8-byte aligned;
 * all four entries must be initialized even when a queue is not selected by valid_bit.
 * @param [in] repeat Number of iterations; each iteration merges one group of up to four queues and advances past the
 * queues' total length.
 * @param [in] element_length_0 Length of the queue starting at src[0], counted in 8-byte region-proposal records
 * holding a float score plus a 4-byte index.
 * @param [in] element_length_1 Length of the queue starting at src[1], counted in 8-byte region-proposal records.
 * @param [in] element_length_2 Length of the queue starting at src[2], counted in 8-byte region-proposal records.
 * @param [in] element_length_3 Length of the queue starting at src[3], counted in 8-byte region-proposal records.
 * @param [in] if_exhausted_suspension Whether the instruction stops once one queue is exhausted (true) or keeps merging
 * the remaining valid queues (false).
 * @param [in] valid_bit Valid-queue mask: 3 keeps the first two queues valid, 7 the first three, and 15 all four
 * queues.
 */
__aicore__ inline void asc_mrgsort4_sync(
    __ubuf__ float* dst, __ubuf__ float* src[ASC_C_API_MRGSORT_ELEMENT_LEN], uint8_t repeat, uint16_t element_length_0,
    uint16_t element_length_1, uint16_t element_length_2, uint16_t element_length_3, bool if_exhausted_suspension,
    uint8_t valid_bit);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_relu and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous ReLU wrapper that writes max(src, 0) per half element and waits for the vector pipe.
 *
 * @override_id asc_relu_sync__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector receiving the rectified values; must be 32-byte
 * aligned.
 * @param [in] src Start address of the source half vector; must be 32-byte aligned.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_relu_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_relu and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous ReLU wrapper that writes max(src, 0) per float element and waits for the vector pipe.
 *
 * @override_id asc_relu_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector receiving the rectified values; must be 32-byte
 * aligned.
 * @param [in] src Start address of the source float vector; must be 32-byte aligned.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_relu_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_relu and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous ReLU wrapper that writes max(src, 0) per int32_t element and waits for the vector pipe.
 *
 * @override_id asc_relu_sync__PUi32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination int32_t vector receiving the rectified values; must be 32-byte
 * aligned.
 * @param [in] src Start address of the source int32_t vector; must be 32-byte aligned.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_relu_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_max and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise maximum of src0 and src1 for count elements and writes the results to dst as one
 * synchronized composite call.
 *
 * @override_id asc_max_sync__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src0 Start address of source vector operand 0 (half elements).
 * @param [in] src1 Start address of source vector operand 1 (half elements).
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_max_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_max and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise maximum of src0 and src1 for count elements and writes the results to dst as one
 * synchronized composite call.
 *
 * @override_id asc_max_sync__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src0 Start address of source vector operand 0 (float elements).
 * @param [in] src1 Start address of source vector operand 1 (float elements).
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_max_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_max and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise maximum of src0 and src1 for count elements and writes the results to dst as one
 * synchronized composite call.
 *
 * @override_id asc_max_sync__PUi16_PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src0 Start address of source vector operand 0 (int16_t elements).
 * @param [in] src1 Start address of source vector operand 1 (int16_t elements).
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_max_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_max and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise maximum of src0 and src1 for count elements and writes the results to dst as one
 * synchronized composite call.
 *
 * @override_id asc_max_sync__PUi32_PUi32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src0 Start address of source vector operand 0 (int32_t elements).
 * @param [in] src1 Start address of source vector operand 1 (int32_t elements).
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_max_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_max_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise maximum between each element of src and the scalar value for count elements and
 * writes the results to dst as one synchronized composite call.
 *
 * @override_id asc_max_scalar_sync__PUf16_PUf16_f16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src Start address of the source vector operand (half elements).
 * @param [in] value Scalar half operand applied to every element.
 * @param [in] count Number of elements that participate in the contiguous computation.
 */
__aicore__ inline void asc_max_scalar_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_max_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise maximum between each element of src and the scalar value for count elements and
 * writes the results to dst as one synchronized composite call.
 *
 * @override_id asc_max_scalar_sync__PUf32_PUf32_f32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src Start address of the source vector operand (float elements).
 * @param [in] value Scalar float operand applied to every element.
 * @param [in] count Number of elements that participate in the contiguous computation.
 */
__aicore__ inline void asc_max_scalar_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_max_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise maximum between each element of src and the scalar value for count elements and
 * writes the results to dst as one synchronized composite call.
 *
 * @override_id asc_max_scalar_sync__PUi16_PUi16_i16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src Start address of the source vector operand (int16_t elements).
 * @param [in] value Scalar int16_t operand applied to every element.
 * @param [in] count Number of elements that participate in the contiguous computation.
 */
__aicore__ inline void asc_max_scalar_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_max_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise maximum between each element of src and the scalar value for count elements and
 * writes the results to dst as one synchronized composite call.
 *
 * @override_id asc_max_scalar_sync__PUi32_PUi32_i32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src Start address of the source vector operand (int32_t elements).
 * @param [in] value Scalar int32_t operand applied to every element.
 * @param [in] count Number of elements that participate in the contiguous computation.
 */
__aicore__ inline void asc_max_scalar_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_min and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise minimum of src0 and src1 for count elements and writes the results to dst as one
 * synchronized composite call.
 *
 * @override_id asc_min_sync__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src0 Start address of source vector operand 0 (half elements).
 * @param [in] src1 Start address of source vector operand 1 (half elements).
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_min_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_min and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise minimum of src0 and src1 for count elements and writes the results to dst as one
 * synchronized composite call.
 *
 * @override_id asc_min_sync__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src0 Start address of source vector operand 0 (float elements).
 * @param [in] src1 Start address of source vector operand 1 (float elements).
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_min_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_min and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise minimum of src0 and src1 for count elements and writes the results to dst as one
 * synchronized composite call.
 *
 * @override_id asc_min_sync__PUi16_PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src0 Start address of source vector operand 0 (int16_t elements).
 * @param [in] src1 Start address of source vector operand 1 (int16_t elements).
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_min_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_min and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise minimum of src0 and src1 for count elements and writes the results to dst as one
 * synchronized composite call.
 *
 * @override_id asc_min_sync__PUi32_PUi32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src0 Start address of source vector operand 0 (int32_t elements).
 * @param [in] src1 Start address of source vector operand 1 (int32_t elements).
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_min_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fma and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous multiply-add wrapper: computes dst = src0 * src1 + dst per element and waits for the
 * vector pipe.
 *
 * @override_id asc_fma_sync__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector; it is read as the addend and written with the fused
 * result; must be 32-byte aligned.
 * @param [in] src0 Start address of the first half source vector; must be 32-byte aligned.
 * @param [in] src1 Start address of the second half source vector; must be 32-byte aligned.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_fma_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fma and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous multiply-add wrapper: computes dst = src0 * src1 + dst per element and waits for the
 * vector pipe.
 *
 * @override_id asc_fma_sync__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector; it is read as the addend and written with the fused
 * result; must be 32-byte aligned.
 * @param [in] src0 Start address of the first float source vector; must be 32-byte aligned.
 * @param [in] src1 Start address of the second float source vector; must be 32-byte aligned.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_fma_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fma and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous multiply-add wrapper: computes dst = src0 * src1 + dst per element and waits for the
 * vector pipe.
 *
 * @override_id asc_fma_sync__PUf32_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector; it is read as the addend and written with the fused
 * result; must be 32-byte aligned.
 * @param [in] src0 Start address of the first half source vector; must be 32-byte aligned.
 * @param [in] src1 Start address of the second half source vector; must be 32-byte aligned.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_fma_sync(__ubuf__ float* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise product of src0 and src1 for count elements and writes the results to dst as one
 * synchronized composite call.
 *
 * @override_id asc_mul_sync__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src0 Start address of source vector operand 0 (half elements).
 * @param [in] src1 Start address of source vector operand 1 (half elements).
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_mul_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise product of src0 and src1 for count elements and writes the results to dst as one
 * synchronized composite call.
 *
 * @override_id asc_mul_sync__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src0 Start address of source vector operand 0 (float elements).
 * @param [in] src1 Start address of source vector operand 1 (float elements).
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_mul_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise product of src0 and src1 for count elements and writes the results to dst as one
 * synchronized composite call.
 *
 * @override_id asc_mul_sync__PUi16_PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src0 Start address of source vector operand 0 (int16_t elements).
 * @param [in] src1 Start address of source vector operand 1 (int16_t elements).
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_mul_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise product of src0 and src1 for count elements and writes the results to dst as one
 * synchronized composite call.
 *
 * @override_id asc_mul_sync__PUi32_PUi32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src0 Start address of source vector operand 0 (int32_t elements).
 * @param [in] src1 Start address of source vector operand 1 (int32_t elements).
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_mul_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise product between each element of src and the scalar value for count elements and
 * writes the results to dst as one synchronized composite call.
 *
 * @override_id asc_mul_scalar_sync__PUf16_PUf16_f16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src Start address of the source vector operand (half elements).
 * @param [in] value Scalar half operand applied to every element.
 * @param [in] count Number of elements that participate in the contiguous computation.
 */
__aicore__ inline void asc_mul_scalar_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise product between each element of src and the scalar value for count elements and
 * writes the results to dst as one synchronized composite call.
 *
 * @override_id asc_mul_scalar_sync__PUf32_PUf32_f32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src Start address of the source vector operand (float elements).
 * @param [in] value Scalar float operand applied to every element.
 * @param [in] count Number of elements that participate in the contiguous computation.
 */
__aicore__ inline void asc_mul_scalar_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise product between each element of src and the scalar value for count elements and
 * writes the results to dst as one synchronized composite call.
 *
 * @override_id asc_mul_scalar_sync__PUi16_PUi16_i16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src Start address of the source vector operand (int16_t elements).
 * @param [in] value Scalar int16_t operand applied to every element.
 * @param [in] count Number of elements that participate in the contiguous computation.
 */
__aicore__ inline void asc_mul_scalar_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise product between each element of src and the scalar value for count elements and
 * writes the results to dst as one synchronized composite call.
 *
 * @override_id asc_mul_scalar_sync__PUi32_PUi32_i32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src Start address of the source vector operand (int32_t elements).
 * @param [in] value Scalar int32_t operand applied to every element.
 * @param [in] count Number of elements that participate in the contiguous computation.
 */
__aicore__ inline void asc_mul_scalar_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_select and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously selects half elements from src0 or src1 according to the comparison mask set by asc_set_cmp_mask
 * and writes the selection to dst.
 *
 * @override_id asc_select_sync__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the selected elements.
 * @param [in] src0 Source vector 0, selected where the comparison mask bit is 1.
 * @param [in] src1 Source vector 1, selected where the comparison mask bit is 0.
 * @param [in] count Number of elements participating in the computation.
 */
__aicore__ inline void asc_select_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_select and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously selects float elements from src0 or src1 according to the comparison mask set by
 * asc_set_cmp_mask and writes the selection to dst.
 *
 * @override_id asc_select_sync__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the selected elements.
 * @param [in] src0 Source vector 0, selected where the comparison mask bit is 1.
 * @param [in] src1 Source vector 1, selected where the comparison mask bit is 0.
 * @param [in] count Number of elements participating in the computation.
 */
__aicore__ inline void asc_select_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise difference of src0 and src1 for count elements and writes the results to dst as one
 * synchronized composite call.
 *
 * @override_id asc_sub_sync__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src0 Start address of source vector operand 0 (half elements).
 * @param [in] src1 Start address of source vector operand 1 (half elements).
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_sub_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise difference of src0 and src1 for count elements and writes the results to dst as one
 * synchronized composite call.
 *
 * @override_id asc_sub_sync__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src0 Start address of source vector operand 0 (float elements).
 * @param [in] src1 Start address of source vector operand 1 (float elements).
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_sub_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise difference of src0 and src1 for count elements and writes the results to dst as one
 * synchronized composite call.
 *
 * @override_id asc_sub_sync__PUi16_PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src0 Start address of source vector operand 0 (int16_t elements).
 * @param [in] src1 Start address of source vector operand 1 (int16_t elements).
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_sub_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise difference of src0 and src1 for count elements and writes the results to dst as one
 * synchronized composite call.
 *
 * @override_id asc_sub_sync__PUi32_PUi32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src0 Start address of source vector operand 0 (int32_t elements).
 * @param [in] src1 Start address of source vector operand 1 (int32_t elements).
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_sub_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise difference between each element of src and the scalar value for count elements and
 * writes the results to dst as one synchronized composite call.
 *
 * @override_id asc_sub_scalar_sync__PUf16_PUf16_f16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src Start address of the source vector operand (half elements).
 * @param [in] value Scalar half operand applied to every element.
 * @param [in] count Number of elements that participate in the contiguous computation.
 */
__aicore__ inline void asc_sub_scalar_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise difference between each element of src and the scalar value for count elements and
 * writes the results to dst as one synchronized composite call.
 *
 * @override_id asc_sub_scalar_sync__PUf32_PUf32_f32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src Start address of the source vector operand (float elements).
 * @param [in] value Scalar float operand applied to every element.
 * @param [in] count Number of elements that participate in the contiguous computation.
 */
__aicore__ inline void asc_sub_scalar_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise difference between each element of src and the scalar value for count elements and
 * writes the results to dst as one synchronized composite call.
 *
 * @override_id asc_sub_scalar_sync__PUi16_PUi16_i16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src Start address of the source vector operand (int16_t elements).
 * @param [in] value Scalar int16_t operand applied to every element.
 * @param [in] count Number of elements that participate in the contiguous computation.
 */
__aicore__ inline void asc_sub_scalar_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise difference between each element of src and the scalar value for count elements and
 * writes the results to dst as one synchronized composite call.
 *
 * @override_id asc_sub_scalar_sync__PUi32_PUi32_i32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src Start address of the source vector operand (int32_t elements).
 * @param [in] value Scalar int32_t operand applied to every element.
 * @param [in] count Number of elements that participate in the contiguous computation.
 */
__aicore__ inline void asc_sub_scalar_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_or and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously computes the element-wise bitwise OR of int16_t vectors src0 and src1 into dst.
 *
 * @override_id asc_or_sync__PUi16_PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving src0 | src1.
 * @param [in] src0 First source vector start address of the bitwise OR.
 * @param [in] src1 Second source vector start address of the bitwise OR.
 * @param [in] count Number of elements participating in the computation.
 */
__aicore__ inline void asc_or_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_or and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously computes the element-wise bitwise OR of uint16_t vectors src0 and src1 into dst.
 *
 * @override_id asc_or_sync__PUu16_PUu16_PUu16_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving src0 | src1.
 * @param [in] src0 First source vector start address of the bitwise OR.
 * @param [in] src1 Second source vector start address of the bitwise OR.
 * @param [in] count Number of elements participating in the computation.
 */
__aicore__ inline void asc_or_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_not and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously computes the bitwise NOT of each int16_t element of src and stores the result in dst.
 *
 * @override_id asc_not_sync__PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the bitwise complement of src.
 * @param [in] src Source vector start address of int16_t elements to be inverted.
 * @param [in] count Number of elements participating in the computation.
 */
__aicore__ inline void asc_not_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_not and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously computes the bitwise NOT of each uint16_t element of src and stores the result in dst.
 *
 * @override_id asc_not_sync__PUu16_PUu16_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the bitwise complement of src.
 * @param [in] src Source vector start address of uint16_t elements to be inverted.
 * @param [in] count Number of elements participating in the computation.
 */
__aicore__ inline void asc_not_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_rcp and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously computes the reciprocal 1/src of each half element and stores the results in dst.
 *
 * @override_id asc_rcp_sync__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the reciprocal of each src element.
 * @param [in] src Source vector start address whose elements are inverted.
 * @param [in] count Number of elements participating in the computation.
 */
__aicore__ inline void asc_rcp_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_rcp and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously computes the reciprocal 1/src of each float element and stores the results in dst.
 *
 * @override_id asc_rcp_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the reciprocal of each src element.
 * @param [in] src Source vector start address whose elements are inverted.
 * @param [in] count Number of elements participating in the computation.
 */
__aicore__ inline void asc_rcp_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_shiftright and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Shifts every int16_t element of src right by value bits into dst using arithmetic shift (the sign bit is
 * copied into the vacated high bits) (deprecated; use asc_shiftright_sync plus asc_sync).
 *
 * @override_id asc_shiftright_sync__PUi16_PUi16_i32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination int16_t vector in Unified Buffer; must be 32-byte aligned and
 * receives the shifted elements.
 * @param [in] src Start address of the source int16_t vector in Unified Buffer; must be 32-byte aligned and is only
 * read.
 * @param [in] value Shift amount in bits applied to every element, passed as int32_t; the doc requires it to be
 * non-negative and not larger than the 16-bit width of int16_t.
 * @param [in] count Number of elements shifted, counted in source elements.
 */
__aicore__ inline void asc_shiftright_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int32_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_shiftright and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Shifts every int32_t element of src right by value bits into dst using arithmetic shift (the sign bit is
 * copied into the vacated high bits) (deprecated; use asc_shiftright_sync plus asc_sync).
 *
 * @override_id asc_shiftright_sync__PUi32_PUi32_i32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination int32_t vector in Unified Buffer; must be 32-byte aligned and
 * receives the shifted elements.
 * @param [in] src Start address of the source int32_t vector in Unified Buffer; must be 32-byte aligned and is only
 * read.
 * @param [in] value Shift amount in bits applied to every element, passed as int32_t; the doc requires it to be
 * non-negative and not larger than the 32-bit width of int32_t.
 * @param [in] count Number of elements shifted, counted in source elements.
 */
__aicore__ inline void asc_shiftright_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_shiftright and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Shifts every uint16_t element of src right by value bits into dst using logical shift (vacated high bits are
 * filled with zeros) (deprecated; use asc_shiftright_sync plus asc_sync).
 *
 * @override_id asc_shiftright_sync__PUu16_PUu16_u32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination uint16_t vector in Unified Buffer; must be 32-byte aligned and
 * receives the shifted elements.
 * @param [in] src Start address of the source uint16_t vector in Unified Buffer; must be 32-byte aligned and is only
 * read.
 * @param [in] value Shift amount in bits applied to every element, passed as uint32_t; the doc requires it to be
 * non-negative and not larger than the 16-bit width of uint16_t.
 * @param [in] count Number of elements shifted, counted in source elements.
 */
__aicore__ inline void asc_shiftright_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_shiftright and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Shifts every uint32_t element of src right by value bits into dst using logical shift (vacated high bits are
 * filled with zeros) (deprecated; use asc_shiftright_sync plus asc_sync).
 *
 * @override_id asc_shiftright_sync__PUu32_PUu32_u32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination uint32_t vector in Unified Buffer; must be 32-byte aligned and
 * receives the shifted elements.
 * @param [in] src Start address of the source uint32_t vector in Unified Buffer; must be 32-byte aligned and is only
 * read.
 * @param [in] value Shift amount in bits applied to every element, passed as uint32_t; the doc requires it to be
 * non-negative and not larger than the 32-bit width of uint32_t.
 * @param [in] count Number of elements shifted, counted in source elements.
 */
__aicore__ inline void asc_shiftright_sync(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_shiftright_round and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Shifts every int16_t element of src right by value bits into dst using arithmetic with rounding shift (a
 * discarded low bit of 1 rounds the shifted result up) (deprecated; use asc_shiftright_round_sync plus asc_sync).
 *
 * @override_id asc_shiftright_round_sync__PUi16_PUi16_i32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination int16_t vector in Unified Buffer; must be 32-byte aligned and
 * receives the shifted elements.
 * @param [in] src Start address of the source int16_t vector in Unified Buffer; must be 32-byte aligned and is only
 * read.
 * @param [in] value Shift amount in bits applied to every element, passed as int32_t; the doc requires it to be
 * non-negative and not larger than the 16-bit width of int16_t.
 * @param [in] count Number of elements shifted, counted in source elements.
 */
__aicore__ inline void asc_shiftright_round_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, int32_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_shiftright_round and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Shifts every int32_t element of src right by value bits into dst using arithmetic with rounding shift (a
 * discarded low bit of 1 rounds the shifted result up) (deprecated; use asc_shiftright_round_sync plus asc_sync).
 *
 * @override_id asc_shiftright_round_sync__PUi32_PUi32_i32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination int32_t vector in Unified Buffer; must be 32-byte aligned and
 * receives the shifted elements.
 * @param [in] src Start address of the source int32_t vector in Unified Buffer; must be 32-byte aligned and is only
 * read.
 * @param [in] value Shift amount in bits applied to every element, passed as int32_t; the doc requires it to be
 * non-negative and not larger than the 32-bit width of int32_t.
 * @param [in] count Number of elements shifted, counted in source elements.
 */
__aicore__ inline void asc_shiftright_round_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_add and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously computes src0 * dst + src1 element-wise and stores the half results back into dst.
 *
 * @override_id asc_mul_add_sync__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [in,out] dst Destination vector start address; read as the multiplicand of src0 and overwritten with src0 *
 * dst + src1.
 * @param [in] src0 Source vector start address providing the first multiply operand.
 * @param [in] src1 Source vector start address providing the addend.
 * @param [in] count Number of elements participating in the computation.
 */
__aicore__ inline void asc_mul_add_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_add and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously computes src0 * dst + src1 element-wise and stores the float results back into dst.
 *
 * @override_id asc_mul_add_sync__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [in,out] dst Destination vector start address; read as the multiplicand of src0 and overwritten with src0 *
 * dst + src1.
 * @param [in] src0 Source vector start address providing the first multiply operand.
 * @param [in] src1 Source vector start address providing the addend.
 * @param [in] count Number of elements participating in the computation.
 */
__aicore__ inline void asc_mul_add_sync(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_add_relu and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously computes relu(src0 * dst + src1) element-wise and stores the half results back into dst.
 *
 * @override_id asc_mul_add_relu_sync__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [in,out] dst Destination vector start address; read as the multiplicand of src0 and overwritten with relu(src0
 * * dst + src1).
 * @param [in] src0 Source vector start address providing the first multiply operand.
 * @param [in] src1 Source vector start address providing the addend.
 * @param [in] count Number of elements participating in the computation.
 */
__aicore__ inline void asc_mul_add_relu_sync(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_add_relu and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously computes relu(src0 * dst + src1) element-wise and stores the float results back into dst.
 *
 * @override_id asc_mul_add_relu_sync__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [in,out] dst Destination vector start address; read as the multiplicand of src0 and overwritten with relu(src0
 * * dst + src1).
 * @param [in] src0 Source vector start address providing the first multiply operand.
 * @param [in] src1 Source vector start address providing the addend.
 * @param [in] count Number of elements participating in the computation.
 */
__aicore__ inline void asc_mul_add_relu_sync(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_eq_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous comparison wrapper: tests src == value per half element, packs one result bit per
 * element into dst and waits for the vector pipe.
 *
 * @override_id asc_eq_scalar_sync__PUu8_PUf16_f16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination buffer holding the packed comparison bits in little-endian order;
 * must be 32-byte aligned.
 * @param [in] src Start address of the source half vector compared element-wise with value; must be 32-byte aligned.
 * @param [in] value Scalar half compared against every element of src.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of dst.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of src.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of dst in adjacent iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of src in adjacent iterations.
 */
__aicore__ inline void asc_eq_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_eq_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous comparison wrapper: tests src == value per float element, packs one result bit per
 * element into dst and waits for the vector pipe.
 *
 * @override_id asc_eq_scalar_sync__PUu8_PUf32_f32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination buffer holding the packed comparison bits in little-endian order;
 * must be 32-byte aligned.
 * @param [in] src Start address of the source float vector compared element-wise with value; must be 32-byte aligned.
 * @param [in] value Scalar float compared against every element of src.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of dst.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of src.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of dst in adjacent iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of src in adjacent iterations.
 */
__aicore__ inline void asc_eq_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_eq_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous comparison wrapper: tests src == value per int32_t element, packs one result bit per
 * element into dst and waits for the vector pipe.
 *
 * @override_id asc_eq_scalar_sync__PUu8_PUi32_i32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination buffer holding the packed comparison bits in little-endian order;
 * must be 32-byte aligned.
 * @param [in] src Start address of the source int32_t vector compared element-wise with value; must be 32-byte aligned.
 * @param [in] value Scalar int32_t compared against every element of src.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of dst.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of src.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of dst in adjacent iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of src in adjacent iterations.
 */
__aicore__ inline void asc_eq_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ int32_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_deq_int162b8_h and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Dequantizes count int16_t elements from src into 8-bit signed integer (int8_t) in dst using the scale and
 * offset configured by asc_set_deq_scale, packs the results into the high half-block of each DataBlock, and inserts a
 * PIPE_ALL barrier; deprecated in favour of asc_deq_int162b8_h plus asc_sync.
 *
 * @override_id asc_deq_int162b8_h_sync__PUi8_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the 8-bit signed integer (int8_t) destination vector in Unified Buffer that
 * receives the dequantized values in the high half-block of every DataBlock.
 * @param [in] src Start address of the int16_t source vector in Unified Buffer holding the values to dequantize.
 * @param [in] count Number of consecutive elements dequantized; the impl passes it to asc_set_mask_count_begin before
 * the dequantization instruction.
 */
__aicore__ inline void asc_deq_int162b8_h_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_deq_int162b8_h and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Dequantizes count int16_t elements from src into 8-bit unsigned integer (uint8_t) in dst using the scale and
 * offset configured by asc_set_deq_scale, packs the results into the high half-block of each DataBlock, and inserts a
 * PIPE_ALL barrier; deprecated in favour of asc_deq_int162b8_h plus asc_sync.
 *
 * @override_id asc_deq_int162b8_h_sync__PUu8_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the 8-bit unsigned integer (uint8_t) destination vector in Unified Buffer that
 * receives the dequantized values in the high half-block of every DataBlock.
 * @param [in] src Start address of the int16_t source vector in Unified Buffer holding the values to dequantize.
 * @param [in] count Number of consecutive elements dequantized; the impl passes it to asc_set_mask_count_begin before
 * the dequantization instruction.
 */
__aicore__ inline void asc_deq_int162b8_h_sync(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_deq_int162b8_l and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Dequantizes count int16_t elements from src into 8-bit signed integer (int8_t) in dst using the scale and
 * offset configured by asc_set_deq_scale, packs the results into the low half-block of each DataBlock, and inserts a
 * PIPE_ALL barrier; deprecated in favour of asc_deq_int162b8_l plus asc_sync.
 *
 * @override_id asc_deq_int162b8_l_sync__PUi8_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the 8-bit signed integer (int8_t) destination vector in Unified Buffer that
 * receives the dequantized values in the low half-block of every DataBlock.
 * @param [in] src Start address of the int16_t source vector in Unified Buffer holding the values to dequantize.
 * @param [in] count Number of consecutive elements dequantized; the impl passes it to asc_set_mask_count_begin before
 * the dequantization instruction.
 */
__aicore__ inline void asc_deq_int162b8_l_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_deq_int162b8_l and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Dequantizes count int16_t elements from src into 8-bit unsigned integer (uint8_t) in dst using the scale and
 * offset configured by asc_set_deq_scale, packs the results into the low half-block of each DataBlock, and inserts a
 * PIPE_ALL barrier; deprecated in favour of asc_deq_int162b8_l plus asc_sync.
 *
 * @override_id asc_deq_int162b8_l_sync__PUu8_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the 8-bit unsigned integer (uint8_t) destination vector in Unified Buffer that
 * receives the dequantized values in the low half-block of every DataBlock.
 * @param [in] src Start address of the int16_t source vector in Unified Buffer holding the values to dequantize.
 * @param [in] count Number of consecutive elements dequantized; the impl passes it to asc_set_mask_count_begin before
 * the dequantization instruction.
 */
__aicore__ inline void asc_deq_int162b8_l_sync(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int42half and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous conversion of packed int4b_t elements to half.
 *
 * @override_id asc_int42half_sync__PUf16_PUi4x2_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector, which must be 32-byte aligned.
 * @param [in] src Start address of the source int4b_t vector, which must be 32-byte aligned.
 * @param [in] count Number of elements participating in the conversion; for the int4b_t source the count must be even
 * as required by the page constraints.
 */
__aicore__ inline void asc_int42half_sync(__ubuf__ half* dst, __ubuf__ int4b_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int82half and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous conversion of int8_t elements to half.
 *
 * @override_id asc_int82half_sync__PUf16_PUi8_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector, which must be 32-byte aligned.
 * @param [in] src Start address of the source int8_t vector, which must be 32-byte aligned.
 * @param [in] count Number of elements participating in the conversion.
 */
__aicore__ inline void asc_int82half_sync(__ubuf__ half* dst, __ubuf__ int8_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint82half and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous conversion of uint8_t elements to half.
 *
 * @override_id asc_uint82half_sync__PUf16_PUu8_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector, which must be 32-byte aligned.
 * @param [in] src Start address of the source uint8_t vector, which must be 32-byte aligned.
 * @param [in] count Number of elements participating in the conversion.
 */
__aicore__ inline void asc_uint82half_sync(__ubuf__ half* dst, __ubuf__ uint8_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162float and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous conversion of int16_t elements to float.
 *
 * @override_id asc_int162float_sync__PUf32_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector, which must be 32-byte aligned.
 * @param [in] src Start address of the source int16_t vector, which must be 32-byte aligned.
 * @param [in] count Number of elements participating in the conversion.
 */
__aicore__ inline void asc_int162float_sync(__ubuf__ float* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322float and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count int32_t elements from src to float values at dst with the default mode, which the prototype
 * comment describes as RINT when precision is lost and no rounding otherwise (deprecated; use asc_int322float plus
 * asc_sync).
 *
 * @override_id asc_int322float_sync__PUf32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source int32_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of int32_t elements converted, counted as elements of the source and destination vectors.
 */
__aicore__ inline void asc_int322float_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322float_rna and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count int32_t elements from src to float values at dst with ROUND rounding (round half away from
 * zero) (deprecated; use asc_int322float plus asc_sync).
 *
 * @override_id asc_int322float_rna_sync__PUf32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source int32_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of int32_t elements converted, counted as elements of the source and destination vectors.
 */
__aicore__ inline void asc_int322float_rna_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322float_ru and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count int32_t elements from src to float values at dst with CEIL rounding (toward plus infinity)
 * (deprecated; use asc_int322float plus asc_sync).
 *
 * @override_id asc_int322float_ru_sync__PUf32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source int32_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of int32_t elements converted, counted as elements of the source and destination vectors.
 */
__aicore__ inline void asc_int322float_ru_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322float_rd and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count int32_t elements from src to float values at dst with FLOOR rounding (toward minus infinity)
 * (deprecated; use asc_int322float plus asc_sync).
 *
 * @override_id asc_int322float_rd_sync__PUf32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source int32_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of int32_t elements converted, counted as elements of the source and destination vectors.
 */
__aicore__ inline void asc_int322float_rd_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322float_rn and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count int32_t elements from src to float values at dst with RINT rounding (round half to even)
 * (deprecated; use asc_int322float plus asc_sync).
 *
 * @override_id asc_int322float_rn_sync__PUf32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source int32_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of int32_t elements converted, counted as elements of the source and destination vectors.
 */
__aicore__ inline void asc_int322float_rn_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322float_rz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count int32_t elements from src to float values at dst with TRUNC rounding (toward zero) (deprecated;
 * use asc_int322float plus asc_sync).
 *
 * @override_id asc_int322float_rz_sync__PUf32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source int32_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of int32_t elements converted, counted as elements of the source and destination vectors.
 */
__aicore__ inline void asc_int322float_rz_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322int16 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous conversion of int32_t elements to int16_t.
 *
 * @override_id asc_int322int16_sync__PUi16_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination int16_t vector, which must be 32-byte aligned.
 * @param [in] src Start address of the source int32_t vector, which must be 32-byte aligned.
 * @param [in] count Number of elements participating in the conversion.
 */
__aicore__ inline void asc_int322int16_sync(__ubuf__ int16_t* dst, __ubuf__ int32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322int64 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous widening conversion of int32_t elements to int64_t.
 *
 * @override_id asc_int322int64_sync__PUi64_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination int64_t vector, which must be 32-byte aligned.
 * @param [in] src Start address of the source int32_t vector, which must be 32-byte aligned.
 * @param [in] count Number of elements participating in the conversion.
 */
__aicore__ inline void asc_int322int64_sync(__ubuf__ int64_t* dst, __ubuf__ int32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642int32 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous narrowing conversion of int64_t elements to int32_t.
 *
 * @override_id asc_int642int32_sync__PUi32_PUi64_u32
 * @archs 2201
 * @param [out] dst Start address of the destination int32_t vector, which must be 32-byte aligned.
 * @param [in] src Start address of the source int64_t vector, which must be 32-byte aligned.
 * @param [in] count Number of elements participating in the conversion.
 */
__aicore__ inline void asc_int642int32_sync(__ubuf__ int32_t* dst, __ubuf__ int64_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gather and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Gathers count int16_t elements from src into dst using the per-element byte offsets stored in src_offset
 * (deprecated; use asc_gather plus asc_sync).
 *
 * @override_id asc_gather_sync__PUi16_PUi16_PUu32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer; must be 32-byte aligned and receives
 * count gathered elements.
 * @param [in] src Start address of the source vector in Unified Buffer, used as the base against which src_offset
 * values are applied; must be 32-byte aligned.
 * @param [in] src_offset Per-element offset table in Unified Buffer: the offset of the source element gathered for each
 * destination slot, relative to the src base address and expressed in bytes; each offset must keep 2-byte element
 * alignment.
 * @param [in] count Number of elements gathered, counted in elements of the source and destination vectors.
 */
__aicore__ inline void asc_gather_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gather and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Gathers count uint16_t elements from src into dst using the per-element byte offsets stored in src_offset
 * (deprecated; use asc_gather plus asc_sync).
 *
 * @override_id asc_gather_sync__PUu16_PUu16_PUu32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer; must be 32-byte aligned and receives
 * count gathered elements.
 * @param [in] src Start address of the source vector in Unified Buffer, used as the base against which src_offset
 * values are applied; must be 32-byte aligned.
 * @param [in] src_offset Per-element offset table in Unified Buffer: the offset of the source element gathered for each
 * destination slot, relative to the src base address and expressed in bytes; each offset must keep 2-byte element
 * alignment.
 * @param [in] count Number of elements gathered, counted in elements of the source and destination vectors.
 */
__aicore__ inline void asc_gather_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gather and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Gathers count half elements from src into dst using the per-element byte offsets stored in src_offset
 * (deprecated; use asc_gather plus asc_sync).
 *
 * @override_id asc_gather_sync__PUf16_PUf16_PUu32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer; must be 32-byte aligned and receives
 * count gathered elements.
 * @param [in] src Start address of the source vector in Unified Buffer, used as the base against which src_offset
 * values are applied; must be 32-byte aligned.
 * @param [in] src_offset Per-element offset table in Unified Buffer: the offset of the source element gathered for each
 * destination slot, relative to the src base address and expressed in bytes; each offset must keep 2-byte element
 * alignment.
 * @param [in] count Number of elements gathered, counted in elements of the source and destination vectors.
 */
__aicore__ inline void asc_gather_sync(
    __ubuf__ half* dst, __ubuf__ half* src, __ubuf__ uint32_t* src_offset, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gather and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Gathers count bfloat16_t elements from src into dst using the per-element byte offsets stored in src_offset
 * (deprecated; use asc_gather plus asc_sync).
 *
 * @override_id asc_gather_sync__PUbf16_PUbf16_PUu32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer; must be 32-byte aligned and receives
 * count gathered elements.
 * @param [in] src Start address of the source vector in Unified Buffer, used as the base against which src_offset
 * values are applied; must be 32-byte aligned.
 * @param [in] src_offset Per-element offset table in Unified Buffer: the offset of the source element gathered for each
 * destination slot, relative to the src base address and expressed in bytes; each offset must keep 2-byte element
 * alignment.
 * @param [in] count Number of elements gathered, counted in elements of the source and destination vectors.
 */
__aicore__ inline void asc_gather_sync(
    __ubuf__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gather and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Gathers count int32_t elements from src into dst using the per-element byte offsets stored in src_offset
 * (deprecated; use asc_gather plus asc_sync).
 *
 * @override_id asc_gather_sync__PUi32_PUi32_PUu32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer; must be 32-byte aligned and receives
 * count gathered elements.
 * @param [in] src Start address of the source vector in Unified Buffer, used as the base against which src_offset
 * values are applied; must be 32-byte aligned.
 * @param [in] src_offset Per-element offset table in Unified Buffer: the offset of the source element gathered for each
 * destination slot, relative to the src base address and expressed in bytes; each offset must keep 4-byte element
 * alignment.
 * @param [in] count Number of elements gathered, counted in elements of the source and destination vectors.
 */
__aicore__ inline void asc_gather_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, __ubuf__ uint32_t* src_offset, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gather and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Gathers count uint32_t elements from src into dst using the per-element byte offsets stored in src_offset
 * (deprecated; use asc_gather plus asc_sync).
 *
 * @override_id asc_gather_sync__PUu32_PUu32_PUu32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer; must be 32-byte aligned and receives
 * count gathered elements.
 * @param [in] src Start address of the source vector in Unified Buffer, used as the base against which src_offset
 * values are applied; must be 32-byte aligned.
 * @param [in] src_offset Per-element offset table in Unified Buffer: the offset of the source element gathered for each
 * destination slot, relative to the src base address and expressed in bytes; each offset must keep 4-byte element
 * alignment.
 * @param [in] count Number of elements gathered, counted in elements of the source and destination vectors.
 */
__aicore__ inline void asc_gather_sync(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, __ubuf__ uint32_t* src_offset, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gather and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Gathers count float elements from src into dst using the per-element byte offsets stored in src_offset
 * (deprecated; use asc_gather plus asc_sync).
 *
 * @override_id asc_gather_sync__PUf32_PUf32_PUu32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer; must be 32-byte aligned and receives
 * count gathered elements.
 * @param [in] src Start address of the source vector in Unified Buffer, used as the base against which src_offset
 * values are applied; must be 32-byte aligned.
 * @param [in] src_offset Per-element offset table in Unified Buffer: the offset of the source element gathered for each
 * destination slot, relative to the src base address and expressed in bytes; each offset must keep 4-byte element
 * alignment.
 * @param [in] count Number of elements gathered, counted in elements of the source and destination vectors.
 */
__aicore__ inline void asc_gather_sync(
    __ubuf__ float* dst, __ubuf__ float* src, __ubuf__ uint32_t* src_offset, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_and and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous wrapper that computes the bitwise AND of src0 and src1 element-wise and waits for the
 * vector pipe.
 *
 * @override_id asc_and_sync__PUi16_PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination int16_t vector receiving the bitwise AND result; must be 32-byte
 * aligned.
 * @param [in] src0 Start address of the first int16_t source vector; must be 32-byte aligned.
 * @param [in] src1 Start address of the second int16_t source vector; must be 32-byte aligned.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_and_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_and and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous wrapper that computes the bitwise AND of src0 and src1 element-wise and waits for the
 * vector pipe.
 *
 * @override_id asc_and_sync__PUu16_PUu16_PUu16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination uint16_t vector receiving the bitwise AND result; must be 32-byte
 * aligned.
 * @param [in] src0 Start address of the first uint16_t source vector; must be 32-byte aligned.
 * @param [in] src1 Start address of the second uint16_t source vector; must be 32-byte aligned.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_and_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_leakyrelu and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous Leaky ReLU wrapper using the scalar alpha as the negative-slope factor, and waits for
 * the vector pipe.
 *
 * @override_id asc_leakyrelu_sync__PUf16_PUf16_f16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector receiving the Leaky ReLU result; must be 32-byte
 * aligned.
 * @param [in] src Start address of the source half vector holding the data to be processed; must be 32-byte aligned.
 * @param [in] value Scalar alpha of Leaky ReLU applied to the elements that are not greater than zero.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_leakyrelu_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_leakyrelu and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous Leaky ReLU wrapper using the scalar alpha as the negative-slope factor, and waits for
 * the vector pipe.
 *
 * @override_id asc_leakyrelu_sync__PUf32_PUf32_f32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector receiving the Leaky ReLU result; must be 32-byte
 * aligned.
 * @param [in] src Start address of the source float vector holding the data to be processed; must be 32-byte aligned.
 * @param [in] value Scalar alpha of Leaky ReLU applied to the elements that are not greater than zero.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_leakyrelu_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_lt_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous comparison wrapper: tests src < value per half element, packs one result bit per
 * element into dst and waits for the vector pipe.
 *
 * @override_id asc_lt_scalar_sync__PUu8_PUf16_f16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination buffer holding one comparison bit per source element in
 * little-endian order; must be 32-byte aligned.
 * @param [in] src Start address of the source half vector compared element-wise with value; must be 32-byte aligned.
 * @param [in] value Scalar half value each element is compared against.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of dst.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of src.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of dst in adjacent iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of src in adjacent iterations.
 */
__aicore__ inline void asc_lt_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_lt_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous comparison wrapper: tests src < value per float element, packs one result bit per
 * element into dst and waits for the vector pipe.
 *
 * @override_id asc_lt_scalar_sync__PUu8_PUf32_f32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination buffer holding one comparison bit per source element in
 * little-endian order; must be 32-byte aligned.
 * @param [in] src Start address of the source float vector compared element-wise with value; must be 32-byte aligned.
 * @param [in] value Scalar float value each element is compared against.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of dst.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of src.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of dst in adjacent iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of src in adjacent iterations.
 */
__aicore__ inline void asc_lt_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub_relu and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes relu(src0 - src1) elementwise into dst for same-type half operands, where dst keeps the half element
 * type (deprecated; use asc_sub_relu plus asc_sync).
 *
 * @override_id asc_sub_relu_sync__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer; must be 32-byte aligned and receives the
 * half results.
 * @param [in] src0 Start address of the minuend half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src1 Start address of the subtrahend half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of elements processed, counted in elements of the source vectors.
 */
__aicore__ inline void asc_sub_relu_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub_relu and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes relu(src0 - src1) elementwise into dst for same-type float operands, where dst keeps the float
 * element type (deprecated; use asc_sub_relu plus asc_sync).
 *
 * @override_id asc_sub_relu_sync__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer; must be 32-byte aligned and receives the
 * float results.
 * @param [in] src0 Start address of the minuend float vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src1 Start address of the subtrahend float vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of elements processed, counted in elements of the source vectors.
 */
__aicore__ inline void asc_sub_relu_sync(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub_relu and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes relu(src0 - src1) elementwise into dst for same-type int16_t operands, where dst keeps the int16_t
 * element type (deprecated; use asc_sub_relu plus asc_sync).
 *
 * @override_id asc_sub_relu_sync__PUi16_PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer; must be 32-byte aligned and receives the
 * int16_t results.
 * @param [in] src0 Start address of the minuend int16_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src1 Start address of the subtrahend int16_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of elements processed, counted in elements of the source vectors.
 */
__aicore__ inline void asc_sub_relu_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub_relu and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes relu(src0 - src1) elementwise into dst for half sources narrowed to int8_t (f162s8), where the
 * difference is narrowed to int8_t in dst (deprecated; use asc_sub_relu plus asc_sync).
 *
 * @override_id asc_sub_relu_sync__PUi8_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer; must be 32-byte aligned and receives the
 * int8_t results.
 * @param [in] src0 Start address of the minuend half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src1 Start address of the subtrahend half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of elements processed, counted in elements of the source vectors.
 */
__aicore__ inline void asc_sub_relu_sync(
    __ubuf__ int8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub_relu and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes relu(src0 - src1) elementwise into dst for float sources narrowed to half (f322f16), where the
 * difference is narrowed to half in dst (deprecated; use asc_sub_relu plus asc_sync).
 *
 * @override_id asc_sub_relu_sync__PUf16_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer; must be 32-byte aligned and receives the
 * half results.
 * @param [in] src0 Start address of the minuend float vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src1 Start address of the subtrahend float vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of elements processed, counted in elements of the source vectors.
 */
__aicore__ inline void asc_sub_relu_sync(
    __ubuf__ half* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub_relu and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes relu(src0 - src1) elementwise into dst for int16_t sources narrowed to int8_t (s162s8), where the
 * difference is narrowed to int8_t in dst (deprecated; use asc_sub_relu plus asc_sync).
 *
 * @override_id asc_sub_relu_sync__PUi8_PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer; must be 32-byte aligned and receives the
 * int8_t results.
 * @param [in] src0 Start address of the minuend int16_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src1 Start address of the subtrahend int16_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of elements processed, counted in elements of the source vectors.
 */
__aicore__ inline void asc_sub_relu_sync(
    __ubuf__ int8_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rna and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into 16-bit signed integer (int16_t) in dst using ROUND rounding
 * (round half away from zero) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2int16_rna plus
 * asc_sync.
 *
 * @override_id asc_float2int16_rna_sync__PUi16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the 16-bit signed integer (int16_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2int16_rna_sync(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_ru and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into 16-bit signed integer (int16_t) in dst using CEIL rounding
 * (round toward positive infinity) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2int16_ru plus
 * asc_sync.
 *
 * @override_id asc_float2int16_ru_sync__PUi16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the 16-bit signed integer (int16_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2int16_ru_sync(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rd and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into 16-bit signed integer (int16_t) in dst using FLOOR rounding
 * (round toward negative infinity) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2int16_rd plus
 * asc_sync.
 *
 * @override_id asc_float2int16_rd_sync__PUi16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the 16-bit signed integer (int16_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2int16_rd_sync(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rn and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into 16-bit signed integer (int16_t) in dst using RINT rounding
 * (round half to even) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2int16_rn plus asc_sync.
 *
 * @override_id asc_float2int16_rn_sync__PUi16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the 16-bit signed integer (int16_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2int16_rn_sync(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count float (fp32) elements from src into 16-bit signed integer (int16_t) in dst using TRUNC rounding
 * (round toward zero) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_float2int16_rz plus asc_sync.
 *
 * @override_id asc_float2int16_rz_sync__PUi16_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the 16-bit signed integer (int16_t) destination vector in Unified Buffer that
 * receives the converted elements.
 * @param [in] src Start address of the float (fp32) source vector in Unified Buffer holding the elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_float2int16_rz_sync(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_rsqrt and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously computes the element-wise reciprocal square root 1/sqrt(src) of half data and writes the results
 * to dst.
 *
 * @override_id asc_rsqrt_sync__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving 1/sqrt(src) for each processed element.
 * @param [in] src Source vector start address whose elements are square-rooted and inverted.
 * @param [in] count Number of elements participating in the computation.
 */
__aicore__ inline void asc_rsqrt_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_rsqrt and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously computes the element-wise reciprocal square root 1/sqrt(src) of float data and writes the
 * results to dst.
 *
 * @override_id asc_rsqrt_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving 1/sqrt(src) for each processed element.
 * @param [in] src Source vector start address whose elements are square-rooted and inverted.
 * @param [in] count Number of elements participating in the computation.
 */
__aicore__ inline void asc_rsqrt_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add_relu and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes relu(src0 + src1) elementwise into dst for same-type half operands, where the destination keeps the
 * half element type (deprecated; use asc_add_relu plus asc_sync).
 *
 * @override_id asc_add_relu_sync__PUf16_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer; must be 32-byte aligned and receives the
 * half results.
 * @param [in] src0 Start address of the first half source vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src1 Start address of the second half source vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of elements processed, counted in elements of the source vectors.
 */
__aicore__ inline void asc_add_relu_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add_relu and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes relu(src0 + src1) elementwise into dst for same-type float operands, where the destination keeps the
 * float element type (deprecated; use asc_add_relu plus asc_sync).
 *
 * @override_id asc_add_relu_sync__PUf32_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer; must be 32-byte aligned and receives the
 * float results.
 * @param [in] src0 Start address of the first float source vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src1 Start address of the second float source vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of elements processed, counted in elements of the source vectors.
 */
__aicore__ inline void asc_add_relu_sync(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add_relu and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes relu(src0 + src1) elementwise into dst for same-type int16_t operands, where the destination keeps
 * the int16_t element type (deprecated; use asc_add_relu plus asc_sync).
 *
 * @override_id asc_add_relu_sync__PUi16_PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer; must be 32-byte aligned and receives the
 * int16_t results.
 * @param [in] src0 Start address of the first int16_t source vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src1 Start address of the second int16_t source vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of elements processed, counted in elements of the source vectors.
 */
__aicore__ inline void asc_add_relu_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ge and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 elementwise on the relation src0 >= src1 in a synchronized composite call and leaves
 * the 1-bit-per-element result in the comparison register for asc_get_cmp_mask, processing a fixed 256 B per call.
 *
 * @override_id asc_ge_sync__PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in] src0 Start address of source operand 0 (half elements).
 * @param [in] src1 Start address of source operand 1 (half elements).
 * @param [in] repeat Number of iterations the comparison repeats; the page states that repeat has no effect in the
 * register-result form.
 * @param [in] dst_block_stride Address stride between the DataBlocks of the destination within a single iteration.
 * @param [in] src0_block_stride Address stride between the DataBlocks of source operand 0 within a single iteration.
 * @param [in] src1_block_stride Address stride between the DataBlocks of source operand 1 within a single iteration.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of the destination across adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of source operand 0 across adjacent
 * iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of source operand 1 across adjacent
 * iterations.
 */
__aicore__ inline void asc_ge_sync(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ge and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 elementwise on the relation src0 >= src1 in a synchronized composite call and leaves
 * the 1-bit-per-element result in the comparison register for asc_get_cmp_mask, processing a fixed 256 B per call.
 *
 * @override_id asc_ge_sync__PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in] src0 Start address of source operand 0 (float elements).
 * @param [in] src1 Start address of source operand 1 (float elements).
 * @param [in] repeat Number of iterations the comparison repeats; the page states that repeat has no effect in the
 * register-result form.
 * @param [in] dst_block_stride Address stride between the DataBlocks of the destination within a single iteration.
 * @param [in] src0_block_stride Address stride between the DataBlocks of source operand 0 within a single iteration.
 * @param [in] src1_block_stride Address stride between the DataBlocks of source operand 1 within a single iteration.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of the destination across adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of source operand 0 across adjacent
 * iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of source operand 1 across adjacent
 * iterations.
 */
__aicore__ inline void asc_ge_sync(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ge and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 elementwise on the relation src0 >= src1 in a synchronized composite call and writes
 * the packed 1-bit-per-element result to the uint8_t buffer dst in little-endian bit order.
 *
 * @override_id asc_ge_sync__PUu8_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination buffer that receives the packed 1-bit-per-element comparison result
 * in little-endian bit order.
 * @param [in] src0 Start address of source operand 0 (half elements).
 * @param [in] src1 Start address of source operand 1 (half elements).
 * @param [in] repeat Number of iterations the comparison repeats across repeat-sized blocks.
 * @param [in] dst_block_stride Address stride between the DataBlocks of the destination within a single iteration.
 * @param [in] src0_block_stride Address stride between the DataBlocks of source operand 0 within a single iteration.
 * @param [in] src1_block_stride Address stride between the DataBlocks of source operand 1 within a single iteration.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of the destination across adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of source operand 0 across adjacent
 * iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of source operand 1 across adjacent
 * iterations.
 */
__aicore__ inline void asc_ge_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ge and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 elementwise on the relation src0 >= src1 in a synchronized composite call and writes
 * the packed 1-bit-per-element result to the uint8_t buffer dst in little-endian bit order.
 *
 * @override_id asc_ge_sync__PUu8_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination buffer that receives the packed 1-bit-per-element comparison result
 * in little-endian bit order.
 * @param [in] src0 Start address of source operand 0 (float elements).
 * @param [in] src1 Start address of source operand 1 (float elements).
 * @param [in] repeat Number of iterations the comparison repeats across repeat-sized blocks.
 * @param [in] dst_block_stride Address stride between the DataBlocks of the destination within a single iteration.
 * @param [in] src0_block_stride Address stride between the DataBlocks of source operand 0 within a single iteration.
 * @param [in] src1_block_stride Address stride between the DataBlocks of source operand 1 within a single iteration.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of the destination across adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of source operand 0 across adjacent
 * iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of source operand 1 across adjacent
 * iterations.
 */
__aicore__ inline void asc_ge_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_reduce and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously gathers uint16_t elements from src0 into dst using the gather mask supplied in src1 and stores
 * the selected elements contiguously.
 *
 * @override_id asc_reduce_sync__PUu16_PUu16_PUu16_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the gathered elements.
 * @param [in] src0 Source vector start address from which elements are selected.
 * @param [in] src1 Value-based gather mask used in user-defined mode to select which src0 elements are collected.
 * @param [in] count Number of elements participating in the computation.
 */
__aicore__ inline void asc_reduce_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_reduce and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously gathers uint32_t elements from src0 into dst using the gather mask supplied in src1 and stores
 * the selected elements contiguously.
 *
 * @override_id asc_reduce_sync__PUu32_PUu32_PUu32_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the gathered elements.
 * @param [in] src0 Source vector start address from which elements are selected.
 * @param [in] src1 Value-based gather mask used in user-defined mode to select which src0 elements are collected.
 * @param [in] count Number of elements participating in the computation.
 */
__aicore__ inline void asc_reduce_sync(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src0, __ubuf__ uint32_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642float_rna and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count 64-bit signed integer (int64_t) elements from src into float (fp32) in dst using ROUND rounding
 * (round half away from zero) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_int642float_rna plus
 * asc_sync.
 *
 * @override_id asc_int642float_rna_sync__PUf32_PUi64_u32
 * @archs 2201
 * @param [out] dst Start address of the float (fp32) destination vector in Unified Buffer that receives the converted
 * elements.
 * @param [in] src Start address of the 64-bit signed integer (int64_t) source vector in Unified Buffer holding the
 * elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_int642float_rna_sync(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642float_ru and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count 64-bit signed integer (int64_t) elements from src into float (fp32) in dst using CEIL rounding
 * (round toward positive infinity) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_int642float_ru plus
 * asc_sync.
 *
 * @override_id asc_int642float_ru_sync__PUf32_PUi64_u32
 * @archs 2201
 * @param [out] dst Start address of the float (fp32) destination vector in Unified Buffer that receives the converted
 * elements.
 * @param [in] src Start address of the 64-bit signed integer (int64_t) source vector in Unified Buffer holding the
 * elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_int642float_ru_sync(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642float_rd and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count 64-bit signed integer (int64_t) elements from src into float (fp32) in dst using FLOOR rounding
 * (round toward negative infinity) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_int642float_rd plus
 * asc_sync.
 *
 * @override_id asc_int642float_rd_sync__PUf32_PUi64_u32
 * @archs 2201
 * @param [out] dst Start address of the float (fp32) destination vector in Unified Buffer that receives the converted
 * elements.
 * @param [in] src Start address of the 64-bit signed integer (int64_t) source vector in Unified Buffer holding the
 * elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_int642float_rd_sync(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642float_rn and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count 64-bit signed integer (int64_t) elements from src into float (fp32) in dst using RINT rounding
 * (round half to even) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_int642float_rn plus asc_sync.
 *
 * @override_id asc_int642float_rn_sync__PUf32_PUi64_u32
 * @archs 2201
 * @param [out] dst Start address of the float (fp32) destination vector in Unified Buffer that receives the converted
 * elements.
 * @param [in] src Start address of the 64-bit signed integer (int64_t) source vector in Unified Buffer holding the
 * elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_int642float_rn_sync(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642float_rz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count 64-bit signed integer (int64_t) elements from src into float (fp32) in dst using TRUNC rounding
 * (round toward zero) and then inserts a PIPE_ALL barrier; deprecated in favour of asc_int642float_rz plus asc_sync.
 *
 * @override_id asc_int642float_rz_sync__PUf32_PUi64_u32
 * @archs 2201
 * @param [out] dst Start address of the float (fp32) destination vector in Unified Buffer that receives the converted
 * elements.
 * @param [in] src Start address of the 64-bit signed integer (int64_t) source vector in Unified Buffer holding the
 * elements to convert.
 * @param [in] count Number of consecutive elements converted; the impl passes it to asc_set_mask_count_begin before the
 * conversion instruction.
 */
__aicore__ inline void asc_int642float_rz_sync(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_vdeq_int162b8_h and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Dequantises int16_t elements of src with the scale and offset previously set by asc_set_deq_scale and stores
 * the resulting int8_t values in the upper half of each DataBlock of dst as one synchronized composite call.
 *
 * @override_id asc_vdeq_int162b8_h_sync__PUi8_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination buffer that receives the 8-bit results in the upper half of each
 * DataBlock.
 * @param [in] src Start address of the source vector operand holding int16_t elements.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_vdeq_int162b8_h_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_vdeq_int162b8_h and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Dequantises int16_t elements of src with the scale and offset previously set by asc_set_deq_scale and stores
 * the resulting uint8_t values in the upper half of each DataBlock of dst as one synchronized composite call.
 *
 * @override_id asc_vdeq_int162b8_h_sync__PUu8_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination buffer that receives the 8-bit results in the upper half of each
 * DataBlock.
 * @param [in] src Start address of the source vector operand holding int16_t elements.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_vdeq_int162b8_h_sync(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_vdeq_int162b8_l and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Dequantises int16_t elements of src with the scale and offset previously set by asc_set_deq_scale and stores
 * the resulting int8_t values in the lower half of each DataBlock of dst as one synchronized composite call.
 *
 * @override_id asc_vdeq_int162b8_l_sync__PUi8_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination buffer that receives the 8-bit results in the lower half of each
 * DataBlock.
 * @param [in] src Start address of the source vector operand holding int16_t elements.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_vdeq_int162b8_l_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_vdeq_int162b8_l and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Dequantises int16_t elements of src with the scale and offset previously set by asc_set_deq_scale and stores
 * the resulting uint8_t values in the lower half of each DataBlock of dst as one synchronized composite call.
 *
 * @override_id asc_vdeq_int162b8_l_sync__PUu8_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination buffer that receives the 8-bit results in the lower half of each
 * DataBlock.
 * @param [in] src Start address of the source vector operand holding int16_t elements.
 * @param [in] count Number of elements that participate in the computation.
 */
__aicore__ inline void asc_vdeq_int162b8_l_sync(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sqrt and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously computes the element-wise square root of half data and writes the results to dst.
 *
 * @override_id asc_sqrt_sync__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving sqrt(src) for each processed element.
 * @param [in] src Source vector start address whose elements are square-rooted; only positive values are supported.
 * @param [in] count Number of elements participating in the computation.
 */
__aicore__ inline void asc_sqrt_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sqrt and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously computes the element-wise square root of float data and writes the results to dst.
 *
 * @override_id asc_sqrt_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving sqrt(src) for each processed element.
 * @param [in] src Source vector start address whose elements are square-rooted; only positive values are supported.
 * @param [in] count Number of elements participating in the computation.
 */
__aicore__ inline void asc_sqrt_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_axpy and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous axpy wrapper: multiplies each element of src by the scalar value, adds it to dst and
 * waits for the vector pipe.
 *
 * @override_id asc_axpy_sync__PUf16_PUf16_f16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector; it is read as the addend and written with the result;
 * must be 32-byte aligned.
 * @param [in] src Start address of the source half vector whose elements are scaled by value; must be 32-byte aligned.
 * @param [in] value Scalar half multiplier applied to every element of src.
 * @param [in] count Number of consecutive elements that participate in the computation.
 */
__aicore__ inline void asc_axpy_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_axpy and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous axpy wrapper: multiplies each element of src by the scalar value, adds it to dst and
 * waits for the vector pipe.
 *
 * @override_id asc_axpy_sync__PUf32_PUf32_f32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector; it is read as the addend and written with the result;
 * must be 32-byte aligned.
 * @param [in] src Start address of the source float vector whose elements are scaled by value; must be 32-byte aligned.
 * @param [in] value Scalar float multiplier applied to every element of src.
 * @param [in] count Number of consecutive elements that participate in the computation.
 */
__aicore__ inline void asc_axpy_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_axpy and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous axpy wrapper: multiplies each element of src by the scalar value, adds it to dst and
 * waits for the vector pipe.
 *
 * @override_id asc_axpy_sync__PUf32_PUf16_f16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination float vector; it is read as the addend and written with the result;
 * must be 32-byte aligned.
 * @param [in] src Start address of the source half vector whose elements are scaled by value; must be 32-byte aligned.
 * @param [in] value Scalar half multiplier applied to every element of src.
 * @param [in] count Number of consecutive elements that participate in the computation.
 */
__aicore__ inline void asc_axpy_sync(__ubuf__ float* dst, __ubuf__ half* src, half value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_lt and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 elementwise on the relation src0 < src1 in a synchronized composite call and leaves the
 * 1-bit-per-element result in the comparison register for asc_get_cmp_mask, processing a fixed 256 B per call.
 *
 * @override_id asc_lt_sync__PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in] src0 Start address of source operand 0 (half elements).
 * @param [in] src1 Start address of source operand 1 (half elements).
 * @param [in] repeat Number of iterations the comparison repeats; the page states that repeat has no effect in the
 * register-result form.
 * @param [in] dst_block_stride Address stride between the DataBlocks of the destination within a single iteration.
 * @param [in] src0_block_stride Address stride between the DataBlocks of source operand 0 within a single iteration.
 * @param [in] src1_block_stride Address stride between the DataBlocks of source operand 1 within a single iteration.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of the destination across adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of source operand 0 across adjacent
 * iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of source operand 1 across adjacent
 * iterations.
 */
__aicore__ inline void asc_lt_sync(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_lt and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 elementwise on the relation src0 < src1 in a synchronized composite call and leaves the
 * 1-bit-per-element result in the comparison register for asc_get_cmp_mask, processing a fixed 256 B per call.
 *
 * @override_id asc_lt_sync__PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in] src0 Start address of source operand 0 (float elements).
 * @param [in] src1 Start address of source operand 1 (float elements).
 * @param [in] repeat Number of iterations the comparison repeats; the page states that repeat has no effect in the
 * register-result form.
 * @param [in] dst_block_stride Address stride between the DataBlocks of the destination within a single iteration.
 * @param [in] src0_block_stride Address stride between the DataBlocks of source operand 0 within a single iteration.
 * @param [in] src1_block_stride Address stride between the DataBlocks of source operand 1 within a single iteration.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of the destination across adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of source operand 0 across adjacent
 * iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of source operand 1 across adjacent
 * iterations.
 */
__aicore__ inline void asc_lt_sync(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_lt and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 elementwise on the relation src0 < src1 in a synchronized composite call and writes the
 * packed 1-bit-per-element result to the uint8_t buffer dst in little-endian bit order.
 *
 * @override_id asc_lt_sync__PUu8_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination buffer that receives the packed 1-bit-per-element comparison result
 * in little-endian bit order.
 * @param [in] src0 Start address of source operand 0 (half elements).
 * @param [in] src1 Start address of source operand 1 (half elements).
 * @param [in] repeat Number of iterations the comparison repeats across repeat-sized blocks.
 * @param [in] dst_block_stride Address stride between the DataBlocks of the destination within a single iteration.
 * @param [in] src0_block_stride Address stride between the DataBlocks of source operand 0 within a single iteration.
 * @param [in] src1_block_stride Address stride between the DataBlocks of source operand 1 within a single iteration.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of the destination across adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of source operand 0 across adjacent
 * iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of source operand 1 across adjacent
 * iterations.
 */
__aicore__ inline void asc_lt_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_lt and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 elementwise on the relation src0 < src1 in a synchronized composite call and writes the
 * packed 1-bit-per-element result to the uint8_t buffer dst in little-endian bit order.
 *
 * @override_id asc_lt_sync__PUu8_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination buffer that receives the packed 1-bit-per-element comparison result
 * in little-endian bit order.
 * @param [in] src0 Start address of source operand 0 (float elements).
 * @param [in] src1 Start address of source operand 1 (float elements).
 * @param [in] repeat Number of iterations the comparison repeats across repeat-sized blocks.
 * @param [in] dst_block_stride Address stride between the DataBlocks of the destination within a single iteration.
 * @param [in] src0_block_stride Address stride between the DataBlocks of source operand 0 within a single iteration.
 * @param [in] src1_block_stride Address stride between the DataBlocks of source operand 1 within a single iteration.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of the destination across adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of source operand 0 across adjacent
 * iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of source operand 1 across adjacent
 * iterations.
 */
__aicore__ inline void asc_lt_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ne_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously compares every half element of src with scalar value for inequality and packs one result bit per
 * element into dst.
 *
 * @override_id asc_ne_scalar_sync__PUu8_PUf16_f16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the packed comparison bits, one bit per source element in
 * little-endian order.
 * @param [in] src Source vector start address of half elements compared against value.
 * @param [in] value Scalar value each src element is compared with.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of the source.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent iterations of the destination.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent iterations of the source.
 */
__aicore__ inline void asc_ne_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ne_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously compares every float element of src with scalar value for inequality and packs one result bit
 * per element into dst.
 *
 * @override_id asc_ne_scalar_sync__PUu8_PUf32_f32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the packed comparison bits, one bit per source element in
 * little-endian order.
 * @param [in] src Source vector start address of float elements compared against value.
 * @param [in] value Scalar value each src element is compared with.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of the destination.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of the source.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of adjacent iterations of the destination.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of adjacent iterations of the source.
 */
__aicore__ inline void asc_ne_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162half and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count int16_t elements from src to half values at dst with the default mode, which the mode list
 * describes as no rounding while the prototype comment says RINT when precision is lost (deprecated; use asc_int162half
 * plus asc_sync).
 *
 * @override_id asc_int162half_sync__PUf16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source int16_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of int16_t elements converted, counted as elements of the source and destination vectors,
 * which have the same element count.
 */
__aicore__ inline void asc_int162half_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162half_rna and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count int16_t elements from src to half values at dst with ROUND rounding (round half away from zero)
 * (deprecated; use asc_int162half plus asc_sync).
 *
 * @override_id asc_int162half_rna_sync__PUf16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source int16_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of int16_t elements converted, counted as elements of the source and destination vectors,
 * which have the same element count.
 */
__aicore__ inline void asc_int162half_rna_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162half_ru and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count int16_t elements from src to half values at dst with CEIL rounding (toward plus infinity)
 * (deprecated; use asc_int162half plus asc_sync).
 *
 * @override_id asc_int162half_ru_sync__PUf16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source int16_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of int16_t elements converted, counted as elements of the source and destination vectors,
 * which have the same element count.
 */
__aicore__ inline void asc_int162half_ru_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162half_rd and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count int16_t elements from src to half values at dst with FLOOR rounding (toward minus infinity)
 * (deprecated; use asc_int162half plus asc_sync).
 *
 * @override_id asc_int162half_rd_sync__PUf16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source int16_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of int16_t elements converted, counted as elements of the source and destination vectors,
 * which have the same element count.
 */
__aicore__ inline void asc_int162half_rd_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162half_rn and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count int16_t elements from src to half values at dst with RINT rounding (round half to even)
 * (deprecated; use asc_int162half plus asc_sync).
 *
 * @override_id asc_int162half_rn_sync__PUf16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source int16_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of int16_t elements converted, counted as elements of the source and destination vectors,
 * which have the same element count.
 */
__aicore__ inline void asc_int162half_rn_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162half_rz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count int16_t elements from src to half values at dst with TRUNC rounding (toward zero) (deprecated;
 * use asc_int162half plus asc_sync).
 *
 * @override_id asc_int162half_rz_sync__PUf16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source int16_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of int16_t elements converted, counted as elements of the source and destination vectors,
 * which have the same element count.
 */
__aicore__ inline void asc_int162half_rz_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gather_datablock and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous DataBlock gather wrapper: collects DataBlocks of src into dst at offsets read from
 * src_offset and waits for the vector pipe.
 *
 * @override_id asc_gather_datablock_sync__PUu16_PUu16_PUu32_u16_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination uint16_t vector receiving the gathered DataBlocks; must be 32-byte
 * aligned.
 * @param [in] src Start address of the source uint16_t vector whose DataBlocks are collected; must be 32-byte aligned.
 * @param [in] src_offset uint32_t offset of each gathered DataBlock inside src; the values must respect the element
 * width alignment and keep src + offset inside the UB range.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of dst in adjacent iterations.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of dst.
 * @param [in] repeat Number of iterations of the gather.
 */
__aicore__ inline void asc_gather_datablock_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride,
    uint8_t dst_block_stride, uint8_t repeat);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gather_datablock and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous DataBlock gather wrapper: collects DataBlocks of src into dst at offsets read from
 * src_offset and waits for the vector pipe.
 *
 * @override_id asc_gather_datablock_sync__PUu32_PUu32_PUu32_u16_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination uint32_t vector receiving the gathered DataBlocks; must be 32-byte
 * aligned.
 * @param [in] src Start address of the source uint32_t vector whose DataBlocks are collected; must be 32-byte aligned.
 * @param [in] src_offset uint32_t offset of each gathered DataBlock inside src; the values must respect the element
 * width alignment and keep src + offset inside the UB range.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of dst in adjacent iterations.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of dst.
 * @param [in] repeat Number of iterations of the gather.
 */
__aicore__ inline void asc_gather_datablock_sync(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride,
    uint8_t dst_block_stride, uint8_t repeat);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_min_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise minimum between each element of src and the scalar value for count elements and
 * writes the results to dst as one synchronized composite call.
 *
 * @override_id asc_min_scalar_sync__PUf16_PUf16_f16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src Start address of the source vector operand (half elements).
 * @param [in] value Scalar half operand applied to every element.
 * @param [in] count Number of elements that participate in the contiguous computation.
 */
__aicore__ inline void asc_min_scalar_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_min_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise minimum between each element of src and the scalar value for count elements and
 * writes the results to dst as one synchronized composite call.
 *
 * @override_id asc_min_scalar_sync__PUf32_PUf32_f32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src Start address of the source vector operand (float elements).
 * @param [in] value Scalar float operand applied to every element.
 * @param [in] count Number of elements that participate in the contiguous computation.
 */
__aicore__ inline void asc_min_scalar_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_min_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise minimum between each element of src and the scalar value for count elements and
 * writes the results to dst as one synchronized composite call.
 *
 * @override_id asc_min_scalar_sync__PUi16_PUi16_i16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src Start address of the source vector operand (int16_t elements).
 * @param [in] value Scalar int16_t operand applied to every element.
 * @param [in] count Number of elements that participate in the contiguous computation.
 */
__aicore__ inline void asc_min_scalar_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_min_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes the elementwise minimum between each element of src and the scalar value for count elements and
 * writes the results to dst as one synchronized composite call.
 *
 * @override_id asc_min_scalar_sync__PUi32_PUi32_i32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector operand in UB.
 * @param [in] src Start address of the source vector operand (int32_t elements).
 * @param [in] value Scalar int32_t operand applied to every element.
 * @param [in] count Number of elements that participate in the contiguous computation.
 */
__aicore__ inline void asc_min_scalar_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add_relu and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes relu(src0 + src1) elementwise into dst for half sources narrowed to int8_t (f162s8), where the sum is
 * narrowed to int8_t in dst (deprecated; use asc_add_relu plus asc_sync).
 *
 * @override_id asc_add_relu_sync__PUi8_PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer; must be 32-byte aligned and receives the
 * int8_t results.
 * @param [in] src0 Start address of the first half source vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src1 Start address of the second half source vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of elements processed, counted in elements of the source vectors.
 */
__aicore__ inline void asc_add_relu_sync(
    __ubuf__ int8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add_relu and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes relu(src0 + src1) elementwise into dst for float sources narrowed to half (f322f16), where the sum is
 * narrowed to half in dst (deprecated; use asc_add_relu plus asc_sync).
 *
 * @override_id asc_add_relu_sync__PUf16_PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer; must be 32-byte aligned and receives the
 * half results.
 * @param [in] src0 Start address of the first float source vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src1 Start address of the second float source vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of elements processed, counted in elements of the source vectors.
 */
__aicore__ inline void asc_add_relu_sync(
    __ubuf__ half* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add_relu and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Computes relu(src0 + src1) elementwise into dst for int16_t sources narrowed to int8_t (s162s8), where the sum
 * is narrowed to int8_t in dst (deprecated; use asc_add_relu plus asc_sync).
 *
 * @override_id asc_add_relu_sync__PUi8_PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination vector in Unified Buffer; must be 32-byte aligned and receives the
 * int8_t results.
 * @param [in] src0 Start address of the first int16_t source vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src1 Start address of the second int16_t source vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of elements processed, counted in elements of the source vectors.
 */
__aicore__ inline void asc_add_relu_sync(
    __ubuf__ int8_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gt and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 elementwise on the relation src0 > src1 in a synchronized composite call and leaves the
 * 1-bit-per-element result in the comparison register for asc_get_cmp_mask, processing a fixed 256 B per call.
 *
 * @override_id asc_gt_sync__PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in] src0 Start address of source operand 0 (half elements).
 * @param [in] src1 Start address of source operand 1 (half elements).
 * @param [in] repeat Number of iterations the comparison repeats; the page states that repeat has no effect in the
 * register-result form.
 * @param [in] dst_block_stride Address stride between the DataBlocks of the destination within a single iteration.
 * @param [in] src0_block_stride Address stride between the DataBlocks of source operand 0 within a single iteration.
 * @param [in] src1_block_stride Address stride between the DataBlocks of source operand 1 within a single iteration.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of the destination across adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of source operand 0 across adjacent
 * iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of source operand 1 across adjacent
 * iterations.
 */
__aicore__ inline void asc_gt_sync(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gt and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 elementwise on the relation src0 > src1 in a synchronized composite call and leaves the
 * 1-bit-per-element result in the comparison register for asc_get_cmp_mask, processing a fixed 256 B per call.
 *
 * @override_id asc_gt_sync__PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in] src0 Start address of source operand 0 (float elements).
 * @param [in] src1 Start address of source operand 1 (float elements).
 * @param [in] repeat Number of iterations the comparison repeats; the page states that repeat has no effect in the
 * register-result form.
 * @param [in] dst_block_stride Address stride between the DataBlocks of the destination within a single iteration.
 * @param [in] src0_block_stride Address stride between the DataBlocks of source operand 0 within a single iteration.
 * @param [in] src1_block_stride Address stride between the DataBlocks of source operand 1 within a single iteration.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of the destination across adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of source operand 0 across adjacent
 * iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of source operand 1 across adjacent
 * iterations.
 */
__aicore__ inline void asc_gt_sync(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gt and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 elementwise on the relation src0 > src1 in a synchronized composite call and writes the
 * packed 1-bit-per-element result to the uint8_t buffer dst in little-endian bit order.
 *
 * @override_id asc_gt_sync__PUu8_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination buffer that receives the packed 1-bit-per-element comparison result
 * in little-endian bit order.
 * @param [in] src0 Start address of source operand 0 (half elements).
 * @param [in] src1 Start address of source operand 1 (half elements).
 * @param [in] repeat Number of iterations the comparison repeats across repeat-sized blocks.
 * @param [in] dst_block_stride Address stride between the DataBlocks of the destination within a single iteration.
 * @param [in] src0_block_stride Address stride between the DataBlocks of source operand 0 within a single iteration.
 * @param [in] src1_block_stride Address stride between the DataBlocks of source operand 1 within a single iteration.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of the destination across adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of source operand 0 across adjacent
 * iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of source operand 1 across adjacent
 * iterations.
 */
__aicore__ inline void asc_gt_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gt and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 elementwise on the relation src0 > src1 in a synchronized composite call and writes the
 * packed 1-bit-per-element result to the uint8_t buffer dst in little-endian bit order.
 *
 * @override_id asc_gt_sync__PUu8_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination buffer that receives the packed 1-bit-per-element comparison result
 * in little-endian bit order.
 * @param [in] src0 Start address of source operand 0 (float elements).
 * @param [in] src1 Start address of source operand 1 (float elements).
 * @param [in] repeat Number of iterations the comparison repeats across repeat-sized blocks.
 * @param [in] dst_block_stride Address stride between the DataBlocks of the destination within a single iteration.
 * @param [in] src0_block_stride Address stride between the DataBlocks of source operand 0 within a single iteration.
 * @param [in] src1_block_stride Address stride between the DataBlocks of source operand 1 within a single iteration.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of the destination across adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of source operand 0 across adjacent
 * iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of source operand 1 across adjacent
 * iterations.
 */
__aicore__ inline void asc_gt_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half elements from src to uint8_t values at dst with the NONE default mode, described as RINT
 * when precision is lost and no rounding otherwise (deprecated; use asc_half2uint8 plus asc_sync).
 *
 * @override_id asc_half2uint8_sync__PUu8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination uint8_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of half elements converted, counted in source elements; it also equals the number of uint8_t
 * results written to dst.
 */
__aicore__ inline void asc_half2uint8_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rna and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half elements from src to uint8_t values at dst with ROUND rounding (round half away from zero)
 * (deprecated; use asc_half2uint8 plus asc_sync).
 *
 * @override_id asc_half2uint8_rna_sync__PUu8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination uint8_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of half elements converted, counted in source elements; it also equals the number of uint8_t
 * results written to dst.
 */
__aicore__ inline void asc_half2uint8_rna_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_ru and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half elements from src to uint8_t values at dst with CEIL rounding (toward plus infinity)
 * (deprecated; use asc_half2uint8 plus asc_sync).
 *
 * @override_id asc_half2uint8_ru_sync__PUu8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination uint8_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of half elements converted, counted in source elements; it also equals the number of uint8_t
 * results written to dst.
 */
__aicore__ inline void asc_half2uint8_ru_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rd and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half elements from src to uint8_t values at dst with FLOOR rounding (toward minus infinity)
 * (deprecated; use asc_half2uint8 plus asc_sync).
 *
 * @override_id asc_half2uint8_rd_sync__PUu8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination uint8_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of half elements converted, counted in source elements; it also equals the number of uint8_t
 * results written to dst.
 */
__aicore__ inline void asc_half2uint8_rd_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rn and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half elements from src to uint8_t values at dst with RINT rounding (round half to even)
 * (deprecated; use asc_half2uint8 plus asc_sync).
 *
 * @override_id asc_half2uint8_rn_sync__PUu8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination uint8_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of half elements converted, counted in source elements; it also equals the number of uint8_t
 * results written to dst.
 */
__aicore__ inline void asc_half2uint8_rn_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rz and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Converts count half elements from src to uint8_t values at dst with TRUNC rounding (toward zero) (deprecated;
 * use asc_half2uint8 plus asc_sync).
 *
 * @override_id asc_half2uint8_rz_sync__PUu8_PUf16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination uint8_t vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] src Start address of the source half vector in Unified Buffer; must be 32-byte aligned.
 * @param [in] count Number of half elements converted, counted in source elements; it also equals the number of uint8_t
 * results written to dst.
 */
__aicore__ inline void asc_half2uint8_rz_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_le and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 elementwise on the relation src0 <= src1 in a synchronized composite call and leaves
 * the 1-bit-per-element result in the comparison register for asc_get_cmp_mask, processing a fixed 256 B per call.
 *
 * @override_id asc_le_sync__PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in] src0 Start address of source operand 0 (half elements).
 * @param [in] src1 Start address of source operand 1 (half elements).
 * @param [in] repeat Number of iterations the comparison repeats; the page states that repeat has no effect in the
 * register-result form.
 * @param [in] dst_block_stride Address stride between the DataBlocks of the destination within a single iteration.
 * @param [in] src0_block_stride Address stride between the DataBlocks of source operand 0 within a single iteration.
 * @param [in] src1_block_stride Address stride between the DataBlocks of source operand 1 within a single iteration.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of the destination across adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of source operand 0 across adjacent
 * iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of source operand 1 across adjacent
 * iterations.
 */
__aicore__ inline void asc_le_sync(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_le and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 elementwise on the relation src0 <= src1 in a synchronized composite call and leaves
 * the 1-bit-per-element result in the comparison register for asc_get_cmp_mask, processing a fixed 256 B per call.
 *
 * @override_id asc_le_sync__PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [in] src0 Start address of source operand 0 (float elements).
 * @param [in] src1 Start address of source operand 1 (float elements).
 * @param [in] repeat Number of iterations the comparison repeats; the page states that repeat has no effect in the
 * register-result form.
 * @param [in] dst_block_stride Address stride between the DataBlocks of the destination within a single iteration.
 * @param [in] src0_block_stride Address stride between the DataBlocks of source operand 0 within a single iteration.
 * @param [in] src1_block_stride Address stride between the DataBlocks of source operand 1 within a single iteration.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of the destination across adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of source operand 0 across adjacent
 * iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of source operand 1 across adjacent
 * iterations.
 */
__aicore__ inline void asc_le_sync(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_le and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 elementwise on the relation src0 <= src1 in a synchronized composite call and writes
 * the packed 1-bit-per-element result to the uint8_t buffer dst in little-endian bit order.
 *
 * @override_id asc_le_sync__PUu8_PUf16_PUf16_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination buffer that receives the packed 1-bit-per-element comparison result
 * in little-endian bit order.
 * @param [in] src0 Start address of source operand 0 (half elements).
 * @param [in] src1 Start address of source operand 1 (half elements).
 * @param [in] repeat Number of iterations the comparison repeats across repeat-sized blocks.
 * @param [in] dst_block_stride Address stride between the DataBlocks of the destination within a single iteration.
 * @param [in] src0_block_stride Address stride between the DataBlocks of source operand 0 within a single iteration.
 * @param [in] src1_block_stride Address stride between the DataBlocks of source operand 1 within a single iteration.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of the destination across adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of source operand 0 across adjacent
 * iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of source operand 1 across adjacent
 * iterations.
 */
__aicore__ inline void asc_le_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_le and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Compares src0 and src1 elementwise on the relation src0 <= src1 in a synchronized composite call and writes
 * the packed 1-bit-per-element result to the uint8_t buffer dst in little-endian bit order.
 *
 * @override_id asc_le_sync__PUu8_PUf32_PUf32_u8_u8_u8_u8_u8_u8_u8
 * @archs 2201
 * @param [out] dst Start address of the destination buffer that receives the packed 1-bit-per-element comparison result
 * in little-endian bit order.
 * @param [in] src0 Start address of source operand 0 (float elements).
 * @param [in] src1 Start address of source operand 1 (float elements).
 * @param [in] repeat Number of iterations the comparison repeats across repeat-sized blocks.
 * @param [in] dst_block_stride Address stride between the DataBlocks of the destination within a single iteration.
 * @param [in] src0_block_stride Address stride between the DataBlocks of source operand 0 within a single iteration.
 * @param [in] src1_block_stride Address stride between the DataBlocks of source operand 1 within a single iteration.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of the destination across adjacent
 * iterations.
 * @param [in] src0_repeat_stride Address stride between the same DataBlock of source operand 0 across adjacent
 * iterations.
 * @param [in] src1_repeat_stride Address stride between the same DataBlock of source operand 1 across adjacent
 * iterations.
 */
__aicore__ inline void asc_le_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_le_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous comparison wrapper: tests src <= value per half element, packs one result bit per
 * element into dst and waits for the vector pipe.
 *
 * @override_id asc_le_scalar_sync__PUu8_PUf16_f16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination buffer holding one comparison bit per source element in
 * little-endian order; must be 32-byte aligned.
 * @param [in] src Start address of the source half vector compared element-wise with value; must be 32-byte aligned.
 * @param [in] value Scalar half value each element is compared against.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of dst.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of src.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of dst in adjacent iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of src in adjacent iterations.
 */
__aicore__ inline void asc_le_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_le_scalar and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous comparison wrapper: tests src <= value per float element, packs one result bit per
 * element into dst and waits for the vector pipe.
 *
 * @override_id asc_le_scalar_sync__PUu8_PUf32_f32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination buffer holding one comparison bit per source element in
 * little-endian order; must be 32-byte aligned.
 * @param [in] src Start address of the source float vector compared element-wise with value; must be 32-byte aligned.
 * @param [in] value Scalar float value each element is compared against.
 * @param [in] repeat Number of iterations of the comparison.
 * @param [in] dst_block_stride Address stride between DataBlocks inside one iteration of dst.
 * @param [in] src_block_stride Address stride between DataBlocks inside one iteration of src.
 * @param [in] dst_repeat_stride Address stride between the same DataBlock of dst in adjacent iterations.
 * @param [in] src_repeat_stride Address stride between the same DataBlock of src in adjacent iterations.
 */
__aicore__ inline void asc_le_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_pair_reduce_sum and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously sums adjacent (even/odd) pairs of half elements from src and writes the pair sums into dst.
 *
 * @override_id asc_pair_reduce_sum_sync__PUf16_PUf16_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the sums of adjacent source pairs (a1+a2, a3+a4, ...).
 * @param [in] src Source vector start address whose adjacent elements are pairwise summed.
 * @param [in] count Number of elements participating in the contiguous computation.
 */
__aicore__ inline void asc_pair_reduce_sum_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_pair_reduce_sum and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously sums adjacent (even/odd) pairs of float elements from src and writes the pair sums into dst.
 *
 * @override_id asc_pair_reduce_sum_sync__PUf32_PUf32_u32
 * @archs 2201
 * @param [out] dst Destination vector start address receiving the sums of adjacent source pairs (a1+a2, a3+a4, ...).
 * @param [in] src Source vector start address whose adjacent elements are pairwise summed.
 * @param [in] count Number of elements participating in the contiguous computation.
 */
__aicore__ inline void asc_pair_reduce_sum_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies count int16_t elements from src to dst inside Unified Buffer and then inserts a PIPE_ALL barrier;
 * deprecated in favour of asc_copy plus asc_sync.
 *
 * @override_id asc_copy_sync__PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the 16-bit signed integer (int16_t) destination vector in Unified Buffer that
 * receives the copied elements.
 * @param [in] src Start address of the 16-bit signed integer (int16_t) source vector in Unified Buffer whose elements
 * are copied.
 * @param [in] count Number of consecutive elements copied; the impl passes it to asc_set_mask_count_begin before the
 * copy instruction.
 */
__aicore__ inline void asc_copy_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies count uint16_t elements from src to dst inside Unified Buffer and then inserts a PIPE_ALL barrier;
 * deprecated in favour of asc_copy plus asc_sync.
 *
 * @override_id asc_copy_sync__PUu16_PUu16_u32
 * @archs 2201
 * @param [out] dst Start address of the 16-bit unsigned integer (uint16_t) destination vector in Unified Buffer that
 * receives the copied elements.
 * @param [in] src Start address of the 16-bit unsigned integer (uint16_t) source vector in Unified Buffer whose
 * elements are copied.
 * @param [in] count Number of consecutive elements copied; the impl passes it to asc_set_mask_count_begin before the
 * copy instruction.
 */
__aicore__ inline void asc_copy_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies count int32_t elements from src to dst inside Unified Buffer and then inserts a PIPE_ALL barrier;
 * deprecated in favour of asc_copy plus asc_sync.
 *
 * @override_id asc_copy_sync__PUi32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the 32-bit signed integer (int32_t) destination vector in Unified Buffer that
 * receives the copied elements.
 * @param [in] src Start address of the 32-bit signed integer (int32_t) source vector in Unified Buffer whose elements
 * are copied.
 * @param [in] count Number of consecutive elements copied; the impl passes it to asc_set_mask_count_begin before the
 * copy instruction.
 */
__aicore__ inline void asc_copy_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Copies count uint32_t elements from src to dst inside Unified Buffer and then inserts a PIPE_ALL barrier;
 * deprecated in favour of asc_copy plus asc_sync.
 *
 * @override_id asc_copy_sync__PUu32_PUu32_u32
 * @archs 2201
 * @param [out] dst Start address of the 32-bit unsigned integer (uint32_t) destination vector in Unified Buffer that
 * receives the copied elements.
 * @param [in] src Start address of the 32-bit unsigned integer (uint32_t) source vector in Unified Buffer whose
 * elements are copied.
 * @param [in] count Number of consecutive elements copied; the impl passes it to asc_set_mask_count_begin before the
 * copy instruction.
 */
__aicore__ inline void asc_copy_sync(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t count);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2ub and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form: copies size bytes from one Unified Buffer region to another.
 *
 * @override_id asc_copy_ub2ub_sync__PUv_PUv_u32
 * @archs 2201 3510
 * @param [out] dst Destination UB address.
 * @param [in] src Source UB address.
 * @param [in] size Number of bytes to transfer; the size must be 32-byte aligned for the bulk copy form.
 */
__aicore__ inline void asc_copy_ub2ub_sync(__ubuf__ void* dst, __ubuf__ void* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form: copies size bytes from Global Memory into the Unified Buffer.
 *
 * @override_id asc_copy_gm2ub_sync__PUv_PGv_u32
 * @archs 2201 3510
 * @param [out] dst Destination UB address; the transfer size must be 32-byte aligned when this bulk form is used.
 * @param [in] src Source Global Memory address.
 * @param [in] size Number of bytes to transfer; the size must satisfy the 32-byte alignment requirement of the bulk
 * copy form.
 */
__aicore__ inline void asc_copy_gm2ub_sync(__ubuf__ void* dst, __gm__ void* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form: copies size bytes from the Unified Buffer to Global Memory.
 *
 * @override_id asc_copy_ub2gm_sync__PGv_PUv_u32
 * @archs 2201 3510
 * @param [out] dst Destination Global Memory address.
 * @param [in] src Source UB address.
 * @param [in] size Number of bytes to transfer; the size must be 32-byte aligned for the bulk copy form.
 */
__aicore__ inline void asc_copy_ub2gm_sync(__gm__ void* dst, __ubuf__ void* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous non-aligned copy that moves size bytes of int8_t data from GM to UB, padding a
 * non-32-byte tail, then waits for the transfer; use asc_copy_gm2ub_align with asc_sync instead.
 *
 * @override_id asc_copy_gm2ub_align_sync__PUi8_PGi8_u32
 * @archs 2201 3510
 * @param [out] dst Destination UB start address of the copied int8_t data; it must be 32-byte aligned.
 * @param [in] src Source GM start address of the int8_t data; it only needs 1-byte alignment.
 * @param [in] size Number of bytes to copy; range 0 to 2097151, and a value of 0 turns the call into a NOP.
 */
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous non-aligned copy that moves size bytes of uint8_t data from GM to UB, padding a
 * non-32-byte tail, then waits for the transfer; use asc_copy_gm2ub_align with asc_sync instead.
 *
 * @override_id asc_copy_gm2ub_align_sync__PUu8_PGu8_u32
 * @archs 2201 3510
 * @param [out] dst Destination UB start address of the copied uint8_t data; it must be 32-byte aligned.
 * @param [in] src Source GM start address of the uint8_t data; it only needs 1-byte alignment.
 * @param [in] size Number of bytes to copy; range 0 to 2097151, and a value of 0 turns the call into a NOP.
 */
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous non-aligned copy that moves size bytes of half data from GM to UB, padding a
 * non-32-byte tail, then waits for the transfer; use asc_copy_gm2ub_align with asc_sync instead.
 *
 * @override_id asc_copy_gm2ub_align_sync__PUf16_PGf16_u32
 * @archs 2201 3510
 * @param [out] dst Destination UB start address of the copied half data; it must be 32-byte aligned.
 * @param [in] src Source GM start address of the half data; it only needs 1-byte alignment.
 * @param [in] size Number of bytes to copy; range 0 to 2097151, and a value of 0 turns the call into a NOP.
 */
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ half* dst, __gm__ half* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous non-aligned copy that moves size bytes of bfloat16_t data from GM to UB, padding a
 * non-32-byte tail, then waits for the transfer; use asc_copy_gm2ub_align with asc_sync instead.
 *
 * @override_id asc_copy_gm2ub_align_sync__PUbf16_PGbf16_u32
 * @archs 2201 3510
 * @param [out] dst Destination UB start address of the copied bfloat16_t data; it must be 32-byte aligned.
 * @param [in] src Source GM start address of the bfloat16_t data; it only needs 1-byte alignment.
 * @param [in] size Number of bytes to copy; range 0 to 2097151, and a value of 0 turns the call into a NOP.
 */
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous non-aligned copy that moves size bytes of int16_t data from GM to UB, padding a
 * non-32-byte tail, then waits for the transfer; use asc_copy_gm2ub_align with asc_sync instead.
 *
 * @override_id asc_copy_gm2ub_align_sync__PUi16_PGi16_u32
 * @archs 2201 3510
 * @param [out] dst Destination UB start address of the copied int16_t data; it must be 32-byte aligned.
 * @param [in] src Source GM start address of the int16_t data; it only needs 1-byte alignment.
 * @param [in] size Number of bytes to copy; range 0 to 2097151, and a value of 0 turns the call into a NOP.
 */
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous non-aligned copy that moves size bytes of uint16_t data from GM to UB, padding a
 * non-32-byte tail, then waits for the transfer; use asc_copy_gm2ub_align with asc_sync instead.
 *
 * @override_id asc_copy_gm2ub_align_sync__PUu16_PGu16_u32
 * @archs 2201 3510
 * @param [out] dst Destination UB start address of the copied uint16_t data; it must be 32-byte aligned.
 * @param [in] src Source GM start address of the uint16_t data; it only needs 1-byte alignment.
 * @param [in] size Number of bytes to copy; range 0 to 2097151, and a value of 0 turns the call into a NOP.
 */
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous non-aligned copy that moves size bytes of float data from GM to UB, padding a
 * non-32-byte tail, then waits for the transfer; use asc_copy_gm2ub_align with asc_sync instead.
 *
 * @override_id asc_copy_gm2ub_align_sync__PUf32_PGf32_u32
 * @archs 2201 3510
 * @param [out] dst Destination UB start address of the copied float data; it must be 32-byte aligned.
 * @param [in] src Source GM start address of the float data; it only needs 1-byte alignment.
 * @param [in] size Number of bytes to copy; range 0 to 2097151, and a value of 0 turns the call into a NOP.
 */
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ float* dst, __gm__ float* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous non-aligned copy that moves size bytes of int32_t data from GM to UB, padding a
 * non-32-byte tail, then waits for the transfer; use asc_copy_gm2ub_align with asc_sync instead.
 *
 * @override_id asc_copy_gm2ub_align_sync__PUi32_PGi32_u32
 * @archs 2201 3510
 * @param [out] dst Destination UB start address of the copied int32_t data; it must be 32-byte aligned.
 * @param [in] src Source GM start address of the int32_t data; it only needs 1-byte alignment.
 * @param [in] size Number of bytes to copy; range 0 to 2097151, and a value of 0 turns the call into a NOP.
 */
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous non-aligned copy that moves size bytes of uint32_t data from GM to UB, padding a
 * non-32-byte tail, then waits for the transfer; use asc_copy_gm2ub_align with asc_sync instead.
 *
 * @override_id asc_copy_gm2ub_align_sync__PUu32_PGu32_u32
 * @archs 2201 3510
 * @param [out] dst Destination UB start address of the copied uint32_t data; it must be 32-byte aligned.
 * @param [in] src Source GM start address of the uint32_t data; it only needs 1-byte alignment.
 * @param [in] size Number of bytes to copy; range 0 to 2097151, and a value of 0 turns the call into a NOP.
 */
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously copies size bytes of unsigned 8-bit integer (uint8_t) data from Unified Buffer to Global Memory,
 * pads each non-32-byte-aligned UB read with dummy data that is dropped on the GM write, and waits for the transfer to
 * finish; deprecated in favor of asc_copy_ub2gm_align plus asc_sync.
 *
 * @override_id asc_copy_ub2gm_align_sync__PGu8_PUu8_u32
 * @archs 2201 3510
 * @param [out] dst Destination start address in Global Memory that receives the unsigned 8-bit integer (uint8_t) data;
 * it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer holding the unsigned 8-bit integer (uint8_t) data; it must be
 * 32-byte aligned.
 * @param [in] size Transfer length in bytes, documented range [0, 2097151]; a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously copies size bytes of signed 8-bit integer (int8_t) data from Unified Buffer to Global Memory,
 * pads each non-32-byte-aligned UB read with dummy data that is dropped on the GM write, and waits for the transfer to
 * finish; deprecated in favor of asc_copy_ub2gm_align plus asc_sync.
 *
 * @override_id asc_copy_ub2gm_align_sync__PGi8_PUi8_u32
 * @archs 2201 3510
 * @param [out] dst Destination start address in Global Memory that receives the signed 8-bit integer (int8_t) data; it
 * must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer holding the signed 8-bit integer (int8_t) data; it must be
 * 32-byte aligned.
 * @param [in] size Transfer length in bytes, documented range [0, 2097151]; a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously copies size bytes of 16-bit floating point (half) data from Unified Buffer to Global Memory,
 * pads each non-32-byte-aligned UB read with dummy data that is dropped on the GM write, and waits for the transfer to
 * finish; deprecated in favor of asc_copy_ub2gm_align plus asc_sync.
 *
 * @override_id asc_copy_ub2gm_align_sync__PGf16_PUf16_u32
 * @archs 2201 3510
 * @param [out] dst Destination start address in Global Memory that receives the 16-bit floating point (half) data; it
 * must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer holding the 16-bit floating point (half) data; it must be
 * 32-byte aligned.
 * @param [in] size Transfer length in bytes, documented range [0, 2097151]; a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ half* dst, __ubuf__ half* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously copies size bytes of unsigned 16-bit integer (uint16_t) data from Unified Buffer to Global
 * Memory, pads each non-32-byte-aligned UB read with dummy data that is dropped on the GM write, and waits for the
 * transfer to finish; deprecated in favor of asc_copy_ub2gm_align plus asc_sync.
 *
 * @override_id asc_copy_ub2gm_align_sync__PGu16_PUu16_u32
 * @archs 2201 3510
 * @param [out] dst Destination start address in Global Memory that receives the unsigned 16-bit integer (uint16_t)
 * data; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer holding the unsigned 16-bit integer (uint16_t) data; it must
 * be 32-byte aligned.
 * @param [in] size Transfer length in bytes, documented range [0, 2097151]; a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously copies size bytes of signed 16-bit integer (int16_t) data from Unified Buffer to Global Memory,
 * pads each non-32-byte-aligned UB read with dummy data that is dropped on the GM write, and waits for the transfer to
 * finish; deprecated in favor of asc_copy_ub2gm_align plus asc_sync.
 *
 * @override_id asc_copy_ub2gm_align_sync__PGi16_PUi16_u32
 * @archs 2201 3510
 * @param [out] dst Destination start address in Global Memory that receives the signed 16-bit integer (int16_t) data;
 * it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer holding the signed 16-bit integer (int16_t) data; it must be
 * 32-byte aligned.
 * @param [in] size Transfer length in bytes, documented range [0, 2097151]; a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously copies size bytes of 16-bit brain float (bfloat16_t) data from Unified Buffer to Global Memory,
 * pads each non-32-byte-aligned UB read with dummy data that is dropped on the GM write, and waits for the transfer to
 * finish; deprecated in favor of asc_copy_ub2gm_align plus asc_sync.
 *
 * @override_id asc_copy_ub2gm_align_sync__PGbf16_PUbf16_u32
 * @archs 2201 3510
 * @param [out] dst Destination start address in Global Memory that receives the 16-bit brain float (bfloat16_t) data;
 * it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer holding the 16-bit brain float (bfloat16_t) data; it must be
 * 32-byte aligned.
 * @param [in] size Transfer length in bytes, documented range [0, 2097151]; a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously copies size bytes of unsigned 32-bit integer (uint32_t) data from Unified Buffer to Global
 * Memory, pads each non-32-byte-aligned UB read with dummy data that is dropped on the GM write, and waits for the
 * transfer to finish; deprecated in favor of asc_copy_ub2gm_align plus asc_sync.
 *
 * @override_id asc_copy_ub2gm_align_sync__PGu32_PUu32_u32
 * @archs 2201 3510
 * @param [out] dst Destination start address in Global Memory that receives the unsigned 32-bit integer (uint32_t)
 * data; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer holding the unsigned 32-bit integer (uint32_t) data; it must
 * be 32-byte aligned.
 * @param [in] size Transfer length in bytes, documented range [0, 2097151]; a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously copies size bytes of 32-bit floating point (float) data from Unified Buffer to Global Memory,
 * pads each non-32-byte-aligned UB read with dummy data that is dropped on the GM write, and waits for the transfer to
 * finish; deprecated in favor of asc_copy_ub2gm_align plus asc_sync.
 *
 * @override_id asc_copy_ub2gm_align_sync__PGf32_PUf32_u32
 * @archs 2201 3510
 * @param [out] dst Destination start address in Global Memory that receives the 32-bit floating point (float) data; it
 * must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer holding the 32-bit floating point (float) data; it must be
 * 32-byte aligned.
 * @param [in] size Transfer length in bytes, documented range [0, 2097151]; a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ float* dst, __ubuf__ float* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously copies size bytes of signed 32-bit integer (int32_t) data from Unified Buffer to Global Memory,
 * pads each non-32-byte-aligned UB read with dummy data that is dropped on the GM write, and waits for the transfer to
 * finish; deprecated in favor of asc_copy_ub2gm_align plus asc_sync.
 *
 * @override_id asc_copy_ub2gm_align_sync__PGi32_PUi32_u32
 * @archs 2201 3510
 * @param [out] dst Destination start address in Global Memory that receives the signed 32-bit integer (int32_t) data;
 * it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer holding the signed 32-bit integer (int32_t) data; it must be
 * 32-byte aligned.
 * @param [in] size Transfer length in bytes, documented range [0, 2097151]; a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously copies size bytes of 64-bit floating point (double) data from Unified Buffer to Global Memory,
 * pads each non-32-byte-aligned UB read with dummy data that is dropped on the GM write, and waits for the transfer to
 * finish; deprecated in favor of asc_copy_ub2gm_align plus asc_sync.
 *
 * @override_id asc_copy_ub2gm_align_sync__PGf64_PUf64_u32
 * @archs 2201
 * @param [out] dst Destination start address in Global Memory that receives the 64-bit floating point (double) data; it
 * must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer holding the 64-bit floating point (double) data; it must be
 * 32-byte aligned.
 * @param [in] size Transfer length in bytes, documented range [0, 2097151]; a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ double* dst, __ubuf__ double* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously copies size bytes of signed 64-bit integer (int64_t) data from Unified Buffer to Global Memory,
 * pads each non-32-byte-aligned UB read with dummy data that is dropped on the GM write, and waits for the transfer to
 * finish; deprecated in favor of asc_copy_ub2gm_align plus asc_sync.
 *
 * @override_id asc_copy_ub2gm_align_sync__PGi64_PUi64_u32
 * @archs 2201
 * @param [out] dst Destination start address in Global Memory that receives the signed 64-bit integer (int64_t) data;
 * it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer holding the signed 64-bit integer (int64_t) data; it must be
 * 32-byte aligned.
 * @param [in] size Transfer length in bytes, documented range [0, 2097151]; a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int64_t* dst, __ubuf__ int64_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Synchronously copies size bytes of unsigned 64-bit integer (uint64_t) data from Unified Buffer to Global
 * Memory, pads each non-32-byte-aligned UB read with dummy data that is dropped on the GM write, and waits for the
 * transfer to finish; deprecated in favor of asc_copy_ub2gm_align plus asc_sync.
 *
 * @override_id asc_copy_ub2gm_align_sync__PGu64_PUu64_u32
 * @archs 2201
 * @param [out] dst Destination start address in Global Memory that receives the unsigned 64-bit integer (uint64_t)
 * data; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer holding the unsigned 64-bit integer (uint64_t) data; it must
 * be 32-byte aligned.
 * @param [in] size Transfer length in bytes, documented range [0, 2097151]; a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint64_t* dst, __ubuf__ uint64_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous non-aligned copy from Global Memory into the Unified Buffer that pads the transfer to
 * 32 bytes and waits for the MTE2 pipe.
 *
 * @override_id asc_copy_gm2ub_align_sync__PUhf8_PGhf8_u32
 * @archs 3510
 * @param [out] dst Start address of the destination UB buffer; it must be 32-byte aligned and must have room for the
 * 32-byte padded size.
 * @param [in] src Start address of the source GM buffer; it only needs 1-byte alignment.
 * @param [in] size Number of bytes to copy, range [0, 2097151]; a value of 0 makes the interface a NOP.
 */
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous non-aligned copy from Global Memory into the Unified Buffer that pads the transfer to
 * 32 bytes and waits for the MTE2 pipe.
 *
 * @override_id asc_copy_gm2ub_align_sync__PUf8e5m2_PGf8e5m2_u32
 * @archs 3510
 * @param [out] dst Start address of the destination UB buffer; it must be 32-byte aligned and must have room for the
 * 32-byte padded size.
 * @param [in] src Start address of the source GM buffer; it only needs 1-byte alignment.
 * @param [in] size Number of bytes to copy, range [0, 2097151]; a value of 0 makes the interface a NOP.
 */
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous non-aligned copy from Global Memory into the Unified Buffer that pads the transfer to
 * 32 bytes and waits for the MTE2 pipe.
 *
 * @override_id asc_copy_gm2ub_align_sync__PUf8e4m3_PGf8e4m3_u32
 * @archs 3510
 * @param [out] dst Start address of the destination UB buffer; it must be 32-byte aligned and must have room for the
 * 32-byte padded size.
 * @param [in] src Start address of the source GM buffer; it only needs 1-byte alignment.
 * @param [in] size Number of bytes to copy, range [0, 2097151]; a value of 0 makes the interface a NOP.
 */
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous non-aligned copy from the Unified Buffer into Global Memory that discards the 32-byte
 * padding on write and waits for the MTE3 pipe.
 *
 * @override_id asc_copy_ub2gm_align_sync__PGhf8_PUhf8_u32
 * @archs 3510
 * @param [out] dst Start address of the destination GM buffer; it only needs 1-byte alignment.
 * @param [in] src Start address of the source UB buffer; it must be 32-byte aligned, and dummy data is read when size
 * is not 32-byte aligned.
 * @param [in] size Number of bytes to copy, range [0, 2097151]; a value of 0 makes the interface a NOP.
 */
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous non-aligned copy from the Unified Buffer into Global Memory that discards the 32-byte
 * padding on write and waits for the MTE3 pipe.
 *
 * @override_id asc_copy_ub2gm_align_sync__PGf8e5m2_PUf8e5m2_u32
 * @archs 3510
 * @param [out] dst Start address of the destination GM buffer; it only needs 1-byte alignment.
 * @param [in] src Start address of the source UB buffer; it must be 32-byte aligned, and dummy data is read when size
 * is not 32-byte aligned.
 * @param [in] size Number of bytes to copy, range [0, 2097151]; a value of 0 makes the interface a NOP.
 */
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous non-aligned copy from the Unified Buffer into Global Memory that discards the 32-byte
 * padding on write and waits for the MTE3 pipe.
 *
 * @override_id asc_copy_ub2gm_align_sync__PGf8e4m3_PUf8e4m3_u32
 * @archs 3510
 * @param [out] dst Start address of the destination GM buffer; it only needs 1-byte alignment.
 * @param [in] src Start address of the source UB buffer; it must be 32-byte aligned, and dummy data is read when size
 * is not 32-byte aligned.
 * @param [in] size Number of bytes to copy, range [0, 2097151]; a value of 0 makes the interface a NOP.
 */
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint32_t size);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2l1 and asc_sync)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated synchronous form: copies a contiguous byte range from the Unified Buffer to L1 Buffer and waits for
 * completion.
 *
 * @override_id asc_copy_ub2l1_sync__PL1v_PUv_u32
 * @archs 3510
 * @param [out] dst Start address of the destination in L1 Buffer.
 * @param [in] src Start address of the source in UB.
 * @param [in] size Number of bytes to transfer.
 */
__aicore__ inline void asc_copy_ub2l1_sync(__cbuf__ void* dst, __ubuf__ void* src, uint32_t size);

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_VECTOR_COMPUTE_COMPOSITE_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_COMPOSITE_VECTOR_COMPUTE_COMPOSITE_H
#endif
