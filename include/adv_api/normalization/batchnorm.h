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
 * \file batchnorm.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "batchnorm.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_BATCHNORM_H__
#endif

#ifndef LIB_NORMALIZATION_BATCHNORM_H
#define LIB_NORMALIZATION_BATCHNORM_H

#include "kernel_tensor.h"
#include "kernel_basic_intf.h"
#include "../../../impl/basic_api/kernel_pop_stack_buffer.h"
#include "kernel_tiling/kernel_tiling.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
#include "../../../impl/adv_api/detail/normalization/batchnorm/batchnorm_common_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 3510
#include "../../../impl/adv_api/detail/normalization/batchnorm/batchnorm_3510_impl.h"
#endif
namespace AscendC {
#pragma begin_pipe(V)
/* **************************************************************************************************
 * BatchNorm                                             *
 * ************************************************************************************************** */
/*
 * @ingroup BatchNorm
 * @brief compute output = gamm * (x - outputMean) * rsqrt(outputVariance + epsilon) + beta
 * @brief compute outputMean = sum(x) / batch
 * @brief compute outputVariance = sqrt(x - outputMean) / batch
 * @param [out] output output LocalTensor
 * @param [out] outputMean output LocalTensor
 * @param [out] outputVariance output  LocalTensor
 * @param [in] inputX input LocalTensor
 * @param [in] gamm input LocalTensor
 * @param [in] beta input LocalTensor
 * @param [in] sharedTmpBuffer input local temporary Tensor
 * @param [in] epsilon
 * @param [in] tiling batchnormtiling
 */
template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
__aicore__ inline void BatchNorm(
    const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance,
    const LocalTensor<T>& inputX, const LocalTensor<T>& gamm, const LocalTensor<T>& beta,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, BatchNormTiling& tiling)
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510)
    BatchNormAPI::BatchNormImpl<T, isReuseSource, isBasicBlock>(
        output, outputMean, outputVariance, inputX, gamm, beta, sharedTmpBuffer, epsilon, tiling);
#endif
}

template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
__aicore__ inline void BatchNorm(
    const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance,
    const LocalTensor<T>& inputX, const LocalTensor<T>& gamm, const LocalTensor<T>& beta, const T epsilon,
    BatchNormTiling& tiling)
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510)
    BatchNormAPI::BatchNormImpl<T, isReuseSource, isBasicBlock>(
        output, outputMean, outputVariance, inputX, gamm, beta, epsilon, tiling);
#endif
}
#pragma end_pipe
} // namespace AscendC
#endif // LIB_NORMALIZATION_BATCHNORM_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_BATCHNORM_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_BATCHNORM_H__
#endif
