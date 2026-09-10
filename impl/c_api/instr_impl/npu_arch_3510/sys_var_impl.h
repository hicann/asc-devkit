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
#pragma message( \
    "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please include c_api/asc_simd.h and use public functions or variables defined in interface header files.")
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

#ifndef IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_SYS_VAR_IMPL_H
#define IMPL_C_API_INSTR_IMPL_NPU_ARCH_3510_SYS_VAR_IMPL_H

#include "c_api/defs/defs.h"
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_get_ar_spr_impl.h"
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_get_program_counter_impl.h"
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_set_ctrl_impl.h"
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_get_block_idx_impl.h"
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_get_block_num_impl.h"
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_get_system_cycle_impl.h"
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_get_ctrl_impl.h"
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_get_phy_buf_addr_impl.h"
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_get_vf_len_impl.h"
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_get_arch_ver_impl.h"
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_get_core_id_impl.h"
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_get_smmu_tag_version_impl.h"
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_get_phy_stack_base_impl.h"
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_get_status_impl.h"
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_get_sub_block_id_impl.h"
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_get_sub_block_num_impl.h"
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_get_sys_virtual_base_impl.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_set_saturation_flag_impl.h"
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_get_saturation_flag_impl.h"
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_set_saturation_strategy_impl.h"
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl/asc_get_saturation_strategy_impl.h"
#endif

__aicore__ inline int64_t asc_get_squeeze_status() { return asc_get_squeeze_status_impl(); }

__aicore__ inline int64_t asc_get_program_counter() { return asc_get_program_counter_impl(); }

__aicore__ inline int64_t asc_get_sub_block_num() { return asc_get_sub_block_num_impl(); }

__aicore__ inline int64_t asc_get_sub_block_id() { return asc_get_sub_block_id_impl(); }

__aicore__ inline int64_t asc_get_system_cycle() { return asc_get_system_cycle_impl(); }

__aicore__ inline int64_t asc_get_system_clock() { return asc_get_system_clock_impl(); }

__aicore__ inline void asc_set_ctrl(uint64_t config) { asc_set_ctrl_impl(config); }

__aicore__ inline int64_t asc_get_ctrl() { return asc_get_ctrl_impl(); }

__aicore__ inline constexpr int64_t asc_get_vf_len() { return asc_get_vf_len_impl(); }

__aicore__ inline uint64_t asc_get_phy_buf_addr(uint64_t offset) { return asc_get_phy_buf_addr_impl(offset); }

__aicore__ inline void asc_get_arch_ver(uint32_t& core_version) { asc_get_arch_ver_impl(core_version); }

__aicore__ inline int64_t asc_get_core_id() { return asc_get_core_id_impl(); }

__aicore__ inline int64_t asc_get_smmu_tag_version() { return asc_get_smmu_tag_version_impl(); }

__aicore__ inline int64_t asc_get_phy_stack_base() { return asc_get_phy_stack_base_impl(); }

__aicore__ inline int64_t asc_get_status() { return asc_get_status_impl(); }

__aicore__ inline int64_t asc_get_sys_virtual_base() { return asc_get_sys_virtual_base_impl(); }

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
__aicore__ inline void asc_set_saturation_flag(asc_saturation_mode saturation_mode, bool enable_sat)
{
    asc_set_saturation_flag_impl(saturation_mode, enable_sat);
}

__aicore__ inline bool asc_get_saturation_flag(asc_saturation_mode saturation_mode)
{
    return asc_get_saturation_flag_impl(saturation_mode);
}

__aicore__ inline void asc_set_saturation_strategy(asc_override_strategy strategy)
{
    asc_set_saturation_strategy_impl(strategy);
}

__aicore__ inline asc_override_strategy asc_get_saturation_strategy() { return asc_get_saturation_strategy_impl(); }
#endif

ASC_DEPRECATED(9.2.0, "2027/09/07", asc_get_squeeze_status) __aicore__ inline int64_t asc_get_ar_spr()
{
    return asc_get_ar_spr_impl();
}

/*
Please use block_idx for pure Vector, pure Cube, and Mix(1, 1).
For Mix(1, 2), please use block_idx on the Cube core and
block_idx * asc_get_sub_block_num() + asc_get_sub_block_id()
on Vector cores instead.
*/
ASC_DEPRECATED(9.2.0, "2027/09/07", block_idx and asc_get_sub_block_num and asc_get_sub_block_id)
__aicore__ inline int64_t asc_get_block_idx() { return asc_get_block_idx_impl(); }

ASC_DEPRECATED(9.2.0, "2027/09/07", block_num) __aicore__ inline int64_t asc_get_block_num()
{
    return asc_get_block_num_impl();
}

#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
