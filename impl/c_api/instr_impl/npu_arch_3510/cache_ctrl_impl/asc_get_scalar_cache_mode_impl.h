/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software; you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file asc_get_scalar_cache_mode_impl.h
 * \brief
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/c_api/instr_impl/npu_arch_3510/cache_ctrl_impl/asc_get_scalar_cache_mode_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_CACHE_CTRL_IMPL_ASC_GET_SCALAR_CACHE_MODE_IMPL_H
#define IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_CACHE_CTRL_IMPL_ASC_GET_SCALAR_CACHE_MODE_IMPL_H

#include "impl/c_api/instr_impl/npu_arch_3510/utils_impl.h"

// Enum underlying values match HW encodings; cast is used as-is (illegal codes are not normalized).
__aicore__ inline asc_load_l2_cache_mode asc_get_scalar_load_cache_mode_impl()
{
    constexpr uint8_t start_bit = 16; // READ mode: CTRL[19:16]
    uint8_t cache_bits = static_cast<uint8_t>((get_ctrl() >> start_bit) & 0xF);
    return static_cast<asc_load_l2_cache_mode>(cache_bits);
}

__aicore__ inline asc_store_l2_cache_mode asc_get_scalar_store_cache_mode_impl()
{
    constexpr uint8_t start_bit = 20; // WRITE mode: CTRL[23:20]
    uint8_t cache_bits = static_cast<uint8_t>((get_ctrl() >> start_bit) & 0xF);
    return static_cast<asc_store_l2_cache_mode>(cache_bits);
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
