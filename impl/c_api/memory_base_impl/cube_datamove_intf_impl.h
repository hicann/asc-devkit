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
    "impl/c_api/memory_base_impl/cube_datamove_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_MEMORY_BASE_IMPL_CUBE_DATAMOVE_INTF_IMPL_H
#define IMPL_C_API_MEMORY_BASE_IMPL_CUBE_DATAMOVE_INTF_IMPL_H

#include "impl/c_api/memory_base_impl/utils_impl.h"

__aicore__ inline void asc_copy_l12l0b_sparse(
    __cb__ int8_t* dst, __cbuf__ int8_t* src, __cbuf__ int8_t* index, uint16_t start_index, uint8_t repeat)
{
    if ASC_IS_AIC {
        asc_sparse_addr_config addr_config;
        addr_config.src = (uint64_t)src & 0xffffffff;
        addr_config.index = (uint64_t)index & 0xffffffff;
        __cbuf__ int8_t* src_addr = reinterpret_cast<__cbuf__ int8_t*>(addr_config.config);
        load_cbuf_to_cb_sp(dst, src_addr, start_index, repeat);
    }
}

__aicore__ inline void asc_copy_l12bt(
    uint64_t dst, __cbuf__ void* src, uint16_t conv_control, uint16_t n_burst, uint16_t len_burst, uint16_t source_gap,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_cbuf_to_bt(dst, src, conv_control, n_burst, len_burst, source_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_l12bt(uint64_t dst, __cbuf__ void* src, uint32_t size)
{
    asc_copy_l12bt(dst, src, 0, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void set_l0c_copy_params_impl(uint16_t nd_num, uint16_t src_nd_stride, uint16_t dst_nd_stride)
{
    if ASC_IS_AIC {
        asc_set_l0c_copy_params_config config;
        config.nd_num = nd_num;
        config.src_nd_stride = src_nd_stride;
        config.dst_nd_stride = dst_nd_stride;
        set_nd_para(config.config);
    }
}

__aicore__ inline void asc_copy_l12l0b_trans(
    __cb__ half* dst, __cbuf__ half* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap,
    bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
    }
}

__aicore__ inline void asc_copy_l12l0b_trans(
    __cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
    }
}

__aicore__ inline void asc_copy_l12l0b_trans(
    __cb__ float* dst, __cbuf__ float* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap,
    bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
    }
}

__aicore__ inline void asc_copy_l12l0b_trans(
    __cb__ int32_t* dst, __cbuf__ int32_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
    }
}

__aicore__ inline void asc_copy_l12l0b_trans(
    __cb__ int8_t* dst, __cbuf__ int8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap,
    bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
    }
}

__aicore__ inline void asc_copy_l12l0b_trans(
    __cb__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
    }
}

__aicore__ inline void asc_copy_l12l0b_trans(
    __cb__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
    }
}

__aicore__ inline void asc_copy_l12l0a_trans(
    __ca__ half* dst, __cbuf__ half* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap,
    bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
    }
}

__aicore__ inline void asc_copy_l12l0a_trans(
    __ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
    }
}

__aicore__ inline void asc_copy_l12l0a_trans(
    __ca__ float* dst, __cbuf__ float* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap,
    bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
    }
}

__aicore__ inline void asc_copy_l12l0a_trans(
    __ca__ int32_t* dst, __cbuf__ int32_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
    }
}

__aicore__ inline void asc_copy_l12l0a_trans(
    __ca__ int8_t* dst, __cbuf__ int8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap,
    bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
    }
}

__aicore__ inline void asc_copy_l12l0a_trans(
    __ca__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
    }
}

__aicore__ inline void asc_copy_l12l0a_trans(
    __ca__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t index_id, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap, bool enable_addr_decrement, uint16_t dst_frac_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca_transpose(dst, src, index_id, repeat, src_stride, dst_gap, enable_addr_decrement, dst_frac_gap);
    }
}

__aicore__ inline void asc_load_image_to_cbuf(
    __cbuf__ half* dst, uint16_t hor_size, uint16_t ver_size, uint16_t hor_start_pos, uint16_t ver_start_pos,
    uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size, uint16_t left_pad_size, uint16_t right_pad_size)
{
    if ASC_IS_AIC {
        load_image_to_cbuf(
            dst, hor_size, ver_size, hor_start_pos, ver_start_pos, src_hor_size, top_pad_size, bot_pad_size,
            left_pad_size, right_pad_size, 0);
    }
}

__aicore__ inline void asc_load_image_to_cbuf(
    __cbuf__ int8_t* dst, uint16_t hor_size, uint16_t ver_size, uint16_t hor_start_pos, uint16_t ver_start_pos,
    uint16_t src_hor_size, uint8_t top_pad_size, uint8_t bot_pad_size, uint16_t left_pad_size, uint16_t right_pad_size)
{
    if ASC_IS_AIC {
        load_image_to_cbuf(
            dst, hor_size, ver_size, hor_start_pos, ver_start_pos, src_hor_size, top_pad_size, bot_pad_size,
            left_pad_size, right_pad_size, 0);
    }
}

__aicore__ inline void asc_fill_l0a(__ca__ half* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0a(__ca__ half* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0a(__ca__ float* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0a(__ca__ float* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0a(__ca__ int16_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0a(__ca__ int16_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0a(__ca__ int32_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0a(__ca__ int32_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0a(__ca__ uint16_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0a(__ca__ uint16_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0a(__ca__ uint32_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0a(__ca__ uint32_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0a(__ca__ bfloat16_t* dst, bfloat16_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix_bf16(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0a(__ca__ bfloat16_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix_h(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0a(__ca__ bfloat16_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_ca_matrix_ui(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0b(__cb__ half* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0b(__cb__ half* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0b(__cb__ float* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0b(__cb__ float* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0b(__cb__ int16_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0b(__cb__ int16_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0b(__cb__ int32_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0b(__cb__ int32_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0b(__cb__ uint16_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0b(__cb__ uint16_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0b(__cb__ uint32_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0b(__cb__ uint32_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0b(__cb__ bfloat16_t* dst, bfloat16_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix_bf16(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0b(__cb__ bfloat16_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix_h(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l0b(__cb__ bfloat16_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cb_matrix_ui(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l1(__cbuf__ half* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l1(__cbuf__ half* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l1(__cbuf__ float* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l1(__cbuf__ float* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l1(__cbuf__ int16_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l1(__cbuf__ int16_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l1(__cbuf__ int32_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l1(__cbuf__ int32_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l1(__cbuf__ uint16_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l1(__cbuf__ uint16_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l1(__cbuf__ uint32_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l1(__cbuf__ uint32_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l1(__cbuf__ bfloat16_t* dst, bfloat16_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix_bf16(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l1(__cbuf__ bfloat16_t* dst, half value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix_h(dst, config.config, value);
    }
}

__aicore__ inline void asc_fill_l1(__cbuf__ bfloat16_t* dst, uint32_t value, const asc_fill_value_config& config)
{
    if ASC_IS_AIC {
        create_cbuf_matrix_ui(dst, config.config, value);
    }
}

__aicore__ inline void asc_copy_l12fb(
    __fbuf__ void* dst, __cbuf__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap_size,
    uint16_t dst_gap_size)
{
    if ASC_IS_AIC {
        copy_cbuf_to_fbuf(dst, src, n_burst, len_burst, src_gap_size, dst_gap_size);
    }
}

__aicore__ inline void asc_copy_l12fb(__fbuf__ void* dst, __cbuf__ void* src, uint32_t size)
{
    asc_copy_l12fb(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ void* dst, __gm__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(dst, src, 0, n_burst, len_burst, src_gap, dst_gap, PAD_NONE);
    }
}

__aicore__ inline void asc_copy_gm2l1(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void asc_copy_gm2l1_pad1(
    __cbuf__ void* dst, __gm__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(dst, src, 0, n_burst, len_burst, src_gap, dst_gap, PAD_MODE1);
    }
}

__aicore__ inline void asc_copy_gm2l1_pad1(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad1(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void asc_copy_gm2l1_pad2(
    __cbuf__ void* dst, __gm__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(dst, src, 0, n_burst, len_burst, src_gap, dst_gap, PAD_MODE2);
    }
}

__aicore__ inline void asc_copy_gm2l1_pad2(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad2(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void asc_copy_gm2l1_pad3(
    __cbuf__ void* dst, __gm__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(dst, src, 0, n_burst, len_burst, src_gap, dst_gap, PAD_MODE3);
    }
}

__aicore__ inline void asc_copy_gm2l1_pad3(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad3(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void asc_copy_gm2l1_pad4(
    __cbuf__ void* dst, __gm__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(dst, src, 0, n_burst, len_burst, src_gap, dst_gap, PAD_MODE4);
    }
}

__aicore__ inline void asc_copy_gm2l1_pad4(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad4(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void asc_copy_gm2l1_pad5(
    __cbuf__ void* dst, __gm__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(dst, src, 0, n_burst, len_burst, src_gap, dst_gap, PAD_MODE5);
    }
}

__aicore__ inline void asc_copy_gm2l1_pad5(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad5(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void asc_copy_gm2l1_pad6(
    __cbuf__ void* dst, __gm__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(dst, src, 0, n_burst, len_burst, src_gap, dst_gap, PAD_MODE6);
    }
}

__aicore__ inline void asc_copy_gm2l1_pad6(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad6(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void asc_copy_gm2l1_pad7(
    __cbuf__ void* dst, __gm__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(dst, src, 0, n_burst, len_burst, src_gap, dst_gap, PAD_MODE7);
    }
}

__aicore__ inline void asc_copy_gm2l1_pad7(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad7(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void asc_copy_gm2l1_pad8(
    __cbuf__ void* dst, __gm__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf(dst, src, 0, n_burst, len_burst, src_gap, dst_gap, PAD_MODE8);
    }
}

__aicore__ inline void asc_copy_gm2l1_pad8(__cbuf__ void* dst, __gm__ void* src, uint32_t size)
{
    asc_copy_gm2l1_pad8(dst, src, 1, size / ASC_C_API_ONE_DATABLOCK_SIZE, 0, 0);
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf(dst, src, base_idx, repeat, src_stride, dst_gap, static_cast<uint8_t>(0), addr_cal_mode_t::inc);
    }
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ half* dst, __gm__ half* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf(dst, src, base_idx, repeat, src_stride, dst_gap, static_cast<uint8_t>(0), addr_cal_mode_t::inc);
    }
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ float* dst, __gm__ float* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf(dst, src, base_idx, repeat, src_stride, dst_gap, static_cast<uint8_t>(0), addr_cal_mode_t::inc);
    }
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ int32_t* dst, __gm__ int32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf(dst, src, base_idx, repeat, src_stride, dst_gap, static_cast<uint8_t>(0), addr_cal_mode_t::inc);
    }
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ int8_t* dst, __gm__ int8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf(dst, src, base_idx, repeat, src_stride, dst_gap, static_cast<uint8_t>(0), addr_cal_mode_t::inc);
    }
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf(dst, src, base_idx, repeat, src_stride, dst_gap, static_cast<uint8_t>(0), addr_cal_mode_t::inc);
    }
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf(dst, src, base_idx, repeat, src_stride, dst_gap, static_cast<uint8_t>(0), addr_cal_mode_t::inc);
    }
}

__aicore__ inline void asc_copy_gm2l0a(
    __ca__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_ca(dst, src, base_idx, repeat, src_stride, dst_gap, 0, inc);
    }
}

__aicore__ inline void asc_copy_gm2l0a(
    __ca__ half* dst, __gm__ half* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_ca(dst, src, base_idx, repeat, src_stride, dst_gap, 0, inc);
    }
}

__aicore__ inline void asc_copy_gm2l0a(
    __ca__ float* dst, __gm__ float* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_ca(dst, src, base_idx, repeat, src_stride, dst_gap, 0, inc);
    }
}

__aicore__ inline void asc_copy_gm2l0a(
    __ca__ int32_t* dst, __gm__ int32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_ca(dst, src, base_idx, repeat, src_stride, dst_gap, 0, inc);
    }
}

__aicore__ inline void asc_copy_gm2l0a(
    __ca__ uint8_t* dst, __gm__ uint8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_ca(dst, src, base_idx, repeat, src_stride, dst_gap, 0, inc);
    }
}

__aicore__ inline void asc_copy_gm2l0a(
    __ca__ int8_t* dst, __gm__ int8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_ca(dst, src, base_idx, repeat, src_stride, dst_gap, 0, inc);
    }
}

__aicore__ inline void asc_copy_gm2l0a(
    __ca__ uint32_t* dst, __gm__ uint32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_ca(dst, src, base_idx, repeat, src_stride, dst_gap, 0, inc);
    }
}

__aicore__ inline void asc_copy_gm2l0a(
    __ca__ int4b_t* dst, __gm__ int4b_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_ca_s4((__ca__ void*)dst, (__gm__ void*)src, base_idx, repeat, src_stride, dst_gap, 0, inc);
    }
}

__aicore__ inline void asc_copy_gm2l1_nd2nz(
    __cbuf__ int8_t* dst, __gm__ int8_t* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz_b8(
            dst, src, 0, nd_num, n_value, d_value, src_nd_matrix_stride, src_d_value, dst_nz_c0_stride, dst_nz_n_stride,
            dst_nz_matrix_stride);
    }
}

__aicore__ inline void asc_copy_gm2l1_nd2nz(
    __cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz_b8(
            dst, src, 0, nd_num, n_value, d_value, src_nd_matrix_stride, src_d_value, dst_nz_c0_stride, dst_nz_n_stride,
            dst_nz_matrix_stride);
    }
}

__aicore__ inline void asc_copy_gm2l1_nd2nz(
    __cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz_b16(
            dst, src, 0, nd_num, n_value, d_value, src_nd_matrix_stride, src_d_value, dst_nz_c0_stride, dst_nz_n_stride,
            dst_nz_matrix_stride);
    }
}

__aicore__ inline void asc_copy_gm2l1_nd2nz(
    __cbuf__ half* dst, __gm__ half* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz_b16(
            dst, src, 0, nd_num, n_value, d_value, src_nd_matrix_stride, src_d_value, dst_nz_c0_stride, dst_nz_n_stride,
            dst_nz_matrix_stride);
    }
}

__aicore__ inline void asc_copy_gm2l1_nd2nz(
    __cbuf__ int16_t* dst, __gm__ int16_t* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz_b16(
            dst, src, 0, nd_num, n_value, d_value, src_nd_matrix_stride, src_d_value, dst_nz_c0_stride, dst_nz_n_stride,
            dst_nz_matrix_stride);
    }
}

__aicore__ inline void asc_copy_gm2l1_nd2nz(
    __cbuf__ float* dst, __gm__ float* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz_b32s(
            dst, src, 0, nd_num, n_value, d_value, src_nd_matrix_stride, src_d_value, dst_nz_c0_stride, dst_nz_n_stride,
            dst_nz_matrix_stride);
    }
}

__aicore__ inline void asc_copy_gm2l1_nd2nz(
    __cbuf__ int32_t* dst, __gm__ int32_t* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz_b32s(
            dst, src, 0, nd_num, n_value, d_value, src_nd_matrix_stride, src_d_value, dst_nz_c0_stride, dst_nz_n_stride,
            dst_nz_matrix_stride);
    }
}

__aicore__ inline void asc_copy_gm2l1_nd2nz(
    __cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint16_t nd_num, uint16_t n_value, uint16_t d_value,
    uint16_t src_nd_matrix_stride, uint16_t src_d_value, uint16_t dst_nz_c0_stride, uint16_t dst_nz_n_stride,
    uint16_t dst_nz_matrix_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz_b32s(
            dst, src, 0, nd_num, n_value, d_value, src_nd_matrix_stride, src_d_value, dst_nz_c0_stride, dst_nz_n_stride,
            dst_nz_matrix_stride);
    }
}

__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ half* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, unit_flag_mode, static_cast<QuantMode_t>(quant_pre),
            relu_pre, enable_channel_split, enable_nz2nd);
    }
}

__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ bfloat16_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split,
    bool enable_nz2nd)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, unit_flag_mode, static_cast<QuantMode_t>(quant_pre),
            relu_pre, enable_channel_split, enable_nz2nd);
    }
}

__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ int8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, unit_flag_mode, static_cast<QuantMode_t>(quant_pre),
            relu_pre, enable_channel_split, enable_nz2nd);
    }
}

__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ half* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, unit_flag_mode, static_cast<QuantMode_t>(quant_pre),
            relu_pre, enable_channel_split, enable_nz2nd);
    }
}

__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ int16_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split,
    bool enable_nz2nd)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, unit_flag_mode, static_cast<QuantMode_t>(quant_pre),
            relu_pre, enable_channel_split, enable_nz2nd);
    }
}

__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split,
    bool enable_nz2nd)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, unit_flag_mode, static_cast<QuantMode_t>(quant_pre),
            relu_pre, enable_channel_split, enable_nz2nd);
    }
}

__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ uint8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split,
    bool enable_nz2nd)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, unit_flag_mode, static_cast<QuantMode_t>(quant_pre),
            relu_pre, enable_channel_split, enable_nz2nd);
    }
}

__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ void* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf_b4(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, unit_flag_mode, static_cast<QuantMode_t>(quant_pre),
            relu_pre, enable_channel_split, enable_nz2nd);
    }
}

__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ void* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool enable_channel_split, bool enable_nz2nd)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf_b4(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, unit_flag_mode, static_cast<QuantMode_t>(quant_pre),
            relu_pre, enable_channel_split, enable_nz2nd);
    }
}

__aicore__ inline void asc_copy_l12l0a(
    __ca__ int4b_t* dst, __cbuf__ int4b_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca_s4(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
    }
}

__aicore__ inline void asc_copy_l12l0a(
    __ca__ int8_t* dst, __cbuf__ int8_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
    }
}

__aicore__ inline void asc_copy_l12l0a(
    __ca__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
    }
}

__aicore__ inline void asc_copy_l12l0a(
    __ca__ half* dst, __cbuf__ half* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
    }
}

__aicore__ inline void asc_copy_l12l0a(
    __ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
    }
}

__aicore__ inline void asc_copy_l12l0a(
    __ca__ int32_t* dst, __cbuf__ int32_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
    }
}

__aicore__ inline void asc_copy_l12l0a(
    __ca__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
    }
}

__aicore__ inline void asc_copy_l12l0a(
    __ca__ float* dst, __cbuf__ float* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
    }
}

__aicore__ inline void asc_copy_l12l0a(
    __ca__ int4b_t* dst, __cbuf__ int4b_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_ca_s4(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
    }
}

__aicore__ inline void asc_copy_l12l0a(
    __ca__ int8_t* dst, __cbuf__ int8_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_ca(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
    }
}

__aicore__ inline void asc_copy_l12l0a(
    __ca__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_ca(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
    }
}

__aicore__ inline void asc_copy_l12l0a(
    __ca__ half* dst, __cbuf__ half* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_ca(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
    }
}

__aicore__ inline void asc_copy_l12l0a(
    __ca__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_ca(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
    }
}

__aicore__ inline void asc_copy_l12l0a(
    __ca__ int32_t* dst, __cbuf__ int32_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_ca(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
    }
}

__aicore__ inline void asc_copy_l12l0a(
    __ca__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_ca(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
    }
}

__aicore__ inline void asc_copy_l12l0a(
    __ca__ float* dst, __cbuf__ float* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_ca(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
    }
}

__aicore__ inline void asc_copy_l12l0b(
    __cb__ int4b_t* dst, __cbuf__ int4b_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb_s4(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
    }
}

__aicore__ inline void asc_copy_l12l0b(
    __cb__ int8_t* dst, __cbuf__ int8_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
    }
}

__aicore__ inline void asc_copy_l12l0b(
    __cb__ uint8_t* dst, __cbuf__ uint8_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
    }
}

__aicore__ inline void asc_copy_l12l0b(
    __cb__ half* dst, __cbuf__ half* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
    }
}

__aicore__ inline void asc_copy_l12l0b(
    __cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
    }
}

__aicore__ inline void asc_copy_l12l0b(
    __cb__ int32_t* dst, __cbuf__ int32_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
    }
}

__aicore__ inline void asc_copy_l12l0b(
    __cb__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
    }
}

__aicore__ inline void asc_copy_l12l0b(
    __cb__ float* dst, __cbuf__ float* src, uint16_t start_index, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb(dst, src, start_index, repeat, src_stride, dst_gap, 0, false, inc);
    }
}

__aicore__ inline void asc_copy_l12l0b(
    __cb__ half* dst, __cbuf__ half* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_cb(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
    }
}

__aicore__ inline void asc_copy_l12l0b(
    __cb__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_cb(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
    }
}

__aicore__ inline void asc_copy_l12l0b(
    __cb__ int32_t* dst, __cbuf__ int32_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_cb(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
    }
}

__aicore__ inline void asc_copy_l12l0b(
    __cb__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_cb(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
    }
}

__aicore__ inline void asc_copy_l12l0b(
    __cb__ float* dst, __cbuf__ float* src, uint16_t k_extension, uint16_t m_extension, uint16_t k_start_pt,
    uint16_t m_start_pt, uint8_t stride_w, uint8_t stride_h, uint8_t filter_w, uint8_t filter_h,
    uint8_t dilation_filter_w, uint8_t dilation_filter_h, bool filter_size_w, bool filter_size_h, bool transpose,
    bool f_matrix_ctrl, uint16_t channel_size)
{
    if ASC_IS_AIC {
        img2colv2_cbuf_to_cb(
            dst, src, k_extension, m_extension, k_start_pt, m_start_pt, stride_w, stride_h, filter_w, filter_h,
            dilation_filter_w, dilation_filter_h, filter_size_w, filter_size_h, transpose, f_matrix_ctrl, channel_size);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ half* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride_dst_d, src_stride, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ bfloat16_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride_dst_d, src_stride, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ int8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride_dst_d, src_stride, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ uint8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride_dst_d, src_stride, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ float* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride_dst_d, src_stride, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ half* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride_dst_d, src_stride, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ int16_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride_dst_d, src_stride, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride_dst_d, src_stride, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ int32_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride_dst_d,
    uint16_t src_stride, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride_dst_d, src_stride, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en);
    }
}

__aicore__ inline void asc_copy_l12gm(
    __gm__ void* dst, __cbuf__ void* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_cbuf_to_gm(dst, src, 0, n_burst, len_burst, src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_l12l0c(
    __cc__ bfloat16_t* dst, __cbuf__ bfloat16_t* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_matrix_cbuf_to_cc(dst, src, n_burst, len_burst, src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_l12l0c(
    __cc__ half* dst, __cbuf__ half* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_matrix_cbuf_to_cc(dst, src, n_burst, len_burst, src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_l12l0c(
    __cc__ half* dst, __cbuf__ float* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_matrix_cbuf_to_cc(dst, src, n_burst, len_burst, src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_l12l0c(
    __cc__ bfloat16_t* dst, __cbuf__ float* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_matrix_cbuf_to_cc(dst, src, n_burst, len_burst, src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_l12l0c(
    __cc__ float* dst, __cbuf__ float* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_matrix_cbuf_to_cc(dst, src, n_burst, len_burst, src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_l12l0c(
    __cc__ int32_t* dst, __cbuf__ int32_t* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_matrix_cbuf_to_cc(dst, src, n_burst, len_burst, src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_l12l0c(
    __cc__ uint32_t* dst, __cbuf__ uint32_t* src, uint16_t n_burst, uint16_t len_burst, uint16_t src_gap,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        copy_matrix_cbuf_to_cc(dst, src, n_burst, len_burst, src_gap, dst_gap);
    }
}

__aicore__ inline void asc_copy_gm2l0b(
    __cb__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cb(dst, src, base_idx, repeat, src_stride, dst_gap, 0, addr_cal_mode_t::inc);
    }
}

__aicore__ inline void asc_copy_gm2l0b(
    __cb__ half* dst, __gm__ half* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cb(dst, src, base_idx, repeat, src_stride, dst_gap, 0, addr_cal_mode_t::inc);
    }
}

__aicore__ inline void asc_copy_gm2l0b(
    __cb__ float* dst, __gm__ float* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cb(dst, src, base_idx, repeat, src_stride, dst_gap, 0, addr_cal_mode_t::inc);
    }
}

__aicore__ inline void asc_copy_gm2l0b(
    __cb__ int32_t* dst, __gm__ int32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cb(dst, src, base_idx, repeat, src_stride, dst_gap, 0, addr_cal_mode_t::inc);
    }
}

__aicore__ inline void asc_copy_gm2l0b(
    __cb__ int8_t* dst, __gm__ int8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cb(dst, src, base_idx, repeat, src_stride, dst_gap, 0, addr_cal_mode_t::inc);
    }
}

__aicore__ inline void asc_copy_gm2l0b(
    __cb__ uint32_t* dst, __gm__ uint32_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride,
    uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cb(dst, src, base_idx, repeat, src_stride, dst_gap, 0, addr_cal_mode_t::inc);
    }
}

__aicore__ inline void asc_copy_gm2l0b(
    __cb__ uint8_t* dst, __gm__ uint8_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cb(dst, src, base_idx, repeat, src_stride, dst_gap, 0, addr_cal_mode_t::inc);
    }
}

__aicore__ inline void asc_copy_gm2l0b(
    __cb__ int4b_t* dst, __gm__ int4b_t* src, uint16_t base_idx, uint8_t repeat, uint16_t src_stride, uint16_t dst_gap)
{
    if ASC_IS_AIC {
        load_gm_to_cb_s4(dst, src, base_idx, repeat, src_stride, dst_gap, 0, addr_cal_mode_t::inc);
    }
}

__aicore__ inline void asc_set_l0c2gm_config(uint64_t relu_pre, uint64_t quant_pre, bool enable_unit_flag)
{
    if ASC_IS_AIC {
        asc_capi_fpc_reg_config config;
        config.config = 0;
        config.relu_units = relu_pre;
        config.quant_units = quant_pre;
        config.unit_flag = enable_unit_flag;
        set_fpc(config.config);
    }
}

__aicore__ inline uint64_t asc_get_l0c2gm_relu()
{
    if ASC_IS_AIC {
        asc_capi_fpc_reg_config config;
        config.config = get_fpc();
        return static_cast<uint64_t>(config.relu_units);
    }
    return 0;
}

__aicore__ inline uint64_t asc_get_l0c2gm_unitflag()
{
    if ASC_IS_AIC {
        asc_capi_fpc_reg_config config;
        config.config = get_fpc();
        return static_cast<uint64_t>(config.unit_flag);
    }
    return 0;
}

__aicore__ inline uint64_t asc_get_l0c2gm_prequant()
{
    if ASC_IS_AIC {
        asc_capi_fpc_reg_config config;
        config.config = get_fpc();
        return static_cast<uint64_t>(config.quant_units);
    }
    return 0;
}

__aicore__ inline void asc_set_l13d_rpt(asc_load3d_v2_config& config)
{
    if ASC_IS_AIC {
        set_l3d_rpt(config.config);
    }
}

__aicore__ inline void asc_set_l0c_copy_params(uint16_t nd_num, uint16_t src_nd_stride, uint16_t dst_nd_stride)
{
    set_l0c_copy_params_impl(nd_num, src_nd_stride, dst_nd_stride);
}

__aicore__ inline void asc_set_l13d_size(uint64_t value)
{
    if ASC_IS_AIC {
        set_l1_3d_size(value);
    }
}

__aicore__ inline void asc_set_l0c_copy_prequant(uint64_t config)
{
    if ASC_IS_AIC {
        set_quant_pre(config);
    }
}

__aicore__ inline void asc_set_l13d_padding(uint64_t config)
{
    if ASC_IS_AIC {
        uint64_t paddingValue = 0;
        uint64_t padValueShiftBit = 8;
        paddingValue = (config << padValueShiftBit) | (config & 0xFF);
        set_padding(paddingValue);
    }
}

__aicore__ inline void asc_set_l13d_padding(half config)
{
    if ASC_IS_AIC {
        asc_scalar_bitcode bitcode;
        bitcode.input_half = config;
        set_padding(bitcode.output);
    }
}

__aicore__ inline void asc_set_l13d_padding(int16_t config)
{
    if ASC_IS_AIC {
        asc_scalar_bitcode bitcode;
        bitcode.input_int16_t = config;
        set_padding(bitcode.output);
    }
}

__aicore__ inline void asc_set_l13d_padding(uint16_t config)
{
    if ASC_IS_AIC {
        asc_scalar_bitcode bitcode;
        bitcode.input_uint16_t = config;
        set_padding(bitcode.output);
    }
}

__aicore__ inline void asc_set_l13d_fmatrix(asc_l13d_fmatrix_config& config)
{
    if ASC_IS_AIC {
        set_fmatrix(config.config);
    }
}

__aicore__ inline void asc_set_l13d_fmatrix_b(asc_l13d_fmatrix_config& config)
{
    if ASC_IS_AIC {
        set_fmatrix_b(config.config);
    }
}

__aicore__ inline void asc_set_l0c2gm_lrelu_alpha(half& config)
{
    if ASC_IS_AIC {
        set_lrelu_alpha(config);
    }
}

__aicore__ inline void asc_set_l0c2gm_lrelu_alpha(float& config)
{
    if ASC_IS_AIC {
        set_lrelu_alpha(config);
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
