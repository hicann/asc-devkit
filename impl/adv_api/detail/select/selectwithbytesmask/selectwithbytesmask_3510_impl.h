/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/select/selectwithbytesmask/selectwithbytesmask_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/select/selectwithbytesmask.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SELECT_SELECTWITHBYTESMASK_SELECTWITHBYTESMASK_C310_IMPL_H__
#endif
#ifndef LIB_SELECT_SELECT_WITH_BYTES_MASK_C310_IMPL_H
#define LIB_SELECT_SELECT_WITH_BYTES_MASK_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../basic_api/kernel_utils.h"

namespace AscendC {
template <typename T, typename U, CMPMODE cmpMode>
__simd_callee__ inline void RegTensorToMaskReg(
    Reg::RegTensor<U>& vMaskReg0, Reg::RegTensor<U>& vMaskReg1, Reg::MaskReg& localMask0, Reg::MaskReg& maskReg0)
{
    Reg::MaskReg maskReg1;
    Reg::MaskReg localMask1;
    if constexpr (sizeof(U) == 1) {
        Reg::CompareScalar<uint8_t, cmpMode>(
            localMask0, (Reg::RegTensor<uint8_t>&)vMaskReg0, static_cast<uint8_t>(0), maskReg0);
    } else if constexpr (sizeof(T) == 2 && sizeof(U) == 4) {
        Reg::MaskUnPack(maskReg1, maskReg0);
        Reg::CompareScalar<U, cmpMode>(localMask0, vMaskReg0, static_cast<U>(0), maskReg1);
        Reg::CompareScalar<U, cmpMode>(localMask1, vMaskReg1, static_cast<U>(0), maskReg1);
        Reg::MaskDeInterleave<T>(localMask0, localMask1, localMask0, localMask1);
    } else {
        Reg::CompareScalar<U, cmpMode>(localMask0, vMaskReg0, static_cast<U>(0), maskReg0);
    }
}

template <typename T, typename U, bool reverse = false>
__simd_vf__ inline void SelectWithBytesMaskPerAxisImpl(
    __ubuf__ T* dstUb, __ubuf__ T* src0Ub, T src1, __ubuf__ U* maskUb, const uint32_t firstAxis,
    const uint32_t srcLastAxis, const uint32_t maskLastAxis)
{
    Reg::RegTensor<T> vSrcReg0;
    Reg::RegTensor<T> vSrcReg1;
    Reg::RegTensor<T> vDstReg;
    Reg::RegTensor<U> vMaskReg0;
    Reg::RegTensor<U> vMaskReg1;
    Reg::MaskReg maskReg0;
    Reg::MaskReg localMask0;
    Reg::Duplicate(vSrcReg1, src1);
    uint32_t sreg;
    uint32_t sregLower = static_cast<uint32_t>(GetVecLen() / sizeof(T));
    uint16_t repeatTimes = static_cast<uint16_t>(DivCeil(srcLastAxis, sregLower));
    for (uint16_t loopH = 0; loopH < static_cast<uint16_t>(firstAxis); ++loopH) {
        sreg = srcLastAxis;
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            maskReg0 = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(vSrcReg0, src0Ub + loopH * srcLastAxis + i * sregLower);
            if constexpr (sizeof(T) == 2 && sizeof(U) == 1) {
                Reg::LoadAlign<uint8_t, Reg::LoadDist::DIST_UNPACK_B8>(
                    (Reg::RegTensor<uint8_t>&)vMaskReg0,
                    (__ubuf__ uint8_t*)maskUb + loopH * maskLastAxis + i * sregLower);
            } else if constexpr (sizeof(T) == 2 && sizeof(U) == 4) {
                Reg::LoadAlign<U>(vMaskReg0, maskUb + loopH * maskLastAxis + i * sregLower);
                Reg::LoadAlign<U>(vMaskReg1, maskUb + loopH * maskLastAxis + i * sregLower + sregLower / 2);
            } else if constexpr (sizeof(T) == 4 && sizeof(U) == 1) {
                Reg::LoadAlign<uint8_t, Reg::LoadDist::DIST_UNPACK4_B8>(
                    (Reg::RegTensor<uint8_t>&)vMaskReg0,
                    (__ubuf__ uint8_t*)maskUb + loopH * maskLastAxis + i * sregLower);
            } else if constexpr (sizeof(T) == 4 && sizeof(U) == 2) {
                Reg::LoadAlign<U, Reg::LoadDist::DIST_UNPACK_B16>(
                    vMaskReg0, maskUb + loopH * maskLastAxis + i * sregLower);
            } else if constexpr (sizeof(T) == sizeof(U)) {
                Reg::LoadAlign<U>(vMaskReg0, maskUb + loopH * maskLastAxis + i * sregLower);
            }

            if constexpr (!reverse) {
                RegTensorToMaskReg<T, U, CMPMODE::EQ>(vMaskReg0, vMaskReg1, localMask0, maskReg0);
            } else {
                RegTensorToMaskReg<T, U, CMPMODE::NE>(vMaskReg0, vMaskReg1, localMask0, maskReg0);
            }

            Reg::Select(vDstReg, vSrcReg0, vSrcReg1, localMask0);
            Reg::StoreAlign<T>(dstUb + loopH * srcLastAxis + i * sregLower, vDstReg, maskReg0);
        }
    }
}

template <typename T, typename U, bool reverse = false>
__aicore__ inline void SelectWithBytesMaskProcess(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, T src1, const LocalTensor<U>& mask,
    const SelectWithBytesMaskShapeInfo& info)
{
    __ubuf__ T* src0Ub = (__ubuf__ T*)src0.GetPhyAddr();
    __ubuf__ T* dstUb = (__ubuf__ T*)dst.GetPhyAddr();
    __ubuf__ U* maskUb = (__ubuf__ U*)mask.GetPhyAddr();
    const uint32_t firstAxis = static_cast<uint32_t>(info.firstAxis);
    const uint32_t srcLastAxis = static_cast<uint32_t>(info.srcLastAxis);
    const uint32_t maskLastAxis = static_cast<uint32_t>(info.maskLastAxis);
    SelectWithBytesMaskPerAxisImpl<T, U, reverse>(dstUb, src0Ub, src1, maskUb, firstAxis, srcLastAxis, maskLastAxis);
}

// Selects Values from two sources and put into dst according to the mask values.
// True: Select scalar, False: select src.
template <typename T, typename U, bool isReuseMask, bool reverse = false>
__aicore__ inline __inout_pipe__(V) void SelectWithBytesMaskImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, T src1, const LocalTensor<U>& mask,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const SelectWithBytesMaskShapeInfo& info)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    static_assert(SupportType<T, float, half>(), "Select do not support this type on current device");
    static_assert(
        SupportType<U, bool, uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t>(),
        "Select do not support this type on current device");
    CheckTensorPos<T>(dst, Hardware::UB, "dst", "VECIN/VECCALC/VECOUT", "Select");
    CheckTensorPos<T>(src0, Hardware::UB, "src", "VECIN/VECCALC/VECOUT", "Select");
    CheckTensorPos<U>(mask, Hardware::UB, "mask", "VECIN/VECCALC/VECOUT", "Select");
    CheckTensorPos<uint8_t>(sharedTmpBuffer, Hardware::UB, "sharedTmpBuffer", "VECIN/VECCALC/VECOUT", "Select");
    ASCENDC_ASSERT((info.srcLastAxis * sizeof(T) % ONE_BLK_SIZE == 0), {
        KERNEL_LOG(KERNEL_ERROR, "srcLastAxis should be 32B aligned, current srcLastAxis is %u", info.srcLastAxis);
    });
    ASCENDC_ASSERT((info.maskLastAxis * sizeof(U) % ONE_BLK_SIZE == 0), {
        KERNEL_LOG(KERNEL_ERROR, "maskLastAxis should be 32B aligned, current maskLastAxis is %u", info.maskLastAxis);
    });
    ASCENDC_ASSERT((info.maskLastAxis % BLOCK_CUBE == 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "maskLastAxis should be multiples of 16, current maskLastAxis is %u", info.maskLastAxis);
    });

    const uint32_t firstAxis = info.firstAxis;
    const uint32_t srcLastAxis = info.srcLastAxis;
    const uint32_t maskLastAxis = info.maskLastAxis;
    const uint32_t srcSize = src0.GetSize();

    ASCENDC_ASSERT((srcSize == firstAxis * srcLastAxis), {
        KERNEL_LOG(KERNEL_ERROR, "ShapeInfo must match with src Tensor size.");
    });
    ASCENDC_ASSERT((mask.GetSize() == firstAxis * maskLastAxis), {
        KERNEL_LOG(KERNEL_ERROR, "ShapeInfo must match with mask Tensor size.");
    });
    ASCENDC_ASSERT((maskLastAxis >= srcLastAxis), {
        KERNEL_LOG(KERNEL_ERROR, "maskLastAxis must be greater than or equal to srcLastAxis.");
    });

    SelectWithBytesMaskProcess<T, U, reverse>(dst, src0, src1, mask, info);
}

template <typename T, typename U, bool isReuseMask = true>
__aicore__ inline void SelectWithBytesMask(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, T src1, const LocalTensor<U>& mask,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const SelectWithBytesMaskShapeInfo& info)
{
    SelectWithBytesMaskImpl<T, U, isReuseMask, false>(dst, src0, src1, mask, sharedTmpBuffer, info);
}

template <typename T, typename U, bool isReuseMask = true>
__aicore__ inline void SelectWithBytesMask(
    const LocalTensor<T>& dst, T src0, const LocalTensor<T>& src1, const LocalTensor<U>& mask,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const SelectWithBytesMaskShapeInfo& info)
{
    SelectWithBytesMaskImpl<T, U, isReuseMask, true>(dst, src1, src0, mask, sharedTmpBuffer, info);
}
} // namespace AscendC
#endif // LIB_SELECT_SELECT_WITH_BYTES_MASK_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SELECT_SELECTWITHBYTESMASK_SELECTWITHBYTESMASK_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SELECT_SELECTWITHBYTESMASK_SELECTWITHBYTESMASK_C310_IMPL_H__
#endif
