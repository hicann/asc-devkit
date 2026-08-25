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
 * \file kernel_operator_vec_vconv_impl.h
 * \brief AscendC l300 support vector cast api.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_l300/kernel_operator_vec_vconv_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_VCONV_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_VEC_VCONV_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_VCONV_IMPL_H
#include "../kernel_utils.h"
#include "kernel_operator_common_impl.h"
#include "../../../include/basic_api/kernel_struct_unary.h"

namespace AscendC {

constexpr Reg::CastTrait layoutZMrgZ = {
    Reg::RegLayout::ZERO, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::UNKNOWN};

constexpr Reg::CastTrait layoutZSatSMrgZ = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::UNKNOWN};

constexpr Reg::CastTrait layoutZSatSMrgZRndA = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_ROUND};

constexpr Reg::CastTrait layoutZSatSMrgZRndR = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};

constexpr Reg::CastTrait layoutZMrgZRndR = {
    Reg::RegLayout::ZERO, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};

constexpr Reg::CastTrait layoutZMrgZRndA = {
    Reg::RegLayout::ZERO, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_ROUND};

constexpr Reg::CastTrait layoutZMrgZRndC = {
    Reg::RegLayout::ZERO, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_CEIL};

constexpr Reg::CastTrait layoutZMrgZRndF = {
    Reg::RegLayout::ZERO, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_FLOOR};

constexpr Reg::CastTrait layoutZMrgZRndZ = {
    Reg::RegLayout::ZERO, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_TRUNC};

constexpr Reg::CastTrait MrgZRndR = {
    Reg::RegLayout::UNKNOWN, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};

constexpr Reg::CastTrait MrgZRndA = {
    Reg::RegLayout::UNKNOWN, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_ROUND};

constexpr Reg::CastTrait MrgZRndF = {
    Reg::RegLayout::UNKNOWN, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_CEIL};

constexpr Reg::CastTrait MrgZRndC = {
    Reg::RegLayout::UNKNOWN, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_FLOOR};

constexpr Reg::CastTrait MrgZRndZ = {
    Reg::RegLayout::UNKNOWN, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_TRUNC};

constexpr Reg::CastTrait MrgZRndRSatS = {
    Reg::RegLayout::UNKNOWN, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};

constexpr Reg::CastTrait MrgZRndASatS = {
    Reg::RegLayout::UNKNOWN, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_ROUND};

constexpr Reg::CastTrait MrgZRndFSatS = {
    Reg::RegLayout::UNKNOWN, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_CEIL};

constexpr Reg::CastTrait MrgZRndCSatS = {
    Reg::RegLayout::UNKNOWN, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_FLOOR};

constexpr Reg::CastTrait MrgZRndZSatS = {
    Reg::RegLayout::UNKNOWN, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_TRUNC};

constexpr Reg::CastTrait LayoutZMrgZRndRSatS = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};

constexpr Reg::CastTrait LayoutZMrgZRndASatS = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_ROUND};

constexpr Reg::CastTrait LayoutZMrgZRndRSatNS = {
    Reg::RegLayout::ZERO, Reg::SatMode::NO_SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};

constexpr Reg::CastTrait LayoutZMrgZRndASatNS = {
    Reg::RegLayout::ZERO, Reg::SatMode::NO_SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_ROUND};

constexpr Reg::CastTrait MrgZRndRSatNS = {
    Reg::RegLayout::UNKNOWN, Reg::SatMode::NO_SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};

namespace CastParam {
constexpr Reg::CastTrait AddReluCastTrait = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};
constexpr Reg::CastTrait SubReluCastTrait = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};
constexpr Reg::CastTrait s162HalfTrait = {
    Reg::RegLayout::UNKNOWN, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};
constexpr Reg::CastTrait s162f32CastTrait = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::UNKNOWN};
constexpr Reg::CastTrait f322s16CastTrait = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};
constexpr Reg::CastTrait TrueHalfBlockCastTrait = {
    Reg::RegLayout::ONE, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::UNKNOWN};
constexpr Reg::CastTrait FalseHalfBlockCastTrait = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::UNKNOWN};
constexpr Reg::CastTrait TrueHalfBlockHalf2S8Trait = {
    Reg::RegLayout::ONE, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};
constexpr Reg::CastTrait FalseHalfBlockHalf2S8Trait = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};
constexpr Reg::CastTrait s322F32CastTrait = {
    Reg::RegLayout::UNKNOWN, Reg::SatMode::NO_SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};
constexpr Reg::CastTrait f322F16CastTrait = {
    Reg::RegLayout::ZERO, Reg::SatMode::NO_SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};

template <typename ORI_TYPE>
struct CastTypeTrait {
    using RealType = ORI_TYPE;
};

template <>
struct CastTypeTrait<int4b_t> {
    using RealType = int4x2_t;
};
} // namespace CastParam

// micro adaptor
template <typename T, typename U, RoundMode roundMode, Mode mode, SatMode satMode, PartMode partMode>
__aicore__ inline void CastAdaptor(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    if constexpr (roundMode != RoundMode::CAST_NONE && satMode != SatMode::UNKNOWN && partMode != PartMode::UNKNOWN) {
        return Cast<T, U, roundMode, mode, satMode, partMode>(dstReg, srcReg, mask);
    } else if constexpr (
        roundMode != RoundMode::CAST_NONE && satMode != SatMode::UNKNOWN && partMode == PartMode::UNKNOWN) {
        return Cast<T, U, roundMode, mode, satMode>(dstReg, srcReg, mask);
    } else if constexpr (
        roundMode != RoundMode::CAST_NONE && satMode == SatMode::UNKNOWN && partMode != PartMode::UNKNOWN) {
        return Cast<T, U, roundMode, mode, partMode>(dstReg, srcReg, mask);
    } else if constexpr (
        roundMode == RoundMode::CAST_NONE && satMode != SatMode::UNKNOWN && partMode != PartMode::UNKNOWN) {
        return Cast<T, U, mode, satMode, partMode>(dstReg, srcReg, mask);
    } else if constexpr (
        roundMode == RoundMode::CAST_NONE && satMode == SatMode::UNKNOWN && partMode != PartMode::UNKNOWN) {
        return Cast<T, U, mode, partMode>(dstReg, srcReg, mask);
    } else if constexpr (
        roundMode != RoundMode::CAST_NONE && satMode == SatMode::UNKNOWN && partMode == PartMode::UNKNOWN) {
        return Cast<T, U, roundMode, mode>(dstReg, srcReg, mask);
    } else {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupport pattern of CastAdaptor"); });
    }
}

template <typename T, typename U, RoundMode roundMode, Mode mode, SatMode satMode, PPMode ppMode>
__aicore__ inline void CastAdaptor(RegTensor<T>& dstReg, RegTensor<U>& srcReg, MaskReg& mask)
{
    if constexpr (roundMode != RoundMode::CAST_NONE && satMode != SatMode::UNKNOWN && ppMode != PPMode::UNKNOWN) {
        return Cast<T, U, roundMode, mode, satMode, ppMode>(dstReg, srcReg, mask);
    }
    if constexpr (roundMode == RoundMode::CAST_NONE && satMode != SatMode::UNKNOWN && ppMode != PPMode::UNKNOWN) {
        return Cast<T, U, mode, satMode, ppMode>(dstReg, srcReg, mask);
    } else if constexpr (
        roundMode == RoundMode::CAST_NONE && satMode == SatMode::UNKNOWN && ppMode != PPMode::UNKNOWN) {
        return Cast<T, U, mode, ppMode>(dstReg, srcReg, mask);
    } else {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupport pattern of CastAdaptor"); });
    }
}

// For Cast L2
#define CAST_LOWER_HALF(srcType, dstType, srcBits, dstBits, rndStr, rndMode, satMode, mode)              \
    __aicore__ inline void CastIntrinsicsImpl##rndStr(                                                   \
        __ubuf__ dstType* dst, __ubuf__ srcType* src, const uint32_t calCount)                           \
    {                                                                                                    \
        __VEC_SCOPE__                                                                                    \
        {                                                                                                \
            uint32_t len = calCount;                                                                     \
            uint32_t sregLower = (uint32_t)(VECTOR_REG_WIDTH / sizeof(dstType));                         \
            uint16_t repeatTimes = CeilDivision(calCount, sregLower);                                    \
            for (uint16_t i = 0; i < (uint16_t)repeatTimes; i++) {                                       \
                RegTensor<srcType> input0;                                                               \
                RegTensor<srcType> input1;                                                               \
                RegTensor<dstType> mid0;                                                                 \
                RegTensor<dstType> mid1;                                                                 \
                RegTensor<dstType> output;                                                               \
                MaskReg p0 = CreatePredicate<dstType>(len);                                              \
                DataCopy(input0, src, i* sregLower);                                                     \
                DataCopy(input1, src + (sregLower >> 1), i * sregLower);                                 \
                DeInterleave(input0, input1, input0, input1);                                            \
                CastAdaptor<dstType, srcType, rndMode, mode, satMode, PartMode::EVEN>(mid0, input0, p0); \
                CastAdaptor<dstType, srcType, rndMode, mode, satMode, PartMode::ODD>(mid1, input1, p0);  \
                Or(output, mid0, mid1, p0);                                                              \
                DataCopy(dst, output, i* sregLower, p0);                                                 \
            }                                                                                            \
        }                                                                                                \
    }

// For Cast L2
#define CAST_UPPER_HALF(srcType, dstType, srcBits, dstBits, rndStr, rndMode, satMode, mode)                    \
    __aicore__ inline void CastIntrinsicsImpl##rndStr(                                                         \
        __ubuf__ dstType* dst, __ubuf__ srcType* src, const uint32_t calCount)                                 \
    {                                                                                                          \
        __VEC_SCOPE__                                                                                          \
        {                                                                                                      \
            uint32_t len = calCount;                                                                           \
            uint32_t sregLower = (uint32_t)(VECTOR_REG_WIDTH / sizeof(srcType));                               \
            uint16_t repeatTimes = CeilDivision(calCount, sregLower);                                          \
            for (uint16_t i = 0; i < (uint16_t)repeatTimes; i++) {                                             \
                RegTensor<srcType> input;                                                                      \
                RegTensor<dstType> output0;                                                                    \
                RegTensor<dstType> output1;                                                                    \
                RegTensor<dstType> output_even;                                                                \
                RegTensor<dstType> output_odd;                                                                 \
                MaskReg p0 = CreatePredicate<srcType>(len);                                                    \
                MaskReg p1;                                                                                    \
                MaskReg p2;                                                                                    \
                DataCopy(input, src, i* sregLower);                                                            \
                CastAdaptor<dstType, srcType, rndMode, mode, satMode, PartMode::EVEN>(output_even, input, p0); \
                CastAdaptor<dstType, srcType, rndMode, mode, satMode, PartMode::ODD>(output_odd, input, p0);   \
                Interleave(output0, output1, output_even, output_odd);                                         \
                PredicateUnPack<HiloPart::Lower>(p1, p0);                                                      \
                PredicateUnPack<HiloPart::Higher>(p2, p0);                                                     \
                DataCopy(dst, output0, i* sregLower, p1);                                                      \
                DataCopy(dst + (sregLower >> 1), output1, i * sregLower, p2);                                  \
            }                                                                                                  \
        }                                                                                                      \
    }

// For Cast L2 s322half(s32->float->half)
#define CAST_S322HALF(srcType, dstType, srcBits, dstBits, rndStr, rndMode, satMode, mode)                              \
    __aicore__ inline void CastIntrinsicsImpl##rndStr(                                                                 \
        __ubuf__ dstType* dst, __ubuf__ srcType* src, const uint32_t calCount)                                         \
    {                                                                                                                  \
        float deqValueTmp = static_cast<float>(g_deqValue);                                                            \
        __VEC_SCOPE__                                                                                                  \
        {                                                                                                              \
            uint32_t len = calCount;                                                                                   \
            uint32_t sregLower = (uint32_t)(VECTOR_REG_WIDTH / sizeof(dstType));                                       \
            uint16_t repeatTimes = CeilDivision(calCount, sregLower);                                                  \
            for (uint16_t i = 0; i < (uint16_t)repeatTimes; i++) {                                                     \
                RegTensor<srcType> input1, input2, input_even, input_odd;                                              \
                RegTensor<float> intermediate_even, intermediate_odd;                                                  \
                RegTensor<dstType> output1, output2, output;                                                           \
                MaskReg pg = CreatePredicate<dstType>(len);                                                            \
                MaskReg pg_all = CreatePredicate<float>();                                                             \
                DataCopy(input1, src, i* sregLower);                                                                   \
                DataCopy(input2, src, (sregLower >> 1) + i * sregLower);                                               \
                DeInterleave(input_even, input_odd, input1, input2);                                                   \
                CastAdaptor<float, srcType, rndMode, mode, SatMode::UNKNOWN, PartMode::UNKNOWN>(                       \
                    intermediate_even, input_even, pg_all);                                                            \
                CastAdaptor<float, srcType, rndMode, mode, SatMode::UNKNOWN, PartMode::UNKNOWN>(                       \
                    intermediate_odd, input_odd, pg_all);                                                              \
                Muls(intermediate_even, intermediate_even, deqValueTmp, pg_all);                                       \
                Muls(intermediate_odd, intermediate_odd, deqValueTmp, pg_all);                                         \
                CastAdaptor<dstType, float, rndMode, mode, satMode, PartMode::EVEN>(                                   \
                    output1, intermediate_even, pg_all);                                                               \
                CastAdaptor<dstType, float, rndMode, mode, satMode, PartMode::ODD>(output2, intermediate_odd, pg_all); \
                Or((RegTensor<uint16_t>&)output, (RegTensor<uint16_t>&)output1, (RegTensor<uint16_t>&)output2, pg);    \
                DataCopy(dst, output, i* sregLower, pg);                                                               \
            }                                                                                                          \
        }                                                                                                              \
    }

// For Cast L2 half2s4(half->int4b_t)
#define CAST_HALF2S4(srcType, dstType, srcBits, dstBits, rndStr, rndMode, satMode, mode)                        \
    __aicore__ inline void CastIntrinsicsImpl##rndStr(                                                          \
        __ubuf__ dstType* dst, __ubuf__ srcType* src, const uint32_t calCount)                                  \
    {                                                                                                           \
        __VEC_SCOPE__                                                                                           \
        {                                                                                                       \
            uint16_t oneRepSize = (uint32_t)(VECTOR_REG_WIDTH / sizeof(srcType));                               \
            uint16_t repeatTimes = CeilDivision(calCount, oneRepSize);                                          \
            uint32_t sreg = static_cast<uint32_t>(calCount);                                                    \
            static constexpr Reg::CastTrait castTrait = {                                                       \
                Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, rndMode};                 \
            Reg::MaskReg preg, pregDst, pregTemp;                                                               \
            Reg::RegTensor<srcType> srcVreg;                                                                    \
            Reg::RegTensor<int4x2_t> dstVreg;                                                                   \
            Reg::RegTensor<uint8_t> tmpVreg;                                                                    \
            for (uint16_t i = 0; i < repeatTimes; ++i) {                                                        \
                preg = Reg::UpdateMask<srcType>(sreg);                                                          \
                Reg::MaskPack(pregTemp, preg);                                                                  \
                Reg::MaskPack(pregDst, pregTemp);                                                               \
                Reg::DataCopy(srcVreg, src + i * oneRepSize);                                                   \
                Reg::Cast<int4x2_t, srcType, castTrait>(dstVreg, srcVreg, preg);                                \
                Reg::DeInterleave(                                                                              \
                    (Reg::RegTensor<uint8_t>&)dstVreg, tmpVreg, (Reg::RegTensor<uint8_t>&)dstVreg, tmpVreg);    \
                Reg::DeInterleave(                                                                              \
                    (Reg::RegTensor<uint8_t>&)dstVreg, tmpVreg, (Reg::RegTensor<uint8_t>&)dstVreg, tmpVreg);    \
                Reg::DataCopy(                                                                                  \
                    (__ubuf__ uint8_t*)dst + (i * oneRepSize) / 2, (Reg::RegTensor<uint8_t>&)dstVreg, pregDst); \
            }                                                                                                   \
        }                                                                                                       \
    }

// For Cast L2
#define CAST_LOWER_QUATER(srcType, dstType, srcBits, dstBits, rndStr, rndMode, satMode, mode)           \
    __aicore__ inline void CastIntrinsicsImpl##rndStr(                                                  \
        __ubuf__ dstType* dst, __ubuf__ srcType* src, const uint32_t calCount)                          \
    {                                                                                                   \
        __VEC_SCOPE__                                                                                   \
        {                                                                                               \
            uint32_t len = calCount;                                                                    \
            uint32_t sregLower = (uint32_t)(VECTOR_REG_WIDTH / sizeof(dstType));                        \
            uint16_t repeatTimes = CeilDivision(calCount, sregLower);                                   \
            for (uint16_t i = 0; i < (uint16_t)repeatTimes; i++) {                                      \
                RegTensor<srcType> input0;                                                              \
                RegTensor<srcType> input1;                                                              \
                RegTensor<srcType> input2;                                                              \
                RegTensor<srcType> input3;                                                              \
                RegTensor<dstType> output;                                                              \
                RegTensor<dstType> mid0;                                                                \
                RegTensor<dstType> mid1;                                                                \
                RegTensor<dstType> mid2;                                                                \
                RegTensor<dstType> mid3;                                                                \
                MaskReg p0 = CreatePredicate<dstType>(len);                                             \
                DataCopy(input0, src, i* sregLower);                                                    \
                DataCopy(input1, src + (sregLower >> 2) * 1, i * sregLower);                            \
                DataCopy(input2, src + (sregLower >> 2) * 2, i * sregLower);                            \
                DataCopy(input3, src + (sregLower >> 2) * 3, i * sregLower);                            \
                DeInterleave(input0, input1, input0, input1);                                           \
                DeInterleave(input2, input3, input2, input3);                                           \
                DeInterleave(input0, input2, input0, input2);                                           \
                DeInterleave(input1, input3, input1, input3);                                           \
                CastAdaptor<dstType, srcType, rndMode, mode, satMode, PPMode::ZERO>(mid0, input0, p0);  \
                CastAdaptor<dstType, srcType, rndMode, mode, satMode, PPMode::ONE>(mid1, input1, p0);   \
                CastAdaptor<dstType, srcType, rndMode, mode, satMode, PPMode::TWO>(mid2, input2, p0);   \
                CastAdaptor<dstType, srcType, rndMode, mode, satMode, PPMode::THREE>(mid3, input3, p0); \
                Or(mid0, mid0, mid1, p0);                                                               \
                Or(mid2, mid2, mid3, p0);                                                               \
                Or(output, mid0, mid2, p0);                                                             \
                DataCopy(dst, output, i* sregLower, p0);                                                \
            }                                                                                           \
        }                                                                                               \
    }

// For Cast L2
#define CAST_UPPER_QUATER(srcType, dstType, srcBits, dstBits, rndStr, rndMode, satMode, mode)          \
    __aicore__ inline void CastIntrinsicsImpl##rndStr(                                                 \
        __ubuf__ dstType* dst, __ubuf__ srcType* src, const uint32_t calCount)                         \
    {                                                                                                  \
        __VEC_SCOPE__                                                                                  \
        {                                                                                              \
            uint32_t len = calCount;                                                                   \
            uint32_t sregLower = (uint32_t)(VECTOR_REG_WIDTH / sizeof(srcType));                       \
            uint16_t repeatTimes = CeilDivision(calCount, sregLower);                                  \
            for (uint16_t i = 0; i < (uint16_t)repeatTimes; i++) {                                     \
                RegTensor<srcType> input;                                                              \
                RegTensor<dstType> output0;                                                            \
                RegTensor<dstType> output1;                                                            \
                RegTensor<dstType> output2;                                                            \
                RegTensor<dstType> output3;                                                            \
                RegTensor<dstType> mid0;                                                               \
                RegTensor<dstType> mid1;                                                               \
                RegTensor<dstType> mid2;                                                               \
                RegTensor<dstType> mid3;                                                               \
                RegTensor<dstType> mid4;                                                               \
                RegTensor<dstType> mid5;                                                               \
                RegTensor<dstType> mid6;                                                               \
                RegTensor<dstType> mid7;                                                               \
                MaskReg p0 = CreatePredicate<srcType>(len);                                            \
                MaskReg p1;                                                                            \
                MaskReg p2;                                                                            \
                MaskReg p11;                                                                           \
                MaskReg p12;                                                                           \
                MaskReg p21;                                                                           \
                MaskReg p22;                                                                           \
                DataCopy(input, src, i* sregLower);                                                    \
                CastAdaptor<dstType, srcType, rndMode, mode, satMode, PPMode::ZERO>(mid0, input, p0);  \
                CastAdaptor<dstType, srcType, rndMode, mode, satMode, PPMode::ONE>(mid1, input, p0);   \
                CastAdaptor<dstType, srcType, rndMode, mode, satMode, PPMode::TWO>(mid2, input, p0);   \
                CastAdaptor<dstType, srcType, rndMode, mode, satMode, PPMode::THREE>(mid3, input, p0); \
                Interleave(mid4, mid5, mid0, mid2);                                                    \
                Interleave(mid6, mid7, mid1, mid3);                                                    \
                Interleave(output0, output1, mid4, mid6);                                              \
                Interleave(output2, output3, mid5, mid7);                                              \
                PredicateUnPack<HiloPart::Lower>(p1, p0);                                              \
                PredicateUnPack<HiloPart::Lower>(p11, p1);                                             \
                PredicateUnPack<HiloPart::Higher>(p12, p1);                                            \
                PredicateUnPack<HiloPart::Higher>(p2, p0);                                             \
                PredicateUnPack<HiloPart::Lower>(p21, p2);                                             \
                PredicateUnPack<HiloPart::Higher>(p22, p2);                                            \
                DataCopy(dst, output0, i* sregLower, p11);                                             \
                DataCopy(dst + (sregLower >> 2) * 1, output1, i * sregLower, p12);                     \
                DataCopy(dst + (sregLower >> 2) * 2, output2, i * sregLower, p21);                     \
                DataCopy(dst + (sregLower >> 2) * 3, output3, i * sregLower, p22);                     \
            }                                                                                          \
        }                                                                                              \
    }

// For Cast L2
#define CAST_EQUAL(srcType, dstType, srcBits, dstBits, rndStr, rndMode, satMode, mode)                       \
    __aicore__ inline void CastIntrinsicsImpl##rndStr(                                                       \
        __ubuf__ dstType* dst, __ubuf__ srcType* src, const uint32_t calCount)                               \
    {                                                                                                        \
        __VEC_SCOPE__                                                                                        \
        {                                                                                                    \
            uint32_t len = calCount;                                                                         \
            uint32_t sregLower = (uint32_t)(VECTOR_REG_WIDTH / sizeof(srcType));                             \
            uint16_t repeatTimes = CeilDivision(calCount, sregLower);                                        \
            for (uint16_t i = 0; i < (uint16_t)repeatTimes; i++) {                                           \
                RegTensor<srcType> input;                                                                    \
                RegTensor<dstType> output;                                                                   \
                MaskReg p0 = CreatePredicate<dstType>(len);                                                  \
                DataCopy(input, src, i* sregLower);                                                          \
                CastAdaptor<dstType, srcType, rndMode, mode, satMode, PartMode::UNKNOWN>(output, input, p0); \
                DataCopy(dst, output, i* sregLower, p0);                                                     \
            }                                                                                                \
        }                                                                                                    \
    }

// For Truncate L2
#define CAST_TRUNCATE(dType, rndStr, rndMode, mode)                            \
    __aicore__ inline void CastIntrinsicsImpl##rndStr(                         \
        __ubuf__ dType* dst, __ubuf__ dType* src, const uint32_t calCount)     \
    {                                                                          \
        __VEC_SCOPE__                                                          \
        {                                                                      \
            uint32_t len = calCount;                                           \
            uint32_t sregLower = (uint32_t)(VECTOR_REG_WIDTH / sizeof(dType)); \
            uint16_t repeatTimes = CeilDivision(calCount, sregLower);          \
            for (uint16_t i = 0; i < (uint16_t)repeatTimes; i++) {             \
                RegTensor<dType> input;                                        \
                RegTensor<dType> output;                                       \
                MaskReg p0 = CreatePredicate<dType>(len);                      \
                DataCopy(input, src, i* sregLower);                            \
                Truncate<dType, rndMode, mode>(output, input, p0);             \
                DataCopy(dst, output, i* sregLower, p0);                       \
            }                                                                  \
        }                                                                      \
    }

#define CAST_TO_EQUAL(dstType, srcType, rndMode, satMode, mode) \
    CastAdaptor<dstType, srcType, rndMode, mode, satMode, PartMode::UNKNOWN>(vreg1, vreg0, preg)

#define LOWER_TO_HALF(dstType, srcType, rndMode, satMode, mode) \
    CastAdaptor<dstType, srcType, rndMode, mode, satMode, PartMode::EVEN>(vreg1, vreg0, preg)

#define UPPER_TO_HALF(dstType, srcType, rndMode, satMode, mode) \
    CastAdaptor<dstType, srcType, rndMode, mode, satMode, PartMode::EVEN>(vreg1, vreg0, preg_dst)

#define LOWER_TO_QUATER(dstType, srcType, rndMode, satMode, mode) \
    CastAdaptor<dstType, srcType, rndMode, mode, satMode, PPMode::ZERO>(vreg1, vreg0, preg)

#define UPPER_TO_QUATER(dstType, srcType, rndMode, satMode, mode) \
    CastAdaptor<dstType, srcType, rndMode, mode, satMode, PPMode::ZERO>(vreg1, vreg0, preg_dst)

#define TRUNCATE_ROUND(dstType, srcType, rndMode, satMode, mode) Truncate<dstType, rndMode, mode>(vreg1, vreg0, preg)

#define REGISTER_CAST_LOWER_HALF(rndStr, rndMode, srcType, dstType, srcBits, dstBits, satMode, mode) \
    CAST_LOWER_HALF(srcType, dstType, srcBits, dstBits, rndStr, rndMode, satMode, mode)

#define REGISTER_CAST_UPPER_HALF(rndStr, rndMode, srcType, dstType, srcBits, dstBits, satMode, mode) \
    CAST_UPPER_HALF(srcType, dstType, srcBits, dstBits, rndStr, rndMode, satMode, mode)

#define REGISTER_CAST_LOWER_QUATER(rndStr, rndMode, srcType, dstType, srcBits, dstBits, satMode, mode) \
    CAST_LOWER_QUATER(srcType, dstType, srcBits, dstBits, rndStr, rndMode, satMode, mode)

#define REGISTER_CAST_S322HALF(rndStr, rndMode, srcType, dstType, srcBits, dstBits, satMode, mode) \
    CAST_S322HALF(srcType, dstType, srcBits, dstBits, rndStr, rndMode, satMode, mode)

#define REGISTER_CAST_HALF2S4(rndStr, rndMode, srcType, dstType, srcBits, dstBits, satMode, mode) \
    CAST_HALF2S4(srcType, dstType, srcBits, dstBits, rndStr, rndMode, satMode, mode)

#define REGISTER_CAST_UPPER_QUATER(rndStr, rndMode, srcType, dstType, srcBits, dstBits, satMode, mode) \
    CAST_UPPER_QUATER(srcType, dstType, srcBits, dstBits, rndStr, rndMode, satMode, mode)

#define REGISTER_CAST_EQUAL(rndStr, rndMode, srcType, dstType, srcBits, dstBits, satMode, mode) \
    CAST_EQUAL(srcType, dstType, srcBits, dstBits, rndStr, rndMode, satMode, mode)

#define REGISTER_CAST_TRUNCATE(rndStr, rndMode, srcType, dstType, srcBits, dstBits, satMode, mode) \
    CAST_TRUNCATE(srcType, rndStr, rndMode, mode)

#define REGISTER_CAST_LV2_NOT_SUPPORTED(rndStr, srcType, dstType)                                                \
    __aicore__ inline void CastIntrinsicsImpl##rndStr(                                                           \
        __ubuf__ dstType* dst, __ubuf__ srcType* src, const uint32_t calCount)                                   \
    {                                                                                                            \
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "rndStr from srcType to dstType not supported!"); }); \
    }

#define REGISTER_CAST_NOT_SUPPORTED(rndStr, srcType, dstType) REGISTER_CAST_LV2_NOT_SUPPORTED(rndStr, srcType, dstType)

// ROUND GROUP 0
// support CAST_RINT, CAST_FLOOR, CAST_CEIL, CAST_ROUND, CAST_TRUNC, CAST_ODD
#define REGISTER_CAST_ROUND_GROUP0(sizeMode, srcType, dstType, srcBits, dstBits, satMode, mode)                    \
    REGISTER_CAST_##sizeMode(CastRound, RoundMode::CAST_ROUND, srcType, dstType, srcBits, dstBits, satMode, mode); \
    REGISTER_CAST_##sizeMode(CastRint, RoundMode::CAST_RINT, srcType, dstType, srcBits, dstBits, satMode, mode);   \
    REGISTER_CAST_##sizeMode(CastFloor, RoundMode::CAST_FLOOR, srcType, dstType, srcBits, dstBits, satMode, mode); \
    REGISTER_CAST_##sizeMode(CastTrunc, RoundMode::CAST_TRUNC, srcType, dstType, srcBits, dstBits, satMode, mode); \
    REGISTER_CAST_##sizeMode(CastCeil, RoundMode::CAST_CEIL, srcType, dstType, srcBits, dstBits, satMode, mode);   \
    REGISTER_CAST_##sizeMode(CastNone, RoundMode::CAST_RINT, srcType, dstType, srcBits, dstBits, satMode, mode);   \
    REGISTER_CAST_##sizeMode(CastOdd, RoundMode::CAST_ODD, srcType, dstType, srcBits, dstBits, satMode, mode)

// ROUND GROUP 1
// support CAST_RINT, CAST_FLOOR, CAST_CEIL, CAST_ROUND, CAST_TRUNC
// not support CAST_ODD
#define REGISTER_CAST_ROUND_GROUP1(sizeMode, srcType, dstType, srcBits, dstBits, satMode, mode)                    \
    REGISTER_CAST_##sizeMode(CastRound, RoundMode::CAST_ROUND, srcType, dstType, srcBits, dstBits, satMode, mode); \
    REGISTER_CAST_##sizeMode(CastRint, RoundMode::CAST_RINT, srcType, dstType, srcBits, dstBits, satMode, mode);   \
    REGISTER_CAST_##sizeMode(CastFloor, RoundMode::CAST_FLOOR, srcType, dstType, srcBits, dstBits, satMode, mode); \
    REGISTER_CAST_##sizeMode(CastTrunc, RoundMode::CAST_TRUNC, srcType, dstType, srcBits, dstBits, satMode, mode); \
    REGISTER_CAST_##sizeMode(CastCeil, RoundMode::CAST_CEIL, srcType, dstType, srcBits, dstBits, satMode, mode);   \
    REGISTER_CAST_##sizeMode(CastNone, RoundMode::CAST_RINT, srcType, dstType, srcBits, dstBits, satMode, mode);   \
    REGISTER_CAST_NOT_SUPPORTED(CastOdd, srcType, dstType)

// ROUND GROUP 2
// support CAST_NONE
#define REGISTER_CAST_ROUND_GROUP2(sizeMode, srcType, dstType, srcBits, dstBits, satMode, mode)                  \
    REGISTER_CAST_NOT_SUPPORTED(CastRound, srcType, dstType);                                                    \
    REGISTER_CAST_NOT_SUPPORTED(CastRint, srcType, dstType);                                                     \
    REGISTER_CAST_NOT_SUPPORTED(CastFloor, srcType, dstType);                                                    \
    REGISTER_CAST_NOT_SUPPORTED(CastTrunc, srcType, dstType);                                                    \
    REGISTER_CAST_NOT_SUPPORTED(CastCeil, srcType, dstType);                                                     \
    REGISTER_CAST_##sizeMode(CastNone, RoundMode::CAST_NONE, srcType, dstType, srcBits, dstBits, satMode, mode); \
    REGISTER_CAST_NOT_SUPPORTED(CastOdd, srcType, dstType)

REGISTER_CAST_ROUND_GROUP1(UPPER_HALF, half, int32_t, 16, 32, SatMode::UNKNOWN, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP0(LOWER_HALF, float, half, 32, 16, SatMode::SAT, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP1(LOWER_HALF, float, int16_t, 32, 16, SatMode::SAT, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP1(LOWER_HALF, half, int8_t, 16, 8, SatMode::SAT, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP1(LOWER_HALF, half, uint8_t, 16, 8, SatMode::SAT, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP1(EQUAL, half, int16_t, 16, 16, SatMode::SAT, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP1(EQUAL, float, int32_t, 32, 32, SatMode::SAT, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP2(UPPER_HALF, half, float, 16, 32, SatMode::UNKNOWN, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP2(UPPER_HALF, uint8_t, half, 8, 16, SatMode::UNKNOWN, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP2(UPPER_HALF, int8_t, half, 8, 16, SatMode::UNKNOWN, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP2(UPPER_HALF, int16_t, float, 16, 32, SatMode::UNKNOWN, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP2(UPPER_HALF, uint8_t, uint16_t, 8, 16, SatMode::UNKNOWN, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP2(UPPER_HALF, int8_t, int16_t, 8, 16, SatMode::UNKNOWN, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP2(UPPER_HALF, uint16_t, uint32_t, 16, 32, SatMode::UNKNOWN, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP2(UPPER_HALF, int16_t, uint32_t, 16, 32, SatMode::UNKNOWN, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP2(UPPER_HALF, int16_t, int32_t, 16, 32, SatMode::UNKNOWN, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP2(UPPER_QUATER, int8_t, int32_t, 8, 32, SatMode::UNKNOWN, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP2(UPPER_QUATER, uint8_t, uint32_t, 8, 32, SatMode::UNKNOWN, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP2(LOWER_HALF, uint16_t, uint8_t, 16, 8, SatMode::SAT, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP2(LOWER_HALF, int16_t, uint8_t, 16, 8, SatMode::SAT, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP2(LOWER_HALF, uint32_t, uint16_t, 32, 16, SatMode::SAT, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP2(LOWER_HALF, uint32_t, int16_t, 32, 16, SatMode::SAT, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP2(LOWER_HALF, int32_t, uint16_t, 32, 16, SatMode::SAT, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP2(LOWER_HALF, int32_t, int16_t, 32, 16, SatMode::SAT, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP2(LOWER_QUATER, int32_t, uint8_t, 32, 8, SatMode::SAT, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP2(LOWER_QUATER, uint32_t, uint8_t, 32, 8, SatMode::SAT, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP1(EQUAL, int16_t, half, 16, 16, SatMode::UNKNOWN, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP1(EQUAL, int32_t, float, 32, 32, SatMode::UNKNOWN, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP1(TRUNCATE, half, half, 16, 16, SatMode::UNKNOWN, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP1(TRUNCATE, float, float, 32, 32, SatMode::UNKNOWN, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP1(HALF2S4, half, int4b_t, 16, 4, SatMode::SAT, Mode::ZEROING);
REGISTER_CAST_ROUND_GROUP1(S322HALF, int32_t, half, 32, 16, SatMode::SAT, Mode::ZEROING);

#define REGISTER_DATA_TYPE_NOT_SUPPORT(rndStr)                                                                   \
    template <typename U, typename T>                                                                            \
    __aicore__ inline void CastIntrinsicsImpl##rndStr(__ubuf__ U* dst, __ubuf__ T* src, const uint32_t calCount) \
    {                                                                                                            \
        ASCENDC_ASSERT((false), "current convert is not supported");                                             \
    }

REGISTER_DATA_TYPE_NOT_SUPPORT(CastRint);
REGISTER_DATA_TYPE_NOT_SUPPORT(CastFloor);
REGISTER_DATA_TYPE_NOT_SUPPORT(CastTrunc);
REGISTER_DATA_TYPE_NOT_SUPPORT(CastCeil);
REGISTER_DATA_TYPE_NOT_SUPPORT(CastRound);
REGISTER_DATA_TYPE_NOT_SUPPORT(CastNone);
REGISTER_DATA_TYPE_NOT_SUPPORT(CastOdd);

// Cast::Level 2
template <typename U, typename T>
__aicore__ inline void CastImpl(__ubuf__ U* dst, __ubuf__ T* src, const RoundMode& roundMode, const uint32_t calCount)
{
    switch (roundMode) {
        case RoundMode::CAST_RINT:
            CastIntrinsicsImplCastRint(dst, src, calCount);
            break;
        case RoundMode::CAST_FLOOR:
            CastIntrinsicsImplCastFloor(dst, src, calCount);
            break;
        case RoundMode::CAST_CEIL:
            CastIntrinsicsImplCastCeil(dst, src, calCount);
            break;
        case RoundMode::CAST_ROUND:
            CastIntrinsicsImplCastRound(dst, src, calCount);
            break;
        case RoundMode::CAST_TRUNC:
            CastIntrinsicsImplCastTrunc(dst, src, calCount);
            break;
        case RoundMode::CAST_ODD:
            CastIntrinsicsImplCastOdd(dst, src, calCount);
            break;
        case RoundMode::CAST_NONE:
            CastIntrinsicsImplCastNone(dst, src, calCount);
            break;
        default:
            ASCENDC_ASSERT(
                (false), { KERNEL_LOG(KERNEL_ERROR, "illegal input cast mode %d", static_cast<int32_t>(roundMode)); });
            break;
    }
}

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode>
__simd_vf__ inline void CastIntrinsicsB64ImplVF2(
    __ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const BasicAPIMaskStruct maskArrayStruct, uint8_t repeatTime,
    const UnaryRepeatParams repeatParams)
{
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, roundMode};
    constexpr uint8_t elePerBlk = GetDataBlockSizeInBytes() / sizeof(uint32_t);
    Reg::RegTensor<SRC_TYPE> srcVreg;
    Reg::RegTensor<DST_TYPE> dstVreg;
    Reg::RegTensor<uint32_t> zeroVreg, tmpVreg;
    Reg::MaskReg fullPreg = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::MaskReg b32Preg = Reg::MoveMask<uint32_t>();
    Reg::MaskReg b64Preg, tmpPreg;
    Reg::MaskInterleave<uint32_t>(b64Preg, tmpPreg, b32Preg, b32Preg);
    Reg::Duplicate(zeroVreg, 0, fullPreg);
    for (uint16_t i = 0; i < repeatTime; ++i) {
        if constexpr (sizeof(DST_TYPE) == sizeof(int64_t)) {
            // b32->b64
            Reg::DataCopy<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (Reg::RegTensor<uint32_t>&)srcVreg,
                (__ubuf__ uint32_t*&)src + i * repeatParams.srcRepStride * elePerBlk, repeatParams.srcBlkStride,
                b32Preg);
            Reg::Interleave((Reg::RegTensor<uint32_t>&)srcVreg, tmpVreg, (Reg::RegTensor<uint32_t>&)srcVreg, zeroVreg);
        } else {
            // b64->b32
            Reg::DataCopy<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (Reg::RegTensor<uint32_t>&)srcVreg,
                (__ubuf__ uint32_t*&)src + i * repeatParams.srcRepStride * elePerBlk, repeatParams.srcBlkStride,
                b64Preg);
        }
        Reg::Cast<DST_TYPE, SRC_TYPE, castTrait>(dstVreg, srcVreg, b64Preg);
        if constexpr (sizeof(DST_TYPE) == sizeof(int64_t)) {
            // b32->b64
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_STORE>();
            Reg::DataCopy<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (__ubuf__ uint32_t*&)dst + i * repeatParams.dstRepStride * elePerBlk,
                (Reg::RegTensor<uint32_t>&)dstVreg, repeatParams.dstBlkStride, b64Preg);
        } else {
            // b64->b32
            Reg::DeInterleave(
                (Reg::RegTensor<uint32_t>&)dstVreg, tmpVreg, (Reg::RegTensor<uint32_t>&)dstVreg, zeroVreg);
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_STORE>();
            Reg::DataCopy<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (__ubuf__ uint32_t*&)dst + i * repeatParams.dstRepStride * elePerBlk,
                (Reg::RegTensor<uint32_t>&)dstVreg, repeatParams.dstBlkStride, b32Preg);
        }
    }
}

template <typename DST_TYPE, typename SRC_TYPE>
__simd_callee__ inline void GenLoadL0(
    Reg::RegTensor<SRC_TYPE>& srcVreg, __ubuf__ SRC_TYPE*& srcAddr, Reg::MaskReg& preg,
    const UnaryRepeatParams& repeatParams)

{
    Reg::DataCopy<SRC_TYPE, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
        srcVreg, srcAddr, static_cast<uint32_t>(repeatParams.srcBlkStride),
        static_cast<uint32_t>(repeatParams.srcRepStride), preg);
    if constexpr (SupportType<SRC_TYPE, int4b_t>() && sizeof(DST_TYPE) == 2) {
        Reg::UnPack<uint16_t, uint8_t>((Reg::RegTensor<uint16_t>&)srcVreg, (Reg::RegTensor<uint8_t>&)srcVreg);
        Reg::UnPack<uint32_t, uint16_t>((Reg::RegTensor<uint32_t>&)srcVreg, (Reg::RegTensor<uint16_t>&)srcVreg);
    } else if constexpr (sizeof(SRC_TYPE) == 1 && sizeof(DST_TYPE) == 2) {
        if constexpr (Std::is_same_v<SRC_TYPE, int8_t>) {
            Reg::UnPack<int16_t, int8_t>((Reg::RegTensor<int16_t>&)srcVreg, srcVreg);
        } else {
            Reg::UnPack<uint16_t, uint8_t>((Reg::RegTensor<uint16_t>&)srcVreg, (Reg::RegTensor<uint8_t>&)srcVreg);
        }
    } else if constexpr (sizeof(SRC_TYPE) == 2 && sizeof(DST_TYPE) == 4) {
        if constexpr (Std::is_same_v<SRC_TYPE, int16_t>) {
            Reg::UnPack<int32_t, int16_t>((Reg::RegTensor<int32_t>&)srcVreg, srcVreg);
        } else {
            Reg::UnPack<uint32_t, uint16_t>((Reg::RegTensor<uint32_t>&)srcVreg, (Reg::RegTensor<uint16_t>&)srcVreg);
        }
    } else if constexpr (sizeof(SRC_TYPE) == 1 && sizeof(DST_TYPE) == 4) {
        if constexpr (Std::is_same_v<SRC_TYPE, int8_t>) {
            Reg::UnPack<int16_t, int8_t>((Reg::RegTensor<int16_t>&)srcVreg, srcVreg);
            Reg::UnPack<int32_t, int16_t>((Reg::RegTensor<int32_t>&)srcVreg, (Reg::RegTensor<int16_t>&)srcVreg);
        } else {
            Reg::UnPack<uint16_t, uint8_t>((Reg::RegTensor<uint16_t>&)srcVreg, (Reg::RegTensor<uint8_t>&)srcVreg);
            Reg::UnPack<uint32_t, uint16_t>((Reg::RegTensor<uint32_t>&)srcVreg, (Reg::RegTensor<uint16_t>&)srcVreg);
        }
    }
}

template <typename DST_TYPE, typename SRC_TYPE>
__simd_callee__ inline void GenStoreL0(
    __ubuf__ DST_TYPE*& dstAddr, Reg::RegTensor<DST_TYPE>& dstVreg, Reg::MaskReg& preg,
    const UnaryRepeatParams& repeatParams)
{
    if constexpr (SupportType<DST_TYPE, int4b_t>() && sizeof(SRC_TYPE) == 2) {
        Reg::Pack<uint16_t, uint32_t>((Reg::RegTensor<uint16_t>&)dstVreg, (Reg::RegTensor<uint32_t>&)dstVreg);
        Reg::Pack<uint8_t, uint16_t>((Reg::RegTensor<uint8_t>&)dstVreg, (Reg::RegTensor<uint16_t>&)dstVreg);
    } else if constexpr (sizeof(DST_TYPE) == 1 && sizeof(SRC_TYPE) == 2) {
        Reg::Pack<uint8_t, uint16_t>((Reg::RegTensor<uint8_t>&)dstVreg, (Reg::RegTensor<uint16_t>&)dstVreg);
    } else if constexpr (sizeof(DST_TYPE) == 2 && sizeof(SRC_TYPE) == 4) {
        Reg::Pack<uint16_t, uint32_t>((Reg::RegTensor<uint16_t>&)dstVreg, (Reg::RegTensor<uint32_t>&)dstVreg);
    } else if constexpr (sizeof(DST_TYPE) == 1 && sizeof(SRC_TYPE) == 4) {
        Reg::Pack<uint16_t, uint32_t>((Reg::RegTensor<uint16_t>&)dstVreg, (Reg::RegTensor<uint32_t>&)dstVreg);
        Reg::Pack<uint8_t, uint16_t>((Reg::RegTensor<uint8_t>&)dstVreg, (Reg::RegTensor<uint16_t>&)dstVreg);
    }
    Reg::DataCopy<DST_TYPE, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
        dstAddr, dstVreg, static_cast<uint32_t>(repeatParams.dstBlkStride),
        static_cast<uint32_t>(repeatParams.dstRepStride), preg);
}

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode>
__simd_vf__ inline void CastIntrinsicsImplVF2(
    __ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const BasicAPIMaskStruct maskArrayStruct, uint8_t repeatTimes,
    const UnaryRepeatParams repeatParams)
{
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, roundMode};
    Reg::MaskReg ldPreg;
    Reg::MaskReg exPreg;
    Reg::MaskReg stPreg;
    Reg::MaskReg dumpPreg;
    Reg::RegTensor<SRC_TYPE> srcVreg;
    Reg::RegTensor<DST_TYPE> dstVreg;
    if constexpr (sizeof(DST_TYPE) == sizeof(SRC_TYPE)) {
        ldPreg = Reg::MoveMask<SRC_TYPE>();
        exPreg = ldPreg;
        stPreg = ldPreg;
    } else if constexpr (sizeof(DST_TYPE) < sizeof(SRC_TYPE)) {
        ldPreg = Reg::MoveMask<SRC_TYPE>();
        exPreg = ldPreg;
        Reg::MaskPack(stPreg, ldPreg);
        if constexpr (
            (SupportType<DST_TYPE, int4b_t>() && sizeof(SRC_TYPE) == 2) ||
            (sizeof(DST_TYPE) == 1 && sizeof(SRC_TYPE) == 4)) {
            Reg::MaskPack(stPreg, stPreg);
        }
    } else if constexpr (sizeof(DST_TYPE) > sizeof(SRC_TYPE)) {
        stPreg = Reg::MoveMask<DST_TYPE>();
        exPreg = stPreg;
        Reg::MaskPack(ldPreg, stPreg);
        if constexpr (
            (SupportType<SRC_TYPE, int4b_t>() && sizeof(DST_TYPE) == 2) ||
            (sizeof(SRC_TYPE) == 1 && sizeof(DST_TYPE) == 4)) {
            Reg::MaskPack(ldPreg, ldPreg);
            if constexpr (SupportType<SRC_TYPE, int4b_t>() && sizeof(DST_TYPE) == 2) {
                Reg::MaskUnPack(stPreg, ldPreg);
                Reg::MaskUnPack(exPreg, stPreg);
                Reg::MaskInterleave<uint16_t>(stPreg, dumpPreg, stPreg, stPreg);
            }
        }
    }
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        GenLoadL0<DST_TYPE, SRC_TYPE>(srcVreg, src, ldPreg, repeatParams);
        if constexpr (Std::is_same_v<SRC_TYPE, int32_t> && Std::is_same_v<DST_TYPE, half>) {
            Reg::Cast<float, SRC_TYPE, castTrait>((Reg::RegTensor<float>&)dstVreg, srcVreg, exPreg);
            float deqValueTmp = static_cast<float>(g_deqValue);
            Reg::Muls((Reg::RegTensor<float>&)dstVreg, (Reg::RegTensor<float>&)dstVreg, deqValueTmp, exPreg);
            Reg::Cast<DST_TYPE, float, castTrait>(dstVreg, (Reg::RegTensor<float>&)dstVreg, exPreg);
        } else if constexpr (Std::is_same_v<SRC_TYPE, float> && Std::is_same_v<DST_TYPE, float>) {
            Reg::Truncate<DST_TYPE, roundMode>(dstVreg, srcVreg, exPreg);
        } else {
            Reg::Cast<DST_TYPE, SRC_TYPE, castTrait>(dstVreg, srcVreg, exPreg);
        }
        GenStoreL0<DST_TYPE, SRC_TYPE>(dst, dstVreg, stPreg, repeatParams);
    }
}

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode, bool isSetMask>
__simd_vf__ inline void CastIntrinsicsImplCounterVF(
    __ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint64_t mask, __ubuf__ uint64_t* maskBuf,
    uint8_t repeatTimes, const UnaryRepeatParams repeatParams)
{
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, roundMode};
    uint32_t sreg = static_cast<uint32_t>(mask);
    Reg::MaskReg ldPreg;
    Reg::MaskReg exPreg;
    Reg::MaskReg stPreg;
    Reg::MaskReg dumpPreg;
    Reg::RegTensor<SRC_TYPE> srcVreg;
    Reg::RegTensor<DST_TYPE> dstVreg;
    uint32_t countSreg = static_cast<uint32_t>(mask);
    if constexpr (!isSetMask) {
        // get SPR.MASK in VF
        Reg::MaskReg sprLoadMaskReg = Reg::MoveMask<uint16_t>();
        Reg::DataCopy<uint64_t, Reg::MaskDist::DIST_PACK>(maskBuf, sprLoadMaskReg);
        // insert membar(vec store operation) before load maskBuf[0](scalar load operation)
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::SCALAR_LOAD>();
        countSreg = static_cast<uint32_t>(maskBuf[0]);
    }
    uint16_t oneRepSize = GetVecLen() / sizeof(SRC_TYPE);
    if constexpr (sizeof(SRC_TYPE) < sizeof(DST_TYPE)) {
        oneRepSize = GetVecLen() / sizeof(DST_TYPE);
    }
    uint16_t newRepeatTimes = CeilDivision(countSreg, oneRepSize);
    for (uint16_t i = 0; i < newRepeatTimes; ++i) {
        if constexpr (sizeof(DST_TYPE) == sizeof(SRC_TYPE)) {
            ldPreg = Reg::UpdateMask<SRC_TYPE>(countSreg);
            exPreg = ldPreg;
            stPreg = ldPreg;
        } else if constexpr (sizeof(DST_TYPE) < sizeof(SRC_TYPE)) {
            ldPreg = Reg::UpdateMask<SRC_TYPE>(countSreg);
            exPreg = ldPreg;
            Reg::MaskPack(stPreg, ldPreg);
            if constexpr (
                (SupportType<DST_TYPE, int4b_t>() && sizeof(SRC_TYPE) == 2) ||
                (sizeof(DST_TYPE) == 1 && sizeof(SRC_TYPE) == 4)) {
                Reg::MaskPack(stPreg, stPreg);
            }
        } else if constexpr (sizeof(DST_TYPE) > sizeof(SRC_TYPE)) {
            stPreg = Reg::UpdateMask<DST_TYPE>(countSreg);
            exPreg = stPreg;
            Reg::MaskPack(ldPreg, stPreg);
            if constexpr (
                (SupportType<SRC_TYPE, int4b_t>() && sizeof(DST_TYPE) == 2) ||
                (sizeof(SRC_TYPE) == 1 && sizeof(DST_TYPE) == 4)) {
                Reg::MaskPack(ldPreg, ldPreg);
                if constexpr (SupportType<SRC_TYPE, int4b_t>() && sizeof(DST_TYPE) == 2) {
                    Reg::MaskUnPack(stPreg, ldPreg);
                    Reg::MaskUnPack(exPreg, stPreg);
                    Reg::MaskInterleave<uint16_t>(stPreg, dumpPreg, stPreg, stPreg);
                }
            }
        }
        GenLoadL0<DST_TYPE, SRC_TYPE>(srcVreg, src, ldPreg, repeatParams);
        if constexpr (Std::is_same_v<SRC_TYPE, int32_t> && Std::is_same_v<DST_TYPE, half>) {
            Reg::Cast<float, SRC_TYPE, castTrait>((Reg::RegTensor<float>&)dstVreg, srcVreg, exPreg);
            float deqValueTmp = static_cast<float>(g_deqValue);
            Reg::Muls((Reg::RegTensor<float>&)dstVreg, (Reg::RegTensor<float>&)dstVreg, deqValueTmp, exPreg);
            Reg::Cast<DST_TYPE, float, castTrait>(dstVreg, (Reg::RegTensor<float>&)dstVreg, exPreg);
        } else if constexpr (Std::is_same_v<SRC_TYPE, float> && Std::is_same_v<DST_TYPE, float>) {
            Reg::Truncate<DST_TYPE, roundMode>(dstVreg, srcVreg, exPreg);
        } else {
            Reg::Cast<DST_TYPE, SRC_TYPE, castTrait>(dstVreg, srcVreg, exPreg);
        }
        GenStoreL0<DST_TYPE, SRC_TYPE>(dst, dstVreg, stPreg, repeatParams);
    }
}

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode, bool isSetMask>
__simd_vf__ inline void CastIntrinsicsB64ImplCounterVF(
    __ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint64_t mask, __ubuf__ uint64_t* maskBuf, uint8_t repeatTime,
    const UnaryRepeatParams repeatParams)
{
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, roundMode};
    constexpr uint8_t elePerBlk = GetDataBlockSizeInBytes() / sizeof(uint32_t);
    uint32_t countSreg = static_cast<uint32_t>(mask);
    if constexpr (!isSetMask) {
        // get SPR.MASK in VF
        Reg::MaskReg sprLoadMaskReg = Reg::MoveMask<uint16_t>();
        Reg::DataCopy<uint64_t, Reg::MaskDist::DIST_PACK>(maskBuf, sprLoadMaskReg);
        // insert membar(vec store operation) before load maskBuf[0](scalar load operation)
        Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::SCALAR_LOAD>();
        countSreg = static_cast<uint32_t>(maskBuf[0]);
    }
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(uint64_t);
    uint16_t newRepeatTimes = CeilDivision(countSreg, oneRepSize);
    Reg::RegTensor<SRC_TYPE> srcVreg;
    Reg::RegTensor<DST_TYPE> dstVreg;
    Reg::RegTensor<uint32_t> zeroVreg, tmpVreg;
    Reg::MaskReg b32Preg, b64Preg, tmpPreg;
    Reg::MaskReg fullPreg = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::Duplicate(zeroVreg, 0, fullPreg);
    for (uint16_t i = 0; i < newRepeatTimes; ++i) {
        b32Preg = Reg::UpdateMask<uint32_t>(countSreg);
        Reg::MaskInterleave<uint32_t>(b64Preg, tmpPreg, b32Preg, b32Preg);
        if constexpr (sizeof(DST_TYPE) == sizeof(int64_t)) {
            // b32->b64
            Reg::DataCopy<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (Reg::RegTensor<uint32_t>&)srcVreg,
                (__ubuf__ uint32_t*&)src + i * repeatParams.srcRepStride * elePerBlk, repeatParams.srcBlkStride,
                b32Preg);
            Reg::Interleave((Reg::RegTensor<uint32_t>&)srcVreg, tmpVreg, (Reg::RegTensor<uint32_t>&)srcVreg, zeroVreg);
        } else {
            // b64->b32
            Reg::DataCopy<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (Reg::RegTensor<uint32_t>&)srcVreg,
                (__ubuf__ uint32_t*&)src + i * repeatParams.srcRepStride * elePerBlk, repeatParams.srcBlkStride,
                b64Preg);
        }
        Reg::Cast<DST_TYPE, SRC_TYPE, castTrait>(dstVreg, srcVreg, b64Preg);
        if constexpr (sizeof(DST_TYPE) == sizeof(int64_t)) {
            // b32->b64
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_STORE>();
            Reg::DataCopy<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (__ubuf__ uint32_t*&)dst + i * repeatParams.dstRepStride * elePerBlk,
                (Reg::RegTensor<uint32_t>&)dstVreg, repeatParams.dstBlkStride, b64Preg);
        } else {
            // b64->b32
            Reg::DeInterleave(
                (Reg::RegTensor<uint32_t>&)dstVreg, tmpVreg, (Reg::RegTensor<uint32_t>&)dstVreg, zeroVreg);
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_STORE>();
            Reg::DataCopy<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (__ubuf__ uint32_t*&)dst + i * repeatParams.dstRepStride * elePerBlk,
                (Reg::RegTensor<uint32_t>&)dstVreg, repeatParams.dstBlkStride, b32Preg);
        }
    }
}

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode, bool isSetMask>
__aicore__ inline void CastIntrinsicsImpl(
    __ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint64_t mask[], uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    constexpr bool b64Cast = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<float, int64_t>, Tuple<int64_t, float>, Tuple<int32_t, int64_t>,
        Tuple<int64_t, int32_t>>();
    bool isCounterMode = Internal::IsCounterMode();

    if (isCounterMode) {
        __ubuf__ uint64_t* maskBuf = nullptr;
        if constexpr (!isSetMask) {
            maskBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(TMP_UB_OFFSET, 2);
        }
        if constexpr (b64Cast) {
            CastIntrinsicsB64ImplCounterVF<DST_TYPE, SRC_TYPE, roundMode, isSetMask>(
                dst, src, mask[0], maskBuf, repeatTime, repeatParams);
        } else {
            CastIntrinsicsImplCounterVF<DST_TYPE, SRC_TYPE, roundMode, isSetMask>(
                dst, src, mask[0], maskBuf, repeatTime, repeatParams);
        }
    } else {
        BasicAPIMaskStruct maskArrayStruct;
        if (mask != nullptr) {
            maskArrayStruct = *(reinterpret_cast<const BasicAPIMaskStruct*>(mask));
        }
        if constexpr (b64Cast) {
            if constexpr (isSetMask) {
                SetVectorMask<uint32_t>(mask[1], mask[0]);
            }
            CastIntrinsicsB64ImplVF2<DST_TYPE, SRC_TYPE, roundMode>(
                dst, src, maskArrayStruct, repeatTime, repeatParams);
        } else {
            if constexpr (isSetMask) {
                if constexpr (sizeof(DST_TYPE) < sizeof(SRC_TYPE)) {
                    SetVectorMask<SRC_TYPE>(mask[1], mask[0]);
                } else {
                    SetVectorMask<DST_TYPE>(mask[1], mask[0]);
                }
            }
            CastIntrinsicsImplVF2<DST_TYPE, SRC_TYPE, roundMode>(dst, src, maskArrayStruct, repeatTime, repeatParams);
        }
    }
}

// Cast::Level 0 - mask bit mode
template <typename DST_TYPE, typename SRC_TYPE, bool isSetMask = true>
__aicore__ inline void CastImpl(
    __ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const RoundMode& roundMode, const uint64_t mask[],
    uint8_t repeatTimes, const UnaryRepeatParams& repeatParams)
{
    constexpr bool cast_round_all = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<half, float>, Tuple<int32_t, float>, Tuple<int16_t, float>,
        Tuple<int32_t, half>, Tuple<int16_t, half>, Tuple<int8_t, half>, Tuple<uint8_t, half>, Tuple<int4b_t, half>,
        Tuple<half, int16_t>, Tuple<float, int32_t>, Tuple<half, int32_t>, Tuple<int64_t, float>,
        Tuple<float, int64_t>>();

    constexpr bool cast_none = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<float, half>, Tuple<half, int4b_t>, Tuple<half, uint8_t>,
        Tuple<uint16_t, uint8_t>, Tuple<uint32_t, uint8_t>, Tuple<half, int8_t>, Tuple<int16_t, int8_t>,
        Tuple<int32_t, int8_t>, Tuple<uint8_t, uint16_t>, Tuple<uint32_t, uint16_t>, Tuple<float, int16_t>,
        Tuple<uint8_t, int16_t>, Tuple<uint32_t, int16_t>, Tuple<int32_t, int16_t>, Tuple<uint8_t, uint32_t>,
        Tuple<uint16_t, uint32_t>, Tuple<int16_t, uint32_t>, Tuple<int16_t, int32_t>, Tuple<uint8_t, int32_t>,
        Tuple<uint16_t, int32_t>, Tuple<int64_t, int32_t>, Tuple<int32_t, int64_t>>();

    constexpr bool using_cast_rint = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<int8_t, half>, Tuple<uint8_t, half>, Tuple<int4b_t, half>, Tuple<half, float>,
        Tuple<half, int16_t>, Tuple<float, int32_t>>();
    constexpr bool cast_odd = SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<half, float>>();
    switch (roundMode) {
        case RoundMode::CAST_RINT:
            if constexpr (cast_round_all) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_RINT, isSetMask>(
                    dst, src, mask, repeatTimes, repeatParams);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast rint"); });
            }
            break;
        case RoundMode::CAST_FLOOR:
            if constexpr (cast_round_all) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_FLOOR, isSetMask>(
                    dst, src, mask, repeatTimes, repeatParams);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast floor"); });
            }
            break;
        case RoundMode::CAST_CEIL:
            if constexpr (cast_round_all) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_CEIL, isSetMask>(
                    dst, src, mask, repeatTimes, repeatParams);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast ceil"); });
            }
            break;
        case RoundMode::CAST_ROUND:
            if constexpr (cast_round_all) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_ROUND, isSetMask>(
                    dst, src, mask, repeatTimes, repeatParams);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast round"); });
            }
            break;
        case RoundMode::CAST_TRUNC:
            if constexpr (cast_round_all) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_TRUNC, isSetMask>(
                    dst, src, mask, repeatTimes, repeatParams);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast trunc"); });
            }
            break;
        case RoundMode::CAST_ODD:
            if constexpr (cast_odd) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_ODD, isSetMask>(
                    dst, src, mask, repeatTimes, repeatParams);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast odd"); });
            }
            break;
        case RoundMode::CAST_NONE:
            if constexpr (cast_none) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_NONE, isSetMask>(
                    dst, src, mask, repeatTimes, repeatParams);
            } else if constexpr (using_cast_rint) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_RINT, isSetMask>(
                    dst, src, mask, repeatTimes, repeatParams);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast none"); });
            }
            break;
        default:
            ASCENDC_ASSERT(
                (false), { KERNEL_LOG(KERNEL_ERROR, "illegal input cast mode %d", static_cast<int32_t>(roundMode)); });
            break;
    }
}

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode, bool isSetMask>
__simd_vf__ inline void CastIntrinsicsB64ImplVF1(
    __ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams repeatParams)
{
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, roundMode};
    constexpr uint8_t elePerBlk = GetDataBlockSizeInBytes() / sizeof(uint32_t);
    uint32_t b32Sreg = static_cast<uint32_t>(mask);
    uint32_t b64Sreg = static_cast<uint32_t>(2 * mask);
    Reg::RegTensor<SRC_TYPE> srcVreg;
    Reg::RegTensor<DST_TYPE> dstVreg;
    Reg::RegTensor<uint32_t> zeroVreg, tmpVreg;
    Reg::MaskReg b32Preg;
    Reg::MaskReg b64Preg, tmpPreg;
    if constexpr (isSetMask) {
        b32Preg = Reg::UpdateMask<uint32_t>(b32Sreg);
        b64Preg = Reg::UpdateMask<uint32_t>(b64Sreg);
    } else {
        b32Preg = Reg::MoveMask<uint32_t>();
        Reg::MaskInterleave<uint32_t>(b64Preg, tmpPreg, b32Preg, b32Preg);
    }
    Reg::MaskReg fullPreg = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    Reg::Duplicate(zeroVreg, 0, fullPreg);
    for (uint16_t i = 0; i < repeatTime; ++i) {
        if constexpr (sizeof(DST_TYPE) == sizeof(int64_t)) {
            // b32 -> b64
            Reg::DataCopy<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (Reg::RegTensor<uint32_t>&)srcVreg,
                (__ubuf__ uint32_t*&)src + i * repeatParams.srcRepStride * elePerBlk, repeatParams.srcBlkStride,
                b32Preg);
            Reg::Interleave((Reg::RegTensor<uint32_t>&)srcVreg, tmpVreg, (Reg::RegTensor<uint32_t>&)srcVreg, zeroVreg);
        } else {
            // b64 -> b32
            Reg::DataCopy<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (Reg::RegTensor<uint32_t>&)srcVreg,
                (__ubuf__ uint32_t*&)src + i * repeatParams.srcRepStride * elePerBlk, repeatParams.srcBlkStride,
                b64Preg);
        }
        Reg::Cast<DST_TYPE, SRC_TYPE, castTrait>(dstVreg, srcVreg, b64Preg);
        if constexpr (sizeof(DST_TYPE) == sizeof(int64_t)) {
            // b32 -> b64
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_STORE>();
            Reg::DataCopy<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (__ubuf__ uint32_t*&)dst + i * repeatParams.dstRepStride * elePerBlk,
                (Reg::RegTensor<uint32_t>&)dstVreg, repeatParams.dstBlkStride, b64Preg);
        } else {
            // b64 -> b32
            Reg::DeInterleave(
                (Reg::RegTensor<uint32_t>&)dstVreg, tmpVreg, (Reg::RegTensor<uint32_t>&)dstVreg, zeroVreg);
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_STORE>();
            Reg::DataCopy<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (__ubuf__ uint32_t*&)dst + i * repeatParams.dstRepStride * elePerBlk,
                (Reg::RegTensor<uint32_t>&)dstVreg, repeatParams.dstBlkStride, b32Preg);
        }
    }
}

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode, bool isSetMask>
__simd_vf__ inline void CastIntrinsicsImplVF1(
    __ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint64_t mask, uint8_t repeatTimes,
    const UnaryRepeatParams repeatParams)
{
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, roundMode};
    uint32_t sreg = static_cast<uint32_t>(mask);
    Reg::MaskReg ldPreg;
    Reg::MaskReg exPreg;
    Reg::MaskReg stPreg;
    Reg::MaskReg dumpPreg;
    Reg::RegTensor<SRC_TYPE> srcVreg;
    Reg::RegTensor<DST_TYPE> dstVreg;
    if constexpr (sizeof(DST_TYPE) == sizeof(SRC_TYPE)) {
        if constexpr (isSetMask) {
            ldPreg = Reg::UpdateMask<SRC_TYPE>(sreg);
        } else {
            ldPreg = Reg::MoveMask<SRC_TYPE>();
        }
        exPreg = ldPreg;
        stPreg = ldPreg;
    } else if constexpr (sizeof(DST_TYPE) < sizeof(SRC_TYPE)) {
        if constexpr (isSetMask) {
            ldPreg = Reg::UpdateMask<SRC_TYPE>(sreg);
        } else {
            ldPreg = Reg::MoveMask<SRC_TYPE>();
        }
        exPreg = ldPreg;
        Reg::MaskPack(stPreg, ldPreg);
        if constexpr (
            (SupportType<DST_TYPE, int4b_t>() && sizeof(SRC_TYPE) == 2) ||
            (sizeof(DST_TYPE) == 1 && sizeof(SRC_TYPE) == 4)) {
            Reg::MaskPack(stPreg, stPreg);
        }
    } else if constexpr (sizeof(DST_TYPE) > sizeof(SRC_TYPE)) {
        if constexpr (isSetMask) {
            stPreg = Reg::UpdateMask<DST_TYPE>(sreg);
        } else {
            stPreg = Reg::MoveMask<DST_TYPE>();
        }
        exPreg = stPreg;
        Reg::MaskPack(ldPreg, stPreg);
        if constexpr (
            (SupportType<SRC_TYPE, int4b_t>() && sizeof(DST_TYPE) == 2) ||
            (sizeof(SRC_TYPE) == 1 && sizeof(DST_TYPE) == 4)) {
            Reg::MaskPack(ldPreg, ldPreg);
            if constexpr (SupportType<SRC_TYPE, int4b_t>() && sizeof(DST_TYPE) == 2) {
                Reg::MaskUnPack(stPreg, ldPreg);
                Reg::MaskUnPack(exPreg, stPreg);
                Reg::MaskInterleave<uint16_t>(stPreg, dumpPreg, stPreg, stPreg);
            }
        }
    }
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        GenLoadL0<DST_TYPE, SRC_TYPE>(srcVreg, src, ldPreg, repeatParams);
        if constexpr (Std::is_same_v<SRC_TYPE, int32_t> && Std::is_same_v<DST_TYPE, half>) {
            Reg::Cast<float, SRC_TYPE, castTrait>((Reg::RegTensor<float>&)dstVreg, srcVreg, exPreg);
            float deqValueTmp = static_cast<float>(g_deqValue);
            Reg::Muls((Reg::RegTensor<float>&)dstVreg, (Reg::RegTensor<float>&)dstVreg, deqValueTmp, exPreg);
            Reg::Cast<DST_TYPE, float, castTrait>(dstVreg, (Reg::RegTensor<float>&)dstVreg, exPreg);
        } else if constexpr (Std::is_same_v<SRC_TYPE, float> && Std::is_same_v<DST_TYPE, float>) {
            Reg::Truncate<DST_TYPE, roundMode>(dstVreg, srcVreg, exPreg);
        } else {
            Reg::Cast<DST_TYPE, SRC_TYPE, castTrait>(dstVreg, srcVreg, exPreg);
        }
        GenStoreL0<DST_TYPE, SRC_TYPE>(dst, dstVreg, stPreg, repeatParams);
    }
}

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode, bool isSetMask>
__aicore__ inline void CastIntrinsicsImpl(
    __ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    constexpr bool b64Cast = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<float, int64_t>, Tuple<int64_t, float>, Tuple<int32_t, int64_t>,
        Tuple<int64_t, int32_t>>();
    bool isCounterMode = Internal::IsCounterMode();
    if (isCounterMode) {
        __ubuf__ uint64_t* maskBuf = nullptr;
        if constexpr (!isSetMask) {
            maskBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(TMP_UB_OFFSET, 2);
        }
        if constexpr (b64Cast) {
            CastIntrinsicsB64ImplCounterVF<DST_TYPE, SRC_TYPE, roundMode, isSetMask>(
                dst, src, mask, maskBuf, repeatTime, repeatParams);
        } else {
            CastIntrinsicsImplCounterVF<DST_TYPE, SRC_TYPE, roundMode, isSetMask>(
                dst, src, mask, maskBuf, repeatTime, repeatParams);
        }
    } else {
        if constexpr (b64Cast) {
            CastIntrinsicsB64ImplVF1<DST_TYPE, SRC_TYPE, roundMode, isSetMask>(
                dst, src, mask, repeatTime, repeatParams);
        } else {
            CastIntrinsicsImplVF1<DST_TYPE, SRC_TYPE, roundMode, isSetMask>(dst, src, mask, repeatTime, repeatParams);
        }
    }
}

// Cast::Level 0 - mask count mode
template <typename DST_TYPE, typename SRC_TYPE, bool isSetMask = true>
__aicore__ inline void CastImpl(
    __ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const RoundMode& roundMode, const uint64_t mask,
    uint8_t repeatTimes, const UnaryRepeatParams& repeatParams)
{
    constexpr bool cast_round_all = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<half, float>, Tuple<int32_t, float>, Tuple<int16_t, float>,
        Tuple<int32_t, half>, Tuple<int16_t, half>, Tuple<int8_t, half>, Tuple<uint8_t, half>, Tuple<int4b_t, half>,
        Tuple<half, int16_t>, Tuple<float, int32_t>, Tuple<half, int32_t>, Tuple<int64_t, float>,
        Tuple<float, int64_t>>();

    constexpr bool cast_none = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<float, half>, Tuple<half, int4b_t>, Tuple<half, uint8_t>,
        Tuple<uint16_t, uint8_t>, Tuple<uint32_t, uint8_t>, Tuple<half, int8_t>, Tuple<int16_t, int8_t>,
        Tuple<int32_t, int8_t>, Tuple<uint8_t, uint16_t>, Tuple<uint32_t, uint16_t>, Tuple<float, int16_t>,
        Tuple<uint8_t, int16_t>, Tuple<uint32_t, int16_t>, Tuple<int32_t, int16_t>, Tuple<uint8_t, uint32_t>,
        Tuple<uint16_t, uint32_t>, Tuple<int16_t, uint32_t>, Tuple<int16_t, int32_t>, Tuple<uint8_t, int32_t>,
        Tuple<uint16_t, int32_t>, Tuple<int64_t, int32_t>, Tuple<int32_t, int64_t>>();

    constexpr bool using_cast_rint = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<int8_t, half>, Tuple<uint8_t, half>, Tuple<int4b_t, half>, Tuple<half, float>,
        Tuple<half, int16_t>, Tuple<float, int32_t>>();

    constexpr bool cast_odd = SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<half, float>>();
    switch (roundMode) {
        case RoundMode::CAST_RINT:
            if constexpr (cast_round_all) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_RINT, isSetMask>(
                    dst, src, mask, repeatTimes, repeatParams);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast rint"); });
            }
            break;
        case RoundMode::CAST_FLOOR:
            if constexpr (cast_round_all) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_FLOOR, isSetMask>(
                    dst, src, mask, repeatTimes, repeatParams);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast floor"); });
            }
            break;
        case RoundMode::CAST_CEIL:
            if constexpr (cast_round_all) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_CEIL, isSetMask>(
                    dst, src, mask, repeatTimes, repeatParams);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast ceil"); });
            }
            break;
        case RoundMode::CAST_ROUND:
            if constexpr (cast_round_all) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_ROUND, isSetMask>(
                    dst, src, mask, repeatTimes, repeatParams);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast round"); });
            }
            break;
        case RoundMode::CAST_TRUNC:
            if constexpr (cast_round_all) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_TRUNC, isSetMask>(
                    dst, src, mask, repeatTimes, repeatParams);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast trunc"); });
            }
            break;
        case RoundMode::CAST_ODD:
            if constexpr (cast_odd) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_ODD, isSetMask>(
                    dst, src, mask, repeatTimes, repeatParams);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast odd"); });
            }
            break;
        case RoundMode::CAST_NONE:
            if constexpr (cast_none) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_NONE, isSetMask>(
                    dst, src, mask, repeatTimes, repeatParams);
            } else if constexpr (using_cast_rint) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_RINT, isSetMask>(
                    dst, src, mask, repeatTimes, repeatParams);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast none"); });
            }
            break;
        default:
            ASCENDC_ASSERT(
                (false), { KERNEL_LOG(KERNEL_ERROR, "illegal input cast mode %d", static_cast<int32_t>(roundMode)); });
            break;
    }
}

template <typename U, typename T, bool isVecDeq, bool halfBlock>
__aicore__ inline void CastDeqImpl(__ubuf__ U* dst, __ubuf__ T* src, const uint32_t calCount)
{
    ASCENDC_ASSERT((false), "CastDeq is not supported");
}

template <typename U, typename T, bool isSetMask = true, bool isVecDeq, bool halfBlock>
__aicore__ inline void CastDeqImpl(
    __ubuf__ U* dst, __ubuf__ T* src, const uint64_t mask[2], uint8_t repeatTimes,
    const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT((false), "CastDeq is not supported");
}

template <typename U, typename T, bool isSetMask = true, bool isVecDeq, bool halfBlock>
__aicore__ inline void CastDeqImpl(
    __ubuf__ U* dst, __ubuf__ T* src, const int32_t mask, uint8_t repeatTimes, const UnaryRepeatParams& repeatParams)
{
    ASCENDC_ASSERT((false), "CastDeq is not supported");
}

namespace RegAddReluCast {
template <typename T1, typename T2, typename RegT, typename RegU>
__simd_callee__ inline void AddReluCast(RegT& dstReg, RegU& src0Reg, RegU& src1Reg, Reg::MaskReg& mask)
{
    Reg::Add(src0Reg, src0Reg, src1Reg, mask);
    Reg::Maxs(src0Reg, src0Reg, static_cast<T2>(0), mask);
    if constexpr (IsSameType<T2, float>::value) {
        Reg::Cast<T1, T2, CastParam::AddReluCastTrait>(dstReg, src0Reg, mask);
        Reg::Pack<uint16_t, uint32_t, Reg::HighLowPart::LOWEST>(
            (Reg::RegTensor<uint16_t>&)dstReg, (Reg::RegTensor<uint32_t>&)dstReg);
    } else {
        if constexpr (IsSameType<T2, int16_t>::value) {
            Reg::RegTensor<half> tmpReg;
            Reg::Cast<half, int16_t, CastParam::s162HalfTrait>(tmpReg, src0Reg, mask);
            Reg::Cast<int8_t, half, CastParam::AddReluCastTrait>(dstReg, tmpReg, mask);
        } else {
            Reg::Cast<T1, T2, CastParam::AddReluCastTrait>(dstReg, src0Reg, mask);
        }
        Reg::Pack<uint8_t, uint16_t, Reg::HighLowPart::LOWEST>(
            (Reg::RegTensor<uint8_t>&)dstReg, (Reg::RegTensor<uint16_t>&)dstReg);
    }
}
} // namespace RegAddReluCast

template <typename T1, typename T2>
constexpr __aicore__ inline void CheckAddReluCastSupportType()
{
    static_assert(
        SupportType<Tuple<T1, T2>, Tuple<half, float>, Tuple<int8_t, half>, Tuple<int8_t, int16_t>>(),
        "Failed to check dtype in AddReluCast, current api support dtype combination is src: float, dst: half; "
        "src: half, dst: int8_t; src: int16_t, dst: int8_t.");
}
// AddReluCast::Level 0 - mask count mode
template <typename T1, typename T2, bool isSetMask = true>
__aicore__ inline void AddReluCastImpl(
    __ubuf__ T1* dst, __ubuf__ T2* src0, __ubuf__ T2* src1, const uint64_t mask, uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    CheckAddReluCastSupportType<T1, T2>();
    constexpr auto func = RegAddReluCast::AddReluCast<T1, T2, Reg::RegTensor<T1>, Reg::RegTensor<T2>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, false>(dst, src0, src1, nullptr, mask, repeatTime, repeatParams);
}

// AddReluCast::Level 0 - mask bit mode
template <typename T1, typename T2, bool isSetMask = true>
__aicore__ inline void AddReluCastImpl(
    __ubuf__ T1* dst, __ubuf__ T2* src0, __ubuf__ T2* src1, const uint64_t mask[], uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    CheckAddReluCastSupportType<T1, T2>();
    constexpr auto func = RegAddReluCast::AddReluCast<T1, T2, Reg::RegTensor<T1>, Reg::RegTensor<T2>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, true>(dst, src0, src1, mask, 0, repeatTime, repeatParams);
}

// AddReluCast::Level 2
template <typename T1, typename T2>
__simd_vf__ inline void AddReluCastImpl(__ubuf__ T1* dst, __ubuf__ T2* src0, __ubuf__ T2* src1, const uint32_t calCount)
{
    static_assert(
        SupportType<
            Tuple<T1, T2>, Tuple<half, float>, Tuple<int8_t, half>, Tuple<int8_t, int16_t>, Tuple<float, int64_t>,
            Tuple<int32_t, int64_t>>(),
        "Failed to check dtype in AddReluCast, current api "
        "support dtype combination is src: float, dst: half; src: half, dst: int8_t; src: int16_t, dst: int8_t; "
        "src: int64_t, dst : int32_t / float.");
    uint32_t sreg = static_cast<uint32_t>(calCount);
    const T2 scalarValue = 0;
    if constexpr (sizeof(T2) == 8) {
        constexpr uint32_t sregLower = static_cast<uint32_t>(B64_DATA_NUM_PER_REPEAT * 2);
        const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, sregLower));
        Reg::RegTensor<T1> vDstReg0;
        Reg::RegTensor<T2, Reg::RegTraitNumTwo> vDstReg1;
        Reg::RegTensor<T2, Reg::RegTraitNumTwo> vSrcReg0;
        Reg::RegTensor<T2, Reg::RegTraitNumTwo> vSrcReg1;
        Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            mask = Reg::UpdateMask<T2, Reg::RegTraitNumTwo>(sreg);
            Reg::LoadAlign(vSrcReg0, src0 + i * sregLower);
            Reg::LoadAlign(vSrcReg1, src1 + i * sregLower);
            Reg::Add(vDstReg1, vSrcReg0, vSrcReg1, mask);
            Reg::Maxs(vDstReg1, vDstReg1, scalarValue, mask);
            Reg::Cast<T1, T2, CastParam::AddReluCastTrait>(vDstReg0, vDstReg1, mask);
            Reg::StoreAlign(dst + i * sregLower, vDstReg0, mask);
        }
    } else {
        constexpr uint32_t sregLower = static_cast<uint32_t>(GetVecLen() / sizeof(T2));
        const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, sregLower));
        Reg::RegTensor<T1> dst0Reg;
        Reg::RegTensor<T2> dst1Reg;
        Reg::RegTensor<T2> src0Reg;
        Reg::RegTensor<T2> src1Reg;
        Reg::MaskReg preg;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            preg = Reg::UpdateMask<T2>(sreg);
            Reg::LoadAlign<T2>(src0Reg, src0 + i * sregLower);
            Reg::LoadAlign<T2>(src1Reg, src1 + i * sregLower);
            Reg::Add<T2>(dst1Reg, src0Reg, src1Reg, preg);
            Reg::Maxs<T2>(dst1Reg, dst1Reg, scalarValue, preg);
            if constexpr (IsSameType<T2, float>::value) {
                Reg::Cast<T1, T2, CastParam::AddReluCastTrait>(dst0Reg, dst1Reg, preg);
                Reg::StoreAlign<T1, Reg::StoreDist::DIST_PACK_B32>(dst + i * sregLower, dst0Reg, preg);
            } else {
                if constexpr (IsSameType<T2, int16_t>::value) {
                    Reg::RegTensor<half> tmpReg;
                    Reg::Cast<half, int16_t, CastParam::s162HalfTrait>(tmpReg, dst1Reg, preg);
                    Reg::Cast<int8_t, half, CastParam::AddReluCastTrait>(dst0Reg, tmpReg, preg);
                } else {
                    Reg::Cast<T1, T2, CastParam::AddReluCastTrait>(dst0Reg, dst1Reg, preg);
                }
                Reg::StoreAlign<T1, Reg::StoreDist::DIST_PACK_B16>(dst + i * sregLower, dst0Reg, preg);
            }
        }
    }
}

namespace RegSubReluCast {
template <typename T1, typename T2, typename RegT, typename RegU>
__simd_callee__ inline void SubReluCast(RegT& dstReg, RegU& src0Reg, RegU& src1Reg, Reg::MaskReg& mask)
{
    Reg::Sub(src0Reg, src0Reg, src1Reg, mask);
    Reg::Maxs<T2>(src0Reg, src0Reg, static_cast<T2>(0), mask);
    if constexpr (IsSameType<T2, float>::value) {
        Reg::Cast<T1, T2, layoutZSatSMrgZRndR>(dstReg, src0Reg, mask);
        Reg::Pack((Reg::RegTensor<uint16_t>&)dstReg, (Reg::RegTensor<uint32_t>&)dstReg);
    } else {
        if constexpr (IsSameType<T2, int16_t>::value) {
            Reg::RegTensor<half> tmpReg;
            Reg::Cast<half, int16_t, MrgZRndRSatS>(tmpReg, src0Reg, mask);
            Reg::Cast<int8_t, half, layoutZSatSMrgZRndR>(dstReg, tmpReg, mask);
        } else {
            Reg::Cast<T1, T2, layoutZSatSMrgZRndR>(dstReg, src0Reg, mask);
        }
        Reg::Pack((Reg::RegTensor<uint8_t>&)dstReg, (Reg::RegTensor<uint16_t>&)dstReg);
    }
}
} // namespace RegSubReluCast

template <typename T1, typename T2>
constexpr __aicore__ inline void CheckSubReluCastSupportType()
{
    static_assert(
        SupportType<Tuple<T1, T2>, Tuple<half, float>, Tuple<int8_t, half>, Tuple<int8_t, int16_t>>(),
        "Failed to check dtype in SubReluCast, current api support dtype combination is src: float, dst: half; "
        "src: half, dst: int8_t; src: int16_t, dst: int8_t.");
}

// SubReluCast::Level 0 - mask count mode
template <typename T1, typename T2, bool isSetMask = true>
__aicore__ inline void SubReluCastImpl(
    __ubuf__ T1* dst, __ubuf__ T2* src0, __ubuf__ T2* src1, const uint64_t mask, uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    CheckSubReluCastSupportType<T1, T2>();
    constexpr auto func = RegSubReluCast::SubReluCast<T1, T2, Reg::RegTensor<T1>, Reg::RegTensor<T2>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, false>(dst, src0, src1, nullptr, mask, repeatTime, repeatParams);
}

// SubReluCast::Level 0 - mask bit mode
template <typename T1, typename T2, bool isSetMask = true>
__aicore__ inline void SubReluCastImpl(
    __ubuf__ T1* dst, __ubuf__ T2* src0, __ubuf__ T2* src1, const uint64_t mask[], uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    CheckSubReluCastSupportType<T1, T2>();
    constexpr auto func = RegSubReluCast::SubReluCast<T1, T2, Reg::RegTensor<T1>, Reg::RegTensor<T2>>;
    Internal::VecBinaryImplTemplate<func, isSetMask, true>(dst, src0, src1, mask, 0, repeatTime, repeatParams);
}

// SubReluCast::Level 2
template <typename T1, typename T2>
__simd_vf__ inline void SubReluCastImpl(__ubuf__ T1* dst, __ubuf__ T2* src0, __ubuf__ T2* src1, const uint32_t calCount)
{
    static_assert(
        SupportType<
            Tuple<T1, T2>, Tuple<half, float>, Tuple<int8_t, half>, Tuple<int8_t, int16_t>, Tuple<float, int64_t>,
            Tuple<int32_t, int64_t>>(),
        "Failed to check dtype in SubReluCast, current api support "
        "dtype combination is src: float, dst: half; src: half, dst: int8_t; src: int16_t, dst: int8_t; src: int64_t, "
        "dst : int32_t / float.");
    uint32_t sreg = static_cast<uint32_t>(calCount);
    const T2 scalarValue = 0;

    if constexpr (sizeof(T2) == 8) {
        constexpr uint32_t sregLower = static_cast<uint32_t>(B64_DATA_NUM_PER_REPEAT * 2);
        const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, sregLower));
        Reg::RegTensor<T1> vDstReg0;
        Reg::RegTensor<T2, Reg::RegTraitNumTwo> vDstReg1;
        Reg::RegTensor<T2, Reg::RegTraitNumTwo> vSrcReg0;
        Reg::RegTensor<T2, Reg::RegTraitNumTwo> vSrcReg1;
        Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            mask = Reg::UpdateMask<T2, Reg::RegTraitNumTwo>(sreg);
            Reg::LoadAlign(vSrcReg0, src0 + i * sregLower);
            Reg::LoadAlign(vSrcReg1, src1 + i * sregLower);
            Reg::Sub(vDstReg1, vSrcReg0, vSrcReg1, mask);
            Reg::Maxs(vDstReg1, vDstReg1, scalarValue, mask);
            Reg::Cast<T1, T2, CastParam::SubReluCastTrait>(vDstReg0, vDstReg1, mask);
            Reg::StoreAlign(dst + i * sregLower, vDstReg0, mask);
        }
    } else {
        const uint32_t repeatStride = static_cast<uint32_t>(GetVecLen() / sizeof(T2));
        const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, repeatStride));
        Reg::RegTensor<T2> src0Reg;
        Reg::RegTensor<T2> src1Reg;
        Reg::RegTensor<T1> dstReg;
        Reg::MaskReg mask;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            mask = Reg::UpdateMask<T2>(sreg);
            Reg::LoadAlign(src0Reg, src0 + i * repeatStride);
            Reg::LoadAlign(src1Reg, src1 + i * repeatStride);
            Reg::Sub(src0Reg, src0Reg, src1Reg, mask);
            Reg::Maxs<T2>(src0Reg, src0Reg, scalarValue, mask);
            if constexpr (IsSameType<T2, float>::value) {
                Reg::Cast<T1, T2, layoutZSatSMrgZRndR>(dstReg, src0Reg, mask);
                Reg::StoreAlign<T1, Reg::StoreDist::DIST_PACK_B32>(dst + i * repeatStride, dstReg, mask);
            } else {
                if constexpr (IsSameType<T2, int16_t>::value) {
                    Reg::RegTensor<half> tmpReg;
                    Reg::Cast<half, int16_t, MrgZRndRSatS>(tmpReg, src0Reg, mask);
                    Reg::Cast<int8_t, half, layoutZSatSMrgZRndR>(dstReg, tmpReg, mask);
                } else {
                    Reg::Cast<T1, T2, layoutZSatSMrgZRndR>(dstReg, src0Reg, mask);
                }
                Reg::StoreAlign<T1, Reg::StoreDist::DIST_PACK_B16>(dst + i * repeatStride, dstReg, mask);
            }
        }
    }
}

//  castDequanValue bit arrange
//  =========================================================================
//  | unused 17bit | 1bit signMode | 9bit offset | unused 5bit | 32bit scale|
//  =========================================================================
__aicore__ inline uint64_t MakeDeqScaleConfig(float scale, int16_t offset, bool signMode)
{
    constexpr uint64_t signModeBit = 46;
    constexpr uint64_t offsetMask = 0x1ff;
    constexpr uint64_t offsetBit = 37;
    uint64_t config =
        ((static_cast<uint64_t>(signMode) << signModeBit) | ((offset & offsetMask) << offsetBit) |
         *(reinterpret_cast<uint32_t*>(&scale)));
    return config;
}

__aicore__ inline void SetDeqScaleImpl(float scale, int16_t offset, bool signMode)
{
    Internal::g_deqScale = MakeDeqScaleConfig(scale, offset, signMode);
}

template <typename T>
__aicore__ inline void SetDeqScaleImpl(const LocalTensor<T>& vdeqTensor, const VdeqInfo& vdeqInfo)
{
    for (uint8_t i = 0; i < VDEQ_TENSOR_SIZE; ++i) {
        float scale = vdeqInfo.vdeqScale[i];
        int16_t offset = vdeqInfo.vdeqOffset[i];
        bool signMode = vdeqInfo.vdeqSignMode[i];
        vdeqTensor.SetValue(i, static_cast<T>(MakeDeqScaleConfig(scale, offset, signMode)));
    }
    Internal::g_deqScale = reinterpret_cast<uint64_t>(vdeqTensor.GetPhyAddr());
}

template <typename T>
__aicore__ inline void SetDeqScaleImpl(T config)
{
    g_deqValue = config;
}

// Truncate::Level2
template <typename T, RoundMode roundMode>
__simd_vf__ inline void TruncateImpl(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t calCount)
{
    static_assert(
        SupportType<T, half, float, bfloat16_t>(), "Failed to check dtype in Truncate, current api "
                                                   "support dtype is src and dst both: half, float, bfloat16_t.");
    static_assert(
        SupportEnum<
            roundMode, RoundMode::CAST_RINT, RoundMode::CAST_FLOOR, RoundMode::CAST_CEIL, RoundMode::CAST_ROUND,
            RoundMode::CAST_TRUNC>(),
        "Failed to check dtype in Truncate, "
        "current api support roundMode is CAST_RINT, CAST_FLOOR, CAST_CEIL, CAST_ROUND, CAST_TRUNC.");
    constexpr uint32_t sregLower = static_cast<uint32_t>(VECTOR_REG_WIDTH / sizeof(T));
    const uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(calCount, sregLower));
    uint32_t sreg = static_cast<uint32_t>(calCount);
    Reg::RegTensor<T> vDstReg;
    Reg::RegTensor<T> vSrcReg;
    Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        mask = Reg::UpdateMask<T>(sreg);
        Reg::DataCopy(vSrcReg, src + i * sregLower);
        Reg::Truncate<T, roundMode>(vDstReg, vSrcReg, mask);
        Reg::DataCopy(dst + i * sregLower, vDstReg, mask);
    }
}

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_VCONV_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_VCONV_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_VCONV_IMPL_H__
#endif
