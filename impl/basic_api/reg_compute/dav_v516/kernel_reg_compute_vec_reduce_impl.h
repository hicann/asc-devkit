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
 * \file kernel_reg_compute_vec_reduce_intf.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_v516/kernel_reg_compute_vec_reduce_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_vec_reduce_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_REDUCE_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_VEC_REDUCE_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_VEC_REDUCE_IMPL_H

#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_copy_intf.h"
#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_vec_binary_intf.h"
#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_vec_binary_scalar_intf.h"
#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_vec_cmpsel_intf.h"
#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_vec_duplicate_intf.h"

namespace AscendC {
namespace Reg {
template <
    typename T = DefaultType, typename U = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S,
    typename V>
__simd_callee__ inline void ReduceSumImpl(S& dstReg, V srcReg, MaskReg mask)
{
    using ActualDstRegT = typename S::ActualT;
    using ActualSrcRegT = typename V::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualDstRegT>, "T type is not correct!");
    static_assert(Std::is_same_v<U, DefaultType> || Std::is_same_v<U, ActualSrcRegT>, "U type is not correct!");
    static_assert(
        (SupportType<
            Tuple<ActualDstRegT, ActualSrcRegT>, Tuple<int32_t, int16_t>, Tuple<uint32_t, uint16_t>,
            Tuple<uint32_t, uint32_t>, Tuple<int32_t, int32_t>, Tuple<half, half>, Tuple<float, float>,
            Tuple<uint64_t, uint64_t>, Tuple<int64_t, int64_t>>()),
        "ReduceSum unsupport this datatype on current device");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current ReduceSum api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (sizeof(ActualSrcRegT) != 8) {
        vcadd(dstReg, srcReg, mask, modeValue);
    } else if constexpr (sizeof(ActualSrcRegT) == 8) {
        if constexpr (CheckRegTrait<V, RegTraitNumTwo>()) {
            S dstTemp;
            ReduceSumB64Impl(dstTemp, srcReg, mask);
            dstReg = dstTemp;
        } else if constexpr (CheckRegTrait<V, RegTraitNumOne>()) {
            MaskReg maskTrait2;
            MaskPack(maskTrait2, mask);
            RegTensor<ActualSrcRegT, RegTraitNumTwo> traitTwoSrcReg0;
            RegTensor<ActualDstRegT, RegTraitNumTwo> traitTwoDstReg;
            B64TraitOneToTraitTwo(traitTwoSrcReg0, srcReg);
            ReduceSumB64Impl(traitTwoDstReg, traitTwoSrcReg0, maskTrait2);
            B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
        }
    }
}

template <typename T>
__simd_callee__ inline void ReduceSumB64Impl(T& dstReg, T srcReg, MaskReg mask)
{
    using ActualT = typename T::ActualT;
    static_assert(SupportType<ActualT, uint64_t, int64_t>(), "ReduceSumB64Impl only support uint64_t int64_type");
    static_assert(CheckRegTrait<T, RegTraitNumTwo>(), "ReduceSumB64Impl only support RegTraitNumTwo");
    constexpr auto modeValue = GetMaskMergeMode<MaskMergeMode::ZEROING>();
    RegTensor<uint32_t> lowReg;
    RegTensor<uint32_t> midReg;
    RegTensor<uint32_t> highReg;
    RegTensor<uint32_t> tmpReg;
    Duplicate(lowReg, 0xffff);
    // srcreg[1] for high 32 bit, tmpReg1 for mid 16 bit, tmpReg0 for low 16 bit
    vand(lowReg, lowReg, (RegTensor<uint32_t>&)srcReg.reg[0], mask, modeValue);
    vcadd(lowReg, lowReg, mask, modeValue);
    Reg::ShiftRights(midReg, (RegTensor<uint32_t>&)srcReg.reg[0], (int16_t)16, mask);
    vcadd(midReg, midReg, mask, modeValue);
    vcadd((RegTensor<uint32_t>&)dstReg.reg[1], (RegTensor<uint32_t>&)srcReg.reg[1], mask, modeValue);
    // add low carry to mid
    Reg::ShiftRights(tmpReg, lowReg, (int16_t)16, mask);
    vadd(midReg, midReg, tmpReg, mask, modeValue);
    // add mid carry to high
    Reg::ShiftRights(tmpReg, midReg, (int16_t)16, mask);
    vadd((RegTensor<uint32_t>&)dstReg.reg[1], (RegTensor<uint32_t>&)dstReg.reg[1], tmpReg, mask, modeValue);
    Interleave(
        (RegTensor<uint16_t>&)dstReg.reg[0], (RegTensor<uint16_t>&)tmpReg, (RegTensor<uint16_t>&)lowReg,
        (RegTensor<uint16_t>&)midReg);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void ReduceMaxImpl(U& dstReg, U srcReg, MaskReg mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        (SupportType<ActualT, uint16_t, int16_t, uint32_t, int32_t, float, half, uint64_t, int64_t>()),
        "ReduceMax unsupport this datatype on current device");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current ReduceMax api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (sizeof(ActualT) != 8) {
        vcmax(dstReg, srcReg, mask, modeValue);
    } else {
        if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            U dstTemp;
            ReduceMaxB64Impl(dstTemp, srcReg, mask);
            dstReg = dstTemp;
        } else if constexpr (CheckRegTrait<U, RegTraitNumOne>()) {
            MaskReg maskTrait2;
            MaskPack(maskTrait2, mask);
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg0;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
            DeInterleave(
                (RegTensor<uint32_t>&)traitTwoSrcReg0.reg[0], (RegTensor<uint32_t>&)traitTwoSrcReg0.reg[1],
                (RegTensor<uint32_t>&)srcReg, (RegTensor<uint32_t>&)srcReg);
            ReduceMaxB64Impl(traitTwoDstReg, traitTwoSrcReg0, maskTrait2);
            B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
        }
    }
}

template <MaskMergeMode mode = MaskMergeMode::ZEROING, typename T>
__simd_callee__ inline void ReduceMaxB64Impl(T& dstReg, T srcReg, MaskReg mask)
{
    using ActualT = typename T::ActualT;
    static_assert(SupportType<ActualT, uint64_t, int64_t>(), "ReduceMaxB64Impl only support uint64_t int64_type");
    static_assert(CheckRegTrait<T, RegTraitNumTwo>(), "ReduceMaxB64Impl only support RegTraitNumTwo");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (SupportType<ActualT, uint64_t>()) {
        RegTensor<uint32_t> tmpReg0;
        RegTensor<uint32_t> tmpReg1;
        Reduce<ReduceType::MAX, DefaultType, DefaultType, mode>(tmpReg0, (RegTensor<uint32_t>&)srcReg.reg[1], mask);
        MaskReg mask0 = CreateMask<uint32_t, MaskPattern::ALL>();
        Duplicate(tmpReg1, tmpReg0, mask0);
        MaskReg mask1;
        Compare(mask1, tmpReg1, (RegTensor<uint32_t>&)srcReg.reg[1], mask);
        RegTensor<uint32_t> tmpReg2;
        Reduce<ReduceType::MAX, DefaultType, DefaultType, mode>(tmpReg2, (RegTensor<uint32_t>&)srcReg.reg[0], mask1);
        mask0 = CreateMask<uint32_t, MaskPattern::VL1>();
        And(tmpReg1, tmpReg0, tmpReg0, mask0);
        Copy((RegTensor<uint32_t>&)dstReg.reg[1], tmpReg1);
        Copy((RegTensor<uint32_t>&)dstReg.reg[0], (RegTensor<uint32_t>&)tmpReg2);
    } else if constexpr (SupportType<ActualT, int64_t>()) {
        RegTensor<int32_t> tmpReg0;
        RegTensor<int32_t> tmpReg1;
        Reduce<ReduceType::MAX, DefaultType, DefaultType, mode>(tmpReg0, (RegTensor<int32_t>&)srcReg.reg[1], mask);
        MaskReg mask0 = CreateMask<int32_t, MaskPattern::ALL>();
        Duplicate(tmpReg1, tmpReg0, mask0);
        MaskReg mask1;
        Compare(mask1, tmpReg1, (RegTensor<int32_t>&)srcReg.reg[1], mask);
        RegTensor<uint32_t> tmpReg2;
        Reduce<ReduceType::MAX, DefaultType, DefaultType, mode>(tmpReg2, (RegTensor<uint32_t>&)srcReg.reg[0], mask1);
        mask0 = CreateMask<uint32_t, MaskPattern::VL1>();
        And(tmpReg1, tmpReg0, tmpReg0, mask0);
        Copy((RegTensor<int32_t>&)dstReg.reg[1], tmpReg1);
        Copy((RegTensor<int32_t>&)dstReg.reg[0], (RegTensor<int32_t>&)tmpReg2);
    }
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void ReduceMinImpl(U& dstReg, U srcReg, MaskReg mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        (SupportType<ActualT, uint16_t, int16_t, uint32_t, int32_t, float, half, uint64_t, int64_t>()),
        "ReduceMin unsupport this datatype on current device");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current ReduceMin api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (sizeof(ActualT) != 8) {
        vcmin(dstReg, srcReg, mask, modeValue);
    } else if constexpr (sizeof(ActualT) == 8) {
        if constexpr (CheckRegTrait<U, RegTraitNumTwo>()) {
            U dstTemp;
            ReduceMinB64Impl(dstTemp, srcReg, mask);
            dstReg = dstTemp;
        } else if constexpr (CheckRegTrait<U, RegTraitNumOne>()) {
            MaskReg maskTrait2;
            MaskPack(maskTrait2, mask);
            RegTensor<ActualT, RegTraitNumTwo> traitTwoSrcReg0;
            RegTensor<ActualT, RegTraitNumTwo> traitTwoDstReg;
            DeInterleave(
                (RegTensor<uint32_t>&)traitTwoSrcReg0.reg[0], (RegTensor<uint32_t>&)traitTwoSrcReg0.reg[1],
                (RegTensor<uint32_t>&)srcReg, (RegTensor<uint32_t>&)srcReg);
            ReduceMinB64Impl(traitTwoDstReg, traitTwoSrcReg0, maskTrait2);
            B64TraitTwoToTraitOne(dstReg, traitTwoDstReg);
        }
    }
}

template <MaskMergeMode mode = MaskMergeMode::ZEROING, typename T>
__simd_callee__ inline void ReduceMinB64Impl(T& dstReg, T srcReg, MaskReg mask)
{
    using ActualT = typename T::ActualT;
    static_assert(SupportType<ActualT, uint64_t, int64_t>(), "ReduceMinB64Impl only support uint64_t int64_type");
    static_assert(CheckRegTrait<T, RegTraitNumTwo>(), "ReduceMinB64Impl only support RegTraitNumTwo");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    if constexpr (SupportType<ActualT, uint64_t>()) {
        RegTensor<uint32_t> tmpReg0;
        RegTensor<uint32_t> tmpReg1;
        Reduce<ReduceType::MIN, DefaultType, DefaultType, mode>(tmpReg0, (RegTensor<uint32_t>&)srcReg.reg[1], mask);
        MaskReg mask0 = CreateMask<uint32_t, MaskPattern::ALL>();
        Duplicate(tmpReg1, tmpReg0, mask0);
        MaskReg mask1;
        Compare(mask1, tmpReg1, (RegTensor<uint32_t>&)srcReg.reg[1], mask);
        RegTensor<uint32_t> tmpReg2;
        Reduce<ReduceType::MIN, DefaultType, DefaultType, mode>(tmpReg2, (RegTensor<uint32_t>&)srcReg.reg[0], mask1);
        mask0 = CreateMask<uint32_t, MaskPattern::VL1>();
        And(tmpReg1, tmpReg0, tmpReg0, mask0);
        Copy((RegTensor<uint32_t>&)dstReg.reg[1], tmpReg1);
        Copy((RegTensor<uint32_t>&)dstReg.reg[0], (RegTensor<uint32_t>&)tmpReg2);
    } else if constexpr (SupportType<ActualT, int64_t>()) {
        RegTensor<int32_t> tmpReg0;
        RegTensor<int32_t> tmpReg1;
        Reduce<ReduceType::MIN, DefaultType, DefaultType, mode>(tmpReg0, (RegTensor<int32_t>&)srcReg.reg[1], mask);
        MaskReg mask0 = CreateMask<int32_t, MaskPattern::ALL>();
        Duplicate(tmpReg1, tmpReg0, mask0);
        MaskReg mask1;
        Compare(mask1, tmpReg1, (RegTensor<int32_t>&)srcReg.reg[1], mask);
        RegTensor<uint32_t> tmpReg2;
        Reduce<ReduceType::MIN, DefaultType, DefaultType, mode>(tmpReg2, (RegTensor<uint32_t>&)srcReg.reg[0], mask1);
        mask0 = CreateMask<uint32_t, MaskPattern::VL1>();
        And(tmpReg1, tmpReg0, tmpReg0, mask0);
        Copy((RegTensor<int32_t>&)dstReg.reg[1], tmpReg1);
        Copy((RegTensor<int32_t>&)dstReg.reg[0], (RegTensor<int32_t>&)tmpReg2);
    }
}

template <
    typename T = DefaultType, typename U = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename S,
    typename V>
__simd_callee__ inline void ReduceSumWithDataBlockImpl(S& dstReg, V srcReg, MaskReg mask)
{
    using ActualDstRegT = typename S::ActualT;
    using ActualSrcRegT = typename V::ActualT;
    static_assert(std::is_same_v<T, DefaultType> || std::is_same_v<T, ActualDstRegT>, "T type is not correct!");
    static_assert(std::is_same_v<U, DefaultType> || std::is_same_v<U, ActualSrcRegT>, "U type is not correct!");
    static_assert(
        (SupportType<
            Tuple<ActualDstRegT, ActualSrcRegT>, Tuple<int32_t, int16_t>, Tuple<uint32_t, uint16_t>,
            Tuple<uint32_t, uint32_t>, Tuple<int32_t, int32_t>, Tuple<half, half>, Tuple<float, float>>()),
        "ReduceSumWithDataBlock unsupport this datatype on current device");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current ReduceSumWithDataBlock api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vcgadd(dstReg, srcReg, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void ReduceSumWithDataBlockImpl(U& dstReg, U srcReg, MaskReg mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        (SupportType<ActualT, uint16_t, int16_t, uint32_t, int32_t, float, half>()),
        "ReduceSumWithDataBlock unsupport this datatype on current device");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current ReduceSumWithDataBlock api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vcgadd(dstReg, srcReg, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void ReduceMaxWithDataBlockImpl(U& dstReg, U srcReg, MaskReg mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        (SupportType<ActualT, uint16_t, int16_t, uint32_t, int32_t, float, half>()),
        "ReduceMaxWithDataBlock unsupport this datatype on current device");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current ReduceMaxWithDataBlock api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vcgmax(dstReg, srcReg, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void ReduceMinWithDataBlockImpl(U& dstReg, U srcReg, MaskReg mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(
        (SupportType<ActualT, uint16_t, int16_t, uint32_t, int32_t, float, half>()),
        "ReduceMinWithDataBlock unsupport this datatype on current device");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current ReduceMinWithDataBlock api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vcgmin(dstReg, srcReg, mask, modeValue);
}

template <typename T = DefaultType, MaskMergeMode mode = MaskMergeMode::ZEROING, typename U>
__simd_callee__ inline void PairReduceSumImpl(U& dstReg, U srcReg, MaskReg mask)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert((SupportType<ActualT, float, half>()), "PairReduceSum unsupport this datatype on current device");
    static_assert(
        SupportEnum<mode, MaskMergeMode::ZEROING>(),
        "current PairReduceSum api only supported Mode ZEROING on current device!");
    constexpr auto modeValue = GetMaskMergeMode<mode>();
    vcpadd(dstReg, srcReg, mask, modeValue);
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_VEC_REDUCE_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_REDUCE_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_VEC_REDUCE_IMPL__
#endif
