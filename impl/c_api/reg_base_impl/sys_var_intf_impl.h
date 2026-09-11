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
    "impl/c_api/reg_base_impl/sys_var_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "c_api/asc_simd.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_REG_BASE_IMPL_SYS_VAR_INTF_IMPL_H
#define IMPL_C_API_REG_BASE_IMPL_SYS_VAR_INTF_IMPL_H

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/reg_base_impl/npu_arch_3510/sys_var_intf_impl.h"
#endif

constexpr int64_t SYSTEM_CLOCK_FREQ_MHZ = 1000;

__aicore__ inline int64_t asc_get_program_counter() { return get_pc(); }

__aicore__ inline int64_t asc_get_sub_block_num() { return get_subblockdim(); }

__aicore__ inline int64_t asc_get_sub_block_id() { return get_subblockid(); }

__aicore__ inline int64_t asc_get_system_cycle() { return get_sys_cnt(); }

__aicore__ inline int64_t asc_get_system_clock() { return get_sys_cnt() / SYSTEM_CLOCK_FREQ_MHZ; }

__aicore__ inline constexpr int64_t asc_get_vf_len()
{
    constexpr int64_t VECTOR_REG_WIDTH_3510 = 256;
    return VECTOR_REG_WIDTH_3510;
}

__aicore__ inline uint64_t asc_get_phy_buf_addr(uint64_t offset) { return get_imm(offset); }

__aicore__ inline void asc_get_arch_ver(uint32_t& core_version) { core_version = 3510U; }

__aicore__ inline int64_t asc_get_core_id() { return get_coreid(); }

__aicore__ inline int64_t asc_get_smmu_tag_version() { return get_smmu_tag_ver(); }

__aicore__ inline int64_t asc_get_phy_stack_base() { return get_stack_phy_base(); }

__aicore__ inline int64_t asc_get_status() { return get_status(); }

__aicore__ inline int64_t asc_get_sys_virtual_base() { return get_sys_va_base(); }

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
