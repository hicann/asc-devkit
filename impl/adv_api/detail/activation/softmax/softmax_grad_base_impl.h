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
 * \file softmax_grad_base_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/activation/softmax/softmax_grad_base_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmaxgrad.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_GRAD_BASE_IMPL_H__
#endif
#ifndef IMPL_ACTIVATION_SOFTMAX_SOFTMAX_GRAD_BASE_IMPL_H
#define IMPL_ACTIVATION_SOFTMAX_SOFTMAX_GRAD_BASE_IMPL_H

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "regbase/3510/softmax_grad_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 3002
#include "regbase/v300/softmax_grad_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
#include "membase/v220/softmax_grad_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
#include "membase/v200/softmax_grad_impl.h"
#endif
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/activation/softmax/softmax_grad/softmax_grad_check.h"
#include "../../api_check/kernel_check/activation/softmax/softmax_grad_front/softmax_grad_front_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
template <typename T, bool isBasicBlock = false, bool isDataFormatNZ = false>
__aicore__ inline void SoftmaxGradFrontImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo)
{
    CHECK_FUNC_HIGHLEVEL_API(
        SoftmaxGradFront, (T, isBasicBlock, isDataFormatNZ),
        (dstTensor, gradTensor, srcTensor, workLocal, tiling, softmaxShapeInfo));
    ShapeInfo srcShape = srcTensor.GetShapeInfo();
    uint32_t elementNumPerBlk = ONE_BLK_SIZE / sizeof(T);
    LastAxisShapeND srcNDinfo;
    LastAxisShapeND originalSrcShape;
    if (softmaxShapeInfo.srcM == 0 || softmaxShapeInfo.srcK == 0) {
        srcNDinfo = GetLastAxisShapeND(srcShape);
        originalSrcShape = GetLastAxisOriginShapeND(srcShape);
    } else {
        srcNDinfo = {softmaxShapeInfo.srcM, softmaxShapeInfo.srcK};
        originalSrcShape = {softmaxShapeInfo.oriSrcM, softmaxShapeInfo.oriSrcK};
    }
    if constexpr (isDataFormatNZ) {
        if (unlikely(srcNDinfo.k != tiling.srcK || srcNDinfo.m != tiling.srcM)) {
            SoftMaxTiling newTiling = tiling;
            SoftMaxGradTilingFunc(workLocal.GetSize(), srcNDinfo, newTiling, elementNumPerBlk, true, false, true);
            SoftmaxGradFrontNZImpl(dstTensor, gradTensor, srcTensor, workLocal, originalSrcShape, newTiling);
        } else {
            SoftmaxGradFrontNZImpl(dstTensor, gradTensor, srcTensor, workLocal, originalSrcShape, tiling);
        }
    } else {
        if (unlikely(srcNDinfo.k != tiling.srcK || srcNDinfo.m != tiling.srcM)) {
            SoftMaxTiling newTiling = tiling;
            SoftMaxGradTilingFunc(workLocal.GetSize(), srcNDinfo, newTiling, elementNumPerBlk, true, isBasicBlock);
            SoftmaxGradFrontNDImpl<T, isBasicBlock>(
                dstTensor, gradTensor, srcTensor, workLocal, newTiling, originalSrcShape);
        } else {
            SoftmaxGradFrontNDImpl<T, isBasicBlock>(
                dstTensor, gradTensor, srcTensor, workLocal, tiling, originalSrcShape);
        }
    }
}

template <typename T, bool isBasicBlock = false, bool isDataFormatNZ = false>
__aicore__ inline void SoftmaxGradFrontImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor,
    const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo)
{
    LocalTensor<float> workLocal;
    PopStackBuffer<float, TPosition::LCM>(workLocal);
    SoftmaxGradFrontImpl<T, isBasicBlock, isDataFormatNZ>(
        dstTensor, gradTensor, srcTensor, workLocal, tiling, softmaxShapeInfo);
}

template <typename T, bool isBasicBlock = false, bool isDataFormatNZ = false>
__aicore__ inline void SoftmaxGradFrontImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo)
{
    auto workLocal = sharedTmpBuffer.ReinterpretCast<float>();
    SoftmaxGradFrontImpl<T, isBasicBlock, isDataFormatNZ>(
        dstTensor, gradTensor, srcTensor, workLocal, tiling, softmaxShapeInfo);
}

template <typename T, bool isReuseSource = false, bool isDataFormatNZ = false>
__aicore__ inline void SoftmaxGradImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling, bool isFront,
    const SoftMaxShapeInfo& softmaxShapeInfo)
{
    CHECK_FUNC_HIGHLEVEL_API(
        SoftmaxGrad, (T, isReuseSource, isDataFormatNZ),
        (dstTensor, gradTensor, srcTensor, workLocal, tiling, isFront, softmaxShapeInfo));

    ShapeInfo srcShape = srcTensor.GetShapeInfo();
    uint32_t elementNumPerBlk = ONE_BLK_SIZE / sizeof(T);

    LastAxisShapeND srcNDinfo;
    LastAxisShapeND originalSrcShape;
    if (softmaxShapeInfo.srcM == 0 || softmaxShapeInfo.srcK == 0) {
        srcNDinfo = GetLastAxisShapeND(srcShape);
        originalSrcShape = GetLastAxisOriginShapeND(srcShape);
    } else {
        srcNDinfo = {softmaxShapeInfo.srcM, softmaxShapeInfo.srcK};
        originalSrcShape = {softmaxShapeInfo.oriSrcM, softmaxShapeInfo.oriSrcK};
    }
    if constexpr (isDataFormatNZ) {
        if (unlikely(srcNDinfo.k != tiling.srcK || srcNDinfo.m != tiling.srcM)) {
            SoftMaxTiling newTiling = tiling;
            SoftMaxGradTilingFunc(workLocal.GetSize(), srcNDinfo, newTiling, elementNumPerBlk, isFront, false, true);
            SoftmaxGradNZImpl(dstTensor, gradTensor, srcTensor, workLocal, originalSrcShape, newTiling, isFront);
        } else {
            SoftmaxGradNZImpl(dstTensor, gradTensor, srcTensor, workLocal, originalSrcShape, tiling, isFront);
        }
    } else {
        if (unlikely(srcNDinfo.k != tiling.srcK || srcNDinfo.m != tiling.srcM)) {
            SoftMaxTiling newTiling = tiling;
            SoftMaxGradTilingFunc(workLocal.GetSize(), srcNDinfo, newTiling, elementNumPerBlk, isFront, false);
            SoftmaxGradPostProcess<T>(
                dstTensor, gradTensor, srcTensor, workLocal, newTiling, originalSrcShape, isFront);
        } else {
            SoftmaxGradPostProcess<T>(dstTensor, gradTensor, srcTensor, workLocal, tiling, originalSrcShape, isFront);
        }
    }
}

template <typename T, bool isReuseSource = false, bool isDataFormatNZ = false>
__aicore__ inline void SoftmaxGradImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor,
    const SoftMaxTiling& tiling, bool isFront, const SoftMaxShapeInfo& softmaxShapeInfo)
{
    LocalTensor<float> workLocal;
    PopStackBuffer<float, TPosition::LCM>(workLocal);
    SoftmaxGradImpl<T, isReuseSource, isDataFormatNZ>(
        dstTensor, gradTensor, srcTensor, workLocal, tiling, isFront, softmaxShapeInfo);
}
template <typename T, bool isReuseSource = false, bool isDataFormatNZ = false>
__aicore__ inline void SoftmaxGradImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, bool isFront,
    const SoftMaxShapeInfo& softmaxShapeInfo)
{
    auto workLocal = sharedTmpBuffer.ReinterpretCast<float>();
    SoftmaxGradImpl<T, isReuseSource, isDataFormatNZ>(
        dstTensor, gradTensor, srcTensor, workLocal, tiling, isFront, softmaxShapeInfo);
}
} // namespace AscendC
#endif // IMPL_ACTIVATION_SOFTMAX_SOFTMAX_GRAD_BASE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_GRAD_BASE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_GRAD_BASE_IMPL_H__
#endif
