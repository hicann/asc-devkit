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
 * \file kernel_operator_utils_intf.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_operator_utils_intf.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_UTILS_INTF_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_UTILS_INTF_H
#define ASCENDC_MODULE_OPERATOR_UTILS_INTF_H

#include "../../impl/basic_api/kernel_macros.h"
#include "kernel_operator_block_sync_intf.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#endif
namespace AscendC {

template <int count = 1>
__aicore__ inline void Nop()
{
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
    if (count <= 0) {
        return;
    }
    PipeBarrier<PIPE_ALL>();
#pragma unroll
    for (int i = 0; i < count; i++) {
        asm volatile("nop");
    }
    PipeBarrier<PIPE_ALL>();
#endif
}

enum class EngineType : int32_t { AIC = 1, AIV = 2 };

template <EngineType engine, auto funPtr, class... Args>
__aicore__ void Async(Args... args)
{
    if constexpr (engine == EngineType::AIV) {
        if ASCEND_IS_AIV {
            funPtr(args...);
        }
    } else if constexpr (engine == EngineType::AIC) {
        if ASCEND_IS_AIC {
            funPtr(args...);
        }
    }
}
} // namespace AscendC

#endif // KERNEL_UTILS_INTF_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_UTILS_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_UTILS_INTF_H__
#endif
