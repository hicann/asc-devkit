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
 * \file dropout_3510_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/filter/dropout/dropout_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/filter/dropout.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DROPOUT_C310_IMPL_H__
#endif

#ifndef LIB_DROPOUT_DROPOUT_C310_IMPL_H
#define LIB_DROPOUT_DROPOUT_C310_IMPL_H

#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"

namespace AscendC {
namespace Internal {
__simd_callee__ inline void DropOutBitModeFP32Main(
    __ubuf__ float* dstUb, __ubuf__ float* srcUb, __ubuf__ uint8_t* maskUb, Reg::RegTensor<float>& vDivValueReg,
    uint32_t sreg, uint32_t newRepeatTimes, uint16_t loopH, uint32_t srcLastAxis, uint32_t maskLastAxis)
{
    constexpr uint32_t unRollConstant = 2;
    constexpr uint32_t maskBitToByte = 8;
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(float);
    constexpr uint32_t selOffset = repeatElm / maskBitToByte * unRollConstant / (sizeof(float) / sizeof(uint8_t));
    Reg::RegTensor<float> src0Reg;
    Reg::RegTensor<float> src1Reg;
    Reg::RegTensor<float> scalarReg;
    Reg::RegTensor<float> dst0Reg;
    Reg::RegTensor<float> dst1Reg;
    Reg::MaskReg maskReg;
    Reg::MaskReg selMask0;
    Reg::MaskReg selMask1;
    Reg::MaskReg tmpMask0;
    Reg::MaskReg tmpMask1 = Reg::CreateMask<uint8_t, Reg::MaskPattern::ALL>();
    Reg::Duplicate(scalarReg, (const float&)0);
    for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTimes); ++i) {
        Reg::LoadAlign<uint32_t, Reg::MaskDist::DIST_US>(
            tmpMask0, (__ubuf__ uint32_t*)maskUb + loopH * (maskLastAxis >> 2) + i * selOffset);
        Reg::MaskInterleave<uint16_t>(selMask0, selMask1, tmpMask0, tmpMask1);
        maskReg = Reg::UpdateMask<float>(sreg);
        Reg::LoadAlign<float>(src0Reg, srcUb + loopH * srcLastAxis + i * unRollConstant * repeatElm);
        Reg::Select(dst0Reg, src0Reg, scalarReg, selMask0);
        Reg::Mul(dst0Reg, dst0Reg, vDivValueReg, selMask0);
        Reg::StoreAlign<float>(dstUb + loopH * srcLastAxis + i * unRollConstant * repeatElm, dst0Reg, maskReg);
        maskReg = Reg::UpdateMask<float>(sreg);
        Reg::LoadAlign<float>(src1Reg, srcUb + loopH * srcLastAxis + (i * unRollConstant + 1) * repeatElm);
        Reg::Select(dst1Reg, src1Reg, scalarReg, selMask1);
        Reg::Mul(dst1Reg, dst1Reg, vDivValueReg, selMask1);
        Reg::StoreAlign<float>(dstUb + loopH * srcLastAxis + (i * unRollConstant + 1) * repeatElm, dst1Reg, maskReg);
    }
}

template <typename T>
__simd_vf__ inline void VFDropOutBitModeCalc(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, __ubuf__ uint8_t* maskUb, const T divValue, const uint32_t dataSize)
{
    Reg::RegTensor<T> vDivValueReg;
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(T);
    uint32_t repeatTimes = CeilDivision(dataSize, repeatElm);
    uint32_t sreg = dataSize;
    Reg::Duplicate(vDivValueReg, divValue);
    uint32_t tail = repeatTimes & 1;
    uint32_t newRepeatTimes = repeatTimes >> 1;
    if constexpr (sizeof(T) == 4) {
        DropOutBitModeFP32Main(dstUb, srcUb, maskUb, vDivValueReg, sreg, newRepeatTimes, 0, 0, 0);
        Reg::MaskReg maskReg;
        Reg::MaskReg selMask2;
        Reg::RegTensor<float> src2Reg;
        Reg::RegTensor<float> dst2Reg;
        Reg::RegTensor<float> scalarReg;
        Reg::Duplicate(scalarReg, (const T&)0);
        uint32_t offset = newRepeatTimes * 2 * repeatElm;
        uint32_t selOffset = newRepeatTimes * 4;
        for (uint16_t i = 0; i < static_cast<uint16_t>(tail); ++i) {
            Reg::LoadAlign<uint32_t, Reg::MaskDist::DIST_US>(selMask2, (__ubuf__ uint32_t*)maskUb + selOffset);
            Reg::MaskUnPack(selMask2, selMask2);
            maskReg = Reg::UpdateMask<float>(sreg);
            Reg::LoadAlign<float>(src2Reg, srcUb + offset);
            Reg::Select(dst2Reg, src2Reg, scalarReg, selMask2);
            Reg::Mul(dst2Reg, dst2Reg, vDivValueReg, selMask2);
            Reg::StoreAlign<float>(dstUb + offset, dst2Reg, maskReg);
        }
    } else {
        Reg::RegTensor<T> src0Reg;
        Reg::RegTensor<T> src1Reg;
        Reg::RegTensor<T> dstReg;
        Reg::MaskReg maskReg;
        Reg::MaskReg selMask;
        Reg::Duplicate(src1Reg, (const T&)0);
        for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTimes); ++i) {
            Reg::LoadAlign<uint32_t, Reg::MaskDist::DIST_US>(selMask, (__ubuf__ uint32_t*)maskUb + i * 4);
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src0Reg, srcUb + i * repeatElm);
            Reg::Select(dstReg, src0Reg, src1Reg, selMask);
            Reg::Mul(dstReg, dstReg, vDivValueReg, selMask);
            Reg::StoreAlign<T>(dstUb + i * repeatElm, dstReg, maskReg);
        }
    }
}

template <typename T>
__simd_vf__ inline void VFDropOutBitModeCalcInfo(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, __ubuf__ uint8_t* maskUb, const T divValue, const DropOutShapeInfo info)
{
    Reg::RegTensor<T> vDivValueReg;
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(T);
    uint32_t repeatTimes = CeilDivision(info.srcLastAxis, repeatElm);
    Reg::Duplicate(vDivValueReg, divValue);
    uint32_t tail = repeatTimes & 1;
    uint32_t newRepeatTimes = repeatTimes >> 1;
    for (uint16_t loopH = 0; loopH < static_cast<uint16_t>(info.firstAxis); ++loopH) {
        uint32_t width = info.srcLastAxis;
        if constexpr (sizeof(T) == 4) {
            DropOutBitModeFP32Main(
                dstUb, srcUb, maskUb, vDivValueReg, width, newRepeatTimes, loopH, info.srcLastAxis, info.maskLastAxis);
        } else {
            Reg::RegTensor<T> src0Reg;
            Reg::RegTensor<T> scalarReg;
            Reg::RegTensor<T> dstReg;
            Reg::MaskReg maskReg;
            Reg::MaskReg selMask;
            Reg::Duplicate(scalarReg, (const T&)0);
            for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTimes); ++i) {
                Reg::LoadAlign<uint32_t, Reg::MaskDist::DIST_US>(
                    selMask, (__ubuf__ uint32_t*)maskUb + loopH * (info.maskLastAxis >> 2) + i * 4);
                maskReg = Reg::UpdateMask<T>(width);
                Reg::LoadAlign<T>(src0Reg, srcUb + loopH * info.srcLastAxis + i * repeatElm);
                Reg::Select(dstReg, src0Reg, scalarReg, selMask);
                Reg::Mul(dstReg, dstReg, vDivValueReg, selMask);
                Reg::StoreAlign<T>(dstUb + loopH * info.srcLastAxis + i * repeatElm, dstReg, maskReg);
            }
        }
    }
    if constexpr (sizeof(T) == 4) {
        if (tail != 0) {
            for (uint16_t loopH = 0; loopH < static_cast<uint16_t>(info.firstAxis); ++loopH) {
                uint32_t selOffset = newRepeatTimes * 4;
                uint32_t offset = newRepeatTimes * 2 * repeatElm;
                uint32_t sreg = info.srcLastAxis - offset;
                Reg::MaskReg maskReg;
                Reg::MaskReg selMask2;
                Reg::RegTensor<float> src2Reg;
                Reg::RegTensor<float> dst2Reg;
                Reg::RegTensor<float> scalarReg;
                Reg::Duplicate(scalarReg, (const T&)0);
                Reg::LoadAlign<uint32_t, Reg::MaskDist::DIST_US>(
                    selMask2, (__ubuf__ uint32_t*)maskUb + selOffset + loopH * (info.maskLastAxis >> 2));
                Reg::MaskUnPack(selMask2, selMask2);
                maskReg = Reg::UpdateMask<float>(sreg);
                Reg::LoadAlign<float>(src2Reg, srcUb + offset + loopH * info.srcLastAxis);
                Reg::Select(dst2Reg, src2Reg, scalarReg, selMask2);
                Reg::Mul(dst2Reg, dst2Reg, vDivValueReg, selMask2);
                Reg::StoreAlign<float>(dstUb + offset + loopH * info.srcLastAxis, dst2Reg, maskReg);
            }
        }
    }
}

template <typename T>
__simd_vf__ inline void VFDropOutByteModeCalc(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, __ubuf__ uint8_t* maskUb, const T divValue, const uint32_t dataSize)
{
    Reg::RegTensor<T> vSrcReg;
    Reg::RegTensor<T> vDstReg;
    Reg::RegTensor<T> vDivValueReg;
    Reg::RegTensor<uint8_t> vMaskReg;
    Reg::RegTensor<half> vFP16Reg;
    Reg::RegTensor<float> vFP32Reg;
#if defined(__NPU_ARCH__)
    Reg::RegTensor<bfloat16_t> vBF16Reg;
#endif
    Reg::MaskReg maskReg;
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(T);
    uint32_t sreg = dataSize;
    uint32_t repeatTimes = CeilDivision(dataSize, repeatElm);
    Reg::Duplicate(vDivValueReg, divValue);
    for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTimes); ++i) {
        maskReg = Reg::UpdateMask<T>(sreg);
        Reg::LoadAlign(vSrcReg, srcUb + i * repeatElm);
        if constexpr (sizeof(T) == 2) {
            Reg::LoadAlign<uint8_t, Reg::LoadDist::DIST_UNPACK_B8>(vMaskReg, maskUb + i * repeatElm);
            Reg::Cast<half, uint8_t, layoutZMrgZ>(vFP16Reg, vMaskReg, maskReg);
#if defined(__NPU_ARCH__)
            if constexpr (SupportType<T, half>()) {
                Reg::Mul(vDstReg, vFP16Reg, vSrcReg, maskReg);
            } else {
                Reg::Cast<bfloat16_t, half, MrgZRndR>(vBF16Reg, vFP16Reg, maskReg);
                Reg::Mul(vDstReg, vBF16Reg, vSrcReg, maskReg);
            }
#else
            Reg::Mul(vDstReg, vFP16Reg, vSrcReg, maskReg);
#endif
        } else {
            Reg::LoadAlign<uint8_t, Reg::LoadDist::DIST_UNPACK4_B8>(vMaskReg, maskUb + i * repeatElm);
            Reg::Cast<half, uint8_t, layoutZMrgZ>(vFP16Reg, vMaskReg, maskReg);
            Reg::Cast<float, half, layoutZMrgZ>(vFP32Reg, vFP16Reg, maskReg);
            Reg::Mul(vDstReg, vFP32Reg, vSrcReg, maskReg);
        }
        Reg::Mul(vDstReg, vDivValueReg, vDstReg, maskReg);
        Reg::StoreAlign(dstUb + i * repeatElm, vDstReg, maskReg);
    }
}

template <typename T>
__simd_vf__ inline void VFDropOutByteModeCalcInfo(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, __ubuf__ uint8_t* maskUb, const T divValue, const DropOutShapeInfo info)
{
    Reg::RegTensor<T> vSrcReg;
    Reg::RegTensor<T> vDstReg;
    Reg::RegTensor<T> vDivValueReg;
    Reg::RegTensor<uint8_t> vMaskReg;
    Reg::RegTensor<half> vFP16Reg;
    Reg::RegTensor<float> vFP32Reg;
#if defined(__NPU_ARCH__)
    Reg::RegTensor<bfloat16_t> vBF16Reg;
#endif
    Reg::MaskReg maskReg;
    constexpr uint32_t repeatElm = GetVecLen() / sizeof(T);
    uint32_t loopWNum = CeilDivision(info.srcLastAxis, repeatElm);
    Reg::Duplicate(vDivValueReg, divValue);
    for (uint16_t loopH = 0; loopH < static_cast<uint16_t>(info.firstAxis); ++loopH) {
        uint32_t width = info.srcLastAxis;
        for (uint16_t loopW = 0; loopW < static_cast<uint16_t>(loopWNum); ++loopW) {
            maskReg = Reg::UpdateMask<T>(width);
            Reg::LoadAlign<T>(vSrcReg, srcUb + loopH * info.srcLastAxis + loopW * repeatElm);
            if constexpr (sizeof(T) == 2) {
                Reg::LoadAlign<uint8_t, Reg::LoadDist::DIST_UNPACK_B8>(
                    vMaskReg, maskUb + loopH * info.maskLastAxis + loopW * repeatElm);
                Reg::Cast<half, uint8_t, layoutZMrgZ>(vFP16Reg, vMaskReg, maskReg);
#if defined(__NPU_ARCH__)
                if constexpr (SupportType<T, half>()) {
                    Reg::Mul(vDstReg, vFP16Reg, vSrcReg, maskReg);
                } else {
                    Reg::Cast<bfloat16_t, half, MrgZRndR>(vBF16Reg, vFP16Reg, maskReg);
                    Reg::Mul(vDstReg, vBF16Reg, vSrcReg, maskReg);
                }
#else
                Reg::Mul(vDstReg, vFP16Reg, vSrcReg, maskReg);
#endif
            } else {
                Reg::LoadAlign<uint8_t, Reg::LoadDist::DIST_UNPACK4_B8>(
                    vMaskReg, maskUb + loopH * info.maskLastAxis + loopW * repeatElm);
                Reg::Cast<half, uint8_t, layoutZMrgZ>(vFP16Reg, vMaskReg, maskReg);
                Reg::Cast<float, half, layoutZMrgZ>(vFP32Reg, vFP16Reg, maskReg);
                Reg::Mul(vDstReg, vFP32Reg, vSrcReg, maskReg);
            }
            Reg::Mul(vDstReg, vDivValueReg, vDstReg, maskReg);
            Reg::StoreAlign(dstUb + loopH * info.srcLastAxis + loopW * repeatElm, vDstReg, maskReg);
        }
    }
}
} // namespace Internal

template <typename T, bool isInitBitMode = false>
__aicore__ inline void DropOutBitMode(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& maskLocal,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const T divValue, const uint32_t dataSize)
{
    static_assert(SupportType<T, half, float, bfloat16_t>(), "current data type is not supported on current device!");
    (void)sharedTmpBuffer;
    __ubuf__ T* srcUb = (__ubuf__ T*)srcLocal.GetPhyAddr();
    __ubuf__ T* dstUb = (__ubuf__ T*)dstLocal.GetPhyAddr();
    __ubuf__ uint8_t* maskUb = (__ubuf__ uint8_t*)maskLocal.GetPhyAddr();

    Internal::VFDropOutBitModeCalc<T>(dstUb, srcUb, maskUb, divValue, dataSize);
}

template <typename T, bool isInitBitMode = false>
__aicore__ inline void DropOutBitMode(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& maskLocal,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const T divValue, const DropOutShapeInfo& info)
{
    static_assert(SupportType<T, half, float, bfloat16_t>(), "current data type is not supported on current device!");
    __ubuf__ T* srcUb = (__ubuf__ T*)srcLocal.GetPhyAddr();
    __ubuf__ T* dstUb = (__ubuf__ T*)dstLocal.GetPhyAddr();
    __ubuf__ uint8_t* maskUb = (__ubuf__ uint8_t*)maskLocal.GetPhyAddr();

    Internal::VFDropOutBitModeCalcInfo<T>(dstUb, srcUb, maskUb, divValue, info);
}

template <typename T>
__aicore__ inline void DropOutByteMode(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& maskLocal,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const T divValue, const uint32_t dataSize)
{
    static_assert(SupportType<T, half, float, bfloat16_t>(), "current data type is not supported on current device!");
    (void)sharedTmpBuffer;
    __ubuf__ T* srcUb = (__ubuf__ T*)srcLocal.GetPhyAddr();
    __ubuf__ T* dstUb = (__ubuf__ T*)dstLocal.GetPhyAddr();
    __ubuf__ uint8_t* maskUb = (__ubuf__ uint8_t*)maskLocal.GetPhyAddr();

    Internal::VFDropOutByteModeCalc<T>(dstUb, srcUb, maskUb, divValue, dataSize);
}

template <typename T>
__aicore__ inline void DropOutByteMode(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& maskLocal,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const T divValue, const DropOutShapeInfo& info)
{
    static_assert(SupportType<T, half, float, bfloat16_t>(), "current data type is not supported on current device!");
    (void)sharedTmpBuffer;
    __ubuf__ T* srcUb = (__ubuf__ T*)srcLocal.GetPhyAddr();
    __ubuf__ T* dstUb = (__ubuf__ T*)dstLocal.GetPhyAddr();
    __ubuf__ uint8_t* maskUb = (__ubuf__ uint8_t*)maskLocal.GetPhyAddr();

    Internal::VFDropOutByteModeCalcInfo<T>(dstUb, srcUb, maskUb, divValue, info);
}
} // namespace AscendC
#endif // LIB_DROPOUT_DROPOUT_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DROPOUT_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DROPOUT_C310_IMPL_H__
#endif
