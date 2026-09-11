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
 * \file kernel_reg_compute_maskreg_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic/reg_compute/dav_v516/kernel_reg_compute_maskreg_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \"#include \"reg_compute/kernel_reg_compute_maskreg_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_MASKREG_IMPL__
#endif

#ifndef ASCENDC_MODULE_REG_COMPUTE_MASKREG_IMPL_H
#define ASCENDC_MODULE_REG_COMPUTE_MASKREG_IMPL_H

namespace AscendC {
namespace Reg {
template <typename T, const RegTrait& regTrait = RegTraitNumOne>
__simd_callee__ inline MaskReg UpdateMaskImpl(uint32_t& scalarValue)
{
    static_assert(SupportBytes<T, 1, 2, 4, 8>(), "UpdateMask only support type b8/b16/b32/b64 on current device");
    MaskReg reg;
    if constexpr (sizeof(T) == 1) {
        reg = plt_b8(scalarValue, POST_UPDATE);
    } else if constexpr (sizeof(T) == 2) {
        reg = plt_b16(scalarValue, POST_UPDATE);
    } else if constexpr (sizeof(T) == 4) {
        if constexpr (SupportType<T, complex32>()) {
            if constexpr (regTrait.REG_NUM == 2) {
                reg = plt_b16(scalarValue, POST_UPDATE);
            } else {
                uint32_t updateScalar = scalarValue;
                reg = plt_b16(scalarValue, POST_UPDATE);
                punpack(reg, reg, LOWER);
                constexpr uint32_t one_repeat_num = VECTOR_REG_WIDTH / sizeof(T);
                scalarValue = (updateScalar > one_repeat_num) ? (updateScalar - one_repeat_num) : 0;
            }
        } else {
            reg = plt_b32(scalarValue, POST_UPDATE);
        }
    } else if constexpr (sizeof(T) == 8) {
        if constexpr (regTrait.REG_NUM == 2) {
            reg = plt_b32(scalarValue, POST_UPDATE);
        } else {
            uint32_t updateScalar = scalarValue;
            reg = plt_b32(scalarValue, POST_UPDATE);
            punpack(reg, reg, LOWER);
            constexpr uint32_t one_repeat_num = VECTOR_REG_WIDTH / sizeof(T);
            scalarValue = (updateScalar > one_repeat_num) ? (updateScalar - one_repeat_num) : 0;
        }
    }
    return reg;
}

template <typename T, MaskPattern mode, const RegTrait& regTrait>
__simd_callee__ inline constexpr MaskPattern GetMaskPattern()
{
    if constexpr ((regTrait.REG_NUM == 1) && (sizeof(T) == 8)) {
        if constexpr (mode == MaskPattern::H) {
            return MaskPattern::VL16;
        } else if constexpr (mode == MaskPattern::Q) {
            return MaskPattern::VL8;
        }
    } else if constexpr ((regTrait.REG_NUM == 1) && SupportType<T, complex32>()) {
        if constexpr (mode == MaskPattern::H) {
            return MaskPattern::VL32;
        } else if constexpr (mode == MaskPattern::Q) {
            return MaskPattern::VL16;
        }
    }
    return mode;
}

template <typename T, MaskPattern mode = MaskPattern::ALL, const RegTrait& regTrait = RegTraitNumOne>
__simd_callee__ inline MaskReg CreateMaskImpl()
{
    static_assert(SupportBytes<T, 1, 2, 4, 8>(), "CreateMask only support type b8/b16/b32/b64 on current device");
    constexpr auto modeValue = std::integral_constant<::Pat, static_cast<::Pat>(GetMaskPattern<T, mode, regTrait>())>();
    MaskReg reg;
    if constexpr (sizeof(T) == 1) {
        reg = pset_b8(modeValue);
    } else if constexpr (sizeof(T) == 2) {
        reg = pset_b16(modeValue);
    } else if constexpr (sizeof(T) == 4) {
        if constexpr (SupportType<T, complex32>()) {
            reg = pset_b16(modeValue);
            if constexpr (regTrait.REG_NUM == 1) {
                punpack(reg, reg, LOWER);
            }
        } else {
            reg = pset_b32(modeValue);
        }
    } else if constexpr (sizeof(T) == 8) {
        reg = pset_b32(modeValue);
        if constexpr (regTrait.REG_NUM == 1) {
            punpack(reg, reg, LOWER);
        }
    }
    return reg;
}

template <typename T = DefaultType, int16_t offset, typename U>
__simd_callee__ inline void MaskGenWithRegTensorImpl(MaskReg& dst, U& srcReg)
{
    using ActualT = typename U::ActualT;
    static_assert(Std::is_same_v<T, DefaultType> || Std::is_same_v<T, ActualT>, "T type is not correct!");
    static_assert(SupportBytes<ActualT, 2, 4>(), "MaskGenWithRegTensor only support type b16/b32 on current device");
    if constexpr (sizeof(ActualT) == 2) {
        static_assert((offset >= 0) && (offset <= 15), "MaskGenWithRegTensor offset must be in 0~15 when T is b16");
        movvp(dst, (RegTensor<uint16_t>&)srcReg, offset);
    } else if constexpr (sizeof(ActualT) == 4) {
        static_assert((offset >= 0) && (offset <= 31), "MaskGenWithRegTensor offset must be in 0~31 when T is b32");
        movvp(dst, (RegTensor<uint32_t>&)srcReg, offset);
    }
}

__simd_callee__ inline void MaskNotImpl(MaskReg& dst, MaskReg& src, MaskReg& mask) { pnot(dst, src, mask); }

__simd_callee__ inline void MaskAndImpl(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask)
{
    pand(dst, src0, src1, mask);
}

__simd_callee__ inline void MaskOrImpl(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask)
{
    por(dst, src0, src1, mask);
}

__simd_callee__ inline void MaskXorImpl(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask)
{
    pxor(dst, src0, src1, mask);
}

__simd_callee__ inline void MaskMovImpl(MaskReg& dst, MaskReg& src, MaskReg& mask) { pmov(dst, src, mask); }

__simd_callee__ inline void MaskMovImpl(MaskReg& dst, MaskReg& src) { pmov(dst, src); }

template <typename T>
__simd_callee__ inline void MaskInterleaveImpl(MaskReg& dst0, MaskReg& dst1, MaskReg& src0, MaskReg& src1)
{
    static_assert(SupportBytes<T, 1, 2, 4>(), "MaskInterleave only support type b8/b16/b32 on current device");
    if constexpr (sizeof(T) == 1) {
        pintlv_b8(dst0, dst1, src0, src1);
    } else if constexpr (sizeof(T) == 2) {
        pintlv_b16(dst0, dst1, src0, src1);
    } else if constexpr (sizeof(T) == 4) {
        pintlv_b32(dst0, dst1, src0, src1);
    }
}

template <typename T>
__simd_callee__ inline void MaskDeInterleaveImpl(MaskReg& dst0, MaskReg& dst1, MaskReg& src0, MaskReg& src1)
{
    static_assert(SupportBytes<T, 1, 2, 4>(), "MaskDeInterleave only support type b8/b16/b32 on current device");
    if constexpr (sizeof(T) == 1) {
        pdintlv_b8(dst0, dst1, src0, src1);
    } else if constexpr (sizeof(T) == 2) {
        pdintlv_b16(dst0, dst1, src0, src1);
    } else if constexpr (sizeof(T) == 4) {
        pdintlv_b32(dst0, dst1, src0, src1);
    }
}

__simd_callee__ inline void MaskSelImpl(MaskReg& dst, MaskReg& src0, MaskReg& src1, MaskReg& mask)
{
    psel(dst, src0, src1, mask);
}

template <HighLowPart part = HighLowPart::LOWEST>
__simd_callee__ inline void MaskPackImpl(MaskReg& dst, MaskReg& src)
{
    constexpr auto partValue = std::integral_constant<::HiloPart, static_cast<::HiloPart>(part)>();
    ppack(dst, src, partValue);
}

template <HighLowPart part = HighLowPart::LOWEST>
__simd_callee__ inline void MaskUnPackImpl(MaskReg& dst, MaskReg& src)
{
    constexpr auto partValue = std::integral_constant<::HiloPart, static_cast<::HiloPart>(part)>();
    punpack(dst, src, partValue);
}

template <typename T>
__simd_callee__ inline MaskReg MoveMaskImpl()
{
    static_assert(SupportBytes<T, 2, 4>(), "MoveMask only support type b16/b32 on current device");

    if constexpr (sizeof(T) == 2) {
        return movp_b16();
    } else if constexpr (sizeof(T) == 4) {
        return movp_b32();
    }
}
} // namespace Reg
} // namespace AscendC
#endif // ASCENDC_MODULE_REG_COMPUTE_MASKREG_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_MASKREG_IMPL__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_REG_COMPUTE_MASKREG_IMPL__
#endif
