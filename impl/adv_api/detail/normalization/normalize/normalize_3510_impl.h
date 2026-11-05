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
 * \file normalize_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/normalization/normalize/normalize_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/layernorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_NORMALIZE_NORMALIZE_C310_IMPL_H__
#endif
#ifndef IMPL_NORMALIZATION_NORMALIZE_NORMALIZE_C310_IMPL_H
#define IMPL_NORMALIZATION_NORMALIZE_NORMALIZE_C310_IMPL_H

#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "../../../../../include/adv_api/normalization/normalize_utils.h"
#include "normalize_config.h"

namespace AscendC {

namespace NormalizeInternal {
template <typename T>
__simd_callee__ inline void LoadDataWithT(
    __ubuf__ T* src, Reg::RegTensor<float>& dstReg, Reg::MaskReg& dstPreg, uint32_t srcOffset)
{
    if constexpr (IsSameType<T, half>::value || IsSameType<T, bfloat16_t>::value) {
        Reg::RegTensor<T> srcOrigin;
        Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(srcOrigin, src + srcOffset);
        Cast<float, T, layoutZMrgZ>(dstReg, srcOrigin, dstPreg);
    } else { // this branch: only support float
        Reg::LoadAlign(dstReg, src + srcOffset);
    }
}
} // namespace NormalizeInternal

template <typename T = Reg::DefaultType, Reg::MaskMergeMode mode = Reg::MaskMergeMode::ZEROING, typename RegT>
__simd_callee__ inline void RsqrtUtil(RegT& dstReg, RegT& srcReg, Reg::MaskReg& mask)
{
    constexpr static float POS_INF = 3.40282366920938E+38;
    using ActualT = typename RegT::ActualT;
    static_assert(std::is_same_v<T, Reg::DefaultType> || std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportType<T, float>(), "RsqrtUtil for high precision mode only supports float.");
    Reg::RegTensor<float> regOne;
    Reg::RegTensor<float> regZero;
    Reg::RegTensor<float> regInf;
    Reg::RegTensor<float> r;
    Reg::RegTensor<float> y;
    Reg::RegTensor<float> s;
    Reg::RegTensor<float> t;
    Reg::RegTensor<float> n15;
    Reg::RegTensor<float> n1;
    Reg::RegTensor<float> n05;

    Reg::RegTensor<float> calReg;
    Reg::RegTensor<float> cal1Reg;
    Reg::MaskReg cmpRegZero;
    Reg::MaskReg cmpRegInf;

    Reg::MaskReg pregFull = Reg::CreateMask<float, Reg::MaskPattern::ALL>();

    Duplicate(regOne, static_cast<float>(1), pregFull);
    Duplicate(regInf, POS_INF, pregFull);
    Duplicate(regZero, static_cast<float>(0), pregFull);
    Duplicate(n15, static_cast<float>(1.5), mask);
    Duplicate(n05, static_cast<float>(0.5), mask);
    Duplicate(s, static_cast<float>(1), mask);

    Div(r, regOne, srcReg, mask);
    Sqrt(y, r, mask);
    // y = y * (1.5 - 0.5*x*y*y)
    Muls(t, srcReg, static_cast<float>(-0.5), mask); // -0.5*x
    Mul(t, t, y, mask);                              // -0.5*x*y
    Reg::MulAddDst(n15, t, y, mask);                 // 1.5 + (-0.5*x*y) * y
    Mul(y, y, n15, mask);                            // y = y * (1.5 + (-0.5*x*y) * y)
    // s = 1 - x*r
    Muls(n1, srcReg, static_cast<float>(-1.0), mask); // -x
    MulAddDst(s, n1, r, mask);                        // s = 1 + (-x) * r, (ps: s = 1)
    // t = r - y*y => r = r + (-y) * y
    Muls(n1, y, static_cast<float>(-1.0), mask); // -y
    MulAddDst(r, n1, y, mask);                   // t = r + (-y) * y
    // e = s + x * t => s = s + x * t
    MulAddDst(s, srcReg, r, mask);
    // y = y + y*e*0.5
    Mul(s, s, y, mask);         // y*e => y*s(e)
    MulAddDst(y, s, n05, mask); // y = y + s*0.5

    // move to the last
    // if x == float(inf): return 0.0f // if mask is 0, then default select srcReg Value
    Compares(cmpRegZero, srcReg, POS_INF, mask);
    Select(dstReg, regZero, y, cmpRegZero);
    // if x == 0.0f: return float(inf)
    Compares(cmpRegInf, srcReg, (float)0, mask);
    Select(dstReg, regInf, dstReg, cmpRegInf);
}

template <typename U, typename T, bool isReuseSource = false, const NormalizeConfig& config = NLCFG_NORM>
__simd_vf__ inline void NormalizeVFImpl(
    __ubuf__ float* rstdUb, __ubuf__ float* meanUb, __ubuf__ float* varianceUb, __ubuf__ T* inputXUb,
    __ubuf__ T* outputUb, __ubuf__ U* gammaUb, __ubuf__ U* betaUb, __ubuf__ float* workUb,
    const LocalTensor<uint8_t> sharedTmpBuffer, const float epsilon, const NormalizePara para, uint16_t aLength,
    uint16_t tailARepeatTimes)
{
    constexpr uint16_t sregLower = (uint32_t)(GetVecLen() / sizeof(float));
    uint32_t rLength = para.rLength;
    uint32_t rLengthWithPadding = para.rLengthWithPadding;
    uint16_t repeatTimes = CeilDivision(rLength, sregLower);
    uint32_t count;
    uint32_t halfA = aLength / 2;

    __ubuf__ float* rstdUb2 = rstdUb + halfA;
    __ubuf__ float* rstdUbTail = rstdUb + aLength - 1;
    __ubuf__ float* meanUb2 = meanUb + halfA;
    __ubuf__ float* meanUbTail = meanUb + aLength - 1;
    __ubuf__ float* varianceUb2 = varianceUb + halfA;
    __ubuf__ float* varianceUbTail = varianceUb + aLength - 1;
    __ubuf__ T* inputXUb2 = inputXUb + halfA * rLengthWithPadding;
    __ubuf__ T* inputXUbTail = inputXUb + (aLength - 1) * rLengthWithPadding;
    __ubuf__ T* outputUb2 = outputUb + halfA * rLengthWithPadding;
    __ubuf__ T* outputUbTail = outputUb + (aLength - 1) * rLengthWithPadding;

    Reg::RegTensor<float> inputReg1;
    Reg::RegTensor<float> inputReg2;
    Reg::RegTensor<float> inputReg;
    Reg::RegTensor<float> gammaReg;
    Reg::RegTensor<float> betaReg;
    Reg::RegTensor<float> dstReg;
    Reg::RegTensor<float> dstReg1;
    Reg::RegTensor<float> dstReg2;

    Reg::RegTensor<float> meanReg;
    Reg::RegTensor<float> varianceReg;
    Reg::RegTensor<float> rstdReg;

    Reg::RegTensor<float> meanReg1;
    Reg::RegTensor<float> varianceReg1;
    Reg::RegTensor<float> rstdReg1;
    Reg::RegTensor<float> meanReg2;
    Reg::RegTensor<float> varianceReg2;
    Reg::RegTensor<float> rstdReg2;

    Reg::MaskReg preg;
    Reg::MaskReg pregFull = Reg::CreateMask<float, Reg::MaskPattern::ALL>();
    Reg::MaskReg pregOne = Reg::CreateMask<float, Reg::MaskPattern::VL1>();

    for (uint16_t j = 0; j < static_cast<uint16_t>(halfA); j++) {
        Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(varianceReg1, varianceUb + j);
        Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(varianceReg2, varianceUb2 + j);
        Adds(varianceReg1, varianceReg1, epsilon, pregFull);
        Adds(varianceReg2, varianceReg2, epsilon, pregFull);
        RsqrtUtil<float>(rstdReg1, varianceReg1, pregFull);
        RsqrtUtil<float>(rstdReg2, varianceReg2, pregFull);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>(rstdUb + j, rstdReg1, pregOne);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>(rstdUb2 + j, rstdReg2, pregOne);
    }
    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    for (uint16_t j = 0; j < static_cast<uint16_t>(halfA); j++) {
        count = rLength;
        Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(rstdReg1, rstdUb + j);
        Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(rstdReg2, rstdUb2 + j);
        Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(meanReg1, meanUb + j);
        Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(meanReg2, meanUb2 + j);
        for (uint16_t i = 0; i < repeatTimes; i++) {
            preg = Reg::UpdateMask<float>(count);
            NormalizeInternal::LoadDataWithT<T>(inputXUb, inputReg1, preg, j * rLengthWithPadding + i * sregLower);
            NormalizeInternal::LoadDataWithT<T>(inputXUb2, inputReg2, preg, j * rLengthWithPadding + i * sregLower);

            if constexpr (IsSameType<U, half>::value || IsSameType<U, bfloat16_t>::value) {
                if constexpr (!config.isNoGamma) {
                    Reg::RegTensor<U> gammaRegOrigin;
                    Reg::LoadAlign<U, Reg::LoadDist::DIST_UNPACK_B16>(gammaRegOrigin, gammaUb + i * sregLower);
                    Cast<float, U, layoutZMrgZ>(gammaReg, gammaRegOrigin, preg);
                }
                if constexpr (!config.isNoBeta) {
                    Reg::RegTensor<U> betaRegOrigin;
                    Reg::LoadAlign<U, Reg::LoadDist::DIST_UNPACK_B16>(betaRegOrigin, betaUb + i * sregLower);
                    Cast<float, U, layoutZMrgZ>(betaReg, betaRegOrigin, preg);
                }
            } else {
                if constexpr (!config.isNoGamma) {
                    Reg::LoadAlign(gammaReg, gammaUb + i * sregLower);
                }
                if constexpr (!config.isNoBeta) {
                    Reg::LoadAlign(betaReg, betaUb + i * sregLower);
                }
            }
            Sub<float, Reg::MaskMergeMode::ZEROING>(dstReg1, inputReg1, meanReg1, preg);
            Mul<float, Reg::MaskMergeMode::ZEROING>(dstReg1, dstReg1, rstdReg1, preg);
            Sub<float, Reg::MaskMergeMode::ZEROING>(dstReg2, inputReg2, meanReg2, preg);
            Mul<float, Reg::MaskMergeMode::ZEROING>(dstReg2, dstReg2, rstdReg2, preg);
            // FusedMulAdd: Vd = Vn * Vd + Vm, dst = gamma * dst + beta
            if constexpr (!config.isNoGamma && !config.isNoBeta) {
                MulDstAdd(dstReg1, gammaReg, betaReg, pregFull);
                MulDstAdd(dstReg2, gammaReg, betaReg, pregFull);
            } else {
                if constexpr (!config.isNoGamma) {
                    Mul<float, Reg::MaskMergeMode::ZEROING>(dstReg1, dstReg1, gammaReg, preg);
                    Mul<float, Reg::MaskMergeMode::ZEROING>(dstReg2, dstReg2, gammaReg, preg);
                }
                if constexpr (!config.isNoBeta) {
                    Add<float, Reg::MaskMergeMode::ZEROING>(dstReg1, dstReg1, betaReg, preg);
                    Add<float, Reg::MaskMergeMode::ZEROING>(dstReg2, dstReg2, betaReg, preg);
                }
            }
            if constexpr (IsSameType<T, half>::value || IsSameType<T, bfloat16_t>::value) {
                Reg::RegTensor<T> yRegOrigin;
                Cast<T, float, LayoutZMrgZRndRSatNS>(yRegOrigin, dstReg1, preg);
                Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(
                    outputUb + j * rLengthWithPadding + i * sregLower, yRegOrigin, preg);
            } else {
                Reg::StoreAlign(outputUb + j * rLengthWithPadding + i * sregLower, dstReg1, preg);
            }
            if constexpr (IsSameType<T, half>::value || IsSameType<T, bfloat16_t>::value) {
                Reg::RegTensor<T> yRegOrigin;
                Cast<T, float, LayoutZMrgZRndRSatNS>(yRegOrigin, dstReg2, preg);
                Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(
                    outputUb2 + j * rLengthWithPadding + i * sregLower, yRegOrigin, preg);
            } else {
                Reg::StoreAlign(outputUb2 + j * rLengthWithPadding + i * sregLower, dstReg2, preg);
            }
        }
    }
    for (uint16_t j = 0; j < tailARepeatTimes; j++) {
        Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(varianceReg, varianceUbTail);
        Adds(varianceReg, varianceReg, epsilon, pregFull);
        RsqrtUtil<float>(rstdReg, varianceReg, pregFull);
        Reg::StoreAlign<float, Reg::StoreDist::DIST_FIRST_ELEMENT_B32>(rstdUbTail, rstdReg, pregOne);
    }
    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    for (uint16_t j = 0; j < tailARepeatTimes; j++) {
        Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(meanReg, meanUbTail);
        Reg::LoadAlign<float, Reg::LoadDist::DIST_BRC_B32>(rstdReg, rstdUbTail);
        count = rLength;
        for (uint16_t i = 0; i < repeatTimes; i++) {
            Reg::MaskReg preg = Reg::UpdateMask<float>(count);
            NormalizeInternal::LoadDataWithT<T>(inputXUbTail, inputReg, preg, i * sregLower);

            if constexpr (IsSameType<U, half>::value || IsSameType<U, bfloat16_t>::value) {
                if constexpr (!config.isNoGamma) {
                    Reg::RegTensor<U> gammaRegOrigin;
                    Reg::LoadAlign<U, Reg::LoadDist::DIST_UNPACK_B16>(gammaRegOrigin, gammaUb + i * sregLower);
                    Cast<float, U, layoutZMrgZ>(gammaReg, gammaRegOrigin, preg);
                }
                if constexpr (!config.isNoBeta) {
                    Reg::RegTensor<U> betaRegOrigin;
                    Reg::LoadAlign<U, Reg::LoadDist::DIST_UNPACK_B16>(betaRegOrigin, betaUb + i * sregLower);
                    Cast<float, U, layoutZMrgZ>(betaReg, betaRegOrigin, preg);
                }
            } else {
                if constexpr (!config.isNoGamma) {
                    Reg::LoadAlign(gammaReg, gammaUb + i * sregLower);
                }
                if constexpr (!config.isNoBeta) {
                    Reg::LoadAlign(betaReg, betaUb + i * sregLower);
                }
            }
            Sub<float, Reg::MaskMergeMode::ZEROING>(dstReg, inputReg, meanReg, preg);
            Mul<float, Reg::MaskMergeMode::ZEROING>(dstReg, dstReg, rstdReg, preg);
            // FusedMulAdd: Vd = Vn * Vd + Vm, dst = gamma * dst + beta
            if constexpr (!config.isNoGamma && !config.isNoBeta) {
                MulDstAdd(dstReg, gammaReg, betaReg, pregFull);
            } else {
                if constexpr (!config.isNoGamma) {
                    Mul<float, Reg::MaskMergeMode::ZEROING>(dstReg, dstReg, gammaReg, preg);
                }
                if constexpr (!config.isNoBeta) {
                    Add<float, Reg::MaskMergeMode::ZEROING>(dstReg, dstReg, betaReg, preg);
                }
            }
            if constexpr (IsSameType<T, half>::value || IsSameType<T, bfloat16_t>::value) {
                Reg::RegTensor<T> yRegOrigin;
                Cast<T, float, LayoutZMrgZRndRSatNS>(yRegOrigin, dstReg, preg);
                Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(outputUbTail + i * sregLower, yRegOrigin, preg);
            } else {
                Reg::StoreAlign(outputUbTail + i * sregLower, dstReg, preg);
            }
        }
    }
}

template <typename U, typename T, bool isReuseSource = false, const NormalizeConfig& config = NLCFG_NORM>
__aicore__ inline void NormalizeImpl(
    const LocalTensor<T>& output, const LocalTensor<float>& outputRstd, const LocalTensor<float>& inputMean,
    const LocalTensor<float>& inputVariance, const LocalTensor<T>& inputX, const LocalTensor<U>& gamma,
    const LocalTensor<U>& beta, const LocalTensor<uint8_t>& sharedTmpBuffer, const float epsilon,
    const NormalizePara& para)
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

    static_assert(
        SupportEnum<config.reducePattern, ReducePattern::AR>(),
        "current api only supported pattern AR on current device!");
    if constexpr (config.aLength != -1) {
        ASCENDC_ASSERT((config.aLength == para.aLength), { KERNEL_LOG(KERNEL_ERROR, "current aLength not match!"); });
    }
    uint16_t aLength = static_cast<uint16_t>(para.aLength);
    uint16_t tailARepeatTimes = aLength % 2;
    LocalTensor<float> workLocal = sharedTmpBuffer.ReinterpretCast<float>();

    __ubuf__ float* rstdUb = (__ubuf__ float*)outputRstd.GetPhyAddr();
    __ubuf__ float* meanUb = (__ubuf__ float*)inputMean.GetPhyAddr();
    __ubuf__ float* varianceUb = (__ubuf__ float*)inputVariance.GetPhyAddr();
    __ubuf__ T* inputXUb = (__ubuf__ T*)inputX.GetPhyAddr();
    __ubuf__ T* outputUb = (__ubuf__ T*)output.GetPhyAddr();
    __ubuf__ U* gammaUb = (__ubuf__ U*)gamma.GetPhyAddr();
    __ubuf__ U* betaUb = (__ubuf__ U*)beta.GetPhyAddr();
    __ubuf__ float* workUb = (__ubuf__ float*)workLocal.GetPhyAddr();

    NormalizeVFImpl<U, T, isReuseSource, config>(
        rstdUb, meanUb, varianceUb, inputXUb, outputUb, gammaUb, betaUb, workUb, sharedTmpBuffer, epsilon, para,
        aLength, tailARepeatTimes);
}

template <typename U, typename T, bool isReuseSource = false, const NormalizeConfig& config = NLCFG_NORM>
__aicore__ inline void NormalizeImpl(
    const LocalTensor<T>& output, const LocalTensor<float>& outputRstd, const LocalTensor<float>& inputMean,
    const LocalTensor<float>& inputVariance, const LocalTensor<T>& inputX, const LocalTensor<U>& gamma,
    const LocalTensor<U>& beta, const float epsilon, const NormalizePara& para)
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    NormalizeImpl<U, T, isReuseSource, config>(
        output, outputRstd, inputMean, inputVariance, inputX, gamma, beta, sharedTmpBuffer, epsilon, para);
}
} // namespace AscendC
#endif // IMPL_NORMALIZATION_NORMALIZE_NORMALIZE_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_NORMALIZE_NORMALIZE_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_NORMALIZE_NORMALIZE_C310_IMPL_H__
#endif
