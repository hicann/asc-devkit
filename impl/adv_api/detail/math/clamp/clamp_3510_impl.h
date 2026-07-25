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
 * \file clamp_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/clamp/clamp_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/clamp.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_CLAMP_CLAMP_C310_IMPL_H__
#endif
#ifndef IMPL_MATH_CLAMP_CLAMP_C310_IMPL_H
#define IMPL_MATH_CLAMP_CLAMP_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/adv_api/math/clamp_utils.h"
#include "../../common/check.h"

#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/clamp/clamp_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {
template <typename T, CLAMPMODE selMode, bool isReuseSource = false>
__simd_vf__ inline void ClampCompute(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, const T scalar, uint32_t calCount, const uint16_t repeatTimes)
{
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(T);
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<T> dstReg;
    Reg::MaskReg maskReg;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        maskReg = Reg::UpdateMask<T>(calCount);
        Reg::LoadAlign<T>(srcReg, srcUb + i * repeatElm);
        if constexpr (selMode == CLAMPMODE::CLAMP_MAX) {
            Reg::Mins(dstReg, srcReg, scalar, maskReg);
        } else {
            Reg::Maxs(dstReg, srcReg, scalar, maskReg);
        }
        Reg::StoreAlign<T>(dstUb + i * repeatElm, dstReg, maskReg);
    }
}
/* **************************************************************************************************
 * ClampMax                                           *
 * ************************************************************************************************* */
template <typename T, bool isReuseSource = false>
__aicore__ inline void ClampMaxImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const T scalar, const uint32_t calCount)
{
    if ASCEND_IS_AIC {
        return;
    }

    static_assert(SupportType<T, float, half>(), "ClampMax only support half/float data type on current device");

    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(sharedTmpBuffer, "sharedTmpBuffer", "VECIN/VECOUT/VECCALC");

    CheckCalCount(calCount, "calCount", srcTensor, "srcTensor", "ClampMax");
    CheckCalCount(calCount, "calCount", dstTensor, "dstTensor", "ClampMax");

    __ubuf__ T* srcUb = (__ubuf__ T*)srcTensor.GetPhyAddr();
    __ubuf__ T* dstUb = (__ubuf__ T*)dstTensor.GetPhyAddr();
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(T);
    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(calCount, repeatElm));
    ClampCompute<T, CLAMPMODE::CLAMP_MAX, isReuseSource>(dstUb, srcUb, scalar, calCount, repeatTimes);
}

/* **************************************************************************************************
 * ClampMin                                           *
 * ************************************************************************************************* */

template <typename T, bool isReuseSource = false>
__aicore__ inline void ClampMinImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const T scalar, const uint32_t calCount)
{
    if ASCEND_IS_AIC {
        return;
    }

    static_assert(SupportType<T, float, half>(), "ClampMin only support half/float data type on current device");

    CheckTensorPosition(dstTensor, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(sharedTmpBuffer, "sharedTmpBuffer", "VECIN/VECOUT/VECCALC");

    CheckCalCount(calCount, "calCount", srcTensor, "srcTensor", "ClampMin");
    CheckCalCount(calCount, "calCount", dstTensor, "dstTensor", "ClampMin");

    __ubuf__ T* srcUb = (__ubuf__ T*)srcTensor.GetPhyAddr();
    __ubuf__ T* dstUb = (__ubuf__ T*)dstTensor.GetPhyAddr();
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(T);
    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(calCount, repeatElm));
    ClampCompute<T, CLAMPMODE::CLAMP_MIN, isReuseSource>(dstUb, srcUb, scalar, calCount, repeatTimes);
}

/* **************************************************************************************************
 * Clamp                                           *
 * ************************************************************************************************* */
template <typename T, typename RegT, const Reg::RegTrait& Trait = Reg::RegTraitNumOne>
__simd_vf__ inline void ClampImplTensorScalarVF(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ T* min, const T max, uint16_t repeatTime, uint32_t count,
    uint32_t oneRepElm)
{
    RegT dstVreg;
    RegT srcVreg;
    RegT minVreg;
    Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<T, Trait>(count);
        Reg::LoadAlign(srcVreg, src + i * oneRepElm);
        Reg::LoadAlign(minVreg, min + i * oneRepElm);
        Reg::Max(dstVreg, srcVreg, minVreg, mask);
        Reg::Mins(dstVreg, dstVreg, max, mask);
        Reg::StoreAlign(dst + i * oneRepElm, dstVreg, mask);
    }
}

template <typename T, typename RegT, const Reg::RegTrait& Trait = Reg::RegTraitNumOne>
__simd_vf__ inline void ClampImplScalarTensorVF(
    __ubuf__ T* dst, __ubuf__ T* src, const T min, __ubuf__ T* max, uint16_t repeatTime, uint32_t count,
    uint32_t oneRepElm)
{
    RegT dstVreg;
    RegT srcVreg;
    RegT maxVreg;
    Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<T, Trait>(count);
        Reg::LoadAlign(srcVreg, src + i * oneRepElm);
        Reg::LoadAlign(maxVreg, max + i * oneRepElm);
        Reg::Maxs(dstVreg, srcVreg, min, mask);
        Reg::Min(dstVreg, dstVreg, maxVreg, mask);
        Reg::StoreAlign(dst + i * oneRepElm, dstVreg, mask);
    }
}

template <typename T, typename RegT, const Reg::RegTrait& Trait = Reg::RegTraitNumOne>
__simd_vf__ inline void ClampImplBothTensorVF(
    __ubuf__ T* dst, __ubuf__ T* src, __ubuf__ T* min, __ubuf__ T* max, uint16_t repeatTime, uint32_t count,
    uint32_t oneRepElm)
{
    RegT dstVreg;
    RegT srcVreg;
    RegT minVreg;
    RegT maxVreg;
    Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<T, Trait>(count);
        Reg::LoadAlign(srcVreg, src + i * oneRepElm);
        Reg::LoadAlign(minVreg, min + i * oneRepElm);
        Reg::LoadAlign(maxVreg, max + i * oneRepElm);
        Reg::Max(dstVreg, srcVreg, minVreg, mask);
        Reg::Min(dstVreg, dstVreg, maxVreg, mask);
        Reg::StoreAlign(dst + i * oneRepElm, dstVreg, mask);
    }
}

template <typename T, typename RegT, const Reg::RegTrait& Trait = Reg::RegTraitNumOne>
__simd_vf__ inline void ClampImplBothScalarVF(
    __ubuf__ T* dst, __ubuf__ T* src, const T min, const T max, uint16_t repeatTime, uint32_t count, uint32_t oneRepElm)
{
    RegT dstVreg;
    RegT srcVreg;
    Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<T, Trait>(count);
        Reg::LoadAlign(srcVreg, src + i * oneRepElm);
        Reg::Maxs(dstVreg, srcVreg, min, mask);
        Reg::Mins(dstVreg, dstVreg, max, mask);
        Reg::StoreAlign(dst + i * oneRepElm, dstVreg, mask);
    }
}

template <const ClampConfig& config = DEFAULT_CLAMP_CONFIG, typename T, typename U, typename S>
__aicore__ inline void ClampImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& min, const S& max, const uint32_t count)
{
    if ASCEND_IS_AIC {
        return;
    }
    static_assert(
        SupportType<
            T, uint8_t, int8_t, half, bfloat16_t, uint16_t, int16_t, float, uint32_t, int32_t, uint64_t, int64_t>(),
        "Clamp only support uint8_t/int8_t/half/bfloat16_t/uint16_t"
        "/int16_t/float/uint32_t/int32_t/uint64_t/int64_t data type on current device");
    CHECK_FUNC_HIGHLEVEL_API(Clamp, (T, U, S, config.isReuseSource), (dst, src, min, max, count));
    constexpr uint32_t CLAMP_B64_REPEAT_STRIDE = 2;
    if constexpr (TypeUtils::IsLocalTensorType<U>() && TypeUtils::IsLocalTensorType<S>()) {
        using ActualU = typename U::PrimType;
        using ActualS = typename S::PrimType;
        static_assert(Std::is_same_v<T, ActualU>, "The data type of T and ActualU should be the same");
        static_assert(Std::is_same_v<T, ActualS>, "The data type of T and ActualS should be the same");
        if constexpr (sizeof(T) == 8) {
            constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(T) * CLAMP_B64_REPEAT_STRIDE);
            uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(count, oneRepElm));
            using RegT = Reg::RegTensor<T, Reg::RegTraitNumTwo>;
            ClampImplBothTensorVF<T, RegT, Reg::RegTraitNumTwo>(
                (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), (__ubuf__ T*)min.GetPhyAddr(),
                (__ubuf__ T*)max.GetPhyAddr(), repeatTime, count, oneRepElm);
        } else {
            constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(T));
            uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(count, oneRepElm));
            using RegT = Reg::RegTensor<T>;
            ClampImplBothTensorVF<T, RegT>(
                (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), (__ubuf__ T*)min.GetPhyAddr(),
                (__ubuf__ T*)max.GetPhyAddr(), repeatTime, count, oneRepElm);
        }

    } else if constexpr (TypeUtils::IsLocalTensorType<U>() && TypeUtils::IsInnerDefaultType<S>()) {
        using ActualU = typename U::PrimType;
        static_assert(Std::is_same_v<T, ActualU>, "The data type of T and ActualU should be the same");
        static_assert(Std::is_same_v<T, S>, "The data type of T and S should be the same");
        if constexpr (sizeof(T) == 8) {
            constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(T) * CLAMP_B64_REPEAT_STRIDE);
            uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(count, oneRepElm));
            using RegT = Reg::RegTensor<T, Reg::RegTraitNumTwo>;
            ClampImplTensorScalarVF<T, RegT, Reg::RegTraitNumTwo>(
                (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), (__ubuf__ T*)min.GetPhyAddr(), max,
                repeatTime, count, oneRepElm);
        } else {
            constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(T));
            uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(count, oneRepElm));
            using RegT = Reg::RegTensor<T>;
            ClampImplTensorScalarVF<T, RegT>(
                (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), (__ubuf__ T*)min.GetPhyAddr(), max,
                repeatTime, count, oneRepElm);
            ;
        }

    } else if constexpr (TypeUtils::IsLocalTensorType<S>() && TypeUtils::IsInnerDefaultType<U>()) {
        using ActualS = typename S::PrimType;
        static_assert(Std::is_same_v<T, U>, "The data type of T and U should be the same");
        static_assert(Std::is_same_v<T, ActualS>, "The data type of T and ActualS should be the same");
        if constexpr (sizeof(T) == 8) {
            constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(T) * CLAMP_B64_REPEAT_STRIDE);
            uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(count, oneRepElm));
            using RegT = Reg::RegTensor<T, Reg::RegTraitNumTwo>;
            ClampImplScalarTensorVF<T, RegT, Reg::RegTraitNumTwo>(
                (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), min, (__ubuf__ T*)max.GetPhyAddr(),
                repeatTime, count, oneRepElm);
        } else {
            constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(T));
            uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(count, oneRepElm));
            using RegT = Reg::RegTensor<T>;
            ClampImplScalarTensorVF<T, RegT>(
                (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), min, (__ubuf__ T*)max.GetPhyAddr(),
                repeatTime, count, oneRepElm);
        }
    } else {
        static_assert(Std::is_same_v<T, U>, "The data type of T and U should be the same");
        static_assert(Std::is_same_v<T, S>, "The data type of T and S should be the same");
        if constexpr (sizeof(T) == 8) {
            constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(T) * CLAMP_B64_REPEAT_STRIDE);
            uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(count, oneRepElm));
            using RegT = Reg::RegTensor<T, Reg::RegTraitNumTwo>;
            ClampImplBothScalarVF<T, RegT, Reg::RegTraitNumTwo>(
                (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), min, max, repeatTime, count, oneRepElm);
        } else {
            constexpr uint32_t oneRepElm = static_cast<uint32_t>(GetVecLen() / sizeof(T));
            uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(count, oneRepElm));
            using RegT = Reg::RegTensor<T>;
            ClampImplBothScalarVF<T, RegT>(
                (__ubuf__ T*)dst.GetPhyAddr(), (__ubuf__ T*)src.GetPhyAddr(), min, max, repeatTime, count, oneRepElm);
        }
    }
}
} // namespace AscendC
#endif // IMPL_MATH_CLAMP_CLAMP_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_CLAMP_CLAMP_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_CLAMP_CLAMP_C310_IMPL_H__
#endif
