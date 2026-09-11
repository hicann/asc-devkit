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
 * \file kernel_reg_compute_vec_cmpsel_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_v516/kernel_reg_compute_vec_cmpsel_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_vec_cmpsel_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_CMPSEL_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_VEC_CMPSEL_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_VEC_CMPSEL_IMPL_H

#include "kernel_reg_compute_common_impl.h"
#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_struct_intf.h"
#include "../../../../include/utils/std/type_traits.h"

namespace AscendC {
namespace Reg {
template <CMPMODE mode = CMPMODE::EQ, typename T>
__simd_callee__ inline void CompareUint64Impl(MaskReg& dst, T& srcReg0, T& srcReg1, MaskReg& mask)
{
    using ActualT = typename T::ActualT;
    static_assert(SupportType<ActualT, uint64_t>(), "CompareUint64Impl only support uint64_t type");
    static_assert(CheckRegTrait<T, RegTraitNumTwo>(), "CompareUint64Impl only support RegTraitNumTwo");
    MaskReg lowEq;
    MaskReg highEq;
    MaskReg lowCmp;
    MaskReg highCmp;
    if constexpr (mode == CMPMODE::EQ) {
        vcmp_eq(lowEq, (RegTensor<uint32_t>&)srcReg0.reg[0], (RegTensor<uint32_t>&)srcReg1.reg[0], mask);
        vcmp_eq(dst, (RegTensor<uint32_t>&)srcReg0.reg[1], (RegTensor<uint32_t>&)srcReg1.reg[1], lowEq);
    } else if constexpr (mode == CMPMODE::NE) {
        vcmp_ne(lowEq, (RegTensor<uint32_t>&)srcReg0.reg[0], (RegTensor<uint32_t>&)srcReg1.reg[0], mask);
        vcmp_ne(highEq, (RegTensor<uint32_t>&)srcReg0.reg[1], (RegTensor<uint32_t>&)srcReg1.reg[1], mask);
        por(dst, lowEq, highEq, mask);
    } else if constexpr (mode == CMPMODE::GT) {
        vcmp_eq(highEq, (RegTensor<uint32_t>&)srcReg0.reg[1], (RegTensor<uint32_t>&)srcReg1.reg[1], mask);
        vcmp_gt(lowCmp, (RegTensor<uint32_t>&)srcReg0.reg[0], (RegTensor<uint32_t>&)srcReg1.reg[0], mask);
        vcmp_gt(highCmp, (RegTensor<uint32_t>&)srcReg0.reg[1], (RegTensor<uint32_t>&)srcReg1.reg[1], mask);
        psel(dst, lowCmp, highCmp, highEq);
    } else if constexpr (mode == CMPMODE::GE) {
        vcmp_eq(highEq, (RegTensor<uint32_t>&)srcReg0.reg[1], (RegTensor<uint32_t>&)srcReg1.reg[1], mask);
        vcmp_ge(lowCmp, (RegTensor<uint32_t>&)srcReg0.reg[0], (RegTensor<uint32_t>&)srcReg1.reg[0], mask);
        vcmp_ge(highCmp, (RegTensor<uint32_t>&)srcReg0.reg[1], (RegTensor<uint32_t>&)srcReg1.reg[1], mask);
        psel(dst, lowCmp, highCmp, highEq);
    } else if constexpr (mode == CMPMODE::LT) {
        vcmp_eq(highEq, (RegTensor<uint32_t>&)srcReg0.reg[1], (RegTensor<uint32_t>&)srcReg1.reg[1], mask);
        vcmp_lt(lowCmp, (RegTensor<uint32_t>&)srcReg0.reg[0], (RegTensor<uint32_t>&)srcReg1.reg[0], mask);
        vcmp_lt(highCmp, (RegTensor<uint32_t>&)srcReg0.reg[1], (RegTensor<uint32_t>&)srcReg1.reg[1], mask);
        psel(dst, lowCmp, highCmp, highEq);
    } else if constexpr (mode == CMPMODE::LE) {
        vcmp_eq(highEq, (RegTensor<uint32_t>&)srcReg0.reg[1], (RegTensor<uint32_t>&)srcReg1.reg[1], mask);
        vcmp_le(lowCmp, (RegTensor<uint32_t>&)srcReg0.reg[0], (RegTensor<uint32_t>&)srcReg1.reg[0], mask);
        vcmp_le(highCmp, (RegTensor<uint32_t>&)srcReg0.reg[1], (RegTensor<uint32_t>&)srcReg1.reg[1], mask);
        psel(dst, lowCmp, highCmp, highEq);
    }
}

template <CMPMODE mode = CMPMODE::EQ, typename T>
__simd_callee__ inline void CompareInt64Impl(MaskReg& dst, T& srcReg0, T& srcReg1, MaskReg& mask)
{
    using ActualT = typename T::ActualT;
    static_assert(SupportType<ActualT, int64_t>(), "CompareInt64Impl only support int64_t type");
    static_assert(CheckRegTrait<T, RegTraitNumTwo>(), "CompareInt64Impl only support RegTraitNumTwo");
    MaskReg lowEq;
    MaskReg highEq;
    MaskReg lowCmp;
    MaskReg highCmp;
    if constexpr (mode == CMPMODE::EQ) {
        vcmp_eq(lowEq, (RegTensor<uint32_t>&)srcReg0.reg[0], (RegTensor<uint32_t>&)srcReg1.reg[0], mask);
        vcmp_eq(dst, (RegTensor<int32_t>&)srcReg0.reg[1], (RegTensor<int32_t>&)srcReg1.reg[1], lowEq);
    } else if constexpr (mode == CMPMODE::NE) {
        vcmp_ne(lowEq, (RegTensor<uint32_t>&)srcReg0.reg[0], (RegTensor<uint32_t>&)srcReg1.reg[0], mask);
        vcmp_ne(highEq, (RegTensor<int32_t>&)srcReg0.reg[1], (RegTensor<int32_t>&)srcReg1.reg[1], mask);
        por(dst, lowEq, highEq, mask);
    } else if constexpr (mode == CMPMODE::GT) {
        vcmp_eq(highEq, (RegTensor<int32_t>&)srcReg0.reg[1], (RegTensor<int32_t>&)srcReg1.reg[1], mask);
        vcmp_gt(lowCmp, (RegTensor<uint32_t>&)srcReg0.reg[0], (RegTensor<uint32_t>&)srcReg1.reg[0], mask);
        vcmp_gt(highCmp, (RegTensor<int32_t>&)srcReg0.reg[1], (RegTensor<int32_t>&)srcReg1.reg[1], mask);
        psel(dst, lowCmp, highCmp, highEq);
    } else if constexpr (mode == CMPMODE::GE) {
        vcmp_eq(highEq, (RegTensor<int32_t>&)srcReg0.reg[1], (RegTensor<int32_t>&)srcReg1.reg[1], mask);
        vcmp_ge(lowCmp, (RegTensor<uint32_t>&)srcReg0.reg[0], (RegTensor<uint32_t>&)srcReg1.reg[0], mask);
        vcmp_ge(highCmp, (RegTensor<int32_t>&)srcReg0.reg[1], (RegTensor<int32_t>&)srcReg1.reg[1], mask);
        psel(dst, lowCmp, highCmp, highEq);
    } else if constexpr (mode == CMPMODE::LT) {
        vcmp_eq(highEq, (RegTensor<int32_t>&)srcReg0.reg[1], (RegTensor<int32_t>&)srcReg1.reg[1], mask);
        vcmp_lt(lowCmp, (RegTensor<uint32_t>&)srcReg0.reg[0], (RegTensor<uint32_t>&)srcReg1.reg[0], mask);
        vcmp_lt(highCmp, (RegTensor<int32_t>&)srcReg0.reg[1], (RegTensor<int32_t>&)srcReg1.reg[1], mask);
        psel(dst, lowCmp, highCmp, highEq);
    } else if constexpr (mode == CMPMODE::LE) {
        vcmp_eq(highEq, (RegTensor<int32_t>&)srcReg0.reg[1], (RegTensor<int32_t>&)srcReg1.reg[1], mask);
        vcmp_le(lowCmp, (RegTensor<uint32_t>&)srcReg0.reg[0], (RegTensor<uint32_t>&)srcReg1.reg[0], mask);
        vcmp_le(highCmp, (RegTensor<int32_t>&)srcReg0.reg[1], (RegTensor<int32_t>&)srcReg1.reg[1], mask);
        psel(dst, lowCmp, highCmp, highEq);
    }
}

template <CMPMODE mode = CMPMODE::EQ, typename T>
__simd_callee__ inline void CompareB64Impl(MaskReg& dst, T& srcReg0, T& srcReg1, MaskReg& mask)
{
    using ActualT = typename T::ActualT;
    static_assert(SupportType<ActualT, uint64_t, int64_t>(), "CompareB64Impl only support uint64_t and int64_t type");
    static_assert(CheckRegTrait<T, RegTraitNumTwo>(), "CompareB64Impl only support RegTraitNumTwo");
    if constexpr (Std::is_same_v<ActualT, uint64_t>) {
        CompareUint64Impl<mode, T>(dst, srcReg0, srcReg1, mask);
    } else if constexpr (Std::is_same_v<ActualT, int64_t>) {
        CompareInt64Impl<mode, T>(dst, srcReg0, srcReg1, mask);
    }
}

template <typename T = DefaultType, CMPMODE mode = CMPMODE::EQ, typename U>
__simd_callee__ inline void CompareImpl(MaskReg& dst, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<
            ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t, uint64_t,
            int64_t>(),
        "current data type is not supported on current device!");
    if constexpr (sizeof(ActualT) < 8) {
        if constexpr (mode == CMPMODE::EQ) {
            vcmp_eq(dst, srcReg0, srcReg1, mask);
        } else if constexpr (mode == CMPMODE::NE) {
            vcmp_ne(dst, srcReg0, srcReg1, mask);
        } else if constexpr (mode == CMPMODE::GT) {
            vcmp_gt(dst, srcReg0, srcReg1, mask);
        } else if constexpr (mode == CMPMODE::GE) {
            vcmp_ge(dst, srcReg0, srcReg1, mask);
        } else if constexpr (mode == CMPMODE::LT) {
            vcmp_lt(dst, srcReg0, srcReg1, mask);
        } else if constexpr (mode == CMPMODE::LE) {
            vcmp_le(dst, srcReg0, srcReg1, mask);
        }
    } else if constexpr (sizeof(ActualT) == 8) {
        if constexpr (CheckRegTrait<U, RegTraitNumOne>()) {
            MaskReg maskTrait2;
            MaskPack(maskTrait2, mask);
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg0;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg1;
            B64TraitOneToTraitTwo(traitTwoSrcReg0, srcReg0);
            B64TraitOneToTraitTwo(traitTwoSrcReg1, srcReg1);
            CompareB64Impl<mode>(dst, traitTwoSrcReg0, traitTwoSrcReg1, maskTrait2);
            MaskUnPack(dst, dst);
        } else if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            CompareB64Impl<mode>(dst, srcReg0, srcReg1, mask);
        }
    }
}

template <typename T = DefaultType, CMPMODE mode = CMPMODE::EQ, typename U, typename S>
__simd_callee__ inline void CompareScalarImpl(MaskReg& dst, U& srcReg, S scalarValue, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportType<
            ActualT, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, half, float, bfloat16_t, uint64_t,
            int64_t>(),
        "current data type is not supported on current device!");
    static_assert(Std::is_convertible<S, ActualT>(), "scalarValue data type could be converted to RegTensor data type");
    if constexpr (sizeof(ActualT) < 8) {
        if constexpr (mode == CMPMODE::EQ) {
            vcmps_eq(dst, srcReg, scalarValue, mask);
        } else if constexpr (mode == CMPMODE::NE) {
            vcmps_ne(dst, srcReg, scalarValue, mask);
        } else if constexpr (mode == CMPMODE::GT) {
            vcmps_gt(dst, srcReg, scalarValue, mask);
        } else if constexpr (mode == CMPMODE::GE) {
            vcmps_ge(dst, srcReg, scalarValue, mask);
        } else if constexpr (mode == CMPMODE::LT) {
            vcmps_lt(dst, srcReg, scalarValue, mask);
        } else if constexpr (mode == CMPMODE::LE) {
            vcmps_le(dst, srcReg, scalarValue, mask);
        }
    } else if constexpr (sizeof(ActualT) == 8) {
        U dupReg;
        Duplicate(dupReg, scalarValue, mask);
        Compare<T, mode, U>(dst, srcReg, dupReg, mask);
    }
}

template <typename T = DefaultType, typename U>
__simd_callee__ inline void SelectImpl(U& dstReg, U& srcReg0, U& srcReg1, MaskReg& mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        SupportBytes<ActualT, 1, 2, 4, 8>(), "Select only supports datatype bool/b8/b16/b32/b64 on current device");
    if constexpr (sizeof(ActualT) == 1) {
        vsel((RegTensor<uint8_t>&)dstReg, (RegTensor<uint8_t>&)srcReg0, (RegTensor<uint8_t>&)srcReg1, mask);
    } else if constexpr (sizeof(ActualT) == 2) {
        vsel((RegTensor<uint16_t>&)dstReg, (RegTensor<uint16_t>&)srcReg0, (RegTensor<uint16_t>&)srcReg1, mask);
    } else if constexpr (sizeof(ActualT) == 4) {
        vsel((RegTensor<uint32_t>&)dstReg, (RegTensor<uint32_t>&)srcReg0, (RegTensor<uint32_t>&)srcReg1, mask);
    } else if constexpr (sizeof(ActualT) == 8) {
        if constexpr (CheckRegTrait<U, RegTraitNumOne>()) {
            MaskReg maskTrait2;
            MaskPack(maskTrait2, mask);
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg0;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg1;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
            B64TraitOneToTraitTwo(traitTwoSrcReg0, srcReg0);
            B64TraitOneToTraitTwo(traitTwoSrcReg1, srcReg1);
            vsel(
                (RegTensor<uint32_t>&)traitTwoDstReg.reg[0], (RegTensor<uint32_t>&)traitTwoSrcReg0.reg[0],
                (RegTensor<uint32_t>&)traitTwoSrcReg1.reg[0], maskTrait2);
            vsel(
                (RegTensor<uint32_t>&)traitTwoDstReg.reg[1], (RegTensor<uint32_t>&)traitTwoSrcReg0.reg[1],
                (RegTensor<uint32_t>&)traitTwoSrcReg1.reg[1], maskTrait2);
            B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
        } else if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            vsel(
                (RegTensor<uint32_t>&)dstReg.reg[0], (RegTensor<uint32_t>&)srcReg0.reg[0],
                (RegTensor<uint32_t>&)srcReg1.reg[0], mask);
            vsel(
                (RegTensor<uint32_t>&)dstReg.reg[1], (RegTensor<uint32_t>&)srcReg0.reg[1],
                (RegTensor<uint32_t>&)srcReg1.reg[1], mask);
        }
    }
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_VEC_CMPSEL_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_CMPSEL_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_CMPSEL_IMPL__
#endif
