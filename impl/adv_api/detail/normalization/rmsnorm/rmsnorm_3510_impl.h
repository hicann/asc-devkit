/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/normalization/rmsnorm/rmsnorm_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/layernorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_RMSNORM_RMSNORM_C310_IMPL_H__
#endif

#ifndef IMPL_NORMALIZATION_RMSNORM_RMSNORM_C310_IMPL_H
#define IMPL_NORMALIZATION_RMSNORM_RMSNORM_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "kernel_tiling/kernel_tiling.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/normalization/rmsnorm/rmsnorm_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
namespace RmsNormAPI {
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
__simd_callee__ inline void ComputeSum(
    __ubuf__ float* dstLocal, __ubuf__ T* srcLocal, uint32_t bsLength, uint32_t hLength, uint32_t oriHLength)
{
    uint16_t mainRepeatTime = static_cast<uint16_t>(oriHLength / oneRepSize);
    uint32_t tailCount = oriHLength % oneRepSize;
    uint16_t tailRepeatTime = static_cast<uint16_t>(CeilDivision(tailCount, oneRepSize));
    Reg::RegTensor<float> srcReg;
    Reg::RegTensor<float> dstReg;
    Reg::RegTensor<float> dstTailReg;
    Reg::MaskReg maskFull = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::MaskReg maskOne = Reg::CreateMask<float, Reg::MaskPattern::VL1>();
    Reg::MaskReg maskReg = Reg::UpdateMask<float>(tailCount);
    for (uint16_t bsIdx = 0; bsIdx < static_cast<uint16_t>(bsLength); bsIdx++) {
        Reg::Duplicate(dstReg, static_cast<float>(0), maskFull);
        for (uint16_t i = 0; i < mainRepeatTime; i++) {
            LoadDataWithT(srcLocal, srcReg, maskFull, bsIdx * hLength + i * oneRepSize);
            // step 1: x²
            Reg::Mul(srcReg, srcReg, srcReg, maskFull);
            // step 2: ∑x²
            Reg::Add(dstReg, dstReg, srcReg, maskFull);
        }
        for (uint16_t i = 0; i < tailRepeatTime; i++) {
            LoadDataWithT(srcLocal, srcReg, maskReg, bsIdx * hLength + mainRepeatTime * oneRepSize);
            // step 1: x²
            Reg::Mul(srcReg, srcReg, srcReg, maskReg);
            // step 2: ∑x²
            Reg::Add(dstTailReg, dstReg, srcReg, maskReg);
            Reg::Select(dstReg, dstTailReg, dstReg, maskReg);
        }
        Reg::ReduceSum(dstReg, dstReg, maskFull);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>(dstLocal + bsIdx, dstReg, maskOne);
    }
}
template <typename T>
__simd_callee__ inline void ComputeY(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* gammaLocal, __ubuf__ float* tmpLocal, uint32_t bsLength,
    uint32_t hLength, uint32_t oriHLength, const float epsilon, float reciprocalOfHLength)
{
    constexpr float rsqrtExponent = -0.5;
    Reg::RegTensor<float> srcReg;
    Reg::RegTensor<float> src2Reg;
    Reg::RegTensor<float> gammaReg;
    Reg::RegTensor<float> dstReg;
    Reg::RegTensor<float> tmpReg;
    Reg::RegTensor<float> dstTailReg;

    static constexpr Reg::LnSpecificMode lnMode = {Reg::MaskMergeMode::ZEROING, LnAlgo::INTRINSIC};
    static constexpr Reg::ExpSpecificMode expMode = {Reg::MaskMergeMode::ZEROING, ExpAlgo::INTRINSIC};
    Reg::MaskReg maskFull = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::MaskReg maskOne = Reg::CreateMask<float, Reg::MaskPattern::VL1>();
    Reg::Duplicate(dstReg, static_cast<float>(0), maskFull);
    uint16_t mainRepeatTime = static_cast<uint16_t>(oriHLength / oneRepSize);
    uint32_t tailCount = oriHLength % oneRepSize;
    uint16_t tailRepeatTime = static_cast<uint16_t>(CeilDivision(tailCount, oneRepSize));
    Reg::MaskReg maskReg = Reg::UpdateMask<float>(tailCount);
    for (uint16_t bsIdx = 0; bsIdx < static_cast<uint16_t>(bsLength); bsIdx++) {
        for (uint16_t i = 0; i < mainRepeatTime; i++) {
            LoadDataWithT(srcLocal, srcReg, maskFull, bsIdx * hLength + i * oneRepSize);
            LoadDataWithT(gammaLocal, gammaReg, maskFull, i * oneRepSize);
            Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(src2Reg, tmpLocal + bsIdx);
            // step 3: rms = 1/n*�?
            Reg::Muls(src2Reg, src2Reg, reciprocalOfHLength, maskFull);
            // step 4: rms + e
            Reg::Adds(src2Reg, src2Reg, epsilon, maskFull);
            // step 5: rsqrt: ln + muls + exp
            Reg::Ln<float, &lnMode>(src2Reg, src2Reg, maskFull);
            Reg::Muls(tmpReg, src2Reg, rsqrtExponent, maskFull);
            Reg::Exp<float, &expMode>(src2Reg, tmpReg, maskFull);
            // step 6: rms = xi * rsqrt
            Reg::Mul(src2Reg, srcReg, src2Reg, maskFull);
            // step 7: rms = rms * gamma
            Reg::Mul(src2Reg, src2Reg, gammaReg, maskFull);
            // save
            SaveDataWithT(dstLocal, src2Reg, maskFull, bsIdx * hLength + i * oneRepSize);
        }
        for (uint16_t i = 0; i < tailRepeatTime; i++) {
            LoadDataWithT(srcLocal, srcReg, maskReg, bsIdx * hLength + mainRepeatTime * oneRepSize);
            LoadDataWithT(gammaLocal, gammaReg, maskReg, mainRepeatTime * oneRepSize);
            Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(src2Reg, tmpLocal + bsIdx);
            // step 3: rms = 1/n*�?
            Reg::Muls(src2Reg, src2Reg, reciprocalOfHLength, maskReg);
            // step 4: rms + e
            Reg::Adds(src2Reg, src2Reg, epsilon, maskReg);
            // step 5: rsqrt: ln + muls + exp
            Reg::Ln<float, &lnMode>(src2Reg, src2Reg, maskReg);
            Reg::Muls(tmpReg, src2Reg, rsqrtExponent, maskReg);
            Reg::Exp<float, &expMode>(src2Reg, tmpReg, maskReg);
            // step 6: rms = xi * rsqrt
            Reg::Mul(src2Reg, srcReg, src2Reg, maskReg);
            // step 7: rms = rms * gamma
            Reg::Mul(src2Reg, src2Reg, gammaReg, maskReg);
            // save
            SaveDataWithT(dstLocal, src2Reg, maskReg, bsIdx * hLength + mainRepeatTime * oneRepSize);
        }
    }
}

template <typename T>
__simd_vf__ inline void RmsNormImplVf(
    __ubuf__ T* dstLocal, __ubuf__ T* srcLocal, __ubuf__ T* gammaLocal, __ubuf__ float* tmpLocal, const float epsilon,
    const RmsNormTiling tiling)
{
    uint32_t bLength = tiling.bLength;
    uint32_t sLength = tiling.sLength;
    uint32_t hLength = tiling.hLength;
    uint32_t oriHLength = tiling.originalHLength;
    float reciprocalOfHLength = tiling.reciprocalOfHLength;
    uint16_t loopRound = static_cast<uint16_t>(tiling.loopRound);
    uint32_t mainBsLength = tiling.mainBsLength;
    uint32_t mainBshLength = tiling.mainBshLength;

    for (uint16_t i = 0; i < loopRound; i++) {
        ComputeSum(tmpLocal, srcLocal + i * mainBshLength, mainBsLength, hLength, oriHLength);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
        ComputeY(
            dstLocal + i * mainBshLength, srcLocal + i * mainBshLength, gammaLocal, tmpLocal, mainBsLength, hLength,
            oriHLength, epsilon, reciprocalOfHLength);
    }
    uint32_t inputTailPos = tiling.inputTailPos;
    uint32_t tailBsLength = tiling.tailBsLength;
    uint16_t tailRound = static_cast<uint16_t>(CeilDivision(tailBsLength, mainBsLength));
    for (uint16_t i = 0; i < tailRound; i++) {
        ComputeSum(tmpLocal, srcLocal + inputTailPos, tailBsLength, hLength, oriHLength);
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
        ComputeY(
            dstLocal + inputTailPos, srcLocal + inputTailPos, gammaLocal, tmpLocal, tailBsLength, hLength, oriHLength,
            epsilon, reciprocalOfHLength);
    }
}

template <typename T, bool isBasicBlock = false>
__aicore__ inline void RmsNormImpl(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<T>& gammaLocal,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, const RmsNormTiling& tiling)
{
    if ASCEND_IS_AIC {
        return;
    }
    static_assert(SupportType<T, half, float>(), "current data type is not supported on current device!");
    CHECK_FUNC_HIGHLEVEL_API(
        RmsNorm, (T, isBasicBlock), (dstLocal, srcLocal, gammaLocal, sharedTmpBuffer, epsilon, tiling));
    LocalTensor<float> tmpLocal = sharedTmpBuffer.ReinterpretCast<float>();
    float eps = static_cast<float>(epsilon);
    RmsNormImplVf<T>(
        (__ubuf__ T*)dstLocal.GetPhyAddr(), (__ubuf__ T*)srcLocal.GetPhyAddr(), (__ubuf__ T*)gammaLocal.GetPhyAddr(),
        (__ubuf__ float*)tmpLocal.GetPhyAddr(), eps, tiling);
}
} // namespace RmsNormAPI
} // namespace AscendC
#endif // IMPL_NORMALIZATION_RMSNORM_RMSNORM_C310_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_RMSNORM_RMSNORM_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_RMSNORM_RMSNORM_C310_IMPL_H__
#endif
