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
    "impl/c_api/reg_base_impl/vector_datamove_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_VECTOR_DATAMOVE_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_VECTOR_DATAMOVE_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/reg_base_impl/npu_arch_3510/vector_datamove_intf_impl.h"
#endif

__aicore__ inline void asc_copy_gm2ub(
    __ubuf__ void* dst, __gm__ void* src, uint16_t burst_count, uint16_t burst_len, uint16_t src_stride,
    uint16_t dst_stride)
{
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ uint8_t*)dst, (__gm__ uint8_t*)src, 0, burst_count, burst_len, 0, 0, false, 0, src_stride,
            dst_stride);
    }
}

__aicore__ inline void asc_copy_gm2ub(__ubuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2ub(dst, src, 1, size, 0, 0);
}

__aicore__ inline void asc_copy_ub2gm(
    __gm__ void* dst, __ubuf__ void* src, uint16_t burst_count, uint16_t burst_len, uint16_t dst_stride,
    uint16_t src_stride)
{
    if ASC_IS_AIV {
        copy_ubuf_to_gm_align_v2(dst, src, 0, burst_count, burst_len, 0, dst_stride, src_stride);
    }
}

__aicore__ inline void asc_copy_ub2gm(__gm__ void* dst, __ubuf__ void* src, uint32_t size)
{
    asc_copy_ub2gm(dst, src, 1, size, 0, 0);
}

__aicore__ inline void asc_copy_ub2l1(
    __cbuf__ void* dst, __ubuf__ void* src, uint16_t burst_count, uint16_t burst_len, uint16_t src_gap,
    uint16_t dst_gap)
{
    if ASC_IS_AIV {
        copy_ubuf_to_cbuf(dst, src, 0, burst_count, burst_len, src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_ub2l1(__cbuf__ void* dst, __ubuf__ void* src, uint32_t size)
{
    asc_copy_ub2l1(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
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

__aicore__ inline void asc_set_ub2gm_loop_size(uint32_t loop1_size, uint32_t loop2_size)
{
    if ASC_IS_AIV {
        uint64_t config = static_cast<uint64_t>(loop2_size) << 21;
        config |= static_cast<uint64_t>(loop1_size);
        set_loop_size_ubtoout(config);
    }
}

__aicore__ inline void asc_set_ub2gm_loop1_stride(uint64_t loop1_src_stride, uint64_t loop1_dst_stride)
{
    if ASC_IS_AIV {
        uint64_t config = loop1_src_stride << 40;
        config |= loop1_dst_stride;
        set_loop1_stride_ubtoout(config);
    }
}

__aicore__ inline void asc_set_ub2gm_loop2_stride(uint64_t loop2_src_stride, uint64_t loop2_dst_stride)
{
    if ASC_IS_AIV {
        uint64_t config = loop2_src_stride << 40;
        config |= loop2_dst_stride;
        set_loop2_stride_ubtoout(config);
    }
}

__aicore__ inline void asc_set_gm2ub_loop_size(uint64_t loop1_size, uint64_t loop2_size)
{
    if ASC_IS_AIV {
        asc_gm2ub_loop_size_config config;
        config.loop1_size = loop1_size;
        config.loop2_size = loop2_size;
        set_loop_size_outtoub(config.config);
    }
}

__aicore__ inline void asc_set_gm2ub_loop1_stride(uint64_t loop1_src_stride, uint64_t loop1_dst_stride)
{
    if ASC_IS_AIV {
        asc_gm2ub_loop_stride_config config;
        config.src_stride = loop1_src_stride;
        config.dst_stride = loop1_dst_stride;
        set_loop1_stride_outtoub(config.config);
    }
}

__aicore__ inline void asc_set_gm2ub_loop2_stride(uint64_t loop2_src_stride, uint64_t loop2_dst_stride)
{
    if ASC_IS_AIV {
        asc_gm2ub_loop_stride_config config;
        config.src_stride = loop2_src_stride;
        config.dst_stride = loop2_dst_stride;
        set_loop2_stride_outtoub(config.config);
    }
}

__aicore__ inline void asc_set_ndim_loop0_stride(uint64_t dst_stride, uint64_t src_stride)
{
    if ASC_IS_AIV {
        uint64_t config = (src_stride << 20) | (dst_stride & 0xfffff);
        set_loop0_stride_nddma(config);
    }
}

__aicore__ inline void asc_set_ndim_loop1_stride(uint64_t dst_stride, uint64_t src_stride)
{
    if ASC_IS_AIV {
        uint64_t config = (src_stride << 20) | (dst_stride & 0xfffff);
        set_loop1_stride_nddma(config);
    }
}

__aicore__ inline void asc_set_ndim_loop2_stride(uint64_t dst_stride, uint64_t src_stride)
{
    if ASC_IS_AIV {
        uint64_t config = (src_stride << 20) | (dst_stride & 0xfffff);
        set_loop2_stride_nddma(config);
    }
}

__aicore__ inline void asc_set_ndim_loop3_stride(uint64_t dst_stride, uint64_t src_stride)
{
    if ASC_IS_AIV {
        uint64_t config = (src_stride << 20) | (dst_stride & 0xfffff);
        set_loop3_stride_nddma(config);
    }
}

__aicore__ inline void asc_set_ndim_loop4_stride(uint64_t dst_stride, uint64_t src_stride)
{
    if ASC_IS_AIV {
        uint64_t config = (src_stride << 20) | (dst_stride & 0xfffff);
        set_loop4_stride_nddma(config);
    }
}

__aicore__ inline void asc_set_ndim_pad_count(asc_ndim_pad_count_config& config)
{
    if ASC_IS_AIV {
        set_pad_cnt_nddma(config.config);
    }
}

__aicore__ inline void asc_set_ndim_pad_value(int8_t pad_value)
{
    if ASC_IS_AIV {
        return set_pad_val_nddma(*reinterpret_cast<uint8_t*>(&pad_value));
    }
}

__aicore__ inline void asc_set_ndim_pad_value(uint8_t pad_value)
{
    if ASC_IS_AIV {
        return set_pad_val_nddma(*reinterpret_cast<uint8_t*>(&pad_value));
    }
}

__aicore__ inline void asc_set_ndim_pad_value(int16_t pad_value)
{
    if ASC_IS_AIV {
        return set_pad_val_nddma(*reinterpret_cast<uint16_t*>(&pad_value));
    }
}

__aicore__ inline void asc_set_ndim_pad_value(uint16_t pad_value)
{
    if ASC_IS_AIV {
        return set_pad_val_nddma(*reinterpret_cast<uint16_t*>(&pad_value));
    }
}

__aicore__ inline void asc_set_ndim_pad_value(half pad_value)
{
    if ASC_IS_AIV {
        return set_pad_val_nddma(*reinterpret_cast<uint16_t*>(&pad_value));
    }
}

__aicore__ inline void asc_set_ndim_pad_value(bfloat16_t pad_value)
{
    if ASC_IS_AIV {
        return set_pad_val_nddma(*reinterpret_cast<uint16_t*>(&pad_value));
    }
}

__aicore__ inline void asc_set_ndim_pad_value(int32_t pad_value)
{
    if ASC_IS_AIV {
        return set_pad_val_nddma(*reinterpret_cast<uint32_t*>(&pad_value));
    }
}

__aicore__ inline void asc_set_ndim_pad_value(uint32_t pad_value)
{
    if ASC_IS_AIV {
        return set_pad_val_nddma(*reinterpret_cast<uint32_t*>(&pad_value));
    }
}

__aicore__ inline void asc_set_ndim_pad_value(float pad_value)
{
    if ASC_IS_AIV {
        return set_pad_val_nddma(*reinterpret_cast<uint32_t*>(&pad_value));
    }
}

__aicore__ inline void asc_ndim_copy_gm2ub(
    __ubuf__ int8_t* dst, __gm__ int8_t* src, uint32_t loop0_size, uint32_t loop1_size, uint32_t loop2_size,
    uint32_t loop3_size, uint32_t loop4_size, uint8_t loop0_lp_count, uint8_t loop0_rp_count, bool padding_mode,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIV {
        nddma_out_to_ub_b8(
            (__ubuf__ void*)dst, (__gm__ void*)src, 0, loop0_size, loop1_size, loop2_size, loop3_size, loop4_size,
            loop0_lp_count, loop0_rp_count, padding_mode, static_cast<uint8_t>(l2_cache_mode));
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
            loop0_lp_count, loop0_rp_count, padding_mode, static_cast<uint8_t>(l2_cache_mode));
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
            loop0_lp_count, loop0_rp_count, padding_mode, static_cast<uint8_t>(l2_cache_mode));
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
            loop0_lp_count, loop0_rp_count, padding_mode, static_cast<uint8_t>(l2_cache_mode));
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
            loop0_lp_count, loop0_rp_count, padding_mode, static_cast<uint8_t>(l2_cache_mode));
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
            loop0_lp_count, loop0_rp_count, padding_mode, static_cast<uint8_t>(l2_cache_mode));
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
            loop0_lp_count, loop0_rp_count, padding_mode, static_cast<uint8_t>(l2_cache_mode));
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
            loop0_lp_count, loop0_rp_count, padding_mode, static_cast<uint8_t>(l2_cache_mode));
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
            loop0_lp_count, loop0_rp_count, padding_mode, static_cast<uint8_t>(l2_cache_mode));
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
            loop0_lp_count, loop0_rp_count, padding_mode, static_cast<uint8_t>(l2_cache_mode));
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
            loop0_lp_count, loop0_rp_count, padding_mode, static_cast<uint8_t>(l2_cache_mode));
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
            loop0_lp_count, loop0_rp_count, padding_mode, static_cast<uint8_t>(l2_cache_mode));
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
            loop0_lp_count, loop0_rp_count, padding_mode, static_cast<uint8_t>(l2_cache_mode));
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
            loop0_lp_count, loop0_rp_count, padding_mode, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_ndim_copy_dci()
{
    if ASC_IS_AIV {
        nd_dma_dci();
    }
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
