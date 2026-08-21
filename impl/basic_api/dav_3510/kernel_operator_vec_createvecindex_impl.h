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
 * \file kernel_operator_vec_createvecindex_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_3510/kernel_operator_vec_createvecindex_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CREATEVECINDEX_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_VEC_CREATEVECINDEX_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_CREATEVECINDEX_IMPL_H
#include "../../../include/basic_api/kernel_tensor.h"
#include "kernel_operator_vec_template_impl.h"
#include "../../../include/basic_api/reg_compute/kernel_reg_compute_intf.h"
#if ASCENDC_CPU_DEBUG
#include "../kernel_check.h"
#endif

namespace AscendC {
template <typename T>
constexpr __aicore__ inline void CheckCreateVecIndexApi0SupportedType()
{
    static_assert(
        SupportType<T, int16_t, int32_t, half, float>(),
        "CreateVecIndex level-0 api only support int16_t/int32_t/half/float on current device");
}

template <typename T>
constexpr __aicore__ inline void CheckCreateVecIndexApi2SupportedType()
{
    static_assert(
        SupportType<T, int8_t, int16_t, int32_t, half, float, int64_t>(),
        "CreateVecIndex level-2 api only support int8_t/int16_t/int32_t/half/float/int64_t on current device");
}

namespace Internal {
template <bool isMaskBitMode, bool isNormalMode, typename T>
__simd_vf__ inline void VecCreateVecIndexLevel0VFImpl(
    __ubuf__ T* dst, const T firstValue, const BasicAPIMaskStruct maskArrayStruct, const uint64_t maskCount,
    const uint8_t repeatTime, uint16_t dstBlkStride, uint8_t dstRepStride, __ubuf__ uint64_t* maskBuf)
{
    constexpr uint32_t sreg = GetVecLen() / sizeof(T);
    uint32_t count = VecMicroGetCount<true, isNormalMode, isMaskBitMode>(maskArrayStruct.maskArray, maskCount, maskBuf);
    uint16_t newRepeatTimes = VecMicroGetRepeatTimes<T, isNormalMode>(count, repeatTime);
    Reg::MaskReg maskReg;
    if constexpr (isNormalMode) {
        maskReg = VecMicroGetMaskReg<T, true, isNormalMode, isMaskBitMode>(maskBuf, count);
    }
    constexpr uint8_t ElePerBlkT = GetDataBlockSizeInBytes() / sizeof(T);
    Reg::RegTensor<T> dstVreg;
    Reg::Arange(dstVreg, firstValue);
    for (uint16_t index = 0; index < newRepeatTimes; ++index) {
        if constexpr (!isNormalMode) {
            maskReg = VecMicroGetMaskReg<T, true, isNormalMode, isMaskBitMode>(maskBuf, count);
        }
        Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            dst + index * dstRepStride * ElePerBlkT, dstVreg, dstBlkStride, maskReg);
        Reg::Adds(dstVreg, dstVreg, sreg, maskReg);
    }
}

template <bool isMaskBitMode, typename T>
__aicore__ inline void VecCreateVecIndexLevel0Template(
    __ubuf__ T* dst, const T firstValue, const uint64_t maskArray[], const uint64_t maskCount, const uint8_t repeatTime,
    uint16_t dstBlkStride, uint8_t dstRepStride)
{
    BasicAPIMaskStruct maskArrayStruct;
    if constexpr (isMaskBitMode) {
        ASCENDC_ASSERT(
            maskCount == 0, { KERNEL_LOG(KERNEL_ERROR, "maskCount must be 0 when isMaskBitMode is true."); });
        maskArrayStruct = *(reinterpret_cast<const BasicAPIMaskStruct*>(maskArray));
    } else {
        ASCENDC_ASSERT(maskArray == nullptr, {
            KERNEL_LOG(KERNEL_ERROR, "maskArray must be nullptr when isMaskBitMode is false.");
        });
    }

    if (Internal::IsCounterMode()) {
        VecCreateVecIndexLevel0VFImpl<isMaskBitMode, false, T>(
            dst, firstValue, maskArrayStruct, maskCount, repeatTime, dstBlkStride, dstRepStride, nullptr);
    } else {
        if constexpr (isMaskBitMode) {
            SetVectorMask<T>(maskArray[1], maskArray[0]); // set mask to SPR.MASK, movp in VF
        }
        VecCreateVecIndexLevel0VFImpl<isMaskBitMode, true, T>(
            dst, firstValue, maskArrayStruct, maskCount, repeatTime, dstBlkStride, dstRepStride, nullptr);
    }
}
} // namespace Internal

// VCI level-0 normal
template <typename T>
__aicore__ inline void CreateVecIndexCalc(
    LocalTensor<T>& dstLocal, const T firstValue, uint64_t mask, uint8_t repeatTime, uint16_t dstBlkStride,
    uint8_t dstRepStride)
{
    CheckCreateVecIndexApi0SupportedType<T>();

    __ubuf__ T* dst = (__ubuf__ T*)dstLocal.GetPhyAddr();
    Internal::VecCreateVecIndexLevel0Template<false>(
        dst, firstValue, nullptr, mask, repeatTime, dstBlkStride, dstRepStride);
}

// VCI level-0 bitwise
template <typename T>
__aicore__ inline void CreateVecIndexCalc(
    LocalTensor<T>& dstLocal, const T firstValue, uint64_t mask[], uint8_t repeatTime, uint16_t dstBlkStride,
    uint8_t dstRepStride)
{
    CheckCreateVecIndexApi0SupportedType<T>();

    __ubuf__ T* dst = (__ubuf__ T*)dstLocal.GetPhyAddr();
    Internal::VecCreateVecIndexLevel0Template<true>(dst, firstValue, mask, 0, repeatTime, dstBlkStride, dstRepStride);
}

template <typename T>
__simd_vf__ inline void CreateVecIndexCalcVci2(
    __ubuf__ T* dstLocalAddr, const T firstValue, uint32_t sreg, uint32_t sregLower, uint16_t repeatTime)
{
    Reg::RegTensor<T> vreg0;
    Reg::MaskReg preg;
    Reg::Arange(vreg0, firstValue);
    for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {
        preg = Reg::UpdateMask<T>(sreg);
        Reg::StoreAlign(dstLocalAddr + i * sregLower, vreg0, preg);
        Reg::Adds(vreg0, vreg0, sregLower, preg);
    }
}

// VCI level-2
template <typename T>
__aicore__ inline void CreateVecIndexCalc(LocalTensor<T> dstLocal, const T firstValue, uint32_t calCount)
{
    CheckCreateVecIndexApi2SupportedType<T>();

    __ubuf__ T* dstLocalAddr = (__ubuf__ T*)dstLocal.GetPhyAddr();
    uint32_t sreg = static_cast<uint32_t>(calCount);
    uint32_t sregLower = static_cast<uint32_t>(GetVecLen() / sizeof(T));
    uint16_t repeatTime = CeilDivision(calCount, sregLower);
    CreateVecIndexCalcVci2<T>(dstLocalAddr, firstValue, sreg, sregLower, repeatTime);
}

template <typename T = int64_t>
__simd_vf__ inline void CreateVecIndexCalcVic(
    __ubuf__ int64_t* dstLocalAddr, const int64_t firstValue, uint32_t calCount)
{
    Reg::RegTensor<int64_t, Reg::RegTraitNumTwo> vreg0;
    uint32_t sreg = static_cast<uint32_t>(calCount);
    Reg::MaskReg preg;
    constexpr uint16_t sregLower = SupportBytes<T, 8>() ? GetVecLen() / sizeof(float) : GetVecLen() / sizeof(T);
    uint16_t repeatTime = CeilDivision(calCount, sregLower);
    for (uint16_t i = 0; i < repeatTime; ++i) {
        preg = Reg::UpdateMask<int64_t, Reg::RegTraitNumTwo>(sreg);
        int64_t offset = static_cast<int64_t>(firstValue + i * sregLower);
        Reg::Arange(vreg0, offset);
        Reg::StoreAlign(dstLocalAddr + i * sregLower, vreg0, preg);
    }
}

template <typename T = int64_t>
__aicore__ inline void CreateVecIndexCalc(LocalTensor<int64_t> dstLocal, const int64_t firstValue, uint32_t calCount)
{
    __ubuf__ int64_t* dstLocalAddr = (__ubuf__ int64_t*)dstLocal.GetPhyAddr();
    CreateVecIndexCalcVic<T>(dstLocalAddr, firstValue, calCount);
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_CREATEVECINDEX_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CREATEVECINDEX_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CREATEVECINDEX_IMPL_H__
#endif
