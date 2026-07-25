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
 * \file sum_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/reduce/sum/sum_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/reduce/sum.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_SUM_SUM_C310_IMPL_H__
#endif

#ifndef IMPL_REDUCE_SUM_C310_IMPL_H
#define IMPL_REDUCE_SUM_C310_IMPL_H

#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/adv_api/reduce/sum_utils.h"

namespace AscendC {
namespace SumInternal {
template <typename T>
__simd_vf__ inline void SumForOneRepeatTime(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, const SumParams sumParams, uint32_t count, uint32_t offset)
{
    uint32_t calCount;
    Reg::MaskReg mask;
    Reg::UnalignReg uregOut;
    Reg::RegTensor<T> srcReg, dstReg;

    for (int i = 0; i < sumParams.outter; i++) {
        calCount = count;
        mask = Reg::UpdateMask<T>(calCount);
        Reg::LoadAlign(srcReg, srcUb + i * offset);
        Reg::ReduceSum(dstReg, srcReg, mask);
        Reg::StoreUnAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(dstUb, dstReg, uregOut, 1);
    }
    Reg::StoreUnAlignPost(dstUb, uregOut, 0);
}

template <typename T, bool isFirstRepeat>
__simd_vf__ inline void ReduceSumNextN(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, const SumParams sumParams, uint32_t calCount, uint32_t repeatTimes,
    uint32_t offset)
{
    uint32_t count;
    Reg::MaskReg mask;
    Reg::UnalignReg uregIn;
    Reg::RegTensor<T> srcReg, dstReg;
    constexpr int32_t eleCountPerVL = GetVecLen() / sizeof(T);
    for (uint16_t i = 0; i < sumParams.outter; i++) {
        count = calCount;
        auto dstTmpUb = dstUb + i * offset;
        for (uint16_t j = 0; j < repeatTimes; j++) {
            mask = Reg::UpdateMask<T>(count);
            if constexpr (isFirstRepeat) {
                Reg::LoadAlign(srcReg, srcUb + i * sumParams.inner + j * eleCountPerVL);
            } else {
                Reg::LoadAlign(srcReg, srcUb + i * offset + j * eleCountPerVL);
            }
            Reg::ReduceSum(dstReg, srcReg, mask);
            Reg::StoreUnAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(dstTmpUb, dstReg, uregIn, 1);
        }
        Reg::StoreUnAlignPost(dstTmpUb, uregIn, 0);
    }
    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
}
} // namespace SumInternal

template <typename T, int32_t reduceDim = -1, bool isReuseSource = false, bool isBasicBlock = false>
__aicore__ inline void SumCheckParams(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const SumParams& sumParams)
{
    static_assert(SupportType<T, half, float>(), "current data type is not supported on current device!");
    CheckTensorPos<T>(dstTensor, Hardware::UB, "dstTensor", "VECIN/VECCALC/VECOUT", "Sum");
    CheckTensorPos<T>(srcTensor, Hardware::UB, "srcTensor", "VECIN/VECCALC/VECOUT", "Sum");
    CheckTensorPos<uint8_t>(sharedTmpBuffer, Hardware::UB, "sharedTmpBuffer", "VECIN/VECCALC/VECOUT", "Sum");
    constexpr uint32_t sumInnerAlignLen = 32;
    ASCENDC_ASSERT((1 <= sumParams.n) && (sumParams.n <= sumParams.inner), {
        KERNEL_LOG(KERNEL_ERROR, "The value of n must be greater than or equal to 1 and less than or equal to inner.");
    });
    ASCENDC_ASSERT((sumParams.inner * sizeof(T) % sumInnerAlignLen == 0), {
        KERNEL_LOG(KERNEL_ERROR, "The value of inner * sizeof(T) must be an integer multiple of 32.");
    });
}

template <typename T, int32_t reduceDim = -1, bool isReuseSource = false, bool isBasicBlock = false>
__aicore__ inline void SumCompute(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const SumParams& sumParams)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }

    SumCheckParams<T, reduceDim, isReuseSource, isBasicBlock>(dstTensor, srcTensor, sharedTmpBuffer, sumParams);
    __ubuf__ T* dstUb = (__ubuf__ T*)dstTensor.GetPhyAddr();
    __ubuf__ T* srcUb = (__ubuf__ T*)srcTensor.GetPhyAddr();
    __ubuf__ T* sharedTmpBufferUb = (__ubuf__ T*)sharedTmpBuffer.GetPhyAddr();

    constexpr int32_t eleCountPerVL = GetVecLen() / sizeof(T);
    uint16_t repeatTimes = CeilDivision(sumParams.n, eleCountPerVL);
    uint32_t loopRepeatTimes;
    uint32_t calCount = sumParams.n;

    uint32_t totalCnt = 1;
    uint32_t dataSize = repeatTimes;
    uint32_t offset = AlignUp(CeilDivision(sumParams.inner, eleCountPerVL), 32);
    while (dataSize > 1) {
        ++totalCnt;
        dataSize = CeilDivision(dataSize, eleCountPerVL);
    }

    if (repeatTimes == 1) {
        SumInternal::SumForOneRepeatTime<T>(dstUb, srcUb, sumParams, sumParams.n, sumParams.inner);
        return;
    }

    SumInternal::ReduceSumNextN<T, true>(sharedTmpBufferUb, srcUb, sumParams, calCount, repeatTimes, offset);

    --totalCnt;
    loopRepeatTimes = repeatTimes;
    while (totalCnt != 0) {
        calCount = loopRepeatTimes;
        loopRepeatTimes = CeilDivision(loopRepeatTimes, eleCountPerVL);
        if (totalCnt == 1) {
            SumInternal::SumForOneRepeatTime<T>(dstUb, sharedTmpBufferUb, sumParams, calCount, offset);
        } else {
            SumInternal::ReduceSumNextN<T, false>(
                sharedTmpBufferUb, sharedTmpBufferUb, sumParams, calCount, loopRepeatTimes, offset);
        }
        --totalCnt;
    }
}
} // namespace AscendC

#endif // IMPL_REDUCE_SUM_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_SUM_SUM_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_SUM_SUM_C310_IMPL_H__
#endif
