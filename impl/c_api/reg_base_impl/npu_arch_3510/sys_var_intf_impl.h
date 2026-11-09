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
    "impl/c_api/reg_base_impl/npu_arch_3510/sys_var_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_3510_SYS_VAR_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_NPU_ARCH_3510_SYS_VAR_INTF_IMPL_H

ASC_DEPRECATED(9.2.0, "2027/09/07", block_idx and asc_get_sub_block_num and asc_get_sub_block_id)
__aicore__ inline int64_t asc_get_block_idx()
{
#if defined(__ASCENDC_ENABLE_SUPER_KERNEL__) && defined(ASCENDC_C_API_SK_LOGICAL_BLOCK_IDX) && \
    ASCENDC_C_API_SK_LOGICAL_BLOCK_IDX
    if ASC_IS_AIV {
        return get_block_idx() * get_subblockdim() + get_subblockid();
    }
#endif
    return get_block_idx();
}

ASC_DEPRECATED(9.2.0, "2027/09/07", block_num)
__aicore__ inline int64_t asc_get_block_num()
{
#ifdef __SUPER_KERNEL_STATIC_BLOCK_NUM__
    return __SUPER_KERNEL_STATIC_BLOCK_NUM__;
#elif defined(__SUPER_KERNEL_DYNAMIC_BLOCK_NUM__)
    return g_super_kernel_dynamic_block_num;
#else
    return get_block_num();
#endif
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
