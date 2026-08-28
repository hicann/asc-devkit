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
 * \file kernel_reg_compute_vec_unary_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_3510/kernel_reg_compute_vec_unary_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_vec_unary_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_UNARY_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_VEC_UNARY_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_VEC_UNARY_IMPL_H

#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_struct_intf.h"
#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_vec_binary_intf.h"
#include "kernel_reg_compute_common_impl.h"
#include "../../../../include/utils/std/type_traits.h"

namespace AscendC {
namespace Reg {
namespace Internal {
__aicore__ inline constexpr SqrtSpecificMode GetSqrtSpecificMode(MaskMergeMode mrgMode)
{
    return {.mrgMode = mrgMode, .precisionMode = false, .algo = SqrtAlgo::INTRINSIC};
}

__aicore__ inline constexpr SqrtSpecificMode GetSqrtSpecificMode(const SqrtSpecificMode* sprMode)
{
    return {.mrgMode = sprMode->mrgMode, .precisionMode = sprMode->precisionMode, .algo = sprMode->algo};
}

__aicore__ inline constexpr ExpSpecificMode GetExpSpecificMode(MaskMergeMode mrgMode)
{
    return {.mrgMode = mrgMode, .algo = ExpAlgo::INTRINSIC};
}

__aicore__ inline constexpr ExpSpecificMode GetExpSpecificMode(const ExpSpecificMode* sprMode)
{
    return {.mrgMode = sprMode->mrgMode, .algo = sprMode->algo};
}

__aicore__ inline constexpr LnSpecificMode GetLnSpecificMode(MaskMergeMode mrgMode)
{
    return {.mrgMode = mrgMode, .algo = LnAlgo::INTRINSIC};
}

__aicore__ inline constexpr LnSpecificMode GetLnSpecificMode(const LnSpecificMode* sprMode)
{
    return {.mrgMode = sprMode->mrgMode, .algo = sprMode->algo};
}

__aicore__ inline constexpr LogSpecificMode GetLogSpecificMode(MaskMergeMode mrgMode)
{
    return {.mrgMode = mrgMode, .algo = LogAlgo::INTRINSIC};
}

__aicore__ inline constexpr LogSpecificMode GetLogSpecificMode(const LogSpecificMode* sprMode)
{
    return {.mrgMode = sprMode->mrgMode, .algo = sprMode->algo};
}

__aicore__ inline constexpr Log2SpecificMode GetLog2SpecificMode(MaskMergeMode mrgMode)
{
    return {.mrgMode = mrgMode, .algo = Log2Algo::INTRINSIC};
}

__aicore__ inline constexpr Log2SpecificMode GetLog2SpecificMode(const Log2SpecificMode* sprMode)
{
    return {.mrgMode = sprMode->mrgMode, .algo = sprMode->algo};
}

__aicore__ inline constexpr Log10SpecificMode GetLog10SpecificMode(MaskMergeMode mrgMode)
{
    return {.mrgMode = mrgMode, .algo = Log10Algo::INTRINSIC};
}

__aicore__ inline constexpr Log10SpecificMode GetLog10SpecificMode(const Log10SpecificMode* sprMode)
{
    return {.mrgMode = sprMode->mrgMode, .algo = sprMode->algo};
}
} // namespace Internal
template <MaskMergeMode mode = MaskMergeMode::ZEROING, typename T>
__simd_callee__ inline void AbsB64Impl(T& dstReg, T& srcReg, MaskReg& mask)
{
    using ActualT = typename T::ActualT;
    static_assert(sizeof(ActualT) == 8, "AbsB64Impl data type should be B64");
    static_assert(CheckRegTrait<T, RegTraitNumTwo>(), "AbsB64Impl T should be RegTraitNumTwo");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    RegTensor<int32_t, RegTraitNumOne> zeroReg, lowReg, highReg;
    MaskReg carryMask, carryLow, carryHigh;
    vbr(zeroReg, 0);
    vcmp_lt(carryMask, (RegTensor<int32_t>&)srcReg.reg[1], zeroReg, mask);
    Sub(carryLow, lowReg, zeroReg, (RegTensor<int32_t>&)srcReg.reg[0], carryMask);
    SubC(carryHigh, highReg, zeroReg, (RegTensor<int32_t>&)srcReg.reg[1], carryLow, carryMask);
    vsel((RegTensor<int32_t>&)dstReg.reg[0], lowReg, (RegTensor<int32_t>&)srcReg.reg[0], carryMask);
    vsel((RegTensor<int32_t>&)dstReg.reg[1], highReg, (RegTensor<int32_t>&)srcReg.reg[1], carryMask);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void AbsImpl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, int8_t, int16_t, int32_t, half, float, int64_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Abs api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (sizeof(ActualT) != 8) {
        vabs(dstReg, srcReg, mask, modeValue);
    } else {
        if constexpr (CheckRegTrait<U, RegTraitNumOne>()) {
            MaskReg maskTrait2;
            MaskPack(maskTrait2, mask);
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
            B64TraitOneToTraitTwo(traitTwoSrcReg, srcReg);
            AbsB64Impl<mode>(traitTwoDstReg, traitTwoSrcReg, maskTrait2);
            B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
        } else if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            U dstTemp;
            AbsB64Impl<mode>(dstTemp, srcReg, mask);
            dstReg = dstTemp;
        }
    }
}

template <
    typename T = DefaultType, typename U = DefaultType, auto mode = MaskMergeMode::ZEROING, typename S, typename V>
__simd_callee__ inline void ComplexAbsKernel(S& dstReg, V& srcReg, MaskReg& mask)
{
    using ActualU = typename V::ActualT;
    static_assert(CheckRegTrait<V, RegTraitNumTwo>(), "V should be RegTraitNumTwo");
    static_assert(CheckRegTrait<S, RegTraitNumOne>(), "S should be RegTraitNumOne");
    static_assert(
        Std::is_same_v<typename S::ActualT, typename ActualU::EleType>,
        "dst type do not match with src complex elements' type");

    RegTensor<typename ActualU::EleType>& srcReal = (RegTensor<typename ActualU::EleType>&)srcReg.reg[0];
    RegTensor<typename ActualU::EleType>& srcImag = (RegTensor<typename ActualU::EleType>&)srcReg.reg[1];
    RegTensor<typename ActualU::EleType> srcRealSquare;
    RegTensor<typename ActualU::EleType> srcImagSquare;

    Mul<T, mode, RegTensor<typename ActualU::EleType>>(srcRealSquare, srcReal, srcReal, mask);
    Mul<T, mode, RegTensor<typename ActualU::EleType>>(srcImagSquare, srcImag, srcImag, mask);
    Add<T, mode, RegTensor<typename ActualU::EleType>>(srcRealSquare, srcRealSquare, srcImagSquare, mask);
    Sqrt<typename ActualU::EleType, mode, RegTensor<typename ActualU::EleType>>(dstReg, srcRealSquare, mask);
}

template <
    typename T = DefaultType, typename U = DefaultType, auto mode = MaskMergeMode::ZEROING, typename S, typename V>
__simd_callee__ inline void AbsImpl(S& dstReg, V& srcReg, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(Std::is_same_v<U, DefaultType> || Std::is_same_v<U, ActualU>, "U type is not correct!");
    static_assert(
        SupportType<ActualU, complex32, complex64>(),
        "current src data type ActualU is not supported on current device!");
    static_assert(
        SupportType<ActualT, half, float>(), "current dst data type ActualT is not supported on current device!");
    static_assert(
        Std::is_same_v<ActualT, typename ActualU::EleType>, "dst type do not match with src complex elements' type");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Abs api only supported Mode ZEROING on current device!");
    if constexpr (CheckRegTrait<V, RegTraitNumOne>()) {
        MaskReg maskTrait2;
        MaskPack(maskTrait2, mask);
        RegTensor<ActualU, RegTraitNumTwo> traitTwoSrcReg;
        TraitOneToTraitTwoTmpl<
            RegTensor<ActualU, RegTraitNumTwo>, RegTensor<ActualU, RegTraitNumOne>, typename ActualU::EleType>(
            traitTwoSrcReg, srcReg);
        ComplexAbsKernel<T, U, mode, RegTensor<ActualT, RegTraitNumOne>, RegTensor<ActualU, RegTraitNumTwo>>(
            dstReg, traitTwoSrcReg, maskTrait2);
    } else {
        ComplexAbsKernel<T, U, mode, RegTensor<ActualT, RegTraitNumOne>, RegTensor<ActualU, RegTraitNumTwo>>(
            dstReg, srcReg, mask);
    }
}

template <MaskMergeMode mode = MaskMergeMode::ZEROING, typename T>
__simd_callee__ inline void ReluB64Impl(T& dstReg, T& srcReg, MaskReg& mask)
{
    using ActualT = typename T::ActualT;
    static_assert(sizeof(ActualT) == 8, "T data type should be B64");
    static_assert(CheckRegTrait<T, RegTraitNumTwo>(), "T should be RegTraitNumTwo");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    Maxs(dstReg, srcReg, (int64_t)0, mask);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void ReluImpl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, int32_t, half, float, int64_t>(), "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Relu api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (sizeof(ActualT) != 8) {
        vrelu(dstReg, srcReg, mask, modeValue);
    } else {
        if constexpr (CheckRegTrait<U, RegTraitNumOne>()) {
            MaskReg maskTrait2;
            MaskPack(maskTrait2, mask);
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
            B64TraitOneToTraitTwo(traitTwoSrcReg, srcReg);
            ReluB64Impl<mode>(traitTwoDstReg, traitTwoSrcReg, maskTrait2);
            B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
        } else if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            U dstTemp;
            ReluB64Impl<mode>(dstTemp, srcReg, mask);
            dstReg = dstTemp;
        }
    }
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void ExpPrecision(U& dstReg, U& srcReg, MaskReg& maskSubnormal)
{
    U regTwo;
    U tmpReg0;
    using ActualT = typename U::ActualT;
    constexpr ExpSpecificMode sprMode = Internal::GetExpSpecificMode(mode);
    constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
    vdup(regTwo, 2, maskSubnormal, modeValue);
    vdiv(tmpReg0, srcReg, regTwo, maskSubnormal, modeValue);
    vexp(tmpReg0, tmpReg0, maskSubnormal, modeValue);
    vmul(dstReg, tmpReg0, tmpReg0, maskSubnormal, modeValue);
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void ExpImpl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportType<ActualT, half, float>(), "current data type is not supported on current device!");
    static_assert(
        IsSameType<decltype(mode), MaskMergeMode>::value || IsSameType<decltype(mode), const ExpSpecificMode*>::value,
        "mode type must be either MaskMergeMode or const ExpSpecificMode* ");
    constexpr ExpSpecificMode sprMode = Internal::GetExpSpecificMode(mode);
    static_assert(
        SupportEnum<sprMode.mrgMode, MaskMergeMode::ZEROING>(),
        "current Exp api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
    if constexpr (
        sprMode.algo == ExpAlgo::PRECISION_1ULP_FTZ_FALSE
#if !defined(__ASC_FTZ__)
        || sprMode.algo == ExpAlgo::INTRINSIC
#endif
    ) {
        MaskReg maskSubnormal;
        U tmpReg;
        if constexpr (SupportType<ActualT, float>()) {
            NotNumUnion subnormalBound;
            subnormalBound.i = 0x7fffff;
            vexp(dstReg, srcReg, mask, modeValue);
            vcmps_le(maskSubnormal, dstReg, subnormalBound.f, mask);
            ExpPrecision(tmpReg, srcReg, maskSubnormal);
            vsel(dstReg, tmpReg, dstReg, maskSubnormal);
        } else {
            NotNumUnion subnormalBound;
            subnormalBound.i = 0x3ff;
            vexp(dstReg, srcReg, mask, modeValue);
            vcmps_le(maskSubnormal, dstReg, subnormalBound.f, mask);
            ExpPrecision(tmpReg, srcReg, maskSubnormal);
            vsel(dstReg, tmpReg, dstReg, maskSubnormal);
        }
    } else {
        vexp(dstReg, srcReg, mask, modeValue);
    }
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void SqrtFastInverseImpl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    constexpr SqrtSpecificMode sprMode = Internal::GetSqrtSpecificMode(mode);
    constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
    /*
     * Improves Reg with high precision mode by using fast_inverse approach with following formula.
     * bool p;
     * p = (b < 1);
     * if (p)
     *     b = b*16777216.0f;  // x = x*2**24, get rid of subnormal
     * float x = errrsqrt(b);  // rsqrt
     * float x1 = x*x;
     * float x2 = 1-b*x1;
     * //float x3 = x2*x*0.5;
     * x = x + x2*x*0.5;
     * x1 = x*b;
     * float err = b - x1*x1;
     * x2 = x*0.5f;
     * x2 = x2*err + x1;
     * if (p)
     *     x2 = x2*0.000244140625f; //x2 = x2 * 2**(-12), 返回input是subnormal的值
     * if (std::isinf(b) || b==0)
     *     x2 = b;
     * return x2;
     */

    constexpr float subnormalBound = 1;
    constexpr float halfFactor = 0.5f;
    constexpr float negOne = -1.0f;
    constexpr float multiplyFactor0 = 16777216.0f;
    constexpr float multiplyFactor1 = 0.000244140625f;
    constexpr uint32_t posInf = 0x7f800000u;
    constexpr uint32_t negZero = 0x80000000u;
    RegTensor<ActualT> regOne;
    RegTensor<ActualT> tmpReg;
    RegTensor<ActualT> errReg;
    RegTensor<ActualT> resReg;
    RegTensor<ActualT> dstRegCopy;
    RegTensor<ActualT> srcRegCopy = srcReg;
    RegTensor<uint32_t> regNegOne;
    RegTensor<uint32_t> zeroReg;

    MaskReg cmpMaskReg;
    MaskReg isInfPreg;
    MaskReg isZeroPreg;
    MaskReg maskFull;
    maskFull = pset_b8(PAT_ALL);

    vcmps_lt(cmpMaskReg, srcRegCopy, subnormalBound, mask);
    vmuls(tmpReg, srcRegCopy, multiplyFactor0, mask, modeValue);
    vsel(srcRegCopy, tmpReg, srcRegCopy, cmpMaskReg);

    vdup(regOne, 1.0f, maskFull, modeValue);
    vsqrt(tmpReg, srcRegCopy, mask, modeValue);
    vdiv(dstRegCopy, regOne, tmpReg, mask, modeValue);

    vmuls(tmpReg, dstRegCopy, negOne, mask, modeValue);     // -x
    vmul(errReg, dstRegCopy, srcRegCopy, mask, modeValue);  // b*x
    vmula(regOne, errReg, tmpReg, mask, modeValue);         // x2 = 1-b*x*x
    vmuls(tmpReg, dstRegCopy, halfFactor, mask, modeValue); // 0.5x
    vmula(dstRegCopy, regOne, tmpReg, mask, modeValue);     // x = x + x2*0.5x

    vmul(resReg, dstRegCopy, srcRegCopy, mask, modeValue);  // x1 = x*b
    vmuls(tmpReg, resReg, negOne, mask, modeValue);         // -x1
    vmov(errReg, srcRegCopy);                               // err = b
    vmula(errReg, resReg, tmpReg, mask, modeValue);         // err = b - x1*x1
    vmuls(tmpReg, dstRegCopy, halfFactor, mask, modeValue); // 0.5x
    vmadd(tmpReg, errReg, resReg, mask, modeValue);         // x2 = x2*err + x1

    vmuls(dstRegCopy, tmpReg, multiplyFactor1, mask, modeValue);
    vsel(tmpReg, dstRegCopy, tmpReg, cmpMaskReg);

    vcmps_eq(isInfPreg, (vector_u32&)srcRegCopy, posInf, mask);
    vdup(regNegOne, negZero, maskFull, modeValue);
    vor(zeroReg, (vector_u32&)srcRegCopy, regNegOne, mask, modeValue);
    vcmps_eq(isZeroPreg, zeroReg, negZero, mask);
    por(cmpMaskReg, isZeroPreg, isInfPreg, mask);
    vsel(dstReg, srcRegCopy, tmpReg, cmpMaskReg);
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void SqrtImpl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(
        IsSameType<decltype(mode), MaskMergeMode>::value || IsSameType<decltype(mode), const SqrtSpecificMode*>::value,
        "mode type must be either MaskMergeMode or const SqrtSpecificMode* ");
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    constexpr SqrtSpecificMode sprMode = Internal::GetSqrtSpecificMode(mode);
    static_assert(SupportType<ActualT, half, float>(), "current data type is not supported on current device!");
    static_assert(
        SupportEnum<sprMode.mrgMode, MaskMergeMode::ZEROING>(),
        "current Sqrt api only supports Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();

    if constexpr (sprMode.precisionMode) {
        static_assert(
            SupportType<ActualT, float>(),
            "Reg Sqrt for high precision mode by using fast_inverse approach only supports float.");
        SqrtFastInverseImpl<ActualT, mode, U>(dstReg, srcReg, mask);
    } else {
        if constexpr (sprMode.algo == SqrtAlgo::PRECISION_0ULP_FTZ_FALSE || sprMode.algo == SqrtAlgo::FAST_INVERSE) {
            static_assert(
                SupportType<ActualT, float>(),
                "Reg Sqrt for high precision mode by using fast_inverse approach only supports float.");
            SqrtFastInverseImpl<ActualT, mode, U>(dstReg, srcReg, mask);
        } else {
#if !defined(__ASC_FTZ__)
            constexpr bool useHardwareSqrt = sprMode.algo == SqrtAlgo::PRECISION_1ULP_FTZ_TRUE;
#else
            constexpr bool useHardwareSqrt = sprMode.algo != SqrtAlgo::PRECISION_1ULP_FTZ_FALSE;
#endif
            if constexpr (useHardwareSqrt) {
                vsqrt(dstReg, srcReg, mask, modeValue);
            } else {
                RegTensor<ActualT> tmpReg;
                RegTensor<ActualT> dstRegCopy;
                RegTensor<ActualT> srcRegCopy = srcReg;
                MaskReg cmpMaskReg;
                if constexpr (IsSameType<ActualT, half>::value) {
                    HalfUnion multiplyFactor0;
                    multiplyFactor0.i = 0x6C00;
                    HalfUnion multiplyFactor1;
                    multiplyFactor1.i = 0x2400;
                    HalfUnion subnormalThreshold;
                    subnormalThreshold.i = 0x03FF;

                    vcmps_lt(cmpMaskReg, srcRegCopy, subnormalThreshold.f, mask);
                    vmuls(tmpReg, srcRegCopy, multiplyFactor0.f, mask, modeValue);
                    vsel(srcRegCopy, tmpReg, srcRegCopy, cmpMaskReg);
                    vsqrt(dstRegCopy, srcRegCopy, mask, modeValue);
                    vmuls(tmpReg, dstRegCopy, multiplyFactor1.f, mask, modeValue);
                    vsel(dstReg, tmpReg, dstRegCopy, cmpMaskReg);
                } else if constexpr (IsSameType<ActualT, float>::value) {
                    NotNumUnion multiplyFactor0;
                    multiplyFactor0.i = 0x4B800000;
                    NotNumUnion multiplyFactor1;
                    multiplyFactor1.i = 0x39800000;
                    NotNumUnion subnormalThreshold;
                    subnormalThreshold.i = 0x007FFFFF;

                    vcmps_lt(cmpMaskReg, srcRegCopy, subnormalThreshold.f, mask);
                    vmuls(tmpReg, srcRegCopy, multiplyFactor0.f, mask, modeValue);
                    vsel(srcRegCopy, tmpReg, srcRegCopy, cmpMaskReg);
                    vsqrt(dstRegCopy, srcRegCopy, mask, modeValue);
                    vmuls(tmpReg, dstRegCopy, multiplyFactor1.f, mask, modeValue);
                    vsel(dstReg, tmpReg, dstRegCopy, cmpMaskReg);
                }
            }
        }
    }
}

template <typename T = DefaultType, typename U, const LogSpecificMode* mode>
__simd_callee__ inline void LnCompute(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    constexpr LogSpecificMode sprMode = Internal::GetLogSpecificMode(mode);
    constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
    if constexpr (
        sprMode.algo == LogAlgo::PRECISION_1ULP_FTZ_FALSE
#if !defined(__ASC_FTZ__)
        || sprMode.algo == LogAlgo::INTRINSIC
#endif
    ) {
        if constexpr (IsSameType<ActualT, half>::value) {
            HalfUnion multiplyFactor;
            multiplyFactor.i = 0x6400; // 2^10
            HalfUnion subnormalThreshold;
            subnormalThreshold.i = 0x03FF;
            const half compensationFactor = -6.931471805599453094172; // -Ln(2^10);
            RegTensor<ActualT> tmpReg;
            RegTensor<ActualT> dstRegCopy;
            RegTensor<ActualT> srcRegCopy = srcReg;
            MaskReg cmpMaskReg;

            vcmps_lt(cmpMaskReg, srcRegCopy, subnormalThreshold.f, mask);
            vmuls(tmpReg, srcRegCopy, multiplyFactor.f, mask, modeValue);
            vsel(srcRegCopy, tmpReg, srcRegCopy, cmpMaskReg);
            vln(dstRegCopy, srcRegCopy, mask, modeValue);
            vadds(tmpReg, dstRegCopy, compensationFactor, mask, modeValue);
            vsel(dstReg, tmpReg, dstRegCopy, cmpMaskReg);
        } else {
            NotNumUnion multiplyFactor;
            multiplyFactor.i = 0x4B000000; // 2^23;
            NotNumUnion subnormalThreshold;
            subnormalThreshold.i = 0x007FFFFF;
            constexpr float compensationFactor = -15.9423851528787421; // -Ln(2^23);
            RegTensor<ActualT> tmpReg;
            RegTensor<ActualT> dstRegCopy;
            RegTensor<ActualT> srcRegCopy = srcReg;
            MaskReg cmpMaskReg;

            vcmps_lt(cmpMaskReg, srcRegCopy, subnormalThreshold.f, mask);
            vmuls(tmpReg, srcRegCopy, multiplyFactor.f, mask, modeValue);
            vsel(srcRegCopy, tmpReg, srcRegCopy, cmpMaskReg);
            vln(dstRegCopy, srcRegCopy, mask, modeValue);
            vadds(tmpReg, dstRegCopy, compensationFactor, mask, modeValue);
            vsel(dstReg, tmpReg, dstRegCopy, cmpMaskReg);
        }
    } else {
        vln(dstReg, srcReg, mask, modeValue);
    }
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void LogImpl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportType<ActualT, half, float>(), "current data type is not supported on current device!");
    static_assert(
        IsSameType<decltype(mode), MaskMergeMode>::value || IsSameType<decltype(mode), const LogSpecificMode*>::value ||
            IsSameType<decltype(mode), const LnSpecificMode*>::value,
        "mode type must be MaskMergeMode or const LogSpecificMode* or const LnSpecificMode* ");
    if constexpr (IsSameType<decltype(mode), const LogSpecificMode*>::value) {
        constexpr LogSpecificMode sprMode = Internal::GetLogSpecificMode(mode);
        static_assert(
            SupportEnum<sprMode.mrgMode, MaskMergeMode::ZEROING>(),
            "current Log api only supports Mode ZEROING on current device!");
        LnCompute<T, U, mode>(dstReg, srcReg, mask);
    } else if constexpr (IsSameType<decltype(mode), const LnSpecificMode*>::value) {
        constexpr LnSpecificMode sprMode = Internal::GetLnSpecificMode(mode);
        static_assert(
            SupportEnum<sprMode.mrgMode, MaskMergeMode::ZEROING>(),
            "current Ln api only supports Mode ZEROING on current device!");
        if constexpr (sprMode.algo == LnAlgo::PRECISION_1ULP_FTZ_FALSE) {
            static constexpr AscendC::Reg::LogSpecificMode logMode = {
                MaskMergeMode::ZEROING, LogAlgo::PRECISION_1ULP_FTZ_FALSE};
            LnCompute<T, U, &logMode>(dstReg, srcReg, mask);
        } else if constexpr (sprMode.algo == LnAlgo::PRECISION_1ULP_FTZ_TRUE) {
            constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
            vln(dstReg, srcReg, mask, modeValue);
        } else {
            static constexpr AscendC::Reg::LogSpecificMode logMode = {MaskMergeMode::ZEROING, LogAlgo::INTRINSIC};
            LnCompute<T, U, &logMode>(dstReg, srcReg, mask);
        }
    } else {
#if !defined(__ASC_FTZ__)
        static constexpr AscendC::Reg::LogSpecificMode logMode = {
            MaskMergeMode::ZEROING, LogAlgo::PRECISION_1ULP_FTZ_FALSE};
        LnCompute<T, U, &logMode>(dstReg, srcReg, mask);
#else
        constexpr auto modeValue = GetMaskMergeMode<mode>();
        vln(dstReg, srcReg, mask, modeValue);
#endif
    }
}

template <MaskMergeMode mode = MaskMergeMode::ZEROING>
__simd_callee__ inline void LogXImpl(
    RegTensor<half>& dstReg, RegTensor<half>& srcReg, MaskReg& mask, const float lnXReciprocal)
{
    vector_f16 f16RegLow;
    vector_f16 f16RegHigh;
    vector_f32 f32RegLow;
    vector_f32 f32RegHigh;
    vector_bool MaskLow;
    vector_bool MaskHigh;
    vector_f16 tmpReg;
    constexpr auto patAll = std::integral_constant<::Pat, static_cast<::Pat>(Reg::MaskPattern::ALL)>();
    vector_bool maskAll = pset_b32(patAll);
    constexpr auto patVal = std::integral_constant<::Pat, static_cast<::Pat>(Reg::MaskPattern::H)>();
    vector_bool selMask = pset_b16(patVal);
    constexpr auto partModeEvenVal = std::integral_constant<::Part, static_cast<::Part>(RegLayout::ZERO)>();
    constexpr auto satModeValue =
        std::integral_constant<::RoundingSaturation, static_cast<::RoundingSaturation>(SatMode::NO_SAT)>();
    constexpr auto roundModeValue = std::integral_constant<::ROUND, GetRound<RoundMode::CAST_RINT>()>();
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    punpack(MaskLow, mask, LOWER);
    punpack(MaskHigh, mask, HIGHER);

    vintlv(f16RegLow, f16RegHigh, srcReg, srcReg);

    vcvt(f32RegLow, f16RegLow, maskAll, partModeEvenVal, modeValue);
    vcvt(f32RegHigh, f16RegHigh, maskAll, partModeEvenVal, modeValue);

    vln(f32RegLow, f32RegLow, maskAll, modeValue);
    vln(f32RegHigh, f32RegHigh, maskAll, modeValue);

    vmuls(f32RegLow, f32RegLow, lnXReciprocal, maskAll, modeValue);
    vmuls(f32RegHigh, f32RegHigh, lnXReciprocal, maskAll, modeValue);

    vcvt(f16RegLow, f32RegLow, MaskLow, roundModeValue, satModeValue, partModeEvenVal, modeValue);
    vcvt(f16RegHigh, f32RegHigh, MaskHigh, roundModeValue, satModeValue, partModeEvenVal, modeValue);

    vdintlv(f16RegLow, tmpReg, f16RegLow, tmpReg);
    vdintlv(f16RegHigh, tmpReg, tmpReg, f16RegHigh);
    vsel(dstReg, f16RegLow, f16RegHigh, selMask);
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void Log2Impl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportType<ActualT, half, float>(), "current data type is not supported on current device!");
    static_assert(
        IsSameType<decltype(mode), MaskMergeMode>::value || IsSameType<decltype(mode), const Log2SpecificMode*>::value,
        "mode type must be either MaskMergeMode or const Log2SpecificMode* ");
    constexpr Log2SpecificMode sprMode = Internal::GetLog2SpecificMode(mode);
    static_assert(
        SupportEnum<sprMode.mrgMode, MaskMergeMode::ZEROING>(),
        "current Log2 api only supports Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
    constexpr float ln2Reciprocal = 1.4426950408889634; // 1.0/Ln2;
    if constexpr (
        sprMode.algo == Log2Algo::PRECISION_1ULP_FTZ_FALSE
#if !defined(__ASC_FTZ__)
        || sprMode.algo == Log2Algo::INTRINSIC
#endif
    ) {
        if constexpr (SupportType<ActualT, half>()) {
            HalfUnion multiplyFactor;
            multiplyFactor.i = 0x6400; // 2^10
            HalfUnion subnormalThreshold;
            subnormalThreshold.i = 0x03FF;
            const half compensationFactor = -10; // -Log2(2^10);
            RegTensor<ActualT> tmpReg;
            RegTensor<ActualT> dstRegCopy;
            RegTensor<ActualT> srcRegCopy = srcReg;
            MaskReg cmpMaskReg;

            vcmps_lt(cmpMaskReg, srcRegCopy, subnormalThreshold.f, mask);
            vmuls(tmpReg, srcRegCopy, multiplyFactor.f, mask, modeValue);
            vsel(srcRegCopy, tmpReg, srcRegCopy, cmpMaskReg);
            LogXImpl<sprMode.mrgMode>(dstRegCopy, srcRegCopy, mask, ln2Reciprocal);
            vadds(tmpReg, dstRegCopy, compensationFactor, mask, modeValue);
            vsel(dstReg, tmpReg, dstRegCopy, cmpMaskReg);
        } else {
            NotNumUnion multiplyFactor;
            multiplyFactor.i = 0x4B000000; // 2^23;
            NotNumUnion subnormalThreshold;
            subnormalThreshold.i = 0x007FFFFF;
            constexpr float compensationFactor = -23; // -Log2(2^23);
            RegTensor<ActualT> tmpReg;
            RegTensor<ActualT> dstRegCopy;
            RegTensor<ActualT> srcRegCopy = srcReg;
            MaskReg cmpMaskReg;

            vcmps_lt(cmpMaskReg, srcRegCopy, subnormalThreshold.f, mask);
            vmuls(tmpReg, srcRegCopy, multiplyFactor.f, mask, modeValue);
            vsel(srcRegCopy, tmpReg, srcRegCopy, cmpMaskReg);
            vln(dstRegCopy, srcRegCopy, mask, modeValue);
            vmuls(dstRegCopy, dstRegCopy, ln2Reciprocal, mask, modeValue);
            vadds(tmpReg, dstRegCopy, compensationFactor, mask, modeValue);
            vsel(dstReg, tmpReg, dstRegCopy, cmpMaskReg);
        }
    } else {
        if constexpr (SupportType<ActualT, half>()) {
            LogXImpl<mode>(dstReg, srcReg, mask, ln2Reciprocal);
        } else {
            vln(dstReg, srcReg, mask, modeValue);
            vmuls(dstReg, dstReg, ln2Reciprocal, mask, modeValue);
        }
    }
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void Log10Impl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportType<ActualT, half, float>(), "current data type is not supported on current device!");
    static_assert(
        IsSameType<decltype(mode), MaskMergeMode>::value || IsSameType<decltype(mode), const Log10SpecificMode*>::value,
        "mode type must be either MaskMergeMode or const Log10SpecificMode* ");
    constexpr Log10SpecificMode sprMode = Internal::GetLog10SpecificMode(mode);
    static_assert(
        SupportEnum<sprMode.mrgMode, MaskMergeMode::ZEROING>(),
        "current Log10 api only supports Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
    constexpr float ln10Reciprocal = 0.43429448190325176; // 1.0/Ln10;
    if constexpr (
        sprMode.algo == Log10Algo::PRECISION_1ULP_FTZ_FALSE
#if !defined(__ASC_FTZ__)
        || sprMode.algo == Log10Algo::INTRINSIC
#endif
    ) {
        if constexpr (SupportType<ActualT, half>()) {
            HalfUnion multiplyFactor;
            multiplyFactor.i = 0x6400; // 2^10
            HalfUnion subnormalThreshold;
            subnormalThreshold.i = 0x03FF;
            const half compensationFactor = -3.01029995663981; // -Log10(2^10);
            RegTensor<ActualT> tmpReg;
            RegTensor<ActualT> dstRegCopy;
            RegTensor<ActualT> srcRegCopy = srcReg;
            MaskReg cmpMaskReg;

            vcmps_lt(cmpMaskReg, srcRegCopy, subnormalThreshold.f, mask);
            vmuls(tmpReg, srcRegCopy, multiplyFactor.f, mask, modeValue);
            vsel(srcRegCopy, tmpReg, srcRegCopy, cmpMaskReg);
            LogXImpl<sprMode.mrgMode>(dstRegCopy, srcRegCopy, mask, ln10Reciprocal);
            vadds(tmpReg, dstRegCopy, compensationFactor, mask, modeValue);
            vsel(dstReg, tmpReg, dstRegCopy, cmpMaskReg);
        } else {
            NotNumUnion multiplyFactor;
            multiplyFactor.i = 0x4B000000; // 2^23;
            NotNumUnion subnormalThreshold;
            subnormalThreshold.i = 0x007FFFFF;
            constexpr float compensationFactor = -6.923689900271567; // -Log10(2^23);
            RegTensor<ActualT> tmpReg;
            RegTensor<ActualT> dstRegCopy;
            RegTensor<ActualT> srcRegCopy = srcReg;
            MaskReg cmpMaskReg;

            vcmps_lt(cmpMaskReg, srcRegCopy, subnormalThreshold.f, mask);
            vmuls(tmpReg, srcRegCopy, multiplyFactor.f, mask, modeValue);
            vsel(srcRegCopy, tmpReg, srcRegCopy, cmpMaskReg);
            vln(dstRegCopy, srcRegCopy, mask, modeValue);
            vmuls(dstRegCopy, dstRegCopy, ln10Reciprocal, mask, modeValue);
            vadds(tmpReg, dstRegCopy, compensationFactor, mask, modeValue);
            vsel(dstReg, tmpReg, dstRegCopy, cmpMaskReg);
        }
    } else {
        if constexpr (SupportType<ActualT, half>()) {
            LogXImpl<mode>(dstReg, srcReg, mask, ln10Reciprocal);
        } else {
            vln(dstReg, srcReg, mask, modeValue);
            vmuls(dstReg, dstReg, ln10Reciprocal, mask, modeValue);
        }
    }
}

template <MaskMergeMode mode = MaskMergeMode::ZEROING, typename T>
__simd_callee__ inline void NegB64Impl(T& dstReg, T& srcReg, MaskReg& mask)
{
    using ActualT = typename T::ActualT;
    static_assert(sizeof(ActualT) == 8, "T data type should be B64");
    static_assert(CheckRegTrait<T, RegTraitNumTwo>(), "T should be RegTraitNumTwo");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    T zeroReg;
    Duplicate(zeroReg, (int64_t)0, mask);
    Sub(dstReg, zeroReg, srcReg, mask);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void NegImpl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, int8_t, int16_t, int32_t, int64_t, half, float>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Neg api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (sizeof(ActualT) != 8) {
        vneg(dstReg, srcReg, mask, modeValue);
    } else {
        if constexpr (CheckRegTrait<U, RegTraitNumOne>()) {
            MaskReg maskTrait2;
            MaskPack(maskTrait2, mask);
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
            B64TraitOneToTraitTwo(traitTwoSrcReg, srcReg);
            NegB64Impl<mode>(traitTwoDstReg, traitTwoSrcReg, maskTrait2);
            B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
        } else if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            NegB64Impl<mode>(dstReg, srcReg, mask);
        }
    }
}

template <MaskMergeMode mode = MaskMergeMode::ZEROING, typename T>
__simd_callee__ inline void NotB64Impl(T& dstReg, T& srcReg, MaskReg& mask)
{
    using ActualT = typename T::ActualT;
    static_assert(sizeof(ActualT) == 8, "NotB64Impl data type should be B64");
    static_assert(CheckRegTrait<T, RegTraitNumTwo>(), "NotB64Impl T should be RegTraitNumTwo");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vnot((RegTensor<uint32_t>&)dstReg.reg[0], (RegTensor<uint32_t>&)srcReg.reg[0], mask, modeValue);
    vnot((RegTensor<uint32_t>&)dstReg.reg[1], (RegTensor<uint32_t>&)srcReg.reg[1], mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void NotImpl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, uint64_t, int64_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Not api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (sizeof(ActualT) != 8) {
        vnot(dstReg, srcReg, mask, modeValue);
    } else {
        if constexpr (CheckRegTrait<U, RegTraitNumOne>()) {
            MaskReg maskTrait2;
            MaskPack(maskTrait2, mask);
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
            B64TraitOneToTraitTwo(traitTwoSrcReg, srcReg);
            NotB64Impl<mode>(traitTwoDstReg, traitTwoSrcReg, maskTrait2);
            B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
        } else if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            U dstTemp;
            NotB64Impl<mode>(dstTemp, srcReg, mask);
            dstReg = dstTemp;
        }
    }
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_VEC_UNARY_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_UNARY_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_UNARY_IMPL__
#endif
