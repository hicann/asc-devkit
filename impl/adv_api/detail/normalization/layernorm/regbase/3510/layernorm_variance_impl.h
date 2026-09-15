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
 * \file layernorm_variance_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/normalization/layernorm/regbase/3510/layernorm_variance_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/layernorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORM_REGBASE_C310_LAYERNORM_VARIANCE_IMPL_H__
#endif
#ifndef IMPL_NORMALIZATION_LAYERNORM_LAYERNORM_VARIANCE_IMPL_H
#define IMPL_NORMALIZATION_LAYERNORM_LAYERNORM_VARIANCE_IMPL_H

#include "../../../../../../../include/basic_api/kernel_tensor.h"
#include "kernel_tiling/kernel_tiling.h"

namespace AscendC {
namespace Internal {
constexpr int32_t oneRegSize = GetVecLen() / sizeof(float);
constexpr Reg::CastTrait float2HalfCastTrait = {
    Reg::RegLayout::ZERO, Reg::SatMode::NO_SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};

struct LayerNormInternalPara {
    uint32_t hRepeatTimes;
    uint32_t hTailSize;
    uint32_t hRepeatCtrl;
    uint32_t hTailCtrl;
    uint32_t hTailOffset;
    uint32_t hDim;
};

template <typename T>
__aicore__ inline void GetLayerNormInternalPara(LayerNormInternalPara& para, const LayerNormTiling& tiling)
{
    para.hRepeatTimes = tiling.hLength / static_cast<uint32_t>(oneRegSize);
    para.hTailSize = tiling.hLength % oneRegSize;
    para.hDim = tiling.hLength;
    para.hRepeatCtrl = 1;
    para.hTailCtrl = 1;
    para.hTailOffset = para.hRepeatTimes * oneRegSize;
    if (para.hRepeatTimes == 0) {
        para.hRepeatCtrl = 0;
    }
    if (para.hTailSize == 0) {
        para.hTailCtrl = 0;
    }
}

template <typename T>
__simd_callee__ inline void CopyInFloatData(Reg::RegTensor<float>& reg, __ubuf__ T* ub, Reg::MaskReg& hFloatAllMask)
{
    if constexpr (SupportType<T, half>()) {
        Reg::RegTensor<T> oriInputH;
        Reg::LoadAlign<T, Reg::LoadDist::DIST_US_B16>(oriInputH, ub);
        Reg::Cast<float, T, layoutZMrgZ>(reg, oriInputH, hFloatAllMask);
    } else {
        Reg::LoadAlign(reg, ub);
    }
}
} // namespace Internal

template <typename T>
__simd_callee__ inline void CalcHMean(
    Reg::RegTensor<float>& outputMean, __ubuf__ T* inputX, Internal::LayerNormInternalPara& para)
{
    Reg::RegTensor<float> hDim;
    Reg::Duplicate(hDim, para.hDim);
    Reg::RegTensor<float> sumResultH;
    Reg::Duplicate(sumResultH, 0);
    Reg::MaskReg hFloatAllMask = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    uint32_t hTailSizeForMask = static_cast<uint32_t>(para.hTailSize);
    Reg::MaskReg hTailFloatMask = Reg::UpdateMask<float>(hTailSizeForMask);

    for (uint32_t repeat = 0; repeat < para.hRepeatCtrl; ++repeat) {
        // Copy first block to sumResultH.
        Internal::CopyInFloatData(sumResultH, inputX, hFloatAllMask);
        // Calc x/H in first block
        Reg::Div(sumResultH, sumResultH, hDim, hFloatAllMask);

        for (uint32_t i = 1; i < para.hRepeatTimes; ++i) {
            Reg::RegTensor<float> inputMeanTempReg;
            // Copy new block to inputMeanTempReg.
            Internal::CopyInFloatData(inputMeanTempReg, inputX + i * Internal::oneRegSize, hFloatAllMask);
            // Calc x/H in new block
            Reg::Div(inputMeanTempReg, inputMeanTempReg, hDim, hFloatAllMask);
            // Accumulate new data onto sumResultH
            Reg::Add(sumResultH, sumResultH, inputMeanTempReg, hFloatAllMask);
        }
    }

    for (uint32_t tail = 0; tail < para.hTailCtrl; ++tail) {
        Reg::RegTensor<float> inputMeanTempReg;
        // Copy tail block to inputMeanTempReg.
        Internal::CopyInFloatData(inputMeanTempReg, inputX + para.hTailOffset, hTailFloatMask);

        // Calc x/H in tail block
        Reg::Div(inputMeanTempReg, inputMeanTempReg, hDim, hTailFloatMask);
        // Accumulate tail data onto sumResultH
        Reg::Add(sumResultH, sumResultH, inputMeanTempReg, hFloatAllMask);
    }
    Reg::ReduceSum(outputMean, sumResultH, hFloatAllMask);
}

template <typename T>
__simd_callee__ inline void CalcHVariance(
    Reg::RegTensor<float>& outputVariance, Reg::RegTensor<float>& meanReg, __ubuf__ T* inputX,
    Internal::LayerNormInternalPara& para)
{
    Reg::RegTensor<float> sumVarianceResultH;
    Reg::Duplicate(sumVarianceResultH, 0);
    Reg::RegTensor<float> hDim;
    Reg::Duplicate(hDim, para.hDim);

    uint32_t hTailSizeForMask = static_cast<uint32_t>(para.hTailSize);
    Reg::MaskReg hTailFloatMask = Reg::UpdateMask<float>(hTailSizeForMask);
    Reg::MaskReg hFloatAllMask = Reg::CreateMask<float, Reg::MaskPattern::ALL>();

    for (uint32_t repeat = 0; repeat < para.hRepeatCtrl; ++repeat) {
        // Copy first block to sumVarianceResultH.
        Internal::CopyInFloatData(sumVarianceResultH, inputX, hFloatAllMask);

        // Calc x - mean in first block
        Reg::Sub(sumVarianceResultH, sumVarianceResultH, meanReg, hFloatAllMask);
        // Calc (x - mean)^2 in first block
        Reg::Mul(sumVarianceResultH, sumVarianceResultH, sumVarianceResultH, hFloatAllMask);
        // Calc (x - mean)^2 / H in first block
        Reg::Div(sumVarianceResultH, sumVarianceResultH, hDim, hFloatAllMask);

        for (uint32_t i = 1; i < para.hRepeatTimes; ++i) {
            Reg::RegTensor<float> inputVarianceReg;
            // Copy new block to inputVarianceReg.
            Internal::CopyInFloatData(inputVarianceReg, inputX + i * Internal::oneRegSize, hFloatAllMask);

            // Calc x - mean in new block
            Reg::Sub(inputVarianceReg, inputVarianceReg, meanReg, hFloatAllMask);
            // Calc (x - mean)^2 in new block
            Reg::Mul(inputVarianceReg, inputVarianceReg, inputVarianceReg, hFloatAllMask);
            // Calc (x - mean)^2 / H in new block
            Reg::Div(inputVarianceReg, inputVarianceReg, hDim, hFloatAllMask);
            // Accumulate new data onto sumVarianceResultH
            Reg::Add(sumVarianceResultH, sumVarianceResultH, inputVarianceReg, hFloatAllMask);
        }
    }

    for (uint32_t tail = 0; tail < para.hTailCtrl; ++tail) {
        Reg::RegTensor<float> inputVarianceReg;
        // Copy tail block to inputVarianceReg.
        Internal::CopyInFloatData(inputVarianceReg, inputX + para.hTailOffset, hTailFloatMask);

        // Calc x - mean in tail block
        Reg::Sub(inputVarianceReg, inputVarianceReg, meanReg, hTailFloatMask);
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
    __ubuf__ T* gamma, __ubuf__ T* beta, Reg::RegTensor<float>& sdReg, Reg::MaskReg& hFloatMask)
{
    Reg::RegTensor<float> resultH;

    if constexpr (SupportType<T, half>()) {
        Reg::RegTensor<T> oriInputH;
        Reg::LoadAlign<T, Reg::LoadDist::DIST_US_B16>(oriInputH, inputX);
        Reg::Cast<float, T, layoutZMrgZ>(resultH, oriInputH, hFloatMask);

        // Calc x - mean in first block.
        Reg::Sub(resultH, resultH, meanReg, hFloatMask);
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
        Reg::Cast<T, float, Internal::float2HalfCastTrait>(oriOutputH, resultH, hFloatMask);
        Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(output, oriOutputH, hFloatMask);
    } else {
        Reg::LoadAlign(resultH, inputX);
        // Calc x - mean in first block.
        Reg::Sub(resultH, resultH, meanReg, hFloatMask);
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
    __ubuf__ T* gamma, __ubuf__ T* beta, const T epsilon, Internal::LayerNormInternalPara& para)
{
    Reg::MaskReg hFloatAllMask = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::RegTensor<float> sdReg; // The standard deviation.

    // Calc variance + epsilon.
    Reg::Adds(sdReg, varianceReg, epsilon, hFloatAllMask);
    // Calc (variance + epsilon)^(1/2).
    Reg::Sqrt(sdReg, sdReg, hFloatAllMask);

    for (uint32_t i = 0; i < para.hRepeatTimes; ++i) {
        CalcHSingleBlockOutPut(
            output + i * Internal::oneRegSize, meanReg, varianceReg, inputX + i * Internal::oneRegSize,
            gamma + i * Internal::oneRegSize, beta + i * Internal::oneRegSize, sdReg, hFloatAllMask);
    }

    for (uint32_t tail = 0; tail < para.hTailCtrl; ++tail) {
        uint32_t hTailSizeForMask = static_cast<uint32_t>(para.hTailSize);
        Reg::MaskReg hTailFloatMask = Reg::UpdateMask<float>(hTailSizeForMask);
        CalcHSingleBlockOutPut(
            output + para.hTailOffset, meanReg, varianceReg, inputX + para.hTailOffset, gamma + para.hTailOffset,
            beta + para.hTailOffset, sdReg, hTailFloatMask);
    }
}

template <typename T>
__simd_vf__ inline void LayerNormImplVf(
    __ubuf__ T* output, __ubuf__ T* outputMean, __ubuf__ T* outputVariance, __ubuf__ T* inputX, __ubuf__ T* gamma,
    __ubuf__ T* beta, const T epsilon, Internal::LayerNormInternalPara para, LayerNormTiling tiling)
{
    Reg::MaskReg floatLowestMask = Reg::CreateMask<float, Reg::MaskPattern::VL1>();
    Reg::MaskReg srcLowestMask = Reg::CreateMask<T, Reg::MaskPattern::VL1>();
    Reg::MaskReg hFloatAllMask = Reg::CreateMask<float, Reg::MaskPattern::ALL>();

    uint32_t bLength = tiling.bLength;
    uint32_t sLength = tiling.sLength;
    uint32_t hLength = tiling.hLength;
    uint32_t bTotalLength = sLength * hLength;

    for (uint32_t bIdx = 0; bIdx < bLength; ++bIdx) {
        for (uint32_t sIdx = 0; sIdx < sLength; ++sIdx) {
            Reg::RegTensor<float> outputMeanReg;
            CalcHMean(outputMeanReg, inputX + bIdx * bTotalLength + sIdx * hLength, para);
            Reg::RegTensor<float> meanRegForNextCalc;
            Reg::Duplicate(meanRegForNextCalc, outputMeanReg, hFloatAllMask);

            if constexpr (SupportType<T, half>()) {
                Reg::RegTensor<T> oriTypeOutputMean;
                Reg::Cast<T, float, Internal::float2HalfCastTrait>(oriTypeOutputMean, outputMeanReg, floatLowestMask);
                Reg::StoreAlign<T, Reg::StoreDist::DIST_FIRST_ELEMENT_B16>(
                    outputMean + bIdx * sLength + sIdx, oriTypeOutputMean, srcLowestMask);
            } else {
                Reg::StoreAlign<T, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>(
                    outputMean + bIdx * sLength + sIdx, outputMeanReg, floatLowestMask);
            }

            Reg::RegTensor<float> outputVarianceReg;
            CalcHVariance(outputVarianceReg, meanRegForNextCalc, inputX + bIdx * bTotalLength + sIdx * hLength, para);
            Reg::RegTensor<float> varianceRegNextCalc;
            Reg::Duplicate(varianceRegNextCalc, outputVarianceReg, hFloatAllMask);

            if constexpr (SupportType<T, half>()) {
                Reg::RegTensor<T> oriTypeOutputVariance;
                Reg::Cast<T, float, Internal::float2HalfCastTrait>(
                    oriTypeOutputVariance, outputVarianceReg, floatLowestMask);
                Reg::StoreAlign<T, Reg::StoreDist::DIST_FIRST_ELEMENT_B16>(
                    outputVariance + bIdx * sLength + sIdx, oriTypeOutputVariance, srcLowestMask);
            } else {
                Reg::StoreAlign<T, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>(
                    outputVariance + bIdx * sLength + sIdx, outputVarianceReg, floatLowestMask);
            }

            CalcHOutPut(
                output + bIdx * bTotalLength + sIdx * hLength, meanRegForNextCalc, varianceRegNextCalc,
                inputX + bIdx * bTotalLength + sIdx * hLength, gamma, beta, epsilon, para);
        }
    }
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void LayerNormImpl(
    const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance,
    const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, LayerNormTiling& tiling)
{
    static_assert(SupportType<T, half, float>(), "current data type is not supported on current device!");
    CHECK_FUNC_HIGHLEVEL_API(
        LayerNorm, (T), (output, outputMean, outputVariance, inputX, gamma, beta, sharedTmpBuffer, epsilon, tiling));
    Internal::LayerNormInternalPara para{};
    Internal::GetLayerNormInternalPara<T>(para, tiling);

    LayerNormImplVf<T>(
        (__ubuf__ T*)output.GetPhyAddr(), (__ubuf__ T*)outputMean.GetPhyAddr(),
        (__ubuf__ T*)outputVariance.GetPhyAddr(), (__ubuf__ T*)inputX.GetPhyAddr(), (__ubuf__ T*)gamma.GetPhyAddr(),
        (__ubuf__ T*)beta.GetPhyAddr(), epsilon, para, tiling);
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void LayerNormImpl(
    const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance,
    const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta, const T epsilon,
    LayerNormTiling& tiling)
{
    static_assert(SupportType<T, half, float>(), "current data type is not supported on current device!");
    const LocalTensor<uint8_t> sharedTmpBuffer; // Not used, no need to alloc memory.
    CHECK_FUNC_HIGHLEVEL_API(
        LayerNorm, (T), (output, outputMean, outputVariance, inputX, gamma, beta, sharedTmpBuffer, epsilon, tiling));
    Internal::LayerNormInternalPara para{};
    Internal::GetLayerNormInternalPara<T>(para, tiling);

    LayerNormImplVf<T>(
        (__ubuf__ T*)output.GetPhyAddr(), (__ubuf__ T*)outputMean.GetPhyAddr(),
        (__ubuf__ T*)outputVariance.GetPhyAddr(), (__ubuf__ T*)inputX.GetPhyAddr(), (__ubuf__ T*)gamma.GetPhyAddr(),
        (__ubuf__ T*)beta.GetPhyAddr(), epsilon, para, tiling);
}

} // namespace AscendC
#endif // IMPL_NORMALIZATION_LAYERNORM_LAYERNORM_VARIANCE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORM_REGBASE_C310_LAYERNORM_VARIANCE_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORM_REGBASE_C310_LAYERNORM_VARIANCE_IMPL_H__
#endif
