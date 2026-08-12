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
 * \file kernel_operator_sys_var_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_m200/kernel_operator_sys_var_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SYS_VAR_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_SYS_VAR_IMPL_H
#define ASCENDC_MODULE_OPERATOR_SYS_VAR_IMPL_H
#include "../kernel_utils.h"

namespace AscendC {
__aicore__ inline int64_t GetSubBlockIdxImpl() { return 0; }

__aicore__ inline int64_t GetTaskRationImpl() { return 1; }

__aicore__ inline int64_t GetBlockIdxImpl()
{
#ifdef __ENABLE_VECTOR_CORE__
    return block_idx + get_data_main_base();
#else
    return block_idx;
#endif
}

__aicore__ inline int64_t GetSubBlockNumImpl() { return 1; }

__aicore__ inline void GetArchVersionImpl(uint32_t& coreVersion) { coreVersion = 2002U; }

__aicore__ inline int64_t GetProgramCounterImpl()
{
    int64_t pc = static_cast<int64_t>(static_cast<uint64_t>(get_pc()) & 0xFFFFFFFFFFFF);
    return pc;
}

__aicore__ inline int64_t GetSystemCycleImpl()
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "GetSystemCycle");
    return 0;
}

__aicore__ inline void TrapImpl() { trap(); }
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_SYS_VAR_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SYS_VAR_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SYS_VAR_IMPL_H__
#endif
