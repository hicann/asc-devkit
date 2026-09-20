/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
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
    "impl/basic/reg_compute/dav_5161/kernel_reg_compute_vec_unary_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_vec_unary_intf.h\"\" and use public functions or variables defined in interface headers files.")
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
} // namespace Internal

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void AbsImpl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, int8_t, int16_t, int32_t, half, float>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Abs api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vabs(dstReg, srcReg, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void ReluImpl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, int32_t, half, float>(), "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Relu api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vrelu(dstReg, srcReg, mask, modeValue);
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
    if constexpr (sprMode.algo == ExpAlgo::PRECISION_1ULP_FTZ_FALSE) {
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
    RegTensor<T> regOne;
    RegTensor<T> tmpReg;
    RegTensor<T> errReg;
    RegTensor<T> resReg;
    RegTensor<T> dstRegCopy;
    RegTensor<T> srcRegCopy = srcReg;
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
            SupportType<T, float>(),
            "Reg Sqrt for high precision mode by using fast_inverse approach only supports float.");
        SqrtFastInverseImpl<T, mode, U>(dstReg, srcReg, mask);
    } else {
        if constexpr (sprMode.algo == SqrtAlgo::PRECISION_0ULP_FTZ_FALSE) {
            static_assert(
                SupportType<T, float>(),
                "Reg Sqrt for high precision mode by using fast_inverse approach only supports float.");
            SqrtFastInverseImpl<T, mode, U>(dstReg, srcReg, mask);
        } else if constexpr (sprMode.algo == SqrtAlgo::PRECISION_1ULP_FTZ_FALSE) {
            RegTensor<T> tmpReg;
            RegTensor<T> dstRegCopy;
            RegTensor<T> srcRegCopy = srcReg;
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
        } else {
            vsqrt(dstReg, srcReg, mask, modeValue);
        }
    }
}

template <typename T = DefaultType, typename U, const LogSpecificMode* mode>
__simd_callee__ inline void LnCompute(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    constexpr LogSpecificMode sprMode = Internal::GetLogSpecificMode(mode);
    constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
    if constexpr (sprMode.algo == LogAlgo::PRECISION_1ULP_FTZ_FALSE) {
        if constexpr (IsSameType<ActualT, half>::value) {
            HalfUnion multiplyFactor;
            multiplyFactor.i = 0x6400; // 2^10
            HalfUnion subnormalThreshold;
            subnormalThreshold.i = 0x03FF;
            const half compensationFactor = -6.931471805599453094172; // -Ln(2^10);
            RegTensor<T> tmpReg;
            RegTensor<T> dstRegCopy;
            RegTensor<T> srcRegCopy = srcReg;
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
            RegTensor<T> tmpReg;
            RegTensor<T> dstRegCopy;
            RegTensor<T> srcRegCopy = srcReg;
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
        } else {
            static constexpr AscendC::Reg::LogSpecificMode logMode = {MaskMergeMode::ZEROING, LogAlgo::INTRINSIC};
            LnCompute<T, U, &logMode>(dstReg, srcReg, mask);
        }
    } else {
        constexpr auto modeValue = GetMaskMergeMode<mode>();
        vln(dstReg, srcReg, mask, modeValue);
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void NegImpl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, int8_t, int16_t, int32_t, half, float>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Neg api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vneg(dstReg, srcReg, mask, modeValue);
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_VEC_UNARY_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_UNARY_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_UNARY_IMPL__
#endif
