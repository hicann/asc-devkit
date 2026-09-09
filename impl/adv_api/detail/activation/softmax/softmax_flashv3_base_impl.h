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
 * \file softmax_flashv3_base_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/activation/softmax/softmax_flashv3_base_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmaxflashv3.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV3_BASE_IMPL_H__
#endif

#ifndef IMPL_ACTIVATION_SOFTMAX_SOFTMAX_FLASHV3_BASE_IMPL_H
#define IMPL_ACTIVATION_SOFTMAX_SOFTMAX_FLASHV3_BASE_IMPL_H

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "regbase/3510/softmax_flashv3_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 3002
#include "regbase/v300/softmax_flashv3_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
#include "membase/v220/softmax_flashv3_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
#include "membase/v200/softmax_flashv3_impl.h"
#endif
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/activation/softmax/softmax_flashv3/softmax_flashv3_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
template <
    typename T, typename U, bool isUpdate, bool isReuseSource, bool isBasicBlock, bool isDataFormatNZ,
    const SoftmaxConfig& config>
__aicore__ inline void SoftmaxFlashV3Impl(
    const LocalTensor<T>& dstTensor, const LocalTensor<U>& meanTensor, const LocalTensor<U>& expSumTensor,
    const LocalTensor<U>& maxTensor, const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor,
    const LocalTensor<U>& inMeanTensor, const LocalTensor<U>& inexpSumTensor, const LocalTensor<U>& inMaxTensor,
    const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling, const SoftMaxParams& params)
{
    CHECK_FUNC_HIGHLEVEL_API(
        SoftmaxFlashV3, (T, U, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config),
        (dstTensor, meanTensor, expSumTensor, maxTensor, srcTensor, expMaxTensor, inMeanTensor, inexpSumTensor,
         inMaxTensor, workLocal, tiling, params));
    static_assert(
        (SupportType<Tuple<T, U>, Tuple<half, float>>()),
        "Failed to check dtype in SoftmaxFlashV3, "
        "Current api support dtype combination is T : half, U : float");

    LastAxisShapeND originalSrcShape = {params.oriSrcM, params.oriSrcK};
    if (params.srcM == 0 || params.srcK == 0) {
        ShapeInfo srcShape = srcTensor.GetShapeInfo();
        originalSrcShape = GetLastAxisOriginShapeND(srcShape);
    }
    SoftmaxFlashV3Process<T, U, isUpdate, isBasicBlock, config>(
        dstTensor, meanTensor, expSumTensor, maxTensor, srcTensor, expMaxTensor, inMeanTensor, inexpSumTensor,
        inMaxTensor, workLocal, originalSrcShape, tiling, params);
}

template <
    typename T, typename U, bool isUpdate, bool isReuseSource, bool isBasicBlock, bool isDataFormatNZ,
    const SoftmaxConfig& config>
__aicore__ inline void SoftmaxFlashV3Impl(
    const LocalTensor<T>& dstTensor, const LocalTensor<U>& meanTensor, const LocalTensor<U>& expSumTensor,
    const LocalTensor<U>& maxTensor, const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor,
    const LocalTensor<U>& inMeanTensor, const LocalTensor<U>& inexpSumTensor, const LocalTensor<U>& inMaxTensor,
    const SoftMaxTiling& tiling, const SoftMaxParams& params)
{
    LocalTensor<float> workLocal;
    bool ans = PopStackBuffer<float, TPosition::LCM>(workLocal);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "SoftmaxFlashv3 PopStackBuffer Error!"); });
    SoftmaxFlashV3Impl<T, U, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
        dstTensor, meanTensor, expSumTensor, maxTensor, srcTensor, expMaxTensor, inMeanTensor, inexpSumTensor,
        inMaxTensor, workLocal, tiling, params);
}

template <
    typename T, typename U, bool isUpdate, bool isReuseSource, bool isBasicBlock, bool isDataFormatNZ,
    const SoftmaxConfig& config>
__aicore__ inline void SoftmaxFlashV3Impl(
    const LocalTensor<T>& dstTensor, const LocalTensor<U>& meanTensor, const LocalTensor<U>& expSumTensor,
    const LocalTensor<U>& maxTensor, const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor,
    const LocalTensor<U>& inMeanTensor, const LocalTensor<U>& inexpSumTensor, const LocalTensor<U>& inMaxTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, const SoftMaxParams& params)
{
    auto workLocal = sharedTmpBuffer.ReinterpretCast<float>();
    SoftmaxFlashV3Impl<T, U, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
        dstTensor, meanTensor, expSumTensor, maxTensor, srcTensor, expMaxTensor, inMeanTensor, inexpSumTensor,
        inMaxTensor, workLocal, tiling, params);
}
} // namespace AscendC
#endif // IMPL_ACTIVATION_SOFTMAX_SOFTMAX_FLASHV3_BASE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV3_BASE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV3_BASE_IMPL_H__
#endif
