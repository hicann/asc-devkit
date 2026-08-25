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
    "impl/adv_api/detail/math/hypot/hypot_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/hypot.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_H__YPOT_HYPOT_COMMON_IMPL_H
#endif
#ifndef LIB_MATH_HYPOT_IMPL_H
#define LIB_MATH_HYPOT_IMPL_H
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
// Implementation Process
// 1. Use vcmp_ne for comparison to find nan.
// 2. Use vcmps_eq for comparison to find ±inf.
// 3. Use Mask Operator to get final mask.

namespace AscendC {
namespace HypotInternal {
constexpr uint16_t B_HALF_ONE = 0x3f80;
constexpr uint32_t INF = 0x7f800000;
constexpr uint32_t NEG_INF = 0xff800000;
constexpr uint32_t ONE = 0x3f800000;
constexpr uint16_t HALF_INF = 0x7c00;
constexpr uint16_t HALF_NEG_INF = 0xfc00;
constexpr uint16_t HALF_ONE = 0x3c00;
constexpr uint16_t B_HALF_INF = 0x7f80;
constexpr uint16_t B_HALF_NEG_INF = 0xff80;

constexpr uint32_t AND_OPERATOR = 0xfe000000;
constexpr uint32_t ADD_OPERATOR = 0x7e800000;
constexpr uint32_t OR_OPERATOR = 0x800000;

constexpr Reg::CastTrait HYPOT_CAST_TRAIT_RINT = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};
} // namespace HypotInternal

template <typename T, typename U>
__simd_callee__ inline void CompareScalar(
    Reg::MaskReg& cmpMaskZero, Reg::MaskReg& cmpMaskSrcInf, Reg::RegTensor<T>& vSrcTmpReg0,
    Reg::RegTensor<T>& vSrcTmpReg1, Reg::MaskReg& cmpMaskSrc0NAN, Reg::MaskReg& cmpMaskSrc1NAN,
    Reg::RegTensor<T>& vRegOne, Reg::RegTensor<T>& vSrcReg0, Reg::RegTensor<T>& vSrcReg1, const U INF, const U NEG_INF,
    Reg::MaskReg maskReg)
{
    Reg::MaskReg cmpMaskSrc0INF;
    Reg::MaskReg cmpMaskSrc1INF;
    Reg::MaskReg cmpMaskSrc0NINF;
    Reg::MaskReg cmpMaskSrc1NINF;
    Reg::MaskReg cmpMaskSrc0Zero;
    Reg::MaskReg cmpMaskSrc1Zero;
    Reg::MaskReg cmpMaskSrc0Inf;
    Reg::MaskReg cmpMaskSrc1Inf;

    Reg::CompareScalar<U>(cmpMaskSrc0INF, (Reg::RegTensor<U>&)vSrcReg0, INF, maskReg);
    Reg::CompareScalar<U>(cmpMaskSrc1INF, (Reg::RegTensor<U>&)vSrcReg1, INF, maskReg);
    Reg::CompareScalar<U>(cmpMaskSrc0NINF, (Reg::RegTensor<U>&)vSrcReg0, NEG_INF, maskReg);
    Reg::CompareScalar<U>(cmpMaskSrc1NINF, (Reg::RegTensor<U>&)vSrcReg1, NEG_INF, maskReg);
    Reg::CompareScalar<U>(cmpMaskSrc0Zero, (Reg::RegTensor<U>&)vSrcReg0, 0, maskReg);
    Reg::CompareScalar<U>(cmpMaskSrc1Zero, (Reg::RegTensor<U>&)vSrcReg1, 0, maskReg);

    Reg::MaskAnd(cmpMaskZero, cmpMaskSrc0Zero, cmpMaskSrc1Zero, maskReg);
    Reg::MaskOr(cmpMaskSrc0Inf, cmpMaskSrc0INF, cmpMaskSrc0NINF, maskReg);
    Reg::MaskOr(cmpMaskSrc1Inf, cmpMaskSrc1INF, cmpMaskSrc1NINF, maskReg);
    Reg::MaskOr(cmpMaskSrcInf, cmpMaskSrc0Inf, cmpMaskSrc1Inf, maskReg);
    Reg::Select(vSrcTmpReg0, vRegOne, vSrcReg0, cmpMaskSrc0NAN);
    Reg::Select(vSrcTmpReg1, vRegOne, vSrcReg1, cmpMaskSrc1NAN);
    Reg::Select(vSrcTmpReg0, vRegOne, vSrcReg0, cmpMaskSrcInf);
    Reg::Select(vSrcTmpReg1, vRegOne, vSrcReg1, cmpMaskSrcInf);
}

__simd_callee__ inline void HypotCommonProcess(
    Reg::RegTensor<float>& vSrcTmpReg0, Reg::RegTensor<float>& vSrcTmpReg1, Reg::RegTensor<float>& vDstReg0,
    Reg::MaskReg maskReg)
{
    Reg::RegTensor<float> vTmpReg0, vTmpReg1, vTmpReg2, vTmpReg3;
    Reg::RegTensor<int32_t> vAndReg, vNegReg, vAddsReg, vOrReg, vMinReg, vMaxReg;
    Reg::RegTensor<int32_t> vConstReg0, vConstReg1;
    Reg::Abs(vSrcTmpReg0, vSrcTmpReg0, maskReg);
    Reg::Abs(vSrcTmpReg1, vSrcTmpReg1, maskReg);
    Reg::Min(vMinReg, (Reg::RegTensor<int32_t>&)vSrcTmpReg0, (Reg::RegTensor<int32_t>&)vSrcTmpReg1, maskReg);
    Reg::Max(vMaxReg, (Reg::RegTensor<int32_t>&)vSrcTmpReg0, (Reg::RegTensor<int32_t>&)vSrcTmpReg1, maskReg);
    Reg::Duplicate(vConstReg0, HypotInternal::AND_OPERATOR, maskReg);
    Reg::And(vAndReg, vMaxReg, vConstReg0, maskReg);
    Reg::Neg(vNegReg, vAndReg, maskReg);
    Reg::Adds(vAddsReg, vNegReg, HypotInternal::ADD_OPERATOR, maskReg);
    Reg::Mul(vTmpReg0, (Reg::RegTensor<float>&)vMinReg, (Reg::RegTensor<float>&)vAddsReg, maskReg);
    Reg::Mul(vTmpReg1, (Reg::RegTensor<float>&)vMaxReg, (Reg::RegTensor<float>&)vAddsReg, maskReg);
    Reg::Mul(vTmpReg2, vTmpReg0, vTmpReg0, maskReg);
    Reg::MulAddDst(vTmpReg2, vTmpReg1, vTmpReg1, maskReg);
    Reg::Sqrt(vTmpReg3, vTmpReg2, maskReg);
    Reg::Duplicate(vConstReg1, HypotInternal::OR_OPERATOR, maskReg);
    Reg::Or(vOrReg, vAndReg, vConstReg1, maskReg);
    Reg::Mul(vDstReg0, vTmpReg3, (Reg::RegTensor<float>&)vOrReg, maskReg);
}

template <typename T>
__simd_callee__ inline void HypotCompute(
    Reg::RegTensor<T>& vSrcTmpReg0, Reg::RegTensor<T>& vSrcTmpReg1, Reg::RegTensor<T>& vDstReg0, Reg::MaskReg maskReg)
{
    if constexpr (IsSameType<T, bfloat16_t>::value) {
        Reg::RegTensor<float> vDstF, vSrc0, vSrc1;
        Reg::Cast<float, bfloat16_t, HypotInternal::HYPOT_CAST_TRAIT_RINT>(vSrc0, vSrcTmpReg0, maskReg);
        Reg::Cast<float, bfloat16_t, HypotInternal::HYPOT_CAST_TRAIT_RINT>(vSrc1, vSrcTmpReg1, maskReg);

        HypotCommonProcess(vSrc0, vSrc1, vDstF, maskReg);

        Reg::Cast<bfloat16_t, float, HypotInternal::HYPOT_CAST_TRAIT_RINT>(vDstReg0, vDstF, maskReg);
    } else if constexpr (IsSameType<T, half>::value) {
        Reg::RegTensor<float> vDstF, vSrc0, vSrc1;
        Reg::Cast<float, half, HypotInternal::HYPOT_CAST_TRAIT_RINT>(vSrc0, vSrcTmpReg0, maskReg);
        Reg::Cast<float, half, HypotInternal::HYPOT_CAST_TRAIT_RINT>(vSrc1, vSrcTmpReg1, maskReg);

        HypotCommonProcess(vSrc0, vSrc1, vDstF, maskReg);

        Reg::Cast<half, float, HypotInternal::HYPOT_CAST_TRAIT_RINT>(vDstReg0, vDstF, maskReg);
    } else {
        HypotCommonProcess(vSrcTmpReg0, vSrcTmpReg1, vDstReg0, maskReg);
    }
}

template <typename T>
__simd_vf__ inline void VfHypotImpl(__ubuf__ T* dstUb, __ubuf__ T* src0Ub, __ubuf__ T* src1Ub, const uint32_t calCount)
{
    Reg::RegTensor<T> vSrcReg0;
    Reg::RegTensor<T> vSrcReg1;
    Reg::RegTensor<T> vDstReg0;
    Reg::RegTensor<T> vRegZero;
    Reg::RegTensor<T> vRegOne;
    Reg::RegTensor<T> vRegInf;
    Reg::RegTensor<T> vSrcTmpReg0;
    Reg::RegTensor<T> vSrcTmpReg1;

    Reg::MaskReg maskReg;
    Reg::MaskReg cmpMaskZero;
    Reg::MaskReg cmpMaskSrcInf;
    Reg::MaskReg cmpMaskSrc0NAN;
    Reg::MaskReg cmpMaskSrc1NAN;

    uint32_t sreg = static_cast<uint32_t>(calCount);
    uint32_t sregLower = static_cast<uint32_t>(GetVecLen() / sizeof(T));
    if constexpr ((IsSameType<T, bfloat16_t>::value) || (IsSameType<T, half>::value)) {
        sregLower = sregLower >> 1;
        sreg = sreg * 2;
    }
    uint16_t repeatTimes = static_cast<uint16_t>(CeilDivision(calCount, sregLower));

    if constexpr (IsSameType<T, float>::value) {
        Reg::Duplicate((Reg::RegTensor<uint32_t>&)vRegInf, HypotInternal::INF);
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
        Reg::Duplicate((Reg::RegTensor<uint32_t>&)vRegOne, HypotInternal::ONE);
#else
        Reg::Duplicate((Reg::RegTensor<uint32_t>&)vRegOne, 1.0f);
#endif
        Reg::Duplicate((Reg::RegTensor<uint32_t>&)vRegZero, 0);
    } else if constexpr (IsSameType<T, half>::value) {
        Reg::Duplicate((Reg::RegTensor<uint16_t>&)vRegInf, HypotInternal::HALF_INF);
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
        Reg::Duplicate((Reg::RegTensor<uint16_t>&)vRegOne, HypotInternal::HALF_ONE);
#else
        Reg::Duplicate((Reg::RegTensor<uint16_t>&)vRegOne, 1.0f);
#endif
        Reg::Duplicate((Reg::RegTensor<uint16_t>&)vRegZero, 0);
    } else if constexpr (IsSameType<T, bfloat16_t>::value) {
        Reg::Duplicate((Reg::RegTensor<uint16_t>&)vRegInf, HypotInternal::B_HALF_INF);
        Reg::Duplicate((Reg::RegTensor<uint16_t>&)vRegOne, HypotInternal::B_HALF_ONE);
        Reg::Duplicate((Reg::RegTensor<uint16_t>&)vRegZero, 0);
    }

    for (uint16_t i = 0; i < repeatTimes; ++i) {
        maskReg = Reg::UpdateMask<T>(sreg);
        if constexpr ((IsSameType<T, bfloat16_t>::value) || (IsSameType<T, half>::value)) {
            Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(vSrcReg0, src0Ub + i * sregLower);
            Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(vSrcReg1, src1Ub + i * sregLower);
            Reg::MaskUnPack(maskReg, maskReg);
        } else {
            Reg::LoadAlign<T>(vSrcReg0, src0Ub + i * sregLower);
            Reg::LoadAlign<T>(vSrcReg1, src1Ub + i * sregLower);
        }

        Reg::Compare<T, CMPMODE::NE>(cmpMaskSrc0NAN, vSrcReg0, vSrcReg0, maskReg);
        Reg::Compare<T, CMPMODE::NE>(cmpMaskSrc1NAN, vSrcReg1, vSrcReg1, maskReg);
        if constexpr (IsSameType<T, float>::value) {
            CompareScalar<T, uint32_t>(
                cmpMaskZero, cmpMaskSrcInf, vSrcTmpReg0, vSrcTmpReg1, cmpMaskSrc0NAN, cmpMaskSrc1NAN, vRegOne, vSrcReg0,
                vSrcReg1, HypotInternal::INF, HypotInternal::NEG_INF, maskReg);
        } else if constexpr (IsSameType<T, half>::value) {
            CompareScalar<T, uint16_t>(
                cmpMaskZero, cmpMaskSrcInf, vSrcTmpReg0, vSrcTmpReg1, cmpMaskSrc0NAN, cmpMaskSrc1NAN, vRegOne, vSrcReg0,
                vSrcReg1, HypotInternal::HALF_INF, HypotInternal::HALF_NEG_INF, maskReg);
        } else if constexpr (IsSameType<T, bfloat16_t>::value) {
            CompareScalar<T, uint16_t>(
                cmpMaskZero, cmpMaskSrcInf, vSrcTmpReg0, vSrcTmpReg1, cmpMaskSrc0NAN, cmpMaskSrc1NAN, vRegOne, vSrcReg0,
                vSrcReg1, HypotInternal::B_HALF_INF, HypotInternal::B_HALF_NEG_INF, maskReg);
        }

        HypotCompute<T>(vSrcTmpReg0, vSrcTmpReg1, vDstReg0, maskReg);

        Reg::Select(vDstReg0, vRegZero, vDstReg0, cmpMaskZero);
        Reg::Select(vDstReg0, vSrcReg0, vDstReg0, cmpMaskSrc0NAN);
        Reg::Select(vDstReg0, vSrcReg1, vDstReg0, cmpMaskSrc1NAN);
        Reg::Select(vDstReg0, vRegInf, vDstReg0, cmpMaskSrcInf);
        if constexpr ((IsSameType<T, bfloat16_t>::value) || (IsSameType<T, half>::value)) {
            Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(dstUb + i * sregLower, vDstReg0, maskReg);
        } else {
            Reg::StoreAlign<T>(dstUb + i * sregLower, vDstReg0, maskReg);
        }
    }
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void HypotImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor,
    const uint32_t calCount)
{
    static_assert(
        SupportType<T, half, bfloat16_t, float>(),
        "Hypot only support half/bfloat16_t/float data type on current device!");
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }

    CheckTensorPos<T>(dstTensor, Hardware::UB, "dstTensor", "VECIN/VECCALC/VECOUT", "Hypot");
    CheckTensorPos<T>(src0Tensor, Hardware::UB, "src0Tensor", "VECIN/VECCALC/VECOUT", "Hypot");
    CheckTensorPos<T>(src1Tensor, Hardware::UB, "src1Tensor", "VECIN/VECCALC/VECOUT", "Hypot");
    ASCENDC_ASSERT((calCount <= src0Tensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should not be larger than src0Tensor length %u", calCount,
            src0Tensor.GetSize());
    });
    ASCENDC_ASSERT((calCount <= src1Tensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should not be larger than src1Tensor length %u", calCount,
            src1Tensor.GetSize());
    });
    ASCENDC_ASSERT((calCount <= dstTensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should not be larger than dstTensor length %u", calCount,
            dstTensor.GetSize());
    });

    __ubuf__ T* src0Ub = (__ubuf__ T*)src0Tensor.GetPhyAddr();
    __ubuf__ T* src1Ub = (__ubuf__ T*)src1Tensor.GetPhyAddr();
    __ubuf__ T* dstUb = (__ubuf__ T*)dstTensor.GetPhyAddr();

    VfHypotImpl<T>(dstUb, src0Ub, src1Ub, calCount);
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void HypotImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& src0Tensor, const LocalTensor<T>& src1Tensor,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    CheckTensorPos<uint8_t>(sharedTmpBuffer, Hardware::UB, "sharedTmpBuffer", "VECIN/VECCALC/VECOUT", "Hypot");

    HypotImpl(dstTensor, src0Tensor, src1Tensor, calCount);
}
} // namespace AscendC
#endif
#endif // IMPL_MATH_HYPOT_HYPOT_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_H__YPOT_HYPOT_COMMON_IMPL_H)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_H__YPOT_HYPOT_COMMON_IMPL_H
#endif
