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
    "impl/c_api/memory_base_impl/utils_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_MEMORY_BASE_IMPL_UTILS_IMPL_H
#define IMPL_C_API_MEMORY_BASE_IMPL_UTILS_IMPL_H

#include "c_api/defs/defs.h"
#include <cstdint>
#include <type_traits>
#include "utils/base/sys_macros.h"
#include "utils/base/sys_constants.h"

constexpr uint32_t C_API_AIC_TYPE = AscendC::AIC;

constexpr uint32_t C_API_AIV_TYPE = AscendC::AIV;

constexpr uint32_t C_API_MIX_TYPE = AscendC::MIX;

#define ASC_IS_AIV ASCEND_IS_AIV

#define ASC_IS_AIC ASCEND_IS_AIC

#define ASC_IS_NOT_AIV ASCEND_IS_NOT_AIV

#define ASC_IS_NOT_AIC ASCEND_IS_NOT_AIC

struct asc_c_api_default_repeat {
    uint8_t U8 = 1;
};

struct asc_c_api_default_block_stride {
    uint8_t U8 = 1;
    uint16_t U16 = 1;
};

struct asc_c_api_default_repeat_stride {
    uint8_t U8 = 8;
    uint16_t U16 = 8;
};

struct asc_c_api_half_default_repeat_stride {
    uint8_t U8 = 4;
    uint16_t U16 = 4;
};

struct asc_c_api_one_fourth_default_repeat_stride {
    uint8_t U8 = 2;
    uint16_t U16 = 2;
};

constexpr asc_c_api_default_repeat ASC_C_API_DEFAULT_REPEAT;

constexpr asc_c_api_default_block_stride ASC_C_API_DEFAULT_BLOCK_STRIDE;

constexpr asc_c_api_default_repeat_stride ASC_C_API_DEFAULT_REPEAT_STRIDE;

constexpr asc_c_api_half_default_repeat_stride ASC_C_API_HALF_DEFAULT_REPEAT_STRIDE;

constexpr asc_c_api_one_fourth_default_repeat_stride ASC_C_API_ONE_FOURTH_DEFAULT_REPEAT_STRIDE;

constexpr uint16_t ASC_C_API_DATABLOCK_NUM = 8;

constexpr uint16_t ASC_C_API_ONE_DATABLOCK_SIZE = 32;

constexpr uint16_t ASC_C_API_REDUCE_DEFAULT_REPEAT_STRIDE = 1;

using order_t = Order_t;

union asc_sparse_addr_config {
    uint64_t config;
    struct {
        uint64_t src : 32;
        uint64_t index : 32;
    };
};

__aicore__ inline void asc_set_mask_count_begin(uint32_t count)
{
    if ASC_IS_AIV {
        set_mask_count();
        set_vector_mask(static_cast<uint64_t>(0), static_cast<uint64_t>(count));
    }
}

__aicore__ inline void asc_set_mask_count_end()
{
    if ASC_IS_AIV {
        set_mask_norm();
    }
}

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

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
