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
 * \file softmaxflashv2.h
 * \brief SoftmaxFlashV2 api of AscendC
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "softmaxflashv2.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAXFLASHV2_H__
#endif
#ifndef LIB_SOFTMAX_SOFTMAXFLASHV2_H
#define LIB_SOFTMAX_SOFTMAXFLASHV2_H

#include "kernel_tensor.h"
#include "kernel_tiling/kernel_tiling.h"
#include "include/adv_api/activation/softmax_utils.h"
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../impl/adv_api/detail/activation/softmax/softmax_common.h"
#include "../../../impl/adv_api/detail/activation/softmax/softmax_flashv2_base_impl.h"
#endif

#pragma begin_pipe(V)
namespace AscendC {

/*!
 * \ingroup SoftmaxFlashV2
 * \brief compute tiling
 * \param [out] softmaxTiling: the new tiling for SoftmaxFlashV2 api
 * \param [in] shapeInfo: input src shape
 * \param [in] dataTypeSize1: typesize of input src tensor data
 * \param [in] dataTypeSize2: typesize of input max&&sum tensor data
 * \param [in] localWorkSpaceSize: input local temporary Tensor size, byte unit
 * \param [in] isUpdate: whether to update the sum and expmax, keep the same with SoftmaxFlashV2 api
 * \param [in] isBasicBlock: keep the same with SoftmaxFlashV2 api
 * \param [in] isDataFormatNZ: if the data format of input srcTensor is NZ, keep the same with SoftmaxFlashV2 api
 */
__aicore__ inline constexpr SoftMaxTiling SoftMaxFlashV2TilingFunc(
    const SoftMaxShapeInfo& shapeInfo, const uint32_t dataTypeSize1, const uint32_t dataTypeSize2,
    const uint32_t localWorkSpaceSize, const bool isUpdate = false, const bool isBasicBlock = false,
    const bool isDataFormatNZ = false, const bool isFlashOutputBrc = false)
{
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    return SoftMaxFlashV2TilingFuncImpl(
        shapeInfo.srcM, shapeInfo.srcK, dataTypeSize1, dataTypeSize2, localWorkSpaceSize / B32_BYTE_SIZE, isUpdate,
        isBasicBlock, isDataFormatNZ, isFlashOutputBrc);
#else
    return {};
#endif
}

/*!
 * \ingroup SoftmaxFlashV2
 * \brief softmax calculation in FlashAttention-2 process: you can refet to the paper:
 *        <FlashAttention-2: Faster Attention with Better Parallelism and Work Partitioning>
 *        if isUpdate = false:
 *            max = rowmax(x),  y = exp(x-max), sum = rowsum(y)
 *        if isUpdate = true:
 *            max = max(rowmax(x), inmax), y = exp(x-max), sum = rowsum(y)
 *            expmax = exp(inmax - max), sum = expmax * insum + sum
 * \note support data type: half and float
 * \param [out] dstTensor: output y
 * \param [out] expSumTensor: output sum
 * \param [out] maxTensor: output max
 * \param [out] expMaxTensor: output expmax, only valid when isUpdate is true
 * \param [in] srcTensor: input x
 * \param [in] inExpSumTensor: input last insum
 * \param [in] inMaxTensor: input last inmax
 * \param [in] softmaxShapeInfo: input src shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isUpdate: whether to update the sum and expmax
 * \param [in] isReuseSource: reserved param
 * \param [in] isBasicBlock: if src shape[m,k] satisfy the condition(m%8 == 0 && k%64 == 0), you can set true to improve
 *                           performance, but it is a reserved param when isDataFormatNZ = true
 * \param [in] isDataFormatNZ: if the data format of input srcTensor is NZ
 */
template <
    typename T, bool isUpdate = false, bool isReuseSource = false, bool isBasicBlock = false,
    bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftmaxFlashV2(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& expSumTensor, const LocalTensor<T>& maxTensor,
    const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor, const LocalTensor<T>& inExpSumTensor,
    const LocalTensor<T>& inMaxTensor, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    TRACE_START(TraceId::SoftmaxFlashV2);
    SoftmaxFlashV2Impl<T, T, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
        dstTensor, expSumTensor, maxTensor, srcTensor, expMaxTensor, inExpSumTensor, inMaxTensor, tiling,
        softmaxShapeInfo);
    TRACE_STOP(TraceId::SoftmaxFlashV2);
#endif
}

/*!
 * \ingroup SoftmaxFlashV2
 * \brief softmax calculation in FlashAttention-2 process: you can refet to the paper:
 *        <FlashAttention-2: Faster Attention with Better Parallelism and Work Partitioning>
 *        if isUpdate = false:
 *            max = rowmax(x),  y = exp(x-max), sum = rowsum(y)
 *        if isUpdate = true:
 *            max = max(rowmax(x), inmax), y = exp(x-max), sum = rowsum(y)
 *            expmax = exp(inmax - max), sum = expmax * insum + sum
 * \param [out] dstTensor: output y with dtype of half
 * \param [out] expSumTensor: output sum with dtype of float
 * \param [out] maxTensor: output max with dtype of float
 * \param [out] expMaxTensor: output expmax with dtype of half, only valid when isUpdate is true
 * \param [in] srcTensor: input x with dtype of half
 * \param [in] inExpSumTensor: input last insum with dtype of float
 * \param [in] inMaxTensor: input last inmax with dtype of float
 * \param [in] softmaxShapeInfo: input src shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isUpdate: whether to update the sum and expmax
 * \param [in] isReuseSource: reserved param
 * \param [in] isBasicBlock: if src shape[m,k] satisfy the condition(m%8 == 0 && k%64 == 0), you can set true to improve
 *                           performance, but it is a reserved param when isDataFormatNZ = true
 * \param [in] isDataFormatNZ: if the data format of input srcTensor is NZ
 */
template <
    typename T, bool isUpdate = false, bool isReuseSource = false, bool isBasicBlock = false,
    bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftmaxFlashV2(
    const LocalTensor<half>& dstTensor, const LocalTensor<float>& expSumTensor, const LocalTensor<float>& maxTensor,
    const LocalTensor<half>& srcTensor, const LocalTensor<half>& expMaxTensor, const LocalTensor<float>& inExpSumTensor,
    const LocalTensor<float>& inMaxTensor, const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    TRACE_START(TraceId::SoftmaxFlashV2);
    SoftmaxFlashV2Impl<half, float, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
        dstTensor, expSumTensor, maxTensor, srcTensor, expMaxTensor, inExpSumTensor, inMaxTensor, tiling,
        softmaxShapeInfo);
    TRACE_STOP(TraceId::SoftmaxFlashV2);
#endif
}

/*!
 * \ingroup SoftmaxFlashV2
 * \brief softmax calculation in FlashAttention-2 process: you can refet to the paper:
 *        <FlashAttention-2: Faster Attention with Better Parallelism and Work Partitioning>
 *        if isUpdate = false:
 *            max = rowmax(x),  y = exp(x-max), sum = rowsum(y)
 *        if isUpdate = true:
 *            max = max(rowmax(x), inmax), y = exp(x-max), sum = rowsum(y)
 *            expmax = exp(inmax - max), sum = expmax * insum + sum
 * \note support data type: half and float
 * \param [out] dstTensor: output y
 * \param [out] expSumTensor: output sum
 * \param [out] maxTensor: output max
 * \param [out] expMaxTensor: output expmax, only valid when isUpdate is true
 * \param [in] srcTensor: input x
 * \param [in] inExpSumTensor: input last insum
 * \param [in] inMaxTensor: input last inmax
 * \param [in] sharedTmpBuffer: input local temporary Tensor, you can get the range by tilingfunc of
 *                              GetSoftMaxFlashV2MinTmpSize/GetSoftMaxFlashV2MaxTmpSize
 * \param [in] softmaxShapeInfo: input src shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isUpdate: whether to update the sum and expmax
 * \param [in] isReuseSource: reserved param
 * \param [in] isBasicBlock: if src shape[m,k] satisfy the condition(m%8 == 0 && k%64 == 0), you can set true to improve
 *                           performance, but it is a reserved param when isDataFormatNZ = true
 * \param [in] isDataFormatNZ: if the data format of input srcTensor is NZ
 */
template <
    typename T, bool isUpdate = false, bool isReuseSource = false, bool isBasicBlock = false,
    bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftmaxFlashV2(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& expSumTensor, const LocalTensor<T>& maxTensor,
    const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor, const LocalTensor<T>& inExpSumTensor,
    const LocalTensor<T>& inMaxTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling,
    const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    TRACE_START(TraceId::SoftmaxFlashV2);
    SoftmaxFlashV2Impl<T, T, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
        dstTensor, expSumTensor, maxTensor, srcTensor, expMaxTensor, inExpSumTensor, inMaxTensor, sharedTmpBuffer,
        tiling, softmaxShapeInfo);
    TRACE_STOP(TraceId::SoftmaxFlashV2);
#endif
}

/*!
 * \ingroup SoftmaxFlashV2
 * \brief softmax calculation in FlashAttention-2 process: you can refet to the paper:
 *        <FlashAttention-2: Faster Attention with Better Parallelism and Work Partitioning>
 *        if isUpdate = false:
 *            max = rowmax(x),  y = exp(x-max), sum = rowsum(y)
 *        if isUpdate = true:
 *            max = max(rowmax(x), inmax), y = exp(x-max), sum = rowsum(y)
 *            expmax = exp(inmax - max), sum = expmax * insum + sum
 * \param [out] dstTensor: output y with dtype of half
 * \param [out] expSumTensor: output sum with dtype of float
 * \param [out] maxTensor: output max with dtype of float
 * \param [out] expMaxTensor: output expmax with dtype of half, only valid when isUpdate is true
 * \param [in] srcTensor: input x with dtype of half
 * \param [in] inExpSumTensor: input last insum with dtype of float
 * \param [in] inMaxTensor: input last inmax with dtype of float
 * \param [in] sharedTmpBuffer: input local temporary Tensor, you can get the range by tilingfunc of
 *                              GetSoftMaxFlashV2MinTmpSize/GetSoftMaxFlashV2MaxTmpSize
 * \param [in] softmaxShapeInfo: input src shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isUpdate: whether to update the sum and expmax
 * \param [in] isReuseSource: reserved param
 * \param [in] isBasicBlock: if src shape[m,k] satisfy the condition(m%8 == 0 && k%64 == 0), you can set true to improve
 *                           performance, but it is a reserved param when isDataFormatNZ = true
 * \param [in] isDataFormatNZ: if the data format of input srcTensor is NZ
 */
template <
    typename T, bool isUpdate = false, bool isReuseSource = false, bool isBasicBlock = false,
    bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftmaxFlashV2(
    const LocalTensor<half>& dstTensor, const LocalTensor<float>& expSumTensor, const LocalTensor<float>& maxTensor,
    const LocalTensor<half>& srcTensor, const LocalTensor<half>& expMaxTensor, const LocalTensor<float>& inExpSumTensor,
    const LocalTensor<float>& inMaxTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling,
    const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    TRACE_START(TraceId::SoftmaxFlashV2);
    SoftmaxFlashV2Impl<half, float, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
        dstTensor, expSumTensor, maxTensor, srcTensor, expMaxTensor, inExpSumTensor, inMaxTensor, sharedTmpBuffer,
        tiling, softmaxShapeInfo);
    TRACE_STOP(TraceId::SoftmaxFlashV2);
#endif
}

/*!
 * \ingroup SoftmaxFlashV2
 * \brief softmax calculation in FlashAttention-2 process: you can refet to the paper:
 *        <FlashAttention-2: Faster Attention with Better Parallelism and Work Partitioning>
 *        if isUpdate = false:
 *            max = rowmax(x),  y = exp(x-max), sum = rowsum(y)
 *        if isUpdate = true:
 *            max = max(rowmax(x), inmax), y = exp(x-max), sum = rowsum(y)
 *            expmax = exp(inmax - max), sum = expmax * insum + sum
 * \note support data type: half and float
 * \param [out] dstTensor: output y
 * \param [out] outReduceMax: output reduce max
 * \param [out] outExpSum: output sum
 * \param [out] outMax: output max
 * \param [out] outExpMax: output expmax, only valid when isUpdate is true
 * \param [in] srcTensor: input x
 * \param [in] inExpSum: input last insum
 * \param [in] inMax: input last inmax
 * \param [in] softmaxShapeInfo: input src shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isUpdate: whether to update the sum and expmax
 * \param [in] isReuseSource: reserved param
 * \param [in] isBasicBlock: if src shape[m,k] satisfy the condition(m%8 == 0 && k%64 == 0), you can set true to improve
 *                           performance, but it is a reserved param when isDataFormatNZ = true
 * \param [in] isDataFormatNZ: if the data format of input srcTensor is NZ
 */
template <
    typename T, bool isUpdate = false, bool isReuseSource = false, bool isBasicBlock = false,
    bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftmaxFlashV2(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& outReduceMax, const LocalTensor<T>& outExpSum,
    const LocalTensor<T>& outMax, const LocalTensor<T>& srcTensor, const LocalTensor<T>& outExpMax,
    const LocalTensor<T>& inExpSum, const LocalTensor<T>& inMax, const SoftMaxTiling& tiling,
    const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    TRACE_START(TraceId::SoftmaxFlashV2);
    SoftmaxFlashV2MaxImpl<T, T, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
        dstTensor, outReduceMax, outExpSum, outMax, srcTensor, outExpMax, inExpSum, inMax, tiling, softmaxShapeInfo);
    TRACE_STOP(TraceId::SoftmaxFlashV2);
#endif
}

/*!
 * \ingroup SoftmaxFlashV2
 * \brief softmax calculation in FlashAttention-2 process: you can refet to the paper:
 *        <FlashAttention-2: Faster Attention with Better Parallelism and Work Partitioning>
 *        if isUpdate = false:
 *            max = rowmax(x),  y = exp(x-max), sum = rowsum(y)
 *        if isUpdate = true:
 *            max = max(rowmax(x), inmax), y = exp(x-max), sum = rowsum(y)
 *            expmax = exp(inmax - max), sum = expmax * insum + sum
 * \param [out] dstTensor: output y with dtype of half
 * \param [out] outReduceMax: output reduce max
 * \param [out] outExpSum: output sum with dtype of float
 * \param [out] outMax: output max with dtype of float
 * \param [out] outExpMax: output expmax with dtype of half, only valid when isUpdate is true
 * \param [in] srcTensor: input x with dtype of half
 * \param [in] inExpSum: input last insum with dtype of float
 * \param [in] inMax: input last inmax with dtype of float
 * \param [in] sharedTmpBuffer: input local temporary Tensor, you can get the range by tilingfunc of
 *                              GetSoftMaxFlashV2MinTmpSize/GetSoftMaxFlashV2MaxTmpSize
 * \param [in] softmaxShapeInfo: input src shape
 * \param [in] tiling: input softmaxtiling
 * \param [in] isUpdate: whether to update the sum and expmax
 * \param [in] isReuseSource: reserved param
 * \param [in] isBasicBlock: if src shape[m,k] satisfy the condition(m%8 == 0 && k%64 == 0), you can set true to improve
 *                           performance, but it is a reserved param when isDataFormatNZ = true
 * \param [in] isDataFormatNZ: if the data format of input srcTensor is NZ
 */
template <
    typename T, bool isUpdate = false, bool isReuseSource = false, bool isBasicBlock = false,
    bool isDataFormatNZ = false, const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftmaxFlashV2(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& outReduceMax, const LocalTensor<T>& outExpSum,
    const LocalTensor<T>& outMax, const LocalTensor<T>& srcTensor, const LocalTensor<T>& outExpMax,
    const LocalTensor<T>& inExpSum, const LocalTensor<T>& inMax, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const SoftMaxTiling& tiling, const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    if ASCEND_IS_AIC {
        return;
    }
#if defined(__NPU_ARCH__) &&                                                                         \
    (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201 || \
     __NPU_ARCH__ == 3002 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    TRACE_START(TraceId::SoftmaxFlashV2);
    SoftmaxFlashV2MaxImpl<T, T, isUpdate, isReuseSource, isBasicBlock, isDataFormatNZ, config>(
        dstTensor, outReduceMax, outExpSum, outMax, srcTensor, outExpMax, inExpSum, inMax, sharedTmpBuffer, tiling,
        softmaxShapeInfo);
    TRACE_STOP(TraceId::SoftmaxFlashV2);
#endif
}

} // namespace AscendC
#pragma end_pipe
#endif // LIB_SOFTMAX_SOFTMAXFLASHV2_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAXFLASHV2_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAXFLASHV2_H__
#endif
