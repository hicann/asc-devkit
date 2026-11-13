/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "vector_datamove.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_DATAMOVE_VECTOR_DATAMOVE_H
#endif

#ifndef INCLUDE_C_API_VECTOR_DATAMOVE_VECTOR_DATAMOVE_H
#define INCLUDE_C_API_VECTOR_DATAMOVE_VECTOR_DATAMOVE_H

#include "c_api/defs/defs.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/memory_base_impl/vector_datamove_intf_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/vector_datamove_intf_impl.h"
#endif

#include "c_api/composite/vector_compute_composite.h"

//===========asc_set_copy_pad_val(int8/uint8/int16/uint16/half/bfloat16/int32/uint32/float/int64_t/uint64_t)===========
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the int8_t value used to pad the left and right sides of consecutive data blocks by asc_copy_gm2ub_align
 * when constant padding is enabled, staying in effect until it is set again.
 *
 * @override_id asc_set_copy_pad_val__i8
 * @archs 2201 3510
 * @param [in] pad_value int8_t padding value to be written by the subsequent asc_copy_gm2ub_align copy; the register
 * write is performed only on AIV and the value is taken from the low 8 bits of the register.
 */
__aicore__ inline void asc_set_copy_pad_val(int8_t pad_value);

/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the uint8_t value used to pad the left and right sides of consecutive data blocks by asc_copy_gm2ub_align
 * when constant padding is enabled, staying in effect until it is set again.
 *
 * @override_id asc_set_copy_pad_val__u8
 * @archs 2201 3510
 * @param [in] pad_value uint8_t padding value to be written by the subsequent asc_copy_gm2ub_align copy; the register
 * write is performed only on AIV and the value is taken from the low 8 bits of the register.
 */
__aicore__ inline void asc_set_copy_pad_val(uint8_t pad_value);

/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the int16_t value used to pad the left and right sides of consecutive data blocks by asc_copy_gm2ub_align
 * when constant padding is enabled, staying in effect until it is set again.
 *
 * @override_id asc_set_copy_pad_val__i16
 * @archs 2201 3510
 * @param [in] pad_value int16_t padding value to be written by the subsequent asc_copy_gm2ub_align copy; the register
 * write is performed only on AIV and the value is taken from the low 16 bits of the register.
 */
__aicore__ inline void asc_set_copy_pad_val(int16_t pad_value);

/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the uint16_t value used to pad the left and right sides of consecutive data blocks by
 * asc_copy_gm2ub_align when constant padding is enabled, staying in effect until it is set again.
 *
 * @override_id asc_set_copy_pad_val__u16
 * @archs 2201 3510
 * @param [in] pad_value uint16_t padding value to be written by the subsequent asc_copy_gm2ub_align copy; the register
 * write is performed only on AIV and the value is taken from the low 16 bits of the register.
 */
__aicore__ inline void asc_set_copy_pad_val(uint16_t pad_value);

/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the half value used to pad the left and right sides of consecutive data blocks by asc_copy_gm2ub_align
 * when constant padding is enabled, staying in effect until it is set again.
 *
 * @override_id asc_set_copy_pad_val__f16
 * @archs 2201 3510
 * @param [in] pad_value half padding value to be written by the subsequent asc_copy_gm2ub_align copy; the register
 * write is performed only on AIV and the value is taken from the low 16 bits of the register.
 */
__aicore__ inline void asc_set_copy_pad_val(half pad_value);

/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the bfloat16_t value used to pad the left and right sides of consecutive data blocks by
 * asc_copy_gm2ub_align when constant padding is enabled, staying in effect until it is set again.
 *
 * @override_id asc_set_copy_pad_val__bf16
 * @archs 2201 3510
 * @param [in] pad_value bfloat16_t padding value to be written by the subsequent asc_copy_gm2ub_align copy; the
 * register write is performed only on AIV and the value is taken from the low 16 bits of the register.
 */
__aicore__ inline void asc_set_copy_pad_val(bfloat16_t pad_value);

/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the int32_t value used to pad the left and right sides of consecutive data blocks by asc_copy_gm2ub_align
 * when constant padding is enabled, staying in effect until it is set again.
 *
 * @override_id asc_set_copy_pad_val__i32
 * @archs 2201 3510
 * @param [in] pad_value int32_t padding value to be written by the subsequent asc_copy_gm2ub_align copy; the register
 * write is performed only on AIV and the value is taken from the low 32 bits of the register.
 */
__aicore__ inline void asc_set_copy_pad_val(int32_t pad_value);

/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the uint32_t value used to pad the left and right sides of consecutive data blocks by
 * asc_copy_gm2ub_align when constant padding is enabled, staying in effect until it is set again.
 *
 * @override_id asc_set_copy_pad_val__u32
 * @archs 2201 3510
 * @param [in] pad_value uint32_t padding value to be written by the subsequent asc_copy_gm2ub_align copy; the register
 * write is performed only on AIV and the value is taken from the low 32 bits of the register.
 */
__aicore__ inline void asc_set_copy_pad_val(uint32_t pad_value);

/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the float value used to pad the left and right sides of consecutive data blocks by asc_copy_gm2ub_align
 * when constant padding is enabled, staying in effect until it is set again.
 *
 * @override_id asc_set_copy_pad_val__f32
 * @archs 2201 3510
 * @param [in] pad_value float padding value to be written by the subsequent asc_copy_gm2ub_align copy; the register
 * write is performed only on AIV and the value is taken from the low 32 bits of the register.
 */
__aicore__ inline void asc_set_copy_pad_val(float pad_value);
__aicore__ inline void asc_set_copy_pad_val(int64_t pad_value);
__aicore__ inline void asc_set_copy_pad_val(uint64_t pad_value);

/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies data inside Unified Buffer on AIV in burst_count blocks of burst_len DataBlocks, with source and
 * destination gaps between blocks.
 *
 * @override_id asc_copy_ub2ub__PUv_PUv_u16_u16_u16_u16
 * @archs 2201 3510
 * @param [out] dst Destination UB start address; must be 32-byte aligned.
 * @param [in] src Source UB start address; must be 32-byte aligned.
 * @param [in] burst_count Number of consecutive data blocks to move; range [1, 65535].
 * @param [in] burst_len Length of each consecutive block in DataBlocks of 32 bytes; range [1, 65535].
 * @param [in] src_gap Gap between adjacent source blocks in DataBlocks of 32 bytes; range [0, 2^16 - 1].
 * @param [in] dst_gap Gap between adjacent destination blocks in DataBlocks of 32 bytes; range [0, 2^16 - 1].
 */
__aicore__ inline void asc_copy_ub2ub(
    __ubuf__ void* dst, __ubuf__ void* src, uint16_t burst_count, uint16_t burst_len, uint16_t src_gap,
    uint16_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies a contiguous byte range inside Unified Buffer on AIV, with a size between 32 bytes and 65535
 * DataBlocks.
 *
 * @override_id asc_copy_ub2ub__PUv_PUv_u32
 * @archs 2201 3510
 * @param [out] dst Destination UB start address; must be 32-byte aligned.
 * @param [in] src Source UB start address; must be 32-byte aligned.
 * @param [in] size Number of bytes to copy; range [32, 65535 x 32] and a multiple of 32.
 */
__aicore__ inline void asc_copy_ub2ub(__ubuf__ void* dst, __ubuf__ void* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies the first count int16_t elements from Unified Buffer src to Unified Buffer dst.
 *
 * @override_id asc_copy__PUi16_PUi16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int16_t elements that receives the copied
 * elements.
 * @param [in] src Start address of the source vector of int16_t elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_copy(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies int16_t elements from Unified Buffer src to Unified Buffer dst over repeat vector iterations using
 * per-operand DataBlock and repeat strides.
 *
 * @override_id asc_copy__PUi16_PUi16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int16_t elements that receives the copied
 * elements.
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
__aicore__ inline void asc_copy(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies the first count uint16_t elements from Unified Buffer src to Unified Buffer dst.
 *
 * @override_id asc_copy__PUu16_PUu16_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of uint16_t elements that receives the copied
 * elements.
 * @param [in] src Start address of the source vector of uint16_t elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_copy(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies uint16_t elements from Unified Buffer src to Unified Buffer dst over repeat vector iterations using
 * per-operand DataBlock and repeat strides.
 *
 * @override_id asc_copy__PUu16_PUu16_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of uint16_t elements that receives the copied
 * elements.
 * @param [in] src Start address of the source vector of uint16_t elements in Unified Buffer, read element-wise.
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
__aicore__ inline void asc_copy(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies the first count int32_t elements from Unified Buffer src to Unified Buffer dst.
 *
 * @override_id asc_copy__PUi32_PUi32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int32_t elements that receives the copied
 * elements.
 * @param [in] src Start address of the source vector of int32_t elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_copy(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies int32_t elements from Unified Buffer src to Unified Buffer dst over repeat vector iterations using
 * per-operand DataBlock and repeat strides.
 *
 * @override_id asc_copy__PUi32_PUi32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of int32_t elements that receives the copied
 * elements.
 * @param [in] src Start address of the source vector of int32_t elements in Unified Buffer, read element-wise.
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
__aicore__ inline void asc_copy(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies the first count uint32_t elements from Unified Buffer src to Unified Buffer dst.
 *
 * @override_id asc_copy__PUu32_PUu32_u32
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of uint32_t elements that receives the copied
 * elements.
 * @param [in] src Start address of the source vector of uint32_t elements in Unified Buffer, read element-wise.
 * @param [in] count Number of leading elements to process; it is programmed as the vector mask count for one default
 * iteration and restored to normal mask mode afterwards.
 */
__aicore__ inline void asc_copy(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t count);

/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies uint32_t elements from Unified Buffer src to Unified Buffer dst over repeat vector iterations using
 * per-operand DataBlock and repeat strides.
 *
 * @override_id asc_copy__PUu32_PUu32_u8_u16_u16_u16_u16
 * @archs 2201
 * @param [out] dst Start address of the destination Unified Buffer region of uint32_t elements that receives the copied
 * elements.
 * @param [in] src Start address of the source vector of uint32_t elements in Unified Buffer, read element-wise.
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
__aicore__ inline void asc_copy(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride);

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub
 * @brief Copies data from Global Memory to Unified Buffer in burst_count blocks of burst_len DataBlocks, with source
 * and destination gaps between blocks.
 *
 * @override_id asc_copy_gm2ub__PUv_PGv_u16_u16_u16_u16
 * @archs 2201 3510
 * @param [out] dst Destination UB address; DataBlocks are written with dst_gap spacing.
 * @param [in] src Source Global Memory address; DataBlocks are read with src_gap spacing.
 * @param [in] burst_count Number of consecutive transfer blocks to move; range [1, 4095].
 * @param [in] burst_len Length of each consecutive transfer block in DataBlocks of 32 bytes; range [1, 65535].
 * @param [in] src_gap Gap between adjacent source blocks (tail of one block to head of the next), in DataBlocks of 32
 * bytes.
 * @param [in] dst_gap Gap between adjacent destination blocks (tail of one block to head of the next), in DataBlocks of
 * 32 bytes.
 */
__aicore__ inline void asc_copy_gm2ub(
    __ubuf__ void* dst, __gm__ void* src, uint16_t burst_count, uint16_t burst_len, uint16_t src_gap, uint16_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm
 * @brief Copies data from Unified Buffer to Global Memory in burst_count blocks of burst_len DataBlocks, with source
 * and destination gaps between blocks.
 *
 * @override_id asc_copy_ub2gm__PGv_PUv_u16_u16_u16_u16
 * @archs 2201 3510
 * @param [out] dst Destination Global Memory address; DataBlocks are written with dst_gap spacing.
 * @param [in] src Source UB address; DataBlocks are read with src_gap spacing.
 * @param [in] burst_count Number of consecutive transfer blocks to move; range [1, 4095].
 * @param [in] burst_len Length of each consecutive transfer block in DataBlocks of 32 bytes; range [1, 65535].
 * @param [in] src_gap Gap between adjacent source blocks, in DataBlocks of 32 bytes.
 * @param [in] dst_gap Gap between adjacent destination blocks, in DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_copy_ub2gm(
    __gm__ void* dst, __ubuf__ void* src, uint16_t burst_count, uint16_t burst_len, uint16_t src_gap, uint16_t dst_gap);
#endif

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub
 * @brief Copies a contiguous byte range from Global Memory to Unified Buffer.
 *
 * @override_id asc_copy_gm2ub__PUv_PGv_u32
 * @archs 2201 3510
 * @param [out] dst Destination UB address receiving the copied data.
 * @param [in] src Source Global Memory address.
 * @param [in] size Number of bytes to copy; the size must be 32-byte aligned for this contiguous prototype.
 */
__aicore__ inline void asc_copy_gm2ub(__ubuf__ void* dst, __gm__ void* src, uint32_t size);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm
 * @brief Copies a contiguous byte range from Unified Buffer to Global Memory.
 *
 * @override_id asc_copy_ub2gm__PGv_PUv_u32
 * @archs 2201 3510
 * @param [out] dst Destination Global Memory address receiving the copied data.
 * @param [in] src Source UB address.
 * @param [in] size Number of bytes to copy; the size must be 32-byte aligned for this contiguous prototype.
 */
__aicore__ inline void asc_copy_ub2gm(__gm__ void* dst, __ubuf__ void* src, uint32_t size);

// asc_copy_gm2ub_align  int8_t / uint8_t / half / bfloat16_t / int16_t / uint16_t / float / int32_t / uint32_t
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Moves int8_t data from Global Memory into Unified Buffer as Normal-mode bursts of burst_count blocks with
 * burst_len bytes each, padding every block to a 32-byte boundary.
 *
 * @override_id asc_copy_gm2ub_align__PUi8_PGi8_u16_u32_u8_u8_u32_u32
 * @archs 2201
 * @param [out] dst Start address of the Unified Buffer region that receives the copied int8_t data; must be 32-byte
 * aligned.
 * @param [in] src Start address of the Global Memory region holding the int8_t data to read; must be 1-byte aligned.
 * @param [in] burst_count Number of consecutive transfer blocks to move, in the range [0, 4095]; a value of 0 makes the
 * call a no-op.
 * @param [in] burst_len Length in bytes of each consecutive transfer block, in the range [0, 2097151]; a value of 0
 * makes the call a no-op.
 * @param [in] left_padding_num Number of int8_t elements padded to the left of each block; the resulting padded data
 * must not exceed 32 bytes.
 * @param [in] right_padding_num Number of int8_t elements padded to the right of each block; the resulting padded data
 * must not exceed 32 bytes.
 * @param [in] src_gap Gap in bytes between the tail of one source block and the head of the next source block.
 * @param [in] dst_gap Gap between consecutive destination blocks, measured in DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ int8_t* dst, __gm__ int8_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Moves int8_t elements from Global Memory into Unified Buffer in one run of size bytes, padding the run up to a
 * 32-byte boundary.
 *
 * @override_id asc_copy_gm2ub_align__PUi8_PGi8_u32
 * @archs 2201 3510
 * @param [out] dst Start address of the Unified Buffer region that receives the copied int8_t data; must be 32-byte
 * aligned.
 * @param [in] src Start address of the Global Memory region holding the int8_t data to read; must be 1-byte aligned.
 * @param [in] size Transfer length in bytes, in the range [0, 2097151]; a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Moves uint8_t data from Global Memory into Unified Buffer as Normal-mode bursts of burst_count blocks with
 * burst_len bytes each, padding every block to a 32-byte boundary.
 *
 * @override_id asc_copy_gm2ub_align__PUu8_PGu8_u16_u32_u8_u8_u32_u32
 * @archs 2201
 * @param [out] dst Start address of the Unified Buffer region that receives the copied uint8_t data; must be 32-byte
 * aligned.
 * @param [in] src Start address of the Global Memory region holding the uint8_t data to read; must be 1-byte aligned.
 * @param [in] burst_count Number of consecutive transfer blocks to move, in the range [0, 4095]; a value of 0 makes the
 * call a no-op.
 * @param [in] burst_len Length in bytes of each consecutive transfer block, in the range [0, 2097151]; a value of 0
 * makes the call a no-op.
 * @param [in] left_padding_num Number of uint8_t elements padded to the left of each block; the resulting padded data
 * must not exceed 32 bytes.
 * @param [in] right_padding_num Number of uint8_t elements padded to the right of each block; the resulting padded data
 * must not exceed 32 bytes.
 * @param [in] src_gap Gap in bytes between the tail of one source block and the head of the next source block.
 * @param [in] dst_gap Gap between consecutive destination blocks, measured in DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Moves uint8_t elements from Global Memory into Unified Buffer in one run of size bytes, padding the run up to
 * a 32-byte boundary.
 *
 * @override_id asc_copy_gm2ub_align__PUu8_PGu8_u32
 * @archs 2201 3510
 * @param [out] dst Start address of the Unified Buffer region that receives the copied uint8_t data; must be 32-byte
 * aligned.
 * @param [in] src Start address of the Global Memory region holding the uint8_t data to read; must be 1-byte aligned.
 * @param [in] size Transfer length in bytes, in the range [0, 2097151]; a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Moves half data from Global Memory into Unified Buffer as Normal-mode bursts of burst_count blocks with
 * burst_len bytes each, padding every block to a 32-byte boundary.
 *
 * @override_id asc_copy_gm2ub_align__PUf16_PGf16_u16_u32_u8_u8_u32_u32
 * @archs 2201
 * @param [out] dst Start address of the Unified Buffer region that receives the copied half data; must be 32-byte
 * aligned.
 * @param [in] src Start address of the Global Memory region holding the half data to read; must be 1-byte aligned.
 * @param [in] burst_count Number of consecutive transfer blocks to move, in the range [0, 4095]; a value of 0 makes the
 * call a no-op.
 * @param [in] burst_len Length in bytes of each consecutive transfer block, in the range [0, 2097151]; a value of 0
 * makes the call a no-op.
 * @param [in] left_padding_num Number of half elements padded to the left of each block; the resulting padded data must
 * not exceed 32 bytes.
 * @param [in] right_padding_num Number of half elements padded to the right of each block; the resulting padded data
 * must not exceed 32 bytes.
 * @param [in] src_gap Gap in bytes between the tail of one source block and the head of the next source block.
 * @param [in] dst_gap Gap between consecutive destination blocks, measured in DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ half* dst, __gm__ half* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Moves half elements from Global Memory into Unified Buffer in one run of size bytes, padding the run up to a
 * 32-byte boundary.
 *
 * @override_id asc_copy_gm2ub_align__PUf16_PGf16_u32
 * @archs 2201 3510
 * @param [out] dst Start address of the Unified Buffer region that receives the copied half data; must be 32-byte
 * aligned.
 * @param [in] src Start address of the Global Memory region holding the half data to read; must be 1-byte aligned.
 * @param [in] size Transfer length in bytes, in the range [0, 2097151]; a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ half* dst, __gm__ half* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Moves bfloat16_t data from Global Memory into Unified Buffer as Normal-mode bursts of burst_count blocks with
 * burst_len bytes each, padding every block to a 32-byte boundary.
 *
 * @override_id asc_copy_gm2ub_align__PUbf16_PGbf16_u16_u32_u8_u8_u32_u32
 * @archs 2201
 * @param [out] dst Start address of the Unified Buffer region that receives the copied bfloat16_t data; must be 32-byte
 * aligned.
 * @param [in] src Start address of the Global Memory region holding the bfloat16_t data to read; must be 1-byte
 * aligned.
 * @param [in] burst_count Number of consecutive transfer blocks to move, in the range [0, 4095]; a value of 0 makes the
 * call a no-op.
 * @param [in] burst_len Length in bytes of each consecutive transfer block, in the range [0, 2097151]; a value of 0
 * makes the call a no-op.
 * @param [in] left_padding_num Number of bfloat16_t elements padded to the left of each block; the resulting padded
 * data must not exceed 32 bytes.
 * @param [in] right_padding_num Number of bfloat16_t elements padded to the right of each block; the resulting padded
 * data must not exceed 32 bytes.
 * @param [in] src_gap Gap in bytes between the tail of one source block and the head of the next source block.
 * @param [in] dst_gap Gap between consecutive destination blocks, measured in DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Moves bfloat16_t elements from Global Memory into Unified Buffer in one run of size bytes, padding the run up
 * to a 32-byte boundary.
 *
 * @override_id asc_copy_gm2ub_align__PUbf16_PGbf16_u32
 * @archs 2201 3510
 * @param [out] dst Start address of the Unified Buffer region that receives the copied bfloat16_t data; must be 32-byte
 * aligned.
 * @param [in] src Start address of the Global Memory region holding the bfloat16_t data to read; must be 1-byte
 * aligned.
 * @param [in] size Transfer length in bytes, in the range [0, 2097151]; a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Moves int16_t data from Global Memory into Unified Buffer as Normal-mode bursts of burst_count blocks with
 * burst_len bytes each, padding every block to a 32-byte boundary.
 *
 * @override_id asc_copy_gm2ub_align__PUi16_PGi16_u16_u32_u8_u8_u32_u32
 * @archs 2201
 * @param [out] dst Start address of the Unified Buffer region that receives the copied int16_t data; must be 32-byte
 * aligned.
 * @param [in] src Start address of the Global Memory region holding the int16_t data to read; must be 1-byte aligned.
 * @param [in] burst_count Number of consecutive transfer blocks to move, in the range [0, 4095]; a value of 0 makes the
 * call a no-op.
 * @param [in] burst_len Length in bytes of each consecutive transfer block, in the range [0, 2097151]; a value of 0
 * makes the call a no-op.
 * @param [in] left_padding_num Number of int16_t elements padded to the left of each block; the resulting padded data
 * must not exceed 32 bytes.
 * @param [in] right_padding_num Number of int16_t elements padded to the right of each block; the resulting padded data
 * must not exceed 32 bytes.
 * @param [in] src_gap Gap in bytes between the tail of one source block and the head of the next source block.
 * @param [in] dst_gap Gap between consecutive destination blocks, measured in DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ int16_t* dst, __gm__ int16_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Moves int16_t elements from Global Memory into Unified Buffer in one run of size bytes, padding the run up to
 * a 32-byte boundary.
 *
 * @override_id asc_copy_gm2ub_align__PUi16_PGi16_u32
 * @archs 2201 3510
 * @param [out] dst Start address of the Unified Buffer region that receives the copied int16_t data; must be 32-byte
 * aligned.
 * @param [in] src Start address of the Global Memory region holding the int16_t data to read; must be 1-byte aligned.
 * @param [in] size Transfer length in bytes, in the range [0, 2097151]; a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Moves uint16_t data from Global Memory into Unified Buffer as Normal-mode bursts of burst_count blocks with
 * burst_len bytes each, padding every block to a 32-byte boundary.
 *
 * @override_id asc_copy_gm2ub_align__PUu16_PGu16_u16_u32_u8_u8_u32_u32
 * @archs 2201
 * @param [out] dst Start address of the Unified Buffer region that receives the copied uint16_t data; must be 32-byte
 * aligned.
 * @param [in] src Start address of the Global Memory region holding the uint16_t data to read; must be 1-byte aligned.
 * @param [in] burst_count Number of consecutive transfer blocks to move, in the range [0, 4095]; a value of 0 makes the
 * call a no-op.
 * @param [in] burst_len Length in bytes of each consecutive transfer block, in the range [0, 2097151]; a value of 0
 * makes the call a no-op.
 * @param [in] left_padding_num Number of uint16_t elements padded to the left of each block; the resulting padded data
 * must not exceed 32 bytes.
 * @param [in] right_padding_num Number of uint16_t elements padded to the right of each block; the resulting padded
 * data must not exceed 32 bytes.
 * @param [in] src_gap Gap in bytes between the tail of one source block and the head of the next source block.
 * @param [in] dst_gap Gap between consecutive destination blocks, measured in DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Moves uint16_t elements from Global Memory into Unified Buffer in one run of size bytes, padding the run up to
 * a 32-byte boundary.
 *
 * @override_id asc_copy_gm2ub_align__PUu16_PGu16_u32
 * @archs 2201 3510
 * @param [out] dst Start address of the Unified Buffer region that receives the copied uint16_t data; must be 32-byte
 * aligned.
 * @param [in] src Start address of the Global Memory region holding the uint16_t data to read; must be 1-byte aligned.
 * @param [in] size Transfer length in bytes, in the range [0, 2097151]; a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Moves float data from Global Memory into Unified Buffer as Normal-mode bursts of burst_count blocks with
 * burst_len bytes each, padding every block to a 32-byte boundary.
 *
 * @override_id asc_copy_gm2ub_align__PUf32_PGf32_u16_u32_u8_u8_u32_u32
 * @archs 2201
 * @param [out] dst Start address of the Unified Buffer region that receives the copied float data; must be 32-byte
 * aligned.
 * @param [in] src Start address of the Global Memory region holding the float data to read; must be 1-byte aligned.
 * @param [in] burst_count Number of consecutive transfer blocks to move, in the range [0, 4095]; a value of 0 makes the
 * call a no-op.
 * @param [in] burst_len Length in bytes of each consecutive transfer block, in the range [0, 2097151]; a value of 0
 * makes the call a no-op.
 * @param [in] left_padding_num Number of float elements padded to the left of each block; the resulting padded data
 * must not exceed 32 bytes.
 * @param [in] right_padding_num Number of float elements padded to the right of each block; the resulting padded data
 * must not exceed 32 bytes.
 * @param [in] src_gap Gap in bytes between the tail of one source block and the head of the next source block.
 * @param [in] dst_gap Gap between consecutive destination blocks, measured in DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ float* dst, __gm__ float* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Moves float elements from Global Memory into Unified Buffer in one run of size bytes, padding the run up to a
 * 32-byte boundary.
 *
 * @override_id asc_copy_gm2ub_align__PUf32_PGf32_u32
 * @archs 2201 3510
 * @param [out] dst Start address of the Unified Buffer region that receives the copied float data; must be 32-byte
 * aligned.
 * @param [in] src Start address of the Global Memory region holding the float data to read; must be 1-byte aligned.
 * @param [in] size Transfer length in bytes, in the range [0, 2097151]; a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ float* dst, __gm__ float* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Moves int32_t data from Global Memory into Unified Buffer as Normal-mode bursts of burst_count blocks with
 * burst_len bytes each, padding every block to a 32-byte boundary.
 *
 * @override_id asc_copy_gm2ub_align__PUi32_PGi32_u16_u32_u8_u8_u32_u32
 * @archs 2201
 * @param [out] dst Start address of the Unified Buffer region that receives the copied int32_t data; must be 32-byte
 * aligned.
 * @param [in] src Start address of the Global Memory region holding the int32_t data to read; must be 1-byte aligned.
 * @param [in] burst_count Number of consecutive transfer blocks to move, in the range [0, 4095]; a value of 0 makes the
 * call a no-op.
 * @param [in] burst_len Length in bytes of each consecutive transfer block, in the range [0, 2097151]; a value of 0
 * makes the call a no-op.
 * @param [in] left_padding_num Number of int32_t elements padded to the left of each block; the resulting padded data
 * must not exceed 32 bytes.
 * @param [in] right_padding_num Number of int32_t elements padded to the right of each block; the resulting padded data
 * must not exceed 32 bytes.
 * @param [in] src_gap Gap in bytes between the tail of one source block and the head of the next source block.
 * @param [in] dst_gap Gap between consecutive destination blocks, measured in DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ int32_t* dst, __gm__ int32_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Moves int32_t elements from Global Memory into Unified Buffer in one run of size bytes, padding the run up to
 * a 32-byte boundary.
 *
 * @override_id asc_copy_gm2ub_align__PUi32_PGi32_u32
 * @archs 2201 3510
 * @param [out] dst Start address of the Unified Buffer region that receives the copied int32_t data; must be 32-byte
 * aligned.
 * @param [in] src Start address of the Global Memory region holding the int32_t data to read; must be 1-byte aligned.
 * @param [in] size Transfer length in bytes, in the range [0, 2097151]; a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Moves uint32_t data from Global Memory into Unified Buffer as Normal-mode bursts of burst_count blocks with
 * burst_len bytes each, padding every block to a 32-byte boundary.
 *
 * @override_id asc_copy_gm2ub_align__PUu32_PGu32_u16_u32_u8_u8_u32_u32
 * @archs 2201
 * @param [out] dst Start address of the Unified Buffer region that receives the copied uint32_t data; must be 32-byte
 * aligned.
 * @param [in] src Start address of the Global Memory region holding the uint32_t data to read; must be 1-byte aligned.
 * @param [in] burst_count Number of consecutive transfer blocks to move, in the range [0, 4095]; a value of 0 makes the
 * call a no-op.
 * @param [in] burst_len Length in bytes of each consecutive transfer block, in the range [0, 2097151]; a value of 0
 * makes the call a no-op.
 * @param [in] left_padding_num Number of uint32_t elements padded to the left of each block; the resulting padded data
 * must not exceed 32 bytes.
 * @param [in] right_padding_num Number of uint32_t elements padded to the right of each block; the resulting padded
 * data must not exceed 32 bytes.
 * @param [in] src_gap Gap in bytes between the tail of one source block and the head of the next source block.
 * @param [in] dst_gap Gap between consecutive destination blocks, measured in DataBlocks of 32 bytes.
 */
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Moves uint32_t elements from Global Memory into Unified Buffer in one run of size bytes, padding the run up to
 * a 32-byte boundary.
 *
 * @override_id asc_copy_gm2ub_align__PUu32_PGu32_u32
 * @archs 2201 3510
 * @param [out] dst Start address of the Unified Buffer region that receives the copied uint32_t data; must be 32-byte
 * aligned.
 * @param [in] src Start address of the Global Memory region holding the uint32_t data to read; must be 1-byte aligned.
 * @param [in] size Transfer length in bytes, in the range [0, 2097151]; a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t size);

// asc_copy_ub2gm_align
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies uint8_t data from Unified Buffer to Global Memory in burst_count bursts of burst_len bytes using Normal
 * mode, padding each burst to 32 bytes on the read and discarding the padding when writing.
 *
 * @override_id asc_copy_ub2gm_align__PGu8_PUu8_u16_u32_u8_u8_u32_u32
 * @archs 2201
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] burst_count Number of consecutive transfer blocks to move; documented range [0, 4095], and a value of 0
 * makes the call a no-op.
 * @param [in] burst_len Length of each transfer block in bytes; documented range [0, 2097151], and a value of 0 makes
 * the call a no-op.
 * @param [in] left_padding_num Reserved parameter that must be set to 0.
 * @param [in] right_padding_num Reserved parameter that must be set to 0.
 * @param [in] src_gap Gap between the end of one source block and the start of the next in Unified Buffer, in
 * DataBlocks of 32 bytes.
 * @param [in] dst_gap Gap between the end of one destination block and the start of the next in Global Memory, in
 * bytes.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies uint8_t data of the given byte size from Unified Buffer to Global Memory, supporting sizes that are not
 * a multiple of 32 bytes by padding the Unified Buffer read with dummy data and discarding that padding on the write.
 *
 * @override_id asc_copy_ub2gm_align__PGu8_PUu8_u32
 * @archs 2201 3510
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] size Number of bytes to copy; documented range [0, 2097151], and a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align(__gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies int8_t data from Unified Buffer to Global Memory in burst_count bursts of burst_len bytes using Normal
 * mode, padding each burst to 32 bytes on the read and discarding the padding when writing.
 *
 * @override_id asc_copy_ub2gm_align__PGi8_PUi8_u16_u32_u8_u8_u32_u32
 * @archs 2201
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] burst_count Number of consecutive transfer blocks to move; documented range [0, 4095], and a value of 0
 * makes the call a no-op.
 * @param [in] burst_len Length of each transfer block in bytes; documented range [0, 2097151], and a value of 0 makes
 * the call a no-op.
 * @param [in] left_padding_num Reserved parameter that must be set to 0.
 * @param [in] right_padding_num Reserved parameter that must be set to 0.
 * @param [in] src_gap Gap between the end of one source block and the start of the next in Unified Buffer, in
 * DataBlocks of 32 bytes.
 * @param [in] dst_gap Gap between the end of one destination block and the start of the next in Global Memory, in
 * bytes.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ int8_t* dst, __ubuf__ int8_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies int8_t data of the given byte size from Unified Buffer to Global Memory, supporting sizes that are not
 * a multiple of 32 bytes by padding the Unified Buffer read with dummy data and discarding that padding on the write.
 *
 * @override_id asc_copy_ub2gm_align__PGi8_PUi8_u32
 * @archs 2201 3510
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] size Number of bytes to copy; documented range [0, 2097151], and a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align(__gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies half data from Unified Buffer to Global Memory in burst_count bursts of burst_len bytes using Normal
 * mode, padding each burst to 32 bytes on the read and discarding the padding when writing.
 *
 * @override_id asc_copy_ub2gm_align__PGf16_PUf16_u16_u32_u8_u8_u32_u32
 * @archs 2201
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] burst_count Number of consecutive transfer blocks to move; documented range [0, 4095], and a value of 0
 * makes the call a no-op.
 * @param [in] burst_len Length of each transfer block in bytes; documented range [0, 2097151], and a value of 0 makes
 * the call a no-op.
 * @param [in] left_padding_num Reserved parameter that must be set to 0.
 * @param [in] right_padding_num Reserved parameter that must be set to 0.
 * @param [in] src_gap Gap between the end of one source block and the start of the next in Unified Buffer, in
 * DataBlocks of 32 bytes.
 * @param [in] dst_gap Gap between the end of one destination block and the start of the next in Global Memory, in
 * bytes.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ half* dst, __ubuf__ half* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies half data of the given byte size from Unified Buffer to Global Memory, supporting sizes that are not a
 * multiple of 32 bytes by padding the Unified Buffer read with dummy data and discarding that padding on the write.
 *
 * @override_id asc_copy_ub2gm_align__PGf16_PUf16_u32
 * @archs 2201 3510
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] size Number of bytes to copy; documented range [0, 2097151], and a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align(__gm__ half* dst, __ubuf__ half* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies uint16_t data from Unified Buffer to Global Memory in burst_count bursts of burst_len bytes using
 * Normal mode, padding each burst to 32 bytes on the read and discarding the padding when writing.
 *
 * @override_id asc_copy_ub2gm_align__PGu16_PUu16_u16_u32_u8_u8_u32_u32
 * @archs 2201
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] burst_count Number of consecutive transfer blocks to move; documented range [0, 4095], and a value of 0
 * makes the call a no-op.
 * @param [in] burst_len Length of each transfer block in bytes; documented range [0, 2097151], and a value of 0 makes
 * the call a no-op.
 * @param [in] left_padding_num Reserved parameter that must be set to 0.
 * @param [in] right_padding_num Reserved parameter that must be set to 0.
 * @param [in] src_gap Gap between the end of one source block and the start of the next in Unified Buffer, in
 * DataBlocks of 32 bytes.
 * @param [in] dst_gap Gap between the end of one destination block and the start of the next in Global Memory, in
 * bytes.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies uint16_t data of the given byte size from Unified Buffer to Global Memory, supporting sizes that are
 * not a multiple of 32 bytes by padding the Unified Buffer read with dummy data and discarding that padding on the
 * write.
 *
 * @override_id asc_copy_ub2gm_align__PGu16_PUu16_u32
 * @archs 2201 3510
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] size Number of bytes to copy; documented range [0, 2097151], and a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align(__gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies int16_t data from Unified Buffer to Global Memory in burst_count bursts of burst_len bytes using Normal
 * mode, padding each burst to 32 bytes on the read and discarding the padding when writing.
 *
 * @override_id asc_copy_ub2gm_align__PGi16_PUi16_u16_u32_u8_u8_u32_u32
 * @archs 2201
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] burst_count Number of consecutive transfer blocks to move; documented range [0, 4095], and a value of 0
 * makes the call a no-op.
 * @param [in] burst_len Length of each transfer block in bytes; documented range [0, 2097151], and a value of 0 makes
 * the call a no-op.
 * @param [in] left_padding_num Reserved parameter that must be set to 0.
 * @param [in] right_padding_num Reserved parameter that must be set to 0.
 * @param [in] src_gap Gap between the end of one source block and the start of the next in Unified Buffer, in
 * DataBlocks of 32 bytes.
 * @param [in] dst_gap Gap between the end of one destination block and the start of the next in Global Memory, in
 * bytes.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ int16_t* dst, __ubuf__ int16_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies int16_t data of the given byte size from Unified Buffer to Global Memory, supporting sizes that are not
 * a multiple of 32 bytes by padding the Unified Buffer read with dummy data and discarding that padding on the write.
 *
 * @override_id asc_copy_ub2gm_align__PGi16_PUi16_u32
 * @archs 2201 3510
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] size Number of bytes to copy; documented range [0, 2097151], and a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align(__gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies bfloat16_t data from Unified Buffer to Global Memory in burst_count bursts of burst_len bytes using
 * Normal mode, padding each burst to 32 bytes on the read and discarding the padding when writing.
 *
 * @override_id asc_copy_ub2gm_align__PGbf16_PUbf16_u16_u32_u8_u8_u32_u32
 * @archs 2201
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] burst_count Number of consecutive transfer blocks to move; documented range [0, 4095], and a value of 0
 * makes the call a no-op.
 * @param [in] burst_len Length of each transfer block in bytes; documented range [0, 2097151], and a value of 0 makes
 * the call a no-op.
 * @param [in] left_padding_num Reserved parameter that must be set to 0.
 * @param [in] right_padding_num Reserved parameter that must be set to 0.
 * @param [in] src_gap Gap between the end of one source block and the start of the next in Unified Buffer, in
 * DataBlocks of 32 bytes.
 * @param [in] dst_gap Gap between the end of one destination block and the start of the next in Global Memory, in
 * bytes.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint16_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies bfloat16_t data of the given byte size from Unified Buffer to Global Memory, supporting sizes that are
 * not a multiple of 32 bytes by padding the Unified Buffer read with dummy data and discarding that padding on the
 * write.
 *
 * @override_id asc_copy_ub2gm_align__PGbf16_PUbf16_u32
 * @archs 2201 3510
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] size Number of bytes to copy; documented range [0, 2097151], and a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align(__gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies uint32_t data from Unified Buffer to Global Memory in burst_count bursts of burst_len bytes using
 * Normal mode, padding each burst to 32 bytes on the read and discarding the padding when writing.
 *
 * @override_id asc_copy_ub2gm_align__PGu32_PUu32_u16_u32_u8_u8_u32_u32
 * @archs 2201
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] burst_count Number of consecutive transfer blocks to move; documented range [0, 4095], and a value of 0
 * makes the call a no-op.
 * @param [in] burst_len Length of each transfer block in bytes; documented range [0, 2097151], and a value of 0 makes
 * the call a no-op.
 * @param [in] left_padding_num Reserved parameter that must be set to 0.
 * @param [in] right_padding_num Reserved parameter that must be set to 0.
 * @param [in] src_gap Gap between the end of one source block and the start of the next in Unified Buffer, in
 * DataBlocks of 32 bytes.
 * @param [in] dst_gap Gap between the end of one destination block and the start of the next in Global Memory, in
 * bytes.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies uint32_t data of the given byte size from Unified Buffer to Global Memory, supporting sizes that are
 * not a multiple of 32 bytes by padding the Unified Buffer read with dummy data and discarding that padding on the
 * write.
 *
 * @override_id asc_copy_ub2gm_align__PGu32_PUu32_u32
 * @archs 2201 3510
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] size Number of bytes to copy; documented range [0, 2097151], and a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align(__gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies float data from Unified Buffer to Global Memory in burst_count bursts of burst_len bytes using Normal
 * mode, padding each burst to 32 bytes on the read and discarding the padding when writing.
 *
 * @override_id asc_copy_ub2gm_align__PGf32_PUf32_u16_u32_u8_u8_u32_u32
 * @archs 2201
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] burst_count Number of consecutive transfer blocks to move; documented range [0, 4095], and a value of 0
 * makes the call a no-op.
 * @param [in] burst_len Length of each transfer block in bytes; documented range [0, 2097151], and a value of 0 makes
 * the call a no-op.
 * @param [in] left_padding_num Reserved parameter that must be set to 0.
 * @param [in] right_padding_num Reserved parameter that must be set to 0.
 * @param [in] src_gap Gap between the end of one source block and the start of the next in Unified Buffer, in
 * DataBlocks of 32 bytes.
 * @param [in] dst_gap Gap between the end of one destination block and the start of the next in Global Memory, in
 * bytes.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ float* dst, __ubuf__ float* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies float data of the given byte size from Unified Buffer to Global Memory, supporting sizes that are not a
 * multiple of 32 bytes by padding the Unified Buffer read with dummy data and discarding that padding on the write.
 *
 * @override_id asc_copy_ub2gm_align__PGf32_PUf32_u32
 * @archs 2201 3510
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] size Number of bytes to copy; documented range [0, 2097151], and a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align(__gm__ float* dst, __ubuf__ float* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies int32_t data from Unified Buffer to Global Memory in burst_count bursts of burst_len bytes using Normal
 * mode, padding each burst to 32 bytes on the read and discarding the padding when writing.
 *
 * @override_id asc_copy_ub2gm_align__PGi32_PUi32_u16_u32_u8_u8_u32_u32
 * @archs 2201
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] burst_count Number of consecutive transfer blocks to move; documented range [0, 4095], and a value of 0
 * makes the call a no-op.
 * @param [in] burst_len Length of each transfer block in bytes; documented range [0, 2097151], and a value of 0 makes
 * the call a no-op.
 * @param [in] left_padding_num Reserved parameter that must be set to 0.
 * @param [in] right_padding_num Reserved parameter that must be set to 0.
 * @param [in] src_gap Gap between the end of one source block and the start of the next in Unified Buffer, in
 * DataBlocks of 32 bytes.
 * @param [in] dst_gap Gap between the end of one destination block and the start of the next in Global Memory, in
 * bytes.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ int32_t* dst, __ubuf__ int32_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies int32_t data of the given byte size from Unified Buffer to Global Memory, supporting sizes that are not
 * a multiple of 32 bytes by padding the Unified Buffer read with dummy data and discarding that padding on the write.
 *
 * @override_id asc_copy_ub2gm_align__PGi32_PUi32_u32
 * @archs 2201 3510
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] size Number of bytes to copy; documented range [0, 2097151], and a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align(__gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies double data from Unified Buffer to Global Memory in burst_count bursts of burst_len bytes using Normal
 * mode, padding each burst to 32 bytes on the read and discarding the padding when writing.
 *
 * @override_id asc_copy_ub2gm_align__PGf64_PUf64_u16_u32_u8_u8_u32_u32
 * @archs 2201
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] burst_count Number of consecutive transfer blocks to move; documented range [0, 4095], and a value of 0
 * makes the call a no-op.
 * @param [in] burst_len Length of each transfer block in bytes; documented range [0, 2097151], and a value of 0 makes
 * the call a no-op.
 * @param [in] left_padding_num Reserved parameter that must be set to 0.
 * @param [in] right_padding_num Reserved parameter that must be set to 0.
 * @param [in] src_gap Gap between the end of one source block and the start of the next in Unified Buffer, in
 * DataBlocks of 32 bytes.
 * @param [in] dst_gap Gap between the end of one destination block and the start of the next in Global Memory, in
 * bytes.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ double* dst, __ubuf__ double* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies double data of the given byte size from Unified Buffer to Global Memory, supporting sizes that are not
 * a multiple of 32 bytes by padding the Unified Buffer read with dummy data and discarding that padding on the write.
 *
 * @override_id asc_copy_ub2gm_align__PGf64_PUf64_u32
 * @archs 2201
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] size Number of bytes to copy; documented range [0, 2097151], and a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align(__gm__ double* dst, __ubuf__ double* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies int64_t data from Unified Buffer to Global Memory in burst_count bursts of burst_len bytes using Normal
 * mode, padding each burst to 32 bytes on the read and discarding the padding when writing.
 *
 * @override_id asc_copy_ub2gm_align__PGi64_PUi64_u16_u32_u8_u8_u32_u32
 * @archs 2201
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] burst_count Number of consecutive transfer blocks to move; documented range [0, 4095], and a value of 0
 * makes the call a no-op.
 * @param [in] burst_len Length of each transfer block in bytes; documented range [0, 2097151], and a value of 0 makes
 * the call a no-op.
 * @param [in] left_padding_num Reserved parameter that must be set to 0.
 * @param [in] right_padding_num Reserved parameter that must be set to 0.
 * @param [in] src_gap Gap between the end of one source block and the start of the next in Unified Buffer, in
 * DataBlocks of 32 bytes.
 * @param [in] dst_gap Gap between the end of one destination block and the start of the next in Global Memory, in
 * bytes.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ int64_t* dst, __ubuf__ int64_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies int64_t data of the given byte size from Unified Buffer to Global Memory, supporting sizes that are not
 * a multiple of 32 bytes by padding the Unified Buffer read with dummy data and discarding that padding on the write.
 *
 * @override_id asc_copy_ub2gm_align__PGi64_PUi64_u32
 * @archs 2201
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] size Number of bytes to copy; documented range [0, 2097151], and a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align(__gm__ int64_t* dst, __ubuf__ int64_t* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies uint64_t data from Unified Buffer to Global Memory in burst_count bursts of burst_len bytes using
 * Normal mode, padding each burst to 32 bytes on the read and discarding the padding when writing.
 *
 * @override_id asc_copy_ub2gm_align__PGu64_PUu64_u16_u32_u8_u8_u32_u32
 * @archs 2201
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] burst_count Number of consecutive transfer blocks to move; documented range [0, 4095], and a value of 0
 * makes the call a no-op.
 * @param [in] burst_len Length of each transfer block in bytes; documented range [0, 2097151], and a value of 0 makes
 * the call a no-op.
 * @param [in] left_padding_num Reserved parameter that must be set to 0.
 * @param [in] right_padding_num Reserved parameter that must be set to 0.
 * @param [in] src_gap Gap between the end of one source block and the start of the next in Unified Buffer, in
 * DataBlocks of 32 bytes.
 * @param [in] dst_gap Gap between the end of one destination block and the start of the next in Global Memory, in
 * bytes.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ uint64_t* dst, __ubuf__ uint64_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies uint64_t data of the given byte size from Unified Buffer to Global Memory, supporting sizes that are
 * not a multiple of 32 bytes by padding the Unified Buffer read with dummy data and discarding that padding on the
 * write.
 *
 * @override_id asc_copy_ub2gm_align__PGu64_PUu64_u32
 * @archs 2201
 * @param [out] dst Destination start address in Global Memory; it must be 1-byte aligned.
 * @param [in] src Source start address in Unified Buffer; it must be 32-byte aligned.
 * @param [in] size Number of bytes to copy; documented range [0, 2097151], and a value of 0 makes the call a no-op.
 */
__aicore__ inline void asc_copy_ub2gm_align(__gm__ uint64_t* dst, __ubuf__ uint64_t* src, uint32_t size);

// ==========asc_copy_gm2ub==========
__aicore__ inline void asc_copy_gm2ub(
    __ubuf__ void* dst, __gm__ void* src, uint32_t burst_count, uint32_t burst_len, uint64_t src_stride,
    uint32_t dst_stride);
__aicore__ inline void asc_copy_ub2gm(
    __gm__ void* dst, __ubuf__ void* src, uint32_t burst_count, uint32_t burst_len, uint64_t dst_stride,
    uint32_t src_stride);
// ==========asc_copy_gm2ub_align(int8_t/uint8_t/fp8_e5m2_t/fp8_e4m3fn_t/hifloat8_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float)==========
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Copies burst_count blocks of burst_len uint8_t bytes from GM into UB, choosing Compact or Normal layout
 * through dst_stride and padding short blocks with constant or dummy data.
 *
 * @override_id asc_copy_gm2ub_align__PUu8_PGu8_u32_u32_u8_u8_b_loadl2_u64_u32
 * @archs 3510
 * @param [out] dst Destination UB start address; it must be 32-byte aligned and large enough for the padded, strided
 * layout of the burst_count blocks.
 * @param [in] src Source GM start address; it only needs 1-byte alignment.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy with 8-bit
 * elements the doc imposes no extra multiple-of-dtype requirement on the byte count.
 * @param [in] left_padding_num Number of uint8_t elements prepended on the left of each block; the corresponding pad
 * bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] right_padding_num Number of uint8_t elements appended on the right of each block; the corresponding pad
 * bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] enable_constant_pad Selects the pad source only when both padding counts are 0: true uses the value
 * configured by asc_set_copy_pad_val, false lets the hardware write dummy data equal to the first element of the block.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * transferred data.
 * @param [in] src_stride Byte distance between the heads of adjacent source blocks, in the range [0, 2^40-1]; it may be
 * 0 when burst_count is 1.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination blocks, in the range [0, 2^21-1];
 * equal to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; it may be 0
 * when burst_count is 1.
 */
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Copies burst_count blocks of burst_len int8_t bytes from GM into UB, choosing Compact or Normal layout through
 * dst_stride and padding short blocks with constant or dummy data.
 *
 * @override_id asc_copy_gm2ub_align__PUi8_PGi8_u32_u32_u8_u8_b_loadl2_u64_u32
 * @archs 3510
 * @param [out] dst Destination UB start address; it must be 32-byte aligned and large enough for the padded, strided
 * layout of the burst_count blocks.
 * @param [in] src Source GM start address; it only needs 1-byte alignment.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy with 8-bit
 * elements the doc imposes no extra multiple-of-dtype requirement on the byte count.
 * @param [in] left_padding_num Number of int8_t elements prepended on the left of each block; the corresponding pad
 * bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] right_padding_num Number of int8_t elements appended on the right of each block; the corresponding pad
 * bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] enable_constant_pad Selects the pad source only when both padding counts are 0: true uses the value
 * configured by asc_set_copy_pad_val, false lets the hardware write dummy data equal to the first element of the block.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * transferred data.
 * @param [in] src_stride Byte distance between the heads of adjacent source blocks, in the range [0, 2^40-1]; it may be
 * 0 when burst_count is 1.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination blocks, in the range [0, 2^21-1];
 * equal to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; it may be 0
 * when burst_count is 1.
 */
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Copies burst_count blocks of burst_len uint16_t bytes from GM into UB, choosing Compact or Normal layout
 * through dst_stride and padding short blocks with constant or dummy data.
 *
 * @override_id asc_copy_gm2ub_align__PUu16_PGu16_u32_u32_u8_u8_b_loadl2_u64_u32
 * @archs 3510
 * @param [out] dst Destination UB start address; it must be 32-byte aligned and large enough for the padded, strided
 * layout of the burst_count blocks.
 * @param [in] src Source GM start address; it only needs 1-byte alignment.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy as a b16
 * dtype the byte count must be a multiple of 2.
 * @param [in] left_padding_num Number of uint16_t elements prepended on the left of each block; the corresponding pad
 * bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] right_padding_num Number of uint16_t elements appended on the right of each block; the corresponding pad
 * bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] enable_constant_pad Selects the pad source only when both padding counts are 0: true uses the value
 * configured by asc_set_copy_pad_val, false lets the hardware write dummy data equal to the first element of the block.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * transferred data.
 * @param [in] src_stride Byte distance between the heads of adjacent source blocks, in the range [0, 2^40-1]; it may be
 * 0 when burst_count is 1.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination blocks, in the range [0, 2^21-1];
 * equal to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; it may be 0
 * when burst_count is 1.
 */
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Copies burst_count blocks of burst_len int16_t bytes from GM into UB, choosing Compact or Normal layout
 * through dst_stride and padding short blocks with constant or dummy data.
 *
 * @override_id asc_copy_gm2ub_align__PUi16_PGi16_u32_u32_u8_u8_b_loadl2_u64_u32
 * @archs 3510
 * @param [out] dst Destination UB start address; it must be 32-byte aligned and large enough for the padded, strided
 * layout of the burst_count blocks.
 * @param [in] src Source GM start address; it only needs 1-byte alignment.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy as a b16
 * dtype the byte count must be a multiple of 2.
 * @param [in] left_padding_num Number of int16_t elements prepended on the left of each block; the corresponding pad
 * bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] right_padding_num Number of int16_t elements appended on the right of each block; the corresponding pad
 * bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] enable_constant_pad Selects the pad source only when both padding counts are 0: true uses the value
 * configured by asc_set_copy_pad_val, false lets the hardware write dummy data equal to the first element of the block.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * transferred data.
 * @param [in] src_stride Byte distance between the heads of adjacent source blocks, in the range [0, 2^40-1]; it may be
 * 0 when burst_count is 1.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination blocks, in the range [0, 2^21-1];
 * equal to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; it may be 0
 * when burst_count is 1.
 */
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Copies burst_count blocks of burst_len uint32_t bytes from GM into UB, choosing Compact or Normal layout
 * through dst_stride and padding short blocks with constant or dummy data.
 *
 * @override_id asc_copy_gm2ub_align__PUu32_PGu32_u32_u32_u8_u8_b_loadl2_u64_u32
 * @archs 3510
 * @param [out] dst Destination UB start address; it must be 32-byte aligned and large enough for the padded, strided
 * layout of the burst_count blocks.
 * @param [in] src Source GM start address; it only needs 1-byte alignment.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy as a b32
 * dtype the byte count must be a multiple of 4.
 * @param [in] left_padding_num Number of uint32_t elements prepended on the left of each block; the corresponding pad
 * bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] right_padding_num Number of uint32_t elements appended on the right of each block; the corresponding pad
 * bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] enable_constant_pad Selects the pad source only when both padding counts are 0: true uses the value
 * configured by asc_set_copy_pad_val, false lets the hardware write dummy data equal to the first element of the block.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * transferred data.
 * @param [in] src_stride Byte distance between the heads of adjacent source blocks, in the range [0, 2^40-1]; it may be
 * 0 when burst_count is 1.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination blocks, in the range [0, 2^21-1];
 * equal to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; it may be 0
 * when burst_count is 1.
 */
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Copies burst_count blocks of burst_len int32_t bytes from GM into UB, choosing Compact or Normal layout
 * through dst_stride and padding short blocks with constant or dummy data.
 *
 * @override_id asc_copy_gm2ub_align__PUi32_PGi32_u32_u32_u8_u8_b_loadl2_u64_u32
 * @archs 3510
 * @param [out] dst Destination UB start address; it must be 32-byte aligned and large enough for the padded, strided
 * layout of the burst_count blocks.
 * @param [in] src Source GM start address; it only needs 1-byte alignment.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy as a b32
 * dtype the byte count must be a multiple of 4.
 * @param [in] left_padding_num Number of int32_t elements prepended on the left of each block; the corresponding pad
 * bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] right_padding_num Number of int32_t elements appended on the right of each block; the corresponding pad
 * bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] enable_constant_pad Selects the pad source only when both padding counts are 0: true uses the value
 * configured by asc_set_copy_pad_val, false lets the hardware write dummy data equal to the first element of the block.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * transferred data.
 * @param [in] src_stride Byte distance between the heads of adjacent source blocks, in the range [0, 2^40-1]; it may be
 * 0 when burst_count is 1.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination blocks, in the range [0, 2^21-1];
 * equal to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; it may be 0
 * when burst_count is 1.
 */
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Copies burst_count blocks of burst_len half bytes from GM into UB, choosing Compact or Normal layout through
 * dst_stride and padding short blocks with constant or dummy data.
 *
 * @override_id asc_copy_gm2ub_align__PUf16_PGf16_u32_u32_u8_u8_b_loadl2_u64_u32
 * @archs 3510
 * @param [out] dst Destination UB start address; it must be 32-byte aligned and large enough for the padded, strided
 * layout of the burst_count blocks.
 * @param [in] src Source GM start address; it only needs 1-byte alignment.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy as a b16
 * dtype the byte count must be a multiple of 2.
 * @param [in] left_padding_num Number of half elements prepended on the left of each block; the corresponding pad bytes
 * must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] right_padding_num Number of half elements appended on the right of each block; the corresponding pad
 * bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] enable_constant_pad Selects the pad source only when both padding counts are 0: true uses the value
 * configured by asc_set_copy_pad_val, false lets the hardware write dummy data equal to the first element of the block.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * transferred data.
 * @param [in] src_stride Byte distance between the heads of adjacent source blocks, in the range [0, 2^40-1]; it may be
 * 0 when burst_count is 1.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination blocks, in the range [0, 2^21-1];
 * equal to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; it may be 0
 * when burst_count is 1.
 */
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ half* dst, __gm__ half* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Copies burst_count blocks of burst_len float bytes from GM into UB, choosing Compact or Normal layout through
 * dst_stride and padding short blocks with constant or dummy data.
 *
 * @override_id asc_copy_gm2ub_align__PUf32_PGf32_u32_u32_u8_u8_b_loadl2_u64_u32
 * @archs 3510
 * @param [out] dst Destination UB start address; it must be 32-byte aligned and large enough for the padded, strided
 * layout of the burst_count blocks.
 * @param [in] src Source GM start address; it only needs 1-byte alignment.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy as a b32
 * dtype the byte count must be a multiple of 4.
 * @param [in] left_padding_num Number of float elements prepended on the left of each block; the corresponding pad
 * bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] right_padding_num Number of float elements appended on the right of each block; the corresponding pad
 * bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] enable_constant_pad Selects the pad source only when both padding counts are 0: true uses the value
 * configured by asc_set_copy_pad_val, false lets the hardware write dummy data equal to the first element of the block.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * transferred data.
 * @param [in] src_stride Byte distance between the heads of adjacent source blocks, in the range [0, 2^40-1]; it may be
 * 0 when burst_count is 1.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination blocks, in the range [0, 2^21-1];
 * equal to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; it may be 0
 * when burst_count is 1.
 */
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ float* dst, __gm__ float* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Copies burst_count blocks of burst_len hifloat8_t bytes from GM into UB, choosing Compact or Normal layout
 * through dst_stride and padding short blocks with constant or dummy data.
 *
 * @override_id asc_copy_gm2ub_align__PUhf8_PGhf8_u32_u32_u8_u8_b_loadl2_u64_u32
 * @archs 3510
 * @param [out] dst Destination UB start address; it must be 32-byte aligned and large enough for the padded, strided
 * layout of the burst_count blocks.
 * @param [in] src Source GM start address; it only needs 1-byte alignment.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy with 8-bit
 * elements the doc imposes no extra multiple-of-dtype requirement on the byte count.
 * @param [in] left_padding_num Number of hifloat8_t elements prepended on the left of each block; the corresponding pad
 * bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] right_padding_num Number of hifloat8_t elements appended on the right of each block; the corresponding
 * pad bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] enable_constant_pad Selects the pad source only when both padding counts are 0: true uses the value
 * configured by asc_set_copy_pad_val, false lets the hardware write dummy data equal to the first element of the block.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * transferred data.
 * @param [in] src_stride Byte distance between the heads of adjacent source blocks, in the range [0, 2^40-1]; it may be
 * 0 when burst_count is 1.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination blocks, in the range [0, 2^21-1];
 * equal to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; it may be 0
 * when burst_count is 1.
 */
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode,
    uint64_t src_stride, uint32_t dst_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Copies burst_count blocks of burst_len bfloat16_t bytes from GM into UB, choosing Compact or Normal layout
 * through dst_stride and padding short blocks with constant or dummy data.
 *
 * @override_id asc_copy_gm2ub_align__PUbf16_PGbf16_u32_u32_u8_u8_b_loadl2_u64_u32
 * @archs 3510
 * @param [out] dst Destination UB start address; it must be 32-byte aligned and large enough for the padded, strided
 * layout of the burst_count blocks.
 * @param [in] src Source GM start address; it only needs 1-byte alignment.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy as a b16
 * dtype the byte count must be a multiple of 2.
 * @param [in] left_padding_num Number of bfloat16_t elements prepended on the left of each block; the corresponding pad
 * bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] right_padding_num Number of bfloat16_t elements appended on the right of each block; the corresponding
 * pad bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] enable_constant_pad Selects the pad source only when both padding counts are 0: true uses the value
 * configured by asc_set_copy_pad_val, false lets the hardware write dummy data equal to the first element of the block.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * transferred data.
 * @param [in] src_stride Byte distance between the heads of adjacent source blocks, in the range [0, 2^40-1]; it may be
 * 0 when burst_count is 1.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination blocks, in the range [0, 2^21-1];
 * equal to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; it may be 0
 * when burst_count is 1.
 */
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode,
    uint64_t src_stride, uint32_t dst_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Copies burst_count blocks of burst_len fp8_e5m2_t bytes from GM into UB, choosing Compact or Normal layout
 * through dst_stride and padding short blocks with constant or dummy data.
 *
 * @override_id asc_copy_gm2ub_align__PUf8e5m2_PGf8e5m2_u32_u32_u8_u8_b_loadl2_u64_u32
 * @archs 3510
 * @param [out] dst Destination UB start address; it must be 32-byte aligned and large enough for the padded, strided
 * layout of the burst_count blocks.
 * @param [in] src Source GM start address; it only needs 1-byte alignment.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy with 8-bit
 * elements the doc imposes no extra multiple-of-dtype requirement on the byte count.
 * @param [in] left_padding_num Number of fp8_e5m2_t elements prepended on the left of each block; the corresponding pad
 * bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] right_padding_num Number of fp8_e5m2_t elements appended on the right of each block; the corresponding
 * pad bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] enable_constant_pad Selects the pad source only when both padding counts are 0: true uses the value
 * configured by asc_set_copy_pad_val, false lets the hardware write dummy data equal to the first element of the block.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * transferred data.
 * @param [in] src_stride Byte distance between the heads of adjacent source blocks, in the range [0, 2^40-1]; it may be
 * 0 when burst_count is 1.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination blocks, in the range [0, 2^21-1];
 * equal to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; it may be 0
 * when burst_count is 1.
 */
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode,
    uint64_t src_stride, uint32_t dst_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Copies burst_count blocks of burst_len fp8_e4m3fn_t bytes from GM into UB, choosing Compact or Normal layout
 * through dst_stride and padding short blocks with constant or dummy data.
 *
 * @override_id asc_copy_gm2ub_align__PUf8e4m3_PGf8e4m3_u32_u32_u8_u8_b_loadl2_u64_u32
 * @archs 3510
 * @param [out] dst Destination UB start address; it must be 32-byte aligned and large enough for the padded, strided
 * layout of the burst_count blocks.
 * @param [in] src Source GM start address; it only needs 1-byte alignment.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy with 8-bit
 * elements the doc imposes no extra multiple-of-dtype requirement on the byte count.
 * @param [in] left_padding_num Number of fp8_e4m3fn_t elements prepended on the left of each block; the corresponding
 * pad bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] right_padding_num Number of fp8_e4m3fn_t elements appended on the right of each block; the corresponding
 * pad bytes must not exceed 32 and the value must be 0 in Compact mode.
 * @param [in] enable_constant_pad Selects the pad source only when both padding counts are 0: true uses the value
 * configured by asc_set_copy_pad_val, false lets the hardware write dummy data equal to the first element of the block.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * transferred data.
 * @param [in] src_stride Byte distance between the heads of adjacent source blocks, in the range [0, 2^40-1]; it may be
 * 0 when burst_count is 1.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination blocks, in the range [0, 2^21-1];
 * equal to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; it may be 0
 * when burst_count is 1.
 */
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode,
    uint64_t src_stride, uint32_t dst_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Copies size bytes of hifloat8_t data from GM into a 32-byte aligned UB address, padding any non-32-byte tail
 * with hardware dummy data.
 *
 * @override_id asc_copy_gm2ub_align__PUhf8_PGhf8_u32
 * @archs 3510
 * @param [out] dst Destination UB start address for the copied hifloat8_t block; it must be 32-byte aligned and must
 * also reserve the 32-byte padded space when size is not a multiple of 32.
 * @param [in] src Source GM start address; it only needs 1-byte alignment.
 * @param [in] size Number of bytes to copy, in the range [1, 2^21-1]; it must also satisfy with 8-bit elements the doc
 * imposes no extra multiple-of-dtype requirement on the byte count so that whole hifloat8_t elements are transferred.
 */
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Copies size bytes of fp8_e5m2_t data from GM into a 32-byte aligned UB address, padding any non-32-byte tail
 * with hardware dummy data.
 *
 * @override_id asc_copy_gm2ub_align__PUf8e5m2_PGf8e5m2_u32
 * @archs 3510
 * @param [out] dst Destination UB start address for the copied fp8_e5m2_t block; it must be 32-byte aligned and must
 * also reserve the 32-byte padded space when size is not a multiple of 32.
 * @param [in] src Source GM start address; it only needs 1-byte alignment.
 * @param [in] size Number of bytes to copy, in the range [1, 2^21-1]; it must also satisfy with 8-bit elements the doc
 * imposes no extra multiple-of-dtype requirement on the byte count so that whole fp8_e5m2_t elements are transferred.
 */
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_gm2ub_align
 * @brief Copies size bytes of fp8_e4m3fn_t data from GM into a 32-byte aligned UB address, padding any non-32-byte tail
 * with hardware dummy data.
 *
 * @override_id asc_copy_gm2ub_align__PUf8e4m3_PGf8e4m3_u32
 * @archs 3510
 * @param [out] dst Destination UB start address for the copied fp8_e4m3fn_t block; it must be 32-byte aligned and must
 * also reserve the 32-byte padded space when size is not a multiple of 32.
 * @param [in] src Source GM start address; it only needs 1-byte alignment.
 * @param [in] size Number of bytes to copy, in the range [1, 2^21-1]; it must also satisfy with 8-bit elements the doc
 * imposes no extra multiple-of-dtype requirement on the byte count so that whole fp8_e4m3fn_t elements are transferred.
 */
__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t size);

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ fp8_e8m0_t* dst, __gm__ fp8_e8m0_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode,
    uint64_t src_stride, uint32_t dst_stride);

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ fp8_e8m0_t* dst, __gm__ fp8_e8m0_t* src, uint32_t size);

// ==========asc_copy_ub2gm_align(int8_t/uint8_t/fp8_e5m2_t/fp8_e4m3fn_t/hifloat8_t/int16_t/uint16_t/half/bfloat16_t/int32_t/uint32_t/float)==========
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies burst_count blocks of burst_len uint8_t bytes from UB to GM, selecting Compact or Normal mode through
 * src_stride while dropping all dummy padding bytes.
 *
 * @override_id asc_copy_ub2gm_align__PGu8_PUu8_u32_u32_storel2_u64_u32
 * @archs 3510
 * @param [out] dst Destination GM start address; it only needs 1-byte alignment and receives only the valid burst_len
 * bytes of every block.
 * @param [in] src Source UB start address; it must be 32-byte aligned and must cover the padded or strided block layout
 * described by src_stride.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy for 8-bit
 * elements the doc adds no extra multiple-of-dtype requirement on the byte count.
 * @param [in] l2_cache_mode asc_store_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * written data.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination GM blocks, in the range [0, 2^40-1];
 * it may be 0 when burst_count is 1.
 * @param [in] src_stride Byte distance between the heads of adjacent source UB blocks, in the range [0, 2^21-1]; equal
 * to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; 0 with more than
 * one block can trigger undefined hardware behaviour.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies burst_count blocks of burst_len int8_t bytes from UB to GM, selecting Compact or Normal mode through
 * src_stride while dropping all dummy padding bytes.
 *
 * @override_id asc_copy_ub2gm_align__PGi8_PUi8_u32_u32_storel2_u64_u32
 * @archs 3510
 * @param [out] dst Destination GM start address; it only needs 1-byte alignment and receives only the valid burst_len
 * bytes of every block.
 * @param [in] src Source UB start address; it must be 32-byte aligned and must cover the padded or strided block layout
 * described by src_stride.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy for 8-bit
 * elements the doc adds no extra multiple-of-dtype requirement on the byte count.
 * @param [in] l2_cache_mode asc_store_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * written data.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination GM blocks, in the range [0, 2^40-1];
 * it may be 0 when burst_count is 1.
 * @param [in] src_stride Byte distance between the heads of adjacent source UB blocks, in the range [0, 2^21-1]; equal
 * to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; 0 with more than
 * one block can trigger undefined hardware behaviour.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies burst_count blocks of burst_len uint16_t bytes from UB to GM, selecting Compact or Normal mode through
 * src_stride while dropping all dummy padding bytes.
 *
 * @override_id asc_copy_ub2gm_align__PGu16_PUu16_u32_u32_storel2_u64_u32
 * @archs 3510
 * @param [out] dst Destination GM start address; it only needs 1-byte alignment and receives only the valid burst_len
 * bytes of every block.
 * @param [in] src Source UB start address; it must be 32-byte aligned and must cover the padded or strided block layout
 * described by src_stride.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy as a b16
 * dtype the byte count must be a multiple of 2.
 * @param [in] l2_cache_mode asc_store_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * written data.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination GM blocks, in the range [0, 2^40-1];
 * it may be 0 when burst_count is 1.
 * @param [in] src_stride Byte distance between the heads of adjacent source UB blocks, in the range [0, 2^21-1]; equal
 * to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; 0 with more than
 * one block can trigger undefined hardware behaviour.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies burst_count blocks of burst_len int16_t bytes from UB to GM, selecting Compact or Normal mode through
 * src_stride while dropping all dummy padding bytes.
 *
 * @override_id asc_copy_ub2gm_align__PGi16_PUi16_u32_u32_storel2_u64_u32
 * @archs 3510
 * @param [out] dst Destination GM start address; it only needs 1-byte alignment and receives only the valid burst_len
 * bytes of every block.
 * @param [in] src Source UB start address; it must be 32-byte aligned and must cover the padded or strided block layout
 * described by src_stride.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy as a b16
 * dtype the byte count must be a multiple of 2.
 * @param [in] l2_cache_mode asc_store_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * written data.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination GM blocks, in the range [0, 2^40-1];
 * it may be 0 when burst_count is 1.
 * @param [in] src_stride Byte distance between the heads of adjacent source UB blocks, in the range [0, 2^21-1]; equal
 * to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; 0 with more than
 * one block can trigger undefined hardware behaviour.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies burst_count blocks of burst_len uint32_t bytes from UB to GM, selecting Compact or Normal mode through
 * src_stride while dropping all dummy padding bytes.
 *
 * @override_id asc_copy_ub2gm_align__PGu32_PUu32_u32_u32_storel2_u64_u32
 * @archs 3510
 * @param [out] dst Destination GM start address; it only needs 1-byte alignment and receives only the valid burst_len
 * bytes of every block.
 * @param [in] src Source UB start address; it must be 32-byte aligned and must cover the padded or strided block layout
 * described by src_stride.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy as a b32
 * dtype the byte count must be a multiple of 4.
 * @param [in] l2_cache_mode asc_store_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * written data.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination GM blocks, in the range [0, 2^40-1];
 * it may be 0 when burst_count is 1.
 * @param [in] src_stride Byte distance between the heads of adjacent source UB blocks, in the range [0, 2^21-1]; equal
 * to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; 0 with more than
 * one block can trigger undefined hardware behaviour.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies burst_count blocks of burst_len int32_t bytes from UB to GM, selecting Compact or Normal mode through
 * src_stride while dropping all dummy padding bytes.
 *
 * @override_id asc_copy_ub2gm_align__PGi32_PUi32_u32_u32_storel2_u64_u32
 * @archs 3510
 * @param [out] dst Destination GM start address; it only needs 1-byte alignment and receives only the valid burst_len
 * bytes of every block.
 * @param [in] src Source UB start address; it must be 32-byte aligned and must cover the padded or strided block layout
 * described by src_stride.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy as a b32
 * dtype the byte count must be a multiple of 4.
 * @param [in] l2_cache_mode asc_store_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * written data.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination GM blocks, in the range [0, 2^40-1];
 * it may be 0 when burst_count is 1.
 * @param [in] src_stride Byte distance between the heads of adjacent source UB blocks, in the range [0, 2^21-1]; equal
 * to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; 0 with more than
 * one block can trigger undefined hardware behaviour.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies burst_count blocks of burst_len half bytes from UB to GM, selecting Compact or Normal mode through
 * src_stride while dropping all dummy padding bytes.
 *
 * @override_id asc_copy_ub2gm_align__PGf16_PUf16_u32_u32_storel2_u64_u32
 * @archs 3510
 * @param [out] dst Destination GM start address; it only needs 1-byte alignment and receives only the valid burst_len
 * bytes of every block.
 * @param [in] src Source UB start address; it must be 32-byte aligned and must cover the padded or strided block layout
 * described by src_stride.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy as a b16
 * dtype the byte count must be a multiple of 2.
 * @param [in] l2_cache_mode asc_store_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * written data.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination GM blocks, in the range [0, 2^40-1];
 * it may be 0 when burst_count is 1.
 * @param [in] src_stride Byte distance between the heads of adjacent source UB blocks, in the range [0, 2^21-1]; equal
 * to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; 0 with more than
 * one block can trigger undefined hardware behaviour.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ half* dst, __ubuf__ half* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies burst_count blocks of burst_len float bytes from UB to GM, selecting Compact or Normal mode through
 * src_stride while dropping all dummy padding bytes.
 *
 * @override_id asc_copy_ub2gm_align__PGf32_PUf32_u32_u32_storel2_u64_u32
 * @archs 3510
 * @param [out] dst Destination GM start address; it only needs 1-byte alignment and receives only the valid burst_len
 * bytes of every block.
 * @param [in] src Source UB start address; it must be 32-byte aligned and must cover the padded or strided block layout
 * described by src_stride.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy as a b32
 * dtype the byte count must be a multiple of 4.
 * @param [in] l2_cache_mode asc_store_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * written data.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination GM blocks, in the range [0, 2^40-1];
 * it may be 0 when burst_count is 1.
 * @param [in] src_stride Byte distance between the heads of adjacent source UB blocks, in the range [0, 2^21-1]; equal
 * to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; 0 with more than
 * one block can trigger undefined hardware behaviour.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ float* dst, __ubuf__ float* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies burst_count blocks of burst_len hifloat8_t bytes from UB to GM, selecting Compact or Normal mode
 * through src_stride while dropping all dummy padding bytes.
 *
 * @override_id asc_copy_ub2gm_align__PGhf8_PUhf8_u32_u32_storel2_u64_u32
 * @archs 3510
 * @param [out] dst Destination GM start address; it only needs 1-byte alignment and receives only the valid burst_len
 * bytes of every block.
 * @param [in] src Source UB start address; it must be 32-byte aligned and must cover the padded or strided block layout
 * described by src_stride.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy for 8-bit
 * elements the doc adds no extra multiple-of-dtype requirement on the byte count.
 * @param [in] l2_cache_mode asc_store_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * written data.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination GM blocks, in the range [0, 2^40-1];
 * it may be 0 when burst_count is 1.
 * @param [in] src_stride Byte distance between the heads of adjacent source UB blocks, in the range [0, 2^21-1]; equal
 * to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; 0 with more than
 * one block can trigger undefined hardware behaviour.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies burst_count blocks of burst_len bfloat16_t bytes from UB to GM, selecting Compact or Normal mode
 * through src_stride while dropping all dummy padding bytes.
 *
 * @override_id asc_copy_ub2gm_align__PGbf16_PUbf16_u32_u32_storel2_u64_u32
 * @archs 3510
 * @param [out] dst Destination GM start address; it only needs 1-byte alignment and receives only the valid burst_len
 * bytes of every block.
 * @param [in] src Source UB start address; it must be 32-byte aligned and must cover the padded or strided block layout
 * described by src_stride.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy as a b16
 * dtype the byte count must be a multiple of 2.
 * @param [in] l2_cache_mode asc_store_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * written data.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination GM blocks, in the range [0, 2^40-1];
 * it may be 0 when burst_count is 1.
 * @param [in] src_stride Byte distance between the heads of adjacent source UB blocks, in the range [0, 2^21-1]; equal
 * to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; 0 with more than
 * one block can trigger undefined hardware behaviour.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies burst_count blocks of burst_len fp8_e5m2_t bytes from UB to GM, selecting Compact or Normal mode
 * through src_stride while dropping all dummy padding bytes.
 *
 * @override_id asc_copy_ub2gm_align__PGf8e5m2_PUf8e5m2_u32_u32_storel2_u64_u32
 * @archs 3510
 * @param [out] dst Destination GM start address; it only needs 1-byte alignment and receives only the valid burst_len
 * bytes of every block.
 * @param [in] src Source UB start address; it must be 32-byte aligned and must cover the padded or strided block layout
 * described by src_stride.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy for 8-bit
 * elements the doc adds no extra multiple-of-dtype requirement on the byte count.
 * @param [in] l2_cache_mode asc_store_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * written data.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination GM blocks, in the range [0, 2^40-1];
 * it may be 0 when burst_count is 1.
 * @param [in] src_stride Byte distance between the heads of adjacent source UB blocks, in the range [0, 2^21-1]; equal
 * to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; 0 with more than
 * one block can trigger undefined hardware behaviour.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies burst_count blocks of burst_len fp8_e4m3fn_t bytes from UB to GM, selecting Compact or Normal mode
 * through src_stride while dropping all dummy padding bytes.
 *
 * @override_id asc_copy_ub2gm_align__PGf8e4m3_PUf8e4m3_u32_u32_storel2_u64_u32
 * @archs 3510
 * @param [out] dst Destination GM start address; it only needs 1-byte alignment and receives only the valid burst_len
 * bytes of every block.
 * @param [in] src Source UB start address; it must be 32-byte aligned and must cover the padded or strided block layout
 * described by src_stride.
 * @param [in] burst_count Number of consecutive blocks to transfer, in the range [1, 2^21-1].
 * @param [in] burst_len Length in bytes of each transferred block, in the range [1, 2^21-1]; it must satisfy for 8-bit
 * elements the doc adds no extra multiple-of-dtype requirement on the byte count.
 * @param [in] l2_cache_mode asc_store_l2_cache_mode enumerator that selects the L2 Cache management policy for the
 * written data.
 * @param [in] dst_stride Byte distance between the heads of adjacent destination GM blocks, in the range [0, 2^40-1];
 * it may be 0 when burst_count is 1.
 * @param [in] src_stride Byte distance between the heads of adjacent source UB blocks, in the range [0, 2^21-1]; equal
 * to burst_len selects Compact mode, any other value selects Normal mode and must be 32-byte aligned; 0 with more than
 * one block can trigger undefined hardware behaviour.
 */
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride);
/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies size bytes of hifloat8_t data from a 32-byte aligned UB address to GM, discarding the hardware dummy
 * bytes used for the 32-byte read padding.
 *
 * @override_id asc_copy_ub2gm_align__PGhf8_PUhf8_u32
 * @archs 3510
 * @param [out] dst Destination GM start address for the written hifloat8_t bytes; it only needs 1-byte alignment and
 * only size valid bytes are written.
 * @param [in] src Source UB start address; it must be 32-byte aligned and must hold the padded read range when size is
 * not a multiple of 32.
 * @param [in] size Number of valid bytes to write to GM, in the range [1, 2^21-1]; it must also satisfy for 8-bit
 * elements the doc adds no extra multiple-of-dtype requirement on the byte count.
 */
__aicore__ inline void asc_copy_ub2gm_align(__gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies size bytes of fp8_e5m2_t data from a 32-byte aligned UB address to GM, discarding the hardware dummy
 * bytes used for the 32-byte read padding.
 *
 * @override_id asc_copy_ub2gm_align__PGf8e5m2_PUf8e5m2_u32
 * @archs 3510
 * @param [out] dst Destination GM start address for the written fp8_e5m2_t bytes; it only needs 1-byte alignment and
 * only size valid bytes are written.
 * @param [in] src Source UB start address; it must be 32-byte aligned and must hold the padded read range when size is
 * not a multiple of 32.
 * @param [in] size Number of valid bytes to write to GM, in the range [1, 2^21-1]; it must also satisfy for 8-bit
 * elements the doc adds no extra multiple-of-dtype requirement on the byte count.
 */
__aicore__ inline void asc_copy_ub2gm_align(__gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove.asc_copy_ub2gm_align
 * @brief Copies size bytes of fp8_e4m3fn_t data from a 32-byte aligned UB address to GM, discarding the hardware dummy
 * bytes used for the 32-byte read padding.
 *
 * @override_id asc_copy_ub2gm_align__PGf8e4m3_PUf8e4m3_u32
 * @archs 3510
 * @param [out] dst Destination GM start address for the written fp8_e4m3fn_t bytes; it only needs 1-byte alignment and
 * only size valid bytes are written.
 * @param [in] src Source UB start address; it must be 32-byte aligned and must hold the padded read range when size is
 * not a multiple of 32.
 * @param [in] size Number of valid bytes to write to GM, in the range [1, 2^21-1]; it must also satisfy for 8-bit
 * elements the doc adds no extra multiple-of-dtype requirement on the byte count.
 */
__aicore__ inline void asc_copy_ub2gm_align(__gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint32_t size);

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ fp8_e8m0_t* dst, __ubuf__ fp8_e8m0_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride);

__aicore__ inline void asc_copy_ub2gm_align(__gm__ fp8_e8m0_t* dst, __ubuf__ fp8_e8m0_t* src, uint32_t size);

/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the inner and outer loop iteration counts for asc_copy_ub2gm_align loop-copy mode.
 *
 * @override_id asc_set_ub2gm_loop_size__u32_u32
 * @archs 3510
 * @param [in] loop1_size Number of iterations of the inner loop loop1; range [0, 2^21 - 1], where 0 suppresses the
 * inner loop so the following asc_copy_ub2gm_align transfers no data.
 * @param [in] loop2_size Number of iterations of the outer loop loop2; range [0, 2^21 - 1], where 0 suppresses the
 * outer loop so the following asc_copy_ub2gm_align transfers no data.
 */
__aicore__ inline void asc_set_ub2gm_loop_size(uint32_t loop1_size, uint32_t loop2_size);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the inner-loop source and destination strides for asc_copy_ub2gm_align loop-copy mode.
 *
 * @override_id asc_set_ub2gm_loop1_stride__u64_u64
 * @archs 3510
 * @param [in] loop1_src_stride Offset between the start addresses of source data blocks of adjacent inner-loop
 * iterations, in bytes; range [0, 2^21 - 1] and must be 32-byte aligned.
 * @param [in] loop1_dst_stride Offset between the start addresses of destination data blocks of adjacent inner-loop
 * iterations, in bytes; range [0, 2^40 - 1].
 */
__aicore__ inline void asc_set_ub2gm_loop1_stride(uint64_t loop1_src_stride, uint64_t loop1_dst_stride);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the outer-loop source and destination strides for asc_copy_ub2gm_align loop-copy mode.
 *
 * @override_id asc_set_ub2gm_loop2_stride__u64_u64
 * @archs 3510
 * @param [in] loop2_src_stride Offset between the start addresses of source data blocks of adjacent outer-loop
 * iterations, in bytes; range [0, 2^21 - 1] and must be 32-byte aligned.
 * @param [in] loop2_dst_stride Offset between the start addresses of destination data blocks of adjacent outer-loop
 * iterations, in bytes; range [0, 2^40 - 1].
 */
__aicore__ inline void asc_set_ub2gm_loop2_stride(uint64_t loop2_src_stride, uint64_t loop2_dst_stride);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the inner and outer loop iteration counts for asc_copy_gm2ub_align loop-copy mode.
 *
 * @override_id asc_set_gm2ub_loop_size__u64_u64
 * @archs 3510
 * @param [in] loop1_size Number of iterations of the inner loop loop1; range [0, 2^21 - 1], and 0 means the inner loop
 * does not execute so the following asc_copy_gm2ub_align transfers no data.
 * @param [in] loop2_size Number of iterations of the outer loop loop2; range [0, 2^22 - 1], and 0 means the outer loop
 * does not execute so the following asc_copy_gm2ub_align transfers no data.
 */
__aicore__ inline void asc_set_gm2ub_loop_size(uint64_t loop1_size, uint64_t loop2_size);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the inner-loop source and destination strides for asc_copy_gm2ub_align loop-copy mode.
 *
 * @override_id asc_set_gm2ub_loop1_stride__u64_u64
 * @archs 3510
 * @param [in] loop1_src_stride Data block interval between adjacent iterations of the inner loop in the source operand,
 * in bytes; range [0, 2^40 - 1].
 * @param [in] loop1_dst_stride Data block interval between adjacent iterations of the inner loop in the destination
 * operand, in bytes; range [0, 2^21 - 1] and must be 32-byte aligned.
 */
__aicore__ inline void asc_set_gm2ub_loop1_stride(uint64_t loop1_src_stride, uint64_t loop1_dst_stride);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the outer-loop source and destination strides for asc_copy_gm2ub_align loop-copy mode.
 *
 * @override_id asc_set_gm2ub_loop2_stride__u64_u64
 * @archs 3510
 * @param [in] loop2_src_stride Data block interval between adjacent iterations of the outer loop in the source operand,
 * in bytes; range [0, 2^40 - 1].
 * @param [in] loop2_dst_stride Data block interval between adjacent iterations of the outer loop in the destination
 * operand, in bytes; range [0, 2^21 - 1] and must be 32-byte aligned.
 */
__aicore__ inline void asc_set_gm2ub_loop2_stride(uint64_t loop2_src_stride, uint64_t loop2_dst_stride);
// ==========asc_set_ndim_loop_stride==========
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the destination and source element strides of the loop0 ND-DMA loop dimension used by
 * asc_ndim_copy_gm2ub, where loop0 is the innermost dimension.
 *
 * @override_id asc_set_ndim_loop0_stride__u64_u64
 * @archs 3510
 * @param [in] dst_stride Offset in elements between the destination element start addresses of two consecutive
 * iterations of this loop dimension; the page gives the range [0, 2^20 - 1].
 * @param [in] src_stride Offset in elements between the source element start addresses of two consecutive iterations of
 * this loop dimension; the page gives the range [0, 2^40 - 1].
 */
__aicore__ inline void asc_set_ndim_loop0_stride(uint64_t dst_stride, uint64_t src_stride);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the destination and source element strides of the loop1 ND-DMA loop dimension used by
 * asc_ndim_copy_gm2ub, where loop0 is the innermost dimension.
 *
 * @override_id asc_set_ndim_loop1_stride__u64_u64
 * @archs 3510
 * @param [in] dst_stride Offset in elements between the destination element start addresses of two consecutive
 * iterations of this loop dimension; the page gives the range [0, 2^20 - 1].
 * @param [in] src_stride Offset in elements between the source element start addresses of two consecutive iterations of
 * this loop dimension; the page gives the range [0, 2^40 - 1].
 */
__aicore__ inline void asc_set_ndim_loop1_stride(uint64_t dst_stride, uint64_t src_stride);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the destination and source element strides of the loop2 ND-DMA loop dimension used by
 * asc_ndim_copy_gm2ub, where loop0 is the innermost dimension.
 *
 * @override_id asc_set_ndim_loop2_stride__u64_u64
 * @archs 3510
 * @param [in] dst_stride Offset in elements between the destination element start addresses of two consecutive
 * iterations of this loop dimension; the page gives the range [0, 2^20 - 1].
 * @param [in] src_stride Offset in elements between the source element start addresses of two consecutive iterations of
 * this loop dimension; the page gives the range [0, 2^40 - 1].
 */
__aicore__ inline void asc_set_ndim_loop2_stride(uint64_t dst_stride, uint64_t src_stride);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the destination and source element strides of the loop3 ND-DMA loop dimension used by
 * asc_ndim_copy_gm2ub, where loop0 is the innermost dimension.
 *
 * @override_id asc_set_ndim_loop3_stride__u64_u64
 * @archs 3510
 * @param [in] dst_stride Offset in elements between the destination element start addresses of two consecutive
 * iterations of this loop dimension; the page gives the range [0, 2^20 - 1].
 * @param [in] src_stride Offset in elements between the source element start addresses of two consecutive iterations of
 * this loop dimension; the page gives the range [0, 2^40 - 1].
 */
__aicore__ inline void asc_set_ndim_loop3_stride(uint64_t dst_stride, uint64_t src_stride);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the destination and source element strides of the loop4 ND-DMA loop dimension used by
 * asc_ndim_copy_gm2ub, where loop0 is the innermost dimension.
 *
 * @override_id asc_set_ndim_loop4_stride__u64_u64
 * @archs 3510
 * @param [in] dst_stride Offset in elements between the destination element start addresses of two consecutive
 * iterations of this loop dimension; the page gives the range [0, 2^20 - 1].
 * @param [in] src_stride Offset in elements between the source element start addresses of two consecutive iterations of
 * this loop dimension; the page gives the range [0, 2^40 - 1].
 */
__aicore__ inline void asc_set_ndim_loop4_stride(uint64_t dst_stride, uint64_t src_stride);
//===========asc_set_ndim_pad_count===========
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Configures the left and right padding element counts of the loop1 to loop4 dimensions for NDDMA
 * high-dimensional copies.
 *
 * @override_id asc_set_ndim_pad_count__Rpadcountcfg
 * @archs 3510
 * @param [in] config Padding element count configuration for NDDMA high-dimensional tiled copies, of type
 * asc_ndim_pad_count_config&, whose per-field layout is defined by the linked union documentation; the loop0 padding
 * counts are configured through asc_ndim_copy_gm2ub's loop0_lp_count and loop0_rp_count instead of this interface.
 */
__aicore__ inline void asc_set_ndim_pad_count(asc_ndim_pad_count_config& config);
//===========asc_set_ndim_pad_value(int8/uint8/e2m1/e1m2/e8m0/e5m2/e4m3fn/int16/uint16/half/bfloat16/int32/uint32/float)===========
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the int8_t value that asc_ndim_copy_gm2ub uses to fill boundary regions in constant-padding mode, taking
 * effect from then on until it is set again.
 *
 * @override_id asc_set_ndim_pad_value__i8
 * @archs 3510
 * @param [in] pad_value int8_t constant fill value consumed by later asc_ndim_copy_gm2ub calls when their padding_mode
 * argument is true; the register write happens only on AIV and takes the low 8 bits of the value.
 */
__aicore__ inline void asc_set_ndim_pad_value(int8_t pad_value);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the uint8_t value that asc_ndim_copy_gm2ub uses to fill boundary regions in constant-padding mode, taking
 * effect from then on until it is set again.
 *
 * @override_id asc_set_ndim_pad_value__u8
 * @archs 3510
 * @param [in] pad_value uint8_t constant fill value consumed by later asc_ndim_copy_gm2ub calls when their padding_mode
 * argument is true; the register write happens only on AIV and takes the low 8 bits of the value.
 */
__aicore__ inline void asc_set_ndim_pad_value(uint8_t pad_value);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the int16_t value that asc_ndim_copy_gm2ub uses to fill boundary regions in constant-padding mode, taking
 * effect from then on until it is set again.
 *
 * @override_id asc_set_ndim_pad_value__i16
 * @archs 3510
 * @param [in] pad_value int16_t constant fill value consumed by later asc_ndim_copy_gm2ub calls when their padding_mode
 * argument is true; the register write happens only on AIV and takes the low 16 bits of the value.
 */
__aicore__ inline void asc_set_ndim_pad_value(int16_t pad_value);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the uint16_t value that asc_ndim_copy_gm2ub uses to fill boundary regions in constant-padding mode,
 * taking effect from then on until it is set again.
 *
 * @override_id asc_set_ndim_pad_value__u16
 * @archs 3510
 * @param [in] pad_value uint16_t constant fill value consumed by later asc_ndim_copy_gm2ub calls when their
 * padding_mode argument is true; the register write happens only on AIV and takes the low 16 bits of the value.
 */
__aicore__ inline void asc_set_ndim_pad_value(uint16_t pad_value);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the half value that asc_ndim_copy_gm2ub uses to fill boundary regions in constant-padding mode, taking
 * effect from then on until it is set again.
 *
 * @override_id asc_set_ndim_pad_value__f16
 * @archs 3510
 * @param [in] pad_value half constant fill value consumed by later asc_ndim_copy_gm2ub calls when their padding_mode
 * argument is true; the register write happens only on AIV and takes the low 16 bits of the value.
 */
__aicore__ inline void asc_set_ndim_pad_value(half pad_value);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the bfloat16_t value that asc_ndim_copy_gm2ub uses to fill boundary regions in constant-padding mode,
 * taking effect from then on until it is set again.
 *
 * @override_id asc_set_ndim_pad_value__bf16
 * @archs 3510
 * @param [in] pad_value bfloat16_t constant fill value consumed by later asc_ndim_copy_gm2ub calls when their
 * padding_mode argument is true; the register write happens only on AIV and takes the low 16 bits of the value.
 */
__aicore__ inline void asc_set_ndim_pad_value(bfloat16_t pad_value);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the int32_t value that asc_ndim_copy_gm2ub uses to fill boundary regions in constant-padding mode, taking
 * effect from then on until it is set again.
 *
 * @override_id asc_set_ndim_pad_value__i32
 * @archs 3510
 * @param [in] pad_value int32_t constant fill value consumed by later asc_ndim_copy_gm2ub calls when their padding_mode
 * argument is true; the register write happens only on AIV and takes the low 32 bits of the value.
 */
__aicore__ inline void asc_set_ndim_pad_value(int32_t pad_value);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the uint32_t value that asc_ndim_copy_gm2ub uses to fill boundary regions in constant-padding mode,
 * taking effect from then on until it is set again.
 *
 * @override_id asc_set_ndim_pad_value__u32
 * @archs 3510
 * @param [in] pad_value uint32_t constant fill value consumed by later asc_ndim_copy_gm2ub calls when their
 * padding_mode argument is true; the register write happens only on AIV and takes the low 32 bits of the value.
 */
__aicore__ inline void asc_set_ndim_pad_value(uint32_t pad_value);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Sets the float value that asc_ndim_copy_gm2ub uses to fill boundary regions in constant-padding mode, taking
 * effect from then on until it is set again.
 *
 * @override_id asc_set_ndim_pad_value__f32
 * @archs 3510
 * @param [in] pad_value float constant fill value consumed by later asc_ndim_copy_gm2ub calls when their padding_mode
 * argument is true; the register write happens only on AIV and takes the low 32 bits of the value.
 */
__aicore__ inline void asc_set_ndim_pad_value(float pad_value);
//===========asc_ndim_copy_gm2ub(int8/uint8/e2m1/e1m2/e8m0/e5m2/e4m3fn/int16/uint16/half/bfloat16/int32/uint32/float)===========
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies a multi-dimensional int8_t tile from Global Memory to Unified Buffer with per-dimension element counts
 * and optional padding, using the strides previously configured by the asc_set_ndim_loop_stride interfaces.
 *
 * @override_id asc_ndim_copy_gm2ub__PUi8_PGi8_u32_u32_u32_u32_u32_u8_u8_b_loadl2
 * @archs 3510
 * @param [out] dst Start address of the destination int8_t region in Unified Buffer, pointing at the start of the
 * padded data; requires 1-byte alignment.
 * @param [in] src Start address of the source int8_t region in Global Memory; requires dtype-width alignment (1, 2 or 4
 * bytes for b8, b16 and b32 types).
 * @param [in] loop0_size Number of int8_t elements transferred in dimension 0, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop1_size Number of int8_t elements transferred in dimension 1, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop2_size Number of int8_t elements transferred in dimension 2, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop3_size Number of int8_t elements transferred in dimension 3, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop4_size Number of int8_t elements transferred in dimension 4, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop0_lp_count Number of elements padded on the left of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] loop0_rp_count Number of elements padded on the right of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] padding_mode Padding fill mode: true fills every padding element with the fixed pad_value set by
 * asc_set_ndim_pad_value, false replicates the nearest value at the left or right edge of the dimension.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enum value selecting the L2 Cache management policy for the load;
 * replaces the deprecated cache_mode parameter.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies a multi-dimensional uint8_t tile from Global Memory to Unified Buffer with per-dimension element counts
 * and optional padding, using the strides previously configured by the asc_set_ndim_loop_stride interfaces.
 *
 * @override_id asc_ndim_copy_gm2ub__PUu8_PGu8_u32_u32_u32_u32_u32_u8_u8_b_loadl2
 * @archs 3510
 * @param [out] dst Start address of the destination uint8_t region in Unified Buffer, pointing at the start of the
 * padded data; requires 1-byte alignment.
 * @param [in] src Start address of the source uint8_t region in Global Memory; requires dtype-width alignment (1, 2 or
 * 4 bytes for b8, b16 and b32 types).
 * @param [in] loop0_size Number of uint8_t elements transferred in dimension 0, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop1_size Number of uint8_t elements transferred in dimension 1, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop2_size Number of uint8_t elements transferred in dimension 2, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop3_size Number of uint8_t elements transferred in dimension 3, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop4_size Number of uint8_t elements transferred in dimension 4, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop0_lp_count Number of elements padded on the left of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] loop0_rp_count Number of elements padded on the right of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] padding_mode Padding fill mode: true fills every padding element with the fixed pad_value set by
 * asc_set_ndim_pad_value, false replicates the nearest value at the left or right edge of the dimension.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enum value selecting the L2 Cache management policy for the load;
 * replaces the deprecated cache_mode parameter.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies a multi-dimensional fp4x2_e2m1_t tile from Global Memory to Unified Buffer with per-dimension element
 * counts and optional padding, using the strides previously configured by the asc_set_ndim_loop_stride interfaces.
 *
 * @override_id asc_ndim_copy_gm2ub__PUf4e2m1x2_PGf4e2m1x2_u32_u32_u32_u32_u32_u8_u8_b_loadl2
 * @archs 3510
 * @param [out] dst Start address of the destination fp4x2_e2m1_t region in Unified Buffer, pointing at the start of the
 * padded data; requires 1-byte alignment.
 * @param [in] src Start address of the source fp4x2_e2m1_t region in Global Memory; requires dtype-width alignment (1,
 * 2 or 4 bytes for b8, b16 and b32 types).
 * @param [in] loop0_size Number of fp4x2_e2m1_t elements transferred in dimension 0, excluding padding elements; unit
 * is elements with range [1, 2097151].
 * @param [in] loop1_size Number of fp4x2_e2m1_t elements transferred in dimension 1, excluding padding elements; unit
 * is elements with range [1, 2097151].
 * @param [in] loop2_size Number of fp4x2_e2m1_t elements transferred in dimension 2, excluding padding elements; unit
 * is elements with range [1, 2097151].
 * @param [in] loop3_size Number of fp4x2_e2m1_t elements transferred in dimension 3, excluding padding elements; unit
 * is elements with range [1, 2097151].
 * @param [in] loop4_size Number of fp4x2_e2m1_t elements transferred in dimension 4, excluding padding elements; unit
 * is elements with range [1, 2097151].
 * @param [in] loop0_lp_count Number of elements padded on the left of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] loop0_rp_count Number of elements padded on the right of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] padding_mode Padding fill mode: true fills every padding element with the fixed pad_value set by
 * asc_set_ndim_pad_value, false replicates the nearest value at the left or right edge of the dimension.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enum value selecting the L2 Cache management policy for the load;
 * replaces the deprecated cache_mode parameter.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp4x2_e2m1_t* dst, __gm__ fp4x2_e2m1_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies a multi-dimensional fp4x2_e1m2_t tile from Global Memory to Unified Buffer with per-dimension element
 * counts and optional padding, using the strides previously configured by the asc_set_ndim_loop_stride interfaces.
 *
 * @override_id asc_ndim_copy_gm2ub__PUf4e1m2x2_PGf4e1m2x2_u32_u32_u32_u32_u32_u8_u8_b_loadl2
 * @archs 3510
 * @param [out] dst Start address of the destination fp4x2_e1m2_t region in Unified Buffer, pointing at the start of the
 * padded data; requires 1-byte alignment.
 * @param [in] src Start address of the source fp4x2_e1m2_t region in Global Memory; requires dtype-width alignment (1,
 * 2 or 4 bytes for b8, b16 and b32 types).
 * @param [in] loop0_size Number of fp4x2_e1m2_t elements transferred in dimension 0, excluding padding elements; unit
 * is elements with range [1, 2097151].
 * @param [in] loop1_size Number of fp4x2_e1m2_t elements transferred in dimension 1, excluding padding elements; unit
 * is elements with range [1, 2097151].
 * @param [in] loop2_size Number of fp4x2_e1m2_t elements transferred in dimension 2, excluding padding elements; unit
 * is elements with range [1, 2097151].
 * @param [in] loop3_size Number of fp4x2_e1m2_t elements transferred in dimension 3, excluding padding elements; unit
 * is elements with range [1, 2097151].
 * @param [in] loop4_size Number of fp4x2_e1m2_t elements transferred in dimension 4, excluding padding elements; unit
 * is elements with range [1, 2097151].
 * @param [in] loop0_lp_count Number of elements padded on the left of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] loop0_rp_count Number of elements padded on the right of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] padding_mode Padding fill mode: true fills every padding element with the fixed pad_value set by
 * asc_set_ndim_pad_value, false replicates the nearest value at the left or right edge of the dimension.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enum value selecting the L2 Cache management policy for the load;
 * replaces the deprecated cache_mode parameter.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp4x2_e1m2_t* dst, __gm__ fp4x2_e1m2_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies a multi-dimensional fp8_e8m0_t tile from Global Memory to Unified Buffer with per-dimension element
 * counts and optional padding, using the strides previously configured by the asc_set_ndim_loop_stride interfaces.
 *
 * @override_id asc_ndim_copy_gm2ub__PUf8e8m0_PGf8e8m0_u32_u32_u32_u32_u32_u8_u8_b_loadl2
 * @archs 3510
 * @param [out] dst Start address of the destination fp8_e8m0_t region in Unified Buffer, pointing at the start of the
 * padded data; requires 1-byte alignment.
 * @param [in] src Start address of the source fp8_e8m0_t region in Global Memory; requires dtype-width alignment (1, 2
 * or 4 bytes for b8, b16 and b32 types).
 * @param [in] loop0_size Number of fp8_e8m0_t elements transferred in dimension 0, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop1_size Number of fp8_e8m0_t elements transferred in dimension 1, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop2_size Number of fp8_e8m0_t elements transferred in dimension 2, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop3_size Number of fp8_e8m0_t elements transferred in dimension 3, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop4_size Number of fp8_e8m0_t elements transferred in dimension 4, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop0_lp_count Number of elements padded on the left of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] loop0_rp_count Number of elements padded on the right of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] padding_mode Padding fill mode: true fills every padding element with the fixed pad_value set by
 * asc_set_ndim_pad_value, false replicates the nearest value at the left or right edge of the dimension.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enum value selecting the L2 Cache management policy for the load;
 * replaces the deprecated cache_mode parameter.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp8_e8m0_t* dst, __gm__ fp8_e8m0_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies a multi-dimensional fp8_e5m2_t tile from Global Memory to Unified Buffer with per-dimension element
 * counts and optional padding, using the strides previously configured by the asc_set_ndim_loop_stride interfaces.
 *
 * @override_id asc_ndim_copy_gm2ub__PUf8e5m2_PGf8e5m2_u32_u32_u32_u32_u32_u8_u8_b_loadl2
 * @archs 3510
 * @param [out] dst Start address of the destination fp8_e5m2_t region in Unified Buffer, pointing at the start of the
 * padded data; requires 1-byte alignment.
 * @param [in] src Start address of the source fp8_e5m2_t region in Global Memory; requires dtype-width alignment (1, 2
 * or 4 bytes for b8, b16 and b32 types).
 * @param [in] loop0_size Number of fp8_e5m2_t elements transferred in dimension 0, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop1_size Number of fp8_e5m2_t elements transferred in dimension 1, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop2_size Number of fp8_e5m2_t elements transferred in dimension 2, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop3_size Number of fp8_e5m2_t elements transferred in dimension 3, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop4_size Number of fp8_e5m2_t elements transferred in dimension 4, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop0_lp_count Number of elements padded on the left of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] loop0_rp_count Number of elements padded on the right of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] padding_mode Padding fill mode: true fills every padding element with the fixed pad_value set by
 * asc_set_ndim_pad_value, false replicates the nearest value at the left or right edge of the dimension.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enum value selecting the L2 Cache management policy for the load;
 * replaces the deprecated cache_mode parameter.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies a multi-dimensional fp8_e4m3fn_t tile from Global Memory to Unified Buffer with per-dimension element
 * counts and optional padding, using the strides previously configured by the asc_set_ndim_loop_stride interfaces.
 *
 * @override_id asc_ndim_copy_gm2ub__PUf8e4m3_PGf8e4m3_u32_u32_u32_u32_u32_u8_u8_b_loadl2
 * @archs 3510
 * @param [out] dst Start address of the destination fp8_e4m3fn_t region in Unified Buffer, pointing at the start of the
 * padded data; requires 1-byte alignment.
 * @param [in] src Start address of the source fp8_e4m3fn_t region in Global Memory; requires dtype-width alignment (1,
 * 2 or 4 bytes for b8, b16 and b32 types).
 * @param [in] loop0_size Number of fp8_e4m3fn_t elements transferred in dimension 0, excluding padding elements; unit
 * is elements with range [1, 2097151].
 * @param [in] loop1_size Number of fp8_e4m3fn_t elements transferred in dimension 1, excluding padding elements; unit
 * is elements with range [1, 2097151].
 * @param [in] loop2_size Number of fp8_e4m3fn_t elements transferred in dimension 2, excluding padding elements; unit
 * is elements with range [1, 2097151].
 * @param [in] loop3_size Number of fp8_e4m3fn_t elements transferred in dimension 3, excluding padding elements; unit
 * is elements with range [1, 2097151].
 * @param [in] loop4_size Number of fp8_e4m3fn_t elements transferred in dimension 4, excluding padding elements; unit
 * is elements with range [1, 2097151].
 * @param [in] loop0_lp_count Number of elements padded on the left of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] loop0_rp_count Number of elements padded on the right of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] padding_mode Padding fill mode: true fills every padding element with the fixed pad_value set by
 * asc_set_ndim_pad_value, false replicates the nearest value at the left or right edge of the dimension.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enum value selecting the L2 Cache management policy for the load;
 * replaces the deprecated cache_mode parameter.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies a multi-dimensional int16_t tile from Global Memory to Unified Buffer with per-dimension element counts
 * and optional padding, using the strides previously configured by the asc_set_ndim_loop_stride interfaces.
 *
 * @override_id asc_ndim_copy_gm2ub__PUi16_PGi16_u32_u32_u32_u32_u32_u8_u8_b_loadl2
 * @archs 3510
 * @param [out] dst Start address of the destination int16_t region in Unified Buffer, pointing at the start of the
 * padded data; requires 1-byte alignment.
 * @param [in] src Start address of the source int16_t region in Global Memory; requires dtype-width alignment (1, 2 or
 * 4 bytes for b8, b16 and b32 types).
 * @param [in] loop0_size Number of int16_t elements transferred in dimension 0, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop1_size Number of int16_t elements transferred in dimension 1, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop2_size Number of int16_t elements transferred in dimension 2, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop3_size Number of int16_t elements transferred in dimension 3, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop4_size Number of int16_t elements transferred in dimension 4, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop0_lp_count Number of elements padded on the left of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] loop0_rp_count Number of elements padded on the right of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] padding_mode Padding fill mode: true fills every padding element with the fixed pad_value set by
 * asc_set_ndim_pad_value, false replicates the nearest value at the left or right edge of the dimension.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enum value selecting the L2 Cache management policy for the load;
 * replaces the deprecated cache_mode parameter.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies a multi-dimensional uint16_t tile from Global Memory to Unified Buffer with per-dimension element
 * counts and optional padding, using the strides previously configured by the asc_set_ndim_loop_stride interfaces.
 *
 * @override_id asc_ndim_copy_gm2ub__PUu16_PGu16_u32_u32_u32_u32_u32_u8_u8_b_loadl2
 * @archs 3510
 * @param [out] dst Start address of the destination uint16_t region in Unified Buffer, pointing at the start of the
 * padded data; requires 1-byte alignment.
 * @param [in] src Start address of the source uint16_t region in Global Memory; requires dtype-width alignment (1, 2 or
 * 4 bytes for b8, b16 and b32 types).
 * @param [in] loop0_size Number of uint16_t elements transferred in dimension 0, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop1_size Number of uint16_t elements transferred in dimension 1, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop2_size Number of uint16_t elements transferred in dimension 2, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop3_size Number of uint16_t elements transferred in dimension 3, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop4_size Number of uint16_t elements transferred in dimension 4, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop0_lp_count Number of elements padded on the left of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] loop0_rp_count Number of elements padded on the right of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] padding_mode Padding fill mode: true fills every padding element with the fixed pad_value set by
 * asc_set_ndim_pad_value, false replicates the nearest value at the left or right edge of the dimension.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enum value selecting the L2 Cache management policy for the load;
 * replaces the deprecated cache_mode parameter.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies a multi-dimensional half tile from Global Memory to Unified Buffer with per-dimension element counts
 * and optional padding, using the strides previously configured by the asc_set_ndim_loop_stride interfaces.
 *
 * @override_id asc_ndim_copy_gm2ub__PUf16_PGf16_u32_u32_u32_u32_u32_u8_u8_b_loadl2
 * @archs 3510
 * @param [out] dst Start address of the destination half region in Unified Buffer, pointing at the start of the padded
 * data; requires 1-byte alignment.
 * @param [in] src Start address of the source half region in Global Memory; requires dtype-width alignment (1, 2 or 4
 * bytes for b8, b16 and b32 types).
 * @param [in] loop0_size Number of half elements transferred in dimension 0, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop1_size Number of half elements transferred in dimension 1, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop2_size Number of half elements transferred in dimension 2, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop3_size Number of half elements transferred in dimension 3, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop4_size Number of half elements transferred in dimension 4, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop0_lp_count Number of elements padded on the left of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] loop0_rp_count Number of elements padded on the right of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] padding_mode Padding fill mode: true fills every padding element with the fixed pad_value set by
 * asc_set_ndim_pad_value, false replicates the nearest value at the left or right edge of the dimension.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enum value selecting the L2 Cache management policy for the load;
 * replaces the deprecated cache_mode parameter.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ half* dst, __gm__ half* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies a multi-dimensional bfloat16_t tile from Global Memory to Unified Buffer with per-dimension element
 * counts and optional padding, using the strides previously configured by the asc_set_ndim_loop_stride interfaces.
 *
 * @override_id asc_ndim_copy_gm2ub__PUbf16_PGbf16_u32_u32_u32_u32_u32_u8_u8_b_loadl2
 * @archs 3510
 * @param [out] dst Start address of the destination bfloat16_t region in Unified Buffer, pointing at the start of the
 * padded data; requires 1-byte alignment.
 * @param [in] src Start address of the source bfloat16_t region in Global Memory; requires dtype-width alignment (1, 2
 * or 4 bytes for b8, b16 and b32 types).
 * @param [in] loop0_size Number of bfloat16_t elements transferred in dimension 0, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop1_size Number of bfloat16_t elements transferred in dimension 1, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop2_size Number of bfloat16_t elements transferred in dimension 2, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop3_size Number of bfloat16_t elements transferred in dimension 3, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop4_size Number of bfloat16_t elements transferred in dimension 4, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop0_lp_count Number of elements padded on the left of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] loop0_rp_count Number of elements padded on the right of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] padding_mode Padding fill mode: true fills every padding element with the fixed pad_value set by
 * asc_set_ndim_pad_value, false replicates the nearest value at the left or right edge of the dimension.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enum value selecting the L2 Cache management policy for the load;
 * replaces the deprecated cache_mode parameter.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies a multi-dimensional int32_t tile from Global Memory to Unified Buffer with per-dimension element counts
 * and optional padding, using the strides previously configured by the asc_set_ndim_loop_stride interfaces.
 *
 * @override_id asc_ndim_copy_gm2ub__PUi32_PGi32_u32_u32_u32_u32_u32_u8_u8_b_loadl2
 * @archs 3510
 * @param [out] dst Start address of the destination int32_t region in Unified Buffer, pointing at the start of the
 * padded data; requires 1-byte alignment.
 * @param [in] src Start address of the source int32_t region in Global Memory; requires dtype-width alignment (1, 2 or
 * 4 bytes for b8, b16 and b32 types).
 * @param [in] loop0_size Number of int32_t elements transferred in dimension 0, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop1_size Number of int32_t elements transferred in dimension 1, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop2_size Number of int32_t elements transferred in dimension 2, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop3_size Number of int32_t elements transferred in dimension 3, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop4_size Number of int32_t elements transferred in dimension 4, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop0_lp_count Number of elements padded on the left of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] loop0_rp_count Number of elements padded on the right of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] padding_mode Padding fill mode: true fills every padding element with the fixed pad_value set by
 * asc_set_ndim_pad_value, false replicates the nearest value at the left or right edge of the dimension.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enum value selecting the L2 Cache management policy for the load;
 * replaces the deprecated cache_mode parameter.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies a multi-dimensional uint32_t tile from Global Memory to Unified Buffer with per-dimension element
 * counts and optional padding, using the strides previously configured by the asc_set_ndim_loop_stride interfaces.
 *
 * @override_id asc_ndim_copy_gm2ub__PUu32_PGu32_u32_u32_u32_u32_u32_u8_u8_b_loadl2
 * @archs 3510
 * @param [out] dst Start address of the destination uint32_t region in Unified Buffer, pointing at the start of the
 * padded data; requires 1-byte alignment.
 * @param [in] src Start address of the source uint32_t region in Global Memory; requires dtype-width alignment (1, 2 or
 * 4 bytes for b8, b16 and b32 types).
 * @param [in] loop0_size Number of uint32_t elements transferred in dimension 0, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop1_size Number of uint32_t elements transferred in dimension 1, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop2_size Number of uint32_t elements transferred in dimension 2, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop3_size Number of uint32_t elements transferred in dimension 3, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop4_size Number of uint32_t elements transferred in dimension 4, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop0_lp_count Number of elements padded on the left of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] loop0_rp_count Number of elements padded on the right of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] padding_mode Padding fill mode: true fills every padding element with the fixed pad_value set by
 * asc_set_ndim_pad_value, false replicates the nearest value at the left or right edge of the dimension.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enum value selecting the L2 Cache management policy for the load;
 * replaces the deprecated cache_mode parameter.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies a multi-dimensional float tile from Global Memory to Unified Buffer with per-dimension element counts
 * and optional padding, using the strides previously configured by the asc_set_ndim_loop_stride interfaces.
 *
 * @override_id asc_ndim_copy_gm2ub__PUf32_PGf32_u32_u32_u32_u32_u32_u8_u8_b_loadl2
 * @archs 3510
 * @param [out] dst Start address of the destination float region in Unified Buffer, pointing at the start of the padded
 * data; requires 1-byte alignment.
 * @param [in] src Start address of the source float region in Global Memory; requires dtype-width alignment (1, 2 or 4
 * bytes for b8, b16 and b32 types).
 * @param [in] loop0_size Number of float elements transferred in dimension 0, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop1_size Number of float elements transferred in dimension 1, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop2_size Number of float elements transferred in dimension 2, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop3_size Number of float elements transferred in dimension 3, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop4_size Number of float elements transferred in dimension 4, excluding padding elements; unit is
 * elements with range [1, 2097151].
 * @param [in] loop0_lp_count Number of elements padded on the left of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] loop0_rp_count Number of elements padded on the right of dimension 0; unit is elements with range [0,
 * 255].
 * @param [in] padding_mode Padding fill mode: true fills every padding element with the fixed pad_value set by
 * asc_set_ndim_pad_value, false replicates the nearest value at the left or right edge of the dimension.
 * @param [in] l2_cache_mode asc_load_l2_cache_mode enum value selecting the L2 Cache management policy for the load;
 * replaces the deprecated cache_mode parameter.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ float* dst, __gm__ float* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode);
//======asc_ndim_copy_dci=====
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Flushes the NDDMA DataCache so that subsequent high-dimensional GM reads observe the latest data.
 *
 * @override_id asc_ndim_copy_dci__z
 * @archs 3510
 */
__aicore__ inline void asc_ndim_copy_dci();
//======asc_copy_ub2l1=======
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies data from Unified Buffer to L1 Buffer on AIV in burst_count blocks of burst_len DataBlocks, with source
 * and destination gaps between blocks.
 *
 * @override_id asc_copy_ub2l1__PL1v_PUv_u16_u16_u16_u16
 * @archs 3510
 * @param [out] dst Destination L1 Buffer start address; must be 32-byte aligned.
 * @param [in] src Source UB start address; must be 32-byte aligned.
 * @param [in] burst_count Number of consecutive transfer blocks; range [1, 4095].
 * @param [in] burst_len Length of each transfer block in DataBlocks of 32 bytes; range [1, 2^16 - 1].
 * @param [in] src_gap Gap between adjacent source blocks in DataBlocks of 32 bytes; range [0, 2^16 - 1].
 * @param [in] dst_gap Gap between adjacent destination blocks in DataBlocks of 32 bytes; range [0, 2^16 - 1].
 */
__aicore__ inline void asc_copy_ub2l1(
    __cbuf__ void* dst, __ubuf__ void* src, uint16_t burst_count, uint16_t burst_len, uint16_t src_gap,
    uint16_t dst_gap);
/**
 * @ingroup simd_c_api.vector_datamove
 * @brief Copies a contiguous byte range from Unified Buffer to L1 Buffer on AIV, with a size between 32 bytes and 4095
 * DataBlocks.
 *
 * @override_id asc_copy_ub2l1__PL1v_PUv_u32
 * @archs 3510
 * @param [out] dst Destination L1 Buffer start address; must be 32-byte aligned.
 * @param [in] src Source UB start address; must be 32-byte aligned.
 * @param [in] size Number of bytes to copy; range [32, 4095 x 32] and a multiple of 32.
 */
__aicore__ inline void asc_copy_ub2l1(__cbuf__ void* dst, __ubuf__ void* src, uint32_t size);

#if (ASC_DEVKIT_VERSION_NUM >= 90200000)
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ fp8_e6m2_t* dst, __gm__ fp8_e6m2_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode,
    uint64_t src_stride, uint32_t dst_stride);

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ fp8_e6m2_t* dst, __gm__ fp8_e6m2_t* src, uint32_t size);

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ fp8_e6m2_t* dst, __ubuf__ fp8_e6m2_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride);

__aicore__ inline void asc_copy_ub2gm_align(__gm__ fp8_e6m2_t* dst, __ubuf__ fp8_e6m2_t* src, uint32_t size);
#endif

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ half* dst, __gm__ half* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ float* dst, __gm__ float* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode,
    uint64_t src_stride, uint32_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode,
    uint64_t src_stride, uint32_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode,
    uint64_t src_stride, uint32_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, uint8_t l2_cache_mode,
    uint64_t src_stride, uint32_t dst_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_store_l2_cache_mode)
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t l2_cache_mode,
    uint64_t dst_stride, uint32_t src_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_store_l2_cache_mode)
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t l2_cache_mode,
    uint64_t dst_stride, uint32_t src_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_store_l2_cache_mode)
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t l2_cache_mode,
    uint64_t dst_stride, uint32_t src_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_store_l2_cache_mode)
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t l2_cache_mode,
    uint64_t dst_stride, uint32_t src_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_store_l2_cache_mode)
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t l2_cache_mode,
    uint64_t dst_stride, uint32_t src_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_store_l2_cache_mode)
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t l2_cache_mode,
    uint64_t dst_stride, uint32_t src_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_store_l2_cache_mode)
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ half* dst, __ubuf__ half* src, uint32_t burst_count, uint32_t burst_len, uint8_t l2_cache_mode,
    uint64_t dst_stride, uint32_t src_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_store_l2_cache_mode)
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ float* dst, __ubuf__ float* src, uint32_t burst_count, uint32_t burst_len, uint8_t l2_cache_mode,
    uint64_t dst_stride, uint32_t src_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_store_l2_cache_mode)
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t l2_cache_mode,
    uint64_t dst_stride, uint32_t src_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_store_l2_cache_mode)
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t l2_cache_mode,
    uint64_t dst_stride, uint32_t src_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_store_l2_cache_mode)
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t l2_cache_mode,
    uint64_t dst_stride, uint32_t src_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_store_l2_cache_mode)
__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t l2_cache_mode, uint64_t dst_stride, uint32_t src_stride);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated five-dimension GM-to-UB copy of int8_t data whose per-dimension sizes, zero-dimension padding
 * counts and padding mode are supplied directly to the call.
 *
 * @override_id asc_ndim_copy_gm2ub__PUi8_PGi8_u32_u32_u32_u32_u32_u8_u8_b_u8
 * @archs 3510
 * @param [out] dst Destination UB address that receives the multi-dimensional int8_t block, including any padding
 * elements added by this call.
 * @param [in] src Source GM address from which the multi-dimensional block is gathered.
 * @param [in] loop0_size Number of int8_t elements processed in dimension 0, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop1_size Number of int8_t elements processed in dimension 1, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop2_size Number of int8_t elements processed in dimension 2, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop3_size Number of int8_t elements processed in dimension 3, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop4_size Number of int8_t elements processed in dimension 4, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop0_lp_count Number of int8_t elements padded on the left of dimension 0; range [0, 255].
 * @param [in] loop0_rp_count Number of int8_t elements padded on the right of dimension 0; range [0, 255].
 * @param [in] padding_mode Padding value selection: true fills every padding element with the constant configured by
 * asc_set_ndim_pad_value, false replicates the nearest element on each side.
 * @param [in] cache_mode Deprecated raw uint8_t L2 Cache policy selector of the old prototype; the document instructs
 * callers to use the asc_load_l2_cache_mode enumerator through the replacement interface instead.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    uint8_t cache_mode);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated five-dimension GM-to-UB copy of uint8_t data whose per-dimension sizes, zero-dimension padding
 * counts and padding mode are supplied directly to the call.
 *
 * @override_id asc_ndim_copy_gm2ub__PUu8_PGu8_u32_u32_u32_u32_u32_u8_u8_b_u8
 * @archs 3510
 * @param [out] dst Destination UB address that receives the multi-dimensional uint8_t block, including any padding
 * elements added by this call.
 * @param [in] src Source GM address from which the multi-dimensional block is gathered.
 * @param [in] loop0_size Number of uint8_t elements processed in dimension 0, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop1_size Number of uint8_t elements processed in dimension 1, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop2_size Number of uint8_t elements processed in dimension 2, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop3_size Number of uint8_t elements processed in dimension 3, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop4_size Number of uint8_t elements processed in dimension 4, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop0_lp_count Number of uint8_t elements padded on the left of dimension 0; range [0, 255].
 * @param [in] loop0_rp_count Number of uint8_t elements padded on the right of dimension 0; range [0, 255].
 * @param [in] padding_mode Padding value selection: true fills every padding element with the constant configured by
 * asc_set_ndim_pad_value, false replicates the nearest element on each side.
 * @param [in] cache_mode Deprecated raw uint8_t L2 Cache policy selector of the old prototype; the document instructs
 * callers to use the asc_load_l2_cache_mode enumerator through the replacement interface instead.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    uint8_t cache_mode);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated five-dimension GM-to-UB copy of fp4x2_e2m1_t data whose per-dimension sizes, zero-dimension padding
 * counts and padding mode are supplied directly to the call.
 *
 * @override_id asc_ndim_copy_gm2ub__PUf4e2m1x2_PGf4e2m1x2_u32_u32_u32_u32_u32_u8_u8_b_u8
 * @archs 3510
 * @param [out] dst Destination UB address that receives the multi-dimensional fp4x2_e2m1_t block, including any padding
 * elements added by this call.
 * @param [in] src Source GM address from which the multi-dimensional block is gathered.
 * @param [in] loop0_size Number of fp4x2_e2m1_t elements processed in dimension 0, excluding padding elements; range
 * [1, 2^20-1].
 * @param [in] loop1_size Number of fp4x2_e2m1_t elements processed in dimension 1, excluding padding elements; range
 * [1, 2^20-1].
 * @param [in] loop2_size Number of fp4x2_e2m1_t elements processed in dimension 2, excluding padding elements; range
 * [1, 2^20-1].
 * @param [in] loop3_size Number of fp4x2_e2m1_t elements processed in dimension 3, excluding padding elements; range
 * [1, 2^20-1].
 * @param [in] loop4_size Number of fp4x2_e2m1_t elements processed in dimension 4, excluding padding elements; range
 * [1, 2^20-1].
 * @param [in] loop0_lp_count Number of fp4x2_e2m1_t elements padded on the left of dimension 0; range [0, 255].
 * @param [in] loop0_rp_count Number of fp4x2_e2m1_t elements padded on the right of dimension 0; range [0, 255].
 * @param [in] padding_mode Padding value selection: true fills every padding element with the constant configured by
 * asc_set_ndim_pad_value, false replicates the nearest element on each side.
 * @param [in] cache_mode Deprecated raw uint8_t L2 Cache policy selector of the old prototype; the document instructs
 * callers to use the asc_load_l2_cache_mode enumerator through the replacement interface instead.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp4x2_e2m1_t* dst, __gm__ fp4x2_e2m1_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    uint8_t cache_mode);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated five-dimension GM-to-UB copy of fp4x2_e1m2_t data whose per-dimension sizes, zero-dimension padding
 * counts and padding mode are supplied directly to the call.
 *
 * @override_id asc_ndim_copy_gm2ub__PUf4e1m2x2_PGf4e1m2x2_u32_u32_u32_u32_u32_u8_u8_b_u8
 * @archs 3510
 * @param [out] dst Destination UB address that receives the multi-dimensional fp4x2_e1m2_t block, including any padding
 * elements added by this call.
 * @param [in] src Source GM address from which the multi-dimensional block is gathered.
 * @param [in] loop0_size Number of fp4x2_e1m2_t elements processed in dimension 0, excluding padding elements; range
 * [1, 2^20-1].
 * @param [in] loop1_size Number of fp4x2_e1m2_t elements processed in dimension 1, excluding padding elements; range
 * [1, 2^20-1].
 * @param [in] loop2_size Number of fp4x2_e1m2_t elements processed in dimension 2, excluding padding elements; range
 * [1, 2^20-1].
 * @param [in] loop3_size Number of fp4x2_e1m2_t elements processed in dimension 3, excluding padding elements; range
 * [1, 2^20-1].
 * @param [in] loop4_size Number of fp4x2_e1m2_t elements processed in dimension 4, excluding padding elements; range
 * [1, 2^20-1].
 * @param [in] loop0_lp_count Number of fp4x2_e1m2_t elements padded on the left of dimension 0; range [0, 255].
 * @param [in] loop0_rp_count Number of fp4x2_e1m2_t elements padded on the right of dimension 0; range [0, 255].
 * @param [in] padding_mode Padding value selection: true fills every padding element with the constant configured by
 * asc_set_ndim_pad_value, false replicates the nearest element on each side.
 * @param [in] cache_mode Deprecated raw uint8_t L2 Cache policy selector of the old prototype; the document instructs
 * callers to use the asc_load_l2_cache_mode enumerator through the replacement interface instead.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp4x2_e1m2_t* dst, __gm__ fp4x2_e1m2_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    uint8_t cache_mode);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated five-dimension GM-to-UB copy of fp8_e8m0_t data whose per-dimension sizes, zero-dimension padding
 * counts and padding mode are supplied directly to the call.
 *
 * @override_id asc_ndim_copy_gm2ub__PUf8e8m0_PGf8e8m0_u32_u32_u32_u32_u32_u8_u8_b_u8
 * @archs 3510
 * @param [out] dst Destination UB address that receives the multi-dimensional fp8_e8m0_t block, including any padding
 * elements added by this call.
 * @param [in] src Source GM address from which the multi-dimensional block is gathered.
 * @param [in] loop0_size Number of fp8_e8m0_t elements processed in dimension 0, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop1_size Number of fp8_e8m0_t elements processed in dimension 1, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop2_size Number of fp8_e8m0_t elements processed in dimension 2, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop3_size Number of fp8_e8m0_t elements processed in dimension 3, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop4_size Number of fp8_e8m0_t elements processed in dimension 4, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop0_lp_count Number of fp8_e8m0_t elements padded on the left of dimension 0; range [0, 255].
 * @param [in] loop0_rp_count Number of fp8_e8m0_t elements padded on the right of dimension 0; range [0, 255].
 * @param [in] padding_mode Padding value selection: true fills every padding element with the constant configured by
 * asc_set_ndim_pad_value, false replicates the nearest element on each side.
 * @param [in] cache_mode Deprecated raw uint8_t L2 Cache policy selector of the old prototype; the document instructs
 * callers to use the asc_load_l2_cache_mode enumerator through the replacement interface instead.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp8_e8m0_t* dst, __gm__ fp8_e8m0_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    uint8_t cache_mode);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated five-dimension GM-to-UB copy of fp8_e5m2_t data whose per-dimension sizes, zero-dimension padding
 * counts and padding mode are supplied directly to the call.
 *
 * @override_id asc_ndim_copy_gm2ub__PUf8e5m2_PGf8e5m2_u32_u32_u32_u32_u32_u8_u8_b_u8
 * @archs 3510
 * @param [out] dst Destination UB address that receives the multi-dimensional fp8_e5m2_t block, including any padding
 * elements added by this call.
 * @param [in] src Source GM address from which the multi-dimensional block is gathered.
 * @param [in] loop0_size Number of fp8_e5m2_t elements processed in dimension 0, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop1_size Number of fp8_e5m2_t elements processed in dimension 1, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop2_size Number of fp8_e5m2_t elements processed in dimension 2, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop3_size Number of fp8_e5m2_t elements processed in dimension 3, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop4_size Number of fp8_e5m2_t elements processed in dimension 4, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop0_lp_count Number of fp8_e5m2_t elements padded on the left of dimension 0; range [0, 255].
 * @param [in] loop0_rp_count Number of fp8_e5m2_t elements padded on the right of dimension 0; range [0, 255].
 * @param [in] padding_mode Padding value selection: true fills every padding element with the constant configured by
 * asc_set_ndim_pad_value, false replicates the nearest element on each side.
 * @param [in] cache_mode Deprecated raw uint8_t L2 Cache policy selector of the old prototype; the document instructs
 * callers to use the asc_load_l2_cache_mode enumerator through the replacement interface instead.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    uint8_t cache_mode);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated five-dimension GM-to-UB copy of fp8_e4m3fn_t data whose per-dimension sizes, zero-dimension padding
 * counts and padding mode are supplied directly to the call.
 *
 * @override_id asc_ndim_copy_gm2ub__PUf8e4m3_PGf8e4m3_u32_u32_u32_u32_u32_u8_u8_b_u8
 * @archs 3510
 * @param [out] dst Destination UB address that receives the multi-dimensional fp8_e4m3fn_t block, including any padding
 * elements added by this call.
 * @param [in] src Source GM address from which the multi-dimensional block is gathered.
 * @param [in] loop0_size Number of fp8_e4m3fn_t elements processed in dimension 0, excluding padding elements; range
 * [1, 2^20-1].
 * @param [in] loop1_size Number of fp8_e4m3fn_t elements processed in dimension 1, excluding padding elements; range
 * [1, 2^20-1].
 * @param [in] loop2_size Number of fp8_e4m3fn_t elements processed in dimension 2, excluding padding elements; range
 * [1, 2^20-1].
 * @param [in] loop3_size Number of fp8_e4m3fn_t elements processed in dimension 3, excluding padding elements; range
 * [1, 2^20-1].
 * @param [in] loop4_size Number of fp8_e4m3fn_t elements processed in dimension 4, excluding padding elements; range
 * [1, 2^20-1].
 * @param [in] loop0_lp_count Number of fp8_e4m3fn_t elements padded on the left of dimension 0; range [0, 255].
 * @param [in] loop0_rp_count Number of fp8_e4m3fn_t elements padded on the right of dimension 0; range [0, 255].
 * @param [in] padding_mode Padding value selection: true fills every padding element with the constant configured by
 * asc_set_ndim_pad_value, false replicates the nearest element on each side.
 * @param [in] cache_mode Deprecated raw uint8_t L2 Cache policy selector of the old prototype; the document instructs
 * callers to use the asc_load_l2_cache_mode enumerator through the replacement interface instead.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    uint8_t cache_mode);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated five-dimension GM-to-UB copy of int16_t data whose per-dimension sizes, zero-dimension padding
 * counts and padding mode are supplied directly to the call.
 *
 * @override_id asc_ndim_copy_gm2ub__PUi16_PGi16_u32_u32_u32_u32_u32_u8_u8_b_u8
 * @archs 3510
 * @param [out] dst Destination UB address that receives the multi-dimensional int16_t block, including any padding
 * elements added by this call.
 * @param [in] src Source GM address from which the multi-dimensional block is gathered.
 * @param [in] loop0_size Number of int16_t elements processed in dimension 0, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop1_size Number of int16_t elements processed in dimension 1, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop2_size Number of int16_t elements processed in dimension 2, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop3_size Number of int16_t elements processed in dimension 3, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop4_size Number of int16_t elements processed in dimension 4, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop0_lp_count Number of int16_t elements padded on the left of dimension 0; range [0, 255].
 * @param [in] loop0_rp_count Number of int16_t elements padded on the right of dimension 0; range [0, 255].
 * @param [in] padding_mode Padding value selection: true fills every padding element with the constant configured by
 * asc_set_ndim_pad_value, false replicates the nearest element on each side.
 * @param [in] cache_mode Deprecated raw uint8_t L2 Cache policy selector of the old prototype; the document instructs
 * callers to use the asc_load_l2_cache_mode enumerator through the replacement interface instead.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    uint8_t cache_mode);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated five-dimension GM-to-UB copy of uint16_t data whose per-dimension sizes, zero-dimension padding
 * counts and padding mode are supplied directly to the call.
 *
 * @override_id asc_ndim_copy_gm2ub__PUu16_PGu16_u32_u32_u32_u32_u32_u8_u8_b_u8
 * @archs 3510
 * @param [out] dst Destination UB address that receives the multi-dimensional uint16_t block, including any padding
 * elements added by this call.
 * @param [in] src Source GM address from which the multi-dimensional block is gathered.
 * @param [in] loop0_size Number of uint16_t elements processed in dimension 0, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop1_size Number of uint16_t elements processed in dimension 1, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop2_size Number of uint16_t elements processed in dimension 2, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop3_size Number of uint16_t elements processed in dimension 3, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop4_size Number of uint16_t elements processed in dimension 4, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop0_lp_count Number of uint16_t elements padded on the left of dimension 0; range [0, 255].
 * @param [in] loop0_rp_count Number of uint16_t elements padded on the right of dimension 0; range [0, 255].
 * @param [in] padding_mode Padding value selection: true fills every padding element with the constant configured by
 * asc_set_ndim_pad_value, false replicates the nearest element on each side.
 * @param [in] cache_mode Deprecated raw uint8_t L2 Cache policy selector of the old prototype; the document instructs
 * callers to use the asc_load_l2_cache_mode enumerator through the replacement interface instead.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    uint8_t cache_mode);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated five-dimension GM-to-UB copy of half data whose per-dimension sizes, zero-dimension padding counts
 * and padding mode are supplied directly to the call.
 *
 * @override_id asc_ndim_copy_gm2ub__PUf16_PGf16_u32_u32_u32_u32_u32_u8_u8_b_u8
 * @archs 3510
 * @param [out] dst Destination UB address that receives the multi-dimensional half block, including any padding
 * elements added by this call.
 * @param [in] src Source GM address from which the multi-dimensional block is gathered.
 * @param [in] loop0_size Number of half elements processed in dimension 0, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop1_size Number of half elements processed in dimension 1, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop2_size Number of half elements processed in dimension 2, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop3_size Number of half elements processed in dimension 3, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop4_size Number of half elements processed in dimension 4, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop0_lp_count Number of half elements padded on the left of dimension 0; range [0, 255].
 * @param [in] loop0_rp_count Number of half elements padded on the right of dimension 0; range [0, 255].
 * @param [in] padding_mode Padding value selection: true fills every padding element with the constant configured by
 * asc_set_ndim_pad_value, false replicates the nearest element on each side.
 * @param [in] cache_mode Deprecated raw uint8_t L2 Cache policy selector of the old prototype; the document instructs
 * callers to use the asc_load_l2_cache_mode enumerator through the replacement interface instead.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ half* dst, __gm__ half* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    uint8_t cache_mode);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated five-dimension GM-to-UB copy of bfloat16_t data whose per-dimension sizes, zero-dimension padding
 * counts and padding mode are supplied directly to the call.
 *
 * @override_id asc_ndim_copy_gm2ub__PUbf16_PGbf16_u32_u32_u32_u32_u32_u8_u8_b_u8
 * @archs 3510
 * @param [out] dst Destination UB address that receives the multi-dimensional bfloat16_t block, including any padding
 * elements added by this call.
 * @param [in] src Source GM address from which the multi-dimensional block is gathered.
 * @param [in] loop0_size Number of bfloat16_t elements processed in dimension 0, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop1_size Number of bfloat16_t elements processed in dimension 1, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop2_size Number of bfloat16_t elements processed in dimension 2, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop3_size Number of bfloat16_t elements processed in dimension 3, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop4_size Number of bfloat16_t elements processed in dimension 4, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop0_lp_count Number of bfloat16_t elements padded on the left of dimension 0; range [0, 255].
 * @param [in] loop0_rp_count Number of bfloat16_t elements padded on the right of dimension 0; range [0, 255].
 * @param [in] padding_mode Padding value selection: true fills every padding element with the constant configured by
 * asc_set_ndim_pad_value, false replicates the nearest element on each side.
 * @param [in] cache_mode Deprecated raw uint8_t L2 Cache policy selector of the old prototype; the document instructs
 * callers to use the asc_load_l2_cache_mode enumerator through the replacement interface instead.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    uint8_t cache_mode);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated five-dimension GM-to-UB copy of int32_t data whose per-dimension sizes, zero-dimension padding
 * counts and padding mode are supplied directly to the call.
 *
 * @override_id asc_ndim_copy_gm2ub__PUi32_PGi32_u32_u32_u32_u32_u32_u8_u8_b_u8
 * @archs 3510
 * @param [out] dst Destination UB address that receives the multi-dimensional int32_t block, including any padding
 * elements added by this call.
 * @param [in] src Source GM address from which the multi-dimensional block is gathered.
 * @param [in] loop0_size Number of int32_t elements processed in dimension 0, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop1_size Number of int32_t elements processed in dimension 1, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop2_size Number of int32_t elements processed in dimension 2, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop3_size Number of int32_t elements processed in dimension 3, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop4_size Number of int32_t elements processed in dimension 4, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop0_lp_count Number of int32_t elements padded on the left of dimension 0; range [0, 255].
 * @param [in] loop0_rp_count Number of int32_t elements padded on the right of dimension 0; range [0, 255].
 * @param [in] padding_mode Padding value selection: true fills every padding element with the constant configured by
 * asc_set_ndim_pad_value, false replicates the nearest element on each side.
 * @param [in] cache_mode Deprecated raw uint8_t L2 Cache policy selector of the old prototype; the document instructs
 * callers to use the asc_load_l2_cache_mode enumerator through the replacement interface instead.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    uint8_t cache_mode);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated five-dimension GM-to-UB copy of uint32_t data whose per-dimension sizes, zero-dimension padding
 * counts and padding mode are supplied directly to the call.
 *
 * @override_id asc_ndim_copy_gm2ub__PUu32_PGu32_u32_u32_u32_u32_u32_u8_u8_b_u8
 * @archs 3510
 * @param [out] dst Destination UB address that receives the multi-dimensional uint32_t block, including any padding
 * elements added by this call.
 * @param [in] src Source GM address from which the multi-dimensional block is gathered.
 * @param [in] loop0_size Number of uint32_t elements processed in dimension 0, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop1_size Number of uint32_t elements processed in dimension 1, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop2_size Number of uint32_t elements processed in dimension 2, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop3_size Number of uint32_t elements processed in dimension 3, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop4_size Number of uint32_t elements processed in dimension 4, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop0_lp_count Number of uint32_t elements padded on the left of dimension 0; range [0, 255].
 * @param [in] loop0_rp_count Number of uint32_t elements padded on the right of dimension 0; range [0, 255].
 * @param [in] padding_mode Padding value selection: true fills every padding element with the constant configured by
 * asc_set_ndim_pad_value, false replicates the nearest element on each side.
 * @param [in] cache_mode Deprecated raw uint8_t L2 Cache policy selector of the old prototype; the document instructs
 * callers to use the asc_load_l2_cache_mode enumerator through the replacement interface instead.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    uint8_t cache_mode);

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_l2_cache_mode)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated five-dimension GM-to-UB copy of float data whose per-dimension sizes, zero-dimension padding counts
 * and padding mode are supplied directly to the call.
 *
 * @override_id asc_ndim_copy_gm2ub__PUf32_PGf32_u32_u32_u32_u32_u32_u8_u8_b_u8
 * @archs 3510
 * @param [out] dst Destination UB address that receives the multi-dimensional float block, including any padding
 * elements added by this call.
 * @param [in] src Source GM address from which the multi-dimensional block is gathered.
 * @param [in] loop0_size Number of float elements processed in dimension 0, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop1_size Number of float elements processed in dimension 1, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop2_size Number of float elements processed in dimension 2, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop3_size Number of float elements processed in dimension 3, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop4_size Number of float elements processed in dimension 4, excluding padding elements; range [1,
 * 2^20-1].
 * @param [in] loop0_lp_count Number of float elements padded on the left of dimension 0; range [0, 255].
 * @param [in] loop0_rp_count Number of float elements padded on the right of dimension 0; range [0, 255].
 * @param [in] padding_mode Padding value selection: true fills every padding element with the constant configured by
 * asc_set_ndim_pad_value, false replicates the nearest element on each side.
 * @param [in] cache_mode Deprecated raw uint8_t L2 Cache policy selector of the old prototype; the document instructs
 * callers to use the asc_load_l2_cache_mode enumerator through the replacement interface instead.
 */
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ float* dst, __gm__ float* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    uint8_t cache_mode);

//======asc_set_gm2ub_pad=======
ASC_DEPRECATED(9.1.0, "2027/09/07", asc_set_copy_pad_val)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated setter for the fill values placed on both sides of unaligned GM-to-UB data blocks.
 *
 * @override_id asc_set_gm2ub_pad__u32
 * @archs 3510
 * @param [in] pad_val Value filled into the left and right sides of the contiguous data blocks when unaligned GM data
 * is moved into UB.
 */
__aicore__ inline void asc_set_gm2ub_pad(uint32_t pad_val);

#if (ASC_DEVKIT_VERSION_NUM >= 90300000)

__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ half* dst, __ubuf__ half* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ float* dst, __ubuf__ float* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint32_t burst_count, uint32_t burst_len,
    bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode,
    uint64_t dst_stride, uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ fp8_e8m0_t* dst, __ubuf__ fp8_e8m0_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ fp8_e6m2_t* dst, __ubuf__ fp8_e6m2_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ half* dst, __ubuf__ half* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ half* dst, __ubuf__ half* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ float* dst, __ubuf__ float* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ float* dst, __ubuf__ float* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint32_t burst_count, uint32_t burst_len,
    bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint32_t size, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ fp8_e8m0_t* dst, __ubuf__ fp8_e8m0_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ fp8_e8m0_t* dst, __ubuf__ fp8_e8m0_t* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ fp8_e6m2_t* dst, __ubuf__ fp8_e6m2_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry);
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ fp8_e6m2_t* dst, __ubuf__ fp8_e6m2_t* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry);

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode);
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode);
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp4x2_e2m1_t* dst, __gm__ fp4x2_e2m1_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode);
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp4x2_e1m2_t* dst, __gm__ fp4x2_e1m2_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode);
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp8_e8m0_t* dst, __gm__ fp8_e8m0_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode);
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode);
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode);
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode);
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode);
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ half* dst, __gm__ half* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode);
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode);
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode);
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode);
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ float* dst, __gm__ float* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode);
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp8_e6m2_t* dst, __gm__ fp8_e6m2_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode);
__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp8_e6m2_t* dst, __gm__ fp8_e6m2_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode);

__aicore__ inline void asc_copy_gm2ub(
    __ubuf__ void* dst, __gm__ void* src, uint32_t burst_count, uint32_t burst_len, bool enable_pre_load_ignore_sync,
    uint64_t src_stride, uint32_t dst_stride, bool enable_retry);

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry);
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry);
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry);
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry);
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry);
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry);
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ half* dst, __gm__ half* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry);
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ float* dst, __gm__ float* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry);
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry);
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry);
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry);
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ fp8_e8m0_t* dst, __gm__ fp8_e8m0_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry);
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry);
__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ fp8_e6m2_t* dst, __gm__ fp8_e6m2_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry);

// ==========asc_scatter_ub2gm===========
__aicore__ inline void asc_scatter_ub2gm(
    __gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint8_t offset_type, __ubuf__ void* offset_addr,
    asc_store_l2_cache_mode l2_cache_mode, uint32_t burst_len, uint32_t src_stride, uint32_t offset_num);

__aicore__ inline void asc_scatter_ub2gm(
    __gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint8_t offset_type, __ubuf__ void* offset_addr,
    asc_store_l2_cache_mode l2_cache_mode, uint32_t burst_len, uint32_t src_stride, uint32_t offset_num);

__aicore__ inline void asc_scatter_ub2gm(
    __gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint8_t offset_type, __ubuf__ void* offset_addr,
    asc_store_l2_cache_mode l2_cache_mode, uint32_t burst_len, uint32_t src_stride, uint32_t offset_num);

__aicore__ inline void asc_scatter_ub2gm(
    __gm__ uint64_t* dst, __ubuf__ uint64_t* src, uint8_t offset_type, __ubuf__ void* offset_addr,
    asc_store_l2_cache_mode l2_cache_mode, uint32_t burst_len, uint32_t src_stride, uint32_t offset_num);

// ==========asc_gather_gm2ub===========
__aicore__ inline void asc_gather_gm2ub(
    __ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint8_t offset_type, bool enable_constant_pad,
    __ubuf__ void* offset_addr, asc_load_l2_cache_mode l2_cache_mode, uint32_t burst_len, uint32_t dst_stride,
    uint32_t offset_num);

__aicore__ inline void asc_gather_gm2ub(
    __ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint8_t offset_type, bool enable_constant_pad,
    __ubuf__ void* offset_addr, asc_load_l2_cache_mode l2_cache_mode, uint32_t burst_len, uint32_t dst_stride,
    uint32_t offset_num);

__aicore__ inline void asc_gather_gm2ub(
    __ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint8_t offset_type, bool enable_constant_pad,
    __ubuf__ void* offset_addr, asc_load_l2_cache_mode l2_cache_mode, uint32_t burst_len, uint32_t dst_stride,
    uint32_t offset_num);

__aicore__ inline void asc_gather_gm2ub(
    __ubuf__ uint64_t* dst, __gm__ uint64_t* src, uint8_t offset_type, bool enable_constant_pad,
    __ubuf__ void* offset_addr, asc_load_l2_cache_mode l2_cache_mode, uint32_t burst_len, uint32_t dst_stride,
    uint32_t offset_num);
#endif

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_DATAMOVE_VECTOR_DATAMOVE_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_VECTOR_DATAMOVE_VECTOR_DATAMOVE_H

#endif
