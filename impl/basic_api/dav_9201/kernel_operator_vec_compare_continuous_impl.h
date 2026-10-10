/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file kernel_operator_vec_compare_continuous_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_9201/kernel_operator_vec_compare_continuous_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_vec_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_COMPARE_CONTINUOUS_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_COMPARE_CONTINUOUS_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_COMPARE_CONTINUOUS_IMPL_H

#include "../kernel_utils.h"
#include "../../../include/basic_api/reg_compute/kernel_reg_compute_intf.h"

namespace AscendC {

template <typename T = Reg::DefaultType, typename RegT>
__simd_callee__ inline void CompareEqualDoubleImpl(
    Reg::MaskReg& dstMask, RegT& srcReg0, RegT& srcReg1, Reg::MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    static_assert(
        SupportType<ActualT, double, uint64_t>(), "CompareEqualDoubleImpl only support double and uint64_t type");
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> tmpSrcReg0 = (Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo>&)srcReg0;
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> tmpSrcReg1 = (Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo>&)srcReg1;
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> exponent0;
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> exponent1;
    Reg::ShiftRights(exponent0, tmpSrcReg0, static_cast<int16_t>(52), mask);
    Reg::ShiftRights(exponent1, tmpSrcReg1, static_cast<int16_t>(52), mask);
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> scalarExponent;
    Reg::Duplicate(scalarExponent, static_cast<uint64_t>(0x7ff), mask);
    Reg::And(exponent0, exponent0, scalarExponent, mask);
    Reg::And(exponent1, exponent1, scalarExponent, mask);
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> mantissa0, mantissa1;
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> scalarMantissa;
    Reg::Duplicate(scalarMantissa, static_cast<uint64_t>(0xfffffffffffff), mask);
    Reg::And(mantissa0, tmpSrcReg0, scalarMantissa, mask);
    Reg::And(mantissa1, tmpSrcReg1, scalarMantissa, mask);
    Reg::MaskReg tmpMask0, tmpMask1;
    Reg::CompareScalar(tmpMask0, exponent0, 0x7ff, mask);
    Reg::CompareScalar(dstMask, exponent1, 0x7ff, tmpMask0);
    Reg::MaskNot(dstMask, dstMask, mask);
    Reg::CompareScalar<uint64_t, CMPMODE::EQ>(tmpMask1, mantissa0, 0, mask);
    Reg::CompareScalar<uint64_t, CMPMODE::EQ>(tmpMask0, mantissa1, 0, tmpMask1);
    Reg::MaskOr(tmpMask0, tmpMask0, dstMask, mask);
    Reg::Compare(dstMask, tmpSrcReg0, tmpSrcReg1, mask);
    Reg::MaskAnd(dstMask, dstMask, tmpMask0, mask);
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> unsignedPart0, unsignedPart1;
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> scalarUnsignedPart;
    Reg::Duplicate(scalarUnsignedPart, static_cast<uint64_t>(0x7fffffffffffffff), mask);
    Reg::And(unsignedPart0, tmpSrcReg0, scalarUnsignedPart, mask);
    Reg::And(unsignedPart1, tmpSrcReg1, scalarUnsignedPart, mask);
    Reg::CompareScalar<uint64_t, CMPMODE::EQ>(tmpMask0, unsignedPart0, 0, mask);
    Reg::CompareScalar<uint64_t, CMPMODE::EQ>(tmpMask1, unsignedPart1, 0, tmpMask0);
    Reg::MaskOr(dstMask, dstMask, tmpMask1, mask);
}

template <typename T = Reg::DefaultType, typename U>
__simd_callee__ inline void CompareEqualDouble(Reg::MaskReg& dstMask, U& srcReg0, U& srcReg1, Reg::MaskReg& mask)
{
    CompareEqualDoubleImpl(dstMask, srcReg0, srcReg1, mask);
}

template <typename T = Reg::DefaultType, typename U>
__simd_callee__ inline void IsNanFull(
    Reg::MaskReg& dstMask, U& low, U& high, Reg::RegTensor<uint32_t>& scalar0, Reg::RegTensor<uint32_t>& scalar1,
    Reg::RegTensor<uint32_t>& scalar2, Reg::MaskReg& cmpMask, Reg::MaskReg& cmpMask0, Reg::MaskReg& cmpMask1,
    Reg::MaskReg& mask)
{
    Reg::RegTensor<uint32_t> tmpReg, resReg;

    // exp_and_mantissa_high = high & 0x7fffffff
    Reg::And(tmpReg, high, scalar0, mask);
    // exponent = (exp_and_mantissa_high >> 20) & 0x7ff
    Reg::ShiftRights(resReg, tmpReg, static_cast<int16_t>(20), mask);
    Reg::And(resReg, resReg, scalar1, mask);

    // cmpMask = (exponent == 0x7ff)
    Reg::Compare(cmpMask, resReg, scalar1, mask);
    // scenario that cmpMask = true
    // mantissa_high = exp_and_mantissa_high & 0xfffff
    Reg::And(resReg, tmpReg, scalar2, cmpMask);
    // return (mantissa_high != 0) || (low != 0)
    Reg::CompareScalar<uint32_t, CMPMODE::NE>(cmpMask0, resReg, static_cast<uint32_t>(0), cmpMask);
    Reg::CompareScalar<uint32_t, CMPMODE::NE>(cmpMask1, low, static_cast<uint32_t>(0), cmpMask);
    // scenario that cmpMask = false, return false
    // cmpMask0 || cmpMask1 -> dstMask && cmpMask
    Reg::MaskOr(dstMask, cmpMask0, cmpMask1, cmpMask);
}

template <typename T = Reg::DefaultType, typename U>
__simd_callee__ inline void IsZero(
    Reg::MaskReg& dstMask, U& low, U& high, Reg::RegTensor<uint32_t>& scalar0, Reg::MaskReg& cmpMask,
    Reg::MaskReg& mask)
{
    Reg::RegTensor<uint32_t> tmpReg;

    // return (high & 0x7fffffff) == 0 && low == 0
    Reg::And(tmpReg, high, scalar0, mask);
    Reg::CompareScalar(cmpMask, tmpReg, static_cast<uint32_t>(0), mask);
    Reg::CompareScalar(dstMask, low, static_cast<uint32_t>(0), cmpMask);
}

template <typename T = Reg::DefaultType, CMPMODE cmpMode, typename U>
__simd_callee__ inline void CompareLessDouble(
    Reg::MaskReg& dstMask, U& srcReg0, U& srcReg1, Reg::RegTensor<uint32_t>& scalar0, Reg::RegTensor<uint32_t>& scalar1,
    Reg::RegTensor<uint32_t>& scalar2, Reg::MaskReg& mask)
{
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> tmpSrcReg0 = (Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo>&)srcReg0;
    Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo> tmpSrcReg1 = (Reg::RegTensor<uint64_t, Reg::RegTraitNumTwo>&)srcReg1;
    Reg::RegTensor<uint32_t> sign0, sign1, low0, low1, high0, high1;
    Reg::MaskReg cmpMask, cmpMask0, cmpMask1, cmpMask2;

    // low = bits64 & 0xffffffff
    Reg::Copy(low0, (Reg::RegTensor<uint32_t>&)tmpSrcReg0.reg[0], mask);
    Reg::Copy(low1, (Reg::RegTensor<uint32_t>&)tmpSrcReg1.reg[0], mask);
    // high = (bits64 >> 32) &  0xffffffff
    Reg::Copy(high0, (Reg::RegTensor<uint32_t>&)tmpSrcReg0.reg[1], mask);
    Reg::Copy(high1, (Reg::RegTensor<uint32_t>&)tmpSrcReg1.reg[1], mask);

    // handle nan: any comparision (except for NE) with nan is false
    IsNanFull(cmpMask0, low0, high0, scalar0, scalar1, scalar2, cmpMask, cmpMask2, dstMask, mask);
    IsNanFull(cmpMask1, low1, high1, scalar0, scalar1, scalar2, cmpMask, cmpMask2, dstMask, mask);

    // if is_nan_full(low0, high0) || is_nan_full(low1, high1), return false
    Reg::MaskOr(cmpMask, cmpMask0, cmpMask1, mask);
    // !cmpMask && mask -> dstMask
    Reg::MaskNot(dstMask, cmpMask, mask);

    // handle zeros: +0 and -0 are equal
    IsZero(cmpMask0, low0, high0, scalar0, cmpMask2, mask);
    IsZero(cmpMask1, low1, high1, scalar0, cmpMask2, mask);
    // if is_zero(low0, high0) && is_zero(low1, high1), return false
    Reg::MaskAnd(cmpMask2, cmpMask0, cmpMask1, mask);

    // handle non-zero and non-nan scenario
    // !cmpMask2 && dstMask -> mask
    Reg::MaskNot(mask, cmpMask2, dstMask);
    if constexpr (cmpMode == CMPMODE::LT) {
        // !cmpMask2 && dstMask -> dstMask
        Reg::MaskNot(dstMask, cmpMask2, dstMask);
    } else if constexpr (cmpMode == CMPMODE::LE) {
        Reg::Compare<uint32_t, CMPMODE::NE>(cmpMask0, low0, low1, mask);
        Reg::Compare<uint32_t, CMPMODE::NE>(cmpMask1, high0, high1, mask);
        // handle non-zero and non-nan and non-equal scenario
        // (cmpMask0 || cmpMask1) && mask -> mask
        Reg::MaskOr(mask, cmpMask0, cmpMask1, mask);
    }

    // extract sign bits
    Reg::ShiftRights(sign0, high0, static_cast<int16_t>(31), mask);
    Reg::ShiftRights(sign1, high1, static_cast<int16_t>(31), mask);

    // negative (sign=1) < positive (sign=0)
    // if sign0 != sign1, return sign0 > sign1
    Reg::Compare<uint32_t, CMPMODE::NE>(cmpMask, sign0, sign1, mask);
    Reg::Compare<uint32_t, CMPMODE::GT>(cmpMask0, sign0, sign1, cmpMask);
    Reg::MaskSel(dstMask, cmpMask0, dstMask, cmpMask);

    // if sign0 == sign1
    Reg::MaskNot(mask, cmpMask, mask);
    /*
        if sign0 == 0:
            if high0 != high1:
                return high0 < high1
            return low0 < low1
        else:
            if high0 != high1:
                return high0 > high1
            return low0 > low1
    */
    Reg::CompareScalar(cmpMask, sign0, static_cast<uint32_t>(0), mask);
    Reg::Compare<uint32_t, CMPMODE::NE>(cmpMask0, high0, high1, cmpMask);
    Reg::Compare<uint32_t, CMPMODE::LT>(cmpMask1, high0, high1, cmpMask0);
    Reg::MaskSel(dstMask, cmpMask1, dstMask, cmpMask0);
    Reg::Compare<uint32_t, CMPMODE::EQ>(cmpMask0, high0, high1, cmpMask);
    Reg::Compare<uint32_t, CMPMODE::LT>(cmpMask1, low0, low1, cmpMask0);
    Reg::MaskSel(dstMask, cmpMask1, dstMask, cmpMask0);

    Reg::MaskNot(cmpMask, cmpMask, mask);
    Reg::Compare<uint32_t, CMPMODE::NE>(cmpMask0, high0, high1, cmpMask);
    Reg::Compare<uint32_t, CMPMODE::GT>(cmpMask1, high0, high1, cmpMask0);
    Reg::MaskSel(dstMask, cmpMask1, dstMask, cmpMask0);
    Reg::Compare<uint32_t, CMPMODE::EQ>(cmpMask0, high0, high1, cmpMask);
    Reg::Compare<uint32_t, CMPMODE::GT>(cmpMask1, low0, low1, cmpMask0);
    Reg::MaskSel(dstMask, cmpMask1, dstMask, cmpMask0);
}

template <typename T = Reg::DefaultType, CMPMODE cmpMode, typename U>
__simd_callee__ inline void CompareDouble(
    Reg::MaskReg& dstMask, U& src0Reg, U& src1Reg, Reg::RegTensor<uint32_t>& scalar0, Reg::RegTensor<uint32_t>& scalar1,
    Reg::RegTensor<uint32_t>& scalar2, Reg::MaskReg& mask)
{
    if constexpr (cmpMode == CMPMODE::LT) {
        CompareLessDouble<T, cmpMode>(dstMask, src0Reg, src1Reg, scalar0, scalar1, scalar2, mask);
    } else if constexpr (cmpMode == CMPMODE::GT) {
        CompareLessDouble<T, CMPMODE::LT>(dstMask, src1Reg, src0Reg, scalar0, scalar1, scalar2, mask);
    } else if constexpr (cmpMode == CMPMODE::EQ) {
        CompareEqualDouble(dstMask, src0Reg, src1Reg, mask);
    } else if constexpr (cmpMode == CMPMODE::LE) {
        CompareLessDouble<T, cmpMode>(dstMask, src0Reg, src1Reg, scalar0, scalar1, scalar2, mask);
    } else if constexpr (cmpMode == CMPMODE::GE) {
        CompareLessDouble<T, CMPMODE::LE>(dstMask, src1Reg, src0Reg, scalar0, scalar1, scalar2, mask);
    } else {
        CompareEqualDouble(dstMask, src0Reg, src1Reg, mask);
        Reg::MaskNot(dstMask, dstMask, mask);
    }
}

// Compare::Level 2 - counter mode
template <typename T, typename U, CMPMODE cmpMode>
__simd_vf__ inline void CompareLevel2(__ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint32_t calCount)
{
    uint32_t repeatElm = GetVecLen() / sizeof(T);
    uint16_t repeatTime = CeilDivision(calCount, repeatElm);
    uint32_t sreg = calCount;
    Reg::MaskReg dstReg, mask;
    Reg::UnalignReg uReg;
    if constexpr (sizeof(T) == 8) {
        repeatElm = repeatElm * 2;
        repeatTime = CeilDivision(calCount, repeatElm);
        __ubuf__ uint32_t* dstT = reinterpret_cast<__ubuf__ uint32_t*>(dst);
        Reg::RegTensor<T, Reg::RegTraitNumTwo> src0Reg, src1Reg;
        Reg::RegTensor<uint32_t> scalar0, scalar1, scalar2;
        Reg::Duplicate(scalar0, static_cast<uint32_t>(0x7fffffff));
        Reg::Duplicate(scalar1, static_cast<uint32_t>(0x7ff));
        Reg::Duplicate(scalar2, static_cast<uint32_t>(0xfffff));
        for (uint16_t i = 0; i < repeatTime; ++i) {
            mask = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
            Reg::LoadAlign(src0Reg, src0 + i * repeatElm);
            Reg::LoadAlign(src1Reg, src1 + i * repeatElm);
            if constexpr (Std::is_same_v<T, double>) {
                CompareDouble<double, cmpMode>(dstReg, src0Reg, src1Reg, scalar0, scalar1, scalar2, mask);
            } else {
                Reg::Compare<T, cmpMode>(dstReg, src0Reg, src1Reg, mask);
            }
            Reg::StoreUnAlign(dstT, dstReg, uReg);
        }
        Reg::StoreUnAlignPost<uint32_t, Reg::PostLiteral::POST_MODE_NORMAL>(dstT, uReg, 0);
    } else {
        Reg::RegTensor<T> src0Reg, src1Reg;
        uint32_t offset = GetVecLen() / sizeof(T) / 8;
        __ubuf__ T* dstT = reinterpret_cast<__ubuf__ T*>(dst);
        for (uint16_t i = 0; i < repeatTime; ++i) {
            mask = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign(src0Reg, src0 + i * repeatElm);
            Reg::LoadAlign(src1Reg, src1 + i * repeatElm);
            Reg::Compare<T, cmpMode>(dstReg, src0Reg, src1Reg, mask);
            if constexpr (sizeof(T) == 1) {
                Reg::StoreAlign(dst + i * offset, dstReg);
            } else {
                Reg::StoreUnAlign(dstT, dstReg, uReg);
            }
        }
        if constexpr (sizeof(T) > 1) {
            Reg::StoreUnAlignPost<T, Reg::PostLiteral::POST_MODE_NORMAL>(dstT, uReg, 0);
        }
    }
}

template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void VcmpvImpl(
    __ubuf__ U* dst, __ubuf__ T* src0, __ubuf__ T* src1, CMPMODE cmpMode, const uint32_t calCount)
{
    static_assert(
        SupportType<
            T, half, int16_t, uint16_t, int32_t, uint32_t, float, uint8_t, int8_t, bfloat16_t, uint64_t, int64_t,
            double>(),
        "current data type is not supported!");
    static_assert(SupportType<U, uint8_t, int8_t>(), "current data type is not supported!");
    switch (cmpMode) {
        case CMPMODE::LT: {
            CompareLevel2<T, U, CMPMODE::LT>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::GT: {
            CompareLevel2<T, U, CMPMODE::GT>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::EQ: {
            CompareLevel2<T, U, CMPMODE::EQ>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::LE: {
            CompareLevel2<T, U, CMPMODE::LE>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::GE: {
            CompareLevel2<T, U, CMPMODE::GE>(dst, src0, src1, calCount);
            break;
        }
        case CMPMODE::NE: {
            CompareLevel2<T, U, CMPMODE::NE>(dst, src0, src1, calCount);
            break;
        }
        default:
            break;
    }
}

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_COMPARE_CONTINUOUS_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_COMPARE_CONTINUOUS_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_COMPARE_CONTINUOUS_IMPL_H__
#endif
