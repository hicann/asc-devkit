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
 * \file groupnorm.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "groupnorm.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_GROUPNORM_H__
#endif

#ifndef LIB_NORMALIZATION_GROUPNORM_H
#define LIB_NORMALIZATION_GROUPNORM_H
#include "kernel_tensor.h"
#include "kernel_tiling/kernel_tiling.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002)
#include "../../../impl/adv_api/detail/normalization/groupnorm/groupnorm_common_impl.h"
#elif defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/normalization/groupnorm/groupnorm_3510_impl.h"
#endif
namespace AscendC {
#pragma begin_pipe(V)
/*!
 * \brief Applies Group Normalization over a mini-batch of inputs as described in the paper Group Normalization.
 *
 * \note support data type: half and float
 *
 * \param [out] output, output LocalTensor, shape is [n, C, H, W]
 * \param [out] outputMean, output LocalTensor, shape is [n, groupNum]
 * \param [out] outputVariance, output LocalTensor, shape is [n, groupNum]
 * \param [in] inputX, input LocalTensor, shape is [n, C, H, W]
 * \param [in] gamma, input LocalTensor, shape is [C]
 * \param [in] beta, input LocalTensor, shape is [C]
 * \param [in] sharedTmpBuffer, input local temporary Tensor
 * \param [in] epsilon, weighting factor
 * \param [in] tiling, groupnormtiling
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void GroupNorm(
    const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance,
    const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, GroupNormTiling& tiling)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || \
                              __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    GroupNormImpl<T, isReuseSource>(
        output, outputMean, outputVariance, inputX, gamma, beta, sharedTmpBuffer, epsilon, tiling);
#endif
}

/*!
 * \brief Applies Group Normalization over a mini-batch of inputs as described in the paper Group Normalization.
 *
 * \note support data type: half and float
 *
 * \param [out] output, output LocalTensor, shape is [n, C, H, W]
 * \param [out] outputMean, output LocalTensor, shape is [n, groupNum]
 * \param [out] outputVariance, output LocalTensor, shape is [n, groupNum]
 * \param [in] inputX, input LocalTensor, shape is [n, C, H, W]
 * \param [in] gamma, input LocalTensor, shape is [C]
 * \param [in] beta, input LocalTensor, shape is [C]
 * \param [in] epsilon, weighting factor
 * \param [in] tiling, groupnormtiling
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void GroupNorm(
    const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance,
    const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta, const T epsilon,
    GroupNormTiling& tiling)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || \
                              __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    GroupNormImpl<T, isReuseSource>(output, outputMean, outputVariance, inputX, gamma, beta, epsilon, tiling);
#endif
}
#pragma end_pipe
} // namespace AscendC
#endif // LIB_NORMALIZATION_GROUPNORM_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_GROUPNORM_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_GROUPNORM_H__
#endif
