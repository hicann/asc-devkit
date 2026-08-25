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
 * \file deepnorm_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/normalization/deepnorm/deepnorm_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/layernorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_DEEPNORM_DEEPNORM_C310_IMPL_H__
#endif
#ifndef IMPL_NORMALIZATION_DEEPNORM_DEEPNORM_C310_IMPL_H
#define IMPL_NORMALIZATION_DEEPNORM_DEEPNORM_C310_IMPL_H

#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "kernel_tiling/kernel_tiling.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/normalization/deepnorm/deepnorm_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
namespace DeepNormAPI {

constexpr uint16_t oneRepSize = GetVecLen() / sizeof(float);

namespace Internal {
struct DeepnormPara {
    uint16_t hRepeatTimes;
    uint16_t hTailSize;
    uint16_t hRepeatCtrl;
    uint16_t hTailCtrl;
    uint16_t hTailOffset;
    float hDim;
};

__aicore__ inline void GetDeepnormPara(DeepnormPara& para, DeepNormTiling& tiling)
{
    para.hRepeatTimes = tiling.hLength / static_cast<uint32_t>(oneRepSize);
    para.hTailSize = tiling.hLength % oneRepSize;
    para.hDim = static_cast<int32_t>(tiling.hLength);
    para.hRepeatCtrl = 1;
    para.hTailCtrl = 1;
    para.hTailOffset = para.hRepeatTimes * oneRepSize;
    if (para.hRepeatTimes == 0) {
        para.hRepeatCtrl = 0;
    }
    if (para.hTailSize == 0) {
        para.hTailCtrl = 0;
    }
}
} // namespace Internal

template <typename T>
__simd_callee__ inline void CopyInFloat(Reg::RegTensor<float>& reg, __ubuf__ T* ub, Reg::MaskReg& hFloatAllMask)
{
    if constexpr (IsSameType<T, half>::value) {
        Reg::RegTensor<T> oriInputH;
        Reg::LoadAlign<T, Reg::LoadDist::DIST_US_B16>(oriInputH, ub);
        Reg::Cast<float, T, layoutZMrgZ>(reg, oriInputH, hFloatAllMask);
    } else {
        Reg::LoadAlign(reg, ub);
    }
}

template <typename T>
__simd_callee__ inline void CalcHMean(
    Reg::RegTensor<float>& outputMean, __ubuf__ T* inputX, __ubuf__ T* gxLocal, const float alpha,
    Internal::DeepnormPara para)
{
    Reg::RegTensor<float> hDim, gxReg;
    Reg::Duplicate(hDim, para.hDim);
    Reg::RegTensor<float> sumResultH;
    Reg::Duplicate(sumResultH, 0);
    Reg::MaskReg hFloatAllMask = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    uint32_t hTailSizeForMask = static_cast<uint32_t>(para.hTailSize);
    Reg::MaskReg hTailFloatMask = Reg::UpdateMask<float>(hTailSizeForMask);
    for (uint16_t i = 0; i < para.hRepeatTimes; ++i) {
        Reg::RegTensor<float> inputMeanTempReg;
        // Copy new block to inputMeanTempReg.
        CopyInFloat(inputMeanTempReg, inputX + i * oneRepSize, hFloatAllMask);
        CopyInFloat(gxReg, gxLocal + i * oneRepSize, hFloatAllMask);
        Reg::Axpy(gxReg, inputMeanTempReg, alpha, hFloatAllMask);
        // Calc x/H in new block
        Reg::Div(inputMeanTempReg, gxReg, hDim, hFloatAllMask);
        // Accumulate new data onto sumResultH
        Reg::Add(sumResultH, sumResultH, inputMeanTempReg, hFloatAllMask);
    }
    for (uint16_t tail = 0; tail < para.hTailCtrl; ++tail) {
        Reg::RegTensor<float> inputMeanTempReg;
        // Copy tail block to inputMeanTempReg.
        CopyInFloat(inputMeanTempReg, inputX + para.hTailOffset, hTailFloatMask);
        CopyInFloat(gxReg, gxLocal + para.hTailOffset, hTailFloatMask);
        Reg::Axpy(gxReg, inputMeanTempReg, alpha, hTailFloatMask);
        // Calc x/H in tail block
        Reg::Div(inputMeanTempReg, gxReg, hDim, hTailFloatMask);
        // Accumulate tail data onto sumResultH
        Reg::Add(sumResultH, sumResultH, inputMeanTempReg, hFloatAllMask);
    }
    Reg::ReduceSum(outputMean, sumResultH, hFloatAllMask);
}

template <typename T>
__simd_callee__ inline void CalcHVariance(
    Reg::RegTensor<float>& outputVariance, Reg::RegTensor<float>& meanReg, __ubuf__ T* inputX, __ubuf__ T* gxLocal,
    const float alpha, Internal::DeepnormPara para)
{
    Reg::RegTensor<float> sumVarianceResultH, gxReg;
    Reg::RegTensor<float> hDim;
    Reg::Duplicate(hDim, para.hDim);
    Reg::Duplicate(sumVarianceResultH, 0);
    uint32_t hTailSizeForMask = static_cast<uint32_t>(para.hTailSize);
    Reg::MaskReg hTailFloatMask = Reg::UpdateMask<float>(hTailSizeForMask);
    Reg::MaskReg hFloatAllMask = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::Duplicate(sumVarianceResultH, 0);
    for (uint16_t i = 0; i < para.hRepeatTimes; ++i) {
        Reg::RegTensor<float> inputVarianceReg;
        // Copy new block to inputVarianceReg.
        CopyInFloat(inputVarianceReg, inputX + i * oneRepSize, hFloatAllMask);
        CopyInFloat(gxReg, gxLocal + i * oneRepSize, hFloatAllMask);
        Reg::Axpy(gxReg, inputVarianceReg, alpha, hFloatAllMask);
        // Calc x - mean in new block
        Reg::Sub(inputVarianceReg, gxReg, meanReg, hFloatAllMask);
        // Calc (x - mean)^2 in new block
        Reg::Mul(inputVarianceReg, inputVarianceReg, inputVarianceReg, hFloatAllMask);
        // Calc (x - mean)^2 / H in new block
        Reg::Div(inputVarianceReg, inputVarianceReg, hDim, hFloatAllMask);
        // Accumulate new data onto sumVarianceResultH
        Reg::Add(sumVarianceResultH, sumVarianceResultH, inputVarianceReg, hFloatAllMask);
    }
    for (uint16_t tail = 0; tail < para.hTailCtrl; ++tail) {
        Reg::RegTensor<float> inputVarianceReg;
        // Copy tail block to inputVarianceReg.
        CopyInFloat(inputVarianceReg, inputX + para.hTailOffset, hTailFloatMask);
        CopyInFloat(gxReg, gxLocal + para.hTailOffset, hTailFloatMask);
        Reg::Axpy(gxReg, inputVarianceReg, alpha, hTailFloatMask);
        // Calc x - mean in tail block
        Reg::Sub(inputVarianceReg, gxReg, meanReg, hTailFloatMask);
        // Calc (x - mean)^2 in tail block
        Reg::Mul(inputVarianceReg, inputVarianceReg, inputVarianceReg, hTailFloatMask);
        // Calc (x - mean)^2 / H in tail block
        Reg::Div(inputVarianceReg, inputVarianceReg, hDim, hTailFloatMask);
        // Accumulate new data onto sumVarianceResultH
        Reg::Add(sumVarianceResultH, sumVarianceResultH, inputVarianceReg, hFloatAllMask);
    }
    Reg::ReduceSum(outputVariance, sumVarianceResultH, hFloatAllMask);
}

template <typename T>
__simd_callee__ inline void CalcHSingleBlockOutPut(
    __ubuf__ T* output, Reg::RegTensor<float>& meanReg, Reg::RegTensor<float>& varianceReg, __ubuf__ T* inputX,
    __ubuf__ T* gxLocal, const float alpha, __ubuf__ T* gamma, __ubuf__ T* beta, Reg::RegTensor<float>& sdReg,
    Reg::MaskReg& hFloatMask)
{
    Reg::RegTensor<float> resultH, gxReg;
    if constexpr (SupportType<T, half>()) {
        Reg::RegTensor<T> oriInputH;
        Reg::LoadAlign<T, Reg::LoadDist::DIST_US_B16>(oriInputH, inputX);
        Reg::Cast<float, T, layoutZMrgZ>(resultH, oriInputH, hFloatMask);
        Reg::LoadAlign<T, Reg::LoadDist::DIST_US_B16>(oriInputH, gxLocal);
        Reg::Cast<float, T, layoutZMrgZ>(gxReg, oriInputH, hFloatMask);
        Reg::Axpy(gxReg, resultH, alpha, hFloatMask);
        // Calc x - mean in first block.
        Reg::Sub(resultH, gxReg, meanReg, hFloatMask);
        // Calc (x - mean) / sdReg in first block.
        Reg::Div(resultH, resultH, sdReg, hFloatMask);
        Reg::RegTensor<T> oriGammaH;
        Reg::RegTensor<float> gammaReg;
        Reg::LoadAlign<T, Reg::LoadDist::DIST_US_B16>(oriGammaH, gamma);
        Reg::Cast<float, T, layoutZMrgZ>(gammaReg, oriGammaH, hFloatMask);
        // Calc (x - mean) / sdReg * gamma in first block.
        Reg::Mul(resultH, resultH, gammaReg, hFloatMask);
        Reg::RegTensor<T> oriBetaH;
        Reg::RegTensor<float> betaReg;
        Reg::LoadAlign<T, Reg::LoadDist::DIST_US_B16>(oriBetaH, beta);
        Reg::Cast<float, T, layoutZMrgZ>(betaReg, oriBetaH, hFloatMask);
        // Calc (x - mean) * sdReg * gamma + in first block.
        Reg::Add(resultH, resultH, betaReg, hFloatMask);
        Reg::RegTensor<T> oriOutputH;
        Reg::Cast<T, float, LayoutZMrgZRndRSatNS>(oriOutputH, resultH, hFloatMask);
        Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(output, oriOutputH, hFloatMask);
    } else {
        Reg::LoadAlign(resultH, inputX);
        Reg::LoadAlign(gxReg, gxLocal);
        Reg::Axpy(gxReg, resultH, alpha, hFloatMask);
        // Calc x - mean in first block.
        Reg::Sub(resultH, gxReg, meanReg, hFloatMask);
        // Calc (x - mean) / sdReg in first block.
        Reg::Div(resultH, resultH, sdReg, hFloatMask);
        Reg::RegTensor<float> gammaReg;
        Reg::LoadAlign(gammaReg, gamma);
        // Calc (x - mean) * sdReg * gamma in first block.
        Reg::Mul(resultH, resultH, gammaReg, hFloatMask);
        Reg::RegTensor<float> betaReg;
        Reg::LoadAlign(betaReg, beta);
        // Calc (x - mean) * sdReg * gamma + in first block.
        Reg::Add(resultH, resultH, betaReg, hFloatMask);
        Reg::StoreAlign(output, resultH, hFloatMask);
    }
}

template <typename T>
__simd_callee__ inline void CalcHOutPut(
    __ubuf__ T* output, Reg::RegTensor<float>& meanReg, Reg::RegTensor<float>& varianceReg, __ubuf__ T* inputX,
    __ubuf__ T* gxLocal, const float alpha, __ubuf__ T* gamma, __ubuf__ T* beta, const T epsilon,
    Internal::DeepnormPara para)
{
    Reg::MaskReg hFloatAllMask = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::RegTensor<float> sdReg; // The standard deviation.
    // Calc variance + epsilon.
    Reg::Adds(sdReg, varianceReg, epsilon, hFloatAllMask);
    // Calc (variance + epsilon)^(1/2).
    Reg::Sqrt(sdReg, sdReg, hFloatAllMask);
    for (uint16_t i = 0; i < para.hRepeatTimes; ++i) {
        CalcHSingleBlockOutPut(
            output + i * oneRepSize, meanReg, varianceReg, inputX + i * oneRepSize, gxLocal + i * oneRepSize, alpha,
            gamma + i * oneRepSize, beta + i * oneRepSize, sdReg, hFloatAllMask);
    }
    for (uint16_t tail = 0; tail < para.hTailCtrl; ++tail) {
        uint32_t hTailSizeForMask = static_cast<uint32_t>(para.hTailSize);
        Reg::MaskReg hTailFloatMask = Reg::UpdateMask<float>(hTailSizeForMask);
        CalcHSingleBlockOutPut(
            output + para.hTailOffset, meanReg, varianceReg, inputX + para.hTailOffset, gxLocal + para.hTailOffset,
            alpha, gamma + para.hTailOffset, beta + para.hTailOffset, sdReg, hTailFloatMask);
    }
}

template <typename T>
__simd_vf__ inline void DeepNormImplVfHalf(
    __ubuf__ T* output, __ubuf__ T* outputMean, __ubuf__ T* outputVariance, __ubuf__ T* inputX, __ubuf__ T* gxLocal,
    __ubuf__ T* gamma, __ubuf__ T* beta, const T epsilon, Internal::DeepnormPara para, DeepNormTiling tiling,
    const float alpha)
{
    Reg::MaskReg floatLowestMask = Reg::CreateMask<float, Reg::MaskPattern::VL1>();
    Reg::MaskReg srcLowestMask = Reg::CreateMask<T, Reg::MaskPattern::VL1>();
    Reg::MaskReg hFloatAllMask = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    uint16_t bLength = tiling.bLength;
    uint16_t sLength = tiling.sLength;
    uint16_t hLength = tiling.hLength;
    for (uint16_t bsIdx = 0; bsIdx < bLength * sLength; ++bsIdx) {
        Reg::RegTensor<float> outputMeanReg;
        CalcHMean(outputMeanReg, inputX + bsIdx * hLength, gxLocal + bsIdx * hLength, alpha, para);
        Reg::RegTensor<float> meanRegForNextCalc;
        Reg::Duplicate(meanRegForNextCalc, outputMeanReg, hFloatAllMask);
        if constexpr (SupportType<T, half>()) {
            Reg::RegTensor<T> oriTypeOutputMean;
            Reg::Cast<T, float, LayoutZMrgZRndRSatNS>(oriTypeOutputMean, outputMeanReg, floatLowestMask);
            Reg::StoreAlign<T, Reg::StoreDist::DIST_FIRST_ELEMENT_B16>(
                outputMean + bsIdx, oriTypeOutputMean, srcLowestMask);
        } else {
            Reg::StoreAlign<T, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>(
                outputMean + bsIdx, outputMeanReg, floatLowestMask);
        }
        Reg::RegTensor<float> outputVarianceReg;
        CalcHVariance(
            outputVarianceReg, meanRegForNextCalc, inputX + bsIdx * hLength, gxLocal + bsIdx * hLength, alpha, para);
        Reg::RegTensor<float> varianceRegNextCalc;
        Reg::Duplicate(varianceRegNextCalc, outputVarianceReg, hFloatAllMask);
        if constexpr (SupportType<T, half>()) {
            Reg::RegTensor<T> oriTypeOutputVariance;
            Reg::Cast<T, float, LayoutZMrgZRndRSatNS>(oriTypeOutputVariance, outputVarianceReg, floatLowestMask);
            Reg::StoreAlign<T, Reg::StoreDist::DIST_FIRST_ELEMENT_B16>(
                outputVariance + bsIdx, oriTypeOutputVariance, srcLowestMask);
        } else {
            Reg::StoreAlign<T, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>(
                outputVariance + bsIdx, outputVarianceReg, floatLowestMask);
        }
        CalcHOutPut(
            output + bsIdx * hLength, meanRegForNextCalc, varianceRegNextCalc, inputX + bsIdx * hLength,
            gxLocal + bsIdx * hLength, alpha, gamma, beta, epsilon, para);
    }
}

template <typename T, bool isBasicBlock = false>
__aicore__ inline bool IsDeepNormParamValid(DeepNormTiling& tiling)
{
    ASCENDC_ASSERT((IsSameType<T, half>::value || IsSameType<T, float>::value), {
        KERNEL_LOG(KERNEL_ERROR, "DeepNorm only support data type: float/half");
    });
    ASCENDC_ASSERT(
        tiling.oneTmpSize > 0, { KERNEL_LOG(KERNEL_ERROR, "In DeepNorm, each tmpsize in sharedTmpBuffer must > 0!"); });
    const bool hDivBy64 = (tiling.hLength % 64 == 0) && (tiling.originalHLength % 64 == 0);
    const bool bsDivBy8 = ((tiling.bLength * tiling.sLength) % 8 == 0);
    if constexpr (isBasicBlock) {
        ASCENDC_ASSERT(hDivBy64 && bsDivBy8, {
            KERNEL_LOG(
                KERNEL_ERROR, "In DeepNorm, when isBasicBlock is true, input must have hLength %% 64 = 0, "
                              "originalHLength %% 64 = 0 and (bLength * sLength) %% 8 = 0 !");
        });
    }
    return true;
}

template <typename T>
__aicore__ inline void DeepNormImplHalf(
    __ubuf__ T* output, __ubuf__ T* outputMean, __ubuf__ T* outputVariance, __ubuf__ T* inputX, __ubuf__ T* gxLocal,
    __ubuf__ T* gamma, __ubuf__ T* beta, const T epsilon, Internal::DeepnormPara& para, DeepNormTiling& tiling,
    const T alpha)
{
    if (IsSameType<T, float>::value) {
        DeepNormImplVfHalf<T>(
            output, outputMean, outputVariance, inputX, gxLocal, gamma, beta, epsilon, para, tiling, alpha);
    } else {
        float alp = alpha;
        DeepNormImplVfHalf<T>(
            output, outputMean, outputVariance, inputX, gxLocal, gamma, beta, epsilon, para, tiling, alp);
    }
}

template <typename T, bool isReuseSrc, bool isBasicBlock>
__aicore__ inline void DeepNormImpl(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& meanLocal, const LocalTensor<T>& rstdLocal,
    const LocalTensor<T>& srcLocal, const LocalTensor<T>& gxLocal, const LocalTensor<T>& betaLocal,
    const LocalTensor<T>& gammaLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const T alpha, const T epsilon,
    DeepNormTiling& tiling)
{
    static_assert(SupportType<T, half, float>(), "template parameter (T) is not half or float");
    if constexpr (isReuseSrc) {
        static_assert(SupportType<T, float>(), "isReuseSrc is only supported for float on current device!");
    }
    CHECK_FUNC_HIGHLEVEL_API(
        DeepNorm, (T, isReuseSrc, isBasicBlock),
        (dstLocal, meanLocal, rstdLocal, srcLocal, gxLocal, betaLocal, gammaLocal, sharedTmpBuffer, alpha, epsilon,
         tiling));
    if (!DeepNormAPI::IsDeepNormParamValid<T, isBasicBlock>(tiling)) {
        return;
    }
    ASCENDC_ASSERT((sharedTmpBuffer.GetSize() > 0), { KERNEL_LOG(KERNEL_ERROR, "sharedTmpBuffer size must > 0!"); });
    Internal::DeepnormPara para;
    Internal::GetDeepnormPara(para, tiling);
    DeepNormImplHalf<T>(
        (__ubuf__ T*)dstLocal.GetPhyAddr(), (__ubuf__ T*)meanLocal.GetPhyAddr(), (__ubuf__ T*)rstdLocal.GetPhyAddr(),
        (__ubuf__ T*)srcLocal.GetPhyAddr(), (__ubuf__ T*)gxLocal.GetPhyAddr(), (__ubuf__ T*)gammaLocal.GetPhyAddr(),
        (__ubuf__ T*)betaLocal.GetPhyAddr(), epsilon, para, tiling, alpha);
}

template <typename T, bool isReuseSrc, bool isBasicBlock>
__aicore__ inline void DeepNormImpl(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& meanLocal, const LocalTensor<T>& rstdLocal,
    const LocalTensor<T>& srcLocal, const LocalTensor<T>& gxLocal, const LocalTensor<T>& betaLocal,
    const LocalTensor<T>& gammaLocal, const T alpha, const T epsilon, DeepNormTiling& tiling)
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    DeepNormImpl<T, isReuseSrc, isBasicBlock>(
        dstLocal, meanLocal, rstdLocal, srcLocal, gxLocal, betaLocal, gammaLocal, sharedTmpBuffer, alpha, epsilon,
        tiling);
}

} // namespace DeepNormAPI
} // namespace AscendC
#endif // IMPL_NORMALIZATION_DEEPNORM_DEEPNORM_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_DEEPNORM_DEEPNORM_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_DEEPNORM_DEEPNORM_C310_IMPL_H__
#endif
