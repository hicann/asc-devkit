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
    "impl/c_api/reg_base_impl/npu_arch_3510/storealign_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_3510_STOREALIGN_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_3510_STOREALIGN_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter)
__simd_callee__ inline void asc_storealign_pack_v2(__ubuf__ int32_t* dst_align32b, vector_int32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vsts(src, dst_align32b, 0, PK4_B32, mask);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter)
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ uint32_t* dst_align32b, vector_uint32_t src, vector_bool mask)
{
    if ASC_IS_AIV {
        vsts(src, dst_align32b, 0, PK4_B32, mask);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter)
__simd_callee__ inline void asc_storealign_pack_v2(__ubuf__ float* dst_align32b, vector_float src, vector_bool mask)
{
    if ASC_IS_AIV {
        vsts(src, dst_align32b, 0, PK4_B32, mask);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter)
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ int32_t* dst_align32b, vector_int32_t src, addr_reg offset, vector_bool mask)
{
    if ASC_IS_AIV {
        vst(src, dst_align32b, offset, PK4_B32, mask);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter)
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ uint32_t* dst_align32b, vector_uint32_t src, addr_reg offset, vector_bool mask)
{
    if ASC_IS_AIV {
        vst(src, dst_align32b, offset, PK4_B32, mask);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter)
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ float* dst_align32b, vector_float src, addr_reg offset, vector_bool mask)
{
    if ASC_IS_AIV {
        vst(src, dst_align32b, offset, PK4_B32, mask);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter)
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ int32_t* dst_align32b, vector_int32_t src, int32_t offset, vector_bool mask)
{
    if ASC_IS_AIV {
        vsts(src, dst_align32b, offset, PK4_B32, mask);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter)
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ uint32_t* dst_align32b, vector_uint32_t src, int32_t offset, vector_bool mask)
{
    if ASC_IS_AIV {
        vsts(src, dst_align32b, offset, PK4_B32, mask);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter)
__simd_callee__ inline void asc_storealign_pack_v2(
    __ubuf__ float* dst_align32b, vector_float src, int32_t offset, vector_bool mask)
{
    if ASC_IS_AIV {
        vsts(src, dst_align32b, offset, PK4_B32, mask);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter_postupdate)
__simd_callee__ inline void asc_storealign_pack_postupdate_v2(
    __ubuf__ int32_t*& dst_align32b, vector_int32_t src, int32_t offset, vector_bool mask)
{
    if ASC_IS_AIV {
        vsts(src, dst_align32b, offset, PK4_B32, mask, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter_postupdate)
__simd_callee__ inline void asc_storealign_pack_postupdate_v2(
    __ubuf__ uint32_t*& dst_align32b, vector_uint32_t src, int32_t offset, vector_bool mask)
{
    if ASC_IS_AIV {
        vsts(src, dst_align32b, offset, PK4_B32, mask, POST_UPDATE);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_storealign_pack_quarter_postupdate)
__simd_callee__ inline void asc_storealign_pack_postupdate_v2(
    __ubuf__ float*& dst_align32b, vector_float src, int32_t offset, vector_bool mask)
{
    if ASC_IS_AIV {
        vsts(src, dst_align32b, offset, PK4_B32, mask, POST_UPDATE);
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
