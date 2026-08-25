/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "sys_var.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H
#endif

#ifndef INCLUDE_C_API_UTILS_SYS_VAR_H
#define INCLUDE_C_API_UTILS_SYS_VAR_H

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/instr_impl/npu_arch_2201/sys_var_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/c_api/instr_impl/npu_arch_3510/sys_var_impl.h"
#endif

#include "c_api/defs/defs.h"
#include "c_api/spr/spr.h"

__aicore__ inline int64_t asc_get_smmu_tag_version();
__aicore__ inline int64_t asc_get_phy_stack_base();
__aicore__ inline int64_t asc_get_status();
__aicore__ inline int64_t asc_get_sys_virtual_base();
__aicore__ inline int64_t asc_get_sub_block_num();
__aicore__ inline int64_t asc_get_sub_block_id();
__aicore__ inline int64_t asc_get_system_cycle();
__aicore__ inline int64_t asc_get_ffts_base_addr();
__aicore__ inline int64_t asc_get_core_id();
__aicore__ inline uint64_t asc_get_overflow_status();
__aicore__ inline uint64_t asc_get_phy_buf_addr(uint64_t offset);
__aicore__ inline int64_t asc_get_program_counter();
__aicore__ inline void asc_get_arch_ver(uint32_t& core_version);
__aicore__ inline void asc_set_ffts_base_addr(uint64_t config);
__aicore__ inline constexpr int64_t asc_get_vf_len();

[[deprecated("NOTICE: asc_get_block_num is deprecated. "
             "Please use block_num instead.")]]
__aicore__ inline int64_t asc_get_block_num();

[[deprecated("NOTICE: asc_get_block_idx is deprecated. "
             "Please use block_idx for pure Vector, pure Cube, and Mix(1, 1). "
             "For Mix(1, 2), please use block_idx on the Cube core and "
             "block_idx * asc_get_sub_block_num() + asc_get_sub_block_id() "
             "on Vector cores instead.")]]
__aicore__ inline int64_t asc_get_block_idx();
#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_C_API_H
#endif
