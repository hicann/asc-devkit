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
 * \file batchnorm_check_c310.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/normalization/batchnorm/batchnorm_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/layernorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_BATCHNORM_BATCHNORM_C310_IMPL_H__
#endif
#ifndef IMPL_NORMALIZATION_BATCHNORM_BATCHNORM_C310_IMPL_H
#define IMPL_NORMALIZATION_BATCHNORM_BATCHNORM_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "kernel_tiling/kernel_tiling.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/normalization/batchnorm/batchnorm_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
namespace BatchNormAPI {
constexpr int32_t oneRepSize = GetVecLen() / sizeof(float);

template <typename T>
__simd_callee__ inline void LoadDataWithT(
    __ubuf__ T* src, Reg::RegTensor<float>& dstReg, Reg::MaskReg& mask, uint32_t offset)
{
    if constexpr (IsSameType<T, half>::value) {
        Reg::RegTensor<T> srcOrigin;
        Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(srcOrigin, src + offset);
        Cast<float, T, layoutZMrgZ>(dstReg, srcOrigin, mask);
    } else {
        Reg::LoadAlign(dstReg, src + offset);
    }
}

template <typename T>
__simd_callee__ inline void LoadDataWithGammBeta(
    __ubuf__ T* src, Reg::RegTensor<float>& dstReg, Reg::MaskReg& mask, uint32_t offset)
{
    if constexpr (IsSameType<T, half>::value) {
        Reg::RegTensor<T> srcOrigin;
        Reg::LoadAlign<T, Reg::LoadDist::DIST_BRC_B16>(srcOrigin, src + offset);
        Cast<float, T, layoutZMrgZ>(dstReg, srcOrigin, mask);
    } else {
        Reg::LoadAlign<T, Reg::LoadDist::DIST_BRC_B32>(dstReg, src + offset);
    }
}

template <typename T>
__simd_callee__ inline void SaveDataWithT(
    __ubuf__ T* dst, Reg::RegTensor<float>& srcReg, Reg::MaskReg& mask, uint32_t offset)
{
    if constexpr (IsSameType<T, half>::value) {
        Reg::RegTensor<T> regT;
        Reg::Cast<T, float, LayoutZMrgZRndRSatNS>(regT, srcReg, mask);
        Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(dst + offset, regT, mask);
    } else {
        Reg::StoreAlign(dst + offset, srcReg, mask);
    }
}

template <typename T>
__simd_callee__ inline void ComputeOutputMean(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, uint32_t oriBLength, uint32_t featureLength, float firstDimValueBack)
{
    Reg::RegTensor<float> srcReg;
    Reg::RegTensor<float> dstReg;
    Reg::RegTensor<float> dstTailReg;
    uint16_t mainRepeatTime = static_cast<uint16_t>(featureLength / oneRepSize);
    uint32_t tailCount = featureLength % oneRepSize;
    uint16_t tailRepeatTime = static_cast<uint16_t>(CeilDivision(tailCount, oneRepSize));
    Reg::MaskReg maskFull = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::MaskReg maskOne = Reg::CreateMask<float, Reg::MaskPattern::VL1>();
    Reg::MaskReg maskReg = Reg::UpdateMask<float>(tailCount);
    for (uint16_t i = 0; i < mainRepeatTime; i++) {
        Reg::Duplicate(dstReg, static_cast<float>(0), maskFull);
        for (uint16_t bIdx = 0; bIdx < oriBLength; bIdx++) {
            LoadDataWithT(srcLocal, srcReg, maskFull, bIdx * featureLength + i * oneRepSize);
            // x / N
            Reg::Muls(srcReg, srcReg, firstDimValueBack, maskFull);
            // �?x / N)
            Reg::Add(dstReg, dstReg, srcReg, maskFull);
        }
        SaveDataWithT(dstLocal, dstReg, maskFull, i * oneRepSize);
    }
    for (uint16_t i = 0; i < tailRepeatTime; i++) {
        Reg::Duplicate(dstReg, static_cast<float>(0), maskFull);
        for (uint16_t bIdx = 0; bIdx < oriBLength; bIdx++) {
            LoadDataWithT(srcLocal, srcReg, maskReg, bIdx * featureLength + mainRepeatTime * oneRepSize);
            // x / N
            Reg::Muls(srcReg, srcReg, firstDimValueBack, maskReg);
            // �?x / N)
            Reg::Add(dstReg, dstReg, srcReg, maskReg);
        }
        SaveDataWithT(dstLocal, dstReg, maskReg, mainRepeatTime * oneRepSize);
    }
}

template <typename T>
__simd_callee__ inline void ComputeFloatMean(
    __ubuf__ float* dstLocal, __ubuf__ T* srcLocal, uint32_t oriBLength, uint32_t featureLength,
    float firstDimValueBack)
{
    Reg::RegTensor<float> srcReg;
    Reg::RegTensor<float> dstReg;
    Reg::RegTensor<float> dstTailReg;
    uint16_t mainRepeatTime = static_cast<uint16_t>(featureLength / oneRepSize);
    uint32_t tailCount = featureLength % oneRepSize;
    uint16_t tailRepeatTime = static_cast<uint16_t>(CeilDivision(tailCount, oneRepSize));
    Reg::MaskReg maskFull = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::MaskReg maskOne = Reg::CreateMask<float, Reg::MaskPattern::VL1>();
    Reg::MaskReg maskReg = Reg::UpdateMask<float>(tailCount);
    for (uint16_t i = 0; i < mainRepeatTime; i++) {
        Reg::Duplicate(dstReg, static_cast<float>(0), maskFull);
        for (uint16_t bIdx = 0; bIdx < oriBLength; bIdx++) {
            LoadDataWithT(srcLocal, srcReg, maskFull, bIdx * featureLength + i * oneRepSize);
            // x / N
            Reg::Muls(srcReg, srcReg, firstDimValueBack, maskFull);
            // �?x / N)
            Reg::Add(dstReg, dstReg, srcReg, maskFull);
        }
        Reg::StoreAlign(dstLocal + i * oneRepSize, dstReg, maskFull);
    }
    for (uint16_t i = 0; i < tailRepeatTime; i++) {
        Reg::Duplicate(dstReg, static_cast<float>(0), maskFull);
        for (uint16_t bIdx = 0; bIdx < oriBLength; bIdx++) {
            LoadDataWithT(srcLocal, srcReg, maskReg, bIdx * featureLength + mainRepeatTime * oneRepSize);
            // x / N
            Reg::Muls(srcReg, srcReg, firstDimValueBack, maskReg);
            // �?x / N)
            Reg::Add(dstReg, dstReg, srcReg, maskReg);
        }
        Reg::StoreAlign(dstLocal + mainRepeatTime * oneRepSize, dstReg, maskReg);
    }
}

template <typename T>
__simd_callee__ inline void ComputeOutputVariance(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ float* meanLocal, uint32_t oriBLength, uint32_t featureLength,
    float firstDimValueBack)
{
    Reg::RegTensor<float> srcReg;
    Reg::RegTensor<float> dstReg;
    Reg::RegTensor<float> meanReg;
    Reg::RegTensor<float> diffReg;
    Reg::RegTensor<float> sqrReg;
    Reg::RegTensor<float> dstTailReg;
    uint16_t mainRepeatTime = static_cast<uint16_t>(featureLength / oneRepSize);
    uint32_t tailCount = featureLength % oneRepSize;
    uint16_t tailRepeatTime = static_cast<uint16_t>(CeilDivision(tailCount, oneRepSize));
    Reg::MaskReg maskFull = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::MaskReg maskOne = Reg::CreateMask<float, Reg::MaskPattern::VL1>();
    Reg::MaskReg maskReg = Reg::UpdateMask<float>(tailCount);
    for (uint16_t i = 0; i < mainRepeatTime; i++) {
        Reg::Duplicate(dstReg, static_cast<float>(0), maskFull);
        Reg::LoadAlign(meanReg, meanLocal + i * oneRepSize);
        for (uint16_t bIdx = 0; bIdx < oriBLength; bIdx++) {
            LoadDataWithT(srcLocal, srcReg, maskFull, bIdx * featureLength + i * oneRepSize);
            // step 1: x - u
            Reg::Sub(diffReg, srcReg, meanReg, maskFull);
            // step 2: (x - u)²
            Reg::Mul(sqrReg, diffReg, diffReg, maskFull);
            // step 3: �?x - u)²
            Reg::Add(dstReg, dstReg, sqrReg, maskFull);
        }
        // step 4: �?x - u)² / N
        Reg::Muls(dstReg, dstReg, firstDimValueBack, maskFull);
        SaveDataWithT(dstLocal, dstReg, maskFull, i * oneRepSize);
    }
    for (uint16_t i = 0; i < tailRepeatTime; i++) {
        Reg::Duplicate(dstReg, static_cast<float>(0), maskFull);
        Reg::LoadAlign(meanReg, meanLocal + mainRepeatTime * oneRepSize);
        for (uint16_t bIdx = 0; bIdx < oriBLength; bIdx++) {
            LoadDataWithT(srcLocal, srcReg, maskReg, bIdx * featureLength + mainRepeatTime * oneRepSize);
            // step 1: x - u
            Reg::Sub(diffReg, srcReg, meanReg, maskReg);
            // step 2: (x - u)²
            Reg::Mul(sqrReg, diffReg, diffReg, maskReg);
            // step 3: �?x - u)²
            Reg::Add(dstReg, dstReg, sqrReg, maskReg);
        }
        // step 4: �?x - u)² / N
        Reg::Muls(dstReg, dstReg, firstDimValueBack, maskReg);
        SaveDataWithT(dstLocal, dstReg, maskReg, mainRepeatTime * oneRepSize);
    }
}

template <typename T>
__simd_callee__ inline void ComputeFloatVariance(
    __ubuf__ float* dstLocal, __ubuf__ T* srcLocal, __ubuf__ float* meanLocal, uint32_t oriBLength,
    uint32_t featureLength, float firstDimValueBack)
{
    Reg::RegTensor<float> srcReg;
    Reg::RegTensor<float> dstReg;
    Reg::RegTensor<float> meanReg;
    Reg::RegTensor<float> diffReg;
    Reg::RegTensor<float> sqrReg;
    Reg::RegTensor<float> dstTailReg;
    uint16_t mainRepeatTime = static_cast<uint16_t>(featureLength / oneRepSize);
    uint32_t tailCount = featureLength % oneRepSize;
    uint16_t tailRepeatTime = static_cast<uint16_t>(CeilDivision(tailCount, oneRepSize));
    Reg::MaskReg maskFull = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::MaskReg maskOne = Reg::CreateMask<float, Reg::MaskPattern::VL1>();
    Reg::MaskReg maskReg = Reg::UpdateMask<float>(tailCount);
    for (uint16_t i = 0; i < mainRepeatTime; i++) {
        Reg::Duplicate(dstReg, static_cast<float>(0), maskFull);
        Reg::LoadAlign(meanReg, meanLocal + i * oneRepSize);
        for (uint16_t bIdx = 0; bIdx < oriBLength; bIdx++) {
            LoadDataWithT(srcLocal, srcReg, maskFull, bIdx * featureLength + i * oneRepSize);
            // step 1: x - u
            Reg::Sub(diffReg, srcReg, meanReg, maskFull);
            // step 2: (x - u)²
            Reg::Mul(sqrReg, diffReg, diffReg, maskFull);
            // step 3: �?x - u)²
            Reg::Add(dstReg, dstReg, sqrReg, maskFull);
        }
        // step 4: �?x - u)² / N
        Reg::Muls(dstReg, dstReg, firstDimValueBack, maskFull);
        Reg::StoreAlign(dstLocal + i * oneRepSize, dstReg, maskFull);
    }
    for (uint16_t i = 0; i < tailRepeatTime; i++) {
        Reg::Duplicate(dstReg, static_cast<float>(0), maskFull);
        Reg::LoadAlign(meanReg, meanLocal + mainRepeatTime * oneRepSize);
        for (uint16_t bIdx = 0; bIdx < oriBLength; bIdx++) {
            LoadDataWithT(srcLocal, srcReg, maskReg, bIdx * featureLength + mainRepeatTime * oneRepSize);
            // step 1: x - u
            Reg::Sub(diffReg, srcReg, meanReg, maskReg);
            // step 2: (x - u)²
            Reg::Mul(sqrReg, diffReg, diffReg, maskReg);
            // step 3: �?x - u)²
            Reg::Add(dstReg, dstReg, sqrReg, maskReg);
        }
        // step 4: �?x - u)² / N
        Reg::Muls(dstReg, dstReg, firstDimValueBack, maskReg);
        Reg::StoreAlign(dstLocal + mainRepeatTime * oneRepSize, dstReg, maskReg);
    }
}

template <typename T>
__simd_callee__ inline void ComputeY(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ float* tmpMeanLocal, __ubuf__ float* tmpVarLocal,
    __ubuf__ T* gammaLocal, __ubuf__ T* betaLocal, uint32_t oriBLength, uint32_t featureLength, const float epsilon)
{
    constexpr float rsqrtExponent = -0.5;
    Reg::RegTensor<float> srcReg;
    Reg::RegTensor<float> meanReg;
    Reg::RegTensor<float> varReg;
    Reg::RegTensor<float> gammaReg;
    Reg::RegTensor<float> betaReg;
    Reg::RegTensor<float> diffReg;
    uint16_t mainRepeatTime = static_cast<uint16_t>(featureLength / oneRepSize);
    uint32_t tailCount = featureLength % oneRepSize;
    uint16_t tailRepeatTime = static_cast<uint16_t>(CeilDivision(tailCount, oneRepSize));
    static constexpr Reg::LnSpecificMode lnMode = {Reg::MaskMergeMode::ZEROING, LnAlgo::INTRINSIC};
    static constexpr Reg::ExpSpecificMode expMode = {Reg::MaskMergeMode::ZEROING, ExpAlgo::INTRINSIC};
    Reg::MaskReg maskFull = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::MaskReg maskReg = Reg::UpdateMask<float>(tailCount);
    for (uint16_t i = 0; i < mainRepeatTime; i++) {
        for (uint16_t bIdx = 0; bIdx < oriBLength; bIdx++) {
            LoadDataWithGammBeta(gammaLocal, gammaReg, maskFull, bIdx);
            LoadDataWithGammBeta(betaLocal, betaReg, maskFull, bIdx);
            Reg::LoadAlign(meanReg, tmpMeanLocal + i * oneRepSize);
            Reg::LoadAlign(varReg, tmpVarLocal + i * oneRepSize);
            LoadDataWithT(srcLocal, srcReg, maskFull, bIdx * featureLength + i * oneRepSize);
            // var + e
            Reg::Adds(varReg, varReg, epsilon, maskFull);
            // rsqrt: ln + muls + exp
            Reg::Ln<float, &lnMode>(varReg, varReg, maskFull);
            Reg::Muls(diffReg, varReg, rsqrtExponent, maskFull);
            Reg::Exp<float, &expMode>(varReg, diffReg, maskFull);
            // rsqrt * (x - mean)
            Reg::Sub(diffReg, srcReg, meanReg, maskFull);
            Reg::Mul(varReg, varReg, diffReg, maskFull);
            // res * gamm + beta
            Reg::Mul(varReg, varReg, gammaReg, maskFull);
            Reg::Add(varReg, varReg, betaReg, maskFull);
            SaveDataWithT(dstLocal, varReg, maskFull, bIdx * featureLength + i * oneRepSize);
        }
    }
    for (uint16_t i = 0; i < tailRepeatTime; i++) {
        for (uint16_t bIdx = 0; bIdx < oriBLength; bIdx++) {
            LoadDataWithGammBeta(gammaLocal, gammaReg, maskReg, bIdx);
            LoadDataWithGammBeta(betaLocal, betaReg, maskReg, bIdx);
            Reg::LoadAlign(meanReg, tmpMeanLocal + mainRepeatTime * oneRepSize);
            Reg::LoadAlign(varReg, tmpVarLocal + mainRepeatTime * oneRepSize);
            LoadDataWithT(srcLocal, srcReg, maskReg, bIdx * featureLength + mainRepeatTime * oneRepSize);
            // var + e
            Reg::Adds(varReg, varReg, epsilon, maskReg);
            // rsqrt: ln + muls + exp
            Reg::Ln<float, &lnMode>(varReg, varReg, maskReg);
            Reg::Muls(diffReg, varReg, rsqrtExponent, maskReg);
            Reg::Exp<float, &expMode>(varReg, diffReg, maskReg);
            // rsqrt * (x - mean)
            Reg::Sub(diffReg, srcReg, meanReg, maskReg);
            Reg::Mul(varReg, varReg, diffReg, maskReg);
            // res * gamm + beta
            Reg::Mul(varReg, varReg, gammaReg, maskReg);
            Reg::Add(varReg, varReg, betaReg, maskReg);
            SaveDataWithT(dstLocal, varReg, maskReg, bIdx * featureLength + mainRepeatTime * oneRepSize);
        }
    }
}

template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
__simd_vf__ inline void BatchNormImplVF(
    __ubuf__ T* output, __ubuf__ T* outputMean, __ubuf__ T* outputVariance, __ubuf__ T* inputX, __ubuf__ T* gamm,
    __ubuf__ T* beta, __ubuf__ float* tmpMeanLocal, __ubuf__ float* tmpVarLocal, const float epsilon,
    const BatchNormTiling tiling, uint32_t oriBLength, uint32_t featureLength, float firstDimValueBack)
{
    ComputeOutputMean(outputMean, inputX, oriBLength, featureLength, firstDimValueBack);
    ComputeFloatMean(tmpMeanLocal, inputX, oriBLength, featureLength, firstDimValueBack);
    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    ComputeOutputVariance(outputVariance, inputX, tmpMeanLocal, oriBLength, featureLength, firstDimValueBack);
    ComputeFloatVariance(tmpVarLocal, inputX, tmpMeanLocal, oriBLength, featureLength, firstDimValueBack);
    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    ComputeY(output, inputX, tmpMeanLocal, tmpVarLocal, gamm, beta, oriBLength, featureLength, epsilon);
}

template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
__aicore__ inline void BatchNormImpl(
    const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance,
    const LocalTensor<T>& inputX, const LocalTensor<T>& gamm, const LocalTensor<T>& beta,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, BatchNormTiling& tiling)
{
    CHECK_FUNC_HIGHLEVEL_API(
        BatchNorm, (T, isReuseSource, isBasicBlock),
        (output, outputMean, outputVariance, inputX, gamm, beta, sharedTmpBuffer, epsilon, tiling));
    static_assert(SupportType<T, half, float>(), "current data type is not supported on current device!");
    uint32_t oriBLength = tiling.originalBLength;
    uint32_t featureLength = tiling.meanVarSize;
    float firstDimValueBack = tiling.firstDimValueBack;
    if constexpr (isBasicBlock) {
        ASCENDC_ASSERT((oriBLength % 8 == 0), {
            KERNEL_LOG(KERNEL_ERROR, "BatchNorm buffer size error: oriBLength is %u not a multiple of 8", oriBLength);
        });
        ASCENDC_ASSERT((featureLength % 64 == 0 && featureLength <= 2048), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "BatchNorm buffer size error: current sLength * hLength is %u not a multiple of 64"
                "AND <= 2048.",
                featureLength);
        });
    }
    float epsilonFloat = static_cast<float>(epsilon);
    LocalTensor<float> tmpLocal = sharedTmpBuffer.ReinterpretCast<float>();
    LocalTensor<float> tmpMeanLocal = tmpLocal;

    LocalTensor<float> tmpVarLocal = tmpLocal[featureLength];
    BatchNormImplVF<T, isReuseSource, isBasicBlock>(
        (__ubuf__ T*)output.GetPhyAddr(), (__ubuf__ T*)outputMean.GetPhyAddr(),
        (__ubuf__ T*)outputVariance.GetPhyAddr(), (__ubuf__ T*)inputX.GetPhyAddr(), (__ubuf__ T*)gamm.GetPhyAddr(),
        (__ubuf__ T*)beta.GetPhyAddr(), (__ubuf__ float*)tmpMeanLocal.GetPhyAddr(),
        (__ubuf__ float*)tmpVarLocal.GetPhyAddr(), epsilonFloat, tiling, oriBLength, featureLength, firstDimValueBack);
}

template <typename T, bool isReuseSource = false, bool isBasicBlock = false>
__aicore__ inline void BatchNormImpl(
    const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance,
    const LocalTensor<T>& inputX, const LocalTensor<T>& gamm, const LocalTensor<T>& beta, const T epsilon,
    BatchNormTiling& tiling)
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ret = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ret), { KERNEL_LOG(KERNEL_ERROR, "BatchNorm failed to apply for tmp buffer!"); });
    BatchNormImpl<T, isReuseSource, isBasicBlock>(
        output, outputMean, outputVariance, inputX, gamm, beta, sharedTmpBuffer, epsilon, tiling);
}
} // namespace BatchNormAPI
} // namespace AscendC
#endif // IMPL_NORMALIZATION_BATCHNORM_BATCHNORM_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_BATCHNORM_BATCHNORM_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_BATCHNORM_BATCHNORM_C310_IMPL_H__
#endif
