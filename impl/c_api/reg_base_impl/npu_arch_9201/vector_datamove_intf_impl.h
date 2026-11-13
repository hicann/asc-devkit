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
    "impl/c_api/reg_base_impl/npu_arch_9201/vector_datamove_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_9201_VECTOR_DATAMOVE_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_9201_VECTOR_DATAMOVE_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

__aicore__ inline void asc_copy_gm2ub(
    __ubuf__ void* dst, __gm__ void* src, uint32_t burst_count, uint32_t burst_len, uint64_t src_stride,
    uint32_t dst_stride)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ uint8_t*)dst, (__gm__ uint8_t*)src, 0, burst_count, burst_len, 0, 0, false, false, 0, src_stride,
            dst_stride, false);
    }
}

__aicore__ inline void asc_copy_gm2ub(__ubuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2ub(dst, src, 1, size, 0, 0);
}

__aicore__ inline void asc_copy_gm2ub(
    __ubuf__ void* dst, __gm__ void* src, uint32_t burst_count, uint32_t burst_len, bool enable_pre_load_ignore_sync,
    uint64_t src_stride, uint32_t dst_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ uint8_t*)dst, (__gm__ uint8_t*)src, 0, burst_count, burst_len, 0, 0, false,
            enable_pre_load_ignore_sync, 0, src_stride, dst_stride, enable_retry);
    }
}

__aicore__ inline void asc_copy_ub2gm(
    __gm__ void* dst, __ubuf__ void* src, uint32_t burst_count, uint32_t burst_len, uint64_t dst_stride,
    uint32_t src_stride)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            dst, src, 0, burst_count, burst_len, false, false, 0, 0, dst_stride, src_stride, false);
    }
}

__aicore__ inline void asc_copy_ub2gm(__gm__ void* dst, __ubuf__ void* src, uint32_t size)
{
    asc_copy_ub2gm(dst, src, 1, size, 0, 0);
}

// ==========asc_scatter_ub2gm===========
__aicore__ inline void asc_scatter_ub2gm(
    __gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint8_t offset_type, __ubuf__ void* offset_addr,
    asc_store_l2_cache_mode l2_cache_mode, uint32_t burst_len, uint32_t src_stride, uint32_t offset_num)
{
    if ASC_IS_AIV {
        scatter_ubuf_to_gm(
            dst, src, 0, offset_type, offset_addr, static_cast<uint8_t>(l2_cache_mode), burst_len, src_stride,
            offset_num);
    }
}

__aicore__ inline void asc_scatter_ub2gm(
    __gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint8_t offset_type, __ubuf__ void* offset_addr,
    asc_store_l2_cache_mode l2_cache_mode, uint32_t burst_len, uint32_t src_stride, uint32_t offset_num)
{
    if ASC_IS_AIV {
        scatter_ubuf_to_gm(
            dst, src, 0, offset_type, offset_addr, static_cast<uint8_t>(l2_cache_mode), burst_len, src_stride,
            offset_num);
    }
}

__aicore__ inline void asc_scatter_ub2gm(
    __gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint8_t offset_type, __ubuf__ void* offset_addr,
    asc_store_l2_cache_mode l2_cache_mode, uint32_t burst_len, uint32_t src_stride, uint32_t offset_num)
{
    if ASC_IS_AIV {
        scatter_ubuf_to_gm(
            dst, src, 0, offset_type, offset_addr, static_cast<uint8_t>(l2_cache_mode), burst_len, src_stride,
            offset_num);
    }
}

__aicore__ inline void asc_scatter_ub2gm(
    __gm__ uint64_t* dst, __ubuf__ uint64_t* src, uint8_t offset_type, __ubuf__ void* offset_addr,
    asc_store_l2_cache_mode l2_cache_mode, uint32_t burst_len, uint32_t src_stride, uint32_t offset_num)
{
    if ASC_IS_AIV {
        scatter_ubuf_to_gm(
            dst, src, 0, offset_type, offset_addr, static_cast<uint8_t>(l2_cache_mode), burst_len, src_stride,
            offset_num);
    }
}

// ==========asc_gather_gm2ub===========
__aicore__ inline void asc_gather_gm2ub(
    __ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint8_t offset_type, bool enable_constant_pad,
    __ubuf__ void* offset_addr, asc_load_l2_cache_mode l2_cache_mode, uint32_t burst_len, uint32_t dst_stride,
    uint32_t offset_num)
{
    if ASC_IS_AIV {
        gather_gm_to_ubuf(
            dst, src, 0, offset_type, enable_constant_pad, offset_addr, static_cast<uint8_t>(l2_cache_mode), burst_len,
            dst_stride, offset_num);
    }
}

__aicore__ inline void asc_gather_gm2ub(
    __ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint8_t offset_type, bool enable_constant_pad,
    __ubuf__ void* offset_addr, asc_load_l2_cache_mode l2_cache_mode, uint32_t burst_len, uint32_t dst_stride,
    uint32_t offset_num)
{
    if ASC_IS_AIV {
        gather_gm_to_ubuf(
            dst, src, 0, offset_type, enable_constant_pad, offset_addr, static_cast<uint8_t>(l2_cache_mode), burst_len,
            dst_stride, offset_num);
    }
}

__aicore__ inline void asc_gather_gm2ub(
    __ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint8_t offset_type, bool enable_constant_pad,
    __ubuf__ void* offset_addr, asc_load_l2_cache_mode l2_cache_mode, uint32_t burst_len, uint32_t dst_stride,
    uint32_t offset_num)
{
    if ASC_IS_AIV {
        gather_gm_to_ubuf(
            dst, src, 0, offset_type, enable_constant_pad, offset_addr, static_cast<uint8_t>(l2_cache_mode), burst_len,
            dst_stride, offset_num);
    }
}

__aicore__ inline void asc_gather_gm2ub(
    __ubuf__ uint64_t* dst, __gm__ uint64_t* src, uint8_t offset_type, bool enable_constant_pad,
    __ubuf__ void* offset_addr, asc_load_l2_cache_mode l2_cache_mode, uint32_t burst_len, uint32_t dst_stride,
    uint32_t offset_num)
{
    if ASC_IS_AIV {
        gather_gm_to_ubuf(
            dst, src, 0, offset_type, enable_constant_pad, offset_addr, static_cast<uint8_t>(l2_cache_mode), burst_len,
            dst_stride, offset_num);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad, false,
            static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, false);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, enable_retry);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, false, static_cast<asc_load_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad, false,
            static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, false);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, enable_retry);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, false, static_cast<asc_load_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad, false,
            static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, false);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, enable_retry);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, false, static_cast<asc_load_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad, false,
            static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, false);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, enable_retry);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, false, static_cast<asc_load_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad, false,
            static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, false);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, enable_retry);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, false, static_cast<asc_load_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad, false,
            static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, false);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, enable_retry);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, false, static_cast<asc_load_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ half* dst, __gm__ half* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad, false,
            static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, false);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ half* dst, __gm__ half* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, enable_retry);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ half* dst, __gm__ half* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, false, static_cast<asc_load_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ float* dst, __gm__ float* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride,
    uint32_t dst_stride)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad, false,
            static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, false);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ float* dst, __gm__ float* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_num,
    uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, enable_retry);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ float* dst, __gm__ float* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, false, static_cast<asc_load_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode,
    uint64_t src_stride, uint32_t dst_stride)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad, false,
            static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, false);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, enable_retry);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, false, static_cast<asc_load_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode,
    uint64_t src_stride, uint32_t dst_stride)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad, false,
            static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, false);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, enable_retry);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, false, static_cast<asc_load_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode,
    uint64_t src_stride, uint32_t dst_stride)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad, false,
            static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, false);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, enable_retry);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, false, static_cast<asc_load_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode,
    uint64_t src_stride, uint32_t dst_stride)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad, false,
            static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, false);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, enable_retry);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, false, static_cast<asc_load_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ fp8_e8m0_t* dst, __gm__ fp8_e8m0_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, enable_retry);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ fp8_e6m2_t* dst, __gm__ fp8_e6m2_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, enable_retry);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ fp8_e8m0_t* dst, __gm__ fp8_e8m0_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode,
    uint64_t src_stride, uint32_t dst_stride)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad, false,
            static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, false);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ fp8_e8m0_t* dst, __gm__ fp8_e8m0_t* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, false, static_cast<asc_load_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_gm2ub_align(
    __ubuf__ fp8_e6m2_t* dst, __gm__ fp8_e6m2_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_num, uint8_t right_padding_num, bool enable_constant_pad, asc_load_l2_cache_mode l2_cache_mode,
    uint64_t src_stride, uint32_t dst_stride)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_num, right_padding_num, enable_constant_pad, false,
            static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride, false);
    }
}

__aicore__ inline void asc_copy_gm2ub_align(__ubuf__ fp8_e6m2_t* dst, __gm__ fp8_e6m2_t* src, uint32_t size)
{
    asc_copy_gm2ub_align(dst, src, 1, size, 0, 0, false, static_cast<asc_load_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, false, false, 0,
            static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, false);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, static_cast<asc_store_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, false, false, 0,
            static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, false);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, static_cast<asc_store_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, false, false, 0,
            static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, false);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, static_cast<asc_store_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, false, false, 0,
            static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, false);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, static_cast<asc_store_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, false, false, 0,
            static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, false);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, static_cast<asc_store_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, false, false, 0,
            static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, false);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, static_cast<asc_store_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ half* dst, __ubuf__ half* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, false, false, 0,
            static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, false);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ half* dst, __ubuf__ half* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, static_cast<asc_store_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ float* dst, __ubuf__ float* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, false, false, 0,
            static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, false);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ float* dst, __ubuf__ float* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, static_cast<asc_store_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, false, false, 0,
            static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, false);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, static_cast<asc_store_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, false, false, 0,
            static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, false);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, static_cast<asc_store_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, false, false, 0,
            static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, false);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, static_cast<asc_store_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, false, false, 0,
            static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, false);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, static_cast<asc_store_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b8(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, false, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b8(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, false, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp4x2_e2m1_t* dst, __gm__ fp4x2_e2m1_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b8(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, false, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp4x2_e1m2_t* dst, __gm__ fp4x2_e1m2_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b8(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, false, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp8_e8m0_t* dst, __gm__ fp8_e8m0_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b8(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, false, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b8(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, false, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b8(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, false, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp8_e6m2_t* dst, __gm__ fp8_e6m2_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b8(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, false, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b16(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, false, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b16(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, false, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ half* dst, __gm__ half* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b16(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, false, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b16(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, false, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b32(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, false, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b32(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, false, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ float* dst, __gm__ float* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b32(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, false, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b8(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, enable_pre_load_ignore_sync,
            static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b8(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, enable_pre_load_ignore_sync,
            static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp4x2_e2m1_t* dst, __gm__ fp4x2_e2m1_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b8(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, enable_pre_load_ignore_sync,
            static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp4x2_e1m2_t* dst, __gm__ fp4x2_e1m2_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b8(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, enable_pre_load_ignore_sync,
            static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp8_e8m0_t* dst, __gm__ fp8_e8m0_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b8(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, enable_pre_load_ignore_sync,
            static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b8(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, enable_pre_load_ignore_sync,
            static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b8(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, enable_pre_load_ignore_sync,
            static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ fp8_e6m2_t* dst, __gm__ fp8_e6m2_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b8(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, enable_pre_load_ignore_sync,
            static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ int16_t* dst, __gm__ int16_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b16(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, enable_pre_load_ignore_sync,
            static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b16(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, enable_pre_load_ignore_sync,
            static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ half* dst, __gm__ half* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b16(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, enable_pre_load_ignore_sync,
            static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b16(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, enable_pre_load_ignore_sync,
            static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ int32_t* dst, __gm__ int32_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b32(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, enable_pre_load_ignore_sync,
            static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b32(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, enable_pre_load_ignore_sync,
            static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ float* dst, __gm__ float* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b32(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, enable_pre_load_ignore_sync,
            static_cast<uint8_t>(l2_cache_mode));
    }
}

// asc_remote_copy_ub2gm_align data types: uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float,
// hifloat8_t, bfloat16_t, fp8_e5m2_t, fp8_e4m3fn_t, fp8_e8m0_t, fp8_e6m2_t
__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ half* dst, __ubuf__ half* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ float* dst, __ubuf__ float* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint32_t burst_count, uint32_t burst_len,
    bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode,
    uint64_t dst_stride, uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ fp8_e8m0_t* dst, __ubuf__ fp8_e8m0_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm_align(
    __gm__ fp8_e6m2_t* dst, __ubuf__ fp8_e6m2_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, enable_retry);
    }
}

// asc_remote_copy_ub2gm data types: uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float,
// hifloat8_t, bfloat16_t, fp8_e5m2_t, fp8_e4m3fn_t, fp8_e8m0_t, fp8_e6m2_t
__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, 0, dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ uint8_t* dst, __ubuf__ uint8_t* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry)
{
    asc_remote_copy_ub2gm(
        dst, src, 1, size, enable_rsw_ctrl, enable_rsw_packet_ctrl, rsw_buffset_size, 0, 0, enable_retry);
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, 0, dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ int8_t* dst, __ubuf__ int8_t* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry)
{
    asc_remote_copy_ub2gm(
        dst, src, 1, size, enable_rsw_ctrl, enable_rsw_packet_ctrl, rsw_buffset_size, 0, 0, enable_retry);
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, 0, dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ uint16_t* dst, __ubuf__ uint16_t* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry)
{
    asc_remote_copy_ub2gm(
        dst, src, 1, size, enable_rsw_ctrl, enable_rsw_packet_ctrl, rsw_buffset_size, 0, 0, enable_retry);
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, 0, dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ int16_t* dst, __ubuf__ int16_t* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry)
{
    asc_remote_copy_ub2gm(
        dst, src, 1, size, enable_rsw_ctrl, enable_rsw_packet_ctrl, rsw_buffset_size, 0, 0, enable_retry);
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, 0, dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ uint32_t* dst, __ubuf__ uint32_t* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry)
{
    asc_remote_copy_ub2gm(
        dst, src, 1, size, enable_rsw_ctrl, enable_rsw_packet_ctrl, rsw_buffset_size, 0, 0, enable_retry);
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, 0, dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ int32_t* dst, __ubuf__ int32_t* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry)
{
    asc_remote_copy_ub2gm(
        dst, src, 1, size, enable_rsw_ctrl, enable_rsw_packet_ctrl, rsw_buffset_size, 0, 0, enable_retry);
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ half* dst, __ubuf__ half* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, 0, dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ half* dst, __ubuf__ half* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry)
{
    asc_remote_copy_ub2gm(
        dst, src, 1, size, enable_rsw_ctrl, enable_rsw_packet_ctrl, rsw_buffset_size, 0, 0, enable_retry);
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ float* dst, __ubuf__ float* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, 0, dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ float* dst, __ubuf__ float* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry)
{
    asc_remote_copy_ub2gm(
        dst, src, 1, size, enable_rsw_ctrl, enable_rsw_packet_ctrl, rsw_buffset_size, 0, 0, enable_retry);
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, 0, dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ hifloat8_t* dst, __ubuf__ hifloat8_t* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry)
{
    asc_remote_copy_ub2gm(
        dst, src, 1, size, enable_rsw_ctrl, enable_rsw_packet_ctrl, rsw_buffset_size, 0, 0, enable_retry);
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, 0, dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ bfloat16_t* dst, __ubuf__ bfloat16_t* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry)
{
    asc_remote_copy_ub2gm(
        dst, src, 1, size, enable_rsw_ctrl, enable_rsw_packet_ctrl, rsw_buffset_size, 0, 0, enable_retry);
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, 0, dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ fp8_e5m2_t* dst, __ubuf__ fp8_e5m2_t* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry)
{
    asc_remote_copy_ub2gm(
        dst, src, 1, size, enable_rsw_ctrl, enable_rsw_packet_ctrl, rsw_buffset_size, 0, 0, enable_retry);
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint32_t burst_count, uint32_t burst_len,
    bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride,
    uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, 0, dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ fp8_e4m3fn_t* dst, __ubuf__ fp8_e4m3fn_t* src, uint32_t size, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, bool enable_retry)
{
    asc_remote_copy_ub2gm(
        dst, src, 1, size, enable_rsw_ctrl, enable_rsw_packet_ctrl, rsw_buffset_size, 0, 0, enable_retry);
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ fp8_e8m0_t* dst, __ubuf__ fp8_e8m0_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, 0, dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ fp8_e8m0_t* dst, __ubuf__ fp8_e8m0_t* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry)
{
    asc_remote_copy_ub2gm(
        dst, src, 1, size, enable_rsw_ctrl, enable_rsw_packet_ctrl, rsw_buffset_size, 0, 0, enable_retry);
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ fp8_e6m2_t* dst, __ubuf__ fp8_e6m2_t* src, uint32_t burst_count, uint32_t burst_len, bool enable_rsw_ctrl,
    bool enable_rsw_packet_ctrl, uint8_t rsw_buffset_size, uint64_t dst_stride, uint32_t src_stride, bool enable_retry)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, enable_rsw_ctrl, enable_rsw_packet_ctrl,
            rsw_buffset_size, 0, dst_stride, src_stride, enable_retry);
    }
}

__aicore__ inline void asc_remote_copy_ub2gm(
    __gm__ fp8_e6m2_t* dst, __ubuf__ fp8_e6m2_t* src, uint32_t size, bool enable_rsw_ctrl, bool enable_rsw_packet_ctrl,
    uint8_t rsw_buffset_size, bool enable_retry)
{
    asc_remote_copy_ub2gm(
        dst, src, 1, size, enable_rsw_ctrl, enable_rsw_packet_ctrl, rsw_buffset_size, 0, 0, enable_retry);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ fp8_e8m0_t* dst, __ubuf__ fp8_e8m0_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, false, false, 0,
            static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, false);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ fp8_e8m0_t* dst, __ubuf__ fp8_e8m0_t* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, static_cast<asc_store_l2_cache_mode>(0), 0, 0);
}

__aicore__ inline void asc_copy_ub2gm_align(
    __gm__ fp8_e6m2_t* dst, __ubuf__ fp8_e6m2_t* src, uint32_t burst_count, uint32_t burst_len,
    asc_store_l2_cache_mode l2_cache_mode, uint64_t dst_stride, uint32_t src_stride)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, burst_count, burst_len, false, false, 0,
            static_cast<uint8_t>(l2_cache_mode), dst_stride, src_stride, false);
    }
}

__aicore__ inline void asc_copy_ub2gm_align(__gm__ fp8_e6m2_t* dst, __ubuf__ fp8_e6m2_t* src, uint32_t size)
{
    asc_copy_ub2gm_align(dst, src, 1, size, static_cast<asc_store_l2_cache_mode>(0), 0, 0);
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
