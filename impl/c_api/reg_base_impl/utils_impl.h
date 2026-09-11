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
    "impl/c_api/reg_base_impl/utils_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_UTILS_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_UTILS_IMPL_H

#include "c_api/defs/defs.h"
#include <cstdint>
#include <type_traits>
#include "impl/utils/sys_macros.h"
#include "utils/base/sys_constants.h"

constexpr uint32_t C_API_AIC_TYPE = AscendC::AIC;

constexpr uint32_t C_API_AIV_TYPE = AscendC::AIV;

constexpr uint32_t C_API_MIX_TYPE = AscendC::MIX;

#define ASC_IS_AIV ASCEND_IS_AIV

#define ASC_IS_AIC ASCEND_IS_AIC

constexpr uint16_t ASC_C_API_ONE_DATABLOCK_SIZE = 32;

__aicore__ inline void asc_sync_post_process() { pipe_barrier(pipe_t::PIPE_ALL); }

union asc_capi_fpc_reg_config {
    uint64_t config;
    struct {
        uint64_t relu_units : 8;
        uint64_t quant_units : 8;
        uint64_t reserved : 47;
        uint64_t unit_flag : 1;
    };
};

union asc_scalar_bitcode {
    __aicore__ asc_scalar_bitcode() {}
    half input_half;
    int16_t input_int16_t;
    uint16_t input_uint16_t;
    uint64_t output;
};

union asc_3d_padding_bitcode {
    __aicore__ asc_3d_padding_bitcode() {}
    half input_half;
    bfloat16_t input_bfloat16;
    int16_t input_int16;
    uint16_t input_uint16;
    int32_t input_int32;
    uint32_t input_uint32;
    float input_float;
    uint64_t output;
};

union asc_gm2l1_loop_size_config {
    uint64_t config;
    struct {
        uint64_t loop1_size : 21;
        uint64_t loop2_size : 42;
    };
};

union asc_gm2l1_loop_stride_config {
    uint64_t config;
    struct {
        uint64_t src_stride : 40;
        uint64_t dst_stride : 24;
    };
};

union asc_gm2ub_loop_size_config {
    uint64_t config;
    struct {
        uint64_t loop1_size : 21;
        uint64_t loop2_size : 42;
    };
};

union asc_gm2ub_loop_stride_config {
    uint64_t config;
    struct {
        uint64_t src_stride : 40;
        uint64_t dst_stride : 24;
    };
};

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
