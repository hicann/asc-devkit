/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file ndtri_3510_impl.h
 * \brief Ndtri implementation for NPU architecture 3510.
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/experimental/math/ndtri/ndtri_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/experimental/ndtri.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_EXPERIMENTAL_DETAIL_MATH_NDTRI_NDTRI_3510_IMPL_H__
#endif

#ifndef IMPL_EXPERIMENTAL_DETAIL_MATH_NDTRI_NDTRI_3510_IMPL_H
#define IMPL_EXPERIMENTAL_DETAIL_MATH_NDTRI_NDTRI_3510_IMPL_H

#include "basic_api/kernel_basic_intf.h"
#include "basic_api/kernel_tensor.h"
#include "impl/adv_api/detail/common/check.h"
#ifdef ASCENDC_CPU_DEBUG
#include "impl/adv_api/detail/experimental/api_check/kernel_check/math/ndtri/ndtri_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "impl/adv_api/detail/api_check/kernel_api_check.h"

namespace AscendC {
namespace experimental {
namespace NdtriInternal {

union FloatU32Union {
    constexpr __aicore__ FloatU32Union(uint32_t value) : u32(value) {}
    float f32;
    uint32_t u32;
};

constexpr FloatU32Union positiveInfinity(0x7f800000U);
constexpr FloatU32Union negativeInfinity(0xff800000U);
constexpr FloatU32Union quietNan(0x7fc00000U);
constexpr float centerRegionBoundary = 0.425f;
constexpr float nearTailBoundary = 5.0f;
constexpr float tailProbabilityScale = 1.8446744073709552e19f;
constexpr float tailLogScaleOffset = -44.3614195558364998f;

__simd_callee__ inline void MulAdds(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& lhsReg, Reg::RegTensor<float>& rhsReg, const float scalar,
    Reg::MaskReg mask)
{
    Reg::Mul(dstReg, lhsReg, rhsReg, mask);
    Reg::Adds(dstReg, dstReg, scalar, mask);
}

__simd_callee__ inline void ComputeCenterSeriesPart1(
    Reg::RegTensor<float>& seriesReg, Reg::RegTensor<float>& xSquaredReg, Reg::MaskReg mask)
{
    constexpr float c12 = 0.020606780424714088f;
    constexpr float c13 = 0.018918218091130257f;
    constexpr float c14 = 0.017476370558142662f;
    constexpr float c15 = 0.01623150147497654f;
    constexpr float c16 = 0.015146315097808838f;
    constexpr float c17 = 0.014192315749824047f;
    constexpr float c18 = 0.013347364030778408f;
    constexpr float c19 = 0.012594005092978477f;
    constexpr float c20 = 0.011918296106159687f;
    constexpr float c21 = 0.011308969929814339f;
    constexpr float c22 = 0.010756825096905231f;
    constexpr float c23 = 0.010254274122416973f;
    Reg::Duplicate(seriesReg, c23, mask);
    MulAdds(seriesReg, seriesReg, xSquaredReg, c22, mask);
    MulAdds(seriesReg, seriesReg, xSquaredReg, c21, mask);
    MulAdds(seriesReg, seriesReg, xSquaredReg, c20, mask);
    MulAdds(seriesReg, seriesReg, xSquaredReg, c19, mask);
    MulAdds(seriesReg, seriesReg, xSquaredReg, c18, mask);
    MulAdds(seriesReg, seriesReg, xSquaredReg, c17, mask);
    MulAdds(seriesReg, seriesReg, xSquaredReg, c16, mask);
    MulAdds(seriesReg, seriesReg, xSquaredReg, c15, mask);
    MulAdds(seriesReg, seriesReg, xSquaredReg, c14, mask);
    MulAdds(seriesReg, seriesReg, xSquaredReg, c13, mask);
    MulAdds(seriesReg, seriesReg, xSquaredReg, c12, mask);
}

__simd_callee__ inline void ComputeCenterSeriesPart2(
    Reg::RegTensor<float>& seriesReg, Reg::RegTensor<float>& xSquaredReg, Reg::MaskReg mask)
{
    constexpr float c0 = 0.88622695207595825f;
    constexpr float c1 = 0.23201367259025574f;
    constexpr float c2 = 0.12755617499351501f;
    constexpr float c3 = 0.086552128195762634f;
    constexpr float c4 = 0.064959615468978882f;
    constexpr float c5 = 0.051731280982494354f;
    constexpr float c6 = 0.042836721986532211f;
    constexpr float c7 = 0.036465927958488464f;
    constexpr float c8 = 0.031689006835222244f;
    constexpr float c9 = 0.027980633080005646f;
    constexpr float c10 = 0.025022275745868683f;
    constexpr float c11 = 0.022609863430261612f;

    MulAdds(seriesReg, seriesReg, xSquaredReg, c11, mask);
    MulAdds(seriesReg, seriesReg, xSquaredReg, c10, mask);
    MulAdds(seriesReg, seriesReg, xSquaredReg, c9, mask);
    MulAdds(seriesReg, seriesReg, xSquaredReg, c8, mask);
    MulAdds(seriesReg, seriesReg, xSquaredReg, c7, mask);
    MulAdds(seriesReg, seriesReg, xSquaredReg, c6, mask);
    MulAdds(seriesReg, seriesReg, xSquaredReg, c5, mask);
    MulAdds(seriesReg, seriesReg, xSquaredReg, c4, mask);
    MulAdds(seriesReg, seriesReg, xSquaredReg, c3, mask);
    MulAdds(seriesReg, seriesReg, xSquaredReg, c2, mask);
    MulAdds(seriesReg, seriesReg, xSquaredReg, c1, mask);
    MulAdds(seriesReg, seriesReg, xSquaredReg, c0, mask);
}

__simd_callee__ inline void ComputeCenterSeries(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& qReg, Reg::MaskReg mask)
{
    constexpr float sqrtTwo = 1.4142135623730951f;
    Reg::RegTensor<float> xReg;
    Reg::RegTensor<float> xSquaredReg;
    Reg::RegTensor<float> seriesReg;
    Reg::Muls(xReg, qReg, 2.0f, mask);
    Reg::Mul(xSquaredReg, xReg, xReg, mask);
    ComputeCenterSeriesPart1(seriesReg, xSquaredReg, mask);
    ComputeCenterSeriesPart2(seriesReg, xSquaredReg, mask);

    Reg::Mul(dstReg, xReg, seriesReg, mask);
    Reg::Muls(dstReg, dstReg, sqrtTwo, mask);
}

// Rational approximation for the near tail region.
__simd_callee__ inline void ComputeNearTail(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& rReg, Reg::MaskReg mask)
{
    constexpr float c0 = 1.42343711074968357734f;
    constexpr float c1 = 4.63033784615654529590f;
    constexpr float c2 = 5.76949722146069140550f;
    constexpr float c3 = 3.64784832476320460504f;
    constexpr float c4 = 1.27045825245236838258f;
    constexpr float c5 = 0.241780725177450611770f;
    constexpr float c6 = 0.0227238449892691845833f;
    constexpr float c7 = 0.000774545014278341407640f;
    constexpr float d0 = 1.0f;
    constexpr float d1 = 2.05319162663775882187f;
    constexpr float d2 = 1.67638483018380384940f;
    constexpr float d3 = 0.689767334985100004550f;
    constexpr float d4 = 0.148103976427480074590f;
    constexpr float d5 = 0.0151986665636164571966f;
    constexpr float d6 = 0.000547593808499534494600f;
    constexpr float d7 = 1.05075007164441684324e-9f;

    Reg::RegTensor<float> zReg;
    Reg::RegTensor<float> numeratorReg;
    Reg::RegTensor<float> denominatorReg;
    Reg::Adds(zReg, rReg, -1.6f, mask);

    Reg::Duplicate(numeratorReg, c7, mask);
    MulAdds(numeratorReg, numeratorReg, zReg, c6, mask);
    MulAdds(numeratorReg, numeratorReg, zReg, c5, mask);
    MulAdds(numeratorReg, numeratorReg, zReg, c4, mask);
    MulAdds(numeratorReg, numeratorReg, zReg, c3, mask);
    MulAdds(numeratorReg, numeratorReg, zReg, c2, mask);
    MulAdds(numeratorReg, numeratorReg, zReg, c1, mask);
    MulAdds(numeratorReg, numeratorReg, zReg, c0, mask);

    Reg::Duplicate(denominatorReg, d7, mask);
    MulAdds(denominatorReg, denominatorReg, zReg, d6, mask);
    MulAdds(denominatorReg, denominatorReg, zReg, d5, mask);
    MulAdds(denominatorReg, denominatorReg, zReg, d4, mask);
    MulAdds(denominatorReg, denominatorReg, zReg, d3, mask);
    MulAdds(denominatorReg, denominatorReg, zReg, d2, mask);
    MulAdds(denominatorReg, denominatorReg, zReg, d1, mask);
    MulAdds(denominatorReg, denominatorReg, zReg, d0, mask);
    Reg::Div(dstReg, numeratorReg, denominatorReg, mask);
}

// Rational approximation for the far tail region.
__simd_callee__ inline void ComputeFarTail(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& rReg, Reg::MaskReg mask)
{
    constexpr float e0 = 6.65790464350110377720f;
    constexpr float e1 = 5.46378491116411436990f;
    constexpr float e2 = 1.78482653991729133580f;
    constexpr float e3 = 0.29656057182850489123f;
    constexpr float e4 = 0.026532189526576123093f;
    constexpr float e5 = 0.00124266094738807843860f;
    constexpr float e6 = 2.71155556874348757815e-5f;
    constexpr float e7 = 2.01033439929228813265e-7f;
    constexpr float f0 = 1.0f;
    constexpr float f1 = 0.599832206555887937690f;
    constexpr float f2 = 0.136929880922735805310f;
    constexpr float f3 = 0.0148753612908506148525f;
    constexpr float f4 = 0.00078686913114561325910f;
    constexpr float f5 = 1.84631831751005468180e-5f;
    constexpr float f6 = 1.42151175831644588870e-7f;
    constexpr float f7 = 2.04426310338993978564e-15f;

    Reg::RegTensor<float> zReg;
    Reg::RegTensor<float> numeratorReg;
    Reg::RegTensor<float> denominatorReg;
    Reg::Adds(zReg, rReg, -5.0f, mask);

    Reg::Duplicate(numeratorReg, e7, mask);
    MulAdds(numeratorReg, numeratorReg, zReg, e6, mask);
    MulAdds(numeratorReg, numeratorReg, zReg, e5, mask);
    MulAdds(numeratorReg, numeratorReg, zReg, e4, mask);
    MulAdds(numeratorReg, numeratorReg, zReg, e3, mask);
    MulAdds(numeratorReg, numeratorReg, zReg, e2, mask);
    MulAdds(numeratorReg, numeratorReg, zReg, e1, mask);
    MulAdds(numeratorReg, numeratorReg, zReg, e0, mask);

    Reg::Duplicate(denominatorReg, f7, mask);
    MulAdds(denominatorReg, denominatorReg, zReg, f6, mask);
    MulAdds(denominatorReg, denominatorReg, zReg, f5, mask);
    MulAdds(denominatorReg, denominatorReg, zReg, f4, mask);
    MulAdds(denominatorReg, denominatorReg, zReg, f3, mask);
    MulAdds(denominatorReg, denominatorReg, zReg, f2, mask);
    MulAdds(denominatorReg, denominatorReg, zReg, f1, mask);
    MulAdds(denominatorReg, denominatorReg, zReg, f0, mask);
    Reg::Div(dstReg, numeratorReg, denominatorReg, mask);
}

__simd_callee__ inline void ComputeTail(
    Reg::RegTensor<float>& tailReg, Reg::RegTensor<float>& srcReg, Reg::RegTensor<float>& qReg, Reg::MaskReg mask)
{
    Reg::RegTensor<float> tailProbabilityReg;
    Reg::RegTensor<float> logInputReg;
    Reg::RegTensor<float> tailRadiusReg;
    Reg::RegTensor<float> nearTailReg;
    Reg::RegTensor<float> farTailReg;
    Reg::RegTensor<float> tmpReg;
    Reg::MaskReg nearTailMask;
    Reg::MaskReg negativeMask;

    Reg::Muls(tmpReg, srcReg, -1.0f, mask);
    Reg::Adds(tmpReg, tmpReg, 1.0f, mask);
    Reg::Min(tailProbabilityReg, srcReg, tmpReg, mask);
    // Scale before Ln so float32 subnormal probabilities remain representable. Restore the log scale afterwards.
    Reg::Muls(logInputReg, tailProbabilityReg, tailProbabilityScale, mask);
    Reg::Ln(tailRadiusReg, logInputReg, mask);
    Reg::Adds(tailRadiusReg, tailRadiusReg, tailLogScaleOffset, mask);
    Reg::Neg(tailRadiusReg, tailRadiusReg, mask);
    Reg::Sqrt(tailRadiusReg, tailRadiusReg, mask);
    ComputeNearTail(nearTailReg, tailRadiusReg, mask);
    ComputeFarTail(farTailReg, tailRadiusReg, mask);

    Reg::CompareScalar<float, CMPMODE::LE>(nearTailMask, tailRadiusReg, nearTailBoundary, mask);
    Reg::Select(tailReg, nearTailReg, farTailReg, nearTailMask);
    Reg::CompareScalar<float, CMPMODE::LT>(negativeMask, qReg, 0.0f, mask);
    Reg::Neg(tmpReg, tailReg, mask);
    Reg::Select(tailReg, tmpReg, tailReg, negativeMask);
}

__simd_callee__ inline void ComputeSpecialValues(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::MaskReg mask)
{
    Reg::RegTensor<float> positiveInfReg;
    Reg::RegTensor<float> negativeInfReg;
    Reg::RegTensor<float> nanReg;
    Reg::MaskReg nearTailMask;
    Reg::MaskReg specialMask;
    Reg::MaskReg nanMask;

    Reg::Duplicate(positiveInfReg, positiveInfinity.f32, mask);
    Reg::Duplicate(negativeInfReg, negativeInfinity.f32, mask);
    Reg::Duplicate(nanReg, quietNan.f32, mask);

    Reg::CompareScalar<float, CMPMODE::EQ>(specialMask, srcReg, 0.0f, mask);
    Reg::Select(dstReg, negativeInfReg, dstReg, specialMask);
    Reg::CompareScalar<float, CMPMODE::EQ>(specialMask, srcReg, 1.0f, mask);
    Reg::Select(dstReg, positiveInfReg, dstReg, specialMask);

    Reg::CompareScalar<float, CMPMODE::LT>(specialMask, srcReg, 0.0f, mask);
    Reg::CompareScalar<float, CMPMODE::GT>(nearTailMask, srcReg, 1.0f, mask);
    Reg::MaskOr(specialMask, specialMask, nearTailMask, mask);
    Reg::Compare<float, CMPMODE::NE>(nanMask, srcReg, srcReg, mask);
    Reg::MaskOr(specialMask, specialMask, nanMask, mask);
    Reg::Select(dstReg, nanReg, dstReg, specialMask);
}

__simd_callee__ inline void Compute(Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::MaskReg mask)
{
    Reg::RegTensor<float> qReg;
    Reg::RegTensor<float> absQReg;
    Reg::RegTensor<float> centralReg;
    Reg::RegTensor<float> tailReg;
    Reg::MaskReg centerMask;

    Reg::Adds(qReg, srcReg, -0.5f, mask);
    Reg::Abs(absQReg, qReg, mask);
    ComputeCenterSeries(centralReg, qReg, mask);
    ComputeTail(tailReg, srcReg, qReg, mask);

    Reg::CompareScalar<float, CMPMODE::LE>(centerMask, absQReg, centerRegionBoundary, mask);
    Reg::Select(dstReg, centralReg, tailReg, centerMask);
    ComputeSpecialValues(dstReg, srcReg, mask);
}

template <bool isReuseSource = false>
__simd_vf__ inline void NdtriImpl(__ubuf__ float* dstUb, __ubuf__ float* srcUb, uint32_t calCount)
{
    constexpr uint32_t elementsPerRepeat = static_cast<uint32_t>(GetVecLen() / sizeof(float));
    const uint32_t repeatTimes = CeilDivision(calCount, elementsPerRepeat);
    Reg::RegTensor<float> dstReg;
    Reg::RegTensor<float> srcReg;
    for (uint32_t i = 0; i < repeatTimes; ++i) {
        Reg::MaskReg mask = Reg::UpdateMask<float>(calCount);
        Reg::LoadAlign(srcReg, srcUb + i * elementsPerRepeat);
        Compute(dstReg, srcReg, mask);
        Reg::StoreAlign(dstUb + i * elementsPerRepeat, dstReg, mask);
    }
}
} // namespace NdtriInternal

template <typename T, bool isReuseSource = false>
__aicore__ inline void NdtriImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
    if ASCEND_IS_AIC {
        return;
    }
    CHECK_FUNC_HIGHLEVEL_API(Ndtri, (T, isReuseSource), (dstTensor, srcTensor, calCount));
    static_assert(SupportType<T, float>(), "Ndtri only supports float data type on current device!");
    CheckTensorPosition(dstTensor, "dstTensor", "VECIN, VECOUT, VECCALC");
    CheckTensorPosition(srcTensor, "srcTensor", "VECIN, VECOUT, VECCALC");
    CheckCalCount(calCount, "calCount", srcTensor, "srcTensor", "Ndtri");
    CheckCalCount(calCount, "calCount", dstTensor, "dstTensor", "Ndtri");
    if (calCount == 0) {
        return;
    }

    __ubuf__ float* dstUb = (__ubuf__ float*)dstTensor.GetPhyAddr();
    __ubuf__ float* srcUb = (__ubuf__ float*)srcTensor.GetPhyAddr();
    NdtriInternal::NdtriImpl<isReuseSource>(dstUb, srcUb, calCount);
}

} // namespace experimental
} // namespace AscendC

#endif // IMPL_EXPERIMENTAL_DETAIL_MATH_NDTRI_NDTRI_3510_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_EXPERIMENTAL_DETAIL_MATH_NDTRI_NDTRI_3510_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_EXPERIMENTAL_DETAIL_MATH_NDTRI_NDTRI_3510_IMPL_H__
#endif
