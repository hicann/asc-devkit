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
 * \file kernel_reg_compute_vec_binary_scalar_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_v516/kernel_reg_compute_vec_binary_scalar_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_vec_binary_scalar_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_BINARY_SCALAR_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_VEC_BINARY_SCALAR_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_VEC_BINARY_SCALAR_IMPL_H

#include "kernel_reg_compute_common_impl.h"
#include "../../../../include/utils/std/type_traits.h"
#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_vec_binary_intf.h"

namespace AscendC {
namespace Reg {
template <typename T = DefaultType, typename U, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
__simd_callee__ inline void AddsComplexTraitTwoImpl(S& dstReg, S& srcReg, const U& scalarValue, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    static_assert(CheckRegTrait<S, RegTraitNumTwo>(), "S should be RegTraitNumTwo");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    RegTensor<typename ActualT::EleType> realSrc1tReg;
    RegTensor<typename ActualT::EleType> imagSrc1Reg;
    ActualT scalarAux(scalarValue);
    Duplicate(realSrc1tReg, scalarAux.real, mask);
    Duplicate(imagSrc1Reg, scalarAux.imag, mask);

    vadd(
        (RegTensor<typename ActualT::EleType>&)dstReg.reg[0], (RegTensor<typename ActualT::EleType>&)srcReg.reg[0],
        realSrc1tReg, mask, modeValue);
    vadd(
        (RegTensor<typename ActualT::EleType>&)dstReg.reg[1], (RegTensor<typename ActualT::EleType>&)srcReg.reg[1],
        imagSrc1Reg, mask, modeValue);
}

template <typename T = DefaultType, typename U, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
__simd_callee__ inline void AddsComplexTraitOneImpl(S& dstReg, S& srcReg, const U& scalarValue, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    static_assert(CheckRegTrait<S, RegTraitNumOne>(), "S should be RegTraitNumOne");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    MaskReg maskTrait2;
    MaskPack(maskTrait2, mask);
    RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg;
    RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
    TraitOneToTraitTwoTmpl<
        RegTensor<ActualT, RegTraitNumTwo>, RegTensor<ActualT, RegTraitNumOne>, typename ActualT::EleType>(
        traitTwoSrcReg, srcReg);
    AddsComplexTraitTwoImpl(traitTwoDstReg, traitTwoSrcReg, scalarValue, maskTrait2);
    TraitTwoToTraitOneTmpl<
        RegTensor<ActualT, RegTraitNumOne>, RegTensor<ActualT, RegTraitNumTwo>, typename ActualT::EleType>(
        dstReg, traitTwoDstReg);
}

template <typename T = DefaultType, typename U, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
__simd_callee__ inline void AddsImpl(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<
            ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t, uint64_t, int64_t,
            complex32, complex64>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportType<
            U, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t, uint64_t, int64_t,
            complex32, complex64>(),
        "current scalarValue data type is not supported on current device!");
    static_assert(Std::is_convertible<U, ActualT>(), "scalarValue data type could be converted to RegTensor data type");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Adds api only supported Mode ZEROING on current device!");

    if constexpr (SupportType<ActualT, complex32>()) {
        if constexpr (CheckRegTrait<S, RegTraitNumTwo>()) {
            AddsComplexTraitTwoImpl(dstReg, srcReg, scalarValue, mask);
        } else {
            AddsComplexTraitOneImpl(dstReg, srcReg, scalarValue, mask);
        }
    } else if constexpr (sizeof(ActualT) == 8) {
        if constexpr (SupportType<ActualT, complex64>()) {
            if constexpr (CheckRegTrait<S, RegTraitNumTwo>()) {
                AddsComplexTraitTwoImpl(dstReg, srcReg, scalarValue, mask);
            } else {
                MaskReg maskTrait2;
                MaskPack(maskTrait2, mask);
                RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg;
                RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
                B64TraitOneToTraitTwo(traitTwoSrcReg, srcReg);
                AddsComplexTraitTwoImpl(traitTwoDstReg, traitTwoSrcReg, scalarValue, maskTrait2);
                B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
            }
        } else {
            S srcReg1;
            Duplicate(srcReg1, scalarValue, mask);
            Add(dstReg, srcReg, srcReg1, mask);
        }
    } else {
        constexpr auto modeValue = GetMaskMergeMode<mode>();
        vadds(dstReg, srcReg, scalarValue, mask, modeValue);
    }
}

template <typename T = DefaultType, typename U, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
__simd_callee__ inline void MulsKernel(S& dstReg, S& srcReg, const U& scalarValue, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    static_assert(CheckRegTrait<S, RegTraitNumTwo>(), "S should be RegTraitNumTwo");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    RegTensor<typename ActualT::EleType> src1Real;
    RegTensor<typename ActualT::EleType> src1Imag;
    U scalarAux(scalarValue);
    Duplicate(src1Real, scalarAux.real, mask);
    Duplicate(src1Imag, scalarAux.imag, mask);

    RegTensor<typename ActualT::EleType>& src0Real = (RegTensor<typename ActualT::EleType>&)srcReg.reg[0];
    RegTensor<typename ActualT::EleType>& src0Imag = (RegTensor<typename ActualT::EleType>&)srcReg.reg[1];
    RegTensor<typename ActualT::EleType>& dstReal = (RegTensor<typename ActualT::EleType>&)dstReg.reg[0];
    RegTensor<typename ActualT::EleType>& dstImag = (RegTensor<typename ActualT::EleType>&)dstReg.reg[1];
    RegTensor<typename ActualT::EleType> e;
    RegTensor<typename ActualT::EleType> f;
    RegTensor<typename ActualT::EleType> g;
    RegTensor<typename ActualT::EleType> h;
    vmul(e, src0Real, src1Real, mask, modeValue);
    vmul(f, src0Imag, src1Imag, mask, modeValue);
    vmul(g, src0Imag, src1Real, mask, modeValue);
    vmul(h, src0Real, src1Imag, mask, modeValue);
    vsub(dstReal, e, f, mask, modeValue);
    vadd(dstImag, g, h, mask, modeValue);
}

template <typename T = DefaultType, typename U, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
__simd_callee__ inline void MulsImpl(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<
            ActualT, uint16_t, int16_t, uint32_t, int32_t, half, float, uint64_t, int64_t, complex32, complex64>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportType<U, uint16_t, int16_t, uint32_t, int32_t, half, float, uint64_t, int64_t, complex32, complex64>(),
        "current scalarValue data type is not supported on current device!");
    static_assert(Std::is_convertible<U, ActualT>(), "scalarValue data type could be converted to RegTensor data type");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Muls api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (sizeof(ActualT) != 8) {
        if constexpr (SupportType<ActualT, complex32>()) {
            if constexpr (CheckRegTrait<S, RegTraitNumTwo>()) {
                MulsKernel<T, U, mode, RegTensor<ActualT, RegTraitNumTwo>>(dstReg, srcReg, scalarValue, mask);
            } else {
                MaskReg maskTrait2;
                MaskPack(maskTrait2, mask);
                RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg;
                RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
                B32TraitOneToTraitTwo(traitTwoSrcReg, srcReg);
                MulsKernel<T, U, mode, RegTensor<ActualT, RegTraitNumTwo>>(
                    traitTwoDstReg, traitTwoSrcReg, scalarValue, maskTrait2);
                B32TraitTwoToTraitOne(dstReg, traitTwoDstReg);
            }
        } else {
            vmuls(dstReg, srcReg, scalarValue, mask, modeValue);
        }
    } else {
        if constexpr (SupportType<ActualT, complex64>()) {
            if constexpr (CheckRegTrait<S, RegTraitNumTwo>()) {
                MulsKernel<T, U, mode, RegTensor<ActualT, RegTraitNumTwo>>(dstReg, srcReg, scalarValue, mask);
            } else {
                MaskReg maskTrait2;
                MaskPack(maskTrait2, mask);
                RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg;
                RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
                B64TraitOneToTraitTwo(traitTwoSrcReg, srcReg);
                MulsKernel<T, U, mode, RegTensor<ActualT, RegTraitNumTwo>>(
                    traitTwoDstReg, traitTwoSrcReg, scalarValue, maskTrait2);
                B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
            }
        } else {
            S srcReg1;
            Duplicate(srcReg1, scalarValue, mask);
            Mul(dstReg, srcReg, srcReg1, mask);
        }
    }
}

template <typename T = DefaultType, typename U, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
__simd_callee__ inline void MaxsImpl(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<
            ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t, uint64_t,
            int64_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportType<
            U, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t, uint64_t, int64_t>(),
        "current scalarValue data type is not supported on current device!");
    static_assert(Std::is_convertible<U, ActualT>(), "scalarValue data type could be converted to RegTensor data type");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Maxs api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (sizeof(ActualT) != 8) {
        vmaxs(dstReg, srcReg, scalarValue, mask, modeValue);
    } else {
        S srcReg1;
        Duplicate(srcReg1, scalarValue, mask);
        Max(dstReg, srcReg, srcReg1, mask);
    }
}

template <typename T = DefaultType, typename U, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
__simd_callee__ inline void MinsImpl(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<
            ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t, uint64_t,
            int64_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportType<
            U, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t, uint64_t, int64_t>(),
        "current scalarValue data type is not supported on current device!");
    static_assert(Std::is_convertible<U, ActualT>(), "scalarValue data type could be converted to RegTensor data type");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Mins api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (sizeof(ActualT) != 8) {
        vmins(dstReg, srcReg, scalarValue, mask, modeValue);
    } else {
        S srcReg1;
        Duplicate(srcReg1, scalarValue, mask);
        Min(dstReg, srcReg, srcReg1, mask);
    }
}

template <typename T = DefaultType, typename U, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
__simd_callee__ inline void ShiftLeftsImpl(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t>(),
        "current data type is not supported on current device!");
    static_assert(SupportType<U, int16_t>(), "current scalarValue data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current ShiftLefts api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (sizeof(ActualT) != 8) {
        constexpr auto modeValue = GetMaskMergeMode<mode>();
        vshls(dstReg, srcReg, scalarValue, mask, modeValue);
    } else {
        if constexpr (CheckRegTrait<S, RegTraitNumTwo>()) {
            S dstTemp;
            ShiftLeftsB64Impl(dstTemp, srcReg, scalarValue, mask);
            dstReg = dstTemp;
        } else if constexpr (CheckRegTrait<S, RegTraitNumOne>()) {
            MaskReg maskTrait2;
            MaskPack(maskTrait2, mask);
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg0;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
            B64TraitOneToTraitTwo(traitTwoSrcReg0, srcReg);
            ShiftLeftsB64Impl(traitTwoDstReg, traitTwoSrcReg0, scalarValue, maskTrait2);
            B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
        }
    }
}

template <typename T, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void ShiftLeftsB64Impl(U& dstReg, U& srcReg, T scalarValue, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    int16_t B32Width = 32;
    if constexpr (Std::is_same_v<ActualT, uint64_t>) {
        RegTensor<uint32_t> tmpReg0;
        RegTensor<uint32_t> tmpReg1;
        vshls(tmpReg0, (RegTensor<uint32_t>&)srcReg.reg[0], scalarValue, mask, modeValue);
        vshls(tmpReg1, (RegTensor<uint32_t>&)srcReg.reg[1], B32Width + scalarValue, mask, modeValue);
        Or((RegTensor<uint32_t>&)dstReg.reg[0], tmpReg0, tmpReg1, mask);
        vshrs(tmpReg0, (RegTensor<uint32_t>&)srcReg.reg[0], B32Width - scalarValue, mask, modeValue);
        vshls(tmpReg1, (RegTensor<uint32_t>&)srcReg.reg[1], scalarValue, mask, modeValue);
        Or((RegTensor<uint32_t>&)dstReg.reg[1], tmpReg0, tmpReg1, mask);
    } else if constexpr (Std::is_same_v<ActualT, int64_t>) {
        RegTensor<int32_t> tmpReg0;
        RegTensor<int32_t> tmpReg1;
        vshls((RegTensor<uint32_t>&)tmpReg0, (RegTensor<uint32_t>&)srcReg.reg[0], scalarValue, mask, modeValue);
        vshls(
            (RegTensor<uint32_t>&)tmpReg1, (RegTensor<uint32_t>&)srcReg.reg[1], B32Width + scalarValue, mask,
            modeValue);
        Or((RegTensor<int32_t>&)dstReg.reg[0], tmpReg0, tmpReg1, mask);
        vshrs(
            (RegTensor<uint32_t>&)tmpReg0, (RegTensor<uint32_t>&)srcReg.reg[0], B32Width - scalarValue, mask,
            modeValue);
        vshls(tmpReg1, (RegTensor<int32_t>&)srcReg.reg[1], scalarValue, mask, modeValue);
        Or((RegTensor<int32_t>&)dstReg.reg[1], tmpReg0, tmpReg1, mask);
    }
}

template <typename T = DefaultType, typename U, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
__simd_callee__ inline void ShiftRightsImpl(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t>(),
        "current data type is not supported on current device!");
    static_assert(SupportType<U, int16_t>(), "current scalarValue data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current ShiftRights api only supported Mode ZEROING on current device!");
    if constexpr (sizeof(ActualT) < 8) {
        constexpr auto modeValue = GetMaskMergeMode<mode>();
        vshrs(dstReg, srcReg, scalarValue, mask, modeValue);
    } else if constexpr (sizeof(ActualT) == 8) {
        if constexpr (CheckRegTrait<S, RegTraitNumTwo>()) {
            S dstTemp;
            ShiftRightsB64Impl(dstTemp, srcReg, scalarValue, mask);
            dstReg = dstTemp;
        } else if constexpr (CheckRegTrait<S, RegTraitNumOne>()) {
            MaskReg maskTrait2;
            MaskPack(maskTrait2, mask);
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg0;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
            B64TraitOneToTraitTwo(traitTwoSrcReg0, srcReg);
            ShiftRightsB64Impl(traitTwoDstReg, traitTwoSrcReg0, scalarValue, maskTrait2);
            B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
        }
    }
}

template <typename T, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void ShiftRightsB64Impl(U& dstReg, U& srcReg, T scalarValue, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    int16_t B32Width = 32;
    if constexpr (Std::is_same_v<ActualT, uint64_t>) {
        RegTensor<uint32_t> tmpReg0;
        RegTensor<uint32_t> tmpReg1;
        vshrs(tmpReg0, (RegTensor<uint32_t>&)srcReg.reg[0], scalarValue, mask, modeValue);
        vshls(tmpReg1, (RegTensor<uint32_t>&)srcReg.reg[1], B32Width - scalarValue, mask, modeValue);
        Or((RegTensor<uint32_t>&)dstReg.reg[0], tmpReg0, tmpReg1, mask);
        vshrs(tmpReg0, (RegTensor<uint32_t>&)srcReg.reg[0], B32Width + scalarValue, mask, modeValue);
        vshrs(tmpReg1, (RegTensor<uint32_t>&)srcReg.reg[1], scalarValue, mask, modeValue);
        Or((RegTensor<uint32_t>&)dstReg.reg[1], tmpReg0, tmpReg1, mask);
    } else if constexpr (Std::is_same_v<ActualT, int64_t>) {
        RegTensor<int32_t> tmpReg0;
        RegTensor<int32_t> tmpReg1;
        vshrs((RegTensor<uint32_t>&)tmpReg0, (RegTensor<uint32_t>&)srcReg.reg[0], scalarValue, mask, modeValue);
        vshls(
            (RegTensor<int32_t>&)tmpReg1, (RegTensor<int32_t>&)srcReg.reg[1], B32Width - scalarValue, mask, modeValue);
        Or((RegTensor<int32_t>&)dstReg.reg[0], tmpReg0, tmpReg1, mask);
        vshrs(
            (RegTensor<uint32_t>&)tmpReg0, (RegTensor<uint32_t>&)srcReg.reg[0], B32Width + scalarValue, mask,
            modeValue);
        vshrs(tmpReg1, (RegTensor<int32_t>&)srcReg.reg[1], scalarValue, mask, modeValue);
        Or((RegTensor<int32_t>&)dstReg.reg[1], tmpReg0, tmpReg1, mask);
    }
}

template <typename T = DefaultType, typename U, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S>
__simd_callee__ inline void LeakyReluImpl(S& dstReg, S& srcReg, U scalarValue, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportType<ActualT, half, float>(), "current data type is not supported on current device!");
    static_assert(SupportType<U, half, float>(), "current scalarValue data type is not supported on current device!");
    static_assert(Std::is_convertible<U, ActualT>(), "scalarValue data type could be converted to RegTensor data type");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current LeakyRelu api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vlrelu(dstReg, srcReg, scalarValue, mask, modeValue);
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_VEC_BINARY_SCALAR_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_BINARY_SCALAR_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_BINARY_SCALAR_IMPL__
#endif
