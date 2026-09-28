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
    "impl/c_api/reg_base_impl/npu_arch_9201/cube_datamove_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_9201_CUBE_DATAMOVE_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_9201_CUBE_DATAMOVE_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

constexpr uint8_t RELU_POST_DEFAULT = 0;

constexpr bool CLIP_RELU_POST_DEFAULT = false;

constexpr uint8_t ELTWISE_OP_DEFAULT = 0;

constexpr uint64_t QUANT_POST_DEFAULT = 0;

// ==========asc_copy_l0c2l1==========
// half  float
__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ half* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode,
    bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, n_size, m_size, dst_stride, src_stride, false, static_cast<uint8_t>(enable_clip_relu_pre),
            static_cast<uint8_t>(unit_flag_mode), static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)),
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, false, false, false, false, false,
            false, false, false, enable_nz2dn);
    }
}

// half  float
__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ half* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, n_size, m_size, dst_stride, src_stride, quant_pre_rnd, static_cast<uint8_t>(enable_clip_relu_pre),
            static_cast<uint8_t>(unit_flag_mode), static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)),
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, false, false, false, false, false,
            false, false, false, enable_nz2dn);
    }
}

// bfloat16_t  float
__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ bfloat16_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, n_size, m_size, dst_stride, src_stride, false, static_cast<uint8_t>(enable_clip_relu_pre),
            static_cast<uint8_t>(unit_flag_mode), static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)),
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, false, false, false, false, false,
            false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ bfloat16_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, n_size, m_size, dst_stride, src_stride, quant_pre_rnd, static_cast<uint8_t>(enable_clip_relu_pre),
            static_cast<uint8_t>(unit_flag_mode), static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)),
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, false, false, false, false, false,
            false, false, false, enable_nz2dn);
    }
}

// int8_t  float
__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ int8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode,
    bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, n_size, m_size, dst_stride, src_stride, false, static_cast<uint8_t>(enable_clip_relu_pre),
            static_cast<uint8_t>(unit_flag_mode), static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)),
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, false, false, false, false, false,
            false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ int8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, n_size, m_size, dst_stride, src_stride, quant_pre_rnd, static_cast<uint8_t>(enable_clip_relu_pre),
            static_cast<uint8_t>(unit_flag_mode), static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)),
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, false, false, false, false, false,
            false, false, false, enable_nz2dn);
    }
}

// uint8_t  float
__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ uint8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, n_size, m_size, dst_stride, src_stride, false, static_cast<uint8_t>(enable_clip_relu_pre),
            static_cast<uint8_t>(unit_flag_mode), static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)),
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, false, false, false, false, false,
            false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ uint8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, n_size, m_size, dst_stride, src_stride, quant_pre_rnd, static_cast<uint8_t>(enable_clip_relu_pre),
            static_cast<uint8_t>(unit_flag_mode), static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)),
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, false, false, false, false, false,
            false, false, false, enable_nz2dn);
    }
}

// float  float
__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ float* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode,
    bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, n_size, m_size, dst_stride, src_stride, false, static_cast<uint8_t>(enable_clip_relu_pre),
            static_cast<uint8_t>(unit_flag_mode), static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)),
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, false, false, false, false, false,
            false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ float* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, n_size, m_size, dst_stride, src_stride, quant_pre_rnd, static_cast<uint8_t>(enable_clip_relu_pre),
            static_cast<uint8_t>(unit_flag_mode), static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)),
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, false, false, false, false, false,
            false, false, false, enable_nz2dn);
    }
}

// half  int32_t
__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ half* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode,
    bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, n_size, m_size, dst_stride, src_stride, false, static_cast<uint8_t>(enable_clip_relu_pre),
            static_cast<uint8_t>(unit_flag_mode), static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)),
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, false, false, false, false, false,
            false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ half* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, n_size, m_size, dst_stride, src_stride, quant_pre_rnd, static_cast<uint8_t>(enable_clip_relu_pre),
            static_cast<uint8_t>(unit_flag_mode), static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)),
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, false, false, false, false, false,
            false, false, false, enable_nz2dn);
    }
}

// bfloat16_t  int32_t
__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ bfloat16_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, n_size, m_size, dst_stride, src_stride, false, static_cast<uint8_t>(enable_clip_relu_pre),
            static_cast<uint8_t>(unit_flag_mode), static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)),
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, false, false, false, false, false,
            false, false, false, enable_nz2dn);
    }
}

// int8_t  int32_t
__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, n_size, m_size, dst_stride, src_stride, false, static_cast<uint8_t>(enable_clip_relu_pre),
            static_cast<uint8_t>(unit_flag_mode), static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)),
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, false, false, false, false, false,
            false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, n_size, m_size, dst_stride, src_stride, quant_pre_rnd, static_cast<uint8_t>(enable_clip_relu_pre),
            static_cast<uint8_t>(unit_flag_mode), static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)),
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, false, false, false, false, false,
            false, false, false, enable_nz2dn);
    }
}

// uint8_t  int32_t
__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ uint8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, n_size, m_size, dst_stride, src_stride, false, static_cast<uint8_t>(enable_clip_relu_pre),
            static_cast<uint8_t>(unit_flag_mode), static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)),
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, false, false, false, false, false,
            false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ uint8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, n_size, m_size, dst_stride, src_stride, quant_pre_rnd, static_cast<uint8_t>(enable_clip_relu_pre),
            static_cast<uint8_t>(unit_flag_mode), static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)),
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, false, false, false, false, false,
            false, false, false, enable_nz2dn);
    }
}

// int32_t  int32_t
__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ int32_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, n_size, m_size, dst_stride, src_stride, false, static_cast<uint8_t>(enable_clip_relu_pre),
            static_cast<uint8_t>(unit_flag_mode), static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)),
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, false, false, false, false, false,
            false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ int32_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, n_size, m_size, dst_stride, src_stride, quant_pre_rnd, static_cast<uint8_t>(enable_clip_relu_pre),
            static_cast<uint8_t>(unit_flag_mode), static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)),
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, false, false, false, false, false,
            false, false, false, enable_nz2dn);
    }
}

// hifloat8_t  float
__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ hifloat8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, n_size, m_size, dst_stride, src_stride, false, static_cast<uint8_t>(enable_clip_relu_pre),
            static_cast<uint8_t>(unit_flag_mode), static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)),
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, false, false, false, false, false,
            false, false, false, enable_nz2dn);
    }
}

// fp8_e4m3fn_t  float
__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ fp8_e4m3fn_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf(
            dst, src, n_size, m_size, dst_stride, src_stride, false, static_cast<uint8_t>(enable_clip_relu_pre),
            static_cast<uint8_t>(unit_flag_mode), static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)),
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, false, false, false, false, false,
            false, false, false, enable_nz2dn);
    }
}

// int4b_t  float (s4)
__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ int4b_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf_s4(
            (__cbuf__ void*)dst, src, n_size, m_size, dst_stride, src_stride, false,
            static_cast<uint8_t>(enable_clip_relu_pre), static_cast<uint8_t>(unit_flag_mode),
            static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)), static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            false, false, false, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ int4b_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf_s4(
            (__cbuf__ void*)dst, src, n_size, m_size, dst_stride, src_stride, quant_pre_rnd,
            static_cast<uint8_t>(enable_clip_relu_pre), static_cast<uint8_t>(unit_flag_mode),
            static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)), static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            false, false, false, false, false, false, false, false, enable_nz2dn);
    }
}

// int4b_t  int32_t (s4)
__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ int4b_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf_s4(
            (__cbuf__ void*)dst, src, n_size, m_size, dst_stride, src_stride, false,
            static_cast<uint8_t>(enable_clip_relu_pre), static_cast<uint8_t>(unit_flag_mode),
            static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)), static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            false, false, false, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2l1(
    __cbuf__ int4b_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_cbuf_s4(
            (__cbuf__ void*)dst, src, n_size, m_size, dst_stride, src_stride, quant_pre_rnd,
            static_cast<uint8_t>(enable_clip_relu_pre), static_cast<uint8_t>(unit_flag_mode),
            static_cast<QuantMode_t>(static_cast<uint64_t>(quant_pre_mode)), static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            false, false, false, false, false, false, false, false, enable_nz2dn);
    }
}

// ==========asc_prefetch_l2cache==========
// =======asc_prefetch_gm2l2cache=======
// int8_t
__aicore__ inline void asc_prefetch_gm2l2cache(
    __gm__ int8_t* src, uint32_t burst_num, uint32_t burst_len, uint64_t burst_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_align_v2(
            (__cbuf__ int8_t*)0, src, 0, burst_num, burst_len, 0, 0, true, false, 3, burst_src_stride, 0);
    }
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ int8_t*)0, src, 0, burst_num, burst_len, 0, 0, false, false, 3, burst_src_stride, 0, false);
    }
}

// uint8_t
__aicore__ inline void asc_prefetch_gm2l2cache(
    __gm__ uint8_t* src, uint32_t burst_num, uint32_t burst_len, uint64_t burst_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_align_v2(
            (__cbuf__ uint8_t*)0, src, 0, burst_num, burst_len, 0, 0, true, false, 3, burst_src_stride, 0);
    }
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ uint8_t*)0, src, 0, burst_num, burst_len, 0, 0, false, false, 3, burst_src_stride, 0, false);
    }
}

// hifloat8_t
__aicore__ inline void asc_prefetch_gm2l2cache(
    __gm__ hifloat8_t* src, uint32_t burst_num, uint32_t burst_len, uint64_t burst_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_align_v2(
            (__cbuf__ hifloat8_t*)0, src, 0, burst_num, burst_len, 0, 0, true, false, 3, burst_src_stride, 0);
    }
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ hifloat8_t*)0, src, 0, burst_num, burst_len, 0, 0, false, false, 3, burst_src_stride, 0, false);
    }
}

// fp8_e5m2_t
__aicore__ inline void asc_prefetch_gm2l2cache(
    __gm__ fp8_e5m2_t* src, uint32_t burst_num, uint32_t burst_len, uint64_t burst_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_align_v2(
            (__cbuf__ fp8_e5m2_t*)0, src, 0, burst_num, burst_len, 0, 0, true, false, 3, burst_src_stride, 0);
    }
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ fp8_e5m2_t*)0, src, 0, burst_num, burst_len, 0, 0, false, false, 3, burst_src_stride, 0, false);
    }
}

// fp8_e4m3fn_t
__aicore__ inline void asc_prefetch_gm2l2cache(
    __gm__ fp8_e4m3fn_t* src, uint32_t burst_num, uint32_t burst_len, uint64_t burst_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_align_v2(
            (__cbuf__ fp8_e4m3fn_t*)0, src, 0, burst_num, burst_len, 0, 0, true, false, 3, burst_src_stride, 0);
    }
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ fp8_e4m3fn_t*)0, src, 0, burst_num, burst_len, 0, 0, false, false, 3, burst_src_stride, 0, false);
    }
}

// int16_t
__aicore__ inline void asc_prefetch_gm2l2cache(
    __gm__ int16_t* src, uint32_t burst_num, uint32_t burst_len, uint64_t burst_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_align_v2(
            (__cbuf__ int16_t*)0, src, 0, burst_num, burst_len, 0, 0, true, false, 3, burst_src_stride, 0);
    }
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ int16_t*)0, src, 0, burst_num, burst_len, 0, 0, false, false, 3, burst_src_stride, 0, false);
    }
}

// uint16_t
__aicore__ inline void asc_prefetch_gm2l2cache(
    __gm__ uint16_t* src, uint32_t burst_num, uint32_t burst_len, uint64_t burst_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_align_v2(
            (__cbuf__ uint16_t*)0, src, 0, burst_num, burst_len, 0, 0, true, false, 3, burst_src_stride, 0);
    }
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ uint16_t*)0, src, 0, burst_num, burst_len, 0, 0, false, false, 3, burst_src_stride, 0, false);
    }
}

// half
__aicore__ inline void asc_prefetch_gm2l2cache(
    __gm__ half* src, uint32_t burst_num, uint32_t burst_len, uint64_t burst_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_align_v2(
            (__cbuf__ half*)0, src, 0, burst_num, burst_len, 0, 0, true, false, 3, burst_src_stride, 0);
    }
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ half*)0, src, 0, burst_num, burst_len, 0, 0, false, false, 3, burst_src_stride, 0, false);
    }
}

// bfloat16_t
__aicore__ inline void asc_prefetch_gm2l2cache(
    __gm__ bfloat16_t* src, uint32_t burst_num, uint32_t burst_len, uint64_t burst_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_align_v2(
            (__cbuf__ bfloat16_t*)0, src, 0, burst_num, burst_len, 0, 0, true, false, 3, burst_src_stride, 0);
    }
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ bfloat16_t*)0, src, 0, burst_num, burst_len, 0, 0, false, false, 3, burst_src_stride, 0, false);
    }
}

// int32_t
__aicore__ inline void asc_prefetch_gm2l2cache(
    __gm__ int32_t* src, uint32_t burst_num, uint32_t burst_len, uint64_t burst_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_align_v2(
            (__cbuf__ int32_t*)0, src, 0, burst_num, burst_len, 0, 0, true, false, 3, burst_src_stride, 0);
    }
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ int32_t*)0, src, 0, burst_num, burst_len, 0, 0, false, false, 3, burst_src_stride, 0, false);
    }
}

// uint32_t
__aicore__ inline void asc_prefetch_gm2l2cache(
    __gm__ uint32_t* src, uint32_t burst_num, uint32_t burst_len, uint64_t burst_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_align_v2(
            (__cbuf__ uint32_t*)0, src, 0, burst_num, burst_len, 0, 0, true, false, 3, burst_src_stride, 0);
    }
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ uint32_t*)0, src, 0, burst_num, burst_len, 0, 0, false, false, 3, burst_src_stride, 0, false);
    }
}

// float
__aicore__ inline void asc_prefetch_gm2l2cache(
    __gm__ float* src, uint32_t burst_num, uint32_t burst_len, uint64_t burst_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_align_v2(
            (__cbuf__ float*)0, src, 0, burst_num, burst_len, 0, 0, true, false, 3, burst_src_stride, 0);
    }
    if ASC_IS_AIV {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ float*)0, src, 0, burst_num, burst_len, 0, 0, false, false, 3, burst_src_stride, 0, false);
    }
}

// =======asc_prefetch_gm2l2cache_dn2nz=======
// int8_t
__aicore__ inline void asc_prefetch_gm2l2cache_dn2nz(
    __gm__ int8_t* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            (__cbuf__ int8_t*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// uint8_t
__aicore__ inline void asc_prefetch_gm2l2cache_dn2nz(
    __gm__ uint8_t* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            (__cbuf__ uint8_t*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// hifloat8_t
__aicore__ inline void asc_prefetch_gm2l2cache_dn2nz(
    __gm__ hifloat8_t* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            (__cbuf__ hifloat8_t*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// fp8_e5m2_t
__aicore__ inline void asc_prefetch_gm2l2cache_dn2nz(
    __gm__ fp8_e5m2_t* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            (__cbuf__ fp8_e5m2_t*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// fp8_e4m3fn_t
__aicore__ inline void asc_prefetch_gm2l2cache_dn2nz(
    __gm__ fp8_e4m3fn_t* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            (__cbuf__ fp8_e4m3fn_t*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// int16_t
__aicore__ inline void asc_prefetch_gm2l2cache_dn2nz(
    __gm__ int16_t* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            (__cbuf__ int16_t*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// uint16_t
__aicore__ inline void asc_prefetch_gm2l2cache_dn2nz(
    __gm__ uint16_t* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            (__cbuf__ uint16_t*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// half
__aicore__ inline void asc_prefetch_gm2l2cache_dn2nz(
    __gm__ half* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            (__cbuf__ half*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// bfloat16_t
__aicore__ inline void asc_prefetch_gm2l2cache_dn2nz(
    __gm__ bfloat16_t* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            (__cbuf__ bfloat16_t*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// int32_t
__aicore__ inline void asc_prefetch_gm2l2cache_dn2nz(
    __gm__ int32_t* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            (__cbuf__ int32_t*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// uint32_t
__aicore__ inline void asc_prefetch_gm2l2cache_dn2nz(
    __gm__ uint32_t* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            (__cbuf__ uint32_t*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// float
__aicore__ inline void asc_prefetch_gm2l2cache_dn2nz(
    __gm__ float* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            (__cbuf__ float*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// =======asc_prefetch_gm2l2cache_nd2nz=======
// int8_t
__aicore__ inline void asc_prefetch_gm2l2cache_nd2nz(
    __gm__ int8_t* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            (__cbuf__ int8_t*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// uint8_t
__aicore__ inline void asc_prefetch_gm2l2cache_nd2nz(
    __gm__ uint8_t* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            (__cbuf__ uint8_t*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// hifloat8_t
__aicore__ inline void asc_prefetch_gm2l2cache_nd2nz(
    __gm__ hifloat8_t* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            (__cbuf__ hifloat8_t*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// fp8_e5m2_t
__aicore__ inline void asc_prefetch_gm2l2cache_nd2nz(
    __gm__ fp8_e5m2_t* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            (__cbuf__ fp8_e5m2_t*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// fp8_e4m3fn_t
__aicore__ inline void asc_prefetch_gm2l2cache_nd2nz(
    __gm__ fp8_e4m3fn_t* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            (__cbuf__ fp8_e4m3fn_t*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// int16_t
__aicore__ inline void asc_prefetch_gm2l2cache_nd2nz(
    __gm__ int16_t* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            (__cbuf__ int16_t*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// uint16_t
__aicore__ inline void asc_prefetch_gm2l2cache_nd2nz(
    __gm__ uint16_t* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            (__cbuf__ uint16_t*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// half
__aicore__ inline void asc_prefetch_gm2l2cache_nd2nz(
    __gm__ half* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            (__cbuf__ half*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// bfloat16_t
__aicore__ inline void asc_prefetch_gm2l2cache_nd2nz(
    __gm__ bfloat16_t* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            (__cbuf__ bfloat16_t*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// int32_t
__aicore__ inline void asc_prefetch_gm2l2cache_nd2nz(
    __gm__ int32_t* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            (__cbuf__ int32_t*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// uint32_t
__aicore__ inline void asc_prefetch_gm2l2cache_nd2nz(
    __gm__ uint32_t* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            (__cbuf__ uint32_t*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// float
__aicore__ inline void asc_prefetch_gm2l2cache_nd2nz(
    __gm__ float* src, uint64_t loop1_src_stride, uint16_t n_value, uint32_t d_value, uint64_t loop4_src_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            (__cbuf__ float*)0, src, 0, loop1_src_stride, 3, n_value, d_value, loop4_src_stride, false, false);
    }
}

// =======asc_prefetch_stop=======
__aicore__ inline void asc_prefetch_stop() { __prefetch_stop(); }

// ==========asc_copy_l12l0a==========
// hif4x2_t
__aicore__ inline void asc_copy_l12l0a(
    __ca__ hif4x2_t* dst, __cbuf__ hif4x2_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca_s4(
            (__ca__ void*)dst, (__cbuf__ void*)src, m_start_position, k_start_position, m_step, k_step, src_stride,
            dst_stride, 0);
    }
}

// transpose
__aicore__ inline void asc_copy_l12l0a_transpose(
    __ca__ hif4x2_t* dst, __cbuf__ hif4x2_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
{
    if ASC_IS_AIC {
        load_cbuf_to_ca_s4(
            (__ca__ void*)dst, (__cbuf__ void*)src, m_start_position, k_start_position, m_step, k_step, src_stride,
            dst_stride, 1);
    }
}

// ==========asc_copy_l12l0b==========
// hif4x2_t
__aicore__ inline void asc_copy_l12l0b(
    __cb__ hif4x2_t* dst, __cbuf__ hif4x2_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb_s4(
            (__cb__ void*)dst, (__cbuf__ void*)src, m_start_position, k_start_position, m_step, k_step, src_stride,
            dst_stride, 0);
    }
}

// transpose
__aicore__ inline void asc_copy_l12l0b_transpose(
    __cb__ hif4x2_t* dst, __cbuf__ hif4x2_t* src, uint16_t m_start_position, uint16_t k_start_position, uint8_t m_step,
    uint8_t k_step, int16_t src_stride, uint16_t dst_stride)
{
    if ASC_IS_AIC {
        load_cbuf_to_cb_s4(
            (__cb__ void*)dst, (__cbuf__ void*)src, m_start_position, k_start_position, m_step, k_step, src_stride,
            dst_stride, 1);
    }
}

// ==========asc_copy_l0c2gm==========
// bfloat16_t  float
__aicore__ inline void asc_copy_l0c2gm(
    __gm__ bfloat16_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_store_l2_cache_mode l2_cache_mode, asc_unit_flag_mode unit_flag_mode,
    asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
    bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), false,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ bfloat16_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_store_l2_cache_mode l2_cache_mode, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode,
    asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
    bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), quant_pre_rnd,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// half  float
__aicore__ inline void asc_copy_l0c2gm(
    __gm__ half* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    asc_store_l2_cache_mode l2_cache_mode, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), false,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ half* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    asc_store_l2_cache_mode l2_cache_mode, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode,
    asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
    bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), quant_pre_rnd,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// fp8_e4m3fn_t  float
__aicore__ inline void asc_copy_l0c2gm(
    __gm__ fp8_e4m3fn_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_store_l2_cache_mode l2_cache_mode, asc_unit_flag_mode unit_flag_mode,
    asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
    bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), false,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ fp8_e4m3fn_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_store_l2_cache_mode l2_cache_mode, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode,
    asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
    bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), quant_pre_rnd,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// hifloat8_t  float
__aicore__ inline void asc_copy_l0c2gm(
    __gm__ hifloat8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_store_l2_cache_mode l2_cache_mode, asc_unit_flag_mode unit_flag_mode,
    asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
    bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), false,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ hifloat8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_store_l2_cache_mode l2_cache_mode, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode,
    asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
    bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), quant_pre_rnd,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// int8_t  float
__aicore__ inline void asc_copy_l0c2gm(
    __gm__ int8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    asc_store_l2_cache_mode l2_cache_mode, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), false,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ int8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    asc_store_l2_cache_mode l2_cache_mode, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode,
    asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
    bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), quant_pre_rnd,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// uint8_t  float
__aicore__ inline void asc_copy_l0c2gm(
    __gm__ uint8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    asc_store_l2_cache_mode l2_cache_mode, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), false,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ uint8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    asc_store_l2_cache_mode l2_cache_mode, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode,
    asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
    bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), quant_pre_rnd,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// float  float
__aicore__ inline void asc_copy_l0c2gm(
    __gm__ float* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    asc_store_l2_cache_mode l2_cache_mode, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), false,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ float* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    asc_store_l2_cache_mode l2_cache_mode, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode,
    asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
    bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), quant_pre_rnd,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// bfloat16_t  int32_t
__aicore__ inline void asc_copy_l0c2gm(
    __gm__ bfloat16_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_store_l2_cache_mode l2_cache_mode, asc_unit_flag_mode unit_flag_mode,
    asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
    bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), false,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ bfloat16_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_store_l2_cache_mode l2_cache_mode, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode,
    asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
    bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), quant_pre_rnd,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// half  int32_t
__aicore__ inline void asc_copy_l0c2gm(
    __gm__ half* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    asc_store_l2_cache_mode l2_cache_mode, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), false,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ half* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    asc_store_l2_cache_mode l2_cache_mode, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode,
    asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
    bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), quant_pre_rnd,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// int8_t  int32_t
__aicore__ inline void asc_copy_l0c2gm(
    __gm__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    asc_store_l2_cache_mode l2_cache_mode, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), false,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    asc_store_l2_cache_mode l2_cache_mode, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode,
    asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
    bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), quant_pre_rnd,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// uint8_t  int32_t
__aicore__ inline void asc_copy_l0c2gm(
    __gm__ uint8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_store_l2_cache_mode l2_cache_mode, asc_unit_flag_mode unit_flag_mode,
    asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
    bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), false,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ uint8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_store_l2_cache_mode l2_cache_mode, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode,
    asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
    bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), quant_pre_rnd,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// int32_t  int32_t
__aicore__ inline void asc_copy_l0c2gm(
    __gm__ int32_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_store_l2_cache_mode l2_cache_mode, asc_unit_flag_mode unit_flag_mode,
    asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
    bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), false,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ int32_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_store_l2_cache_mode l2_cache_mode, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode,
    asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
    bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm(
            dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode), quant_pre_rnd,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// int4b_t  float
__aicore__ inline void asc_copy_l0c2gm(
    __gm__ int4b_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    asc_store_l2_cache_mode l2_cache_mode, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm_s4(
            (__gm__ void*)dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode),
            false, enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ int4b_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    asc_store_l2_cache_mode l2_cache_mode, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode,
    asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
    bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm_s4(
            (__gm__ void*)dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode),
            quant_pre_rnd, enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// int4b_t  int32_t
__aicore__ inline void asc_copy_l0c2gm(
    __gm__ int4b_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_store_l2_cache_mode l2_cache_mode, asc_unit_flag_mode unit_flag_mode,
    asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
    bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm_s4(
            (__gm__ void*)dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode),
            false, enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2gm(
    __gm__ int4b_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_store_l2_cache_mode l2_cache_mode, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode,
    asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd,
    bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_gm_s4(
            (__gm__ void*)dst, src, 0, n_size, m_size, dst_stride, src_stride, static_cast<uint8_t>(l2_cache_mode),
            quant_pre_rnd, enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// ==========asc_copy_l0c2ub==========
// bfloat16_t  float
__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ bfloat16_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, false, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ bfloat16_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, quant_pre_rnd, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// half  float
__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ half* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode,
    bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, false, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ half* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, quant_pre_rnd, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// fp8_e4m3fn_t  float
__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ fp8_e4m3fn_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, false, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ fp8_e4m3fn_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, quant_pre_rnd, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// hifloat8_t  float
__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ hifloat8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, false, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ hifloat8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, quant_pre_rnd, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// int8_t  float
__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ int8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode,
    bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, false, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ int8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, quant_pre_rnd, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// uint8_t  float
__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ uint8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, false, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ uint8_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, quant_pre_rnd, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// float  float
__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ float* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode,
    bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, false, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ float* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, quant_pre_rnd, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// bfloat16_t  int32_t
__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ bfloat16_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, false, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ bfloat16_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, quant_pre_rnd, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// half  int32_t
__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ half* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode, asc_relu_pre_mode relu_pre_mode,
    bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn, bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, false, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ half* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride, uint16_t src_stride,
    bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, quant_pre_rnd, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// int8_t  int32_t
__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, false, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ int8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, quant_pre_rnd, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// uint8_t  int32_t
__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ uint8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, false, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ uint8_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, quant_pre_rnd, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// int32_t  int32_t
__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ int32_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, false, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ int32_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub(
            dst, src, n_size, m_size, dst_stride, src_stride, 0, false, quant_pre_rnd, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// int4b_t  float
__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ int4b_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub_s4(
            (__ubuf__ void*)dst, src, n_size, m_size, dst_stride, src_stride, 0, false, false, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ int4b_t* dst, __cc__ float* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub_s4(
            (__ubuf__ void*)dst, src, n_size, m_size, dst_stride, src_stride, 0, false, quant_pre_rnd,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

// int4b_t  int32_t
__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ int4b_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub_s4(
            (__ubuf__ void*)dst, src, n_size, m_size, dst_stride, src_stride, 0, false, false, enable_clip_relu_pre,
            static_cast<uint8_t>(unit_flag_mode), quant_pre_mode, static_cast<uint8_t>(relu_pre_mode),
            enable_channel_split, enable_nz2nd, static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT,
            CLIP_RELU_POST_DEFAULT, false, ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_l0c2ub(
    __ubuf__ int4b_t* dst, __cc__ int32_t* src, uint16_t n_size, uint16_t m_size, uint32_t dst_stride,
    uint16_t src_stride, bool quant_pre_rnd, asc_unit_flag_mode unit_flag_mode, asc_quant_mode quant_pre_mode,
    asc_relu_pre_mode relu_pre_mode, bool enable_channel_split, bool enable_nz2nd, bool enable_nz2dn,
    bool enable_clip_relu_pre)
{
    if ASC_IS_AIC {
        copy_matrix_cc_to_ub_s4(
            (__ubuf__ void*)dst, src, n_size, m_size, dst_stride, src_stride, 0, false, quant_pre_rnd,
            enable_clip_relu_pre, static_cast<uint8_t>(unit_flag_mode), quant_pre_mode,
            static_cast<uint8_t>(relu_pre_mode), enable_channel_split, enable_nz2nd,
            static_cast<QuantMode_post>(QUANT_POST_DEFAULT), RELU_POST_DEFAULT, CLIP_RELU_POST_DEFAULT, false,
            ELTWISE_OP_DEFAULT, false, false, false, false, false, enable_nz2dn);
    }
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ void* dst, __gm__ void* src, uint32_t burst_count, uint32_t burst_len, asc_channel_pad_mode pad_mode,
    bool enable_pre_load_ignore_sync, uint64_t src_stride, uint32_t dst_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_v2(
            dst, src, 0, burst_count, burst_len, static_cast<uint8_t>(pad_mode), 0, src_stride, dst_stride,
            enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf_2dv2(
            dst, src, m_start_position, k_start_position, dst_stride, m_step, k_step, ASC_GM2L1_SID_DEFAULT,
            ASC_GM2L1_DECOMP_MODE_DEFAULT, enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ float* dst, __gm__ float* src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride,
    uint16_t m_step, uint16_t k_step, bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf_2dv2(
            dst, src, m_start_position, k_start_position, dst_stride, m_step, k_step, ASC_GM2L1_SID_DEFAULT,
            ASC_GM2L1_DECOMP_MODE_DEFAULT, enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf_2dv2(
            dst, src, m_start_position, k_start_position, dst_stride, m_step, k_step, ASC_GM2L1_SID_DEFAULT,
            ASC_GM2L1_DECOMP_MODE_DEFAULT, enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf_2dv2(
            dst, src, m_start_position, k_start_position, dst_stride, m_step, k_step, ASC_GM2L1_SID_DEFAULT,
            ASC_GM2L1_DECOMP_MODE_DEFAULT, enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ half* dst, __gm__ half* src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride,
    uint16_t m_step, uint16_t k_step, bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf_2dv2(
            dst, src, m_start_position, k_start_position, dst_stride, m_step, k_step, ASC_GM2L1_SID_DEFAULT,
            ASC_GM2L1_DECOMP_MODE_DEFAULT, enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf_2dv2(
            dst, src, m_start_position, k_start_position, dst_stride, m_step, k_step, ASC_GM2L1_SID_DEFAULT,
            ASC_GM2L1_DECOMP_MODE_DEFAULT, enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ int16_t* dst, __gm__ int16_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf_2dv2(
            dst, src, m_start_position, k_start_position, dst_stride, m_step, k_step, ASC_GM2L1_SID_DEFAULT,
            ASC_GM2L1_DECOMP_MODE_DEFAULT, enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ int32_t* dst, __gm__ int32_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf_2dv2(
            dst, src, m_start_position, k_start_position, dst_stride, m_step, k_step, ASC_GM2L1_SID_DEFAULT,
            ASC_GM2L1_DECOMP_MODE_DEFAULT, enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ int8_t* dst, __gm__ int8_t* src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride,
    uint16_t m_step, uint16_t k_step, bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf_2dv2(
            dst, src, m_start_position, k_start_position, dst_stride, m_step, k_step, ASC_GM2L1_SID_DEFAULT,
            ASC_GM2L1_DECOMP_MODE_DEFAULT, enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf_2dv2(
            dst, src, m_start_position, k_start_position, dst_stride, m_step, k_step, ASC_GM2L1_SID_DEFAULT,
            ASC_GM2L1_DECOMP_MODE_DEFAULT, enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf_2dv2(
            dst, src, m_start_position, k_start_position, dst_stride, m_step, k_step, ASC_GM2L1_SID_DEFAULT,
            ASC_GM2L1_DECOMP_MODE_DEFAULT, enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf_2dv2(
            dst, src, m_start_position, k_start_position, dst_stride, m_step, k_step, ASC_GM2L1_SID_DEFAULT,
            ASC_GM2L1_DECOMP_MODE_DEFAULT, enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ fp4x2_e1m2_t* dst, __gm__ fp4x2_e1m2_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf_2dv2_s4(
            dst, src, m_start_position, k_start_position, dst_stride, m_step, k_step, ASC_GM2L1_SID_DEFAULT,
            ASC_GM2L1_DECOMP_MODE_DEFAULT, enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ fp4x2_e2m1_t* dst, __gm__ fp4x2_e2m1_t* src, uint32_t m_start_position, uint32_t k_start_position,
    uint16_t dst_stride, uint16_t m_step, uint16_t k_step, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf_2dv2_s4(
            dst, src, m_start_position, k_start_position, dst_stride, m_step, k_step, ASC_GM2L1_SID_DEFAULT,
            ASC_GM2L1_DECOMP_MODE_DEFAULT, enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_copy_gm2l1(
    __cbuf__ void* dst, __gm__ void* src, uint32_t m_start_position, uint32_t k_start_position, uint16_t dst_stride,
    uint16_t m_step, uint16_t k_step, bool enable_pre_load_ignore_sync, asc_load_l2_cache_mode l2_cache_mode)
{
    if ASC_IS_AIC {
        load_gm_to_cbuf_2dv2_s4(
            dst, src, m_start_position, k_start_position, dst_stride, m_step, k_step, ASC_GM2L1_SID_DEFAULT,
            ASC_GM2L1_DECOMP_MODE_DEFAULT, enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode));
    }
}

__aicore__ inline void asc_copy_gm2l1_align(
    __cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_count,
    uint8_t right_padding_count, bool enable_data_select, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_count, right_padding_count, enable_data_select,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride);
    }
}

__aicore__ inline void asc_copy_gm2l1_align(
    __cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool enable_data_select, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_count, right_padding_count, enable_data_select,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride);
    }
}

__aicore__ inline void asc_copy_gm2l1_align(
    __cbuf__ half* dst, __gm__ half* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_count,
    uint8_t right_padding_count, bool enable_data_select, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_count, right_padding_count, enable_data_select,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride);
    }
}

__aicore__ inline void asc_copy_gm2l1_align(
    __cbuf__ float* dst, __gm__ float* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_count,
    uint8_t right_padding_count, bool enable_data_select, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_count, right_padding_count, enable_data_select,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride);
    }
}

__aicore__ inline void asc_copy_gm2l1_align(
    __cbuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint32_t burst_count, uint32_t burst_len,
    uint8_t left_padding_count, uint8_t right_padding_count, bool enable_data_select, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_count, right_padding_count, enable_data_select,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride);
    }
}

__aicore__ inline void asc_copy_gm2l1_align(
    __cbuf__ int16_t* dst, __gm__ int16_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_count,
    uint8_t right_padding_count, bool enable_data_select, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_count, right_padding_count, enable_data_select,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride);
    }
}

__aicore__ inline void asc_copy_gm2l1_align(
    __cbuf__ int32_t* dst, __gm__ int32_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_count,
    uint8_t right_padding_count, bool enable_data_select, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_count, right_padding_count, enable_data_select,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride);
    }
}

__aicore__ inline void asc_copy_gm2l1_align(
    __cbuf__ int8_t* dst, __gm__ int8_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_count,
    uint8_t right_padding_count, bool enable_data_select, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_count, right_padding_count, enable_data_select,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride);
    }
}

__aicore__ inline void asc_copy_gm2l1_align(
    __cbuf__ uint16_t* dst, __gm__ uint16_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_count,
    uint8_t right_padding_count, bool enable_data_select, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_count, right_padding_count, enable_data_select,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride);
    }
}

__aicore__ inline void asc_copy_gm2l1_align(
    __cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint32_t burst_count, uint32_t burst_len, uint8_t left_padding_count,
    uint8_t right_padding_count, bool enable_data_select, bool enable_pre_load_ignore_sync,
    asc_load_l2_cache_mode l2_cache_mode, uint64_t src_stride, uint32_t dst_stride)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_align_v2(
            dst, src, 0, burst_count, burst_len, left_padding_count, right_padding_count, enable_data_select,
            enable_pre_load_ignore_sync, static_cast<uint8_t>(l2_cache_mode), src_stride, dst_stride);
    }
}

__aicore__ inline void asc_copy_gm2l1_dn2nz(
    __cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode,
    uint16_t n_value, uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0,
    bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_dn2nz(
    __cbuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode,
    uint16_t n_value, uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0,
    bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_dn2nz(
    __cbuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode,
    uint16_t n_value, uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0,
    bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_dn2nz(
    __cbuf__ half* dst, __gm__ half* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode, uint16_t n_value,
    uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0, bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_dn2nz(
    __cbuf__ float* dst, __gm__ float* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode,
    uint16_t n_value, uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0,
    bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_dn2nz(
    __cbuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode,
    uint16_t n_value, uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0,
    bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_dn2nz(
    __cbuf__ int16_t* dst, __gm__ int16_t* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode,
    uint16_t n_value, uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0,
    bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_dn2nz(
    __cbuf__ int32_t* dst, __gm__ int32_t* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode,
    uint16_t n_value, uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0,
    bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_dn2nz(
    __cbuf__ int8_t* dst, __gm__ int8_t* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode,
    uint16_t n_value, uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0,
    bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_dn2nz(
    __cbuf__ uint16_t* dst, __gm__ uint16_t* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode,
    uint16_t n_value, uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0,
    bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_dn2nz(
    __cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode,
    uint16_t n_value, uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0,
    bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_dn2nz(
    __cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode,
    uint16_t n_value, uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0,
    bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_dn2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_nd2nz(
    __cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode,
    uint16_t n_value, uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0,
    bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_nd2nz(
    __cbuf__ fp8_e4m3fn_t* dst, __gm__ fp8_e4m3fn_t* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode,
    uint16_t n_value, uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0,
    bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_nd2nz(
    __cbuf__ fp8_e5m2_t* dst, __gm__ fp8_e5m2_t* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode,
    uint16_t n_value, uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0,
    bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_nd2nz(
    __cbuf__ half* dst, __gm__ half* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode, uint16_t n_value,
    uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0, bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_nd2nz(
    __cbuf__ float* dst, __gm__ float* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode,
    uint16_t n_value, uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0,
    bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_nd2nz(
    __cbuf__ hifloat8_t* dst, __gm__ hifloat8_t* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode,
    uint16_t n_value, uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0,
    bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_nd2nz(
    __cbuf__ int16_t* dst, __gm__ int16_t* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode,
    uint16_t n_value, uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0,
    bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_nd2nz(
    __cbuf__ int32_t* dst, __gm__ int32_t* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode,
    uint16_t n_value, uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0,
    bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_nd2nz(
    __cbuf__ int8_t* dst, __gm__ int8_t* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode,
    uint16_t n_value, uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0,
    bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_nd2nz(
    __cbuf__ uint16_t* dst, __gm__ uint16_t* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode,
    uint16_t n_value, uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0,
    bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_nd2nz(
    __cbuf__ uint32_t* dst, __gm__ uint32_t* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode,
    uint16_t n_value, uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0,
    bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

__aicore__ inline void asc_copy_gm2l1_nd2nz(
    __cbuf__ uint8_t* dst, __gm__ uint8_t* src, uint64_t src_d_value, asc_load_l2_cache_mode l2_cache_mode,
    uint16_t n_value, uint32_t d_value, uint64_t src_nd_matrix_stride, bool enable_small_c0,
    bool enable_pre_load_ignore_sync)
{
    if ASC_IS_AIC {
        copy_gm_to_cbuf_multi_nd2nz(
            dst, src, 0, src_d_value, static_cast<uint8_t>(l2_cache_mode), n_value, d_value, src_nd_matrix_stride,
            enable_small_c0, enable_pre_load_ignore_sync);
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
