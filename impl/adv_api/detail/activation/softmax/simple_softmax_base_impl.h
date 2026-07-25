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
 * \file simple_softmax_base_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/activation/softmax/simple_softmax_base_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/simplesoftmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIMPLE_SOFTMAX_BASE_IMPL_H__
#endif
#ifndef IMPL_ACTIVATION_SOFTMAX_SIMPLE_SOFTMAX_BASE_IMPL_H
#define IMPL_ACTIVATION_SOFTMAX_SIMPLE_SOFTMAX_BASE_IMPL_H

#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "regbase/3510/simple_softmax_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 3002
#include "regbase/v300/simple_softmax_impl.h"
#include "softmax_common/softmax_common_simple.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
#include "membase/v220/simple_softmax_impl.h"
#include "softmax_common/softmax_common_simple.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
#include "membase/v200/simple_softmax_impl.h"
#include "softmax_common/softmax_common_simple.h"
#endif
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/activation/softmax/simple_softmax/simple_softmax_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
template <
    typename T1, typename T2, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SimpleSoftMaxImpl(
    const LocalTensor<T1>& dst, const LocalTensor<T2>& inSumTensor, const LocalTensor<T2>& inMaxTensor,
    const LocalTensor<T1>& src, const LocalTensor<float>& workLocal, const SoftMaxTiling& tiling,
    const SoftMaxShapeInfo& softmaxShapeInfo)
{
    CHECK_FUNC_HIGHLEVEL_API(
        SimpleSoftMax, (T1, T2, isReuseSource, isBasicBlock, isDataFormatNZ, config),
        (dst, inSumTensor, inMaxTensor, src, workLocal, tiling, softmaxShapeInfo));
    SimpleSoftMaxBaseImpl<T1, T2, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
        dst, inSumTensor, inMaxTensor, src, workLocal, tiling, softmaxShapeInfo);
}

template <
    typename T1, typename T2, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SimpleSoftMaxImpl(
    const LocalTensor<T1>& dst, const LocalTensor<T2>& inSumTensor, const LocalTensor<T2>& inMaxTensor,
    const LocalTensor<T1>& src, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo)
{
    LocalTensor<float> workLocal;
    PopStackBuffer<float, TPosition::LCM>(workLocal);
    SimpleSoftMaxImpl<T1, T2, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
        dst, inSumTensor, inMaxTensor, src, workLocal, tiling, softmaxShapeInfo);
}

template <
    typename T1, typename T2, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SimpleSoftMaxImpl(
    const LocalTensor<T1>& dst, const LocalTensor<T2>& inSumTensor, const LocalTensor<T2>& inMaxTensor,
    const LocalTensor<T1>& src, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling,
    const SoftMaxShapeInfo& softmaxShapeInfo)
{
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    CheckTensorPos<uint8_t>(sharedTmpBuffer, Hardware::UB, "sharedTmpBuffer", "VECIN/VECCALC/VECOUT", "SimpleSoftMax");
#endif
    auto workLocal = sharedTmpBuffer.ReinterpretCast<float>();
    SimpleSoftMaxImpl<T1, T2, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
        dst, inSumTensor, inMaxTensor, src, workLocal, tiling, softmaxShapeInfo);
}
} // namespace AscendC
#endif // IMPL_ACTIVATION_SOFTMAX_SIMPLE_SOFTMAX_BASE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIMPLE_SOFTMAX_BASE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIMPLE_SOFTMAX_BASE_IMPL_H__
#endif
