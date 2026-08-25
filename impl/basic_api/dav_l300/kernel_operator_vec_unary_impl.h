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
 * \file kernel_operator_vec_unary_impl.h
 * \brief AscendC l300 support vector unary api.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_l300/kernel_operator_vec_unary_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_vec_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_UNARY_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_UNARY_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_UNARY_IMPL_H
#include "../kernel_utils.h"
#include "kernel_operator_common_impl.h"
#include "../../../include/basic_api/kernel_struct_unary.h"

namespace AscendC {
namespace Internal {
template <auto func, typename T, typename RegType>
__simd_vf__ inline void VecUnaryLevel2VFImpl(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t count)
{
    RegType srcReg;
    RegType dstReg;
    uint32_t sreg = static_cast<uint32_t>(count);
    Reg::MaskReg mask;
    constexpr uint32_t repeatStride = static_cast<uint32_t>(GetVecLen() / sizeof(T) * RegType::trait.REG_NUM);
    uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(sreg, repeatStride));
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<T, RegType::trait>(sreg);
        Reg::DataCopy(srcReg, src + i * repeatStride);
        func(dstReg, srcReg, mask);
        Reg::DataCopy(dst + i * repeatStride, dstReg, mask);
    }
}

template <auto func, typename T>
__aicore__ inline void VecUnaryLevel2ImplTemplate(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t count)
{
    if constexpr (SupportBytes<T, 8>()) {
        VecUnaryLevel2VFImpl<func, T, Reg::RegTensor<T, Reg::RegTraitNumTwo>>(dst, src, count);
    } else {
        VecUnaryLevel2VFImpl<func, T, Reg::RegTensor<T>>(dst, src, count);
    }
}

template <auto func, bool isSetMask, bool isMaskBitMode, bool isNormalMode, typename T>
__simd_vf__ inline void VecUnaryLevel0VFImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const BasicAPIMaskStruct maskArrayStruct, const uint64_t maskCount,
    const uint8_t repeatTimes, const UnaryRepeatParams repeatParams, __ubuf__ uint64_t* maskBuf)
{
    uint32_t count =
        VecMicroGetCount<isSetMask, isNormalMode, isMaskBitMode>(maskArrayStruct.maskArray, maskCount, maskBuf);
    uint16_t newRepeatTimes = 0;
    newRepeatTimes = VecMicroGetRepeatTimes<T, isNormalMode>(count, repeatTimes);
    Reg::MaskReg maskReg;
    if constexpr (isNormalMode) {
        maskReg = VecMicroGetMaskReg<T, isSetMask, isNormalMode, isMaskBitMode>(maskBuf, count);
    }
    constexpr uint8_t ElePerBlkT = GetDataBlockSizeInBytes() / sizeof(T);
    for (uint16_t index = 0; index < newRepeatTimes; ++index) {
        if constexpr (!isNormalMode) {
            maskReg = VecMicroGetMaskReg<T, isSetMask, isNormalMode, isMaskBitMode>(maskBuf, count);
        }
        Reg::RegTensor<T> dstVreg;
        Reg::RegTensor<T> srcVreg;
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
        Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            srcVreg, src + index * repeatParams.srcRepStride * ElePerBlkT, repeatParams.srcBlkStride, maskReg);
        func(dstVreg, srcVreg, maskReg);
        Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            dst + index * repeatParams.dstRepStride * ElePerBlkT, dstVreg, repeatParams.dstBlkStride, maskReg);
    }
}

template <auto func, bool isSetMask, bool isMaskBitMode, typename T>
__aicore__ inline void VecUnaryLevel0Template(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t maskArray[], const uint64_t maskCount, const uint8_t repeatTimes,
    const UnaryRepeatParams repeatParams)
{
    if constexpr (isMaskBitMode) {
        ASCENDC_ASSERT(maskCount == 0, "maskCount must be 0 when isMaskBitMode is true.");
    } else {
        ASCENDC_ASSERT(maskArray == nullptr, "maskArray must be nullptr when isMaskBitMode is false.");
    }
    __ubuf__ uint64_t* maskBuf = nullptr;
    uint16_t maskArraySize = (maskArray == nullptr) ? 0 : MASK_ARRAY_SIZE;
    BasicAPIMaskStruct maskArrayStruct;
    for (uint16_t i = 0; i < maskArraySize; i++) {
        maskArrayStruct.maskArray[i] = maskArray[i];
    }
    if (Internal::IsCounterMode()) {
        if constexpr (!isSetMask) {
            maskBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(TMP_UB_OFFSET, 2); // maskReg 256bit PK-> 128bit
        }
        VecUnaryLevel0VFImpl<func, isSetMask, isMaskBitMode, false, T>(
            dst, src, maskArrayStruct, maskCount, repeatTimes, repeatParams, maskBuf);
        if constexpr (!isSetMask) {
            AscendCUtils::FreeTemporaryBuffer<uint64_t>(maskBuf);
        }
    } else {
        if constexpr (isMaskBitMode) {
            if constexpr (SupportBytes<T, 1>()) {
                ASCENDC_ASSERT(isSetMask, "mask must be set when sizeof(T) is 1.");
                auto eventIDV2S = GetTPipePtr()->FetchEventID(HardEvent::V_S);
                SetFlag<HardEvent::V_S>(eventIDV2S);
                WaitFlag<HardEvent::V_S>(eventIDV2S);
                maskBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(TMP_UB_OFFSET, 4);
                maskBuf[0] = maskArray[0];
                maskBuf[1] = maskArray[1];
                maskBuf[2] = maskArray[2];
                maskBuf[3] = maskArray[3];
                auto eventIDS2V = GetTPipePtr()->FetchEventID(HardEvent::S_V);
                SetFlag<HardEvent::S_V>(eventIDS2V);
                WaitFlag<HardEvent::S_V>(eventIDS2V);
            } else if constexpr (isSetMask) {
                SetVectorMask<T>(maskArray[1], maskArray[0]); // set mask to SPR.MASK, movp in VF
            }
        }
        // when isSetMask is false, normal mode, maskBuf = nullptr, not support B8
        VecUnaryLevel0VFImpl<func, isSetMask, isMaskBitMode, true, T>(
            dst, src, maskArrayStruct, maskCount, repeatTimes, repeatParams, maskBuf);
        if constexpr (isMaskBitMode && SupportBytes<T, 1>()) {
            AscendC::AscendCUtils::FreeTemporaryBuffer<uint64_t>(maskBuf);
        }
    }
}
} // namespace Internal

// Macros for level-0 api with type not support
#define UNARY_VEC_NORMAL_NOT_SUPPORT(FUNC_NAME)                                                      \
    template <typename T, bool isSetMask = true>                                                     \
    __aicore__ inline void FUNC_NAME(                                                                \
        __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask, const uint8_t repeatTimes,            \
        const UnaryRepeatParams& reapeatParams)                                                      \
    {                                                                                                \
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); }); \
    }

#define UNARY_VEC_BITWISE_NOT_SUPPORT(FUNC_NAME)                                                     \
    template <typename T, bool isSetMask = true>                                                     \
    __aicore__ inline void FUNC_NAME(                                                                \
        __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask[2], const uint8_t repeatTimes,         \
        const UnaryRepeatParams& reapeatParams)                                                      \
    {                                                                                                \
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); }); \
    }

// Macros for level-2 api with type not support
#define UNARY_VEC_COUNTER_NOT_SUPPORT(FUNC_NAME)                                                     \
    template <typename T>                                                                            \
    __aicore__ inline void FUNC_NAME(__ubuf__ T* dst, __ubuf__ T* src, const int32_t& calCount)      \
    {                                                                                                \
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); }); \
    }

// Macros for level-0 api
// for normal op
#define UNARY_VEC_NORMAL_IMPL(FUNC_NAME, OP_NAME, DATA_TYPE)                                              \
    template <typename T = DATA_TYPE, bool isSetMask = true>                                              \
    __aicore__ inline void FUNC_NAME(                                                                     \
        __ubuf__ DATA_TYPE* dst, __ubuf__ DATA_TYPE* src, const uint64_t mask, const uint8_t repeatTimes, \
        const UnaryRepeatParams& repeatParams)                                                            \
    {                                                                                                     \
        __VEC_SCOPE__                                                                                     \
        {                                                                                                 \
            RegTensor<DATA_TYPE> vreg0;                                                                   \
            RegTensor<DATA_TYPE> vreg1;                                                                   \
            uint32_t sreg = (uint32_t)mask;                                                               \
            MaskReg preg = CreatePredicate<DATA_TYPE>(sreg);                                              \
            uint32_t strideConfig0 = static_cast<uint32_t>(repeatParams.srcBlkStride);                    \
            uint32_t repeatStrideConfig0 = static_cast<uint32_t>(repeatParams.srcRepStride);              \
            uint32_t strideConfig1 = static_cast<uint32_t>(repeatParams.dstBlkStride);                    \
            uint32_t repeatStrideConfig1 = static_cast<uint32_t>(repeatParams.dstRepStride);              \
            for (uint16_t i = 0; i < (uint16_t)repeatTimes; ++i) {                                        \
                DataCopy<DATA_TYPE, PostLiteral::POST_MODE_UPDATE>(                                       \
                    vreg0, src, strideConfig0, repeatStrideConfig0, preg);                                \
                OP_NAME(vreg1, vreg0, preg);                                                              \
                DataCopy<DATA_TYPE, PostLiteral::POST_MODE_UPDATE>(                                       \
                    dst, vreg1, strideConfig1, repeatStrideConfig1, preg);                                \
            }                                                                                             \
        }                                                                                                 \
    }

// for bit-wise op
#define UNARY_VEC_BITWISE_IMPL(FUNC_NAME, OP_NAME, DATA_TYPE)                                                \
    template <typename T = DATA_TYPE, bool isSetMask = true>                                                 \
    __aicore__ inline void FUNC_NAME(                                                                        \
        __ubuf__ DATA_TYPE* dst, __ubuf__ DATA_TYPE* src, const uint64_t mask[2], const uint8_t repeatTimes, \
        const UnaryRepeatParams& repeatParams)                                                               \
    {                                                                                                        \
        if constexpr (isSetMask) {                                                                           \
            SetVectorMask<DATA_TYPE>(mask[1], mask[0]);                                                      \
        }                                                                                                    \
        __VEC_SCOPE__                                                                                        \
        {                                                                                                    \
            RegTensor<DATA_TYPE> vreg0;                                                                      \
            RegTensor<DATA_TYPE> vreg1;                                                                      \
            MaskReg preg = MovePredicate<DATA_TYPE>();                                                       \
            uint32_t strideConfig0 = static_cast<uint32_t>(repeatParams.srcBlkStride);                       \
            uint32_t repeatStrideConfig0 = static_cast<uint32_t>(repeatParams.srcRepStride);                 \
            uint32_t strideConfig1 = static_cast<uint32_t>(repeatParams.dstBlkStride);                       \
            uint32_t repeatStrideConfig1 = static_cast<uint32_t>(repeatParams.dstRepStride);                 \
            for (uint16_t i = 0; i < (uint16_t)repeatTimes; ++i) {                                           \
                DataCopy<DATA_TYPE, PostLiteral::POST_MODE_UPDATE>(                                          \
                    vreg0, src, strideConfig0, repeatStrideConfig0, preg);                                   \
                OP_NAME(vreg1, vreg0, preg);                                                                 \
                DataCopy<DATA_TYPE, PostLiteral::POST_MODE_UPDATE>(                                          \
                    dst, vreg1, strideConfig1, repeatStrideConfig1, preg);                                   \
            }                                                                                                \
        }                                                                                                    \
    }

// for counter level-2 op
#define UNARY_VEC_COUNTER_IMPL(FUNC_NAME, OP_NAME, DATA_TYPE)                                                   \
    __aicore__ inline void FUNC_NAME(__ubuf__ DATA_TYPE* dst, __ubuf__ DATA_TYPE* src, const int32_t& calCount) \
    {                                                                                                           \
        __VEC_SCOPE__                                                                                           \
        {                                                                                                       \
            RegTensor<DATA_TYPE> vreg0;                                                                         \
            RegTensor<DATA_TYPE> vreg1;                                                                         \
            uint32_t sreg = (uint32_t)calCount;                                                                 \
            MaskReg preg;                                                                                       \
            uint32_t sregLower = (uint32_t)(VECTOR_REG_WIDTH / sizeof(DATA_TYPE));                              \
            uint16_t repeatTimes = CeilDivision(calCount, sregLower);                                           \
            for (uint16_t i = 0; i < (uint16_t)repeatTimes; ++i) {                                              \
                preg = CreatePredicate<DATA_TYPE>(sreg);                                                        \
                DataCopy(vreg0, src, i* sregLower);                                                             \
                OP_NAME(vreg1, vreg0, preg);                                                                    \
                DataCopy(dst, vreg1, i* sregLower, preg);                                                       \
            }                                                                                                   \
        }                                                                                                       \
    }

/* **************************************************************************************************
 * Abs                                             *
 * ************************************************************************************************* */
// Abs::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void AbsImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(
        (SupportType<T, half, int16_t, float, int32_t>()), "current data type is not supported on current device!");
    constexpr auto func = Reg::Abs<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecUnaryLevel0Template<func, isSetMask, true>(dst, src, mask, 0, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void AbsImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert(
        (SupportType<T, half, int16_t, float, int32_t>()), "current data type is not supported on current device!");
    constexpr auto func = Reg::Abs<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecUnaryLevel0Template<func, isSetMask, false>(dst, src, nullptr, mask, repeatTime, repeatParams);
}

// Abs::Level 2
UNARY_VEC_COUNTER_NOT_SUPPORT(AbsImpl);
UNARY_VEC_COUNTER_IMPL(AbsImpl, Abs, int8_t);
UNARY_VEC_COUNTER_IMPL(AbsImpl, Abs, half);
UNARY_VEC_COUNTER_IMPL(AbsImpl, Abs, float);
UNARY_VEC_COUNTER_IMPL(AbsImpl, Abs, int16_t);
UNARY_VEC_COUNTER_IMPL(AbsImpl, Abs, int32_t);

/* **************************************************************************************************
 * Relu                                             *
 * ************************************************************************************************* */
// Relu::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void ReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert((SupportType<T, half, float, int32_t>()), "current data type is not supported on current device!");
    constexpr auto func = Reg::Relu<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecUnaryLevel0Template<func, isSetMask, true>(dst, src, mask, 0, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void ReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert((SupportType<T, half, float, int32_t>()), "current data type is not supported on current device!");
    constexpr auto func = Reg::Relu<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecUnaryLevel0Template<func, isSetMask, false>(dst, src, nullptr, mask, repeatTime, repeatParams);
}

// Relu::Level 2
UNARY_VEC_COUNTER_NOT_SUPPORT(ReluImpl);
UNARY_VEC_COUNTER_IMPL(ReluImpl, Relu, half);
UNARY_VEC_COUNTER_IMPL(ReluImpl, Relu, float);
UNARY_VEC_COUNTER_IMPL(ReluImpl, Relu, int32_t);

/* **************************************************************************************************
 * Exp                                             *
 * ************************************************************************************************* */
// Exp::Level 0
// bit mode
template <typename T, bool isSetMask = true>
__aicore__ inline void ExpImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask[], const uint8_t repeatTimes,
    const UnaryRepeatParams& repeatParams)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    constexpr auto func = Reg::Exp<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecUnaryLevel0Template<func, isSetMask, true>(dst, src, mask, 0, repeatTimes, repeatParams);
}
// normal mode
template <typename T, bool isSetMask = true>
__aicore__ inline void ExpImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask, const uint8_t repeatTimes,
    const UnaryRepeatParams& repeatParams)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    constexpr auto func = Reg::Exp<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecUnaryLevel0Template<func, isSetMask, false>(dst, src, nullptr, mask, repeatTimes, repeatParams);
}

// Exp::Level 2
UNARY_VEC_COUNTER_NOT_SUPPORT(ExpImpl);
UNARY_VEC_COUNTER_IMPL(ExpImpl, Exp, half);
UNARY_VEC_COUNTER_IMPL(ExpImpl, Exp, float);

/* **************************************************************************************************
 * Sqrt                                             *
 * ************************************************************************************************* */
// Sqrt::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void SqrtImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    constexpr auto func = Reg::Sqrt<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecUnaryLevel0Template<func, isSetMask, true>(dst, src, mask, 0, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void SqrtImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    constexpr auto func = Reg::Sqrt<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecUnaryLevel0Template<func, isSetMask, false>(dst, src, nullptr, mask, repeatTime, repeatParams);
}

// Sqrt::Level 2
UNARY_VEC_COUNTER_NOT_SUPPORT(SqrtImpl);
UNARY_VEC_COUNTER_IMPL(SqrtImpl, Sqrt, half);
UNARY_VEC_COUNTER_IMPL(SqrtImpl, Sqrt, float);

/* **************************************************************************************************
 * Rsqrt                                             *
 * ************************************************************************************************* */
// Rsqrt::Level 0
namespace RegRsqrt {
template <typename T, typename RegT, bool precisionMode = false>
__simd_callee__ inline void Rsqrt(RegT& dstReg, RegT& srcReg, Reg::MaskReg& mask)
{
    Reg::MaskReg cmpMask;
    Reg::Duplicate(dstReg, static_cast<T>(1.0f), mask);
    Reg::CompareScalar<T, CMPMODE::LT>(cmpMask, srcReg, static_cast<T>(0.0f), mask);
    if constexpr (!precisionMode) {
        Reg::Sqrt(srcReg, srcReg, mask);
        Reg::Div(dstReg, dstReg, srcReg, mask);
        Reg::Select(dstReg, srcReg, dstReg, cmpMask);
    } else {
        if constexpr (SupportType<T, half>()) {
            static constexpr AscendC::Reg::SqrtSpecificMode SqrtMode = {
                Reg::MaskMergeMode::ZEROING, false, SqrtAlgo::PRECISION_1ULP_FTZ_FALSE};
            Reg::Sqrt<T, &SqrtMode>(srcReg, srcReg, mask);
            static constexpr AscendC::Reg::DivSpecificMode divMode = {
                Reg::MaskMergeMode::ZEROING, false, DivAlgo::PRECISION_1ULP_FTZ_FALSE};
            Reg::Div<T, &divMode>(dstReg, dstReg, srcReg, mask);
        } else {
            static constexpr AscendC::Reg::SqrtSpecificMode SqrtMode = {
                Reg::MaskMergeMode::ZEROING, false, SqrtAlgo::PRECISION_0ULP_FTZ_FALSE};
            Reg::Sqrt<T, &SqrtMode>(srcReg, srcReg, mask);
            static constexpr AscendC::Reg::DivSpecificMode divMode = {
                Reg::MaskMergeMode::ZEROING, false, DivAlgo::PRECISION_0ULP_FTZ_FALSE};
            Reg::Div<T, &divMode>(dstReg, dstReg, srcReg, mask);
        }
    }
}
} // namespace RegRsqrt

template <typename T, bool isSetMask = true, const RsqrtConfig& config = DEFAULT_RSQRT_CONFIG>
__aicore__ inline void RsqrtImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    if constexpr (config.algo == RsqrtAlgo::INTRINSIC || config.algo == RsqrtAlgo::PRECISION_1ULP_FTZ_TRUE) {
        constexpr auto func = RegRsqrt::Rsqrt<T, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel0Template<func, isSetMask, true>(dst, src, mask, 0, repeatTime, repeatParams);
    } else if constexpr (
        config.algo == RsqrtAlgo::FAST_INVERSE || config.algo == RsqrtAlgo::PRECISION_0ULP_FTZ_FALSE ||
        config.algo == RsqrtAlgo::PRECISION_1ULP_FTZ_FALSE) {
        constexpr auto func = RegRsqrt::Rsqrt<T, Reg::RegTensor<T>, true>;
        Internal::VecUnaryLevel0Template<func, isSetMask, true>(dst, src, mask, 0, repeatTime, repeatParams);
    }
}

template <typename T, bool isSetMask = true, const RsqrtConfig& config = DEFAULT_RSQRT_CONFIG>
__aicore__ inline void RsqrtImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    if constexpr (config.algo == RsqrtAlgo::INTRINSIC || config.algo == RsqrtAlgo::PRECISION_1ULP_FTZ_TRUE) {
        constexpr auto func = RegRsqrt::Rsqrt<T, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel0Template<func, isSetMask, false>(dst, src, nullptr, mask, repeatTime, repeatParams);
    } else if constexpr (
        config.algo == RsqrtAlgo::FAST_INVERSE || config.algo == RsqrtAlgo::PRECISION_0ULP_FTZ_FALSE ||
        config.algo == RsqrtAlgo::PRECISION_1ULP_FTZ_FALSE) {
        constexpr auto func = RegRsqrt::Rsqrt<T, Reg::RegTensor<T>, true>;
        Internal::VecUnaryLevel0Template<func, isSetMask, false>(dst, src, nullptr, mask, repeatTime, repeatParams);
    }
}
// Rsqrt::Level 2
UNARY_VEC_COUNTER_NOT_SUPPORT(RsqrtImpl);
UNARY_VEC_COUNTER_IMPL(RsqrtImpl, Rsqrt, half);
UNARY_VEC_COUNTER_IMPL(RsqrtImpl, Rsqrt, float);

/* **************************************************************************************************
 * Rec                                             *
 * ************************************************************************************************* */
// Rec::Level 0
UNARY_VEC_NORMAL_NOT_SUPPORT(ReciprocalImpl);
UNARY_VEC_BITWISE_NOT_SUPPORT(ReciprocalImpl);
// normal mode
UNARY_VEC_NORMAL_IMPL(ReciprocalImpl, Rec, half);
UNARY_VEC_NORMAL_IMPL(ReciprocalImpl, Rec, float);
// bit mode
UNARY_VEC_BITWISE_IMPL(ReciprocalImpl, Rec, half);
UNARY_VEC_BITWISE_IMPL(ReciprocalImpl, Rec, float);
// Rec::Level 2
UNARY_VEC_COUNTER_NOT_SUPPORT(ReciprocalImpl);
UNARY_VEC_COUNTER_IMPL(ReciprocalImpl, Rec, half);
UNARY_VEC_COUNTER_IMPL(ReciprocalImpl, Rec, float);

/* **************************************************************************************************
 * Ln                                             *
 * ************************************************************************************************* */
// Ln::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void LnImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    constexpr auto func = Reg::Ln<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecUnaryLevel0Template<func, isSetMask, true>(dst, src, mask, 0, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void LnImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    constexpr auto func = Reg::Ln<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecUnaryLevel0Template<func, isSetMask, false>(dst, src, nullptr, mask, repeatTime, repeatParams);
}

// Ln::Level 2
template <typename T>
__aicore__ inline void LnImpl(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t count)
{
    static_assert((SupportType<T, half, float>()), "current data type is not supported on current device!");
    constexpr auto func = Reg::Ln<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
}

/* **************************************************************************************************
 * Not                                             *
 * ************************************************************************************************* */
// Not::Level 0
UNARY_VEC_NORMAL_NOT_SUPPORT(NotImpl);
UNARY_VEC_BITWISE_NOT_SUPPORT(NotImpl);
// normal mode
UNARY_VEC_NORMAL_IMPL(NotImpl, Not, uint8_t)
UNARY_VEC_NORMAL_IMPL(NotImpl, Not, int8_t)
UNARY_VEC_NORMAL_IMPL(NotImpl, Not, uint16_t);
UNARY_VEC_NORMAL_IMPL(NotImpl, Not, int16_t);
UNARY_VEC_NORMAL_IMPL(NotImpl, Not, half);
UNARY_VEC_NORMAL_IMPL(NotImpl, Not, float);
UNARY_VEC_NORMAL_IMPL(NotImpl, Not, uint32_t);
UNARY_VEC_NORMAL_IMPL(NotImpl, Not, int32_t);
// bit mode
UNARY_VEC_BITWISE_IMPL(NotImpl, Not, uint16_t);
UNARY_VEC_BITWISE_IMPL(NotImpl, Not, int16_t);
UNARY_VEC_BITWISE_IMPL(NotImpl, Not, half);
UNARY_VEC_BITWISE_IMPL(NotImpl, Not, float);
UNARY_VEC_BITWISE_IMPL(NotImpl, Not, uint32_t);
UNARY_VEC_BITWISE_IMPL(NotImpl, Not, int32_t);
// Not::Level 2
UNARY_VEC_COUNTER_NOT_SUPPORT(NotImpl);
UNARY_VEC_COUNTER_IMPL(NotImpl, Not, uint8_t);
UNARY_VEC_COUNTER_IMPL(NotImpl, Not, int8_t);
UNARY_VEC_COUNTER_IMPL(NotImpl, Not, uint16_t);
UNARY_VEC_COUNTER_IMPL(NotImpl, Not, int16_t);
UNARY_VEC_COUNTER_IMPL(NotImpl, Not, half);
UNARY_VEC_COUNTER_IMPL(NotImpl, Not, float);
UNARY_VEC_COUNTER_IMPL(NotImpl, Not, uint32_t);
UNARY_VEC_COUNTER_IMPL(NotImpl, Not, int32_t);

/* **************************************************************************************************
 * Neg                                            *
 * ************************************************************************************************* */
// Neg::Level 2
template <typename T>
__aicore__ inline void NegImpl(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t count)
{
    static_assert(
        (SupportType<T, int8_t, int16_t, int32_t, half, float>()),
        "current data type is not supported on current device!");
    if constexpr (SupportBytes<T, 8>()) {
        constexpr auto func = Reg::Neg<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumTwo>>;
        Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
    } else {
        constexpr auto func = Reg::Neg<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
        Internal::VecUnaryLevel2ImplTemplate<func, T>(dst, src, count);
    }
}

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_UNARY_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_UNARY_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_UNARY_IMPL_H__
#endif
