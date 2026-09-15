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
 * \file layernorm_check.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/api_check/kernel_check/normalization/layernorm/layernorm_check.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/layernorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORM_CHECK_H__
#endif

#ifndef IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_LAYERNORM_LAYERNORM_CHECK_H_
#define IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_LAYERNORM_LAYERNORM_CHECK_H_

#include "kernel_tiling/kernel_tiling.h"
#include "../../../../../../../include/adv_api/normalization/layernorm_utils.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
#include "layernorm_check_common.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "layernorm_check_310.h"
#else
#include "layernorm_check_aicore.h"
#endif

namespace AscendC {
namespace HighLevelApiCheck {

template <typename T, bool isReuseSource = false>
__aicore__ inline void CheckFuncLayerNorm(
    __gm__ const char* apiName, const LocalTensor<T>& output, const LocalTensor<T>& outputMean,
    const LocalTensor<T>& outputVariance, const LocalTensor<T>& inputX, const LocalTensor<T>& gamma,
    const LocalTensor<T>& beta, const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, LayerNormTiling& tiling)
{
    CheckFuncClassLayerNorm<T, isReuseSource> checkFun(apiName);
    checkFun.VerifyingParameters(
        output, outputMean, outputVariance, inputX, gamma, beta, sharedTmpBuffer, epsilon, tiling);
}

template <typename U, typename T, bool isReuseSource = false, const LayerNormConfig& config = LNCFG_NORM>
__aicore__ inline void CheckFuncLayerNorm(
    __gm__ const char* apiName, const LocalTensor<T>& output, const LocalTensor<float>& outputMean,
    const LocalTensor<float>& outputRstd, const LocalTensor<T>& inputX, const LocalTensor<U>& gamma,
    const LocalTensor<U>& beta, const float epsilon, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const LayerNormPara& para, const LayerNormSeparateTiling& tiling)
{
    CheckFuncClassLayerNormRstd<U, T, isReuseSource, config> checkFun(apiName);
    checkFun.VerifyingParameters(
        output, outputMean, outputRstd, inputX, gamma, beta, epsilon, sharedTmpBuffer, para, tiling);
}

} // namespace HighLevelApiCheck
} // namespace AscendC
#endif // IMPL_API_CHECK_KERNEL_CHECK_NORMALIZATION_LAYERNORM_LAYERNORM_CHECK_H_

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORM_CHECK_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORM_CHECK_H__
#endif
