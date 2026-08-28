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
 * \file simple_softmax_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/activation/softmax/regbase/3510/softmax_flash_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmaxflash.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASH_IMPL_H__
#endif

#ifndef IMPL_ACTIVATION_SOFTMAX_C310_SOFTMAX_FLASH_IMPL_H
#define IMPL_ACTIVATION_SOFTMAX_C310_SOFTMAX_FLASH_IMPL_H

#include <type_traits>
#include "../../../../../../../include/basic_api/kernel_basic_intf.h"
#include "softmax_common_impl.h"
#include "softmax_impl.h"

namespace AscendC {
namespace Internal {
template <typename T1, typename T2>
__simd_vf__ inline void SoftmaxFlashNDImpl(
    __ubuf__ T1* dstUb, __ubuf__ T2* sumUb, __ubuf__ T2* maxUb, __ubuf__ T1* srcUb, __ubuf__ T1* expMaxUb,
    __ubuf__ T2* inSumUb, __ubuf__ T2* inMaxUb, __ubuf__ float* workUb, __ubuf__ float* tmpUb, const uint16_t srcM,
    const uint16_t repeatTimes, const uint32_t srcK)
{
    constexpr uint32_t stride = GetVecLen() / sizeof(float);
    constexpr uint32_t blockStride = GetDataBlockSizeInBytes() / sizeof(T2);
    NotNumUnion notNum;
    notNum.i = F32_NEG_INF;

    Reg::MaskReg maskFull = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::MaskReg maskExpMax = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL16>();
    Reg::MaskReg maskOneBlk;
    if constexpr (IsSameType<T2, half>::value) {
        maskOneBlk = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL16>();
    } else {
        maskOneBlk = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL8>();
    }
    Reg::RegTensor<float> srcVreg;
    Reg::RegTensor<float> maxVreg;
    Reg::RegTensor<float> expMaxVreg;
    Reg::RegTensor<float> sumVreg;
    Reg::RegTensor<float> tmpVreg;
    Reg::RegTensor<float> dstVreg;
    Reg::RegTensor<T1> t1Reg;

    for (uint16_t i = 0; i < srcM; ++i) {
        Reg::Duplicate(maxVreg, notNum.f);
        for (uint16_t j = 0; j < repeatTimes; ++j) {
            LoadIfNeedCast<T1>(srcVreg, srcUb + i * srcK + j * stride, maskFull);
            Reg::Max(maxVreg, maxVreg, srcVreg, maskFull);
        }
        Reg::ReduceMax(maxVreg, maxVreg, maskFull);
        Reg::Duplicate(maxVreg, maxVreg, maskOneBlk);
        LoadIfNeedCast<T2>(tmpVreg, inMaxUb + i * blockStride, maskOneBlk);
        Reg::Max(maxVreg, maxVreg, tmpVreg, maskOneBlk);
        StoreIfNeedCast<T2>(maxUb + i * blockStride, maxVreg, maskOneBlk);

        SoftmaxExpSub(expMaxVreg, tmpVreg, maxVreg, maskOneBlk);
        Reg::Duplicate(sumVreg, 0);
        Reg::Duplicate(maxVreg, maxVreg, maskFull);
        for (uint16_t j = 0; j < repeatTimes; ++j) {
            LoadIfNeedCast<T1>(srcVreg, srcUb + i * srcK + j * stride, maskFull);
            SoftmaxExpSub(tmpVreg, srcVreg, maxVreg, maskFull);
            StoreIfNeedCast<float>(workUb + i * srcK + j * stride, tmpVreg, maskFull);
            Reg::Add(sumVreg, sumVreg, tmpVreg, maskFull);
        }
        Reg::ReduceSum(sumVreg, sumVreg, maskFull);
        Reg::Duplicate(sumVreg, sumVreg, maskOneBlk);
        LoadIfNeedCast<T2>(tmpVreg, inSumUb + i * blockStride, maskOneBlk);
        Reg::MulAddDst(sumVreg, expMaxVreg, tmpVreg, maskOneBlk);
        Reg::Mul(tmpVreg, expMaxVreg, tmpVreg, maskOneBlk);
        Reg::Div(expMaxVreg, tmpVreg, sumVreg, maskOneBlk);
        if constexpr (sizeof(T1) == 2 && sizeof(T2) == 4) {
            Reg::Interleave(expMaxVreg, tmpVreg, expMaxVreg, expMaxVreg);
            StoreIfNeedCast<T1>(expMaxUb + i * blockStride * 2, expMaxVreg, maskExpMax);
        } else {
            StoreIfNeedCast<T1>(expMaxUb + i * blockStride, expMaxVreg, maskOneBlk);
        }

        StoreIfNeedCast<T2>(sumUb + i * blockStride, sumVreg, maskOneBlk);
        if constexpr (sizeof(T2) == sizeof(half)) {
            Reg::StoreAlign(tmpUb + i * blockStride, sumVreg, maskOneBlk);
        }
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < srcM; ++i) {
        if constexpr (sizeof(T2) == sizeof(half)) {
            Reg::LoadAlign(sumVreg, tmpUb + i * blockStride);
        } else {
            Reg::LoadAlign(sumVreg, sumUb + i * blockStride);
        }
        Reg::Duplicate(sumVreg, sumVreg, maskFull);
        for (uint16_t j = 0; j < repeatTimes; ++j) {
            Reg::LoadAlign(tmpVreg, workUb + i * srcK + j * stride);
            Reg::Div(dstVreg, tmpVreg, sumVreg, maskFull);
            StoreIfNeedCast<T1>(dstUb + i * srcK + j * stride, dstVreg, maskFull);
        }
    }
}

template <typename T1, typename T2>
__simd_vf__ inline void SoftmaxFlashNDWithTailImpl(
    __ubuf__ T1* dstUb, __ubuf__ T2* sumUb, __ubuf__ T2* maxUb, __ubuf__ T1* srcUb, __ubuf__ T1* expMaxUb,
    __ubuf__ T2* inSumUb, __ubuf__ T2* inMaxUb, __ubuf__ float* workUb, __ubuf__ float* tmpUb, const uint16_t srcM,
    const uint16_t repeatTimes, const uint32_t srcK, const uint32_t originK)
{
    constexpr uint32_t stride = GetVecLen() / sizeof(float);
    constexpr uint32_t blockStride = GetDataBlockSizeInBytes() / sizeof(T2);
    NotNumUnion notNum;
    notNum.i = F32_NEG_INF;

    Reg::MaskReg maskCnt;
    Reg::MaskReg maskFull = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::MaskReg maskExpMax = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL16>();
    Reg::MaskReg maskOneBlk;
    if constexpr (IsSameType<T2, half>::value) {
        maskOneBlk = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL16>();
    } else {
        maskOneBlk = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL8>();
    }
    Reg::RegTensor<float> srcVreg;
    Reg::RegTensor<float> maxVreg;
    Reg::RegTensor<float> expMaxVreg;
    Reg::RegTensor<float> sumVreg;
    Reg::RegTensor<float> tmpVreg;
    Reg::RegTensor<float> minVreg;
    Reg::RegTensor<float> dstVreg;
    Reg::RegTensor<T1> t1Reg;

    Duplicate(minVreg, notNum.f);
    for (uint16_t i = 0; i < srcM; ++i) {
        uint32_t sreg = originK;
        Duplicate(maxVreg, notNum.f);
        for (uint16_t j = 0; j < static_cast<uint16_t>(repeatTimes - 1); ++j) {
            maskCnt = Reg::UpdateMask<uint32_t>(sreg);
            LoadIfNeedCast<T1>(srcVreg, srcUb + i * srcK + j * stride, maskFull);
            Reg::Max(maxVreg, maxVreg, srcVreg, maskCnt);
        }
        maskCnt = Reg::UpdateMask<uint32_t>(sreg);
        LoadIfNeedCast<T1>(srcVreg, srcUb + i * srcK + (repeatTimes - 1) * stride, maskFull);
        Reg::Select(srcVreg, srcVreg, minVreg, maskCnt);
        Reg::Max(maxVreg, maxVreg, srcVreg, maskFull);

        Reg::ReduceMax(maxVreg, maxVreg, maskFull);
        Duplicate(maxVreg, maxVreg, maskOneBlk);
        LoadIfNeedCast<T2>(tmpVreg, inMaxUb + i * blockStride, maskOneBlk);
        Reg::Max(maxVreg, maxVreg, tmpVreg, maskOneBlk);
        StoreIfNeedCast<T2>(maxUb + i * blockStride, maxVreg, maskOneBlk);

        SoftmaxExpSub(expMaxVreg, tmpVreg, maxVreg, maskOneBlk);
        Duplicate(sumVreg, 0);
        Duplicate(maxVreg, maxVreg, maskFull);
        sreg = originK;
        for (uint16_t j = 0; j < repeatTimes; ++j) {
            maskCnt = Reg::UpdateMask<uint32_t>(sreg);
            LoadIfNeedCast<T1>(srcVreg, srcUb + i * srcK + j * stride, maskFull);
            SoftmaxExpSub(tmpVreg, srcVreg, maxVreg, maskCnt);
            StoreIfNeedCast<float>(workUb + i * srcK + j * stride, tmpVreg, maskCnt);
            Reg::Add(sumVreg, sumVreg, tmpVreg, maskFull);
        }
        Reg::ReduceSum(sumVreg, sumVreg, maskFull);
        Duplicate(sumVreg, sumVreg, maskOneBlk);
        LoadIfNeedCast<T2>(tmpVreg, inSumUb + i * blockStride, maskOneBlk);
        Reg::MulAddDst(sumVreg, expMaxVreg, tmpVreg, maskOneBlk);
        Reg::Mul(tmpVreg, expMaxVreg, tmpVreg, maskOneBlk);
        Reg::Div(expMaxVreg, tmpVreg, sumVreg, maskOneBlk);
        if constexpr (sizeof(T1) == 2 && sizeof(T2) == 4) {
            Reg::Interleave(expMaxVreg, tmpVreg, expMaxVreg, expMaxVreg);
            StoreIfNeedCast<T1>(expMaxUb + i * blockStride * 2, expMaxVreg, maskExpMax);
        } else {
            StoreIfNeedCast<T1>(expMaxUb + i * blockStride, expMaxVreg, maskOneBlk);
        }
        StoreIfNeedCast<T2>(sumUb + i * blockStride, sumVreg, maskOneBlk);
        if constexpr (sizeof(T2) == sizeof(half)) {
            Reg::StoreAlign(tmpUb + i * blockStride, sumVreg, maskOneBlk);
        }
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < srcM; ++i) {
        if constexpr (sizeof(T2) == sizeof(half)) {
            Reg::LoadAlign(sumVreg, tmpUb + i * blockStride);
        } else {
            Reg::LoadAlign(sumVreg, sumUb + i * blockStride);
        }
        Duplicate(sumVreg, sumVreg, maskFull);
        uint32_t sreg = originK;
        for (uint16_t j = 0; j < repeatTimes; ++j) {
            maskCnt = Reg::UpdateMask<uint32_t>(sreg);
            Reg::LoadAlign(tmpVreg, workUb + i * srcK + j * stride);
            Reg::Div(dstVreg, tmpVreg, sumVreg, maskCnt);
            StoreIfNeedCast<T1>(dstUb + i * srcK + j * stride, dstVreg, maskCnt);
        }
    }
}
} // namespace Internal

template <typename T1, typename T2, bool isBasicBlock = false>
__aicore__ inline void SoftmaxFlashPostProcess(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& sumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inSumTensor,
    const LocalTensor<T2>& inMaxTensor, const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape,
    const SoftMaxTiling& tiling, bool isUpdate = false, const SoftMaxShapeInfo& softmaxShapeInfo = {})
{
    static_assert(
        (SupportType<Tuple<T1, T2>, Tuple<half, float>, Tuple<half, half>, Tuple<float, float>>()),
        "Failed to check dtype in SoftmaxFlash, current api "
        "support dtype combination is T1 : half, T2 : float; T1 : half, T2 : half; "
        "T1 : float, T2 : float");
    constexpr uint32_t stride = GetVecLen() / sizeof(float);
    constexpr uint32_t blockStride = GetDataBlockSizeInBytes() / sizeof(T2);
    uint32_t srcK = tiling.srcK;
    uint32_t originK = originalSrcShape.k;
    uint16_t srcM = static_cast<uint16_t>(originalSrcShape.m);
    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(originK, stride));

    __ubuf__ T1* dstUb = (__ubuf__ T1*)dstTensor.GetPhyAddr();
    __ubuf__ T2* sumUb = (__ubuf__ T2*)sumTensor.GetPhyAddr();
    __ubuf__ T2* inSumUb = (__ubuf__ T2*)inSumTensor.GetPhyAddr();
    __ubuf__ T2* maxUb = (__ubuf__ T2*)maxTensor.GetPhyAddr();
    __ubuf__ T2* inMaxUb = (__ubuf__ T2*)inMaxTensor.GetPhyAddr();
    __ubuf__ T1* srcUb = (__ubuf__ T1*)srcTensor.GetPhyAddr();
    __ubuf__ T1* expMaxUb = (__ubuf__ T1*)expMaxTensor.GetPhyAddr();
    __ubuf__ float* tmpUb = (__ubuf__ float*)workLocal.GetPhyAddr();
    __ubuf__ float* workUb = (__ubuf__ float*)workLocal.GetPhyAddr(srcM * blockStride);

    if (!isUpdate) {
        SoftMaxNDImpl<T1, T2, isBasicBlock>(
            dstTensor, sumTensor, maxTensor, srcTensor, workLocal, originalSrcShape, tiling);
    } else {
        if constexpr (isBasicBlock) {
            Internal::SoftmaxFlashNDImpl<T1, T2>(
                dstUb, sumUb, maxUb, srcUb, expMaxUb, inSumUb, inMaxUb, workUb, tmpUb, srcM, repeatTimes, srcK);
        } else {
            if (originalSrcShape.k % stride != 0) {
                Internal::SoftmaxFlashNDWithTailImpl<T1, T2>(
                    dstUb, sumUb, maxUb, srcUb, expMaxUb, inSumUb, inMaxUb, workUb, tmpUb, srcM, repeatTimes, srcK,
                    originK);
            } else {
                Internal::SoftmaxFlashNDImpl<T1, T2>(
                    dstUb, sumUb, maxUb, srcUb, expMaxUb, inSumUb, inMaxUb, workUb, tmpUb, srcM, repeatTimes, srcK);
            }
        }
    }
}

template <typename T1, typename T2, bool isReuseSource = false, bool isBasicBlock = false>
__aicore__ inline void SoftmaxFlashCommonImpl(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& sumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inSumTensor,
    const LocalTensor<T2>& inMaxTensor, const SoftMaxTiling& tiling, bool isUpdate,
    const SoftMaxShapeInfo& softmaxShapeInfo)
{
    LocalTensor<float> workLocal;
    PopStackBuffer<float, TPosition::LCM>(workLocal);
    uint32_t workLocalSize = workLocal.GetSize();
    LastAxisShapeND srcNDinfo;
    LastAxisShapeND originalSrcShape;
    if (softmaxShapeInfo.srcM == 0 || softmaxShapeInfo.srcK == 0) {
        ShapeInfo srcShape = srcTensor.GetShapeInfo();
        srcNDinfo = GetLastAxisShapeND(srcShape);
        originalSrcShape = GetLastAxisOriginShapeND(srcShape);
    } else {
        srcNDinfo = {softmaxShapeInfo.srcM, softmaxShapeInfo.srcK};
        originalSrcShape = {softmaxShapeInfo.oriSrcM, softmaxShapeInfo.oriSrcK};
    }
    if constexpr (std::is_same_v<T1, half> && std::is_same_v<T2, float>) {
        if (srcNDinfo.k != tiling.srcK || srcNDinfo.m != tiling.srcM) {
            SoftMaxTiling newTiling = tiling;
            SoftMaxFlashTilingFunc(workLocalSize, srcNDinfo, newTiling, FLOAT_NUM_PER_BLK, isUpdate, isBasicBlock);
            SoftmaxFlashPostProcess<half, float, isBasicBlock>(
                dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, workLocal,
                originalSrcShape, newTiling, isUpdate);
        } else {
            SoftmaxFlashPostProcess<half, float, isBasicBlock>(
                dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, workLocal,
                originalSrcShape, tiling, isUpdate);
        }
    } else if (std::is_same_v<T1, T2>) {
        const uint32_t elementNumPerBlk = ONE_BLK_SIZE / sizeof(T1);
        const uint32_t elementNumPerRep = ONE_REPEAT_BYTE_SIZE / sizeof(T1);
        if (unlikely(srcNDinfo.k != tiling.srcK || srcNDinfo.m != tiling.srcM)) {
            SoftMaxTiling newTiling = tiling;
            SoftMaxFlashTilingFunc(workLocalSize, srcNDinfo, newTiling, elementNumPerBlk, isUpdate, isBasicBlock);
            SoftmaxFlashPostProcess<T1, T2, isBasicBlock>(
                dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, workLocal,
                originalSrcShape, newTiling, isUpdate);
        } else {
            SoftmaxFlashPostProcess<T1, T2, isBasicBlock>(
                dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, workLocal,
                originalSrcShape, tiling, isUpdate);
        }
    }
}

template <typename T1, typename T2, bool isReuseSource = false, bool isBasicBlock = false>
__aicore__ inline void SoftmaxFlashTmpBufCommonImpl(
    const LocalTensor<T1>& dstTensor, const LocalTensor<T2>& sumTensor, const LocalTensor<T2>& maxTensor,
    const LocalTensor<T1>& srcTensor, const LocalTensor<T1>& expMaxTensor, const LocalTensor<T2>& inSumTensor,
    const LocalTensor<T2>& inMaxTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const SoftMaxTiling& tiling,
    bool isUpdate, const SoftMaxShapeInfo& softmaxShapeInfo)
{
    auto tempBuffer = sharedTmpBuffer.ReinterpretCast<float>();
    uint32_t workLocalSize = tempBuffer.GetSize();
    LastAxisShapeND srcNDinfo;
    LastAxisShapeND originalSrcShape;
    if (softmaxShapeInfo.srcM == 0 || softmaxShapeInfo.srcK == 0) {
        ShapeInfo srcShape = srcTensor.GetShapeInfo();
        srcNDinfo = GetLastAxisShapeND(srcShape);
        originalSrcShape = GetLastAxisOriginShapeND(srcShape);
    } else {
        srcNDinfo = {softmaxShapeInfo.srcM, softmaxShapeInfo.srcK};
        originalSrcShape = {softmaxShapeInfo.oriSrcM, softmaxShapeInfo.oriSrcK};
    }
    if constexpr (std::is_same_v<T1, half> && std::is_same_v<T2, float>) {
        if (srcNDinfo.k != tiling.srcK || srcNDinfo.m != tiling.srcM) {
            SoftMaxTiling newTiling = tiling;
            SoftMaxFlashTilingFunc(workLocalSize, srcNDinfo, newTiling, FLOAT_NUM_PER_BLK, isUpdate, isBasicBlock);
            SoftmaxFlashPostProcess<half, float, isBasicBlock>(
                dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, tempBuffer,
                originalSrcShape, newTiling, isUpdate);
        } else {
            SoftmaxFlashPostProcess<half, float, isBasicBlock>(
                dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, tempBuffer,
                originalSrcShape, tiling, isUpdate);
        }
    } else if (std::is_same_v<T1, T2>) {
        const uint32_t elementNumPerBlk = ONE_BLK_SIZE / sizeof(T1);
        const uint32_t elementNumPerRep = ONE_REPEAT_BYTE_SIZE / sizeof(T1);
        if (unlikely(srcNDinfo.k != tiling.srcK || srcNDinfo.m != tiling.srcM)) {
            SoftMaxTiling newTiling = tiling;
            SoftMaxFlashTilingFunc(workLocalSize, srcNDinfo, newTiling, elementNumPerBlk, isUpdate, isBasicBlock);
            SoftmaxFlashPostProcess<T1, T2, isBasicBlock>(
                dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, tempBuffer,
                originalSrcShape, newTiling, isUpdate);
        } else {
            SoftmaxFlashPostProcess<T1, T2, isBasicBlock>(
                dstTensor, sumTensor, maxTensor, srcTensor, expMaxTensor, inSumTensor, inMaxTensor, tempBuffer,
                originalSrcShape, tiling, isUpdate);
        }
    }
}

} // namespace AscendC
#endif // IMPL_ACTIVATION_SOFTMAX_C310_SOFTMAX_FLASH_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASH_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASH_IMPL_H__
#endif
