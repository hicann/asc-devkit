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
 * \file silu_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/activation/silu/silu_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/silu.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SILU_COMMON_IMPL_H__
#endif

#ifndef IMPL_ACTIVATION_SILU_SILU_COMMON_IMPL_H
#define IMPL_ACTIVATION_SILU_SILU_COMMON_IMPL_H

#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../api_check/kernel_api_check.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/activation/silu/silu_check.h"
#include "../../../../basic_api/kernel_log.h"
#include <type_traits>
#endif
namespace AscendC {
template <typename T>
__aicore__ inline void SiluCalcSimplified(
    const LocalTensor<T>& dstAddr, const LocalTensor<T>& srcAddr, uint32_t repeatTimes)
{
    const UnaryRepeatParams unaryParams;
    const BinaryRepeatParams binaryParams;
    // silu(x) = x / (1 + e^(-x))
    Muls<T, false>(dstAddr, srcAddr, T(-1), MASK_PLACEHOLDER, repeatTimes, unaryParams);
    PipeBarrier<PIPE_V>();

    Exp<T, false>(dstAddr, dstAddr, MASK_PLACEHOLDER, repeatTimes, unaryParams);
    PipeBarrier<PIPE_V>();

    Adds<T, false>(dstAddr, dstAddr, 1.0, MASK_PLACEHOLDER, repeatTimes, unaryParams);
    PipeBarrier<PIPE_V>();

    // silu(x) = x / x1
    Div<T, false>(dstAddr, srcAddr, dstAddr, MASK_PLACEHOLDER, repeatTimes, binaryParams);
    PipeBarrier<PIPE_V>();
}

template <typename T, bool isReuseSource = false>
__aicore__ inline __inout_pipe__(V) void SiluCompute(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, uint32_t dataSize)
{
    CHECK_FUNC_HIGHLEVEL_API(Silu, (T, isReuseSource), (dstLocal, srcLocal, dataSize));
#if ASCENDC_CPU_DEBUG
    bool ans = (dataSize <= srcLocal.GetSize()) && (dataSize <= dstLocal.GetSize());
    ASCENDC_ASSERT(ans, { KERNEL_LOG(KERNEL_ERROR, "dataSize must be less than or equal to src/dst tensor"); });

    ans = (std::is_same<T, half>::value) || (std::is_same<T, float>::value);
    ASCENDC_ASSERT(ans, { KERNEL_LOG(KERNEL_ERROR, "type must be half or float"); });
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002)
    SetMaskCount();
    SetVectorMask<T>(0, dataSize);
    SiluCalcSimplified<T>(dstLocal, srcLocal, 1);
    SetMaskNorm();
#else
    ResetMask();
    uint32_t maxRepeatSize = MAX_REPEAT_FLOAT_SIZE; // for float
    uint32_t oneRepeatSize = ONE_REPEAT_FLOAT_SIZE; // for float
    if constexpr (sizeof(T) == sizeof(half)) {
        maxRepeatSize = MAX_REPEAT_HALF_SIZE;
        oneRepeatSize = ONE_REPEAT_HALF_SIZE;
    }
    uint32_t tail = dataSize % oneRepeatSize;
    uint32_t repeatCount = dataSize / oneRepeatSize;
    uint32_t repeatRounding = repeatCount / MAX_REPEAT_TIMES;
    uint32_t repeatRemaining = repeatCount % MAX_REPEAT_TIMES;

    uint32_t offsetCount = 0;
    uint32_t repeatTimes = MAX_REPEAT_TIMES;
    for (uint32_t i = 0; i < repeatRounding; i++) {
        SiluCalcSimplified<T>(dstLocal[offsetCount], srcLocal[offsetCount], repeatTimes);
        offsetCount += maxRepeatSize;
    }

    if (repeatRemaining != 0) {
        repeatTimes = repeatRemaining;
        SiluCalcSimplified<T>(dstLocal[offsetCount], srcLocal[offsetCount], repeatTimes);
    }

    if (tail != 0) {
        offsetCount = dataSize - tail;
        repeatTimes = 1;
        SetVectorMask<T>(tail);
        SiluCalcSimplified<T>(dstLocal[offsetCount], srcLocal[offsetCount], repeatTimes);
    }
#endif
    ResetMask();
}
} // namespace AscendC
#endif

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SILU_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SILU_COMMON_IMPL_H__
#endif
