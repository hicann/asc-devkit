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
    "impl/basic_api/dav_3510/kernel_operator_vec_binary_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_vec_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_VEC_BINARY_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_BINARY_IMPL_H

#include "../kernel_utils.h"
#include "kernel_operator_common_impl.h"
#include "kernel_operator_vec_binary_continuous_impl.h"
#include "kernel_operator_vec_template_impl.h"
#include "../../../include/basic_api/reg_compute/kernel_reg_compute_intf.h"

namespace AscendC {
template <typename T, bool isSetMask = true>
__aicore__ inline void AddImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        (SupportType<T, half, uint16_t, int16_t, bfloat16_t, uint32_t, int32_t, float>()),
        "current data type is not supported on current device!");
    constexpr auto func = Reg::Add<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, true>(dst, src0, src1, mask, 0, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void SubImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        (SupportType<T, half, uint16_t, int16_t, bfloat16_t, uint32_t, int32_t, float>()),
        "current data type is not supported on current device!");
    constexpr auto func = Reg::Sub<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, true>(dst, src0, src1, mask, 0, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void MulImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        (SupportType<T, half, uint16_t, int16_t, bfloat16_t, uint32_t, int32_t, float>()),
        "current data type is not supported on current device!");
    constexpr auto func = Reg::Mul<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, true>(dst, src0, src1, mask, 0, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true, const DivConfig& config = DEFAULT_DIV_CONFIG>
__aicore__ inline void DivImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        (SupportType<T, uint16_t, int16_t, uint32_t, int32_t, half, float>()),
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
        constexpr auto func = Reg::Div<T, &mode, Reg::RegTensor<T>>;
        Internal::VecBinaryImplTemplate<func, isSetMask, true>(dst, src0, src1, mask, 0, repeatTime, repeatParams);
    } else if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == DivAlgo::PRECISION_0ULP_FTZ_TRUE
#else
        config.algo == DivAlgo::DIFF_COMPENSATION || config.algo == DivAlgo::PRECISION_0ULP_FTZ_TRUE
#endif
    ) {
        static constexpr AscendC::Reg::DivSpecificMode mode = {
            Reg::MaskMergeMode::ZEROING, true, DivAlgo::PRECISION_0ULP_FTZ_TRUE};
        constexpr auto func = Reg::Div<T, &mode, Reg::RegTensor<T>>;
        Internal::VecBinaryImplTemplate<func, isSetMask, true>(dst, src0, src1, mask, 0, repeatTime, repeatParams);
    } else if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == DivAlgo::DIFF_COMPENSATION || config.algo == DivAlgo::PRECISION_0ULP_FTZ_FALSE
#else
        config.algo == DivAlgo::PRECISION_0ULP_FTZ_FALSE
#endif
    ) {
        static constexpr AscendC::Reg::DivSpecificMode mode = {
            Reg::MaskMergeMode::ZEROING, false, DivAlgo::PRECISION_0ULP_FTZ_FALSE};
        constexpr auto func = Reg::Div<T, &mode, Reg::RegTensor<T>>;
        Internal::VecBinaryImplTemplate<func, isSetMask, true>(dst, src0, src1, mask, 0, repeatTime, repeatParams);
    } else if constexpr (
#if !defined(__ASC_FTZ__)
        (config.algo == DivAlgo::INTRINSIC && SupportType<T, half, float>()) ||
        config.algo == DivAlgo::PRECISION_1ULP_FTZ_FALSE
#else
        config.algo == DivAlgo::PRECISION_1ULP_FTZ_FALSE
#endif
    ) {
        static constexpr AscendC::Reg::DivSpecificMode mode = {
            Reg::MaskMergeMode::ZEROING, false, DivAlgo::PRECISION_1ULP_FTZ_FALSE};
        constexpr auto func = Reg::Div<T, &mode, Reg::RegTensor<T>>;
        Internal::VecBinaryImplTemplate<func, isSetMask, true>(dst, src0, src1, mask, 0, repeatTime, repeatParams);
    }
}

template <typename T, bool isSetMask = true>
__aicore__ inline void MaxImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        (SupportType<T, half, uint16_t, int16_t, bfloat16_t, uint32_t, int32_t, float>()),
        "current data type is not supported on current device!");
    constexpr auto func = Reg::Max<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, true>(dst, src0, src1, mask, 0, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void MinImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        (SupportType<T, half, uint16_t, int16_t, bfloat16_t, uint32_t, int32_t, float>()),
        "current data type is not supported on current device!");
    constexpr auto func = Reg::Min<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, true>(dst, src0, src1, mask, 0, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void AndImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        (SupportType<T, int16_t, uint16_t, uint32_t, int32_t>()),
        "current data type is not supported on current device!");
    constexpr auto func = Reg::And<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, true>(dst, src0, src1, mask, 0, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void OrImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        (SupportType<T, int16_t, uint16_t, uint32_t, int32_t>()),
        "current data type is not supported on current device!");
    constexpr auto func = Reg::Or<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, true>(dst, src0, src1, mask, 0, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void AddImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        (SupportType<T, half, uint16_t, int16_t, bfloat16_t, uint32_t, int32_t, float>()),
        "current data type is not supported on current device!");
    constexpr auto func = Reg::Add<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, false>(dst, src0, src1, nullptr, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void SubImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        (SupportType<T, half, uint16_t, int16_t, bfloat16_t, uint32_t, int32_t, float>()),
        "current data type is not supported on current device!");
    constexpr auto func = Reg::Sub<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, false>(dst, src0, src1, nullptr, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void MulImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        (SupportType<T, half, uint16_t, int16_t, bfloat16_t, uint32_t, int32_t, float>()),
        "current data type is not supported on current device!");
    constexpr auto func = Reg::Mul<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, false>(dst, src0, src1, nullptr, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true, const DivConfig& config = DEFAULT_DIV_CONFIG>
__aicore__ inline void DivImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        (SupportType<T, uint16_t, int16_t, uint32_t, int32_t, half, float>()),
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
        constexpr auto func = Reg::Div<T, &mode, Reg::RegTensor<T>>;
        Internal::VecBinaryImplTemplate<func, isSetMask, false>(
            dst, src0, src1, nullptr, mask, repeatTime, repeatParams);
    } else if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == DivAlgo::PRECISION_0ULP_FTZ_TRUE
#else
        config.algo == DivAlgo::DIFF_COMPENSATION || config.algo == DivAlgo::PRECISION_0ULP_FTZ_TRUE
#endif
    ) {
        static constexpr AscendC::Reg::DivSpecificMode mode = {
            Reg::MaskMergeMode::ZEROING, true, DivAlgo::PRECISION_0ULP_FTZ_TRUE};
        constexpr auto func = Reg::Div<T, &mode, Reg::RegTensor<T>>;
        Internal::VecBinaryImplTemplate<func, isSetMask, false>(
            dst, src0, src1, nullptr, mask, repeatTime, repeatParams);
    } else if constexpr (
#if !defined(__ASC_FTZ__)
        config.algo == DivAlgo::DIFF_COMPENSATION || config.algo == DivAlgo::PRECISION_0ULP_FTZ_FALSE
#else
        config.algo == DivAlgo::PRECISION_0ULP_FTZ_FALSE
#endif
    ) {
        static constexpr AscendC::Reg::DivSpecificMode mode = {
            Reg::MaskMergeMode::ZEROING, false, DivAlgo::PRECISION_0ULP_FTZ_FALSE};
        constexpr auto func = Reg::Div<T, &mode, Reg::RegTensor<T>>;
        Internal::VecBinaryImplTemplate<func, isSetMask, false>(
            dst, src0, src1, nullptr, mask, repeatTime, repeatParams);
    } else if constexpr (
#if !defined(__ASC_FTZ__)
        (config.algo == DivAlgo::INTRINSIC && SupportType<T, half, float>()) ||
        config.algo == DivAlgo::PRECISION_1ULP_FTZ_FALSE
#else
        config.algo == DivAlgo::PRECISION_1ULP_FTZ_FALSE
#endif
    ) {
        static constexpr AscendC::Reg::DivSpecificMode mode = {
            Reg::MaskMergeMode::ZEROING, false, DivAlgo::PRECISION_1ULP_FTZ_FALSE};
        constexpr auto func = Reg::Div<T, &mode, Reg::RegTensor<T>>;
        Internal::VecBinaryImplTemplate<func, isSetMask, false>(
            dst, src0, src1, nullptr, mask, repeatTime, repeatParams);
    }
}

template <typename T, bool isSetMask = true>
__aicore__ inline void MaxImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        (SupportType<T, half, uint16_t, int16_t, bfloat16_t, uint32_t, int32_t, float>()),
        "current data type is not supported on current device!");
    constexpr auto func = Reg::Max<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, false>(dst, src0, src1, nullptr, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void MinImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        (SupportType<T, half, uint16_t, int16_t, bfloat16_t, uint32_t, int32_t, float>()),
        "current data type is not supported on current device!");
    constexpr auto func = Reg::Min<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, false>(dst, src0, src1, nullptr, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void AndImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        (SupportType<T, int16_t, uint16_t, uint32_t, int32_t>()),
        "current data type is not supported on current device!");
    constexpr auto func = Reg::And<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, false>(dst, src0, src1, nullptr, mask, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void OrImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        (SupportType<T, int16_t, uint16_t, uint32_t, int32_t>()),
        "current data type is not supported on current device!");
    constexpr auto func = Reg::Or<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, false>(dst, src0, src1, nullptr, mask, repeatTime, repeatParams);
}

/* **************************************************************************************************
 * AddRelu                                             *
 * ************************************************************************************************* */
// AddRelu::Level 0
namespace RegAddRelu {
template <typename T, typename RegT>
__aicore__ inline void AddRelu(RegT& dstReg, RegT& srcReg0, RegT& srcReg1, Reg::MaskReg& mask)
{
    Reg::Add(dstReg, srcReg0, srcReg1, mask);
    Reg::Maxs(dstReg, dstReg, (T)0, mask);
}
} // namespace RegAddRelu

template <typename T, bool isSetMask = true>
__aicore__ inline void AddReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, float, int16_t>(), "Failed to check dtype in AddRelu, current api support "
                                                "dtype combination is src and dst both: half / float / int16_t.");
    constexpr auto func = RegAddRelu::AddRelu<T, Reg::RegTensor<T>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, true>(dst, src0, src1, mask, 0, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void AddReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, float, int16_t>(), "Failed to check dtype in AddRelu, current api support "
                                                "dtype combination is src and dst both: half / float / int16_t.");
    constexpr auto func = RegAddRelu::AddRelu<T, Reg::RegTensor<T>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, false>(dst, src0, src1, nullptr, mask, repeatTime, repeatParams);
}

/* **************************************************************************************************
 * FusedMulAdd                                             *
 * ************************************************************************************************* */
// FusedMulAdd::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void FusedMulAddImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, float, bfloat16_t>(), "Failed to check dtype in FusedMulAdd, current api support dtype "
                                                   "combination is src and dst both: half/float/bfloat16_t.");
    constexpr auto func = Reg::FusedMulDstAdd<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, true, Internal::BinaryFuncMode::DST_SRC_INPUT>(
        dst, src0, src1, mask, 0, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void FusedMulAddImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, float, bfloat16_t>(), "Failed to check dtype in FusedMulAdd, current api support dtype "
                                                   "combination is src and dst both: half/float/bfloat16_t.");
    constexpr auto func = Reg::FusedMulDstAdd<T, Reg::MaskMergeMode::ZEROING, Reg::RegTensor<T>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, false, Internal::BinaryFuncMode::DST_SRC_INPUT>(
        dst, src0, src1, nullptr, mask, repeatTime, repeatParams);
}

/* **************************************************************************************************
 * FusedMulAddRelu                                             *
 * ************************************************************************************************* */
// FusedMulAddRelu::Level 0
namespace RegFusedMulAddRelu {
template <typename T, typename RegT>
__aicore__ inline void FusedMulAddRelu(RegT& dstReg, RegT& srcReg0, RegT& srcReg1, Reg::MaskReg& mask)
{
    Reg::FusedMulDstAdd(dstReg, srcReg0, srcReg1, mask);
    Reg::Maxs(dstReg, dstReg, (T)0, mask);
}
} // namespace RegFusedMulAddRelu
template <typename T, bool isSetMask = true>
__aicore__ inline void FusedMulAddReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, float>(), "Failed to check dtype in FusedMulAddRelu, current api support dtype "
                                       "combination is src and dst both: half / float.");
    constexpr auto func = RegFusedMulAddRelu::FusedMulAddRelu<T, Reg::RegTensor<T>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, true, Internal::BinaryFuncMode::DST_SRC_INPUT>(
        dst, src0, src1, mask, 0, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void FusedMulAddReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, float>(), "Failed to check dtype in FusedMulAddRelu, current api support dtype "
                                       "combination is src and dst both: half / float.");
    constexpr auto func = RegFusedMulAddRelu::FusedMulAddRelu<T, Reg::RegTensor<T>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, false, Internal::BinaryFuncMode::DST_SRC_INPUT>(
        dst, src0, src1, nullptr, mask, repeatTime, repeatParams);
}
/* **************************************************************************************************
 * MulAddDst                                             *
 * ************************************************************************************************* */
// MulAddDst::Level 0
namespace RegMulAddDst {
template <typename T, typename U, typename RegT, typename RegU>
__aicore__ inline void MulAddDst(RegT& dstReg, RegU& srcReg0, RegU& srcReg1, Reg::MaskReg& mask)
{
    if constexpr (std::is_same<T, U>::value) {
        Reg::MulAddDst(dstReg, srcReg0, srcReg1, mask);
    } else {
        Reg::RegTensor<half> fp16RegTemp;
        Reg::RegTensor<float> castReg1, castReg2;
        // the first 64 half is needed to do muladddst in each repeat
        Reg::UnPack<uint32_t, uint16_t, AscendC::Reg::HighLowPart::LOWEST>(
            (Reg::RegTensor<uint32_t>&)fp16RegTemp, (Reg::RegTensor<uint16_t>&)srcReg0);
        Reg::Cast<float, half, CastParam::mulAddDstTrait>(castReg1, fp16RegTemp, mask);
        Reg::UnPack<uint32_t, uint16_t, AscendC::Reg::HighLowPart::LOWEST>(
            (Reg::RegTensor<uint32_t>&)fp16RegTemp, (Reg::RegTensor<uint16_t>&)srcReg1);
        Reg::Cast<float, half, CastParam::mulAddDstTrait>(castReg2, fp16RegTemp, mask);
        Reg::MulAddDst(dstReg, castReg1, castReg2, mask);
    }
}
} // namespace RegMulAddDst

template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void MulAddDstImpl(
    __ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<Tuple<T, U>, Tuple<half, half>, Tuple<float, float>, Tuple<float, half>>(),
        "Failed to "
        "check dtype in MulAddDst, current api support dtype combination is src: half, dst: half / float; src: float, "
        "dst: float.");
    constexpr auto func = RegMulAddDst::MulAddDst<T, U, Reg::RegTensor<T>, Reg::RegTensor<U>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, true, Internal::BinaryFuncMode::DST_SRC_INPUT>(
        dst, src0, src1, mask, 0, repeatTime, repeatParams);
}

template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void MulAddDstImpl(
    __ubuf__ T* dst, __ubuf__ U* src0, __ubuf__ U* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<Tuple<T, U>, Tuple<half, half>, Tuple<float, float>, Tuple<float, half>>(),
        "Failed to "
        "check dtype in MulAddDst, current api support dtype combination is src: half, dst: half / float; src: float, "
        "dst: float.");
    constexpr auto func = RegMulAddDst::MulAddDst<T, U, Reg::RegTensor<T>, Reg::RegTensor<U>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, false, Internal::BinaryFuncMode::DST_SRC_INPUT>(
        dst, src0, src1, nullptr, mask, repeatTime, repeatParams);
}

/* **************************************************************************************************
 * SubRelu                                             *
 * ************************************************************************************************* */
// SubRelu::Level 0
namespace RegSubRelu {
template <typename T, typename RegT>
__aicore__ inline void SubRelu(RegT& dstReg, RegT& srcReg0, RegT& srcReg1, Reg::MaskReg& mask)
{
    Reg::Sub(dstReg, srcReg0, srcReg1, mask);
    Reg::Maxs(dstReg, dstReg, (T)0, mask);
}
} // namespace RegSubRelu

// SubRelu::Level 0
template <typename T, bool isSetMask = true>
__aicore__ inline void SubReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, float, int16_t>(), "Failed to check dtype in SubRelu, current api support dtype "
                                                "combination is src and dst both: half / float / int16_t.");
    constexpr auto func = RegSubRelu::SubRelu<T, Reg::RegTensor<T>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, true>(dst, src0, src1, mask, 0, repeatTime, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void SubReluImpl(
    __ubuf__ T* dst, __ubuf__ T* src0, __ubuf__ T* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<T, half, float, int16_t>(), "Failed to check dtype in SubRelu, current api support dtype "
                                                "combination is src and dst both: half / float / int16_t.");
    constexpr auto func = RegSubRelu::SubRelu<T, Reg::RegTensor<T>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, false>(dst, src0, src1, nullptr, mask, repeatTime, repeatParams);
}

/* **************************************************************************************************
 * AddDeqRelu                                             *
 * ************************************************************************************************* */
namespace RegAddDeqRelu {
template <typename T, typename RegT, typename RegU>
__aicore__ inline void AddDeqRelu(RegU& dstReg, RegT& srcReg0, RegT& srcReg1, Reg::MaskReg& mask)
{
    // max(float(srcReg0 + srcReg1) * (1/131072) * g_deqValue * 131072, 0)
    Reg::RegTensor<float> tmpReg;
    Reg::Add(srcReg0, srcReg0, srcReg1, mask);
    Reg::Cast<float, int32_t, CastParam::s322floatCastTrait>(tmpReg, srcReg0, mask);
    Reg::Muls(tmpReg, tmpReg, static_cast<float>(DEQ_SHIFT_RIGHT_17_BIT), mask);
    Reg::Muls(tmpReg, tmpReg, static_cast<float>(Internal::g_deqValue), mask);
    Reg::Muls(tmpReg, tmpReg, static_cast<float>(DEQ_SHIFT_LEFT_17_BIT), mask);
    Reg::Maxs(tmpReg, tmpReg, (T)0, mask);
    Reg::Cast<half, float, CastParam::float2halfCastTrait>(dstReg, tmpReg, mask);
    Reg::Pack<uint16_t, uint32_t, Reg::HighLowPart::LOWEST>(
        (Reg::RegTensor<uint16_t>&)dstReg, (Reg::RegTensor<uint32_t>&)dstReg);
}
} // namespace RegAddDeqRelu
template <bool isSetMask = true>
__aicore__ inline void AddDeqReluImpl(
    __ubuf__ half* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, const uint64_t mask[], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    constexpr auto func = RegAddDeqRelu::AddDeqRelu<float, Reg::RegTensor<int32_t>, Reg::RegTensor<half>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, true>(dst, src0, src1, mask, 0, repeatTime, repeatParams);
}

template <bool isSetMask = true>
__aicore__ inline void AddDeqReluImpl(
    __ubuf__ half* dst, __ubuf__ int32_t* src0, __ubuf__ int32_t* src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    constexpr auto func = RegAddDeqRelu::AddDeqRelu<float, Reg::RegTensor<int32_t>, Reg::RegTensor<half>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, false>(dst, src0, src1, nullptr, mask, repeatTime, repeatParams);
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_BINARY_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_IMPL_H__
#endif
