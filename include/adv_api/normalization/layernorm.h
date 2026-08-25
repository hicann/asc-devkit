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
 * \file layernorm.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "layernorm.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORM_H__
#endif

#ifndef LIB_NORMALIZATION_LAYERNORM_H
#define LIB_NORMALIZATION_LAYERNORM_H
#include "include/adv_api/normalization/layernorm_utils.h"
#include "../../../impl/adv_api/detail/normalization/layernorm/layernorm_normal_config.h"
#include "kernel_tensor.h"
#include "kernel_tiling/kernel_tiling.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
#include "../../../impl/adv_api/detail/normalization/layernorm/layernorm_common_impl.h"
#elif defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/normalization/layernorm/layernorm_3510_impl.h"
#include "../../../impl/adv_api/detail/normalization/layernorm/regbase/3510/layernorm_variance_impl.h"
#endif
namespace AscendC {
#pragma begin_pipe(V)

/*!
 * \brief Applies Layer Normalization over a mini-batch of inputs as described in the paper Layer Normalization.
 * For details about the interface description, see
 * https://pytorch.org/docs/1.10/generated/torch.nn.LayerNorm.html.
 *
 * \note support data type: half and float
 *
 * \param [out] output, output LocalTensor, shape is [B, S, H]
 * \param [out] outputMean, output LocalTensor, shape is [B, S]
 * \param [out] outputVariance, output LocalTensor, shape is [B, S]
 * \param [in] inputX, input LocalTensor, shape is [B, S, H]
 * \param [in] gamma, input LocalTensor, shape is [H]
 * \param [in] beta, input LocalTensor, shape is [H]
 * \param [in] sharedTmpBuffer, input local temporary Tensor
 * \param [in] epsilon, weighting factor
 * \param [in] tiling, layernormtiling
 */
template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(2201, "LayerNorm is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void LayerNorm(
    const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance,
    const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, LayerNormTiling& tiling)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || \
                              __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    LayerNormImpl<T, isReuseSource>(
        output, outputMean, outputVariance, inputX, gamma, beta, sharedTmpBuffer, epsilon, tiling);
#endif
}

/*!
 * \brief Applies Layer Normalization over a mini-batch of inputs as described in the paper Layer Normalization.
 *
 * \note support data type: half and float
 *
 * \param [out] output, output LocalTensor, shape is [B, S, H]
 * \param [out] outputMean, output LocalTensor, shape is [B, S]
 * \param [out] outputVariance, output LocalTensor, shape is [B, S]
 * \param [in] inputX, input LocalTensor, shape is [B, S, H]
 * \param [in] gamma, input LocalTensor, shape is [H]
 * \param [in] beta, input LocalTensor, shape is [H]
 * \param [in] epsilon, weighting factor
 * \param [in] tiling, layernormtiling
 */
template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(2201, "LayerNorm is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void LayerNorm(
    const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance,
    const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta, const T epsilon,
    LayerNormTiling& tiling)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || \
                              __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    LayerNormImpl<T, isReuseSource>(output, outputMean, outputVariance, inputX, gamma, beta, epsilon, tiling);
#endif
}

/*!
 * \brief Applies Layer Normalization over a mini-batch of inputs as described in the paper Layer Normalization.

 * \note support data type: half and float
 *
 * \param [out] output, output LocalTensor, shape is [A, R]
 * \param [out] outputMean, output LocalTensor, shape is [A]
 * \param [out] outputRstd, output LocalTensor, shape is [A]
 * \param [in] inputX, input LocalTensor, shape is [A, R]
 * \param [in] gamma, input LocalTensor, shape is [R]
 * \param [in] beta, input LocalTensor, shape is [R]
 * \param [in] epsilon, weighting factor
 * \param [in] para, LayerNormPara
 * \param [in] tiling, LayerNormSeparateTiling
 */
template <typename U, typename T, bool isReuseSource = false, const LayerNormConfig& config = LNCFG_NORM>
__ASC_USE_RESERVED_UBUF__(2201, "LayerNorm is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void LayerNorm(
    const LocalTensor<T>& output, const LocalTensor<float>& outputMean, const LocalTensor<float>& outputRstd,
    const LocalTensor<T>& inputX, const LocalTensor<U>& gamma, const LocalTensor<U>& beta, const float epsilon,
    const LayerNormPara& para, const LayerNormSeparateTiling& tiling)
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || \
                              __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    LayerNormImpl<U, T, isReuseSource, config>(
        output, outputMean, outputRstd, inputX, gamma, beta, epsilon, para, tiling);
#endif
}

/*!
 * \brief Applies Layer Normalization over a mini-batch of inputs as described in the paper Layer Normalization.

 * \note support data type: half and float
 *
 * \param [out] output, output LocalTensor, shape is [A, R]
 * \param [out] outputMean, output LocalTensor, shape is [A]
 * \param [out] outputRstd, output LocalTensor, shape is [A]
 * \param [in] inputX, input LocalTensor, shape is [A, R]
 * \param [in] gamma, input LocalTensor, shape is [R]
 * \param [in] beta, input LocalTensor, shape is [R]
 * \param [in] epsilon, weighting factor
 * \param [in] sharedTmpBuffer, input local temporary Tensor
 * \param [in] para, LayerNormPara
 * \param [in] tiling, LayerNormSeparateTiling
 */
template <typename U, typename T, bool isReuseSource = false, const LayerNormConfig& config = LNCFG_NORM>
__ASC_USE_RESERVED_UBUF__(2201, "LayerNorm is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void LayerNorm(
    const LocalTensor<T>& output, const LocalTensor<float>& outputMean, const LocalTensor<float>& outputRstd,
    const LocalTensor<T>& inputX, const LocalTensor<U>& gamma, const LocalTensor<U>& beta, const float epsilon,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const LayerNormPara& para, const LayerNormSeparateTiling& tiling)
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || \
                              __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    LayerNormImpl<U, T, isReuseSource, config>(
        output, outputMean, outputRstd, inputX, gamma, beta, epsilon, sharedTmpBuffer, para, tiling);
#endif
}

/*!
 * \brief Calculate the mean and variance for each time using the Welford algorithm.
 *
 * \note support data type: T(half and float)、U(float)
 *
 * \param [out] outputMean, output LocalTensor, shape is [A, R]
 * \param [out] outputVariance, output LocalTensor, shape is [A, R]
 * \param [in] inputMean, input LocalTensor, shape is [A, R]
 * \param [in] inputVariance, input LocalTensor, shape is [A, R]
 * \param [in] inputX, input LocalTensor, shape is [A, R]
 * \param [in] para, para detailed information about the original data shape
 */
template <typename T, typename U, bool isReuseSource = false, const WelfordUpdateConfig& config = WFUPDATE_DEFAULT_CFG>
__aicore__ inline void WelfordUpdate(
    const LocalTensor<U>& outputMean, const LocalTensor<U>& outputVariance, const LocalTensor<U>& inputMean,
    const LocalTensor<U>& inputVariance, const LocalTensor<T>& inputX, const WelfordUpdateParam& para)
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || \
                              __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    WelfordUpdateImpl<T, U, isReuseSource, config>(outputMean, outputVariance, inputMean, inputVariance, inputX, para);
#endif
}

/*!
 * \brief Calculate the mean and variance for each time using the Welford algorithm.
 *
 * \note support data type: T(half and float)、U(float)
 *
 * \param [out] outputMean, output LocalTensor, shape is [A, R]
 * \param [out] outputVariance, output LocalTensor, shape is [A, R]
 * \param [in] inputMean, input LocalTensor, shape is [A, R]
 * \param [in] inputVariance, input LocalTensor, shape is [A, R]
 * \param [in] inputX, input LocalTensor, shape is [A, R]
 * \param [in] sharedTmpBuffer, input local temporary Tensor
 * \param [in] para, para detailed information about the original data shape
 */
template <typename T, typename U, bool isReuseSource = false, const WelfordUpdateConfig& config = WFUPDATE_DEFAULT_CFG>
__aicore__ inline void WelfordUpdate(
    const LocalTensor<U>& outputMean, const LocalTensor<U>& outputVariance, const LocalTensor<U>& inputMean,
    const LocalTensor<U>& inputVariance, const LocalTensor<T>& inputX, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const WelfordUpdateParam& para)
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || \
                              __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    WelfordUpdateImpl<T, U, isReuseSource, config>(
        outputMean, outputVariance, inputMean, inputVariance, inputX, sharedTmpBuffer, para);
#endif
}
#pragma end_pipe
} // namespace AscendC
#endif // LIB_NORMALIZATION_LAYERNORM_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORM_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORM_H__
#endif
