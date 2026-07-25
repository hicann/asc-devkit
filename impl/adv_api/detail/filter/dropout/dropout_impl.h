/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file dropout_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/filter/dropout/dropout_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/filter/dropout.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DROPOUT_IMPL_H__
#endif

#ifndef IMPL_FILTER_DROPOUT_DROPOUT_IMPL_H
#define IMPL_FILTER_DROPOUT_DROPOUT_IMPL_H

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002)
#include "dropout_m200_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
#include "dropout_c220_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 3002
#include "dropout_m300_impl.h"
#elif defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "dropout_3510_impl.h"
#endif
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/filter/dropout/dropout_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
#pragma begin_pipe(V)
template <typename T, bool isInitBitMode = false, uint32_t dropOutMode = 0>
__aicore__ inline void DropOutOpt(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& maskLocal,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const float keepProb, const DropOutShapeInfo& info)
{
    float divValue = 1.0;
    divValue = divValue / keepProb;

    const uint32_t dataSize = info.firstAxis * info.srcLastAxis;
    T actualVal;
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102)
    if constexpr (IsSameType<T, bfloat16_t>::value) {
        actualVal = ToBfloat16(divValue);
    } else {
        actualVal = static_cast<T>(divValue);
    }
#else
    actualVal = static_cast<T>(divValue);
#endif
    if constexpr (dropOutMode == DROPOUT_MODE_BYTE_MISALIGN) {
        DropOutByteMode(dstLocal, srcLocal, maskLocal, sharedTmpBuffer, actualVal, info);
    } else if constexpr (dropOutMode == DROPOUT_MODE_BYTE_ALIGN) {
        DropOutByteMode(dstLocal, srcLocal, maskLocal, sharedTmpBuffer, actualVal, dataSize);
    } else if constexpr (dropOutMode == DROPOUT_MODE_BIT_ALIGN) {
        DropOutBitMode<T, isInitBitMode>(dstLocal, srcLocal, maskLocal, sharedTmpBuffer, actualVal, dataSize);
    } else if constexpr (dropOutMode == DROPOUT_MODE_BIT_MISALIGN) {
        DropOutBitMode<T, isInitBitMode>(dstLocal, srcLocal, maskLocal, sharedTmpBuffer, actualVal, info);
    }
}

template <typename T, bool isInitBitMode = false, uint32_t dropOutMode = 0>
__aicore__ inline void DropOutImpl(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& maskLocal,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const float keepProb, const DropOutShapeInfo& info)
{
    CHECK_FUNC_HIGHLEVEL_API(
        DropOut, (T, isInitBitMode, dropOutMode), (dstLocal, srcLocal, maskLocal, sharedTmpBuffer, keepProb, info));
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    CheckTensorPos<T>(dstLocal, Hardware::UB, "dstLocal", "VECIN/VECCALC/VECOUT", "DropOut");
    CheckTensorPos<T>(srcLocal, Hardware::UB, "srcLocal", "VECIN/VECCALC/VECOUT", "DropOut");
    CheckTensorPos<uint8_t>(maskLocal, Hardware::UB, "maskLocal", "VECIN/VECCALC/VECOUT", "DropOut");
    CheckTensorPos<uint8_t>(sharedTmpBuffer, Hardware::UB, "sharedTmpBuffer", "VECIN/VECCALC/VECOUT", "DropOut");
#endif
    TRACE_START(TraceId::DropOut);
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    static_assert(
        (dropOutMode == 0 || dropOutMode == 1 || dropOutMode == 2 || dropOutMode == 3 || dropOutMode == 4),
        "dropOutMode should be 0 / 1 / 2 / 3 / 4");
    ASCENDC_ASSERT((info.firstAxis > 0), { KERNEL_LOG(KERNEL_ERROR, "info.firstAxis must > 0!"); });
    constexpr float keepProbMin = 0;
    constexpr float keepProbMax = 1;
    ASCENDC_ASSERT(((keepProb > keepProbMin) && (keepProb < keepProbMax)), {
        KERNEL_LOG(
            KERNEL_ERROR, "keepProb should be larger than 0 and smaller than 1, current keepProb is %f!", keepProb);
    });
    if constexpr (dropOutMode == 1 || dropOutMode == 4) {
        ASCENDC_ASSERT((info.maskLastAxis % 2 == 0), {
            KERNEL_LOG(KERNEL_ERROR, "maskLastAxis should be multiples of 2 when dropOutMode is 1 or 4!");
        });
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    static_assert(SupportType<T, half, float>(), "Dropout Only Supports half, float on current device.");
#else
    static_assert(
        SupportType<T, half, float, bfloat16_t>(), "Dropout Only Supports half, float, bfloat16_t on current device.");
#endif
#endif

    if constexpr (dropOutMode != 0) {
        DropOutOpt<T, isInitBitMode, dropOutMode>(dstLocal, srcLocal, maskLocal, sharedTmpBuffer, keepProb, info);
    } else if (info.srcLastAxis < info.maskLastAxis) {
        DropOutOpt<T, isInitBitMode, DROPOUT_MODE_BYTE_MISALIGN>(
            dstLocal, srcLocal, maskLocal, sharedTmpBuffer, keepProb, info);
    } else if (info.srcLastAxis == info.maskLastAxis) {
        DropOutOpt<T, isInitBitMode, DROPOUT_MODE_BYTE_ALIGN>(
            dstLocal, srcLocal, maskLocal, sharedTmpBuffer, keepProb, info);
    } else if (info.srcLastAxis == (info.maskLastAxis * ONE_BYTE_BIT_SIZE)) {
        DropOutOpt<T, isInitBitMode, DROPOUT_MODE_BIT_ALIGN>(
            dstLocal, srcLocal, maskLocal, sharedTmpBuffer, keepProb, info);
    } else {
        DropOutOpt<T, isInitBitMode, DROPOUT_MODE_BIT_MISALIGN>(
            dstLocal, srcLocal, maskLocal, sharedTmpBuffer, keepProb, info);
    }
    TRACE_STOP(TraceId::DropOut);
}

template <typename T, bool isInitBitMode = false, uint32_t dropOutMode = 0>
__aicore__ inline void DropOutImpl(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& maskLocal,
    const float keepProb, const DropOutShapeInfo& info)
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });

    DropOutImpl<T, isInitBitMode, dropOutMode>(dstLocal, srcLocal, maskLocal, sharedTmpBuffer, keepProb, info);
}
#pragma end_pipe
} // namespace AscendC
#endif // IMPL_FILTER_DROPOUT_DROPOUT_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DROPOUT_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DROPOUT_IMPL_H__
#endif
