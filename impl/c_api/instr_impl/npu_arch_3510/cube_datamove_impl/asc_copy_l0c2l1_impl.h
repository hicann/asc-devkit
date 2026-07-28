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
    "impl/c_api/instr_impl/npu_arch_3510/cube_datamove_impl/asc_copy_l0c2l1_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_CUBE_DATAMOVE_IMPL_ASC_COPY_L0C2L1_IMPL_H
#define IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_CUBE_DATAMOVE_IMPL_ASC_COPY_L0C2L1_IMPL_H

#include "impl/c_api/instr_impl/npu_arch_3510/utils_impl/utils_impl.h"

// half  float
__aicore__ inline void asc_copy_l0c2l1_impl(
    __cbuf__ half* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
    uint8_t eltwise_antq_en, bool c0_pad_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, 0, clip_relu_pre, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en,
            static_cast<QuantMode_post>(quant_post), relu_post, clip_relu_post, false, eltwise_op, eltwise_antq_en,
            false, c0_pad_en, false, false, false);
    }
}

// int8_t  float
__aicore__ inline void asc_copy_l0c2l1_impl(
    __cbuf__ int8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
    uint8_t eltwise_antq_en, bool c0_pad_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, 0, clip_relu_pre, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en,
            static_cast<QuantMode_post>(quant_post), relu_post, clip_relu_post, false, eltwise_op, eltwise_antq_en,
            false, c0_pad_en, false, false, false);
    }
}

// uint8_t  float
__aicore__ inline void asc_copy_l0c2l1_impl(
    __cbuf__ uint8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre,
    bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
    uint8_t eltwise_antq_en, bool c0_pad_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, 0, clip_relu_pre, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en,
            static_cast<QuantMode_post>(quant_post), relu_post, clip_relu_post, false, eltwise_op, eltwise_antq_en,
            false, c0_pad_en, false, false, false);
    }
}

// float  float
__aicore__ inline void asc_copy_l0c2l1_impl(
    __cbuf__ float* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
    uint8_t eltwise_antq_en, bool c0_pad_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, 0, clip_relu_pre, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en,
            static_cast<QuantMode_post>(quant_post), relu_post, clip_relu_post, false, eltwise_op, eltwise_antq_en,
            false, c0_pad_en, false, false, false);
    }
}

// half int32_t
__aicore__ inline void asc_copy_l0c2l1_impl(
    __cbuf__ half* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre, bool channel_split,
    bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
    uint8_t eltwise_antq_en, bool c0_pad_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, 0, clip_relu_pre, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en,
            static_cast<QuantMode_post>(quant_post), relu_post, clip_relu_post, false, eltwise_op, eltwise_antq_en,
            false, c0_pad_en, false, false, false);
    }
}

// int8_t int32_t
__aicore__ inline void asc_copy_l0c2l1_impl(
    __cbuf__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre,
    bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
    uint8_t eltwise_antq_en, bool c0_pad_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, 0, clip_relu_pre, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en,
            static_cast<QuantMode_post>(quant_post), relu_post, clip_relu_post, false, eltwise_op, eltwise_antq_en,
            false, c0_pad_en, false, false, false);
    }
}

// uint8_t int32_t
__aicore__ inline void asc_copy_l0c2l1_impl(
    __cbuf__ uint8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre,
    bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
    uint8_t eltwise_antq_en, bool c0_pad_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, 0, clip_relu_pre, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en,
            static_cast<QuantMode_post>(quant_post), relu_post, clip_relu_post, false, eltwise_op, eltwise_antq_en,
            false, c0_pad_en, false, false, false);
    }
}

// int32_t int32_t
__aicore__ inline void asc_copy_l0c2l1_impl(
    __cbuf__ int32_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, uint8_t clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre,
    bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
    uint8_t eltwise_antq_en, bool c0_pad_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, 0, clip_relu_pre, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en,
            static_cast<QuantMode_post>(quant_post), relu_post, clip_relu_post, false, eltwise_op, eltwise_antq_en,
            false, c0_pad_en, false, false, false);
    }
}

// void float
__aicore__ inline void asc_copy_l0c2l1_impl(
    __cbuf__ void* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t l2_cache_ctl, uint8_t clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre,
    bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
    bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf_s4(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, l2_cache_ctl, clip_relu_pre, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en,
            static_cast<QuantMode_post>(quant_post), relu_post, clip_relu_post, false, eltwise_op, eltwise_antq_en,
            false, c0_pad_en, false, broadcast_en, nz2dn_en);
    }
}

__aicore__ inline void asc_copy_l0c2l1_sync_impl(
    __cbuf__ void* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t l2_cache_ctl, uint8_t clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre,
    bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
    bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
{
    asc_copy_l0c2l1_impl(
        dst, src, n_size, m_size, dst_stride, src_stride, l2_cache_ctl, clip_relu_pre, unit_flag_mode, quant_pre,
        relu_pre, channel_split, nz2nd_en, quant_post, relu_post, clip_relu_post, eltwise_op, eltwise_antq_en,
        c0_pad_en, broadcast_en, nz2dn_en);
    asc_sync_post_process();
}

// void int32_t
__aicore__ inline void asc_copy_l0c2l1_impl(
    __cbuf__ void* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t l2_cache_ctl, uint8_t clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre,
    bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
    bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf_s4(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, l2_cache_ctl, clip_relu_pre, unit_flag_mode,
            static_cast<QuantMode_t>(quant_pre), relu_pre, channel_split, nz2nd_en,
            static_cast<QuantMode_post>(quant_post), relu_post, clip_relu_post, false, eltwise_op, eltwise_antq_en,
            false, c0_pad_en, false, broadcast_en, nz2dn_en);
    }
}

__aicore__ inline void asc_copy_l0c2l1_sync_impl(
    __cbuf__ void* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    uint8_t l2_cache_ctl, uint8_t clip_relu_pre, uint8_t unit_flag_mode, uint64_t quant_pre, uint8_t relu_pre,
    bool channel_split, bool nz2nd_en, uint64_t quant_post, uint8_t relu_post, bool clip_relu_post, uint8_t eltwise_op,
    bool eltwise_antq_en, bool c0_pad_en, bool broadcast_en, bool nz2dn_en)
{
    asc_copy_l0c2l1_impl(
        dst, src, n_size, m_size, dst_stride, src_stride, l2_cache_ctl, clip_relu_pre, unit_flag_mode, quant_pre,
        relu_pre, channel_split, nz2nd_en, quant_post, relu_post, clip_relu_post, eltwise_op, eltwise_antq_en,
        c0_pad_en, broadcast_en, nz2dn_en);
    asc_sync_post_process();
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
