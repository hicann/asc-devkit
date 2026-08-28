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
 * \file kernel_reg_compute_vec_binary_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_3510/kernel_reg_compute_vec_binary_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_vec_binary_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_BINARY_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_VEC_BINARY_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_VEC_BINARY_IMPL_H

#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_vec_cmpsel_intf.h"
#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_vec_duplicate_intf.h"

namespace AscendC {
namespace Reg {
namespace Internal {
__aicore__ inline constexpr DivSpecificMode GetDivSpecificMode(MaskMergeMode mrgMode)
{
    return {.mrgMode = mrgMode, .precisionMode = false, .algo = DivAlgo::INTRINSIC};
}

__aicore__ inline constexpr DivSpecificMode GetDivSpecificMode(const DivSpecificMode* sprMode)
{
    return {.mrgMode = sprMode->mrgMode, .precisionMode = sprMode->precisionMode, .algo = sprMode->algo};
}
} // namespace Internal
template <typename T, MaskMergeMode mode, typename U>
__simd_callee__ inline void AddComplexTwoRegImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(CheckRegTrait<U, RegTraitNumTwo>(), "U should be RegTraitNumTwo");
    Add((RegTensor<typename ActualT::EleType>&)dstReg.reg[0], (RegTensor<typename ActualT::EleType>&)srcReg0.reg[0],
        (RegTensor<typename ActualT::EleType>&)srcReg1.reg[0], mask);
    Add((RegTensor<typename ActualT::EleType>&)dstReg.reg[1], (RegTensor<typename ActualT::EleType>&)srcReg0.reg[1],
        (RegTensor<typename ActualT::EleType>&)srcReg1.reg[1], mask);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void AddB64Impl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(sizeof(ActualT) == 8, "data type should be B64");
    static_assert(CheckRegTrait<U, RegTraitNumTwo>(), "U should be RegTraitNumTwo");
    if constexpr (SupportType<ActualT, complex64>()) {
        AddComplexTwoRegImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
    } else {
        MaskReg carryMask;
        MaskReg carrySrcMask;
        if constexpr (Std::is_same_v<ActualT, uint64_t>) {
            Add(carryMask, (RegTensor<uint32_t>&)dstReg.reg[0], (RegTensor<uint32_t>&)srcReg0.reg[0],
                (RegTensor<uint32_t>&)srcReg1.reg[0], mask);
            AddC(
                carrySrcMask, (RegTensor<uint32_t>&)dstReg.reg[1], (RegTensor<uint32_t>&)srcReg0.reg[1],
                (RegTensor<uint32_t>&)srcReg1.reg[1], carryMask, mask);
        } else if constexpr (Std::is_same_v<ActualT, int64_t>) {
            Add(carryMask, (RegTensor<int32_t>&)dstReg.reg[0], (RegTensor<int32_t>&)srcReg0.reg[0],
                (RegTensor<int32_t>&)srcReg1.reg[0], mask);
            AddC(
                carrySrcMask, (RegTensor<int32_t>&)dstReg.reg[1], (RegTensor<int32_t>&)srcReg0.reg[1],
                (RegTensor<int32_t>&)srcReg1.reg[1], carryMask, mask);
        }
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void AddComplex32OnetraitImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(sizeof(ActualT) == 4, "data type should be B32");
    static_assert(CheckRegTrait<U, RegTraitNumOne>(), "U should be RegTraitNumOne");
    MaskReg maskTrait2;
    MaskPack(maskTrait2, mask);
    RegTensor<ActualT, RegTraitNumTwo> addTraitTwoSrcReg0;
    RegTensor<ActualT, RegTraitNumTwo> addTraitTwoSrcReg1;
    RegTensor<ActualT, RegTraitNumTwo> addTraitTwoDstReg;
    B32TraitOneToTraitTwo(addTraitTwoSrcReg0, srcReg0);
    B32TraitOneToTraitTwo(addTraitTwoSrcReg1, srcReg1);
    AddComplexTwoRegImpl<T, mode, RegTensor<ActualT, RegTraitNumTwo>>(
        addTraitTwoDstReg, addTraitTwoSrcReg0, addTraitTwoSrcReg1, maskTrait2);
    B32TraitTwoToTraitOne(dstReg, addTraitTwoDstReg);
}

template <typename T, auto mode, typename U, auto func>
__simd_callee__ inline void CalTraitOneByTransToTraitTwo(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    MaskReg maskTrait2;
    MaskPack(maskTrait2, mask);
    RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg0;
    RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg1;
    RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
    B64TraitOneToTraitTwo(traitTwoSrcReg0, srcReg0);
    B64TraitOneToTraitTwo(traitTwoSrcReg1, srcReg1);
    func(traitTwoDstReg, traitTwoSrcReg0, traitTwoSrcReg1, maskTrait2);
    B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void AddOperator(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    if constexpr (SupportType<ActualT, complex32>()) {
        if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            AddComplexTwoRegImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
        } else {
            AddComplex32OnetraitImpl(dstReg, srcReg0, srcReg1, mask);
        }
    } else if constexpr (SupportBytes<ActualT, 8>()) {
        if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            U dstTemp;
            AddB64Impl(dstTemp, srcReg0, srcReg1, mask);
            dstReg = dstTemp;
        } else {
            CalTraitOneByTransToTraitTwo<T, mode, U, AddB64Impl<T, mode, Reg::RegTensor<ActualT, Reg::RegTraitNumTwo>>>(
                dstReg, srcReg0, srcReg1, mask);
        }
    } else {
        constexpr auto modeValue = GetMaskMergeMode<mode>();
        vadd(dstReg, srcReg0, srcReg1, mask, modeValue);
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void AddImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<
            ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t, uint64_t, int64_t,
            complex32, complex64>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Add api only supported Mode ZEROING/MERGING on current device!");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        AddOperator<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        U dstCopyReg;
        AddOperator<T, MaskMergeMode::ZEROING, U>(dstCopyReg, srcReg0, srcReg1, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void SubB64Impl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(sizeof(ActualT) == 8, "data type should be B64");
    static_assert(CheckRegTrait<U, RegTraitNumTwo>(), "U should be RegTraitNumTwo");
    MaskReg carryMask;
    MaskReg carrySrcMask;
    if constexpr (SupportType<ActualT, complex64>()) {
        SubComplex64Impl(dstReg, srcReg0, srcReg1, mask);
    } else {
        if constexpr (Std::is_same_v<ActualT, uint64_t>) {
            Sub(carryMask, (RegTensor<uint32_t>&)dstReg.reg[0], (RegTensor<uint32_t>&)srcReg0.reg[0],
                (RegTensor<uint32_t>&)srcReg1.reg[0], mask);
            SubC(
                carrySrcMask, (RegTensor<uint32_t>&)dstReg.reg[1], (RegTensor<uint32_t>&)srcReg0.reg[1],
                (RegTensor<uint32_t>&)srcReg1.reg[1], carryMask, mask);
        } else if constexpr (Std::is_same_v<ActualT, int64_t>) {
            Sub(carryMask, (RegTensor<int32_t>&)dstReg.reg[0], (RegTensor<int32_t>&)srcReg0.reg[0],
                (RegTensor<int32_t>&)srcReg1.reg[0], mask);
            SubC(
                carrySrcMask, (RegTensor<int32_t>&)dstReg.reg[1], (RegTensor<int32_t>&)srcReg0.reg[1],
                (RegTensor<int32_t>&)srcReg1.reg[1], carryMask, mask);
        }
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void SubComplex64Impl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(sizeof(ActualT) == 8, "data type should be B64");
    static_assert(CheckRegTrait<U, RegTraitNumTwo>(), "U should be RegTraitNumTwo");
    Sub((RegTensor<float>&)dstReg.reg[0], (RegTensor<float>&)srcReg0.reg[0], (RegTensor<float>&)srcReg1.reg[0], mask);
    Sub((RegTensor<float>&)dstReg.reg[1], (RegTensor<float>&)srcReg0.reg[1], (RegTensor<float>&)srcReg1.reg[1], mask);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void SubComplex32TwoImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(sizeof(ActualT) == 4, "data type should be B32");
    static_assert(CheckRegTrait<U, RegTraitNumTwo>(), "U should be RegTraitNumTwo");
    Sub((RegTensor<half>&)dstReg.reg[0], (RegTensor<half>&)srcReg0.reg[0], (RegTensor<half>&)srcReg1.reg[0], mask);
    Sub((RegTensor<half>&)dstReg.reg[1], (RegTensor<half>&)srcReg0.reg[1], (RegTensor<half>&)srcReg1.reg[1], mask);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void SubComplex32OnetraitImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(sizeof(ActualT) == 4, "data type should be B32");
    static_assert(CheckRegTrait<U, RegTraitNumOne>(), "U should be RegTraitNumOne");
    MaskReg maskTrait2;
    MaskPack(maskTrait2, mask);
    RegTensor<ActualT, RegTraitNumTwo> subTraitTwoSrcReg0;
    RegTensor<ActualT, RegTraitNumTwo> subTraitTwoSrcReg1;
    RegTensor<ActualT, RegTraitNumTwo> subTraitTwoDstReg;
    B32TraitOneToTraitTwo(subTraitTwoSrcReg0, srcReg0);
    B32TraitOneToTraitTwo(subTraitTwoSrcReg1, srcReg1);
    SubComplex32TwoImpl<T, mode, RegTensor<ActualT, RegTraitNumTwo>>(
        subTraitTwoDstReg, subTraitTwoSrcReg0, subTraitTwoSrcReg1, maskTrait2);
    B32TraitTwoToTraitOne(dstReg, subTraitTwoDstReg);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void SubImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<
            ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t, uint64_t, int64_t,
            complex32, complex64>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Sub api only supported Mode ZEROING on current device!");
    if constexpr (SupportType<ActualT, complex32>()) {
        if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            SubComplex32TwoImpl(dstReg, srcReg0, srcReg1, mask);
        } else {
            SubComplex32OnetraitImpl(dstReg, srcReg0, srcReg1, mask);
        }
    } else if constexpr (SupportBytes<ActualT, 8>()) {
        if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            U dstTemp;
            SubB64Impl(dstTemp, srcReg0, srcReg1, mask);
            dstReg = dstTemp;
        } else {
            CalTraitOneByTransToTraitTwo<T, mode, U, SubB64Impl<T, mode, Reg::RegTensor<ActualT, Reg::RegTraitNumTwo>>>(
                dstReg, srcReg0, srcReg1, mask);
        }
    } else {
        constexpr auto modeValue = GetMaskMergeMode<mode>();
        vsub(dstReg, srcReg0, srcReg1, mask, modeValue);
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void MulB64Impl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(sizeof(ActualT) == 8, "MulB64Impl data type should be B64");
    static_assert(CheckRegTrait<U, RegTraitNumTwo>(), "MulB64Impl U should be RegTraitNumTwo");
    static_assert(SupportEnum<mode, MaskMergeMode::ZEROING>(), "MulB64Impl only support Mode ZEROING");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (SupportType<ActualT, complex64>()) {
        MulComplex64Impl(dstReg, srcReg0, srcReg1, mask);
    } else {
        if constexpr (Std::is_same_v<ActualT, uint64_t>) {
            Mull(
                (RegTensor<uint32_t>&)dstReg.reg[0], (RegTensor<uint32_t>&)dstReg.reg[1],
                (RegTensor<uint32_t>&)srcReg0.reg[0], (RegTensor<uint32_t>&)srcReg1.reg[0], mask);
            vmula(
                (RegTensor<uint32_t>&)dstReg.reg[1], (RegTensor<uint32_t>&)srcReg0.reg[0],
                (RegTensor<uint32_t>&)srcReg1.reg[1], mask, modeValue);
            vmula(
                (RegTensor<uint32_t>&)dstReg.reg[1], (RegTensor<uint32_t>&)srcReg0.reg[1],
                (RegTensor<uint32_t>&)srcReg1.reg[0], mask, modeValue);
        } else if constexpr (Std::is_same_v<ActualT, int64_t>) {
            Mull(
                (RegTensor<uint32_t>&)dstReg.reg[0], (RegTensor<uint32_t>&)dstReg.reg[1],
                (RegTensor<uint32_t>&)srcReg0.reg[0], (RegTensor<uint32_t>&)srcReg1.reg[0], mask);
            vmula(
                (RegTensor<int32_t>&)dstReg.reg[1], (RegTensor<int32_t>&)srcReg0.reg[0],
                (RegTensor<int32_t>&)srcReg1.reg[1], mask, modeValue);
            vmula(
                (RegTensor<int32_t>&)dstReg.reg[1], (RegTensor<int32_t>&)srcReg0.reg[1],
                (RegTensor<int32_t>&)srcReg1.reg[0], mask, modeValue);
        }
    }
}

template <typename T, MaskMergeMode mode, typename U>
__simd_callee__ inline void ComplexMulKernel(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(CheckRegTrait<U, RegTraitNumTwo>(), "U should be RegTraitNumTwo");
    RegTensor<typename ActualT::EleType>& src0Real = (RegTensor<typename ActualT::EleType>&)srcReg0.reg[0];
    RegTensor<typename ActualT::EleType>& src0Imag = (RegTensor<typename ActualT::EleType>&)srcReg0.reg[1];
    RegTensor<typename ActualT::EleType>& src1Real = (RegTensor<typename ActualT::EleType>&)srcReg1.reg[0];
    RegTensor<typename ActualT::EleType>& src1Imag = (RegTensor<typename ActualT::EleType>&)srcReg1.reg[1];
    RegTensor<typename ActualT::EleType>& dstReal = (RegTensor<typename ActualT::EleType>&)dstReg.reg[0];
    RegTensor<typename ActualT::EleType>& dstImag = (RegTensor<typename ActualT::EleType>&)dstReg.reg[1];
    RegTensor<typename ActualT::EleType> e;
    RegTensor<typename ActualT::EleType> f;
    RegTensor<typename ActualT::EleType> g;
    RegTensor<typename ActualT::EleType> h;
    Mul(e, src0Real, src1Real, mask);
    Mul(f, src0Imag, src1Imag, mask);
    Mul(g, src0Imag, src1Real, mask);
    Mul(h, src0Real, src1Imag, mask);
    Sub(dstReal, e, f, mask);
    Add(dstImag, g, h, mask);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void MulComplex64Impl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(sizeof(ActualT) == 8, "data type should be B64");
    static_assert(CheckRegTrait<U, RegTraitNumTwo>(), "U should be RegTraitNumTwo");
    ComplexMulKernel<T, mode, Reg::RegTensor<ActualT, Reg::RegTraitNumTwo>>(dstReg, srcReg0, srcReg1, mask);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void MulComplex32TwoImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(sizeof(ActualT) == 4, "data type should be B32");
    static_assert(CheckRegTrait<U, RegTraitNumTwo>(), "U should be RegTraitNumTwo");
    ComplexMulKernel<T, mode, Reg::RegTensor<ActualT, Reg::RegTraitNumTwo>>(dstReg, srcReg0, srcReg1, mask);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void MulComplex32OnetraitImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(sizeof(ActualT) == 4, "data type should be B32");
    static_assert(CheckRegTrait<U, RegTraitNumOne>(), "U should be RegTraitNumOne");

    MaskReg maskTrait2;
    MaskPack(maskTrait2, mask);
    RegTensor<ActualT, RegTraitNumTwo> mulTraitTwoSrcReg0;
    RegTensor<ActualT, RegTraitNumTwo> mulTraitTwoSrcReg1;
    RegTensor<ActualT, RegTraitNumTwo> mulTraitTwoDstReg;

    B32TraitOneToTraitTwo(mulTraitTwoSrcReg0, srcReg0);
    B32TraitOneToTraitTwo(mulTraitTwoSrcReg1, srcReg1);
    MulComplex32TwoImpl(mulTraitTwoDstReg, mulTraitTwoSrcReg0, mulTraitTwoSrcReg1, maskTrait2);
    B32TraitTwoToTraitOne(dstReg, mulTraitTwoDstReg);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void MulImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<
            ActualT, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t, uint64_t, int64_t, complex32,
            complex64>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Mul api only supported Mode ZEROING on current device!");
    if constexpr (SupportType<ActualT, complex32>()) {
        if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            MulComplex32TwoImpl(dstReg, srcReg0, srcReg1, mask);
        } else {
            MulComplex32OnetraitImpl(dstReg, srcReg0, srcReg1, mask);
        }
    } else if constexpr (SupportBytes<ActualT, 8>()) {
        if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            U dstTemp;
            MulB64Impl(dstTemp, srcReg0, srcReg1, mask);
            dstReg = dstTemp;
        } else {
            CalTraitOneByTransToTraitTwo<T, mode, U, MulB64Impl<T, mode, Reg::RegTensor<ActualT, Reg::RegTraitNumTwo>>>(
                dstReg, srcReg0, srcReg1, mask);
        }
    } else {
        constexpr auto modeValue = GetMaskMergeMode<mode>();
        vmul(dstReg, srcReg0, srcReg1, mask, modeValue);
    }
}

template <typename T>
__simd_callee__ inline void AbsUsingS32(T& dstReg, T& srcReg0, MaskReg& mask)
{
    RegTensor<int32_t, RegTraitNumOne> vTmp1, vTmp2, vTmp3;
    MaskReg carry0, carry1, carry2;
    vbr(vTmp1, 0);
    vcmp_lt(carry0, (RegTensor<int32_t>&)srcReg0.reg[1], vTmp1, mask);
    vsubc(carry1, vTmp2, vTmp1, (RegTensor<int32_t>&)srcReg0.reg[0], carry0);
    vsubcs(carry2, vTmp3, vTmp1, (RegTensor<int32_t>&)srcReg0.reg[1], carry1, carry0);
    vsel((RegTensor<int32_t>&)dstReg.reg[0], vTmp2, (RegTensor<int32_t>&)srcReg0.reg[0], carry0);
    vsel((RegTensor<int32_t>&)dstReg.reg[1], vTmp3, (RegTensor<int32_t>&)srcReg0.reg[1], carry0);
}

template <typename T>
__simd_callee__ inline void VbrUsingU32(T& dstReg, uint32_t highScalar, uint32_t lowScalar)
{
    vbr((RegTensor<uint32_t>&)dstReg.reg[0], highScalar);
    vbr((RegTensor<uint32_t>&)dstReg.reg[1], lowScalar);
}

template <typename T, typename U>
__simd_callee__ inline void VcvtS642F32(T& vDst, U& vSrc)
{
    RegTensor<float, RegTraitNumOne> vDummy, vVcvt0Tmp1, vVcvt0Tmp2;
    RegTensor<int64_t, RegTraitNumTwo> vVcvt0Tmp0;

    vintlv(
        (RegTensor<int32_t>&)vVcvt0Tmp0.reg[0], (RegTensor<int32_t>&)vVcvt0Tmp0.reg[1],
        (RegTensor<int32_t>&)vSrc.reg[0], (RegTensor<int32_t>&)vSrc.reg[1]);
    MaskReg maskTmp = pset_b32(PAT_ALL);
    vcvt(vVcvt0Tmp1, (RegTensor<int64_t>&)vVcvt0Tmp0.reg[0], maskTmp, ROUND_R, PART_EVEN);
    vcvt(vVcvt0Tmp2, (RegTensor<int64_t>&)vVcvt0Tmp0.reg[1], maskTmp, ROUND_R, PART_EVEN);
    vdintlv(vDst, vDummy, vVcvt0Tmp1, vVcvt0Tmp2);
}

template <typename T, typename U>
__simd_callee__ inline void VcvtF322S64(T& vDst, U& vSrc)
{
    RegTensor<int64_t, RegTraitNumOne> vVcvt1Tmp0, vVcvt1Tmp1;
    RegTensor<float, RegTraitNumOne> vVcvt1Tmp2, vVcvt1Tmp3;
    vintlv(vVcvt1Tmp2, vVcvt1Tmp3, (RegTensor<float>&)vSrc, (RegTensor<float>&)vSrc);
    MaskReg maskTmp = pset_b32(PAT_ALL);
    vcvt(vVcvt1Tmp0, vVcvt1Tmp2, maskTmp, ROUND_Z, RS_DISABLE, PART_EVEN);
    vcvt(vVcvt1Tmp1, vVcvt1Tmp3, maskTmp, ROUND_Z, RS_DISABLE, PART_EVEN);
    vdintlv(
        (RegTensor<int32_t>&)vDst.reg[0], (RegTensor<int32_t>&)vDst.reg[1], (RegTensor<int32_t>&)vVcvt1Tmp0,
        (RegTensor<int32_t>&)vVcvt1Tmp1);
}

template <typename T>
__simd_callee__ inline void VnotInPlace(T& vReg, MaskReg& mask)
{
    vnot((RegTensor<uint32_t>&)vReg.reg[0], (RegTensor<uint32_t>&)vReg.reg[0], mask, MODE_ZEROING);
    vnot((RegTensor<uint32_t>&)vReg.reg[1], (RegTensor<uint32_t>&)vReg.reg[1], mask, MODE_ZEROING);
}

template <typename T, typename U, typename S>
__simd_callee__ inline void VmulUsingU32(T& vDst, U& vSrc0, S& vSrc1, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<MaskMergeMode::ZEROING>();
    Mull(
        (RegTensor<uint32_t>&)vDst.reg[0], (RegTensor<uint32_t>&)vDst.reg[1], (RegTensor<uint32_t>&)vSrc0.reg[0],
        (RegTensor<uint32_t>&)vSrc1.reg[0], mask);
    vmula(
        (RegTensor<uint32_t>&)vDst.reg[1], (RegTensor<uint32_t>&)vSrc0.reg[0], (RegTensor<uint32_t>&)vSrc1.reg[1], mask,
        modeValue);
    vmula(
        (RegTensor<uint32_t>&)vDst.reg[1], (RegTensor<uint32_t>&)vSrc0.reg[1], (RegTensor<uint32_t>&)vSrc1.reg[0], mask,
        modeValue);
}

template <typename T, typename U, typename S>
__simd_callee__ inline void B128Calc(T& vSrc0, U& vSrc1, S& vSrc2, MaskReg& mask)
{
    RegTensor<uint64_t, RegTraitNumTwo> vTmp128Mul0, vTmp128Mul1, vTmp128Mul2, vTmp128Mul3;
    RegTensor<uint32_t, RegTraitNumOne> vTmp128Dst0, vTmp128Dst1;
    MaskReg pTmp128Carry0, pTmp128Carry1;
    Mull(
        (RegTensor<uint32_t>&)vTmp128Mul0.reg[0], (RegTensor<uint32_t>&)vTmp128Mul0.reg[1],
        (RegTensor<uint32_t>&)vSrc0.reg[0], (RegTensor<uint32_t>&)vSrc1.reg[0], mask);
    Mull(
        (RegTensor<uint32_t>&)vTmp128Mul1.reg[0], (RegTensor<uint32_t>&)vTmp128Mul1.reg[1],
        (RegTensor<uint32_t>&)vSrc0.reg[0], (RegTensor<uint32_t>&)vSrc1.reg[1], mask);
    Mull(
        (RegTensor<uint32_t>&)vTmp128Mul2.reg[0], (RegTensor<uint32_t>&)vTmp128Mul2.reg[1],
        (RegTensor<uint32_t>&)vSrc0.reg[1], (RegTensor<uint32_t>&)vSrc1.reg[0], mask);
    Mull(
        (RegTensor<uint32_t>&)vTmp128Mul3.reg[0], (RegTensor<uint32_t>&)vTmp128Mul3.reg[1],
        (RegTensor<uint32_t>&)vSrc0.reg[1], (RegTensor<uint32_t>&)vSrc1.reg[1], mask);
    Add(pTmp128Carry0, vTmp128Dst0, (RegTensor<uint32_t>&)vTmp128Mul0.reg[1], (RegTensor<uint32_t>&)vTmp128Mul1.reg[0],
        mask);
    Add(pTmp128Carry1, vTmp128Dst1, vTmp128Dst0, (RegTensor<uint32_t>&)vTmp128Mul2.reg[0], mask);
    AddC(
        pTmp128Carry0, vTmp128Dst0, (RegTensor<uint32_t>&)vTmp128Mul3.reg[0], (RegTensor<uint32_t>&)vTmp128Mul1.reg[1],
        pTmp128Carry0, mask);
    AddC(
        pTmp128Carry1, (RegTensor<uint32_t>&)vSrc1.reg[0], vTmp128Dst0, (RegTensor<uint32_t>&)vTmp128Mul2.reg[1],
        pTmp128Carry1, mask);
    AddC(pTmp128Carry0, vTmp128Dst0, vSrc2, (RegTensor<uint32_t>&)vTmp128Mul3.reg[1], pTmp128Carry0, mask);
    AddC(pTmp128Carry0, (RegTensor<uint32_t>&)vSrc1.reg[1], vSrc2, vTmp128Dst0, pTmp128Carry1, mask);
}

template <typename T, typename U, typename S>
__simd_callee__ inline void VselUsingU32(T& vDst, U& vSrc0, S& vSrc1, MaskReg& mask)
{
    vsel(
        (RegTensor<uint32_t>&)vDst.reg[0], (RegTensor<uint32_t>&)vSrc0.reg[0], (RegTensor<uint32_t>&)vSrc1.reg[0],
        mask);
    vsel(
        (RegTensor<uint32_t>&)vDst.reg[1], (RegTensor<uint32_t>&)vSrc0.reg[1], (RegTensor<uint32_t>&)vSrc1.reg[1],
        mask);
}

template <typename T, typename U>
__simd_callee__ inline void VcmpEqUsingU32(MaskReg& Dst, T& vSrc0, U& vSrc1, MaskReg& Src)
{
    MaskReg cmpEqLow, cmpEqHigh;
    vcmp_eq(cmpEqLow, (RegTensor<uint32_t>&)vSrc0.reg[0], (RegTensor<uint32_t>&)vSrc1.reg[0], Src);
    vcmp_eq(cmpEqHigh, (RegTensor<uint32_t>&)vSrc0.reg[1], (RegTensor<uint32_t>&)vSrc1.reg[1], Src);
    pand(Dst, cmpEqLow, cmpEqHigh, Src);
}

template <typename T, typename U>
__simd_callee__ inline void VcmpGeUsingU32(MaskReg& Dst, T& vSrc0, U& vSrc1, MaskReg& Src)
{
    MaskReg highEq, lowCmp, highCmp;
    vcmp_eq(highEq, (RegTensor<uint32_t>&)vSrc0.reg[1], (RegTensor<uint32_t>&)vSrc1.reg[1], Src);
    vcmp_ge(lowCmp, (RegTensor<uint32_t>&)vSrc0.reg[0], (RegTensor<uint32_t>&)vSrc1.reg[0], Src);
    vcmp_ge(highCmp, (RegTensor<uint32_t>&)vSrc0.reg[1], (RegTensor<uint32_t>&)vSrc1.reg[1], Src);
    psel(Dst, lowCmp, highCmp, highEq);
}

template <typename T, typename U, typename S>
__simd_callee__ inline void VaddUsingU32(T& vDst, U& vSrc0, S& vSrc1, MaskReg& mask)
{
    MaskReg pTmpAdd0, pTmpAdd1;
    Add(pTmpAdd0, (RegTensor<uint32_t>&)vDst.reg[0], (RegTensor<uint32_t>&)vSrc0.reg[0],
        (RegTensor<uint32_t>&)vSrc1.reg[0], mask);
    AddC(
        pTmpAdd1, (RegTensor<uint32_t>&)vDst.reg[1], (RegTensor<uint32_t>&)vSrc0.reg[1],
        (RegTensor<uint32_t>&)vSrc1.reg[1], pTmpAdd0, mask);
}

template <typename T, typename U, typename S>
__simd_callee__ inline void VsubUsingU32(T& vDst, U& vSrc0, S& vSrc1, MaskReg& mask)
{
    MaskReg carry0, carry1;
    vsubc(
        carry0, (RegTensor<uint32_t>&)vDst.reg[0], (RegTensor<uint32_t>&)vSrc0.reg[0],
        (RegTensor<uint32_t>&)vSrc1.reg[0], mask);
    vsubcs(
        carry1, (RegTensor<uint32_t>&)vDst.reg[1], (RegTensor<uint32_t>&)vSrc0.reg[1],
        (RegTensor<uint32_t>&)vSrc1.reg[1], carry0, mask);
}

template <typename T, typename U, typename S>
__simd_callee__ inline void DivSignCal(MaskReg& signResP, T& vSrc0, U& vSrc1, S& vSrc2, MaskReg& mask)
{
    MaskReg src0Ge0P, src1Ge0P;
    vcmp_ge(src0Ge0P, (RegTensor<int32_t>&)vSrc0.reg[1], (RegTensor<int32_t>&)vSrc2, mask);
    vcmp_ge(src1Ge0P, (RegTensor<int32_t>&)vSrc1.reg[1], (RegTensor<int32_t>&)vSrc2, mask);
    pxor(signResP, src0Ge0P, src1Ge0P, mask);
    pnot(signResP, signResP, mask);
}

template <typename T, typename U, typename S>
__simd_callee__ inline void F32PreProcess(T& vTmp4, U& vTmp3, S& vTmp2, MaskReg& mask)
{
    constexpr uint32_t divF32Bias = 0x1FFFFFFEU;
    vbr(vTmp3, static_cast<float>(1));
    vdiv(vTmp3, vTmp3, vTmp2, mask, MODE_ZEROING);
    vadds(vTmp4, (RegTensor<uint32_t>&)vTmp3, divF32Bias, mask);
}

template <typename T>
__simd_callee__ inline void DivS64Impl(T& dstReg, T& srcReg0, T& srcReg1, MaskReg& mask)
{
    MaskReg startMask = mask;
    RegTensor<int64_t, RegTraitNumTwo> vAbsSrc0, vAbsSrc1, vTmp5;
    RegTensor<float, RegTraitNumOne> vTmp2, vTmp3;
    RegTensor<uint32_t, RegTraitNumOne> vTmp4;
    RegTensor<uint64_t, RegTraitNumTwo> vConstDup0, qZero, vConstDup1, vTmp6, vTmp7, vTmp8, vTmp9;
    MaskReg zeroMask, nonZeroMask, oriMask, nonOneMask, oneMask, signResP, ge0P;
    AbsUsingS32(vAbsSrc0, srcReg0, mask);
    AbsUsingS32(vAbsSrc1, srcReg1, mask);
    VbrUsingU32(vConstDup0, static_cast<uint32_t>(0), static_cast<uint32_t>(0));
    constexpr uint32_t u32MaxNum = 0xFFFFFFFFU;
    VbrUsingU32(qZero, u32MaxNum, u32MaxNum);
    VbrUsingU32(vConstDup1, static_cast<uint32_t>(1), static_cast<uint32_t>(0));
    VcmpEqUsingU32(zeroMask, srcReg1, vConstDup0, mask);
    pnot(nonZeroMask, zeroMask, mask);
    mask = nonZeroMask;
    VcmpEqUsingU32(oneMask, vAbsSrc1, vConstDup1, mask);
    pnot(nonOneMask, oneMask, mask);
    oriMask = mask;
    pand(mask, nonOneMask, nonZeroMask, mask);
    VcvtS642F32(vTmp2, vAbsSrc1);
    F32PreProcess(vTmp4, vTmp3, vTmp2, mask);
    VcvtF322S64(vTmp5, vTmp4);
    VmulUsingU32(vTmp6, vAbsSrc1, vTmp5, mask);
    VnotInPlace(vTmp6, mask);
    AddB64Impl(vTmp6, vTmp6, vConstDup1, mask);
    vbr(vTmp4, 0);
    B128Calc(vTmp5, vTmp6, vTmp4, mask);
    VaddUsingU32(vTmp7, vTmp5, vTmp6, mask);
    VmulUsingU32(vTmp6, vAbsSrc1, vTmp7, mask);
    VnotInPlace(vTmp6, mask);
    AddB64Impl(vTmp6, vTmp6, vConstDup1, mask);
    B128Calc(vTmp7, vTmp6, vTmp4, mask);
    VaddUsingU32(vTmp6, vTmp7, vTmp6, mask);
    B128Calc(vAbsSrc0, vTmp6, vTmp4, mask);
    VmulUsingU32(vTmp7, vTmp6, vAbsSrc1, mask);
    VsubUsingU32(vTmp7, vAbsSrc0, vTmp7, mask);
    VcmpGeUsingU32(ge0P, vTmp7, vAbsSrc1, mask);
    VsubUsingU32(vTmp8, vTmp7, vAbsSrc1, ge0P);
    AddB64Impl(vTmp9, vTmp6, vConstDup1, ge0P);
    VselUsingU32(vTmp7, vTmp8, vTmp7, ge0P);
    VselUsingU32(vTmp6, vTmp9, vTmp6, ge0P);
    VcmpGeUsingU32(ge0P, vTmp7, vAbsSrc1, mask);
    AddB64Impl(vTmp9, vTmp6, vConstDup1, ge0P);
    VselUsingU32(vTmp6, vTmp9, vTmp6, ge0P);
    VselUsingU32(vTmp6, vAbsSrc0, vTmp6, oneMask);
    mask = oriMask;
    DivSignCal(signResP, srcReg0, srcReg1, vTmp4, mask);
    MaskReg pg1 = pset_b32(PAT_ALL);
    VsubUsingU32(vTmp8, vConstDup0, vTmp6, pg1);
    VselUsingU32(vTmp6, vTmp6, vTmp8, signResP);
    VselUsingU32(dstReg, qZero, vTmp6, zeroMask);
    mask = startMask;
}

template <typename T>
__simd_callee__ inline void DivU64Impl(T& dstReg, T& srcReg0, T& srcReg1, MaskReg& mask)
{
    MaskReg beginMask = mask;
    MaskReg zeroMask, nonZeroMask, nonOneMask, oneMask, sDivMask, uSrc1Mask, srcCmpMask, cmpDivMask, ge0P;
    RegTensor<uint64_t, RegTraitNumTwo> vConstDup0, qZero, vConstDup1, vAbsSrc0, vResQ, vTmp6, vTmp7, vTmp8, vTmp9;
    RegTensor<int64_t, RegTraitNumTwo> vAbsSrc1, vTmp5;
    RegTensor<float, RegTraitNumOne> vTmp2, vTmp3;
    RegTensor<uint32_t, RegTraitNumOne> vTmp4;
    VbrUsingU32(vConstDup0, static_cast<uint32_t>(0), static_cast<uint32_t>(0));
    constexpr uint32_t u32MaxNum = 0xFFFFFFFFU;
    VbrUsingU32(qZero, u32MaxNum, u32MaxNum);
    VbrUsingU32(vConstDup1, static_cast<uint32_t>(1), static_cast<uint32_t>(0));
    VcmpEqUsingU32(zeroMask, srcReg1, vConstDup0, mask);
    pnot(nonZeroMask, zeroMask, mask);
    mask = nonZeroMask;
    vAbsSrc0 = srcReg0;
    vmov((RegTensor<int32_t>&)vAbsSrc1.reg[0], (RegTensor<int32_t>&)srcReg1.reg[0]);
    vmov((RegTensor<int32_t>&)vAbsSrc1.reg[1], (RegTensor<int32_t>&)srcReg1.reg[1]);
    constexpr uint64_t vdivU64Const = 0x7FFFFFFFFFFFFFFFULL;
    CompareScalar<uint64_t, CMPMODE::GT, T>(uSrc1Mask, srcReg1, vdivU64Const, mask);
    Compare<uint64_t, CMPMODE::GE, T>(srcCmpMask, srcReg0, srcReg1, mask);
    pand(cmpDivMask, srcCmpMask, uSrc1Mask, mask);
    Select<uint64_t, T>(vResQ, vConstDup1, vConstDup0, cmpDivMask);
    cmpDivMask = pset_b32(PAT_ALLF);
    srcCmpMask = pset_b32(PAT_ALLF);
    Compare<uint64_t, CMPMODE::LT, T>(srcCmpMask, srcReg0, srcReg1, mask);
    pand(cmpDivMask, srcCmpMask, uSrc1Mask, mask);
    Select<uint64_t, T>(vResQ, vConstDup0, vConstDup1, cmpDivMask);
    CompareScalar<uint64_t, CMPMODE::LE, T>(sDivMask, srcReg1, vdivU64Const, mask);
    mask = sDivMask;
    VcmpEqUsingU32(oneMask, vAbsSrc1, vConstDup1, mask);
    pnot(nonOneMask, oneMask, mask);
    pand(mask, nonOneMask, nonZeroMask, mask);
    VcvtS642F32(vTmp2, vAbsSrc1);
    F32PreProcess(vTmp4, vTmp3, vTmp2, mask);
    VcvtF322S64(vTmp5, vTmp4);
    VmulUsingU32(vTmp6, vAbsSrc1, vTmp5, mask);
    VnotInPlace(vTmp6, mask);
    AddB64Impl(vTmp6, vTmp6, vConstDup1, mask);
    vbr(vTmp4, 0);
    B128Calc(vTmp5, vTmp6, vTmp4, mask);
    VaddUsingU32(vTmp7, vTmp5, vTmp6, mask);
    VmulUsingU32(vTmp6, vAbsSrc1, vTmp7, mask);
    VnotInPlace(vTmp6, mask);
    AddB64Impl(vTmp6, vTmp6, vConstDup1, mask);
    B128Calc(vTmp7, vTmp6, vTmp4, mask);
    VaddUsingU32(vTmp6, vTmp7, vTmp6, mask);
    B128Calc(vAbsSrc0, vTmp6, vTmp4, mask);
    VmulUsingU32(vTmp7, vTmp6, vAbsSrc1, mask);
    VsubUsingU32(vTmp7, vAbsSrc0, vTmp7, mask);
    VcmpGeUsingU32(ge0P, vTmp7, vAbsSrc1, mask);
    VsubUsingU32(vTmp8, vTmp7, vAbsSrc1, mask);
    AddB64Impl(vTmp9, vTmp6, vConstDup1, ge0P);
    VselUsingU32(vTmp7, vTmp8, vTmp7, ge0P);
    VselUsingU32(vTmp6, vTmp9, vTmp6, ge0P);
    VcmpGeUsingU32(ge0P, vTmp7, vAbsSrc1, mask);
    AddB64Impl(vTmp9, vTmp6, vConstDup1, ge0P);
    VselUsingU32(vTmp6, vTmp9, vTmp6, ge0P);
    VselUsingU32(vTmp6, vResQ, vTmp6, uSrc1Mask);
    VselUsingU32(vTmp6, vAbsSrc0, vTmp6, oneMask);
    VselUsingU32(dstReg, qZero, vTmp6, zeroMask);
    mask = beginMask;
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void ComplexDivKernel(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(CheckRegTrait<U, RegTraitNumTwo>(), "U should be RegTraitNumTwo");
    static_assert(
        SupportType<ActualT, complex32, complex64>(), "current data type is not supported on current device!");

    RegTensor<typename ActualT::EleType>& src0Real = (RegTensor<typename ActualT::EleType>&)srcReg0.reg[0];
    RegTensor<typename ActualT::EleType>& src0Imag = (RegTensor<typename ActualT::EleType>&)srcReg0.reg[1];
    RegTensor<typename ActualT::EleType>& src1Real = (RegTensor<typename ActualT::EleType>&)srcReg1.reg[0];
    RegTensor<typename ActualT::EleType>& src1Imag = (RegTensor<typename ActualT::EleType>&)srcReg1.reg[1];
    RegTensor<typename ActualT::EleType>& dstReal = (RegTensor<typename ActualT::EleType>&)dstReg.reg[0];
    RegTensor<typename ActualT::EleType>& dstImag = (RegTensor<typename ActualT::EleType>&)dstReg.reg[1];
    RegTensor<typename ActualT::EleType> absSrc1Real;
    RegTensor<typename ActualT::EleType> absSrc1Imag;
    RegTensor<typename ActualT::EleType> dstRealTmp;
    RegTensor<typename ActualT::EleType> dstImagTmp;
    // abs_c
    Abs(absSrc1Real, src1Real, mask);
    // abs_d
    Abs(absSrc1Imag, src1Imag, mask);
    RegTensor<typename ActualT::EleType> one;
    RegTensor<typename ActualT::EleType> zero;
    Reg::Duplicate<typename ActualT::EleType, typename ActualT::EleType, RegTensor<typename ActualT::EleType>>(
        one, 1.0f);
    Reg::Duplicate<typename ActualT::EleType, typename ActualT::EleType, RegTensor<typename ActualT::EleType>>(
        zero, 0.0f);
    MaskReg maskFull = Reg::CreateMask<typename ActualT::EleType, Reg::MaskPattern::ALL>();

    // condition : if abs_c >= abs_d
    MaskReg compareAbsCDMask;
    Compare<typename ActualT::EleType, CMPMODE::GE, RegTensor<typename ActualT::EleType>>(
        compareAbsCDMask, absSrc1Real, absSrc1Imag, mask);
    // condition : if abs_c < abs_d
    MaskReg compareAbsCDMaskNot;
    MaskNot(compareAbsCDMaskNot, compareAbsCDMask, maskFull);
    // condition: abs_c == 0 && abs_d == 0
    MaskReg equalZero;
    Compare<typename ActualT::EleType, CMPMODE::EQ, RegTensor<typename ActualT::EleType>>(
        equalZero, absSrc1Real, zero, compareAbsCDMask);
    Compare<typename ActualT::EleType, CMPMODE::EQ, RegTensor<typename ActualT::EleType>>(
        equalZero, absSrc1Imag, zero, equalZero);
    RegTensor<typename ActualT::EleType> dstRealTmp0;
    RegTensor<typename ActualT::EleType> dstImagTmp0;
    static constexpr DivSpecificMode divMode = {MaskMergeMode::ZEROING, false, DivAlgo::PRECISION_1ULP_FTZ_TRUE};
    Div<typename ActualT::EleType, &divMode, RegTensor<typename ActualT::EleType>>(
        dstRealTmp0, src0Real, absSrc1Real, equalZero);
    Select(dstRealTmp, dstRealTmp0, zero, equalZero);
    Div<typename ActualT::EleType, &divMode, RegTensor<typename ActualT::EleType>>(
        dstImagTmp0, src0Imag, absSrc1Imag, equalZero);
    Select(dstImagTmp, dstImagTmp0, zero, equalZero);

    // abs_c >= abs_d && !(abs_c == 0 && abs_d == 0)
    MaskNot(compareAbsCDMask, equalZero, compareAbsCDMask);
    RegTensor<typename ActualT::EleType> rat;
    // formula: rat = d / c
    Div<typename ActualT::EleType, mode, RegTensor<typename ActualT::EleType>>(
        rat, src1Imag, src1Real, compareAbsCDMask);
    RegTensor<typename ActualT::EleType> dMulRat;
    // formula: d*rat
    Mul(dMulRat, src1Imag, rat, compareAbsCDMask);
    // formula: c + d*rat
    Add(dMulRat, src1Real, dMulRat, compareAbsCDMask);
    // formula: scl = 1/(c + d*rat)
    Div<typename ActualT::EleType, mode, RegTensor<typename ActualT::EleType>>(dMulRat, one, dMulRat, compareAbsCDMask);
    RegTensor<typename ActualT::EleType> bMulRat;
    // formula: b * rat
    Mul(bMulRat, src0Imag, rat, compareAbsCDMask);
    // formula: a + b*rat
    Add(bMulRat, src0Real, bMulRat, compareAbsCDMask);
    // formula: (a + b*rat)*scl
    Mul<typename ActualT::EleType, MaskMergeMode::ZEROING, RegTensor<typename ActualT::EleType>>(
        dstRealTmp0, bMulRat, dMulRat, compareAbsCDMask);
    Select(dstRealTmp, dstRealTmp0, dstRealTmp, compareAbsCDMask);
    RegTensor<typename ActualT::EleType> aMulRat;
    // formula: a * rat
    Mul(aMulRat, src0Real, rat, compareAbsCDMask);
    // formula: b - a * rat
    Sub(aMulRat, src0Imag, aMulRat, compareAbsCDMask);
    // formula: (b - a * rat)*scl
    Mul<typename ActualT::EleType, MaskMergeMode::ZEROING, RegTensor<typename ActualT::EleType>>(
        dstImagTmp0, aMulRat, dMulRat, compareAbsCDMask);
    Select(dstImagTmp, dstImagTmp0, dstImagTmp, compareAbsCDMask);

    // formula: rat = c/d
    Div<typename ActualT::EleType, mode, RegTensor<typename ActualT::EleType>>(
        rat, src1Real, src1Imag, compareAbsCDMaskNot);
    // formula: c*rat
    Mul(dMulRat, src1Real, rat, compareAbsCDMaskNot);
    // formula: d+c*rat
    Add(dMulRat, src1Imag, dMulRat, compareAbsCDMaskNot);
    // formula: scl = 1/(d+c*rat)
    Div<typename ActualT::EleType, mode, RegTensor<typename ActualT::EleType>>(
        dMulRat, one, dMulRat, compareAbsCDMaskNot);
    // formula: a*rat
    Mul(aMulRat, src0Real, rat, compareAbsCDMaskNot);
    // formula: a*rat + b
    Add(aMulRat, aMulRat, src0Imag, compareAbsCDMaskNot);
    // formula: (a*rat + b)*scl
    Mul<typename ActualT::EleType, MaskMergeMode::ZEROING, RegTensor<typename ActualT::EleType>>(
        dstRealTmp0, aMulRat, dMulRat, compareAbsCDMaskNot);
    Select(dstRealTmp, dstRealTmp0, dstRealTmp, compareAbsCDMaskNot);
    // formula: b*rat
    Mul(bMulRat, src0Imag, rat, compareAbsCDMaskNot);
    // formula: b*rat - a
    Sub(bMulRat, bMulRat, src0Real, compareAbsCDMaskNot);
    // formula: (b*rat - a) * scl
    Mul<typename ActualT::EleType, MaskMergeMode::ZEROING, RegTensor<typename ActualT::EleType>>(
        dstImagTmp0, bMulRat, dMulRat, compareAbsCDMaskNot);

    Select(dstImagTmp, dstImagTmp0, dstImagTmp, compareAbsCDMaskNot);
    dstReal = dstRealTmp;
    dstImag = dstImagTmp;
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void DivComplex64Impl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(sizeof(ActualT) == 8, "data type should be B64");
    static_assert(CheckRegTrait<U, RegTraitNumTwo>(), "U should be RegTraitNumTwo");
    ComplexDivKernel<T, mode, Reg::RegTensor<ActualT, Reg::RegTraitNumTwo>>(dstReg, srcReg0, srcReg1, mask);
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void DivB64Impl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(sizeof(ActualT) == 8, "DivB64Impl data type should be B64");
    static_assert(CheckRegTrait<U, RegTraitNumTwo>(), "DivB64Impl U should be RegTraitNumTwo");
    if constexpr (SupportType<ActualT, complex64>()) {
        DivComplex64Impl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
    } else {
        if constexpr (Std::is_same_v<ActualT, uint64_t>) {
            U dstTemp;
            DivU64Impl(dstTemp, srcReg0, srcReg1, mask);
            dstReg = dstTemp;
        } else if constexpr (Std::is_same_v<ActualT, int64_t>) {
            U dstTemp;
            DivS64Impl(dstTemp, srcReg0, srcReg1, mask);
            dstReg = dstTemp;
        }
    }
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void MergeTwoFloatELementRegs(U& dst, U& src0, U& src1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(CheckRegTrait<U, RegTraitNumOne>(), "U should be RegTraitNumOne");
    static_assert(SupportType<ActualT, float, half>(), "current data type is not supported on current device!");
    if constexpr (SupportType<ActualT, float>()) {
        Or((RegTensor<uint32_t>&)dst, (RegTensor<uint32_t>&)src0, (RegTensor<uint32_t>&)src1, mask);
    } else if constexpr (SupportType<ActualT, half>()) {
        Or((RegTensor<uint16_t>&)dst, (RegTensor<uint16_t>&)src0, (RegTensor<uint16_t>&)src1, mask);
    }
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void DivComplex32TwoImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(sizeof(ActualT) == 4, "data type should be B32");
    static_assert(CheckRegTrait<U, RegTraitNumTwo>(), "U should be RegTraitNumTwo");
    ComplexDivKernel<T, mode, Reg::RegTensor<ActualT, Reg::RegTraitNumTwo>>(dstReg, srcReg0, srcReg1, mask);
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void DivComplex32OnetraitImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(sizeof(ActualT) == 4, "data type should be B32");
    static_assert(CheckRegTrait<U, RegTraitNumOne>(), "U should be RegTraitNumOne");
    MaskReg maskTrait2;
    MaskPack(maskTrait2, mask);
    RegTensor<ActualT, RegTraitNumTwo> divTraitTwoSrcReg0;
    RegTensor<ActualT, RegTraitNumTwo> divTraitTwoSrcReg1;
    RegTensor<ActualT, RegTraitNumTwo> divTraitTwoDstReg;

    B32TraitOneToTraitTwo(divTraitTwoSrcReg0, srcReg0);
    B32TraitOneToTraitTwo(divTraitTwoSrcReg1, srcReg1);
    DivComplex32TwoImpl<T, mode, Reg::RegTensor<ActualT, RegTraitNumTwo>>(
        divTraitTwoDstReg, divTraitTwoSrcReg0, divTraitTwoSrcReg1, maskTrait2);
    B32TraitTwoToTraitOne(dstReg, divTraitTwoDstReg);
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void DivPrecisionImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    // Improves Reg with high precision mode by using error-complemention approach with following formula.
    //
    // sign = vand(a, -0.0)
    // sign_one = vor(1.0, sign)

    // inf_nan_tmp = vor(x1, neg_zero)
    // inf_nan_temp_uint32 = 0xFF800000
    // inf_nan_cmp_mask = vcmp(inf_nan_temp_uint32, neg_inf_uint32, "ge", "bit")

    // x1 = div(a, b)
    // temp_b = -1.0*b

    // a = fma(a, temp_b, x1)  // a = a + temp_b*x1, high precision fma instr is required.
    // temp_mask = vcmp(a, a, "eq", "bit")
    // temp_a = vsel(temp_mask, a, one_temp)
    // e = div(temp_a, b)

    // x = x1 + e
    // x = vsel(inf_nan_cmp_mask, x1, x)
    using ActualT = typename U::ActualT;
    constexpr DivSpecificMode sprMode = Internal::GetDivSpecificMode(mode);
    constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
    constexpr uint32_t infNanBound = 0xff800000u;
    constexpr uint32_t signBitNum = 0x80000000u;
    constexpr int32_t precisionThreshold = -64;
    constexpr uint32_t exponentMask = 0x7F800000u;
    constexpr uint32_t mantissaMask = 0x007FFFFFu;
    constexpr int32_t exponentBias = 127;

    RegTensor<ActualT> regNegZero;
    RegTensor<ActualT> tmpDst;
    RegTensor<ActualT> r, z, y;
    RegTensor<uint32_t> infNan;

    MaskReg cmpMaskReg;
    MaskReg infNanCmp;
    MaskReg zeroCmp;
    MaskReg preg;
    preg = pset_b8(PAT_ALL);
    vdup((vector_u32&)regNegZero, signBitNum, preg, modeValue);
    vdiv(z, srcReg0, srcReg1, mask, modeValue);
    vor(infNan, (vector_u32&)z, (vector_u32&)regNegZero, mask, modeValue);
    tmpDst = z;
    vcmps_eq(zeroCmp, z, 0.0f, mask);
    vcmps_ge(infNanCmp, infNan, infNanBound, mask);
    por(infNanCmp, infNanCmp, zeroCmp, mask);

    RegTensor<uint32_t> src0ExpBits, src0Reg;
    RegTensor<int32_t> src0Exp;
    RegTensor<uint32_t> scaleBits;
    RegTensor<ActualT> scale;
    RegTensor<ActualT> aScaled, bScaled;
    MaskReg needScaleMask;
    MaskReg noScaleMask;

    vdup(src0Reg, exponentMask, mask, modeValue);
    And(src0ExpBits, (RegTensor<uint32_t>&)srcReg0, src0Reg, mask);
    vshrs(src0ExpBits, src0ExpBits, (int16_t)23, mask, modeValue);
    vdup(src0Reg, exponentBias, mask, modeValue);
    vsub(src0Exp, (RegTensor<int32_t>&)src0ExpBits, (RegTensor<int32_t>&)src0Reg, mask, modeValue);

    Compares<int32_t, CMPMODE::LT>(needScaleMask, src0Exp, precisionThreshold, mask);
    MaskNot(noScaleMask, needScaleMask, mask);

    RegTensor<int32_t> k;
    RegTensor<int32_t> thresholdVec;
    RegTensor<int32_t> zeroVec;
    vdup(thresholdVec, precisionThreshold, mask, modeValue);
    vdup(zeroVec, 0, mask, modeValue);
    vsub(k, thresholdVec, src0Exp, mask, modeValue);
    vmax(k, k, zeroVec, mask, modeValue);

    RegTensor<int32_t> newExp;
    vadds(newExp, k, exponentBias, needScaleMask, modeValue);
    vshls(scaleBits, (RegTensor<uint32_t>&)newExp, (int16_t)23, needScaleMask, modeValue);

    RegTensor<ActualT> scaleOne;
    vdup(scaleOne, 1.0f, mask, modeValue);
    vsel(scale, (RegTensor<ActualT>&)scaleBits, scaleOne, needScaleMask);

    vmul(aScaled, srcReg0, scale, mask, modeValue);
    vmul(bScaled, srcReg1, scale, mask, modeValue);

    vmuls(y, bScaled, -1.0f, mask, modeValue);
    r = aScaled;
    vmula(r, z, y, mask, modeValue);
    RegTensor<ActualT> rPre, rNext, zPre, zNext;

    vadds((vector_s32&)zPre, (vector_s32&)z, -1, mask, modeValue);
    vadds((vector_s32&)zNext, (vector_s32&)z, 1, mask, modeValue);

    rPre = aScaled;
    rNext = aScaled;

    vmula(rPre, zPre, y, mask, modeValue);
    vmula(rNext, zNext, y, mask, modeValue);

    vabs(r, r, mask, modeValue);
    vabs(rPre, rPre, mask, modeValue);
    vabs(rNext, rNext, mask, modeValue);
    vcmp_lt(cmpMaskReg, r, rPre, mask);
    vsel(r, r, rPre, cmpMaskReg);
    vsel(z, z, zPre, cmpMaskReg);

    vcmp_lt(cmpMaskReg, rNext, r, mask);
    vsel(z, zNext, z, cmpMaskReg);
    vsel(dstReg, tmpDst, z, infNanCmp);
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U, bool is0ULP>
__simd_callee__ inline void DivIEEE754FloatImpl(
    RegTensor<float>& dst, RegTensor<float>& src0, RegTensor<float>& src1, MaskReg& mask)
{
    constexpr uint32_t exponentExtractor = 0x807FFFFF;
    constexpr uint32_t signExtractor = 0x80000000;
    constexpr uint32_t exponentNormalizer = 0x3F800000;

    NotNumUnion subnormalThreshold;
    subnormalThreshold.i = 0x007FFFFF;

    NotNumUnion nan;
    nan.i = F32_NAN;
    NotNumUnion min_denormal;
    min_denormal.i = 0x1;

    NotNumUnion normalizeScaleEnlarge;
    normalizeScaleEnlarge.i = 0x4B000000; // 2^23
    NotNumUnion normalizeScaleReduce;
    normalizeScaleReduce.i = 0x34000000; // 2^-23

    Reg::RegTensor<float> maxSubnormal;
    Reg::RegTensor<uint32_t> tmp0;
    Reg::RegTensor<int32_t> tmp1;
    Reg::RegTensor<uint32_t> tmp2;

    Reg::RegTensor<float> src0Abs;
    Reg::RegTensor<float> src0Subnormal;
    Reg::RegTensor<float> src0Norm;
    Reg::RegTensor<float> src0All;
    Reg::RegTensor<float> src0AbsNorm;

    Reg::RegTensor<float> src1Abs;
    Reg::RegTensor<float> src1Subnormal;
    Reg::RegTensor<float> src1Norm;
    Reg::RegTensor<float> src1All;
    Reg::RegTensor<float> src1AbsNorm;

    MaskReg mask0;
    MaskReg maskSrc0Normal;
    MaskReg maskSrc0Subnormal;
    MaskReg maskSrc1Normal;
    MaskReg maskSrc1Subnormal;
    MaskReg maskTmp;
    MaskReg maskNan;      // divisor or dividend 0
    MaskReg maskInf;      // divisor or dividend inf
    MaskReg maskSrc0Zero; // dividend 0
    MaskReg maskSrc1Zero; // divisor 0
    MaskReg maskValid;
    MaskReg maskNorm;

    RegTensor<uint32_t> src0Exponent;
    RegTensor<uint32_t> src1Exponent;

    RegTensor<float> z1;
    RegTensor<float> z2;
    RegTensor<int32_t> scale;
    RegTensor<uint32_t> dstExponent;
    RegTensor<uint32_t> dstSign;

    // subnormal threshold
    Duplicate(maxSubnormal, subnormalThreshold.f, mask);

    // ===========================================================
    // acquiring valid numbers (no inf, no 0)
    // ===========================================================
    Abs(src0Abs, src0, mask);
    Abs(src1Abs, src1, mask);

    // get positions of inf values
    Duplicate(tmp0, F32_INF, mask);
    Compare<uint32_t, CMPMODE::EQ>(maskInf, (RegTensor<uint32_t>&)src0Abs, tmp0, mask);
    Compare<uint32_t, CMPMODE::EQ>(maskTmp, (RegTensor<uint32_t>&)src1Abs, tmp0, mask);
    MaskOr(maskValid, maskInf, maskTmp, mask);
    // get positions of 0 divisor or dividend
    Duplicate(tmp0, 0, mask);
    Compare<uint32_t, CMPMODE::EQ>(maskSrc0Zero, (RegTensor<uint32_t>&)src0Abs, tmp0, mask);
    // merge for positions of invalid numbers
    MaskOr(maskValid, maskValid, maskSrc0Zero, mask);
    Compare<uint32_t, CMPMODE::EQ>(maskSrc1Zero, (RegTensor<uint32_t>&)src1Abs, tmp0, mask);
    // negating for positions of valid numbers
    MaskOr(maskValid, maskValid, maskSrc1Zero, mask);
    MaskNot(maskValid, maskValid, mask);

    // normalize subnormal elements of src0
    // get positions of subnormal numbers in dividend
    Compare<float, CMPMODE::LT>(maskSrc0Subnormal, src0Abs, maxSubnormal, mask);
    // negating for normal positions
    MaskNot(maskSrc0Normal, maskSrc0Subnormal, mask);
    // normalizatoin
    Muls(src0Subnormal, src0, normalizeScaleEnlarge.f, maskSrc0Subnormal);

    // normalize subnormal elements of src1
    Compare<float, CMPMODE::LT>(maskSrc1Subnormal, src1Abs, maxSubnormal, mask);
    MaskNot(maskSrc1Normal, maskSrc1Subnormal, mask);
    Muls(src1Subnormal, src1, normalizeScaleEnlarge.f, maskSrc1Subnormal);

    // merge the normalized subnormal elements with normal elements
    Select(src0All, src0, src0Subnormal, maskSrc0Normal);
    Select(src1All, src1, src1Subnormal, maskSrc1Normal);

    // standardized the exponent bits of src0 and src1
    // zero out the exponent bits 00000000
    Duplicate(tmp0, exponentExtractor, mask);
    And((RegTensor<uint32_t>&)src0Norm, (RegTensor<uint32_t>&)src0All, tmp0, maskValid);
    And((RegTensor<uint32_t>&)src1Norm, (RegTensor<uint32_t>&)src1All, tmp0, maskValid);
    // set the exponent bits to 01111111
    Duplicate(tmp0, exponentNormalizer, mask);
    Add((RegTensor<uint32_t>&)src0Norm, (RegTensor<uint32_t>&)src0Norm, tmp0, maskValid);
    Add((RegTensor<uint32_t>&)src1Norm, (RegTensor<uint32_t>&)src1Norm, tmp0, maskValid);
    Select(src0Norm, src0Norm, src0All, maskValid);
    Select(src1Norm, src1Norm, src1All, maskValid);
    Abs(src0AbsNorm, src0Norm, maskValid);
    Abs(src1AbsNorm, src1Norm, maskValid);
    Compare<float, CMPMODE::LE>(maskNorm, src0AbsNorm, src1AbsNorm, maskValid);

    if constexpr (is0ULP) {
        DivPrecisionImpl<float, mode, U>(dst, src0Norm, src1Norm, mask);
    } else {
        constexpr DivSpecificMode sprMode = Internal::GetDivSpecificMode(MaskMergeMode::ZEROING);
        constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
        vdiv(dst, src0Norm, src1Norm, mask, modeValue);
    }

    // subnormal dividend, normal divisor
    MaskAnd(mask0, maskSrc0Subnormal, maskSrc1Normal, mask);
    // normalization compensation
    Muls(z1, dst, normalizeScaleReduce.f, mask0);
    Select(dst, z1, dst, mask0);

    // normal dividend, subnormal divisor
    MaskAnd(mask0, maskSrc0Normal, maskSrc1Subnormal, mask);
    // normalization compensation
    Muls(z1, dst, normalizeScaleEnlarge.f, mask0);
    // merge the compensated result
    Select(dst, z1, dst, mask0);

    // preserve sign for error handling section below
    Duplicate(tmp0, signExtractor, mask);
    And((RegTensor<uint32_t>&)dstSign, (RegTensor<uint32_t>&)dst, tmp0, mask);

    // ===========================================================
    // exponent operation
    // ===========================================================
    // extract the exponent section 0 11..11 00..00
    Duplicate(tmp0, F32_INF, mask);
    And(src0Exponent, (RegTensor<uint32_t>&)src0All, tmp0, mask);
    And(src1Exponent, (RegTensor<uint32_t>&)src1All, tmp0, mask);
    And(dstExponent, (RegTensor<uint32_t>&)dst, tmp0, mask);

    // exponent subtraction (effectively fp number division)
    ShiftRights(src0Exponent, src0Exponent, (int16_t)23, mask);
    ShiftRights(src1Exponent, src1Exponent, (int16_t)23, mask);
    ShiftRights(dstExponent, dstExponent, (int16_t)23, mask);
    Sub(scale, (RegTensor<int32_t>&)src0Exponent, (RegTensor<int32_t>&)src1Exponent, mask);
    Adds(scale, scale, 127, mask);
    // ===========================================================
    // exception handling
    // ===========================================================
    // overflow (exponent over 255) underflow (exponent under 0) detection // FP32:1S + 8E + 23M
    Duplicate(tmp1, -23, mask);
    // True if underflow/overflow
    Compare<int32_t, CMPMODE::EQ>(mask0, scale, (RegTensor<int32_t>&)tmp1, mask);
    MaskAnd(mask0, mask0, maskValid, mask);
    Duplicate(tmp0, min_denormal.i, mask0);
    Add((RegTensor<uint32_t>&)z1, (RegTensor<uint32_t>&)dstSign, tmp0, mask0);
    Duplicate(tmp2, static_cast<uint32_t>(0), mask0);
    Add((RegTensor<uint32_t>&)z2, (RegTensor<uint32_t>&)dstSign, tmp2, mask0);
    Select(z1, z2, z1, maskNorm);
    Select(dst, z1, dst, mask0);
    MaskNot(mask0, mask0, mask);
    MaskAnd(maskValid, mask0, maskValid, mask);
    Compare<int32_t, CMPMODE::LT>(mask0, scale, (RegTensor<int32_t>&)tmp1, mask);
    // set overflown/underflown result to infinity
    MaskAnd(mask0, mask0, maskValid, mask);
    Duplicate(tmp0, 0, mask); // set to 0
    Add((RegTensor<uint32_t>&)z1, (RegTensor<uint32_t>&)dstSign, tmp0, mask0);
    Select(dst, z1, dst, mask0);
    MaskNot(mask0, mask0, mask);
    MaskAnd(maskValid, mask0, maskValid, mask);

    Duplicate(tmp0, 255, mask);
    Compare<int32_t, CMPMODE::EQ>(mask0, scale, (RegTensor<int32_t>&)tmp0, mask);
    MaskAnd(mask0, mask0, maskValid, mask);
    Reg::Duplicate(tmp1, 1, mask0);
    Reg::Sub(tmp1, scale, tmp1, mask0);
    Select(scale, tmp1, scale, mask0);
    Muls(z1, dst, 2, mask0);
    Select(dst, z1, dst, mask0);

    Compare<int32_t, CMPMODE::GT>(mask0, scale, (RegTensor<int32_t>&)tmp0, mask);
    MaskAnd(mask0, mask0, maskValid, mask);
    Duplicate(tmp0, F32_INF, mask); // set to infinity
    Add((RegTensor<uint32_t>&)z1, (RegTensor<uint32_t>&)dstSign, tmp0, mask0);
    Select(dst, z1, dst, mask0);
    MaskNot(mask0, mask0, mask);
    MaskAnd(maskValid, mask0, maskValid, mask);

    Duplicate(tmp0, 0, maskValid);
    Compare<int32_t, CMPMODE::GT>(mask0, scale, (RegTensor<int32_t>&)tmp0, maskValid);
    ShiftLefts(tmp1, scale, (int16_t)23, mask0);
    Mul(z1, dst, (RegTensor<float>&)tmp1, mask0);
    Select(dst, z1, dst, mask0);

    MaskNot(mask0, mask0, maskValid);
    Duplicate(tmp0, 4194304, mask0); // set 0x0040 0000
    Abs(scale, scale, mask0);
    ShiftRight(scale, (RegTensor<int32_t>&)tmp0, scale, mask0);
    Mul(z1, dst, (RegTensor<float>&)scale, mask0);
    Select(dst, z1, dst, mask0);

    // get the position of nan
    Duplicate(tmp0, nan.i, mask);
    Compare<float, CMPMODE::NE>(maskNan, src0Abs, src0Abs, mask);
    Compare<float, CMPMODE::NE>(maskTmp, src1Abs, src1Abs, mask);
    MaskOr(maskNan, maskNan, maskTmp, mask);
    // set output with nan input to nan
    Select(dst, (RegTensor<float>&)tmp0, dst, maskNan);
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void DivIEEE754HalfImpl(
    RegTensor<half>& dst, RegTensor<half>& src0, RegTensor<half>& src1, MaskReg& mask)
{
    constexpr uint16_t exponentExtractor = 0x83FF;
    constexpr uint16_t signExtractor = 0x8000;
    constexpr uint16_t exponentNormalizer = 0x3C00;
    constexpr uint16_t F16_INF = 0x7C00;

    HalfUnion subnormalThreshold;
    subnormalThreshold.i = 0x03FF;

    HalfUnion nan;
    nan.i = 0x7E00;
    HalfUnion min_denormal;
    min_denormal.i = 0x1;

    HalfUnion normalizeScaleEnlarge;
    normalizeScaleEnlarge.i = 0x6400; // 2^10
    HalfUnion normalizeScaleReduce;
    normalizeScaleReduce.i = 0x1400; // 2^-10

    Reg::RegTensor<half> maxSubnormal;
    Reg::RegTensor<uint16_t> tmp0;
    Reg::RegTensor<int16_t> tmp1;
    Reg::RegTensor<uint16_t> tmp2;

    Reg::RegTensor<half> src0Abs;
    Reg::RegTensor<half> src0Subnormal;
    Reg::RegTensor<half> src0Norm;
    Reg::RegTensor<half> src0All;
    Reg::RegTensor<half> src0AbsNorm;

    Reg::RegTensor<half> src1Abs;
    Reg::RegTensor<half> src1Subnormal;
    Reg::RegTensor<half> src1Norm;
    Reg::RegTensor<half> src1All;
    Reg::RegTensor<half> src1AbsNorm;

    MaskReg mask0;
    MaskReg maskSrc0Normal;
    MaskReg maskSrc0Subnormal;
    MaskReg maskSrc1Normal;
    MaskReg maskSrc1Subnormal;
    MaskReg maskTmp;
    MaskReg maskNan;      // divisor or dividend 0
    MaskReg maskInf;      // divisor or dividend inf
    MaskReg maskSrc0Zero; // dividend 0
    MaskReg maskSrc1Zero; // divisor 0
    MaskReg maskValid;
    MaskReg maskNorm;

    RegTensor<uint16_t> src0Exponent;
    RegTensor<uint16_t> src1Exponent;

    RegTensor<half> z1;
    RegTensor<half> z2;
    RegTensor<int16_t> scale;
    RegTensor<uint16_t> dstExponent;
    RegTensor<uint16_t> dstSign;

    // subnormal threshold
    Duplicate(maxSubnormal, subnormalThreshold.f, mask);

    // ===========================================================
    // acquiring valid numbers (no inf, no 0)
    // ===========================================================
    Abs(src0Abs, src0, mask);
    Abs(src1Abs, src1, mask);

    // get positions of inf values
    Duplicate(tmp0, F16_INF, mask);
    Compare<uint16_t, CMPMODE::EQ>(maskInf, (RegTensor<uint16_t>&)src0Abs, tmp0, mask);
    Compare<uint16_t, CMPMODE::EQ>(maskTmp, (RegTensor<uint16_t>&)src1Abs, tmp0, mask);
    MaskOr(maskValid, maskInf, maskTmp, mask);
    // get positions of 0 divisor or dividend
    Duplicate(tmp0, 0, mask);
    Compare<uint16_t, CMPMODE::EQ>(maskSrc0Zero, (RegTensor<uint16_t>&)src0Abs, tmp0, mask);
    // merge for positions of invalid numbers
    MaskOr(maskValid, maskValid, maskSrc0Zero, mask);
    Compare<uint16_t, CMPMODE::EQ>(maskSrc1Zero, (RegTensor<uint16_t>&)src1Abs, tmp0, mask);
    // negating for positions of valid numbers
    MaskOr(maskValid, maskValid, maskSrc1Zero, mask);
    MaskNot(maskValid, maskValid, mask);

    // normalize subnormal elements of src0
    // get positions of subnormal numbers in dividend
    Compare<half, CMPMODE::LT>(maskSrc0Subnormal, src0Abs, maxSubnormal, mask);
    // negating for normal positions
    MaskNot(maskSrc0Normal, maskSrc0Subnormal, mask);
    // normalizatoin
    Muls(src0Subnormal, src0, normalizeScaleEnlarge.f, maskSrc0Subnormal);

    // normalize subnormal elements of src1
    Compare<half, CMPMODE::LT>(maskSrc1Subnormal, src1Abs, maxSubnormal, mask);
    MaskNot(maskSrc1Normal, maskSrc1Subnormal, mask);
    Muls(src1Subnormal, src1, normalizeScaleEnlarge.f, maskSrc1Subnormal);

    // merge the normalized subnormal elements with normal elements
    Select(src0All, src0, src0Subnormal, maskSrc0Normal);
    Select(src1All, src1, src1Subnormal, maskSrc1Normal);

    // standardized the exponent bits of src0 and src1
    // zero out the exponent bits 00000000
    Duplicate(tmp0, exponentExtractor, mask);
    And((RegTensor<uint16_t>&)src0Norm, (RegTensor<uint16_t>&)src0All, tmp0, maskValid);
    And((RegTensor<uint16_t>&)src1Norm, (RegTensor<uint16_t>&)src1All, tmp0, maskValid);
    // set the exponent bits to 01111111
    Duplicate(tmp0, exponentNormalizer, mask);
    Add((RegTensor<uint16_t>&)src0Norm, (RegTensor<uint16_t>&)src0Norm, tmp0, maskValid);
    Add((RegTensor<uint16_t>&)src1Norm, (RegTensor<uint16_t>&)src1Norm, tmp0, maskValid);
    Select(src0Norm, src0Norm, src0All, maskValid);
    Select(src1Norm, src1Norm, src1All, maskValid);
    Abs(src0AbsNorm, src0Norm, maskValid);
    Abs(src1AbsNorm, src1Norm, maskValid);
    Compare<half, CMPMODE::LE>(maskNorm, src0AbsNorm, src1AbsNorm, maskValid);

    constexpr DivSpecificMode sprMode = Internal::GetDivSpecificMode(MaskMergeMode::ZEROING);
    constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
    vdiv(dst, src0Norm, src1Norm, mask, modeValue);

    // subnormal dividend, normal divisor
    MaskAnd(mask0, maskSrc0Subnormal, maskSrc1Normal, mask);
    // normalization compensation
    Muls(z1, dst, normalizeScaleReduce.f, mask0);
    Select(dst, z1, dst, mask0);

    // normal dividend, subnormal divisor
    MaskAnd(mask0, maskSrc0Normal, maskSrc1Subnormal, mask);
    // normalization compensation
    Muls(z1, dst, normalizeScaleEnlarge.f, mask0);
    // merge the compensated result
    Select(dst, z1, dst, mask0);

    // preserve sign for error handling section below
    Duplicate(tmp0, signExtractor, mask);
    And((RegTensor<uint16_t>&)dstSign, (RegTensor<uint16_t>&)dst, tmp0, mask);

    // ===========================================================
    // exponent operation
    // ===========================================================
    // extract the exponent section 0 11..11 00..00
    Duplicate(tmp0, F16_INF, mask);
    And(src0Exponent, (RegTensor<uint16_t>&)src0All, tmp0, mask);
    And(src1Exponent, (RegTensor<uint16_t>&)src1All, tmp0, mask);
    And(dstExponent, (RegTensor<uint16_t>&)dst, tmp0, mask);

    // exponent subtraction (effectively fp number division)
    ShiftRights(src0Exponent, src0Exponent, (int16_t)10, mask);
    ShiftRights(src1Exponent, src1Exponent, (int16_t)10, mask);
    ShiftRights(dstExponent, dstExponent, (int16_t)10, mask);
    Sub(scale, (RegTensor<int16_t>&)src0Exponent, (RegTensor<int16_t>&)src1Exponent, mask);
    Adds(scale, scale, 15, mask);
    // ===========================================================
    // exception handling
    // ===========================================================
    // overflow (exponent over 31) underflow (exponent under -9) detection // FP16:1S + 5E + 9M
    Duplicate(tmp1, -9, mask);
    Compare<int16_t, CMPMODE::EQ>(mask0, scale, (RegTensor<int16_t>&)tmp1, mask);
    MaskAnd(mask0, mask0, maskValid, mask);
    Duplicate(tmp0, min_denormal.i, mask0);
    Add((RegTensor<uint16_t>&)z1, (RegTensor<uint16_t>&)dstSign, tmp0, mask0);
    Duplicate(tmp2, static_cast<uint16_t>(0), mask0);
    Add((RegTensor<uint16_t>&)z2, (RegTensor<uint16_t>&)dstSign, tmp2, mask0);
    Select(z1, z2, z1, maskNorm);
    Select(dst, z1, dst, mask0);
    MaskNot(mask0, mask0, mask);
    MaskAnd(maskValid, mask0, maskValid, mask);
    // True if underflow/overflow
    Compare<int16_t, CMPMODE::LT>(mask0, scale, (RegTensor<int16_t>&)tmp1, mask);
    // set overflown/underflown result to infinity
    MaskAnd(mask0, mask0, maskValid, mask);
    Duplicate(tmp0, 0, mask); // set to 0
    Add((RegTensor<uint16_t>&)z1, (RegTensor<uint16_t>&)dstSign, tmp0, mask0);
    Select(dst, z1, dst, mask0);
    MaskNot(mask0, mask0, mask);
    MaskAnd(maskValid, mask0, maskValid, mask);

    Duplicate(tmp0, 31, mask);
    Compare<int16_t, CMPMODE::EQ>(mask0, scale, (RegTensor<int16_t>&)tmp0, mask);
    MaskAnd(mask0, mask0, maskValid, mask);
    Reg::Duplicate(tmp1, 1, mask0);
    Reg::Sub(tmp1, scale, tmp1, mask0);
    Select(scale, tmp1, scale, mask0);
    Muls(z1, dst, 2, mask0);
    Select(dst, z1, dst, mask0);

    Compare<int16_t, CMPMODE::GT>(mask0, scale, (RegTensor<int16_t>&)tmp0, mask);
    MaskAnd(mask0, mask0, maskValid, mask);
    Duplicate(tmp0, F16_INF, mask); // set to infinity
    Add((RegTensor<uint16_t>&)z1, (RegTensor<uint16_t>&)dstSign, tmp0, mask0);
    Select(dst, z1, dst, mask0);
    MaskNot(mask0, mask0, mask);
    MaskAnd(maskValid, mask0, maskValid, mask);

    Duplicate(tmp0, 0, maskValid);
    Compare<int16_t, CMPMODE::GT>(mask0, scale, (RegTensor<int16_t>&)tmp0, maskValid);
    ShiftLefts(tmp1, scale, (int16_t)10, mask0);
    Mul(z1, dst, (RegTensor<half>&)tmp1, mask0);
    Select(dst, z1, dst, mask0);

    MaskNot(mask0, mask0, maskValid);
    Duplicate(tmp0, 512, mask0); // set 0x0200
    Abs(scale, scale, mask0);
    ShiftRight(scale, (RegTensor<int16_t>&)tmp0, scale, mask0);
    Mul(z1, dst, (RegTensor<half>&)scale, mask0);
    Select(dst, z1, dst, mask0);

    // get the position of nan
    Duplicate(tmp0, nan.i, mask);
    Compare<half, CMPMODE::NE>(maskNan, src0Abs, src0Abs, mask);
    Compare<half, CMPMODE::NE>(maskTmp, src1Abs, src1Abs, mask);
    MaskOr(maskNan, maskNan, maskTmp, mask);
    // set output with nan input to nan
    Select(dst, (RegTensor<half>&)tmp0, dst, maskNan);
}

template <typename T = DefaultType, auto mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void DivImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(
        IsSameType<decltype(mode), MaskMergeMode>::value || IsSameType<decltype(mode), const DivSpecificMode*>::value,
        "mode type must be either MaskMergeMode or const DivSpecificMode* ");
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    constexpr DivSpecificMode sprMode = Internal::GetDivSpecificMode(mode);
    static_assert(
        SupportType<
            ActualT, uint16_t, int16_t, uint32_t, int32_t, half, float, int64_t, uint64_t, complex32, complex64>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<sprMode.mrgMode, MaskMergeMode::ZEROING>(),
        "current Div api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<sprMode.mrgMode>();
    if constexpr (sprMode.precisionMode) {
        static_assert(
            SupportType<ActualT, float, complex64>(),
            "only float and complex64 data type is supported in precsion mode.");
    }
    if constexpr (sprMode.algo == DivAlgo::PRECISION_1ULP_FTZ_FALSE) {
        static_assert(
            SupportType<ActualT, half, float>(), "Reg Div for PRECISION_1ULP_FTZ_FALSE only supports half/float.");
    } else if constexpr (sprMode.algo == DivAlgo::PRECISION_0ULP_FTZ_FALSE) {
        static_assert(SupportType<ActualT, float>(), "Reg Div for PRECISION_0ULP_FTZ_FALSE only supports float.");
    } else if constexpr (sprMode.algo == DivAlgo::PRECISION_0ULP_FTZ_TRUE) {
        static_assert(
            SupportType<ActualT, float, complex64>(),
            "Reg Div for PRECISION_0ULP_FTZ_TRUE only supports float/complex64.");
    }
    if constexpr (SupportType<ActualT, complex32>()) {
        if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            DivComplex32TwoImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
        } else {
            DivComplex32OnetraitImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
        }
    } else if constexpr (SupportBytes<ActualT, 8>()) {
        if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            U dstTemp;
            DivB64Impl<T, mode, Reg::RegTensor<ActualT, Reg::RegTraitNumTwo>>(dstTemp, srcReg0, srcReg1, mask);
            dstReg = dstTemp;
        } else {
            CalTraitOneByTransToTraitTwo<T, mode, U, DivB64Impl<T, mode, Reg::RegTensor<ActualT, Reg::RegTraitNumTwo>>>(
                dstReg, srcReg0, srcReg1, mask);
        }
    } else if constexpr (SupportType<ActualT, half>()) {
        if constexpr (
            sprMode.algo == DivAlgo::PRECISION_1ULP_FTZ_FALSE
#if !defined(__ASC_FTZ__)
            || sprMode.algo == DivAlgo::INTRINSIC
#endif
        ) {
            DivIEEE754HalfImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
        } else {
            vdiv(dstReg, srcReg0, srcReg1, mask, modeValue);
        }
    } else if constexpr (SupportType<ActualT, float>()) {
#if !defined(__ASC_FTZ__)
        if constexpr (sprMode.precisionMode) {
            if constexpr (sprMode.algo == DivAlgo::PRECISION_0ULP_FTZ_TRUE) {
                DivPrecisionImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
            } else {
                DivIEEE754FloatImpl<T, mode, U, true>(dstReg, srcReg0, srcReg1, mask);
            }
        } else {
            if constexpr (sprMode.algo == DivAlgo::PRECISION_0ULP_FTZ_TRUE) {
                DivPrecisionImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
            } else if constexpr (sprMode.algo == DivAlgo::PRECISION_1ULP_FTZ_TRUE) {
                vdiv(dstReg, srcReg0, srcReg1, mask, modeValue);
            } else if constexpr (
                sprMode.algo == DivAlgo::DIFF_COMPENSATION || sprMode.algo == DivAlgo::PRECISION_0ULP_FTZ_FALSE) {
                DivIEEE754FloatImpl<T, mode, U, true>(dstReg, srcReg0, srcReg1, mask);
            } else {
                DivIEEE754FloatImpl<T, mode, U, false>(dstReg, srcReg0, srcReg1, mask);
            }
        }
#else
        if constexpr (sprMode.precisionMode) {
            if constexpr (sprMode.algo == DivAlgo::PRECISION_0ULP_FTZ_FALSE) {
                DivIEEE754FloatImpl<T, mode, U, true>(dstReg, srcReg0, srcReg1, mask);
            } else {
                DivPrecisionImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
            }
        } else {
            if constexpr (sprMode.algo == DivAlgo::PRECISION_0ULP_FTZ_FALSE) {
                DivIEEE754FloatImpl<T, mode, U, true>(dstReg, srcReg0, srcReg1, mask);
            } else if constexpr (sprMode.algo == DivAlgo::PRECISION_1ULP_FTZ_FALSE) {
                DivIEEE754FloatImpl<T, mode, U, false>(dstReg, srcReg0, srcReg1, mask);
            } else if constexpr (
                sprMode.algo == DivAlgo::PRECISION_0ULP_FTZ_TRUE || sprMode.algo == DivAlgo::DIFF_COMPENSATION) {
                DivPrecisionImpl<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
            } else {
                vdiv(dstReg, srcReg0, srcReg1, mask, modeValue);
            }
        }
#endif
    } else {
        vdiv(dstReg, srcReg0, srcReg1, mask, modeValue);
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void MaxOperator(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (CheckRegTrait<U, RegTraitNumOne>()) {
        if constexpr (sizeof(ActualT) < 8) {
            vmax(dstReg, srcReg0, srcReg1, mask, modeValue);
        } else if constexpr (sizeof(ActualT) == 8) {
            MaskReg maskTrait2;
            MaskPack(maskTrait2, mask);
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg0;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg1;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoTmpReg;
            B64TraitOneToTraitTwo(traitTwoSrcReg0, srcReg0);
            B64TraitOneToTraitTwo(traitTwoSrcReg1, srcReg1);
            MaskReg selMask;
            Compare<ActualT, CMPMODE::GT>(selMask, traitTwoSrcReg0, traitTwoSrcReg1, maskTrait2);
            Select(traitTwoDstReg, traitTwoSrcReg0, traitTwoSrcReg1, selMask);
            Duplicate(traitTwoTmpReg, static_cast<ActualT>(0));
            Select(traitTwoDstReg, traitTwoDstReg, traitTwoTmpReg, maskTrait2);
            B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
        }
    } else if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
        U tmpReg;
        MaskReg selMask;
        Compare<ActualT, CMPMODE::GT>(selMask, srcReg0, srcReg1, mask);
        Select(dstReg, srcReg0, srcReg1, selMask);
        Duplicate(tmpReg, static_cast<ActualT>(0));
        Select(dstReg, dstReg, tmpReg, mask);
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void MaxImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<
            ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t, uint64_t,
            int64_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Max api only supported Mode ZEROING/MERGING on current device!");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        MaxOperator<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        U dstCopyReg;
        MaxOperator<T, MaskMergeMode::ZEROING, U>(dstCopyReg, srcReg0, srcReg1, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void MinOperator(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (CheckRegTrait<U, RegTraitNumOne>()) {
        if constexpr (sizeof(ActualT) < 8) {
            vmin(dstReg, srcReg0, srcReg1, mask, modeValue);
        } else if constexpr (sizeof(ActualT) == 8) {
            MaskReg maskTrait2;
            MaskPack(maskTrait2, mask);
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg0;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg1;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoTmpReg;
            B64TraitOneToTraitTwo(traitTwoSrcReg0, srcReg0);
            B64TraitOneToTraitTwo(traitTwoSrcReg1, srcReg1);
            MaskReg selMask;
            Compare<ActualT, CMPMODE::LT>(selMask, traitTwoSrcReg0, traitTwoSrcReg1, maskTrait2);
            Select(traitTwoDstReg, traitTwoSrcReg0, traitTwoSrcReg1, selMask);
            Duplicate(traitTwoTmpReg, static_cast<ActualT>(0));
            Select(traitTwoDstReg, traitTwoDstReg, traitTwoTmpReg, maskTrait2);
            B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
        }
    } else if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
        U tmpReg;
        MaskReg selMask;
        Compare<ActualT, CMPMODE::LT>(selMask, srcReg0, srcReg1, mask);
        Select(dstReg, srcReg0, srcReg1, selMask);
        Duplicate(tmpReg, static_cast<ActualT>(0));
        Select(dstReg, dstReg, tmpReg, mask);
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void MinImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<
            ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t, uint64_t,
            int64_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING, MaskMergeMode::MERGING>(),
        "current Min api only supported Mode ZEROING/MERGING on current device!");
    if constexpr (mode == MaskMergeMode::ZEROING) {
        MinOperator<T, mode, U>(dstReg, srcReg0, srcReg1, mask);
    } else if constexpr (mode == MaskMergeMode::MERGING) {
        U dstCopyReg;
        MinOperator<T, MaskMergeMode::ZEROING, U>(dstCopyReg, srcReg0, srcReg1, mask);
        CopyMerging(dstReg, dstCopyReg, mask);
    }
}

template <typename T = int32_t, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U, typename S>
__simd_callee__ inline void ShiftL(U& dstReg, U& srcReg0, S& srcReg1, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    constexpr int32_t Bias = 32;
    RegTensor<int32_t> vRegTemp0, vRegTemp1, vRegTemp2;
    RegTensor<uint32_t> vRegTemp3;
    RegTensor<T> vRegTemp4;
    Duplicate(vRegTemp0, Bias, mask);
    Sub(vRegTemp1, vRegTemp0, srcReg1, mask);
    Adds(vRegTemp2, srcReg1, Bias, mask);
    vshl(vRegTemp3, (RegTensor<uint32_t>&)srcReg0.reg[0], srcReg1, mask, modeValue);
    vshl(vRegTemp4, (RegTensor<T>&)srcReg0.reg[1], vRegTemp2, mask, modeValue);
    Or((RegTensor<uint32_t>&)dstReg.reg[0], vRegTemp3, (RegTensor<uint32_t>&)vRegTemp4, mask);
    vshr(vRegTemp3, (RegTensor<uint32_t>&)srcReg0.reg[0], vRegTemp1, mask, modeValue);
    vshl(vRegTemp4, (RegTensor<T>&)srcReg0.reg[1], srcReg1, mask, modeValue);
    Or((RegTensor<T>&)dstReg.reg[1], (RegTensor<T>&)vRegTemp3, vRegTemp4, mask);
}

template <MaskMergeMode mode = MaskMergeMode::ZEROING, typename T, typename U>
__simd_callee__ inline void ShiftLeftB64Impl(T& dstReg, T& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename T::ActualT;
    using ActualU = typename U::ActualT;
    static_assert(sizeof(ActualT) == 8, "T data type should be B64");
    static_assert(sizeof(ActualU) == 4, "U data type should be int32_t");
    static_assert(CheckRegTrait<T, RegTraitNumTwo>(), "T should be RegTraitNumTwo");
    if (Std::is_same_v<ActualT, uint64_t>) {
        ShiftL<uint32_t, mode>(dstReg, srcReg0, srcReg1, mask);
    } else if (Std::is_same_v<ActualT, int64_t>) {
        ShiftL<int32_t, mode>(dstReg, srcReg0, srcReg1, mask);
    }
}

template <
    typename T = DefaultType, typename U = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S,
    typename V>
__simd_callee__ inline void ShiftLeftImpl(S& dstReg, S& srcReg0, V& srcReg1, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(Std::is_same_v<U, DefaultType> || Std::is_same_v<U, ActualU>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportType<ActualU, int8_t, int16_t, int32_t, int64_t>(),
        "current src1 data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current ShiftLeft api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (sizeof(ActualT) != 8) {
        vshl(dstReg, srcReg0, srcReg1, mask, modeValue);
    } else {
        if constexpr (CheckRegTrait<S, RegTraitNumOne>()) {
            MaskReg maskTrait2;
            MaskPack(maskTrait2, mask);
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg0, traitTwoSrcReg1;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
            B64TraitOneToTraitTwo(traitTwoSrcReg0, srcReg0);
            B64TraitOneToTraitTwo(traitTwoSrcReg1, srcReg1);
            ShiftLeftB64Impl<mode>(traitTwoDstReg, traitTwoSrcReg0, (RegTensor<int32_t>&)traitTwoSrcReg1, maskTrait2);
            B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
        } else if constexpr (CheckRegTrait<S, RegTraitNumTwo>()) {
            S dstTemp;
            ShiftLeftB64Impl<mode>(dstTemp, srcReg0, (RegTensor<int32_t>&)srcReg1, mask);
            dstReg = dstTemp;
        }
    }
}

template <typename T = int32_t, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U, typename S>
__simd_callee__ inline void ShiftR(U& dstReg, U& srcReg0, S& srcReg1, MaskReg& mask)
{
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    constexpr int32_t Bias = 32;
    RegTensor<int32_t> vRegTemp0, vRegTemp1, vRegTemp2;
    RegTensor<uint32_t> vRegTemp3;
    RegTensor<T> vRegTemp4;
    Duplicate(vRegTemp0, Bias, mask);
    Sub(vRegTemp1, vRegTemp0, srcReg1, mask);
    Adds(vRegTemp2, srcReg1, Bias, mask);
    vshr(vRegTemp3, (RegTensor<uint32_t>&)srcReg0.reg[0], srcReg1, mask, modeValue);
    vshl(vRegTemp4, (RegTensor<T>&)srcReg0.reg[1], vRegTemp1, mask, modeValue);
    Or((RegTensor<uint32_t>&)dstReg.reg[0], vRegTemp3, (RegTensor<uint32_t>&)vRegTemp4, mask);
    vshr(vRegTemp3, (RegTensor<uint32_t>&)srcReg0.reg[0], vRegTemp2, mask, modeValue);
    vshr(vRegTemp4, (RegTensor<T>&)srcReg0.reg[1], srcReg1, mask, modeValue);
    Or((RegTensor<T>&)dstReg.reg[1], (RegTensor<T>&)vRegTemp3, vRegTemp4, mask);
}

template <MaskMergeMode mode = MaskMergeMode::ZEROING, typename T, typename U>
__simd_callee__ inline void ShiftRightB64Impl(T& dstReg, T& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename T::ActualT;
    using ActualU = typename U::ActualT;
    static_assert(sizeof(ActualT) == 8, "T data type should be B64");
    static_assert(sizeof(ActualU) == 4, "U data type should be int32_t");
    static_assert(CheckRegTrait<T, RegTraitNumTwo>(), "T should be RegTraitNumTwo");
    if (Std::is_same_v<ActualT, uint64_t>) {
        ShiftR<uint32_t, mode>(dstReg, srcReg0, srcReg1, mask);
    } else if (Std::is_same_v<ActualT, int64_t>) {
        ShiftR<int32_t, mode>(dstReg, srcReg0, srcReg1, mask);
    }
}

template <
    typename T = DefaultType, typename U = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S,
    typename V>
__simd_callee__ inline void ShiftRightImpl(S& dstReg, S& srcReg0, V& srcReg1, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    using ActualU = typename V::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(Std::is_same_v<U, DefaultType> || Std::is_same_v<U, ActualU>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportType<ActualU, int8_t, int16_t, int32_t, int64_t>(),
        "current src1 data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current ShiftRight api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (sizeof(ActualT) != 8) {
        vshr(dstReg, srcReg0, srcReg1, mask, modeValue);
    } else {
        if constexpr (CheckRegTrait<S, RegTraitNumOne>()) {
            MaskReg maskTrait2;
            MaskPack(maskTrait2, mask);
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg0, traitTwoSrcReg1;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
            B64TraitOneToTraitTwo(traitTwoSrcReg0, srcReg0);
            B64TraitOneToTraitTwo(traitTwoSrcReg1, srcReg1);
            ShiftRightB64Impl<mode>(traitTwoDstReg, traitTwoSrcReg0, (RegTensor<int32_t>&)traitTwoSrcReg1, maskTrait2);
            B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
        } else if constexpr (CheckRegTrait<S, RegTraitNumTwo>()) {
            S dstTemp;
            ShiftRightB64Impl<mode>(dstTemp, srcReg0, (RegTensor<int32_t>&)srcReg1, mask);
            dstReg = dstTemp;
        }
    }
}

template <MaskMergeMode mode = MaskMergeMode::ZEROING, typename T>
__simd_callee__ inline void AndB64Impl(T& dstReg, T& srcReg0, T& srcReg1, MaskReg& mask)
{
    using ActualT = typename T::ActualT;
    static_assert(sizeof(ActualT) == 8, "AndB64Impl data type should be B64");
    static_assert(CheckRegTrait<T, RegTraitNumTwo>(), "AndB64Impl T should be RegTraitNumTwo");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (Std::is_same_v<ActualT, uint64_t>) {
        vand(
            (RegTensor<uint32_t>&)dstReg.reg[0], (RegTensor<uint32_t>&)srcReg0.reg[0],
            (RegTensor<uint32_t>&)srcReg1.reg[0], mask, modeValue);
        vand(
            (RegTensor<uint32_t>&)dstReg.reg[1], (RegTensor<uint32_t>&)srcReg0.reg[1],
            (RegTensor<uint32_t>&)srcReg1.reg[1], mask, modeValue);
    } else if constexpr (Std::is_same_v<ActualT, int64_t>) {
        vand(
            (RegTensor<uint32_t>&)dstReg.reg[0], (RegTensor<uint32_t>&)srcReg0.reg[0],
            (RegTensor<uint32_t>&)srcReg1.reg[0], mask, modeValue);
        vand(
            (RegTensor<int32_t>&)dstReg.reg[1], (RegTensor<int32_t>&)srcReg0.reg[1],
            (RegTensor<int32_t>&)srcReg1.reg[1], mask, modeValue);
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void AndImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, bool, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current And api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (IsSameType<ActualT, bool>::value) {
        vand((RegTensor<int8_t>&)dstReg, (RegTensor<int8_t>&)srcReg0, (RegTensor<int8_t>&)srcReg1, mask, modeValue);
    } else if constexpr (sizeof(ActualT) != 8) {
        vand(dstReg, srcReg0, srcReg1, mask, modeValue);
    } else {
        if constexpr (CheckRegTrait<U, RegTraitNumOne>()) {
            MaskReg maskTrait2;
            MaskPack(maskTrait2, mask);
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg0;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg1;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
            B64TraitOneToTraitTwo(traitTwoSrcReg0, srcReg0);
            B64TraitOneToTraitTwo(traitTwoSrcReg1, srcReg1);
            AndB64Impl<mode>(traitTwoDstReg, traitTwoSrcReg0, traitTwoSrcReg1, maskTrait2);
            B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
        } else if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            U dstTemp;
            AndB64Impl<mode>(dstTemp, srcReg0, srcReg1, mask);
            dstReg = dstTemp;
        }
    }
}

template <MaskMergeMode mode = MaskMergeMode::ZEROING, typename T>
__simd_callee__ inline void OrB64Impl(T& dstReg, T& srcReg0, T& srcReg1, MaskReg& mask)
{
    using ActualT = typename T::ActualT;
    static_assert(sizeof(ActualT) == 8, "OrB64Impl data type should be B64");
    static_assert(CheckRegTrait<T, RegTraitNumTwo>(), "OrB64Impl T should be RegTraitNumTwo");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (Std::is_same_v<ActualT, uint64_t>) {
        vor((RegTensor<uint32_t>&)dstReg.reg[0], (RegTensor<uint32_t>&)srcReg0.reg[0],
            (RegTensor<uint32_t>&)srcReg1.reg[0], mask, modeValue);
        vor((RegTensor<uint32_t>&)dstReg.reg[1], (RegTensor<uint32_t>&)srcReg0.reg[1],
            (RegTensor<uint32_t>&)srcReg1.reg[1], mask, modeValue);
    } else if constexpr (Std::is_same_v<ActualT, int64_t>) {
        vor((RegTensor<uint32_t>&)dstReg.reg[0], (RegTensor<uint32_t>&)srcReg0.reg[0],
            (RegTensor<uint32_t>&)srcReg1.reg[0], mask, modeValue);
        vor((RegTensor<int32_t>&)dstReg.reg[1], (RegTensor<int32_t>&)srcReg0.reg[1],
            (RegTensor<int32_t>&)srcReg1.reg[1], mask, modeValue);
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void OrImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, bool, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Or api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (IsSameType<ActualT, bool>::value) {
        vor((RegTensor<int8_t>&)dstReg, (RegTensor<int8_t>&)srcReg0, (RegTensor<int8_t>&)srcReg1, mask, modeValue);
    } else if constexpr (sizeof(ActualT) != 8) {
        vor(dstReg, srcReg0, srcReg1, mask, modeValue);
    } else {
        if constexpr (CheckRegTrait<U, RegTraitNumOne>()) {
            MaskReg maskTrait2;
            MaskPack(maskTrait2, mask);
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg0;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg1;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
            B64TraitOneToTraitTwo(traitTwoSrcReg0, srcReg0);
            B64TraitOneToTraitTwo(traitTwoSrcReg1, srcReg1);
            OrB64Impl<mode>(traitTwoDstReg, traitTwoSrcReg0, traitTwoSrcReg1, maskTrait2);
            B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
        } else if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            U dstTemp;
            OrB64Impl<mode>(dstTemp, srcReg0, srcReg1, mask);
            dstReg = dstTemp;
        }
    }
}

template <MaskMergeMode mode = MaskMergeMode::ZEROING, typename T>
__simd_callee__ inline void XorB64Impl(T& dstReg, T& srcReg0, T& srcReg1, MaskReg& mask)
{
    using ActualT = typename T::ActualT;
    static_assert(sizeof(ActualT) == 8, "XorB64Impl data type should be B64");
    static_assert(CheckRegTrait<T, RegTraitNumTwo>(), "XorB64Impl T should be RegTraitNumTwo");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (Std::is_same_v<ActualT, uint64_t>) {
        vxor(
            (RegTensor<uint32_t>&)dstReg.reg[0], (RegTensor<uint32_t>&)srcReg0.reg[0],
            (RegTensor<uint32_t>&)srcReg1.reg[0], mask, modeValue);
        vxor(
            (RegTensor<uint32_t>&)dstReg.reg[1], (RegTensor<uint32_t>&)srcReg0.reg[1],
            (RegTensor<uint32_t>&)srcReg1.reg[1], mask, modeValue);
    } else if constexpr (Std::is_same_v<ActualT, int64_t>) {
        vxor(
            (RegTensor<uint32_t>&)dstReg.reg[0], (RegTensor<uint32_t>&)srcReg0.reg[0],
            (RegTensor<uint32_t>&)srcReg1.reg[0], mask, modeValue);
        vxor(
            (RegTensor<int32_t>&)dstReg.reg[1], (RegTensor<int32_t>&)srcReg0.reg[1],
            (RegTensor<int32_t>&)srcReg1.reg[1], mask, modeValue);
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void XorImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, bool, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(), "current Xor api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (IsSameType<ActualT, bool>::value) {
        vxor((RegTensor<int8_t>&)dstReg, (RegTensor<int8_t>&)srcReg0, (RegTensor<int8_t>&)srcReg1, mask, modeValue);
    } else if constexpr (sizeof(ActualT) != 8) {
        vxor(dstReg, srcReg0, srcReg1, mask, modeValue);
    } else {
        if constexpr (CheckRegTrait<U, RegTraitNumOne>()) {
            MaskReg maskTrait2;
            MaskPack(maskTrait2, mask);
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg0;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg1;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
            B64TraitOneToTraitTwo(traitTwoSrcReg0, srcReg0);
            B64TraitOneToTraitTwo(traitTwoSrcReg1, srcReg1);
            XorB64Impl<mode>(traitTwoDstReg, traitTwoSrcReg0, traitTwoSrcReg1, maskTrait2);
            B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
        } else if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            U dstTemp;
            XorB64Impl<mode>(dstTemp, srcReg0, srcReg1, mask);
            dstReg = dstTemp;
        }
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void PreluImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportType<ActualT, half, float>(), "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current Prelu api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vprelu(dstReg, srcReg0, srcReg1, mask, modeValue);
}

template <typename T = DefaultType, typename U>
__simd_callee__ inline void MullImpl(U& dstReg0, U& dstReg1, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportType<ActualT, uint32_t, int32_t>(), "current data type is not supported on current device!");
    vmull(dstReg0, dstReg1, srcReg0, srcReg1, mask);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void MulAddDstImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<ActualT, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t, int64_t, uint64_t>(),
        "current data type is not supported on current device!");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current MulAddDst api only supported Mode ZEROING on current device!");
    if constexpr (sizeof(ActualT) == 8) {
        RegTensor<ActualT, RegTraitNumTwo> traitTwoDstTmpReg;
        if constexpr (CheckRegTrait<U, RegTraitNumOne>()) {
            MaskReg maskTrait2;
            MaskPack(maskTrait2, mask);
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg0;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg1;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
            B64TraitOneToTraitTwo(traitTwoSrcReg0, srcReg0);
            B64TraitOneToTraitTwo(traitTwoSrcReg1, srcReg1);
            B64TraitOneToTraitTwo(traitTwoDstReg, dstReg);
            Mul(traitTwoDstTmpReg, traitTwoSrcReg0, traitTwoSrcReg1, maskTrait2);
            Add(traitTwoDstReg, traitTwoDstTmpReg, traitTwoDstReg, maskTrait2);

            B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
        } else if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            Mul(traitTwoDstTmpReg, srcReg0, srcReg1, mask);
            Add(dstReg, traitTwoDstTmpReg, dstReg, mask);
        }
    } else {
        constexpr auto modeValue = GetMaskMergeMode<mode>();
        vmula(dstReg, srcReg0, srcReg1, mask, modeValue);
    }
}

template <typename T = DefaultType, typename U>
__simd_callee__ inline void AddCarryOutsImpl(
    MaskReg& carry, U& dstReg, U& srcReg0, U& srcReg1, MaskReg& carrySrc, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportType<ActualT, uint32_t, int32_t>(), "current data type is not supported on current device!");
    vaddcs(carry, dstReg, srcReg0, srcReg1, carrySrc, mask);
}

template <typename T = DefaultType, typename U>
__simd_callee__ inline void SubCarryOutsImpl(
    MaskReg& carry, U& dstReg, U& srcReg0, U& srcReg1, MaskReg& carrySrc, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportType<ActualT, uint32_t, int32_t>(), "current data type is not supported on current device!");
    vsubcs(carry, dstReg, srcReg0, srcReg1, carrySrc, mask);
}

template <typename T = DefaultType, typename U>
__simd_callee__ inline void AddCarryOutImpl(MaskReg& carry, U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportType<ActualT, uint32_t, int32_t>(), "current data type is not supported on current device!");
    vaddc(carry, dstReg, srcReg0, srcReg1, mask);
}

template <typename T = DefaultType, typename U>
__simd_callee__ inline void SubCarryOutImpl(MaskReg& carry, U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportType<ActualT, uint32_t, int32_t>(), "current data type is not supported on current device!");
    vsubc(carry, dstReg, srcReg0, srcReg1, mask);
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_VEC_BINARY_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_BINARY_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_BINARY_IMPL__
#endif
