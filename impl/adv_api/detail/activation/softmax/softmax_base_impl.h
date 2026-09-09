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
 * \file softmax_base_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/activation/softmax/softmax_base_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_BASE_IMPL_H__
#endif
#ifndef IMPL_ACTIVATION_SOFTMAX_SOFTMAX_BASE_IMPL_H
#define IMPL_ACTIVATION_SOFTMAX_SOFTMAX_BASE_IMPL_H

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 3003 || \
                              __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5102)
#include "regbase/3510/softmax_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 3002
#include "regbase/v300/softmax_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
#include "membase/v220/softmax_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
#include "membase/v200/softmax_impl.h"
#endif
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/activation/softmax/adjust_softmax_res/adjust_softmax_res_check.h"
#include "../../api_check/kernel_check/activation/softmax/softmax/softmax_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
template <
    typename T, bool isReuseSource = false, bool isBasicBlock = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftMaxImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<float>& workLocal,
    const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo)
{
    CHECK_FUNC_HIGHLEVEL_API(
        SoftMax, (T, isReuseSource, isBasicBlock, config), (dst, src, workLocal, tiling, softmaxShapeInfo));

    ShapeInfo srcShape = src.GetShapeInfo();
    LastAxisShapeND srcNDinfo;
    LastAxisShapeND originalSrcShape;
    if (softmaxShapeInfo.srcM == 0 || softmaxShapeInfo.srcK == 0) {
        srcNDinfo = GetLastAxisShapeND(srcShape);
        originalSrcShape = GetLastAxisOriginShapeND(srcShape);
    } else {
        srcNDinfo = {softmaxShapeInfo.srcM, softmaxShapeInfo.srcK};
        originalSrcShape = {softmaxShapeInfo.oriSrcM, softmaxShapeInfo.oriSrcK};
    }
    // when the shape is changed, need recalculate the softmax's tiling
    if (unlikely(srcNDinfo.k != tiling.srcK || srcNDinfo.m != tiling.srcM)) {
        SoftMaxTiling newTiling = tiling;
        SoftMaxTilingFunc(
            workLocal.GetSize(), {srcNDinfo.m, srcNDinfo.k, originalSrcShape.m, srcNDinfo.k}, newTiling, sizeof(T),
            sizeof(float), isBasicBlock);
        SoftMaxNDImpl<T, isReuseSource, isBasicBlock, config>(dst, src, workLocal, originalSrcShape, newTiling);
    } else {
        SoftMaxNDImpl<T, isReuseSource, isBasicBlock, config>(dst, src, workLocal, originalSrcShape, tiling);
    }
}
template <
    typename T, bool isReuseSource = false, bool isBasicBlock = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftMaxImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const SoftMaxTiling& tiling,
    const SoftMaxShapeInfo& softmaxShapeInfo)
{
    LocalTensor<float> workLocal;
    PopStackBuffer<float, TPosition::LCM>(workLocal);
    SoftMaxImpl<T, isReuseSource, isBasicBlock, config>(dst, src, workLocal, tiling, softmaxShapeInfo);
}
template <
    typename T, bool isReuseSource = false, bool isBasicBlock = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftMaxImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo)
{
    auto workLocal = sharedTmpBuffer.ReinterpretCast<float>();
    SoftMaxImpl<T, isReuseSource, isBasicBlock, config>(dst, src, workLocal, tiling, softmaxShapeInfo);
}

template <
    typename T1, typename T2, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftMaxImpl(
    const LocalTensor<T1>& dst, const LocalTensor<T2>& sumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& src, const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling,
    const SoftMaxShapeInfo& softmaxShapeInfo)
{
    CHECK_FUNC_HIGHLEVEL_API(
        SoftMax, (T1, T2, isReuseSource, isBasicBlock, isDataFormatNZ, config),
        (dst, sumTensor, maxTensor, src, workLocal, tiling, softmaxShapeInfo));

    ShapeInfo srcShape = src.GetShapeInfo();
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
        // when the shape is changed, need recalculate the softmax's tiling
        if (unlikely(srcNDinfo.k != tiling.srcK || originalSrcShape.m != tiling.srcM)) {
            SoftMaxTiling newTiling = tiling;
            SoftMaxTilingFunc(
                workLocal.GetSize(), {srcNDinfo.m, srcNDinfo.k, originalSrcShape.m, srcNDinfo.k}, newTiling, sizeof(T1),
                sizeof(T2), false, isDataFormatNZ);
            SoftMaxNZImpl<T1, T2, isBasicBlock>(dst, sumTensor, maxTensor, src, workLocal, originalSrcShape, newTiling);
        } else {
            SoftMaxNZImpl<T1, T2, isBasicBlock>(dst, sumTensor, maxTensor, src, workLocal, originalSrcShape, tiling);
        }
    } else {
        // when the shape is changed, need recalculate the softmax's tiling
        if (unlikely(srcNDinfo.k != tiling.srcK || srcNDinfo.m != tiling.srcM)) {
            SoftMaxTiling newTiling = tiling;
            SoftMaxTilingFunc(
                workLocal.GetSize(), {srcNDinfo.m, srcNDinfo.k, originalSrcShape.m, srcNDinfo.k}, newTiling, sizeof(T1),
                sizeof(T2), isBasicBlock);
            SoftMaxNDImpl<T1, T2, isBasicBlock, config>(
                dst, sumTensor, maxTensor, src, workLocal, originalSrcShape, newTiling);
        } else {
            SoftMaxNDImpl<T1, T2, isBasicBlock, config>(
                dst, sumTensor, maxTensor, src, workLocal, originalSrcShape, tiling);
        }
    }
}

template <
    typename T1, typename T2, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftMaxImpl(
    const LocalTensor<T1>& dst, const LocalTensor<T2>& sumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& src, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo)
{
    LocalTensor<float> workLocal;
    PopStackBuffer<float, TPosition::LCM>(workLocal);
    SoftMaxImpl<T1, T2, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
        dst, sumTensor, maxTensor, src, workLocal, tiling, softmaxShapeInfo);
}

template <
    typename T1, typename T2, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftMaxImpl(
    const LocalTensor<T1>& dst, const LocalTensor<T2>& sumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& src, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling,
    const SoftMaxShapeInfo& softmaxShapeInfo)
{
    auto workLocal = sharedTmpBuffer.ReinterpretCast<float>();
    SoftMaxImpl<T1, T2, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
        dst, sumTensor, maxTensor, src, workLocal, tiling, softmaxShapeInfo);
}

template <typename T1, typename T2, bool isDataFormatNZ = false, uint8_t stepSizeMode = 0>
__aicore__ inline bool AdjustSoftMaxResImpl(
    const LocalTensor<T1>& softMaxRes, const LocalTensor<T2>& maxTensor, const uint32_t from, const T1 to,
    const SoftMaxShapeInfo& softmaxShapeInfo)
{
    CHECK_FUNC_HIGHLEVEL_API(
        AdjustSoftMaxRes, (T1, T2, isDataFormatNZ, stepSizeMode), (softMaxRes, maxTensor, from, to, softmaxShapeInfo));
    return AdjustSoftMaxResBaseImpl<T1, T2, isDataFormatNZ, stepSizeMode>(
        softMaxRes, maxTensor, from, to, softmaxShapeInfo);
}
} // namespace AscendC

#endif // IMPL_ACTIVATION_SOFTMAX_SOFTMAX_BASE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_BASE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_BASE_IMPL_H__
#endif
