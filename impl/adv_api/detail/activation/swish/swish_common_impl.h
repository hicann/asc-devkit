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
 * \file swish_common_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/activation/swish/swish_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/swish.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SWISH_COMMON_IMPL_H__
#endif
#ifndef IMPL_ACTIVATION_SWISH_SWISH_COMMON_IMPL_H
#define IMPL_ACTIVATION_SWISH_SWISH_COMMON_IMPL_H

#if ASCENDC_CPU_DEBUG
#include <type_traits>
#include "../../../../basic_api/kernel_log.h"
#endif
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/activation/swish/swish_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
template <typename T>
__aicore__ inline void SwishCalcSimplified(
    const LocalTensor<T>& dstAddr, const LocalTensor<T>& srcAddr, T& scalarValue, uint32_t repeatTimes)
{
    // swish(x) = x / (1 + e^(-βx))
    // x1 = 1 + e^(-βx)
    const UnaryRepeatParams unaryParams;
    Muls<T, false>(dstAddr, srcAddr, scalarValue, MASK_PLACEHOLDER, repeatTimes, unaryParams);
    PipeBarrier<PIPE_V>();

    Exp<T, false>(dstAddr, dstAddr, MASK_PLACEHOLDER, repeatTimes, unaryParams);
    PipeBarrier<PIPE_V>();

    Adds<T, false>(dstAddr, dstAddr, static_cast<T>(1), MASK_PLACEHOLDER, repeatTimes, unaryParams);
    PipeBarrier<PIPE_V>();

    const BinaryRepeatParams binaryParams;
    Div<T, false>(dstAddr, srcAddr, dstAddr, MASK_PLACEHOLDER, repeatTimes, binaryParams);
    PipeBarrier<PIPE_V>();
}

template <typename T, bool isReuseSource = false>
__aicore__ inline __inout_pipe__(V) void SwishCompute(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, uint32_t dataSize, const T scalarValue)
{
    // Only for AI Vector Core
    if ASCEND_IS_AIC {
        return;
    }
    CHECK_FUNC_HIGHLEVEL_API(Swish, (T, isReuseSource), (dstLocal, srcLocal, dataSize, scalarValue));
    T scalar = static_cast<T>(static_cast<float>(-1) * static_cast<float>(scalarValue));
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002)
    SetMaskCount();
    SetVectorMask<T, MaskMode::COUNTER>(0, dataSize);
    SwishCalcSimplified(dstLocal, srcLocal, scalar, 1);
    SetMaskNorm();
#else
    ResetMask();
    uint32_t maxRepeatSize = MAX_REPEAT_FLOAT_SIZE; // for float
    uint32_t oneRepeatSize = ONE_REPEAT_FLOAT_SIZE; // for float
    if constexpr (sizeof(T) == sizeof(half)) {
        maxRepeatSize = MAX_REPEAT_HALF_SIZE;
        oneRepeatSize = ONE_REPEAT_HALF_SIZE;
    }
    SetVectorMask<T, MaskMode::NORMAL>(oneRepeatSize);
    uint32_t tail = dataSize % oneRepeatSize;
    uint32_t repeatCount = dataSize / oneRepeatSize;
    uint32_t repeatRounding = repeatCount / MAX_REPEAT_TIMES;
    uint32_t repeatRemaining = repeatCount % MAX_REPEAT_TIMES;

    uint32_t OffsetCount = 0;
    uint32_t repeatTimes = MAX_REPEAT_TIMES;
    for (uint32_t i = 0; i < repeatRounding; i++) {
        SwishCalcSimplified<T>(dstLocal[OffsetCount], srcLocal[OffsetCount], scalar, repeatTimes);
        OffsetCount += maxRepeatSize;
    }

    if (repeatRemaining != 0) {
        repeatTimes = repeatRemaining;
        SwishCalcSimplified<T>(dstLocal[OffsetCount], srcLocal[OffsetCount], scalar, repeatTimes);
    }

    if (tail != 0) {
        OffsetCount = dataSize - tail;
        repeatTimes = 1;
        SetVectorMask<T>(tail);
        SwishCalcSimplified<T>(dstLocal[OffsetCount], srcLocal[OffsetCount], scalar, repeatTimes);
    }
#endif
    ResetMask();
}
} // namespace AscendC
#endif // IMPL_ACTIVATION_SWISH_SWISH_COMMON_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SWISH_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SWISH_COMMON_IMPL_H__
#endif
