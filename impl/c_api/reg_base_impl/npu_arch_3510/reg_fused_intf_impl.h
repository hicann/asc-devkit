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
    "impl/c_api/reg_base_impl/npu_arch_3510/reg_fused_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_3510_REG_FUSED_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_3510_REG_FUSED_INTF_IMPL_H

#include "impl/c_api/reg_base_impl/utils_impl.h"

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_mul_scalar_float2half_rn)
__simd_callee__ inline void asc_muls_v2(vector_half& dst, vector_float src, float value, vector_bool mask)
{
    if ASC_IS_AIV {
        vmulscvt(dst, src, value, mask, PART_ODD);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_exp_sub_half2float)
__simd_callee__ inline void asc_exp_sub(vector_float& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vexpdif(dst, src0, src1, mask, PART_EVEN);
    }
}

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_exp_sub_half2float)
__simd_callee__ inline void asc_exp_sub_v2(vector_float& dst, vector_half src0, vector_half src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vexpdif(dst, src0, src1, mask, PART_ODD);
    }
}

ASC_DEPRECATED(9.1.0, "2027/09/07", asc_exp_sub)
__simd_callee__ inline void asc_exp_sub_v2(vector_float& dst, vector_float src0, vector_float src1, vector_bool mask)
{
    if ASC_IS_AIV {
        vexpdif(dst, src0, src1, mask, PART_ODD);
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
