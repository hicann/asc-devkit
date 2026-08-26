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
 * \file kernel_operator_sys_var_intf.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_operator_sys_var_intf.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SYS_VAR_INTF_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_SYS_VAR_INTERFACE_H
#define ASCENDC_MODULE_OPERATOR_SYS_VAR_INTERFACE_H

#include "../../impl/basic_api/kernel_macros.h"
#include "../../impl/basic_api/utils/kernel_utils_mode.h"
#include "../../impl/basic_api/utils/kernel_utils_constants.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#endif

namespace AscendC {

__aicore__ inline int64_t GetBlockNum();

__aicore__ inline int64_t GetBlockIdx();

__aicore__ inline int64_t GetSubBlockIdx();

__aicore__ inline int64_t GetTaskRatio();

__aicore__ inline constexpr int16_t GetDataBlockSizeInBytes() { return ONE_BLK_SIZE; }

__aicore__ inline void GetArchVersion(uint32_t& coreVersion);

__aicore__ inline int64_t GetSubBlockNum();

__aicore__ inline int64_t GetProgramCounter();

__aicore__ inline void Trap();

__aicore__ inline int64_t GetSystemCycle();

template <SpecialPurposeReg spr>
__aicore__ inline int64_t GetSpr();

template <SpecialPurposeReg spr>
__aicore__ inline void ClearSpr();

__aicore__ inline constexpr uint32_t GetUBSizeInBytes()
{
#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 3510) || \
     (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
    return TOTAL_UB_SIZE;
#else
    return 0;
#endif
}

__aicore__ inline constexpr uint32_t GetVecLen()
{
#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 3510) || \
     (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
    return VECTOR_REG_WIDTH;
#else
    return 0;
#endif
}

__aicore__ inline uint32_t GetRuntimeUBSize()
{
#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 3510) || \
     (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    return TOTAL_UB_SIZE;
#else
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#if defined(SPLIT_CORE_VEC)
#if defined(__ASC_DISABLE_VF_STACK_RESERVED__)
    constexpr uint32_t RESERVED_UB_SIZE = 2 * 1024;
#else
    constexpr uint32_t RESERVED_UB_SIZE = 8 * 1024;
#endif
    return get_shmem_sz() - RESERVED_UB_SIZE;
#else
    return TOTAL_UB_SIZE; // cube core not support get_shmem_sz
#endif
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5102)
    constexpr uint32_t RESERVED_UB_SIZE = 8 * 1024;
    return get_shmem_sz() - RESERVED_UB_SIZE; // m510 aicore reserve 8KB
#else
    return TOTAL_UB_SIZE;
#endif
#endif
#else
    return 0;
#endif
}

__aicore__ inline __ssbuf__ void* GetSsbufBaseAddr()
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#if ASCENDC_CPU_DEBUG
    return reinterpret_cast<__ssbuf__ void*>(ConstDefiner::Instance().cpuSSbuf);
#else
    return (__ssbuf__ void*)0;
#endif
#else
    return (__ssbuf__ void*)0;
#endif
}
} // namespace AscendC

#include "../../impl/basic_api/kernel_operator_sys_var_intf_impl.h"
#endif // ASCENDC_MODULE_OPERATOR_SYS_VAR_INTERFACE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SYS_VAR_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SYS_VAR_INTF_H__
#endif
