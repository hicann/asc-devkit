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
    "impl/c_api/memory_base_impl/scalar_atomic_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_MEMORY_BASE_IMPL_SCALAR_ATOMIC_INTF_IMPL_H
#define IMPL_C_API_MEMORY_BASE_IMPL_SCALAR_ATOMIC_INTF_IMPL_H

ASC_DEPRECATED(9.1.0, "2027/09/07", asc_atomic_add)
__aicore__ inline void asc_get_store_atomic_config(asc_store_atomic_config& config)
{
    config.config = get_st_atomic_cfg();
}

__aicore__ inline void asc_set_store_atomic_config_v1(uint16_t type, uint16_t op)
{
    constexpr uint64_t type_mask = 0x7;
    constexpr uint64_t op_mask = 0x3;
    constexpr uint64_t op_bit = 3;
    uint64_t config = (type & type_mask) | ((op & op_mask) << op_bit);
    set_st_atomic_cfg(config);
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
