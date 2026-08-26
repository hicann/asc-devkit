/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file kernel_operator_sys_var_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_sys_var_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_sys_var_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SYS_VAR_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_SYS_VAR_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_SYS_VAR_INTERFACE_IMPL_H

#include <cstdint>
#include "kernel_macros.h"

#if __NPU_ARCH__ == 1001
#include "dav_c100/kernel_operator_sys_var_impl.h"
#elif __NPU_ARCH__ == 2002
#include "dav_m200/kernel_operator_sys_var_impl.h"
#elif __NPU_ARCH__ == 2201
#include "dav_c220/kernel_operator_sys_var_impl.h"
#elif __NPU_ARCH__ == 3002
#include "dav_m300/kernel_operator_sys_var_impl.h"
#elif __NPU_ARCH__ == 3102
#include "dav_m310/kernel_operator_sys_var_impl.h"
#elif __NPU_ARCH__ == 3510
#include "dav_3510/kernel_operator_sys_var_impl.h"
#elif (__NPU_ARCH__ == 5102)
#include "dav_m510/kernel_operator_sys_var_impl.h"
#elif __NPU_ARCH__ == 3003
#include "dav_l300/kernel_operator_sys_var_impl.h"
#elif __NPU_ARCH__ == 3113
#include "dav_l311/kernel_operator_sys_var_impl.h"
#elif (__NPU_ARCH__ == 5101 || __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
#include "dav_5161/kernel_operator_sys_var_impl.h"
#endif

#if __NPU_ARCH__ == 2201 || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
#ifdef __SUPER_KERNEL_DYNAMIC_BLOCK_NUM__
__BLOCK_LOCAL__ __inline__ uint32_t g_super_kernel_dynamic_block_num;
#endif
#endif

namespace sk {
// transmit block local variable information in SK "sub-kernel"
struct SkSystemArgs {
    uint16_t skBlockIdx;    // sub-kernel block idx
    uint16_t skNumBlocks;   // sub-kernel block num
    uint16_t skTaskSyncCfg; // sync type for "early-start"
    uint8_t reserve[10];
    inline __aicore__ uint16_t SkGetBlockIdx() { return skBlockIdx; }
    inline __aicore__ uint16_t SkGetNumBlocks() { return skNumBlocks; }
    inline __aicore__ uint16_t SkGetTaskSyncCfg() { return skTaskSyncCfg; }
    inline __aicore__ void SkSetTaskSyncCfg(uint16_t config) { skTaskSyncCfg = config; }
};
} // namespace sk

namespace AscendC {
__aicore__ inline int64_t GetTaskRation() { return GetTaskRatio(); }

#if defined(__NPU_ARCH__)
__aicore__ inline int64_t GetBlockIdx() { return GetBlockIdxImpl(); }

__aicore__ inline int64_t GetBlockNum()
{
#ifdef __SUPER_KERNEL_STATIC_BLOCK_NUM__
    return __SUPER_KERNEL_STATIC_BLOCK_NUM__;
#elif defined(__SUPER_KERNEL_DYNAMIC_BLOCK_NUM__)
    return g_super_kernel_dynamic_block_num;
#else
    return get_block_num();
#endif
}

__aicore__ inline int64_t GetSubBlockIdx() { return GetSubBlockIdxImpl(); }

__aicore__ inline int64_t GetTaskRatio() { return GetTaskRationImpl(); }

__aicore__ inline void GetArchVersion(uint32_t& coreVersion) { GetArchVersionImpl(coreVersion); }

__aicore__ inline int64_t GetSubBlockNum() { return GetSubBlockNumImpl(); }

__aicore__ inline int64_t GetProgramCounter() { return GetProgramCounterImpl(); }

__aicore__ inline void Trap() { TrapImpl(); }

__aicore__ inline int64_t GetSystemCycle() { return GetSystemCycleImpl(); }

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
template <SpecialPurposeReg spr>
__aicore__ inline int64_t GetSpr()
{
    return GetSprImpl<spr>();
}

template <SpecialPurposeReg spr>
__aicore__ inline void ClearSpr()
{
    ClearSprImpl<spr>();
}
#endif
#endif // defined(__NPU_ARCH__)
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_SYS_VAR_INTERFACE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SYS_VAR_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SYS_VAR_INTF_IMPL_H__
#endif
