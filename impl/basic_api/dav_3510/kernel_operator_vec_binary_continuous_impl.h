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
    "impl/basic_api/dav_3510/kernel_operator_vec_binary_continuous_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_vec_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H
#include "../kernel_utils.h"
#include "kernel_operator_common_impl.h"
#include "../../../include/basic_api/kernel_operator_sys_var_intf.h"
#include "../../../include/basic_api/reg_compute/kernel_reg_compute_intf.h"

namespace AscendC {
namespace CastParam {
constexpr Reg::CastTrait s322floatCastTrait = {
    Reg::RegLayout::UNKNOWN, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};
constexpr Reg::CastTrait float2halfCastTrait = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};
constexpr Reg::CastTrait mulAddDstTrait = {
    Reg::RegLayout::ZERO, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::UNKNOWN};
} // namespace CastParam

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

template <typename T>
__aicore__ inline void AddImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& calCount)
{
    static_assert(
        (SupportType<
            T, uint8_t, int8_t, half, uint16_t, int16_t, bfloat16_t, uint32_t, int32_t, float, int64_t, uint64_t,
            complex32, complex64>()),
        "current data type is not supported on current device!");
    if constexpr (SupportBytes<T, 8>() || SupportType<T, complex32>()) {
        BinaryContinuousImplTemplate<
            T, Reg::RegTensor<T, Reg::RegTraitNumTwo>,
            Reg::Add<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumTwo>>>(
            dst, src0, src1, calCount);
    } else {
        BinaryContinuousImplTemplate<T, Reg::RegTensor<T>, Reg::Add<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>>(
            dst, src0, src1, calCount);
    }
}

template <typename T>
__aicore__ inline void SubImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& calCount)
{
    static_assert(
        (SupportType<
            T, uint8_t, int8_t, half, uint16_t, int16_t, bfloat16_t, uint32_t, int32_t, float, int64_t, uint64_t,
            complex32, complex64>()),
        "current data type is not supported on current device!");
    if constexpr (SupportBytes<T, 8>() || SupportType<T, complex32>()) {
        BinaryContinuousImplTemplate<
            T, Reg::RegTensor<T, Reg::RegTraitNumTwo>,
            Reg::Sub<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumTwo>>>(
            dst, src0, src1, calCount);
    } else {
        BinaryContinuousImplTemplate<T, Reg::RegTensor<T>, Reg::Sub<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>>(
            dst, src0, src1, calCount);
    }
}

/* **************************************************************************************************
 * Mul                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void MulImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& calCount)
{
    static_assert(
        (SupportType<
            T, half, uint16_t, int16_t, bfloat16_t, uint32_t, int32_t, float, int64_t, uint64_t, complex32,
            complex64>()),
        "current data type is not supported on current device!");
    if constexpr (SupportBytes<T, 8>() || SupportType<T, complex32>()) {
        BinaryContinuousImplTemplate<
            T, Reg::RegTensor<T, Reg::RegTraitNumTwo>,
            Reg::Mul<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumTwo>>>(
            dst, src0, src1, calCount);
    } else {
        BinaryContinuousImplTemplate<T, Reg::RegTensor<T>, Reg::Mul<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>>(
            dst, src0, src1, calCount);
    }
}

/* **************************************************************************************************
 * Div                                             *
 * ************************************************************************************************* */
// Div::Level 2
template <typename T, const DivConfig& config = DEFAULT_DIV_CONFIG>
__aicore__ inline void DivImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& calCount)
{
    static_assert(
        (SupportType<T, uint16_t, int16_t, uint32_t, int32_t, half, float, int64_t, uint64_t, complex32, complex64>()),
        "current data type is not supported on current device!");
    if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == DivAlgo::PRECISION_1ULP_FTZ_TRUE ||
        (config.algo == DivAlgo::INTRINSIC && !SupportType<T, half, float>())
#else
        config.algo == DivAlgo::INTRINSIC || config.algo == DivAlgo::PRECISION_1ULP_FTZ_TRUE
#endif
    ) {
        static constexpr Reg::DivSpecificMode mode = {
            Reg::MaskMergeMode::ZEROING, false, DivAlgo::PRECISION_1ULP_FTZ_TRUE};
        if constexpr (SupportBytes<T, 8>() || SupportType<T, complex32>()) {
            BinaryContinuousImplTemplate<
                T, Reg::RegTensor<T, Reg::RegTraitNumTwo>, Reg::Div<T, &mode, Reg::RegTensor<T, Reg::RegTraitNumTwo>>>(
                dst, src0, src1, calCount);
        } else {
            BinaryContinuousImplTemplate<T, Reg::RegTensor<T>, Reg::Div<T, &mode, Reg::RegTensor<T>>>(
                dst, src0, src1, calCount);
        }
    } else if constexpr (
#if !defined(__ASC_FTZ__)
        (config.algo == DivAlgo::DIFF_COMPENSATION && !SupportType<T, float>()) ||
        config.algo == DivAlgo::PRECISION_0ULP_FTZ_TRUE
#else
        config.algo == DivAlgo::DIFF_COMPENSATION || config.algo == DivAlgo::PRECISION_0ULP_FTZ_TRUE
#endif
    ) {
        static constexpr Reg::DivSpecificMode mode = {
            Reg::MaskMergeMode::ZEROING, true, DivAlgo::PRECISION_0ULP_FTZ_TRUE};
        if constexpr (SupportBytes<T, 8>()) {
            constexpr auto func = Reg::Div<T, &mode, Reg::RegTensor<T, Reg::RegTraitNumTwo>>;
            BinaryContinuousImplTemplate<T, Reg::RegTensor<T, Reg::RegTraitNumTwo>, func>(dst, src0, src1, calCount);
        } else {
            constexpr auto func = Reg::Div<T, &mode, Reg::RegTensor<T>>;
            BinaryContinuousImplTemplate<T, Reg::RegTensor<T>, func>(dst, src0, src1, calCount);
        }
    } else if constexpr (
#if !defined(__ASC_FTZ__)
        (config.algo == DivAlgo::DIFF_COMPENSATION && SupportType<T, float>()) ||
        config.algo == DivAlgo::PRECISION_0ULP_FTZ_FALSE
#else
        config.algo == DivAlgo::PRECISION_0ULP_FTZ_FALSE
#endif
    ) {
        static constexpr Reg::DivSpecificMode mode = {
            Reg::MaskMergeMode::ZEROING, false, DivAlgo::PRECISION_0ULP_FTZ_FALSE};
        if constexpr (SupportBytes<T, 8>()) {
            constexpr auto func = Reg::Div<T, &mode, Reg::RegTensor<T, Reg::RegTraitNumTwo>>;
            BinaryContinuousImplTemplate<T, Reg::RegTensor<T, Reg::RegTraitNumTwo>, func>(dst, src0, src1, calCount);
        } else {
            constexpr auto func = Reg::Div<T, &mode, Reg::RegTensor<T>>;
            BinaryContinuousImplTemplate<T, Reg::RegTensor<T>, func>(dst, src0, src1, calCount);
        }
    } else if constexpr (
#if !defined(__ASC_FTZ__)
        (config.algo == DivAlgo::INTRINSIC && SupportType<T, half, float>()) ||
        config.algo == DivAlgo::PRECISION_1ULP_FTZ_FALSE
#else
        config.algo == DivAlgo::PRECISION_1ULP_FTZ_FALSE
#endif
    ) {
        static constexpr Reg::DivSpecificMode mode = {
            Reg::MaskMergeMode::ZEROING, false, DivAlgo::PRECISION_1ULP_FTZ_FALSE};
        if constexpr (SupportBytes<T, 8>()) {
            constexpr auto func = Reg::Div<T, &mode, Reg::RegTensor<T, Reg::RegTraitNumTwo>>;
            BinaryContinuousImplTemplate<T, Reg::RegTensor<T, Reg::RegTraitNumTwo>, func>(dst, src0, src1, calCount);
        } else {
            constexpr auto func = Reg::Div<T, &mode, Reg::RegTensor<T>>;
            BinaryContinuousImplTemplate<T, Reg::RegTensor<T>, func>(dst, src0, src1, calCount);
        }
    }
}

/* **************************************************************************************************
 * Max                                             *
 * ************************************************************************************************* */
// Max::Level 2
template <typename T>
__aicore__ inline void MaxImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& calCount)
{
    static_assert(
        (SupportType<
            T, uint8_t, int8_t, half, uint16_t, int16_t, bfloat16_t, uint32_t, int32_t, float, int64_t, uint64_t>()),
        "current data type is not supported on current device!");
    if constexpr (SupportBytes<T, 8>()) {
        BinaryContinuousImplTemplate<
            T, Reg::RegTensor<T, Reg::RegTraitNumTwo>,
            Reg::Max<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumTwo>>>(
            dst, src0, src1, calCount);
    } else {
        BinaryContinuousImplTemplate<T, Reg::RegTensor<T>, Reg::Max<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>>(
            dst, src0, src1, calCount);
    }
}

/* **************************************************************************************************
 * Min                                             *
 * ************************************************************************************************* */
// Min::Level 2
template <typename T>
__aicore__ inline void MinImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& calCount)
{
    static_assert(
        (SupportType<
            T, uint8_t, int8_t, half, uint16_t, int16_t, bfloat16_t, uint32_t, int32_t, float, int64_t, uint64_t>()),
        "current data type is not supported on current device!");
    if constexpr (SupportBytes<T, 8>()) {
        BinaryContinuousImplTemplate<
            T, Reg::RegTensor<T, Reg::RegTraitNumTwo>,
            Reg::Min<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumTwo>>>(
            dst, src0, src1, calCount);
    } else {
        BinaryContinuousImplTemplate<T, Reg::RegTensor<T>, Reg::Min<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>>(
            dst, src0, src1, calCount);
    }
}

/* **************************************************************************************************
 * And                                             *
 * ************************************************************************************************* */
// And::Level 2
template <typename T>
__aicore__ inline void AndImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& calCount)
{
    static_assert(
        (SupportType<T, uint8_t, int8_t, int16_t, uint16_t, uint32_t, int32_t, int64_t, uint64_t>()),
        "current data type is not supported on current device!");
    if constexpr (SupportBytes<T, 8>()) {
        BinaryContinuousImplTemplate<
            T, Reg::RegTensor<T, Reg::RegTraitNumTwo>,
            Reg::And<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumTwo>>>(
            dst, src0, src1, calCount);
    } else {
        BinaryContinuousImplTemplate<T, Reg::RegTensor<T>, Reg::And<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>>(
            dst, src0, src1, calCount);
    }
}

/* **************************************************************************************************
 * Or                                             *
 * ************************************************************************************************* */
// Or::Level 2
template <typename T>
__aicore__ inline void OrImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t& calCount)
{
    static_assert(
        (SupportType<T, uint8_t, int8_t, int16_t, uint16_t, uint32_t, int32_t, int64_t, uint64_t>()),
        "current data type is not supported on current device!");
    if constexpr (SupportBytes<T, 8>()) {
        BinaryContinuousImplTemplate<
            T, Reg::RegTensor<T, Reg::RegTraitNumTwo>,
            Reg::Or<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T, Reg::RegTraitNumTwo>>>(dst, src0, src1, calCount);
    } else {
        BinaryContinuousImplTemplate<T, Reg::RegTensor<T>, Reg::Or<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>>(
            dst, src0, src1, calCount);
    }
}

// AddRelu::Level 2
template <typename T>
__simd_vf__ inline void AddReluImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t calCount)
{
    static_assert(
        SupportType<T, half, float, int16_t, uint64_t, int64_t>(),
        "Failed to check dtype in AddRelu, "
        "current api support dtype combination is src and dst both: half / float / int16_t/ uint64_t/ int64_t.");
    const T scalarValue = 0;
    uint32_t sreg = static_cast<uint32_t>(calCount);
    if constexpr (sizeof(T) == 8) {
        constexpr uint32_t sregLower = static_cast<uint32_t>(B64_DATA_NUM_PER_REPEAT * 2);
        const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, sregLower));
        Reg::RegTensor<T, Reg::RegTraitNumTwo> vDstReg;
        Reg::RegTensor<T, Reg::RegTraitNumTwo> vSrcReg0;
        Reg::RegTensor<T, Reg::RegTraitNumTwo> vSrcReg1;
        Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            mask = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
            Reg::LoadAlign(vSrcReg0, src0 + i * sregLower);
            Reg::LoadAlign(vSrcReg1, src1 + i * sregLower);
            Reg::Add(vDstReg, vSrcReg0, vSrcReg1, mask);
            Reg::Maxs(vDstReg, vDstReg, scalarValue, mask);
            Reg::StoreAlign(dst + i * sregLower, vDstReg, mask);
        }
    } else {
        constexpr uint32_t sregLower = static_cast<uint32_t>(GetVecLen() / sizeof(T));
        const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, sregLower));
        Reg::RegTensor<T> dstReg;
        Reg::RegTensor<T> src0Reg;
        Reg::RegTensor<T> src1Reg;
        Reg::MaskReg preg;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            preg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign<T>(src0Reg, src0 + i * sregLower);
            Reg::LoadAlign<T>(src1Reg, src1 + i * sregLower);
            Reg::Add<T>(dstReg, src0Reg, src1Reg, preg);
            Reg::Maxs<T>(dstReg, dstReg, scalarValue, preg);
            Reg::StoreAlign<T>(dst + i * sregLower, dstReg, preg);
        }
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
            Tuple<T, U>, Tuple<int64_t, int64_t>, Tuple<uint64_t, int64_t>, Tuple<int32_t, int32_t>,
            Tuple<uint32_t, int32_t>, Tuple<int16_t, int16_t>, Tuple<uint16_t, int16_t>, Tuple<int8_t, int8_t>,
            Tuple<uint8_t, int8_t>>(),
        "Failed to check dtype in ShiftLeft, current api support dtype combination is src0: int64_t, src1: int64_t; "
        "src0: uint64_t, src1: int64_t; src0: int32_t, src1: int32_t; src0: uint32_t, src1: int32_t; src0: int16_t, "
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
            Tuple<T, U>, Tuple<int64_t, int64_t>, Tuple<uint64_t, int64_t>, Tuple<int32_t, int32_t>,
            Tuple<uint32_t, int32_t>, Tuple<int16_t, int16_t>, Tuple<uint16_t, int16_t>, Tuple<int8_t, int8_t>,
            Tuple<uint8_t, int8_t>>(),
        "Failed to check dtype in ShiftRight, current api support dtype combination is src0: int64_t, src1: int64_t; "
        "src0: uint64_t, src1: int64_t; src0: int32_t, src1: int32_t; src0: uint32_t, src1: int32_t; src0: int16_t, "
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

/* **************************************************************************************************
 * FusedMulAdd                                             *
 * ************************************************************************************************* */
template <typename T, bool isSetMask = true>
__simd_vf__ inline void FusedMulAddImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t calCount)
{
    static_assert(
        SupportType<T, half, float, bfloat16_t, uint64_t, int64_t>(),
        "Failed to check dtype in FusedMulAdd,"
        "current api support dtype combination is src and dst both: half / float / bfloat16_t / uint64_t / int64_t.");
    uint32_t sreg = static_cast<uint32_t>(calCount);
    if constexpr (sizeof(T) == 8) {
        constexpr uint32_t sregLower = static_cast<uint32_t>(B64_DATA_NUM_PER_REPEAT * 2);
        const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, sregLower));
        Reg::RegTensor<T, Reg::RegTraitNumTwo> vDstReg0;
        Reg::RegTensor<T, Reg::RegTraitNumTwo> vDstReg1;
        Reg::RegTensor<T, Reg::RegTraitNumTwo> vSrcReg0;
        Reg::RegTensor<T, Reg::RegTraitNumTwo> vSrcReg1;
        Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            mask = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
            Reg::LoadAlign(vSrcReg0, src0 + i * sregLower);
            Reg::LoadAlign(vSrcReg1, src1 + i * sregLower);
            Reg::LoadAlign(vDstReg0, dst + i * sregLower);
            Reg::Mul(vDstReg1, vSrcReg0, vDstReg0, mask);
            Reg::Add(vDstReg0, vDstReg1, vSrcReg1, mask);
            Reg::StoreAlign(dst + i * sregLower, vDstReg0, mask);
        }
    } else {
        constexpr uint32_t repeatStride = static_cast<uint32_t>(GetVecLen() / sizeof(T));
        const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, repeatStride));
        Reg::RegTensor<T> src0Reg;
        Reg::RegTensor<T> src1Reg;
        Reg::RegTensor<T> dstReg;
        Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            mask = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign(src0Reg, src0 + i * repeatStride);
            Reg::LoadAlign(src1Reg, src1 + i * repeatStride);
            Reg::LoadAlign(dstReg, dst + i * repeatStride);
            Reg::FusedMulDstAdd(dstReg, src0Reg, src1Reg, mask);
            Reg::StoreAlign(dst + i * repeatStride, dstReg, mask);
        }
    }
}

/* **************************************************************************************************
 * FusedMulAddRelu                                             *
 * ************************************************************************************************* */
// FusedMulAddRelu::Level 2
template <typename T>
__simd_vf__ inline void FusedMulAddReluImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t calCount)
{
    static_assert(
        SupportType<T, half, float, uint64_t, int64_t>(),
        "Failed to check dtype in FusedMulAddRelu, current "
        "api support dtype combination is src and dst both: half / float / uint64_t / int64_t.");
    const T scalarValue = 0;
    uint32_t sreg = static_cast<uint32_t>(calCount);
    if constexpr (sizeof(T) == 8) {
        constexpr uint32_t sregLower = static_cast<uint32_t>(B64_DATA_NUM_PER_REPEAT * 2);
        const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, sregLower));
        Reg::RegTensor<T, Reg::RegTraitNumTwo> vDstReg0;
        Reg::RegTensor<T, Reg::RegTraitNumTwo> vDstReg1;
        Reg::RegTensor<T, Reg::RegTraitNumTwo> vSrcReg0;
        Reg::RegTensor<T, Reg::RegTraitNumTwo> vSrcReg1;
        Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            mask = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
            Reg::LoadAlign(vSrcReg0, src0 + i * sregLower);
            Reg::LoadAlign(vSrcReg1, src1 + i * sregLower);
            Reg::LoadAlign(vDstReg0, dst + i * sregLower);
            Reg::Mul(vDstReg1, vSrcReg0, vDstReg0, mask);
            Reg::Add(vDstReg0, vDstReg1, vSrcReg1, mask);
            Reg::Maxs(vDstReg0, vDstReg0, scalarValue, mask);
            Reg::StoreAlign(dst + i * sregLower, vDstReg0, mask);
        }
    } else {
        const uint32_t repeatStride = static_cast<uint32_t>(GetVecLen() / sizeof(T));
        const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, repeatStride));
        Reg::RegTensor<T> src0Reg;
        Reg::RegTensor<T> src1Reg;
        Reg::RegTensor<T> dstReg;
        Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            mask = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign(src0Reg, src0 + i * repeatStride);
            Reg::LoadAlign(src1Reg, src1 + i * repeatStride);
            Reg::LoadAlign(dstReg, dst + i * repeatStride);
            Reg::FusedMulDstAdd(dstReg, src0Reg, src1Reg, mask);
            Reg::Maxs(dstReg, dstReg, scalarValue, mask);
            Reg::StoreAlign(dst + i * repeatStride, dstReg, mask);
        }
    }
}
/* **************************************************************************************************
 * MulAddDst                                             *
 * ************************************************************************************************* */
// MulAddDst::Level 2
template <typename T, typename U>
__simd_vf__ inline void MulAddDstImpl(__ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, const int32_t calCount)
{
    static_assert(
        SupportType<
            Tuple<T, U>, Tuple<half, half>, Tuple<float, float>, Tuple<float, half>, Tuple<uint64_t, uint64_t>,
            Tuple<int64_t, int64_t>>(),
        "Failed to check dtype in MulAddDst, current api "
        "support dtype combination is src: half, dst: half / float; src: float, dst: float; src: uint64_t, dst: "
        "uint64_t; src: int64_t, dst: int64_t.");
    uint32_t sreg = static_cast<uint32_t>(calCount);
    if constexpr (sizeof(T) == 8) {
        constexpr uint32_t sregLower = static_cast<uint32_t>(B64_DATA_NUM_PER_REPEAT * 2);
        const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, sregLower));
        Reg::RegTensor<T, Reg::RegTraitNumTwo> vDstReg0;
        Reg::RegTensor<U, Reg::RegTraitNumTwo> vSrcReg0;
        Reg::RegTensor<U, Reg::RegTraitNumTwo> vSrcReg1;
        Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            mask = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
            Reg::LoadAlign(vSrcReg0, src0 + i * sregLower);
            Reg::LoadAlign(vSrcReg1, src1 + i * sregLower);
            Reg::LoadAlign(vDstReg0, dst + i * sregLower);
            Reg::MulAddDst(vDstReg0, vSrcReg0, vSrcReg1, mask);
            Reg::StoreAlign(dst + i * sregLower, vDstReg0, mask);
        }
    } else {
        constexpr uint16_t numPerRep = GetVecLen() / sizeof(T);
        const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, numPerRep));
        Reg::RegTensor<U> src0Reg, src1Reg;
        Reg::RegTensor<T> dstReg;
        Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            mask = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign(src0Reg, src0 + i * numPerRep);
            Reg::LoadAlign(src1Reg, src1 + i * numPerRep);
            Reg::LoadAlign(dstReg, dst + i * numPerRep);
            Reg::MulAddDst(dstReg, src0Reg, src1Reg, mask);
            Reg::StoreAlign(dst + i * numPerRep, dstReg, mask);
        }
    }
}

__simd_vf__ inline void MulAddDstImpl(
    __ubuf__ float* dst, __ubuf__ half* src0, __ubuf__ half* src1, const int32_t calCount)
{
    uint32_t sregB32 = static_cast<uint32_t>(calCount);         // updated when float calculation
    constexpr uint16_t numPerRep = GetVecLen() / sizeof(float); // each repeat 64 half->float to calculate
    const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, numPerRep));
    Reg::RegTensor<half> src0Reg, src1Reg;
    Reg::RegTensor<float> dstReg, castReg1, castReg2;
    Reg::MaskReg maskB32; // updated when float calculation
    for (uint16_t i = 0; i < repeatTime; ++i) {
        maskB32 = Reg::UpdateMask<float>(sregB32);
        Reg::LoadAlign<half, Reg::LoadDist::DIST_UNPACK_B16>(src0Reg, src0 + i * numPerRep); // 64 half
        Reg::LoadAlign<half, Reg::LoadDist::DIST_UNPACK_B16>(src1Reg, src1 + i * numPerRep); // 64 half
        Reg::Cast<float, half, CastParam::mulAddDstTrait>(castReg1, src0Reg, maskB32);       // 64 float
        Reg::Cast<float, half, CastParam::mulAddDstTrait>(castReg2, src1Reg, maskB32);       // 64 float
        Reg::LoadAlign(dstReg, dst + i * numPerRep);
        Reg::MulAddDst(dstReg, castReg1, castReg2, maskB32);
        Reg::StoreAlign(dst + i * numPerRep, dstReg, maskB32);
    }
}

/* **************************************************************************************************
 * SubRelu                                             *
 * ************************************************************************************************* */
// SubRelu::Level 2
template <typename T>
__simd_vf__ inline void SubReluImpl(__ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const int32_t calCount)
{
    static_assert(
        SupportType<T, half, float, int16_t, uint64_t, int64_t>(),
        "Failed to check dtype in SubRelu, "
        "current api support dtype combination is src and dst both: half / float / int16_t / uint64_t / int64_t.");
    uint32_t sreg = static_cast<uint32_t>(calCount);
    const T scalarValue = 0;
    if constexpr (sizeof(T) == 8) {
        constexpr uint32_t sregLower = static_cast<uint32_t>(B64_DATA_NUM_PER_REPEAT * 2);
        const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, sregLower));
        Reg::RegTensor<T, Reg::RegTraitNumTwo> vDstReg;
        Reg::RegTensor<T, Reg::RegTraitNumTwo> vSrcReg0;
        Reg::RegTensor<T, Reg::RegTraitNumTwo> vSrcReg1;
        Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            mask = Reg::UpdateMask<T, Reg::RegTraitNumTwo>(sreg);
            Reg::LoadAlign(vSrcReg0, src0 + i * sregLower);
            Reg::LoadAlign(vSrcReg1, src1 + i * sregLower);
            Reg::Sub(vDstReg, vSrcReg0, vSrcReg1, mask);
            Reg::Maxs(vDstReg, vDstReg, scalarValue, mask);
            Reg::StoreAlign(dst + i * sregLower, vDstReg, mask);
        }
    } else {
        constexpr uint16_t numPerRep = GetVecLen() / sizeof(T);
        const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, numPerRep));
        Reg::RegTensor<T> dstReg, src0Reg, src1Reg;
        Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            mask = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign(src0Reg, src0 + i * numPerRep);
            Reg::LoadAlign(src1Reg, src1 + i * numPerRep);
            Reg::Sub(dstReg, src0Reg, src1Reg, mask);
            Reg::Maxs(dstReg, dstReg, scalarValue, mask);
            Reg::StoreAlign(dst + i * numPerRep, dstReg, mask);
        }
    }
}

/* **************************************************************************************************
 * AddDeqRelu                                             *
 * ************************************************************************************************* */
// AddDeqRelu::Level 2
__simd_vf__ inline void AddDeqReluImpl(
    __ubuf__ half* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, const int32_t calCount)
{
    const float scalarValue = 0.;
    constexpr uint32_t sregLower = static_cast<uint32_t>(GetVecLen() / sizeof(int32_t));
    const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, sregLower));
    Reg::RegTensor<half> dstReg;
    Reg::RegTensor<float> tmpReg;
    Reg::RegTensor<int32_t> src0Reg;
    Reg::RegTensor<int32_t> src1Reg;
    uint32_t sreg = static_cast<uint32_t>(calCount);
    Reg::MaskReg preg;
    for (uint16_t i = 0; i < repeatTime; ++i) {
        preg = Reg::UpdateMask<int32_t>(sreg);
        Reg::LoadAlign<int32_t>(src0Reg, src0 + i * sregLower);
        Reg::LoadAlign<int32_t>(src1Reg, src1 + i * sregLower);
        Reg::Add<int32_t>(src0Reg, src0Reg, src1Reg, preg);
        Reg::Cast<float, int32_t, CastParam::s322floatCastTrait>(tmpReg, src0Reg, preg);
        Reg::Muls<float>(tmpReg, tmpReg, static_cast<float>(DEQ_SHIFT_RIGHT_17_BIT), preg);
        Reg::Muls<float>(tmpReg, tmpReg, static_cast<float>(Internal::g_deqValue), preg);
        Reg::Muls<float>(tmpReg, tmpReg, static_cast<float>(DEQ_SHIFT_LEFT_17_BIT), preg);
        Reg::Maxs<float>(tmpReg, tmpReg, scalarValue, preg);
        Reg::Cast<half, float, CastParam::float2halfCastTrait>(dstReg, tmpReg, preg);
        Reg::StoreAlign<half, Reg::StoreDist::DIST_PACK_B32>(dst + i * sregLower, dstReg, preg);
    }
}

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
 * Mull                                            *
 * ************************************************************************************************* */
// Mull::Level 2
template <typename T>
__simd_vf__ inline void MullImpl(
    __ubuf__ T* dst0, __ubuf__ T* dst1, __ubuf__ T* src0, __ubuf__ T* src1, const uint32_t calCount)
{
    static_assert(
        SupportType<T, uint32_t, int32_t>(), "Failed to check dtype in Mull, current api support "
                                             "dtype is uint32_t, int32_t");
    constexpr uint32_t sregLower = static_cast<uint32_t>(GetVecLen() / sizeof(T));
    const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, sregLower));
    uint32_t sreg = static_cast<uint32_t>(calCount);
    Reg::RegTensor<T> vDstReg0;
    Reg::RegTensor<T> vDstReg1;
    Reg::RegTensor<T> vSrcReg0;
    Reg::RegTensor<T> vSrcReg1;
    Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<T>(sreg);
        Reg::LoadAlign(vSrcReg0, src0 + i * sregLower);
        Reg::LoadAlign(vSrcReg1, src1 + i * sregLower);
        Reg::Mull(vDstReg0, vDstReg1, vSrcReg0, vSrcReg1, mask);
        Reg::StoreAlign(dst0 + i * sregLower, vDstReg0, mask);
        Reg::StoreAlign(dst1 + i * sregLower, vDstReg1, mask);
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
            Reg::FusedExpSub(vDstReg0, vSrcReg0, vSrcReg1, mask);
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

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_CONTINUOUS_IMPL_H__
#endif
