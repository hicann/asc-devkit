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
 * \file kernel_reg_compute_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_9201/kernel_reg_compute_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_COMMON_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_COMMON_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_COMMON_IMPL_H

#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_utils.h"
#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_struct_intf.h"
#include "../../../../include/basic_api/reg_compute/kernel_reg_compute_maskreg_intf.h"

namespace AscendC {
namespace Reg {
template <typename T, StoreDist dist>
__aicore__ inline constexpr StoreDist GetStoreDist()
{
    if constexpr (dist == StoreDist::DIST_NORM) {
        static_assert(
            SupportBytes<T, 1, 2, 4, 8>(), "StoreDist DIST_NORM only support type b8/b16/b32/b64 on current device");
        if constexpr (sizeof(T) == 1) {
            return StoreDist::DIST_NORM_B8;
        } else if constexpr (sizeof(T) == 2) {
            return StoreDist::DIST_NORM_B16;
        } else if constexpr (sizeof(T) == 4) {
            return StoreDist::DIST_NORM_B32;
        } else if constexpr (sizeof(T) == 8) {
            return StoreDist::DIST_NORM_B32;
        }
    }
    return dist;
}

template <typename T, const RegTrait& otherTrait = RegTraitNumOne>
constexpr __aicore__ inline bool CheckRegTrait()
{
    constexpr RegTrait regTrait = T::trait;
    return regTrait.REG_NUM == otherTrait.REG_NUM;
}

#ifndef __ASC_NPU_HOST__
template <RoundMode mode>
__aicore__ inline constexpr ::ROUND GetRound()
{
// To avoid naming conflicts of ROUND member variables in cpu debug,
// the names of the returned member variables are changed to be different from those of
// the ROUND enumeration class of the compiler.
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    if constexpr (mode == RoundMode::CAST_RINT) {
        return ::ROUND::CAST_RINT;
    } else if constexpr (mode == RoundMode::CAST_ROUND) {
        return ::ROUND::CAST_ROUND;
    } else if constexpr (mode == RoundMode::CAST_FLOOR) {
        return ::ROUND::CAST_FLOOR;
    } else if constexpr (mode == RoundMode::CAST_CEIL) {
        return ::ROUND::CAST_CEIL;
    } else if constexpr (mode == RoundMode::CAST_TRUNC) {
        return ::ROUND::CAST_TRUNC;
    } else if constexpr (mode == RoundMode::CAST_ODD) {
        return ::ROUND::CAST_ODD;
    } else {
        return ::ROUND::CAST_HYBRID;
    }
#else
    if constexpr (mode == RoundMode::CAST_RINT) {
        return ::ROUND::R;
    } else if constexpr (mode == RoundMode::CAST_ROUND) {
        return ::ROUND::A;
    } else if constexpr (mode == RoundMode::CAST_FLOOR) {
        return ::ROUND::F;
    } else if constexpr (mode == RoundMode::CAST_CEIL) {
        return ::ROUND::C;
    } else if constexpr (mode == RoundMode::CAST_TRUNC) {
        return ::ROUND::Z;
    } else if constexpr (mode == RoundMode::CAST_ODD) {
        return ::ROUND::O;
    } else {
        return ::ROUND::H;
    }
#endif
}
#endif

#ifndef __ASC_NPU_HOST__
template <MaskMergeMode mode>
__aicore__ inline constexpr auto GetMaskMergeMode()
{
// To avoid naming conflicts of mode struct in cpu debug.
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    return std::integral_constant<::CpuMode, static_cast<::CpuMode>(mode)>();
#else
    return std::integral_constant<::Mode, static_cast<::Mode>(mode)>();
#endif
}
#endif // #ifndef __ASC_NPU_HOST__

template <typename T, typename U, typename ShortType>
__simd_callee__ inline void TraitOneToTraitTwoTmpl(T& dstReg, U& srcReg)
{
    using ActualT1 = typename U::ActualT;
    using ActualT2 = typename T::ActualT;
    static_assert(
        CheckRegTrait<T, RegTraitNumTwo>() && CheckRegTrait<U, RegTraitNumOne>(),
        "T should be RegTraitNumTwo and U should be RegTraitNumOne");
    static_assert(
        sizeof(ActualT2) == (sizeof(ShortType) * 2) && sizeof(ActualT1) == (sizeof(ShortType) * 2),
        "T and U should be 2 times of shortType length");
    RegTensor<ShortType> zeroReg;
    MaskReg maskFull = CreateMask<ShortType, MaskPattern::ALL>();
    Duplicate(zeroReg, 0, maskFull);
    DeInterleave(
        (RegTensor<ShortType>&)dstReg.reg[0], (RegTensor<ShortType>&)dstReg.reg[1], (RegTensor<ShortType>&)srcReg,
        zeroReg);
}

template <typename T, typename U, typename ShortType>
__simd_callee__ inline void TraitTwoToTraitOneTmpl(T& dstReg, U& srcReg)
{
    using ActualT1 = typename T::ActualT;
    using ActualT2 = typename U::ActualT;
    static_assert(
        CheckRegTrait<T, RegTraitNumOne>() && CheckRegTrait<U, RegTraitNumTwo>(),
        "T should be RegTraitNumOne and U should be RegTraitNumTwo");
    static_assert(
        sizeof(ActualT2) == (sizeof(ShortType) * 2) && sizeof(ActualT1) == (sizeof(ShortType) * 2),
        "U and T should be 2 times of shortType length");
    RegTensor<ShortType> dstRegShortFake;
    Interleave(
        (RegTensor<ShortType>&)dstReg, dstRegShortFake, (RegTensor<ShortType>&)srcReg.reg[0],
        (RegTensor<ShortType>&)srcReg.reg[1]);
}

template <RegLayout layoutMode>
__simd_callee__ inline void AdjustMask_b32_b8(MaskReg& dstMask, MaskReg& srcMask)
{
    static_assert(
        SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE>(),
        "AdjustMask_b32_b8 only supports RegLayout ZERO, ONE, TWO and THREE");
    MaskReg zeroMask = CreateMask<uint8_t, MaskPattern::ALLF>();
    MaskReg tmpMask;
    ppack(dstMask, srcMask, std::integral_constant<::HiloPart, static_cast<::HiloPart>(HighLowPart::LOWEST)>());
    ppack(dstMask, dstMask, std::integral_constant<::HiloPart, static_cast<::HiloPart>(HighLowPart::LOWEST)>());
    if constexpr (layoutMode == RegLayout::ZERO) {
        pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
        pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
    } else if constexpr (layoutMode == RegLayout::ONE) {
        pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
        pintlv_b8(dstMask, tmpMask, zeroMask, dstMask);
    } else if constexpr (layoutMode == RegLayout::TWO) {
        pintlv_b8(dstMask, tmpMask, zeroMask, dstMask);
        pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
    } else {
        pintlv_b8(dstMask, tmpMask, zeroMask, dstMask);
        pintlv_b8(dstMask, tmpMask, zeroMask, dstMask);
    }
}

template <RegLayout layoutMode>
__simd_callee__ inline void AdjustMask_b32_b16(MaskReg& dstMask, MaskReg& srcMask)
{
    static_assert(
        SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE>(),
        "AdjustMask_b32_b16 only supports RegLayout ZERO and ONE");
    MaskReg zeroMask = CreateMask<uint8_t, MaskPattern::ALLF>();
    MaskReg tmpMask;
    ppack(dstMask, srcMask, std::integral_constant<::HiloPart, static_cast<::HiloPart>(HighLowPart::LOWEST)>());
    ppack(dstMask, dstMask, std::integral_constant<::HiloPart, static_cast<::HiloPart>(HighLowPart::LOWEST)>());
    if constexpr (layoutMode == RegLayout::ZERO) {
        pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
        pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
    } else {
        pintlv_b8(dstMask, tmpMask, zeroMask, dstMask);
        pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
    }
}

template <RegLayout layoutMode>
__simd_callee__ inline void AdjustMask_b16_b8(MaskReg& dstMask, MaskReg& srcMask)
{
    static_assert(
        SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE>(),
        "AdjustMask_b16_b8 only supports RegLayout ZERO and ONE");
    MaskReg zeroMask = CreateMask<uint8_t, MaskPattern::ALLF>();
    MaskReg tmpMask;
    ppack(dstMask, srcMask, std::integral_constant<::HiloPart, static_cast<::HiloPart>(HighLowPart::LOWEST)>());
    if constexpr (layoutMode == RegLayout::ZERO) {
        pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
    } else {
        pintlv_b8(dstMask, tmpMask, zeroMask, dstMask);
    }
}

template <RegLayout layoutMode>
__simd_callee__ inline void AdjustMask_b16_b4(MaskReg& dstMask, MaskReg& srcMask)
{
    static_assert(
        SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE>(),
        "AdjustMask_b16_b4 only supports RegLayout ZERO, ONE, TWO and THREE");
    MaskReg zeroMask = CreateMask<uint8_t, MaskPattern::ALLF>();
    MaskReg tmpMask;
    ppack(dstMask, srcMask, std::integral_constant<::HiloPart, static_cast<::HiloPart>(HighLowPart::LOWEST)>());
    ppack(dstMask, dstMask, std::integral_constant<::HiloPart, static_cast<::HiloPart>(HighLowPart::LOWEST)>());
    if constexpr (layoutMode == RegLayout::ZERO) {
        pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
        pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
    } else if constexpr (layoutMode == RegLayout::ONE) {
        pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
        pintlv_b8(dstMask, tmpMask, zeroMask, dstMask);
    } else if constexpr (layoutMode == RegLayout::TWO) {
        pintlv_b8(dstMask, tmpMask, zeroMask, dstMask);
        pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
    } else {
        pintlv_b8(dstMask, tmpMask, zeroMask, dstMask);
        pintlv_b8(dstMask, tmpMask, zeroMask, dstMask);
    }
}

template <RegLayout layoutMode>
__simd_callee__ inline void AdjustMask_b4_b16(MaskReg& dstMask, MaskReg& srcMask)
{
    static_assert(
        SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE>(),
        "AdjustMask_b4_b16 only supports RegLayout ZERO, ONE, TWO and THREE");
    MaskReg zeroMask = CreateMask<uint8_t, MaskPattern::ALLF>();
    MaskReg tmpMask;
    if constexpr (layoutMode == RegLayout::ZERO) {
        pdintlv_b8(dstMask, tmpMask, srcMask, srcMask);
        pdintlv_b8(dstMask, tmpMask, dstMask, dstMask);
    } else if constexpr (layoutMode == RegLayout::ONE) {
        pdintlv_b8(tmpMask, dstMask, srcMask, srcMask);
        pdintlv_b8(dstMask, tmpMask, dstMask, dstMask);
    } else if constexpr (layoutMode == RegLayout::TWO) {
        pdintlv_b8(dstMask, tmpMask, srcMask, srcMask);
        pdintlv_b8(tmpMask, dstMask, dstMask, dstMask);
    } else {
        pdintlv_b8(tmpMask, dstMask, srcMask, srcMask);
        pdintlv_b8(tmpMask, dstMask, dstMask, dstMask);
    }
    pintlv_b8(dstMask, tmpMask, dstMask, dstMask);
    pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
}

template <RegLayout layoutMode>
__simd_callee__ inline void AdjustMask_b8_b32(MaskReg& dstMask, MaskReg& srcMask)
{
    static_assert(
        SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE, RegLayout::TWO, RegLayout::THREE>(),
        "AdjustMask_b8_b32 only supports RegLayout ZERO, ONE, TWO and THREE");
    MaskReg zeroMask = CreateMask<uint8_t, MaskPattern::ALLF>();
    MaskReg tmpMask;
    if constexpr (layoutMode == RegLayout::ZERO) {
        pdintlv_b8(dstMask, tmpMask, srcMask, srcMask);
        pdintlv_b8(dstMask, tmpMask, dstMask, dstMask);
    } else if constexpr (layoutMode == RegLayout::ONE) {
        pdintlv_b8(tmpMask, dstMask, srcMask, srcMask);
        pdintlv_b8(dstMask, tmpMask, dstMask, dstMask);
    } else if constexpr (layoutMode == RegLayout::TWO) {
        pdintlv_b8(dstMask, tmpMask, srcMask, srcMask);
        pdintlv_b8(tmpMask, dstMask, dstMask, dstMask);
    } else {
        pdintlv_b8(tmpMask, dstMask, srcMask, srcMask);
        pdintlv_b8(tmpMask, dstMask, dstMask, dstMask);
    }
    pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
    pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
}

template <RegLayout layoutMode>
__simd_callee__ inline void AdjustMask_b8_b16(MaskReg& dstMask, MaskReg& srcMask)
{
    static_assert(
        SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE>(),
        "AdjustMask_b8_b16 only supports RegLayout ZERO and ONE");
    MaskReg zeroMask = CreateMask<uint8_t, MaskPattern::ALLF>();
    MaskReg tmpMask;
    if constexpr (layoutMode == RegLayout::ZERO) {
        pdintlv_b8(dstMask, tmpMask, srcMask, srcMask);
    } else {
        pdintlv_b8(tmpMask, dstMask, srcMask, srcMask);
    }
    pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
}

template <RegLayout layoutMode>
__simd_callee__ inline void AdjustMask_b16_b32(MaskReg& dstMask, MaskReg& srcMask)
{
    static_assert(
        SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE>(),
        "AdjustMask_b16_b32 only supports RegLayout ZERO and ONE");
    MaskReg zeroMask = CreateMask<uint8_t, MaskPattern::ALLF>();
    MaskReg tmpMask;
    ppack(dstMask, srcMask, std::integral_constant<::HiloPart, static_cast<::HiloPart>(HighLowPart::LOWEST)>());
    if constexpr (layoutMode == RegLayout::ZERO) {
        pdintlv_b8(dstMask, tmpMask, dstMask, dstMask);
    } else {
        pdintlv_b8(tmpMask, dstMask, dstMask, dstMask);
    }
    pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
    pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
}

template <RegLayout layoutMode>
__simd_callee__ inline void AdjustMask_b64_b32(MaskReg& dstMask, MaskReg& srcMask)
{
    static_assert(
        SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE>(),
        "AdjustMask_b64_b32 only supports RegLayout ZERO and ONE");
    MaskReg zeroMask = CreateMask<uint8_t, MaskPattern::ALLF>();
    MaskReg tmpMask;
    ppack(dstMask, srcMask, std::integral_constant<::HiloPart, static_cast<::HiloPart>(HighLowPart::LOWEST)>());
    ppack(dstMask, dstMask, std::integral_constant<::HiloPart, static_cast<::HiloPart>(HighLowPart::LOWEST)>());
    ppack(dstMask, dstMask, std::integral_constant<::HiloPart, static_cast<::HiloPart>(HighLowPart::LOWEST)>());
    if constexpr (layoutMode == RegLayout::ZERO) {
        pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
        pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
        pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
    } else {
        pintlv_b8(dstMask, tmpMask, zeroMask, dstMask);
        pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
        pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
    }
}

template <RegLayout layoutMode>
__simd_callee__ inline void AdjustMask_b32_b64(MaskReg& dstMask, MaskReg& srcMask)
{
    static_assert(
        SupportEnum<layoutMode, RegLayout::ZERO, RegLayout::ONE>(),
        "AdjustMask_b32_b64 only supports RegLayout ZERO and ONE");
    MaskReg zeroMask = CreateMask<uint8_t, MaskPattern::ALLF>();
    MaskReg tmpMask;
    ppack(dstMask, srcMask, std::integral_constant<::HiloPart, static_cast<::HiloPart>(HighLowPart::LOWEST)>());
    ppack(dstMask, dstMask, std::integral_constant<::HiloPart, static_cast<::HiloPart>(HighLowPart::LOWEST)>());
    if constexpr (layoutMode == RegLayout::ZERO) {
        pdintlv_b8(dstMask, tmpMask, dstMask, dstMask);
    } else {
        pdintlv_b8(tmpMask, dstMask, dstMask, dstMask);
    }
    pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
    pintlv_b8(dstMask, tmpMask, dstMask, zeroMask);
    // since vmov not support b64, need to interleave
    pintlv_b32(dstMask, tmpMask, dstMask, dstMask);
}

template <typename T, typename U = DefaultType, RegLayout srcLayout = RegLayout::ZERO>
__simd_callee__ inline void AdjustMask(MaskReg& dstMask, MaskReg& srcMask)
{
    if constexpr (std::is_same_v<U, DefaultType>) {
        dstMask = srcMask;
        return;
    }
    constexpr size_t srcSize = sizeof(U);
    constexpr size_t dstSize = sizeof(T);
    if constexpr (srcSize > dstSize) {
        if constexpr (srcSize == 4 && dstSize == 1) {
            AdjustMask_b32_b8<srcLayout>(dstMask, srcMask);
        } else if constexpr (srcSize == 4 && dstSize == 2) {
            AdjustMask_b32_b16<srcLayout>(dstMask, srcMask);
        } else if constexpr (srcSize == 2 && dstSize == 1) {
            if constexpr (
                IsSameType<fp4x2_e2m1_t, T>::value || IsSameType<fp4x2_e1m2_t, T>::value ||
                IsSameType<int4x2_t, T>::value) {
                AdjustMask_b16_b4<srcLayout>(dstMask, srcMask);
            } else {
                AdjustMask_b16_b8<srcLayout>(dstMask, srcMask);
            }
        } else if constexpr (srcSize == 8 && dstSize == 4) {
            AdjustMask_b64_b32<srcLayout>(dstMask, srcMask);
        } else {
            dstMask = srcMask;
        }
    } else if constexpr (srcSize < dstSize) {
        if constexpr (srcSize == 1 && dstSize == 4) {
            AdjustMask_b8_b32<srcLayout>(dstMask, srcMask);
        } else if constexpr (srcSize == 1 && dstSize == 2) {
            if constexpr (
                IsSameType<fp4x2_e2m1_t, U>::value || IsSameType<fp4x2_e1m2_t, U>::value ||
                IsSameType<int4x2_t, U>::value) {
                AdjustMask_b4_b16<srcLayout>(dstMask, srcMask);
            } else {
                AdjustMask_b8_b16<srcLayout>(dstMask, srcMask);
            }
        } else if constexpr (srcSize == 2 && dstSize == 4) {
            AdjustMask_b16_b32<srcLayout>(dstMask, srcMask);
        } else if constexpr (srcSize == 4 && dstSize == 8) {
            AdjustMask_b32_b64<srcLayout>(dstMask, srcMask);
        } else {
            dstMask = srcMask;
        }
    } else {
        dstMask = srcMask;
    }
}

template <
    typename T = DefaultType, typename U = DefaultType, RegLayout srcLayout = RegLayout::ZERO,
    const RegTrait& srcRegTrait = RegTraitNumOne, typename RegT>
__simd_callee__ inline void CopyMergingImpl(RegT& dstReg, RegT& srcReg, MaskReg& mask)
{
    using ActualT = typename RegT::ActualT;
    constexpr auto modeValue = GetMaskMergeMode<MaskMergeMode::MERGING>();

    MaskReg adjustedMask;
    AdjustMask<ActualT, U, srcLayout>(adjustedMask, mask);

    if constexpr (IsSameType<ActualT, bool>::value) {
        vmov((RegTensor<int8_t>&)dstReg, (RegTensor<int8_t>&)srcReg, adjustedMask, modeValue);
    } else if constexpr (sizeof(ActualT) == 1) {
        vmov((RegTensor<uint8_t>&)dstReg, (RegTensor<uint8_t>&)srcReg, adjustedMask, modeValue);
    } else if constexpr (sizeof(ActualT) == 2) {
        vmov((RegTensor<uint16_t>&)dstReg, (RegTensor<uint16_t>&)srcReg, adjustedMask, modeValue);
    } else if constexpr (sizeof(ActualT) == 4) {
        if constexpr (srcRegTrait.REG_NUM == RegTraitNumOne.REG_NUM) {
            vmov((RegTensor<uint32_t>&)dstReg, (RegTensor<uint32_t>&)srcReg, adjustedMask, modeValue);
        } else {
            vmov((RegTensor<uint32_t>&)dstReg, (RegTensor<uint32_t>&)srcReg, mask, modeValue);
        }
    } else if constexpr (sizeof(ActualT) == 8) {
        if constexpr (CheckRegTrait<RegT, RegTraitNumOne>()) {
            // since vmov only supports b32, interleave the mask for the b32 view
            MaskReg tmpMask;
            MaskReg dumpMask;
            ppack(
                tmpMask, adjustedMask,
                std::integral_constant<::HiloPart, static_cast<::HiloPart>(HighLowPart::LOWEST)>());
            pintlv_b32(adjustedMask, dumpMask, tmpMask, tmpMask);
            vmov((RegTensor<uint32_t>&)dstReg, (RegTensor<uint32_t>&)srcReg, adjustedMask, modeValue);
        } else if constexpr (CheckRegTrait<RegT, RegTraitNumTwo>()) {
            vmov((RegTensor<uint32_t>&)dstReg.reg[0], (RegTensor<uint32_t>&)srcReg.reg[0], mask, modeValue);
            vmov((RegTensor<uint32_t>&)dstReg.reg[1], (RegTensor<uint32_t>&)srcReg.reg[1], mask, modeValue);
        }
    }
}

template <typename T, typename U>
__simd_callee__ inline void B64TraitOneToTraitTwo(T& dstReg, U& srcReg)
{
    TraitOneToTraitTwoTmpl<T, U, uint32_t>(dstReg, srcReg);
}

template <typename T, typename U>
__simd_callee__ inline void B64TraitTwoToTraitOne(T& dstReg, U& srcReg)
{
    TraitTwoToTraitOneTmpl<T, U, uint32_t>(dstReg, srcReg);
}

template <typename T, typename U>
__simd_callee__ inline void B32TraitOneToTraitTwo(T& dstReg, U& srcReg)
{
    TraitOneToTraitTwoTmpl<T, U, uint16_t>(dstReg, srcReg);
}

template <typename T, typename U>
__simd_callee__ inline void B32TraitTwoToTraitOne(T& dstReg, U& srcReg)
{
    TraitTwoToTraitOneTmpl<T, U, uint16_t>(dstReg, srcReg);
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_COMMON_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_COMMON_IMPL__
#endif
