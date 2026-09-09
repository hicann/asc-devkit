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
 * \file softmaxflash.h
 * \brief SoftmaxFlash api of AscendC, this api will be deprecated, please use SoftmaxFlashV2 api instead
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "softmaxflash.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAXFLASH_H__
#endif

#ifndef LIB_SOFTMAX_SOFTMAXFLASH_H
#define LIB_SOFTMAX_SOFTMAXFLASH_H

#include "kernel_tensor.h"
#include "kernel_tiling/kernel_tiling.h"
#include "include/adv_api/activation/softmax_utils.h"
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5102)
#include "../../../impl/adv_api/detail/activation/softmax/softmax_common.h"
#include "../../../impl/adv_api/detail/activation/softmax/softmax_flash_base_impl.h"
#endif

#pragma begin_pipe(V)
namespace AscendC {
/*!
 * \ingroup SoftmaxFlash
 * \brief softmax calculation in FlashAttention process: you can refet to the paper:
 *        <Flashattention: Fast and memory-efficient exact attention with io-awareness>
 *        if isUpdate = false:
 *            max = rowmax(x),  y = exp(x-max), sum = rowsum(y), y = y / sum
 *        if isUpdate = true:
 *            max = max(rowmax(x), inmax), y = exp(x-max), sum = rowsum(y)
 *            expmax = exp(inmax - max), sum = expmax * insum + sum
 *            expmax = expmax * insum / sum, y = y / sum
 * \note support data type: half and float
 * \param [out] dstTensor: output y
 * \param [out] sumTensor: output sum
 * \param [out] maxTensor: output max
 * \param [out] expMaxTensor: output expmax, only valid when isUpdate is true
 * \param [in] srcTensor: input x
 * \param [in] inSumTensor: input last insum
 * \param [in] inMaxTensor: input last inmax
 * \param [in] softmaxShapeInfo: input src shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isUpdate: whether to update the sum and expmax
 * \param [in] isReuseSource: reserved param
 * \param [in] isBasicBlock: if src shape[m,k] satisfy the condition(m%8 == 0 && k%64 == 0),
 *                           you can set true to improve performance
 */
template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
[[deprecated(__FILE__ " is deprecated, please use softmaxflashv2.h instead!")]] __aicore__ inline void SoftmaxFlash(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& sumTensor, const LocalTensor<T>& maxTensor,
    const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor, const LocalTensor<T>& inSumTensor,
    const LocalTensor<T>& inMaxTensor, const SoftMaxTiling& tiling, bool isUpdate = false,
    const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5102)
    TRACE_START(TraceId::SoftmaxFlash);
    SoftmaxFlashImpl<T, isReuseSource, isBasicBlock>(
        dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, tiling, isUpdate,
        softmaxShapeInfo);
    TRACE_STOP(TraceId::SoftmaxFlash);
#endif
}
/* !
 * \ingroup SoftmaxFlash
 * \brief softmax calculation in FlashAttention process: you can refet to the paper:
 *        <Flashattention: Fast and memory-efficient exact attention with io-awareness>
 *        if isUpdate = false:
 *            max = rowmax(x),  y = exp(x-max), sum = rowsum(y), y = y / sum
 *        if isUpdate = true:
 *            max = max(rowmax(x), inmax), y = exp(x-max), sum = rowsum(y)
 *            expmax = exp(inmax - max), sum = expmax * insum + sum
 *            expmax = expmax * insum / sum, y = y / sum
 * \param [out] dstTensor: output y with dtype of half
 * \param [out] sumTensor: output sum with dtype of float
 * \param [out] maxTensor: output max with dtype of float
 * \param [out] expMaxTensor: output expmax with dtype of half, only valid when isUpdate is true
 * \param [in] srcTensor: input x with dtype of half
 * \param [in] inSumTensor: input last insum with dtype of float
 * \param [in] inMaxTensor: input last inmax with dtype of float
 * \param [in] softmaxShapeInfo: input src shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isUpdate: whether to update the sum and expmax
 * \param [in] isReuseSource: reserved param
 * \param [in] isBasicBlock: if src shape[m,k] satisfy the condition(m%8 == 0 && k%64 == 0),
 *                           you can set true to improve performance
 */
template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
[[deprecated(__FILE__ " is deprecated, please use softmaxflashv2.h instead!")]]
__aicore__ inline void SoftmaxFlash(
    const LocalTensor<half>& dstTensor, const LocalTensor<float>& sumTensor, const LocalTensor<float>& maxTensor,
    const LocalTensor<half>& srcTensor, const LocalTensor<half>& expMaxTensor, const LocalTensor<float>& inSumTensor,
    const LocalTensor<float>& inMaxTensor, const SoftMaxTiling& tiling, bool isUpdate = false,
    const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5102)
    TRACE_START(TraceId::SoftmaxFlash);
    SoftmaxFlashImpl<T, isReuseSource, isBasicBlock>(
        dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, tiling, isUpdate,
        softmaxShapeInfo);
    TRACE_STOP(TraceId::SoftmaxFlash);
#endif
}
/*!
 * \ingroup SoftmaxFlash
 * \brief softmax calculation in FlashAttention process: you can refet to the paper:
 *        <Flashattention: Fast and memory-efficient exact attention with io-awareness>
 *        if isUpdate = false:
 *            max = rowmax(x),  y = exp(x-max), sum = rowsum(y), y = y / sum
 *        if isUpdate = true:
 *            max = max(rowmax(x), inmax), y = exp(x-max), sum = rowsum(y)
 *            expmax = exp(inmax - max), sum = expmax * insum + sum
 *            expmax = expmax * insum / sum, y = y / sum
 * \note support data type: half and float
 * \param [out] dstTensor: output y
 * \param [out] sumTensor: output sum
 * \param [out] maxTensor: output max
 * \param [out] expMaxTensor: output expmax, only valid when isUpdate is true
 * \param [in] srcTensor: input x
 * \param [in] inSumTensor: input last insum
 * \param [in] inMaxTensor: input last inmax
 * \param [in] sharedTmpBuffer: input local temporary Tensor, you can get the range by tilingfunc of
 *                              GetSoftMaxFlashMinTmpSize/GetSoftMaxFlashMaxTmpSize
 * \param [in] softmaxShapeInfo: input src shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isUpdate: whether to update the sum and expmax
 * \param [in] isReuseSource: reserved param
 * \param [in] isBasicBlock: if src shape[m,k] satisfy the condition(m%8 == 0 && k%64 == 0),
 *                           you can set true to improve performance
 */
template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
[[deprecated(__FILE__ " is deprecated, please use softmaxflashv2.h instead!")]]
__aicore__ inline void SoftmaxFlash(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& sumTensor, const LocalTensor<T>& maxTensor,
    const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor, const LocalTensor<T>& inSumTensor,
    const LocalTensor<T>& inMaxTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling,
    bool isUpdate = false, const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5102)
    TRACE_START(TraceId::SoftmaxFlash);
    SoftmaxFlashImpl<T, isReuseSource, isBasicBlock>(
        dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, sharedTmpBuffer, tiling,
        isUpdate, softmaxShapeInfo);
    TRACE_STOP(TraceId::SoftmaxFlash);
#endif
}
/*!
 * \ingroup SoftmaxFlash
 * \brief softmax calculation in FlashAttention process: you can refet to the paper:
 *        <Flashattention: Fast and memory-efficient exact attention with io-awareness>
 *        if isUpdate = false:
 *            max = rowmax(x),  y = exp(x-max), sum = rowsum(y), y = y / sum
 *        if isUpdate = true:
 *            max = max(rowmax(x), inmax), y = exp(x-max), sum = rowsum(y)
 *            expmax = exp(inmax - max), sum = expmax * insum + sum
 *            expmax = expmax * insum / sum, y = y / sum
 * \param [out] dstTensor: output y with dtype of half
 * \param [out] sumTensor: output sum with dtype of float
 * \param [out] maxTensor: output max with dtype of float
 * \param [out] expMaxTensor: output expmax with dtype of half, only valid when isUpdate is true
 * \param [in] srcTensor: input x with dtype of half
 * \param [in] inSumTensor: input last insum with dtype of float
 * \param [in] inMaxTensor: input last inmax with dtype of float
 * \param [in] sharedTmpBuffer: input local temporary Tensor, you can get the range by tilingfunc of
 *                              GetSoftMaxFlashMinTmpSize/GetSoftMaxFlashMaxTmpSize
 * \param [in] softmaxShapeInfo: input src shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isUpdate: whether to update the sum and expmax
 * \param [in] isReuseSource: reserved param
 * \param [in] isBasicBlock: if src shape[m,k] satisfy the condition(m%8 == 0 && k%64 == 0),
 *                           you can set true to improve performance
 */
template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
[[deprecated(__FILE__ " is deprecated, please use softmaxflashv2.h instead!")]]
__aicore__ inline void SoftmaxFlash(
    const LocalTensor<half>& dstTensor, const LocalTensor<float>& sumTensor, const LocalTensor<float>& maxTensor,
    const LocalTensor<half>& srcTensor, const LocalTensor<half>& expMaxTensor, const LocalTensor<float>& inSumTensor,
    const LocalTensor<float>& inMaxTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling,
    bool isUpdate = false, const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5102)
    TRACE_START(TraceId::SoftmaxFlash);
    SoftmaxFlashImpl<T, isReuseSource, isBasicBlock>(
        dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, sharedTmpBuffer, tiling,
        isUpdate, softmaxShapeInfo);
    TRACE_STOP(TraceId::SoftmaxFlash);
#endif
}
} // namespace AscendC
#pragma end_pipe
#endif // LIB_SOFTMAX_SOFTMAXFLASHV2_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAXFLASH_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAXFLASH_H__
#endif
