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
 * \file layernormgradbeta.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "layernormgradbeta.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORMGRADBETA_H__
#endif

#ifndef LIB_NORMALIZATION_LAYERNORMGRADBETA_H
#define LIB_NORMALIZATION_LAYERNORMGRADBETA_H

#include "kernel_tensor.h"
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/normalization/layernormgrad/layernormgradbeta_3510_impl.h"
#else
#include "../../../impl/adv_api/detail/normalization/layernormgrad/layernormgradbeta_common_impl.h"
#endif
#include "kernel_tiling/kernel_tiling.h"

namespace AscendC {
#pragma begin_pipe(V)
/*!
 * \brief layernormgradbeta intf function:
 * reduce_axis = (0, 1)
 * pd_gamma = sum(inputDy*res_for_gamma, reduce_axis, keepdims=True)
 * pd_beta  = sum(inputDy, reduce_axis, keepdims=True)
 *
 * \note support data type: half and float
 *
 * \param [out] outputPdGamma, output LocalTensor, shape is [H]
 * \param [out] outputPdBeta, output LocalTensor, shape is [H]
 * \param [in] resForGamma, input LocalTensor, shape is [B, S, H]
 * \param [in] inputDy, input LocalTensor, shape is [B, S, H]
 * \param [in] sharedTmpBuffer, input local temporary Tensor
 * \param [in] tiling, layernormgradbeta tiling
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void LayerNormGradBeta(
    const LocalTensor<T>& outputPdGamma, const LocalTensor<T>& outputPdBeta, const LocalTensor<T>& resForGamma,
    const LocalTensor<T>& inputDy, const LocalTensor<uint8_t>& sharedTmpBuffer, const LayerNormGradBetaTiling& tiling)
{
    LayerNormGradBetaImpl<T, isReuseSource>(outputPdGamma, outputPdBeta, resForGamma, inputDy, sharedTmpBuffer, tiling);
}

/*!
 * \brief layernormgradbeta intf function
 *
 * \note support data type: half and float
 *
 * \param [out] outputPdGamma, output LocalTensor, shape is [H]
 * \param [out] outputPdBeta, output LocalTensor, shape is [H]
 * \param [in] resForGamma, input LocalTensor, shape is [B, S, H]
 * \param [in] inputDy, input LocalTensor, shape is [B, S, H]
 * \param [in] tiling, layernormgradbeta tiling
 */
template <typename T, bool isReuseSource = false>
__aicore__ inline void LayerNormGradBeta(
    const LocalTensor<T>& outputPdGamma, const LocalTensor<T>& outputPdBeta, const LocalTensor<T>& resForGamma,
    const LocalTensor<T>& inputDy, LayerNormGradBetaTiling& tiling)
{
    LayerNormGradBetaImpl<T, isReuseSource>(outputPdGamma, outputPdBeta, resForGamma, inputDy, tiling);
}
#pragma end_pipe
} // namespace AscendC
#endif // LIB_NORMALIZATION_LAYERNORMGRADBETA_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORMGRADBETA_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_LAYERNORMGRADBETA_H__
#endif
