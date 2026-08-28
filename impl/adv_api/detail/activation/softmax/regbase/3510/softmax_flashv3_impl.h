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
 * \file softmax_flashv3_c310_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/activation/softmax/regbase/3510/softmax_flashv3_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/softmaxflashv3.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV3_IMPL_H__
#endif

#ifndef IMPL_ACTIVATION_SOFTMAX_C310_SOFTMAX_FLASHV3_IMPL_H
#define IMPL_ACTIVATION_SOFTMAX_C310_SOFTMAX_FLASHV3_IMPL_H

#include "softmax_common_impl.h"
#include "../../../../common/common.h"

namespace AscendC {
template <typename T, typename U>
__simd_vf__ __aicore__ inline void SoftmaxFlashV3NDNoUpdateImpl(
    __ubuf__ T* dstUb, __ubuf__ U* meanUb, __ubuf__ U* expSumUb, __ubuf__ U* maxUb, __ubuf__ T* srcUb,
    __ubuf__ float* workUb, __ubuf__ float* newSrcUb, const uint16_t srcM, const uint16_t srcK,
    const uint16_t splitMeanCnt, const uint16_t baseK, const uint16_t tail, const uint16_t remainRepeatTime,
    const uint16_t kRepeatTime, const uint16_t baseKRepeatTime, const float scalar, const float r0, const float r1)
{
    constexpr uint32_t repeatStride = GetVecLen() / sizeof(float);
    constexpr uint32_t blockStride = GetDataBlockSizeInBytes() / sizeof(U);
    constexpr uint16_t repeatTime = static_cast<uint16_t>(repeatStride / blockStride);

    Reg::MaskReg maskCnt;
    Reg::MaskReg maskFull = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::MaskReg maskOnePt = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL1>();
    Reg::MaskReg maskOneBlk = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL8>();
    Reg::RegTensor<float> srcVreg;
    Reg::RegTensor<float> maxVreg;
    Reg::RegTensor<float> sumVreg;
    Reg::RegTensor<float> meanVreg;
    Reg::RegTensor<float> tmpVreg;
    Reg::RegTensor<float> dstVreg;
    Reg::RegTensor<float> minVreg;
    Reg::RegTensor<T> castVreg;
    Reg::UnalignReg ureg0, ureg1;
    NotNumUnion notNum;
    notNum.i = F32_NEG_INF;

    Reg::Duplicate(minVreg, notNum.f);
    for (uint16_t i = 0; i < srcM; ++i) {
        for (uint16_t j = 0; j < repeatTime; ++j) {
            LoadIfNeedCast<T>(srcVreg, srcUb + i * srcK + j * repeatStride, maskFull);
            Reg::ReduceSumWithDataBlock(sumVreg, srcVreg, maskFull);
            Reg::StoreAlign<float>(workUb + i * repeatStride + j * blockStride, sumVreg, maskOneBlk);
        }
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < srcM; ++i) {
        Reg::LoadAlign<float>(sumVreg, workUb + i * repeatStride);
        for (uint16_t j = 0; j < remainRepeatTime; ++j) {
            LoadIfNeedCast<T>(srcVreg, srcUb + i * srcK + repeatStride * splitMeanCnt + j * repeatStride, maskFull);
            Reg::Add(sumVreg, srcVreg, sumVreg, maskFull);
        }
        Reg::StoreAlign<float>(workUb + i * repeatStride, sumVreg, maskFull);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < srcM; ++i) {
        Reg::LoadAlign<float>(sumVreg, workUb + i * repeatStride);
        Reg::ReduceSumWithDataBlock(sumVreg, sumVreg, maskFull);
        Reg::Muls(meanVreg, sumVreg, r0, maskOneBlk);

        Reg::ReduceSum(tmpVreg, meanVreg, maskOneBlk);
        Reg::Muls(tmpVreg, tmpVreg, r1, maskOnePt);
        Reg::Duplicate(tmpVreg, tmpVreg, maskOneBlk);
        StoreIfNeedCast<U>(meanUb + i * blockStride, tmpVreg, maskOneBlk);
        Reg::Sub(tmpVreg, tmpVreg, meanVreg, maskOneBlk);
        Reg::Muls(tmpVreg, tmpVreg, scalar, maskOneBlk); // scalar = alpha / (1 - alpha)
        Reg::StoreAlign<float>(workUb + i * blockStride, tmpVreg, maskOneBlk);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < srcM; ++i) {
        Reg::Duplicate(maxVreg, notNum.f);
        for (uint16_t j = 0; j < splitMeanCnt; ++j) { // 8
            Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(meanVreg, workUb + i * splitMeanCnt + j);
            uint32_t sreg = baseK;
            for (uint16_t k = 0; k < baseKRepeatTime; ++k) { // baseK / 64
                maskCnt = Reg::UpdateMask<uint32_t>(sreg);
                __ubuf__ T* srcUbTmp = srcUb + i * srcK + j * baseK + k * repeatStride;
                Reg::LoadUnAlignPre(ureg0, srcUbTmp);
                Reg::LoadUnAlign(castVreg, ureg0, srcUbTmp, repeatStride);
                Reg::UnPack<uint32_t, uint16_t>(
                    (Reg::RegTensor<uint32_t>&)castVreg, (Reg::RegTensor<uint16_t>&)castVreg);
                Reg::Cast<float, T, Internal::castTraitB16ToB32>(srcVreg, castVreg, maskCnt);
                Reg::Sub(srcVreg, srcVreg, meanVreg, maskCnt);
                __ubuf__ float* newSrcUbTmp = newSrcUb + i * srcK + j * baseK + k * repeatStride;
                Reg::StoreUnAlign(newSrcUbTmp, srcVreg, ureg1, repeatStride);
                Reg::StoreUnAlignPost(newSrcUbTmp, ureg1, 0);
                Reg::Max(maxVreg, maxVreg, srcVreg, maskFull);
            }
            maskCnt = Reg::UpdateMask<uint32_t>(sreg);
            __ubuf__ T* srcUbTmp = srcUb + i * srcK + j * baseK + baseKRepeatTime * repeatStride;
            Reg::LoadUnAlignPre(ureg0, srcUbTmp);
            Reg::LoadUnAlign(castVreg, ureg0, srcUbTmp, tail);
            Reg::UnPack<uint32_t, uint16_t>((Reg::RegTensor<uint32_t>&)castVreg, (Reg::RegTensor<uint16_t>&)castVreg);
            Reg::Cast<float, T, Internal::castTraitB16ToB32>(srcVreg, castVreg, maskCnt);
            Reg::Sub(srcVreg, srcVreg, meanVreg, maskCnt);
            __ubuf__ float* newSrcUbTmp = newSrcUb + i * srcK + j * baseK + baseKRepeatTime * repeatStride;
            Reg::StoreUnAlign(newSrcUbTmp, srcVreg, ureg1, tail);
            Reg::StoreUnAlignPost(newSrcUbTmp, ureg1, 0);
            Reg::Select(srcVreg, srcVreg, minVreg, maskCnt);
            Reg::Max(maxVreg, maxVreg, srcVreg, maskFull);
        }
        Reg::ReduceMax(maxVreg, maxVreg, maskFull);
        Reg::Duplicate(maxVreg, maxVreg, maskOneBlk);
        StoreIfNeedCast<U>(maxUb + i * blockStride, maxVreg, maskOneBlk);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < srcM; ++i) {
        Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(maxVreg, maxUb + i * blockStride);
        Reg::Duplicate(sumVreg, 0);
        for (uint16_t k = 0; k < kRepeatTime; ++k) { // k / 64
            Reg::LoadAlign<float>(srcVreg, newSrcUb + i * srcK + k * repeatStride);
            SoftmaxExpSub(dstVreg, srcVreg, maxVreg, maskFull);
            StoreIfNeedCast<T>(dstUb + i * srcK + k * repeatStride, dstVreg, maskFull);
            Reg::Add(sumVreg, sumVreg, dstVreg, maskFull);
        }
        Reg::ReduceSum(sumVreg, sumVreg, maskFull);
        Reg::Duplicate(sumVreg, sumVreg, maskOneBlk);
        StoreIfNeedCast<U>(expSumUb + i * blockStride, sumVreg, maskOneBlk);
    }
}

template <typename T, typename U>
__simd_vf__ __aicore__ inline void SoftmaxFlashV3NDUpdateImpl(
    __ubuf__ T* dstUb, __ubuf__ U* meanUb, __ubuf__ U* expSumUb, __ubuf__ U* maxUb, __ubuf__ T* srcUb,
    __ubuf__ T* expMaxUb, __ubuf__ U* inMeanUb, __ubuf__ U* inExpSumUb, __ubuf__ U* inMaxUb, __ubuf__ float* workUb,
    __ubuf__ float* newSrcUb, __ubuf__ float* tmpUb, const uint16_t srcM, const uint16_t srcK,
    const uint16_t splitMeanCnt, const uint16_t baseK, const uint16_t tail, const uint16_t remainRepeatTime,
    const uint16_t kRepeatTime, const uint16_t baseKRepeatTime, const uint32_t loopCnt, const float scalar,
    const float r0, const float r1, const float r2, const float r3)
{
    constexpr uint32_t repeatStride = GetVecLen() / sizeof(float);
    constexpr uint32_t blockStride = GetDataBlockSizeInBytes() / sizeof(U);
    constexpr uint16_t repeatTime = static_cast<uint16_t>(repeatStride / blockStride);

    Reg::MaskReg maskCnt;
    Reg::MaskReg maskFull = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::MaskReg maskOnePt = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL1>();
    Reg::MaskReg maskOneBlk = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL8>();
    Reg::MaskReg maskOut = Reg::CreateMask<uint32_t, Reg::MaskPattern::VL16>();
    Reg::RegTensor<float> srcVreg;
    Reg::RegTensor<float> maxVreg;
    Reg::RegTensor<float> sumVreg;
    Reg::RegTensor<float> meanVreg;
    Reg::RegTensor<float> inputVreg;
    Reg::RegTensor<float> shiftVreg;
    Reg::RegTensor<float> tmpVreg;
    Reg::RegTensor<float> dstVreg;
    Reg::RegTensor<float> minVreg;
    Reg::RegTensor<T> castVreg;
    Reg::UnalignReg ureg0, ureg1;
    NotNumUnion notNum;
    notNum.i = F32_NEG_INF;

    Reg::Duplicate(minVreg, notNum.f);
    for (uint16_t i = 0; i < srcM; ++i) {
        for (uint16_t j = 0; j < repeatTime; ++j) {
            LoadIfNeedCast<T>(srcVreg, srcUb + i * srcK + j * repeatStride, maskFull);
            Reg::ReduceSumWithDataBlock(sumVreg, srcVreg, maskFull);
            Reg::StoreAlign<float>(workUb + i * repeatStride + j * blockStride, sumVreg, maskOneBlk);
        }
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < srcM; ++i) {
        Reg::LoadAlign<float>(sumVreg, workUb + i * repeatStride);
        for (uint16_t j = 0; j < remainRepeatTime; ++j) {
            LoadIfNeedCast<T>(srcVreg, srcUb + i * srcK + repeatStride * splitMeanCnt + j * repeatStride, maskFull);
            Reg::Add(sumVreg, srcVreg, sumVreg, maskFull);
        }
        Reg::StoreAlign<float>(workUb + i * repeatStride, sumVreg, maskFull);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < srcM; ++i) {
        Reg::LoadAlign<float>(sumVreg, workUb + i * repeatStride);
        Reg::ReduceSumWithDataBlock(sumVreg, sumVreg, maskFull);
        Reg::Muls(meanVreg, sumVreg, r0, maskOneBlk);

        Reg::ReduceSum(tmpVreg, meanVreg, maskOneBlk);
        Reg::Muls(tmpVreg, tmpVreg, r1, maskOnePt);
        Reg::Duplicate(tmpVreg, tmpVreg, maskOneBlk);
        Reg::StoreAlign<float>(tmpUb + i * blockStride, tmpVreg, maskOneBlk);
        Reg::Sub(tmpVreg, tmpVreg, meanVreg, maskOneBlk);
        Reg::Muls(tmpVreg, tmpVreg, scalar, maskOneBlk); // scalar = alpha / (1 - alpha)
        Reg::StoreAlign<float>(workUb + i * blockStride, tmpVreg, maskOneBlk);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < srcM; ++i) {
        LoadIfNeedCast<U>(inputVreg, inMeanUb + i * blockStride, maskOneBlk);
        Reg::LoadAlign<float>(tmpVreg, tmpUb + i * blockStride);
        Reg::Muls(shiftVreg, inputVreg, r2, maskOneBlk);
        Reg::Add(shiftVreg, shiftVreg, tmpVreg, maskOneBlk);
        Reg::Muls(shiftVreg, shiftVreg, r3, maskOneBlk);
        StoreIfNeedCast<U>(meanUb + i * blockStride, shiftVreg, maskOneBlk);

        Reg::Duplicate(maxVreg, notNum.f);
        for (uint16_t j = 0; j < splitMeanCnt; ++j) { // 8
            Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(meanVreg, workUb + i * splitMeanCnt + j);
            uint32_t sreg = baseK;
            for (uint16_t k = 0; k < baseKRepeatTime; ++k) { // baseK / 64
                maskCnt = Reg::UpdateMask<uint32_t>(sreg);
                __ubuf__ T* srcUbTmp = srcUb + i * srcK + j * baseK + k * repeatStride;
                Reg::LoadUnAlignPre(ureg0, srcUbTmp);
                Reg::LoadUnAlign(castVreg, ureg0, srcUbTmp, repeatStride);
                Reg::UnPack<uint32_t, uint16_t>(
                    (Reg::RegTensor<uint32_t>&)castVreg, (Reg::RegTensor<uint16_t>&)castVreg);
                Reg::Cast<float, T, Internal::castTraitB16ToB32>(srcVreg, castVreg, maskCnt);
                Reg::Sub(srcVreg, srcVreg, meanVreg, maskCnt);
                __ubuf__ float* newSrcUbTmp = newSrcUb + i * srcK + j * baseK + k * repeatStride;
                Reg::StoreUnAlign(newSrcUbTmp, srcVreg, ureg1, repeatStride);
                Reg::StoreUnAlignPost(newSrcUbTmp, ureg1, 0);
                Reg::Max(maxVreg, maxVreg, srcVreg, maskFull);
            }
            maskCnt = Reg::UpdateMask<uint32_t>(sreg);
            __ubuf__ T* srcUbTmp = srcUb + i * srcK + j * baseK + baseKRepeatTime * repeatStride;
            Reg::LoadUnAlignPre(ureg0, srcUbTmp);
            Reg::LoadUnAlign(castVreg, ureg0, srcUbTmp, tail);
            Reg::UnPack<uint32_t, uint16_t>((Reg::RegTensor<uint32_t>&)castVreg, (Reg::RegTensor<uint16_t>&)castVreg);
            Reg::Cast<float, T, Internal::castTraitB16ToB32>(srcVreg, castVreg, maskCnt);
            Reg::Sub(srcVreg, srcVreg, meanVreg, maskCnt);
            __ubuf__ float* newSrcUbTmp = newSrcUb + i * srcK + j * baseK + baseKRepeatTime * repeatStride;
            Reg::StoreUnAlign(newSrcUbTmp, srcVreg, ureg1, tail);
            Reg::StoreUnAlignPost(newSrcUbTmp, ureg1, 0);
            Reg::Select(srcVreg, srcVreg, minVreg, maskCnt);
            Reg::Max(maxVreg, maxVreg, srcVreg, maskFull);
        }
        Reg::ReduceMax(maxVreg, maxVreg, maskFull);
        Reg::Duplicate(maxVreg, maxVreg, maskOneBlk);

        Reg::Sub(dstVreg, tmpVreg, shiftVreg, maskOneBlk);
        Reg::Muls(dstVreg, dstVreg, scalar, maskOneBlk); // scalar = alpha / (1 - alpha)
        Reg::Sub(tmpVreg, inputVreg, shiftVreg, maskOneBlk);
        Reg::Muls(tmpVreg, tmpVreg, scalar, maskOneBlk); // scalar = alpha / (1 - alpha)
        Reg::Add(maxVreg, dstVreg, maxVreg, maskOneBlk);
        LoadIfNeedCast<U>(inputVreg, inMaxUb + i * blockStride, maskOneBlk);
        Reg::Add(tmpVreg, inputVreg, tmpVreg, maskOneBlk);
        Reg::Max(maxVreg, tmpVreg, maxVreg, maskOneBlk);
        StoreIfNeedCast<U>(maxUb + i * blockStride, maxVreg, maskOneBlk);
        Reg::Sub(maxVreg, maxVreg, dstVreg, maskOneBlk);
        Reg::StoreAlign<float>(tmpUb + i * blockStride, maxVreg, maskOneBlk);
        SoftmaxExpSub(tmpVreg, tmpVreg, maxVreg, maskFull);
        LoadIfNeedCast<U>(inputVreg, inExpSumUb + i * blockStride, maskOneBlk);
        Reg::Mul(sumVreg, tmpVreg, inputVreg, maskOneBlk);
        Reg::StoreAlign<float>(expSumUb + i * blockStride, sumVreg, maskOneBlk);
        Reg::Interleave(tmpVreg, dstVreg, tmpVreg, tmpVreg);
        StoreIfNeedCast<T>(expMaxUb + i * blockStride * 2, tmpVreg, maskOut);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < srcM; ++i) {
        Reg::Duplicate(sumVreg, 0);
        Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(maxVreg, tmpUb + i * blockStride);
        for (uint16_t k = 0; k < kRepeatTime; ++k) { // k / 64
            Reg::LoadAlign<float>(srcVreg, newSrcUb + i * srcK + k * repeatStride);
            SoftmaxExpSub(dstVreg, srcVreg, maxVreg, maskFull);
            StoreIfNeedCast<T>(dstUb + i * srcK + k * repeatStride, dstVreg, maskFull);
            Reg::Add(sumVreg, sumVreg, dstVreg, maskFull);
        }
        Reg::ReduceSum(sumVreg, sumVreg, maskFull);
        Reg::Duplicate(sumVreg, sumVreg, maskOneBlk);
        Reg::LoadAlign<float>(tmpVreg, expSumUb + i * blockStride);
        Reg::Add(sumVreg, tmpVreg, sumVreg, maskOneBlk);
        StoreIfNeedCast<U>(expSumUb + i * blockStride, sumVreg, maskOneBlk);
    }
}

template <
    typename T, typename U, bool isUpdate = false, bool isBasicBlock = false,
    const SoftmaxConfig& config = SOFTMAX_DEFAULT_CFG>
__aicore__ inline void SoftmaxFlashV3Process(
    const LocalTensor<T>& dstTensor, const LocalTensor<U>& meanTensor, const LocalTensor<U>& expSumTensor,
    const LocalTensor<U>& maxTensor, const LocalTensor<T>& srcTensor, const LocalTensor<T>& expMaxTensor,
    const LocalTensor<U>& inMeanTensor, const LocalTensor<U>& inExpSumTensor, const LocalTensor<U>& inMaxTensor,
    const LocalTensor<float>& workLocal, const LastAxisShapeND& originalSrcShape, const SoftMaxTiling& tiling,
    const SoftMaxParams& params)
{
    constexpr uint16_t repeatStride = GetVecLen() / sizeof(float);
    uint16_t srcM = tiling.srcM;
    uint16_t srcK = tiling.srcK;
    uint16_t splitMeanCnt = static_cast<uint16_t>(params.splitMeanCnt);
    uint16_t baseK = static_cast<uint16_t>(srcK / splitMeanCnt);
    uint16_t kRepeatTime = static_cast<uint16_t>(srcK / repeatStride);
    uint16_t remainRepeatTime = kRepeatTime - splitMeanCnt;
    uint16_t baseKRepeatTime = CeilDivision(baseK, repeatStride) - 1;
    uint16_t tail = baseK - baseKRepeatTime * repeatStride;
    uint32_t loopCnt = params.loopCnt;
    float scalar = params.alpha / (1 - params.alpha);
    float r0 = static_cast<float>(1.0f / baseK);
    float r1 = static_cast<float>(1.0f / splitMeanCnt);

    __ubuf__ T* dstUb = (__ubuf__ T*)dstTensor.GetPhyAddr();
    __ubuf__ U* meanUb = (__ubuf__ U*)meanTensor.GetPhyAddr();
    __ubuf__ U* inMeanUb = (__ubuf__ U*)inMeanTensor.GetPhyAddr();
    __ubuf__ U* expSumUb = (__ubuf__ U*)expSumTensor.GetPhyAddr();
    __ubuf__ U* inExpSumUb = (__ubuf__ U*)inExpSumTensor.GetPhyAddr();
    __ubuf__ U* maxUb = (__ubuf__ U*)maxTensor.GetPhyAddr();
    __ubuf__ U* inMaxUb = (__ubuf__ U*)inMaxTensor.GetPhyAddr();
    __ubuf__ T* srcUb = (__ubuf__ T*)srcTensor.GetPhyAddr();
    __ubuf__ T* expMaxUb = (__ubuf__ T*)expMaxTensor.GetPhyAddr();
    __ubuf__ float* workUb = (__ubuf__ float*)workLocal.GetPhyAddr();
    __ubuf__ float* newSrcUb = (__ubuf__ float*)workLocal.GetPhyAddr(srcM * repeatStride);

    if constexpr (!isUpdate) {
        SoftmaxFlashV3NDNoUpdateImpl<T, U>(
            dstUb, meanUb, expSumUb, maxUb, srcUb, workUb, newSrcUb, srcM, srcK, splitMeanCnt, baseK, tail,
            remainRepeatTime, kRepeatTime, baseKRepeatTime, scalar, r0, r1);
    } else {
        float r2 = static_cast<float>(loopCnt - 1.0f);
        float r3 = static_cast<float>(1.0f / loopCnt);
        __ubuf__ float* tmpUb = (__ubuf__ float*)workLocal.GetPhyAddr(srcM * repeatStride + srcM * srcK);
        SoftmaxFlashV3NDUpdateImpl<T, U>(
            dstUb, meanUb, expSumUb, maxUb, srcUb, expMaxUb, inMeanUb, inExpSumUb, inMaxUb, workUb, newSrcUb, tmpUb,
            srcM, srcK, splitMeanCnt, baseK, tail, remainRepeatTime, kRepeatTime, baseKRepeatTime, loopCnt, scalar, r0,
            r1, r2, r3);
    }
}
} // namespace AscendC
#endif // IMPL_ACTIVATION_SOFTMAX_C310_SOFTMAX_FLASHV3_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV3_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SOFTMAX_FLASHV3_IMPL_H__
#endif
