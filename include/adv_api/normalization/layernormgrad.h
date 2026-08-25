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
 * \file layernormgrad.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "layernormgrad.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORMGRAD_H__
#endif

#ifndef LIB_NORMALIZATION_LAYERNORMGRAD_H
#define LIB_NORMALIZATION_LAYERNORMGRAD_H

#include "kernel_tensor.h"
#include "include/adv_api/normalization/layernormgrad_utils.h"
#include "kernel_tiling/kernel_tiling.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
#include "../../../impl/adv_api/detail/normalization/layernormgrad/layernormgrad_common_impl.h"
#elif defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/normalization/layernormgrad/regbase/3510/layernormgrad_3510_impl.h"
#endif

namespace AscendC {
#pragma begin_pipe(V)
/*!
 * \brief layernormgrad intf function:
 * tips : x ^ y = exp (y * ln(x))
 * main process:
 * x1Tensor = inputDy * inputGamma
 * x2Tensor = inputX - inputMean
 * pd_var = np.sum(((-0.5) * x1Tensor * (x2Tensor) * np.power((inputVariace + EPSLON), (-1.5))), reduce_axis,
 * keepdims=True)
 * pd_mean = np.sum(((-1.0) * x1Tensor * np.power((inputVariace + EPSLON), (-0.5))), reduce_axis, keepdims = True) +
 * pd_var * (1.0/m) * np.sum(((-2.0) * (x2Tensor)), reduce_axis, keepdims=True)
 * pd_x = x1Tensor * np.power((inputVariace + EPSLON), (-0.5)) +
 * pd_var*(2.0 / H)*(x2Tensor) +
 * pd_mean*(1.0 / H)
 *
 * \note support data type: half and float
 *
 * \param [out] outputPdX, output LocalTensor, shape is [B, S, H]
 * \param [out] resForGamma, output LocalTensor, shape is [B, S, H]
 * \param [in] inputDy, input LocalTensor, shape is [B, S, H]
 * \param [in] inputX, input LocalTensor, shape is [B, S, H]
 * \param [in] inputVariance, input LocalTensor, shape is [B, S, 1]
 * \param [in] inputMean, input LocalTensor, shape is [B, S, 1]
 * \param [in] inputGamma, input LocalTensor, shape is [H]
 * \param [in] sharedTmpBuffer, input local temporary Tensor
 * \param [in] epsilon, weighting factor
 * \param [in] tiling, layernormgrad tiling
 * \param [in] shapeInfo, LayerNormGrad Shape Info
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void LayerNormGrad(
    const LocalTensor<T>& outputPdX, const LocalTensor<T>& resForGamma, const LocalTensor<T>& inputDy,
    const LocalTensor<T>& inputX, const LocalTensor<T>& inputVariance, const LocalTensor<T>& inputMean,
    const LocalTensor<T>& inputGamma, LocalTensor<uint8_t>& sharedTmpBuffer, T epsilon, LayerNormGradTiling& tiling,
    const LayerNormGradShapeInfo& shapeInfo = {})
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || \
                              __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    LayerNormGradImpl<T, isReuseSource>(
        outputPdX, resForGamma, inputDy, inputX, inputVariance, inputMean, inputGamma, sharedTmpBuffer, epsilon, tiling,
        shapeInfo);
#endif
}
/* !
 * \brief layernormgrad intf function
 *
 * \note support data type: half and float
 *
 * \param [out] outputPdX, output LocalTensor, shape is [B, S, H]
 * \param [out] resForGamma, output LocalTensor, shape is [B, S, H]
 * \param [in] inputDy, input LocalTensor, shape is [B, S, H]
 * \param [in] inputX, input LocalTensor, shape is [B, S, H]
 * \param [in] inputVariance, input LocalTensor, shape is [B, S, 1]
 * \param [in] inputMean, input LocalTensor, shape is [B, S, 1]
 * \param [in] inputGamma, input LocalTensor, shape is [H]
 * \param [in] epsilon, weighting factor
 * \param [in] tiling, layernormgrad tiling
 * \param [in] shapeInfo, LayerNormGrad Shape Info
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void LayerNormGrad(
    const LocalTensor<T>& outputPdX, const LocalTensor<T>& resForGamma, const LocalTensor<T>& inputDy,
    const LocalTensor<T>& inputX, const LocalTensor<T>& inputVariance, const LocalTensor<T>& inputMean,
    const LocalTensor<T>& inputGamma, T epsilon, LayerNormGradTiling& tiling,
    const LayerNormGradShapeInfo& shapeInfo = {})
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || \
                              __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    LayerNormGradImpl<T, isReuseSource>(
        outputPdX, resForGamma, inputDy, inputX, inputVariance, inputMean, inputGamma, epsilon, tiling, shapeInfo);
#endif
}
#pragma end_pipe
} // namespace AscendC
#endif // LIB_NORMALIZATION_LAYERNORMGRAD_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORMGRAD_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORMGRAD_H__
#endif
