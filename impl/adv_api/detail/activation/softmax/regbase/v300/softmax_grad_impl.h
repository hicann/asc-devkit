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
 * \file softmax_grad_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/activation/softmax/regbase/v300/softmax_grad_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmaxgrad.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_GRAD_IMPL_H__
#endif

#ifndef IMPL_ACTIVATION_SOFTMAX_V300_SOFTMAX_GRAD_IMPL_H
#define IMPL_ACTIVATION_SOFTMAX_V300_SOFTMAX_GRAD_IMPL_H

#include "softmax_common_impl.h"

namespace AscendC {
template <typename T>
__aicore__ inline void SoftmaxGradFrontNZImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape, const SoftMaxTiling& tiling)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "softmaxgradfront format NZ is not supported on current device!"); });
}

template <typename T>
__aicore__ inline void SoftmaxGradNZImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape, const SoftMaxTiling& tiling,
    bool isFront = false)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "softmaxgrad format NZ is not supported on current device!"); });
}

__aicore__ inline void SoftmaxGradFrontGenericNDImpl(
    const LocalTensor<half>& dstTensor, const LocalTensor<half>& gradTensor, const LocalTensor<half>& srcTensor,
    const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling, const uint32_t offset1, const uint32_t offset2,
    const uint32_t splitSize, const uint32_t reduceSize, const ReduceLastND& reduceSumParam)
{
    LocalTensor<float> srcBuffer = workLocal;
    LocalTensor<float> gradBuffer = workLocal[tiling.splitSize];
    LocalTensor<float> dstBuffer = workLocal[tiling.splitSize + tiling.splitSize];
    LocalTensor<float> reduceBuffer = workLocal[tiling.splitSize + tiling.splitSize + tiling.splitSize];
    LocalTensor<float> addBuffer =
        workLocal[tiling.splitSize + tiling.splitSize + tiling.splitSize + tiling.reduceSize];

    Cast(srcBuffer, srcTensor[offset1], RoundMode::CAST_NONE, splitSize);
    Cast(gradBuffer, gradTensor[offset1], RoundMode::CAST_NONE, splitSize);
    Mul(dstBuffer, srcBuffer, gradBuffer, splitSize);
    ReduceSumImpl(addBuffer, dstBuffer, srcBuffer, reduceSumParam);
    Cast(dstTensor[offset2], addBuffer, RoundMode::CAST_ROUND, reduceSize);
}

__aicore__ inline void SoftmaxGradFrontGenericNDImpl(
    const LocalTensor<float>& dstTensor, const LocalTensor<float>& gradTensor, const LocalTensor<float>& srcTensor,
    const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling, const uint32_t offset1, const uint32_t offset2,
    const uint32_t splitSize, const uint32_t reduceSize, const ReduceLastND& reduceSumParam)
{
    LocalTensor<float> srcBuffer = workLocal;
    LocalTensor<float> reduceBuffer = workLocal[tiling.splitSize]; // need splitM*64
    Mul(srcBuffer, srcTensor[offset1], gradTensor[offset1], splitSize);
    ReduceSumImpl(dstTensor[offset2], srcBuffer, reduceBuffer, reduceSumParam);
}

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
__aicore__ inline void SoftmaxGradFrontGenericNDImpl(
    const LocalTensor<float>& dstTensor, const LocalTensor<float>& gradTensor, const LocalTensor<float>& srcTensor,
    const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling, const LastAxisShapeND& originalSrcShape)
{
    uint16_t srcK = tiling.srcK;
    uint16_t reduceK = FLOAT_NUM_PER_BLK;
    uint16_t srcM = tiling.srcM;
    uint16_t originK = (uint16_t)originalSrcShape.k;
    for (uint16_t i = 0; i < (uint16_t)srcM; i++) {
        Mul(workLocal, srcTensor[i * srcK], gradTensor[i * srcK], originK);
        ReduceSum(dstTensor[i * reduceK], workLocal, workLocal, originK);
        Duplicate(dstTensor[i * reduceK], dstTensor[i * reduceK], reduceK);
    }
}

__aicore__ inline void SoftmaxGradFrontGenericNDImpl(
    const LocalTensor<half>& dstTensor, const LocalTensor<half>& gradTensor, const LocalTensor<half>& srcTensor,
    const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling, const LastAxisShapeND& originalSrcShape)
{
    uint16_t srcK = tiling.srcK;
    uint16_t reduceK = HALF_NUM_PER_BLK;
    uint16_t srcM = tiling.srcM;
    uint16_t originK = (uint16_t)originalSrcShape.k;

    LocalTensor<float> srcBuffer = workLocal;
    LocalTensor<float> gradBuffer = workLocal[tiling.splitSize];
    LocalTensor<float> dstBuffer = workLocal[tiling.splitSize + tiling.splitSize];
    LocalTensor<float> reduceBuffer = workLocal[tiling.splitSize + tiling.splitSize + tiling.splitSize];
    auto halfWorkLocal = gradBuffer.ReinterpretCast<half>();

    for (uint16_t i = 0; i < (uint16_t)srcM; i++) {
        Cast(srcBuffer, srcTensor[i * srcK], RoundMode::CAST_NONE, originK);
        Cast(gradBuffer, gradTensor[i * srcK], RoundMode::CAST_NONE, originK);
        Mul(dstBuffer, srcBuffer, gradBuffer, originK);
        ReduceSum(reduceBuffer, dstBuffer, dstBuffer, originK);
        Cast(halfWorkLocal, reduceBuffer, RoundMode::CAST_ROUND, reduceK);
        Duplicate(dstTensor[i * reduceK], halfWorkLocal, reduceK);
    }
}
#endif
template <typename T, bool isBasicBlock = false>
__aicore__ inline void SoftmaxGradFrontNDImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling, const LastAxisShapeND& originalSrcShape)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    SoftmaxGradFrontGenericNDImpl(dstTensor, gradTensor, srcTensor, workLocal, tiling, originalSrcShape);
#else
    ReduceLastND reduceSumParam = {tiling.splitM, originalSrcShape.k, tiling.splitM,
                                   tiling.splitK, tiling.reduceM,     tiling.reduceK};

    uint32_t offset1 = 0;
    uint32_t offset2 = 0;
    uint32_t splitSize = tiling.splitSize;
    uint32_t reduceSize = tiling.reduceSize;
    for (uint32_t i = 0; i <= tiling.rangeM; i++) {
        SoftmaxGradFrontGenericNDImpl(
            dstTensor, gradTensor, srcTensor, workLocal, tiling, offset1, offset2, splitSize, reduceSize,
            reduceSumParam);
        offset1 += tiling.splitSize;
        offset2 += tiling.reduceSize;
        if (i == (tiling.rangeM - 1)) {
            if (tiling.tailM == 0) {
                break;
            }
            offset1 = tiling.rangeM * tiling.splitSize;
            offset2 = tiling.rangeM * tiling.reduceSize;
            splitSize = tiling.tailSplitSize;
            reduceSize = tiling.tailReduceSize;
            reduceSumParam.originalSrcM = tiling.tailM;
            reduceSumParam.srcM = tiling.tailM;
            reduceSumParam.dstM = tiling.tailM;
        }
    }
#endif
}

__aicore__ inline void SoftMaxGradGenericNDImpl(
    const LocalTensor<half>& dst, const LocalTensor<half>& gradTensor, const LocalTensor<half>& src,
    const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling, const uint32_t offset1, const uint32_t offset2,
    const uint32_t splitSize, const uint32_t reduceSize, const ReduceLastND& reduceParam)
{
    LocalTensor<float> srcBuffer = workLocal;
    LocalTensor<float> gradBuffer = workLocal[tiling.splitSize];
    LocalTensor<float> dstBuffer = workLocal[tiling.splitSize + tiling.splitSize];
    LocalTensor<float> addBuffer = workLocal[tiling.splitSize + tiling.splitSize + tiling.splitSize];
    LocalTensor<float> reduceBuffer =
        workLocal[tiling.splitSize + tiling.splitSize + tiling.splitSize + tiling.reduceSize]; // need splitM*64

    Cast(srcBuffer, src[offset1], RoundMode::CAST_NONE, splitSize);
    Cast(gradBuffer, gradTensor[offset1], RoundMode::CAST_NONE, splitSize);
    Mul(dstBuffer, srcBuffer, gradBuffer, splitSize);
    ReduceSumImpl(addBuffer, dstBuffer, reduceBuffer, reduceParam);
    SubNDImpl(dstBuffer, gradBuffer, addBuffer, reduceParam.originalSrcM, tiling.srcK, tiling.reduceK);
    Mul(dstBuffer, dstBuffer, srcBuffer, splitSize);
    Cast(dst[offset1], dstBuffer, RoundMode::CAST_ROUND, splitSize);
}

__aicore__ inline void SoftMaxGradGenericNDImpl(
    const LocalTensor<float>& dst, const LocalTensor<float>& gradTensor, const LocalTensor<float>& src,
    const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling, const uint32_t offset1, const uint32_t offset2,
    const uint32_t splitSize, const uint32_t reduceSize, const ReduceLastND& reduceParam)
{
    LocalTensor<float> splitBuffer = workLocal;
    LocalTensor<float> addBuffer = workLocal[tiling.splitSize];
    LocalTensor<float> reduceBuffer = workLocal[tiling.splitSize + tiling.reduceSize]; // need splitM*64

    Mul(splitBuffer, src[offset1], gradTensor[offset1], splitSize);
    ReduceSumImpl(addBuffer, splitBuffer, reduceBuffer, reduceParam);
    SubNDImpl(splitBuffer, gradTensor[offset1], addBuffer, reduceParam.originalSrcM, tiling.srcK, tiling.reduceK);
    Mul(dst[offset1], src[offset1], splitBuffer, splitSize);
}

template <typename T>
__aicore__ inline void SoftmaxGradPostProcess(
    const LocalTensor<T>& dst, const LocalTensor<T>& gradTensor, const LocalTensor<T>& src,
    const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling, const LastAxisShapeND& originalSrcShape,
    bool isFront = false)
{
    if (isFront) {
        SoftmaxGradFrontNDImpl<T>(dst, gradTensor, src, workLocal, tiling, originalSrcShape);
    } else {
        ReduceLastND reduceParam = {tiling.splitM, originalSrcShape.k, tiling.splitM,
                                    tiling.splitK, tiling.reduceM,     tiling.reduceK};
        uint32_t offset1 = 0;
        uint32_t offset2 = 0;
        uint32_t splitSize = tiling.splitSize;
        uint32_t reduceSize = tiling.reduceSize;
        for (uint32_t i = 0; i <= tiling.rangeM; i++) {
            SoftMaxGradGenericNDImpl(
                dst, gradTensor, src, workLocal, tiling, offset1, offset2, splitSize, reduceSize, reduceParam);
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
    }
}
} // namespace AscendC
#endif // IMPL_ACTIVATION_SOFTMAX_V300_SOFTMAX_GRAD_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_GRAD_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_GRAD_IMPL_H__
#endif
