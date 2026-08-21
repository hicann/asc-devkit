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
 * \file kernel_operator_vec_ternary_scalar_impl.h
 * \brief AscendC c310 support vaxpy level 0/2 api.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_3510/kernel_operator_vec_ternary_scalar_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_vec_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TERNARY_SCALAR_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_VEC_TERNARY_SCALAR_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_TERNARY_SCALAR_IMPL_H
#include "kernel_operator_common_impl.h"
#include "../kernel_utils.h"
#include "../../../include/basic_api/kernel_operator_sys_var_intf.h"
#include "kernel_operator_vec_template_impl.h"
#include "../../../include/basic_api/reg_compute/kernel_reg_compute_intf.h"

namespace AscendC {
namespace Internal {
template <auto func, typename T, typename U, typename RegT, typename RegU>
__simd_vf__ inline void VecAxpyLevel2VFImpl(__ubuf__ T* dst, __ubuf__ U* src, U scalarValue, const uint32_t calCount)
{
    RegU srcReg;
    RegT dstReg;
    uint32_t count = static_cast<uint32_t>(calCount);
    Reg::MaskReg mask;
    constexpr uint32_t repeatStride = static_cast<uint32_t>(GetVecLen() / sizeof(T) * RegT::trait.REG_NUM);
    uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, repeatStride));
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<T, RegT::trait>(count);
        Reg::LoadAlign(srcReg, src + i * repeatStride);
        Reg::LoadAlign(dstReg, dst + i * repeatStride);
        func(dstReg, srcReg, scalarValue, mask);
        Reg::StoreAlign(dst + i * repeatStride, dstReg, mask);
    }
}

template <auto func, typename T, typename U>
__aicore__ inline void VecAxpyLevel2ImplTemplate(
    __ubuf__ T* dst, __ubuf__ U* src, U scalarValue, const uint32_t calCount)
{
    if constexpr (SupportBytes<T, 8>()) {
        VecAxpyLevel2VFImpl<func, T, U, Reg::RegTensor<T, Reg::RegTraitNumTwo>, Reg::RegTensor<U, Reg::RegTraitNumTwo>>(
            dst, src, scalarValue, calCount);
    } else {
        VecAxpyLevel2VFImpl<func, T, U, Reg::RegTensor<T>, Reg::RegTensor<U>>(dst, src, scalarValue, calCount);
    }
}

/*
 * T: data type
 * func: Reg input/output function
 * isSetMask: basic api whether to set mask
 * isNormalMode: true: NormalMode, false: CounterMode
 * isMaskBitMode: true: mask bit mode, false: mask count mode
 */
template <auto func, bool isSetMask, bool isMaskBitMode, bool isNormalMode, typename T, typename U>
__simd_vf__ inline void VecAxpyVFImpl(
    __ubuf__ T* dst, __ubuf__ U* src, U scalarValue, const BasicAPIMaskStruct maskArrayStruct, const uint64_t maskCount,
    const uint8_t repeatTime, const UnaryRepeatParams repeatParams, __ubuf__ uint64_t* maskBuf)
{
    uint32_t count =
        VecMicroGetCount<isSetMask, isNormalMode, isMaskBitMode>(maskArrayStruct.maskArray, maskCount, maskBuf);
    uint16_t newRepeatTimes = 0;
    constexpr bool TUCompare = sizeof(T) > sizeof(U);
    using TT = typename Conditional<TUCompare, T, U>::type;
    newRepeatTimes = VecMicroGetRepeatTimes<TT, isNormalMode>(count, repeatTime);
    Reg::MaskReg maskReg;
    Reg::MaskReg maskRegDst;
    Reg::MaskReg maskRegSrc;
    if constexpr (isNormalMode) {
        maskReg = VecMicroGetMaskReg<TT, isSetMask, isNormalMode, isMaskBitMode>(maskBuf, count);
        maskRegSrc = maskReg;
        maskRegDst = maskReg;
        if constexpr (sizeof(U) == 2 * sizeof(T)) {
            Reg::MaskPack(maskRegDst, maskReg);
        } else if constexpr (sizeof(T) == 2 * sizeof(U)) {
            Reg::MaskPack(maskRegSrc, maskReg);
        }
    }
    constexpr uint8_t ElePerBlkT = GetDataBlockSizeInBytes() / sizeof(T);
    constexpr uint8_t ElePerBlkU = GetDataBlockSizeInBytes() / sizeof(U);
    for (uint16_t index = 0; index < newRepeatTimes; ++index) {
        if constexpr (!isNormalMode) {
            maskReg = VecMicroGetMaskReg<TT, isSetMask, isNormalMode, isMaskBitMode>(maskBuf, count);
            maskRegSrc = maskReg;
            maskRegDst = maskReg;
            if constexpr (sizeof(U) == 2 * sizeof(T)) {
                Reg::MaskPack(maskRegDst, maskReg);
            } else if constexpr (sizeof(T) == 2 * sizeof(U)) {
                Reg::MaskPack(maskRegSrc, maskReg);
            }
        }
        Reg::RegTensor<T> dstVreg;
        Reg::RegTensor<U> srcVreg;
#ifndef NO_OVERLAP_IN_MULTI_REPEAT
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
#endif
        Reg::LoadAlign<U, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            srcVreg, src + index * repeatParams.srcRepStride * ElePerBlkU, repeatParams.srcBlkStride, maskRegSrc);

        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            dstVreg, dst + index * repeatParams.dstRepStride * ElePerBlkT, repeatParams.dstBlkStride, maskRegDst);
        func(dstVreg, srcVreg, scalarValue, maskReg);
        Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            dst + index * repeatParams.dstRepStride * ElePerBlkT, dstVreg, repeatParams.dstBlkStride, maskRegDst);
    }
}

template <auto func, bool isSetMask, bool isMaskBitMode, typename T, typename U>
__aicore__ inline void VecAxpyImplTemplate(
    __ubuf__ T* dst, __ubuf__ U* src, U scalarValue, const uint64_t maskArray[], const uint64_t maskCount,
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
    constexpr bool TUCompare = sizeof(T) > sizeof(U);
    using TT = typename Conditional<TUCompare, T, U>::type;
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
    __ubuf__ uint64_t* maskBuf = nullptr;

    if (Internal::IsCounterMode()) {
        if constexpr (!isSetMask) {
            maskBuf =
                AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 2); // maskReg 256bit PK-> 128bit
        }
        VecAxpyVFImpl<func, isSetMask, isMaskBitMode, false, T, U>(
            dst, src, scalarValue, maskArrayStruct, maskCount, repeatTime, repeatParams, maskBuf);
        if constexpr (!isSetMask) {
            AscendCUtils::FreeTemporaryBuffer<uint64_t>(maskBuf);
        }
    } else {
        if constexpr (isMaskBitMode && isSetMask) {
            SetVectorMask<TT>(maskArray[1], maskArray[0]); // set mask to SPR.MASK, movp in VF
        }
        VecAxpyVFImpl<func, isSetMask, isMaskBitMode, true, T, U>(
            dst, src, scalarValue, maskArrayStruct, maskCount, repeatTime, repeatParams, maskBuf);
    }
}
} // namespace Internal

namespace RegAxpy {
namespace CastParam {
constexpr Reg::CastTrait half2floatTrait = {
    Reg::RegLayout::ZERO, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::UNKNOWN};
}
template <typename T, typename U, typename RegT, typename RegU>
__simd_callee__ inline void Axpy(RegT& dstReg, RegU& srcReg, U scalarValue, Reg::MaskReg& mask)
{
    if constexpr (SupportType<
                      Tuple<T, U>, Tuple<half, half>, Tuple<float, float>, Tuple<uint64_t, uint64_t>,
                      Tuple<int64_t, int64_t>>()) {
        Reg::Axpy(dstReg, srcReg, scalarValue, mask);
    } else if constexpr (SupportType<Tuple<T, U>, Tuple<bfloat16_t, bfloat16_t>>()) {
        RegT tmpReg;
        Reg::Duplicate(tmpReg, scalarValue, mask);
        Reg::Mul(tmpReg, srcReg, tmpReg, mask);
        Reg::Add(dstReg, tmpReg, dstReg, mask);
    } else if constexpr (SupportType<Tuple<T, U>, Tuple<float, half>>()) {
        RegU tmpReg;
        RegT cvtReg;
        Reg::UnPack<uint32_t, uint16_t, AscendC::Reg::HighLowPart::LOWEST>(
            (Reg::RegTensor<uint32_t>&)tmpReg, (Reg::RegTensor<uint16_t>&)srcReg);
        Reg::Cast<float, half, CastParam::half2floatTrait>(cvtReg, tmpReg, mask);
        Reg::Muls(cvtReg, cvtReg, static_cast<T>(scalarValue), mask);
        Reg::Add(dstReg, cvtReg, dstReg, mask);
    }
}
} // namespace RegAxpy

// Axpy::Level 0
template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void AxpyImpl(
    __ubuf__ T* dst, __ubuf__ U* src, const U& scalarValue, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<
            Tuple<T, U>, Tuple<half, half>, Tuple<float, float>, Tuple<bfloat16_t, bfloat16_t>, Tuple<float, half>>(),
        "current data type is not supported on current device!");
    constexpr auto func = RegAxpy::Axpy<T, U, Reg::RegTensor<T>, Reg::RegTensor<U>>;
    Internal::VecAxpyImplTemplate<func, isSetMask, true>(dst, src, scalarValue, mask, 0, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void AxpyImpl(
    __ubuf__ T* dst, __ubuf__ U* src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<
            Tuple<T, U>, Tuple<half, half>, Tuple<float, float>, Tuple<bfloat16_t, bfloat16_t>, Tuple<float, half>>(),
        "current data type is not supported on current device!");
    constexpr auto func = RegAxpy::Axpy<T, U, Reg::RegTensor<T>, Reg::RegTensor<U>>;
    Internal::VecAxpyImplTemplate<func, isSetMask, false>(
        dst, src, scalarValue, nullptr, mask, repeatTime, repeatParams);
}

// Axpy::Level 2
template <typename T, typename U>
__aicore__ inline void AxpyImpl(__ubuf__ T* dst, __ubuf__ U* src, const U& scalarValue, const int32_t& calCount)
{
    static_assert(
        SupportType<
            Tuple<T, U>, Tuple<half, half>, Tuple<float, float>, Tuple<bfloat16_t, bfloat16_t>, Tuple<float, half>,
            Tuple<uint64_t, uint64_t>, Tuple<int64_t, int64_t>>(),
        "current data type is not supported on current device!");
    if constexpr (SupportBytes<T, 8>()) {
        constexpr auto func =
            RegAxpy::Axpy<T, U, Reg::RegTensor<T, Reg::RegTraitNumTwo>, Reg::RegTensor<U, Reg::RegTraitNumTwo>>;
        Internal::VecAxpyLevel2ImplTemplate<func, T>(dst, src, scalarValue, calCount);
    } else {
        constexpr auto func = RegAxpy::Axpy<T, U, Reg::RegTensor<T>, Reg::RegTensor<U>>;
        Internal::VecAxpyLevel2ImplTemplate<func, T>(dst, src, scalarValue, calCount);
    }
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_TERNARY_SCALAR_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TERNARY_SCALAR_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_TERNARY_SCALAR_IMPL_H__
#endif
