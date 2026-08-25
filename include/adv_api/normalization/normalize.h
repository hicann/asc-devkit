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
 * \file noramlize.h
 * \brief Given mean and variance, calculate rstd and output.
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "normalize.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZE_H__
#endif

#ifndef LIB_NORMALIZATION_NORMALIZE_H
#define LIB_NORMALIZATION_NORMALIZE_H
#include "include/adv_api/normalization/normalize_utils.h"
#include "kernel_tensor.h"
#include "../../../impl/adv_api/detail/normalization/normalize/normalize_config.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
#include "../../../impl/adv_api/detail/normalization/normalize/normalize_common_impl.h"
#elif defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/normalization/normalize/normalize_3510_impl.h"
#endif
namespace AscendC {
#pragma begin_pipe(V)

/*!
 * \brief Applies Layer Normalization over a mini-batch of inputs,
 *        performs the operation of taking the reciprocal of the standard deviation of the intermediate output results.
 *
 * \note support data type: half and float
 *
 * \param [out] output, output LocalTensor, shape is [A, R]
 * \param [out] outputRstd, output LocalTensor, shape is [A]
 * \param [in] inputMean, input LocalTensor, shape is [A]
 * \param [in] inputVariance, input LocalTensor, shape is [A]
 * \param [in] inputX, input LocalTensor, shape is [A, R]
 * \param [in] gamma, input LocalTensor, shape is [R]
 * \param [in] beta, input LocalTensor, shape is [R]
 * \param [in] sharedTmpBuffer, input local temporary Tensor
 * \param [in] epsilon, weighting factor, prevent division by zero.
 * \param [in] para, NormalizePara struct, contains aLength, rLength and rLengthWithPadding.
 */
template <typename U, typename T, bool isReuseSource = false, const NormalizeConfig& config = NLCFG_NORM>
__aicore__ inline void Normalize(
    const LocalTensor<T>& output, const LocalTensor<float>& outputRstd, const LocalTensor<float>& inputMean,
    const LocalTensor<float>& inputVariance, const LocalTensor<T>& inputX, const LocalTensor<U>& gamma,
    const LocalTensor<U>& beta, const LocalTensor<uint8_t>& sharedTmpBuffer, const float epsilon,
    const NormalizePara& para)
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || \
                              __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    NormalizeImpl<U, T, isReuseSource, config>(
        output, outputRstd, inputMean, inputVariance, inputX, gamma, beta, sharedTmpBuffer, epsilon, para);
#endif
}

/*!
 * \brief Applies Layer Normalization over a mini-batch of inputs,
 *        performs the operation of taking the reciprocal of the standard deviation of the intermediate output results.
 *
 * \note support data type: half and float
 *
 * \param [out] output, output LocalTensor, shape is [A, R]
 * \param [out] outputRstd, output LocalTensor, shape is [A]
 * \param [in] inputMean, input LocalTensor, shape is [A]
 * \param [in] inputVariance, input LocalTensor, shape is [A]
 * \param [in] inputX, input LocalTensor, shape is [A, R]
 * \param [in] gamma, input LocalTensor, shape is [R]
 * \param [in] beta, input LocalTensor, shape is [R]
 * \param [in] epsilon, weighting factor, prevent division by zero.
 * \param [in] para, NormalizePara struct, contains aLength, rLength and rLengthWithPadding.
 */
template <typename U, typename T, bool isReuseSource = false, const NormalizeConfig& config = NLCFG_NORM>
__aicore__ inline void Normalize(
    const LocalTensor<T>& output, const LocalTensor<float>& outputRstd, const LocalTensor<float>& inputMean,
    const LocalTensor<float>& inputVariance, const LocalTensor<T>& inputX, const LocalTensor<U>& gamma,
    const LocalTensor<U>& beta, const float epsilon, const NormalizePara& para)
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || \
                              __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    NormalizeImpl<U, T, isReuseSource, config>(
        output, outputRstd, inputMean, inputVariance, inputX, gamma, beta, epsilon, para);
#endif
}
#pragma end_pipe
} // namespace AscendC
#endif // LIB_NORMALIZATION_NORMALIZE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZE_H__
#endif
