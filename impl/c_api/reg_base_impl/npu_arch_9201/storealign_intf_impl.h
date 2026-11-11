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
    "impl/c_api/reg_base_impl/npu_arch_9201/storealign_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_9201_STOREALIGN_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_9201_STOREALIGN_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

__simd_callee__ inline void asc_storealign(__ubuf__ fp8_e6m2_t* dst, vector_fp8_e6m2_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vsts(src, dst, 0, NORM_B8, mask);
    }
}

__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e6m2_t* dst, vector_fp8_e6m2_t src, addr_reg offset, vector_bool mask)
{
    if ASC_IS_AIV {
        vst(src, dst, offset, NORM_B8, mask);
    }
}

__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e6m2_t* dst, vector_fp8_e6m2_t src, int32_t offset, vector_bool mask)
{
    if ASC_IS_AIV {
        vsts(src, dst, offset, NORM_B8, mask);
    }
}

__simd_callee__ inline void asc_storealign(
    __ubuf__ fp8_e6m2_t* dst, vector_fp8_e6m2_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
{
    if ASC_IS_AIV {
        asc_store_align_config config;
        config.block_stride = block_stride;
        config.repeat_stride = repeat_stride;
        vsstb(src, dst, config.config, mask);
    }
}

__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e6m2_t* dst, vector_fp8_e6m2_t src)
{
    if ASC_IS_AIV {
        vector_bool mask;
        vsts(src, dst, 0, ONEPT_B8, mask);
    }
}

__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e6m2_t* dst, vector_fp8_e6m2_t src, addr_reg offset)
{
    if ASC_IS_AIV {
        vector_bool mask;
        vst(src, dst, offset, ONEPT_B8, mask);
    }
}

__simd_callee__ inline void asc_storealign_1st(__ubuf__ fp8_e6m2_t* dst, vector_fp8_e6m2_t src, int32_t offset)
{
    if ASC_IS_AIV {
        vector_bool mask;
        vsts(src, dst, offset, ONEPT_B8, mask);
    }
}

__simd_callee__ inline void asc_storealign_1st_postupdate(
    __ubuf__ fp8_e6m2_t*& dst, vector_fp8_e6m2_t src, int32_t offset)
{
    if ASC_IS_AIV {
        vector_bool mask;
        vsts(src, dst, offset, ONEPT_B8, mask, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e6m2_t* dst, vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1)
{
    if ASC_IS_AIV {
        vector_bool mask;
        vsts(src0, src1, dst, 0, INTLV_B8, mask);
    }
}

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e6m2_t* dst, vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1, addr_reg offset)
{
    if ASC_IS_AIV {
        vector_bool mask;
        vst(src0, src1, dst, offset, INTLV_B8, mask);
    }
}

__simd_callee__ inline void asc_storealign_intlv(
    __ubuf__ fp8_e6m2_t* dst, vector_fp8_e6m2_t src0, vector_fp8_e6m2_t src1, int32_t offset)
{
    if ASC_IS_AIV {
        vector_bool mask;
        vsts(src0, src1, dst, offset, INTLV_B8, mask);
    }
}

__simd_callee__ inline void asc_storealign_postupdate(
    __ubuf__ fp8_e6m2_t*& dst, vector_fp8_e6m2_t src, uint16_t block_stride, uint16_t repeat_stride, vector_bool mask)
{
    if ASC_IS_AIV {
        asc_store_align_config_post config;
        config.block_stride = block_stride;
        config.repeat_stride = repeat_stride;
        vsstb(src, dst, config.config, mask, POST_UPDATE);
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
