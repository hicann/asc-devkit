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
 * \file softmax_flashv2_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/activation/softmax/regbase/v300/softmax_flashv2_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmaxflashv2.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV2_IMPL_H__
#endif

#ifndef IMPL_ACTIVATION_SOFTMAX_V300_SOFTMAX_FLASHV2_IMPL_H
#define IMPL_ACTIVATION_SOFTMAX_V300_SOFTMAX_FLASHV2_IMPL_H

#include "softmax_common_impl.h"

namespace AscendC {
template <typename T1, typename T2, bool isBasicBlock = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftmaxFlashV2Update(
    const LocalTensor<half>& dst, const LocalTensor<float>& expSumTensor, const LocalTensor<float>& maxTensor,
    const LocalTensor<half>& src, const LocalTensor<half>& expMaxTensor, const LocalTensor<float>& inExpSumTensor,
    const LocalTensor<float>& inMaxTensor, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "softmaxflashv2 is not supported on current device!"); });
}
template <typename T1, typename T2, bool isBasicBlock = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftmaxFlashV2NoUpdate(
    const LocalTensor<T1>& dst, const LocalTensor<T1>& expSumTensor, const LocalTensor<T1>& maxTensor,
    const LocalTensor<T1>& src, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    if (originalSrcShape.k % FLOAT_REPEAT_SIZE) {
        SoftMaxGenericNDWithTailImpl<T1, T1, isBasicBlock, true>(
            dst, expSumTensor, maxTensor, src, workLocal, originalSrcShape, tiling);
    } else {
        SoftMaxGenericNDImpl<T1, T1, isBasicBlock, true>(
            dst, expSumTensor, maxTensor, src, workLocal, originalSrcShape, tiling);
    }
#else
    uint32_t offset1 = 0;
    uint32_t offset2 = 0;
    uint32_t splitSize = tiling.splitSize;
    uint32_t reduceSize = tiling.reduceSize;
    ReduceLastND reduceParam = {tiling.splitM, originalSrcShape.k, tiling.splitM,
                                tiling.splitK, tiling.reduceM,     tiling.reduceK};
    for (uint32_t i = 0; i <= tiling.rangeM; i++) {
        SoftMaxGenericNDImpl<true>(
            dst, expSumTensor, maxTensor, src, workLocal, tiling, offset1, offset2, splitSize, reduceSize, reduceParam);
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

__aicore__ inline void SoftmaxFlashV2UpdateImpl(
    const LocalTensor<half>& dst, const LocalTensor<half>& sumTensor, const LocalTensor<half>& maxTensor,
    const LocalTensor<half>& src, const LocalTensor<half>& expMaxTensor, const LocalTensor<half>& inSumTensor,
    const LocalTensor<half>& inMaxTensor, const LocalTensor<float>& workLocal, const ReduceLastND& reduceParam,
    const SoftMaxTiling& tiling, const uint32_t& offset1, const uint32_t& offset2, const uint32_t& splitSize,
    const uint32_t& reduceSize)
{
    const LocalTensor<float>& tmpBuffer0 = workLocal;                   // for src cast
    const LocalTensor<float>& tmpBuffer1 = workLocal[tiling.splitSize]; // need splitM * 64
    const LocalTensor<float>& tmpBuffer2 = workLocal[tiling.splitSize + tiling.splitM * FLOAT_REPEAT_SIZE];
    const LocalTensor<float>& tmpBuffer3 =
        workLocal[tiling.splitSize + tiling.splitM * FLOAT_REPEAT_SIZE + tiling.reduceSize];

    Cast(tmpBuffer0, src[offset1], RoundMode::CAST_NONE, splitSize);
    Cast(tmpBuffer3, inMaxTensor[offset2], RoundMode::CAST_NONE, reduceSize);

    ReduceMaxImpl(tmpBuffer2, tmpBuffer0, tmpBuffer1, reduceParam);
    Max(tmpBuffer2, tmpBuffer2, tmpBuffer3, reduceSize); // new_max
    Cast(maxTensor[offset2], tmpBuffer2, RoundMode::CAST_ROUND, reduceSize);

    Sub(tmpBuffer3, tmpBuffer3, tmpBuffer2, reduceSize);
    Exp(tmpBuffer3, tmpBuffer3, reduceSize); // exp(inmax - new_max)

    Cast(expMaxTensor[offset2], tmpBuffer3, RoundMode::CAST_ROUND, reduceSize);

    SubNDImpl(tmpBuffer0, tmpBuffer0, tmpBuffer2, reduceParam.originalSrcM, tiling.srcK, tiling.reduceK);
    Exp(tmpBuffer0, tmpBuffer0, splitSize); // exp(x - new_max)
    Cast(dst[offset1], tmpBuffer0, RoundMode::CAST_ROUND, splitSize);

    ReduceSumImpl(tmpBuffer2, tmpBuffer0, tmpBuffer1, reduceParam);

    Cast(tmpBuffer1, inSumTensor[offset2], RoundMode::CAST_NONE, reduceSize);
    Mul(tmpBuffer1, tmpBuffer3, tmpBuffer1, reduceSize);
    Add(tmpBuffer1, tmpBuffer1, tmpBuffer2, reduceSize);
    Cast(sumTensor[offset2], tmpBuffer1, RoundMode::CAST_ROUND, reduceSize);
}
__aicore__ inline void SoftmaxFlashV2UpdateImpl(
    const LocalTensor<float>& dst, const LocalTensor<float>& expSumTensor, const LocalTensor<float>& maxTensor,
    const LocalTensor<float>& src, const LocalTensor<float>& expMaxTensor, const LocalTensor<float>& inExpSumTensor,
    const LocalTensor<float>& inMaxTensor, const LocalTensor<float>& workLocal, const ReduceLastND& reduceParam,
    const SoftMaxTiling& tiling, const uint32_t& offset1, const uint32_t& offset2, const uint32_t& splitSize,
    const uint32_t& reduceSize)
{
    const LocalTensor<float>& tmpBuffer0 = workLocal;
    const LocalTensor<float>& tmpBuffer1 = workLocal[tiling.reduceSize]; // tiling.splitM * FLOAT_REPEAT_SIZE

    ReduceMaxImpl(tmpBuffer0, src[offset1], tmpBuffer1, reduceParam);
    Max(tmpBuffer0, inMaxTensor[offset2], tmpBuffer0, reduceSize);

    Sub(tmpBuffer1, inMaxTensor[offset2], tmpBuffer0, reduceSize);
    Exp(expMaxTensor[offset2], tmpBuffer1, reduceSize); // exp(inmax - new_max)
    DataCopy(maxTensor[offset2], tmpBuffer0, reduceSize);

    SubNDImpl(dst[offset1], src[offset1], maxTensor[offset2], reduceParam.originalSrcM, tiling.srcK, tiling.reduceK);
    Exp(dst[offset1], dst[offset1], splitSize); // exp(x - new_max)
    ReduceSumImpl(tmpBuffer0, dst[offset1], tmpBuffer1, reduceParam);

    Mul(expSumTensor[offset2], expMaxTensor[offset2], inExpSumTensor[offset2], reduceSize);
    Add(expSumTensor[offset2], expSumTensor[offset2], tmpBuffer0, reduceSize);
}

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
__aicore__ inline void SoftmaxFlashV2UpdateNDImpl(
    const LocalTensor<half>& dst, const LocalTensor<half>& expSumTensor, const LocalTensor<half>& maxTensor,
    const LocalTensor<half>& src, const LocalTensor<half>& expMaxTensor, const LocalTensor<half>& inExpSumTensor,
    const LocalTensor<half>& inMaxTensor, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
    uint16_t srcK = tiling.srcK;
    uint16_t reduceK = HALF_NUM_PER_BLK;
    uint16_t srcM = tiling.srcM;
    uint16_t reduceSize = tiling.reduceSize;
    uint16_t originK = (uint16_t)originalSrcShape.k;

    const LocalTensor<float>& tmpBuffer0 = workLocal;                   // for src cast
    const LocalTensor<float>& tmpBuffer1 = workLocal[tiling.splitSize]; // need splitM * 64
    const LocalTensor<float>& tmpBuffer2 = workLocal[tiling.splitSize + tiling.splitM * FLOAT_REPEAT_SIZE];
    const LocalTensor<float>& tmpBuffer3 =
        workLocal[tiling.splitSize + tiling.splitM * FLOAT_REPEAT_SIZE + tiling.reduceSize];
    auto halfMaxLocal = tmpBuffer2.ReinterpretCast<half>();

    for (uint16_t k = 0; k < (uint16_t)srcM; k++) {
        ReduceMax(halfMaxLocal[k * reduceK], src[k * srcK], halfMaxLocal, originK);
        Duplicate(halfMaxLocal[k * reduceK], halfMaxLocal[k * reduceK], reduceK);
    }

    Cast(tmpBuffer3, inMaxTensor, RoundMode::CAST_NONE, tiling.reduceSize);
    Max(maxTensor, halfMaxLocal, inMaxTensor, tiling.reduceSize);
    Cast(tmpBuffer2, maxTensor, RoundMode::CAST_NONE, tiling.reduceSize);

    for (uint16_t k = 0; k < (uint16_t)srcM; k++) {
        Cast(tmpBuffer0, src[k * srcK], RoundMode::CAST_NONE, originK);
        Subs(tmpBuffer0, tmpBuffer0, tmpBuffer2[k * reduceK], originK);
        Exp(tmpBuffer0, tmpBuffer0, originK);
        ReduceSum(tmpBuffer1[k * reduceK], tmpBuffer0, tmpBuffer1, originK);
        Duplicate(tmpBuffer1[k * reduceK], tmpBuffer1[k * reduceK], reduceK);
        Cast(dst[k * srcK], tmpBuffer0, RoundMode::CAST_ROUND, originK);
    }

    Cast(tmpBuffer0, inExpSumTensor, RoundMode::CAST_NONE, tiling.reduceSize);
    Sub(tmpBuffer3, tmpBuffer3, tmpBuffer2, tiling.reduceSize);
    Exp(tmpBuffer3, tmpBuffer3, tiling.reduceSize);
    Cast(expMaxTensor, tmpBuffer3, RoundMode::CAST_ROUND, tiling.reduceSize);
    Mul(tmpBuffer0, tmpBuffer3, tmpBuffer0, tiling.reduceSize);
    Add(tmpBuffer0, tmpBuffer1, tmpBuffer0, tiling.reduceSize);
    Cast(expSumTensor, tmpBuffer0, RoundMode::CAST_ROUND, tiling.reduceSize);
}

__aicore__ inline void SoftmaxFlashV2UpdateNDImpl(
    const LocalTensor<float>& dst, const LocalTensor<float>& expSumTensor, const LocalTensor<float>& maxTensor,
    const LocalTensor<float>& src, const LocalTensor<float>& expMaxTensor, const LocalTensor<float>& inExpSumTensor,
    const LocalTensor<float>& inMaxTensor, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
    uint16_t srcK = tiling.srcK;
    uint16_t reduceK = FLOAT_NUM_PER_BLK;
    uint16_t srcM = tiling.srcM;
    uint16_t reduceSize = tiling.reduceSize;
    uint16_t originK = (uint16_t)originalSrcShape.k;

    const LocalTensor<float>& tmpBuffer0 = workLocal;
    const LocalTensor<float>& tmpBuffer1 = workLocal[reduceSize]; // tiling.splitM * FLOAT_REPEAT_SIZE
    Copy(tmpBuffer0, inMaxTensor, reduceSize);
    Copy(tmpBuffer1, inExpSumTensor, reduceSize);
    for (uint16_t k = 0; k < (uint16_t)srcM; k++) {
        ReduceMax(maxTensor[k * reduceK], src[k * srcK], tmpBuffer0, originK);
        Duplicate(maxTensor[k * reduceK], maxTensor[k * reduceK], reduceK);
    }
    Max(maxTensor, maxTensor, tmpBuffer0, reduceSize);
    for (uint16_t k = 0; k < (uint16_t)srcM; k++) {
        Subs(dst[k * srcK], src[k * srcK], maxTensor[k * reduceK], originK);
        Exp(dst[k * srcK], dst[k * srcK], originK);
        ReduceSum(expSumTensor[k * reduceK], dst[k * srcK], tmpBuffer0, originK);
        Duplicate(expSumTensor[k * reduceK], expSumTensor[k * reduceK], reduceK);
    }

    Sub(expMaxTensor, tmpBuffer0, maxTensor, reduceSize);
    Exp(expMaxTensor, expMaxTensor, reduceSize);
    Mul(tmpBuffer1, expMaxTensor, tmpBuffer1, reduceSize);
    Add(expSumTensor, tmpBuffer1, expSumTensor, reduceSize);
}
#endif
template <typename T1, typename T2, bool isBasicBlock = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftmaxFlashV2Update(
    const LocalTensor<T1>& dst, const LocalTensor<T2>& expSumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& src, const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inExpSumTensor,
    const LocalTensor<T2>& inMaxTensor, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    SoftmaxFlashV2UpdateNDImpl(
        dst, expSumTensor, maxTensor, src, expMaxTensor, inExpSumTensor, inMaxTensor, workLocal, originalSrcShape,
        tiling);
#else
    ReduceLastND reduceParam = {tiling.splitM, originalSrcShape.k, tiling.splitM,
                                tiling.splitK, tiling.reduceM,     tiling.reduceK};
    uint32_t offset1 = 0;
    uint32_t offset2 = 0;
    uint32_t splitSize = tiling.splitSize;
    uint32_t reduceSize = tiling.reduceSize;
    for (uint32_t i = 0; i <= tiling.rangeM; i++) {
        SoftmaxFlashV2UpdateImpl(
            dst, expSumTensor, maxTensor, src, expMaxTensor, inExpSumTensor, inMaxTensor, workLocal, reduceParam,
            tiling, offset1, offset2, splitSize, reduceSize);
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

template <
    typename T1, typename T2, bool isUpdate = false, bool isBasicBlock = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftmaxFlashV2PostProcess(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& expSumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inExpSumTensor,
    const LocalTensor<T2>& inMaxTensor, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
    if constexpr (!isUpdate) {
        SoftmaxFlashV2NoUpdate<T1, T2, isBasicBlock>(
            dstTensor, expSumTensor, maxTensor, srcTensor, workLocal, originalSrcShape, tiling);
    } else {
        SoftmaxFlashV2Update<T1, T2, isBasicBlock>(
            dstTensor, expSumTensor, maxTensor, srcTensor, expMaxTensor, inExpSumTensor, inMaxTensor, workLocal,
            originalSrcShape, tiling);
    }
}
template <typename T1, typename T2, bool isUpdate = false, bool isBasicBlock = false>
__aicore__ inline void SoftMaxFlashV2NZImpl(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& sumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inSumTensor,
    const LocalTensor<T2>& inMaxTensor, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "softmaxflashv2 format NZ is not supported on current device!"); });
}
template <typename T1, typename T2, bool isUpdate = false, bool isBasicBlock = false>
__aicore__ inline void SoftMaxFlashV2NZImpl(
    const LocalTensor<half>& dstTensor, const LocalTensor<float>& sumTensor, const LocalTensor<float>& maxTensor,
    const LocalTensor<half>& srcTensor, const LocalTensor<half>& expMaxTensor, const LocalTensor<float>& inSumTensor,
    const LocalTensor<float>& inMaxTensor, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "softmaxflashv2 format NZ is not supported on current device!"); });
}
template <typename T1, typename T2, bool isBasicBlock = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftmaxFlashV2NDImpl(
    const LocalTensor<half>& dst, const LocalTensor<float>& expSumTensor, const LocalTensor<float>& maxTensor,
    const LocalTensor<half>& src, const LocalTensor<half>& expMaxTensor, const LocalTensor<float>& inExpSumTensor,
    const LocalTensor<float>& inMaxTensor, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "softmaxflashv2 current data type is not supported on current device!"); });
}

template <typename T1, typename T2, bool isUpdate = false, bool isBasicBlock = false, bool isOutputReduceMax = false>
__aicore__ inline void SoftmaxFlashV2M1PostProcess(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& outReduceMax, const LocalTensor<T2>& expSumTensor,
    const LocalTensor<T2>& maxTensor, const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor,
    const LocalTensor<T2>& inExpSumTensor, const LocalTensor<T2>& inMaxTensor, const LocalTensor<float>& workLocal,
    const LastAxisShapeND& originalSrcShape, const SoftMaxTiling& tiling)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "softmaxflashv2 current data struct is not supported on current device!"); });
}
} // namespace AscendC
#endif // IMPL_ACTIVATION_SOFTMAX_V300_SOFTMAX_FLASHV2_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV2_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV2_IMPL_H__
#endif
