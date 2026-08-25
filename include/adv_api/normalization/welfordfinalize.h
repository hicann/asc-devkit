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
 * \file welfordfinalize.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "welfordfinalize.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_WELFORDFINALIZE_H__
#endif

#ifndef LIB_NORMALIZATION_WELFORDFINALIZE_H
#define LIB_NORMALIZATION_WELFORDFINALIZE_H
#include "include/adv_api/normalization/welfordfinalize_utils.h"
#include "kernel_tensor.h"
#include "kernel_tiling/kernel_tiling.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
#include "../../../impl/adv_api/detail/normalization/welfordfinalize/welfordfinalize_common_impl.h"
#elif defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/normalization/welfordfinalize/welfordfinalize_3510_impl.h"
#endif
namespace AscendC {
#pragma begin_pipe(V)
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
/*!
 * \brief Calculate the final mean and variance using the Welford algorithm.
 *
 * \note support data type: float
 *
 * \param [out] outputMean, output LocalTensor, shape is [8]
 * \param [out] outputVariance, output LocalTensor, shape is [8]
 * \param [in] inputMean, input LocalTensor, shape is [abLength]
 * \param [in] inputVariance, input LocalTensor, shape is [abLength]
 * \param [in] sharedTmpBuffer, input local temporary Tensor
 * \param [in] para, para detailed information about the original data shape
 */
template <bool isReuseSource = false>
__aicore__ inline void WelfordFinalize(
    const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance, const LocalTensor<float>& inputMean,
    const LocalTensor<float>& inputVariance, const LocalTensor<uint8_t>& sharedTmpBuffer, WelfordFinalizePara& para)
{
    if ASCEND_IS_AIC {
        return;
    }
    WelfordFinalizeImpl<isReuseSource>(outputMean, outputVariance, inputMean, inputVariance, sharedTmpBuffer, para);
}
/*!
 * \brief Calculate the final mean and variance using the Welford algorithm.
 *
 * \note support data type: float
 *
 * \param [out] outputMean, output LocalTensor, shape is [8]
 * \param [out] outputVariance, output LocalTensor, shape is [8]
 * \param [in] inputMean, input LocalTensor, shape is [abLength]
 * \param [in] inputVariance, input LocalTensor, shape is [abLength]
 * \param [in] counts, input LocalTensor, shape is [abLength]
 * \param [in] sharedTmpBuffer, input local temporary Tensor
 * \param [in] para, para detailed information about the original data shape
 */
template <bool isReuseSource = false>
__aicore__ inline void WelfordFinalize(
    const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance, const LocalTensor<float>& inputMean,
    const LocalTensor<float>& inputVariance, const LocalTensor<int32_t>& counts,
    const LocalTensor<uint8_t>& sharedTmpBuffer, WelfordFinalizePara& para)
{
    if ASCEND_IS_AIC {
        return;
    }
    WelfordFinalizeImpl<isReuseSource>(
        outputMean, outputVariance, inputMean, inputVariance, counts, sharedTmpBuffer, para);
}

/*!
 * \brief Calculate the final mean and variance using the Welford algorithm.
 *
 * \note support data type: float
 *
 * \param [out] outputMean, output LocalTensor, shape is [8]
 * \param [out] outputVariance, output LocalTensor, shape is [8]
 * \param [in] inputMean, input LocalTensor, shape is [abLength]
 * \param [in] inputVariance, input LocalTensor, shape is [abLength]
 * \param [in] para, para detailed information about the original data shape
 */
template <bool isReuseSource = false>
__aicore__ inline void WelfordFinalize(
    const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance, const LocalTensor<float>& inputMean,
    const LocalTensor<float>& inputVariance, WelfordFinalizePara& para)
{
    if ASCEND_IS_AIC {
        return;
    }
    WelfordFinalizeImpl<isReuseSource>(outputMean, outputVariance, inputMean, inputVariance, para);
}

/*!
 * \brief Calculate the final mean and variance using the Welford algorithm.
 *
 * \note support data type: float
 *
 * \param [out] outputMean, output LocalTensor, shape is [8]
 * \param [out] outputVariance, output LocalTensor, shape is [8]
 * \param [in] inputMean, input LocalTensor, shape is [abLength]
 * \param [in] inputVariance, input LocalTensor, shape is [abLength]
 * \param [in] counts, input LocalTensor, shape is [abLength]
 * \param [in] para, para detailed information about the original data shape
 */
template <bool isReuseSource = false>
__aicore__ inline void WelfordFinalize(
    const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance, const LocalTensor<float>& inputMean,
    const LocalTensor<float>& inputVariance, const LocalTensor<int32_t>& counts, WelfordFinalizePara& para)
{
    if ASCEND_IS_AIC {
        return;
    }
    WelfordFinalizeImpl<isReuseSource>(outputMean, outputVariance, inputMean, inputVariance, counts, para);
}
#else
/*!
 * \brief Calculate the final mean and variance using the Welford algorithm.
 *
 * \note support data type: float
 *
 * \param [out] outputMean, output LocalTensor, shape is [8]
 * \param [out] outputVariance, output LocalTensor, shape is [8]
 * \param [in] inputMean, input LocalTensor, shape is [abLength]
 * \param [in] inputVariance, input LocalTensor, shape is [abLength]
 * \param [in] sharedTmpBuffer, input local temporary Tensor
 * \param [in] para, para detailed information about the original data shape
 */
template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
__aicore__ inline void WelfordFinalize(
    const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance, const LocalTensor<float>& inputMean,
    const LocalTensor<float>& inputVariance, const LocalTensor<uint8_t>& sharedTmpBuffer, WelfordFinalizePara& para)
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    WelfordFinalizeImpl<isReuseSource, config>(
        outputMean, outputVariance, inputMean, inputVariance, sharedTmpBuffer, para);
#endif
}
/*!
 * \brief Calculate the final mean and variance using the Welford algorithm.
 *
 * \note support data type: float
 *
 * \param [out] outputMean, output LocalTensor, shape is [8]
 * \param [out] outputVariance, output LocalTensor, shape is [8]
 * \param [in] inputMean, input LocalTensor, shape is [abLength]
 * \param [in] inputVariance, input LocalTensor, shape is [abLength]
 * \param [in] counts, input LocalTensor, shape is [abLength]
 * \param [in] sharedTmpBuffer, input local temporary Tensor
 * \param [in] para, para detailed information about the original data shape
 */
template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
__aicore__ inline void WelfordFinalize(
    const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance, const LocalTensor<float>& inputMean,
    const LocalTensor<float>& inputVariance, const LocalTensor<int32_t>& counts,
    const LocalTensor<uint8_t>& sharedTmpBuffer, WelfordFinalizePara& para)
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    WelfordFinalizeImpl<isReuseSource, config>(
        outputMean, outputVariance, inputMean, inputVariance, counts, sharedTmpBuffer, para);
#endif
}

/*!
 * \brief Calculate the final mean and variance using the Welford algorithm.
 *
 * \note support data type: float
 *
 * \param [out] outputMean, output LocalTensor, shape is [8]
 * \param [out] outputVariance, output LocalTensor, shape is [8]
 * \param [in] inputMean, input LocalTensor, shape is [abLength]
 * \param [in] inputVariance, input LocalTensor, shape is [abLength]
 * \param [in] para, para detailed information about the original data shape
 */
template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
__aicore__ inline void WelfordFinalize(
    const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance, const LocalTensor<float>& inputMean,
    const LocalTensor<float>& inputVariance, WelfordFinalizePara& para)
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    WelfordFinalizeImpl<isReuseSource, config>(outputMean, outputVariance, inputMean, inputVariance, para);
#endif
}

/*!
 * \brief Calculate the final mean and variance using the Welford algorithm.
 *
 * \note support data type: float
 *
 * \param [out] outputMean, output LocalTensor, shape is [8]
 * \param [out] outputVariance, output LocalTensor, shape is [8]
 * \param [in] inputMean, input LocalTensor, shape is [abLength]
 * \param [in] inputVariance, input LocalTensor, shape is [abLength]
 * \param [in] counts, input LocalTensor, shape is [abLength]
 * \param [in] para, para detailed information about the original data shape
 */
template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
__aicore__ inline void WelfordFinalize(
    const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance, const LocalTensor<float>& inputMean,
    const LocalTensor<float>& inputVariance, const LocalTensor<int32_t>& counts, WelfordFinalizePara& para)
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    WelfordFinalizeImpl<isReuseSource, config>(outputMean, outputVariance, inputMean, inputVariance, counts, para);
#endif
}
#endif
#pragma end_pipe
} // namespace AscendC
#endif // LIB_NORMALIZATION_WELFORDFINALIZE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_WELFORDFINALIZE_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_WELFORDFINALIZE_H__
#endif
