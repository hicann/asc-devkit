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
 * \file mean_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/reduce/mean/mean_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/reduce/mean.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_MEAN_MEAN_C310_IMPL_H__
#endif

#ifndef IMPL_REDUCE_MEAN_C310_IMPL_H
#define IMPL_REDUCE_MEAN_C310_IMPL_H

#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/adv_api/reduce/mean_utils.h"
#include "../../common/common.h"

namespace AscendC {
namespace Internal {

template <typename T, typename accType>
struct GetConvType {
    using type = T;
};

template <>
struct GetConvType<half, float> {
    using type = float;
};

template <typename T, typename ConvType>
__simd_callee__ inline void LoadSrcData(
    Reg::RegTensor<ConvType>& srcReg, __ubuf__ T* src, uint16_t index, uint32_t offset, Reg::MaskReg& mask)
{
    Reg::RegTensor<T> srcTmpReg;
    if constexpr (std::is_same<T, half>::value && std::is_same<ConvType, float>::value) {
        Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(srcTmpReg, src + index * offset);
        Reg::Cast<float, T, castTraitB16ToB32>(srcReg, srcTmpReg, mask);
    } else {
        Reg::LoadAlign(srcReg, src + index * offset);
    }
}

template <typename T, typename U, typename ConvType>
__simd_vf__ inline void MeanForOneRepeatTime(
    __ubuf__ T* dstUb, __ubuf__ U* srcUb, const MeanParams meanParams, uint32_t calCount, uint32_t offset)
{
    uint32_t count;
    ConvType scalarValue = static_cast<ConvType>(1.0f / meanParams.n);
    Reg::MaskReg mask;
    Reg::UnalignReg uregOut;
    Reg::RegTensor<ConvType> srcTmpReg, dstTmpReg;
    Reg::RegTensor<T> dstReg;

    for (int i = 0; i < meanParams.outter; i++) {
        count = calCount;
        mask = Reg::UpdateMask<ConvType>(count);
        LoadSrcData(srcTmpReg, srcUb, i, offset, mask);
        Reg::ReduceSum(dstTmpReg, srcTmpReg, mask);
        Reg::Muls(dstTmpReg, dstTmpReg, scalarValue, mask);
        if constexpr (sizeof(T) == sizeof(half) && sizeof(ConvType) == sizeof(float)) {
            Reg::Cast<T, float, castTraitB32ToB16>(dstReg, dstTmpReg, mask);
            Reg::Pack<uint16_t, uint32_t, Reg::HighLowPart::LOWEST>(
                (Reg::RegTensor<uint16_t>&)dstReg, (Reg::RegTensor<uint32_t>&)dstReg);
        } else {
            dstReg = dstTmpReg;
        }
        Reg::StoreUnAlign<T, Reg::PostLiteral::POST_MODE_UPDATE>(dstUb, dstReg, uregOut, 1);
    }
    Reg::StoreUnAlignPost(dstUb, uregOut, 0);
}

template <typename T, typename ConvType, bool isFirstRepeat>
__simd_vf__ inline void ReduceSumNextN(
    __ubuf__ ConvType* dstUb, __ubuf__ T* srcUb, const MeanParams meanParams, uint32_t calCount, uint32_t repeatTimes,
    uint32_t offset)
{
    uint32_t count;
    Reg::MaskReg mask;
    Reg::UnalignReg uregIn;
    Reg::RegTensor<ConvType> srcReg, dstReg;
    constexpr int32_t eleCountPerVL = GetVecLen() / sizeof(ConvType);

    for (uint16_t i = 0; i < meanParams.outter; i++) {
        count = calCount;
        auto dstTmpUb = dstUb + i * offset;
        for (uint16_t j = 0; j < repeatTimes; j++) {
            mask = Reg::UpdateMask<ConvType>(count);
            if constexpr (isFirstRepeat) {
                LoadSrcData(srcReg, srcUb + i * meanParams.inner, j, eleCountPerVL, mask);
            } else {
                LoadSrcData(srcReg, srcUb + i * offset, j, eleCountPerVL, mask);
            }
            Reg::ReduceSum(dstReg, srcReg, mask);
            Reg::StoreUnAlign<ConvType, Reg::PostLiteral::POST_MODE_UPDATE>(dstTmpUb, dstReg, uregIn, 1);
        }
        Reg::StoreUnAlignPost(dstTmpUb, uregIn, 0);
    }
    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
}
} // namespace Internal

template <typename T, typename accType, bool isReuseSource, bool isBasicBlock, int32_t reduceDim>
__aicore__ inline void MeanCheckParams(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const MeanParams& meanParams)
{
    static_assert(SupportType<T, half, float>(), "current data type is not supported on current device!");
    CheckTensorPos<T>(dstTensor, Hardware::UB, "dstTensor", "VECIN/VECCALC/VECOUT", "Mean");
    CheckTensorPos<T>(srcTensor, Hardware::UB, "srcTensor", "VECIN/VECCALC/VECOUT", "Mean");
    CheckTensorPos<uint8_t>(sharedTmpBuffer, Hardware::UB, "sharedTmpBuffer", "VECIN/VECCALC/VECOUT", "Mean");
    constexpr uint32_t meanInnerAlignLen = 32;
    ASCENDC_ASSERT((1 <= meanParams.n) && (meanParams.n <= meanParams.inner), {
        KERNEL_LOG(KERNEL_ERROR, "The value of n must be greater than or equal to 1 and less than or equal to inner.");
    });
    ASCENDC_ASSERT((meanParams.inner * sizeof(T) % meanInnerAlignLen == 0), {
        KERNEL_LOG(KERNEL_ERROR, "The value of inner * sizeof(T) must be an integer multiple of 32.");
    });
}

template <
    typename T, typename accType = T, bool isReuseSource = false, bool isBasicBlock = false, int32_t reduceDim = -1>
__aicore__ inline void MeanImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const MeanParams& meanParams)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }

    MeanCheckParams<T, accType, isReuseSource, isBasicBlock, reduceDim>(
        dstTensor, srcTensor, sharedTmpBuffer, meanParams);

    using ConvType = typename Internal::GetConvType<T, accType>::type;
    __ubuf__ T* dstUb = (__ubuf__ T*)dstTensor.GetPhyAddr();
    __ubuf__ T* srcUb = (__ubuf__ T*)srcTensor.GetPhyAddr();
    __ubuf__ ConvType* sharedTmpBufferUb = (__ubuf__ ConvType*)sharedTmpBuffer.GetPhyAddr();

    constexpr int32_t eleCountPerVL = GetVecLen() / sizeof(ConvType);
    uint16_t repeatTimes = CeilDivision(meanParams.n, eleCountPerVL);
    uint32_t loopRepeatTimes;
    uint32_t calCount = meanParams.n;

    uint32_t totalCnt = 1;
    uint32_t dataSize = repeatTimes;
    uint32_t offset = AlignUp(CeilDivision(meanParams.inner, eleCountPerVL), 32);
    while (dataSize > 1) {
        ++totalCnt;
        dataSize = CeilDivision(dataSize, eleCountPerVL);
    }

    if (repeatTimes == 1) {
        Internal::MeanForOneRepeatTime<T, T, ConvType>(dstUb, srcUb, meanParams, meanParams.n, meanParams.inner);
        return;
    }

    Internal::ReduceSumNextN<T, ConvType, true>(sharedTmpBufferUb, srcUb, meanParams, calCount, repeatTimes, offset);

    --totalCnt;
    loopRepeatTimes = repeatTimes;
    while (totalCnt != 0) {
        calCount = loopRepeatTimes;
        loopRepeatTimes = CeilDivision(loopRepeatTimes, eleCountPerVL);
        if (totalCnt == 1) {
            Internal::MeanForOneRepeatTime<T, ConvType, ConvType>(
                dstUb, sharedTmpBufferUb, meanParams, calCount, offset);
        } else {
            Internal::ReduceSumNextN<ConvType, ConvType, false>(
                sharedTmpBufferUb, sharedTmpBufferUb, meanParams, calCount, loopRepeatTimes, offset);
        }
        --totalCnt;
    }
}
} // namespace AscendC

#endif // IMPL_REDUCE_MEAN_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_MEAN_MEAN_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_REDUCE_MEAN_MEAN_C310_IMPL_H__
#endif
