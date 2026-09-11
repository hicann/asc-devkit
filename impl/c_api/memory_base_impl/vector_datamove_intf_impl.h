/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/c_api/memory_base_impl/vector_datamove_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_MEMORY_BASE_IMPL_VECTOR_DATAMOVE_INTF_IMPL_H
#define IMPL_C_API_MEMORY_BASE_IMPL_VECTOR_DATAMOVE_INTF_IMPL_H

#include "impl/c_api/memory_base_impl/utils_impl.h"

__aicore__ inline void asc_copy(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    vcopy(dst, src, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
}

__aicore__ inline void asc_copy(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_copy(
        dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_copy(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    vcopy(dst, src, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
}

__aicore__ inline void asc_copy(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_copy(
        dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_copy(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    vcopy(dst, src, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
}

__aicore__ inline void asc_copy(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_copy(
        dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_copy(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    vcopy(dst, src, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
}

__aicore__ inline void asc_copy(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_copy(
        dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_copy_ub2ub(
    __ubuf__ void* dst, __ubuf__ void* src, uint16_t burst_count, uint16_t burst_len, uint16_t src_gap,
    uint16_t dst_gap)
{
    if ASC_IS_AIV {
        copy_ubuf_to_ubuf(dst, src, 0, burst_count, burst_len, src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_ub2ub(__ubuf__ void* dst, __ubuf__ void* src, uint32_t size)
{
    asc_copy_ub2ub(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void asc_copy_gm2ub(
    __ubuf__ void* dst, __gm__ void* src, uint16_t burst_count, uint16_t burst_len, uint16_t src_gap, uint16_t dst_gap)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf(dst, src, 0, burst_count, burst_len, src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_gm2ub(__ubuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2ub(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void asc_copy_ub2gm(
    __gm__ void* dst, __ubuf__ void* src, uint16_t burst_count, uint16_t burst_len, uint16_t src_gap, uint16_t dst_gap)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm(dst, src, 0, burst_count, burst_len, src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_ub2gm(__gm__ void* dst, __ubuf__ void* src, uint32_t size)
{
    asc_copy_ub2gm(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_b8(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, burst_count, burst_len, left_padding_num, right_padding_num,
            src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, 0, 0);
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ int8_t* dst, __gm__ int8_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_b8(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, burst_count, burst_len, left_padding_num, right_padding_num,
            src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, 0, 0);
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ half* dst, __gm__ half* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_b16(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, burst_count, burst_len, left_padding_num, right_padding_num,
            src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ half* dst, __gm__ half* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, 0, 0);
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_b16(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, burst_count, burst_len, left_padding_num, right_padding_num,
            src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, 0, 0);
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ int16_t* dst, __gm__ int16_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_b16(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, burst_count, burst_len, left_padding_num, right_padding_num,
            src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, 0, 0);
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_b16(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, burst_count, burst_len, left_padding_num, right_padding_num,
            src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, 0, 0);
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_b32(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, burst_count, burst_len, left_padding_num, right_padding_num,
            src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, 0, 0);
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ int32_t* dst, __gm__ int32_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_b32(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, burst_count, burst_len, left_padding_num, right_padding_num,
            src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, 0, 0);
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ float* dst, __gm__ float* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_b32(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, burst_count, burst_len, left_padding_num, right_padding_num,
            src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ float* dst, __gm__ float* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b8(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, left_padding_num, right_padding_num,
            src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, 0, 0, 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ int8_t* dst, __ubuf__ int8_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b8(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, left_padding_num, right_padding_num,
            src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, 0, 0, 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ half* dst, __ubuf__ half* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b8(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, left_padding_num, right_padding_num,
            src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ half* dst, __ubuf__ half* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, 0, 0, 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b16(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, left_padding_num, right_padding_num,
            src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, 0, 0, 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ int16_t* dst, __ubuf__ int16_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b16(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, left_padding_num, right_padding_num,
            src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, 0, 0, 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint16_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b16(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, left_padding_num, right_padding_num,
            src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, 0, 0, 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b32(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, left_padding_num, right_padding_num,
            src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, 0, 0, 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ float* dst, __ubuf__ float* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b32(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, left_padding_num, right_padding_num,
            src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ float* dst, __ubuf__ float* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, 0, 0, 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ int32_t* dst, __ubuf__ int32_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b32(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, left_padding_num, right_padding_num,
            src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, 0, 0, 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ double* dst, __ubuf__ double* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b32(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, left_padding_num, right_padding_num,
            src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ double* dst, __ubuf__ double* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, 0, 0, 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ int64_t* dst, __ubuf__ int64_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b32(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, left_padding_num, right_padding_num,
            src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ int64_t* dst, __ubuf__ int64_t* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, 0, 0, 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ uint64_t* dst, __ubuf__ uint64_t* src, uint16_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, uint32_t src_gap, uint32_t dst_gap)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b32(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, left_padding_num, right_padding_num,
            src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ uint64_t* dst, __ubuf__ uint64_t* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, 0, 0, 0, 0);
}

__aicore__ inline void asc_set_copy_pad_val(int8_t pad_value)
{
    if ASC_IS_AIV {
        set_mov_pad_val(*reinterpret_cast<uint8_t*>(&pad_value));
    }
}

__aicore__ inline void asc_set_copy_pad_val(uint8_t pad_value)
{
    if ASC_IS_AIV {
        set_mov_pad_val(*reinterpret_cast<uint8_t*>(&pad_value));
    }
}

__aicore__ inline void asc_set_copy_pad_val(int16_t pad_value)
{
    if ASC_IS_AIV {
        set_mov_pad_val(*reinterpret_cast<uint16_t*>(&pad_value));
    }
}

__aicore__ inline void asc_set_copy_pad_val(uint16_t pad_value)
{
    if ASC_IS_AIV {
        set_mov_pad_val(*reinterpret_cast<uint16_t*>(&pad_value));
    }
}

__aicore__ inline void asc_set_copy_pad_val(half pad_value)
{
    if ASC_IS_AIV {
        set_mov_pad_val(*reinterpret_cast<uint16_t*>(&pad_value));
    }
}

__aicore__ inline void asc_set_copy_pad_val(bfloat16_t pad_value)
{
    if ASC_IS_AIV {
        set_mov_pad_val(*reinterpret_cast<uint16_t*>(&pad_value));
    }
}

__aicore__ inline void asc_set_copy_pad_val(int32_t pad_value)
{
    if ASC_IS_AIV {
        set_mov_pad_val(*reinterpret_cast<uint32_t*>(&pad_value));
    }
}

__aicore__ inline void asc_set_copy_pad_val(uint32_t pad_value)
{
    if ASC_IS_AIV {
        set_mov_pad_val(*reinterpret_cast<uint32_t*>(&pad_value));
    }
}

__aicore__ inline void asc_set_copy_pad_val(float pad_value)
{
    if ASC_IS_AIV {
        set_mov_pad_val(*reinterpret_cast<uint32_t*>(&pad_value));
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
