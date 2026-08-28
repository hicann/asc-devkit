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
    "impl/basic_api/dav_3510/kernel_operator_vec_vconv_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_VCONV_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_VEC_VCONV_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_VCONV_IMPL_H
#include "../kernel_utils.h"
#include "kernel_operator_vec_template_impl.h"
#include "../../../include/basic_api/reg_compute/kernel_reg_compute_intf.h"
namespace AscendC {
constexpr Reg::CastTrait layoutZMrgZ = {
    Reg::RegLayout::ZERO, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::UNKNOWN};

constexpr Reg::CastTrait layoutZSatSMrgZ = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::UNKNOWN};

constexpr Reg::CastTrait layoutZSatSMrgZRndA = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_ROUND};

constexpr Reg::CastTrait layoutZSatSMrgZRndH = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_HYBRID};

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

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode>
__simd_vf__ inline void CastIntrinsicsB64ImplVF(__ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint32_t calCount)
{
    constexpr uint16_t oneRepSize = 2 * GetVecLen() / sizeof(int64_t);
    uint16_t repeatTime = CeilDivision(calCount, oneRepSize);
    uint32_t sreg = static_cast<uint32_t>(calCount);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, roundMode};
    if constexpr (
        AscendC::Std::is_same<SRC_TYPE, complex64>::value && AscendC::Std::is_same<DST_TYPE, complex32>::value) {
        Reg::MaskReg preg;
        Reg::RegTensor<SRC_TYPE, Reg::RegTraitNumTwo> srcVreg;
        Reg::RegTensor<DST_TYPE, Reg::RegTraitNumTwo> dstVreg;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            preg = Reg::UpdateMask<int64_t, Reg::RegTraitNumTwo>(sreg);
            Reg::LoadAlign(srcVreg, src + i * oneRepSize);
            Reg::Cast<typename DST_TYPE::EleType, typename SRC_TYPE::EleType, castTrait>(
                (Reg::RegTensor<typename DST_TYPE::EleType>&)dstVreg.reg[0],
                (Reg::RegTensor<typename SRC_TYPE::EleType>&)srcVreg.reg[0], preg);
            Reg::Cast<typename DST_TYPE::EleType, typename SRC_TYPE::EleType, castTrait>(
                (Reg::RegTensor<typename DST_TYPE::EleType>&)dstVreg.reg[1],
                (Reg::RegTensor<typename SRC_TYPE::EleType>&)srcVreg.reg[1], preg);
            Reg::Pack((Reg::RegTensor<uint16_t>&)dstVreg.reg[0], (Reg::RegTensor<uint32_t>&)dstVreg.reg[0]);
            Reg::Pack((Reg::RegTensor<uint16_t>&)dstVreg.reg[1], (Reg::RegTensor<uint32_t>&)dstVreg.reg[1]);
            Reg::MaskPack(preg, preg);
            Reg::StoreAlign(dst + i * oneRepSize, dstVreg, preg);
        }
    } else if constexpr (
        AscendC::Std::is_same<SRC_TYPE, complex64>::value && AscendC::Std::is_same<DST_TYPE, complex64>::value) {
        Reg::MaskReg preg;
        Reg::RegTensor<SRC_TYPE, Reg::RegTraitNumTwo> srcVreg;
        Reg::RegTensor<DST_TYPE, Reg::RegTraitNumTwo> dstVreg;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            preg = Reg::UpdateMask<int64_t, Reg::RegTraitNumTwo>(sreg);
            Reg::LoadAlign(srcVreg, src + i * oneRepSize);
            Reg::Truncate<float, roundMode>(
                (Reg::RegTensor<float>&)dstVreg.reg[0], (Reg::RegTensor<float>&)srcVreg.reg[0], preg);
            Reg::Truncate<float, roundMode>(
                (Reg::RegTensor<float>&)dstVreg.reg[1], (Reg::RegTensor<float>&)srcVreg.reg[1], preg);
            Reg::StoreAlign(dst + i * oneRepSize, dstVreg, preg);
        }
    } else if constexpr (
        AscendC::Std::is_same<SRC_TYPE, complex32>::value && AscendC::Std::is_same<DST_TYPE, complex64>::value) {
        Reg::MaskReg preg;
        Reg::RegTensor<SRC_TYPE, Reg::RegTraitNumTwo> srcVreg;
        Reg::RegTensor<DST_TYPE, Reg::RegTraitNumTwo> dstVreg;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            preg = Reg::UpdateMask<int64_t, Reg::RegTraitNumTwo>(sreg);
            Reg::LoadAlign(srcVreg, src + i * oneRepSize);
            Reg::UnPack((Reg::RegTensor<uint32_t>&)srcVreg.reg[0], (Reg::RegTensor<uint16_t>&)srcVreg.reg[0]);
            Reg::UnPack((Reg::RegTensor<uint32_t>&)srcVreg.reg[1], (Reg::RegTensor<uint16_t>&)srcVreg.reg[1]);
            Reg::Cast<typename DST_TYPE::EleType, typename SRC_TYPE::EleType, castTrait>(
                (Reg::RegTensor<typename DST_TYPE::EleType>&)dstVreg.reg[0],
                (Reg::RegTensor<typename SRC_TYPE::EleType>&)srcVreg.reg[0], preg);
            Reg::Cast<typename DST_TYPE::EleType, typename SRC_TYPE::EleType, castTrait>(
                (Reg::RegTensor<typename DST_TYPE::EleType>&)dstVreg.reg[1],
                (Reg::RegTensor<typename SRC_TYPE::EleType>&)srcVreg.reg[1], preg);
            Reg::StoreAlign(dst + i * oneRepSize, dstVreg, preg);
        }
    } else if constexpr (sizeof(DST_TYPE) == sizeof(int64_t)) {
        Reg::MaskReg preg;
        Reg::RegTensor<SRC_TYPE> srcVreg;
        Reg::RegTensor<DST_TYPE, Reg::RegTraitNumTwo> dstVreg;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            preg = Reg::UpdateMask<int64_t, Reg::RegTraitNumTwo>(sreg);
            Reg::LoadAlign(srcVreg, src + i * oneRepSize);
            Reg::Cast<DST_TYPE, SRC_TYPE, castTrait>(dstVreg, srcVreg, preg);
            Reg::StoreAlign(dst + i * oneRepSize, dstVreg, preg);
        }
    } else {
        Reg::MaskReg preg;
        Reg::RegTensor<SRC_TYPE, Reg::RegTraitNumTwo> srcVreg;
        Reg::RegTensor<DST_TYPE> dstVreg;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            preg = Reg::UpdateMask<int64_t, Reg::RegTraitNumTwo>(sreg);
            Reg::LoadAlign(srcVreg, src + i * oneRepSize);
            Reg::Cast<DST_TYPE, SRC_TYPE, castTrait>(dstVreg, srcVreg, preg);
            Reg::StoreAlign(dst + i * oneRepSize, dstVreg, preg);
        }
    }
}

template <typename DST_TYPE, typename SRC_TYPE>
__simd_callee__ inline void GenLoadL2(Reg::RegTensor<SRC_TYPE>& srcVreg, __ubuf__ SRC_TYPE* srcAddr, Reg::MaskReg& preg)
{
    if constexpr (SupportType<SRC_TYPE, int4x2_t, fp4x2_e2m1_t, fp4x2_e1m2_t>() && sizeof(DST_TYPE) == 2) {
        Reg::LoadAlign<uint8_t, Reg::LoadDist::DIST_UNPACK4_B8>(
            (Reg::RegTensor<uint8_t>&)srcVreg, (__ubuf__ uint8_t*)srcAddr);
    } else if constexpr (sizeof(SRC_TYPE) == 1 && sizeof(DST_TYPE) == 2) {
        Reg::LoadAlign<SRC_TYPE, Reg::LoadDist::DIST_UNPACK_B8>(srcVreg, srcAddr);
    } else if constexpr (sizeof(SRC_TYPE) == 2 && sizeof(DST_TYPE) == 4) {
        Reg::LoadAlign<SRC_TYPE, Reg::LoadDist::DIST_UNPACK_B16>(srcVreg, srcAddr);
    } else if constexpr (sizeof(SRC_TYPE) == 1 && sizeof(DST_TYPE) == 4) {
        Reg::LoadAlign<SRC_TYPE, Reg::LoadDist::DIST_UNPACK4_B8>(srcVreg, srcAddr);
    } else {
        Reg::LoadAlign(srcVreg, srcAddr);
    }
}

template <typename DST_TYPE, typename SRC_TYPE>
__simd_callee__ inline void GenStoreL2(
    __ubuf__ DST_TYPE* dstAddr, Reg::RegTensor<DST_TYPE>& dstVreg, Reg::MaskReg& preg)
{
    if constexpr (SupportType<DST_TYPE, int4x2_t, fp4x2_e2m1_t, fp4x2_e1m2_t>() && sizeof(SRC_TYPE) == 2) {
        Reg::StoreAlign<uint8_t, Reg::StoreDist::DIST_PACK4_B32>(
            (__ubuf__ uint8_t*)dstAddr, (Reg::RegTensor<uint8_t>&)dstVreg, preg);
    } else if constexpr (sizeof(DST_TYPE) == 1 && sizeof(SRC_TYPE) == 2) {
        Reg::StoreAlign<DST_TYPE, Reg::StoreDist::DIST_PACK_B16>(dstAddr, dstVreg, preg);
    } else if constexpr (sizeof(DST_TYPE) == 2 && sizeof(SRC_TYPE) == 4) {
        Reg::StoreAlign<DST_TYPE, Reg::StoreDist::DIST_PACK_B32>(dstAddr, dstVreg, preg);
    } else if constexpr (sizeof(DST_TYPE) == 1 && sizeof(SRC_TYPE) == 4) {
        Reg::StoreAlign<DST_TYPE, Reg::StoreDist::DIST_PACK4_B32>(dstAddr, dstVreg, preg);
    } else {
        Reg::StoreAlign(dstAddr, dstVreg, preg);
    }
}

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode>
__simd_vf__ inline void CastIntrinsicsImplVF(
    __ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint32_t calCount, const half scale)
{
    uint16_t oneRepSize = GetVecLen() / sizeof(SRC_TYPE);
    if constexpr (sizeof(SRC_TYPE) < sizeof(DST_TYPE)) {
        oneRepSize = GetVecLen() / sizeof(DST_TYPE);
    }
    uint16_t repeatTime = CeilDivision(calCount, oneRepSize);
    uint32_t sreg = static_cast<uint32_t>(calCount);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, roundMode};
    Reg::MaskReg preg;
    Reg::RegTensor<SRC_TYPE> srcVreg;
    Reg::RegTensor<DST_TYPE> dstVreg;
    Reg::RegTensor<float> tmpVreg;
    for (uint16_t i = 0; i < repeatTime; ++i) {
        if constexpr (sizeof(SRC_TYPE) < sizeof(DST_TYPE)) {
            preg = Reg::UpdateMask<DST_TYPE>(sreg);
        } else {
            preg = Reg::UpdateMask<SRC_TYPE>(sreg);
        }
        if constexpr (SupportType<SRC_TYPE, int4x2_t, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
            GenLoadL2<DST_TYPE, SRC_TYPE>(srcVreg, src + (i * oneRepSize) / 2, preg);
        } else {
            GenLoadL2<DST_TYPE, SRC_TYPE>(srcVreg, src + i * oneRepSize, preg);
        }
        if constexpr (AscendC::Std::is_same<SRC_TYPE, int32_t>::value && AscendC::Std::is_same<DST_TYPE, half>::value) {
            Reg::Cast<float, SRC_TYPE, CastParam::s322floatCastTrait>(tmpVreg, srcVreg, preg);
            Reg::Muls(tmpVreg, tmpVreg, DEQ_SHIFT_RIGHT_17_BIT, preg);
            Reg::Muls(tmpVreg, tmpVreg, static_cast<float>(scale), preg);
            Reg::Muls(tmpVreg, tmpVreg, DEQ_SHIFT_LEFT_17_BIT, preg);
            Reg::Cast<DST_TYPE, float, CastParam::f322F16CastTrait>(dstVreg, tmpVreg, preg);
        } else if constexpr (
            AscendC::Std::is_same<SRC_TYPE, float>::value && AscendC::Std::is_same<DST_TYPE, float>::value) {
            Reg::Truncate<DST_TYPE, roundMode>(dstVreg, srcVreg, preg);
        } else {
            Reg::Cast<DST_TYPE, SRC_TYPE, castTrait>(dstVreg, srcVreg, preg);
        }
        if constexpr (SupportType<DST_TYPE, int4x2_t, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
            GenStoreL2<DST_TYPE, SRC_TYPE>(dst + (i * oneRepSize) / 2, dstVreg, preg);
        } else {
            GenStoreL2<DST_TYPE, SRC_TYPE>(dst + i * oneRepSize, dstVreg, preg);
        }
    }
}

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode>
__aicore__ inline void CastIntrinsicsImpl(__ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint32_t calCount)
{
    constexpr bool b64Cast = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<float, int64_t>, Tuple<int64_t, float>, Tuple<int32_t, int64_t>,
        Tuple<int64_t, int32_t>, Tuple<complex64, complex64>, Tuple<complex64, complex32>,
        Tuple<complex32, complex64>>();
    half scale = 0;
    if constexpr (b64Cast) {
        CastIntrinsicsB64ImplVF<DST_TYPE, SRC_TYPE, roundMode>(dst, src, calCount);
    } else {
        if constexpr (AscendC::Std::is_same<SRC_TYPE, int32_t>::value && AscendC::Std::is_same<DST_TYPE, half>::value) {
            scale = Internal::g_deqValue;
        }
        CastIntrinsicsImplVF<DST_TYPE, SRC_TYPE, roundMode>(dst, src, calCount, scale);
    }
}

__aicore__ inline bool GetOverflow()
{
    constexpr uint32_t CTRL_COUNTER = 48;
    return (get_ctrl() >> CTRL_COUNTER) == 0x1;
}

template <RoundMode roundMode>
__simd_callee__ inline void DealMantissa0(
    Reg::MaskReg& dstMask, Reg::RegTensor<uint64_t>& src, Reg::MaskReg& mask, Reg::MaskReg& maskInf,
    Reg::MaskReg& maskMax)
{
    Reg::MaskReg mask0, mask1, maskReg;
    Reg::Xor(maskReg, maskInf, mask, mask);
    Reg::Xor(maskReg, maskMax, maskReg, mask);
    Reg::RegTensor<uint64_t> dst0, dst1;
    constexpr uint64_t midValue = 0x800000000000000;
    constexpr uint64_t scalar3 = 0x8000000000000000;
    constexpr uint64_t scalar0 = 0x0;
    constexpr uint64_t scalar1 = 0x1;
    constexpr int16_t shiftScalar0 = 0x23; // 35
    constexpr int16_t shiftScalar1 = 0x3f; // 63
    constexpr int16_t shiftScalar3 = 0x22; // 34
    if constexpr (roundMode == RoundMode::CAST_RINT) {
        Reg::ShiftLefts(dst0, src, shiftScalar0, maskReg);
        Reg::CompareScalar<uint64_t, CMPMODE::GT>(mask0, dst0, scalar3, maskReg);
        Reg::CompareScalar<uint64_t, CMPMODE::EQ>(mask1, dst0, scalar3, maskReg);
        Reg::ShiftLefts(dst1, src, shiftScalar3, maskReg);
        Reg::CompareScalar<uint64_t, CMPMODE::GE>(mask1, dst1, scalar3, mask1);
        Reg::MaskOr(dstMask, mask1, mask0, maskReg);
    } else if constexpr (roundMode == RoundMode::CAST_FLOOR) {
        Reg::ShiftLefts(dst0, src, shiftScalar0, maskReg);
        Reg::ShiftRights(dst1, src, shiftScalar1, maskReg);
        Reg::CompareScalar<uint64_t, CMPMODE::EQ>(mask0, dst1, scalar1, maskReg);
        Reg::CompareScalar<uint64_t, CMPMODE::GT>(dstMask, dst0, scalar0, mask0);
    } else if constexpr (roundMode == RoundMode::CAST_CEIL) {
        Reg::ShiftLefts(dst0, src, shiftScalar0, maskReg);
        Reg::ShiftRights(dst1, src, shiftScalar1, maskReg);
        Reg::CompareScalar<uint64_t, CMPMODE::EQ>(mask0, dst1, scalar0, maskReg);
        Reg::CompareScalar<uint64_t, CMPMODE::GT>(dstMask, dst0, scalar0, mask0);
    } else if constexpr (roundMode == RoundMode::CAST_ROUND) {
        Reg::ShiftLefts(dst0, src, shiftScalar0, maskReg);
        Reg::CompareScalar<uint64_t, CMPMODE::GE>(dstMask, dst0, scalar3, maskReg);
    } else if constexpr (roundMode == RoundMode::CAST_TRUNC) {
        Reg::CompareScalar<uint64_t, CMPMODE::LT>(dstMask, src, scalar0, maskReg);
    }
}

__simd_callee__ inline void TruncateForDoubleToFloat(
    Reg::MaskReg& maskNan, Reg::MaskReg& maskInf, Reg::MaskReg& maskZero, Reg::MaskReg& maskMax,
    Reg::RegTensor<uint64_t>& dst, Reg::RegTensor<uint64_t>& src, Reg::MaskReg& preg)
{
    constexpr int16_t shiftScalar0 = 0x1;  // 1
    constexpr int16_t shiftScalar1 = 0x3f; // 63
    constexpr int16_t shiftScalar2 = 0x35; // 53
    constexpr int16_t shiftScalar3 = 0x1f; // 31
    constexpr int16_t shiftScalar4 = 0xc;  // 12
    constexpr int16_t shiftScalar5 = 0x29; // 41
    constexpr int16_t shiftScalar6 = 0x17; // 23
    constexpr uint64_t zero = 0x0;
    Reg::MaskReg mask0, mask1, maskPositive;
    Reg::RegTensor<uint64_t> tmpSrcSign0, tmpSrcExponent0, tmpSrcMantissa0, tmpReg;
    Reg::ShiftLefts(tmpSrcExponent0, src, shiftScalar0, preg);
    Reg::ShiftRights(tmpSrcExponent0, tmpSrcExponent0, shiftScalar2, preg);
    Reg::ShiftRights(tmpSrcSign0, src, shiftScalar1, preg);
    Reg::ShiftLefts(tmpSrcSign0, tmpSrcSign0, shiftScalar3, preg);
    Reg::CompareScalar<uint64_t, CMPMODE::EQ>(maskPositive, tmpSrcSign0, zero, preg);
    constexpr uint64_t double0 = 0x380;
    constexpr uint64_t double1 = 0x47f;
    constexpr uint64_t exponentMax = 0x7ff;
    constexpr uint64_t negative = 0x80000000;
    constexpr uint64_t positive = 0x7fffffff;
    Reg::CompareScalar<uint64_t, CMPMODE::EQ>(mask0, tmpSrcExponent0, exponentMax, preg);
    Reg::CompareScalar<uint64_t, CMPMODE::LT>(mask1, tmpSrcExponent0, exponentMax, preg);
    Reg::CompareScalar<uint64_t, CMPMODE::LT>(maskZero, tmpSrcExponent0, double0, mask1);
    Reg::CompareScalar<uint64_t, CMPMODE::GE>(maskMax, tmpSrcExponent0, double1, mask1);
    Reg::RegTensor<uint64_t> dstExponent;
    Reg::Duplicate(dstExponent, double0, preg);
    Reg::Sub(tmpSrcExponent0, tmpSrcExponent0, dstExponent, preg);
    Reg::ShiftLefts(tmpSrcMantissa0, src, shiftScalar4, preg);
    Reg::ShiftRights(tmpSrcMantissa0, tmpSrcMantissa0, shiftScalar5, preg);
    constexpr uint64_t double2 = 0x0;
    Reg::CompareScalar<uint64_t, CMPMODE::GT>(maskNan, tmpSrcMantissa0, double2, mask0);
    Reg::CompareScalar<uint64_t, CMPMODE::EQ>(maskInf, tmpSrcMantissa0, double2, mask0);
    Reg::ShiftLefts(tmpSrcExponent0, tmpSrcExponent0, shiftScalar6, preg);
    Reg::Add(tmpSrcExponent0, tmpSrcExponent0, tmpSrcMantissa0, preg);
    Reg::Duplicate(tmpReg, positive, preg);
    Reg::And(tmpSrcMantissa0, tmpSrcExponent0, tmpReg, maskPositive);
    Reg::Select(tmpSrcExponent0, tmpSrcMantissa0, tmpSrcExponent0, maskPositive);
    Reg::Duplicate(tmpReg, negative, preg);
    Reg::CompareScalar<uint64_t, CMPMODE::NE>(maskPositive, tmpSrcSign0, zero, preg);
    Reg::Or(tmpSrcMantissa0, tmpSrcExponent0, tmpReg, maskPositive);
    Reg::Select(dst, tmpSrcMantissa0, tmpSrcExponent0, maskPositive);
}

__simd_callee__ inline void SelectNanInfZero0(
    Reg::MaskReg& preg, Reg::MaskReg& maskNan, Reg::MaskReg& maskInf, Reg::MaskReg& maskZero, Reg::MaskReg& maskMax,
    Reg::RegTensor<uint32_t>& dst)
{
    constexpr uint32_t num0 = 0x0;
    Reg::MaskReg maskInfNegative, maskInfPositive;
    Reg::CompareScalar<uint32_t, CMPMODE::LT>(maskInfNegative, dst, num0, maskInf);
    Reg::CompareScalar<uint32_t, CMPMODE::GE>(maskInfPositive, dst, num0, maskInf);
    Reg::MaskReg maskNegative, maskPositive;
    Reg::CompareScalar<uint32_t, CMPMODE::LT>(maskNegative, dst, num0, maskMax);
    Reg::CompareScalar<uint32_t, CMPMODE::GE>(maskPositive, dst, num0, maskMax);
    constexpr uint32_t num1 = 0x7f7fffff;
    constexpr uint32_t num2 = 0xff7fffff;
    Reg::RegTensor<uint32_t> dstMaxNegative, dstMaxPositive;
    Reg::Duplicate(dstMaxPositive, num1);
    Reg::Select(dst, dstMaxPositive, dst, maskPositive);
    Reg::Duplicate(dstMaxNegative, num2);
    Reg::Select(dst, dstMaxNegative, dst, maskNegative);

    constexpr uint32_t numInfPositive = 0x7f800000;
    constexpr uint32_t numInfNegative = 0xff800000;
    constexpr uint32_t numNan = 0x7f800001;
    Reg::RegTensor<uint32_t> dstNan, dstInfPositive, dstInfNegative;
    Reg::Duplicate(dstNan, numNan);
    Reg::Duplicate(dstInfPositive, numInfPositive);
    Reg::Duplicate(dstInfNegative, numInfNegative);
    Reg::Select(dst, dstNan, dst, maskNan);
    Reg::Select(dst, dstInfPositive, dst, maskInfPositive);
    Reg::Select(dst, dstInfNegative, dst, maskInfNegative);
    Reg::RegTensor<uint32_t> dstZero;
    Reg::Duplicate(dstZero, num0);
    Reg::Select(dst, dstZero, dst, maskZero);
}

__simd_callee__ inline void SelectNanInfZero00(
    Reg::MaskReg& preg, Reg::MaskReg& maskNan, Reg::MaskReg& maskInf, Reg::MaskReg& maskZero, Reg::MaskReg& maskMax,
    Reg::RegTensor<uint32_t>& dst)
{
    constexpr uint32_t num0 = 0x0;
    constexpr uint32_t num1 = 0x80000000;
    Reg::MaskReg maskInfNegative, maskInfPositive;
    Reg::CompareScalar<uint32_t, CMPMODE::LT>(maskInfPositive, dst, num1, maskInf);
    Reg::CompareScalar<uint32_t, CMPMODE::GE>(maskInfNegative, dst, num1, maskInf);
    Reg::MaskReg maskNegative, maskPositive;
    Reg::CompareScalar<uint32_t, CMPMODE::LT>(maskPositive, dst, num1, maskMax);
    Reg::CompareScalar<uint32_t, CMPMODE::GE>(maskNegative, dst, num1, maskMax);
    constexpr uint32_t numInfPositive = 0x7f800000;
    constexpr uint32_t numInfNegative = 0xff800000;
    constexpr uint32_t numNan = 0x7f800001;
    Reg::RegTensor<uint32_t> dstNan, dstInfPositive, dstInfNegative;
    Reg::Duplicate(dstNan, numNan);
    Reg::Duplicate(dstInfPositive, numInfPositive);
    Reg::Duplicate(dstInfNegative, numInfNegative);
    Reg::Select(dst, dstNan, dst, maskNan);
    Reg::Select(dst, dstInfPositive, dst, maskInfPositive);
    Reg::Select(dst, dstInfNegative, dst, maskInfNegative);
    Reg::Select(dst, dstInfPositive, dst, maskPositive);
    Reg::Select(dst, dstInfNegative, dst, maskNegative);
    Reg::RegTensor<uint32_t> dstZero, dstNegative, dstPositive;
    Reg::Duplicate(dstZero, num0);
    Reg::Select(dst, dstZero, dst, maskZero);
}

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode>
__simd_vf__ inline void CastDoubleToFloat(
    __ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint32_t calCount, bool isGetOverflow)
{
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(double);
    uint16_t repeatTime = CeilDivision(calCount, oneRepSize);
    uint32_t sreg = calCount;
    Reg::MaskReg preg;
    Reg::RegTensor<uint64_t> srvVreg0, srvVreg1, tmpSrcReg0, tmpSrcReg1;
    Reg::MaskReg mask0, dstMask0, maskMax0, maskZero0, maskNan0, maskInf0;
    Reg::RegTensor<uint32_t> dstZero, dstVreg, dstFloat0, dstAdd;
    dstMask0 = Reg::CreateMask<DST_TYPE, Reg::MaskPattern::ALLF>();
    constexpr float num0 = 0;
    Reg::RegTensor<float> dstFloat;
    constexpr uint32_t num = 0x1;
    Reg::Duplicate(dstAdd, num);
    Reg::RegTensor<float> dstFloatAdd;
    for (uint16_t i = 0; i < repeatTime; ++i) {
        preg = Reg::UpdateMask<int64_t, Reg::RegTraitNumOne>(sreg);
        Reg::LoadAlign(tmpSrcReg0, (__ubuf__ uint64_t*&)src + i * oneRepSize);
        TruncateForDoubleToFloat(maskNan0, maskInf0, maskZero0, maskMax0, srvVreg0, tmpSrcReg0, preg);
        DealMantissa0<roundMode>(mask0, tmpSrcReg0, preg, maskInf0, maskMax0);
        Reg::DeInterleave(dstVreg, dstZero, (Reg::RegTensor<uint32_t>&)srvVreg0, (Reg::RegTensor<uint32_t>&)srvVreg0);
        Reg::MaskDeInterleave<uint32_t>(preg, dstMask0, preg, dstMask0);
        Reg::MaskDeInterleave<uint32_t>(mask0, dstMask0, mask0, dstMask0);
        Reg::MaskDeInterleave<uint32_t>(maskNan0, dstMask0, maskNan0, dstMask0);
        Reg::MaskDeInterleave<uint32_t>(maskMax0, dstMask0, maskMax0, dstMask0);
        Reg::MaskDeInterleave<uint32_t>(maskInf0, dstMask0, maskInf0, dstMask0);
        Reg::MaskDeInterleave<uint32_t>(maskZero0, dstMask0, maskZero0, dstMask0);
        Reg::Add(dstFloat0, dstVreg, dstAdd, mask0);
        Reg::Select(dstVreg, dstFloat0, dstVreg, mask0);
        if (isGetOverflow) {
            SelectNanInfZero0(preg, maskNan0, maskInf0, maskZero0, maskMax0, dstVreg);
        } else {
            SelectNanInfZero00(preg, maskNan0, maskInf0, maskZero0, maskMax0, dstVreg);
        }
        dstFloat = (Reg::RegTensor<float>&)dstVreg;
        Reg::StoreAlign(dst + i * oneRepSize, dstFloat, preg);
    }
}

template <RoundMode roundMode>
__simd_callee__ inline void DealMantissa1(
    Reg::MaskReg& dstMask, Reg::RegTensor<uint64_t>& src, Reg::MaskReg& mask, Reg::MaskReg& maskInf,
    Reg::MaskReg& maskMax)
{
    Reg::MaskReg mask0, mask1, mask2;
    Reg::Xor(mask2, maskInf, mask, mask);
    Reg::Xor(mask2, maskMax, mask2, mask);
    Reg::RegTensor<uint64_t> dst0, dst1;
    constexpr uint64_t scalar3 = 0x8000000000000000;
    constexpr uint64_t scalar0 = 0x0;
    constexpr uint64_t scalar1 = 0x1;
    constexpr int16_t shiftScalar0 = 0x13; // 19..
    constexpr int16_t shiftScalar1 = 0x3f; // 63
    constexpr int16_t shiftScalar3 = 0x12; // 18..
    if constexpr (roundMode == RoundMode::CAST_RINT) {
        Reg::ShiftLefts(dst0, src, shiftScalar0, mask2);
        Reg::CompareScalar<uint64_t, CMPMODE::GT>(mask0, dst0, scalar3, mask2);
        Reg::CompareScalar<uint64_t, CMPMODE::EQ>(mask1, dst0, scalar3, mask2);
        Reg::ShiftLefts(dst1, src, shiftScalar3, mask2);
        Reg::CompareScalar<uint64_t, CMPMODE::GT>(mask1, dst1, scalar3, mask1);
        Reg::MaskOr(dstMask, mask1, mask0, mask2);
    } else if constexpr (roundMode == RoundMode::CAST_FLOOR) {
        Reg::ShiftLefts(dst0, src, shiftScalar0, mask2);
        Reg::ShiftRights(dst1, src, shiftScalar1, mask2);
        Reg::CompareScalar<uint64_t, CMPMODE::EQ>(mask0, dst1, scalar1, mask2);
        Reg::CompareScalar<uint64_t, CMPMODE::GT>(dstMask, dst0, scalar0, mask0);
    } else if constexpr (roundMode == RoundMode::CAST_CEIL) {
        Reg::ShiftLefts(dst0, src, shiftScalar0, mask2);
        Reg::ShiftRights(dst1, src, shiftScalar1, mask2);
        Reg::CompareScalar<uint64_t, CMPMODE::EQ>(mask0, dst1, scalar0, mask2);
        Reg::CompareScalar<uint64_t, CMPMODE::GT>(dstMask, dst0, scalar0, mask0);
    } else if constexpr (roundMode == RoundMode::CAST_ROUND) {
        Reg::ShiftLefts(dst0, src, shiftScalar0, mask2);
        Reg::CompareScalar<uint64_t, CMPMODE::GE>(dstMask, dst0, scalar3, mask2);
    } else if constexpr (roundMode == RoundMode::CAST_TRUNC) {
        Reg::CompareScalar<uint64_t, CMPMODE::LT>(dstMask, src, scalar0, mask2);
    }
}

__simd_callee__ inline void TruncateForDoubleToBf16(
    Reg::MaskReg& maskNan, Reg::MaskReg& maskInf, Reg::MaskReg& maskZero, Reg::MaskReg& maskMax,
    Reg::RegTensor<uint64_t>& dst, Reg::RegTensor<uint64_t>& src, Reg::MaskReg& preg)
{
    constexpr int16_t shiftScalar0 = 0x1;  // 1
    constexpr int16_t shiftScalar1 = 0x3f; // 63
    constexpr int16_t shiftScalar2 = 0x35; // 53
    constexpr int16_t shiftScalar3 = 0xf;  // 15..
    constexpr int16_t shiftScalar4 = 0xc;  // 12
    constexpr int16_t shiftScalar5 = 0x39; // 57..
    constexpr int16_t shiftScalar6 = 0x7;  // 7..
    constexpr uint64_t zero = 0x0;
    Reg::MaskReg maskPositive;
    Reg::RegTensor<uint64_t> tmpSrcSign0, tmpSrcExponent0, tmpSrcMantissa0, tmpReg;
    Reg::ShiftLefts(tmpSrcExponent0, src, shiftScalar0, preg);
    Reg::ShiftRights(tmpSrcExponent0, tmpSrcExponent0, shiftScalar2, preg);
    Reg::ShiftRights(tmpSrcSign0, src, shiftScalar1, preg);
    Reg::ShiftLefts(tmpSrcSign0, tmpSrcSign0, shiftScalar3, preg);
    Reg::CompareScalar<uint64_t, CMPMODE::EQ>(maskPositive, tmpSrcSign0, zero, preg);
    constexpr uint64_t double0 = 0x380;
    constexpr uint64_t double1 = 0x47f;
    constexpr uint64_t exponentMax = 0x7ff;
    Reg::MaskReg mask0, mask1;
    Reg::CompareScalar<uint64_t, CMPMODE::EQ>(mask0, tmpSrcExponent0, exponentMax, preg);
    Reg::CompareScalar<uint64_t, CMPMODE::LT>(mask1, tmpSrcExponent0, exponentMax, preg);
    Reg::CompareScalar<uint64_t, CMPMODE::LT>(maskZero, tmpSrcExponent0, double0, mask1);
    Reg::CompareScalar<uint64_t, CMPMODE::GE>(maskMax, tmpSrcExponent0, double1, mask1);
    Reg::RegTensor<uint64_t> dstExponent;
    Reg::Duplicate(dstExponent, double0, preg);
    Reg::Sub(tmpSrcExponent0, tmpSrcExponent0, dstExponent, preg);
    Reg::ShiftLefts(tmpSrcMantissa0, src, shiftScalar4, preg);
    constexpr uint64_t double2 = 0x0;
    constexpr uint64_t negative = 0x8000;
    constexpr uint64_t positive = 0x7fff;
    Reg::CompareScalar<uint64_t, CMPMODE::GT>(maskNan, tmpSrcMantissa0, double2, mask0);
    Reg::CompareScalar<uint64_t, CMPMODE::EQ>(maskInf, tmpSrcMantissa0, double2, mask0);
    Reg::ShiftRights(tmpSrcMantissa0, tmpSrcMantissa0, shiftScalar5, preg);
    Reg::ShiftLefts(tmpSrcExponent0, tmpSrcExponent0, shiftScalar6, preg);
    Reg::Add(tmpSrcExponent0, tmpSrcExponent0, tmpSrcMantissa0, preg);
    Reg::Duplicate(tmpReg, positive, preg);
    Reg::And(tmpSrcMantissa0, tmpSrcExponent0, tmpReg, maskPositive);
    Reg::Select(tmpSrcExponent0, tmpSrcMantissa0, tmpSrcExponent0, maskPositive);
    Reg::Duplicate(tmpReg, negative, preg);
    Reg::CompareScalar<uint64_t, CMPMODE::NE>(maskPositive, tmpSrcSign0, zero, preg);
    Reg::Or(tmpSrcMantissa0, tmpSrcExponent0, tmpReg, maskPositive);
    Reg::Select(dst, tmpSrcMantissa0, tmpSrcExponent0, maskPositive);
}

__simd_callee__ inline void SelectNanInfZero1(
    Reg::MaskReg& preg, Reg::MaskReg& maskNan, Reg::MaskReg& maskInf, Reg::MaskReg& maskZero, Reg::MaskReg& maskMax,
    Reg::RegTensor<uint16_t>& dst)
{
    constexpr uint16_t num0 = 0x0;
    constexpr uint16_t num = 0x8000;
    Reg::MaskReg maskInfNegative, maskInfPositive;
    Reg::CompareScalar<uint16_t, CMPMODE::LT>(maskInfPositive, dst, num, maskInf);
    Reg::CompareScalar<uint16_t, CMPMODE::GE>(maskInfNegative, dst, num, maskInf);
    Reg::MaskReg maskNegative, maskPositive;
    Reg::CompareScalar<uint16_t, CMPMODE::LT>(maskPositive, dst, num, maskMax);
    Reg::CompareScalar<uint16_t, CMPMODE::GE>(maskNegative, dst, num, maskMax);
    constexpr uint16_t num1 = 0x7f7f;
    constexpr uint16_t num2 = 0xff7f;
    Reg::RegTensor<uint16_t> dstMaxNegative, dstMaxPositive;
    Reg::Duplicate(dstMaxPositive, num1);
    Reg::Select(dst, dstMaxPositive, dst, maskPositive);
    Reg::Duplicate(dstMaxNegative, num2);
    Reg::Select(dst, dstMaxNegative, dst, maskNegative);
    constexpr uint16_t numInfPositive = 0x7f80;
    constexpr uint16_t numInfNegative = 0xff80;
    constexpr uint16_t numNan = 0x7f81;
    Reg::RegTensor<uint16_t> dstNan, dstInfPositive, dstInfNegative;
    Reg::Duplicate(dstNan, numNan);
    Reg::Duplicate(dstInfPositive, numInfPositive);
    Reg::Duplicate(dstInfNegative, numInfNegative);
    Reg::Select(dst, dstNan, dst, maskNan);
    Reg::Select(dst, dstInfPositive, dst, maskInfPositive);
    Reg::Select(dst, dstInfNegative, dst, maskInfNegative);
}

__simd_callee__ inline void SelectNanInfZero10(
    Reg::MaskReg& preg, Reg::MaskReg& maskNan, Reg::MaskReg& maskInf, Reg::MaskReg& maskZero, Reg::MaskReg& maskMax,
    Reg::RegTensor<uint16_t>& dst)
{
    constexpr uint16_t num0 = 0x0;
    constexpr uint16_t num = 0x8000;
    Reg::MaskReg maskInfNegative, maskInfPositive;
    Reg::CompareScalar<uint16_t, CMPMODE::LT>(maskInfPositive, dst, num, maskInf);
    Reg::CompareScalar<uint16_t, CMPMODE::GE>(maskInfNegative, dst, num, maskInf);
    Reg::MaskReg maskNegative, maskPositive;
    Reg::CompareScalar<uint16_t, CMPMODE::LT>(maskPositive, dst, num, maskMax);
    Reg::CompareScalar<uint16_t, CMPMODE::GE>(maskNegative, dst, num, maskMax);
    constexpr uint16_t numInfPositive = 0x7f80;
    constexpr uint16_t numInfNegative = 0xff80;
    constexpr uint16_t numNan = 0x7f81;
    Reg::RegTensor<uint16_t> dstNan, dstInfPositive, dstInfNegative;
    Reg::Duplicate(dstNan, numNan);
    Reg::Duplicate(dstInfPositive, numInfPositive);
    Reg::Duplicate(dstInfNegative, numInfNegative);
    Reg::Select(dst, dstNan, dst, maskNan);
    Reg::Select(dst, dstInfPositive, dst, maskInfPositive);
    Reg::Select(dst, dstInfPositive, dst, maskPositive);
    Reg::Select(dst, dstInfNegative, dst, maskInfNegative);
    Reg::Select(dst, dstInfNegative, dst, maskNegative);
    Reg::RegTensor<uint16_t> dstZero, dstNegative, dstPositive;
    Reg::Duplicate(dstZero, num0);
    Reg::Select(dst, dstZero, dst, maskZero);
}

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode>
__simd_vf__ inline void CastDoubleToBf16(
    __ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint32_t calCount, bool isGetOverflow)
{
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(double);
    uint16_t repeatTime = CeilDivision(calCount, oneRepSize);
    uint32_t sreg = static_cast<uint16_t>(calCount);
    Reg::MaskReg preg;
    Reg::RegTensor<uint16_t> dstFloat0, dstFloat1;
    Reg::RegTensor<uint16_t> dstAdd;
    constexpr uint16_t num = 0x1;
    Reg::RegTensor<uint64_t> tmpSrcReg0;
    Reg::MaskReg mask0, mask1, mask2, mask3;
    Reg::RegTensor<uint16_t> dstZero, dstVreg0;
    Reg::RegTensor<uint16_t> dstVreg, dstZero0;
    Reg::RegTensor<uint64_t> srvVreg0;
    Reg::MaskReg dstMask, maskMax0, maskZero0, maskNan0, maskInf0;
    Reg::MaskReg dstMask0 = Reg::CreateMask<DST_TYPE, Reg::MaskPattern::ALLF>();
    Reg::RegTensor<bfloat16_t> dstFloat;
    Reg::RegTensor<uint16_t> dstFloatAdd;
    constexpr uint16_t num0 = 0x0;
    constexpr uint16_t num1 = 0x8000;
    for (uint16_t i = 0; i < repeatTime; ++i) {
        preg = Reg::UpdateMask<int64_t, Reg::RegTraitNumOne>(sreg);
        Reg::LoadAlign(tmpSrcReg0, (__ubuf__ uint64_t*&)src + i * oneRepSize);
        TruncateForDoubleToBf16(maskNan0, maskInf0, maskZero0, maskMax0, srvVreg0, tmpSrcReg0, preg);
        DealMantissa1<roundMode>(mask0, tmpSrcReg0, preg, maskInf0, maskMax0);
        Reg::DeInterleave(dstVreg0, dstZero, (Reg::RegTensor<uint16_t>&)srvVreg0, (Reg::RegTensor<uint16_t>&)dstZero);
        Reg::DeInterleave(dstVreg, dstZero, (Reg::RegTensor<uint16_t>&)dstVreg0, (Reg::RegTensor<uint16_t>&)dstZero);
        Reg::MaskDeInterleave<uint32_t>(preg, dstMask0, preg, dstMask0);
        Reg::MaskDeInterleave<uint16_t>(preg, dstMask0, preg, dstMask0);
        Reg::MaskDeInterleave<uint32_t>(mask0, dstMask0, mask0, dstMask0);
        Reg::MaskDeInterleave<uint16_t>(dstMask, dstMask0, mask0, dstMask0);
        Reg::MaskDeInterleave<uint32_t>(maskNan0, dstMask0, maskNan0, dstMask0);
        Reg::MaskDeInterleave<uint16_t>(maskNan0, dstMask0, maskNan0, dstMask0);
        Reg::MaskDeInterleave<uint32_t>(maskMax0, dstMask0, maskMax0, dstMask0);
        Reg::MaskDeInterleave<uint16_t>(maskMax0, dstMask0, maskMax0, dstMask0);
        Reg::MaskDeInterleave<uint32_t>(maskInf0, dstMask0, maskInf0, dstMask0);
        Reg::MaskDeInterleave<uint16_t>(maskInf0, dstMask0, maskInf0, dstMask0);
        Reg::MaskDeInterleave<uint32_t>(maskZero0, dstMask0, maskZero0, dstMask0);
        Reg::MaskDeInterleave<uint16_t>(maskZero0, dstMask0, maskZero0, dstMask0);
        Reg::Duplicate(dstFloatAdd, num1);
        Reg::And(dstFloatAdd, dstFloatAdd, dstVreg, dstMask);
        Reg::Duplicate(dstAdd, num);
        Reg::Add(dstFloat0, dstVreg, dstAdd, dstMask);
        Reg::Select(dstVreg, dstFloat0, dstVreg, dstMask);
        if (isGetOverflow) {
            SelectNanInfZero1(preg, maskNan0, maskInf0, maskZero0, maskMax0, dstVreg);
        } else {
            SelectNanInfZero10(preg, maskNan0, maskInf0, maskZero0, maskMax0, dstVreg);
        }
        dstFloat = (Reg::RegTensor<bfloat16_t>&)dstVreg;
        Reg::StoreAlign(dst + i * oneRepSize, dstFloat, preg);
    }
}

__simd_callee__ inline void Clz(
    Reg::MaskReg& preg, Reg::RegTensor<uint64_t>& srcReg, Reg::RegTensor<uint64_t>& countZero)
{
    constexpr uint64_t num1 = 0;
    constexpr int16_t shiftScalarZero = 32;
    constexpr uint64_t addNumZero = 32;
    Reg::RegTensor<uint64_t> tmpOne, tmpAdd, tmpTwo, tmpThree;
    Reg::Duplicate(tmpAdd, addNumZero);
    Reg::MaskReg tempMask;
    Reg::ShiftRights(tmpOne, srcReg, shiftScalarZero, preg);
    Reg::CompareScalar<uint64_t, CMPMODE::EQ>(tempMask, tmpOne, num1, preg);
    Reg::Add(tmpTwo, countZero, tmpAdd, tempMask);
    Reg::ShiftLefts(tmpAdd, srcReg, shiftScalarZero, tempMask);
    Reg::Select(tmpThree, tmpAdd, srcReg, tempMask);
    Reg::Select(countZero, tmpTwo, countZero, tempMask);
    constexpr int16_t rightScalarZero = 48;
    constexpr uint64_t addNumOne = 16;
    Reg::Duplicate(tmpAdd, addNumOne);
    Reg::ShiftRights(tmpOne, tmpThree, rightScalarZero, preg);
    Reg::CompareScalar<uint64_t, CMPMODE::EQ>(tempMask, tmpOne, num1, preg);
    Reg::Add(tmpTwo, countZero, tmpAdd, tempMask);
    constexpr int16_t shiftScalarOne = 16;
    Reg::ShiftLefts(tmpAdd, tmpThree, shiftScalarOne, tempMask);
    Reg::Select(tmpThree, tmpAdd, tmpThree, tempMask);
    Reg::Select(countZero, tmpTwo, countZero, tempMask);
    constexpr int16_t rightScalarOne = 56;
    constexpr uint64_t addNumTwo = 8;
    Reg::Duplicate(tmpAdd, addNumTwo);
    Reg::ShiftRights(tmpOne, tmpThree, rightScalarOne, preg);
    Reg::CompareScalar<uint64_t, CMPMODE::EQ>(tempMask, tmpOne, num1, preg);
    Reg::Add(tmpTwo, countZero, tmpAdd, tempMask);
    constexpr int16_t shiftScalarTwo = 8;
    Reg::ShiftLefts(tmpAdd, tmpThree, shiftScalarTwo, tempMask);
    Reg::Select(tmpThree, tmpAdd, tmpThree, tempMask);
    Reg::Select(countZero, tmpTwo, countZero, tempMask);
    constexpr int16_t rightScalarTwo = 60;
    constexpr uint64_t addNumThree = 4;
    Reg::Duplicate(tmpAdd, addNumThree);
    Reg::ShiftRights(tmpOne, tmpThree, rightScalarTwo, preg);
    Reg::CompareScalar<uint64_t, CMPMODE::EQ>(tempMask, tmpOne, num1, preg);
    Reg::Add(tmpTwo, countZero, tmpAdd, tempMask);
    constexpr int16_t shiftScalarThree = 4;
    Reg::ShiftLefts(tmpAdd, tmpThree, shiftScalarThree, tempMask);
    Reg::Select(tmpThree, tmpAdd, tmpThree, tempMask);
    Reg::Select(countZero, tmpTwo, countZero, tempMask);
    constexpr int16_t rightScalarThree = 62;
    constexpr uint64_t addNumFour = 2;
    Reg::Duplicate(tmpAdd, addNumFour);
    Reg::ShiftRights(tmpOne, tmpThree, rightScalarThree, preg);
    Reg::CompareScalar<uint64_t, CMPMODE::EQ>(tempMask, tmpOne, num1, preg);
    Reg::Add(tmpTwo, countZero, tmpAdd, tempMask);
    constexpr int16_t shiftScalarFour = 2;
    Reg::ShiftLefts(tmpAdd, tmpThree, shiftScalarFour, tempMask);
    Reg::Select(tmpThree, tmpAdd, tmpThree, tempMask);
    Reg::Select(countZero, tmpTwo, countZero, tempMask);
    constexpr int16_t rightScalarFour = 63;
    constexpr uint64_t addNumFive = 1;
    Reg::Duplicate(tmpAdd, addNumFive);
    Reg::ShiftRights(tmpOne, tmpThree, rightScalarFour, preg);
    Reg::CompareScalar<uint64_t, CMPMODE::EQ>(tempMask, tmpOne, num1, preg);
    Reg::Add(tmpTwo, countZero, tmpAdd, tempMask);
    constexpr int16_t shiftScalarFive = 16;
    Reg::ShiftLefts(tmpAdd, tmpThree, shiftScalarFive, tempMask);
    Reg::Select(tmpThree, tmpAdd, tmpThree, tempMask);
    Reg::Select(countZero, tmpTwo, countZero, tempMask);
    Reg::CompareScalar<uint64_t, CMPMODE::EQ>(tempMask, srcReg, num1, preg);
    constexpr uint64_t addNumSix = 64;
    Reg::Duplicate(tmpAdd, addNumSix);
    Reg::Select(countZero, tmpAdd, countZero, tempMask);
}

__simd_callee__ inline void DealLowerThan52(
    Reg::MaskReg& tmpMask1, Reg::RegTensor<uint64_t>& srcReg, Reg::RegTensor<uint64_t>& countZero)
{
    constexpr uint64_t dupNum0 = 1023;
    Reg::RegTensor<uint64_t> tmpReg1, expReg, tmpReg2;
    Reg::RegTensor<int64_t> scalar;
    Reg::Duplicate(tmpReg1, dupNum0);
    Reg::Add(expReg, tmpReg1, countZero, tmpMask1);
    constexpr uint64_t dupNum1 = 52;
    Reg::Duplicate(tmpReg1, dupNum1);
    Reg::Sub(tmpReg2, tmpReg1, countZero, tmpMask1);
    scalar = (Reg::RegTensor<int64_t>&)tmpReg1;
    Reg::ShiftLeft(expReg, expReg, scalar, tmpMask1);
    constexpr uint64_t dupNum2 = 1;
    Reg::Duplicate(tmpReg1, dupNum2);
    scalar = (Reg::RegTensor<int64_t>&)countZero;
    Reg::ShiftLeft(tmpReg1, tmpReg1, scalar, tmpMask1);
    Reg::Sub(tmpReg1, srcReg, tmpReg1, tmpMask1);
    scalar = (Reg::RegTensor<int64_t>&)tmpReg2;
    Reg::ShiftLeft(tmpReg1, tmpReg1, scalar, tmpMask1);
    Reg::Add(tmpReg1, tmpReg1, expReg, tmpMask1);
    Reg::Select(srcReg, tmpReg1, srcReg, tmpMask1);
}

template <RoundMode roundMode>
__simd_callee__ inline void DealHigherThan52(
    Reg::MaskReg& tmpMask1, Reg::MaskReg& tmpNegative, Reg::RegTensor<uint64_t>& srcReg,
    Reg::RegTensor<uint64_t>& countZero)
{
    constexpr uint64_t dupNum0 = 1;
    Reg::MaskReg tmpMask;
    Reg::RegTensor<int64_t> scalar;
    Reg::RegTensor<uint64_t> tmpReg1, expReg, mantissaReg, discardBit, midTmp, tmpReg2;
    Reg::Duplicate(tmpReg1, dupNum0);
    scalar = (Reg::RegTensor<int64_t>&)countZero;
    Reg::ShiftLeft(tmpReg1, tmpReg1, scalar, tmpMask1);
    Reg::Sub(tmpReg2, srcReg, tmpReg1, tmpMask1);
    constexpr uint64_t dupNum1 = 1023;
    Reg::Duplicate(tmpReg1, dupNum1);
    Reg::Add(expReg, tmpReg1, countZero, tmpMask1);
    constexpr uint64_t dupNum2 = 52;
    Reg::Duplicate(tmpReg1, dupNum2);
    scalar = (Reg::RegTensor<int64_t>&)tmpReg1;
    Reg::ShiftLeft(expReg, expReg, scalar, tmpMask1);
    Reg::Sub(tmpReg1, countZero, tmpReg1, tmpMask1);
    scalar = (Reg::RegTensor<int64_t>&)tmpReg1;
    Reg::ShiftRight(mantissaReg, tmpReg2, scalar, tmpMask1);
    Reg::ShiftLeft(mantissaReg, mantissaReg, scalar, tmpMask1);
    Reg::Sub(discardBit, tmpReg2, mantissaReg, tmpMask1);
    Reg::ShiftRight(mantissaReg, mantissaReg, scalar, tmpMask1);
    constexpr uint64_t dupNum3 = 1;
    Reg::Duplicate(midTmp, dupNum3);
    Reg::Sub(tmpReg1, tmpReg1, midTmp, tmpMask1);
    scalar = (Reg::RegTensor<int64_t>&)tmpReg1;
    Reg::ShiftLeft(midTmp, midTmp, scalar, tmpMask1);
    Reg::Duplicate(tmpReg1, dupNum3);
    if constexpr (roundMode == RoundMode::CAST_RINT) {
        Reg::Compare<uint64_t, CMPMODE::GT>(tmpMask, discardBit, midTmp, tmpMask1);
        Reg::Add(tmpReg2, mantissaReg, tmpReg1, tmpMask);
        Reg::Select(mantissaReg, tmpReg2, mantissaReg, tmpMask);
        Reg::Compare<uint64_t, CMPMODE::EQ>(tmpMask, discardBit, midTmp, tmpMask1);
        Reg::And(discardBit, mantissaReg, tmpReg1, tmpMask);
        Reg::CompareScalar<uint64_t, CMPMODE::EQ>(tmpMask, discardBit, 1, tmpMask);
        Reg::Add(tmpReg2, mantissaReg, tmpReg1, tmpMask);
        Reg::Select(mantissaReg, tmpReg2, mantissaReg, tmpMask);
    } else if constexpr (roundMode == RoundMode::CAST_FLOOR) {
        Reg::MaskAnd(tmpMask, tmpNegative, tmpMask1, tmpMask1);
        Reg::CompareScalar<uint64_t, CMPMODE::GT>(tmpMask, discardBit, 0, tmpMask);
        Reg::Add(tmpReg2, mantissaReg, tmpReg1, tmpMask);
        Reg::Select(mantissaReg, tmpReg2, mantissaReg, tmpMask);
    } else if constexpr (roundMode == RoundMode::CAST_CEIL) {
        Reg::MaskXor(tmpMask, tmpNegative, tmpMask1, tmpMask1);
        Reg::CompareScalar<uint64_t, CMPMODE::GT>(tmpMask, discardBit, 0, tmpMask);
        Reg::Add(tmpReg2, mantissaReg, tmpReg1, tmpMask);
        Reg::Select(mantissaReg, tmpReg2, mantissaReg, tmpMask);
    } else if constexpr (roundMode == RoundMode::CAST_ROUND) {
        Reg::Compare<uint64_t, CMPMODE::GE>(tmpMask, discardBit, midTmp, tmpMask1);
        Reg::Add(tmpReg2, mantissaReg, tmpReg1, tmpMask);
        Reg::Select(mantissaReg, tmpReg2, mantissaReg, tmpMask);
    }
    Reg::Add(tmpReg2, mantissaReg, expReg, tmpMask1);
    Reg::Select(srcReg, tmpReg2, srcReg, tmpMask1);
}

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode>
__simd_vf__ inline void CastInt64ToDouble(__ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint32_t calCount)
{
    constexpr uint16_t oneRepSize = GetVecLen() / sizeof(double);
    uint16_t repeatTime = CeilDivision(calCount, oneRepSize);
    uint32_t sreg = static_cast<uint16_t>(calCount);
    uint64_t num1;
    Reg::MaskReg preg, tmpMask, tmpMask1, tmpNegative;
    Reg::RegTensor<int64_t> tmp, tmp1;
    Reg::RegTensor<uint64_t> srcReg, countZero, tmpReg;
    Reg::RegTensor<double> dstReg;
    for (uint16_t i = 0; i < repeatTime; ++i) {
        preg = Reg::UpdateMask<int64_t, Reg::RegTraitNumOne>(sreg);
        Reg::LoadAlign(tmp, src + i * oneRepSize);
        Reg::CompareScalar<int64_t, CMPMODE::LT>(tmpNegative, tmp, 0, preg);
        Reg::Duplicate(tmp1, 0);
        Reg::Sub(tmp1, tmp1, tmp, tmpNegative);
        Reg::Select(tmp, tmp1, tmp, tmpNegative);
        srcReg = (Reg::RegTensor<uint64_t>&)tmp;
        num1 = 0;
        Reg::Duplicate(countZero, num1);
        num1 = 64;
        Clz(preg, srcReg, countZero);
        Reg::CompareScalar<uint64_t, CMPMODE::EQ>(tmpMask, countZero, num1, preg);
        num1 = 0;
        Reg::Duplicate(tmpReg, num1);
        Reg::Select(srcReg, tmpReg, srcReg, tmpMask);
        Reg::MaskXor(tmpMask, tmpMask, preg, preg);
        num1 = 63;
        Reg::Duplicate(tmpReg, num1);
        Reg::Sub(countZero, tmpReg, countZero, tmpMask);
        num1 = 52;
        Reg::CompareScalar<uint64_t, CMPMODE::LE>(tmpMask1, countZero, num1, tmpMask);
        DealLowerThan52(tmpMask1, srcReg, countZero);
        Reg::MaskXor(tmpMask, tmpMask1, tmpMask, tmpMask);
        DealHigherThan52<roundMode>(tmpMask, tmpNegative, srcReg, countZero);
        num1 = 0x8000000000000000;
        Reg::Duplicate(tmpReg, num1);
        Reg::Add(tmpReg, srcReg, tmpReg, tmpNegative);
        Reg::Select(srcReg, tmpReg, srcReg, tmpNegative);
        dstReg = (Reg::RegTensor<double>&)srcReg;
        Reg::StoreAlign(dst + i * oneRepSize, dstReg, preg);
    }
}

template <typename T = Reg::DefaultType, typename U = Reg::DefaultType, typename S, typename V>
__simd_callee__ inline void CastDoubleToInt32Impl(
    V& dstVreg, V& int32Max, V& int32Min, S& srcReg, Reg::RegTensor<int32_t>& scalar0, Reg::RegTensor<int32_t>& scalar1,
    Reg::RegTensor<int32_t>& scalar2, Reg::MaskReg& mask)
{
    Reg::RegTensor<int64_t, Reg::RegTraitNumTwo> tmpSrcReg = (Reg::RegTensor<int64_t, Reg::RegTraitNumTwo>&)srcReg;
    Reg::RegTensor<int32_t> sign, mLow, mHigh, exponent;
    Reg::RegTensor<int32_t> tmpReg, resReg, tmpReg0, resReg0, scalar;
    Reg::MaskReg cmpMask, cmpMask0, cmpMask1, cmpMask2;
    Reg::Duplicate(dstVreg, static_cast<int32_t>(0));

    // m_low = bits_low
    Reg::Copy(mLow, (Reg::RegTensor<int32_t>&)tmpSrcReg.reg[0], mask);
    // exponent = (bits_high >> 20) & 0x7ff
    Reg::ShiftRights(exponent, (Reg::RegTensor<int32_t>&)tmpSrcReg.reg[1], static_cast<int16_t>(20), mask);
    Reg::And(exponent, exponent, scalar1, mask);
    // sign = (bits_high >> 31) & 1
    Reg::ShiftRights(sign, (Reg::RegTensor<int32_t>&)tmpSrcReg.reg[1], static_cast<int16_t>(31), mask);
    // m_high = bits_high & 0xfffff
    Reg::And(mHigh, (Reg::RegTensor<int32_t>&)tmpSrcReg.reg[1], scalar2, mask);

    /*
        if E == 0:
            return 0

        if E == 0x7ff:
            if m_high != 0 or m_low != 0: (nan condition)
                return 0
            else: (inf condition)
                if S == 0:
                    return INT32_MAX
                else:
                    return INT32_MIN
    */
    Reg::Compare(cmpMask0, exponent, scalar1, mask);
    Reg::CompareScalar(cmpMask1, mLow, static_cast<int32_t>(0), cmpMask0);
    Reg::CompareScalar(cmpMask1, mHigh, static_cast<int32_t>(0), cmpMask1);

    Reg::CompareScalar(cmpMask2, sign, static_cast<int32_t>(0), cmpMask1);
    Reg::Select(resReg, int32Max, int32Min, cmpMask2);
    Reg::Select(dstVreg, resReg, dstVreg, cmpMask1);

    Reg::CompareScalar<int32_t, CMPMODE::NE>(cmpMask1, exponent, static_cast<int32_t>(0), mask);
    // handle E != 0 and E != 0x7ff scenario
    // !cmpMask0 && cmpMask1 -> cmpMask
    Reg::MaskNot(cmpMask, cmpMask0, cmpMask1);

    /*
        exp = E - 1023

        if exp >= 31: (deal with overflow and saturation scenario)
            if S == 0:
                return INT32_MAX
            else:
                return INT32_MIN

        if exp < 0:
            return 0
    */
    Reg::Adds(exponent, exponent, static_cast<int32_t>(-1023), cmpMask);
    Reg::CompareScalar<int32_t, CMPMODE::GE>(cmpMask0, exponent, static_cast<int32_t>(31), cmpMask);
    Reg::CompareScalar(cmpMask1, sign, static_cast<int32_t>(0), cmpMask0);
    Reg::Select(resReg, int32Max, int32Min, cmpMask1);
    Reg::Select(dstVreg, resReg, dstVreg, cmpMask0);

    Reg::CompareScalar<int32_t, CMPMODE::GE>(cmpMask1, exponent, static_cast<int32_t>(0), cmpMask);
    // handle E != 0 and E != 0x7ff and 0 <= exp < 31 scenario
    // (cmpMask0 ^ cmpMask1) && cmpMask -> cmpMask
    Reg::MaskXor(cmpMask, cmpMask0, cmpMask1, cmpMask);

    /*
        shift = 52 - exp
        mantissa_high_21bits = 0x100000 | m_high
        if shift >= 32:
            result = mantissa_high_21bits >> (shift - 32)
        else:
            high_contribution = mantissa_high_21bits << (32 - shift)
            low_contribution = m_low >> shift
            result = (high_contribution | low_contribution)
    */
    Reg::Duplicate(scalar, static_cast<int32_t>(52), cmpMask);
    Reg::Sub(exponent, scalar, exponent, cmpMask);
    Reg::Or(tmpReg, mHigh, scalar0, cmpMask);

    Reg::CompareScalar<int32_t, CMPMODE::GE>(cmpMask0, exponent, static_cast<int32_t>(32), cmpMask);
    Reg::Duplicate(scalar, static_cast<int32_t>(32), cmpMask);
    Reg::Sub(tmpReg0, exponent, scalar, cmpMask0);
    Reg::ShiftRight(resReg, tmpReg, tmpReg0, cmpMask0);

    Reg::MaskNot(cmpMask1, cmpMask0, cmpMask);
    Reg::Sub(tmpReg0, scalar, exponent, cmpMask1);
    Reg::ShiftLeft(resReg0, tmpReg, tmpReg0, cmpMask1);
    Reg::ShiftRight((Reg::RegTensor<uint32_t>&)tmpReg0, (Reg::RegTensor<uint32_t>&)mLow, exponent, cmpMask1);
    Reg::Or(resReg0, resReg0, tmpReg0, cmpMask1);
    Reg::Or(resReg, resReg0, resReg, cmpMask);

    /*
        if S == 1:
            if result = 0x80000000:
                return INT32_MIN

            result = -result
            if result < INT32_MIN:
                return INT32_MIN
        else:
            if result > INT32_MAX:
                return INT32_MAX
        return result
    */
    Reg::CompareScalar(cmpMask0, sign, static_cast<int32_t>(-1), cmpMask);
    Reg::CompareScalar(cmpMask1, resReg, static_cast<int32_t>(0x80000000), cmpMask0);
    Reg::Neg(resReg0, resReg, cmpMask0);
    Reg::Compare<int32_t, CMPMODE::LT>(cmpMask2, resReg0, int32Min, cmpMask0);
    Reg::MaskOr(cmpMask1, cmpMask1, cmpMask2, cmpMask0);
    Reg::Select(dstVreg, int32Min, dstVreg, cmpMask1);
    Reg::Select(resReg, resReg0, resReg, cmpMask0);
    Reg::MaskNot(cmpMask0, cmpMask0, cmpMask);
    Reg::Compare<int32_t, CMPMODE::GT>(cmpMask2, resReg, int32Max, cmpMask0);
    Reg::Select(dstVreg, int32Max, dstVreg, cmpMask2);
    // handle non-inf scenario
    Reg::MaskOr(cmpMask1, cmpMask1, cmpMask2, cmpMask);
    Reg::MaskNot(cmpMask2, cmpMask1, cmpMask);
    Reg::Select(dstVreg, resReg, dstVreg, cmpMask2);
}

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode>
__simd_vf__ inline void CastDoubleToInt32(__ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint32_t calCount)
{
    constexpr uint16_t oneRepSize = 2 * GetVecLen() / sizeof(double);
    uint16_t repeatTime = CeilDivision(calCount, oneRepSize);
    uint32_t sreg = static_cast<uint32_t>(calCount);

    Reg::MaskReg mask;
    Reg::RegTensor<SRC_TYPE, Reg::RegTraitNumTwo> srcVreg;
    Reg::RegTensor<DST_TYPE> dstVreg, int32Max, int32Min;
    Reg::RegTensor<int32_t> scalar0, scalar1, scalar2;
    Reg::Duplicate(scalar0, static_cast<int32_t>(0x100000));
    Reg::Duplicate(scalar1, static_cast<int32_t>(0x7ff));
    Reg::Duplicate(scalar2, static_cast<int32_t>(0xfffff));
    Reg::Duplicate(int32Max, 2147483647);
    Reg::Duplicate(int32Min, -2147483648);
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<uint64_t, Reg::RegTraitNumTwo>(sreg);
        Reg::LoadAlign(srcVreg, src + i * oneRepSize);
        CastDoubleToInt32Impl(dstVreg, int32Max, int32Min, srcVreg, scalar0, scalar1, scalar2, mask);
        Reg::StoreAlign(dst + i * oneRepSize, dstVreg, mask);
    }
}

__simd_callee__ inline void ShiftRightDual32(
    Reg::RegTensor<int32_t>& dstVregLow, Reg::RegTensor<int32_t>& dstVregHigh, Reg::RegTensor<int32_t>& mLow,
    Reg::RegTensor<int32_t>& mHigh, Reg::RegTensor<int32_t>& shiftReg, Reg::RegTensor<int32_t>& tmpReg0,
    Reg::RegTensor<int32_t>& tmpReg1, Reg::RegTensor<int32_t>& scalar, Reg::MaskReg& cmpMask0, Reg::MaskReg& cmpMask1,
    Reg::MaskReg& mask)
{
    constexpr int32_t i32 = 32;
    constexpr int32_t in32 = -32;
    constexpr int32_t i64 = 64;
    /*
        if n == 0:
            return (high, low)
        elif n >= 64:
            return (0, 0)
        elif n >= 32:
            return (0, high >> (n - 32))
        else:
            new_low = (low >> n) | ((high & ((1 << n) - 1)) << (32 - n))
            new_high = high >> n
            return (new_high, new_low & 0xffffffff)

    */
    Reg::CompareScalar<int32_t, CMPMODE::LT>(cmpMask0, shiftReg, i64, mask);
    Reg::CompareScalar<int32_t, CMPMODE::LT>(cmpMask1, shiftReg, i32, mask);
    Reg::MaskXor(cmpMask0, cmpMask0, cmpMask1, mask);
    Reg::Adds(tmpReg0, shiftReg, in32, cmpMask0);
    Reg::ShiftRight(tmpReg0, mHigh, tmpReg0, cmpMask0);
    Reg::Select(dstVregLow, tmpReg0, dstVregLow, cmpMask0);

    Reg::ShiftRight((Reg::RegTensor<uint32_t>&)tmpReg0, (Reg::RegTensor<uint32_t>&)mLow, shiftReg, cmpMask1);
    Reg::Duplicate(tmpReg1, static_cast<int32_t>(1), cmpMask1);
    Reg::ShiftLeft(tmpReg1, tmpReg1, shiftReg, cmpMask1);
    Reg::Adds(tmpReg1, tmpReg1, static_cast<int32_t>(-1), cmpMask1);
    Reg::And(tmpReg1, tmpReg1, mHigh, cmpMask1);
    Reg::Duplicate(scalar, i32, cmpMask1);
    Reg::Sub(scalar, scalar, shiftReg, cmpMask1);
    Reg::ShiftLeft(tmpReg1, tmpReg1, scalar, cmpMask1);
    Reg::Or(tmpReg0, tmpReg1, tmpReg0, cmpMask1);
    Reg::ShiftRight(tmpReg1, mHigh, shiftReg, cmpMask1);
    Reg::Select(dstVregLow, tmpReg0, dstVregLow, cmpMask1);
    Reg::Select(dstVregHigh, tmpReg1, dstVregHigh, cmpMask1);

    Reg::CompareScalar<int32_t, CMPMODE::EQ>(cmpMask0, shiftReg, static_cast<int32_t>(0), mask);
    Reg::Select(dstVregLow, mLow, dstVregLow, cmpMask0);
    Reg::Select(dstVregHigh, mHigh, dstVregHigh, cmpMask0);
}

__simd_callee__ inline void ShiftLeftDual32(
    Reg::RegTensor<int32_t>& dstVregLow, Reg::RegTensor<int32_t>& dstVregHigh, Reg::RegTensor<int32_t>& mLow,
    Reg::RegTensor<int32_t>& mHigh, Reg::RegTensor<int32_t>& shiftReg, Reg::RegTensor<int32_t>& tmpReg0,
    Reg::RegTensor<int32_t>& tmpReg1, Reg::RegTensor<int32_t>& scalar, Reg::MaskReg& cmpMask0, Reg::MaskReg& cmpMask1,
    Reg::MaskReg& mask)
{
    constexpr int32_t i32 = 32;
    constexpr int32_t in32 = -32;
    constexpr int32_t i64 = 64;
    /*
        if n == 0:
            return (high, low)
        elif n >= 64:
            return (0, 0)
        elif n >= 32:
            return ((low << (n - 32)), 0)
        else:
            new_high = (high << n) | (low  >> (32 - n))
            new_low = (low << n)
            return (new_high, new_low)
    */
    Reg::CompareScalar<int32_t, CMPMODE::LT>(cmpMask0, shiftReg, i64, mask);
    Reg::CompareScalar<int32_t, CMPMODE::LT>(cmpMask1, shiftReg, i32, mask);
    Reg::MaskXor(cmpMask0, cmpMask0, cmpMask1, mask);
    Reg::Adds(tmpReg0, shiftReg, in32, cmpMask0);
    Reg::ShiftLeft(tmpReg0, mLow, tmpReg0, cmpMask0);
    Reg::Select(dstVregHigh, tmpReg0, dstVregHigh, cmpMask0);

    Reg::ShiftLeft(tmpReg0, mHigh, shiftReg, cmpMask1);
    Reg::Duplicate(scalar, i32, cmpMask1);
    Reg::Sub(scalar, scalar, shiftReg, cmpMask1);
    Reg::ShiftRight((Reg::RegTensor<uint32_t>&)tmpReg1, (Reg::RegTensor<uint32_t>&)mLow, scalar, cmpMask1);
    Reg::Or(tmpReg0, tmpReg1, tmpReg0, cmpMask1);
    Reg::ShiftLeft(tmpReg1, mLow, shiftReg, cmpMask1);
    Reg::Select(dstVregLow, tmpReg1, dstVregLow, cmpMask1);
    Reg::Select(dstVregHigh, tmpReg0, dstVregHigh, cmpMask1);

    Reg::CompareScalar<int32_t, CMPMODE::EQ>(cmpMask0, shiftReg, static_cast<int32_t>(0), mask);
    Reg::Select(dstVregLow, mLow, dstVregLow, cmpMask0);
    Reg::Select(dstVregHigh, mHigh, dstVregHigh, cmpMask0);
}

__simd_callee__ inline void NegateDual32(
    Reg::RegTensor<int32_t>& dstVregLow, Reg::RegTensor<int32_t>& dstVregHigh, Reg::RegTensor<int32_t>& tmpReg0,
    Reg::RegTensor<int32_t>& tmpReg1, Reg::MaskReg& cmpMask0, Reg::MaskReg& mask)
{
    /*
        low = -low
        high = ~high
        if low == 0:
            high += 1

        return (high, low)
    */
    Reg::Neg(tmpReg0, dstVregLow, mask);
    Reg::Select(dstVregLow, tmpReg0, dstVregLow, mask);
    Reg::Not(tmpReg1, dstVregHigh, mask);
    Reg::CompareScalar<int32_t, CMPMODE::EQ>(cmpMask0, tmpReg0, static_cast<int32_t>(0), mask);
    Reg::Adds(tmpReg0, tmpReg1, static_cast<int32_t>(1), cmpMask0);
    Reg::Select(tmpReg1, tmpReg0, tmpReg1, cmpMask0);
    Reg::Select(dstVregHigh, tmpReg1, dstVregHigh, mask);
}

template <typename T = Reg::DefaultType, typename U>
__simd_callee__ inline void CastDoubleToInt64Impl(
    Reg::RegTensor<int32_t>& dstVregLow, Reg::RegTensor<int32_t>& dstVregHigh, U& srcReg,
    Reg::RegTensor<int32_t>& scalar0, Reg::RegTensor<int32_t>& scalar1, Reg::RegTensor<int32_t>& scalar2,
    Reg::RegTensor<int32_t>& posInfLow, Reg::RegTensor<int32_t>& posInfHigh, Reg::RegTensor<int32_t>& negInfLow,
    Reg::RegTensor<int32_t>& negInfHigh, Reg::RegTensor<int32_t>& zeroReg, Reg::MaskReg& mask)
{
    Reg::RegTensor<int64_t, Reg::RegTraitNumTwo> tmpSrcReg = (Reg::RegTensor<int64_t, Reg::RegTraitNumTwo>&)srcReg;
    Reg::RegTensor<int32_t> sign, mLow, mHigh, exponent;
    Reg::RegTensor<int32_t> tmpReg, tmpReg0, tmpReg1, tmpReg2, tmpReg3, tmpReg4, resRegLow, resRegHigh, scalar;
    Reg::MaskReg cmpMask0, cmpMask1, cmpMask2;
    Reg::Duplicate(dstVregLow, static_cast<int32_t>(0));
    Reg::Duplicate(dstVregHigh, static_cast<int32_t>(0));
    Reg::Duplicate(resRegLow, static_cast<int32_t>(0));
    Reg::Duplicate(resRegHigh, static_cast<int32_t>(0));

    // m_low = bits_low
    Reg::Copy(mLow, (Reg::RegTensor<int32_t>&)tmpSrcReg.reg[0], mask);
    // exponent = (bits_high >> 20) & 0x7ff
    Reg::ShiftRights(exponent, (Reg::RegTensor<int32_t>&)tmpSrcReg.reg[1], static_cast<int16_t>(20), mask);
    Reg::And(exponent, exponent, scalar1, mask);
    // sign = (bits_high >> 31) & 1
    Reg::ShiftRights(sign, (Reg::RegTensor<int32_t>&)tmpSrcReg.reg[1], static_cast<int16_t>(31), mask);
    // m_high = bits_high & 0xfffff
    Reg::And(mHigh, (Reg::RegTensor<int32_t>&)tmpSrcReg.reg[1], scalar2, mask);

    /*
        mantissa_high = 0x100000 | m_high
        mantissa_low = m_low

        exp = E - 1023
        shift_r = 52 - exp
        shift_l = exp - 52

        s_r = shift_r if shift_r > 0 else 0
        s_l = shift_l if shift_l > 0 else 0

        val_r_hi, val_r_lo = shift_right_dual32(mantissa_high, mantissa_low, s_r)
        val_l_hi, val_l_lo = shift_left_dual32(mantissa_high, mantissa_low, s_l)

        is_right_shift = (exp < 52)
        raw_hi = val_r_hi if is_right_shift else val_l_hi
        raw_lo = val_r_lo if is_right_shift else val_l_lo
    */
    Reg::Or(tmpReg, mHigh, scalar0, mask);
    Reg::Adds(tmpReg0, exponent, static_cast<int32_t>(-1023), mask);
    Reg::Duplicate(scalar, static_cast<int32_t>(52), mask);
    Reg::Sub(tmpReg1, scalar, tmpReg0, mask);
    Reg::Sub(tmpReg2, tmpReg0, scalar, mask);
    Reg::CompareScalar<int32_t, CMPMODE::GT>(cmpMask0, tmpReg1, static_cast<int32_t>(0), mask);
    Reg::Or(tmpReg1, tmpReg1, zeroReg, cmpMask0);
    Reg::CompareScalar<int32_t, CMPMODE::GT>(cmpMask1, tmpReg2, static_cast<int32_t>(0), mask);
    Reg::Or(tmpReg2, tmpReg2, zeroReg, cmpMask1);
    ShiftRightDual32(
        dstVregLow, dstVregHigh, mLow, tmpReg, tmpReg1, tmpReg3, tmpReg4, scalar, cmpMask0, cmpMask1, mask);
    ShiftLeftDual32(resRegLow, resRegHigh, mLow, tmpReg, tmpReg2, tmpReg3, tmpReg4, scalar, cmpMask0, cmpMask1, mask);

    Reg::CompareScalar<int32_t, CMPMODE::LT>(cmpMask2, tmpReg0, static_cast<int32_t>(52), mask);
    Reg::Select(dstVregLow, dstVregLow, resRegLow, cmpMask2);
    Reg::Select(dstVregHigh, dstVregHigh, resRegHigh, cmpMask2);

    /*
        neg_hi, neg_lo = negate_dual32(result_high, result_low)

        res_hi = neg_hi if S == 1 else raw_hi
        res_lo = neg_lo if S == 1 else raw_lo

        over_hi = 0x80000000 if S else 0x7fffffff
        over_lo = 0x00000000 if S else 0xffffffff
    */
    Reg::CompareScalar(cmpMask0, sign, static_cast<int32_t>(-1), mask);
    NegateDual32(dstVregLow, dstVregHigh, tmpReg1, tmpReg2, cmpMask1, cmpMask0);
    Reg::Select(tmpReg3, negInfLow, posInfLow, cmpMask0);
    Reg::Select(tmpReg4, negInfHigh, posInfHigh, cmpMask0);

    /*
        is_inf = (E == 0x7ff) and ((m_high | m_low) == 0)
        is_large = exp >= 63
        is_int64_min = (exp == 63) and (m_high == 0) and (m_low == 0) and (S == 1)

        is_overflow = if_inf or (is_large and !is_int64_min)

        fin_hi = over_hi if is_overflow else res_hi
        fin_lo = over_lo if is_overflow else res_lo

        is_nan = (E == 0x7ff) and ((m_high | m_low) != 0)
        is_not_zero = (E != 0) and (exp >= 0) and !is_nan

        fin_hi = fin_hi if is_not_zero else 0
        fin_lo = fin_lo if is_not_zero else 0

        return (fin_hi, fin_lo)
    */
    Reg::Compare(cmpMask0, exponent, scalar1, mask);
    Reg::Or(tmpReg1, mLow, mHigh, cmpMask0);
    Reg::CompareScalar(cmpMask0, tmpReg1, static_cast<int32_t>(0), cmpMask0);

    Reg::CompareScalar<int32_t, CMPMODE::GE>(cmpMask1, tmpReg0, static_cast<int32_t>(63), mask);

    Reg::CompareScalar<int32_t, CMPMODE::EQ>(cmpMask2, tmpReg0, static_cast<int32_t>(63), mask);
    Reg::CompareScalar<int32_t, CMPMODE::EQ>(cmpMask2, mLow, static_cast<int32_t>(0), cmpMask2);
    Reg::CompareScalar<int32_t, CMPMODE::EQ>(cmpMask2, mHigh, static_cast<int32_t>(0), cmpMask2);
    Reg::CompareScalar<int32_t, CMPMODE::EQ>(cmpMask2, sign, static_cast<int32_t>(-1), cmpMask2);

    Reg::MaskNot(cmpMask2, cmpMask2, cmpMask1);
    Reg::MaskOr(cmpMask0, cmpMask2, cmpMask0, mask);

    Reg::Select(dstVregLow, tmpReg3, dstVregLow, cmpMask0);
    Reg::Select(dstVregHigh, tmpReg4, dstVregHigh, cmpMask0);

    Reg::Compare(cmpMask2, exponent, scalar1, mask);
    Reg::CompareScalar<int32_t, CMPMODE::NE>(cmpMask2, tmpReg1, static_cast<int32_t>(0), cmpMask2);

    Reg::Compare<int32_t, CMPMODE::NE>(cmpMask1, exponent, zeroReg, mask);
    Reg::CompareScalar<int32_t, CMPMODE::GE>(cmpMask1, tmpReg0, static_cast<int32_t>(0), cmpMask1);
    Reg::MaskNot(cmpMask2, cmpMask2, cmpMask1);

    Reg::Select(dstVregLow, dstVregLow, zeroReg, cmpMask2);
    Reg::Select(dstVregHigh, dstVregHigh, zeroReg, cmpMask2);
}

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode>
__simd_vf__ inline void CastDoubleToInt64(__ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint32_t calCount)
{
    constexpr uint16_t oneRepSize = 2 * GetVecLen() / sizeof(double);
    uint16_t repeatTime = CeilDivision(calCount, oneRepSize);
    uint32_t sreg = static_cast<uint32_t>(calCount);

    Reg::MaskReg mask;
    Reg::RegTensor<SRC_TYPE, Reg::RegTraitNumTwo> srcVreg;
    Reg::RegTensor<DST_TYPE, Reg::RegTraitNumTwo> dstVreg, tmpVreg;
    Reg::RegTensor<int32_t> dstVregLow;
    Reg::RegTensor<int32_t> dstVregHigh;
    Reg::RegTensor<int32_t> scalar0, scalar1, scalar2, zeroReg;
    Reg::RegTensor<int32_t> posInfLow, posInfHigh, negInfLow, negInfHigh;

    Reg::Duplicate(scalar0, static_cast<int32_t>(0x100000));
    Reg::Duplicate(scalar1, static_cast<int32_t>(0x7ff));
    Reg::Duplicate(scalar2, static_cast<int32_t>(0xfffff));
    Reg::Duplicate(zeroReg, static_cast<int32_t>(0));
    Reg::Duplicate(posInfHigh, static_cast<int32_t>(0x7fffffff));
    Reg::Duplicate(posInfLow, static_cast<int32_t>(0xffffffff));
    Reg::Duplicate(negInfHigh, static_cast<int32_t>(0x80000000));
    Reg::Duplicate(negInfLow, static_cast<int32_t>(0x00000000));
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<uint64_t, Reg::RegTraitNumTwo>(sreg);
        Reg::LoadAlign(srcVreg, src + i * oneRepSize);
        CastDoubleToInt64Impl(
            dstVregLow, dstVregHigh, srcVreg, scalar0, scalar1, scalar2, posInfLow, posInfHigh, negInfLow, negInfHigh,
            zeroReg, mask);
        /*
            result = (result_high << 32) | result_low
            return result
        */
        Reg::Copy((Reg::RegTensor<int32_t>&)dstVreg.reg[0], dstVregLow, mask);
        Reg::Copy((Reg::RegTensor<int32_t>&)dstVreg.reg[1], dstVregHigh, mask);
        Reg::StoreAlign(dst + i * oneRepSize, dstVreg, mask);
    }
}

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode>
__aicore__ inline void CastDouble(__ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint32_t calCount)
{
    bool isGetOverflow = GetOverflow();
    constexpr bool cast_DoubleToFloat = SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<float, double>>();
    constexpr bool cast_DoubleToBf16 = SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<bfloat16_t, double>>();
    constexpr bool cast_Int64ToDouble = SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<double, int64_t>>();
    constexpr bool cast_DoubleToInt32 = SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<int32_t, double>>();
    constexpr bool cast_DoubleToInt64 = SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<int64_t, double>>();
    if constexpr (cast_DoubleToFloat) {
        CastDoubleToFloat<DST_TYPE, SRC_TYPE, roundMode>(dst, src, calCount, isGetOverflow);
    } else if constexpr (cast_DoubleToBf16) {
        CastDoubleToBf16<DST_TYPE, SRC_TYPE, roundMode>(dst, src, calCount, isGetOverflow);
    } else if constexpr (cast_Int64ToDouble) {
        CastInt64ToDouble<DST_TYPE, SRC_TYPE, roundMode>(dst, src, calCount);
    } else if constexpr (cast_DoubleToInt32) {
        CastDoubleToInt32<DST_TYPE, SRC_TYPE, roundMode>(dst, src, calCount);
    } else if constexpr (cast_DoubleToInt64) {
        CastDoubleToInt64<DST_TYPE, SRC_TYPE, roundMode>(dst, src, calCount);
    } else {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast"); });
    }
}

// Cast::Level 2
template <typename ORI_DST_TYPE, typename ORI_SRC_TYPE>
__aicore__ inline void CastImpl(
    __ubuf__ ORI_DST_TYPE* oriDst, __ubuf__ ORI_SRC_TYPE* oriSrc, const RoundMode& roundMode, const uint32_t calCount)
{
    using DST_TYPE = typename CastParam::CastTypeTrait<ORI_DST_TYPE>::RealType;
    using SRC_TYPE = typename CastParam::CastTypeTrait<ORI_SRC_TYPE>::RealType;
    __ubuf__ DST_TYPE* dst = reinterpret_cast<__ubuf__ DST_TYPE*>(oriDst);
    __ubuf__ SRC_TYPE* src = reinterpret_cast<__ubuf__ SRC_TYPE*>(oriSrc);

    constexpr bool cast_round_all = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<half, float>, Tuple<int64_t, float>, Tuple<int32_t, float>,
        Tuple<int16_t, float>, Tuple<bfloat16_t, float>, Tuple<int32_t, half>, Tuple<int16_t, half>,
        Tuple<int8_t, half>, Tuple<uint8_t, half>, Tuple<int4x2_t, half>, Tuple<bfloat16_t, half>, Tuple<half, int16_t>,
        Tuple<float, int32_t>, Tuple<float, int64_t>, Tuple<int32_t, bfloat16_t>, Tuple<half, bfloat16_t>,
        Tuple<fp4x2_e1m2_t, bfloat16_t>, Tuple<fp4x2_e2m1_t, bfloat16_t>, Tuple<half, int32_t>, Tuple<float, float>,
        Tuple<complex64, complex64>, Tuple<complex32, complex64>>();
    constexpr bool cast_none = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<half, int32_t>, Tuple<float, half>, Tuple<float, bfloat16_t>,
        Tuple<half, int4x2_t>, Tuple<half, uint8_t>, Tuple<uint16_t, uint8_t>, Tuple<uint32_t, uint8_t>,
        Tuple<half, int8_t>, Tuple<int16_t, int8_t>, Tuple<int32_t, int8_t>, Tuple<uint8_t, uint16_t>,
        Tuple<uint32_t, uint16_t>, Tuple<float, int16_t>, Tuple<uint8_t, int16_t>, Tuple<uint32_t, int16_t>,
        Tuple<int32_t, int16_t>, Tuple<uint8_t, uint32_t>, Tuple<uint16_t, uint32_t>, Tuple<int16_t, uint32_t>,
        Tuple<int64_t, int32_t>, Tuple<int16_t, int32_t>, Tuple<uint8_t, int32_t>, Tuple<uint16_t, int32_t>,
        Tuple<int32_t, int64_t>, Tuple<half, hifloat8_t>, Tuple<float, hifloat8_t>, Tuple<float, fp8_e4m3fn_t>,
        Tuple<float, fp8_e5m2_t>, Tuple<bfloat16_t, fp4x2_e1m2_t>, Tuple<bfloat16_t, fp4x2_e2m1_t>,
        Tuple<int4x2_t, int16_t>, Tuple<int16_t, int4x2_t>, Tuple<bfloat16_t, int4x2_t>, Tuple<complex64, complex32>>();
    constexpr bool using_cast_rint = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<int8_t, half>, Tuple<uint8_t, half>, Tuple<int4x2_t, half>, Tuple<half, float>,
        Tuple<half, int16_t>, Tuple<float, int32_t>, Tuple<complex32, complex64>>();
    constexpr bool cast_odd =
        SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<half, float>, Tuple<complex32, complex64>, Tuple<half, int32_t>>();
    constexpr bool cast_rint =
        SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<fp8_e5m2_t, float>, Tuple<fp8_e4m3fn_t, float>>();
    constexpr bool cast_round =
        SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<hifloat8_t, float>, Tuple<hifloat8_t, half>>();
    constexpr bool cast_hybrid = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<hifloat8_t, float>, Tuple<hifloat8_t, half>, Tuple<half, int32_t>>();
    constexpr bool cast_double = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<float, double>, Tuple<double, int64_t>, Tuple<bfloat16_t, double>>();
    constexpr bool cast_double0 =
        SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<int32_t, double>, Tuple<int64_t, double>>();
    switch (roundMode) {
        case RoundMode::CAST_RINT:
            if constexpr (cast_round_all || cast_rint) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_RINT>(dst, src, calCount);
            } else if constexpr (cast_double) {
                CastDouble<DST_TYPE, SRC_TYPE, RoundMode::CAST_RINT>(dst, src, calCount);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast rint"); });
            }
            break;
        case RoundMode::CAST_FLOOR:
            if constexpr (cast_round_all) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_FLOOR>(dst, src, calCount);
            } else if constexpr (cast_double) {
                CastDouble<DST_TYPE, SRC_TYPE, RoundMode::CAST_FLOOR>(dst, src, calCount);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast floor"); });
            }
            break;
        case RoundMode::CAST_CEIL:
            if constexpr (cast_round_all) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_CEIL>(dst, src, calCount);
            } else if constexpr (cast_double) {
                CastDouble<DST_TYPE, SRC_TYPE, RoundMode::CAST_CEIL>(dst, src, calCount);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast ceil"); });
            }
            break;
        case RoundMode::CAST_ROUND:
            if constexpr (cast_round_all || cast_round) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_ROUND>(dst, src, calCount);
            } else if constexpr (cast_double) {
                CastDouble<DST_TYPE, SRC_TYPE, RoundMode::CAST_ROUND>(dst, src, calCount);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast round"); });
            }
            break;
        case RoundMode::CAST_TRUNC:
            if constexpr (cast_round_all) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_TRUNC>(dst, src, calCount);
            } else if constexpr (cast_double || cast_double0) {
                CastDouble<DST_TYPE, SRC_TYPE, RoundMode::CAST_TRUNC>(dst, src, calCount);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast trunc"); });
            }
            break;
        case RoundMode::CAST_ODD:
            if constexpr (cast_odd) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_ODD>(dst, src, calCount);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast odd"); });
            }
            break;
        case RoundMode::CAST_NONE:
            if constexpr (cast_none) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_NONE>(dst, src, calCount);
            } else if constexpr (using_cast_rint) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_RINT>(dst, src, calCount);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast none"); });
            }
            break;
        case RoundMode::CAST_HYBRID:
            if constexpr (cast_hybrid) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_HYBRID>(dst, src, calCount);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast hybrid"); });
            }
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
            Reg::LoadAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (Reg::RegTensor<uint32_t>&)srcVreg,
                (__ubuf__ uint32_t*&)src + i * repeatParams.srcRepStride * elePerBlk, repeatParams.srcBlkStride,
                b32Preg);
            Reg::Interleave((Reg::RegTensor<uint32_t>&)srcVreg, tmpVreg, (Reg::RegTensor<uint32_t>&)srcVreg, zeroVreg);
        } else {
            // b64->b32
            Reg::LoadAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (Reg::RegTensor<uint32_t>&)srcVreg,
                (__ubuf__ uint32_t*&)src + i * repeatParams.srcRepStride * elePerBlk, repeatParams.srcBlkStride,
                b64Preg);
        }
        Reg::Cast<DST_TYPE, SRC_TYPE, castTrait>(dstVreg, srcVreg, b64Preg);
        if constexpr (sizeof(DST_TYPE) == sizeof(int64_t)) {
            // b32->b64
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_STORE>();
            Reg::StoreAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (__ubuf__ uint32_t*&)dst + i * repeatParams.dstRepStride * elePerBlk,
                (Reg::RegTensor<uint32_t>&)dstVreg, repeatParams.dstBlkStride, b64Preg);
        } else {
            // b64->b32
            Reg::DeInterleave(
                (Reg::RegTensor<uint32_t>&)dstVreg, tmpVreg, (Reg::RegTensor<uint32_t>&)dstVreg, zeroVreg);
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_STORE>();
            Reg::StoreAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (__ubuf__ uint32_t*&)dst + i * repeatParams.dstRepStride * elePerBlk,
                (Reg::RegTensor<uint32_t>&)dstVreg, repeatParams.dstBlkStride, b32Preg);
        }
    }
}

template <typename DST_TYPE, typename SRC_TYPE>
__simd_callee__ inline void GenLoadL0(
    Reg::RegTensor<SRC_TYPE>& srcVreg, __ubuf__ SRC_TYPE*& srcAddr, Reg::MaskReg& preg,
    const UnaryRepeatParams& repeatParams, uint16_t index)
{
    constexpr uint8_t elePerBlk = GetDataBlockSizeInBytes() / sizeof(SRC_TYPE);
    Reg::LoadAlign<SRC_TYPE, Reg::DataCopyMode::DATA_BLOCK_COPY>(
        srcVreg, srcAddr + index * repeatParams.srcRepStride * elePerBlk, repeatParams.srcBlkStride, preg);
    if constexpr (SupportType<SRC_TYPE, int4x2_t, fp4x2_e2m1_t, fp4x2_e1m2_t>() && sizeof(DST_TYPE) == 2) {
        Reg::UnPack<uint16_t, uint8_t>((Reg::RegTensor<uint16_t>&)srcVreg, (Reg::RegTensor<uint8_t>&)srcVreg);
        Reg::UnPack<uint32_t, uint16_t>((Reg::RegTensor<uint32_t>&)srcVreg, (Reg::RegTensor<uint16_t>&)srcVreg);
    } else if constexpr (sizeof(SRC_TYPE) == 1 && sizeof(DST_TYPE) == 2) {
        if constexpr (AscendC::Std::is_same<SRC_TYPE, int8_t>::value) {
            Reg::UnPack<int16_t, int8_t>((Reg::RegTensor<int16_t>&)srcVreg, srcVreg);
        } else {
            Reg::UnPack<uint16_t, uint8_t>((Reg::RegTensor<uint16_t>&)srcVreg, (Reg::RegTensor<uint8_t>&)srcVreg);
        }
    } else if constexpr (sizeof(SRC_TYPE) == 2 && sizeof(DST_TYPE) == 4) {
        if constexpr (AscendC::Std::is_same<SRC_TYPE, int16_t>::value) {
            Reg::UnPack<int32_t, int16_t>((Reg::RegTensor<int32_t>&)srcVreg, srcVreg);
        } else {
            Reg::UnPack<uint32_t, uint16_t>((Reg::RegTensor<uint32_t>&)srcVreg, (Reg::RegTensor<uint16_t>&)srcVreg);
        }
    } else if constexpr (sizeof(SRC_TYPE) == 1 && sizeof(DST_TYPE) == 4) {
        if constexpr (AscendC::Std::is_same<SRC_TYPE, int8_t>::value) {
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
    const UnaryRepeatParams& repeatParams, uint16_t index)
{
    constexpr uint8_t elePerBlk = GetDataBlockSizeInBytes() / sizeof(DST_TYPE);
    if constexpr (SupportType<DST_TYPE, int4x2_t, fp4x2_e2m1_t, fp4x2_e1m2_t>() && sizeof(SRC_TYPE) == 2) {
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
    Reg::StoreAlign<DST_TYPE, Reg::DataCopyMode::DATA_BLOCK_COPY>(
        dstAddr + index * repeatParams.dstRepStride * elePerBlk, dstVreg, repeatParams.dstBlkStride, preg);
}

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode>
__simd_vf__ inline void CastIntrinsicsImplVF2(
    __ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const BasicAPIMaskStruct maskArrayStruct, uint8_t repeatTime,
    const UnaryRepeatParams repeatParams, const half scale)
{
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, roundMode};
    Reg::MaskReg ldPreg;
    Reg::MaskReg exPreg;
    Reg::MaskReg stPreg;
    Reg::MaskReg dumpPreg;
    Reg::RegTensor<SRC_TYPE> srcVreg;
    Reg::RegTensor<DST_TYPE> dstVreg;
    Reg::RegTensor<float> tmpVreg;
    if constexpr (sizeof(DST_TYPE) == sizeof(SRC_TYPE)) {
        ldPreg = Reg::MoveMask<SRC_TYPE>();
        exPreg = ldPreg;
        stPreg = ldPreg;
    } else if constexpr (sizeof(DST_TYPE) < sizeof(SRC_TYPE)) {
        ldPreg = Reg::MoveMask<SRC_TYPE>();
        exPreg = ldPreg;
        Reg::MaskPack(stPreg, ldPreg);
        if constexpr (
            (SupportType<DST_TYPE, int4x2_t, fp4x2_e2m1_t, fp4x2_e1m2_t>() && sizeof(SRC_TYPE) == 2) ||
            (sizeof(DST_TYPE) == 1 && sizeof(SRC_TYPE) == 4)) {
            Reg::MaskPack(stPreg, stPreg);
        }
    } else if constexpr (sizeof(DST_TYPE) > sizeof(SRC_TYPE)) {
        stPreg = Reg::MoveMask<DST_TYPE>();
        exPreg = stPreg;
        Reg::MaskPack(ldPreg, stPreg);
        if constexpr (
            (SupportType<SRC_TYPE, int4x2_t, fp4x2_e2m1_t, fp4x2_e1m2_t>() && sizeof(DST_TYPE) == 2) ||
            (sizeof(SRC_TYPE) == 1 && sizeof(DST_TYPE) == 4)) {
            Reg::MaskPack(ldPreg, ldPreg);
            if constexpr (SupportType<SRC_TYPE, int4x2_t, fp4x2_e2m1_t, fp4x2_e1m2_t>() && sizeof(DST_TYPE) == 2) {
                Reg::MaskUnPack(stPreg, ldPreg);
                Reg::MaskUnPack(exPreg, stPreg);
                Reg::MaskInterleave<uint16_t>(stPreg, dumpPreg, stPreg, stPreg);
            }
        }
    }
    for (uint16_t i = 0; i < repeatTime; ++i) {
        GenLoadL0<DST_TYPE, SRC_TYPE>(srcVreg, src, ldPreg, repeatParams, i);
        if constexpr (AscendC::Std::is_same<SRC_TYPE, int32_t>::value && AscendC::Std::is_same<DST_TYPE, half>::value) {
            Reg::Cast<float, SRC_TYPE, CastParam::s322floatCastTrait>(tmpVreg, srcVreg, exPreg);
            Reg::Muls(tmpVreg, tmpVreg, DEQ_SHIFT_RIGHT_17_BIT, exPreg);
            Reg::Muls(tmpVreg, tmpVreg, static_cast<float>(scale), exPreg);
            Reg::Muls(tmpVreg, tmpVreg, DEQ_SHIFT_LEFT_17_BIT, exPreg);
            Reg::Cast<DST_TYPE, float, CastParam::f322F16CastTrait>(dstVreg, tmpVreg, exPreg);
        } else if constexpr (
            AscendC::Std::is_same<SRC_TYPE, float>::value && AscendC::Std::is_same<DST_TYPE, float>::value) {
            Reg::Truncate<DST_TYPE, roundMode>(dstVreg, srcVreg, exPreg);
        } else {
            Reg::Cast<DST_TYPE, SRC_TYPE, castTrait>(dstVreg, srcVreg, exPreg);
        }
        GenStoreL0<DST_TYPE, SRC_TYPE>(dst, dstVreg, stPreg, repeatParams, i);
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
        Reg::StoreAlign<uint64_t, Reg::MaskDist::DIST_PACK>(maskBuf, sprLoadMaskReg);
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
            Reg::LoadAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (Reg::RegTensor<uint32_t>&)srcVreg,
                (__ubuf__ uint32_t*&)src + i * repeatParams.srcRepStride * elePerBlk, repeatParams.srcBlkStride,
                b32Preg);
            Reg::Interleave((Reg::RegTensor<uint32_t>&)srcVreg, tmpVreg, (Reg::RegTensor<uint32_t>&)srcVreg, zeroVreg);
        } else {
            // b64->b32
            Reg::LoadAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (Reg::RegTensor<uint32_t>&)srcVreg,
                (__ubuf__ uint32_t*&)src + i * repeatParams.srcRepStride * elePerBlk, repeatParams.srcBlkStride,
                b64Preg);
        }
        Reg::Cast<DST_TYPE, SRC_TYPE, castTrait>(dstVreg, srcVreg, b64Preg);
        if constexpr (sizeof(DST_TYPE) == sizeof(int64_t)) {
            // b32->b64
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_STORE>();
            Reg::StoreAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (__ubuf__ uint32_t*&)dst + i * repeatParams.dstRepStride * elePerBlk,
                (Reg::RegTensor<uint32_t>&)dstVreg, repeatParams.dstBlkStride, b64Preg);
        } else {
            // b64->b32
            Reg::DeInterleave(
                (Reg::RegTensor<uint32_t>&)dstVreg, tmpVreg, (Reg::RegTensor<uint32_t>&)dstVreg, zeroVreg);
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_STORE>();
            Reg::StoreAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (__ubuf__ uint32_t*&)dst + i * repeatParams.dstRepStride * elePerBlk,
                (Reg::RegTensor<uint32_t>&)dstVreg, repeatParams.dstBlkStride, b32Preg);
        }
    }
}

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode, bool isSetMask>
__simd_vf__ inline void CastIntrinsicsImplCounterVF(
    __ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint64_t mask, __ubuf__ uint64_t* maskBuf, uint8_t repeatTime,
    const UnaryRepeatParams repeatParams, const half scale)
{
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, roundMode};
    Reg::MaskReg ldPreg;
    Reg::MaskReg exPreg;
    Reg::MaskReg stPreg;
    Reg::MaskReg dumpPreg;
    Reg::RegTensor<SRC_TYPE> srcVreg;
    Reg::RegTensor<DST_TYPE> dstVreg;
    Reg::RegTensor<float> tmpVreg;
    uint32_t countSreg = static_cast<uint32_t>(mask);
    if constexpr (!isSetMask) {
        // get SPR.MASK in VF
        Reg::MaskReg sprLoadMaskReg = Reg::MoveMask<uint16_t>();
        Reg::StoreAlign<uint64_t, Reg::MaskDist::DIST_PACK>(maskBuf, sprLoadMaskReg);
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
                (SupportType<DST_TYPE, int4x2_t, fp4x2_e2m1_t, fp4x2_e1m2_t>() && sizeof(SRC_TYPE) == 2) ||
                (sizeof(DST_TYPE) == 1 && sizeof(SRC_TYPE) == 4)) {
                Reg::MaskPack(stPreg, stPreg);
            }
        } else if constexpr (sizeof(DST_TYPE) > sizeof(SRC_TYPE)) {
            stPreg = Reg::UpdateMask<DST_TYPE>(countSreg);
            exPreg = stPreg;
            Reg::MaskPack(ldPreg, stPreg);
            if constexpr (
                (SupportType<SRC_TYPE, int4x2_t, fp4x2_e2m1_t, fp4x2_e1m2_t>() && sizeof(DST_TYPE) == 2) ||
                (sizeof(SRC_TYPE) == 1 && sizeof(DST_TYPE) == 4)) {
                Reg::MaskPack(ldPreg, ldPreg);
                if constexpr (SupportType<SRC_TYPE, int4x2_t, fp4x2_e2m1_t, fp4x2_e1m2_t>() && sizeof(DST_TYPE) == 2) {
                    Reg::MaskUnPack(stPreg, ldPreg);
                    Reg::MaskUnPack(exPreg, stPreg);
                    Reg::MaskInterleave<uint16_t>(stPreg, dumpPreg, stPreg, stPreg);
                }
            }
        }
        GenLoadL0<DST_TYPE, SRC_TYPE>(srcVreg, src, ldPreg, repeatParams, i);
        if constexpr (AscendC::Std::is_same<SRC_TYPE, int32_t>::value && AscendC::Std::is_same<DST_TYPE, half>::value) {
            Reg::Cast<float, SRC_TYPE, CastParam::s322floatCastTrait>(tmpVreg, srcVreg, exPreg);
            Reg::Muls(tmpVreg, tmpVreg, DEQ_SHIFT_RIGHT_17_BIT, exPreg);
            Reg::Muls(tmpVreg, tmpVreg, static_cast<float>(scale), exPreg);
            Reg::Muls(tmpVreg, tmpVreg, DEQ_SHIFT_LEFT_17_BIT, exPreg);
            Reg::Cast<DST_TYPE, float, CastParam::f322F16CastTrait>(dstVreg, tmpVreg, exPreg);
        } else if constexpr (
            AscendC::Std::is_same<SRC_TYPE, float>::value && AscendC::Std::is_same<DST_TYPE, float>::value) {
            Reg::Truncate<DST_TYPE, roundMode>(dstVreg, srcVreg, exPreg);
        } else {
            Reg::Cast<DST_TYPE, SRC_TYPE, castTrait>(dstVreg, srcVreg, exPreg);
        }
        GenStoreL0<DST_TYPE, SRC_TYPE>(dst, dstVreg, stPreg, repeatParams, i);
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
    half scale = 0;
    if (isCounterMode) {
        __ubuf__ uint64_t* maskBuf = nullptr;
        if constexpr (!isSetMask) {
            maskBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 2);
        }
        if constexpr (b64Cast) {
            CastIntrinsicsB64ImplCounterVF<DST_TYPE, SRC_TYPE, roundMode, isSetMask>(
                dst, src, mask[0], maskBuf, repeatTime, repeatParams);
        } else {
            if constexpr (
                AscendC::Std::is_same<SRC_TYPE, int32_t>::value && AscendC::Std::is_same<DST_TYPE, half>::value) {
                scale = Internal::g_deqValue;
            }
            CastIntrinsicsImplCounterVF<DST_TYPE, SRC_TYPE, roundMode, isSetMask>(
                dst, src, mask[0], maskBuf, repeatTime, repeatParams, scale);
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
            if constexpr (
                AscendC::Std::is_same<SRC_TYPE, int32_t>::value && AscendC::Std::is_same<DST_TYPE, half>::value) {
                scale = Internal::g_deqValue;
            }
            CastIntrinsicsImplVF2<DST_TYPE, SRC_TYPE, roundMode>(
                dst, src, maskArrayStruct, repeatTime, repeatParams, scale);
        }
    }
}

template <typename ORI_DST_TYPE, typename ORI_SRC_TYPE, bool isSetMask = true, typename MaskType>
__aicore__ inline void CastImplCommon(
    __ubuf__ ORI_DST_TYPE* oriDst, __ubuf__ ORI_SRC_TYPE* oriSrc, const RoundMode& roundMode, MaskType mask,
    uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
    using SRC_TYPE = typename CastParam::CastTypeTrait<ORI_SRC_TYPE>::RealType;
    using DST_TYPE = typename CastParam::CastTypeTrait<ORI_DST_TYPE>::RealType;
    __ubuf__ SRC_TYPE* src = reinterpret_cast<__ubuf__ SRC_TYPE*>(oriSrc);
    __ubuf__ DST_TYPE* dst = reinterpret_cast<__ubuf__ DST_TYPE*>(oriDst);

    constexpr bool cast_round_all = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<half, float>, Tuple<int64_t, float>, Tuple<int32_t, float>,
        Tuple<int16_t, float>, Tuple<bfloat16_t, float>, Tuple<int32_t, half>, Tuple<int16_t, half>,
        Tuple<int8_t, half>, Tuple<uint8_t, half>, Tuple<int4x2_t, half>, Tuple<bfloat16_t, half>, Tuple<half, int16_t>,
        Tuple<float, int32_t>, Tuple<float, int64_t>, Tuple<int32_t, bfloat16_t>, Tuple<half, bfloat16_t>,
        Tuple<fp4x2_e1m2_t, bfloat16_t>, Tuple<fp4x2_e2m1_t, bfloat16_t>, Tuple<half, int32_t>, Tuple<float, float>>();
    constexpr bool cast_none = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<half, int32_t>, Tuple<float, half>, Tuple<float, bfloat16_t>,
        Tuple<half, int4x2_t>, Tuple<half, uint8_t>, Tuple<uint16_t, uint8_t>, Tuple<uint32_t, uint8_t>,
        Tuple<half, int8_t>, Tuple<int16_t, int8_t>, Tuple<int32_t, int8_t>, Tuple<uint8_t, uint16_t>,
        Tuple<uint32_t, uint16_t>, Tuple<float, int16_t>, Tuple<uint8_t, int16_t>, Tuple<uint32_t, int16_t>,
        Tuple<int32_t, int16_t>, Tuple<uint8_t, uint32_t>, Tuple<uint16_t, uint32_t>, Tuple<int16_t, uint32_t>,
        Tuple<int64_t, int32_t>, Tuple<int16_t, int32_t>, Tuple<uint8_t, int32_t>, Tuple<uint16_t, int32_t>,
        Tuple<int32_t, int64_t>, Tuple<half, hifloat8_t>, Tuple<float, hifloat8_t>, Tuple<float, fp8_e4m3fn_t>,
        Tuple<float, fp8_e5m2_t>, Tuple<bfloat16_t, fp4x2_e1m2_t>, Tuple<bfloat16_t, fp4x2_e2m1_t>,
        Tuple<int4x2_t, int16_t>, Tuple<int16_t, int4x2_t>, Tuple<bfloat16_t, int4x2_t>>();
    constexpr bool using_cast_rint = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<int8_t, half>, Tuple<uint8_t, half>, Tuple<int4x2_t, half>, Tuple<half, float>,
        Tuple<half, int16_t>, Tuple<float, int32_t>>();
    constexpr bool cast_odd = SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<half, float>, Tuple<half, int32_t>>();
    constexpr bool cast_rint =
        SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<fp8_e5m2_t, float>, Tuple<fp8_e4m3fn_t, float>>();
    constexpr bool cast_round =
        SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<hifloat8_t, float>, Tuple<hifloat8_t, half>>();
    constexpr bool cast_hybrid = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<hifloat8_t, float>, Tuple<hifloat8_t, half>, Tuple<half, int32_t>>();
    switch (roundMode) {
        case RoundMode::CAST_RINT:
            if constexpr (cast_round_all || cast_rint) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_RINT, isSetMask>(
                    dst, src, mask, repeatTime, repeatParams);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast rint"); });
            }
            break;
        case RoundMode::CAST_FLOOR:
            if constexpr (cast_round_all) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_FLOOR, isSetMask>(
                    dst, src, mask, repeatTime, repeatParams);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast floor"); });
            }
            break;
        case RoundMode::CAST_CEIL:
            if constexpr (cast_round_all) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_CEIL, isSetMask>(
                    dst, src, mask, repeatTime, repeatParams);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast ceil"); });
            }
            break;
        case RoundMode::CAST_ROUND:
            if constexpr (cast_round_all || cast_round) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_ROUND, isSetMask>(
                    dst, src, mask, repeatTime, repeatParams);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast round"); });
            }
            break;
        case RoundMode::CAST_TRUNC:
            if constexpr (cast_round_all) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_TRUNC, isSetMask>(
                    dst, src, mask, repeatTime, repeatParams);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast trunc"); });
            }
            break;
        case RoundMode::CAST_ODD:
            if constexpr (cast_odd) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_ODD, isSetMask>(
                    dst, src, mask, repeatTime, repeatParams);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast odd"); });
            }
            break;
        case RoundMode::CAST_NONE:
            if constexpr (cast_none) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_NONE, isSetMask>(
                    dst, src, mask, repeatTime, repeatParams);
            } else if constexpr (using_cast_rint) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_RINT, isSetMask>(
                    dst, src, mask, repeatTime, repeatParams);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast none"); });
            }
            break;
        case RoundMode::CAST_HYBRID:
            if constexpr (cast_hybrid) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_HYBRID, isSetMask>(
                    dst, src, mask, repeatTime, repeatParams);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast hybrid"); });
            }
            break;
        default:
            ASCENDC_ASSERT(
                (false), { KERNEL_LOG(KERNEL_ERROR, "illegal input cast mode %d", static_cast<int32_t>(roundMode)); });
            break;
    }
}

// Cast::Level 0 - mask bit mode
template <typename ORI_DST_TYPE, typename ORI_SRC_TYPE, bool isSetMask = true>
__aicore__ inline void CastImpl(
    __ubuf__ ORI_DST_TYPE* oriDst, __ubuf__ ORI_SRC_TYPE* oriSrc, const RoundMode& roundMode, const uint64_t mask[],
    uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
    CastImplCommon<ORI_DST_TYPE, ORI_SRC_TYPE, isSetMask>(oriDst, oriSrc, roundMode, mask, repeatTime, repeatParams);
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
            Reg::LoadAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (Reg::RegTensor<uint32_t>&)srcVreg,
                (__ubuf__ uint32_t*&)src + i * repeatParams.srcRepStride * elePerBlk, repeatParams.srcBlkStride,
                b32Preg);
            Reg::Interleave((Reg::RegTensor<uint32_t>&)srcVreg, tmpVreg, (Reg::RegTensor<uint32_t>&)srcVreg, zeroVreg);
        } else {
            // b64 -> b32
            Reg::LoadAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (Reg::RegTensor<uint32_t>&)srcVreg,
                (__ubuf__ uint32_t*&)src + i * repeatParams.srcRepStride * elePerBlk, repeatParams.srcBlkStride,
                b64Preg);
        }
        Reg::Cast<DST_TYPE, SRC_TYPE, castTrait>(dstVreg, srcVreg, b64Preg);
        if constexpr (sizeof(DST_TYPE) == sizeof(int64_t)) {
            // b32 -> b64
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_STORE>();
            Reg::StoreAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (__ubuf__ uint32_t*&)dst + i * repeatParams.dstRepStride * elePerBlk,
                (Reg::RegTensor<uint32_t>&)dstVreg, repeatParams.dstBlkStride, b64Preg);
        } else {
            // b64 -> b32
            Reg::DeInterleave(
                (Reg::RegTensor<uint32_t>&)dstVreg, tmpVreg, (Reg::RegTensor<uint32_t>&)dstVreg, zeroVreg);
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_STORE>();
            Reg::StoreAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                (__ubuf__ uint32_t*&)dst + i * repeatParams.dstRepStride * elePerBlk,
                (Reg::RegTensor<uint32_t>&)dstVreg, repeatParams.dstBlkStride, b32Preg);
        }
    }
}

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode, bool isSetMask>
__simd_vf__ inline void CastIntrinsicsImplVF1(
    __ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams repeatParams, const half scale)
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
    Reg::RegTensor<float> tmpVreg;
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
            (SupportType<DST_TYPE, int4x2_t, fp4x2_e2m1_t, fp4x2_e1m2_t>() && sizeof(SRC_TYPE) == 2) ||
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
            (SupportType<SRC_TYPE, int4x2_t, fp4x2_e2m1_t, fp4x2_e1m2_t>() && sizeof(DST_TYPE) == 2) ||
            (sizeof(SRC_TYPE) == 1 && sizeof(DST_TYPE) == 4)) {
            Reg::MaskPack(ldPreg, ldPreg);
            if constexpr (SupportType<SRC_TYPE, int4x2_t, fp4x2_e2m1_t, fp4x2_e1m2_t>() && sizeof(DST_TYPE) == 2) {
                Reg::MaskUnPack(stPreg, ldPreg);
                Reg::MaskUnPack(exPreg, stPreg);
                Reg::MaskInterleave<uint16_t>(stPreg, dumpPreg, stPreg, stPreg);
            }
        }
    }
    for (uint16_t i = 0; i < repeatTime; ++i) {
        GenLoadL0<DST_TYPE, SRC_TYPE>(srcVreg, src, ldPreg, repeatParams, i);
        if constexpr (AscendC::Std::is_same<SRC_TYPE, int32_t>::value && AscendC::Std::is_same<DST_TYPE, half>::value) {
            Reg::Cast<float, SRC_TYPE, CastParam::s322floatCastTrait>(tmpVreg, srcVreg, exPreg);
            Reg::Muls(tmpVreg, tmpVreg, DEQ_SHIFT_RIGHT_17_BIT, exPreg);
            Reg::Muls(tmpVreg, tmpVreg, static_cast<float>(scale), exPreg);
            Reg::Muls(tmpVreg, tmpVreg, DEQ_SHIFT_LEFT_17_BIT, exPreg);
            Reg::Cast<DST_TYPE, float, CastParam::f322F16CastTrait>(dstVreg, tmpVreg, exPreg);
        } else if constexpr (
            AscendC::Std::is_same<SRC_TYPE, float>::value && AscendC::Std::is_same<DST_TYPE, float>::value) {
            Reg::Truncate<DST_TYPE, roundMode>(dstVreg, srcVreg, exPreg);
        } else {
            Reg::Cast<DST_TYPE, SRC_TYPE, castTrait>(dstVreg, srcVreg, exPreg);
        }
        GenStoreL0<DST_TYPE, SRC_TYPE>(dst, dstVreg, stPreg, repeatParams, i);
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
    half scale = 0;
    if (isCounterMode) {
        __ubuf__ uint64_t* maskBuf = nullptr;
        if constexpr (!isSetMask) {
            maskBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 2);
        }
        if constexpr (b64Cast) {
            CastIntrinsicsB64ImplCounterVF<DST_TYPE, SRC_TYPE, roundMode, isSetMask>(
                dst, src, mask, maskBuf, repeatTime, repeatParams);
        } else {
            if constexpr (
                AscendC::Std::is_same<SRC_TYPE, int32_t>::value && AscendC::Std::is_same<DST_TYPE, half>::value) {
                scale = Internal::g_deqValue;
            }
            CastIntrinsicsImplCounterVF<DST_TYPE, SRC_TYPE, roundMode, isSetMask>(
                dst, src, mask, maskBuf, repeatTime, repeatParams, scale);
        }
    } else {
        if constexpr (b64Cast) {
            CastIntrinsicsB64ImplVF1<DST_TYPE, SRC_TYPE, roundMode, isSetMask>(
                dst, src, mask, repeatTime, repeatParams);
        } else {
            if constexpr (
                AscendC::Std::is_same<SRC_TYPE, int32_t>::value && AscendC::Std::is_same<DST_TYPE, half>::value) {
                scale = Internal::g_deqValue;
            }
            CastIntrinsicsImplVF1<DST_TYPE, SRC_TYPE, roundMode, isSetMask>(
                dst, src, mask, repeatTime, repeatParams, scale);
        }
    }
}

// Cast::Level 0 - mask count mode
template <typename ORI_DST_TYPE, typename ORI_SRC_TYPE, bool isSetMask = true>
__aicore__ inline void CastImpl(
    __ubuf__ ORI_DST_TYPE* oriDst, __ubuf__ ORI_SRC_TYPE* oriSrc, const RoundMode& roundMode, const uint64_t mask,
    uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
    CastImplCommon<ORI_DST_TYPE, ORI_SRC_TYPE, isSetMask>(oriDst, oriSrc, roundMode, mask, repeatTime, repeatParams);
}

// scale is stored as  1 sign bit, 8 exponent bits and 10 mantissa bits in 1971 hardware
// ===============================================================================
// | 1 signMode bit  | 8 exponent bits | 10bit mantissa bits |       000..0       |
// ===============================================================================
__simd_callee__ inline float GetCastDeqScale(const uint64_t deqScale)
{
    uint32_t tmp = static_cast<uint32_t>(deqScale & 0xffffffff);
    tmp = tmp & 0xffffe000;
    float ret = *(reinterpret_cast<float*>(&tmp));
    return ret;
}

__simd_callee__ inline int16_t GetCastDeqOffset(const uint64_t deqScale)
{
    int16_t ret = static_cast<int16_t>((deqScale >> 37) & 0x1ff);
    return ret;
}

__simd_callee__ inline bool GetCastDeqSignMode(const uint64_t deqScale)
{
    bool ret = static_cast<bool>(deqScale >> 46);
    return ret;
}

template <typename T, Reg::HighLowPart part>
__simd_callee__ inline void CastDeqMulsCal(
    Reg::RegTensor<float>& tmpReg, Reg::RegTensor<T>& srcReg, Reg::MaskReg& maskReg, const float scale)
{
    Reg::Cast<float, T, CastParam::s162f32CastTrait>(tmpReg, srcReg, maskReg);
    Reg::Muls(tmpReg, tmpReg, scale, maskReg);
    Reg::Cast<T, float, CastParam::f322s16CastTrait>(srcReg, tmpReg, maskReg);
    Reg::Pack<uint16_t, uint32_t, part>((Reg::RegTensor<uint16_t>&)srcReg, (Reg::RegTensor<uint32_t>&)srcReg);
}

template <typename T, Reg::HighLowPart part>
__simd_callee__ inline void CastVecDeqMulsCal(
    Reg::RegTensor<int32_t>& tmpReg, Reg::RegTensor<T>& srcReg, Reg::MaskReg& maskReg, Reg::RegTensor<float>& scaleReg)
{
    Reg::Cast<float, T, CastParam::s162f32CastTrait>((Reg::RegTensor<float>&)tmpReg, srcReg, maskReg);
    Reg::Mul((Reg::RegTensor<float>&)tmpReg, (Reg::RegTensor<float>&)tmpReg, scaleReg, maskReg);
    Reg::Cast<T, float, CastParam::f322s16CastTrait>(srcReg, (Reg::RegTensor<float>&)tmpReg, maskReg);
    Reg::Pack<uint16_t, uint32_t, part>((Reg::RegTensor<uint16_t>&)srcReg, (Reg::RegTensor<uint32_t>&)srcReg);
}

template <typename U, typename T, bool halfBlock, bool signMode>
__simd_callee__ inline void CastFromS92B8(Reg::RegTensor<T>& srcReg, Reg::RegTensor<U>& dstReg, Reg::MaskReg& maskReg)
{
    if constexpr (signMode) {
        if constexpr (halfBlock) {
            Reg::Cast<half, T, CastParam::s162HalfTrait>((Reg::RegTensor<half>&)dstReg, srcReg, maskReg);
            Reg::Cast<int8_t, half, CastParam::TrueHalfBlockHalf2S8Trait>(
                (Reg::RegTensor<int8_t>&)dstReg, (Reg::RegTensor<half>&)dstReg, maskReg);
        } else {
            Reg::Cast<half, T, CastParam::s162HalfTrait>((Reg::RegTensor<half>&)dstReg, srcReg, maskReg);
            Reg::Cast<int8_t, half, CastParam::FalseHalfBlockHalf2S8Trait>(
                (Reg::RegTensor<int8_t>&)dstReg, (Reg::RegTensor<half>&)dstReg, maskReg);
        }
    } else {
        if constexpr (halfBlock) {
            Reg::Cast<uint8_t, T, CastParam::TrueHalfBlockCastTrait>((Reg::RegTensor<uint8_t>&)dstReg, srcReg, maskReg);
        } else {
            Reg::Cast<uint8_t, T, CastParam::FalseHalfBlockCastTrait>(
                (Reg::RegTensor<uint8_t>&)dstReg, srcReg, maskReg);
        }
    }
}

// This function is used to generate an index array
// Here is the following python pseudocode:
// BLOCK_SIZE = 32
// HALF_BLOCK_SIZE = 16
// block_id = int(index / BLOCK_SIZE)
// out[index] = block_id * BLOCK_SIZE + (index % HALF_BLOCK_SIZE) * 2 + (index // HALF_BLOCK_SIZE) % 2
__simd_callee__ inline void GenGatherIndex(Reg::RegTensor<int8_t>& dstReg)
{
    Reg::RegTensor<int8_t> arangeReg;
    Reg::RegTensor<int8_t> lowHalfReg;
    Reg::RegTensor<int8_t> dupReg;
    Reg::RegTensor<int8_t> highHalfReg;
    Reg::RegTensor<int8_t> baseOffsetReg;
    Reg::MaskReg mask;
    constexpr int8_t dupConstant = 15;
    constexpr int8_t one = 1;
    constexpr int16_t highHalfRegShiftLeftScalar = 1;
    constexpr int8_t firstIndex = 0;
    constexpr int16_t arangeShiftScalar = 5;
    constexpr int16_t highHalfRegShiftRightScalar = 4;

    mask = Reg::CreateMask<int8_t>();
    Reg::Arange(arangeReg, firstIndex);
    Reg::ShiftRights(baseOffsetReg, arangeReg, arangeShiftScalar, mask);
    Reg::ShiftLefts(baseOffsetReg, baseOffsetReg, arangeShiftScalar, mask);

    Reg::Duplicate(dupReg, dupConstant);
    Reg::And(lowHalfReg, arangeReg, dupReg, mask);
    Reg::ShiftLefts(lowHalfReg, lowHalfReg, highHalfRegShiftLeftScalar, mask);
    Reg::ShiftRights(highHalfReg, arangeReg, highHalfRegShiftRightScalar, mask);
    Reg::Duplicate(dupReg, one);
    Reg::And(highHalfReg, highHalfReg, dupReg, mask);
    Reg::Add(lowHalfReg, lowHalfReg, highHalfReg, mask);
    Reg::Add(dstReg, lowHalfReg, baseOffsetReg, mask);
}

__simd_callee__ inline void GenVecCastDeqParam(
    uint64_t deqScaleAddr, Reg::RegTensor<float>& scaleReg, Reg::RegTensor<int16_t>& offsetReg, Reg::MaskReg& signMask,
    Reg::MaskReg& unSignMask, Reg::RegTensor<int32_t>& tmpReg, Reg::MaskReg& fullMask)
{
    constexpr int16_t offsetShiftLeftScalar = 18;
    constexpr int16_t offsetShiftRightScalar = 55;
    constexpr int16_t signModeShiftRightScalar = 46;
    constexpr int16_t gatherConstant = 16;
    constexpr int16_t zero = 0;
    constexpr int16_t one = 1;
    constexpr int32_t scaleMask = 0xffffe000;
    Reg::RegTensor<int32_t> scaleIndexReg;
    Reg::RegTensor<int16_t> offsetIndexReg;
    Reg::RegTensor<int16_t> signModeReg;
    Reg::LoadAlign(scaleReg, (__ubuf__ float*)deqScaleAddr);
    Reg::Pack((Reg::RegTensor<uint32_t>&)scaleReg, (Reg::RegTensor<uint64_t>&)scaleReg);

    Reg::LoadAlign(offsetReg, (__ubuf__ int16_t*)deqScaleAddr);
    signMask = Reg::CreateMask<uint64_t, Reg::MaskPattern::VL16>();
    Reg::ShiftLefts(
        (Reg::RegTensor<uint64_t>&)offsetReg, (Reg::RegTensor<uint64_t>&)offsetReg, offsetShiftLeftScalar, signMask);
    Reg::ShiftRights(
        (Reg::RegTensor<uint64_t>&)offsetReg, (Reg::RegTensor<uint64_t>&)offsetReg, offsetShiftRightScalar, signMask);
    Reg::Pack((Reg::RegTensor<uint32_t>&)offsetReg, (Reg::RegTensor<uint64_t>&)offsetReg);
    Reg::Pack((Reg::RegTensor<uint16_t>&)offsetReg, (Reg::RegTensor<uint32_t>&)offsetReg);

    Reg::LoadAlign(signModeReg, (__ubuf__ int16_t*)deqScaleAddr);
    Reg::ShiftRights(
        (Reg::RegTensor<uint64_t>&)signModeReg, (Reg::RegTensor<uint64_t>&)signModeReg, signModeShiftRightScalar,
        signMask);
    Reg::Pack((Reg::RegTensor<uint32_t>&)signModeReg, (Reg::RegTensor<uint64_t>&)signModeReg);
    Reg::Pack((Reg::RegTensor<uint16_t>&)signModeReg, (Reg::RegTensor<uint32_t>&)signModeReg);

    // Gen b32 fullMask to deal scaleReg (which datatype is float)
    fullMask = Reg::CreateMask<uint32_t, Reg::MaskPattern::ALL>();
    static constexpr Reg::DivSpecificMode divMode = {
        Reg::MaskMergeMode::ZEROING, false, DivAlgo::PRECISION_1ULP_FTZ_TRUE};
    Reg::Arange(scaleIndexReg, zero);
    Reg::Duplicate(tmpReg, gatherConstant);
    Reg::Div<int32_t, &divMode>(tmpReg, scaleIndexReg, tmpReg, fullMask);
    Reg::Muls(tmpReg, tmpReg, gatherConstant, fullMask);
    Reg::Sub(scaleIndexReg, scaleIndexReg, tmpReg, fullMask);
    Reg::Gather(
        (Reg::RegTensor<uint32_t>&)scaleReg, (Reg::RegTensor<uint32_t>&)scaleReg,
        (Reg::RegTensor<uint32_t>&)scaleIndexReg);
    Reg::Duplicate(tmpReg, scaleMask);
    Reg::And((Reg::RegTensor<int32_t>&)scaleReg, (Reg::RegTensor<int32_t>&)scaleReg, tmpReg, fullMask);

    // Pack lowest 16bit of mrg2ChnIndexReg, because in API "Gather", the sizeof(offsetReg) should be the same as
    // sizeof(mrg2ChnIndexReg)
    fullMask = Reg::CreateMask<uint16_t, Reg::MaskPattern::ALL>();
    Reg::Arange(offsetIndexReg, zero);
    Reg::Duplicate((Reg::RegTensor<int16_t>&)tmpReg, gatherConstant);
    Reg::Div<int16_t, &divMode>(
        (Reg::RegTensor<int16_t>&)tmpReg, offsetIndexReg, (Reg::RegTensor<int16_t>&)tmpReg, fullMask);
    Reg::Muls((Reg::RegTensor<int16_t>&)tmpReg, (Reg::RegTensor<int16_t>&)tmpReg, gatherConstant, fullMask);
    Reg::Sub(offsetIndexReg, offsetIndexReg, (Reg::RegTensor<int16_t>&)tmpReg, fullMask);
    Reg::Gather(
        (Reg::RegTensor<uint16_t>&)offsetReg, (Reg::RegTensor<uint16_t>&)offsetReg,
        (Reg::RegTensor<uint16_t>&)offsetIndexReg);

    Reg::Gather(
        (Reg::RegTensor<uint16_t>&)signModeReg, (Reg::RegTensor<uint16_t>&)signModeReg,
        (Reg::RegTensor<uint16_t>&)offsetIndexReg);
    // Gen mask for elements which cast to sign and cast to unsign
    Reg::Duplicate((Reg::RegTensor<int16_t>&)tmpReg, one);
    Reg::Compare(signMask, signModeReg, (Reg::RegTensor<int16_t>&)tmpReg, fullMask);
    Reg::Duplicate((Reg::RegTensor<int16_t>&)tmpReg, zero);
    Reg::Compare(unSignMask, signModeReg, (Reg::RegTensor<int16_t>&)tmpReg, fullMask);
}

template <bool halfBlock>
__simd_callee__ inline void GenLevel0StoreMask(
    Reg::MaskReg& srcMask, Reg::MaskReg& dstMask, Reg::RegTensor<uint8_t>& mrg2ChnIndexReg,
    Reg::RegTensor<uint8_t>& tmpReg, Reg::MaskReg& fullMask)
{
    constexpr uint8_t cmpScalar = 1;
    if constexpr (halfBlock) {
        constexpr uint16_t scalar = 0x0100;
        Reg::Duplicate((Reg::RegTensor<uint16_t>&)tmpReg, scalar, srcMask);
    } else {
        constexpr uint16_t scalar = 0x0001;
        Reg::Duplicate((Reg::RegTensor<uint16_t>&)tmpReg, scalar, srcMask);
    }
    Reg::Gather(tmpReg, tmpReg, mrg2ChnIndexReg);
    Reg::CompareScalar(dstMask, tmpReg, cmpScalar, fullMask);
}

template <typename U, typename T, bool halfBlock>
__simd_vf__ inline void CastVecDeqImplVF(
    __ubuf__ U* dst, __ubuf__ T* src, const uint32_t calCount, uint64_t deqScaleAddr)
{
    Reg::RegTensor<U> dstReg;
    Reg::RegTensor<T> srcReg0, srcReg1;
    Reg::RegTensor<float> scaleReg;
    Reg::RegTensor<int16_t> offsetReg, signDstReg, vAndReg;
    Reg::RegTensor<uint16_t> unSignDstReg;
    Reg::RegTensor<int32_t> tmpReg;
    Reg::RegTensor<uint8_t> mrg2ChnIndexReg;
    Reg::MaskReg maskReg0, maskReg1, maskReg2, fullMask, signMask, unSignMask;

    constexpr int16_t s9MaxValue = 255;
    constexpr int16_t s9MinValue = -256;
    constexpr int16_t unRollConstant = 2;
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(T);
    constexpr uint32_t halfRepSize = GetVecLen() / unRollConstant / sizeof(T);
    uint16_t repeatTime = CeilDivision(calCount, oneRepSize);
    GenVecCastDeqParam(deqScaleAddr, scaleReg, offsetReg, signMask, unSignMask, tmpReg, fullMask);
    uint32_t sreg = static_cast<uint32_t>(calCount);
    Reg::Duplicate((Reg::RegTensor<int16_t>&)vAndReg, 0x00ff);
    GenGatherIndex((Reg::RegTensor<int8_t>&)mrg2ChnIndexReg);
    for (uint16_t i = 0; i < repeatTime; ++i) {
        maskReg0 = Reg::UpdateMask<T>(sreg);
        Reg::MaskInterleave<T>(maskReg1, maskReg2, maskReg0, fullMask);
        Reg::LoadAlign<T, Reg::LoadDist::DIST_US_B16>(srcReg0, src + i * unRollConstant * halfRepSize);
        CastVecDeqMulsCal<T, Reg::HighLowPart::LOWEST>(tmpReg, srcReg0, maskReg1, scaleReg);
        Reg::LoadAlign<T, Reg::LoadDist::DIST_US_B16>(srcReg1, src + (i * unRollConstant + 1) * halfRepSize);
        CastVecDeqMulsCal<T, Reg::HighLowPart::HIGHEST>(tmpReg, srcReg1, maskReg2, scaleReg);
        maskReg1 = Reg::CreateMask<T, Reg::MaskPattern::H>();
        Reg::Select(srcReg0, srcReg0, srcReg1, maskReg1);
        Reg::Maxs(srcReg0, srcReg0, s9MinValue, maskReg0);
        Reg::Mins(srcReg0, srcReg0, s9MaxValue, maskReg0);
        Reg::Add(srcReg0, srcReg0, offsetReg, maskReg0);
        CastFromS92B8<int8_t, T, halfBlock, true>(srcReg0, (Reg::RegTensor<int8_t>&)signDstReg, signMask);
        CastFromS92B8<uint8_t, T, halfBlock, false>(srcReg0, (Reg::RegTensor<uint8_t>&)unSignDstReg, unSignMask);
        Reg::Select((Reg::RegTensor<int16_t>&)dstReg, signDstReg, (Reg::RegTensor<int16_t>&)unSignDstReg, signMask);
        Gather(dstReg, dstReg, mrg2ChnIndexReg);
        Reg::StoreAlign((__ubuf__ T*)dst + i * oneRepSize, (Reg::RegTensor<T>&)dstReg, fullMask);
    }
}

template <typename U, typename T, bool halfBlock, bool signMode>
__simd_vf__ inline void CastDeqImplVF(__ubuf__ U* dst, __ubuf__ T* src, const uint32_t calCount, uint64_t deqScale)
{
    Reg::RegTensor<T> srcReg0, srcReg1;
    Reg::RegTensor<float> tmpReg;
    Reg::RegTensor<U> dstReg;
    Reg::RegTensor<uint8_t> mrg2ChnIndexReg;
    Reg::MaskReg maskReg0, maskReg1, maskReg2, fullMask;

    constexpr int16_t s9MaxValue = 255;
    constexpr int16_t s9MinValue = -256;
    constexpr int16_t unRollConstant = 2;
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(T);
    constexpr uint32_t halfRepSize = GetVecLen() / unRollConstant / sizeof(T);
    uint16_t repeatTime = CeilDivision(calCount, oneRepSize);
    float scale = GetCastDeqScale(deqScale);
    uint16_t offset = GetCastDeqOffset(deqScale);
    uint32_t sreg = static_cast<uint32_t>(calCount);
    fullMask = Reg::CreateMask<T, Reg::MaskPattern::ALL>();
    GenGatherIndex((Reg::RegTensor<int8_t>&)mrg2ChnIndexReg);
    for (uint16_t i = 0; i < repeatTime; ++i) {
        maskReg0 = Reg::UpdateMask<T>(sreg);
        Reg::MaskInterleave<T>(maskReg1, maskReg2, maskReg0, fullMask);
        Reg::LoadAlign<T, Reg::LoadDist::DIST_US_B16>(srcReg0, src + i * unRollConstant * halfRepSize);
        CastDeqMulsCal<T, Reg::HighLowPart::LOWEST>(tmpReg, srcReg0, maskReg1, scale);
        Reg::LoadAlign<T, Reg::LoadDist::DIST_US_B16>(srcReg1, src + (i * unRollConstant + 1) * halfRepSize);
        CastDeqMulsCal<T, Reg::HighLowPart::HIGHEST>(tmpReg, srcReg1, maskReg2, scale);
        Reg::Or(srcReg0, srcReg0, srcReg1, maskReg0);
        Reg::Maxs(srcReg0, srcReg0, s9MinValue, maskReg0);
        Reg::Mins(srcReg0, srcReg0, s9MaxValue, maskReg0);
        Reg::Adds(srcReg0, srcReg0, offset, maskReg0);
        CastFromS92B8<U, T, halfBlock, signMode>(srcReg0, dstReg, maskReg0);
        Gather(dstReg, dstReg, mrg2ChnIndexReg);
        Reg::StoreAlign((__ubuf__ T*)dst + i * oneRepSize, (Reg::RegTensor<T>&)dstReg, fullMask);
    }
}

template <typename U, typename T>
__simd_vf__ inline void CastDeqS322f16ImplVF(
    __ubuf__ U* dst, __ubuf__ T* src, const uint32_t calCount, const half deqScale)
{
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<float> tmpVreg;
    Reg::RegTensor<U> dstReg;
    Reg::MaskReg maskReg;
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(T);
    uint16_t repeatTime = CeilDivision(calCount, oneRepSize);
    uint32_t sreg = calCount;
    for (uint16_t i = 0; i < repeatTime; ++i) {
        maskReg = Reg::UpdateMask<T>(sreg);
        Reg::LoadAlign(srcReg, src + i * oneRepSize);
        Reg::Cast<float, T, CastParam::s322F32CastTrait>(tmpVreg, srcReg, maskReg);
        Reg::Muls(tmpVreg, tmpVreg, DEQ_SHIFT_RIGHT_17_BIT, maskReg);
        Reg::Muls(tmpVreg, tmpVreg, static_cast<float>(deqScale), maskReg);
        Reg::Muls(tmpVreg, tmpVreg, DEQ_SHIFT_LEFT_17_BIT, maskReg);
        Reg::Cast<U, float, CastParam::f322F16CastTrait>(dstReg, tmpVreg, maskReg);
        Reg::StoreAlign<U, Reg::StoreDist::DIST_PACK_B32>(dst + i * oneRepSize, dstReg, maskReg);
    }
}

template <typename U, typename T, bool isVecDeq, bool halfBlock>
__aicore__ inline void CastDeqImpl(__ubuf__ U* dst, __ubuf__ T* src, const uint32_t calCount)
{
    static_assert(
        SupportType<Tuple<T, U>, Tuple<int16_t, int8_t>, Tuple<int16_t, uint8_t>, Tuple<int32_t, half>>(),
        "Failed to check dtype in CastDeqImpl, current api support dtype combination is src: int16_t dst: "
        "int8_t/uint8_t"
        ", src:int32_t dst:half.");
    if constexpr (IsSameType<T, int32_t>::value) {
        half scale = Internal::g_deqValue;
        CastDeqS322f16ImplVF<U, T>(dst, src, calCount, scale);
    } else {
        uint64_t deqScale = Internal::g_deqScale;
        if constexpr (isVecDeq) {
            CastVecDeqImplVF<U, T, halfBlock>(dst, src, calCount, deqScale);
        } else {
            bool signMode = GetCastDeqSignMode(deqScale);
            if (signMode) {
                CastDeqImplVF<U, T, halfBlock, true>(dst, src, calCount, deqScale);
            } else {
                CastDeqImplVF<U, T, halfBlock, false>(dst, src, calCount, deqScale);
            }
        }
    }
}

template <typename U, typename T, bool isCounterMode, bool isBitMap, bool isSetMask, bool halfBlock>
__simd_vf__ inline void CastVecDeqLevel0ImplVF(
    __ubuf__ U* dst, __ubuf__ T* src, const int32_t mask, __ubuf__ uint64_t* tempBuf, uint8_t repeatTime,
    const UnaryRepeatParams repeatParams, uint64_t deqScaleAddr)
{
    Reg::RegTensor<U> dstReg;
    Reg::RegTensor<T> srcReg0, srcReg1;
    Reg::RegTensor<float> scaleReg;
    Reg::RegTensor<int16_t> offsetReg, signDstReg;
    Reg::RegTensor<uint8_t> mrg2ChnIndexReg;
    Reg::RegTensor<uint16_t> unSignDstReg;
    Reg::RegTensor<int32_t> tmpReg;
    Reg::MaskReg maskReg0, maskReg1, maskReg2, fullMask, signMask, unSignMask, dstMask;
    constexpr int16_t unRollConstant = 2;
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(T);
    constexpr uint32_t halfRepSize = GetVecLen() / unRollConstant / sizeof(T);
    constexpr uint32_t blockElm = GetDataBlockSizeInBytes() / sizeof(T);
    constexpr uint32_t dstBlockElm = GetDataBlockSizeInBytes() / sizeof(U);
    constexpr int16_t s9MaxValue = 255;
    constexpr int16_t s9MinValue = -256;
    constexpr int16_t zero = 0;
    constexpr int16_t halfRepBlkSize = 4;
    uint32_t halfRepStride = halfRepBlkSize * blockElm * repeatParams.srcBlkStride;
    uint32_t sreg = static_cast<uint32_t>(mask);
    GenVecCastDeqParam(deqScaleAddr, scaleReg, offsetReg, signMask, unSignMask, tmpReg, fullMask);
    GenGatherIndex((Reg::RegTensor<int8_t>&)mrg2ChnIndexReg);
    if constexpr (isCounterMode) {
        if constexpr (!isSetMask) {
            maskReg0 = Reg::MoveMask<uint16_t>();
            Reg::StoreAlign<uint64_t, Reg::MaskDist::DIST_PACK>(tempBuf, maskReg0);
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::SCALAR_LOAD>();
            sreg = static_cast<uint32_t>(tempBuf[0]);
        }
        repeatTime = CeilDivision(sreg, oneRepSize);
    } else {
        if constexpr (isBitMap) {
            maskReg0 = Reg::MoveMask<T>();
        } else {
            if constexpr (isSetMask) {
                uint32_t sreg = static_cast<uint32_t>(mask);
                maskReg0 = Reg::UpdateMask<T>(sreg);
            } else {
                maskReg0 = Reg::MoveMask<T>();
            }
        }
    }
    fullMask = Reg::CreateMask<U, Reg::MaskPattern::ALL>();
    for (uint16_t i = 0; i < repeatTime; ++i) {
        if constexpr (isCounterMode) {
            maskReg0 = Reg::UpdateMask<T>(sreg);
        }
        Reg::MaskInterleave<T>(maskReg1, maskReg2, maskReg0, fullMask);
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            srcReg0, src + i * blockElm * repeatParams.srcRepStride, static_cast<uint32_t>(repeatParams.srcBlkStride),
            maskReg1);
        Reg::UnPack((Reg::RegTensor<uint32_t>&)srcReg0, (Reg::RegTensor<uint16_t>&)srcReg0);
        CastVecDeqMulsCal<T, Reg::HighLowPart::LOWEST>(tmpReg, srcReg0, maskReg1, scaleReg);
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            srcReg1, src + i * blockElm * repeatParams.srcRepStride + halfRepStride,
            static_cast<uint32_t>(repeatParams.srcBlkStride), maskReg2);
        Reg::UnPack((Reg::RegTensor<uint32_t>&)srcReg1, (Reg::RegTensor<uint16_t>&)srcReg1);
        CastVecDeqMulsCal<T, Reg::HighLowPart::HIGHEST>(tmpReg, srcReg1, maskReg2, scaleReg);
        maskReg1 = Reg::CreateMask<T, Reg::MaskPattern::H>();
        Reg::Select(srcReg0, srcReg0, srcReg1, maskReg1);
        Reg::Maxs(srcReg0, srcReg0, s9MinValue, maskReg0);
        Reg::Mins(srcReg0, srcReg0, s9MaxValue, maskReg0);
        Reg::Add(srcReg0, srcReg0, offsetReg, maskReg0);
        CastFromS92B8<int8_t, T, halfBlock, true>(srcReg0, (Reg::RegTensor<int8_t>&)signDstReg, signMask);
        CastFromS92B8<uint8_t, T, halfBlock, false>(srcReg0, (Reg::RegTensor<uint8_t>&)unSignDstReg, unSignMask);
        Reg::Select((Reg::RegTensor<int16_t>&)dstReg, signDstReg, (Reg::RegTensor<int16_t>&)unSignDstReg, signMask);
        Reg::Gather(dstReg, dstReg, mrg2ChnIndexReg);
        GenLevel0StoreMask<halfBlock>(maskReg0, dstMask, mrg2ChnIndexReg, (Reg::RegTensor<uint8_t>&)tmpReg, fullMask);
        Reg::StoreAlign<U, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            dst + i * dstBlockElm * repeatParams.dstRepStride, dstReg, static_cast<uint32_t>(repeatParams.dstBlkStride),
            dstMask);
    }
}

template <typename U, typename T, bool isCounterMode, bool isBitMap, bool isSetMask, bool halfBlock, bool signMode>
__simd_vf__ inline void CastDeqLevel0ImplVF(
    __ubuf__ U* dst, __ubuf__ T* src, const int32_t mask, __ubuf__ uint64_t* tempBuf, uint8_t repeatTime,
    const UnaryRepeatParams repeatParams, uint64_t deqScale)
{
    Reg::RegTensor<T> srcReg0, srcReg1;
    Reg::RegTensor<float> tmpReg;
    Reg::RegTensor<U> dstReg;
    Reg::RegTensor<uint8_t> mrg2ChnIndexReg;
    Reg::MaskReg maskReg0, maskReg1, maskReg2, fullMask, dstMask;
    constexpr int16_t unRollConstant = 2;
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(T);
    constexpr uint32_t halfRepSize = GetVecLen() / unRollConstant / sizeof(T);
    constexpr uint32_t blockElm = GetDataBlockSizeInBytes() / sizeof(T);
    constexpr uint32_t dstBlockElm = GetDataBlockSizeInBytes() / sizeof(U);
    float scale = GetCastDeqScale(deqScale);
    uint16_t offset = GetCastDeqOffset(deqScale);
    constexpr int16_t s9MaxValue = 255;
    constexpr int16_t s9MinValue = -256;
    constexpr int16_t halfRepBlkSize = 4;
    constexpr int8_t zero = 0;
    uint32_t halfRepStride = halfRepBlkSize * blockElm * repeatParams.srcBlkStride;
    uint32_t sreg = static_cast<uint32_t>(mask);
    if constexpr (isCounterMode) {
        if constexpr (!isSetMask) {
            maskReg0 = Reg::MoveMask<uint16_t>();
            Reg::StoreAlign<uint64_t, Reg::MaskDist::DIST_PACK>(tempBuf, maskReg0);
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::SCALAR_LOAD>();
            sreg = static_cast<uint32_t>(tempBuf[0]);
        }
        repeatTime = CeilDivision(sreg, oneRepSize);
    } else {
        if constexpr (isBitMap) {
            maskReg0 = Reg::MoveMask<T>();
        } else {
            if constexpr (isSetMask) {
                uint32_t sreg = static_cast<uint32_t>(mask);
                maskReg0 = Reg::UpdateMask<T>(sreg);
            } else {
                maskReg0 = Reg::MoveMask<T>();
            }
        }
    }
    fullMask = Reg::CreateMask<U, Reg::MaskPattern::ALL>();
    GenGatherIndex((Reg::RegTensor<int8_t>&)mrg2ChnIndexReg);
    for (uint16_t i = 0; i < repeatTime; ++i) {
        if constexpr (isCounterMode) {
            maskReg0 = Reg::UpdateMask<T>(sreg);
        }
        Reg::MaskInterleave<T>(maskReg1, maskReg2, maskReg0, fullMask);
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            srcReg0, src + i * blockElm * repeatParams.srcRepStride, static_cast<uint32_t>(repeatParams.srcBlkStride),
            maskReg1);
        Reg::UnPack((Reg::RegTensor<uint32_t>&)srcReg0, (Reg::RegTensor<uint16_t>&)srcReg0);
        CastDeqMulsCal<T, Reg::HighLowPart::LOWEST>(tmpReg, srcReg0, maskReg1, scale);
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            srcReg1, src + i * blockElm * repeatParams.srcRepStride + halfRepStride,
            static_cast<uint32_t>(repeatParams.srcBlkStride), maskReg2);
        Reg::UnPack((Reg::RegTensor<uint32_t>&)srcReg1, (Reg::RegTensor<uint16_t>&)srcReg1);
        CastDeqMulsCal<T, Reg::HighLowPart::HIGHEST>(tmpReg, srcReg1, maskReg2, scale);
        Reg::Or(srcReg0, srcReg0, srcReg1, maskReg0);
        Reg::Maxs(srcReg0, srcReg0, s9MinValue, maskReg0);
        Reg::Mins(srcReg0, srcReg0, s9MaxValue, maskReg0);
        Reg::Adds(srcReg0, srcReg0, offset, maskReg0);
        CastFromS92B8<U, T, halfBlock, signMode>(srcReg0, dstReg, maskReg0);
        Reg::Gather((Reg::RegTensor<uint8_t>&)dstReg, (Reg::RegTensor<uint8_t>&)dstReg, mrg2ChnIndexReg);
        GenLevel0StoreMask<halfBlock>(maskReg0, dstMask, mrg2ChnIndexReg, (Reg::RegTensor<uint8_t>&)tmpReg, fullMask);
        Reg::StoreAlign<U, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            dst + i * dstBlockElm * repeatParams.dstRepStride, dstReg, static_cast<uint32_t>(repeatParams.dstBlkStride),
            dstMask);
    }
}

template <typename U, typename T, bool isCounterMode, bool isBitMap, bool isSetMask>
__simd_vf__ inline void CastDeqS322f16Level0ImplVF(
    __ubuf__ U* dst, __ubuf__ T* src, const int32_t mask, __ubuf__ uint64_t* tempBuf, uint8_t repeatTime,
    const UnaryRepeatParams repeatParams, const half deqScale)
{
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<float> tmpVreg;
    Reg::RegTensor<U> dstReg;
    Reg::MaskReg maskReg;
    Reg::MaskReg dstMask;
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(T);
    constexpr uint32_t srcBlockElm = GetDataBlockSizeInBytes() / sizeof(T);
    constexpr uint32_t dstBlockElm = GetDataBlockSizeInBytes() / sizeof(U);
    uint32_t sreg = mask;
    if constexpr (isCounterMode) {
        if constexpr (!isSetMask) {
            maskReg = Reg::MoveMask<uint16_t>();
            Reg::StoreAlign<uint64_t, Reg::MaskDist::DIST_PACK>(tempBuf, maskReg);
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::SCALAR_LOAD>();
            sreg = static_cast<uint32_t>(tempBuf[0]);
        }
        repeatTime = CeilDivision(sreg, oneRepSize);
    } else {
        if constexpr (isBitMap) {
            maskReg = Reg::MoveMask<T>();
        } else {
            if constexpr (isSetMask) {
                uint32_t sreg = static_cast<uint32_t>(mask);
                maskReg = Reg::UpdateMask<T>(sreg);
            } else {
                maskReg = Reg::MoveMask<T>();
            }
        }
        Reg::MaskPack(dstMask, maskReg);
    }
    for (uint16_t i = 0; i < repeatTime; ++i) {
        if constexpr (isCounterMode) {
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::MaskPack(dstMask, maskReg);
        }
        Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            srcReg, src + i * srcBlockElm * repeatParams.srcRepStride, static_cast<uint32_t>(repeatParams.srcBlkStride),
            maskReg);
        Reg::Cast<float, T, CastParam::s322F32CastTrait>(tmpVreg, srcReg, maskReg);
        Reg::Muls(tmpVreg, tmpVreg, DEQ_SHIFT_RIGHT_17_BIT, maskReg);
        Reg::Muls(tmpVreg, tmpVreg, static_cast<float>(deqScale), maskReg);
        Reg::Muls(tmpVreg, tmpVreg, DEQ_SHIFT_LEFT_17_BIT, maskReg);
        Reg::Cast<U, float, CastParam::f322F16CastTrait>(dstReg, tmpVreg, maskReg);
        Reg::Pack((Reg::RegTensor<uint16_t>&)dstReg, (Reg::RegTensor<uint32_t>&)dstReg);
        Reg::StoreAlign<U, Reg::DataCopyMode::DATA_BLOCK_COPY>(
            dst + i * dstBlockElm * repeatParams.dstRepStride, dstReg, static_cast<uint32_t>(repeatParams.dstBlkStride),
            dstMask);
    }
}

template <typename U, typename T, bool isSetMask = true, bool isVecDeq, bool halfBlock>
__aicore__ inline void CastDeqImpl(
    __ubuf__ U* dst, __ubuf__ T* src, const uint64_t mask[], uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<Tuple<T, U>, Tuple<int16_t, int8_t>, Tuple<int16_t, uint8_t>, Tuple<int32_t, half>>(),
        "Failed to check dtype in CastDeqImpl, current api support dtype combination is src: int16_t dst: "
        "int8_t/uint8_t"
        ", src:int32_t dst:half.");
    bool isCounterMode = Internal::IsCounterMode();
    __ubuf__ uint64_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 4);
    if constexpr (isSetMask) {
        SetVectorMask<T>(mask[1], mask[0]);
    }
    if constexpr (IsSameType<T, int32_t>::value) {
        half scale = Internal::g_deqValue;
        if (isCounterMode) {
            CastDeqS322f16Level0ImplVF<U, T, true, true, isSetMask>(
                dst, src, mask[0], tempBuf, repeatTime, repeatParams, scale);
        } else {
            CastDeqS322f16Level0ImplVF<U, T, false, true, isSetMask>(
                dst, src, mask[0], tempBuf, repeatTime, repeatParams, scale);
        }
    } else {
        uint64_t deqScale = Internal::g_deqScale;
        bool signMode = GetCastDeqSignMode(deqScale);
        if (isCounterMode) {
            if constexpr (isVecDeq) {
                CastVecDeqLevel0ImplVF<U, T, true, true, isSetMask, halfBlock>(
                    dst, src, mask[0], tempBuf, repeatTime, repeatParams, deqScale);
            } else {
                if (signMode) {
                    CastDeqLevel0ImplVF<U, T, true, true, isSetMask, halfBlock, true>(
                        dst, src, mask[0], tempBuf, repeatTime, repeatParams, deqScale);
                } else {
                    CastDeqLevel0ImplVF<U, T, true, true, isSetMask, halfBlock, false>(
                        dst, src, mask[0], tempBuf, repeatTime, repeatParams, deqScale);
                }
            }
        } else {
            if constexpr (isVecDeq) {
                CastVecDeqLevel0ImplVF<U, T, false, true, isSetMask, halfBlock>(
                    dst, src, 0, tempBuf, repeatTime, repeatParams, deqScale);
            } else {
                if (signMode) {
                    CastDeqLevel0ImplVF<U, T, false, true, isSetMask, halfBlock, true>(
                        dst, src, 0, tempBuf, repeatTime, repeatParams, deqScale);
                } else {
                    CastDeqLevel0ImplVF<U, T, false, true, isSetMask, halfBlock, false>(
                        dst, src, 0, tempBuf, repeatTime, repeatParams, deqScale);
                }
            }
        }
    }
    AscendCUtils::FreeTemporaryBuffer(tempBuf);
}

template <typename U, typename T, bool isSetMask = true, bool isVecDeq, bool halfBlock>
__aicore__ inline void CastDeqImpl(
    __ubuf__ U* dst, __ubuf__ T* src, const int32_t mask, uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
    static_assert(
        SupportType<Tuple<T, U>, Tuple<int16_t, int8_t>, Tuple<int16_t, uint8_t>, Tuple<int32_t, half>>(),
        "Failed to check dtype in CastDeqImpl, current api support dtype combination is src: int16_t dst: "
        "int8_t/uint8_t"
        ", src:int32_t dst:half.");
    bool isCounterMode = Internal::IsCounterMode();
    __ubuf__ uint64_t* tempBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 4);
    if constexpr (IsSameType<T, int32_t>::value) {
        half scale = Internal::g_deqValue;
        if (isCounterMode) {
            CastDeqS322f16Level0ImplVF<U, T, true, false, isSetMask>(
                dst, src, mask, tempBuf, repeatTime, repeatParams, scale);
        } else {
            CastDeqS322f16Level0ImplVF<U, T, false, false, isSetMask>(
                dst, src, mask, tempBuf, repeatTime, repeatParams, scale);
        }
    } else {
        uint64_t deqScale = Internal::g_deqScale;
        bool signMode = GetCastDeqSignMode(deqScale);
        if (isCounterMode) {
            if constexpr (isVecDeq) {
                CastVecDeqLevel0ImplVF<U, T, true, false, isSetMask, halfBlock>(
                    dst, src, mask, tempBuf, repeatTime, repeatParams, deqScale);
            } else {
                if (signMode) {
                    CastDeqLevel0ImplVF<U, T, true, false, isSetMask, halfBlock, true>(
                        dst, src, mask, tempBuf, repeatTime, repeatParams, deqScale);
                } else {
                    CastDeqLevel0ImplVF<U, T, true, false, isSetMask, halfBlock, false>(
                        dst, src, mask, tempBuf, repeatTime, repeatParams, deqScale);
                }
            }
        } else {
            if constexpr (isVecDeq) {
                CastVecDeqLevel0ImplVF<U, T, false, false, isSetMask, halfBlock>(
                    dst, src, mask, tempBuf, repeatTime, repeatParams, deqScale);
            } else {
                if (signMode) {
                    CastDeqLevel0ImplVF<U, T, false, false, isSetMask, halfBlock, true>(
                        dst, src, mask, tempBuf, repeatTime, repeatParams, deqScale);
                } else {
                    CastDeqLevel0ImplVF<U, T, false, false, isSetMask, halfBlock, false>(
                        dst, src, mask, tempBuf, repeatTime, repeatParams, deqScale);
                }
            }
        }
    }
    AscendCUtils::FreeTemporaryBuffer(tempBuf);
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
    Internal::g_deqValue = config;
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
    constexpr uint32_t sregLower = static_cast<uint32_t>(GetVecLen() / sizeof(T));
    const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, sregLower));
    uint32_t sreg = static_cast<uint32_t>(calCount);
    Reg::RegTensor<T> vDstReg;
    Reg::RegTensor<T> vSrcReg;
    Reg::MaskReg mask;
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<T>(sreg);
        Reg::LoadAlign(vSrcReg, src + i * sregLower);
        Reg::Truncate<T, roundMode>(vDstReg, vSrcReg, mask);
        Reg::StoreAlign(dst + i * sregLower, vDstReg, mask);
    }
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_VCONV_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_VCONV_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_VCONV_IMPL_H__
#endif
