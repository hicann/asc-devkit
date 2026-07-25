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
 * \file erfc_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/erfc/erfc_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/erfc.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ERFC_ERFC_C310_IMPL_H__
#endif
#ifndef IMPL_MATH_ERFC_ERFC_C310_IMPL_H
#define IMPL_MATH_ERFC_ERFC_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
namespace AscendC {
namespace ERFC {

constexpr Reg::CastTrait castTraitF162F32 = {
    Reg::RegLayout::ZERO, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::UNKNOWN};
constexpr Reg::CastTrait castTraitF322F16 = {
    Reg::RegLayout::ZERO, Reg::SatMode::NO_SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};

__simd_callee__ inline void MulAdds(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg1, Reg::RegTensor<float>& srcReg2,
    const float scalarValue, Reg::MaskReg& mask)
{
    // dst = src1 * src2 + scalarValue
    Reg::RegTensor<float> tmpReg;
    Reg::Mul(tmpReg, srcReg1, srcReg2, mask);
    Reg::Adds(dstReg, tmpReg, scalarValue, mask);
}

// compute Erfc with xa = |x| + fp32_min
__simd_callee__ inline void ErfcPreCompute(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::MaskReg mask)
{
    constexpr float SCALAR_ERFC_FP32_MIN = 2.168404344971009e-19; // 2^-62
    Reg::RegTensor<float> tmpReg;
    Reg::Abs(tmpReg, srcReg, mask);
    Reg::Adds(dstReg, tmpReg, SCALAR_ERFC_FP32_MIN, mask);
}

// compute Erfc R(z) = ((((((((z*r0 + r1)*z + r2)*z + r3)*z + r4)*z + r5)*z + r6)*z + r7)*z + r8)
__simd_callee__ inline void ErfcComputeR(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::MaskReg mask)
{
    // Specific value used for approximate calculation.
    constexpr float R0 = 0.1735313680e-7;
    constexpr float R1 = -0.9856738394e-6;
    constexpr float R2 = 0.2517003236e-4;
    constexpr float R3 = -0.3848015171e-3;
    constexpr float R4 = 0.5681528564e0;
    constexpr float R5 = 0.5245623129e1;
    constexpr float R6 = 0.2107740710e2;
    constexpr float R7 = 0.4212761755e2;
    constexpr float R8 = 0.4380524149e2;

    Reg::RegTensor<float> rReg;
    Reg::Duplicate(rReg, R0, mask);
    MulAdds(dstReg, srcReg, rReg, R1, mask);
    MulAdds(dstReg, dstReg, srcReg, R2, mask);
    MulAdds(dstReg, dstReg, srcReg, R3, mask);
    MulAdds(dstReg, dstReg, srcReg, R4, mask);
    MulAdds(dstReg, dstReg, srcReg, R5, mask);
    MulAdds(dstReg, dstReg, srcReg, R6, mask);
    MulAdds(dstReg, dstReg, srcReg, R7, mask);
    MulAdds(dstReg, dstReg, srcReg, R8, mask);
}

// compute Erfc S(z) = (((((z + s1)*z + s2)*z + s3)*z + s4)*z + s5)
__simd_callee__ inline void ErfcComputeS(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::MaskReg mask)
{
    // Specific value used for approximate calculation.
    constexpr float S1 = 0.9349684299e1;
    constexpr float S2 = 0.3756930664e2;
    constexpr float S3 = 0.8058268949e2;
    constexpr float S4 = 0.9155653738e2;
    constexpr float S5 = 0.4380524152e2;

    Reg::Adds(dstReg, srcReg, S1, mask);
    MulAdds(dstReg, dstReg, srcReg, S2, mask);
    MulAdds(dstReg, dstReg, srcReg, S3, mask);
    MulAdds(dstReg, dstReg, srcReg, S4, mask);
    MulAdds(dstReg, dstReg, srcReg, S5, mask);
}

__simd_callee__ inline void ErfcClip(Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::MaskReg mask)
{
    constexpr float ERFC_BOUNDARY_MAX = 10.0f;
    Reg::Mins(dstReg, srcReg, ERFC_BOUNDARY_MAX, mask);
    Reg::Maxs(dstReg, dstReg, -ERFC_BOUNDARY_MAX, mask);
}

// Compute Erfc: exp(-xa^2) * (R(z) / S(z)) * xb + (1 - xb)
__simd_callee__ inline void ErfcPublicSteps(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::MaskReg mask)
{
    constexpr float MIN_BOUNDARY = 10.0f;
    // Compute xa = |x| + min_pf32, exp(-xa^2)
    Reg::RegTensor<float> xaReg, xbReg, zReg, rzReg, szReg;
    Reg::RegTensor<float> oneReg, tmpReg;
    ErfcPreCompute(xaReg, srcReg, mask);
    Reg::Mul(tmpReg, xaReg, xaReg, mask);
    Reg::Neg(tmpReg, tmpReg, mask);
    Reg::Exp(dstReg, tmpReg, mask);
    // Compute z = min(xa, 10), xb = x / xa, exp(-xa^2) * xb
    Reg::Mins(zReg, xaReg, MIN_BOUNDARY, mask);
    Reg::Div(xbReg, srcReg, xaReg, mask);
    Reg::Mul(dstReg, dstReg, xbReg, mask);
    // Compute exp(-xa^2) * xb * (R(z) / S(z))
    ErfcComputeR(rzReg, zReg, mask);
    ErfcComputeS(szReg, zReg, mask);
    Reg::Mul(dstReg, dstReg, rzReg, mask);
    Reg::Div(dstReg, dstReg, szReg, mask);

    Reg::Duplicate(oneReg, 1.0f, mask);
    Reg::Sub(tmpReg, oneReg, xbReg, mask);
    Reg::Add(dstReg, dstReg, tmpReg, mask);
}

template <typename T>
__simd_vf__ inline void ErfcCoreImpl(__ubuf__ T* dstUb, __ubuf__ T* srcUb, uint32_t calCount, uint16_t repeatTimes)
{
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<float> castReg;
    Reg::RegTensor<float> tmpReg;
    Reg::RegTensor<float> dstReg;

    for (uint16_t i = 0; i < repeatTimes; ++i) {
        Reg::MaskReg mask = Reg::UpdateMask<float>(calCount);
        if constexpr (sizeof(T) == sizeof(half)) {
            Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(srcReg, srcUb + i * B32_DATA_NUM_PER_REPEAT);
            Reg::Cast<float, T, castTraitF162F32>(castReg, srcReg, mask);
        } else {
            Reg::LoadAlign(castReg, srcUb + i * B32_DATA_NUM_PER_REPEAT);
        }
        ErfcClip(tmpReg, castReg, mask);
        ErfcPublicSteps(dstReg, tmpReg, mask);
        if constexpr (sizeof(T) == sizeof(half)) {
            Reg::Cast<T, float, castTraitF322F16>(srcReg, dstReg, mask);
            Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(dstUb + i * B32_DATA_NUM_PER_REPEAT, srcReg, mask);
        } else {
            Reg::StoreAlign(dstUb + i * B32_DATA_NUM_PER_REPEAT, dstReg, mask);
        }
    }
}

} // namespace ERFC

template <typename T, bool isReuseSource = false>
__aicore__ inline void ErfcImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
    CheckTensorPos(sharedTmpBuffer, Hardware::UB, "sharedTmpBuffer", "VECIN/VECOUT/VECCALC", "Erfc");
    ErfcImpl(dstTensor, srcTensor, calCount);
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void ErfcImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }

    static_assert(SupportType<T, half, float>(), "current data type is not supported on current device!");
    CheckTensorPos<T>(dstTensor, Hardware::UB, "dstTensor", "VECIN/VECCALC/VECOUT", "ERFC");
    CheckTensorPos<T>(srcTensor, Hardware::UB, "srcTensor", "VECIN/VECCALC/VECOUT", "ERFC");
    ASCENDC_ASSERT((calCount <= srcTensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should not be larger than srcTensor length %u", calCount,
            srcTensor.GetSize());
    });
    ASCENDC_ASSERT((calCount <= dstTensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should not be larger than dstTensor length %u", calCount,
            dstTensor.GetSize());
    });

    __ubuf__ T* dstUb = (__ubuf__ T*)dstTensor.GetPhyAddr();
    __ubuf__ T* srcUb = (__ubuf__ T*)srcTensor.GetPhyAddr();
    uint16_t repeatTimes = CeilDivision(calCount, B32_DATA_NUM_PER_REPEAT);
    ERFC::ErfcCoreImpl<T>(dstUb, srcUb, calCount, repeatTimes);
}
} // namespace AscendC

#endif // IMPL_MATH_ERFC_ERFC_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ERFC_ERFC_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_ERFC_ERFC_C310_IMPL_H__
#endif
