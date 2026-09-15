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
 * \file sum_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/reduce/sum/sum_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/reduce/sum.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_SUM_SUM_COMMON_IMPL_H__
#endif

#ifndef IMPL_REDUCE_SUM_SUM_COMMON_IMPL_H
#define IMPL_REDUCE_SUM_SUM_COMMON_IMPL_H

#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/adv_api/reduce/sum_utils.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/reduce/sum/sum_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"
namespace AscendC {

template <typename T>
__aicore__ inline void SumForOneRepeatTime(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const SumParams& sumParams)
{
    SetVectorMask<T>(0, sumParams.n);
    for (uint32_t row = 0; row < sumParams.outter; ++row) {
        RepeatReduceSum<T, false>(
            dstTensor[row], srcTensor[row * sumParams.inner], 1, MASK_PLACEHOLDER, DEFAULT_BLK_STRIDE,
            DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    }
    SetMaskNorm();
    ResetMask();
}

template <typename T, int32_t reduceDim = -1, bool isReuseSource = false, bool isBasicBlock = false>
__aicore__ inline void SumCompute(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const SumParams& sumParams)
{
    if ASCEND_IS_AIC {
        return;
    }
    CHECK_FUNC_HIGHLEVEL_API(
        Sum, (T, reduceDim, isReuseSource, isBasicBlock), (dstTensor, srcTensor, sharedTmpBuffer, sumParams));
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002)
    uint32_t elementNumPerRep = ONE_REPEAT_BYTE_SIZE / sizeof(T);
    uint32_t elementNumPerBlk = ONE_BLK_SIZE / sizeof(T);
    uint32_t firstRepeatTimes = (sumParams.n + elementNumPerRep - 1) / elementNumPerRep;
    SetMaskCount();
    if (firstRepeatTimes == 1) {
        return SumForOneRepeatTime(dstTensor, srcTensor, sumParams);
    }
    uint32_t totalCnt = 1;
    uint32_t dataSize = firstRepeatTimes;
    while (dataSize > 1) {
        ++totalCnt;
        dataSize = (dataSize + elementNumPerRep - 1) / elementNumPerRep;
    }
    LocalTensor<T> tmpTensor = sharedTmpBuffer.ReinterpretCast<T>();
    for (uint32_t row = 0; row < sumParams.outter; ++row) {
        uint32_t cnt = totalCnt;
        uint64_t lowMask = sumParams.n;
        SetVectorMask<T>(0, lowMask);
        RepeatReduceSum<T, false>(
            tmpTensor, srcTensor[row * sumParams.inner], 1, MASK_PLACEHOLDER, DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE,
            DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);

        PipeBarrier<PIPE_V>();
        lowMask = (lowMask + elementNumPerRep - 1) / elementNumPerRep;
        --cnt;
        while (cnt != 0) {
            SetVectorMask<T>(0, lowMask);
            if (cnt == 1) {
                RepeatReduceSum<T, false>(
                    dstTensor[row], tmpTensor, 1, MASK_PLACEHOLDER, DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE,
                    DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
            } else {
                RepeatReduceSum<T, false>(
                    tmpTensor, tmpTensor, 1, MASK_PLACEHOLDER, DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE,
                    DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
            }
            PipeBarrier<PIPE_V>();
            lowMask = (lowMask + elementNumPerRep - 1) / elementNumPerRep;
            --cnt;
        }
    }
    SetMaskNorm();
#endif
}

} // namespace AscendC
#endif // IMPL_REDUCE_SUM_SUM_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_SUM_SUM_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_SUM_SUM_COMMON_IMPL_H__
#endif
