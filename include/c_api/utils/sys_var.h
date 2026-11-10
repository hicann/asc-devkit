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
#define UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_UTILS_SYS_VAR_H
#endif

#ifndef INCLUDE_C_API_UTILS_SYS_VAR_H
#define INCLUDE_C_API_UTILS_SYS_VAR_H

#include "c_api/defs/defs.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "impl/c_api/memory_base_impl/sys_var_intf_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201)
#include "impl/c_api/reg_base_impl/sys_var_intf_impl.h"
#endif

#include "c_api/spr/spr.h"

/**
 * @ingroup simd_c_api.utils.sys_var
 * @brief Returns the SMMU tag version, encoding the release date of the system memory management unit.
 *
 * @override_id asc_get_smmu_tag_version__z
 * @archs 2201 3510
 */
__aicore__ inline int64_t asc_get_smmu_tag_version();
/**
 * @ingroup simd_c_api.utils.sys_var
 * @brief Returns the physical base address of the current core's stack.
 *
 * @override_id asc_get_phy_stack_base__z
 * @archs 2201 3510
 */
__aicore__ inline int64_t asc_get_phy_stack_base();
/**
 * @ingroup simd_c_api.utils.sys_var
 * @brief Reads the 64-bit STATUS register value that records warning events during kernel execution.
 *
 * @override_id asc_get_status__z
 * @archs 2201 3510
 */
__aicore__ inline int64_t asc_get_status();
/**
 * @ingroup simd_c_api.utils.sys_var
 * @brief Returns the system virtual base address of the current core context.
 *
 * @override_id asc_get_sys_virtual_base__z
 * @archs 2201 3510
 */
__aicore__ inline int64_t asc_get_sys_virtual_base();
/**
 * @ingroup simd_c_api.utils.sys_var
 * @brief Reads the number of Cube or Vector cores configured on the current AI Core.
 *
 * @override_id asc_get_sub_block_num__z
 * @archs 2201 3510
 */
__aicore__ inline int64_t asc_get_sub_block_num();
/**
 * @ingroup simd_c_api.utils.sys_var
 * @brief Returns the logical index of the AIC or AIV within the current logical AI Core in separation mode.
 *
 * @override_id asc_get_sub_block_id__z
 * @archs 2201 3510
 */
__aicore__ inline int64_t asc_get_sub_block_id();
/**
 * @ingroup simd_c_api.utils.sys_var
 * @brief Returns the raw 64-bit system cycle counter value.
 *
 * @override_id asc_get_system_cycle__z
 * @archs 2201 3510
 */
__aicore__ inline int64_t asc_get_system_cycle();
/**
 * @ingroup simd_c_api.utils.sys_var
 * @brief Returns the current system time in microseconds, converted internally from the cycle counter.
 *
 * @override_id asc_get_system_clock__z
 * @archs 2201 3510
 */
__aicore__ inline int64_t asc_get_system_clock();
/**
 * @ingroup simd_c_api.utils.sys_var
 * @brief Reads the base address of the inter-core synchronization register.
 *
 * @override_id asc_get_ffts_base_addr__z
 * @archs 2201
 */
__aicore__ inline int64_t asc_get_ffts_base_addr();
/**
 * @ingroup simd_c_api.utils.sys_var
 * @brief Returns the index of the current AI Core among the cores of the task.
 *
 * @override_id asc_get_core_id__z
 * @archs 2201 3510
 */
__aicore__ inline int64_t asc_get_core_id();
__aicore__ inline uint64_t asc_get_overflow_status();
/**
 * @ingroup simd_c_api.utils.sys_var
 * @brief Converts an on-chip memory offset into the physical address used directly by hardware.
 *
 * @override_id asc_get_phy_buf_addr__u64
 * @archs 2201 3510
 * @param [in] offset Address offset within on-chip memory, ranging from 0 to the physical memory size.
 */
__aicore__ inline uint64_t asc_get_phy_buf_addr(uint64_t offset);
/**
 * @ingroup simd_c_api.utils.sys_var
 * @brief Returns the program counter value pointing at the current instruction.
 *
 * @override_id asc_get_program_counter__z
 * @archs 2201 3510
 */
__aicore__ inline int64_t asc_get_program_counter();
/**
 * @ingroup simd_c_api.utils.sys_var
 * @brief Reads the AI processor architecture version into an output parameter.
 *
 * @override_id asc_get_arch_ver__Ru32
 * @archs 2201 3510
 * @param [out] core_version Receives the core version field of the ARCH_VER register as uint32_t; the parameter is
 * overwritten by the call, needs no initialization beforehand, and the documented values are 3510 for Ascend
 * 950PR&950DT and 2201 for Atlas A3 and Atlas A2 products.
 */
__aicore__ inline void asc_get_arch_ver(uint32_t& core_version);
/**
 * @ingroup simd_c_api.utils.sys_var
 * @brief Sets the base address of the inter-core synchronization register before paired block arrive/wait calls.
 *
 * @override_id asc_set_ffts_base_addr__u64
 * @archs 2201
 * @param [in] config Base address for inter-core synchronization; range [0, 2^48 - 1], to be obtained on the host side
 * via aclrtGetHardwareSyncAddr and passed in.
 */
__aicore__ inline void asc_set_ffts_base_addr(uint64_t config);
/**
 * @ingroup simd_c_api.utils.sys_var
 * @brief Returns the chip's vector register length VL in bytes as a compile-time constant.
 *
 * @override_id asc_get_vf_len__z
 * @archs 3510
 */
__aicore__ inline constexpr int64_t asc_get_vf_len();

ASC_DEPRECATED(9.2.0, "2027/09/07", block_num)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated getter returning the number of cores configured for the current task.
 *
 * @override_id asc_get_block_num__z
 * @archs 2201 3510
 */
__aicore__ inline int64_t asc_get_block_num();

/*
Please use block_idx for pure Vector, pure Cube, and Mix(1, 1).
For Mix(1, 2), please use block_idx on the Cube core and
block_idx * asc_get_sub_block_num() + asc_get_sub_block_id()
on Vector cores instead.
*/
ASC_DEPRECATED(9.2.0, "2027/09/07", block_idx and asc_get_sub_block_num and asc_get_sub_block_id)
/**
 * @ingroup simd_c_api.deprecated_interface
 * @brief Deprecated getter returning the index of the current logical core.
 *
 * @override_id asc_get_block_idx__z
 * @archs 2201 3510
 */
__aicore__ inline int64_t asc_get_block_idx();
#endif

#if defined(UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_UTILS_SYS_VAR_H)
#undef ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_C_API_INCLUDE_COMPILER_INTERNAL_HEADERS_UTILS_SYS_VAR_H
#endif
