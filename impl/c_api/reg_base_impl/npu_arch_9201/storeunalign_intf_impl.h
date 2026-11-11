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
    "impl/c_api/reg_base_impl/npu_arch_9201/storeunalign_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_9201_STOREUNALIGN_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_9201_STOREUNALIGN_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

__simd_callee__ inline void asc_storeunalign(
    __ubuf__ fp8_e6m2_t* dst, vector_store_unalign& src0, vector_fp8_e6m2_t src1, uint32_t count)
{
    if ASC_IS_AIV {
        vstus(src0, count, src1, dst);
    }
}

__simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e6m2_t* dst, vector_store_unalign src, int32_t offset)
{
    if ASC_IS_AIV {
        vstas(src, dst, offset);
    }
}

__simd_callee__ inline void asc_storeunalign_post(__ubuf__ fp8_e6m2_t* dst, vector_store_unalign src, addr_reg offset)
{
    if ASC_IS_AIV {
        vsta(src, dst, offset);
    }
}

__simd_callee__ inline void asc_storeunalign_post_postupdate(
    __ubuf__ fp8_e6m2_t*& dst, vector_store_unalign src, int32_t offset)
{
    if ASC_IS_AIV {
        vstas(src, dst, offset, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ fp8_e6m2_t*& dst, vector_store_unalign& src0, vector_fp8_e6m2_t src1, uint32_t count)
{
    if ASC_IS_AIV {
        vstus(src0, count, src1, dst, POST_UPDATE);
    }
}

__simd_callee__ inline void asc_storeunalign_postupdate(
    __ubuf__ fp8_e6m2_t* dst, vector_store_unalign& src0, vector_fp8_e6m2_t src1, addr_reg& count)
{
    if ASC_IS_AIV {
        vstu(src0, count, src1, dst, POST_UPDATE);
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
