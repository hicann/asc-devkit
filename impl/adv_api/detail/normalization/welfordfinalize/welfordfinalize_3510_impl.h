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
 * \file welfordfinalize_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/normalization/welfordfinalize/welfordfinalize_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/layernorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_WELFORDFINALIZE_WELFORDFINALIZE_C310_IMPL_H__
#endif
#ifndef IMPL_NORMALIZATION_WELFORDFINALIZE_C310_IMPL_H
#define IMPL_NORMALIZATION_WELFORDFINALIZE_C310_IMPL_H

#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "kernel_tiling/kernel_tiling.h"
#include "../../../../../include/adv_api/normalization/welfordfinalize_utils.h"
#include "../layernorm/layernorm_3510_utils.h"

namespace AscendC {
namespace Internal {
const uint16_t kWelfordFinalizeFoldNum = 2;
constexpr uint32_t WELFORD_B32_VF_LEN = GetVecLen() / sizeof(uint32_t);
} // namespace Internal

template <uint32_t HalfAddTimes>
__aicore__ constexpr inline uint16_t CalculatePower()
{
    constexpr uint16_t kMaxOffset = 16;
    uint16_t fold = 0;
    for (uint16_t i = 1; i < kMaxOffset; i++) {
        if ((HalfAddTimes >> i) == 0) {
            break;
        }
        fold++;
    }
    return fold;
}

// Calculate the sum of two points based on count. The main block size is ex.2000->1024 900->512.
__aicore__ inline uint32_t CalculateMainBlock(uint32_t count)
{
    count |= count >> 1;
    count |= count >> 2;
    count |= count >> 4;
    count |= count >> 8;
    count |= count >> 16;
    return (count + 1) >> 1;
}

// only support rLength <= 64
template <typename T, bool isCorrection = false>
__simd_vf__ inline void ComputeMean64(
    __ubuf__ float* meanUb, __ubuf__ float* varianceUb, __ubuf__ T* srcUb, const uint32_t aLength,
    const uint32_t rLength, const uint32_t rLengthWithPadding, const float k2Rec, const float k2RRec,
    const float rRecWithCorrection)
{
    constexpr uint16_t sregLower = static_cast<uint16_t>(GetVecLen() / sizeof(float)); // 64
    uint32_t count = rLength;
    Reg::RegTensor<float> src0Reg;
    Reg::RegTensor<float> src1Reg;
    Reg::RegTensor<float> dstReg;
    Reg::RegTensor<float> meanReg;
    Reg::RegTensor<float> varianceReg;

    Reg::MaskReg preg = Reg::UpdateMask<float>(count);
    Reg::MaskReg pregFull = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::MaskReg pregOne = Reg::CreateMask<float, Reg::MaskPattern::VL1>();
    for (uint16_t i = 0; i < static_cast<uint16_t>(aLength); i++) {
        LoadDataWithT<T>(srcUb, src0Reg, preg, i * rLengthWithPadding);
        Muls(src1Reg, src0Reg, k2Rec, preg);
        ReduceSum(dstReg, src1Reg, preg);
        if constexpr (isCorrection) {
            Muls(meanReg, dstReg, rRecWithCorrection, pregOne);
        } else {
            Muls(meanReg, dstReg, k2RRec, pregOne);
        }
        Reg::StoreAlign<float, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>((meanUb + i), meanReg, pregOne);
    }
}

template <typename T, bool isCorrection = false>
__simd_vf__ inline void ComputeMeanVF(
    __ubuf__ float* meanUb, __ubuf__ float* varianceUb, __ubuf__ T* srcUb, __ubuf__ float* workUbOrigin,
    const uint32_t k, const uint32_t aLength, const uint32_t rLength, const uint32_t rLengthWithPadding,
    const uint32_t rHeadLength, const float k2Rec, const float k2RRec, float rRecWithCorrection, uint32_t workCount,
    int16_t halfAddRepeatTimes)
{
    constexpr uint16_t sregLower = static_cast<uint16_t>(GetVecLen() / sizeof(float)); // 64
    const uint32_t m = rLength - rHeadLength;
    uint32_t count;
    const uint32_t mVL = static_cast<uint32_t>(CeilDivision(m, sregLower) * sregLower);
    const uint32_t mainTailCount = rHeadLength - mVL;
    const uint16_t halfAddCount = static_cast<uint16_t>(CeilDivision(rHeadLength, sregLower)); // total count
    const uint16_t halfAddTimes = static_cast<uint16_t>(CeilDivision(halfAddCount, sregLower));
    int16_t lastCount = static_cast<int16_t>(halfAddCount); // last vcadd times less than 64
    if (lastCount > sregLower) {
        lastCount = sregLower;
    }
    uint16_t repeatTimes1 = static_cast<uint16_t>(CeilDivision(m, sregLower));
    uint16_t repeatTimes2 = static_cast<uint16_t>(CeilDivision(mainTailCount, sregLower));

    Reg::RegTensor<float> src0Reg;
    Reg::RegTensor<float> src1Reg;
    Reg::RegTensor<float> dstReg;
    Reg::RegTensor<float> meanReg;

    Reg::MaskReg preg;
    Reg::MaskReg pregFull = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::MaskReg pregOne = Reg::CreateMask<float, Reg::MaskPattern::VL1>();

    for (uint16_t j = 0; j < static_cast<uint16_t>(aLength); j++) {
        // tail block add to main block
        count = m;
        for (uint16_t i = 0; i < repeatTimes1; i++) {
            preg = Reg::UpdateMask<float>(count);
            LoadDataWithT<T>(
                srcUb, srcUb, src0Reg, src1Reg, pregFull, preg, j * rLengthWithPadding + i * sregLower,
                j * rLengthWithPadding + rHeadLength + i * sregLower);
            Muls(src0Reg, src0Reg, k2Rec, pregFull);
            Muls(src1Reg, src1Reg, k2Rec, preg);
            Add(dstReg, src0Reg, src1Reg, pregFull);
            ReduceSum(dstReg, dstReg, pregFull);
            Reg::StoreAlign<float, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>((workUbOrigin + i), dstReg, pregOne);
        }

        // Processes the remaining data of the entire block.
        count = mainTailCount;
        for (uint16_t i = 0; i < repeatTimes2; i++) {
            preg = Reg::UpdateMask<float>(count);
            LoadDataWithT<T>(srcUb, src0Reg, pregFull, j * rLengthWithPadding + mVL + i * sregLower);
            Muls(dstReg, src0Reg, k2Rec, pregFull);
            ReduceSum(dstReg, dstReg, pregFull);
            Reg::StoreAlign<float, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>(
                (workUbOrigin + repeatTimes1 + i), dstReg, pregOne);
        }
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
        // Fold the tmpbuffer in half.
        uint16_t currentHalfAddTimes = halfAddTimes;
        for (uint16_t k = 0; k < static_cast<uint16_t>(halfAddRepeatTimes); k++) {
            currentHalfAddTimes = currentHalfAddTimes / Internal::kWelfordFinalizeFoldNum; // Fold
            for (uint16_t i = 0; i < currentHalfAddTimes; i++) {
                Reg::LoadAlign(src0Reg, workUbOrigin + i * sregLower);
                Reg::LoadAlign(src1Reg, workUbOrigin + (currentHalfAddTimes + i) * sregLower);
                Add(dstReg, src0Reg, src1Reg, pregFull);
                Reg::StoreAlign(workUbOrigin + i * sregLower, dstReg, pregFull);
            }
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
        }
        // the last reducesum
        count = lastCount;
        preg = Reg::UpdateMask<float>(count);
        Reg::LoadAlign(src0Reg, workUbOrigin);
        ReduceSum(dstReg, src0Reg, preg);
        // save mean
        if constexpr (isCorrection) {
            Muls(meanReg, dstReg, rRecWithCorrection, preg);
        } else {
            Muls(meanReg, dstReg, k2RRec, preg);
        }
        Reg::StoreAlign<float, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>((meanUb + j), meanReg, pregOne);
    }
}

template <typename T, bool isCorrection = false>
__aicore__ inline void ComputeMean(
    __ubuf__ float* meanUb, __ubuf__ float* varianceUb, __ubuf__ T* srcUb, __ubuf__ float* workUbOrigin,
    const uint32_t k, const uint32_t aLength, const uint32_t rLength, const uint32_t rLengthWithPadding,
    const uint32_t rHeadLength, const float k2Rec, const float k2RRec, float rRecWithCorrection)
{
    constexpr uint16_t sregLower = static_cast<uint16_t>(GetVecLen() / sizeof(float)); // 64
    uint32_t workCount = static_cast<uint32_t>(
        CeilDivision(rHeadLength / sregLower * sizeof(float), sregLower * Internal::kWelfordFinalizeFoldNum) *
        (sregLower * Internal::kWelfordFinalizeFoldNum)); // 256 * 4 = 1024; 1024/128 * 128
    int16_t halfAddRepeatTimes = static_cast<int16_t>(k - CalculatePower<sregLower * sregLower>());
    if (halfAddRepeatTimes < 0) {
        halfAddRepeatTimes = 0;
    }
    ComputeMeanVF<T, isCorrection>(
        meanUb, varianceUb, srcUb, workUbOrigin, k, aLength, rLength, rLengthWithPadding, rHeadLength, k2Rec, k2RRec,
        rRecWithCorrection, workCount, halfAddRepeatTimes);
}

template <bool isCorrection = false>
__aicore__ inline void BinaryReduceSum(
    __ubuf__ float* dstUb, __ubuf__ float* srcUb, __ubuf__ float* workUbOrigin, uint32_t rLength, uint32_t rHeadLength,
    float k2Rec, float k2RRec, float rRecWithCorrection)
{
    constexpr uint32_t sregLower = static_cast<uint32_t>(GetVecLen() / sizeof(float));
    if (rLength < sregLower) {
        ComputeMean64<float, isCorrection>(
            dstUb, nullptr, srcUb, 1, rLength, rLength, k2Rec, k2RRec, rRecWithCorrection);
    } else {
        uint32_t rLengthWithPadding = static_cast<uint32_t>(CeilDivision(rLength, sregLower)) * sregLower;
        uint32_t rHeadLengthTmp = sregLower;
        uint32_t k = CalculatePower<sregLower>();
        for (uint32_t i = 0; i < rLengthWithPadding; i++) {
            if (rHeadLengthTmp * Internal::kWelfordFinalizeFoldNum > rLengthWithPadding) {
                k += i;
                break;
            }
            rHeadLengthTmp *= Internal::kWelfordFinalizeFoldNum;
        }
        ComputeMean<float, isCorrection>(
            dstUb, nullptr, srcUb, workUbOrigin, k, 1, rLength, rLengthWithPadding, rHeadLength, k2Rec, k2RRec,
            rRecWithCorrection);
    }
}

template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
__simd_vf__ inline void WelfordFinalizeWithCountsOutMeanVF(
    __ubuf__ float* outMean, __ubuf__ int32_t* counts, __ubuf__ float* inMean, __ubuf__ float* inVar,
    __ubuf__ float* tmpbuffer, uint32_t K, uint32_t sregLower, uint16_t repeat, uint16_t m)
{
    Reg::MaskReg preg;
    Reg::RegTensor<int32_t> srcVreg;
    Reg::RegTensor<float> f32vreg;
    Reg::RegTensor<float> tmpVreg;

    Reg::RegTensor<float> meanVreg;
    Reg::RegTensor<float> varVreg;
    Reg::RegTensor<float> outMeanreg;
    Reg::RegTensor<float> outVarreg;

    uint32_t sreg = static_cast<uint32_t>(K);

    for (uint16_t i = 0; i < repeat; ++i) {
        preg = Reg::UpdateMask<uint32_t>(sreg);
        Reg::LoadAlign<int32_t, Reg::LoadDist::DIST_NORM>(srcVreg, counts + i * sregLower);
        Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(meanVreg, inMean + i * sregLower + m * K);
        Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(varVreg, inVar + i * sregLower + m * K);

        Reg::Cast<float, int32_t, MrgZRndA>(f32vreg, srcVreg, preg);

        Reg::Mul(outMeanreg, f32vreg, meanVreg, preg);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM_B32>(tmpbuffer + i * sregLower, outMeanreg, preg);
    }
}

template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
__simd_vf__ inline void WelfordFinalizeWithCountsOutVarVF(
    __ubuf__ float* outMean, __ubuf__ int32_t* counts, __ubuf__ float* inMean, __ubuf__ float* inVar,
    __ubuf__ float* tmpbuffer, uint32_t K, uint32_t sregLower, uint16_t repeat, uint16_t m)
{
    Reg::MaskReg preg;
    Reg::RegTensor<int32_t> srcVreg;
    Reg::RegTensor<float> f32vreg;
    Reg::RegTensor<float> meanVreg;
    Reg::RegTensor<float> varVreg;
    Reg::RegTensor<float> outMeanreg;
    Reg::RegTensor<float> outVarreg;
    Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(outMeanreg, outMean + m);
    uint32_t sreg = static_cast<uint32_t>(K);
    for (uint16_t i = 0; i < repeat; ++i) {
        preg = Reg::UpdateMask<uint32_t>(sreg);
        Reg::LoadAlign<int32_t, Reg::LoadDist::DIST_NORM>(srcVreg, counts + i * sregLower);
        Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(varVreg, inVar + i * sregLower + m * K);
        Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(meanVreg, inMean + i * sregLower + m * K);

        Reg::Cast<float, int32_t, MrgZRndA>(f32vreg, srcVreg, preg);
        Reg::Sub(meanVreg, meanVreg, outMeanreg, preg);
        Reg::Mul(meanVreg, meanVreg, meanVreg, preg);

        Reg::Mul(meanVreg, meanVreg, f32vreg, preg);
        Reg::Add(outVarreg, meanVreg, varVreg, preg);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM_B32>(tmpbuffer + i * sregLower, outVarreg, preg);
    }
}

template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
__aicore__ inline void WelfordFinalizeWithCounts(
    __ubuf__ float* outMean, __ubuf__ float* outVar, __ubuf__ int32_t* counts, __ubuf__ float* inMean,
    __ubuf__ float* inVar, __ubuf__ float* tmpbuffer, __ubuf__ float* sumTmpbuffer, const WelfordFinalizePara& para)
{
    // K is actually abLength, which needs to be aligned with 32 bytes.
    uint32_t K = para.headCountLength + para.tailCountLength;

    uint32_t sregLower = static_cast<uint32_t>(Internal::WELFORD_B32_VF_LEN);
    uint16_t repeat = static_cast<uint16_t>(CeilDivision(K, sregLower));

    for (uint16_t m = 0; m < 1; m++) {
        WelfordFinalizeWithCountsOutMeanVF<isReuseSource, config>(
            outMean, counts, inMean, inVar, tmpbuffer, K, sregLower, repeat, m);
        uint32_t k = CalculateMainBlock(K);
        uint32_t kOverflow = k < K ? (k << 1) : k;
        BinaryReduceSum(
            outMean + m, tmpbuffer, sumTmpbuffer, K, k, 1.0f / kOverflow, para.rRec * kOverflow,
            para.rRecWithCorrection * kOverflow);
        WelfordFinalizeWithCountsOutVarVF<isReuseSource, config>(
            outMean, counts, inMean, inVar, tmpbuffer, K, sregLower, repeat, m);
        BinaryReduceSum<config.isCorrection>(
            outVar + m, tmpbuffer, sumTmpbuffer, K, k, 1.0f / kOverflow, para.rRec * kOverflow,
            para.rRecWithCorrection * kOverflow);
    }
}

template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
__simd_vf__ inline void WelfordFinalizeForB32RnVF(
    __ubuf__ float* outMean, __ubuf__ float* inMean, __ubuf__ float* inVar, __ubuf__ float* tmpbuffer,
    const WelfordFinalizePara para, uint16_t m)
{
    Reg::RegTensor<float> outmeanReg;
    Reg::RegTensor<float> inMeanReg;
    Reg::RegTensor<float> invarReg;
    Reg::RegTensor<float> outVarreg;
    Reg::RegTensor<float> tmpVreg;
    uint32_t K = para.headCountLength;
    uint32_t sregLower = static_cast<uint32_t>(Internal::WELFORD_B32_VF_LEN);
    uint16_t repeat = static_cast<uint16_t>(CeilDivision(K, sregLower));

    Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(outmeanReg, outMean + m);
    uint32_t sreg = static_cast<uint32_t>(K);
    float rn = static_cast<int32_t>(para.rnLength);
    for (uint16_t i = 0; i < repeat; ++i) {
        Reg::MaskReg preg = Reg::UpdateMask<uint32_t>(sreg);
        Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(inMeanReg, inMean + i * sregLower + m * para.headCountLength);
        Reg::Sub(inMeanReg, inMeanReg, outmeanReg, preg);
        Reg::Mul(inMeanReg, inMeanReg, inMeanReg, preg);
        Reg::Muls(inMeanReg, inMeanReg, rn, preg);
        Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(invarReg, inVar + i * sregLower + m * para.headCountLength);
        Reg::Add(outVarreg, invarReg, inMeanReg, preg);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM_B32>(tmpbuffer + i * sregLower, outVarreg, preg);
    }
}

template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
__simd_vf__ inline void WelfordFinalizeForB32OutMeanVF(
    __ubuf__ float* inMean, __ubuf__ float* tmpbuffer, const WelfordFinalizePara para, uint32_t sregLower, uint32_t K,
    uint16_t m, uint16_t hRepeat, uint16_t abRepeat)
{
    Reg::RegTensor<float> inMeanReg;
    Reg::RegTensor<float> headReg;
    Reg::RegTensor<float> tailReg;
    Reg::MaskReg preg;
    Duplicate(headReg, static_cast<float>((int32_t)para.headCount) / static_cast<float>((int32_t)para.tailCount));
    Duplicate(tailReg, static_cast<float>((int32_t)para.tailCount));
    uint32_t sreg = K;
    for (uint16_t i = 0; i < abRepeat; ++i) {
        preg = Reg::UpdateMask<uint32_t>(sreg);
        Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(inMeanReg, inMean + i * sregLower + m * K);
        Reg::Mul(inMeanReg, inMeanReg, tailReg, preg);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM_B32>(tmpbuffer + i * sregLower, inMeanReg, preg);
    }
    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    sreg = static_cast<uint32_t>(para.headCountLength);
    for (uint16_t i = 0; i < hRepeat; ++i) {
        preg = Reg::UpdateMask<uint32_t>(sreg);
        Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(inMeanReg, tmpbuffer + i * sregLower);
        Reg::Mul(inMeanReg, inMeanReg, headReg, preg);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM_B32>(tmpbuffer + i * sregLower, inMeanReg, preg);
    }
}

template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
__simd_vf__ inline void WelfordFinalizeForB32OutVarVF(
    __ubuf__ float* outMean, __ubuf__ float* inMean, __ubuf__ float* inVar, __ubuf__ float* tmpbuffer,
    const WelfordFinalizePara para, uint32_t sregLower, uint32_t K, uint16_t m, uint16_t hRepeat, uint16_t abRepeat)
{
    Reg::RegTensor<float> outmeanReg;
    Reg::RegTensor<float> inMeanReg;
    Reg::RegTensor<float> invarReg;
    Reg::RegTensor<float> outVarreg;
    Reg::RegTensor<float> tmpVreg;
    Reg::MaskReg preg;
    Reg::RegTensor<float> headReg;
    Reg::RegTensor<float> tailReg;
    Duplicate(headReg, static_cast<float>((int32_t)para.headCount) / static_cast<float>((int32_t)para.tailCount));
    Duplicate(tailReg, static_cast<float>((int32_t)para.tailCount));

    Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(outmeanReg, outMean + m);
    uint32_t sreg = static_cast<uint32_t>(K);
    for (uint16_t i = 0; i < abRepeat; ++i) {
        preg = Reg::UpdateMask<uint32_t>(sreg);
        Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(inMeanReg, inMean + i * sregLower + m * K);
        Reg::Sub(inMeanReg, inMeanReg, outmeanReg, preg);
        Reg::Mul(inMeanReg, inMeanReg, inMeanReg, preg);
        Reg::Mul(inMeanReg, inMeanReg, tailReg, preg);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM_B32>(tmpbuffer + i * sregLower, inMeanReg, preg);
    }
    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    sreg = static_cast<uint32_t>(para.headCountLength);
    for (uint16_t i = 0; i < hRepeat; ++i) {
        preg = Reg::UpdateMask<uint32_t>(sreg);
        Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(inMeanReg, tmpbuffer + i * sregLower);
        Reg::Mul(inMeanReg, inMeanReg, headReg, preg);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM_B32>(tmpbuffer + i * sregLower, inMeanReg, preg);
    }
    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    sreg = static_cast<uint32_t>(K);
    for (uint16_t i = 0; i < abRepeat; ++i) {
        preg = Reg::UpdateMask<uint32_t>(sreg);
        Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(tmpVreg, tmpbuffer + i * sregLower);
        Reg::LoadAlign<float, Reg::LoadDist::DIST_NORM>(outVarreg, inVar + i * sregLower + m * K);
        Reg::Add(outVarreg, outVarreg, tmpVreg, preg);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_NORM_B32>(tmpbuffer + i * sregLower, outVarreg, preg);
    }
}

template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
__aicore__ inline void WelfordFinalizeForB32(
    __ubuf__ float* outMean, __ubuf__ float* outVar, __ubuf__ float* inMean, __ubuf__ float* inVar,
    __ubuf__ float* tmpbuffer, __ubuf__ float* sumTmpbuffer, const WelfordFinalizePara& para)
{
    if (para.tailCount == 0 || para.tailCountLength == 0) {
        uint32_t K = para.abLength;
        uint32_t k = CalculateMainBlock(para.headCountLength);
        uint32_t kOverflow = k < K ? (k << 1) : k;
        for (uint16_t m = 0; m < 1; m++) {
            BinaryReduceSum(
                outMean + m, inMean + m * para.headCountLength, sumTmpbuffer, para.headCountLength, k, 1.0f / kOverflow,
                para.abRec * kOverflow, para.rRecWithCorrection * kOverflow);

            WelfordFinalizeForB32RnVF<isReuseSource, config>(outMean, inMean, inVar, tmpbuffer, para, m);
            BinaryReduceSum<config.isCorrection>(
                outVar + m, tmpbuffer, sumTmpbuffer, para.headCountLength, k, 1.0f / kOverflow, para.rRec * kOverflow,
                para.rRecWithCorrection * kOverflow);
        }
    } else {
        uint32_t K = para.abLength;
        uint32_t sregLower = static_cast<uint32_t>(Internal::WELFORD_B32_VF_LEN);
        uint16_t abRepeat = static_cast<uint16_t>(CeilDivision(K, sregLower));
        uint16_t hRepeat = static_cast<uint16_t>(CeilDivision(para.headCountLength, sregLower));
        uint32_t k = CalculateMainBlock(K);
        uint32_t kOverflow = k < K ? (k << 1) : k;
        for (uint16_t m = 0; m < 1; m++) {
            WelfordFinalizeForB32OutMeanVF<isReuseSource, config>(
                inMean, tmpbuffer, para, sregLower, K, m, hRepeat, abRepeat);
            BinaryReduceSum(
                outMean + m, tmpbuffer, sumTmpbuffer, K, k, 1.0f / kOverflow, para.rRec * kOverflow,
                para.rRecWithCorrection * kOverflow);
            WelfordFinalizeForB32OutVarVF<isReuseSource, config>(
                outMean, inMean, inVar, tmpbuffer, para, sregLower, K, m, hRepeat, abRepeat);
            BinaryReduceSum<config.isCorrection>(
                outVar + m, tmpbuffer, sumTmpbuffer, K, k, 1.0f / kOverflow, para.rRec * kOverflow,
                para.rRecWithCorrection * kOverflow);
        }
    }
}

__aicore__ inline void CheckWelfordFinalizePara(const WelfordFinalizePara& para)
{
    bool ans = para.abLength > 0 && (para.abLength * sizeof(float) % ONE_BLK_SIZE == 0);
    ASCENDC_ASSERT(ans, { KERNEL_LOG(KERNEL_ERROR, "abLength is %u, is not 32B aligned.", para.abLength); });
    ans = para.abLength == para.headCountLength + para.tailCountLength;
    ASCENDC_ASSERT(ans, {
        KERNEL_LOG(
            KERNEL_ERROR, "abLength is %u, not equal to the sum of headCountLength %u and tailCountLength %u.",
            para.abLength, para.headCountLength, para.tailCountLength);
    });
}

template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
__aicore__ inline void WelfordFinalizeImpl(
    const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance, const LocalTensor<float>& inputMean,
    const LocalTensor<float>& inputVariance, const LocalTensor<uint8_t>& sharedTmpBuffer, WelfordFinalizePara& para)
{
#if ASCENDC_CPU_DEBUG
    CheckWelfordFinalizePara(para);
#endif
    __ubuf__ float* inMean = (__ubuf__ float*)inputMean.GetPhyAddr();
    __ubuf__ float* inVar = (__ubuf__ float*)inputVariance.GetPhyAddr();
    __ubuf__ float* outMean = (__ubuf__ float*)outputMean.GetPhyAddr();
    __ubuf__ float* outVar = (__ubuf__ float*)outputVariance.GetPhyAddr();
    LocalTensor<float> stackBuffer = sharedTmpBuffer.ReinterpretCast<float>();
    __ubuf__ float* tmpbuffer1 = (__ubuf__ float*)stackBuffer.GetPhyAddr();
    __ubuf__ float* tmpbuffer2 = (__ubuf__ float*)stackBuffer[para.abLength].GetPhyAddr();

    WelfordFinalizeForB32<isReuseSource, config>(outMean, outVar, inMean, inVar, tmpbuffer1, tmpbuffer2, para);
}

template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
__aicore__ inline void WelfordFinalizeImpl(
    const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance, const LocalTensor<float>& inputMean,
    const LocalTensor<float>& inputVariance, WelfordFinalizePara& para)
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    WelfordFinalizeImpl<isReuseSource, config>(
        outputMean, outputVariance, inputMean, inputVariance, sharedTmpBuffer, para);
}

template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
__aicore__ inline void WelfordFinalizeImpl(
    const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance, const LocalTensor<float>& inputMean,
    const LocalTensor<float>& inputVariance, const LocalTensor<int32_t>& counts,
    const LocalTensor<uint8_t>& sharedTmpBuffer, WelfordFinalizePara& para)
{
#if ASCENDC_CPU_DEBUG
    CheckWelfordFinalizePara(para);
#endif
    LocalTensor<float> stackBuffer = sharedTmpBuffer.ReinterpretCast<float>();
    __ubuf__ int32_t* countsUb = (__ubuf__ int32_t*)counts.GetPhyAddr();
    __ubuf__ float* inMean = (__ubuf__ float*)inputMean.GetPhyAddr();
    __ubuf__ float* inVar = (__ubuf__ float*)inputVariance.GetPhyAddr();
    __ubuf__ float* outMean = (__ubuf__ float*)outputMean.GetPhyAddr();
    __ubuf__ float* outVar = (__ubuf__ float*)outputVariance.GetPhyAddr();
    __ubuf__ float* tmpbuffer1 = (__ubuf__ float*)stackBuffer.GetPhyAddr();
    __ubuf__ float* tmpbuffer2 = (__ubuf__ float*)stackBuffer[para.abLength].GetPhyAddr();

    WelfordFinalizeWithCounts<isReuseSource, config>(
        outMean, outVar, countsUb, inMean, inVar, tmpbuffer1, tmpbuffer2, para);
}

template <bool isReuseSource = false, const WelfordFinalizeConfig& config = WFFINALIZE_DEFAULT_CFG>
__aicore__ inline void WelfordFinalizeImpl(
    const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance, const LocalTensor<float>& inputMean,
    const LocalTensor<float>& inputVariance, const LocalTensor<int32_t>& counts, WelfordFinalizePara& para)
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    WelfordFinalizeImpl<isReuseSource, config>(
        outputMean, outputVariance, inputMean, inputVariance, counts, sharedTmpBuffer, para);
}
} // namespace AscendC
#endif // IMPL_NORMALIZATION_WELFORDFINALIZE_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_WELFORDFINALIZE_WELFORDFINALIZE_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_WELFORDFINALIZE_WELFORDFINALIZE_C310_IMPL_H__
#endif
