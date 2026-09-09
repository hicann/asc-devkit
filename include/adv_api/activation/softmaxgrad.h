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
 * \file softmaxgrad.h
 * \brief SoftmaxGrad and SoftmaxGradFront api of AscendC
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "softmaxgrad.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAXGRAD_H__
#endif

#ifndef LIB_SOFTMAX_SOFTMAXGRAD_H
#define LIB_SOFTMAX_SOFTMAXGRAD_H

#include "kernel_tensor.h"
#include "kernel_tiling/kernel_tiling.h"
#include "include/adv_api/activation/softmax_utils.h"
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/activation/softmax/softmax_common.h"
#include "../../../impl/adv_api/detail/activation/softmax/softmax_grad_base_impl.h"
#endif

#pragma begin_pipe(V)
namespace AscendC {
/*!
 * \ingroup SoftmaxGrad
 * \brief compute process: sum = rowsum(grad * x), grad * x - sum * x
 *                         if isFront = true
 *                             y = sum
 *                         if isFront = false
 *                             y = grad * x - sum * x
 * \note support data type: half and float
 * \param [out] dstTensor: output y
 * \param [in] gradTensor: input grad
 * \param [in] srcTensor: input x
 * \param [in] softmaxShapeInfo: input x shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isFront: compute mode refer to brief
 * \param [in] isReuseSource: reserved param
 * \param [in] isDataFormatNZ: if the data format of input srcTensor is NZ
 */
template <typename T, bool isReuseSource = false, bool isDataFormatNZ = false>
__aicore__ inline void SoftmaxGrad(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor,
    const SoftMaxTiling& tiling, bool isFront = false, const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    TRACE_START(TraceId::SoftmaxGrad);
    SoftmaxGradImpl<T, isReuseSource, isDataFormatNZ>(
        dstTensor, gradTensor, srcTensor, tiling, isFront, softmaxShapeInfo);
    TRACE_STOP(TraceId::SoftmaxGrad);
#endif
}

/*!
 * \ingroup SoftmaxGradFront
 * \brief compute process: y = rowsum(grad * x)
 * \note support data type: half and float
 * \param [out] dstTensor: output y
 * \param [in] gradTensor: input grad
 * \param [in] srcTensor: input x
 * \param [in] softmaxShapeInfo: input x shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isBasicBlock: if src shape[m,k] satisfy the condition(m%8 == 0 && k%64 == 0), you can set true to improve
 *                           performance , but it is a reserved param when isDataFormatNZ = true
 * \param [in] isDataFormatNZ: if the data format of input srcTensor is NZ
 */
template <typename T, bool isBasicBlock = false, bool isDataFormatNZ = false>
__aicore__ inline void SoftmaxGradFront(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor,
    const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    TRACE_START(TraceId::SoftmaxGrad);
    SoftmaxGradFrontImpl<T, isBasicBlock, isDataFormatNZ>(dstTensor, gradTensor, srcTensor, tiling, softmaxShapeInfo);
    TRACE_STOP(TraceId::SoftmaxGrad);
#endif
}

/*!
 * \ingroup SoftmaxGrad
 * \brief compute process: sum = rowsum(grad * x), grad * x - sum * x
 *                         if isFront = true
 *                             y = sum
 *                         if isFront = false
 *                             y = grad * x - sum * x
 * \note support data type: half and float
 * \param [out] dstTensor: output y
 * \param [in] gradTensor: input grad
 * \param [in] srcTensor: input x
 * \param [in] softmaxShapeInfo: input x shape
 * \param [in] sharedTmpBuffer: input local temporary Tensor,you can get the range by tilingfunc of
 *                              GetSoftMaxGradMinTmpSize/GetSoftMaxGradMaxTmpSize
 * \param [in] tiling: input softmaxtiling
 * \param [in] isFront: compute mode refer to brief
 * \param [in] isReuseSource: reserved param
 * \param [in] isDataFormatNZ: if the data format of input srcTensor is NZ
 */
template <typename T, bool isReuseSource = false, bool isDataFormatNZ = false>
__aicore__ inline void SoftmaxGrad(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling, bool isFront = false,
    const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    TRACE_START(TraceId::SoftmaxGrad);
    SoftmaxGradImpl<T, isReuseSource, isDataFormatNZ>(
        dstTensor, gradTensor, srcTensor, sharedTmpBuffer, tiling, isFront, softmaxShapeInfo);
    TRACE_STOP(TraceId::SoftmaxGrad);
#endif
}

/*!
 * \ingroup SoftmaxGradFront
 * \brief compute process: y = rowsum(grad * x)
 * \note support data type: half and float
 * \param [out] dstTensor: output y
 * \param [in] gradTensor: input grad
 * \param [in] srcTensor: input x
 * \param [in] softmaxShapeInfo: input x shape
 * \param [in] sharedTmpBuffer: input local temporary Tensor,you can get the range by tilingfunc of
 *                              GetSoftMaxGradMinTmpSize/GetSoftMaxGradMaxTmpSize
 * \param [in] tiling: input softmaxtiling
 * \param [in] isBasicBlock: if src shape[m,k] satisfy the condition(m%8 == 0 && k%64 == 0), you can set true to improve
 *                           performance , but it is a reserved param when isDataFormatNZ = true
 * \param [in] isDataFormatNZ: if the data format of input srcTensor is NZ
 */
template <typename T, bool isBasicBlock = false, bool isDataFormatNZ = false>
__aicore__ inline void SoftmaxGradFront(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& gradTensor, const LocalTensor<T>& srcTensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling,
    const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    SoftmaxGradFrontImpl<T, isBasicBlock, isDataFormatNZ>(
        dstTensor, gradTensor, srcTensor, sharedTmpBuffer, tiling, softmaxShapeInfo);
#endif
}
} // namespace AscendC
#pragma end_pipe
#endif // LIB_SOFTMAX_SOFTMAXGRAD_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAXGRAD_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAXGRAD_H__
#endif
