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
    "impl/c_api/memory_base_impl/vector_compute_composite_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_MEMORY_BASE_IMPL_VECTOR_COMPUTE_COMPOSITE_INTF_IMPL_H
#define IMPL_C_API_MEMORY_BASE_IMPL_VECTOR_COMPUTE_COMPOSITE_INTF_IMPL_H

#include "impl/c_api/memory_base_impl/utils_impl.h"

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_log and asc_sync)
__aicore__ inline void asc_log_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vln(dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_log and asc_sync)
__aicore__ inline void asc_log_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vln(dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gt_scalar and asc_sync)
__aicore__ inline void asc_gt_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpvs_gt(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gt_scalar and asc_sync)
__aicore__ inline void asc_gt_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpvs_gt(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bitsort and asc_sync)
__aicore__ inline void asc_bitsort_sync(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ uint32_t* src1, int32_t repeat)
{
    if ASC_IS_AIV {
        vbitsort(dst, src0, src1, repeat);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bitsort and asc_sync)
__aicore__ inline void asc_bitsort_sync(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ uint32_t* src1, int32_t repeat)
{
    if ASC_IS_AIV {
        vbitsort(dst, src0, src1, repeat);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mrgsort4 and asc_sync)
__aicore__ inline void asc_mrgsort4_sync(
    __ubuf__ half* dst, __ubuf__ half* src[ASC_C_API_MRGSORT_ELEMENT_LEN], uint8_t repeat, uint16_t element_length_0,
    uint16_t element_length_1, uint16_t element_length_2, uint16_t element_length_3, bool if_exhausted_suspension,
    uint8_t valid_bit)
{
    if ASC_IS_AIV {
        vmrgsort4(
            dst, src, repeat, element_length_0, element_length_1, element_length_2, element_length_3,
            if_exhausted_suspension, valid_bit);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mrgsort4 and asc_sync)
__aicore__ inline void asc_mrgsort4_sync(
    __ubuf__ float* dst, __ubuf__ float* src[ASC_C_API_MRGSORT_ELEMENT_LEN], uint8_t repeat, uint16_t element_length_0,
    uint16_t element_length_1, uint16_t element_length_2, uint16_t element_length_3, bool if_exhausted_suspension,
    uint8_t valid_bit)
{
    if ASC_IS_AIV {
        vmrgsort4(
            dst, src, repeat, element_length_0, element_length_1, element_length_2, element_length_3,
            if_exhausted_suspension, valid_bit);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add and asc_sync)
__aicore__ inline void asc_add_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vadd(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add and asc_sync)
__aicore__ inline void asc_add_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vadd(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add and asc_sync)
__aicore__ inline void asc_add_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vadd(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add and asc_sync)
__aicore__ inline void asc_add_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vadd(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub and asc_sync)
__aicore__ inline void asc_sub_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vsub(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub and asc_sync)
__aicore__ inline void asc_sub_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vsub(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub and asc_sync)
__aicore__ inline void asc_sub_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vsub(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub and asc_sync)
__aicore__ inline void asc_sub_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vsub(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul and asc_sync)
__aicore__ inline void asc_mul_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmul(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul and asc_sync)
__aicore__ inline void asc_mul_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmul(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul and asc_sync)
__aicore__ inline void asc_mul_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmul(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul and asc_sync)
__aicore__ inline void asc_mul_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmul(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_div and asc_sync)
__aicore__ inline void asc_div_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vdiv(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_div and asc_sync)
__aicore__ inline void asc_div_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vdiv(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_max and asc_sync)
__aicore__ inline void asc_max_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmax(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_max and asc_sync)
__aicore__ inline void asc_max_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmax(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_max and asc_sync)
__aicore__ inline void asc_max_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmax(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_max and asc_sync)
__aicore__ inline void asc_max_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmax(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_min and asc_sync)
__aicore__ inline void asc_min_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmin(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_min and asc_sync)
__aicore__ inline void asc_min_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmin(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_min and asc_sync)
__aicore__ inline void asc_min_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmin(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_min and asc_sync)
__aicore__ inline void asc_min_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmin(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_datablock_reduce_sum and asc_sync)
__aicore__ inline void asc_datablock_reduce_sum_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcgadd(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_datablock_reduce_sum and asc_sync)
__aicore__ inline void asc_datablock_reduce_sum_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcgadd(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_datablock_reduce_max and asc_sync)
__aicore__ inline void asc_datablock_reduce_max_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcgmax(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_datablock_reduce_max and asc_sync)
__aicore__ inline void asc_datablock_reduce_max_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcgmax(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_datablock_reduce_min and asc_sync)
__aicore__ inline void asc_datablock_reduce_min_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcgmin(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_datablock_reduce_min and asc_sync)
__aicore__ inline void asc_datablock_reduce_min_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcgmin(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_sum and asc_sync)
__aicore__ inline void asc_repeat_reduce_sum_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcadd(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, 0);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_sum and asc_sync)
__aicore__ inline void asc_repeat_reduce_sum_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcadd(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, 0);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_max_index_value and asc_sync)
__aicore__ inline void asc_repeat_reduce_max_index_value_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcmax(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, Order_t::INDEX_VALUE);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_max_index_value and asc_sync)
__aicore__ inline void asc_repeat_reduce_max_index_value_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcmax(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, Order_t::INDEX_VALUE);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_max_value_index and asc_sync)
__aicore__ inline void asc_repeat_reduce_max_value_index_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcmax(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, Order_t::VALUE_INDEX);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_max_value_index and asc_sync)
__aicore__ inline void asc_repeat_reduce_max_value_index_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcmax(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, Order_t::VALUE_INDEX);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_max_only_value and asc_sync)
__aicore__ inline void asc_repeat_reduce_max_only_value_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcmax(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, Order_t::ONLY_VALUE);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_max_only_value and asc_sync)
__aicore__ inline void asc_repeat_reduce_max_only_value_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcmax(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, Order_t::ONLY_VALUE);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_max_only_index and asc_sync)
__aicore__ inline void asc_repeat_reduce_max_only_index_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcmax(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, Order_t::ONLY_INDEX);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_max_only_index and asc_sync)
__aicore__ inline void asc_repeat_reduce_max_only_index_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcmax(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, Order_t::ONLY_INDEX);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_min_index_value and asc_sync)
__aicore__ inline void asc_repeat_reduce_min_index_value_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcmin(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, Order_t::INDEX_VALUE);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_min_index_value and asc_sync)
__aicore__ inline void asc_repeat_reduce_min_index_value_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcmin(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, Order_t::INDEX_VALUE);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_min_value_index and asc_sync)
__aicore__ inline void asc_repeat_reduce_min_value_index_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcmin(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, Order_t::VALUE_INDEX);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_min_value_index and asc_sync)
__aicore__ inline void asc_repeat_reduce_min_value_index_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcmin(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, Order_t::VALUE_INDEX);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_min_only_value and asc_sync)
__aicore__ inline void asc_repeat_reduce_min_only_value_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcmin(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, Order_t::ONLY_VALUE);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_min_only_value and asc_sync)
__aicore__ inline void asc_repeat_reduce_min_only_value_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcmin(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, Order_t::ONLY_VALUE);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_min_only_index and asc_sync)
__aicore__ inline void asc_repeat_reduce_min_only_index_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcmin(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, Order_t::ONLY_INDEX);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_repeat_reduce_min_only_index and asc_sync)
__aicore__ inline void asc_repeat_reduce_min_only_index_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcmin(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, Order_t::ONLY_INDEX);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_max_scalar and asc_sync)
__aicore__ inline void asc_max_scalar_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmaxs(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_max_scalar and asc_sync)
__aicore__ inline void asc_max_scalar_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmaxs(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_max_scalar and asc_sync)
__aicore__ inline void asc_max_scalar_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmaxs(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_max_scalar and asc_sync)
__aicore__ inline void asc_max_scalar_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmaxs(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_scalar and asc_sync)
__aicore__ inline void asc_mul_scalar_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmuls(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_scalar and asc_sync)
__aicore__ inline void asc_mul_scalar_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmuls(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_scalar and asc_sync)
__aicore__ inline void asc_mul_scalar_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmuls(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_scalar and asc_sync)
__aicore__ inline void asc_mul_scalar_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmuls(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add_scalar and asc_sync)
__aicore__ inline void asc_add_scalar_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vadds(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add_scalar and asc_sync)
__aicore__ inline void asc_add_scalar_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vadds(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add_scalar and asc_sync)
__aicore__ inline void asc_add_scalar_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vadds(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add_scalar and asc_sync)
__aicore__ inline void asc_add_scalar_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vadds(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub_scalar and asc_sync)
__aicore__ inline void asc_sub_scalar_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        float tmpValue = static_cast<float>(value);
        tmpValue = 0 - tmpValue;
        vadds(
            dst, src, static_cast<half>(tmpValue), ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub_scalar and asc_sync)
__aicore__ inline void asc_sub_scalar_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        float tmpValue = static_cast<float>(value);
        tmpValue = 0 - tmpValue;
        vadds(
            dst, src, static_cast<float>(tmpValue), ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub_scalar and asc_sync)
__aicore__ inline void asc_sub_scalar_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        float tmpValue = static_cast<float>(value);
        tmpValue = 0 - tmpValue;
        vadds(
            dst, src, static_cast<int16_t>(tmpValue), ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub_scalar and asc_sync)
__aicore__ inline void asc_sub_scalar_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        float tmpValue = static_cast<float>(value);
        tmpValue = 0 - tmpValue;
        vadds(
            dst, src, static_cast<int32_t>(tmpValue), ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_leakyrelu and asc_sync)
__aicore__ inline void asc_leakyrelu_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vlrelu(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_leakyrelu and asc_sync)
__aicore__ inline void asc_leakyrelu_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vlrelu(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_exp and asc_sync)
__aicore__ inline void asc_exp_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vexp(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_exp and asc_sync)
__aicore__ inline void asc_exp_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vexp(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_abs and asc_sync)
__aicore__ inline void asc_abs_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vabs(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_abs and asc_sync)
__aicore__ inline void asc_abs_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vabs(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_rcp and asc_sync)
__aicore__ inline void asc_rcp_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vrec(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_rcp and asc_sync)
__aicore__ inline void asc_rcp_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vrec(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_rsqrt and asc_sync)
__aicore__ inline void asc_rsqrt_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vrsqrt(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_rsqrt and asc_sync)
__aicore__ inline void asc_rsqrt_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vrsqrt(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sqrt and asc_sync)
__aicore__ inline void asc_sqrt_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vsqrt(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sqrt and asc_sync)
__aicore__ inline void asc_sqrt_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vsqrt(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_relu and asc_sync)
__aicore__ inline void asc_relu_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vrelu(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_relu and asc_sync)
__aicore__ inline void asc_relu_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vrelu(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_relu and asc_sync)
__aicore__ inline void asc_relu_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vrelu(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_not and asc_sync)
__aicore__ inline void asc_not_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vnot(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_not and asc_sync)
__aicore__ inline void asc_not_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vnot(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_and and asc_sync)
__aicore__ inline void asc_and_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vand(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_and and asc_sync)
__aicore__ inline void asc_and_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vand(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_or and asc_sync)
__aicore__ inline void asc_or_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vor(dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_or and asc_sync)
__aicore__ inline void asc_or_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vor(dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_shiftleft and asc_sync)
__aicore__ inline void asc_shiftleft_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t distance, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vshl(
            dst, src, distance, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_shiftleft and asc_sync)
__aicore__ inline void asc_shiftleft_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t distance, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vshl(
            dst, src, distance, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_shiftleft and asc_sync)
__aicore__ inline void asc_shiftleft_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t distance, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vshl(
            dst, src, distance, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_shiftleft and asc_sync)
__aicore__ inline void asc_shiftleft_sync(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t distance, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vshl(
            dst, src, distance, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_shiftright and asc_sync)
__aicore__ inline void asc_shiftright_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int32_t value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vshr(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, false);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_shiftright and asc_sync)
__aicore__ inline void asc_shiftright_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vshr(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, false);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_shiftright and asc_sync)
__aicore__ inline void asc_shiftright_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vshr(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, false);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_shiftright and asc_sync)
__aicore__ inline void asc_shiftright_sync(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vshr(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, false);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_shiftright_round and asc_sync)
__aicore__ inline void asc_shiftright_round_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, int32_t value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vshr(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, true);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_shiftright_round and asc_sync)
__aicore__ inline void asc_shiftright_round_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vshr(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, true);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_min_scalar and asc_sync)
__aicore__ inline void asc_min_scalar_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmins(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_min_scalar and asc_sync)
__aicore__ inline void asc_min_scalar_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmins(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_min_scalar and asc_sync)
__aicore__ inline void asc_min_scalar_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, int16_t value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmins(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_min_scalar and asc_sync)
__aicore__ inline void asc_min_scalar_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, int32_t value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmins(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_lt and asc_sync)
__aicore__ inline void asc_lt_sync(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmp_lt(
            src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_lt and asc_sync)
__aicore__ inline void asc_lt_sync(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmp_lt(
            src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_lt and asc_sync)
__aicore__ inline void asc_lt_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpv_lt(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_lt and asc_sync)
__aicore__ inline void asc_lt_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpv_lt(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gt and asc_sync)
__aicore__ inline void asc_gt_sync(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmp_gt(
            src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gt and asc_sync)
__aicore__ inline void asc_gt_sync(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmp_gt(
            src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gt and asc_sync)
__aicore__ inline void asc_gt_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpv_gt(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gt and asc_sync)
__aicore__ inline void asc_gt_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpv_gt(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ge and asc_sync)
__aicore__ inline void asc_ge_sync(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmp_ge(
            src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ge and asc_sync)
__aicore__ inline void asc_ge_sync(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmp_ge(
            src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ge and asc_sync)
__aicore__ inline void asc_ge_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpv_ge(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ge and asc_sync)
__aicore__ inline void asc_ge_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpv_ge(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_eq and asc_sync)
__aicore__ inline void asc_eq_sync(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmp_eq(
            src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_eq and asc_sync)
__aicore__ inline void asc_eq_sync(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmp_eq(
            src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_eq and asc_sync)
__aicore__ inline void asc_eq_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpv_eq(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_eq and asc_sync)
__aicore__ inline void asc_eq_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpv_eq(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_eq and asc_sync)
__aicore__ inline void asc_eq_sync(
    __ubuf__ uint8_t* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpv_eq(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ne and asc_sync)
__aicore__ inline void asc_ne_sync(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmp_ne(
            src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ne and asc_sync)
__aicore__ inline void asc_ne_sync(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmp_ne(
            src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ne and asc_sync)
__aicore__ inline void asc_ne_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpv_ne(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ne and asc_sync)
__aicore__ inline void asc_ne_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpv_ne(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_le and asc_sync)
__aicore__ inline void asc_le_sync(
    __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmp_le(
            src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_le and asc_sync)
__aicore__ inline void asc_le_sync(
    __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride, uint8_t src0_block_stride,
    uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride, uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmp_le(
            src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_le and asc_sync)
__aicore__ inline void asc_le_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpv_le(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_le and asc_sync)
__aicore__ inline void asc_le_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint8_t repeat, uint8_t dst_block_stride,
    uint8_t src0_block_stride, uint8_t src1_block_stride, uint8_t dst_repeat_stride, uint8_t src0_repeat_stride,
    uint8_t src1_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpv_le(
            dst, src0, src1, repeat, dst_block_stride, src0_block_stride, src1_block_stride, dst_repeat_stride,
            src0_repeat_stride, src1_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_lt_scalar and asc_sync)
__aicore__ inline void asc_lt_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpvs_lt(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_lt_scalar and asc_sync)
__aicore__ inline void asc_lt_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpvs_lt(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ge_scalar and asc_sync)
__aicore__ inline void asc_ge_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpvs_ge(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ge_scalar and asc_sync)
__aicore__ inline void asc_ge_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpvs_ge(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_eq_scalar and asc_sync)
__aicore__ inline void asc_eq_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpvs_eq(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_eq_scalar and asc_sync)
__aicore__ inline void asc_eq_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpvs_eq(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_eq_scalar and asc_sync)
__aicore__ inline void asc_eq_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ int32_t* src, int32_t value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpvs_eq(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ne_scalar and asc_sync)
__aicore__ inline void asc_ne_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpvs_ne(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_ne_scalar and asc_sync)
__aicore__ inline void asc_ne_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpvs_ne(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_le_scalar and asc_sync)
__aicore__ inline void asc_le_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src, half value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpvs_le(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_le_scalar and asc_sync)
__aicore__ inline void asc_le_scalar_sync(
    __ubuf__ uint8_t* dst, __ubuf__ float* src, float value, uint8_t repeat, uint16_t dst_block_stride,
    uint16_t src_block_stride, uint16_t dst_repeat_stride, uint16_t src_repeat_stride)
{
    if ASC_IS_AIV {
        vcmpvs_le(dst, src, value, repeat, dst_block_stride, src_block_stride, dst_repeat_stride, src_repeat_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_select and asc_sync)
__aicore__ inline void asc_select_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vsel(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_select and asc_sync)
__aicore__ inline void asc_select_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vsel(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_reduce and asc_sync)
__aicore__ inline void asc_reduce_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src0, __ubuf__ uint16_t* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vreducev2(
            dst, src0, src1, static_cast<uint16_t>(ASC_C_API_DEFAULT_REPEAT.U8), ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            static_cast<uint8_t>(0), ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            static_cast<uint8_t>(ASC_C_API_DEFAULT_REPEAT_STRIDE.U16));
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_reduce and asc_sync)
__aicore__ inline void asc_reduce_sync(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src0, __ubuf__ uint32_t* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vreducev2(
            dst, src0, src1, static_cast<uint16_t>(ASC_C_API_DEFAULT_REPEAT.U8), ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            static_cast<uint8_t>(0), ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            static_cast<uint8_t>(ASC_C_API_DEFAULT_REPEAT_STRIDE.U16));
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add_relu and asc_sync)
__aicore__ inline void asc_add_relu_sync(__ubuf__ int8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vaddreluconv_f162s8(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, false);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add_relu and asc_sync)
__aicore__ inline void asc_add_relu_sync(__ubuf__ half* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vaddreluconv_f322f16(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, false);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add_relu and asc_sync)
__aicore__ inline void asc_add_relu_sync(
    __ubuf__ int8_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vaddreluconv_s162s8(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, false);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add_relu and asc_sync)
__aicore__ inline void asc_add_relu_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vaddrelu(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add_relu and asc_sync)
__aicore__ inline void asc_add_relu_sync(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vaddrelu(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_add_relu and asc_sync)
__aicore__ inline void asc_add_relu_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vaddrelu(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_axpy and asc_sync)
__aicore__ inline void asc_axpy_sync(__ubuf__ half* dst, __ubuf__ half* src, half value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vaxpy(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_axpy and asc_sync)
__aicore__ inline void asc_axpy_sync(__ubuf__ float* dst, __ubuf__ float* src, float value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vaxpy(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_axpy and asc_sync)
__aicore__ inline void asc_axpy_sync(__ubuf__ float* dst, __ubuf__ half* src, half value, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vaxpy(
            dst, src, value, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fma and asc_sync)
__aicore__ inline void asc_fma_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmla(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fma and asc_sync)
__aicore__ inline void asc_fma_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmla(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fma and asc_sync)
__aicore__ inline void asc_fma_sync(__ubuf__ float* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmla(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gather_datablock and asc_sync)
__aicore__ inline void asc_gather_datablock_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride,
    uint8_t dst_block_stride, uint8_t repeat)
{
    if ASC_IS_AIV {
        vgatherb(dst, src_offset, (uint64_t)src, dst_repeat_stride, dst_block_stride, repeat);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gather_datablock and asc_sync)
__aicore__ inline void asc_gather_datablock_sync(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, __ubuf__ uint32_t* src_offset, uint16_t dst_repeat_stride,
    uint8_t dst_block_stride, uint8_t repeat)
{
    if ASC_IS_AIV {
        vgatherb(dst, src_offset, (uint64_t)src, dst_repeat_stride, dst_block_stride, repeat);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gather and asc_sync)
__aicore__ inline void asc_gather_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vgather(
            (__ubuf__ uint16_t*)dst, src_offset, (uint64_t)src, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gather and asc_sync)
__aicore__ inline void asc_gather_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vgather(dst, src_offset, (uint64_t)src, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gather and asc_sync)
__aicore__ inline void asc_gather_sync(
    __ubuf__ half* dst, __ubuf__ half* src, __ubuf__ uint32_t* src_offset, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vgather(
            (__ubuf__ uint16_t*)dst, src_offset, (uint64_t)src, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gather and asc_sync)
__aicore__ inline void asc_gather_sync(
    __ubuf__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vgather(
            (__ubuf__ uint16_t*)dst, src_offset, (uint64_t)src, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gather and asc_sync)
__aicore__ inline void asc_gather_sync(
    __ubuf__ int32_t* dst, __ubuf__ int32_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vgather(
            (__ubuf__ uint32_t*)dst, src_offset, (uint64_t)src, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gather and asc_sync)
__aicore__ inline void asc_gather_sync(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, __ubuf__ uint32_t* src_offset, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vgather(dst, src_offset, (uint64_t)src, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_gather and asc_sync)
__aicore__ inline void asc_gather_sync(
    __ubuf__ float* dst, __ubuf__ float* src, __ubuf__ uint32_t* src_offset, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vgather(
            (__ubuf__ uint32_t*)dst, src_offset, (uint64_t)src, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_add and asc_sync)
__aicore__ inline void asc_mul_add_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmadd(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_add and asc_sync)
__aicore__ inline void asc_mul_add_sync(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmadd(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_add_relu and asc_sync)
__aicore__ inline void asc_mul_add_relu_sync(
    __ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmaddrelu(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_add_relu and asc_sync)
__aicore__ inline void asc_mul_add_relu_sync(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vmaddrelu(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_cast_half2int8 and asc_sync)
__aicore__ inline void asc_mul_cast_half2int8_sync(
    __ubuf__ int8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        constexpr uint8_t dst_repeat_stride =
            static_cast<uint8_t>(ASC_C_API_DEFAULT_REPEAT_STRIDE.U8 / (sizeof(half) / sizeof(int8_t)));
        vmulconv_f162s8(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, dst_repeat_stride,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_cast_half2uint8 and asc_sync)
__aicore__ inline void asc_mul_cast_half2uint8_sync(
    __ubuf__ uint8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        constexpr uint8_t dst_repeat_stride =
            static_cast<uint8_t>(ASC_C_API_DEFAULT_REPEAT_STRIDE.U8 / (sizeof(half) / sizeof(uint8_t)));
        vmulconv_f162u8(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, dst_repeat_stride,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub_relu and asc_sync)
__aicore__ inline void asc_sub_relu_sync(__ubuf__ int8_t* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vsubreluconv_f162s8(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, false);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub_relu and asc_sync)
__aicore__ inline void asc_sub_relu_sync(__ubuf__ half* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vsubreluconv_f322f16(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, false);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub_relu and asc_sync)
__aicore__ inline void asc_sub_relu_sync(
    __ubuf__ int8_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vsubreluconv_s162s8(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, false);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub_relu and asc_sync)
__aicore__ inline void asc_sub_relu_sync(__ubuf__ half* dst, __ubuf__ half* src0, __ubuf__ half* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vsubrelu(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub_relu and asc_sync)
__aicore__ inline void asc_sub_relu_sync(
    __ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vsubrelu(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_sub_relu and asc_sync)
__aicore__ inline void asc_sub_relu_sync(
    __ubuf__ int16_t* dst, __ubuf__ int16_t* src0, __ubuf__ int16_t* src1, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vsubrelu(
            dst, src0, src1, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U8, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162float and asc_sync)
__aicore__ inline void asc_bfloat162float_sync(__ubuf__ float* dst, __ubuf__ bfloat16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_bf162f32(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rna and asc_sync)
__aicore__ inline void asc_bfloat162int32_rna_sync(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_bf162s32a(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_ru and asc_sync)
__aicore__ inline void asc_bfloat162int32_ru_sync(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_bf162s32c(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rd and asc_sync)
__aicore__ inline void asc_bfloat162int32_rd_sync(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_bf162s32f(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rn and asc_sync)
__aicore__ inline void asc_bfloat162int32_rn_sync(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_bf162s32r(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_bfloat162int32_rz and asc_sync)
__aicore__ inline void asc_bfloat162int32_rz_sync(__ubuf__ int32_t* dst, __ubuf__ bfloat16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_bf162s32z(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2float and asc_sync)
__aicore__ inline void asc_half2float_sync(__ubuf__ float* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162f32(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int16_rna and asc_sync)
__aicore__ inline void asc_half2int16_rna_sync(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162s16a(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int16_ru and asc_sync)
__aicore__ inline void asc_half2int16_ru_sync(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162s16c(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int16_rd and asc_sync)
__aicore__ inline void asc_half2int16_rd_sync(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162s16f(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int16_rn and asc_sync)
__aicore__ inline void asc_half2int16_rn_sync(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162s16r(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int16_rz and asc_sync)
__aicore__ inline void asc_half2int16_rz_sync(__ubuf__ int16_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162s16z(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int32_rna and asc_sync)
__aicore__ inline void asc_half2int32_rna_sync(__ubuf__ int32_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162s32a(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int32_ru and asc_sync)
__aicore__ inline void asc_half2int32_ru_sync(__ubuf__ int32_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162s32c(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int32_rd and asc_sync)
__aicore__ inline void asc_half2int32_rd_sync(__ubuf__ int32_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162s32f(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int32_rn and asc_sync)
__aicore__ inline void asc_half2int32_rn_sync(__ubuf__ int32_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162s32r(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int32_rz and asc_sync)
__aicore__ inline void asc_half2int32_rz_sync(__ubuf__ int32_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162s32z(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4 and asc_sync)
__aicore__ inline void asc_half2int4_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162s4(
            (__ubuf__ void*)dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_ONE_FOURTH_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4_rna and asc_sync)
__aicore__ inline void asc_half2int4_rna_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162s4a(
            (__ubuf__ void*)dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_ONE_FOURTH_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4_ru and asc_sync)
__aicore__ inline void asc_half2int4_ru_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162s4c(
            (__ubuf__ void*)dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_ONE_FOURTH_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4_rd and asc_sync)
__aicore__ inline void asc_half2int4_rd_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162s4f(
            (__ubuf__ void*)dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_ONE_FOURTH_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4_rn and asc_sync)
__aicore__ inline void asc_half2int4_rn_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162s4r(
            (__ubuf__ void*)dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_ONE_FOURTH_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int4_rz and asc_sync)
__aicore__ inline void asc_half2int4_rz_sync(__ubuf__ int4b_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162s4z(
            (__ubuf__ void*)dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_ONE_FOURTH_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8 and asc_sync)
__aicore__ inline void asc_half2int8_sync(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162s8(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rna and asc_sync)
__aicore__ inline void asc_half2int8_rna_sync(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162s8a(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_ru and asc_sync)
__aicore__ inline void asc_half2int8_ru_sync(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162s8c(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rd and asc_sync)
__aicore__ inline void asc_half2int8_rd_sync(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162s8f(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rn and asc_sync)
__aicore__ inline void asc_half2int8_rn_sync(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162s8r(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2int8_rz and asc_sync)
__aicore__ inline void asc_half2int8_rz_sync(__ubuf__ int8_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162s8z(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8 and asc_sync)
__aicore__ inline void asc_half2uint8_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162u8(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rna and asc_sync)
__aicore__ inline void asc_half2uint8_rna_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162u8a(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_ru and asc_sync)
__aicore__ inline void asc_half2uint8_ru_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162u8c(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rd and asc_sync)
__aicore__ inline void asc_half2uint8_rd_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162u8f(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rn and asc_sync)
__aicore__ inline void asc_half2uint8_rn_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162u8r(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_half2uint8_rz and asc_sync)
__aicore__ inline void asc_half2uint8_rz_sync(__ubuf__ uint8_t* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f162u8z(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half and asc_sync)
__aicore__ inline void asc_float2half_sync(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322f16(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rn and asc_sync)
__aicore__ inline void asc_float2half_rn_sync(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322f16r(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rna and asc_sync)
__aicore__ inline void asc_float2half_rna_sync(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322f16a(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rd and asc_sync)
__aicore__ inline void asc_float2half_rd_sync(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322f16f(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_ru and asc_sync)
__aicore__ inline void asc_float2half_ru_sync(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322f16c(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_rz and asc_sync)
__aicore__ inline void asc_float2half_rz_sync(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322f16z(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2half_ro and asc_sync)
__aicore__ inline void asc_float2half_ro_sync(__ubuf__ half* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322f16o(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2float_rn and asc_sync)
__aicore__ inline void asc_float2float_rn_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322f32r(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2float_rd and asc_sync)
__aicore__ inline void asc_float2float_rd_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322f32f(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2float_ru and asc_sync)
__aicore__ inline void asc_float2float_ru_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322f32c(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2float_rna and asc_sync)
__aicore__ inline void asc_float2float_rna_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322f32a(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2float_rz and asc_sync)
__aicore__ inline void asc_float2float_rz_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322f32z(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rna and asc_sync)
__aicore__ inline void asc_float2int16_rna_sync(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322s16a(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_ru and asc_sync)
__aicore__ inline void asc_float2int16_ru_sync(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322s16c(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rd and asc_sync)
__aicore__ inline void asc_float2int16_rd_sync(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322s16f(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rn and asc_sync)
__aicore__ inline void asc_float2int16_rn_sync(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322s16r(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int16_rz and asc_sync)
__aicore__ inline void asc_float2int16_rz_sync(__ubuf__ int16_t* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322s16z(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int32_rna and asc_sync)
__aicore__ inline void asc_float2int32_rna_sync(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322s32a(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int32_ru and asc_sync)
__aicore__ inline void asc_float2int32_ru_sync(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322s32c(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int32_rd and asc_sync)
__aicore__ inline void asc_float2int32_rd_sync(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322s32f(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int32_rn and asc_sync)
__aicore__ inline void asc_float2int32_rn_sync(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322s32r(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int32_rz and asc_sync)
__aicore__ inline void asc_float2int32_rz_sync(__ubuf__ int32_t* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322s32z(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rna and asc_sync)
__aicore__ inline void asc_float2int64_rna_sync(__ubuf__ int64_t* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322s64a(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_ru and asc_sync)
__aicore__ inline void asc_float2int64_ru_sync(__ubuf__ int64_t* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322s64c(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rd and asc_sync)
__aicore__ inline void asc_float2int64_rd_sync(__ubuf__ int64_t* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322s64f(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rn and asc_sync)
__aicore__ inline void asc_float2int64_rn_sync(__ubuf__ int64_t* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322s64r(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2int64_rz and asc_sync)
__aicore__ inline void asc_float2int64_rz_sync(__ubuf__ int64_t* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322s64z(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rn and asc_sync)
__aicore__ inline void asc_float2bfloat16_rn_sync(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322bf16r(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rna and asc_sync)
__aicore__ inline void asc_float2bfloat16_rna_sync(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322bf16a(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rd and asc_sync)
__aicore__ inline void asc_float2bfloat16_rd_sync(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322bf16f(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_ru and asc_sync)
__aicore__ inline void asc_float2bfloat16_ru_sync(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322bf16c(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_float2bfloat16_rz and asc_sync)
__aicore__ inline void asc_float2bfloat16_rz_sync(__ubuf__ bfloat16_t* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_f322bf16z(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_deq_int322half and asc_sync)
__aicore__ inline void asc_deq_int322half_sync(__ubuf__ half* dst, __ubuf__ int32_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        constexpr uint8_t dst_repeat_stride =
            static_cast<uint8_t>(ASC_C_API_DEFAULT_REPEAT_STRIDE.U8 / (sizeof(int32_t) / sizeof(half)));
        vconv_deq(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, dst_repeat_stride, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_deq_int162b8_h and asc_sync)
__aicore__ inline void asc_deq_int162b8_h_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_deqs162b8h(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, static_cast<uint8_t>(ASC_C_API_DEFAULT_REPEAT_STRIDE.U16),
            static_cast<uint8_t>(ASC_C_API_DEFAULT_REPEAT_STRIDE.U16));
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_deq_int162b8_h and asc_sync)
__aicore__ inline void asc_deq_int162b8_h_sync(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_deqs162b8h(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, static_cast<uint8_t>(ASC_C_API_DEFAULT_REPEAT_STRIDE.U16),
            static_cast<uint8_t>(ASC_C_API_DEFAULT_REPEAT_STRIDE.U16));
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_deq_int162b8_l and asc_sync)
__aicore__ inline void asc_deq_int162b8_l_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_deqs162b8l(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, static_cast<uint8_t>(ASC_C_API_DEFAULT_REPEAT_STRIDE.U16),
            static_cast<uint8_t>(ASC_C_API_DEFAULT_REPEAT_STRIDE.U16));
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_deq_int162b8_l and asc_sync)
__aicore__ inline void asc_deq_int162b8_l_sync(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_deqs162b8l(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, static_cast<uint8_t>(ASC_C_API_DEFAULT_REPEAT_STRIDE.U16),
            static_cast<uint8_t>(ASC_C_API_DEFAULT_REPEAT_STRIDE.U16));
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int42half and asc_sync)
__aicore__ inline void asc_int42half_sync(__ubuf__ half* dst, __ubuf__ int4b_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s42f16(
            dst, (__ubuf__ void*)src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U8,
            ASC_C_API_ONE_FOURTH_DEFAULT_REPEAT_STRIDE.U8);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int82half and asc_sync)
__aicore__ inline void asc_int82half_sync(__ubuf__ half* dst, __ubuf__ int8_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s82f16(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_uint82half and asc_sync)
__aicore__ inline void asc_uint82half_sync(__ubuf__ half* dst, __ubuf__ uint8_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_u82f16(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162half and asc_sync)
__aicore__ inline void asc_int162half_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s162f16(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162half_rna and asc_sync)
__aicore__ inline void asc_int162half_rna_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s162f16a(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162half_ru and asc_sync)
__aicore__ inline void asc_int162half_ru_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s162f16c(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162half_rd and asc_sync)
__aicore__ inline void asc_int162half_rd_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s162f16f(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162half_rn and asc_sync)
__aicore__ inline void asc_int162half_rn_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s162f16r(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162half_rz and asc_sync)
__aicore__ inline void asc_int162half_rz_sync(__ubuf__ half* dst, __ubuf__ int16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s162f16z(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int162float and asc_sync)
__aicore__ inline void asc_int162float_sync(__ubuf__ float* dst, __ubuf__ int16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s162f32(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322float and asc_sync)
__aicore__ inline void asc_int322float_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s322f32(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322float_rna and asc_sync)
__aicore__ inline void asc_int322float_rna_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s322f32a(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322float_ru and asc_sync)
__aicore__ inline void asc_int322float_ru_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s322f32c(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322float_rd and asc_sync)
__aicore__ inline void asc_int322float_rd_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s322f32f(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322float_rn and asc_sync)
__aicore__ inline void asc_int322float_rn_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s322f32r(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322float_rz and asc_sync)
__aicore__ inline void asc_int322float_rz_sync(__ubuf__ float* dst, __ubuf__ int32_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s322f32z(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322int16 and asc_sync)
__aicore__ inline void asc_int322int16_sync(__ubuf__ int16_t* dst, __ubuf__ int32_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s322s16(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int322int64 and asc_sync)
__aicore__ inline void asc_int322int64_sync(__ubuf__ int64_t* dst, __ubuf__ int32_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s322s64(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642int32 and asc_sync)
__aicore__ inline void asc_int642int32_sync(__ubuf__ int32_t* dst, __ubuf__ int64_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s642s32(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642float_rna and asc_sync)
__aicore__ inline void asc_int642float_rna_sync(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s642f32a(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642float_ru and asc_sync)
__aicore__ inline void asc_int642float_ru_sync(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s642f32c(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642float_rd and asc_sync)
__aicore__ inline void asc_int642float_rd_sync(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s642f32f(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642float_rn and asc_sync)
__aicore__ inline void asc_int642float_rn_sync(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s642f32r(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_int642float_rz and asc_sync)
__aicore__ inline void asc_int642float_rz_sync(__ubuf__ float* dst, __ubuf__ int64_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_s642f32z(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_vdeq_int162b8_h and asc_sync)
__aicore__ inline void asc_vdeq_int162b8_h_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_vdeqs162b8h(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, static_cast<uint8_t>(ASC_C_API_DEFAULT_REPEAT_STRIDE.U16),
            static_cast<uint8_t>(ASC_C_API_DEFAULT_REPEAT_STRIDE.U16));
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_vdeq_int162b8_h and asc_sync)
__aicore__ inline void asc_vdeq_int162b8_h_sync(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_vdeqs162b8h(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, static_cast<uint8_t>(ASC_C_API_DEFAULT_REPEAT_STRIDE.U16),
            static_cast<uint8_t>(ASC_C_API_DEFAULT_REPEAT_STRIDE.U16));
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_vdeq_int162b8_l and asc_sync)
__aicore__ inline void asc_vdeq_int162b8_l_sync(__ubuf__ int8_t* dst, __ubuf__ int16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_vdeqs162b8l(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, static_cast<uint8_t>(ASC_C_API_DEFAULT_REPEAT_STRIDE.U16),
            static_cast<uint8_t>(ASC_C_API_DEFAULT_REPEAT_STRIDE.U16));
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_vdeq_int162b8_l and asc_sync)
__aicore__ inline void asc_vdeq_int162b8_l_sync(__ubuf__ uint8_t* dst, __ubuf__ int16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vconv_vdeqs162b8l(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, static_cast<uint8_t>(ASC_C_API_DEFAULT_REPEAT_STRIDE.U16),
            static_cast<uint8_t>(ASC_C_API_DEFAULT_REPEAT_STRIDE.U16));
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_transpose and asc_sync)
__aicore__ inline void asc_transpose_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src)
{
    if ASC_IS_AIV {
        vtranspose(dst, src);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_transpose and asc_sync)
__aicore__ inline void asc_transpose_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src)
{
    if ASC_IS_AIV {
        vtranspose(dst, src);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_transto5hd_b8 and asc_sync)
__aicore__ inline void asc_transto5hd_b8_sync(
    ub_addr8_t dst, ub_addr8_t src, uint8_t repeat, uint16_t dst_stride, uint16_t src_stride, bool dst_high_half,
    bool src_high_half)
{
    if ASC_IS_AIV {
        if (dst == ub_addr8_t::VA0) {
            if (src == ub_addr8_t::VA2) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA0, VA2, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA0, VA2, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA0, VA2, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA0, VA2, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            } else if (src == ub_addr8_t::VA4) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA0, VA4, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA0, VA4, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA0, VA4, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA0, VA4, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            } else if (src == ub_addr8_t::VA6) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA0, VA6, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA0, VA6, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA0, VA6, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA0, VA6, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            }
        } else if (dst == ub_addr8_t::VA2) {
            if (src == ub_addr8_t::VA0) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA2, VA0, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA2, VA0, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA2, VA0, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA2, VA0, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            } else if (src == ub_addr8_t::VA4) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA2, VA4, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA2, VA4, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA2, VA4, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA2, VA4, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            } else if (src == ub_addr8_t::VA6) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA2, VA6, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA2, VA6, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA2, VA6, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA2, VA6, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            }
        } else if (dst == ub_addr8_t::VA4) {
            if (src == ub_addr8_t::VA0) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA4, VA0, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA4, VA0, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA4, VA0, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA4, VA0, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            } else if (src == ub_addr8_t::VA2) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA4, VA2, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA4, VA2, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA4, VA2, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA4, VA2, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            } else if (src == ub_addr8_t::VA6) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA4, VA6, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA4, VA6, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA4, VA6, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA4, VA6, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            }
        } else if (dst == ub_addr8_t::VA6) {
            if (src == ub_addr8_t::VA0) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA6, VA0, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA6, VA0, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA6, VA0, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA6, VA0, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            } else if (src == ub_addr8_t::VA2) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA6, VA2, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA6, VA2, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA6, VA2, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA6, VA2, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            } else if (src == ub_addr8_t::VA4) {
                do {
                    if (!(dst_high_half)) {
                        if (!(src_high_half)) {
                            scatter_vnchwconv_b8(VA6, VA4, repeat, dst_stride, src_stride, false, false);
                        } else {
                            scatter_vnchwconv_b8(VA6, VA4, repeat, dst_stride, src_stride, false, true);
                        }
                    } else if (!(src_high_half)) {
                        scatter_vnchwconv_b8(VA6, VA4, repeat, dst_stride, src_stride, true, false);
                    } else {
                        scatter_vnchwconv_b8(VA6, VA4, repeat, dst_stride, src_stride, true, true);
                    }
                } while (0);
            }
        }

        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_transto5hd_b16 and asc_sync)
__aicore__ inline void asc_transto5hd_b16_sync(
    ub_addr8_t dst, ub_addr8_t src, uint8_t repeat, uint16_t dst_stride, uint16_t src_stride)
{
    if ASC_IS_AIV {
        if (dst == ub_addr8_t::VA0) {
            if (src == ub_addr8_t::VA2) {
                scatter_vnchwconv_b16(VA0, VA2, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA4) {
                scatter_vnchwconv_b16(VA0, VA4, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA6) {
                scatter_vnchwconv_b16(VA0, VA6, repeat, dst_stride, src_stride);
            }
        } else if (dst == ub_addr8_t::VA2) {
            if (src == ub_addr8_t::VA0) {
                scatter_vnchwconv_b16(VA2, VA0, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA4) {
                scatter_vnchwconv_b16(VA2, VA4, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA6) {
                scatter_vnchwconv_b16(VA2, VA6, repeat, dst_stride, src_stride);
            }
        } else if (dst == ub_addr8_t::VA4) {
            if (src == ub_addr8_t::VA0) {
                scatter_vnchwconv_b16(VA4, VA0, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA2) {
                scatter_vnchwconv_b16(VA4, VA2, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA6) {
                scatter_vnchwconv_b16(VA4, VA6, repeat, dst_stride, src_stride);
            }
        } else if (dst == ub_addr8_t::VA6) {
            if (src == ub_addr8_t::VA0) {
                scatter_vnchwconv_b16(VA6, VA0, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA2) {
                scatter_vnchwconv_b16(VA6, VA2, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA4) {
                scatter_vnchwconv_b16(VA6, VA4, repeat, dst_stride, src_stride);
            }
        }

        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_transto5hd_b32 and asc_sync)
__aicore__ inline void asc_transto5hd_b32_sync(
    ub_addr8_t dst, ub_addr8_t src, uint8_t repeat, uint16_t dst_stride, uint16_t src_stride)
{
    if ASC_IS_AIV {
        if (dst == ub_addr8_t::VA0) {
            if (src == ub_addr8_t::VA2) {
                scatter_vnchwconv_b32(VA0, VA2, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA4) {
                scatter_vnchwconv_b32(VA0, VA4, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA6) {
                scatter_vnchwconv_b32(VA0, VA6, repeat, dst_stride, src_stride);
            }
        } else if (dst == ub_addr8_t::VA2) {
            if (src == ub_addr8_t::VA0) {
                scatter_vnchwconv_b32(VA2, VA0, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA4) {
                scatter_vnchwconv_b32(VA2, VA4, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA6) {
                scatter_vnchwconv_b32(VA2, VA6, repeat, dst_stride, src_stride);
            }
        } else if (dst == ub_addr8_t::VA4) {
            if (src == ub_addr8_t::VA0) {
                scatter_vnchwconv_b32(VA4, VA0, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA2) {
                scatter_vnchwconv_b32(VA4, VA2, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA6) {
                scatter_vnchwconv_b32(VA4, VA6, repeat, dst_stride, src_stride);
            }
        } else if (dst == ub_addr8_t::VA6) {
            if (src == ub_addr8_t::VA0) {
                scatter_vnchwconv_b32(VA6, VA0, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA2) {
                scatter_vnchwconv_b32(VA6, VA2, repeat, dst_stride, src_stride);
            } else if (src == ub_addr8_t::VA4) {
                scatter_vnchwconv_b32(VA6, VA4, repeat, dst_stride, src_stride);
            }
        }

        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_duplicate and asc_sync)
__aicore__ inline void asc_duplicate_sync(__ubuf__ half* dst, half src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vector_dup(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, 1,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, 0);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_duplicate and asc_sync)
__aicore__ inline void asc_duplicate_sync(__ubuf__ int16_t* dst, int16_t src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vector_dup(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, 1,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, 0);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_duplicate and asc_sync)
__aicore__ inline void asc_duplicate_sync(__ubuf__ uint16_t* dst, uint16_t src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vector_dup(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, 1,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, 0);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_duplicate and asc_sync)
__aicore__ inline void asc_duplicate_sync(__ubuf__ bfloat16_t* dst, bfloat16_t src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vector_dup(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, 1,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, 0);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_duplicate and asc_sync)
__aicore__ inline void asc_duplicate_sync(__ubuf__ float* dst, float src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vector_dup(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, 1,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, 0);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_duplicate and asc_sync)
__aicore__ inline void asc_duplicate_sync(__ubuf__ int32_t* dst, int32_t src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vector_dup(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, 1,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, 0);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_duplicate and asc_sync)
__aicore__ inline void asc_duplicate_sync(__ubuf__ uint32_t* dst, uint32_t src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vector_dup(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, 1,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16, 0);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_brcb and asc_sync)
__aicore__ inline void asc_brcb_sync(
    __ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint16_t dst_block_stride, uint16_t dst_repeat_stride,
    uint8_t repeat)
{
    if ASC_IS_AIV {
        vbrcb(dst, src, dst_block_stride, dst_repeat_stride, repeat);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_brcb and asc_sync)
__aicore__ inline void asc_brcb_sync(
    __ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint16_t dst_block_stride, uint16_t dst_repeat_stride,
    uint8_t repeat)
{
    if ASC_IS_AIV {
        vbrcb(dst, src, dst_block_stride, dst_repeat_stride, repeat);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_pair_reduce_sum and asc_sync)
__aicore__ inline void asc_pair_reduce_sum_sync(__ubuf__ half* dst, __ubuf__ half* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcpadd(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_pair_reduce_sum and asc_sync)
__aicore__ inline void asc_pair_reduce_sum_sync(__ubuf__ float* dst, __ubuf__ float* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcpadd(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy and asc_sync)
__aicore__ inline void asc_copy_sync(__ubuf__ int16_t* dst, __ubuf__ int16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcopy(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy and asc_sync)
__aicore__ inline void asc_copy_sync(__ubuf__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcopy(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy and asc_sync)
__aicore__ inline void asc_copy_sync(__ubuf__ int32_t* dst, __ubuf__ int32_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcopy(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy and asc_sync)
__aicore__ inline void asc_copy_sync(__ubuf__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t count)
{
    if ASC_IS_AIV {
        asc_set_mask_count_begin(count);
        vcopy(
            dst, src, ASC_C_API_DEFAULT_REPEAT.U8, ASC_C_API_DEFAULT_BLOCK_STRIDE.U16,
            ASC_C_API_DEFAULT_BLOCK_STRIDE.U16, ASC_C_API_DEFAULT_REPEAT_STRIDE.U16,
            ASC_C_API_DEFAULT_REPEAT_STRIDE.U16);
        asc_set_mask_count_end();
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2ub and asc_sync)
__aicore__ inline void asc_copy_ub2ub_sync(__ubuf__ void* dst, __ubuf__ void* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_ubuf(
            dst, src, 0, static_cast<uint16_t>(1), size / ASC_C_API_ONE_DATABLOCK_SIZE, static_cast<uint16_t>(0),
            static_cast<uint16_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub and asc_sync)
__aicore__ inline void asc_copy_gm2ub_sync(__ubuf__ void* dst, __gm__ void* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf(
            dst, src, 0, static_cast<uint16_t>(1), size / ASC_C_API_ONE_DATABLOCK_SIZE, static_cast<uint16_t>(0),
            static_cast<uint16_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm and asc_sync)
__aicore__ inline void asc_copy_ub2gm_sync(__gm__ void* dst, __ubuf__ void* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm(
            dst, src, 0, static_cast<uint16_t>(1), size / ASC_C_API_ONE_DATABLOCK_SIZE, static_cast<uint16_t>(0),
            static_cast<uint16_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_b8(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, static_cast<uint16_t>(1), size, static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), static_cast<uint32_t>(0), static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_b8(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, static_cast<uint16_t>(1), size, static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), static_cast<uint32_t>(0), static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_b16(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, static_cast<uint16_t>(1), size, static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), static_cast<uint32_t>(0), static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_b16(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, static_cast<uint16_t>(1), size, static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), static_cast<uint32_t>(0), static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ half* dst, __gm__ half* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_b16(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, static_cast<uint16_t>(1), size, static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), static_cast<uint32_t>(0), static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_b16(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, static_cast<uint16_t>(1), size, static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), static_cast<uint32_t>(0), static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_b32(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, static_cast<uint16_t>(1), size, static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), static_cast<uint32_t>(0), static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_b32(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, static_cast<uint16_t>(1), size, static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), static_cast<uint32_t>(0), static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2ub_align and asc_sync)
__aicore__ inline void asc_copy_gm2ub_align_sync(__ubuf__ float* dst, __gm__ float* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_b32(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, static_cast<uint16_t>(1), size, static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), static_cast<uint32_t>(0), static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b8(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint16_t>(1), size, static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), static_cast<uint32_t>(0), static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b8(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint16_t>(1), size, static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), static_cast<uint32_t>(0), static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ half* dst, __ubuf__ half* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b8(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint16_t>(1), size, static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), static_cast<uint32_t>(0), static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b16(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint16_t>(1), size, static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), static_cast<uint32_t>(0), static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b16(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint16_t>(1), size, static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), static_cast<uint32_t>(0), static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b16(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint16_t>(1), size, static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), static_cast<uint32_t>(0), static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b32(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint16_t>(1), size, static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), static_cast<uint32_t>(0), static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ float* dst, __ubuf__ float* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b32(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint16_t>(1), size, static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), static_cast<uint32_t>(0), static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b32(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint16_t>(1), size, static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), static_cast<uint32_t>(0), static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ double* dst, __ubuf__ double* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b32(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint16_t>(1), size, static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), static_cast<uint32_t>(0), static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ int64_t* dst, __ubuf__ int64_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b32(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint16_t>(1), size, static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), static_cast<uint32_t>(0), static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_ub2gm_align and asc_sync)
__aicore__ inline void asc_copy_ub2gm_align_sync(__gm__ uint64_t* dst, __ubuf__ uint64_t* src, uint32_t size)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_b32(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, static_cast<uint16_t>(1), size, static_cast<uint8_t>(0),
            static_cast<uint8_t>(0), static_cast<uint32_t>(0), static_cast<uint32_t>(0));
        asc_sync_post_process();
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
