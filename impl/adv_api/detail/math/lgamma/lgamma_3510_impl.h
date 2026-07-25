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
 * \file lgamma_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/lgamma/lgamma_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/lgamma.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_LGAMMA_LGAMMA_C310_IMPL_H__
#endif
#ifndef IMPL_MATH_LGAMMA_LGAMMA_C310_IMPL_H
#define IMPL_MATH_LGAMMA_LGAMMA_C310_IMPL_H
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "lgamma_common_utils.h"
#include "../../common/check.h"

namespace AscendC {
namespace LgammaInternal {
__simd_callee__ inline void LgammaCalPow70To023(
    Reg::RegTensor<float>& resReg, Reg::RegTensor<float>& tmpReg, Reg::MaskReg mask)
{
    constexpr float r0 = -7.72156649015328655494e-02;
    Reg::RegTensor<float> aReg;
    Reg::RegTensor<float> bReg;

    constexpr MulAddsParams params0 = GetConstantsPow70To023();
    // y = (((((r5 * x + r4) * x + r3) * x + r2) * x + r1) * x + r0) * x
    LGammaCalcMulAdd(aReg, tmpReg, mask, params0);
    Reg::Mul(aReg, aReg, tmpReg, mask);
    Reg::Adds(aReg, aReg, r0, mask);
    Reg::Mul(aReg, aReg, tmpReg, mask);

    constexpr MulAddsParams params1 = GetConstantsPow70To023V2();
    // y = (((((v5 * x + v4) * x + v3) * x + v2) * x + v1) * x + 1
    LGammaCalcMulAdd(bReg, tmpReg, mask, params1);
    Reg::Mul(bReg, bReg, tmpReg, mask);
    Reg::Adds(bReg, bReg, f1, mask);

    Reg::Div(resReg, aReg, bReg, mask);
    Reg::Muls(aReg, tmpReg, fn05, mask);
    Reg::Add(resReg, resReg, aReg, mask);
}

__simd_callee__ inline void LgammaCal023To073(
    Reg::RegTensor<float>& resReg, Reg::RegTensor<float>& srcReg, Reg::RegTensor<float>& tmpReg, Reg::MaskReg mask)
{
    constexpr float r0 = -1.0348905219959115;
    constexpr float r1 = -0.3808607740435038;
    constexpr float r2 = 0.10589227895856956;
    constexpr float r3 = 0.20700451798849123;
    constexpr float r4 = -1.7736742652812825;
    constexpr float r5 = -0.577911391162728;
    constexpr float r6 = -0.3713263607336305;
    constexpr float r7 = 1.8253839945289898;

    Reg::RegTensor<float> aReg;
    Reg::RegTensor<float> bReg;
    Reg::RegTensor<float> cReg;
    Reg::RegTensor<float> dReg;
    Reg::RegTensor<float> eReg;
    Reg::RegTensor<float> fReg;

    // a = x + r0
    Reg::Adds(aReg, srcReg, r0, mask);
    // b = r3 / x
    Reg::Duplicate(tmpReg, r3, mask);
    Reg::Div(bReg, tmpReg, srcReg, mask);
    // c = b + r1
    Reg::Adds(cReg, bReg, r1, mask);
    // d = r6 - b
    Reg::Duplicate(tmpReg, r6, mask);
    Reg::Sub(dReg, tmpReg, bReg, mask);
    // e = b + r7
    Reg::Adds(eReg, bReg, r7, mask);
    // f = r5 / d
    Reg::Duplicate(tmpReg, r5, mask);
    Reg::Div(fReg, tmpReg, dReg, mask);
    // res = f - e
    Reg::Sub(resReg, fReg, eReg, mask);
    // res = r4 / res
    Reg::Duplicate(tmpReg, r4, mask);
    Reg::Div(resReg, tmpReg, resReg, mask);
    // res = res + f
    Reg::Add(resReg, resReg, fReg, mask);
    // res = res * a
    Reg::Mul(resReg, resReg, aReg, mask);
    // res = r2 - res
    Reg::Duplicate(tmpReg, r2, mask);
    Reg::Sub(resReg, tmpReg, resReg, mask);
    // res = res * c
    Reg::Mul(resReg, resReg, cReg, mask);
    // res = res - a
    Reg::Sub(resReg, resReg, aReg, mask);
}

__simd_callee__ inline void LgammaCal07To123(
    Reg::RegTensor<float>& resReg, Reg::RegTensor<float>& absReg, Reg::RegTensor<float>& tmpReg, Reg::MaskReg mask)
{
    Reg::Adds(tmpReg, absReg, fn1, mask);
    LgammaCalPow70To023(resReg, tmpReg, mask);
}

__simd_callee__ inline void LgammaCal123o173(
    Reg::RegTensor<float>& resReg, Reg::RegTensor<float>& absReg, Reg::RegTensor<float>& tmpReg, Reg::MaskReg mask)
{
    constexpr float tc = -1.46163214496836224576e+00;
    Reg::RegTensor<float> aReg;
    Reg::RegTensor<float> bReg;
    Reg::RegTensor<float> cReg;
    Reg::RegTensor<float> dReg;
    Reg::RegTensor<float> wReg;
    Reg::RegTensor<float> zReg;

    constexpr MulAddsParams params0 = GetConstants123To173();
    // a = (((r4 * x + r3) * x + r2) * x + r1) * x + r0
    LGammaCalcMulAdd(aReg, absReg, mask, params0);

    constexpr float tt = -3.63867699703950536541e-18;
    constexpr float tf = -1.21486290535849611461e-01;
    // y = x + tc
    Reg::Adds(tmpReg, absReg, tc, mask);
    // z = y * y
    Reg::Mul(zReg, tmpReg, tmpReg, mask);
    // w = z * y
    Reg::Mul(wReg, zReg, tmpReg, mask);
    constexpr MulAddsParams params1 = GetConstants123To173V2();
    // b = (((t4 * w + t3) * w + t2) * w + t1) * w + t0
    LGammaCalcMulAdd(bReg, wReg, mask, params1);
    constexpr MulAddsParams params2 = GetConstants123To173V3();
    // c = (((t9 * w + t8) * w + t7) * w + t6) * w + t5
    LGammaCalcMulAdd(cReg, wReg, mask, params2);
    constexpr MulAddsParams params3 = GetConstants123To173V4();
    // d = (((t14 * w + t13) * w + t12) * w + t11) * w + t10
    LGammaCalcMulAdd(dReg, wReg, mask, params3);
    // res = z * b - （tt - w * (c + y * d))
    Reg::Mul(resReg, zReg, bReg, mask);
    Reg::Mul(tmpReg, tmpReg, dReg, mask);
    Reg::Add(tmpReg, tmpReg, cReg, mask);
    Reg::Mul(tmpReg, tmpReg, wReg, mask);
    Reg::Duplicate(bReg, tt, mask);
    Reg::Sub(tmpReg, bReg, tmpReg, mask);
    Reg::Sub(resReg, resReg, tmpReg, mask);
    // res = res + tf
    Reg::Adds(resReg, resReg, tf, mask);
    // res = res + a
    Reg::Add(resReg, resReg, aReg, mask);
}

__simd_callee__ inline void LgammaCal173To2(
    Reg::RegTensor<float>& resReg, Reg::RegTensor<float>& srcReg, Reg::RegTensor<float>& tmpReg, Reg::MaskReg mask)
{
    constexpr float r0 = 0.422784325282832;
    constexpr float r1 = -2.000000000156967;
    constexpr float r2 = 1.5704527182920263;
    constexpr float r3 = 4.56346777685429;
    constexpr float r4 = 11.072315459770081;
    constexpr float r5 = -2.351768784938553;

    Reg::RegTensor<float> aReg;
    Reg::RegTensor<float> bReg;
    Reg::RegTensor<float> cReg;
    Reg::RegTensor<float> dReg;

    // a = x + r1
    Reg::Adds(aReg, srcReg, r1, mask);
    // b =  x + r2
    Reg::Adds(bReg, srcReg, r2, mask);
    // c = x / r5
    Reg::Duplicate(tmpReg, r5, mask);
    Reg::Div(cReg, srcReg, tmpReg, mask);
    // d = r4 / a
    Reg::Duplicate(tmpReg, r4, mask);
    Reg::Div(dReg, tmpReg, aReg, mask);
    // res = r3 - c
    Reg::Duplicate(tmpReg, r3, mask);
    Reg::Sub(resReg, tmpReg, cReg, mask);
    // res = res + d
    Reg::Add(resReg, resReg, dReg, mask);
    // res = b / res
    Reg::Div(resReg, bReg, resReg, mask);
    // res = res + r0
    Reg::Adds(resReg, resReg, r0, mask);
    // res = res * a
    Reg::Mul(resReg, aReg, resReg, mask);
}

__simd_callee__ inline void LgammaCal2To22(
    Reg::RegTensor<float>& resReg, Reg::RegTensor<float>& srcReg, Reg::RegTensor<float>& tmpReg, Reg::MaskReg mask)
{
    constexpr float r0 = -1.0012922592272755;
    constexpr float r1 = 2.4477420497054303;
    constexpr float r2 = -0.9987077407790342;
    constexpr float r3 = 0.18634242386907246;
    constexpr float r4 = 1.7427676919841077;
    constexpr float r5 = 2.2595210499965988;

    Reg::RegTensor<float> aReg;
    Reg::RegTensor<float> bReg;
    Reg::RegTensor<float> cReg;

    // a = x + r2
    Reg::Adds(aReg, srcReg, r2, mask);
    // b =  a - r0
    Reg::Adds(bReg, aReg, r0, mask);
    // c = a / b
    Reg::Div(cReg, aReg, bReg, mask);
    // c = r4 / c
    Reg::Duplicate(tmpReg, r4, mask);
    Reg::Div(cReg, tmpReg, cReg, mask);
    // res = r5 - c
    Reg::Duplicate(tmpReg, r5, mask);
    Reg::Sub(resReg, tmpReg, cReg, mask);
    // res = r3 / res
    Reg::Duplicate(tmpReg, r3, mask);
    Reg::Div(resReg, tmpReg, resReg, mask);
    // res = res + c
    Reg::Add(resReg, cReg, resReg, mask);
    // res = r1 - res
    Reg::Duplicate(tmpReg, r1, mask);
    Reg::Sub(resReg, tmpReg, resReg, mask);
    // res = b / res
    Reg::Div(resReg, bReg, resReg, mask);
}

__simd_callee__ inline void LgammaCal22To25(
    Reg::RegTensor<float>& resReg, Reg::RegTensor<float>& srcReg, Reg::RegTensor<float>& tmpReg, Reg::MaskReg mask)
{
    constexpr float r0 = 0.5071375856257216;
    constexpr float r1 = -2.0000069063575214;
    constexpr float r2 = -0.08525477083158943;
    constexpr float r3 = -0.7681154946764155;
    constexpr float r4 = -1.7452430203554998;

    Reg::RegTensor<float> aReg;
    Reg::RegTensor<float> bReg;

    // a = x + r1
    Reg::Adds(aReg, srcReg, r1, mask);
    // res =  r4 - x
    Reg::Muls(tmpReg, srcReg, fn1, mask);
    Reg::Adds(resReg, tmpReg, r4, mask);
    // res = r3 / res
    Reg::Duplicate(tmpReg, r3, mask);
    Reg::Div(resReg, tmpReg, resReg, mask);
    // b = r2 -res
    Reg::Duplicate(tmpReg, r2, mask);
    Reg::Sub(bReg, tmpReg, resReg, mask);
    // res = b + a
    Reg::Add(resReg, bReg, aReg, mask);
    // res = res * b
    Reg::Mul(resReg, bReg, resReg, mask);
    // res = r0 - res
    Reg::Duplicate(tmpReg, r0, mask);
    Reg::Sub(resReg, tmpReg, resReg, mask);
    // res = a * res
    Reg::Mul(resReg, aReg, resReg, mask);
}

__simd_callee__ inline void LgammaCal25To3(
    Reg::RegTensor<float>& resReg, Reg::RegTensor<float>& srcReg, Reg::RegTensor<float>& tmpReg, Reg::MaskReg mask)
{
    constexpr float r0 = -2.225444571099429;
    constexpr float r1 = -2.7949471270267456;
    constexpr float r2 = -0.017787316457839795;
    constexpr float r3 = 0.14449865133660186;
    constexpr float r4 = 0.2566236573575009;

    Reg::RegTensor<float> aReg;
    Reg::RegTensor<float> bReg;
    Reg::RegTensor<float> cReg;
    Reg::RegTensor<float> dReg;
    Reg::RegTensor<float> eReg;

    // a = x + r0
    Reg::Adds(aReg, srcReg, r0, mask);
    // b =  r4 - x
    Reg::Muls(tmpReg, srcReg, fn1, mask);
    Reg::Adds(bReg, tmpReg, r4, mask);
    // c = a + r1
    Reg::Adds(cReg, aReg, r1, mask);
    // d = r0 + b
    Reg::Adds(dReg, bReg, r0, mask);
    // c = x + c
    Reg::Add(cReg, srcReg, cReg, mask);
    // d = r3 / d
    Reg::Duplicate(tmpReg, r3, mask);
    Reg::Div(dReg, tmpReg, dReg, mask);
    // d = d + r2
    Reg::Adds(dReg, dReg, r2, mask);
    // c = d + c
    Reg::Add(cReg, dReg, cReg, mask);
    // e = d * c
    Reg::Mul(eReg, cReg, dReg, mask);
    // d = c + r1
    Reg::Adds(dReg, cReg, r1, mask);
    // res = e * d
    Reg::Mul(resReg, eReg, dReg, mask);
    // res = a - res
    Reg::Sub(resReg, aReg, resReg, mask);
}

__simd_callee__ inline void LgammaCal3To8(
    Reg::RegTensor<float>& resReg, Reg::RegTensor<float>& absReg, Reg::RegTensor<float>& tmpReg, Reg::MaskReg mask)
{
    constexpr float r0 = -7.72156649015328655494e-02;
    constexpr float r1 = 2.14982415960608852501e-01;
    constexpr float v1 = 1.39200533467621045958e+00;
    Reg::RegTensor<float> aReg;
    Reg::RegTensor<float> bReg;
    Reg::RegTensor<float> floorReg;
    Reg::MaskReg maskReg;

    // i = floor(x)
    Reg::Truncate<float, RoundMode::CAST_FLOOR, Reg::MaskMergeMode::ZEROING>(floorReg, absReg, mask);
    // y = x - i
    Reg::Sub(tmpReg, absReg, floorReg, mask);
    constexpr MulAddsParams params0 = GetConstants3To8();
    // a = ((((((r6 * y + r5) * y + r4) * y + r3) * y + r2) * y + r1) * y + r0) * y
    LGammaCalcMulAdd(aReg, tmpReg, mask, params0);
    Reg::Mul(aReg, aReg, tmpReg, mask);
    Reg::Adds(aReg, aReg, r1, mask);
    Reg::Mul(aReg, aReg, tmpReg, mask);
    Reg::Adds(aReg, aReg, r0, mask);
    Reg::Mul(aReg, aReg, tmpReg, mask);

    constexpr MulAddsParams params1 = GetConstants3To8V2();
    // b = ((((((y * v6 + v5) + v4) * y + v3) * y + v2) * y + v1) * y + 1
    LGammaCalcMulAdd(bReg, tmpReg, mask, params1);
    Reg::Mul(bReg, bReg, tmpReg, mask);
    Reg::Adds(bReg, bReg, v1, mask);
    Reg::Mul(bReg, bReg, tmpReg, mask);
    Reg::Adds(bReg, bReg, f1, mask);

    // res = 0.5 * y + a / b
    Reg::Div(aReg, aReg, bReg, mask);
    Reg::Muls(bReg, tmpReg, f05, mask);
    Reg::Add(resReg, bReg, aReg, mask);

    Reg::Duplicate(aReg, f1, mask);
    // a[i >= 3] *= y[i >= 3] + 2
    Reg::CompareScalar<float, CMPMODE::GE>(maskReg, floorReg, f3, mask);
    Reg::Adds(bReg, tmpReg, f2, mask);
    Reg::Mul(bReg, aReg, bReg, mask);
    Reg::Select(aReg, bReg, aReg, maskReg);
    // a[i >= 4] *= y[i >= 4] + 3
    Reg::CompareScalar<float, CMPMODE::GE>(maskReg, floorReg, F4, mask);
    Reg::Adds(bReg, tmpReg, f3, mask);
    Reg::Mul(bReg, aReg, bReg, mask);
    Reg::Select(aReg, bReg, aReg, maskReg);
    // a[i >= 5] *= y[i >= 5] + 4
    Reg::CompareScalar<float, CMPMODE::GE>(maskReg, floorReg, F5, mask);
    Reg::Adds(bReg, tmpReg, F4, mask);
    Reg::Mul(bReg, aReg, bReg, mask);
    Reg::Select(aReg, bReg, aReg, maskReg);
    // a[i >= 6] *= y[i >= 6] + 5
    Reg::CompareScalar<float, CMPMODE::GE>(maskReg, floorReg, F6, mask);
    Reg::Adds(bReg, tmpReg, F5, mask);
    Reg::Mul(bReg, aReg, bReg, mask);
    Reg::Select(aReg, bReg, aReg, maskReg);
    // a[i >= 7] *= y[i >= 7] + 6
    Reg::CompareScalar<float, CMPMODE::GE>(maskReg, floorReg, F7, mask);
    Reg::Adds(bReg, tmpReg, F6, mask);
    Reg::Mul(bReg, aReg, bReg, mask);
    Reg::Select(aReg, bReg, aReg, maskReg);
    // res = res + ln(a)
    Reg::Ln(aReg, aReg, mask);
    Reg::Add(resReg, resReg, aReg, mask);
}

__simd_callee__ inline void LgammaCal8ToPow58(
    Reg::RegTensor<float>& resReg, Reg::RegTensor<float>& absReg, Reg::RegTensor<float>& tmpReg, Reg::MaskReg mask)
{
    constexpr float r0 = 4.18938533204672725052e-01;
    constexpr float r1 = 8.33333333333329678849e-02;
    Reg::RegTensor<float> aReg;
    Reg::RegTensor<float> bReg;

    // a = ln(x)
    Reg::Ln(aReg, absReg, mask);
    // b = 1 / x
    Reg::Duplicate(tmpReg, f1, mask);
    Reg::Div(bReg, tmpReg, absReg, mask);
    // y = b * b
    Reg::Mul(tmpReg, bReg, bReg, mask);

    constexpr MulAddsParams params0 = GetConstants8ToPow58();
    // res = (((((y * r6 + r5) * y + r4) * y + r3) * y + r2) * y + r1) * b + r0
    LGammaCalcMulAdd(resReg, tmpReg, mask, params0);
    Reg::Mul(resReg, resReg, tmpReg, mask);
    Reg::Adds(resReg, resReg, r1, mask);
    Reg::Mul(resReg, resReg, bReg, mask);
    Reg::Adds(resReg, resReg, r0, mask);

    // res = (x - 0.5) * (t - 1) + res
    Reg::Adds(bReg, absReg, fn05, mask);
    Reg::Adds(aReg, aReg, fn1, mask);
    Reg::Mul(aReg, aReg, bReg, mask);
    Reg::Add(resReg, resReg, aReg, mask);
}

__simd_callee__ inline void SearchSinPi(
    Reg::RegTensor<float>& resReg, Reg::RegTensor<float>& srcReg, Reg::RegTensor<float>& tmpReg, Reg::MaskReg mask)
{
    constexpr float r0 = 0.9452154240716536;
    constexpr float r1 = 1.323678940948241;
    constexpr float r2 = 0.27798021173736565;
    constexpr float r3 = -5.189778155221027;
    constexpr float r4 = -3.605483266634817;
    constexpr float r5 = -0.01134864151666764;
    constexpr float r6 = 0.96602443393936;

    Reg::RegTensor<float> aReg;
    Reg::RegTensor<float> bReg;
    Reg::RegTensor<float> cReg;

    // a = x * x
    Reg::Mul(aReg, srcReg, srcReg, mask);
    // b = r2 * a
    Reg::Muls(bReg, aReg, r2, mask);
    // c = b - r0
    Reg::Duplicate(tmpReg, r0, mask);
    Reg::Sub(cReg, bReg, tmpReg, mask);
    // res = r6 + c
    Reg::Adds(resReg, cReg, r6, mask);
    // res = res * c
    Reg::Mul(resReg, resReg, cReg, mask);
    // res = res + r5
    Reg::Adds(resReg, resReg, r5, mask);
    // res = res * r4
    Reg::Muls(resReg, resReg, r4, mask);
    // res = res + b
    Reg::Add(resReg, resReg, bReg, mask);
    // res = res + r3
    Reg::Adds(resReg, resReg, r3, mask);
    // res = a * res
    Reg::Mul(resReg, resReg, aReg, mask);
    // res = res + r1
    Reg::Adds(resReg, resReg, r1, mask);
    // res = res * c
    Reg::Mul(resReg, resReg, cReg, mask);
    // res = r0 - res
    Reg::Duplicate(tmpReg, r0, mask);
    Reg::Sub(resReg, tmpReg, resReg, mask);
    // res = res + ro
    Reg::Adds(resReg, resReg, r0, mask);
    // res = res * x
    Reg::Mul(resReg, resReg, srcReg, mask);
}

__simd_callee__ inline void SinPi(
    Reg::RegTensor<float>& resReg, Reg::RegTensor<float>& srcReg, Reg::RegTensor<float>& tmpReg, Reg::MaskReg mask)
{
    Reg::RegTensor<float> aReg;
    Reg::RegTensor<float> bReg;
    Reg::RegTensor<float> cReg;
    Reg::MaskReg maskReg;

    Reg::Duplicate(resReg, 0.0f, mask);
    Reg::Abs(aReg, srcReg, mask);
    // y = abs_x - floor(abs_x)
    Reg::Truncate<float, RoundMode::CAST_FLOOR, Reg::MaskMergeMode::ZEROING>(tmpReg, aReg, mask);
    Reg::Sub(tmpReg, aReg, tmpReg, mask);

    Reg::CompareScalar<float, CMPMODE::GE>(maskReg, tmpReg, f05, mask);
    Reg::Duplicate(aReg, f1, mask);
    Reg::Sub(aReg, aReg, tmpReg, mask);
    SearchSinPi(cReg, aReg, bReg, mask);
    Reg::Select(resReg, cReg, resReg, maskReg);

    Reg::CompareScalar<float, CMPMODE::LT>(maskReg, tmpReg, f05, mask);
    SearchSinPi(cReg, tmpReg, bReg, mask);
    Reg::Select(resReg, cReg, resReg, maskReg);

    Reg::Duplicate(bReg, 0.0f, mask);
    Reg::Duplicate(aReg, f1, mask);
    Reg::Duplicate(cReg, fn1, mask);
    Reg::CompareScalar<float, CMPMODE::LT>(maskReg, srcReg, 0.0f, mask);
    Reg::Select(bReg, cReg, bReg, maskReg);
    Reg::CompareScalar<float, CMPMODE::GT>(maskReg, srcReg, 0.0f, mask);
    Reg::Select(bReg, aReg, bReg, maskReg);
    Reg::Mul(resReg, resReg, bReg, mask);
}

__simd_callee__ inline void LgammaCalNegPow70(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& resReg, Reg::RegTensor<float>& srcReg,
    Reg::RegTensor<float>& tmpReg, Reg::MaskReg mask)
{
    constexpr float logpi = 1.14472988584940016388e+00;
    Reg::RegTensor<float> aReg;

    // res = logpi - log(abs(sinpi(x))) - log(-x) - dst
    Reg::Duplicate(tmpReg, logpi, mask);
    SinPi(resReg, srcReg, aReg, mask);
    Reg::Abs(resReg, resReg, mask);
    Reg::Ln(resReg, resReg, mask);
    Reg::Sub(resReg, tmpReg, resReg, mask);
    Reg::Muls(tmpReg, srcReg, fn1, mask);
    Reg::Ln(tmpReg, tmpReg, mask);
    Reg::Sub(resReg, resReg, tmpReg, mask);
    Reg::Sub(resReg, resReg, dstReg, mask);
}

__simd_callee__ inline void LgammaCompute1(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& absReg, Reg::RegTensor<float>& resReg,
    Reg::RegTensor<float>& tmpReg, Reg::MaskReg cmpMaskReg1, Reg::MaskReg cmpMaskReg2, Reg::MaskReg cmpMaskReg,
    Reg::MaskReg mask)
{
    // abs_x <= 2^-70
    Reg::CompareScalar<float, CMPMODE::LE>(cmpMaskReg, absReg, (float&)POW_70, mask);
    Reg::Ln(tmpReg, absReg, mask);
    Reg::Muls(resReg, tmpReg, fn1, mask);
    Reg::Select(dstReg, resReg, dstReg, cmpMaskReg);

    // 2^-70 < abs_x < 0.23
    Reg::CompareScalar<float, CMPMODE::LT>(cmpMaskReg1, absReg, (float&)F023, mask);
    Reg::MaskXor(cmpMaskReg, cmpMaskReg1, cmpMaskReg, mask);
    LgammaCalPow70To023(resReg, absReg, mask);
    Reg::Sub(resReg, resReg, tmpReg, mask);
    Reg::Select(dstReg, resReg, dstReg, cmpMaskReg);

    // 0.23 <= abs_x < 0.7
    Reg::CompareScalar<float, CMPMODE::LT>(cmpMaskReg2, absReg, (float&)F07, mask);
    Reg::MaskXor(cmpMaskReg, cmpMaskReg1, cmpMaskReg2, mask);
    LgammaCal023To073(resReg, absReg, tmpReg, mask);
    Reg::Select(dstReg, resReg, dstReg, cmpMaskReg);

    // 0.7 <= abs_x < 1.23
    Reg::CompareScalar<float, CMPMODE::LT>(cmpMaskReg1, absReg, (float&)F123, mask);
    Reg::MaskXor(cmpMaskReg, cmpMaskReg1, cmpMaskReg2, mask);
    Reg::CompareScalar<float, CMPMODE::NE>(cmpMaskReg2, absReg, f1, mask);
    Reg::MaskAnd(cmpMaskReg, cmpMaskReg2, cmpMaskReg, mask);
    LgammaCal07To123(resReg, absReg, tmpReg, mask);
    Reg::Select(dstReg, resReg, dstReg, cmpMaskReg);

    // 1.23 <= abs_x < 1.73
    Reg::CompareScalar<float, CMPMODE::LT>(cmpMaskReg2, absReg, (float&)F173, mask);
    Reg::MaskXor(cmpMaskReg, cmpMaskReg1, cmpMaskReg2, mask);
    LgammaCal123o173(resReg, absReg, tmpReg, mask);
    Reg::Select(dstReg, resReg, dstReg, cmpMaskReg);

    // 1.73 <= abs_x < 2
    Reg::CompareScalar<float, CMPMODE::LT>(cmpMaskReg1, absReg, f2, mask);
    Reg::MaskXor(cmpMaskReg, cmpMaskReg1, cmpMaskReg2, mask);
    LgammaCal173To2(resReg, absReg, tmpReg, mask);
    Reg::Select(dstReg, resReg, dstReg, cmpMaskReg);

    // 2 <= abs_x < 2.2
    Reg::CompareScalar<float, CMPMODE::LT>(cmpMaskReg2, absReg, (float&)F22, mask);
    Reg::MaskXor(cmpMaskReg, cmpMaskReg1, cmpMaskReg2, mask);
    LgammaCal2To22(resReg, absReg, tmpReg, mask);
    Reg::Select(dstReg, resReg, dstReg, cmpMaskReg);

    // 2.2 <= abs_x < 2.5
    Reg::CompareScalar<float, CMPMODE::LT>(cmpMaskReg1, absReg, (float&)F25, mask);
    Reg::MaskXor(cmpMaskReg, cmpMaskReg1, cmpMaskReg2, mask);
    LgammaCal22To25(resReg, absReg, tmpReg, mask);
    Reg::Select(dstReg, resReg, dstReg, cmpMaskReg);

    // 2.5 <= abs_x < 3
    Reg::CompareScalar<float, CMPMODE::LT>(cmpMaskReg2, absReg, f3, mask);
    Reg::MaskXor(cmpMaskReg, cmpMaskReg1, cmpMaskReg2, mask);
    LgammaCal25To3(resReg, absReg, tmpReg, mask);
    Reg::Select(dstReg, resReg, dstReg, cmpMaskReg);

    // 3 <= abs_x < 8
    Reg::CompareScalar<float, CMPMODE::LT>(cmpMaskReg1, absReg, F8, mask);
    Reg::MaskXor(cmpMaskReg, cmpMaskReg1, cmpMaskReg2, mask);
    LgammaCal3To8(resReg, absReg, tmpReg, mask);
    Reg::Select(dstReg, resReg, dstReg, cmpMaskReg);

    // 8 <= abs_x < 2^58
    Reg::CompareScalar<float, CMPMODE::LT>(cmpMaskReg2, absReg, (float&)POW_58, mask);
    Reg::MaskXor(cmpMaskReg, cmpMaskReg1, cmpMaskReg2, mask);
    LgammaCal8ToPow58(resReg, absReg, tmpReg, mask);
    Reg::Select(dstReg, resReg, dstReg, cmpMaskReg);
}

__simd_callee__ inline void LgammaCompute2(
    Reg::RegTensor<float>& dstReg, Reg::RegTensor<float>& srcReg, Reg::RegTensor<float>& resReg,
    Reg::RegTensor<float>& tmpReg, Reg::RegTensor<float>& infReg, Reg::RegTensor<float>& nanReg,
    Reg::RegTensor<float>& scalar0, Reg::RegTensor<float>& scalar1, Reg::MaskReg cmpMaskReg, Reg::MaskReg mask)
{
    Reg::Abs(tmpReg, srcReg, mask);
    // 2^58 <= abs_x
    Reg::CompareScalar<float, CMPMODE::GE>(cmpMaskReg, tmpReg, (float&)POW_58, mask);
    // x * (log(abs_x) - 1)
    Reg::Ln(tmpReg, tmpReg, mask);
    Reg::Adds(tmpReg, tmpReg, fn1, mask);
    Reg::Mul(resReg, tmpReg, srcReg, mask);
    Reg::Select(dstReg, resReg, dstReg, cmpMaskReg);

    // x < 2^-70
    Reg::CompareScalar<float, CMPMODE::LT>(cmpMaskReg, srcReg, (float&)NEG_POW_70, mask);
    LgammaCalNegPow70(dstReg, resReg, srcReg, tmpReg, mask);
    Reg::Select(dstReg, resReg, dstReg, cmpMaskReg);

    // x == -inf
    Reg::CompareScalar<float, CMPMODE::EQ>(cmpMaskReg, srcReg, (float&)F32_NEG_INF, mask);
    Reg::Select(dstReg, infReg, dstReg, cmpMaskReg);

    // x == nan
    Reg::ShiftRights(
        (Reg::RegTensor<uint32_t>&)tmpReg, (Reg::RegTensor<uint32_t>&)srcReg, static_cast<int16_t>(MANTISSA), mask);
    Reg::And(
        (Reg::RegTensor<uint32_t>&)tmpReg, (Reg::RegTensor<uint32_t>&)tmpReg, (Reg::RegTensor<uint32_t>&)scalar0, mask);
    Reg::And(
        (Reg::RegTensor<uint32_t>&)resReg, (Reg::RegTensor<uint32_t>&)srcReg, (Reg::RegTensor<uint32_t>&)scalar1, mask);
    Reg::Compare(cmpMaskReg, tmpReg, scalar0, mask);
    Reg::CompareScalar<float, CMPMODE::NE>(cmpMaskReg, resReg, 0.0f, cmpMaskReg);
    Reg::Select(dstReg, nanReg, dstReg, cmpMaskReg);
}

template <typename T, bool isReuseSource = false>
__simd_vf__ inline void LgammaComputeImpl(
    __ubuf__ T* dstUb, __ubuf__ T* srcUb, __ubuf__ float* workUb, uint32_t calCount, uint16_t repeatTime)
{
    constexpr uint32_t stride = GetVecLen() / sizeof(float);
    uint32_t sreg0 = calCount;
    uint32_t sreg1 = calCount;

    Reg::MaskReg cmpMaskReg1;
    Reg::MaskReg cmpMaskReg2;
    Reg::MaskReg cmpMaskReg;
    Reg::MaskReg mask = Reg::CreateMask<float>();
    Reg::RegTensor<T> srcReg;
    Reg::RegTensor<float> castReg;
    Reg::RegTensor<float> tmpReg;
    Reg::RegTensor<float> resReg;
    Reg::RegTensor<float> dstReg;
    Reg::RegTensor<float> absReg;
    Reg::RegTensor<float> infReg;
    Reg::RegTensor<float> nanReg;
    Reg::RegTensor<float> scalar0;
    Reg::RegTensor<float> scalar1;

    Reg::Duplicate(infReg, (float&)F32_POS_INF);
    Reg::Duplicate(nanReg, (float&)F32_NAN);
    Reg::Duplicate(scalar0, (float&)SCALAR0);
    Reg::Duplicate(scalar1, (float&)SCALAR1);
    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<float>(sreg0);
        Reg::Duplicate(dstReg, static_cast<float>(0));
        LgammaLoadData<T>(castReg, srcReg, srcUb + i * stride, mask);
        Reg::Abs(absReg, castReg, mask);
        LgammaCompute1(dstReg, absReg, resReg, tmpReg, cmpMaskReg1, cmpMaskReg2, cmpMaskReg, mask);
        Reg::StoreAlign(workUb + i * stride, dstReg, mask);
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();

    for (uint16_t i = 0; i < repeatTime; ++i) {
        mask = Reg::UpdateMask<float>(sreg1);
        LgammaLoadData<T>(castReg, srcReg, srcUb + i * stride, mask);
        Reg::LoadAlign(dstReg, workUb + i * stride);
        LgammaCompute2(dstReg, castReg, resReg, tmpReg, infReg, nanReg, scalar0, scalar1, cmpMaskReg, mask);
        if constexpr (IsSameType<T, half>::value) {
            Reg::Cast<T, float, LGAMMA_CAST_TRAIT_F322F16>(srcReg, dstReg, mask);
            Reg::StoreAlign<T, Reg::StoreDist::DIST_PACK_B32>(dstUb + i * stride, srcReg, mask);
        } else {
            Reg::StoreAlign(dstUb + i * stride, dstReg, mask);
        }
    }
}
} // namespace LgammaInternal

template <typename T, bool isReuseSource = false>
__aicore__ inline void LgammaImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint8_t>& tmp, const uint32_t calCount)
{
    CheckTensorPosition(dst, "dstTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(src, "srcTensor", "VECIN/VECOUT/VECCALC");
    CheckTensorPosition(tmp, "sharedTmpBuffer", "VECIN/VECOUT/VECCALC");

    CheckCalCount(calCount, "calCount", src, "srcTensor", "Lgamma");
    CheckCalCount(calCount, "calCount", dst, "dstTensor", "Lgamma");

    static_assert(SupportType<T, half, float>(), "current data type is not supported on current device!");
    constexpr uint32_t stride = GetVecLen() / sizeof(float);
    uint16_t repeatTime = CeilDivision(calCount, stride);
    auto workLocal = tmp.ReinterpretCast<float>();

    __ubuf__ T* dstUb = (__ubuf__ T*)dst.GetPhyAddr();
    __ubuf__ T* srcUb = (__ubuf__ T*)src.GetPhyAddr();
    __ubuf__ float* workUb = (__ubuf__ float*)workLocal.GetPhyAddr();
    LgammaInternal::LgammaComputeImpl<T, isReuseSource>(dstUb, srcUb, workUb, calCount, repeatTime);
}
} // namespace AscendC
#endif // IMPL_MATH_LGAMMA_LGAMMA_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_LGAMMA_LGAMMA_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_LGAMMA_LGAMMA_C310_IMPL_H__
#endif
