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
 * \file kernel_operator_vec_binary_continuous_impl.h
 * \brief AscendC l311 support vec binary continuous data api.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_l311/kernel_operator_vec_binary_continuous_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_vec_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H
#include "../kernel_utils.h"
#include "kernel_operator_common_impl.h"

namespace AscendC {

// for Level 2 binary op
#define BINARY_OP_CONTINUOUS_IMPL_NOT_SUPPORT(FUNC_NAME)                                                        \
    template <typename T>                                                                                       \
    __aicore__ inline void FUNC_NAME(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& count) \
    {                                                                                                           \
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });            \
    }

// for Level 2 binary op
#define BINARY_OP_CONTINUOUS_IMPL(FUNC_NAME, OP_NAME, DataType)                                         \
    __aicore__ inline void FUNC_NAME(                                                                   \
        __ubuf__ DataType* dst, __ubuf__ DataType* src0, __ubuf__ DataType* src1, const int32_t& count) \
    {                                                                                                   \
        __VEC_SCOPE__                                                                                   \
        {                                                                                               \
            RegTensor<DataType> vreg0;                                                                  \
            RegTensor<DataType> vreg1;                                                                  \
            RegTensor<DataType> vreg2;                                                                  \
            uint32_t sreg = (uint32_t)count;                                                            \
            MaskReg preg;                                                                               \
            uint32_t sregLower = (uint32_t)(VECTOR_REG_WIDTH / sizeof(DataType));                       \
            uint16_t repeatTime = CeilDivision(count, sregLower);                                       \
            for (uint16_t i = 0; i < (uint16_t)repeatTime; ++i) {                                       \
                preg = CreatePredicate<DataType>(sreg);                                                 \
                DataCopy(vreg0, src0, i* sregLower);                                                    \
                DataCopy(vreg1, src1, i* sregLower);                                                    \
                OP_NAME(vreg2, vreg0, vreg1, preg);                                                     \
                DataCopy(dst, vreg2, i* sregLower, preg);                                               \
            }                                                                                           \
        }                                                                                               \
    }

/* **************************************************************************************************
 * Add                                                                                              *
 * **************************************************************************************************/
// Add::Level 2
BINARY_OP_CONTINUOUS_IMPL_NOT_SUPPORT(AddImpl)
BINARY_OP_CONTINUOUS_IMPL(AddImpl, Add, int8_t)
BINARY_OP_CONTINUOUS_IMPL(AddImpl, Add, uint8_t)
BINARY_OP_CONTINUOUS_IMPL(AddImpl, Add, int16_t)
BINARY_OP_CONTINUOUS_IMPL(AddImpl, Add, uint16_t)
BINARY_OP_CONTINUOUS_IMPL(AddImpl, Add, int32_t)
BINARY_OP_CONTINUOUS_IMPL(AddImpl, Add, uint32_t)
BINARY_OP_CONTINUOUS_IMPL(AddImpl, Add, half)
BINARY_OP_CONTINUOUS_IMPL(AddImpl, Add, float)

/* **************************************************************************************************
 * Sub                                                                                              *
 * **************************************************************************************************/
// Sub::Level 2
BINARY_OP_CONTINUOUS_IMPL_NOT_SUPPORT(SubImpl)
BINARY_OP_CONTINUOUS_IMPL(SubImpl, Sub, int8_t)
BINARY_OP_CONTINUOUS_IMPL(SubImpl, Sub, uint8_t)
BINARY_OP_CONTINUOUS_IMPL(SubImpl, Sub, int16_t)
BINARY_OP_CONTINUOUS_IMPL(SubImpl, Sub, uint16_t)
BINARY_OP_CONTINUOUS_IMPL(SubImpl, Sub, int32_t)
BINARY_OP_CONTINUOUS_IMPL(SubImpl, Sub, uint32_t)
BINARY_OP_CONTINUOUS_IMPL(SubImpl, Sub, half)
BINARY_OP_CONTINUOUS_IMPL(SubImpl, Sub, float)

/* **************************************************************************************************
 * Mul                                                                                              *
 * **************************************************************************************************/
// Mul::Level 2
BINARY_OP_CONTINUOUS_IMPL_NOT_SUPPORT(MulImpl)
BINARY_OP_CONTINUOUS_IMPL(MulImpl, Mul, int8_t)
BINARY_OP_CONTINUOUS_IMPL(MulImpl, Mul, uint8_t)
BINARY_OP_CONTINUOUS_IMPL(MulImpl, Mul, int16_t)
BINARY_OP_CONTINUOUS_IMPL(MulImpl, Mul, uint16_t)
BINARY_OP_CONTINUOUS_IMPL(MulImpl, Mul, int32_t)
BINARY_OP_CONTINUOUS_IMPL(MulImpl, Mul, uint32_t)
BINARY_OP_CONTINUOUS_IMPL(MulImpl, Mul, half)
BINARY_OP_CONTINUOUS_IMPL(MulImpl, Mul, float)
/* **************************************************************************************************
 * Div                                                                                              *
 * **************************************************************************************************/
// Div::Level 2
BINARY_OP_CONTINUOUS_IMPL_NOT_SUPPORT(DivImpl)
BINARY_OP_CONTINUOUS_IMPL(DivImpl, Div, int16_t)
BINARY_OP_CONTINUOUS_IMPL(DivImpl, Div, uint16_t)
BINARY_OP_CONTINUOUS_IMPL(DivImpl, Div, int32_t)
BINARY_OP_CONTINUOUS_IMPL(DivImpl, Div, uint32_t)
BINARY_OP_CONTINUOUS_IMPL(DivImpl, Div, half)
BINARY_OP_CONTINUOUS_IMPL(DivImpl, Div, float)

/* **************************************************************************************************
 * Max                                                                                              *
 * **************************************************************************************************/
// Max::Level 2
BINARY_OP_CONTINUOUS_IMPL_NOT_SUPPORT(MaxImpl)
BINARY_OP_CONTINUOUS_IMPL(MaxImpl, Max, int16_t)
BINARY_OP_CONTINUOUS_IMPL(MaxImpl, Max, uint16_t)
BINARY_OP_CONTINUOUS_IMPL(MaxImpl, Max, int32_t)
BINARY_OP_CONTINUOUS_IMPL(MaxImpl, Max, uint32_t)
BINARY_OP_CONTINUOUS_IMPL(MaxImpl, Max, half)
BINARY_OP_CONTINUOUS_IMPL(MaxImpl, Max, float)
BINARY_OP_CONTINUOUS_IMPL(MaxImpl, Max, int8_t)
BINARY_OP_CONTINUOUS_IMPL(MaxImpl, Max, uint8_t)
/* **************************************************************************************************
 * Min                                                                                              *
 * **************************************************************************************************/
// Min::Level 2
BINARY_OP_CONTINUOUS_IMPL_NOT_SUPPORT(MinImpl)
BINARY_OP_CONTINUOUS_IMPL(MinImpl, Min, int8_t)
BINARY_OP_CONTINUOUS_IMPL(MinImpl, Min, uint8_t)
BINARY_OP_CONTINUOUS_IMPL(MinImpl, Min, int16_t)
BINARY_OP_CONTINUOUS_IMPL(MinImpl, Min, uint16_t)
BINARY_OP_CONTINUOUS_IMPL(MinImpl, Min, int32_t)
BINARY_OP_CONTINUOUS_IMPL(MinImpl, Min, uint32_t)
BINARY_OP_CONTINUOUS_IMPL(MinImpl, Min, half)
BINARY_OP_CONTINUOUS_IMPL(MinImpl, Min, float)
/* **************************************************************************************************
 * And                                                                                              *
 * **************************************************************************************************/
// And::Level 2
BINARY_OP_CONTINUOUS_IMPL_NOT_SUPPORT(AndImpl)
BINARY_OP_CONTINUOUS_IMPL(AndImpl, And, int8_t)
BINARY_OP_CONTINUOUS_IMPL(AndImpl, And, uint8_t)
BINARY_OP_CONTINUOUS_IMPL(AndImpl, And, int16_t)
BINARY_OP_CONTINUOUS_IMPL(AndImpl, And, uint16_t)
BINARY_OP_CONTINUOUS_IMPL(AndImpl, And, int32_t)
BINARY_OP_CONTINUOUS_IMPL(AndImpl, And, uint32_t)
/* **************************************************************************************************
 * Or                                                                                               *
 * **************************************************************************************************/
// Or::Level 2
BINARY_OP_CONTINUOUS_IMPL_NOT_SUPPORT(OrImpl)
BINARY_OP_CONTINUOUS_IMPL(OrImpl, Or, int8_t)
BINARY_OP_CONTINUOUS_IMPL(OrImpl, Or, uint8_t)
BINARY_OP_CONTINUOUS_IMPL(OrImpl, Or, int16_t)
BINARY_OP_CONTINUOUS_IMPL(OrImpl, Or, uint16_t)
BINARY_OP_CONTINUOUS_IMPL(OrImpl, Or, int32_t)
BINARY_OP_CONTINUOUS_IMPL(OrImpl, Or, uint32_t)

/* **************************************************************************************************
 * Prelu                                             *
 * ************************************************************************************************* */
// Prelu::Level 2
template <typename T>
__simd_vf__ inline void PreluImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint32_t calCount)
{
    static_assert(
        SupportType<T, half, float>(), "Failed to check dtype in Prelu, current api support "
                                       "dtype is half, float.");
    constexpr uint32_t sregLower = static_cast<uint32_t>(GetVecLen() / sizeof(T));
    const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, sregLower));
    uint32_t sreg = static_cast<uint32_t>(calCount);
    Reg::RegTensor<T> vDstReg0;
    Reg::RegTensor<T> vSrcReg0;
    Reg::RegTensor<T> vSrcReg1;
    Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<T>(sreg);
        Reg::LoadAlign(vSrcReg0, src0 + i * sregLower);
        Reg::LoadAlign(vSrcReg1, src1 + i * sregLower);
        Reg::Prelu(vDstReg0, vSrcReg0, vSrcReg1, mask);
        Reg::StoreAlign(dst + i * sregLower, vDstReg0, mask);
    }
}

/* **************************************************************************************************
 * FusedAbsSub                                            *
 * ************************************************************************************************* */
// FusedAbsSub::Level 2
template <typename T>
__simd_vf__ inline void FusedAbsSubImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint32_t calCount)
{
    static_assert(
        SupportType<T, half, float>(), "Failed to check dtype in FusedAbsSub, current api support "
                                       "dtype is src and dst both: half, float.");
    constexpr uint32_t sregLower = static_cast<uint32_t>(GetVecLen() / sizeof(T));
    const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, sregLower));
    uint32_t sreg = static_cast<uint32_t>(calCount);
    Reg::RegTensor<T> vDstReg0;
    Reg::RegTensor<T> vSrcReg0;
    Reg::RegTensor<T> vSrcReg1;
    Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<T>(sreg);
        Reg::LoadAlign(vSrcReg0, src0 + i * sregLower);
        Reg::LoadAlign(vSrcReg1, src1 + i * sregLower);
        Reg::FusedAbsSub(vDstReg0, vSrcReg0, vSrcReg1, mask);
        Reg::StoreAlign(dst + i * sregLower, vDstReg0, mask);
    }
}

/* **************************************************************************************************
 * FusedExpSub                                        *
 * ************************************************************************************************* */
// FusedExpSub::Level 2
template <typename T, typename U>
__simd_vf__ inline void FusedExpSubImpl(__ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, const uint32_t calCount)
{
    static_assert(
        SupportType<Tuple<T, U>, Tuple<float, half>, Tuple<float, float>>(),
        "Failed to check dtype in "
        "FusedExpSub, current api support dtype combination is src : half / float, dst: float.");
    constexpr uint32_t sregLower = static_cast<uint32_t>(GetVecLen() / sizeof(float));
    const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, sregLower));
    uint32_t sreg = static_cast<uint32_t>(calCount);
    Reg::RegTensor<T> vDstReg0;
    Reg::RegTensor<U> vSrcReg0;
    Reg::RegTensor<U> vSrcReg1;
    Reg::MaskReg mask;
    if constexpr (IsSameType<U, half>::value) {
        for (uint16_t i = 0; i < repeatTime; ++i) {
            mask = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<U, Reg::LoadDist::DIST_UNPACK_B16>(vSrcReg0, src0 + i * sregLower);
            Reg::LoadAlign<U, Reg::LoadDist::DIST_UNPACK_B16>(vSrcReg1, src1 + i * sregLower);
            Reg::SubImpl<U>(vSrcReg0, vSrcReg0, vSrcReg1, mask);
            Reg::CastImpl<T, U, Reg::RegLayout::ZERO, Reg::MaskMergeMode::ZEROING>(vDstReg0, vSrcReg0, mask);
            Reg::ExpImpl<T>(vDstReg0, vDstReg0, mask);
            Reg::StoreAlign(dst + i * sregLower, vDstReg0, mask);
        }
    } else if constexpr (IsSameType<U, float>::value) {
        for (uint16_t i = 0; i < repeatTime; ++i) {
            mask = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign(vSrcReg0, src0 + i * sregLower);
            Reg::LoadAlign(vSrcReg1, src1 + i * sregLower);
            Reg::FusedExpSub(vDstReg0, vSrcReg0, vSrcReg1, mask);
            Reg::StoreAlign(dst + i * sregLower, vDstReg0, mask);
        }
    }
}

template <typename T, typename RegType, auto func>
__simd_vf__ inline void BinaryContinuousImplTemplate(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t calCount)
{
    RegType src0Reg;
    RegType src1Reg;
    RegType dstReg;
    uint32_t sreg = static_cast<uint32_t>(calCount);
    Reg::MaskReg mask;
    constexpr uint32_t repeatStride = static_cast<uint32_t>(GetVecLen() / sizeof(T) * RegType::trait.REG_NUM);
    uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, repeatStride));
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<T, RegType::trait>(sreg);
        Reg::LoadAlign(src0Reg, src0 + i * repeatStride);
        Reg::LoadAlign(src1Reg, src1 + i * repeatStride);
        func(dstReg, src0Reg, src1Reg, mask);
        Reg::StoreAlign(dst + i * repeatStride, dstReg, mask);
    }
}

template <typename T, typename U, typename RegTypeT, typename RegTypeU, auto func>
__simd_vf__ inline void BinaryContinuousImplTemplate(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ U* src1, const int32_t calCount)
{
    RegTypeT src0Reg;
    RegTypeU src1Reg;
    RegTypeT dstReg;
    uint32_t sreg = static_cast<uint32_t>(calCount);
    Reg::MaskReg mask;
    constexpr uint32_t repeatStride = static_cast<uint32_t>(GetVecLen() / sizeof(T) * RegTypeT::trait.REG_NUM);
    uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, repeatStride));
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<T, RegTypeT::trait>(sreg);
        Reg::LoadAlign(src0Reg, src0 + i * repeatStride);
        Reg::LoadAlign(src1Reg, src1 + i * repeatStride);
        func(dstReg, src0Reg, src1Reg, mask);
        Reg::StoreAlign(dst + i * repeatStride, dstReg, mask);
    }
}

/* **************************************************************************************************
 * ShiftLeft                                             *
 * ************************************************************************************************* */
// ShiftLeft::Level 2
template <typename T, typename U>
__aicore__ inline void ShiftLeftImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ U* src1, const int32_t& calCount)
{
    static_assert(
        SupportType<
            Tuple<T, U>, Tuple<int32_t, int32_t>, Tuple<uint32_t, int32_t>, Tuple<int16_t, int16_t>,
            Tuple<uint16_t, int16_t>, Tuple<int8_t, int8_t>, Tuple<uint8_t, int8_t>>(),
        "Failed to check dtype in ShiftLeft, current api support dtype combination is  "
        "src0: int32_t, src1: int32_t; src0: uint32_t, src1: int32_t; src0: int16_t, "
        "src1: int16_t; src0: uint16_t, src1: int16_t; "
        "src0: int8_t, src1: int8_t; src0: uint8_t, src1: int8_t.");
    if constexpr (SupportBytes<T, 8>()) {
        BinaryContinuousImplTemplate<
            T, U, Reg::RegTensor<T, Reg::RegTraitNumTwo>, Reg::RegTensor<U, Reg::RegTraitNumTwo>,
            Reg::ShiftLeft<
                T, U, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumTwo>,
                Reg::RegTensor<U, Reg::RegTraitNumTwo>>>(dst, src0, src1, calCount);
    } else {
        BinaryContinuousImplTemplate<
            T, U, Reg::RegTensor<T>, Reg::RegTensor<U>,
            Reg::ShiftLeft<T, U, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>, Reg::RegTensor<U>>>(
            dst, src0, src1, calCount);
    }
}

/* **************************************************************************************************
 * ShiftRight                                             *
 * ************************************************************************************************* */
// ShiftRight::Level 2
template <typename T, typename U>
__aicore__ inline void ShiftRightImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ U* src1, const int32_t& calCount)
{
    static_assert(
        SupportType<
            Tuple<T, U>, Tuple<int32_t, int32_t>, Tuple<uint32_t, int32_t>, Tuple<int16_t, int16_t>,
            Tuple<uint16_t, int16_t>, Tuple<int8_t, int8_t>, Tuple<uint8_t, int8_t>>(),
        "Failed to check dtype in ShiftRight, current api support dtype combination is  "
        "src0: int32_t, src1: int32_t; src0: uint32_t, src1: int32_t; src0: int16_t, "
        "src1: int16_t; src0: uint16_t, src1: int16_t; "
        "src0: int8_t, src1: int8_t; src0: uint8_t, src1: int8_t.");
    if constexpr (SupportBytes<T, 8>()) {
        BinaryContinuousImplTemplate<
            T, U, Reg::RegTensor<T, Reg::RegTraitNumTwo>, Reg::RegTensor<U, Reg::RegTraitNumTwo>,
            Reg::ShiftRight<
                T, U, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumTwo>,
                Reg::RegTensor<U, Reg::RegTraitNumTwo>>>(dst, src0, src1, calCount);
    } else {
        BinaryContinuousImplTemplate<
            T, U, Reg::RegTensor<T>, Reg::RegTensor<U>,
            Reg::ShiftRight<T, U, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>, Reg::RegTensor<U>>>(
            dst, src0, src1, calCount);
    }
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H__
#endif
