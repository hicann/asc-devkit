/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_v516/kernel_operator_vec_vconv_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_VCONV_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_VEC_VCONV_IMPL_H
#define ASCENDC_MODULE_OPERATOR_VEC_VCONV_IMPL_H
#include "../kernel_utils.h"
#include "kernel_operator_vec_template_impl.h"
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
__aicore__ inline void CastIntrinsicsB64ImplVF(__ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint32_t calCount)
{
    constexpr uint16_t oneRepSize = 2 * GetVecLen() / sizeof(int64_t);
    uint16_t repeatTime = CeilDivision(calCount, oneRepSize);
    uint32_t sreg = static_cast<uint32_t>(calCount);
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, roundMode};
    if constexpr (Std::is_same_v<SRC_TYPE, complex64> && Std::is_same_v<DST_TYPE, complex32>) {
        __VEC_SCOPE__ // complex 64 -> 32
        {
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
        }
    } else if constexpr (Std::is_same_v<SRC_TYPE, complex64> && Std::is_same_v<DST_TYPE, complex64>) {
        __VEC_SCOPE__ // complex64 -> 64
        {
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
        }
    } else if constexpr (Std::is_same_v<SRC_TYPE, complex32> && Std::is_same_v<DST_TYPE, complex64>) {
        __VEC_SCOPE__ // complex32 -> 64
        {
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
        }
    } else if constexpr (sizeof(DST_TYPE) == sizeof(int64_t)) {
        __VEC_SCOPE__ // B32 -> B64
        {
            Reg::MaskReg preg;
            Reg::RegTensor<SRC_TYPE> srcVreg;
            Reg::RegTensor<DST_TYPE, Reg::RegTraitNumTwo> dstVreg;
            for (uint16_t i = 0; i < repeatTime; ++i) {
                preg = Reg::UpdateMask<int64_t, Reg::RegTraitNumTwo>(sreg);
                Reg::LoadAlign(srcVreg, src + i * oneRepSize);
                Reg::Cast<DST_TYPE, SRC_TYPE, castTrait>(dstVreg, srcVreg, preg);
                Reg::StoreAlign(dst + i * oneRepSize, dstVreg, preg);
            }
        }
    } else {
        __VEC_SCOPE__ // B64 -> B32
        {
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
}

template <typename DST_TYPE, typename SRC_TYPE>
__aicore__ inline void GenLoadL2(Reg::RegTensor<SRC_TYPE>& srcVreg, __ubuf__ SRC_TYPE* srcAddr, Reg::MaskReg& preg)
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
__aicore__ inline void GenStoreL2(__ubuf__ DST_TYPE* dstAddr, Reg::RegTensor<DST_TYPE>& dstVreg, Reg::MaskReg& preg)
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
__aicore__ inline void CastIntrinsicsImplVF(__ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint32_t calCount)
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
        if constexpr (Std::is_same_v<SRC_TYPE, int32_t> && Std::is_same_v<DST_TYPE, half>) {
            Reg::Cast<float, SRC_TYPE, castTrait>((Reg::RegTensor<float>&)dstVreg, srcVreg, preg);
            Reg::Cast<DST_TYPE, float, castTrait>(dstVreg, (Reg::RegTensor<float>&)dstVreg, preg);
        } else if constexpr (Std::is_same_v<SRC_TYPE, float> && Std::is_same_v<DST_TYPE, float>) {
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
    if constexpr (b64Cast) {
        VF_CALL<CastIntrinsicsB64ImplVF<DST_TYPE, SRC_TYPE, roundMode>>(dst, src, calCount);
    } else {
        VF_CALL<CastIntrinsicsImplVF<DST_TYPE, SRC_TYPE, roundMode>>(dst, src, calCount);
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
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<float, half>, Tuple<float, bfloat16_t>, Tuple<half, int4x2_t>,
        Tuple<half, uint8_t>, Tuple<uint16_t, uint8_t>, Tuple<uint32_t, uint8_t>, Tuple<half, int8_t>,
        Tuple<int16_t, int8_t>, Tuple<int32_t, int8_t>, Tuple<uint8_t, uint16_t>, Tuple<uint32_t, uint16_t>,
        Tuple<float, int16_t>, Tuple<uint8_t, int16_t>, Tuple<uint32_t, int16_t>, Tuple<int32_t, int16_t>,
        Tuple<uint8_t, uint32_t>, Tuple<uint16_t, uint32_t>, Tuple<int16_t, uint32_t>, Tuple<int64_t, int32_t>,
        Tuple<int16_t, int32_t>, Tuple<uint8_t, int32_t>, Tuple<uint16_t, int32_t>, Tuple<int32_t, int64_t>,
        Tuple<half, hifloat8_t>, Tuple<float, hifloat8_t>, Tuple<float, fp8_e4m3fn_t>, Tuple<float, fp8_e5m2_t>,
        Tuple<bfloat16_t, fp4x2_e1m2_t>, Tuple<bfloat16_t, fp4x2_e2m1_t>, Tuple<int4x2_t, int16_t>,
        Tuple<int16_t, int4x2_t>, Tuple<bfloat16_t, int4x2_t>, Tuple<complex64, complex32>>();
    constexpr bool using_cast_rint = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<int8_t, half>, Tuple<uint8_t, half>, Tuple<int4x2_t, half>, Tuple<half, float>,
        Tuple<half, int16_t>, Tuple<float, int32_t>, Tuple<complex32, complex64>>();
    constexpr bool cast_odd = SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<half, float>, Tuple<complex32, complex64>>();
    constexpr bool cast_rint =
        SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<fp8_e5m2_t, float>, Tuple<fp8_e4m3fn_t, float>>();
    constexpr bool cast_round =
        SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<hifloat8_t, float>, Tuple<hifloat8_t, half>>();
    constexpr bool cast_hybrid =
        SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<hifloat8_t, float>, Tuple<hifloat8_t, half>>();
    switch (roundMode) {
        case RoundMode::CAST_RINT:
            if constexpr (cast_round_all || cast_rint) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_RINT>(dst, src, calCount);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast rint"); });
            }
            break;
        case RoundMode::CAST_FLOOR:
            if constexpr (cast_round_all) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_FLOOR>(dst, src, calCount);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast floor"); });
            }
            break;
        case RoundMode::CAST_CEIL:
            if constexpr (cast_round_all) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_CEIL>(dst, src, calCount);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast ceil"); });
            }
            break;
        case RoundMode::CAST_ROUND:
            if constexpr (cast_round_all || cast_round) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_ROUND>(dst, src, calCount);
            } else {
                ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "illegal type for cast round"); });
            }
            break;
        case RoundMode::CAST_TRUNC:
            if constexpr (cast_round_all) {
                CastIntrinsicsImpl<DST_TYPE, SRC_TYPE, RoundMode::CAST_TRUNC>(dst, src, calCount);
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
__aicore__ inline void CastIntrinsicsB64ImplVF2(
    __ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint64_t mask[], uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, roundMode};
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
            Reg::LoadAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                (Reg::RegTensor<uint32_t>&)srcVreg, (__ubuf__ uint32_t*&)src,
                static_cast<uint32_t>(repeatParams.srcBlkStride), static_cast<uint32_t>(repeatParams.srcRepStride),
                b32Preg);
            Reg::Interleave((Reg::RegTensor<uint32_t>&)srcVreg, tmpVreg, (Reg::RegTensor<uint32_t>&)srcVreg, zeroVreg);
        } else {
            // b64->b32
            Reg::LoadAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                (Reg::RegTensor<uint32_t>&)srcVreg, (__ubuf__ uint32_t*&)src,
                static_cast<uint32_t>(repeatParams.srcBlkStride), static_cast<uint32_t>(repeatParams.srcRepStride),
                b64Preg);
        }
        Reg::Cast<DST_TYPE, SRC_TYPE, castTrait>(dstVreg, srcVreg, b64Preg);
        if constexpr (sizeof(DST_TYPE) == sizeof(int64_t)) {
            // b32->b64
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_STORE>();
            Reg::StoreAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                (__ubuf__ uint32_t*&)dst, (Reg::RegTensor<uint32_t>&)dstVreg,
                static_cast<uint32_t>(repeatParams.dstBlkStride), static_cast<uint32_t>(repeatParams.dstRepStride),
                b64Preg);
        } else {
            // b64->b32
            Reg::DeInterleave(
                (Reg::RegTensor<uint32_t>&)dstVreg, tmpVreg, (Reg::RegTensor<uint32_t>&)dstVreg, zeroVreg);
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_STORE>();
            Reg::StoreAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                (__ubuf__ uint32_t*&)dst, (Reg::RegTensor<uint32_t>&)dstVreg,
                static_cast<uint32_t>(repeatParams.dstBlkStride), static_cast<uint32_t>(repeatParams.dstRepStride),
                b32Preg);
        }
    }
}

template <typename DST_TYPE, typename SRC_TYPE>
__aicore__ inline void GenLoadL0(
    Reg::RegTensor<SRC_TYPE>& srcVreg, __ubuf__ SRC_TYPE*& srcAddr, Reg::MaskReg& preg,
    const UnaryRepeatParams& repeatParams)
{
    Reg::LoadAlign<SRC_TYPE, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
        srcVreg, srcAddr, static_cast<uint32_t>(repeatParams.srcBlkStride),
        static_cast<uint32_t>(repeatParams.srcRepStride), preg);
    if constexpr (SupportType<SRC_TYPE, int4x2_t, fp4x2_e2m1_t, fp4x2_e1m2_t>() && sizeof(DST_TYPE) == 2) {
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
__aicore__ inline void GenStoreL0(
    __ubuf__ DST_TYPE*& dstAddr, Reg::RegTensor<DST_TYPE>& dstVreg, Reg::MaskReg& preg,
    const UnaryRepeatParams& repeatParams)
{
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
    Reg::StoreAlign<DST_TYPE, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
        dstAddr, dstVreg, static_cast<uint32_t>(repeatParams.dstBlkStride),
        static_cast<uint32_t>(repeatParams.dstRepStride), preg);
}

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode>
__aicore__ inline void CastIntrinsicsImplVF2(
    __ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint64_t mask[], uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
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
        GenLoadL0<DST_TYPE, SRC_TYPE>(srcVreg, src, ldPreg, repeatParams);
        if constexpr (Std::is_same_v<SRC_TYPE, int32_t> && Std::is_same_v<DST_TYPE, half>) {
            Reg::Cast<float, SRC_TYPE, castTrait>((Reg::RegTensor<float>&)dstVreg, srcVreg, exPreg);
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
__aicore__ inline void CastIntrinsicsB64ImplCounterVF(
    __ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint64_t mask, __ubuf__ uint64_t* maskBuf, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, roundMode};
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
            Reg::LoadAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                (Reg::RegTensor<uint32_t>&)srcVreg, (__ubuf__ uint32_t*&)src,
                static_cast<uint32_t>(repeatParams.srcBlkStride), static_cast<uint32_t>(repeatParams.srcRepStride),
                b32Preg);
            Reg::Interleave((Reg::RegTensor<uint32_t>&)srcVreg, tmpVreg, (Reg::RegTensor<uint32_t>&)srcVreg, zeroVreg);
        } else {
            // b64->b32
            Reg::LoadAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                (Reg::RegTensor<uint32_t>&)srcVreg, (__ubuf__ uint32_t*&)src,
                static_cast<uint32_t>(repeatParams.srcBlkStride), static_cast<uint32_t>(repeatParams.srcRepStride),
                b64Preg);
        }
        Reg::Cast<DST_TYPE, SRC_TYPE, castTrait>(dstVreg, srcVreg, b64Preg);
        if constexpr (sizeof(DST_TYPE) == sizeof(int64_t)) {
            // b32->b64
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_STORE>();
            Reg::StoreAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                (__ubuf__ uint32_t*&)dst, (Reg::RegTensor<uint32_t>&)dstVreg,
                static_cast<uint32_t>(repeatParams.dstBlkStride), static_cast<uint32_t>(repeatParams.dstRepStride),
                b64Preg);
        } else {
            // b64->b32
            Reg::DeInterleave(
                (Reg::RegTensor<uint32_t>&)dstVreg, tmpVreg, (Reg::RegTensor<uint32_t>&)dstVreg, zeroVreg);
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_STORE>();
            Reg::StoreAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                (__ubuf__ uint32_t*&)dst, (Reg::RegTensor<uint32_t>&)dstVreg,
                static_cast<uint32_t>(repeatParams.dstBlkStride), static_cast<uint32_t>(repeatParams.dstRepStride),
                b32Preg);
        }
    }
}

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode, bool isSetMask>
__aicore__ inline void CastIntrinsicsImplCounterVF(
    __ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint64_t mask, __ubuf__ uint64_t* maskBuf, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, roundMode};
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
        GenLoadL0<DST_TYPE, SRC_TYPE>(srcVreg, src, ldPreg, repeatParams);
        if constexpr (Std::is_same_v<SRC_TYPE, int32_t> && Std::is_same_v<DST_TYPE, half>) {
            Reg::Cast<float, SRC_TYPE, castTrait>((Reg::RegTensor<float>&)dstVreg, srcVreg, exPreg);
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
            maskBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 2);
        }
        if constexpr (b64Cast) {
            VF_CALL<CastIntrinsicsB64ImplCounterVF<DST_TYPE, SRC_TYPE, roundMode, isSetMask>>(
                dst, src, mask[0], maskBuf, repeatTime, repeatParams);
        } else {
            VF_CALL<CastIntrinsicsImplCounterVF<DST_TYPE, SRC_TYPE, roundMode, isSetMask>>(
                dst, src, mask[0], maskBuf, repeatTime, repeatParams);
        }
    } else {
        if constexpr (b64Cast) {
            if constexpr (isSetMask) {
                SetVectorMask<uint32_t>(mask[1], mask[0]);
            }
            VF_CALL<CastIntrinsicsB64ImplVF2<DST_TYPE, SRC_TYPE, roundMode>>(dst, src, mask, repeatTime, repeatParams);
        } else {
            if constexpr (isSetMask) {
                if constexpr (sizeof(DST_TYPE) < sizeof(SRC_TYPE)) {
                    SetVectorMask<SRC_TYPE>(mask[1], mask[0]);
                } else {
                    SetVectorMask<DST_TYPE>(mask[1], mask[0]);
                }
            }
            VF_CALL<CastIntrinsicsImplVF2<DST_TYPE, SRC_TYPE, roundMode>>(dst, src, mask, repeatTime, repeatParams);
        }
    }
}

// Cast::Level 0 - mask bit mode
template <typename ORI_DST_TYPE, typename ORI_SRC_TYPE, bool isSetMask = true>
__aicore__ inline void CastImpl(
    __ubuf__ ORI_DST_TYPE* oriDst, __ubuf__ ORI_SRC_TYPE* oriSrc, const RoundMode& roundMode, const uint64_t mask[],
    uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
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
        Tuple<fp4x2_e1m2_t, bfloat16_t>, Tuple<fp4x2_e2m1_t, bfloat16_t>, Tuple<half, int32_t>, Tuple<float, float>>();
    constexpr bool cast_none = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<float, half>, Tuple<float, bfloat16_t>, Tuple<half, int4x2_t>,
        Tuple<half, uint8_t>, Tuple<uint16_t, uint8_t>, Tuple<uint32_t, uint8_t>, Tuple<half, int8_t>,
        Tuple<int16_t, int8_t>, Tuple<int32_t, int8_t>, Tuple<uint8_t, uint16_t>, Tuple<uint32_t, uint16_t>,
        Tuple<float, int16_t>, Tuple<uint8_t, int16_t>, Tuple<uint32_t, int16_t>, Tuple<int32_t, int16_t>,
        Tuple<uint8_t, uint32_t>, Tuple<uint16_t, uint32_t>, Tuple<int16_t, uint32_t>, Tuple<int64_t, int32_t>,
        Tuple<int16_t, int32_t>, Tuple<uint8_t, int32_t>, Tuple<uint16_t, int32_t>, Tuple<int32_t, int64_t>,
        Tuple<half, hifloat8_t>, Tuple<float, hifloat8_t>, Tuple<float, fp8_e4m3fn_t>, Tuple<float, fp8_e5m2_t>,
        Tuple<bfloat16_t, fp4x2_e1m2_t>, Tuple<bfloat16_t, fp4x2_e2m1_t>, Tuple<int4x2_t, int16_t>,
        Tuple<int16_t, int4x2_t>, Tuple<bfloat16_t, int4x2_t>>();
    constexpr bool using_cast_rint = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<int8_t, half>, Tuple<uint8_t, half>, Tuple<int4x2_t, half>, Tuple<half, float>,
        Tuple<half, int16_t>, Tuple<float, int32_t>>();
    constexpr bool cast_odd = SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<half, float>>();
    constexpr bool cast_rint =
        SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<fp8_e5m2_t, float>, Tuple<fp8_e4m3fn_t, float>>();
    constexpr bool cast_round =
        SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<hifloat8_t, float>, Tuple<hifloat8_t, half>>();
    constexpr bool cast_hybrid =
        SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<hifloat8_t, float>, Tuple<hifloat8_t, half>>();
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

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode, bool isSetMask>
__aicore__ inline void CastIntrinsicsB64ImplVF1(
    __ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    static constexpr Reg::CastTrait castTrait = {
        Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, roundMode};
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
            // b32-> b64
            Reg::LoadAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                (Reg::RegTensor<uint32_t>&)srcVreg, (__ubuf__ uint32_t*&)src,
                static_cast<uint32_t>(repeatParams.srcBlkStride), static_cast<uint32_t>(repeatParams.srcRepStride),
                b32Preg);
            Reg::Interleave((Reg::RegTensor<uint32_t>&)srcVreg, tmpVreg, (Reg::RegTensor<uint32_t>&)srcVreg, zeroVreg);
        } else {
            // b64 -> b32
            Reg::LoadAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                (Reg::RegTensor<uint32_t>&)srcVreg, (__ubuf__ uint32_t*&)src,
                static_cast<uint32_t>(repeatParams.srcBlkStride), static_cast<uint32_t>(repeatParams.srcRepStride),
                b64Preg);
        }
        Reg::Cast<DST_TYPE, SRC_TYPE, castTrait>(dstVreg, srcVreg, b64Preg);
        if constexpr (sizeof(DST_TYPE) == sizeof(int64_t)) {
            // b32-> b64
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_STORE>();
            Reg::StoreAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                (__ubuf__ uint32_t*&)dst, (Reg::RegTensor<uint32_t>&)dstVreg,
                static_cast<uint32_t>(repeatParams.dstBlkStride), static_cast<uint32_t>(repeatParams.dstRepStride),
                b64Preg);
        } else {
            // b64 -> b32
            Reg::DeInterleave(
                (Reg::RegTensor<uint32_t>&)dstVreg, tmpVreg, (Reg::RegTensor<uint32_t>&)dstVreg, zeroVreg);
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_STORE>();
            Reg::StoreAlign<uint32_t, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                (__ubuf__ uint32_t*&)dst, (Reg::RegTensor<uint32_t>&)dstVreg,
                static_cast<uint32_t>(repeatParams.dstBlkStride), static_cast<uint32_t>(repeatParams.dstRepStride),
                b32Preg);
        }
    }
}

template <typename DST_TYPE, typename SRC_TYPE, RoundMode roundMode, bool isSetMask>
__aicore__ inline void CastIntrinsicsImplVF1(
    __ubuf__ DST_TYPE* dst, __ubuf__ SRC_TYPE* src, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
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
        GenLoadL0<DST_TYPE, SRC_TYPE>(srcVreg, src, ldPreg, repeatParams);
        if constexpr (Std::is_same_v<SRC_TYPE, int32_t> && Std::is_same_v<DST_TYPE, half>) {
            Reg::Cast<float, SRC_TYPE, castTrait>((Reg::RegTensor<float>&)dstVreg, srcVreg, exPreg);
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
            maskBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 2);
        }
        if constexpr (b64Cast) {
            VF_CALL<CastIntrinsicsB64ImplCounterVF<DST_TYPE, SRC_TYPE, roundMode, isSetMask>>(
                dst, src, mask, maskBuf, repeatTime, repeatParams);
        } else {
            VF_CALL<CastIntrinsicsImplCounterVF<DST_TYPE, SRC_TYPE, roundMode, isSetMask>>(
                dst, src, mask, maskBuf, repeatTime, repeatParams);
        }
    } else {
        if constexpr (b64Cast) {
            VF_CALL<CastIntrinsicsB64ImplVF1<DST_TYPE, SRC_TYPE, roundMode, isSetMask>>(
                dst, src, mask, repeatTime, repeatParams);
        } else {
            VF_CALL<CastIntrinsicsImplVF1<DST_TYPE, SRC_TYPE, roundMode, isSetMask>>(
                dst, src, mask, repeatTime, repeatParams);
        }
    }
}

// Cast::Level 0 - mask count mode
template <typename ORI_DST_TYPE, typename ORI_SRC_TYPE, bool isSetMask = true>
__aicore__ inline void CastImpl(
    __ubuf__ ORI_DST_TYPE* oriDst, __ubuf__ ORI_SRC_TYPE* oriSrc, const RoundMode& roundMode, const uint64_t mask,
    uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
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
        Tuple<fp4x2_e1m2_t, bfloat16_t>, Tuple<fp4x2_e2m1_t, bfloat16_t>, Tuple<half, int32_t>, Tuple<float, float>>();
    constexpr bool cast_none = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<float, half>, Tuple<float, bfloat16_t>, Tuple<half, int4x2_t>,
        Tuple<half, uint8_t>, Tuple<uint16_t, uint8_t>, Tuple<uint32_t, uint8_t>, Tuple<half, int8_t>,
        Tuple<int16_t, int8_t>, Tuple<int32_t, int8_t>, Tuple<uint8_t, uint16_t>, Tuple<uint32_t, uint16_t>,
        Tuple<float, int16_t>, Tuple<uint8_t, int16_t>, Tuple<uint32_t, int16_t>, Tuple<int32_t, int16_t>,
        Tuple<uint8_t, uint32_t>, Tuple<uint16_t, uint32_t>, Tuple<int16_t, uint32_t>, Tuple<int64_t, int32_t>,
        Tuple<int16_t, int32_t>, Tuple<uint8_t, int32_t>, Tuple<uint16_t, int32_t>, Tuple<int32_t, int64_t>,
        Tuple<half, hifloat8_t>, Tuple<float, hifloat8_t>, Tuple<float, fp8_e4m3fn_t>, Tuple<float, fp8_e5m2_t>,
        Tuple<bfloat16_t, fp4x2_e1m2_t>, Tuple<bfloat16_t, fp4x2_e2m1_t>, Tuple<int4x2_t, int16_t>,
        Tuple<int16_t, int4x2_t>, Tuple<bfloat16_t, int4x2_t>>();
    constexpr bool using_cast_rint = SupportType<
        Tuple<DST_TYPE, SRC_TYPE>, Tuple<int8_t, half>, Tuple<uint8_t, half>, Tuple<int4x2_t, half>, Tuple<half, float>,
        Tuple<half, int16_t>, Tuple<float, int32_t>>();
    constexpr bool cast_odd = SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<half, float>>();
    constexpr bool cast_rint =
        SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<fp8_e5m2_t, float>, Tuple<fp8_e4m3fn_t, float>>();
    constexpr bool cast_round =
        SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<hifloat8_t, float>, Tuple<hifloat8_t, half>>();
    constexpr bool cast_hybrid =
        SupportType<Tuple<DST_TYPE, SRC_TYPE>, Tuple<hifloat8_t, float>, Tuple<hifloat8_t, half>>();
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

// scale is stored as  1 sign bit, 8 exponent bits and 10 mantissa bits in 1971 hardware
// ===============================================================================
// | 1 signMode bit  | 8 exponent bits | 10bit mantissa bits |       000..0       |
// ===============================================================================
__aicore__ inline float GetCastDeqScale(const uint64_t deqScale)
{
    uint32_t tmp = static_cast<uint32_t>(deqScale & 0xffffffff);
    tmp = tmp & 0xffffe000;
    float ret = *(reinterpret_cast<float*>(&tmp));
    return ret;
}

__aicore__ inline int16_t GetCastDeqOffset(const uint64_t deqScale)
{
    int16_t ret = static_cast<int16_t>((deqScale >> 37) & 0x1ff);
    return ret;
}

__aicore__ inline bool GetCastDeqSignMode(const uint64_t deqScale)
{
    bool ret = static_cast<bool>(deqScale >> 46);
    return ret;
}

template <typename T, Reg::HighLowPart part>
__aicore__ inline void CastDeqMulsCal(
    Reg::RegTensor<float>& tmpReg, Reg::RegTensor<T>& srcReg, Reg::MaskReg& maskReg, const float scale)
{
    Reg::Cast<float, T, CastParam::s162f32CastTrait>(tmpReg, srcReg, maskReg);
    Reg::Muls(tmpReg, tmpReg, scale, maskReg);
    Reg::Cast<T, float, CastParam::f322s16CastTrait>(srcReg, tmpReg, maskReg);
    Reg::Pack<uint16_t, uint32_t, part>((Reg::RegTensor<uint16_t>&)srcReg, (Reg::RegTensor<uint32_t>&)srcReg);
}

template <typename T, Reg::HighLowPart part>
__aicore__ inline void CastVecDeqMulsCal(
    Reg::RegTensor<int32_t>& tmpReg, Reg::RegTensor<T>& srcReg, Reg::MaskReg& maskReg, Reg::RegTensor<float>& scaleReg)
{
    Reg::Cast<float, T, CastParam::s162f32CastTrait>((Reg::RegTensor<float>&)tmpReg, srcReg, maskReg);
    Reg::Mul((Reg::RegTensor<float>&)tmpReg, (Reg::RegTensor<float>&)tmpReg, scaleReg, maskReg);
    Reg::Cast<T, float, CastParam::f322s16CastTrait>(srcReg, (Reg::RegTensor<float>&)tmpReg, maskReg);
    Reg::Pack<uint16_t, uint32_t, part>((Reg::RegTensor<uint16_t>&)srcReg, (Reg::RegTensor<uint32_t>&)srcReg);
}

template <typename U, typename T, bool halfBlock, bool signMode>
__aicore__ inline void CastFromS92B8(Reg::RegTensor<T>& srcReg, Reg::RegTensor<U>& dstReg, Reg::MaskReg& maskReg)
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
__aicore__ inline void GenGatherIndex(Reg::RegTensor<int8_t>& dstReg)
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

__aicore__ inline void GenVecCastDeqParam(
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
    Reg::Arange(scaleIndexReg, zero);
    Reg::Duplicate(tmpReg, gatherConstant);
    Reg::Div(tmpReg, scaleIndexReg, tmpReg, fullMask);
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
    Reg::Div((Reg::RegTensor<int16_t>&)tmpReg, offsetIndexReg, (Reg::RegTensor<int16_t>&)tmpReg, fullMask);
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
__aicore__ inline void GenLevel0StoreMask(
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
__aicore__ inline void CastVecDeqImplVF(
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
__aicore__ inline void CastDeqImplVF(__ubuf__ U* dst, __ubuf__ T* src, const uint32_t calCount, uint64_t deqScale)
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
__aicore__ inline void CastDeqS322f16ImplVF(
    __ubuf__ U* dst, __ubuf__ T* src, const uint32_t calCount, const half deqScale)
{
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<float> tmpReg;
    Reg::RegTensor<U> dstReg;
    Reg::MaskReg maskReg;
    constexpr uint32_t oneRepSize = GetVecLen() / sizeof(T);
    uint16_t repeatTime = CeilDivision(calCount, oneRepSize);
    uint32_t sreg = calCount;
    for (uint16_t i = 0; i < repeatTime; ++i) {
        maskReg = Reg::UpdateMask<T>(sreg);
        Reg::LoadAlign(srcReg, src + i * oneRepSize);
        Reg::Cast<float, T, CastParam::s322F32CastTrait>(tmpReg, srcReg, maskReg);
        Reg::Cast<U, float, CastParam::f322F16CastTrait>(dstReg, tmpReg, maskReg);
        Reg::Muls(dstReg, dstReg, deqScale, maskReg);
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
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
        SetFlag<HardEvent::S_V>(eventIdSToV);
        WaitFlag<HardEvent::S_V>(eventIdSToV);
        VF_CALL<CastDeqS322f16ImplVF<U, T>>(dst, src, calCount, scale);
    } else {
        uint64_t deqScale = Internal::g_deqScale;
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
        SetFlag<HardEvent::S_V>(eventIdSToV);
        WaitFlag<HardEvent::S_V>(eventIdSToV);
        if constexpr (isVecDeq) {
            VF_CALL<CastVecDeqImplVF<U, T, halfBlock>>(dst, src, calCount, deqScale);
        } else {
            bool signMode = GetCastDeqSignMode(deqScale);
            if (signMode) {
                VF_CALL<CastDeqImplVF<U, T, halfBlock, true>>(dst, src, calCount, deqScale);
            } else {
                VF_CALL<CastDeqImplVF<U, T, halfBlock, false>>(dst, src, calCount, deqScale);
            }
        }
    }
}

template <typename U, typename T, bool isCounterMode, bool isBitMap, bool isSetMask, bool halfBlock>
__aicore__ inline void CastVecDeqLevel0ImplVF(
    __ubuf__ U* dst, __ubuf__ T* src, const int32_t mask, __ubuf__ uint64_t* tempBuf, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, uint64_t deqScaleAddr)
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
__aicore__ inline void CastDeqLevel0ImplVF(
    __ubuf__ U* dst, __ubuf__ T* src, const int32_t mask, __ubuf__ uint64_t* tempBuf, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, uint64_t deqScale)
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
__aicore__ inline void CastDeqS322f16Level0ImplVF(
    __ubuf__ U* dst, __ubuf__ T* src, const int32_t mask, __ubuf__ uint64_t* tempBuf, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, const half deqScale)
{
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<float> tmpReg;
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
        Reg::Cast<float, T, CastParam::s322F32CastTrait>(tmpReg, srcReg, maskReg);
        Reg::Cast<U, float, CastParam::f322F16CastTrait>(dstReg, tmpReg, maskReg);
        Reg::Muls(dstReg, dstReg, deqScale, maskReg);
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
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
        SetFlag<HardEvent::S_V>(eventIdSToV);
        WaitFlag<HardEvent::S_V>(eventIdSToV);
        if (isCounterMode) {
            VF_CALL<CastDeqS322f16Level0ImplVF<U, T, true, true, isSetMask>>(
                dst, src, mask[0], tempBuf, repeatTime, repeatParams, scale);
        } else {
            VF_CALL<CastDeqS322f16Level0ImplVF<U, T, false, true, isSetMask>>(
                dst, src, mask[0], tempBuf, repeatTime, repeatParams, scale);
        }
    } else {
        uint64_t deqScale = Internal::g_deqScale;
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
        SetFlag<HardEvent::S_V>(eventIdSToV);
        WaitFlag<HardEvent::S_V>(eventIdSToV);
        bool signMode = GetCastDeqSignMode(deqScale);
        if (isCounterMode) {
            if constexpr (isVecDeq) {
                VF_CALL<CastVecDeqLevel0ImplVF<U, T, true, true, isSetMask, halfBlock>>(
                    dst, src, mask[0], tempBuf, repeatTime, repeatParams, deqScale);
            } else {
                if (signMode) {
                    VF_CALL<CastDeqLevel0ImplVF<U, T, true, true, isSetMask, halfBlock, true>>(
                        dst, src, mask[0], tempBuf, repeatTime, repeatParams, deqScale);
                } else {
                    VF_CALL<CastDeqLevel0ImplVF<U, T, true, true, isSetMask, halfBlock, false>>(
                        dst, src, mask[0], tempBuf, repeatTime, repeatParams, deqScale);
                }
            }
        } else {
            if constexpr (isVecDeq) {
                VF_CALL<CastVecDeqLevel0ImplVF<U, T, false, true, isSetMask, halfBlock>>(
                    dst, src, 0, tempBuf, repeatTime, repeatParams, deqScale);
            } else {
                if (signMode) {
                    VF_CALL<CastDeqLevel0ImplVF<U, T, false, true, isSetMask, halfBlock, true>>(
                        dst, src, 0, tempBuf, repeatTime, repeatParams, deqScale);
                } else {
                    VF_CALL<CastDeqLevel0ImplVF<U, T, false, true, isSetMask, halfBlock, false>>(
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
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
        SetFlag<HardEvent::S_V>(eventIdSToV);
        WaitFlag<HardEvent::S_V>(eventIdSToV);
        if (isCounterMode) {
            VF_CALL<CastDeqS322f16Level0ImplVF<U, T, true, false, isSetMask>>(
                dst, src, mask, tempBuf, repeatTime, repeatParams, scale);
        } else {
            VF_CALL<CastDeqS322f16Level0ImplVF<U, T, false, false, isSetMask>>(
                dst, src, mask, tempBuf, repeatTime, repeatParams, scale);
        }
    } else {
        uint64_t deqScale = Internal::g_deqScale;
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
        SetFlag<HardEvent::S_V>(eventIdSToV);
        WaitFlag<HardEvent::S_V>(eventIdSToV);
        bool signMode = GetCastDeqSignMode(deqScale);
        if (isCounterMode) {
            if constexpr (isVecDeq) {
                VF_CALL<CastVecDeqLevel0ImplVF<U, T, true, false, isSetMask, halfBlock>>(
                    dst, src, mask, tempBuf, repeatTime, repeatParams, deqScale);
            } else {
                if (signMode) {
                    VF_CALL<CastDeqLevel0ImplVF<U, T, true, false, isSetMask, halfBlock, true>>(
                        dst, src, mask, tempBuf, repeatTime, repeatParams, deqScale);
                } else {
                    VF_CALL<CastDeqLevel0ImplVF<U, T, true, false, isSetMask, halfBlock, false>>(
                        dst, src, mask, tempBuf, repeatTime, repeatParams, deqScale);
                }
            }
        } else {
            if constexpr (isVecDeq) {
                VF_CALL<CastVecDeqLevel0ImplVF<U, T, false, false, isSetMask, halfBlock>>(
                    dst, src, mask, tempBuf, repeatTime, repeatParams, deqScale);
            } else {
                if (signMode) {
                    VF_CALL<CastDeqLevel0ImplVF<U, T, false, false, isSetMask, halfBlock, true>>(
                        dst, src, mask, tempBuf, repeatTime, repeatParams, deqScale);
                } else {
                    VF_CALL<CastDeqLevel0ImplVF<U, T, false, false, isSetMask, halfBlock, false>>(
                        dst, src, mask, tempBuf, repeatTime, repeatParams, deqScale);
                }
            }
        }
    }
    AscendCUtils::FreeTemporaryBuffer(tempBuf);
}

namespace RegAddReluCast {
template <typename T1, typename T2, typename RegT, typename RegU>
__aicore__ inline void AddReluCast(RegT& dstReg, RegU& src0Reg, RegU& src1Reg, Reg::MaskReg& mask)
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
__aicore__ inline void AddReluCastImpl(__ubuf__ T1* dst, __ubuf__ T2* src0, __ubuf__ T2* src1, const uint32_t calCount)
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
        __VEC_SCOPE__
        {
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
        }
    } else {
        constexpr uint32_t sregLower = static_cast<uint32_t>(VECTOR_REG_WIDTH / sizeof(T2));
        const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, sregLower));
        __VEC_SCOPE__
        {
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
}

namespace RegSubReluCast {
template <typename T1, typename T2, typename RegT, typename RegU>
__aicore__ inline void SubReluCast(RegT& dstReg, RegU& src0Reg, RegU& src1Reg, Reg::MaskReg& mask)
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
__aicore__ inline void SubReluCastImpl(__ubuf__ T1* dst, __ubuf__ T2* src0, __ubuf__ T2* src1, const uint32_t calCount)
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
        __VEC_SCOPE__
        {
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
        }
    } else {
        const uint32_t repeatStride = static_cast<uint32_t>(VECTOR_REG_WIDTH / sizeof(T2));
        const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, repeatStride));
        __VEC_SCOPE__
        {
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
}

//  castDequantValue bit arrange
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
    event_t eventIdVToS = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_S));
    SetFlag<HardEvent::V_S>(eventIdVToS);
    WaitFlag<HardEvent::V_S>(eventIdVToS);
    Internal::g_deqScale = MakeDeqScaleConfig(scale, offset, signMode);
}

template <typename T>
__aicore__ inline void SetDeqScaleImpl(const LocalTensor<T>& vdeqTensor, const VdeqInfo& vdeqInfo)
{
    event_t eventIdVToS = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_S));
    SetFlag<HardEvent::V_S>(eventIdVToS);
    WaitFlag<HardEvent::V_S>(eventIdVToS);
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
    event_t eventIdVToS = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_S));
    SetFlag<HardEvent::V_S>(eventIdVToS);
    WaitFlag<HardEvent::V_S>(eventIdVToS);
    Internal::g_deqValue = config;
}
// Truncate::Level2
template <typename T, RoundMode roundMode>
__aicore__ inline void TruncateImpl(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t calCount)
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
    const uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(calCount, sregLower));
    uint32_t sreg = static_cast<uint32_t>(calCount);
    __VEC_SCOPE__
    {
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
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_VEC_VCONV_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_VCONV_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_VCONV_IMPL_H__
#endif
