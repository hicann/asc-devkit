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
 * \file layernorm_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/normalization/layernorm/layernorm_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/layernorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORM_LAYERNORM_C310_IMPL_H__
#endif
#ifndef IMPL_NORMALIZATION_LAYERNORM_LAYERNORM_C310_IMPL_H
#define IMPL_NORMALIZATION_LAYERNORM_LAYERNORM_C310_IMPL_H

#include "../../../../../include/adv_api/normalization/normalize.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/normalization/layernorm/layernorm_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"
#include "welford_3510_impl.h"
#include "layernorm_3510_utils.h"

namespace AscendC {
// Extracted helper for rLength <= 64, invoked via VF_CALL<>
template <typename T, bool isOutputVariance = true, bool isCorrection = false>
__simd_vf__ inline void ComputeMeanVariance64VF(
    __ubuf__ float* const meanUb, __ubuf__ float* const varianceUb, __ubuf__ T* const srcUb, const uint32_t aLength,
    const uint32_t rLengthWithPadding, const float k2Rec, const float k2RRec, const float rRecWithCorrection,
    const uint32_t count)
{
    Reg::RegTensor<float> src0Reg;
    Reg::RegTensor<float> src1Reg;
    Reg::RegTensor<float> dstReg;
    Reg::RegTensor<float> meanReg;
    Reg::RegTensor<float> varianceReg;

    uint32_t countTmp = count;
    Reg::MaskReg preg = Reg::UpdateMask<float>(countTmp);
    Reg::MaskReg pregFull = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::MaskReg pregOne = Reg::CreateMask<float, Reg::MaskPattern::VL1>();
    for (uint16_t i = 0; i < static_cast<uint16_t>(aLength); i++) {
        LoadDataWithT<T>(srcUb, src0Reg, preg, i * rLengthWithPadding);
        Muls(src1Reg, src0Reg, k2Rec, preg);
        Reg::Reduce<ReduceType::SUM>(dstReg, src1Reg, preg);
        Muls(meanReg, dstReg, k2RRec, pregOne);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>((meanUb + i), meanReg, pregOne);
        if constexpr (isOutputVariance) {
            Duplicate(meanReg, meanReg, pregFull);
            Sub(src0Reg, src0Reg, meanReg, pregFull);
            Mul(src0Reg, src0Reg, src0Reg, pregFull);
            Muls(src0Reg, src0Reg, k2Rec, pregFull);
            Reg::Reduce<ReduceType::SUM>(dstReg, src0Reg, preg);
            if constexpr (isCorrection) {
                Muls(varianceReg, dstReg, rRecWithCorrection, pregOne);
            } else {
                Muls(varianceReg, dstReg, k2RRec, pregOne);
            }
            Reg::StoreAlign<float, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>((varianceUb + i), varianceReg, pregOne);
        }
    }
}

// only support rLength <= 64
template <typename T, bool isOutputVariance = true, bool isCorrection = false>
__aicore__ inline void ComputeMeanVariance64(
    __ubuf__ float* const meanUb, __ubuf__ float* const varianceUb, __ubuf__ T* const srcUb, const uint32_t aLength,
    const uint32_t rLength, const uint32_t rLengthWithPadding, const float k2Rec, const float k2RRec,
    const float rRecWithCorrection)
{
    const uint32_t count = rLength;

    ComputeMeanVariance64VF<T, isOutputVariance, isCorrection>(
        meanUb, varianceUb, srcUb, aLength, rLengthWithPadding, k2Rec, k2RRec, rRecWithCorrection, count);
}

// Extracted helper for rLength in (64, 128], invoked via VF_CALL<>
template <typename T, bool isOutputVariance = true>
__simd_vf__ inline void ComputeMeanVariance128VF(
    __ubuf__ float* const meanUb, __ubuf__ float* const varianceUb, __ubuf__ T* const srcUb, const uint32_t aLength,
    const uint32_t rLengthWithPadding, const float k2Rec, const float k2RRec, const uint16_t sregLower, uint32_t count)
{
    Reg::RegTensor<float> src0Reg;
    Reg::RegTensor<float> src1Reg;
    Reg::RegTensor<float> src0CalReg;
    Reg::RegTensor<float> src1CalReg;
    Reg::RegTensor<float> dstReg;
    Reg::RegTensor<float> meanReg;
    Reg::RegTensor<float> varianceReg;

    Reg::MaskReg preg = Reg::UpdateMask<float>(count);
    Reg::MaskReg pregFull = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::MaskReg pregOne = Reg::CreateMask<float, Reg::MaskPattern::VL1>();
    for (uint16_t i = 0; i < static_cast<uint16_t>(aLength); i++) {
        LoadDataWithT<T>(
            srcUb, srcUb, src0Reg, src1Reg, pregFull, preg, i * rLengthWithPadding, i * rLengthWithPadding + sregLower);
        Muls(src0CalReg, src0Reg, k2Rec, pregFull);
        Muls(src1CalReg, src1Reg, k2Rec, preg);
        Add(dstReg, src0CalReg, src1CalReg, pregFull);
        Reg::Reduce<ReduceType::SUM>(dstReg, dstReg, pregFull);
        Muls(meanReg, dstReg, k2RRec, pregOne);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>((meanUb + i), meanReg, pregOne);
        if constexpr (isOutputVariance) {
            Duplicate(meanReg, meanReg, pregFull);
            Sub(src0CalReg, src0Reg, meanReg, pregFull);
            Mul(src0CalReg, src0CalReg, src0CalReg, pregFull);
            Muls(src0CalReg, src0CalReg, k2Rec, pregFull);

            Sub(src1CalReg, src1Reg, meanReg, preg);
            Mul(src1CalReg, src1CalReg, src1CalReg, preg);
            Muls(src1CalReg, src1CalReg, k2Rec, preg);

            Add(dstReg, src0CalReg, src1CalReg, pregFull);

            Reg::Reduce<ReduceType::SUM>(dstReg, dstReg, pregFull);
            Muls(varianceReg, dstReg, k2RRec, pregOne);
            Reg::StoreAlign<float, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>((varianceUb + i), varianceReg, pregOne);
        }
    }
}

// only support rLength <= 128
template <typename T, bool isOutputVariance = true>
__aicore__ inline void ComputeMeanVariance128(
    __ubuf__ float* const meanUb, __ubuf__ float* const varianceUb, __ubuf__ T* const srcUb, const uint32_t aLength,
    const uint32_t rLength, const uint32_t rLengthWithPadding, const float k2Rec, const float k2RRec,
    const uint16_t sregLower)
{
    const uint32_t count = rLength - sregLower;

    ComputeMeanVariance128VF<T, isOutputVariance>(
        meanUb, varianceUb, srcUb, aLength, rLengthWithPadding, k2Rec, k2RRec, sregLower, count);
}

// Helper for ComputeMeanVarianceUseY branching logic
template <typename T, uint16_t HalfAddTimes, bool isOutputVariance>
__simd_vf__ inline void ComputeMeanVarianceUseYVF(
    __ubuf__ T* const srcUb, __ubuf__ T* const workUbYOrigin, __ubuf__ float* const meanUb,
    __ubuf__ float* const varianceUb, const uint32_t aLength, const uint32_t rLengthWithPadding,
    const uint32_t rHeadLength, const uint32_t m, const uint16_t repeatTimes1, const uint16_t repeatTimes2,
    const uint16_t repeatTimes3, const uint32_t count2, const uint32_t mVL, const float k2Rec,
    const uint16_t halfAddRepeatTimes, const uint32_t lastCount, const float k2RRec, const uint16_t sregLower,
    const uint16_t dynamicHalfAddTimes)
{
    Reg::MaskReg pregFull = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::MaskReg pregOne = Reg::CreateMask<float, Reg::MaskPattern::VL1>();
    uint32_t lastCountTmp = lastCount;
    Reg::MaskReg pregLastCount = Reg::UpdateMask<float>(lastCountTmp);
    uint32_t count = count2;
    Reg::MaskReg preg2 = Reg::UpdateMask<float>(count);

    ComputeMeanUseY<T>(
        srcUb, workUbYOrigin, pregFull, pregOne, pregLastCount, preg2, aLength, rLengthWithPadding, rHeadLength, m,
        repeatTimes1, repeatTimes2, repeatTimes3, mVL, k2Rec, sregLower);
    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    ReduceWorkBufferAndStore<T, HalfAddTimes>(
        workUbYOrigin, meanUb, pregFull, pregOne, pregLastCount, aLength, rLengthWithPadding, halfAddRepeatTimes,
        lastCountTmp, k2RRec, sregLower, HalfAddTimes == 0 ? dynamicHalfAddTimes : 0);
    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    if constexpr (isOutputVariance) {
        ComputeVarianceUseY<T>(
            srcUb, workUbYOrigin, meanUb, pregFull, pregOne, pregLastCount, preg2, aLength, rLengthWithPadding,
            rHeadLength, m, repeatTimes1, repeatTimes2, repeatTimes3, mVL, k2Rec, sregLower);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
        ReduceWorkBufferAndStore<T, HalfAddTimes>(
            workUbYOrigin, varianceUb, pregFull, pregOne, pregLastCount, aLength, rLengthWithPadding,
            halfAddRepeatTimes, lastCountTmp, k2RRec, sregLower, HalfAddTimes == 0 ? dynamicHalfAddTimes : 0);
    }
}

template <typename T, bool isOutputVariance = true>
__aicore__ inline void ComputeMeanVarianceUseY(
    __ubuf__ float* const meanUb, __ubuf__ float* const varianceUb, __ubuf__ T* const srcUb,
    __ubuf__ T* const workUbYOrigin, const uint32_t k, const uint32_t aLength, const uint32_t rLength,
    const uint32_t rLengthWithPadding, const uint32_t rHeadLength, const float k2Rec, const float k2RRec,
    const uint16_t sregLower)
{
    const uint32_t m = rLength - rHeadLength;
    const uint16_t halfAddCount = CeilDivision(rHeadLength / 2, sregLower); // total count
    const uint16_t halfAddTimes = CeilDivision(halfAddCount, sregLower);

    const uint16_t varianceOffset = CeilDivision(aLength, sregLower);
    const uint16_t halfAddRepeatTimes = CalculateHalfAddRepeatTimes(halfAddTimes);
    int16_t lastCount = halfAddCount; // last vcadd times less than 64
    uint16_t repeatTimes1 = CeilDivision(m, sregLower) / 2;
    uint16_t count2 = m % (sregLower * 2);
    if (repeatTimes1 * sregLower * 2 > m) {
        count2 = 0;
    }
    uint16_t repeatTimes2 = CeilDivision(count2, sregLower);
    const uint32_t mainTailCount = rHeadLength - repeatTimes1 * 2 * sregLower - repeatTimes2 * sregLower * 2;

    const uint32_t mVL = (repeatTimes1 + repeatTimes2) * sregLower * 2;

    uint16_t repeatTimes3 = mainTailCount / sregLower / 2;
    uint32_t lastCountTmp = static_cast<uint32_t>(lastCount);
    if (halfAddTimes == 1) {
        ComputeMeanVarianceUseYVF<T, 1, isOutputVariance>(
            srcUb, workUbYOrigin, meanUb, varianceUb, aLength, rLengthWithPadding, rHeadLength, m, repeatTimes1,
            repeatTimes2, repeatTimes3, count2, mVL, k2Rec, halfAddRepeatTimes, lastCountTmp, k2RRec, sregLower, 0);
    } else if (halfAddTimes == 2) {
        ComputeMeanVarianceUseYVF<T, 2, isOutputVariance>(
            srcUb, workUbYOrigin, meanUb, varianceUb, aLength, rLengthWithPadding, rHeadLength, m, repeatTimes1,
            repeatTimes2, repeatTimes3, count2, mVL, k2Rec, halfAddRepeatTimes, lastCountTmp, k2RRec, sregLower, 0);
    } else if (halfAddTimes == 4) {
        ComputeMeanVarianceUseYVF<T, 4, isOutputVariance>(
            srcUb, workUbYOrigin, meanUb, varianceUb, aLength, rLengthWithPadding, rHeadLength, m, repeatTimes1,
            repeatTimes2, repeatTimes3, count2, mVL, k2Rec, halfAddRepeatTimes, lastCountTmp, k2RRec, sregLower, 0);
    } else {
        ComputeMeanVarianceUseYVF<T, 0, isOutputVariance>(
            srcUb, workUbYOrigin, meanUb, varianceUb, aLength, rLengthWithPadding, rHeadLength, m, repeatTimes1,
            repeatTimes2, repeatTimes3, count2, mVL, k2Rec, halfAddRepeatTimes, lastCountTmp, k2RRec, sregLower,
            halfAddTimes);
    }
}

template <typename U, typename T, const LayerNormConfig& config>
__aicore__ inline void LayerNormTypeChecks()
{
    static_assert(SupportType<T, half, float, bfloat16_t>(), "current data type is not supported on current device!");
    if constexpr (IsSameType<T, half>::value) {
        static_assert(SupportType<U, half, float>(), "current data type is not supported on current device!");
    } else if constexpr (IsSameType<T, bfloat16_t>::value) {
        static_assert(SupportType<U, bfloat16_t, float>(), "current data type is not supported on current device!");
    } else if constexpr (IsSameType<T, float>::value) {
        static_assert(SupportType<U, float>(), "current data type is not supported on current device!");
    }
    static_assert(config.isOnlyOutput == false, "current value is not supported on current device!");
}

// Helper function to dispatch mean and variance computation based on rLength
template <typename T>
__aicore__ inline void ComputeMeanVariance(
    const LocalTensor<float>& outputMean, const LocalTensor<float>& varianceLocal, const LocalTensor<T>& inputX,
    const LocalTensor<T>& output, const LayerNormPara& para, const LayerNormSeparateTiling& tiling,
    const uint16_t sregLower)
{
    if (tiling.rLength <= sregLower) {
        ComputeMeanVariance64(
            (__ubuf__ float*)outputMean.GetPhyAddr(), (__ubuf__ float*)varianceLocal.GetPhyAddr(),
            (__ubuf__ T*)inputX.GetPhyAddr(), para.aLength, tiling.rLength, para.rLengthWithPadding, tiling.k2Rec,
            tiling.k2RRec, tiling.k2RRec);
    } else if (tiling.rLength <= sregLower * 2) {
        ComputeMeanVariance128(
            (__ubuf__ float*)outputMean.GetPhyAddr(), (__ubuf__ float*)varianceLocal.GetPhyAddr(),
            (__ubuf__ T*)inputX.GetPhyAddr(), para.aLength, tiling.rLength, para.rLengthWithPadding, tiling.k2Rec,
            tiling.k2RRec, sregLower);
    } else {
        ComputeMeanVarianceUseY(
            (__ubuf__ float*)outputMean.GetPhyAddr(), (__ubuf__ float*)varianceLocal.GetPhyAddr(),
            (__ubuf__ T*)inputX.GetPhyAddr(), (__ubuf__ T*)output.GetPhyAddr(), tiling.oneTmpSize, para.aLength,
            tiling.rLength, para.rLengthWithPadding, tiling.rHeadLength, tiling.k2Rec, tiling.k2RRec, sregLower);
    }
}

template <typename U, typename T, bool isReuseSource = false, const LayerNormConfig& config = LNCFG_NORM>
__aicore__ inline void LayerNormImpl(
    const LocalTensor<T>& output, const LocalTensor<float>& outputMean, const LocalTensor<float>& output1,
    const LocalTensor<T>& inputX, const LocalTensor<U>& gamma, const LocalTensor<U>& beta, const float epsilon,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const LayerNormPara& para, const LayerNormSeparateTiling& tiling)
{
    CHECK_FUNC_HIGHLEVEL_API(
        LayerNorm, (U, T, isReuseSource, config),
        (output, outputMean, output1, inputX, gamma, beta, epsilon, sharedTmpBuffer, para, tiling));

    LayerNormTypeChecks<U, T, config>();

    constexpr uint16_t sregLower = (uint32_t)(GetVecLen() / sizeof(float)); // 64;

    LocalTensor<float> workLocalBegin = sharedTmpBuffer.ReinterpretCast<float>();
    LocalTensor<float> varianceLocal;
    if constexpr (config.isOutputRstd) {
        varianceLocal = workLocalBegin;
    } else {
        varianceLocal = output1;
    }

    uint32_t tempOffset = CeilDivision(para.aLength, 8) * 8;
    // prevent buffer overlaps, reserve space from tempOffset onwards
    LocalTensor<float> workLocal = workLocalBegin[tempOffset];

    ComputeMeanVariance<T>(outputMean, varianceLocal, inputX, output, para, tiling, sregLower);

    NormalizePara nlPara{para.aLength, tiling.rLength, para.rLengthWithPadding};

    LocalTensor<float> rstdLocal;
    if constexpr (config.isOutputRstd) {
        rstdLocal = output1;
    } else {
        rstdLocal = workLocalBegin[tempOffset];
    }

    if constexpr (!config.isNoBeta && !config.isNoGamma) {
        NormalizeImpl<U, T, false, NLCFG_NORM>(
            output, rstdLocal, outputMean, varianceLocal, inputX, gamma, beta, sharedTmpBuffer, epsilon, nlPara);
    } else if constexpr (!config.isNoBeta && config.isNoGamma) {
        NormalizeImpl<U, T, false, NLCFG_NOGAMMA>(
            output, rstdLocal, outputMean, varianceLocal, inputX, gamma, beta, sharedTmpBuffer, epsilon, nlPara);
    } else if constexpr (config.isNoBeta && !config.isNoGamma) {
        NormalizeImpl<U, T, false, NLCFG_NOBETA>(
            output, rstdLocal, outputMean, varianceLocal, inputX, gamma, beta, sharedTmpBuffer, epsilon, nlPara);
    } else if constexpr (config.isNoBeta && config.isNoGamma) {
        NormalizeImpl<U, T, false, NLCFG_NOOPT>(
            output, rstdLocal, outputMean, varianceLocal, inputX, gamma, beta, sharedTmpBuffer, epsilon, nlPara);
    }
}

template <typename U, typename T, bool isReuseSource = false, const LayerNormConfig& config = LNCFG_NORM>
__aicore__ inline void LayerNormImpl(
    const LocalTensor<T>& output, const LocalTensor<float>& outputMean, const LocalTensor<float>& output1,
    const LocalTensor<T>& inputX, const LocalTensor<U>& gamma, const LocalTensor<U>& beta, const float epsilon,
    const LayerNormPara& para, const LayerNormSeparateTiling& tiling)
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });

    LayerNormImpl<U, T, isReuseSource, config>(
        output, outputMean, output1, inputX, gamma, beta, epsilon, sharedTmpBuffer, para, tiling);
}
} // namespace AscendC
#endif // IMPL_NORMALIZATION_LAYERNORM_LAYERNORM_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORM_LAYERNORM_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORM_LAYERNORM_C310_IMPL_H__
#endif
