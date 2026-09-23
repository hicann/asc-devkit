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
    "impl/c_api/memory_base_impl/cube_datamove_composite_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_MEMORY_BASE_IMPL_CUBE_DATAMOVE_COMPOSITE_INTF_IMPL_H
#define IMPL_C_API_MEMORY_BASE_IMPL_CUBE_DATAMOVE_COMPOSITE_INTF_IMPL_H

#include "impl/c_api/memory_base_impl/utils_impl.h"

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_sparse and asc_sync)
__aicore__ inline void asc_copy_l12l0b_sparse_sync(
    __cb__ int8_t* dst, __cbuf__ int8_t* src, __cbuf__ int8_t* index, uint16_t start_index, uint8_t repeat)
{
    if ASC_IS_AIC {
        asc_sparse_addr_config addr_config;
        addr_config.src = (uint64_t)src & 0xffffffff;
        addr_config.index = (uint64_t)index & 0xffffffff;
        __cbuf__ int8_t* src_addr = reinterpret_cast<__cbuf__ int8_t*>(addr_config.config);
        load_cbuf_to_cb_sp(dst, src_addr, start_index, repeat);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ int4b_t* dst, __cbuf__ int4b_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb_s4(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ int8_t* dst, __cbuf__ int8_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ half* dst, __cbuf__ half* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ int32_t* dst, __cbuf__ int32_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ float* dst, __cbuf__ float* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ half* dst, __cbuf__ half* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_cb(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_cb(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ int32_t* dst, __cbuf__ int32_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_cb(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_cb(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b and asc_sync)
__aicore__ inline void asc_copy_l12l0b_sync(
    __cb__ float* dst, __cbuf__ float* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_cb(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ int4b_t* dst, __cbuf__ int4b_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca_s4(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ int8_t* dst, __cbuf__ int8_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ half* dst, __cbuf__ half* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ int32_t* dst, __cbuf__ int32_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ float* dst, __cbuf__ float* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ int4b_t* dst, __cbuf__ int4b_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_ca_s4(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ int8_t* dst, __cbuf__ int8_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_ca(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_ca(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ half* dst, __cbuf__ half* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_ca(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_ca(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ int32_t* dst, __cbuf__ int32_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_ca(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_ca(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a and asc_sync)
__aicore__ inline void asc_copy_l12l0a_sync(
    __ca__ float* dst, __cbuf__ float* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_ca(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_image_to_cbuf and asc_sync)
__aicore__ inline void asc_load_image_to_cbuf_sync(
    __cbuf__ half* dst, uint16_t hor_size, uint16_t ver_size, uint16_t hor_start_pos, uint16_t ver_start_pos,
    uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size, uint16_t left_pad_size, uint16_t right_pad_size)
{
    if ASC_IS_AIC {
        load_image_to_cbuf(
            dst, static_cast<uint16_t>(hor_size - 1), static_cast<uint16_t>(ver_size - 1), hor_start_pos, ver_start_pos,
            static_cast<uint16_t>(src_hor_size - 1), top_pad_size, bot_pad_size, left_pad_size, right_pad_size, 0);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_load_image_to_cbuf and asc_sync)
__aicore__ inline void asc_load_image_to_cbuf_sync(
    __cbuf__ int8_t* dst, uint16_t hor_size, uint16_t ver_size, uint16_t hor_start_pos, uint16_t ver_start_pos,
    uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size, uint16_t left_pad_size, uint16_t right_pad_size)
{
    if ASC_IS_AIC {
        load_image_to_cbuf(
            dst, static_cast<uint16_t>(hor_size - 1), static_cast<uint16_t>(ver_size - 1), hor_start_pos, ver_start_pos,
            static_cast<uint16_t>(src_hor_size - 1), top_pad_size, bot_pad_size, left_pad_size, right_pad_size, 0);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
__aicore__ inline void asc_fill_l0a_sync(__ca__ half* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
__aicore__ inline void asc_fill_l0a_sync(__ca__ half* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
__aicore__ inline void asc_fill_l0a_sync(__ca__ float* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
__aicore__ inline void asc_fill_l0a_sync(__ca__ float* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
__aicore__ inline void asc_fill_l0a_sync(__ca__ int16_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
__aicore__ inline void asc_fill_l0a_sync(__ca__ int16_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
__aicore__ inline void asc_fill_l0a_sync(__ca__ int32_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
__aicore__ inline void asc_fill_l0a_sync(__ca__ int32_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
__aicore__ inline void asc_fill_l0a_sync(__ca__ uint16_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
__aicore__ inline void asc_fill_l0a_sync(__ca__ uint16_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
__aicore__ inline void asc_fill_l0a_sync(__ca__ uint32_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
__aicore__ inline void asc_fill_l0a_sync(__ca__ uint32_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
__aicore__ inline void asc_fill_l0a_sync(__ca__ bfloat16_t* dst, bfloat16_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix_bf16(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
__aicore__ inline void asc_fill_l0a_sync(__ca__ bfloat16_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix_h(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0a and asc_sync)
__aicore__ inline void asc_fill_l0a_sync(__ca__ bfloat16_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix_ui(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
__aicore__ inline void asc_fill_l0b_sync(__cb__ half* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
__aicore__ inline void asc_fill_l0b_sync(__cb__ half* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
__aicore__ inline void asc_fill_l0b_sync(__cb__ float* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
__aicore__ inline void asc_fill_l0b_sync(__cb__ float* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
__aicore__ inline void asc_fill_l0b_sync(__cb__ int16_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
__aicore__ inline void asc_fill_l0b_sync(__cb__ int16_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
__aicore__ inline void asc_fill_l0b_sync(__cb__ int32_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
__aicore__ inline void asc_fill_l0b_sync(__cb__ int32_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
__aicore__ inline void asc_fill_l0b_sync(__cb__ uint16_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
__aicore__ inline void asc_fill_l0b_sync(__cb__ uint16_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
__aicore__ inline void asc_fill_l0b_sync(__cb__ uint32_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
__aicore__ inline void asc_fill_l0b_sync(__cb__ uint32_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
__aicore__ inline void asc_fill_l0b_sync(__cb__ bfloat16_t* dst, bfloat16_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix_bf16(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
__aicore__ inline void asc_fill_l0b_sync(__cb__ bfloat16_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix_h(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l0b and asc_sync)
__aicore__ inline void asc_fill_l0b_sync(__cb__ bfloat16_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix_ui(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ half* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ half* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ float* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ float* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ int16_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ int16_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ int32_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ int32_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ uint16_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ uint16_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ uint32_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ uint32_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ bfloat16_t* dst, bfloat16_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix_bf16(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ bfloat16_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix_h(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_fill_l1 and asc_sync)
__aicore__ inline void asc_fill_l1_sync(__cbuf__ bfloat16_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix_ui(dst, config.config, value);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_trans and asc_sync)
__aicore__ inline void asc_copy_l12l0a_trans_sync(
    __ca__ half* dst, __cbuf__ half* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap,
    bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_trans and asc_sync)
__aicore__ inline void asc_copy_l12l0a_trans_sync(
    __ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_trans and asc_sync)
__aicore__ inline void asc_copy_l12l0a_trans_sync(
    __ca__ float* dst, __cbuf__ float* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap,
    bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_trans and asc_sync)
__aicore__ inline void asc_copy_l12l0a_trans_sync(
    __ca__ int32_t* dst, __cbuf__ int32_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_trans and asc_sync)
__aicore__ inline void asc_copy_l12l0a_trans_sync(
    __ca__ int8_t* dst, __cbuf__ int8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap,
    bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_trans and asc_sync)
__aicore__ inline void asc_copy_l12l0a_trans_sync(
    __ca__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0a_trans and asc_sync)
__aicore__ inline void asc_copy_l12l0a_trans_sync(
    __ca__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ half* dst, __cbuf__ half* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap,
    bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ float* dst, __cbuf__ float* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap,
    bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ int32_t* dst, __cbuf__ int32_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ int8_t* dst, __cbuf__ int8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap,
    bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0b_trans and asc_sync)
__aicore__ inline void asc_copy_l12l0b_trans_sync(
    __cb__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
__aicore__ inline void asc_copy_gm2l1_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(
            dst, src, 0, static_cast<uint16_t>(1), size / ASC_C_API_ONE_DATABLOCK_SIZE, static_cast<uint16_t>(0),
            static_cast<uint16_t>(0), PAD_NONE);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_pad1 and asc_sync)
__aicore__ inline void asc_copy_gm2l1_pad1_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(
            dst, src, 0, static_cast<uint16_t>(1), size / ASC_C_API_ONE_DATABLOCK_SIZE, static_cast<uint16_t>(0),
            static_cast<uint16_t>(0), PAD_MODE1);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_pad2 and asc_sync)
__aicore__ inline void asc_copy_gm2l1_pad2_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(
            dst, src, 0, static_cast<uint16_t>(1), size / ASC_C_API_ONE_DATABLOCK_SIZE, static_cast<uint16_t>(0),
            static_cast<uint16_t>(0), PAD_MODE2);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_pad3 and asc_sync)
__aicore__ inline void asc_copy_gm2l1_pad3_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(
            dst, src, 0, static_cast<uint16_t>(1), size / ASC_C_API_ONE_DATABLOCK_SIZE, static_cast<uint16_t>(0),
            static_cast<uint16_t>(0), PAD_MODE3);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_pad4 and asc_sync)
__aicore__ inline void asc_copy_gm2l1_pad4_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(
            dst, src, 0, static_cast<uint16_t>(1), size / ASC_C_API_ONE_DATABLOCK_SIZE, static_cast<uint16_t>(0),
            static_cast<uint16_t>(0), PAD_MODE4);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_pad5 and asc_sync)
__aicore__ inline void asc_copy_gm2l1_pad5_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(
            dst, src, 0, static_cast<uint16_t>(1), size / ASC_C_API_ONE_DATABLOCK_SIZE, static_cast<uint16_t>(0),
            static_cast<uint16_t>(0), PAD_MODE5);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_pad6 and asc_sync)
__aicore__ inline void asc_copy_gm2l1_pad6_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(
            dst, src, 0, static_cast<uint16_t>(1), size / ASC_C_API_ONE_DATABLOCK_SIZE, static_cast<uint16_t>(0),
            static_cast<uint16_t>(0), PAD_MODE6);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_pad7 and asc_sync)
__aicore__ inline void asc_copy_gm2l1_pad7_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(
            dst, src, 0, static_cast<uint16_t>(1), size / ASC_C_API_ONE_DATABLOCK_SIZE, static_cast<uint16_t>(0),
            static_cast<uint16_t>(0), PAD_MODE7);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_pad8 and asc_sync)
__aicore__ inline void asc_copy_gm2l1_pad8_sync(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(
            dst, src, 0, static_cast<uint16_t>(1), size / ASC_C_API_ONE_DATABLOCK_SIZE, static_cast<uint16_t>(0),
            static_cast<uint16_t>(0), PAD_MODE8);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ int8_t* dst, __gm__ int8_t* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz_b8(
            dst, src, 0, nd_num, n_value, d_value, src_nd_matrix_stride, src_d_value, dst_nz_c0_stride, dst_nz_n_stride,
            dst_nz_matrix_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz_b8(
            dst, src, 0, nd_num, n_value, d_value, src_nd_matrix_stride, src_d_value, dst_nz_c0_stride, dst_nz_n_stride,
            dst_nz_matrix_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz_b16(
            dst, src, 0, nd_num, n_value, d_value, src_nd_matrix_stride, src_d_value, dst_nz_c0_stride, dst_nz_n_stride,
            dst_nz_matrix_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ half* dst, __gm__ half* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz_b16(
            dst, src, 0, nd_num, n_value, d_value, src_nd_matrix_stride, src_d_value, dst_nz_c0_stride, dst_nz_n_stride,
            dst_nz_matrix_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ int16_t* dst, __gm__ int16_t* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz_b16(
            dst, src, 0, nd_num, n_value, d_value, src_nd_matrix_stride, src_d_value, dst_nz_c0_stride, dst_nz_n_stride,
            dst_nz_matrix_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ float* dst, __gm__ float* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz_b32s(
            dst, src, 0, nd_num, n_value, d_value, src_nd_matrix_stride, src_d_value, dst_nz_c0_stride, dst_nz_n_stride,
            dst_nz_matrix_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ int32_t* dst, __gm__ int32_t* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz_b32s(
            dst, src, 0, nd_num, n_value, d_value, src_nd_matrix_stride, src_d_value, dst_nz_c0_stride, dst_nz_n_stride,
            dst_nz_matrix_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1_nd2nz and asc_sync)
__aicore__ inline void asc_copy_gm2l1_nd2nz_sync(
    __cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz_b32s(
            dst, src, 0, nd_num, n_value, d_value, src_nd_matrix_stride, src_d_value, dst_nz_c0_stride, dst_nz_n_stride,
            dst_nz_matrix_stride);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12bt and asc_sync)
__aicore__ inline void asc_copy_l12bt_sync(uint64_t dst, __cbuf__ void* src, uint32_t size)
{
    if ASC_IS_AIC {
        copy_cbuf_to_bt(
            dst, src, static_cast<uint16_t>(0), static_cast<uint16_t>(1), size / ASC_C_API_ONE_DATABLOCK_SIZE,
            static_cast<uint16_t>(0), static_cast<uint16_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12fb and asc_sync)
__aicore__ inline void asc_copy_l12fb_sync(__fbuf__ void* dst, __cbuf__ void* src, uint32_t size)
{
    if ASC_IS_AIC {
        copy_cbuf_to_fbuf(
            dst, src, static_cast<uint16_t>(1), size / ASC_C_API_ONE_DATABLOCK_SIZE, static_cast<uint16_t>(0),
            static_cast<uint16_t>(0));
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ half* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride_dst_d, src_stride, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ bfloat16_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride_dst_d, src_stride, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ int8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride_dst_d, src_stride, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ uint8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride_dst_d, src_stride, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ float* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride_dst_d, src_stride, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ half* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride_dst_d, src_stride, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ int16_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride_dst_d, src_stride, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride_dst_d, src_stride, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2gm and asc_sync)
__aicore__ inline void asc_copy_l0c2gm_sync(
    __gm__ int32_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride_dst_d, src_stride, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2l1 and asc_sync)
__aicore__ inline void asc_copy_l0c2l1_sync(
    __cbuf__ half* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, unit_flag_mode, static_cast<QuantMode_t>(quant_pre),
            relu_pre, enable_channel_split, enable_nz2nd);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2l1 and asc_sync)
__aicore__ inline void asc_copy_l0c2l1_sync(
    __cbuf__ bfloat16_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split,
    bool enable_nz2nd)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, unit_flag_mode, static_cast<QuantMode_t>(quant_pre),
            relu_pre, enable_channel_split, enable_nz2nd);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2l1 and asc_sync)
__aicore__ inline void asc_copy_l0c2l1_sync(
    __cbuf__ int8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, unit_flag_mode, static_cast<QuantMode_t>(quant_pre),
            relu_pre, enable_channel_split, enable_nz2nd);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2l1 and asc_sync)
__aicore__ inline void asc_copy_l0c2l1_sync(
    __cbuf__ half* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, unit_flag_mode, static_cast<QuantMode_t>(quant_pre),
            relu_pre, enable_channel_split, enable_nz2nd);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2l1 and asc_sync)
__aicore__ inline void asc_copy_l0c2l1_sync(
    __cbuf__ int16_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split,
    bool enable_nz2nd)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, unit_flag_mode, static_cast<QuantMode_t>(quant_pre),
            relu_pre, enable_channel_split, enable_nz2nd);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2l1 and asc_sync)
__aicore__ inline void asc_copy_l0c2l1_sync(
    __cbuf__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split,
    bool enable_nz2nd)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, unit_flag_mode, static_cast<QuantMode_t>(quant_pre),
            relu_pre, enable_channel_split, enable_nz2nd);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2l1 and asc_sync)
__aicore__ inline void asc_copy_l0c2l1_sync(
    __cbuf__ uint8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split,
    bool enable_nz2nd)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, unit_flag_mode, static_cast<QuantMode_t>(quant_pre),
            relu_pre, enable_channel_split, enable_nz2nd);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2l1 and asc_sync)
__aicore__ inline void asc_copy_l0c2l1_sync(
    __cbuf__ void* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf_b4(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, unit_flag_mode, static_cast<QuantMode_t>(quant_pre),
            relu_pre, enable_channel_split, enable_nz2nd);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l0c2l1 and asc_sync)
__aicore__ inline void asc_copy_l0c2l1_sync(
    __cbuf__ void* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf_b4(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, unit_flag_mode, static_cast<QuantMode_t>(quant_pre),
            relu_pre, enable_channel_split, enable_nz2nd);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0a and asc_sync)
__aicore__ inline void asc_copy_gm2l0a_sync(
    __ca__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_ca(dst, src, base_idx, repeat, src_stride, dst_gap, 0, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0a and asc_sync)
__aicore__ inline void asc_copy_gm2l0a_sync(
    __ca__ half* dst, __gm__ half* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_ca(dst, src, base_idx, repeat, src_stride, dst_gap, 0, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0a and asc_sync)
__aicore__ inline void asc_copy_gm2l0a_sync(
    __ca__ float* dst, __gm__ float* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_ca(dst, src, base_idx, repeat, src_stride, dst_gap, 0, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0a and asc_sync)
__aicore__ inline void asc_copy_gm2l0a_sync(
    __ca__ int32_t* dst, __gm__ int32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_ca(dst, src, base_idx, repeat, src_stride, dst_gap, 0, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0a and asc_sync)
__aicore__ inline void asc_copy_gm2l0a_sync(
    __ca__ uint8_t* dst, __gm__ uint8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_ca(dst, src, base_idx, repeat, src_stride, dst_gap, 0, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0a and asc_sync)
__aicore__ inline void asc_copy_gm2l0a_sync(
    __ca__ int8_t* dst, __gm__ int8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_ca(dst, src, base_idx, repeat, src_stride, dst_gap, 0, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0a and asc_sync)
__aicore__ inline void asc_copy_gm2l0a_sync(
    __ca__ uint32_t* dst, __gm__ uint32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_ca(dst, src, base_idx, repeat, src_stride, dst_gap, 0, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0a and asc_sync)
__aicore__ inline void asc_copy_gm2l0a_sync(
    __ca__ int4b_t* dst, __gm__ int4b_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_ca_s4((__ca__ void*)dst, (__gm__ void*)src, base_idx, repeat, src_stride, dst_gap, 0, inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12gm and asc_sync)
__aicore__ inline void asc_copy_l12gm_sync(
    __gm__ void* dst, __cbuf__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_cbuf_to_gm(dst, src, 0, n_burst, len_burst, src_gap, dst_gap);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0c and asc_sync)
__aicore__ inline void asc_copy_l12l0c_sync(
    __cc__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_matrix_cbuf_to_cc(dst, src, n_burst, len_burst, src_gap, dst_gap);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0c and asc_sync)
__aicore__ inline void asc_copy_l12l0c_sync(
    __cc__ half* dst, __cbuf__ half* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_matrix_cbuf_to_cc(dst, src, n_burst, len_burst, src_gap, dst_gap);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0c and asc_sync)
__aicore__ inline void asc_copy_l12l0c_sync(
    __cc__ half* dst, __cbuf__ float* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_matrix_cbuf_to_cc(dst, src, n_burst, len_burst, src_gap, dst_gap);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0c and asc_sync)
__aicore__ inline void asc_copy_l12l0c_sync(
    __cc__ bfloat16_t* dst, __cbuf__ float* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_matrix_cbuf_to_cc(dst, src, n_burst, len_burst, src_gap, dst_gap);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0c and asc_sync)
__aicore__ inline void asc_copy_l12l0c_sync(
    __cc__ float* dst, __cbuf__ float* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_matrix_cbuf_to_cc(dst, src, n_burst, len_burst, src_gap, dst_gap);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0c and asc_sync)
__aicore__ inline void asc_copy_l12l0c_sync(
    __cc__ int32_t* dst, __cbuf__ int32_t* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_matrix_cbuf_to_cc(dst, src, n_burst, len_burst, src_gap, dst_gap);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_l12l0c and asc_sync)
__aicore__ inline void asc_copy_l12l0c_sync(
    __cc__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_matrix_cbuf_to_cc(dst, src, n_burst, len_burst, src_gap, dst_gap);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0b and asc_sync)
__aicore__ inline void asc_copy_gm2l0b_sync(
    __cb__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cb(dst, src, base_idx, repeat, src_stride, dst_gap, 0, addr_cal_mode_t::inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0b and asc_sync)
__aicore__ inline void asc_copy_gm2l0b_sync(
    __cb__ half* dst, __gm__ half* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cb(dst, src, base_idx, repeat, src_stride, dst_gap, 0, addr_cal_mode_t::inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0b and asc_sync)
__aicore__ inline void asc_copy_gm2l0b_sync(
    __cb__ float* dst, __gm__ float* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cb(dst, src, base_idx, repeat, src_stride, dst_gap, 0, addr_cal_mode_t::inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0b and asc_sync)
__aicore__ inline void asc_copy_gm2l0b_sync(
    __cb__ int32_t* dst, __gm__ int32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cb(dst, src, base_idx, repeat, src_stride, dst_gap, 0, addr_cal_mode_t::inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0b and asc_sync)
__aicore__ inline void asc_copy_gm2l0b_sync(
    __cb__ int8_t* dst, __gm__ int8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cb(dst, src, base_idx, repeat, src_stride, dst_gap, 0, addr_cal_mode_t::inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0b and asc_sync)
__aicore__ inline void asc_copy_gm2l0b_sync(
    __cb__ uint32_t* dst, __gm__ uint32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cb(dst, src, base_idx, repeat, src_stride, dst_gap, 0, addr_cal_mode_t::inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0b and asc_sync)
__aicore__ inline void asc_copy_gm2l0b_sync(
    __cb__ uint8_t* dst, __gm__ uint8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cb(dst, src, base_idx, repeat, src_stride, dst_gap, 0, addr_cal_mode_t::inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l0b and asc_sync)
__aicore__ inline void asc_copy_gm2l0b_sync(
    __cb__ int4b_t* dst, __gm__ int4b_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cb_s4(dst, src, base_idx, repeat, src_stride, dst_gap, 0, addr_cal_mode_t::inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf(dst, src, base_idx, repeat, src_stride, dst_gap, static_cast<uint8_t>(0), addr_cal_mode_t::inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ half* dst, __gm__ half* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf(dst, src, base_idx, repeat, src_stride, dst_gap, static_cast<uint8_t>(0), addr_cal_mode_t::inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ float* dst, __gm__ float* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf(dst, src, base_idx, repeat, src_stride, dst_gap, static_cast<uint8_t>(0), addr_cal_mode_t::inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ int32_t* dst, __gm__ int32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf(dst, src, base_idx, repeat, src_stride, dst_gap, static_cast<uint8_t>(0), addr_cal_mode_t::inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ int8_t* dst, __gm__ int8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf(dst, src, base_idx, repeat, src_stride, dst_gap, static_cast<uint8_t>(0), addr_cal_mode_t::inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf(dst, src, base_idx, repeat, src_stride, dst_gap, static_cast<uint8_t>(0), addr_cal_mode_t::inc);
        asc_sync_post_process();
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_copy_gm2l1 and asc_sync)
__aicore__ inline void asc_copy_gm2l1_sync(
    __cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf(dst, src, base_idx, repeat, src_stride, dst_gap, static_cast<uint8_t>(0), addr_cal_mode_t::inc);
        asc_sync_post_process();
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
