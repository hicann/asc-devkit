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
    "impl/c_api/memory_base_impl/vector_arith_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_MEMORY_BASE_IMPL_VECTOR_ARITH_INTF_IMPL_H
#define IMPL_C_API_MEMORY_BASE_IMPL_VECTOR_ARITH_INTF_IMPL_H

#include "impl/c_api/memory_base_impl/utils_impl.h"

__aicore__ inline void asc_max(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vmax(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
    }
}

__aicore__ inline void asc_max(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vmax(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
    }
}

__aicore__ inline void asc_max(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vmax(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
    }
}

__aicore__ inline void asc_max(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vmax(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
    }
}

__aicore__ inline void asc_max(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_max(
        dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_max(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_max(
        dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_max(__ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_max(
        dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_max(__ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_max(
        dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_mul(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vmul(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
    }
}

__aicore__ inline void asc_mul(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vmul(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
    }
}

__aicore__ inline void asc_mul(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vmul(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
    }
}

__aicore__ inline void asc_mul(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vmul(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
    }
}

__aicore__ inline void asc_mul(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_mul(
        dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_mul(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_mul(
        dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_mul(__ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_mul(
        dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_mul(__ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_mul(
        dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_add(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vadd(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
    }
}

__aicore__ inline void asc_add(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vadd(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
    }
}

__aicore__ inline void asc_add(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vadd(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
    }
}

__aicore__ inline void asc_add(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vadd(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
    }
}

__aicore__ inline void asc_add(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_add(
        dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_add(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_add(
        dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_add(__ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_add(
        dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_add(__ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_add(
        dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_sub(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vsub(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
    }
}

__aicore__ inline void asc_sub(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vsub(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
    }
}

__aicore__ inline void asc_sub(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vsub(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
    }
}

__aicore__ inline void asc_sub(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vsub(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
    }
}

__aicore__ inline void asc_sub(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_sub(
        dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_sub(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_sub(
        dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_sub(__ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_sub(
        dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_sub(__ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_sub(
        dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_min(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vmin(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
    }
}

__aicore__ inline void asc_min(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vmin(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
    }
}

__aicore__ inline void asc_min(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vmin(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
    }
}

__aicore__ inline void asc_min(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vmin(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
    }
}

__aicore__ inline void asc_min(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_min(
        dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_min(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_min(
        dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_min(__ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_min(
        dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_min(__ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_min(
        dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_div(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vdiv(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
    }
}

__aicore__ inline void asc_div(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vdiv(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
    }
}

__aicore__ inline void asc_div(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_div(
        dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_div(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_div(
        dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_max_scalar(
    __ubuf__ half* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vmaxs(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_max_scalar(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_max_scalar(
        dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_max_scalar(
    __ubuf__ float* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vmaxs(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_max_scalar(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_max_scalar(
        dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_max_scalar(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vmaxs(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_max_scalar(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_max_scalar(
        dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_max_scalar(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vmaxs(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_max_scalar(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_max_scalar(
        dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_mul_scalar(
    __ubuf__ half* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vmuls(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_mul_scalar(
    __ubuf__ float* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vmuls(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_mul_scalar(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vmuls(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_mul_scalar(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vmuls(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_mul_scalar(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_mul_scalar(
        dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_mul_scalar(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_mul_scalar(
        dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_mul_scalar(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_mul_scalar(
        dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_mul_scalar(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_mul_scalar(
        dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_add_scalar(
    __ubuf__ half* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vadds(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_add_scalar(
    __ubuf__ float* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vadds(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_add_scalar(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vadds(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_add_scalar(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vadds(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_add_scalar(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_add_scalar(
        dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_add_scalar(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_add_scalar(
        dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_add_scalar(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_add_scalar(
        dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_add_scalar(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_add_scalar(
        dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_sub_scalar(
    __ubuf__ half* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    float tmpValue = static_cast<float>(value);
    tmpValue = 0 - tmpValue;
    asc_add_scalar(
        dst, src, static_cast<half>(tmpValue), repeat, dst_block_stride, src_block_stride, dst_repeat_stride,
        src_repeat_stride);
}

__aicore__ inline void asc_sub_scalar(
    __ubuf__ float* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    float tmpValue = static_cast<float>(value);
    tmpValue = 0 - tmpValue;
    asc_add_scalar(
        dst, src, static_cast<float>(tmpValue), repeat, dst_block_stride, src_block_stride, dst_repeat_stride,
        src_repeat_stride);
}

__aicore__ inline void asc_sub_scalar(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    float tmpValue = static_cast<float>(value);
    tmpValue = 0 - tmpValue;
    asc_add_scalar(
        dst, src, static_cast<int16_t>(tmpValue), repeat, dst_block_stride, src_block_stride, dst_repeat_stride,
        src_repeat_stride);
}

__aicore__ inline void asc_sub_scalar(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    float tmpValue = static_cast<float>(value);
    tmpValue = 0 - tmpValue;
    asc_add_scalar(
        dst, src, static_cast<int32_t>(tmpValue), repeat, dst_block_stride, src_block_stride, dst_repeat_stride,
        src_repeat_stride);
}

__aicore__ inline void asc_sub_scalar(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_sub_scalar(
        dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_sub_scalar(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_sub_scalar(
        dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_sub_scalar(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_sub_scalar(
        dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_sub_scalar(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_sub_scalar(
        dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_min_scalar(
    __ubuf__ half* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vmins(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_min_scalar(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_min_scalar(
        dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_min_scalar(
    __ubuf__ float* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vmins(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_min_scalar(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_min_scalar(
        dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_min_scalar(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vmins(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_min_scalar(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_min_scalar(
        dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_min_scalar(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vmins(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_min_scalar(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_min_scalar(
        dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_exp(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vexp(dst, src, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_exp(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vexp(dst, src, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_exp(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_exp(
        dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_exp(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_exp(
        dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_log(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vln(dst, src, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_log(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_log(
        dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_log(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vln(dst, src, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_log(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_log(
        dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_abs(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vabs(dst, src, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_abs(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_abs(
        dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_abs(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vabs(dst, src, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_abs(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_abs(
        dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_rcp(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vrec(dst, src, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_rcp(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_rcp(
        dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_rcp(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vrec(dst, src, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_rcp(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_rcp(
        dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_sqrt(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vsqrt(dst, src, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_sqrt(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_sqrt(
        dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_sqrt(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vsqrt(dst, src, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_sqrt(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_sqrt(
        dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_rsqrt(
    __ubuf__ half* dst, __ubuf__ half* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vrsqrt(dst, src, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_rsqrt(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_rsqrt(
        dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_rsqrt(
    __ubuf__ float* dst, __ubuf__ float* src, uint8_t repeat, uint16_t dst_block_stride, uint16_t src_block_stride,
    uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vrsqrt(dst, src, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
    }
}

__aicore__ inline void asc_rsqrt(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_rsqrt(
        dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
        ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
    asc_set_mask_count_end();
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
