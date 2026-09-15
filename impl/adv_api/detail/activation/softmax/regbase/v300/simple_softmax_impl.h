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
 * \file simple_softmax_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/activation/softmax/regbase/v300/simple_softmax_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/simplesoftmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIMPLE_SOFTMAX_IMPL_H__
#endif

#ifndef IMPL_ACTIVATION_SOFTMAX_V300_SIMPLE_SOFTMAX_IMPL_H
#define IMPL_ACTIVATION_SOFTMAX_V300_SIMPLE_SOFTMAX_IMPL_H

#include "softmax_common_impl.h"

namespace AscendC {
template <typename T>
__aicore__ inline void SoftMaxLogV2NZImpl(
    const LocalTensor<T>& dst, const LocalTensor<float>& inLogSumExpTensor, const LocalTensor<T>& src,
    const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "softmaxlogv2 format NZ is not supported on current device!"); });
}
template <typename T, bool isBasicBlock = false>
__aicore__ inline void SoftMaxLogSumExpImpl(
    const LocalTensor<T>& dst, const LocalTensor<float>& inLogSumExpTensor, const LocalTensor<T>& src,
    const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "softmaxlogv2 is not supported on current device!"); });
}
template <typename T1, typename T2>
__aicore__ inline void SimpleSoftMaxNZImpl(
    const LocalTensor<T1>& dst, const LocalTensor<T1>& inSumTensor, const LocalTensor<T1>& inMaxTensor,
    const LocalTensor<T1>& src, const LocalTensor<float> workLocal, const SoftMaxTiling& tiling)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "simplesoftmax format NZ is not supported on current device!"); });
}
template <typename T1, typename T2>
__aicore__ inline void SimpleSoftMaxNZImpl(
    const LocalTensor<half>& dst, const LocalTensor<float>& inSumTensor, const LocalTensor<float>& inMaxTensor,
    const LocalTensor<half>& src, const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "simplesoftmax format NZ is not supported on current device!"); });
}

__aicore__ inline void SimpleSoftMaxGenericNDImpl(
    const LocalTensor<half>& dst, const LocalTensor<half>& inSumTensor, const LocalTensor<half>& inMaxTensor,
    const LocalTensor<half>& src, const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling,
    const uint32_t offset1, const uint32_t offset2, const uint32_t curSplitM)
{
    const LocalTensor<float>& tmpBuffer0 = workLocal;
    const LocalTensor<float>& tmpBuffer2 = workLocal[tiling.splitSize];
    const uint32_t splitSize = curSplitM * tiling.splitK;
    const uint32_t reduceSize = curSplitM * tiling.reduceK;

    Cast(tmpBuffer0, src[offset1], RoundMode::CAST_NONE, splitSize);
    Cast(tmpBuffer2, inMaxTensor[offset2], RoundMode::CAST_NONE, reduceSize);
    SubNDImpl(tmpBuffer0, tmpBuffer0, tmpBuffer2, curSplitM, tiling.srcK, tiling.reduceK);
    Exp(tmpBuffer0, tmpBuffer0, splitSize);
    Cast(tmpBuffer2, inSumTensor[offset2], RoundMode::CAST_NONE, reduceSize);
    DivNDImpl(tmpBuffer0, tmpBuffer0, tmpBuffer2, curSplitM, tiling.srcK, tiling.reduceK);
    Cast(dst[offset1], tmpBuffer0, RoundMode::CAST_ROUND, splitSize);
}
__aicore__ inline void SimpleSoftMaxGenericNDImpl(
    const LocalTensor<float>& dst, const LocalTensor<float>& inSumTensor, const LocalTensor<float>& inMaxTensor,
    const LocalTensor<float>& src, const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling,
    const uint32_t offset1, const uint32_t offset2, const uint32_t curSplitM)
{
    const uint32_t splitSize = curSplitM * tiling.splitK;
    SubNDImpl(dst[offset1], src[offset1], inMaxTensor[offset2], curSplitM, tiling.srcK, tiling.reduceK);
    Exp(dst[offset1], dst[offset1], splitSize);
    DivNDImpl(dst[offset1], dst[offset1], inSumTensor[offset2], curSplitM, tiling.srcK, tiling.reduceK);
}
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
__aicore__ inline void SimpleSoftMaxGenericNDImpl(
    const LocalTensor<float>& dst, const LocalTensor<float>& inSumTensor, const LocalTensor<float>& inMaxTensor,
    const LocalTensor<float>& src, const LocalTensor<float> workLocal, const SoftMaxTiling& tiling)
{
    uint16_t srcK = tiling.srcK;
    uint16_t reduceK = FLOAT_NUM_PER_BLK;
    uint16_t srcM = tiling.srcM;

    for (uint16_t i = 0; i < (uint16_t)srcM; i++) {
        Subs(dst[i * srcK], src[i * srcK], inMaxTensor[i * reduceK], srcK);
        Exp(dst[i * srcK], dst[i * srcK], srcK);
    }
    for (uint16_t i = 0; i < (uint16_t)srcM; i++) {
        Divs(dst[i * srcK], dst[i * srcK], inSumTensor[i * reduceK], srcK);
    }
}

__aicore__ inline void SimpleSoftMaxGenericNDImpl(
    const LocalTensor<half>& dst, const LocalTensor<half>& inSumTensor, const LocalTensor<half>& inMaxTensor,
    const LocalTensor<half>& src, const LocalTensor<float> workLocal, const SoftMaxTiling& tiling)
{
    uint16_t srcK = tiling.srcK;
    uint16_t reduceK = HALF_NUM_PER_BLK;
    uint16_t srcM = tiling.srcM;

    const LocalTensor<float>& tmpBuffer0 = workLocal[0];
    const LocalTensor<float>& tmpBuffer2 = workLocal[srcK];
    for (uint16_t i = 0; i < (uint16_t)srcM; i++) {
        Cast(tmpBuffer0, src[i * srcK], RoundMode::CAST_NONE, srcK);
        Cast(tmpBuffer2, inMaxTensor[i * reduceK], RoundMode::CAST_NONE, reduceK);
        Subs(tmpBuffer0, tmpBuffer0, tmpBuffer2[0], srcK);
        Exp(tmpBuffer0, tmpBuffer0, srcK);
        Cast(tmpBuffer2, inSumTensor[i * reduceK], RoundMode::CAST_NONE, reduceK);
        Divs(tmpBuffer0, tmpBuffer0, tmpBuffer2[0], srcK);
        Cast(dst[i * srcK], tmpBuffer0, RoundMode::CAST_ROUND, srcK);
    }
}
#endif
template <typename T, bool isBasicBlock = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SimpleSoftMaxNDImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& inSumTensor, const LocalTensor<T>& inMaxTensor,
    const LocalTensor<T>& src, const LocalTensor<float> workLocal, const SoftMaxTiling& tiling)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    SimpleSoftMaxGenericNDImpl(dst, inSumTensor, inMaxTensor, src, workLocal, tiling);
#else

    if constexpr (sizeof(T) == sizeof(float)) {
        SimpleSoftMaxGenericNDImpl(dst, inSumTensor, inMaxTensor, src, workLocal, tiling, 0, 0, tiling.srcM);
    } else {
        uint32_t offset1 = 0;
        uint32_t offset2 = 0;
        uint32_t curSplitM = tiling.splitM;
        for (uint32_t i = 0; i <= tiling.rangeM; i++) {
            offset1 = i * tiling.splitSize;
            offset2 = i * tiling.reduceSize;
            SimpleSoftMaxGenericNDImpl(
                dst, inSumTensor, inMaxTensor, src, workLocal, tiling, offset1, offset2, curSplitM);
            if (i == (tiling.rangeM - 1)) {
                if (tiling.tailM == 0) {
                    break;
                }
                offset1 = tiling.rangeM * tiling.splitSize;
                offset2 = tiling.rangeM * tiling.reduceSize;
                curSplitM = tiling.tailM;
            }
        }
    }
#endif
}

template <typename T, bool isBasicBlock = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SimpleSoftMaxNDImpl(
    const LocalTensor<half>& dst, const LocalTensor<float>& inSumTensor, const LocalTensor<float>& inMaxTensor,
    const LocalTensor<half>& src, const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "simplesoftmax current data type is not supported on current device!"); });
}
} // namespace AscendC
#endif // IMPL_ACTIVATION_SOFTMAX_V300_SIMPLE_SOFTMAX_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIMPLE_SOFTMAX_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIMPLE_SOFTMAX_IMPL_H__
#endif
