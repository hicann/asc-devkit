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
    "impl/c_api/reg_base_impl/reg_mask_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_REG_MASK_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_REG_MASK_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

__simd_callee__ inline vector_bool asc_update_mask_b8(uint32_t& value)
{
    if ASC_IS_AIV {
        return plt_b8(value, POST_UPDATE);
    }
    return vector_bool{};
}

__simd_callee__ inline vector_bool asc_update_mask_b16(uint32_t& value)
{
    if ASC_IS_AIV {
        return plt_b16(value, POST_UPDATE);
    }
    return vector_bool{};
}

__simd_callee__ inline vector_bool asc_update_mask_b32(uint32_t& value)
{
    if ASC_IS_AIV {
        return plt_b32(value, POST_UPDATE);
    }
    return vector_bool{};
}

__simd_callee__ inline vector_bool asc_get_mask_spr_b16() { return movp_b16(); }

__simd_callee__ inline vector_bool asc_get_mask_spr_b32() { return movp_b32(); }

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
