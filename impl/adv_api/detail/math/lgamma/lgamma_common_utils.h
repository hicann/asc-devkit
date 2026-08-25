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
 * \file lgamma_common_utils.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/lgamma/lgamma_common_utils.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/lgamma.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_LGAMMA_LGAMMA_COMMON_UTILS_H__
#endif
#ifndef IMPL_MATH_LGAMMA_LGAMMA_COMMON_UTILS_H
#define IMPL_MATH_LGAMMA_LGAMMA_COMMON_UTILS_H
#include "../../../../../include/basic_api/kernel_basic_intf.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510 || \
                              __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
namespace AscendC {
namespace {
constexpr float f05 = 0.5;
constexpr float f07 = 0.7;
constexpr float f15 = 1.5;
constexpr float f58 = 5.8;
constexpr float f1 = 1.0;
constexpr float f2 = 2.0;
constexpr float f3 = 3.0;
constexpr float fn05 = -0.5;
constexpr float fn1 = -1.0;
constexpr float fn2 = -2.0;
constexpr float fn3 = -3.0;
constexpr float fPi = 3.1415927410125732421875;
constexpr uint32_t FLOAT_NOREUSE_CALC_PROC = 8;
constexpr uint32_t FLOAT_REUSE_CALC_PROC = 7;
constexpr uint32_t LGAMMA_HALF_CALC_PROCEDURE = 13;
constexpr uint32_t i2 = 2;
constexpr uint32_t i4 = 4;
constexpr uint32_t i6 = 6;
constexpr uint32_t i7 = 7;
constexpr uint32_t i16 = 16;
constexpr float PI = 3.14159265358979323846264338327950288;
constexpr float t0 = 0.0f;
constexpr float t4 = 4.0f;
constexpr float t5 = 5.0f;
constexpr float t01 = 0.1f;
constexpr float t12 = 12.0f;
constexpr float N01 = -0.1f;

constexpr size_t params007Len = 7U;
constexpr float params007[params007Len] = {0.00358751555905, -0.00547128543258, -0.0446271263063, 0.167317703366,
                                           -0.0421359799802, -0.655867278576,   0.577215373516};

constexpr size_t params0715Len = 11U;
constexpr float params0715[params0715Len] = {0.0458826646209, 0.103739671409, 0.122803635895, 0.127524212003,
                                             0.143216684461,  0.169343575835, 0.207407936454, 0.27058750391,
                                             0.400685429573,  0.82246696949,  0.577215671539};

constexpr size_t params153Len = 10U;
constexpr float params153[params153Len] = {4.95984932058e-05, -0.000220894842641, 0.000541314249858, -0.00120451697148,
                                           0.00288425176404,  -0.00738275796175,  0.0205813199282,   -0.067352488637,
                                           0.322467029095,    0.422784328461};

constexpr size_t params378X1Len = 4U;
constexpr size_t params378X2Len = 3U;
constexpr float params378X1[params378X1Len] = {-748.890319824, -12349.7421875, -41061.375, -48310.6640625};
constexpr float params378X2[params378X2Len] = {-259.250976562, -10777.1796875, -92685.046875};

constexpr size_t params58Len = 2U;
constexpr float params58[params58Len] = {0.000777830660809, -0.00277765537612};

constexpr size_t negParamsOddLen = 4U;
constexpr float negParamsOdd[negParamsOddLen] = {
    0.00002427957952022552490234375, -0.001388786011375486850738525390625, 0.0416667275130748748779296875,
    -0.4999999701976776123046875};
constexpr size_t negParamsEvenLen = 3U;
constexpr float negParamsEven[negParamsEvenLen] = {
    -0.000195746586541645228862762451171875, 0.0083327032625675201416015625, -0.16666662693023681640625};
} // namespace

struct LGammaParams {
    __aicore__ LGammaParams() {}
    LocalTensor<float> tmp1;
    LocalTensor<float> tmp2;
    LocalTensor<float> tmp3;
    LocalTensor<float> tmp4;
    LocalTensor<float> tmp5;
    LocalTensor<float> tmp6;
    LocalTensor<float> tmpScalar;
    LocalTensor<uint8_t> mask;
    LocalTensor<uint8_t> tmpMask1;
    LocalTensor<uint8_t> tmpMask2;
    LocalTensor<uint8_t> tmpMask3;
    UnaryRepeatParams unaryParams;
    BinaryRepeatParams binaryParams;
    uint32_t splitSize;
};

#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
namespace LgammaInternal {
constexpr Reg::CastTrait LGAMMA_CAST_TRAIT_F162F32 = {
    Reg::RegLayout::ZERO, Reg::SatMode::UNKNOWN, Reg::MaskMergeMode::ZEROING, RoundMode::UNKNOWN};
constexpr Reg::CastTrait LGAMMA_CAST_TRAIT_F322F16 = {
    Reg::RegLayout::ZERO, Reg::SatMode::SAT, Reg::MaskMergeMode::ZEROING, RoundMode::CAST_RINT};
constexpr uint32_t POW_70 = 0x1c800000;
constexpr uint32_t NEG_POW_70 = 0x9c800000;
constexpr uint32_t F023 = 0x3e6d3308;
constexpr uint32_t F07 = 0x3f333333;
constexpr uint32_t F123 = 0x3f9da620;
constexpr uint32_t F173 = 0x3fdda618;
constexpr uint32_t F22 = 0x400ccccd;
constexpr uint32_t F25 = 0x40200000;
constexpr uint32_t POW_58 = 0x5c800000;
constexpr uint32_t F32_POS_INF = 0x7f800000;
constexpr uint32_t F32_NEG_INF = 0xff800000;
constexpr uint32_t F32_NAN = 0x7fc00000;
constexpr uint32_t MANTISSA = 23;
constexpr uint32_t SCALAR0 = 0xff;
constexpr uint32_t SCALAR1 = 0x7fffff;
constexpr float F4 = 4.0;
constexpr float F5 = 5.0;
constexpr float F6 = 6.0;
constexpr float F7 = 7.0;
constexpr float F8 = 8.0;
struct MulAddsParams {
    float r0;
    float r1;
    float r2;
    float r3;
    float r4;
    __aicore__ constexpr MulAddsParams(
        const float r0In, const float r1In, const float r2In, const float r3In, const float r4In)
        : r0(r0In), r1(r1In), r2(r2In), r3(r3In), r4(r4In)
    {}
};

__aicore__ inline constexpr MulAddsParams GetConstantsPow70To023()
{
    constexpr float r4 = 6.32827064025093366517e-01;
    constexpr float r3 = 1.45492250137234768737e+00;
    constexpr float r2 = 9.77717527963372745603e-01;
    constexpr float r1 = 2.28963728064692451092e-01;
    constexpr float r0 = 1.33810918536787660377e-02;

    constexpr MulAddsParams params(r0, r1, r2, r3, r4);
    return params;
}

__aicore__ inline constexpr MulAddsParams GetConstantsPow70To023V2()
{
    constexpr float r4 = 2.45597793713041134822e+00;
    constexpr float r3 = 2.12848976379893395361e+00;
    constexpr float r2 = 7.69285150456672783825e-01;
    constexpr float r1 = 1.04222645593369134254e-01;
    constexpr float r0 = 3.21709242282423911810e-03;

    constexpr MulAddsParams params(r0, r1, r2, r3, r4);
    return params;
}

__aicore__ inline constexpr MulAddsParams GetConstants123To173()
{
    constexpr float r4 = 5.73642027e-07;
    constexpr float r3 = -1.47723704e-06;
    constexpr float r2 = 1.45738871e-06;
    constexpr float r1 = -6.47978282e-07;
    constexpr float r0 = 1.08704044e-07;

    constexpr MulAddsParams params(r0, r1, r2, r3, r4);
    return params;
}

__aicore__ inline constexpr MulAddsParams GetConstants123To173V2()
{
    constexpr float r4 = 4.83836122723810047042e-01;
    constexpr float r3 = -3.27885410759859649565e-02;
    constexpr float r2 = 6.10053870246291332635e-03;
    constexpr float r1 = -1.40346469989232843813e-03;
    constexpr float r0 = 3.15632070903625950361e-04;

    constexpr MulAddsParams params(r0, r1, r2, r3, r4);
    return params;
}

__aicore__ inline constexpr MulAddsParams GetConstants123To173V3()
{
    constexpr float r4 = -1.47587722994593911752e-01;
    constexpr float r3 = 1.79706750811820387126e-02;
    constexpr float r2 = -3.68452016781138256760e-03;
    constexpr float r1 = 8.81081882437654011382e-04;
    constexpr float r0 = -3.12754168375120860518e-04;

    constexpr MulAddsParams params(r0, r1, r2, r3, r4);
    return params;
}

__aicore__ inline constexpr MulAddsParams GetConstants123To173V4()
{
    constexpr float r4 = 6.46249402391333854778e-02;
    constexpr float r3 = -1.03142241298341437450e-02;
    constexpr float r2 = 2.25964780900612472250e-03;
    constexpr float r1 = -5.38595305356740546715e-04;
    constexpr float r0 = 3.35529192635519073543e-04;

    constexpr MulAddsParams params(r0, r1, r2, r3, r4);
    return params;
}

__aicore__ inline constexpr MulAddsParams GetConstants3To8()
{
    constexpr float r4 = 3.25778796408930981787e-01;
    constexpr float r3 = 1.46350472652464452805e-01;
    constexpr float r2 = 2.66422703033638609560e-02;
    constexpr float r1 = 1.84028451407337715652e-03;
    constexpr float r0 = 3.19475326584100867617e-05;

    constexpr MulAddsParams params(r0, r1, r2, r3, r4);
    return params;
}

__aicore__ inline constexpr MulAddsParams GetConstants3To8V2()
{
    constexpr float r4 = 7.21935547567138069525e-01;
    constexpr float r3 = 1.71933865632803078993e-01;
    constexpr float r2 = 1.86459191715652901344e-02;
    constexpr float r1 = 7.77942496381893596434e-04;
    constexpr float r0 = 7.32668430744625636189e-06;

    constexpr MulAddsParams params(r0, r1, r2, r3, r4);
    return params;
}

__aicore__ inline constexpr MulAddsParams GetConstants8ToPow58()
{
    constexpr float r4 = -2.77777777728775536470e-03;
    constexpr float r3 = 7.93650558643019558500e-04;
    constexpr float r2 = -5.95187557450339963135e-04;
    constexpr float r1 = 8.36339918996282139126e-04;
    constexpr float r0 = -1.63092934096575273989e-03;

    constexpr MulAddsParams params(r0, r1, r2, r3, r4);
    return params;
}

__simd_callee__ inline void LGammaCalcMulAdd(
    Reg::RegTensor<float>& resReg, Reg::RegTensor<float>& tmpReg, Reg::MaskReg mask, const MulAddsParams& params)
{
    Reg::Muls(resReg, tmpReg, params.r0, mask);
    Reg::Adds(resReg, resReg, params.r1, mask);
    Reg::Mul(resReg, resReg, tmpReg, mask);
    Reg::Adds(resReg, resReg, params.r2, mask);
    Reg::Mul(resReg, resReg, tmpReg, mask);
    Reg::Adds(resReg, resReg, params.r3, mask);
    Reg::Mul(resReg, resReg, tmpReg, mask);
    Reg::Adds(resReg, resReg, params.r4, mask);
}

template <typename T>
__simd_callee__ inline void LgammaLoadData(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<T> srcReg, __ubuf__ T* srcUb, Reg::MaskReg& mask)
{
    if constexpr (IsSameType<T, half>::value) {
        Reg::LoadAlign<T, Reg::LoadDist::DIST_UNPACK_B16>(srcReg, srcUb);
        Reg::Cast<float, T, LGAMMA_CAST_TRAIT_F162F32>(dstReg, srcReg, mask);
    } else {
        Reg::LoadAlign<T, Reg::LoadDist::DIST_NORM>(dstReg, srcUb);
    }
}
} // namespace LgammaInternal
#endif
} // namespace AscendC
#endif
#endif // IMPL_MATH_LGAMMA_LGAMMA_COMMON_UTILS_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_LGAMMA_LGAMMA_COMMON_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_LGAMMA_LGAMMA_COMMON_UTILS_H__
#endif
