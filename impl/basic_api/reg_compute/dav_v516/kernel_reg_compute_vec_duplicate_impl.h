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
 * \file kernel_reg_compute_vec_duplicate_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_v516/kernel_reg_compute_vec_duplicate_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_vec_duplicate_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_DUPLICATE_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_VEC_DUPLICATE_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_VEC_DUPLICATE_IMPL_H

#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_utils.h"
#include "kernel_reg_compute_common_impl.h"
#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_struct_intf.h"
#include "../../../../include/utils/std/type_traits.h"

namespace AscendC {
namespace Reg {
template <typename T, typename U>
__simd_callee__ inline void DuplicateComplexTraitTwoImpl(U& dstReg, T scalarValue)
{
    using ActualT = typename U::ActualT;
    static_assert(
        SupportType<ActualT, complex32, complex64>(), "current data type is not supported on current device!");
    static_assert(CheckRegTrait<U, RegTraitNumTwo>(), "U should be RegTraitNumTwo for DuplicateComplexTraitTwoImpl");
    ActualT scalarAux(scalarValue);
    vbr((RegTensor<typename ActualT::EleType>&)dstReg.reg[0], scalarAux.real);
    vbr((RegTensor<typename ActualT::EleType>&)dstReg.reg[1], scalarAux.imag);
}

template <typename T, typename U>
__simd_callee__ inline void DuplicateB64Impl(U& dstReg, T scalarValue)
{
    using ActualT = typename U::ActualT;
    static_assert(sizeof(ActualT) == 8, "data type should be B64");
    static_assert(CheckRegTrait<U, RegTraitNumTwo>(), "U should be RegTraitNumTwo for DuplicateB64Impl");
    if constexpr (SupportType<ActualT, complex64>()) {
        DuplicateComplexTraitTwoImpl(dstReg, scalarValue);
    } else {
        vbr((RegTensor<uint32_t>&)dstReg.reg[0], static_cast<uint32_t>(scalarValue));
        if constexpr (sizeof(T) == 8) {
            vbr((RegTensor<uint32_t>&)dstReg.reg[1], static_cast<uint32_t>(scalarValue >> 32));
        } else if constexpr (IsSameType<T, uint32_t>::value) {
            vbr((RegTensor<uint32_t>&)dstReg.reg[1], 0);
        } else {
            vbr((RegTensor<uint32_t>&)dstReg.reg[1], static_cast<uint32_t>(scalarValue >> 31));
        }
    }
}

template <typename T = DefaultType, typename U, typename S>
__simd_callee__ inline void DuplicateImpl(S& dstReg, U scalarValue)
{
    using ActualT = typename S::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        (SupportType<
            ActualT, bool, int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t,
            fp8_e8m0_t, uint16_t, int16_t, bfloat16_t, uint32_t, int32_t, float, half, uint64_t, int64_t, complex32,
            complex64>()),
        "current data type is not supported on current device!");
    static_assert(Std::is_convertible<U, ActualT>(), "scalarValue data type could be converted to RegTensor data type");

    if constexpr (IsSameType<ActualT, bool>::value) {
        vbr((RegTensor<int8_t>&)dstReg, (int8_t)scalarValue);
    } else if constexpr (
        IsSameType<ActualT, fp4x2_e2m1_t>::value || IsSameType<ActualT, fp4x2_e1m2_t>::value ||
        IsSameType<ActualT, hifloat8_t>::value || IsSameType<ActualT, fp8_e8m0_t>::value ||
        IsSameType<ActualT, fp8_e5m2_t>::value || IsSameType<ActualT, fp8_e4m3fn_t>::value) {
        vbr((RegTensor<int8_t>&)dstReg, (int8_t&)scalarValue);
    } else if constexpr (sizeof(ActualT) != 8) {
        if constexpr (SupportType<ActualT, complex32>()) {
            if constexpr (CheckRegTrait<S, RegTraitNumOne>()) {
                RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
                DuplicateComplexTraitTwoImpl(traitTwoDstReg, scalarValue);
                B32TraitTwoToTraitOne(dstReg, traitTwoDstReg);
            } else {
                DuplicateComplexTraitTwoImpl(dstReg, scalarValue);
            }
        } else {
            vbr(dstReg, (ActualT)scalarValue);
        }
    } else {
        if constexpr (CheckRegTrait<S, RegTraitNumOne>()) {
            RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
            DuplicateB64Impl(traitTwoDstReg, scalarValue);
            B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
        } else if constexpr (CheckRegTrait<S, RegTraitNumTwo>()) {
            S dstTemp;
            DuplicateB64Impl(dstTemp, scalarValue);
            dstReg = dstTemp;
        }
    }
}

template <MaskMergeMode mode = MaskMergeMode::ZEROING, typename T, typename U>
__simd_callee__ inline void DuplicateComplexTraitTwoImpl(U& dstReg, T scalarValue, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(
        SupportType<ActualT, complex32, complex64>(), "current data type is not supported on current device!");
    static_assert(CheckRegTrait<U, RegTraitNumTwo>(), "U should be RegTraitNumTwo for DuplicateComplexTraitTwoImpl");
    ActualT scalarAux(scalarValue);
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vdup((RegTensor<typename ActualT::EleType>&)dstReg.reg[0], scalarAux.real, mask, modeValue);
    vdup((RegTensor<typename ActualT::EleType>&)dstReg.reg[1], scalarAux.imag, mask, modeValue);
}

template <MaskMergeMode mode = MaskMergeMode::ZEROING, typename T, typename U>
__simd_callee__ inline void DuplicateB64Impl(U& dstReg, T scalarValue, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(sizeof(ActualT) == 8, "data type should be B64");
    static_assert(CheckRegTrait<U, RegTraitNumTwo>(), "U should be RegTraitNumTwo for DuplicateB64Impl");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (SupportType<ActualT, complex64>()) {
        DuplicateComplexTraitTwoImpl(dstReg, scalarValue, mask);
    } else {
        vdup((RegTensor<uint32_t>&)dstReg.reg[0], static_cast<uint32_t>(scalarValue), mask, modeValue);
        if constexpr (sizeof(T) == 8) {
            vdup((RegTensor<uint32_t>&)dstReg.reg[1], static_cast<uint32_t>(scalarValue >> 32), mask, modeValue);
        } else if constexpr (IsSameType<T, uint32_t>::value) {
            vdup((RegTensor<uint32_t>&)dstReg.reg[1], 0, mask, modeValue);
        } else {
            vdup((RegTensor<uint32_t>&)dstReg.reg[1], static_cast<uint32_t>(scalarValue >> 31), mask, modeValue);
        }
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U, typename S>
__simd_callee__ inline void DuplicateImpl(S& dstReg, U scalarValue, MaskReg& mask)
{
    using ActualT = typename S::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        (SupportType<
            ActualT, bool, int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t,
            fp8_e8m0_t, uint16_t, int16_t, bfloat16_t, uint32_t, int32_t, float, half, uint64_t, int64_t, complex32,
            complex64>()),
        "current data type is not supported on current device!");
    static_assert(Std::is_convertible<U, ActualT>(), "scalarValue data type could be converted to RegTensor data type");
    ASCENDC_ASSERT((mode != MaskMergeMode::UNKNOWN), {
        KERNEL_LOG(KERNEL_ERROR, "The MergeMode only support: MODE_MERGING, MODE_ZEROING.");
    });
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (IsSameType<ActualT, bool>::value) {
        vdup((RegTensor<int8_t>&)dstReg, (int8_t)scalarValue, mask, modeValue);
    } else if constexpr (
        IsSameType<ActualT, fp4x2_e2m1_t>::value || IsSameType<ActualT, fp4x2_e1m2_t>::value ||
        IsSameType<ActualT, hifloat8_t>::value || IsSameType<ActualT, fp8_e8m0_t>::value ||
        IsSameType<ActualT, fp8_e5m2_t>::value || IsSameType<ActualT, fp8_e4m3fn_t>::value) {
        vdup((RegTensor<int8_t>&)dstReg, (int8_t&)scalarValue, mask, modeValue);
    } else if constexpr (sizeof(ActualT) != 8) {
        if constexpr (IsSameType<ActualT, complex32>::value) {
            if constexpr (CheckRegTrait<S, RegTraitNumOne>()) {
                MaskReg maskTrait2;
                MaskPack(maskTrait2, mask);
                RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
                DuplicateComplexTraitTwoImpl(traitTwoDstReg, scalarValue, maskTrait2);
                B32TraitTwoToTraitOne(dstReg, traitTwoDstReg);
            } else {
                DuplicateComplexTraitTwoImpl(dstReg, scalarValue, mask);
            }
        } else {
            vdup(dstReg, (ActualT)scalarValue, mask, modeValue);
        }
    } else {
        if constexpr (CheckRegTrait<S, RegTraitNumOne>()) {
            MaskReg maskTrait2;
            MaskPack(maskTrait2, mask);
            if constexpr (mode == MaskMergeMode::MERGING) {
                RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
                B64TraitOneToTraitTwo(traitTwoDstReg, dstReg);
                DuplicateB64Impl<mode>(traitTwoDstReg, scalarValue, maskTrait2);
                B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
            } else {
                RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
                DuplicateB64Impl<mode>(traitTwoDstReg, scalarValue, maskTrait2);
                B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
            }
        } else if constexpr (CheckRegTrait<S, RegTraitNumTwo>()) {
            S dstTemp;
            DuplicateB64Impl<mode>(dstTemp, scalarValue, mask);
            dstReg = dstTemp;
        }
    }
}

template <HighLowPart pos = HighLowPart::LOWEST, MaskMergeMode mode = MaskMergeMode::ZEROING, typename T>
__simd_callee__ inline void DuplicateComplexTraitTwoImpl(T& dstReg, T& srcReg, MaskReg& mask)
{
    using ActualT = typename T::ActualT;
    static_assert(
        SupportType<ActualT, complex32, complex64>(), "current data type is not supported on current device!");
    static_assert(CheckRegTrait<T, RegTraitNumTwo>(), "T should be RegTraitNumTwo for DuplicateComplexTraitTwoImpl");
    constexpr auto posValue = std::integral_constant<::Pos, static_cast<::Pos>(pos)>();
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vdup(
        (RegTensor<typename ActualT::EleType>&)dstReg.reg[0], (RegTensor<typename ActualT::EleType>&)srcReg.reg[0],
        mask, posValue, modeValue);
    vdup(
        (RegTensor<typename ActualT::EleType>&)dstReg.reg[1], (RegTensor<typename ActualT::EleType>&)srcReg.reg[1],
        mask, posValue, modeValue);
}

template <HighLowPart pos = HighLowPart::LOWEST, MaskMergeMode mode = MaskMergeMode::ZEROING, typename T>
__simd_callee__ inline void DuplicateB64Impl(T& dstReg, T& srcReg, MaskReg& mask)
{
    using ActualT = typename T::ActualT;
    static_assert(sizeof(ActualT) == 8, "data type should be B64");
    static_assert(CheckRegTrait<T, RegTraitNumTwo>(), "T should be RegTraitNumTwo for DuplicateB64Impl");
    if constexpr (SupportType<ActualT, complex64>()) {
        DuplicateComplexTraitTwoImpl(dstReg, srcReg, mask);
    } else {
        constexpr auto posValue = std::integral_constant<::Pos, static_cast<::Pos>(pos)>();
        constexpr auto modeValue = GetMaskMergeMode<mode>();
        vdup((RegTensor<uint32_t>&)dstReg.reg[0], (RegTensor<uint32_t>&)srcReg.reg[0], mask, posValue, modeValue);
        vdup((RegTensor<uint32_t>&)dstReg.reg[1], (RegTensor<uint32_t>&)srcReg.reg[1], mask, posValue, modeValue);
    }
}

template <
    typename T = DefaultType, HighLowPart pos = HighLowPart::LOWEST, MaskMergeMode mode = MaskMergeMode::ZEROING,
    typename U>
__simd_callee__ inline void DuplicateImpl(U& dstReg, U& srcReg, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        (SupportType<
            ActualT, bool, int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t,
            fp8_e8m0_t, uint16_t, int16_t, bfloat16_t, uint32_t, int32_t, float, half, uint64_t, int64_t, complex32,
            complex64>()),
        "current data type is not supported on current device!");
    ASCENDC_ASSERT((mode != MaskMergeMode::UNKNOWN), {
        KERNEL_LOG(KERNEL_ERROR, "The MergeMode only support: MODE_MERGING, MODE_ZEROING.");
    });
    constexpr auto posValue = std::integral_constant<::Pos, static_cast<::Pos>(pos)>();
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (IsSameType<ActualT, bool>::value) {
        vdup((RegTensor<int8_t>&)dstReg, (RegTensor<int8_t>&)srcReg, mask, posValue, modeValue);
    } else if constexpr (
        IsSameType<ActualT, fp4x2_e2m1_t>::value || IsSameType<ActualT, fp4x2_e1m2_t>::value ||
        IsSameType<ActualT, hifloat8_t>::value || IsSameType<ActualT, fp8_e8m0_t>::value ||
        IsSameType<ActualT, fp8_e5m2_t>::value || IsSameType<ActualT, fp8_e4m3fn_t>::value) {
        vdup((RegTensor<int8_t>&)dstReg, (RegTensor<int8_t>&)srcReg, mask, posValue, modeValue);
    } else if constexpr (sizeof(ActualT) != 8) {
        if constexpr (IsSameType<ActualT, complex32>::value) {
            if constexpr (CheckRegTrait<U, RegTraitNumOne>()) {
                MaskReg maskTrait2;
                MaskPack(maskTrait2, mask);
                RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
                RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg;
                B32TraitOneToTraitTwo(traitTwoSrcReg, srcReg);
                DuplicateComplexTraitTwoImpl(traitTwoDstReg, traitTwoSrcReg, maskTrait2);
                B32TraitTwoToTraitOne(dstReg, traitTwoDstReg);
            } else {
                DuplicateComplexTraitTwoImpl(dstReg, srcReg, mask);
            }
        } else {
            vdup(dstReg, srcReg, mask, posValue, modeValue);
        }
    } else {
        if constexpr (CheckRegTrait<U, RegTraitNumOne>()) {
            MaskReg maskTrait2;
            MaskPack(maskTrait2, mask);
            RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg;
            B64TraitOneToTraitTwo(traitTwoSrcReg, srcReg);
            DuplicateB64Impl<pos, mode>(traitTwoDstReg, traitTwoSrcReg, maskTrait2);
            B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
        } else if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            U dstTemp;
            DuplicateB64Impl<pos, mode>(dstTemp, srcReg, mask);
            dstReg = dstTemp;
        }
    }
}

template <typename T>
__simd_callee__ inline void InterleaveB64Impl(T& dstReg0, T& dstReg1, T& srcReg0, T& srcReg1)
{
    using ActualT = typename T::ActualT;
    static_assert(sizeof(ActualT) == 8, "data type should be B64");
    static_assert(CheckRegTrait<T, RegTraitNumTwo>(), "T should be RegTraitNumTwo for InterleaveB64Impl");
    Interleave(
        (RegTensor<uint32_t>&)dstReg0.reg[0], (RegTensor<uint32_t>&)dstReg1.reg[0],
        (RegTensor<uint32_t>&)srcReg0.reg[0], (RegTensor<uint32_t>&)srcReg1.reg[0]);
    Interleave(
        (RegTensor<uint32_t>&)dstReg0.reg[1], (RegTensor<uint32_t>&)dstReg1.reg[1],
        (RegTensor<uint32_t>&)srcReg0.reg[1], (RegTensor<uint32_t>&)srcReg1.reg[1]);
}

template <typename T = DefaultType, typename U>
__simd_callee__ inline void InterleaveImpl(U& dstReg0, U& dstReg1, U& srcReg0, U& srcReg1)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportBytes<ActualT, 1, 2, 4, 8>(), "Interleave only support type bool/b8/b16/b32/b64 on current device");
    if constexpr (sizeof(ActualT) == 1) {
        vintlv(
            (RegTensor<int8_t>&)dstReg0, (RegTensor<int8_t>&)dstReg1, (RegTensor<int8_t>&)srcReg0,
            (RegTensor<int8_t>&)srcReg1);
    } else if constexpr (sizeof(ActualT) == 2) {
        vintlv(
            (RegTensor<int16_t>&)dstReg0, (RegTensor<int16_t>&)dstReg1, (RegTensor<int16_t>&)srcReg0,
            (RegTensor<int16_t>&)srcReg1);
    } else if constexpr (sizeof(ActualT) == 4) {
        vintlv(
            (RegTensor<int32_t>&)dstReg0, (RegTensor<int32_t>&)dstReg1, (RegTensor<int32_t>&)srcReg0,
            (RegTensor<int32_t>&)srcReg1);
    } else {
        static_assert(CheckRegTrait<U, RegTraitNumTwo>(), "Interleave only support RegTraitNumTwo on current device");
        U dstTemp0;
        U dstTemp1;
        InterleaveB64Impl(dstTemp0, dstTemp1, srcReg0, srcReg1);
        dstReg0 = dstTemp0;
        dstReg1 = dstTemp1;
    }
}

template <typename T>
__simd_callee__ inline void DeInterleaveB64Impl(T& dstReg0, T& dstReg1, T& srcReg0, T& srcReg1)
{
    using ActualT = typename T::ActualT;
    static_assert(sizeof(ActualT) == 8, "data type should be B64");
    static_assert(CheckRegTrait<T, RegTraitNumTwo>(), "T should be RegTraitNumTwo for DeInterleaveB64Impl");
    DeInterleave(
        (RegTensor<uint32_t>&)dstReg0.reg[0], (RegTensor<uint32_t>&)dstReg1.reg[0],
        (RegTensor<uint32_t>&)srcReg0.reg[0], (RegTensor<uint32_t>&)srcReg1.reg[0]);
    DeInterleave(
        (RegTensor<uint32_t>&)dstReg0.reg[1], (RegTensor<uint32_t>&)dstReg1.reg[1],
        (RegTensor<uint32_t>&)srcReg0.reg[1], (RegTensor<uint32_t>&)srcReg1.reg[1]);
}

template <typename T = DefaultType, typename U>
__simd_callee__ inline void DeInterleaveImpl(U& dstReg0, U& dstReg1, U& srcReg0, U& srcReg1)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportBytes<ActualT, 1, 2, 4, 8>(), "DeInterleave only support type bool/b8/b16/b32/b64 on current device");
    if constexpr (sizeof(ActualT) == 1) {
        vdintlv(
            (RegTensor<int8_t>&)dstReg0, (RegTensor<int8_t>&)dstReg1, (RegTensor<int8_t>&)srcReg0,
            (RegTensor<int8_t>&)srcReg1);
    } else if constexpr (sizeof(ActualT) == 2) {
        vdintlv(
            (RegTensor<int16_t>&)dstReg0, (RegTensor<int16_t>&)dstReg1, (RegTensor<int16_t>&)srcReg0,
            (RegTensor<int16_t>&)srcReg1);
    } else if constexpr (sizeof(ActualT) == 4) {
        vdintlv(
            (RegTensor<int32_t>&)dstReg0, (RegTensor<int32_t>&)dstReg1, (RegTensor<int32_t>&)srcReg0,
            (RegTensor<int32_t>&)srcReg1);
    } else {
        static_assert(CheckRegTrait<U, RegTraitNumTwo>(), "DeInterleave only support RegTraitNumTwo on current device");
        U dstTemp0;
        U dstTemp1;
        DeInterleaveB64Impl(dstTemp0, dstTemp1, srcReg0, srcReg1);
        dstReg0 = dstTemp0;
        dstReg1 = dstTemp1;
    }
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_VEC_DUPLICATE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_DUPLICATE_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_DUPLICATE_IMPL__
#endif
