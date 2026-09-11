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
    "impl/c_api/memory_base_impl/vector_gather_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_MEMORY_BASE_IMPL_VECTOR_GATHER_INTF_IMPL_H
#define IMPL_C_API_MEMORY_BASE_IMPL_VECTOR_GATHER_INTF_IMPL_H

#include "impl/c_api/memory_base_impl/utils_impl.h"

__aicore__ inline void asc_gather_datablock(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride,
    uint8_t dst_block_stride, uint8_t repeat)
{
    if ASC_IS_AIV {
        vgatherb(dst, src_offset, (uint64_t)src, dst_repeat_stride, dst_block_stride, repeat);
    }
}

__aicore__ inline void asc_gather_datablock(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride,
    uint8_t dst_block_stride, uint8_t repeat)
{
    if ASC_IS_AIV {
        vgatherb(dst, src_offset, (uint64_t)src, dst_repeat_stride, dst_block_stride, repeat);
    }
}

__aicore__ inline void asc_gather(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride,
    uint8_t repeat)
{
    if ASC_IS_AIV {
        vgather((__ubuf__ uint16_t*)dst, src_offset, (uint64_t)src, dst_repeat_stride, repeat);
    }
}

__aicore__ inline void asc_gather(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_gather(dst, src, src_offset, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_gather(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride,
    uint8_t repeat)
{
    if ASC_IS_AIV {
        vgather(dst, src_offset, (uint64_t)src, dst_repeat_stride, repeat);
    }
}

__aicore__ inline void asc_gather(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_gather(dst, src, src_offset, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_gather(
    __ubuf__ half* dst, __ubuf__ half* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t repeat)
{
    if ASC_IS_AIV {
        vgather((__ubuf__ uint16_t*)dst, src_offset, (uint64_t)src, dst_repeat_stride, repeat);
    }
}

__aicore__ inline void asc_gather(__ubuf__ half* dst, __ubuf__ half* src, __ubuf__ uint32_t* src_offset, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_gather(dst, src, src_offset, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_gather(
    __ubuf__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride,
    uint8_t repeat)
{
    if ASC_IS_AIV {
        vgather((__ubuf__ uint16_t*)dst, src_offset, (uint64_t)src, dst_repeat_stride, repeat);
    }
}

__aicore__ inline void asc_gather(
    __ubuf__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_gather(dst, src, src_offset, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_gather(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride,
    uint8_t repeat)
{
    if ASC_IS_AIV {
        vgather((__ubuf__ uint32_t*)dst, src_offset, (uint64_t)src, dst_repeat_stride, repeat);
    }
}

__aicore__ inline void asc_gather(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_gather(dst, src, src_offset, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_gather(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride,
    uint8_t repeat)
{
    if ASC_IS_AIV {
        vgather(dst, src_offset, (uint64_t)src, dst_repeat_stride, repeat);
    }
}

__aicore__ inline void asc_gather(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_gather(dst, src, src_offset, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT.U8);
    asc_set_mask_count_end();
}

__aicore__ inline void asc_gather(
    __ubuf__ float* dst, __ubuf__ float* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride, uint8_t repeat)
{
    if ASC_IS_AIV {
        vgather((__ubuf__ uint32_t*)dst, src_offset, (uint64_t)src, dst_repeat_stride, repeat);
    }
}

__aicore__ inline void asc_gather(
    __ubuf__ float* dst, __ubuf__ float* src, __ubuf__ uint32_t* src_offset, uint32_t count)
{
    asc_set_mask_count_begin(count);
    asc_gather(dst, src, src_offset, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT.U8);
    asc_set_mask_count_end();
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
