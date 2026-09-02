/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file asc_pack_impl.h
 * \brief
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#pragma message( \
    "impl/c_api/instr_impl/npu_arch_3510/vector_compute_impl/asc_pack_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"c_api/asc_simd.h\"\" and use public functions or variables defined in interface headers files.")
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_CAPI_INSTR_IMPL_NPU_ARCH_3510_VECTOR_COMPUTE_IMPL_ASC_PACK_IMPL_H
#define IMPL_CAPI_INSTR_IMPL_NPU_ARCH_3510_VECTOR_COMPUTE_IMPL_ASC_PACK_IMPL_H

#include "impl/c_api/instr_impl/npu_arch_3510/utils_impl.h"

__simd_callee__ inline void asc_pack_impl(vector_uint8_t& dst, vector_uint16_t src)
{
    if ASC_IS_AIV {
        vpack(dst, src, LOWER, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_pack_impl(vector_uint8_t& dst, vector_int16_t src)
{
    if ASC_IS_AIV {
        vpack(dst, src, LOWER, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_pack_impl(vector_uint16_t& dst, vector_uint32_t src)
{
    if ASC_IS_AIV {
        vpack(dst, src, LOWER, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_pack_impl(vector_uint16_t& dst, vector_int32_t src)
{
    if ASC_IS_AIV {
        vpack(dst, src, LOWER, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_pack_impl(vector_bool& dst, vector_bool src)
{
    if ASC_IS_AIV {
        ppack(dst, src, LOWER);
    }
}

__simd_callee__ inline void asc_pack_v2_impl(vector_uint8_t& dst, vector_uint16_t src)
{
    if ASC_IS_AIV {
        vpack(dst, src, HIGHER, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_pack_v2_impl(vector_uint8_t& dst, vector_int16_t src)
{
    if ASC_IS_AIV {
        vpack(dst, src, HIGHER, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_pack_v2_impl(vector_uint16_t& dst, vector_uint32_t src)
{
    if ASC_IS_AIV {
        vpack(dst, src, HIGHER, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_pack_v2_impl(vector_uint16_t& dst, vector_int32_t src)
{
    if ASC_IS_AIV {
        vpack(dst, src, HIGHER, MODE_UNKNOWN);
    }
}

__simd_callee__ inline void asc_pack_v2_impl(vector_bool& dst, vector_bool src)
{
    if ASC_IS_AIV {
        ppack(dst, src, HIGHER);
    }
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
