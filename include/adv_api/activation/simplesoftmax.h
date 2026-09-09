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
 * \file simplesoftmax.h
 * \brief SimpleSoftMax api of AscendC
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "simplesoftmax.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIMPLESOFTMAX_H__
#endif

#ifndef LIB_SOFTMAX_SIMPLESOFTMAX_H
#define LIB_SOFTMAX_SIMPLESOFTMAX_H

#include "kernel_tensor.h"
#include "kernel_tiling/kernel_tiling.h"
#include "include/adv_api/activation/softmax_utils.h"
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/activation/softmax/softmax_common.h"
#include "../../../impl/adv_api/detail/activation/softmax/simple_softmax_base_impl.h"
#endif

#pragma begin_pipe(V)
namespace AscendC {
/*!
 * \ingroup SimpleSoftMax
 * \brief compute process: y = exp(x-inmax)/insum
 * \note support data type: half and float
 * \param [out] dstTensor: output y
 * \param [in] inSumTensor: input insum
 * \param [in] inMaxTensor: input inmax
 * \param [in] srcTensor: input x
 * \param [in] softmaxShapeInfo: input srcTensor shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isReuseSource: reserved param
 * \param [in] isBasicBlock: if srcTensor shape[m,k] satisfy the condition(m%8 == 0 && k%64 == 0),you can set true to
 *                           improve performance, but it is a reserved param when isDataFormatNZ = true
 * \param [in] isDataFormatNZ: if the data format of input srcTensor is NZ
 */
template <
    typename T, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SimpleSoftMax(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& inSumTensor, const LocalTensor<T>& inMaxTensor,
    const LocalTensor<T>& srcTensor, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    SimpleSoftMaxImpl<T, T, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
        dstTensor, inSumTensor, inMaxTensor, srcTensor, tiling, softmaxShapeInfo);
#endif
}

/*!
 * \ingroup SimpleSoftMax
 * \brief compute process: y = exp(x-inmax)/insum
 * \param [out] dstTensor: output y with dtype of half
 * \param [in] inSumTensor: input insum with dtype of float
 * \param [in] inMaxTensor: input inmax with dtype of float
 * \param [in] srcTensor: input x with dtype of half
 * \param [in] softmaxShapeInfo: input srcTensor shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isReuseSource: reserved param
 * \param [in] isBasicBlock: if srcTensor shape[m,k] satisfy the condition(m%8 == 0 && k%64 == 0), you can set true to
 *                           improve performance, but it is a reserved param when isDataFormatNZ = true
 * \param [in] isDataFormatNZ: if the data format of input srcTensor is NZ
 */
template <
    typename T, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SimpleSoftMax(
    const LocalTensor<half>& dstTensor, const LocalTensor<float>& inSumTensor, const LocalTensor<float>& inMaxTensor,
    const LocalTensor<half>& srcTensor, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    SimpleSoftMaxImpl<half, float, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
        dstTensor, inSumTensor, inMaxTensor, srcTensor, tiling, softmaxShapeInfo);
#endif
}

/*!
 * \ingroup SimpleSoftMax
 * \brief compute process: y = exp(x-inmax)/insum
 * \note support data type: half and float
 * \param [out] dstTensor: output y
 * \param [in] inSumTensor: input insum
 * \param [in] inMaxTensor: input inmax
 * \param [in] srcTensor: input x
 * \param [in] sharedTmpBuffer: input local temporary Tensor,you can get the range by tilingfunc of
 *                              GetSoftMaxMinTmpSize/GetSoftMaxMaxTmpSize
 * \param [in] softmaxShapeInfo: input srcTensor shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isReuseSource: reserved param
 * \param [in] isBasicBlock: if srcTensor shape[m,k] satisfy the condition(m%8 == 0 && k%64 == 0), you can set true to
 *                           improve performance, but it is a reserved param when isDataFormatNZ = true
 * \param [in] isDataFormatNZ: if the data format of input srcTensor is NZ
 */
template <
    typename T, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SimpleSoftMax(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& inSumTensor, const LocalTensor<T>& inMaxTensor,
    const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling,
    const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    SimpleSoftMaxImpl<T, T, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
        dstTensor, inSumTensor, inMaxTensor, srcTensor, sharedTmpBuffer, tiling, softmaxShapeInfo);
#endif
}

/*!
 * \ingroup SimpleSoftMax
 * \brief compute process: y = exp(x-inmax)/insum
 * \param [out] dstTensor: output y with dtype of half
 * \param [in] inSumTensor: input insum with dtype of float
 * \param [in] inMaxTensor: input inmax with dtype of float
 * \param [in] srcTensor: input x with dtype of half
 * \param [in] sharedTmpBuffer: input local temporary Tensor,you can get the range by tilingfunc of
 *                              GetSoftMaxMinTmpSize/GetSoftMaxMaxTmpSize
 * \param [in] softmaxShapeInfo: input srcTensor shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isReuseSource: reserved param
 * \param [in] isBasicBlock: if srcTensor shape[m,k] satisfy the condition(m%8 == 0 && k%64 == 0), you can set true to
 *                           improve performance, but it is a reserved param when isDataFormatNZ = true
 * \param [in] isDataFormatNZ: if the data format of input srcTensor is NZ
 */
template <
    typename T, bool isReuseSource = false, bool isBasicBlock = false, bool isDataFormatNZ = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SimpleSoftMax(
    const LocalTensor<half>& dstTensor, const LocalTensor<float>& inSumTensor, const LocalTensor<float>& inMaxTensor,
    const LocalTensor<half>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling,
    const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    SimpleSoftMaxImpl<half, float, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
        dstTensor, inSumTensor, inMaxTensor, srcTensor, sharedTmpBuffer, tiling, softmaxShapeInfo);
#endif
}
} // namespace AscendC

#pragma end_pipe
#endif // LIB_SOFTMAX_SIMPLESOFTMAX_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIMPLESOFTMAX_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SIMPLESOFTMAX_H__
#endif
