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
 * \file asc_set_scalar_cache_mode_impl.h
 * \brief
 */

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#pragma message( \
    "impl/c_api/instr_impl/npu_arch_3510/cache_ctrl_impl/asc_set_scalar_cache_mode_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"c_api/asc_simd.h\"\" and use public functions or variables defined in interface headers files.")
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_CACHE_CTRL_IMPL_ASC_SET_SCALAR_CACHE_MODE_IMPL_H
#define IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_CACHE_CTRL_IMPL_ASC_SET_SCALAR_CACHE_MODE_IMPL_H

#include "impl/c_api/instr_impl/npu_arch_3510/utils_impl.h"

union ctrl_scalar_cache_bits {
    uint64_t value;
    struct {
        uint64_t reserved0 : 16;
        uint64_t scalar_load_cache_mode : 4;
        uint64_t scalar_store_cache_mode : 4;
        uint64_t reserved1 : 40;
    };
};

__aicore__ inline void asc_set_scalar_cache_mode_impl(asc_load_l2_cache_mode l2_cache_mode)
{
    ctrl_scalar_cache_bits ctrl;
    ctrl.value = static_cast<uint64_t>(get_ctrl());
    ctrl.scalar_load_cache_mode = static_cast<uint8_t>(l2_cache_mode);
    set_ctrl(static_cast<int64_t>(ctrl.value));
}

__aicore__ inline void asc_set_scalar_cache_mode_impl(asc_store_l2_cache_mode l2_cache_mode)
{
    ctrl_scalar_cache_bits ctrl;
    ctrl.value = static_cast<uint64_t>(get_ctrl());
    ctrl.scalar_store_cache_mode = static_cast<uint8_t>(l2_cache_mode);
    set_ctrl(static_cast<int64_t>(ctrl.value));
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
