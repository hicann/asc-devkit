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
 * \file log_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/log/log_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/log.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_LOG_LOG_COMMON_IMPL_H__
#endif
#ifndef IMPL_MATH_LOG_LOG_COMMON_IMPL_H
#define IMPL_MATH_LOG_LOG_COMMON_IMPL_H
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "kernel_tiling/kernel_tiling.h"
#include "../../common/check.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/log/log_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 5162 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "log_3510_impl.h"
#endif

#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || \
     __NPU_ARCH__ == 5162 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)

namespace AscendC {
template <typename T>
__ASC_USE_RESERVED_UBUF__(3510, "Log2 is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Log2Compute(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
{
    // Log2x = Lnx/Ln2
    const T Ln2Reciprocal = 1.4426950408889634; // 1.0/Ln2;
    const UnaryRepeatParams unaryParams;
    Ln<float, false>(dstTensor, srcTensor, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Muls<float, false>(dstTensor, dstTensor, Ln2Reciprocal, MASK_PLACEHOLDER, 1, unaryParams);
}

template <typename T>
__ASC_USE_RESERVED_UBUF__(3510, "Log2 is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Log2Compute(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& tmpTensor)
{
    // Log2x = Lnx/Ln2
    const float Ln2Reciprocal = 1.4426950408889634; // 1.0/Ln2;
    const UnaryRepeatParams unaryParams;

    // src->tmp
    Cast<float, T, false>(
        tmpTensor.ReinterpretCast<float>(), srcTensor, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        {1, 1, DEFAULT_REPEAT_STRIDE, HALF_DEFAULT_REPEAT_STRIDE});
    PipeBarrier<PIPE_V>();

    // tmp->tmp
    Ln<float, false>(
        tmpTensor.ReinterpretCast<float>(), tmpTensor.ReinterpretCast<float>(), MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Muls<float, false>(
        tmpTensor.ReinterpretCast<float>(), tmpTensor.ReinterpretCast<float>(), static_cast<float>(Ln2Reciprocal),
        MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    // tmp->dst
    Cast<T, float, false>(
        dstTensor, tmpTensor.ReinterpretCast<float>(), RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        {1, 1, HALF_DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
    PipeBarrier<PIPE_V>();
}

template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(3510, "Log is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void LogImpl(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, uint32_t calCount)
{
    // Logx = Lnx
    CHECK_FUNC_HIGHLEVEL_API(Log, (T, isReuseSource), (dstTensor, srcTensor, calCount));

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 5162 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    LogImpl((__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), calCount);
#else
    const UnaryRepeatParams unaryParams;
    SetMaskCount();
    SetVectorMask<T, MaskMode::COUNTER>(0, calCount);
    Ln<T, false>(dstTensor, srcTensor, MASK_PLACEHOLDER, 1, unaryParams);
    SetMaskNorm();
    SetVectorMask<half, MaskMode::NORMAL>(FULL_MASK, FULL_MASK);
#endif
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void Log2Impl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    uint32_t calCount)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 5162 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    const float Ln2Reciprocal = 1.4426950408889634; // 1.0/Ln2;
    LogXImpl((__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), calCount, Ln2Reciprocal);
#else
    SetMaskCount();
    if constexpr (sizeof(T) == sizeof(float)) {
        CHECK_FUNC_HIGHLEVEL_API(Log2, (T, isReuseSource), (dstTensor, srcTensor, calCount));
        SetVectorMask<T, MaskMode::COUNTER>(0, calCount);
        Log2Compute(dstTensor, srcTensor);
    } else {
        CHECK_FUNC_HIGHLEVEL_API(Log2, (T, isReuseSource), (dstTensor, srcTensor, sharedTmpBuffer, calCount));
        uint32_t tmpBufferSize = sharedTmpBuffer.GetSize();
        uint32_t splitSize = tmpBufferSize / sizeof(float) / ONE_BLK_SIZE * ONE_BLK_SIZE;
        CheckTmpBufferSize(splitSize, 0, tmpBufferSize);
        uint32_t loopCount = calCount / splitSize;
        uint32_t calcTail = calCount % splitSize;
        SetVectorMask<T, MaskMode::COUNTER>(0, splitSize);
        for (uint32_t i = 0; i < loopCount; ++i) {
            Log2Compute(dstTensor[i * splitSize], srcTensor[i * splitSize], sharedTmpBuffer);
        }
        if (calcTail > 0) {
            SetVectorMask<T, MaskMode::COUNTER>(0, calcTail);
            Log2Compute(dstTensor[loopCount * splitSize], srcTensor[loopCount * splitSize], sharedTmpBuffer);
        }
    }
    SetMaskNorm();
    SetVectorMask<half, MaskMode::NORMAL>(FULL_MASK, FULL_MASK);
#endif
}

template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(3510, "Log10 is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Log10Impl(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, uint32_t calCount)
{
    // Log10x = Lnx/Ln10
    CHECK_FUNC_HIGHLEVEL_API(Log10, (T, isReuseSource), (dstTensor, srcTensor, calCount));

    const T Ln10Reciprocal = 0.43429448190325176; // 1.0/Ln10;
    const UnaryRepeatParams unaryParams;

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 5162 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    LogXImpl((__ubuf__ T*)dstTensor.GetPhyAddr(), (__ubuf__ T*)srcTensor.GetPhyAddr(), calCount, Ln10Reciprocal);
#else
    SetMaskCount();
    SetVectorMask<T, MaskMode::COUNTER>(0, calCount);
    Ln<T, false>(dstTensor, srcTensor, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Muls<T, false>(dstTensor, dstTensor, Ln10Reciprocal, MASK_PLACEHOLDER, 1, unaryParams);
    SetMaskNorm();
    SetVectorMask<half, MaskMode::NORMAL>(FULL_MASK, FULL_MASK);
#endif
}
} // namespace AscendC
#endif
#endif // IMPL_MATH_LOG_LOG_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_LOG_LOG_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_LOG_LOG_COMMON_IMPL_H__
#endif
