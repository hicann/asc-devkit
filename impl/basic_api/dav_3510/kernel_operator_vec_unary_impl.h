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
    "impl/basic_api/dav_3510/kernel_operator_vec_unary_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_vec_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_UNARY_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_VEC_UNARY_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_UNARY_IMPL_H
#include "../kernel_utils.h"
#include "kernel_operator_common_impl.h"
#include "../../../include/basic_api/kernel_operator_sys_var_intf.h"
#include "kernel_operator_vec_template_impl.h"
#include "../../../include/basic_api/reg_compute/kernel_reg_compute_intf.h"

namespace AscendC {
namespace Internal {
template <auto func, typename T, typename RegType>
__simd_vf__ inline void VecUnaryLevel2VFImpl(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t count)
{
    RegType srcReg;
    RegType dstReg;
    uint32_t sreg = static_cast<uint32_t>(count);
    Reg::MaskReg mask;
    constexpr uint32_t repeatStride = static_cast<uint32_t>(GetVecLen() / sizeof(T) * RegType::trait.REG_NUM);
    uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(sreg, repeatStride));
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<T, RegType::trait>(sreg);
        Reg::LoadAlign(srcReg, src + i * repeatStride);
        func(dstReg, srcReg, mask);
        Reg::StoreAlign(dst + i * repeatStride, dstReg, mask);
    }
}

template <auto func, typename T>
__aicore__ inline void VecUnaryLevel2ImplTemplate(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t count)
{
    if constexpr (SupportBytes<T, 8>()) {
        VecUnaryLevel2VFImpl<func, T, Reg::RegTensor<T, Reg::RegTraitNumTwo>>(dst, src, count);
    } else {
        VecUnaryLevel2VFImpl<func, T, Reg::RegTensor<T>>(dst, src, count);
    }
}

template <typename T>
__simd_vf__ inline void VecUnaryLevel2ImplFloat(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t count)
{
    static_assert(
        SupportType<T, float>(), "Failed to check dtype in Rsqrt FAST_INVERSE mode, "
                                 "current api only supports float. ");
    constexpr uint32_t sregLower = static_cast<uint32_t>(GetVecLen() / sizeof(T));
    constexpr uint32_t posZero = 0x00000000u;
    constexpr uint32_t negZero = 0x80000000u;
    constexpr uint32_t posInf = 0x7f800000u;
    constexpr float subnormalBound = 1.1754944e-38;
    constexpr float halfFactor = 0.5f;
    constexpr float negHalfFactor = -0.5f;
    constexpr float oneHalf = 1.5f;
    constexpr float negOne = -1.0f;
    constexpr float multiplyFactor0 = 16777216.0f;
    constexpr float multiplyFactor1 = 4096.0f;
    uint32_t sreg = static_cast<uint32_t>(count);
    const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(count, sregLower));
    NotNumUnion notNum0;
    notNum0.i = F32_INF;
    NotNumUnion notNum1;
    notNum1.i = F32_NEG_INF;
    /*
     * Improves Basic Api with high precision mode by using fast_inverse approach with following formula.
     * if x == float("inf"):
     * return 0.0f
     * if x == 0.0f:
     * return float("inf")
     * float r, r1, y, s, t, e;
     * bool p;
     * p = (x < 1.1754944e-38);
     * if (p)
     * x = x*16777216.0f;
     * r = errdiv(1.0, x); // div 指令
     * y = errsqrt(r); // sqrt 指令
     * y = y*(1.5 - 0.5*x*y*y);
     * s = 1 - x*r;
     * t = r - y*y;
     * e = s + x*t;
     * y = y + y*e*0.5;
     * if (p)
     * y = y*4096.0f; // y = y*2**12, 返回input是subnorma的结果
     * return y;
     */
    Reg::RegTensor<T> regZero;
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<T> dstReg;
    Reg::RegTensor<T> regOne;
    Reg::RegTensor<T> regOneHalf;
    Reg::RegTensor<T> tmpReg;
    Reg::RegTensor<T> divReg;
    Reg::RegTensor<T> mulReg;
    Reg::RegTensor<T> resReg;
    Reg::RegTensor<T> negInfReg;
    Reg::RegTensor<T> posInfReg;
    Reg::MaskReg mask;
    Reg::MaskReg isInfMask;
    Reg::MaskReg isPosZeroMask;
    Reg::MaskReg isNegZeroMask;
    Reg::MaskReg cmpMask;
    Reg::MaskReg maskFull = Reg::CreateMask<T>();
    static constexpr Reg::DivSpecificMode divMode = {
        Reg::MaskMergeMode::ZEROING, false, DivAlgo::PRECISION_1ULP_FTZ_TRUE};
    static constexpr Reg::SqrtSpecificMode sqrtMode = {
        Reg::MaskMergeMode::ZEROING, false, SqrtAlgo::PRECISION_1ULP_FTZ_TRUE};
    Reg::Duplicate(regZero, 0.0f, maskFull);
    Reg::Duplicate(posInfReg, notNum0.f, maskFull);
    Reg::Duplicate(negInfReg, notNum1.f, maskFull);
    for (uint16_t i = 0; i < repeatTime; i++) {
        mask = Reg::UpdateMask<T>(sreg);
        Reg::LoadAlign(srcReg, src + i * sregLower);

        Reg::Duplicate(regOne, 1.0f, maskFull);
        Reg::Duplicate(regOneHalf, oneHalf, maskFull);

        Reg::CompareScalar<T, CMPMODE::LT>(cmpMask, srcReg, subnormalBound, mask);
        Reg::Muls(tmpReg, srcReg, multiplyFactor0, mask);
        Reg::Select(srcReg, tmpReg, srcReg, cmpMask);

        Reg::Div<T, &divMode>(divReg, regOne, srcReg, mask); // r = errdiv(1.0, x);
        Reg::Sqrt<T, &sqrtMode>(resReg, divReg, mask);       // y = errsqrt(r);
        Reg::Muls(tmpReg, srcReg, negHalfFactor, mask);      // -0.5x
        Reg::Mul(mulReg, tmpReg, resReg, mask);              // -0.5xy
        Reg::MulAddDst(regOneHalf, mulReg, resReg, mask);    // 1.5 - 0.5xy*y
        Reg::Mul(resReg, regOneHalf, resReg, mask);          // y = y * (1.5 + (-0.5*x*y) * y)

        Reg::Muls(tmpReg, srcReg, negOne, mask);      // -x
        Reg::MulAddDst(regOne, tmpReg, divReg, mask); // s = 1 - x*r
        Reg::Muls(tmpReg, resReg, negOne, mask);      // -y
        Reg::MulAddDst(divReg, tmpReg, resReg, mask); // t = r + (-y) * y
        // e = s + x * t => s = s + x * t
        Reg::MulAddDst(regOne, srcReg, divReg, mask);
        // y = y + y * e * 0.5
        Reg::Muls(mulReg, resReg, halfFactor, mask);  // 0.5*y
        Reg::MulAddDst(resReg, mulReg, regOne, mask); // y = y + s*0.5y

        Reg::Muls(tmpReg, resReg, multiplyFactor1, mask);
        Reg::Select(dstReg, tmpReg, resReg, cmpMask);

        Reg::CompareScalar(isInfMask, (Reg::RegTensor<uint32_t>&)srcReg, posInf, mask);
        Reg::Select(dstReg, regZero, dstReg, isInfMask);
        Reg::CompareScalar(isPosZeroMask, (Reg::RegTensor<uint32_t>&)srcReg, posZero, mask);
        Reg::Select(dstReg, posInfReg, dstReg, isPosZeroMask);
        Reg::CompareScalar(isNegZeroMask, (Reg::RegTensor<uint32_t>&)srcReg, negZero, mask);
        Reg::Select(dstReg, negInfReg, dstReg, isNegZeroMask);

        Reg::StoreAlign(dst + i * sregLower, dstReg, mask);
    }
}

template <typename T>
__simd_vf__ inline void VecUnaryLevel2ImplB64(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t count)
{
    Reg::RegTensor<T, Reg::RegTraitNumTwo> vSrcReg0;
    Reg::RegTensor<T, Reg::RegTraitNumTwo> vDstReg0;
    Reg::RegTensor<T, Reg::RegTraitNumTwo> vRegOne, vRegNegOne, vRegZero, vRegF;
    uint32_t sreg = static_cast<uint32_t>(count);
    Reg::MaskReg preg, maskZero, maskOne, maskNegOne;
    uint32_t sregLower = static_cast<uint32_t>(VECTOR_REG_WIDTH_2XVL / sizeof(T));
    uint16_t repeatTime = CeilDivision(count, sregLower);
    for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {
        preg = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
        Reg::Duplicate(vRegOne, T(1), preg);
        Reg::Duplicate(vRegZero, T(0), preg);
        Reg::Duplicate(vRegF, static_cast<T>(0xffffffffffffffff), preg);
        Reg::LoadAlign(vSrcReg0, src + i * sregLower);
        Reg::CompareScalar(maskZero, vSrcReg0, T(0), preg);
        Reg::Select(vDstReg0, vRegF, vRegZero, maskZero);
        Reg::CompareScalar(maskOne, vSrcReg0, T(1), preg);
        Reg::Select(vDstReg0, vRegOne, vDstReg0, maskOne);
        if constexpr (IsSameType<T, int64_t>::value) {
            Reg::Duplicate(vRegNegOne, T(-1), preg);
            Reg::CompareScalar(maskNegOne, vSrcReg0, T(-1), preg);
            Reg::Select(vDstReg0, vRegNegOne, vDstReg0, maskNegOne);
        }
        Reg::StoreAlign(dst + i * sregLower, vDstReg0, preg);
    }
}

template <auto func, bool isSetMask, bool isMaskBitMode, bool isNormalMode, typename T>
__simd_vf__ inline void VecUnaryLevel0VFImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const BasicAPIMaskStruct maskArrayStruct, const uint64_t maskCount,
    const uint8_t repeatTime, const UnaryRepeatParams repeatParams, __ubuf__ uint64_t* maskBuf)
{
    uint32_t count =
        VecMicroGetCount<isSetMask, isNormalMode, isMaskBitMode>(maskArrayStruct.maskArray, maskCount, maskBuf);
    uint16_t newRepeatTimes = 0;
    newRepeatTimes = VecMicroGetRepeatTimes<T, isNormalMode>(count, repeatTime);
    Reg::MaskReg maskReg;
    if constexpr (isNormalMode) {
        maskReg = VecMicroGetMaskReg<T, isSetMask, isNormalMode, isMaskBitMode>(maskBuf, count);
    }
    constexpr uint8_t ElePerBlkT = GetDataBlockSizeInBytes() / sizeof(T);
    for (uint16_t index = 0; index < newRepeatTimes; ++index) {
        if constexpr (!isNormalMode) {
            maskReg = VecMicroGetMaskReg<T, isSetMask, isNormalMode, isMaskBitMode>(maskBuf, count);
        }
        Reg::RegTensor<T> dstVreg;
        Reg::RegTensor<T> srcVreg;
#ifndef NO_OVERLAP_IN_MULTI_REPEAT
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
#endif
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            srcVreg, src + index * repeatParams.srcRepStride * ElePerBlkT, repeatParams.srcBlkStride, maskReg);
        func(dstVreg, srcVreg, maskReg);
        Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            dst + index * repeatParams.dstRepStride * ElePerBlkT, dstVreg, repeatParams.dstBlkStride, maskReg);
    }
}

template <auto func, bool isSetMask, bool isMaskBitMode, typename T>
__aicore__ inline void VecUnaryLevel0Template(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t maskArray[], const uint64_t maskCount, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    BasicAPIMaskStruct maskArrayStruct;
    if constexpr (isMaskBitMode) {
        ASCENDC_ASSERT(
            maskCount == 0, { KERNEL_LOG(KERNEL_ERROR, "maskCount must be 0 when isMaskBitMode is true."); });
        maskArrayStruct = *(reinterpret_cast<const BasicAPIMaskStruct*>(maskArray));
    } else {
        ASCENDC_ASSERT(maskArray == nullptr, {
            KERNEL_LOG(KERNEL_ERROR, "maskArray must be nullptr when isMaskBitMode is false.");
        });
    }
    __ubuf__ uint64_t* maskBuf = nullptr;

    if (Internal::IsCounterMode()) {
        if constexpr (!isSetMask) {
            maskBuf =
                AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 2); // maskReg 256bit PK-> 128bit
        }
        VecUnaryLevel0VFImpl<func, isSetMask, isMaskBitMode, false, T>(
            dst, src, maskArrayStruct, maskCount, repeatTime, repeatParams, maskBuf);
        if constexpr (!isSetMask) {
            AscendCUtils::FreeTemporaryBuffer<uint64_t>(maskBuf);
        }
    } else {
        if constexpr (isMaskBitMode) {
            if constexpr (SupportBytes<T, 1>()) {
                ASCENDC_ASSERT(isSetMask, { KERNEL_LOG(KERNEL_ERROR, "mask must be set when sizeof(T) is 1."); });
                auto eventIDV2S = GetTPipePtr()->FetchEventID(HardEvent::V_S);
                SetFlag<HardEvent::V_S>(eventIDV2S);
                WaitFlag<HardEvent::V_S>(eventIDV2S);
                maskBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 4);
                maskBuf[0] = maskArray[0];
                maskBuf[1] = maskArray[1];
                maskBuf[2] = maskArray[2];
                maskBuf[3] = maskArray[3];
                auto eventIDS2V = GetTPipePtr()->FetchEventID(HardEvent::S_V);
                SetFlag<HardEvent::S_V>(eventIDS2V);
                WaitFlag<HardEvent::S_V>(eventIDS2V);
            } else if constexpr (isSetMask) {
                SetVectorMask<T>(maskArray[1], maskArray[0]); // set mask to SPR.MASK, movp in VF
            }
        }
        // when isSetMask is false, normal mode, maskBuf = nullptr, not support B8
        VecUnaryLevel0VFImpl<func, isSetMask, isMaskBitMode, true, T>(
            dst, src, maskArrayStruct, maskCount, repeatTime, repeatParams, maskBuf);
        if constexpr (isMaskBitMode && SupportBytes<T, 1>()) {
            AscendC::AscendCUtils::FreeTemporaryBuffer<uint64_t>(maskBuf);
        }
    }
}
} // namespace Internal

template <typename T, bool isSetMask = true, const ExpConfig& config = DEFAULT_EXP_CONFIG>
__aicore__ inline void ExpImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == ExpAlgo::PRECISION_1ULP_FTZ_TRUE
#else
        config.algo == ExpAlgo::INTRINSIC || config.algo == ExpAlgo::PRECISION_1ULP_FTZ_TRUE
#endif
    ) {
        static constexpr Reg::ExpSpecificMode mode = {Reg::MaskMergeMode::ZEROING, ExpAlgo::PRECISION_1ULP_FTZ_TRUE};
        constexpr auto func = Reg::Exp<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel0Template<func, isSetMask, true>(dst, src, mask, 0, repeatTime, repeatParams);
    } else if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == ExpAlgo::INTRINSIC || config.algo == ExpAlgo::PRECISION_1ULP_FTZ_FALSE
#else
        config.algo == ExpAlgo::PRECISION_1ULP_FTZ_FALSE
#endif
    ) {
        static constexpr Reg::ExpSpecificMode mode = {Reg::MaskMergeMode::ZEROING, ExpAlgo::PRECISION_1ULP_FTZ_FALSE};
        constexpr auto func = Reg::Exp<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel0Template<func, isSetMask, true>(dst, src, mask, 0, repeatTime, repeatParams);
    }
}

template <typename T, bool isSetMask = true, const ExpConfig& config = DEFAULT_EXP_CONFIG>
__aicore__ inline void ExpImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == ExpAlgo::PRECISION_1ULP_FTZ_TRUE
#else
        config.algo == ExpAlgo::INTRINSIC || config.algo == ExpAlgo::PRECISION_1ULP_FTZ_TRUE
#endif
    ) {
        static constexpr Reg::ExpSpecificMode mode = {Reg::MaskMergeMode::ZEROING, ExpAlgo::PRECISION_1ULP_FTZ_TRUE};
        constexpr auto func = Reg::Exp<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel0Template<func, isSetMask, false>(dst, src, nullptr, mask, repeatTime, repeatParams);
    } else if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == ExpAlgo::INTRINSIC || config.algo == ExpAlgo::PRECISION_1ULP_FTZ_FALSE
#else
        config.algo == ExpAlgo::PRECISION_1ULP_FTZ_FALSE
#endif
    ) {
        static constexpr Reg::ExpSpecificMode mode = {Reg::MaskMergeMode::ZEROING, ExpAlgo::PRECISION_1ULP_FTZ_FALSE};
        constexpr auto func = Reg::Exp<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel0Template<func, isSetMask, false>(dst, src, nullptr, mask, repeatTime, repeatParams);
    }
}

template <typename T, bool isSetMask = true, const LnConfig& config = DEFAULT_LN_CONFIG>
__aicore__ inline void LnImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == LnAlgo::PRECISION_1ULP_FTZ_TRUE
#else
        config.algo == LnAlgo::INTRINSIC || config.algo == LnAlgo::PRECISION_1ULP_FTZ_TRUE
#endif
    ) {
        static constexpr Reg::LnSpecificMode mode = {Reg::MaskMergeMode::ZEROING, LnAlgo::PRECISION_1ULP_FTZ_TRUE};
        constexpr auto func = Reg::Ln<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel0Template<func, isSetMask, true>(dst, src, mask, 0, repeatTime, repeatParams);
    } else if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == LnAlgo::INTRINSIC || config.algo == LnAlgo::PRECISION_1ULP_FTZ_FALSE
#else
        config.algo == LnAlgo::PRECISION_1ULP_FTZ_FALSE
#endif
    ) {
        static constexpr Reg::LnSpecificMode mode = {Reg::MaskMergeMode::ZEROING, LnAlgo::PRECISION_1ULP_FTZ_FALSE};
        constexpr auto func = Reg::Ln<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel0Template<func, isSetMask, true>(dst, src, mask, 0, repeatTime, repeatParams);
    }
}

template <typename T, bool isSetMask = true, const LnConfig& config = DEFAULT_LN_CONFIG>
__aicore__ inline void LnImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == LnAlgo::PRECISION_1ULP_FTZ_TRUE
#else
        config.algo == LnAlgo::INTRINSIC || config.algo == LnAlgo::PRECISION_1ULP_FTZ_TRUE
#endif
    ) {
        static constexpr Reg::LnSpecificMode mode = {Reg::MaskMergeMode::ZEROING, LnAlgo::PRECISION_1ULP_FTZ_TRUE};
        constexpr auto func = Reg::Ln<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel0Template<func, isSetMask, false>(dst, src, nullptr, mask, repeatTime, repeatParams);
    } else if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == LnAlgo::INTRINSIC || config.algo == LnAlgo::PRECISION_1ULP_FTZ_FALSE
#else
        config.algo == LnAlgo::PRECISION_1ULP_FTZ_FALSE
#endif
    ) {
        static constexpr Reg::LnSpecificMode mode = {Reg::MaskMergeMode::ZEROING, LnAlgo::PRECISION_1ULP_FTZ_FALSE};
        constexpr auto func = Reg::Ln<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel0Template<func, isSetMask, false>(dst, src, nullptr, mask, repeatTime, repeatParams);
    }
}

template <typename T, bool isSetMask = true>
__aicore__ inline void AbsImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(
        (SupportType<T, half, int16_t, float, int32_t>()), "current data type is not supported on current device!");
    constexpr auto func = Reg::Abs<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecUnaryLevel0Template<func, isSetMask, true>(dst, src, mask, 0, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void AbsImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(
        (SupportType<T, half, int16_t, float, int32_t>()), "current data type is not supported on current device!");
    constexpr auto func = Reg::Abs<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecUnaryLevel0Template<func, isSetMask, false>(dst, src, nullptr, mask, repeatTime, repeatParams);
}

namespace RegReciprocal {
template <typename T, typename RegT, bool precisionMode = false, const Reg::DivSpecificMode* divMode = nullptr>
__aicore__ inline void Reciprocal(RegT& dstReg, RegT& srcReg, Reg::MaskReg& mask)
{
    Reg::Duplicate<T>(dstReg, 1.0f, mask);
    if constexpr (!precisionMode) {
        Reg::Div<T, divMode>(dstReg, dstReg, srcReg, mask);
    } else {
        static constexpr AscendC::Reg::DivSpecificMode precisionDivMode = {
            Reg::MaskMergeMode::ZEROING, false, DivAlgo::PRECISION_1ULP_FTZ_FALSE};
        Reg::Div<T, &precisionDivMode>(dstReg, dstReg, srcReg, mask);
    }
}
} // namespace RegReciprocal
template <typename T, bool isSetMask = true, const ReciprocalConfig& config = DEFAULT_RECIPROCAL_CONFIG>
__aicore__ inline void ReciprocalImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == ReciprocalAlgo::PRECISION_1ULP_FTZ_TRUE
#else
        config.algo == ReciprocalAlgo::INTRINSIC || config.algo == ReciprocalAlgo::PRECISION_1ULP_FTZ_TRUE
#endif
    ) {
        static constexpr Reg::DivSpecificMode mode = {
            Reg::MaskMergeMode::ZEROING, false, DivAlgo::PRECISION_1ULP_FTZ_TRUE};
        constexpr auto func = RegReciprocal::Reciprocal<T, Reg::RegTensor<T>, false, &mode>;
        Internal::VecUnaryLevel0Template<func, isSetMask, true>(dst, src, mask, 0, repeatTime, repeatParams);
    } else if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == ReciprocalAlgo::INTRINSIC || config.algo == ReciprocalAlgo::PRECISION_1ULP_FTZ_FALSE
#else
        config.algo == ReciprocalAlgo::PRECISION_1ULP_FTZ_FALSE
#endif
    ) {
        constexpr auto func = RegReciprocal::Reciprocal<T, Reg::RegTensor<T>, true>;
        Internal::VecUnaryLevel0Template<func, isSetMask, true>(dst, src, mask, 0, repeatTime, repeatParams);
    }
}

template <typename T, bool isSetMask = true, const ReciprocalConfig& config = DEFAULT_RECIPROCAL_CONFIG>
__aicore__ inline void ReciprocalImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == ReciprocalAlgo::PRECISION_1ULP_FTZ_TRUE
#else
        config.algo == ReciprocalAlgo::INTRINSIC || config.algo == ReciprocalAlgo::PRECISION_1ULP_FTZ_TRUE
#endif
    ) {
        static constexpr Reg::DivSpecificMode mode = {
            Reg::MaskMergeMode::ZEROING, false, DivAlgo::PRECISION_1ULP_FTZ_TRUE};
        constexpr auto func = RegReciprocal::Reciprocal<T, Reg::RegTensor<T>, false, &mode>;
        Internal::VecUnaryLevel0Template<func, isSetMask, false>(dst, src, nullptr, mask, repeatTime, repeatParams);
    } else if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == ReciprocalAlgo::INTRINSIC || config.algo == ReciprocalAlgo::PRECISION_1ULP_FTZ_FALSE
#else
        config.algo == ReciprocalAlgo::PRECISION_1ULP_FTZ_FALSE
#endif
    ) {
        constexpr auto func = RegReciprocal::Reciprocal<T, Reg::RegTensor<T>, true>;
        Internal::VecUnaryLevel0Template<func, isSetMask, false>(dst, src, nullptr, mask, repeatTime, repeatParams);
    }
}

template <typename T, bool isSetMask = true, const SqrtConfig& config = DEFAULT_SQRT_CONFIG>
__aicore__ inline void SqrtImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == SqrtAlgo::PRECISION_1ULP_FTZ_TRUE
#else
        config.algo == SqrtAlgo::INTRINSIC || config.algo == SqrtAlgo::PRECISION_1ULP_FTZ_TRUE
#endif
    ) {
        static constexpr Reg::SqrtSpecificMode mode = {
            Reg::MaskMergeMode::ZEROING, false, SqrtAlgo::PRECISION_1ULP_FTZ_TRUE};
        constexpr auto func = Reg::Sqrt<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel0Template<func, isSetMask, true>(dst, src, mask, 0, repeatTime, repeatParams);
    } else if constexpr (config.algo == SqrtAlgo::FAST_INVERSE) {
        static constexpr Reg::SqrtSpecificMode mode = {Reg::MaskMergeMode::ZEROING, true, SqrtAlgo::FAST_INVERSE};
        constexpr auto func = Reg::Sqrt<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel0Template<func, isSetMask, true>(dst, src, mask, 0, repeatTime, repeatParams);
    } else if constexpr (config.algo == SqrtAlgo::PRECISION_0ULP_FTZ_FALSE) {
        static constexpr Reg::SqrtSpecificMode mode = {
            Reg::MaskMergeMode::ZEROING, false, SqrtAlgo::PRECISION_0ULP_FTZ_FALSE};
        constexpr auto func = Reg::Sqrt<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel0Template<func, isSetMask, true>(dst, src, mask, 0, repeatTime, repeatParams);
    } else if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == SqrtAlgo::INTRINSIC || config.algo == SqrtAlgo::PRECISION_1ULP_FTZ_FALSE
#else
        config.algo == SqrtAlgo::PRECISION_1ULP_FTZ_FALSE
#endif
    ) {
        static constexpr Reg::SqrtSpecificMode mode = {
            Reg::MaskMergeMode::ZEROING, false, SqrtAlgo::PRECISION_1ULP_FTZ_FALSE};
        constexpr auto func = Reg::Sqrt<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel0Template<func, isSetMask, true>(dst, src, mask, 0, repeatTime, repeatParams);
    }
}

template <typename T, bool isSetMask = true, const SqrtConfig& config = DEFAULT_SQRT_CONFIG>
__aicore__ inline void SqrtImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == SqrtAlgo::PRECISION_1ULP_FTZ_TRUE
#else
        config.algo == SqrtAlgo::INTRINSIC || config.algo == SqrtAlgo::PRECISION_1ULP_FTZ_TRUE
#endif
    ) {
        static constexpr Reg::SqrtSpecificMode mode = {
            Reg::MaskMergeMode::ZEROING, false, SqrtAlgo::PRECISION_1ULP_FTZ_TRUE};
        constexpr auto func = Reg::Sqrt<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel0Template<func, isSetMask, false>(dst, src, nullptr, mask, repeatTime, repeatParams);
    } else if constexpr (config.algo == SqrtAlgo::FAST_INVERSE) {
        static constexpr Reg::SqrtSpecificMode mode = {Reg::MaskMergeMode::ZEROING, true, SqrtAlgo::FAST_INVERSE};
        constexpr auto func = Reg::Sqrt<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel0Template<func, isSetMask, false>(dst, src, nullptr, mask, repeatTime, repeatParams);
    } else if constexpr (config.algo == SqrtAlgo::PRECISION_0ULP_FTZ_FALSE) {
        static constexpr Reg::SqrtSpecificMode mode = {
            Reg::MaskMergeMode::ZEROING, false, SqrtAlgo::PRECISION_0ULP_FTZ_FALSE};
        constexpr auto func = Reg::Sqrt<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel0Template<func, isSetMask, false>(dst, src, nullptr, mask, repeatTime, repeatParams);
    } else if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == SqrtAlgo::INTRINSIC || config.algo == SqrtAlgo::PRECISION_1ULP_FTZ_FALSE
#else
        config.algo == SqrtAlgo::PRECISION_1ULP_FTZ_FALSE
#endif
    ) {
        static constexpr Reg::SqrtSpecificMode mode = {
            Reg::MaskMergeMode::ZEROING, false, SqrtAlgo::PRECISION_1ULP_FTZ_FALSE};
        constexpr auto func = Reg::Sqrt<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel0Template<func, isSetMask, false>(dst, src, nullptr, mask, repeatTime, repeatParams);
    }
}

namespace RegRsqrt {
template <
    typename T, typename RegT, bool precisionMode = false, const Reg::SqrtSpecificMode* sqrtMode = nullptr,
    const Reg::DivSpecificMode* divMode = nullptr>
__simd_callee__ inline void Rsqrt(RegT& dstReg, RegT& srcReg, Reg::MaskReg& mask)
{
    Reg::MaskReg cmpMask;
    Reg::Duplicate<T>(dstReg, static_cast<T>(1.0f), mask);
    Reg::CompareScalar<T, CMPMODE::LT>(cmpMask, srcReg, static_cast<T>(0.0f), mask);
    if constexpr (!precisionMode) {
        Reg::Sqrt<T, sqrtMode>(srcReg, srcReg, mask);
        Reg::Div<T, divMode>(dstReg, dstReg, srcReg, mask);
        Reg::Select(dstReg, srcReg, dstReg, cmpMask);
    } else {
        if constexpr (SupportType<T, half>()) {
            static constexpr AscendC::Reg::SqrtSpecificMode SqrtMode = {
                Reg::MaskMergeMode::ZEROING, false, SqrtAlgo::PRECISION_1ULP_FTZ_FALSE};
            Reg::Sqrt<T, &SqrtMode>(srcReg, srcReg, mask);
            static constexpr AscendC::Reg::DivSpecificMode precisionDivMode = {
                Reg::MaskMergeMode::ZEROING, false, DivAlgo::PRECISION_1ULP_FTZ_FALSE};
            Reg::Div<T, &precisionDivMode>(dstReg, dstReg, srcReg, mask);
        } else {
            static constexpr AscendC::Reg::SqrtSpecificMode SqrtMode = {
                Reg::MaskMergeMode::ZEROING, false, SqrtAlgo::PRECISION_0ULP_FTZ_FALSE};
            Reg::Sqrt<T, &SqrtMode>(srcReg, srcReg, mask);
            static constexpr AscendC::Reg::DivSpecificMode precisionDivMode = {
                Reg::MaskMergeMode::ZEROING, false, DivAlgo::PRECISION_0ULP_FTZ_FALSE};
            Reg::Div<T, &precisionDivMode>(dstReg, dstReg, srcReg, mask);
        }
    }
}
} // namespace RegRsqrt

template <typename T, bool isSetMask = true, const RsqrtConfig& config = DEFAULT_RSQRT_CONFIG>
__aicore__ inline void RsqrtImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == RsqrtAlgo::PRECISION_1ULP_FTZ_TRUE
#else
        config.algo == RsqrtAlgo::INTRINSIC || config.algo == RsqrtAlgo::PRECISION_1ULP_FTZ_TRUE
#endif
    ) {
        static constexpr Reg::SqrtSpecificMode sqrtMode = {
            Reg::MaskMergeMode::ZEROING, false, SqrtAlgo::PRECISION_1ULP_FTZ_TRUE};
        static constexpr Reg::DivSpecificMode divMode = {
            Reg::MaskMergeMode::ZEROING, false, DivAlgo::PRECISION_1ULP_FTZ_TRUE};
        constexpr auto func = RegRsqrt::Rsqrt<T, Reg::RegTensor<T>, false, &sqrtMode, &divMode>;
        Internal::VecUnaryLevel0Template<func, isSetMask, true>(dst, src, mask, 0, repeatTime, repeatParams);
    } else if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == RsqrtAlgo::INTRINSIC || config.algo == RsqrtAlgo::FAST_INVERSE ||
        config.algo == RsqrtAlgo::PRECISION_0ULP_FTZ_FALSE || config.algo == RsqrtAlgo::PRECISION_1ULP_FTZ_FALSE
#else
        config.algo == RsqrtAlgo::FAST_INVERSE || config.algo == RsqrtAlgo::PRECISION_0ULP_FTZ_FALSE ||
        config.algo == RsqrtAlgo::PRECISION_1ULP_FTZ_FALSE
#endif
    ) {
        constexpr auto func = RegRsqrt::Rsqrt<T, Reg::RegTensor<T>, true>;
        Internal::VecUnaryLevel0Template<func, isSetMask, true>(dst, src, mask, 0, repeatTime, repeatParams);
    }
}

template <typename T, bool isSetMask = true, const RsqrtConfig& config = DEFAULT_RSQRT_CONFIG>
__aicore__ inline void RsqrtImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == RsqrtAlgo::PRECISION_1ULP_FTZ_TRUE
#else
        config.algo == RsqrtAlgo::INTRINSIC || config.algo == RsqrtAlgo::PRECISION_1ULP_FTZ_TRUE
#endif
    ) {
        static constexpr Reg::SqrtSpecificMode sqrtMode = {
            Reg::MaskMergeMode::ZEROING, false, SqrtAlgo::PRECISION_1ULP_FTZ_TRUE};
        static constexpr Reg::DivSpecificMode divMode = {
            Reg::MaskMergeMode::ZEROING, false, DivAlgo::PRECISION_1ULP_FTZ_TRUE};
        constexpr auto func = RegRsqrt::Rsqrt<T, Reg::RegTensor<T>, false, &sqrtMode, &divMode>;
        Internal::VecUnaryLevel0Template<func, isSetMask, false>(dst, src, nullptr, mask, repeatTime, repeatParams);
    } else if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == RsqrtAlgo::INTRINSIC || config.algo == RsqrtAlgo::FAST_INVERSE ||
        config.algo == RsqrtAlgo::PRECISION_0ULP_FTZ_FALSE || config.algo == RsqrtAlgo::PRECISION_1ULP_FTZ_FALSE
#else
        config.algo == RsqrtAlgo::FAST_INVERSE || config.algo == RsqrtAlgo::PRECISION_0ULP_FTZ_FALSE ||
        config.algo == RsqrtAlgo::PRECISION_1ULP_FTZ_FALSE
#endif
    ) {
        constexpr auto func = RegRsqrt::Rsqrt<T, Reg::RegTensor<T>, true>;
        Internal::VecUnaryLevel0Template<func, isSetMask, false>(dst, src, nullptr, mask, repeatTime, repeatParams);
    }
}

template <typename T, bool isSetMask = true>
__aicore__ inline void NotImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(
        (SupportType<T, half, float, uint16_t, int16_t, uint32_t, int32_t>()),
        "current data type is not supported on current device!");
    constexpr auto func = Reg::Not<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecUnaryLevel0Template<func, isSetMask, true>(dst, src, mask, 0, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void NotImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(
        (SupportType<T, half, float, uint16_t, int16_t, uint32_t, int32_t>()),
        "current data type is not supported on current device!");
    constexpr auto func = Reg::Not<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecUnaryLevel0Template<func, isSetMask, false>(dst, src, nullptr, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void ReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert((SupportType<T, half, float, int32_t>()), "current data type is not supported on current device!");
    constexpr auto func = Reg::Relu<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecUnaryLevel0Template<func, isSetMask, true>(dst, src, mask, 0, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void ReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert((SupportType<T, half, float, int32_t>()), "current data type is not supported on current device!");
    constexpr auto func = Reg::Relu<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecUnaryLevel0Template<func, isSetMask, false>(dst, src, nullptr, mask, repeatTime, repeatParams);
}

template <typename T, const ExpConfig& config = DEFAULT_EXP_CONFIG>
__aicore__ inline void ExpImpl(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t count)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == ExpAlgo::PRECISION_1ULP_FTZ_TRUE
#else
        config.algo == ExpAlgo::INTRINSIC || config.algo == ExpAlgo::PRECISION_1ULP_FTZ_TRUE
#endif
    ) {
        static constexpr Reg::ExpSpecificMode mode = {Reg::MaskMergeMode::ZEROING, ExpAlgo::PRECISION_1ULP_FTZ_TRUE};
        constexpr auto func = Reg::Exp<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
    } else if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == ExpAlgo::INTRINSIC || config.algo == ExpAlgo::PRECISION_1ULP_FTZ_FALSE
#else
        config.algo == ExpAlgo::PRECISION_1ULP_FTZ_FALSE
#endif
    ) {
        static constexpr Reg::ExpSpecificMode mode = {Reg::MaskMergeMode::ZEROING, ExpAlgo::PRECISION_1ULP_FTZ_FALSE};
        constexpr auto func = Reg::Exp<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
    }
}

template <typename T, const LnConfig& config = DEFAULT_LN_CONFIG>
__aicore__ inline void LnImpl(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t count)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == LnAlgo::PRECISION_1ULP_FTZ_TRUE
#else
        config.algo == LnAlgo::INTRINSIC || config.algo == LnAlgo::PRECISION_1ULP_FTZ_TRUE
#endif
    ) {
        static constexpr Reg::LnSpecificMode mode = {Reg::MaskMergeMode::ZEROING, LnAlgo::PRECISION_1ULP_FTZ_TRUE};
        constexpr auto func = Reg::Ln<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
    } else if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == LnAlgo::INTRINSIC || config.algo == LnAlgo::PRECISION_1ULP_FTZ_FALSE
#else
        config.algo == LnAlgo::PRECISION_1ULP_FTZ_FALSE
#endif
    ) {
        static constexpr Reg::LnSpecificMode mode = {Reg::MaskMergeMode::ZEROING, LnAlgo::PRECISION_1ULP_FTZ_FALSE};
        constexpr auto func = Reg::Ln<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
    }
}

template <typename T>
__aicore__ inline void AbsImpl(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t count)
{
    static_assert(
        (SupportType<T, int8_t, half, int16_t, float, int32_t, int64_t>()),
        "current data type is not supported on current device!");
    if constexpr (SupportBytes<T, 8>()) {
        constexpr auto func = Reg::Abs<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumTwo>>;
        Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
    } else {
        constexpr auto func = Reg::Abs<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
    }
}

template <typename T, typename U, typename std::enable_if<!IsSameType<T, U>::value, bool>::type = true>
__simd_vf__ inline void AbsImpl(__ubuf__ T* dst, __ubuf__ U* src, const uint32_t count)
{
    static_assert(
        SupportType<T, half, float>() && SupportType<U, complex32, complex64>(),
        "current data type is not supported on current device!");
    static_assert(
        AscendC::Std::is_same<T, typename U::EleType>::value, "dst type do not match with src complex elements' type");
    Reg::RegTensor<U, Reg::RegTraitNumTwo> vSrcReg0;
    Reg::RegTensor<T, Reg::RegTraitNumOne> vDstReg0;
    uint32_t sreg = static_cast<uint32_t>(count);
    Reg::MaskReg preg;
    static constexpr uint32_t repeatStride =
        static_cast<uint32_t>(GetVecLen() / sizeof(U) * Reg::RegTraitNumTwo.REG_NUM);
    uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(count, repeatStride));
    for (uint16_t i = 0; i < repeatTime; ++i) {
        preg = Reg::UpdateMask<U, Reg::RegTraitNumTwo>(sreg);
        Reg::LoadAlign(vSrcReg0, src + i * repeatStride);
        Reg::Abs<
            T, U, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumOne>,
            Reg::RegTensor<U, Reg::RegTraitNumTwo>>(vDstReg0, vSrcReg0, preg);
        Reg::StoreAlign(dst + i * repeatStride, vDstReg0, preg);
    }
}

template <typename T, const ReciprocalConfig& config = DEFAULT_RECIPROCAL_CONFIG>
__aicore__ inline void ReciprocalImpl(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t count)
{
    static_assert(
        (SupportType<T, half, float, int64_t, uint64_t>()), "current data type is not supported on current device!");

    if constexpr (SupportType<T, half, float>()) {
        if constexpr (
#if !defined(__ASC_FTZ__)
            config.algo == ReciprocalAlgo::PRECISION_1ULP_FTZ_TRUE
#else
            config.algo == ReciprocalAlgo::INTRINSIC || config.algo == ReciprocalAlgo::PRECISION_1ULP_FTZ_TRUE
#endif
        ) {
            static constexpr Reg::DivSpecificMode mode = {
                Reg::MaskMergeMode::ZEROING, false, DivAlgo::PRECISION_1ULP_FTZ_TRUE};
            constexpr auto func = RegReciprocal::Reciprocal<T, Reg::RegTensor<T>, false, &mode>;
            Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
        } else if constexpr (
#if !defined(__ASC_FTZ__)
            config.algo == ReciprocalAlgo::INTRINSIC || config.algo == ReciprocalAlgo::PRECISION_1ULP_FTZ_FALSE
#else
            config.algo == ReciprocalAlgo::PRECISION_1ULP_FTZ_FALSE
#endif
        ) {
            constexpr auto func = RegReciprocal::Reciprocal<T, Reg::RegTensor<T>, true>;
            Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
        }
    } else {
        Internal::VecUnaryLevel2ImplB64<T>(dst, src, count);
    }
}

template <typename T, const SqrtConfig& config = DEFAULT_SQRT_CONFIG>
__aicore__ inline void SqrtImpl(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t count)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == SqrtAlgo::PRECISION_1ULP_FTZ_TRUE
#else
        config.algo == SqrtAlgo::INTRINSIC || config.algo == SqrtAlgo::PRECISION_1ULP_FTZ_TRUE
#endif
    ) {
        static constexpr Reg::SqrtSpecificMode mode = {
            Reg::MaskMergeMode::ZEROING, false, SqrtAlgo::PRECISION_1ULP_FTZ_TRUE};
        constexpr auto func = Reg::Sqrt<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
    } else if constexpr (config.algo == SqrtAlgo::FAST_INVERSE) {
        static constexpr Reg::SqrtSpecificMode mode = {Reg::MaskMergeMode::ZEROING, true, SqrtAlgo::FAST_INVERSE};
        constexpr auto func = Reg::Sqrt<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
    } else if constexpr (config.algo == SqrtAlgo::PRECISION_0ULP_FTZ_FALSE) {
        static constexpr Reg::SqrtSpecificMode mode = {
            Reg::MaskMergeMode::ZEROING, false, SqrtAlgo::PRECISION_0ULP_FTZ_FALSE};
        constexpr auto func = Reg::Sqrt<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
    } else if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == SqrtAlgo::INTRINSIC || config.algo == SqrtAlgo::PRECISION_1ULP_FTZ_FALSE
#else
        config.algo == SqrtAlgo::PRECISION_1ULP_FTZ_FALSE
#endif
    ) {
        static constexpr Reg::SqrtSpecificMode mode = {
            Reg::MaskMergeMode::ZEROING, false, SqrtAlgo::PRECISION_1ULP_FTZ_FALSE};
        constexpr auto func = Reg::Sqrt<T, &mode, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
    }
}

template <typename T>
__aicore__ inline void RsqrtPrecisionModeImpl(__ubuf__ T* dst, __ubuf__ T* src, const int32_t& count)
{
    if constexpr (IsSameType<T, half>::value) {
        constexpr auto func = RegRsqrt::Rsqrt<T, Reg::RegTensor<T>, true>;
        Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
    } else {
        Internal::VecUnaryLevel2ImplFloat<T>(dst, src, count);
    }
}

template <typename T, const RsqrtConfig& config = DEFAULT_RSQRT_CONFIG>
__aicore__ inline void RsqrtImpl(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t count)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == RsqrtAlgo::PRECISION_1ULP_FTZ_TRUE
#else
        config.algo == RsqrtAlgo::INTRINSIC || config.algo == RsqrtAlgo::PRECISION_1ULP_FTZ_TRUE
#endif
    ) {
        static constexpr Reg::SqrtSpecificMode sqrtMode = {
            Reg::MaskMergeMode::ZEROING, false, SqrtAlgo::PRECISION_1ULP_FTZ_TRUE};
        static constexpr Reg::DivSpecificMode divMode = {
            Reg::MaskMergeMode::ZEROING, false, DivAlgo::PRECISION_1ULP_FTZ_TRUE};
        constexpr auto func = RegRsqrt::Rsqrt<T, Reg::RegTensor<T>, false, &sqrtMode, &divMode>;
        Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
    } else {
        RsqrtPrecisionModeImpl(dst, src, count);
    }
}

template <typename T>
__aicore__ inline void NotImpl(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t count)
{
    static_assert(
        (SupportType<T, int8_t, uint8_t, half, float, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t>()),
        "current data type is not supported on current device!");
    if constexpr (SupportBytes<T, 8>()) {
        constexpr auto func = Reg::Not<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumTwo>>;
        Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
    } else {
        constexpr auto func = Reg::Not<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
    }
}

template <typename T>
__aicore__ inline void ReluImpl(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t count)
{
    static_assert(
        (SupportType<T, half, float, int32_t, int64_t>()), "current data type is not supported on current device!");
    if constexpr (SupportBytes<T, 8>()) {
        constexpr auto func = Reg::Relu<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumTwo>>;
        Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
    } else {
        constexpr auto func = Reg::Relu<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
    }
}

/* **************************************************************************************************
 * Neg                                            *
 * ************************************************************************************************* */
// Neg::Level 2
template <typename T>
__aicore__ inline void NegImpl(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t count)
{
    static_assert(
        (SupportType<T, int8_t, int16_t, int32_t, half, float, int64_t>()),
        "current data type is not supported on current device!");
    if constexpr (SupportBytes<T, 8>()) {
        constexpr auto func = Reg::Neg<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumTwo>>;
        Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
    } else {
        constexpr auto func = Reg::Neg<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
    }
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_UNARY_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_UNARY_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_UNARY_IMPL_H__
#endif
