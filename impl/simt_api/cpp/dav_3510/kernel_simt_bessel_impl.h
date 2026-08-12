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
 * \file kernel_simt_bessel_impl.h
 * \brief
 */
#ifndef IMPL_SIMT_API_CPP_DAV_C310_KERNEL_SIMT_BESSEL_IMPL_H
#define IMPL_SIMT_API_CPP_DAV_C310_KERNEL_SIMT_BESSEL_IMPL_H

#if defined(ASCENDC_CPU_DEBUG)
#include <cmath>

#include "../../../basic_api/kernel_utils.h"
#include "stub_def.h"
#endif

#include "impl/simt_api/cpp/dav_3510/kernel_simt_cmp_impl.h"
#include "impl/simt_api/cpp/dav_3510/kernel_simt_math_impl.h"
#include "impl/simt_api/cpp/dav_3510/kernel_simt_transcendental_impl.h"

namespace AscendC {
namespace Simt {

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T J0Impl(T x);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T J1Impl(T x);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Y0Impl(T x);

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Y1Impl(T x);

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float TrigRedSlowpathFFastMode(float a, int* quadrant)
{
    uint64_t q, q2;
    q = (uint64_t)(a * ConstantsInternal::TWO_OVER_PI);
    a = FmaImpl(q, ConstantsInternal::MINUS_PI_OVER_TWO_HI, a);
    a = FmaImpl(q, ConstantsInternal::MINUS_PI_OVER_TWO_LO, a);
    q2 = (uint64_t)(a * ConstantsInternal::TWO_OVER_PI);
    a = FmaImpl(q2, ConstantsInternal::MINUS_PI_OVER_TWO_HI, a);
    q = q % 4 + q2 % 4; // 4:Number of quadrants
    a = a - 0.7853982f;
    *quadrant = q % 4; // 4:Number of quadrants
    return a;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float SinfPoly(float a, float s)
{
    float r = 2.86567956e-6f;
    r = FmaImpl(r, s, -1.98559923e-4f);
    r = FmaImpl(r, s, 8.33338592e-3f);
    r = FmaImpl(r, s, -1.66666672e-1f);
    float t = FmaImpl(a, s, 0.0f);
    r = FmaImpl(r, t, a);
    return r;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float CosfPoly(float s)
{
    float r = 2.44677067e-5f;
    r = FmaImpl(r, s, -1.38877297e-3f);
    r = FmaImpl(r, s, 4.16666567e-2f);
    r = FmaImpl(r, s, -5.00000000e-1f);
    r = FmaImpl(r, s, 1.00000000e+0f);
    return r;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float SinCosfMinusPIOverFour(float a, int index)
{
    float r;
    int i;
    a = a * 0.0f + a;
    r = TrigRedSlowpathFFastMode(a, &i);
    float c, s, t;
    s = r * r;
    c = CosfPoly(s);
    s = SinfPoly(r, s);
    if (i & 2) { // 2:Third and Fourth Quadrants
        s = 0.0f - s;
        c = 0.0f - c;
    }
    if (index == 0) { // 0:Calculate CosfMinusPIOverFour
        if (i & 1) {
            c = 0.0f - s;
        }
        return c;
    } else {
        if (i & 1) {
            s = c;
        }
        return s;
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float CalJ0XLarger8(float x)
{
    if (IsInfImpl(x)) {
        return 0.0f;
    }
    float invX = 1.0f / x;
    float invX2 = invX * invX;
    float beta = FmaImpl(5.848699569702148f, invX2, -0.5428466796875f);
    beta = FmaImpl(beta, invX2, 0.103515625f);
    beta = FmaImpl(beta, invX2, -0.0625f);
    beta = FmaImpl(beta, invX2, 1.0f);
    float alpha = FmaImpl(1.6380658830915178f, invX2, -0.2095703125f);
    alpha = FmaImpl(alpha, invX2, 0.06510416666666666f);
    alpha = FmaImpl(alpha, invX2, -0.125f);
    alpha = FmaImpl(alpha, invX, x);
    float theta = RsqrtImpl(x);
    theta = theta * 0.7978846f;
    float preCoeff = beta * theta;
    float afterCoeff = SinCosfMinusPIOverFour(alpha, 0); // 0:Calculate CosfMinusPIOverFour
    return afterCoeff * preCoeff;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float CalJ1XLarger8(float x)
{
    if (IsInfImpl(x)) {
        return 0.0f;
    }
    float invX = 1.0f / x;
    float invX2 = invX * invX;
    float beta = FmaImpl(-7.739953994751f, invX2, 0.8052978515625f);
    beta = FmaImpl(beta, invX2, -0.193359375f);
    beta = FmaImpl(beta, invX2, 0.1875f);
    beta = FmaImpl(beta, invX2, 1.0f);
    float alpha = FmaImpl(-2.3693978445870534f, invX2, 0.3708984375f);
    alpha = FmaImpl(alpha, invX2, -0.1640625f);
    alpha = FmaImpl(alpha, invX2, 0.375f);
    alpha = FmaImpl(alpha, invX, x);
    float theta = RsqrtImpl(x);
    theta = theta * 0.7978846f;
    float preCoeff = beta * theta;
    float afterCoeff = SinCosfMinusPIOverFour(alpha, 1); // 1:Calculate SinfMinusPIOverFour
    return afterCoeff * preCoeff;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float CalJ0XLess8(float x)
{
    float d1 = x - 2.4048254f;
    d1 = d1 - 1.087059e-7f;
    float res = 9.619266247e-13f;
    res = FmaImpl(res, d1, 5.702105547e-12f);
    res = FmaImpl(res, d1, -4.398487105e-10f);
    res = FmaImpl(res, d1, 4.604940853e-10f);
    res = FmaImpl(res, d1, 5.847321173e-08f);
    res = FmaImpl(res, d1, 2.084518856e-09f);
    res = FmaImpl(res, d1, -5.452075416e-06f);
    res = FmaImpl(res, d1, -7.342953250e-06f);
    res = FmaImpl(res, d1, 3.017067874e-04f);
    res = FmaImpl(res, d1, 7.739535477e-04f);
    res = FmaImpl(res, d1, -7.283463700e-03f);
    res = FmaImpl(res, d1, -2.666820353e-02f);
    res = d1 * res;
    float d2 = x - 5.520078f;
    d2 = d2 + 7.1934145e-8f;
    res = d2 * res;

    float d3 = x - 8.653728f;
    d3 = d3 - 3.8147791e-7f;
    res = d3 * res;
    return res;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float CalJ1XLess8(float x)
{
    float d1 = x - 3.831706f;
    d1 = d1 + 7.685059e-8f;
    float res = 9.206492556e-14f;
    res = FmaImpl(res, d1, 9.126927192e-13f);
    res = FmaImpl(res, d1, -2.641634001e-11f);
    res = FmaImpl(res, d1, -2.014359882e-10f);
    res = FmaImpl(res, d1, 4.525844770e-09f);
    res = FmaImpl(res, d1, 2.701145918e-08f);
    res = FmaImpl(res, d1, -5.348958058e-07f);
    res = FmaImpl(res, d1, -2.360248564e-06f);
    res = FmaImpl(res, d1, 4.121127279e-05f);
    res = FmaImpl(res, d1, 1.191702295e-04f);
    res = FmaImpl(res, d1, -1.807559530e-03f);
    res = FmaImpl(res, d1, -2.554892713e-03f);
    res = FmaImpl(res, d1, 3.301389139e-02f);
    res = d1 * res;
    float d2 = x - 7.015587f;
    d2 = d2 + 1.8321172e-7f;
    res = d2 * res;
    res = res * x;
    return res;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float JnYnAsymptoticBesselAmplitude(int n, float x, int index)
{
    float s = 1.0f;
    float mu = 4 * n * n;
    float txq = 2 * x;
    txq *= txq;
    if (index == 0) { // 0:Calculate JnAsymptoticBesselAmplitude
        // 1 + (4 * n^2 - 1) / (8 * x^2) + 3 * (4 * n^2 - 1) * (4 * n^2 - 9) / (128 * x^4)
        s += (mu - 1) / (2 * txq);                      // 1,2:Constants in formulas
        s += 3 * (mu - 1) * (mu - 9) / (txq * txq * 8); // 3,1,9,8:Constants in formulas
    } else {
        s += (mu - 1) / (2 * txq);                                             // 1,2:Constants in formulas
        s += 3 * (mu - 1) * (mu - 9) / (txq * txq * 8);                        // 3,1,9,8:Constants in formulas
        s += 15 * (mu - 1) * (mu - 9) * (mu - 25) / (txq * txq * txq * 8 * 6); // 15,1,9,25,8,6:Constants in formulas
    }
    return SqrtImpl(s * 2 / (ConstantsInternal::PI * x)); // 2:Constants in formulas    sqrt(2*s/(π*x))
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float JnYnAsymptoticBesselPhaseMx(int n, float x)
{
    float mu = 4 * n * n;
    float denom = 4 * x;
    float denomMult = denom * denom;
    float s = 0;
    // (4 * n^2 - 1) / (8 * x) + (4 * n^2 - 1) * (4 * n^2 - 25) / (384 * x^3) + (4 * n^2 - 1) * (16 * n^4 - 456 * n^2 +
    // 1073) / (5120 * x^5)
    s += (mu - 1) / (2 * denom); // 1,2:Constants in formulas
    denom *= denomMult;
    s += (mu - 1) * (mu - 25) / (6 * denom); // 1,25,6:Constants in formulas
    denom *= denomMult;
    s += (mu - 1) * (mu * mu - 114 * mu + 1073) / (5 * denom); // 1,114,1073,5:Constants in formulas
    return s;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float JnCase1(int n, float x)
{
    float ampl = JnYnAsymptoticBesselAmplitude(n, x, 0);
    float phase = JnYnAsymptoticBesselPhaseMx(n, x);
    float cx, sx, ci, si, cp, sp;
    SinCosImpl(x, sx, cx);
    float offset = (float)n / 2 + 0.25f;
    SinCospiImpl(offset, si, ci);
    SinCosImpl(phase, sp, cp);
    float sinPhase = cp * (cx * ci + sx * si) - sp * (sx * ci - cx * si);
    return sinPhase * ampl;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float JnCase2(int n, float x)
{
    float prev = J0Impl(x);
    float current = J1Impl(x);
    for (int k = 1; k < n; k++) {
        float value = (2 * k * current / x) - prev;
        prev = current;
        current = value;
    }
    return current;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float JnCase3(int n, float x)
{
    float prefix = n * LogImpl(x / 2);
    for (int i = 2; i < n + 1; i++) {
        prefix = prefix - LogImpl((float)i);
    }
    prefix = ExpImpl(prefix);
    float mult = x / 2;
    mult *= -mult;
    float term = 1;
    float res = 0;
    int case3K = 14;
    for (int i = 1; i < case3K + 1; i++) { // 1:Order of Series Expansion 14
        res += term;
        term *= mult / (i * (i + n));
    }
    return prefix * res;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float JnCase4(int n, float x)
{
    float maxValue = PowImpl(2.0f, 60.0f);
    int N = n + 30;
    float prev = 1e-30f;
    float current = 0;
    float s = 0;
    float scale = 1;
    float res;
    for (int k = N - 1; k >= 0; k--) {
        float fact = 2 * (k + 1) / x;
        if (fact > 1 && AbsImpl(current) > maxValue) {
            prev /= maxValue;
            s /= maxValue;
            scale /= maxValue;
            current /= maxValue;
        }
        float tmp = 2 * (k + 1) / x * current - prev;
        if (k % 2 == 0) { // 2:Used for summation:U(2i, x)
            s += 2 * tmp; // 2:coefficient of summation
        }
        if (k == n) {
            res = tmp / scale;
        }
        prev = current;
        current = tmp;
    }
    s -= current;
    res /= s;
    return res * scale;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float J0XLess8(float x)
{
    float d1 = x - 2.4048254f;
    d1 = d1 - -1.087059e-7f;
    float res = -1.026110251e-13f;
    res = FmaImpl(res, d1, 2.926116439e-12f);
    res = FmaImpl(res, d1, -6.819261288e-12f);
    res = FmaImpl(res, d1, -4.233725725e-10f);
    res = FmaImpl(res, d1, 5.903298799e-10f);
    res = FmaImpl(res, d1, 5.804848319e-08f);
    res = FmaImpl(res, d1, 1.808731234e-09f);
    res = FmaImpl(res, d1, -5.449918970e-06f);
    res = FmaImpl(res, d1, -7.343399316e-06f);
    res = FmaImpl(res, d1, 3.017029154e-04f);
    res = FmaImpl(res, d1, 7.739547690e-04f);
    res = FmaImpl(res, d1, -7.283461771e-03f);
    res = FmaImpl(res, d1, -2.666820378e-02f);
    res = d1 * res;
    float d2 = x - 5.520078f;
    d2 = d2 + 7.1934145e-8f;
    res = d2 * res;

    float d3 = x - 8.653728f;
    d3 = d3 - 3.8147791e-7f;
    res = d3 * res;
    return res;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float CalY0XLessdot5(float x)
{
    float part1 = 0.636619772367f * J0XLess8(x) * LogImpl(x);
    float part2 = FmaImpl(0.0007977247950890495f, x, -0.016524315326267768f);
    part2 = FmaImpl(part2, x, 0.0001196180186f);
    part2 = FmaImpl(part2, x, 0.17759110676f);
    part2 = FmaImpl(part2, x, 0.00000074368805978f);
    part2 = FmaImpl(part2, x, -0.07380430393219066f);
    return part1 + part2;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float CalY0XPart1(float x)
{
    float d1 = x - 0.893576980f;
    d1 = d1 + 1.33579787e-8f;
    float res = -4.485103697e-03f;
    res = FmaImpl(res, d1, 3.231012427e-02f);
    res = FmaImpl(res, d1, -1.014045593e-01f);
    res = FmaImpl(res, d1, 1.847167541e-01f);
    res = FmaImpl(res, d1, -2.253558856e-01f);
    res = FmaImpl(res, d1, 2.147535120e-01f);
    res = FmaImpl(res, d1, -1.959638733e-01f);
    res = FmaImpl(res, d1, 1.944536283e-01f);
    res = FmaImpl(res, d1, -2.040570397e-01f);
    res = FmaImpl(res, d1, 2.190628354e-01f);
    res = FmaImpl(res, d1, -2.261730477e-01f);
    res = FmaImpl(res, d1, 2.205519494e-01f);
    res = FmaImpl(res, d1, -4.920781667e-01f);
    res = FmaImpl(res, d1, 8.794208015e-01f);
    res = d1 * res;
    return res;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float CalY0XPart2(float x)
{
    float d3 = x - 7.08605099f;
    d3 = d3 - 7.30581178e-8f;
    float res = 8.146675423e-11f;
    res = FmaImpl(res, d3, 1.030741112e-09f);
    res = FmaImpl(res, d3, 1.610027889e-09f);
    res = FmaImpl(res, d3, 1.063494888e-08f);
    res = FmaImpl(res, d3, 6.693347461e-07f);
    res = FmaImpl(res, d3, 7.816005861e-07f);
    res = FmaImpl(res, d3, -4.836658731e-05f);
    res = FmaImpl(res, d3, 1.049324298e-05f);
    res = FmaImpl(res, d3, 2.142965752e-03f);
    res = FmaImpl(res, d3, -3.385610246e-03f);
    res = FmaImpl(res, d3, -3.743254148e-02f);
    res = FmaImpl(res, d3, 9.592770584e-02f);
    res = d3 * res;
    float d2 = x - 3.95767832f;
    d2 = d2 - 1.01291178e-7f;
    res = d2 * res;
    return res;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float CalY0XLarger8(float x)
{
    if (IsInfImpl(x)) {
        return 0.0f;
    }
    float invX = 1.0f / x;
    float invX2 = invX * invX;
    float beta = FmaImpl(5.848699569702148f, invX2, -0.5428466796875f);
    beta = FmaImpl(beta, invX2, 0.103515625f);
    beta = FmaImpl(beta, invX2, -0.0625f);
    beta = FmaImpl(beta, invX2, 1.0f);
    float alpha = FmaImpl(1.6380658830915178f, invX2, -0.2095703125f);
    alpha = FmaImpl(alpha, invX2, 0.06510416666666666f);
    alpha = FmaImpl(alpha, invX2, -0.125f);
    alpha = FmaImpl(alpha, invX, x);
    float theta = RsqrtImpl(x);
    theta = theta * 0.7978846f;
    float pre_coeff = beta * theta;
    float after_coeff = SinCosfMinusPIOverFour(alpha, 1); // 1:Calculate SinfMinusPIOverFour
    return after_coeff * pre_coeff;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float CalY1XLess1dot2(float x, float minus_two_over_pi_mul_inv_x)
{
    float part1 = 0.636619772367f * CalJ1XLess8(x) * LogImpl(x);
    float part2 = FmaImpl(0.0002798307076f, x, -0.0034028867918f);
    part2 = FmaImpl(part2, x, 0.0003643335439f);
    part2 = FmaImpl(part2, x, 0.0541922288594f);
    part2 = FmaImpl(part2, x, 0.00003339972037f);
    part2 = FmaImpl(part2, x, -0.1960600316f);
    part2 = FmaImpl(part2, x, 0.0000000624278f);

    return part1 + minus_two_over_pi_mul_inv_x + part2;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float CalY1XPart1(float x)
{
    float d1 = x - 2.19714141f;
    d1 = d1 + 8.28892723e-8f;
    float res = -7.210192066e-05f;
    res = FmaImpl(res, d1, 6.665645689e-05f);
    res = FmaImpl(res, d1, -3.106003176e-05f);
    res = FmaImpl(res, d1, 2.276838750e-04f);
    res = FmaImpl(res, d1, -5.566432475e-04f);
    res = FmaImpl(res, d1, 1.068050095e-03f);
    res = FmaImpl(res, d1, -2.582285756e-03f);
    res = FmaImpl(res, d1, 7.422557063e-03f);
    res = FmaImpl(res, d1, -4.799279782e-03f);
    res = FmaImpl(res, d1, -3.285740200e-02f);
    res = FmaImpl(res, d1, -1.185144993e-01f);
    res = FmaImpl(res, d1, 5.207864124e-01f);
    res = d1 * res;
    return res;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float CalY1XPart2(float x)
{
    float d2 = x - 5.42968082f;
    d2 = d2 - 2.16514351e-7f;
    float res = -4.575132868e-10f;
    res = FmaImpl(res, d2, 4.435273368e-09f);
    res = FmaImpl(res, d2, 3.963341878e-08f);
    res = FmaImpl(res, d2, -4.231424306e-07f);
    res = FmaImpl(res, d2, -4.201841643e-06f);
    res = FmaImpl(res, d2, 3.316061621e-05f);
    res = FmaImpl(res, d2, 2.516106023e-04f);
    res = FmaImpl(res, d2, -1.369325160e-03f);
    res = FmaImpl(res, d2, -8.495834725e-03f);
    res = FmaImpl(res, d2, 2.404736994e-02f);
    res = FmaImpl(res, d2, 1.074804589e-01f);
    res = d2 * res;
    float d3 = x - 8.59600544f;
    d3 = d3 - 4.28572861e-7f;
    res = d3 * res;
    return res;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float CalY1XLarger8(float x)
{
    if (IsInfImpl(x)) {
        return 0.0f;
    }
    float invX = 1.0f / x;
    float invX2 = invX * invX;
    float beta = FmaImpl(-7.739953994751f, invX2, 0.8052978515625f);
    beta = FmaImpl(beta, invX2, -0.193359375f);
    beta = FmaImpl(beta, invX2, 0.1875f);
    beta = FmaImpl(beta, invX2, 1.0f);
    float alpha = FmaImpl(-2.3693978445870534f, invX2, 0.3708984375f);
    alpha = FmaImpl(alpha, invX2, -0.1640625f);
    alpha = FmaImpl(alpha, invX2, 0.375f);
    alpha = FmaImpl(alpha, invX, x);
    float theta = RsqrtImpl(x);
    theta = theta * 0.7978846f;
    float pre_coeff = beta * theta;
    float after_coeff = SinCosfMinusPIOverFour(alpha, 0); // 0:Calculate CosfMinusPIOverFour
    return -after_coeff * pre_coeff;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float YnCase1(int n, float x)
{
    float lgammaN = LgammaImpl(n);
    float gammaN = ExpImpl(lgammaN);
    return -gammaN / ConstantsInternal::PI *
           PowImpl(2 / x, (float)n); // 2:Constants in formulas -(n - 1)! * (2 / x)^n / π
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float YnCase2(int n, float x)
{
    float ampl = JnYnAsymptoticBesselAmplitude(n, x, 1); // 1: Calculate YnAsymptoticBesselAmplitude
    float phase = JnYnAsymptoticBesselPhaseMx(n, x);
    float phaseShift = n * float(ConstantsInternal::PI) / 2 + float(ConstantsInternal::PI) / 4;
    float cosX = CosImpl(x);
    float sinX = SinImpl(x);
    float cosShift = CosImpl(phaseShift - phase);
    float sinShift = SinImpl(phaseShift - phase);
    float sinCombined = sinX * cosShift - cosX * sinShift;
    return sinCombined * ampl;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float YnCase3(int n, float x)
{
    float prev = Y0Impl(x);
    float current = Y1Impl(x);
    float scaleFactor = 1.0f;
    float invX = 2.0f / x;

    float mult = 0.0f;
    float value = 0.0f;
    float inv = 0.0f;
    int k = 1;
    for (; k + 2 < n; k += 3) { // 2,3: loop unrolling parameters
        mult = k * invX;
        value = mult * current - prev;
        prev = current;
        current = value;

        mult = (k + 1) * invX;
        value = mult * current - prev;
        prev = current;
        current = value;

        mult = (k + 2) * invX; // 2: offset for the third unrolled iteration
        value = mult * current - prev;
        prev = current;
        current = value;

        if (AbsImpl(mult) > 1.0f && AbsImpl(current) > 1.0f) {
            inv = 1.0f / current;
            prev *= inv;
            scaleFactor *= inv;
            value *= inv;
            current = 1.0f;
        }
    }
    while (k < n) {
        mult = k * invX;
        value = mult * current - prev;
        prev = current;
        current = value;
        k++;
    }
    return value / scaleFactor;
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T J0Impl(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type only supports float.");
    if (IsNanImpl(x)) {
        return ConstantsInternal::SIMT_FP32_INF / ConstantsInternal::SIMT_FP32_INF;
    }
    float f1 = AbsImpl(x);
    if (f1 > 1e13f && IsFiniteImpl(f1)) {
        return 0;
    }
    float res;
    if (f1 > 8.0f) {
        res = CalJ0XLarger8(f1);
    } else {
        res = CalJ0XLess8(f1);
    }
    return res;
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T J1Impl(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type only supports float.");
    if (IsNanImpl(x)) {
        return ConstantsInternal::SIMT_FP32_INF / ConstantsInternal::SIMT_FP32_INF;
    }
    float f1 = AbsImpl(x);
    if (f1 > 1e13f && IsFiniteImpl(f1)) {
        return 0;
    }
    float res;
    if (f1 > 8.0f) {
        res = CalJ1XLarger8(f1);
    } else {
        res = CalJ1XLess8(f1);
    }
    return (x < 0) ? -res : res;
}

template <typename T, typename U>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline U JnImpl(T n, U x)
{
    static_assert(SupportTypeSimtInternel<T, int>, "Input(n) type only supports int.");
    static_assert(SupportTypeSimtInternel<U, float>, "Input(x) type only supports float.");
    if (n == 0) {
        return J0Impl(x);
    }
    if (n == 1) {
        return J1Impl(x);
    }
    if (n < 0 || IsNanImpl(x)) {
        return ConstantsInternal::SIMT_FP32_INF / ConstantsInternal::SIMT_FP32_INF;
    }
    if (x == 0) {
        return 0;
    }
    if (IsInfImpl(x)) {
        return 0;
    }
    float res = 1;
    if (x < 0) {
        res *= (n & 1) ? -1 : 1;
        x = -x;
    }
    if (n < x * 0.1f) {
        res = res * JnCase1(n, x);
    } else if (n < x) {
        res *= JnCase2(n, x);
    } else if (n > x * x / 10) {
        res *= JnCase3(n, x);
    } else {
        res *= JnCase4(n, x);
    }
    return res;
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Y0Impl(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type only supports float.");
    if (x < 0 || IsNanImpl(x)) {
        return ConstantsInternal::SIMT_FP32_INF / ConstantsInternal::SIMT_FP32_INF;
    }
    float f1 = AbsImpl(x);
    if (f1 > 1e13f && IsFiniteImpl(f1)) {
        return 0;
    }
    float res;
    if (f1 < 0.5f) {
        res = CalY0XLessdot5(f1);
    } else if (f1 < 2.1971413260310170351f) {
        res = CalY0XPart1(f1);
    } else if (f1 < 8.0f) {
        res = CalY0XPart2(f1);
    } else {
        res = CalY0XLarger8(f1);
    }
    return res;
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Y1Impl(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type only supports float.");
    if (x < 0 || IsNanImpl(x)) {
        return ConstantsInternal::SIMT_FP32_INF / ConstantsInternal::SIMT_FP32_INF;
    }
    float f1 = AbsImpl(x);
    if (f1 > 1e13f && IsFiniteImpl(f1)) {
        return 0;
    }
    if (x == 0.0f) {
        return -ConstantsInternal::SIMT_FP32_INF;
    }
    float minus_two_over_pi_mul_inv_x = -0.636619772367f / f1;
    float res;
    if (f1 < 1.17549435e-38f) {
        res = minus_two_over_pi_mul_inv_x;
    }
    if (f1 < 1.2f) {
        res = CalY1XLess1dot2(f1, minus_two_over_pi_mul_inv_x);
    } else if (f1 < 3.0f) {
        res = CalY1XPart1(f1);
    } else if (f1 < 8.0f) {
        res = CalY1XPart2(f1);
    } else {
        res = CalY1XLarger8(f1);
    }
    return res;
}

template <typename T, typename U>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline U YnImpl(T n, U x)
{
    static_assert(SupportTypeSimtInternel<T, int>, "Input(n) type only supports int.");
    static_assert(SupportTypeSimtInternel<U, float>, "Input(x) type only supports float.");
    if (n < 0 || x < 0 || IsNanImpl(x)) {
        return ConstantsInternal::SIMT_FP32_INF / ConstantsInternal::SIMT_FP32_INF;
    }
    if (x == 0) {
        return -ConstantsInternal::SIMT_FP32_INF;
    }
    if (IsPositiveInfImpl(x)) {
        return 0;
    }
    if (n == 0) {
        return Y0Impl(x);
    }
    if (n == 1) {
        return Y1Impl(x);
    }

    float largeXThreshold = n * 10;
    float smallXThreshold = 1e-8f;

    if (x < smallXThreshold) {
        return YnCase1(n, x);
    }
    if (x > largeXThreshold) {
        return YnCase2(n, x);
    }

    return YnCase3(n, x);
}

} // namespace Simt
} // namespace AscendC
#endif // IMPL_SIMT_API_CPP_DAV_C310_KERNEL_SIMT_BESSEL_IMPL_H
