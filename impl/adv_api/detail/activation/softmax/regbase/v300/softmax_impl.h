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
 * \file softmax_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/activation/softmax/regbase/v300/softmax_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_IMPL_H__
#endif

#ifndef IMPL_ACTIVATION_SOFTMAX_V300_SOFTMAX_IMPL_H
#define IMPL_ACTIVATION_SOFTMAX_V300_SOFTMAX_IMPL_H

#include "softmax_common_impl.h"

namespace AscendC {
template <typename T1, typename T2, bool isBasicBlock = false>
__aicore__ inline void SoftMaxNZImpl(
    const LocalTensor<T1>& dst, const LocalTensor<T2>& sumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& src, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "softmax format NZ is not supported on current device!"); });
}
template <bool isFlashV2 = false>
__aicore__ inline void SoftMaxGenericNDImpl(
    const LocalTensor<half>& dst, const LocalTensor<half>& sumTensor, const LocalTensor<half>& maxTensor,
    const LocalTensor<half>& src, const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling,
    const uint32_t& offset1, const uint32_t& offset2, const uint32_t& splitSize, const uint32_t& reduceSize,
    const ReduceLastND& reduceParam)
{
    const LocalTensor<float>& tmpBuffer0 = workLocal[0];
    const LocalTensor<float>& tmpBuffer2 = workLocal[tiling.splitSize];
    const LocalTensor<float>& tmpBuffer3 = workLocal[tiling.splitSize + tiling.reduceSize]; // need splitM * 64

    auto halfBuffer0 = tmpBuffer0.ReinterpretCast<half>();
    ReduceMaxImpl(maxTensor[offset2], src[offset1], halfBuffer0, reduceParam);
    Cast(tmpBuffer0, src[offset1], RoundMode::CAST_NONE, splitSize);
    Cast(tmpBuffer2, maxTensor[offset2], RoundMode::CAST_NONE, reduceSize);

    SubNDImpl(tmpBuffer0, tmpBuffer0, tmpBuffer2, reduceParam.originalSrcM, tiling.srcK, tiling.reduceK);

    Exp(tmpBuffer0, tmpBuffer0, splitSize);
    ReduceSumImpl(tmpBuffer2, tmpBuffer0, tmpBuffer3, reduceParam);

    Cast(sumTensor[offset2], tmpBuffer2, RoundMode::CAST_ROUND, reduceSize);
    if constexpr (!isFlashV2) {
        DivNDImpl(tmpBuffer0, tmpBuffer0, tmpBuffer2, reduceParam.originalSrcM, tiling.srcK, tiling.reduceK);
    }
    Cast(dst[offset1], tmpBuffer0, RoundMode::CAST_ROUND, splitSize);
}

template <bool isFlashV2 = false>
__aicore__ inline void SoftMaxGenericNDImpl(
    const LocalTensor<float>& dst, const LocalTensor<float>& sumTensor, const LocalTensor<float>& maxTensor,
    const LocalTensor<float>& src, const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling,
    const uint32_t& offset1, const uint32_t& offset2, const uint32_t& splitSize, const uint32_t& reduceSize,
    const ReduceLastND& reduceParam)
{
    const LocalTensor<float>& tmpBuffer1 = workLocal[0]; // need splitM * 64

    ReduceMaxImpl(maxTensor[offset2], src[offset1], tmpBuffer1, reduceParam);
    SubNDImpl(dst[offset1], src[offset1], maxTensor[offset2], reduceParam.originalSrcM, tiling.srcK, tiling.reduceK);

    Exp(dst[offset1], dst[offset1], splitSize);

    ReduceSumImpl(sumTensor[offset2], dst[offset1], tmpBuffer1, reduceParam);
    if constexpr (!isFlashV2) {
        DivNDImpl(
            dst[offset1], dst[offset1], sumTensor[offset2], reduceParam.originalSrcM, tiling.srcK, tiling.reduceK);
    }
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
template <bool isFlashV2 = false>
__aicore__ inline void SoftMaxGenericNDImpl(
    const LocalTensor<half>& dst, const LocalTensor<half>& sumTensor, const LocalTensor<half>& maxTensor,
    const LocalTensor<half>& src, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
    uint16_t srcK = tiling.srcK;
    uint16_t reduceK = HALF_NUM_PER_BLK;
    uint16_t srcM = tiling.srcM;
    uint16_t originK = (uint16_t)originalSrcShape.k;

    const LocalTensor<float>& tmpBuffer0 = workLocal;
    const LocalTensor<float>& tmpBuffer1 = workLocal[tiling.splitSize];
    const LocalTensor<float>& tmpBuffer2 = workLocal[tiling.splitSize + tiling.reduceSize]; // need splitM * 64
    auto halfWorkLocal = workLocal.ReinterpretCast<half>();
    for (uint16_t i = 0; i < (uint16_t)srcM; i++) {
        ReduceMax(maxTensor[i * reduceK], src[i * srcK], halfWorkLocal, originK);
        Duplicate(maxTensor[i * reduceK], maxTensor[i * reduceK], reduceK);
    }

    for (uint16_t i = 0; i < (uint16_t)srcM; i++) {
        Cast(tmpBuffer0, src[i * srcK], RoundMode::CAST_NONE, originK);
        Cast(tmpBuffer2, maxTensor[i * reduceK], RoundMode::CAST_NONE, reduceK);
        Subs(tmpBuffer0, tmpBuffer0, tmpBuffer2[0], originK);
        Exp(tmpBuffer0, tmpBuffer0, originK);
        ReduceSum(tmpBuffer2, tmpBuffer0, workLocal, originK);
        Duplicate(tmpBuffer2, tmpBuffer2, reduceK);
        Cast(sumTensor[i * reduceK], tmpBuffer2, RoundMode::CAST_ROUND, reduceK);
        if constexpr (!isFlashV2) {
            Divs(tmpBuffer0, tmpBuffer0, tmpBuffer2[0], originK);
        }
        Cast(dst[i * srcK], tmpBuffer0, RoundMode::CAST_ROUND, originK);
    }
}

template <bool isFlashV2 = false>
__aicore__ inline void SoftMaxGenericNDImpl(
    const LocalTensor<float>& dst, const LocalTensor<float>& sumTensor, const LocalTensor<float>& maxTensor,
    const LocalTensor<float>& src, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
    uint16_t srcK = tiling.srcK;
    uint16_t reduceK = FLOAT_NUM_PER_BLK;
    uint16_t srcM = tiling.srcM;
    uint16_t originK = (uint16_t)originalSrcShape.k;

    for (uint16_t i = 0; i < (uint16_t)srcM; i++) {
        ReduceMax(maxTensor[i * reduceK], src[i * srcK], workLocal, originK);
        Duplicate(maxTensor[i * reduceK], maxTensor[i * reduceK], reduceK);
    }
    for (uint16_t i = 0; i < (uint16_t)srcM; i++) {
        Subs(dst[i * srcK], src[i * srcK], maxTensor[i * reduceK], originK);
        Exp(dst[i * srcK], dst[i * srcK], originK);
        ReduceSum(sumTensor[i * reduceK], dst[i * srcK], workLocal, originK);
        Duplicate(sumTensor[i * reduceK], sumTensor[i * reduceK], reduceK);
    }
    if constexpr (!isFlashV2) {
        for (uint16_t i = 0; i < (uint16_t)srcM; i++) {
            Divs(dst[i * srcK], dst[i * srcK], sumTensor[i * reduceK], originK);
        }
    }
}
#endif
template <typename T1, typename T2, bool isBasicBlock = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftMaxNDImpl(
    const LocalTensor<T1>& dst, const LocalTensor<T2>& sumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& src, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    SoftMaxGenericNDImpl(dst, sumTensor, maxTensor, src, workLocal, originalSrcShape, tiling);
#else
    ReduceLastND reduceParam = {tiling.splitM, originalSrcShape.k, tiling.splitM,
                                tiling.splitK, tiling.reduceM,     tiling.reduceK};
    uint32_t offset1 = 0;
    uint32_t offset2 = 0;
    uint32_t splitSize = tiling.splitSize;
    uint32_t reduceSize = tiling.reduceSize;

    for (uint32_t i = 0; i <= tiling.rangeM; i++) {
        SoftMaxGenericNDImpl(
            dst, sumTensor, maxTensor, src, workLocal, tiling, offset1, offset2, splitSize, reduceSize, reduceParam);
        offset1 += tiling.splitSize;
        offset2 += tiling.reduceSize;
        if (i == (tiling.rangeM - 1)) {
            if (tiling.tailM == 0) {
                break;
            }
            offset2 = tiling.rangeM * tiling.reduceSize;
            offset1 = tiling.rangeM * tiling.splitSize;
            splitSize = tiling.tailSplitSize;
            reduceSize = tiling.tailReduceSize;
            reduceParam.originalSrcM = tiling.tailM;
            reduceParam.srcM = tiling.tailM;
            reduceParam.dstM = tiling.tailM;
        }
    }
#endif
}
template <typename T1, typename T2, bool isBasicBlock = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftMaxNDImpl(
    const LocalTensor<half>& dst, const LocalTensor<float>& sumTensor, const LocalTensor<float>& maxTensor,
    const LocalTensor<half>& src, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "softmax current data type is not supported on current device!"); });
}
template <typename T1, typename T2, bool isBasicBlock = false>
__aicore__ inline void SoftMaxNZImpl(
    const LocalTensor<half>& dst, const LocalTensor<float>& sumTensor, const LocalTensor<float>& maxTensor,
    const LocalTensor<half>& src, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "softmax current data type is not supported on current device!"); });
}

template <typename T1, typename T2, bool isDataFormatNZ = false, uint8_t stepSizeMode = 0>
__aicore__ inline bool AdjustSoftMaxResBaseImpl(
    const LocalTensor<T1>& softMaxRes, const LocalTensor<T2>& maxTensor, const uint32_t from, const T1 to,
    const SoftMaxShapeInfo& softmaxShapeInfo)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "AdjustSoftMax is not supported on current device!"); });
}
} // namespace AscendC
#endif // IMPL_ACTIVATION_SOFTMAX_V300_SOFTMAX_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_IMPL_H__
#endif
