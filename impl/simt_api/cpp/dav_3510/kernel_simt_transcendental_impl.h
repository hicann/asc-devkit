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
 * \file kernel_simt_transcendental_impl.h
 * \brief
 */
#ifndef IMPL_SIMT_API_CPP_DAV_C310_KERNEL_SIMT_TRANSCENDENTAL_IMPL_H
#define IMPL_SIMT_API_CPP_DAV_C310_KERNEL_SIMT_TRANSCENDENTAL_IMPL_H

#if defined(ASCENDC_CPU_DEBUG)
#include <cmath>

#include "../../../basic_api/kernel_utils.h"
#include "stub_def.h"
#endif

#include "impl/simt_api/cpp/dav_3510/kernel_simt_cmp_impl.h"
#include "impl/simt_api/cpp/dav_3510/kernel_simt_math_impl.h"

namespace AscendC {
namespace Simt {

#if defined(ASCENDC_CPU_DEBUG)
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T ExpImpl(T x)
{
    return exp(x);
}
#else
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T ExpImpl(T x)
{
    return __expf(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 ExpImpl(half2 x) { return __exp(x); }
#endif

/**
 * Performs Payne-Hanek radian reduction for trigonometric functions.
 * Reduce the input angle to the range [0, pi/2) and determine the quadrant.
 *
 * @param x The input angle in radians.
 * @param outputQuadrant Pointer to store the quadrant information.
 * @return The reduced angle in the range [0, pi/2).
 */
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float PayneHanekRadianReduction(float x, int* outputQuadrant)
{
    // Step 1: Extract raw bits of the input angle
    uint32_t inputBits = reinterpret_cast<uint32_t&>(x);

    // Step 2: Extract exponent and compute index into 2/pi table
    int32_t exponent = ((inputBits & 0x7F800000) >> 23) - 127;
    uint32_t exponentIndex = static_cast<uint32_t>(exponent) >> 5;

    // Step 3: Get the 2/pi table entries for this exponent index
    constexpr uint32_t twoOverPiTable[] = {0x517cc1b7, 0x27220a94, 0xfe13abe8, 0xfa9a6ee0, 0x6db14acc, 0x9e21c820};
    uint32_t highTerm = exponentIndex ? twoOverPiTable[exponentIndex - 1] : 0;
    uint32_t midTerm = twoOverPiTable[exponentIndex];
    uint32_t lowTerm = twoOverPiTable[exponentIndex + 1];
    uint32_t lastTerm = twoOverPiTable[exponentIndex + 2];

    // Step 4: Compute exponent remainder and shift table entries accordingly
    int32_t exponentRemainder = static_cast<uint32_t>(exponent) & 0x1F;
    if (exponentRemainder != 0) {
        highTerm =
            (highTerm << exponentRemainder) | (midTerm >> (ConstantsInternal::FOUR_BYTE_LEN - exponentRemainder));
        midTerm = (midTerm << exponentRemainder) | (lowTerm >> (ConstantsInternal::FOUR_BYTE_LEN - exponentRemainder));
        lowTerm = (lowTerm << exponentRemainder) | (lastTerm >> (ConstantsInternal::FOUR_BYTE_LEN - exponentRemainder));
    }

    // Step 5: Extract and normalize the mantissa
    uint32_t mantissa = (inputBits & 0x007FFFFF) | 0x4F000000;
    uint32_t normalizedMantissa = static_cast<uint32_t>(reinterpret_cast<float&>(mantissa));

    // Step 6: Compute product = (mantissa * highTerm) << 32 + mantissa * midTerm + mantissa * lowTerm
    uint64_t product = static_cast<uint64_t>(normalizedMantissa) * lowTerm;
    product = static_cast<uint64_t>(normalizedMantissa) * midTerm + (product >> ConstantsInternal::FOUR_BYTE_LEN);
    product = (static_cast<uint64_t>(normalizedMantissa * highTerm) << ConstantsInternal::FOUR_BYTE_LEN) + product;

    // Step 7: Extract quotient and remainder
    int32_t quotient = static_cast<int32_t>(product >> 62);
    product = product & 0x3FFFFFFFFFFFFFFFULL;

    // Step 8: Handle carry
    if (product & 0x2000000000000000ULL) {
        product -= 0x4000000000000000ULL;
        quotient += 1;
    }

    // Step 9: Split product into high and low
    int64_t productInt64 = static_cast<int64_t>(product);
    int64_t highFloat = static_cast<float>(productInt64);
    productInt64 = productInt64 - static_cast<int64_t>(highFloat);
    int64_t lowFloat = static_cast<float>(productInt64);

    // Step 10: Compute final result = (high + low) * pi/2 * 2^-62
    float piOverTwoLow = 3.4061215800865545e-19f; // pi/2 * 2^-62
    float reducedAngle = (highFloat + lowFloat) * piOverTwoLow;

    // Step 11: Handle negative input
    if (x < 0.0f) {
        reducedAngle = -reducedAngle;
        quotient = -quotient;
    }

    // Step 12: Return result
    *outputQuadrant = quotient;
    return reducedAngle;
}

/**
 * Performs Cody-Waite radian reduction for trigonometric functions.
 * Reduce the input angle using a simpler method compared to Payne-Hanek.
 *
 * @param x The input angle in radians.
 * @param quadrant Pointer to store the quadrant information.
 * @return The reduced angle in the range [0, pi/2).
 */
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float CodyWaiteRadianReduction(float x, int* quadrant)
{
    float y = FmaImpl(x, 0.636619747f, 12582912.0f); // 0.636619747f: 2/pi
    *quadrant = reinterpret_cast<int&>(y);
    y = y - 12582912.0f;                    // 12582912.0f: used to truncate mantissa of x*(2/pi)
    x = FmaImpl(y, -1.57079601e+00f, x);    // 1.57079601e+00f: high of pi/2
    x = FmaImpl(y, -3.13916473e-07f, x);    // 3.13916473e-07f: middle of pi/2
    return FmaImpl(y, -5.39030253e-15f, x); // 5.39030253e-15f: low of pi/2
}

/**
 * Determines the appropriate radian reduction method based on the input angle's magnitude.
 * Uses Payne-Hanek for large angles and Cody-Waite for small angles.
 *
 * @param x The input angle in radians.
 * @param threshold The threshold to decide between Payne-Hanek and Cody-Waite.
 * @param quadrant Pointer to store the quadrant information.
 * @return The reduced angle in the range [0, pi/2).
 */
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float TrigRadianReduction(float x, float threshold, int* quadrant)
{
    x = FmaImpl(x, 0.0f, x);
    if (AbsImpl(x) > threshold) {
        return PayneHanekRadianReduction(x, quadrant);
    } else {
        return CodyWaiteRadianReduction(x, quadrant);
    }
}

/**
 * Computes the cosine of an angle using a polynomial approximation.
 * Formula: cos(x) = 1-(1/2!)x^2+(1/4!)x^4+...+((-1)^n/(2n)!)x^2n+O(x^{2n+2})
 * The input angle should be in the range [-pi/2, pi/2].
 *
 * @param x The input angle in radians.
 * @return The cosine of the input angle.
 */
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float CosPoly(float x)
{
    x = x * x;
    float y = FmaImpl(x, 2.44677067e-5f, -1.38877297e-3f); // 2.44677067e-5f: 1/8! -1.38877297e-3f: -1/6!
    y = FmaImpl(x, y, 4.16666567e-2f);                     //  4.16666567e-2f: 1/4!
    y = FmaImpl(x, y, -5.00000000e-1f);                    // -5.00000000e-1f: -1/2!
    return FmaImpl(x, y, 1.00000000e+0f);                  //  1.00000000e+0f: 1
}

/**
 * Computes the sine of an angle using a polynomial approximation.
 * Formula: sin(x) = x-(1/3!)x^3+(1/5!)x^5+...+((-1)^n/(2n+1)!)x^{2n+1}+O(x^{2n+3})
 * The input angle should be in the range [-pi/2, pi/2].
 *
 * @param x The input angle in radians.
 * @return The sine of the input angle.
 */
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float SinPoly(float x)
{
    float y = x * x;
    float m = FmaImpl(x, y, 0.0f);

    float z = FmaImpl(y, 2.86567956e-6f, -1.98559923e-4f); //  2.86567956e-6f:  1/9! * x^2 -1.98559923e-4f: -1/7!
    z = FmaImpl(y, z, 8.33338592e-3f);                     // 8.33338592e-3f: 1/5! * x^2
    z = FmaImpl(y, z, -1.66666672e-1f);                    // -1.66666672e-1f: -1/3! * x^2

    return FmaImpl(z, m, x); // * x^3 + x
}

/**
 * Computes the cosine of an angle using trigonometric identities and polynomial approximations.
 * Handles angles in the full range by reducing them to [-pi/2, pi/2] and applying identities.
 *
 * @param x The input angle in radians.
 * @return The cosine of the input angle.
 * Special cases:
 *    if x is NaN, return NaN;
 *    if x is inf, return NaN;
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CosImpl(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type of input only supports float.");

    // Step 1: Reduce the angle to the range [0, pi/2) and determine the quadrant
    int quadrant;
    float y = TrigRadianReduction(x, 71476.0625f, &quadrant); // 71476.0625f: Threshold for reduction algorithm

    // Step 2: Compute cosine and sine of the reduced angle using polynomial approximations
    float c = CosPoly(y);
    float s = SinPoly(y);

    // Step 3: Adjust the cosine value based on the quadrant
    if (quadrant & 2) { // Quadrants 2 and 3: cos(pi + x) = -cos(x)
        s = -s;
        c = -c;
    }
    if (quadrant & 1) { // Quadrants 1 and 3: cos(pi/2 + x) = -sin(x)
        c = -s;
    }

    // Return the final cosine value
    return c;
}

/**
 * Computes the sine of an angle using trigonometric identities and polynomial approximations.
 * Handles angles in the full range by reducing them to [-pi/2, pi/2] and applying identities.
 *
 * @param x The input angle in radians.
 * @return The sine of the input angle.
 * Special cases:
 *    if x is NaN, return NaN;
 *    if x is inf, return NaN;
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T SinImpl(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type of input only supports float.");

    // Step 1: Reduce the angle to the range [0, pi/2) and determine the quadrant
    int quadrant;
    float y = TrigRadianReduction(x, 71476.0625f, &quadrant); // 71476.0625f: Threshold for reduction algorithm

    // Step 2: Compute cosine and sine of the reduced angle using polynomial approximations
    float c = CosPoly(y);
    float s = SinPoly(y);

    // Step 3: Adjust the sine value based on the quadrant
    if (quadrant & 2) { // Quadrants 2 and 3: sin(pi + x) = -sin(x)
        s = -s;
        c = -c;
    }
    if (quadrant & 1) { // Quadrants 1 and 3: sin(pi/2 + x) = cos(x)
        s = c;
    }

    return s;
}

/**
 * Computes both sine and cosine of an angle using trigonometric identities and polynomial approximations.
 * Handles angles in the full range by reducing them to [-pi/2, pi/2] and applying identities.
 *
 * @param x The input angle in radians.
 * @param s Reference to store the sine of the input angle.
 * @param c Reference to store the cosine of the input angle.
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void SinCosImpl(T x, T& s, T& c)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type of input only supports float.");

    int quadrant;
    float t;
    float y = TrigRadianReduction(x, 71476.0625f, &quadrant); // 71476.0625f: threshold for reduce algorithm
    float cos = CosPoly(y);
    float sin = SinPoly(y);
    if (quadrant & 2) { // 2: sin(pi+x) = -sin(x), cos(pi+x) = -cos(x)
        sin = -sin;
        cos = -cos;
    }
    if (quadrant & 1) { // 1: sin(pi/2+x) = cos(x), cos(pi/2+x) = -sin(x)
        t = -sin;
        sin = cos;
        cos = t;
    }
    s = sin;
    c = cos;
}

/**
 * Computes the tangent of an angle using a polynomial approximation.
 * The input angle should be in the range [-pi/2, pi/2].
 * Formula: tanx=x+(1/3)*x^3+(2/15)*x^5+(17/315)*x^7+(62/2835)*x^9+(1382/155925)*x^11+
 *           +(21844/6081075)*x^13+(929569/638512875)*x^15+...+O(...)
 *
 * @param x The input angle in radians.
 * @return The tangent of the input angle.
 */
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float TanPoly(float x)
{
    x = x * x;
    float y = FmaImpl(x, 4.38117981e-3f, 8.94600598e-5f); // 4.38117981e-3f: 8.94600598e-5f:
    y = FmaImpl(x, y, 1.08341556e-2f);                    // 1.08341556e-2f:
    y = FmaImpl(x, y, 2.12811474e-2f);                    // 2.12811474e-2f: 62/2838
    y = FmaImpl(x, y, 5.40602170e-2f);                    // 5.40602170e-2f: 17/315
    y = FmaImpl(x, y, 1.33326918e-1f);                    // 1.33326918e-1f: 2/15
    y = FmaImpl(x, y, 3.33333433e-1f);                    // 3.33333433e-1f: 1/3
    return x * y;
}

/**
 * Computes the tangent of an angle using trigonometric identities and polynomial approximations.
 * Handles angles in the full range by reducing them to [-pi/2, pi/2] and applying identities.
 *
 * @param x The input angle in radians.
 * @return The tangent of the input angle.
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T TanImpl(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type of input only supports float.");

    // Step 1: Reduce the angle to the range [0, pi/2) and determine the quadrant
    int quadrant;
    float y = TrigRadianReduction(x, 252.898206f, &quadrant); // 252.898206f: Threshold for reduction algorithm

    // Step 2: Compute the tangent using polynomial approximation
    float t = TanPoly(y);

    // Step 3: Compute the initial approximation of tan(y)
    float z = FmaImpl(t, y, y);

    // Step 4: Adjust the tangent value based on the quadrant
    if (quadrant & 1) { // Quadrants 1 and 3: tan(pi/2 + x) = -cot(x)
        float s = y - z;
        s = FmaImpl(t, y, s);
        t = -1.0f / z;
        z = FmaImpl(z, t, 1.0f);
        z = FmaImpl(s, t, z);
        z = FmaImpl(z, t, t);
    }

    return z;
}

#if defined(ASCENDC_CPU_DEBUG)
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float TanhImpl(float x) { return tanh(x); }
#else
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float TanhImpl(float x) { return 1.0f - (2.0f / (ExpImpl(2.0f * x) + 1.0f)); }
#endif

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float TanPiImpl(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type of input only supports float.");
    return TanImpl(x * ConstantsInternal::PI);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void TaylorExpand(
    float& dst, float& src, float& squareV, uint32_t expandLevel, float* factor)
{
    squareV = src * src;
    dst = src * src;
    dst = dst * factor[expandLevel];
    for (int i = expandLevel - 1; i > 0; i--) {
        dst = dst + factor[i];
        dst = dst * squareV;
    }
    dst = dst + factor[0];
    dst = dst * src;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void TaylorExpand(float& dst, float& src, float& squareV, uint32_t expandLevel)
{
    float factor[] = {1,
                      -0.3333333333333333,
                      0.2,
                      -0.14285714285714285,
                      0.1111111111111111,
                      -0.09090909090909091,
                      0.07692307692307693};
    TaylorExpand(dst, src, squareV, expandLevel, factor);
}

#if defined(ASCENDC_CPU_DEBUG)
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float AtanImpl(float x) { return atan(x); }
#else
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void AtanExpand(float& dst, float& src, float& tmp, float transFactor)
{
    dst = src * transFactor;
    dst = dst + 1.0f;
    tmp = src - transFactor;
    dst = tmp / dst;
    dst = AbsImpl(dst);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline void Sign(float& dst, float& src, float& denominator)
{
    dst = src * 4611686018427387904.0f; // 4611686018427387904 : ATAN_FP32_MAX
    denominator = AbsImpl(dst);
    denominator = denominator + 2.168404344971009e-19f; // 2.168404344971009e-19 : ATAN_FP32_MIN
    dst = dst / denominator;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float AtanImpl(float x)
{
    float clip = MinImpl(x, 10000.0f); // 10000 : MAX_INPUT_VALUE
    clip = MaxImpl(clip, -10000.0f);   // -10000 : MIN_INPUT_VALUE
    float absV = AbsImpl(clip);

    float dst = 0;
    float squareV = 0;
    float tmp = 0;
    float tmp2 = 0;

    TaylorExpand(dst, absV, squareV, 4);             // 4 : Taylor expansion count
    AtanExpand(tmp, absV, tmp2, 0.4142135623730950); // 0.4142135623730950 : TAN_PI_OF_8
    TaylorExpand(tmp2, tmp, squareV, 4);             // 4 : Taylor expansion count

    tmp2 = tmp2 + ConstantsInternal::PI_OF_8;
    dst = MinImpl(dst, tmp2);

    tmp2 = absV + 1.0f;
    tmp = absV - 1.0f;
    tmp = tmp / tmp2;
    tmp = AbsImpl(tmp);

    TaylorExpand(tmp2, tmp, squareV, 4); // 4 : Taylor expansion count
    tmp2 = tmp2 + ConstantsInternal::PI_OF_4;
    dst = MinImpl(dst, tmp2);

    AtanExpand(tmp2, tmp, squareV, 0.4142135623730950); // 0.4142135623730950 : TAN_PI_OF_8
    TaylorExpand(tmp, tmp2, squareV, 6);                // 6 : Taylor expansion count

    tmp = tmp + ConstantsInternal::PI_OF_8;
    tmp = tmp + ConstantsInternal::PI_OF_4;
    dst = MinImpl(dst, tmp);

    Sign(tmp, clip, tmp2);

    dst = dst * tmp;
    return dst;
}
#endif

#if defined(ASCENDC_CPU_DEBUG)
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float Atan2Impl(float y, float x) { return atan2(y, x); }
#else
/*
atan2(y, x) =
atan(y/x)       x > 0
atan(y/x) + PI  y >= 0, x < 0
atan(y/x) - PI  y < 0, x < 0
PI/2            y > 0, x = 0
-PI/2           y < 0, x = 0
0               y = 0, x = 0
*/
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float Atan2Impl(float y, float x)
{
    if (IsNanImpl(y)) {
        return y;
    } else if (IsNanImpl(x)) {
        return x;
    }

    int d = (y >= 0) ? 1 : -1;

    if (IsInfImpl(y) && IsInfImpl(x)) {
        int s = 1;
        if (x < 0) {
            s = 3; // 3 : ATAN2_THREE
        }
        return d * ConstantsInternal::PI_OF_4 * s;
    } else if (IsInfImpl(y)) {
        return d * ConstantsInternal::PI_OF_2;
    } else if (IsInfImpl(x)) {
        if (x > 0) {
            d = 0;
        }
        return d * ConstantsInternal::PI;
    }

    if (x == 0) {
        if (y == 0) {
            d = 0;
        }
        return d * ConstantsInternal::PI_OF_2;
    } else if (x > 0) {
        d = 0;
    }

    return AtanImpl(y / x) + d * ConstantsInternal::PI;
}
#endif

#if defined(ASCENDC_CPU_DEBUG)
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T LogImpl(T x)
{
    return logf(x);
}
#else
// f32/f16
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T LogImpl(T x)
{
    if (x > 0.0f && x < 1.17549435e-38f) { // 1.17549435e-38f: subnormal floating-point number boundary
        return __logf(ExpImpl(23.0f) * x) - 23.0f;
    }

    return __logf(x);
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline half2 LogImpl(half2 x) { return __log(x); }
#endif

#if defined(ASCENDC_CPU_DEBUG)
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float AtanhImpl(float x) { return atanh(x); }
#else
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float AtanhImpl(float x) { return LogImpl((1.0f + x) / (1.0f - x)) / 2.0f; }
#endif

/**
 * Rquare root of X
 * Formula:Sqrt(x) = sqrt(x)
 * @param x a value
 * @return SqrtImpl(x)
 * Special cases:
 *    if x is NaN, return NaN;
 *    if x is inf, return inf;
 *    if x is -inf, return NaN;
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T SqrtImpl(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type of input only supports float.");
#if defined(ASCENDC_CPU_DEBUG)
    return sqrt(x);
#else
    return __sqrtf(x);
#endif
}

/**
 * Reciprocal of Sqrt(x)
 * Formula:Rsqrt(x) = 1 / sqrt(x)
 * @param x a value
 * @return RsqrtImpl(x)
 * Special cases:
 *    if x is NaN, return NaN;
 *    if x is inf, return 0;
 *    if x is -inf, return NaN;
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T RsqrtImpl(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type of input only supports float.");
    return 1.0f / SqrtImpl(x);
}

/**
 * * Computes cosh values based on input.
 * * Formula: cosh(x) = (e^x+e^(-x))/2 = e^(x-ln2) + 0.25/(e^(x-ln2)).
 * *
 * * @param x
 * * @return Cosh(x)
 * *  Special cases:
 * *      if x is NaN, return NaN;
 * *      if x = 89.14 y = 2.5821419964548915e+38, exp(89.14-Ln(2)) is limit overflow in 32
 * *      if x = -89.14 y = 2.5821419964548915e+38, exp(-89.14-Ln(2)) limit 0 in 32 bit,
 * *           0.25/exp(-89.14-Ln(2)) is overflow in 32 bit
 * *           use Cosh(x) = Cosh(-x) to solve exp(-89.14-Ln(2)) overflow
 * */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CoshImpl(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type of input only supports float.");
    float y = AbsImpl(x);
    const float tmp = ExpImpl(y - ConstantsInternal::SCALAR_LN2);
    return tmp + 0.25f / tmp;
}

/**
 * Computes cospi values based on input.
 * Formula: cospi(x) = cos(x*PI)
 *
 * @param x
 * @return Cospi(x)
 *  Special cases:
 *      if x is NaN, return NaN;
 *      if x is Inf, return NaN;
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CospiImpl(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type of input only supports float.");
    return CosImpl(x * ConstantsInternal::PI);
}

/**
 * Computes cos values based on input.
 * Formula:
 * asin(x) = asin(sqrt(1-x^2)) - pi/2, x belongs to (-1, -2^(-0.5))
 * asin(x) = the  taylor expansion, x belongs to (-2^(-0.5), 2^(-0.5))
 * asin(x) = pi/2 -asin(sqrt(1-x^2)), x belongs to (2^(-0.5), 1)
 * taylor :   (((k_nx^2 + k_n) * x^2 + k_(n-1)) * x^2 +k_(n-2) ……)*x^2 +k_0)*x.
 *   if x in [-1, 1]
 *       x >= -1 && x < 0 && x^2 > 0.5f => x belongs to (-1, -2^(-0.5))
 *       x > 0 && x <= 1 && x^2 > 0.5f => x belongs to (2^(-0.5), 1)
 *       other, x belongs to (-2^(-0.5), 2^(-0.5))
 *
 * @param x
 * @return Asin(x)
 * Special cases:
 *      if x is NaN, return x itself;
 *      if x is inf, return NaN;
 *      if |x|>1, return NaN with invalid signal.
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AsinImpl(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type of input only supports float.");
    if (AbsImpl(x) > 1) {
        return ConstantsInternal::SIMT_FP32_INF / ConstantsInternal::SIMT_FP32_INF;
    }
    float squareV = 0;
    float dst = 0;
    float src = x;
    float factor[] = {
        1.0,
        0.16666666666666666666666666666667,
        0.075,
        0.04464285714285714285714285714286,
        0.03038194444444444444444444444444,
        0.02237215909090909090909090909091,
        0.01735276442307692307692307692308,
        0.01396484375,
    };
    if (AbsImpl(x) <= 0.7071067811865476f) {        // 0.7071067811865476 : SCALAR_ACOS_MAX_LIMIT
        TaylorExpand(dst, src, squareV, 7, factor); // 7 : Taylor expansion count
        return dst;
    } else if (x < -0.7071067811865476f) { // -0.7071067811865476 : SCALAR_ACOS_MIN_LIMIT
        src = SqrtImpl(1.0f - x * x);
        TaylorExpand(dst, src, squareV, 7, factor); // 7 : Taylor expansion count
        return dst - ConstantsInternal::PI_OF_2;
    } else {
        src = SqrtImpl(1.0f - x * x);
        TaylorExpand(dst, src, squareV, 7, factor); // 7 : Taylor expansion count
        return ConstantsInternal::PI_OF_2 - dst;
    }
}

/**
 * Computes Acos values based on input.
 * Formula: Acos(x) = pi/2-Asin(x).
 *
 * @param x
 * @return Acos(x)
 * Special cases:
 *      if x is NaN, return x itself;
 *      if x is inf, return NaN;
 *      if |x|>1, return NaN.
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AcosImpl(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type of input only supports float.");
    return ConstantsInternal::PI_OF_2 - AsinImpl(x);
}

/**
 * Computes Acosh values based on input.
 * Formula:acosh(x) = ln(x + sqrt(x^2 - 1))
 *
 * @param x
 * @return Acosh(x)
 * Special cases:
 *      if x is NaN, return x itself;
 *      if x<-1, return NaN.
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AcoshImpl(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type of input only supports float.");
    if (x < 1) {
        return ConstantsInternal::SIMT_FP32_INF / ConstantsInternal::SIMT_FP32_INF;
    }
    return LogImpl(x + SqrtImpl(x * x - 1.0f));
}

/**
 * Computes Sinh values based on input.
 * Formula: sinh(x) = (e^x-e^(-x))/2 = e^(x-ln2) - 0.25/(e^(x-ln2)).
 *
 * @param x
 * @return Sinh(x)
 *  Special cases:
 *      if x is NaN, return NaN;
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T SinhImpl(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type of input only supports float.");
    if (AbsImpl(x) > 0.1f) {
        return ExpImpl(x - ConstantsInternal::SCALAR_LN2) - ExpImpl(x * (-1.0f) - ConstantsInternal::SCALAR_LN2);
    } else {
        float squareV = 0;
        float dst = 0;
        float src = x;
        float factor[] = {
            1.0,
            0.16666666666666666666666666666667,
            0.00833333333333333333333333333333,
            0.0001984126984126984,
            2.7557319223985893e-06,
            2.505210838544172e-08};
        TaylorExpand(dst, src, squareV, 5, factor); // 5: Taylor expansion count
        return dst;
    }
}

/**
 * Computes Sinpi values based on input.
 * Formula: sinpi(x) = sin(x*PI)
 *
 * @param x
 * @return Sinpi(x)
 *  Special cases:
 *      if x is NaN, return NaN;
 *      if x is Inf, return NaN;
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T SinpiImpl(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type of input only supports float.");
    return SinImpl(x * ConstantsInternal::PI);
}

/**
 * Computes Asinh values based on input.
 * Formula:asinh(x) = ln(x + sqrt(x^2 + 1))
 * asinh(x) = -asinh(-x)
 *
 * @param x
 * @return Asinh(x)
 * Special cases:
 *      if x is NaN, return x itself;
 *      if inf, return inf.
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T AsinhImpl(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type of input only supports float.");
    if (AbsImpl(x) > 0.1f) {
        return x > 0 ? LogImpl(x + SqrtImpl(x * x + 1.0f)) : LogImpl(SqrtImpl(x * x + 1.0f) - x) * (-1);
    } else {
        float squareV = 0;
        float dst = 0;
        float src = x;
        float factor[] = {
            1.0,
            -0.16666666666666666666666666666667,
            0.075,
            -0.04464285714285714285714285714286,
            0.03038194444444444444444444444444,
            -0.02237215909090909090909090909091,
            0.01735276442307692307692307692308,
            -0.01396484375,
        };
        TaylorExpand(dst, src, squareV, 7, factor); // 7 : Taylor expansion count
        return dst;
    }
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline void SinCospiImpl(T x, T& s, T& c)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type of input only supports float.");
    return SinCosImpl(x * ConstantsInternal::PI, s, c);
}

/**
 * Computes Hypot values based on input.
 * Formula:HypotImpl(x, y) = sqrt(x^2 + y^2)
 *
 * @param x, y
 * @return HypotImpl(x, y)
 * Special cases:
 *      HypotImpl(x,y) is INF if x or y is +INF or -INF; else
 *      HypotImpl(x,y) is NAN if x or y is NAN.
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T HypotImpl(T x, T y)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type of input only supports float.");
    float absX = AbsImpl(x);
    float absY = AbsImpl(y);
    if (IsPositiveInfImpl(absX) || IsPositiveInfImpl(absY)) {
        return ConstantsInternal::SIMT_FP32_INF;
    }
    if (IsNanImpl(absX)) {
        return absX;
    }
    if (IsNanImpl(absY)) {
        return absY;
    }
    float a = MaxImpl(absX, absY);
    float b = MinImpl(absX, absY);
    if (b == 0.0f) {
        return a;
    }
    float r = b / a;
    return a * SqrtImpl(FmaImpl(r, r, 1.0f));
}

/**
 * Computes Rhypot values based on input.
 * Formula:RhypotImpl(x, y) = 1 / sqrt(x^2 + y^2) = 1 / HypotImpl(x, y)
 *
 * @param x a value
 * @param y a value
 * @return RhypotImpl(x, y)
 * Special cases:
 *      RhypotImpl(x,y) is 0 if x or y is +INF or -INF; else
 *      RhypotImpl(x,y) is NAN if x or y is NAN.
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T RhypotImpl(T x, T y)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type of input only supports float.");
    return 1.0f / HypotImpl(x, y);
}

/**
 * Break VALUE into a normalized fraction and an integral power of 2.
 * Formula:x = Frexp(x, exp) * 2^exp
 *
 * @param x a value
 * @param &exp a value
 * @return FrexpImpl(x, &exp)
 * Special cases:
 *      if x is NaN, return x itself, exp=0;
 *      if x is inf, return x itself, exp=0;
 *      if x is -inf, return x itself, exp=0;
 */
template <typename T1, typename T2>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T1 FrexpImpl(T1 x, T2& exp)
{
    static_assert(SupportTypeSimtInternel<T1, float>, "Input type of input(x) only supports float.");
    static_assert(SupportTypeSimtInternel<T2, int>, "Input type of input(exp) only supports int.");
    if (x == 0.0f || IsPositiveInfImpl(AbsImpl(x)) || IsNanImpl(x)) {
        exp = 0;
        return x;
    }
    uint32_t u32 = reinterpret_cast<uint32_t&>(x);
    int32_t exponent = u32 & 0x7f800000; // 0x7f800000: get exponent
    int32_t f32ExpVal = exponent >> 23;  // 23: mantissa bit count
    uint32_t manU32 = u32 & 0x007fffff;  // 0x007fffff: get mantissa
    float f32ManU32 = static_cast<float>(manU32);
    f32ManU32 = f32ManU32 / (1 << 23); // 23: mantissa bit count
    if (f32ExpVal == 0) {
        if (f32ManU32 < 0.5f) {
            while (f32ManU32 < 0.5f) {
                f32ManU32 = f32ManU32 * 2;
                f32ExpVal--;
            }
        }
    } else {
        f32ManU32 = f32ManU32 / 2 + 0.5f;
    }
    exp = f32ExpVal - 126; // 126: subnormal float exp
    return CopySignImpl(f32ManU32, x);
}

/**
 * X times (two to the EXP power).
 * Formula:Ldexp(x, exp) = x * 2^exp
 * @param x a value
 * @param exp  a value
 * @return LdexpImpl(x, &exp)
 * Special cases:
 *      if x is NaN, return x itself;
 *      if x is inf, return x itself;
 *      if x is -inf, return x itself;
 */
template <typename T1, typename T2>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T1 LdexpImpl(T1 x, T2 exp)
{
    static_assert(SupportTypeSimtInternel<T1, float>, "Input type of input only supports float.");
    static_assert(SupportTypeSimtInternel<T2, int>, "Input type of input(exp) only supports int.");

    if (x == 0.0f || IsPositiveInfImpl(AbsImpl(x)) || IsNanImpl(x) || exp == 0) {
        return x;
    }
    if (exp > 280) { // 280: 1e-45*(2^280) = inf
        return CopySignImpl(ConstantsInternal::SIMT_FP32_INF, x);
    }
    if (exp < -280) { // -280: 3.4028234e+38*(2^-280) = 0
        return CopySignImpl(0.0f, x);
    }
    int32_t shift = 30;
    if (exp > 0) {
        while (exp > shift) {
            x *= (1 << shift);
            exp -= shift;
        }
        x *= (1 << exp);
    } else {
        while (exp < -30) { // -30: exp < -30, move 30
            x *= 1.0f / (1 << shift);
            exp += shift;
        }
        x *= 1.0f / (1 << (-exp));
    }
    return x;
}

/**
 * Calculate the square root of the sum of squares of 3D coordinates.
 * Formula:Norm3dImpl(a, b, c) = sqrt(a^2 + b^2 + c^2)
 *                                       = max(a,b,c)*sqrt({a/max}^2 + {b/max}^2 + {c/max}^2)
 *
 * @param a  float value
 * @param b  float value
 * @param c  float value
 * @return Norm3dImpl(a, b, c)
 * Special cases:
 *      If any one of a,b,c is ±INF, return INF.
 *      If any one of a,b,c is NAN and other is not ±INF, return NAN.
 *      If all of a,b,c is 0, return 0.
 *      If sqrt(a^2 + b^2 + c^2) overflows, return INF.
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Norm3dImpl(T a, T b, T c)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type only supports float.");

    if (IsInfImpl(a) || IsInfImpl(b) || IsInfImpl(c)) {
        return ConstantsInternal::SIMT_FP32_INF;
    }
    if (IsNanImpl(a) || IsNanImpl(b) || IsNanImpl(c)) {
        return ConstantsInternal::SIMT_FP32_INF / ConstantsInternal::SIMT_FP32_INF;
    }
    float m = MaxImpl(AbsImpl(a), AbsImpl(b));
    m = MaxImpl(m, AbsImpl(c));
    if (m == 0.0f) {
        return 0.0f;
    }
    float r = 0.0f;
    r = FmaImpl((a / m), (a / m), r);
    r = FmaImpl((b / m), (b / m), r);
    r = FmaImpl((c / m), (c / m), r);
    return m * SqrtImpl(r);
}

/**
 * Calculate the reciprocal of the square root of the sum of squares of 3D coordinates.
 * Formula:Rnorm3dImpl(a, b, c) = 1 / sqrt(a^2 + b^2 + c^2)
 *                                       = 1 / Norm3dImpl(a, b, c)
 *
 * @param a  float value
 * @param b  float value
 * @param c  float value
 * @return Norm3dImpl(a, b, c)
 * Special cases:
 *      If any one of a,b,c is ±INF, return 0.
 *      If any one of a,b,c is NAN and other is not ±INF, return NAN.
 *      If all of a,b,c is 0, return INF.
 *      If sqrt(a^2 + b^2 + c^2) overflows, return INF.
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Rnorm3dImpl(T a, T b, T c)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type only supports float.");

    return 1.0f / Norm3dImpl(a, b, c);
}

/**
 * Calculate the square root of the sum of squares of 4D coordinates.
 * Formula:Norm4dImpl(a, b, c, d) = sqrt(a^2 + b^2 + c^2 + d^2)
 *                                       = max(a,b,c,d)*sqrt({a/max}^2 + {b/max}^2 + {c/max}^2+ {d/max}^2)
 *
 * @param a  float value
 * @param b  float value
 * @param c  float value
 * @param d  float value
 * @return Norm4dImpl(a, b, c, d)
 * Special cases:
 *      If any one of a,b,c,d is ±INF, return INF.
 *      If any one of a,b,c,d is NAN and other is not ±INF, return NAN.
 *      If all of a,b,c,d is 0, return 0.
 *      If sqrt(a^2 + b^2 + c^2+ d^2) overflows, return INF.
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Norm4dImpl(T a, T b, T c, T d)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type only supports float.");

    if (IsInfImpl(a) || IsInfImpl(b) || IsInfImpl(c) || IsInfImpl(d)) {
        return ConstantsInternal::SIMT_FP32_INF;
    }
    if (IsNanImpl(a) || IsNanImpl(b) || IsNanImpl(c) || IsNanImpl(d)) {
        return ConstantsInternal::SIMT_FP32_INF / ConstantsInternal::SIMT_FP32_INF;
    }
    float m = MaxImpl(AbsImpl(a), AbsImpl(b));
    m = MaxImpl(m, AbsImpl(c));
    m = MaxImpl(m, AbsImpl(d));
    if (m == 0.0f) {
        return 0.0f;
    }
    float r = 0.0f;
    r = FmaImpl((a / m), (a / m), r);
    r = FmaImpl((b / m), (b / m), r);
    r = FmaImpl((c / m), (c / m), r);
    r = FmaImpl((d / m), (d / m), r);
    return m * SqrtImpl(r);
}

/**
 * Calculate the reciprocal of the square root of the sum of squares of 4D coordinates.
 * Formula:Rnorm4dImpl(a, b, c, d) = 1 / sqrt(a^2 + b^2 + c^2 + d^2)
 *                                           = 1 / Norm4dImpl(a, b, c, d)
 *
 * @param a  float value
 * @param b  float value
 * @param c  float value
 * @param d  float value
 * @return Rnorm4dImpl(a, b, c, d)
 * Special cases:
 *      If any one of a,b,c,d is ±INF, return 0.
 *      If any one of a,b,c,d is NAN and other is not ±INF, return NAN.
 *      If all of a,b,c,d is 0, return INF.
 *      If sqrt(a^2 + b^2 + c^2 + d^2) overflows,return 0.
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T Rnorm4dImpl(T a, T b, T c, T d)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type only supports float.");

    return 1.0f / Norm4dImpl(a, b, c, d);
}

/**
 * Calculate the square root of the sum of squares of N coordinates.
 * Formula:NormImpl(n, a) =  sqrt(a[0]^2 +... + a[n-1]^2)
 *                                           = max(a[i]) * sqrt({a[0]/max}^2+...+{a[n-i]/max}^2)
 *
 * @param n int value, computes dimension of a
 * @param a float* value, computes data
 * @return NormImpl(n, a)
 * Special cases:
 *      If any one of a[i] is ±INF, return 0.
 *      If any one of a[i] is NAN and other is not ±INF, return NAN.
 *      If all of a,b,c,d is 0, return 0.
 *      If sqrt(a[0]^2 +... + a[n-1]^2) overflows, return INF.
 *      If n is less than 1, return |a[0]|.
 */
#define __INTERNAL_NORM_IMPL(n, a)                                                      \
    do {                                                                                \
        if ((n) <= 0) {                                                                 \
            return AbsImpl((a)[0]);                                                     \
        }                                                                               \
        float m = 0;                                                                    \
        int remainder = (n) & 3;                                                        \
        int end = (n) - remainder;                                                      \
        if ((n) > 3) {                                                                  \
            for (int i = 0; i < end; i += 4) {                                          \
                float a0 = (a)[i];                                                      \
                float a1 = (a)[i + 1];                                                  \
                float a2 = (a)[i + 2];                                                  \
                float a3 = (a)[i + 3];                                                  \
                if (IsInfImpl(a0) || IsInfImpl(a1) || IsInfImpl(a2) || IsInfImpl(a3)) { \
                    return ConstantsInternal::SIMT_FP32_INF;                            \
                }                                                                       \
                m = MaxImpl(m, AbsImpl(a0));                                            \
                m = MaxImpl(m, AbsImpl(a1));                                            \
                m = MaxImpl(m, AbsImpl(a2));                                            \
                m = MaxImpl(m, AbsImpl(a3));                                            \
            }                                                                           \
        }                                                                               \
        if (remainder != 0) {                                                           \
            for (int i = end; i < (n); i++) {                                           \
                if (IsInfImpl((a)[i])) {                                                \
                    return ConstantsInternal::SIMT_FP32_INF;                            \
                }                                                                       \
                m = MaxImpl(m, AbsImpl((a)[i]));                                        \
            }                                                                           \
        }                                                                               \
        if (m == 0.0f || IsNanImpl(m)) {                                                \
            return m;                                                                   \
        }                                                                               \
        float sum = 0.0f;                                                               \
        if ((n) > 3) {                                                                  \
            for (int i = 0; i < end; i += 4) {                                          \
                float n0 = (a)[i] / m;                                                  \
                float n1 = (a)[i + 1] / m;                                              \
                float n2 = (a)[i + 2] / m;                                              \
                float n3 = (a)[i + 3] / m;                                              \
                sum = FmaImpl(n0, n0, sum);                                             \
                sum = FmaImpl(n1, n1, sum);                                             \
                sum = FmaImpl(n2, n2, sum);                                             \
                sum = FmaImpl(n3, n3, sum);                                             \
            }                                                                           \
        }                                                                               \
        if (remainder != 0) {                                                           \
            for (int i = end; i < (n); i++) {                                           \
                float ni = (a)[i] / m;                                                  \
                sum = FmaImpl(ni, ni, sum);                                             \
            }                                                                           \
        }                                                                               \
        return m * SqrtImpl(sum);                                                       \
    } while (0)

template <typename T1, typename T2>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T2 NormImpl(T1 n, T2* a)
{
    __INTERNAL_NORM_IMPL(n, a);
}

/**
 * Calculate the reciprocal of the square root of the sum of squares of N coordinates.
 * Formula:RnormImpl(n, a) = 1 / sqrt(a[0]^2 +... + a[n-1]^2)
 *                                           = 1 / NormImpl(n, a)
 *
 * @param n int value, computes dimension of a
 * @param a float* value, computes data
 * @return RnormImpl(n, a)
 * Special cases:
 *      If any one of a[i] is ±INF, return 0.
 *      If any one of a[i] is NAN and other is not ±INF, return NAN.
 *      If all of a[i] is 0, return INF.
 *      If sqrt(a[0]^2 +... + a[n-1]^2) overflows, return 0.
 *      If n is less than 1, return 1/|a[0]|.
 */
template <typename T1, typename T2>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T2 RnormImpl(T1 n, T2* a)
{
    return 1.0f / NormImpl(n, a);
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T PowImpl(T x, T y)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input type only supports float.");

#if defined(ASCENDC_CPU_DEBUG)
    if (x < 0.0f) {
        return NAN;
    }

    if (AbsImpl(x) == 1.0f && std::isinf(y)) {
        return NAN;
    }
    if (x == 1.0f && std::isnan(y)) {
        return NAN;
    }

    if ((x == 0.0f || std::isnan(x) || std::isinf(x)) && y == 0.0f) {
        return NAN;
    }

    return pow(x, y);
#else
    return __powf(x, y);
#endif
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float Exp2Impl(float x) { return PowImpl(2.0f, x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float Exp10Impl(float x) { return PowImpl(10.0f, x); }

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float Expm1Impl(float x) { return ExpImpl(x) - 1.0f; }

#if defined(ASCENDC_CPU_DEBUG)
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float Log2Impl(float x) { return log2(x); }
#else
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float Log2Impl(float x) { return LogImpl(x) / LogImpl(2.0f); }
#endif

#if defined(ASCENDC_CPU_DEBUG)
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float Log10Impl(float x) { return log10(x); }
#else
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float Log10Impl(float x) { return LogImpl(x) / LogImpl(10.0f); }
#endif

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float Log1pImpl(float x) { return LogImpl(1.0f + x); }

#if defined(ASCENDC_CPU_DEBUG)
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float LogbImpl(float x)
{
    if (x < 0) {
        x = -x;
    }
    return floor(log2(x));
}
#else
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float LogbImpl(float x)
{
    if (IsNanImpl(x)) {
        return x;
    }
    if (x < 0) {
        x = -x;
    }
    float inf = ConstantsInternal::SIMT_FP32_INF;
    if (IsInfImpl(x)) {
        return inf;
    }
    if (x == 0) {
        return -inf;
    }

    uint32_t fp32InfExponent = 255;
    uint32_t fp32DecimalBit = 23;
    uint32_t fp32SignBit = 256;
    uint32_t fp32ExponentH = 127;
    uint32_t* exponent = (uint32_t*)&x;
    (*exponent) >>= fp32DecimalBit;
    uint32_t sign = fp32SignBit;
    if ((*exponent) > sign) {
        (*exponent) -= sign;
    }
    if ((*exponent) == fp32InfExponent) {
        return inf;
    } else {
        float res = (*exponent);
        res -= fp32ExponentH;
        return res;
    }
}
#endif

#if defined(ASCENDC_CPU_DEBUG)
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int ILogbImpl(float x)
{
    if (x < 0) {
        x = -x;
    }
    if (x == INFINITY) {
        return ConstantsInternal::S32_MAX_VAL;
    }

    return static_cast<int>(LogbImpl(x));
}
#else
__SIMT_DEVICE_FUNCTIONS_DECL__ inline int ILogbImpl(float x)
{
    if (x < 0) {
        x = -x;
    }
    if (IsNanImpl(x)) {
        return ConstantsInternal::S32_MIN_VAL;
    }
    return static_cast<int>(LogbImpl(x));
}
#endif

/**
 * calculates a cube root by input x.
 * @param x a value
 * @return cbrt(x)
 * Special cases:
 *      if x is 0, return 0
 *      if x is Nan, return Nan;
 *      if x is Inf, return Inf;
 *      if x is -Inf, return -Inf;
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CbrtImpl(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input value type only supports float.");

    // get the exponent part of x
    uint32_t xBits = *reinterpret_cast<uint32_t*>(&x);
    int32_t expBits = (xBits >> 23) & 0xFF;
    if (x == 0.0f || expBits == 0xFF) {
        return x;
    }

    // In order for Newtonian iteration method to converge quickly, we need to reduce x to a certain range(0.125, 8).
    // Depending on the computer's float number storage structure, we can adjust the exponential part of x.
    // the adjustment factor(k) ensures the exponent of x' is in (-3, 3)
    int32_t exponent = expBits - 127;
    int32_t k;
    if (exponent >= 3) {
        k = ((exponent - 3) / 3) + 1;
    } else if (exponent <= -4) {
        k = (exponent + 1) / 3;
    } else {
        k = 0;
    }

    // get the adjusted x value
    int32_t expAdjustedBits = exponent - 3 * k + 127;
    uint32_t xAdjustedBits = (xBits & 0x7FFFFF) | (expAdjustedBits << 23);
    float xAdjusted = *reinterpret_cast<float*>(&xAdjustedBits);

    // Newton's iteration method,f(x) = x^3 - b, x_i+1 = x_i - f(x_i)/f'(x_i) = (2*x_i + b/x_i^2)/3
    // the initial value of x_i = 1.0
    float y = 1.0f;
    y = (2.0f * y + xAdjusted / (y * y)) / 3.0f;
    y = (2.0f * y + xAdjusted / (y * y)) / 3.0f;
    y = (2.0f * y + xAdjusted / (y * y)) / 3.0f;
    y = (2.0f * y + xAdjusted / (y * y)) / 3.0f;
    y = (2.0f * y + xAdjusted / (y * y)) / 3.0f;

    // adjust the exponent of y by k
    uint32_t yBits = *reinterpret_cast<uint32_t*>(&y);
    int32_t yExpBits = ((yBits >> 23) & 0xFF) + k;
    yBits = (yBits & 0x807FFFFF) | ((yExpBits & 0xFF) << 23) | (xBits & 0x80000000);
    return *reinterpret_cast<float*>(&yBits);
}

/**
 * calculates reciprocal of the cube root by input x.
 * @param x a value
 * @return rcbrt(x)
 * Special cases:
 *      if x is 0, return Inf
 *      if x is Nan, return Nan;
 *      if x is Inf, return 0;
 *      if x is -Inf, return 0;
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T RcbrtImpl(T x)
{
    static_assert(SupportTypeSimtInternel<T, float>, "Input value type only supports float.");

    if (x == 0.0f) {
        return ConstantsInternal::SIMT_FP32_INF;
    }
    if (IsNanImpl(x)) {
        return x;
    }
    if (IsInfImpl(x)) {
        return 0.0f;
    }

    // get the exponent part of x
    uint32_t xBits = *reinterpret_cast<uint32_t*>(&x);
    int32_t expBits = (xBits >> 23) & 0xFF;

    // Depending on the computer's float number storage structure
    // The exponent bits of x is E = (x >> 23) && 0xFF
    // The exponent value is e = E - 127
    // The exponent value of rcbrt(x) is e' = -e/3
    // The exponent bits of rcbrt(x) is E' = round(127 + e') = 127 - e/3 = (3*127 - e)/3 = (508 - E) / 3
    // Assume that the initial value of the Newton's iteration method is y, the exponent bits of y is E'
    int32_t yExpBits = (508 - expBits) / 3;
    uint32_t yBits = (xBits & 0x80000000) | (yExpBits << 23);
    float y = *reinterpret_cast<float*>(&yBits);

    // The Newton's iteration method, f(x) = x^(-3) - b;
    // x_i+1 = x_i - f(x_i)/f'(x_i)
    // x_i+1 = x_i - (x_i^(-3) - b)/(-3*x_i^(-4))
    // x_i+1 = x_i*(4 - b * x_i^3) / 3
    y = y * (4.0f - x * y * y * y) / 3.0f;
    y = y * (4.0f - x * y * y * y) / 3.0f;
    y = y * (4.0f - x * y * y * y) / 3.0f;
    y = y * (4.0f - x * y * y * y) / 3.0f;
    y = y * (4.0f - x * y * y * y) / 3.0f;
    return y;
}

#if defined(ASCENDC_CPU_DEBUG)
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T ErfImpl(T x)
{
    return erf(x);
}
#else
/**
 * Calculate the error function of the input x.
 * @param x a value
 * @return erf(x)
 * Special cases:
 *      if x is 0, return 0
 *      if x is Inf, return 1;
 *      if x is -Inf, return -1;
 *      if x is Nan, return Nan;
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T ErfImpl(T x)
{
    float absX = AbsImpl(x);
    float xSquared = x * x;
    if (absX >= 1.00296f) {
        float term = absX;
        const float a1 = 0.000112198715f;
        const float a2 = -0.0013275252f;
        const float a3 = 0.008396535f;
        const float a4 = -0.040246583f;
        const float a5 = 0.15950431f;
        const float a6 = 0.9129177f;
        const float a7 = 0.62906002f;

        float polyTerm = FmaImpl(a1, term, a2);
        polyTerm = FmaImpl(polyTerm, term, a3);
        polyTerm = FmaImpl(polyTerm, term, a4);
        polyTerm = FmaImpl(polyTerm, term, a5);
        polyTerm = FmaImpl(polyTerm, term, a6);
        polyTerm = FmaImpl(polyTerm, term, a7);

        float result = FmaImpl(polyTerm, -absX, -absX);
        float expResult = Exp2Impl(result);
        float adjustedExp = 1.0f - expResult;
        uint32_t signBit = *reinterpret_cast<uint32_t*>(&x) & 0x80000000;
        uint32_t finalBits = signBit | *reinterpret_cast<uint32_t*>(&adjustedExp);

        return *reinterpret_cast<float*>(&finalBits);
    } else {
        float term = xSquared;
        const float a1 = 0.000084834944f;
        const float a2 = -0.00082130916f;
        const float a3 = 0.005213489f;
        const float a4 = -0.026868773f;
        const float a5 = 0.11284005f;
        const float a6 = -0.37612664f;
        const float a7 = 0.12837915f;

        float polyTerm = FmaImpl(a1, term, a2);
        polyTerm = FmaImpl(polyTerm, term, a3);
        polyTerm = FmaImpl(polyTerm, term, a4);
        polyTerm = FmaImpl(polyTerm, term, a5);
        polyTerm = FmaImpl(polyTerm, term, a6);
        polyTerm = FmaImpl(polyTerm, term, a7);

        return FmaImpl(polyTerm, x, x);
    }
}
#endif

#if defined(ASCENDC_CPU_DEBUG)
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T ErfcImpl(T x)
{
    return erfc(x);
}
#else
/**
 * Calculate the complementary error function of the input x.
 * @param x a value
 * @return erfc(x)
 * Special cases:
 *      if x is Inf, return 2;
 *      if x is -Inf, return +0;
 *      if x is Nan, return Nan;
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T ErfcImpl(T x)
{
    float absX = AbsImpl(x);
    float term1 = absX + -4.0f;
    float term2 = absX + 4.0f;
    float invTerm2 = 1.0f / term2;
    float y = term1 * invTerm2;
    float z = y + 1.0f;
    float numerator = FmaImpl(-4.0f, z, absX);
    float tmp = FmaImpl(-y, absX, numerator);
    float w = FmaImpl(invTerm2, tmp, y);

    float poly = FmaImpl(0.0008912171f, w, 0.007045788f);
    poly = FmaImpl(poly, w, -0.015866896f);
    poly = FmaImpl(poly, w, 0.036429625f);
    poly = FmaImpl(poly, w, -0.06664343f);
    poly = FmaImpl(poly, w, 0.09381453f);
    poly = FmaImpl(poly, w, -0.10099056f);
    poly = FmaImpl(poly, w, 0.068094f);
    poly = FmaImpl(poly, w, 0.015377387f);
    poly = FmaImpl(poly, w, -0.13962108f);
    poly = FmaImpl(poly, w, 1.2329951f);

    float tmp2 = FmaImpl(2.0f, absX, 1.0f);
    float invTmp2 = 1.0f / tmp2;
    float q = poly * invTmp2;
    float t = FmaImpl(absX, q * -2.0f, poly);
    float u = t - q;
    float v = FmaImpl(u, invTmp2, q);

    float xSquared = absX * absX;
    float negX2 = -xSquared;
    float f1 = 1.442695f;
    float scaled = negX2 * f1;
    float intPart = TruncImpl(scaled);
    float absPart = AbsImpl(intPart);
    uint32_t signBit = *reinterpret_cast<uint32_t*>(&intPart) & 0x80000000;
    float clampedBits = signBit | 0x42FC0000;
    float clamped = *reinterpret_cast<float*>(&clampedBits);
    float safeInt = (absPart > 126.0f) ? clamped : intPart;

    float remainder = FmaImpl(safeInt, -0.6931472f, negX2);
    remainder = FmaImpl(safeInt, 1.9046542e-9f, remainder);
    float exponentArg = remainder * f1;
    float exponentBase = safeInt + 12583039.0f;
    uint32_t exponentBits = *reinterpret_cast<uint32_t*>(&exponentBase) << 23;
    float exponentScale = *reinterpret_cast<float*>(&exponentBits);
    float expVal = Exp2Impl(exponentArg) * exponentScale;

    float term3 = FmaImpl(-absX, absX, xSquared);
    float term4 = FmaImpl(expVal, term3, expVal);
    float result = v * term4;

    if (absX > 10.055f) {
        result = 0.0f;
    }

    return (x < 0) ? (2.0f - result) : result;
}
#endif

/**
 * Calculate the inverse error function of the input x.
 * @param x a value
 * @return erfinv(x)
 * Special cases:
 *      if x is 0, return 0;
 *      if x is 1, return Inf;
 *      if x is -1, return -Inf;
 *      if x outside [-1, 1], return Nan;
 *      if x is Nan, return Nan;
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T ErfinvImpl(T x)
{
    float oppositeX = -x;
    float temp1 = FmaImpl(x, oppositeX, 1.0f);
    float log2Temp = Log2Impl(temp1);
    float negLog2 = -log2Temp;
    if (log2Temp < -8.2f) {
        float rsqrtNegLog = RsqrtImpl(negLog2);
        float poly = FmaImpl(-0.5899144f, rsqrtNegLog, -0.6630042f);
        poly = FmaImpl(poly, rsqrtNegLog, 1.5970111f);
        poly = FmaImpl(poly, rsqrtNegLog, -0.67521554f);
        poly = FmaImpl(poly, rsqrtNegLog, -0.09522479f);
        poly = FmaImpl(poly, rsqrtNegLog, 0.83535343f);
        float denominator = 1.0f / rsqrtNegLog;
        float finalTerm = denominator * poly;

        uint32_t signBit = *reinterpret_cast<uint32_t*>(&x) & 0x80000000;
        uint32_t resultBits = signBit | *reinterpret_cast<uint32_t*>(&finalTerm);
        return *reinterpret_cast<float*>(&resultBits);
    } else {
        float poly = FmaImpl(-2.5172708e-10f, negLog2, 9.427429e-9f);
        poly = FmaImpl(poly, negLog2, -1.2054752e-7f);
        poly = FmaImpl(poly, negLog2, 2.1697005e-7f);
        poly = FmaImpl(poly, negLog2, 0.0000080621484f);
        poly = FmaImpl(poly, negLog2, -0.000031675492f);
        poly = FmaImpl(poly, negLog2, -0.0007743631f);
        poly = FmaImpl(poly, negLog2, 0.005546588f);
        poly = FmaImpl(poly, negLog2, 0.16082023f);
        poly = FmaImpl(poly, negLog2, 0.8862269f);
        return poly * x;
    }
}

/**
 * Calculate the inverse complementary error function of the input x.
 * @param x a value
 * @return erfcinv(x)
 * Special cases:
 *      if x is 0, return Inf;
 *      if x is 2, return -Inf;
 *      if x outside [0, 2], return Nan;
 *      if x is Nan, return Nan;
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T ErfcinvImpl(T x)
{
    float oppositeX = -x;
    float term = 2.0f + oppositeX;

    if (x <= 1.9966f && x >= 0.0034f) {
        float term2 = term * x;
        float logTerm = Log2Impl(term2);
        float negLog = -logTerm;

        float poly = FmaImpl(-2.5172708e-10f, negLog, 9.427429e-9f);
        poly = FmaImpl(poly, negLog, -1.2054752e-7f);
        poly = FmaImpl(poly, negLog, 2.1697005e-7f);
        poly = FmaImpl(poly, negLog, 0.0000080621484f);
        poly = FmaImpl(poly, negLog, -0.000031675492f);
        poly = FmaImpl(poly, negLog, -0.0007743631f);
        poly = FmaImpl(poly, negLog, 0.005546588f);
        poly = FmaImpl(poly, negLog, 0.16082023f);
        poly = FmaImpl(poly, negLog, 0.8862269f);
        return FmaImpl(poly, oppositeX, poly);
    } else {
        bool isGtOne = x > 1.0f;
        float term2 = isGtOne ? term : x;
        float logTerm = Log2Impl(term2);
        float negLog = -logTerm;
        float rsqrtLog = RsqrtImpl(negLog);
        float poly = FmaImpl(-63.113224f, rsqrtLog, 127.48469f);
        poly = FmaImpl(poly, rsqrtLog, -114.10568f);
        poly = FmaImpl(poly, rsqrtLog, 60.325786f);
        poly = FmaImpl(poly, rsqrtLog, -21.789892f);
        poly = FmaImpl(poly, rsqrtLog, 6.467409f);
        poly = FmaImpl(poly, rsqrtLog, -1.8329474f);
        poly = FmaImpl(poly, rsqrtLog, -0.030327774f);
        poly = FmaImpl(poly, rsqrtLog, 0.83287745f);
        float invRsqrt = 1.0f / rsqrtLog;
        float signAdj = isGtOne ? -invRsqrt : invRsqrt;
        return poly * signAdj;
    }
}

/**
 * Calculate the scaled complementary error function of the input x.
 * @param x a value
 * @return erfcx(x)
 * Special cases:
 *      if x is -Inf, return Inf;
 *      if x is Inf, return +0;
 *      if x is Nan, return Nan;
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T ErfcxImpl(T x)
{
    if (x < -9.43f) {
        return ConstantsInternal::SIMT_FP32_INF;
    }

    float absX = AbsImpl(x);
    if (absX < 10.055f) {
        float term1 = absX - 4.0f;
        float term2 = absX + 4.0f;
        float invTerm2 = 1.0f / term2;
        float y = term1 * invTerm2;
        float z = y + 1.0f;
        float numerator = FmaImpl(-4.0f, z, absX);
        float tmp = FmaImpl(-y, absX, numerator);
        float w = FmaImpl(invTerm2, tmp, y);

        float poly = FmaImpl(0.0008912171f, w, 0.007045788f);
        poly = FmaImpl(poly, w, -0.015866896f);
        poly = FmaImpl(poly, w, 0.036429625f);
        poly = FmaImpl(poly, w, -0.06664343f);
        poly = FmaImpl(poly, w, 0.09381453f);
        poly = FmaImpl(poly, w, -0.10099056f);
        poly = FmaImpl(poly, w, 0.068094f);
        poly = FmaImpl(poly, w, 0.015377387f);
        poly = FmaImpl(poly, w, -0.13962108f);
        poly = FmaImpl(poly, w, 1.2329951f);
        float term3 = FmaImpl(2.0f, absX, 1.0f);
        float invTerm3 = 1.0f / term3;
        float q = poly * invTerm3;
        float t = FmaImpl(absX, q * -2.0f, poly);
        float u = t - q;
        float result = FmaImpl(u, invTerm3, q);

        if (x > 0) {
            return result;
        }

        float xSq = absX * absX;
        float negX2 = -xSq;
        float term4 = FmaImpl(absX, absX, negX2);
        float term5 = FmaImpl(xSq, 0.00572498f, 0.5f);
        term5 = MinImpl(term5, ConstantsInternal::SIMT_FP32_INF); // prevent overflow
        float term6 = FmaImpl(term5, 252.0f, 12582913.0f);
        float term7 = term6 - 12583039.0f;
        float negTerm7 = -term7;
        float term8 = FmaImpl(xSq, 1.442695f, negTerm7);
        float term9 = FmaImpl(xSq, 1.925963e-8f, term8);
        uint32_t exponent = *reinterpret_cast<uint32_t*>(&term6) << 23; // Extract exponent bits from term6
        float exponentScale = *reinterpret_cast<float*>(&exponent);
        float term9Exp = Exp2Impl(term9);
        float scaledExp = term9Exp * exponentScale;
        float expApprox = FmaImpl(term9Exp, exponentScale, scaledExp);
        float finalExpApprox = FmaImpl(expApprox, term4, expApprox);
        bool isInf = IsInfImpl(expApprox);
        result = isInf ? expApprox : (finalExpApprox - result);
        return result;
    } else {
        float scaledX = absX * 0.25f;
        float reciprocalX = 0.25f / scaledX;
        float w = reciprocalX * reciprocalX;
        float poly = FmaImpl(6.5625f, w, -1.875f);
        poly = FmaImpl(poly, w, 0.75f);
        poly = FmaImpl(poly, w, -0.5f);
        poly = FmaImpl(poly, w, 1.0f);
        float scaledReciprocal = reciprocalX * 0.5641896f;
        float result = scaledReciprocal * poly;
        return result;
    }
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float ComputeSinpi(float x)
{
    float doubleX = x * 2;

    //  Split a float-value into integer[i] and decimal[f]
    float y0 = static_cast<float>(NearByIntImpl(doubleX));
    int32_t i = static_cast<int32_t>(y0);
    float f = FmaImpl(-y0, 0.5f, x);
    float fPi = f * 3.14159274f;
    float fPiSquare = fPi * fPi;

    float y = 0.0f;
    if ((i & 1) != 0) {
        //  (2k + 1 + f) * pi
        y = 2.42795795e-05f;                        // 2.42795795e-05f : 1/8!
        y = FmaImpl(y, fPiSquare, -0.00138878601f); // -0.001388786f   : -1/6!
        y = FmaImpl(y, fPiSquare, 0.0416667275f);   // 0.041666727f    : 1/4!
        y = FmaImpl(y, fPiSquare, -0.49999997f);    // -0.49999997f    : -1/2!
        float y2 = FmaImpl(fPiSquare, 1.0f, 0.0f);
        y = FmaImpl(y, y2, 1.0f);
    } else {
        //  (2k + f) * pi
        y = -0.000195746587f;                      // -0.000195746587f : 1/7!
        y = FmaImpl(y, fPiSquare, 0.00833270326f); // 0.008332703f     : 1/5!
        y = FmaImpl(y, fPiSquare, -0.166666627f);  // -0.16666662f     : 1/3!
        float y2 = FmaImpl(fPiSquare, fPi, 0.0f);
        y = FmaImpl(y, y2, fPi);
    }

    if ((i & 2) != 0) { //  2: sin(pi+x) = -sin(x)
        y = FmaImpl(y, -1.0f, 0.0f);
    }
    return y;
}

__SIMT_DEVICE_FUNCTIONS_DECL__ inline float ComputeLn(float x)
{
    float offset = 0;
    // sub-norm - > norm
    if (x < 1.17549435e-38f) {
        offset = -23;    //  -23 : sub-norm   - > norm
        x = x * 8388608; //  8388608 : 2^23
    }
    uint32_t u32 = *reinterpret_cast<uint32_t*>(&x);
    int32_t y1 = (u32 - 1059760811) & -8388608; //  -8388608 : -2^23
    int32_t y2 = u32 - y1;
    float mantissa = *reinterpret_cast<float*>(&y2);
    mantissa = mantissa - 1.0f;
    float exponent = FmaImpl(static_cast<float>(y1), 1.1920929e-07f, offset); // 1.1920929e-07: 2^-23

    //  ln(mantissa)
    float y = -0.130188569f;                 // -0.130188569f   :   Coefficient of O(10)
    y = FmaImpl(y, mantissa, 0.140846103f);  //  0.140846103f    :   Coefficient of O(9)
    y = FmaImpl(y, mantissa, -0.121486276f); //  -0.121486276f  :   Coefficient of O(8)
    y = FmaImpl(y, mantissa, 0.139806107f);  //  0.139806107f    :   Coefficient of O(7)
    y = FmaImpl(y, mantissa, -0.166842356f); //  -0.166842356f  :   -1/6
    y = FmaImpl(y, mantissa, 0.200122997f);  //  0.200122997f     :   1/5
    y = FmaImpl(y, mantissa, -0.249996692f); //  -0.249996692f  :   -1/4
    y = FmaImpl(y, mantissa, 0.333331823f);  //  0.333331823f   :   1/3
    y = FmaImpl(y, mantissa, -0.5f);         //  -0.5f         :   -1/2
    y = mantissa * y;
    y = FmaImpl(y, mantissa, mantissa);

    // ln(mantissa) + exponent*ln(2)
    y = FmaImpl(exponent, 0.693147182f, y); // 0.693147182f     :   ln2

    if (u32 >= ConstantsInternal::SIMT_INT32_INF || x == 0) {
        y = FmaImpl(x, ConstantsInternal::SIMT_FP32_INF, ConstantsInternal::SIMT_FP32_INF);
    }
    return y;
}

/**
 * Calculates gamma value by input x, x in (-1.5, 1.5).
 *
 * @param x a value
 * @return x's gamma-value
 */
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float EulerGammaFunction(float x)
{
    float frac = x - NearByIntImpl(x);
    //  1/gamma(x + 1)
    //  = 1 + γx + (γ^2 - pi^2/6) * x^2/2! + O(3)
    float y = -0.00107286568f;            // -0.00107286568f  : Coefficient of O(8)
    y = FmaImpl(y, frac, 0.00711105345f); // 0.00711105345f   : Coefficient of O(7)
    y = FmaImpl(frac, y, -0.0096437186f); // -0.0096437186f   : Coefficient of O(6)
    y = FmaImpl(frac, y, -0.042180188f);  // -0.042180188f    : Coefficient of O(5)
    y = FmaImpl(frac, y, 0.166540906f);   // 0.166540906f     : Coefficient of O(4)
    y = FmaImpl(frac, y, -0.0420036502f); // -0.0420036502f   : Coefficient of O(3)
    y = FmaImpl(frac, y, -0.655878186f);  // -0.655878186f    : [0.577*0.577-pi*pi/6]/2
    y = FmaImpl(frac, y, 0.577215672f);   // 0.577215672f     : Euler-Mascheroni constant
    y = FmaImpl(frac, y, 1.0f);

    if (x < -0.5f) {
        //  1/gamma(x)
        //  = 1/gamma(frac-1)
        //  = 1/[frac*(frac-1)*gamma(frac+1)] = 1/[frac*x*gamma(frac+1)]
        y = y * x * frac;
    }
    if (x <= 0.5f && x >= -0.5f) {
        //  1/gamma(x)
        //  = 1/gamma(frac)
        //  = 1/[frac*gamma(x+1)]
        y = y * frac;
    }
    //  1/(1/gamma(x))
    if (AbsImpl(y) < 1.1754943e-38f) {
        int32_t e = 0;
        float m = FrexpImpl(y, e);
        return LdexpImpl(1.0f / m, 0 - e);
    } else {
        return 1.0f / y;
    }
}

/**
 * Calculates gamma value by input x, x in (-inf, -1.5] or [1.5, inf).
 *
 * Formula:
 *      f(x) = (x-1)! = sqrt(2*pi*x) * [(x/e)^x] * [1 + 1/(12*x) + 1/(288*x^2) + O(3)] / x, x >= 1.5
 *      f(x) = pi / [sin(pi*x) * f(1-x)], x <= -1.5
 * @param x a value
 * @return x's gamma-value
 */
__SIMT_DEVICE_FUNCTIONS_DECL__ inline float StirlingAndEulerReflection(float x)
{
    float absX = AbsImpl(x);
    if (absX > 41.0999985f) {
        x = CopySignImpl(41.0999985f, x);
        absX = AbsImpl(x);
    }

    //  Split the Stirling's Approximation into the main term and the remainder term
    //  Calculate the main term: sqrt(2*pi*x) * (x/e)^x * x^(-1)
    //  sqrt(2*pi*x) * (x/e)^(x-1)
    //  = sqrt(2*pi) * x^(0.5)*(x/e)^x*x^(-1)
    //  = sqrt(2*pi) * (x/e)^x*x^(-0.5)
    //  = sqrt(2*pi) * [x^(x-0.5)/e^x]
    //  = sqrt(2*pi) * 2^log[x^(x-0.5)/e^x]
    //  = sqrt(2*pi) * 2^[(x-0.5)log(x) - xlog(e)]
    //      note:
    //          let y0 = [(x-0.5)log(x) - xlog(e)], split float-value[y0] into integer[i] and decimal[f]
    //          let y01 = (x-0.5)log(x), y02 = xlog(e), then y0 = y01 - y02
    //  = sqrt(2*pi) * 2^[i+f]
    //  = sqrt(2*pi) * 2^f * 2^i
    uint32_t u32 = reinterpret_cast<uint32_t&>(absX);
    int32_t expU32 = (u32 - 1060439283) & 0xFF800000; // 0xFF800000: 2^128
    int32_t manU32 = u32 - expU32;
    float mantissa = *reinterpret_cast<float*>(&manU32);
    float exponent = FmaImpl(static_cast<float>(expU32), 1.1920929e-07f, 0.0f); // 1.1920929e-07 : 2^-23
    float lnMantissa = 2.0f / (mantissa + 1.0f) * (mantissa - 1.0f);

    //  log(x) = log(m*2^exp) = log(m) + exp= ln(m)/loge + exp
    float logX = FmaImpl(lnMantissa, 1.44269502f, exponent); //  1.44269502f : log_2(e)

    //  Calculates log(x)'s error-value
    float logXDiff = FmaImpl(lnMantissa, 1.44269502f, exponent - logX); //  1.44269502f : log_2(e)

    float y3 = 0.000656886259f;
    y3 = FmaImpl(y3, lnMantissa * lnMantissa, 0.00321816537f); // 0.00321816537f : Coefficient of O(3)
    y3 = FmaImpl(y3, lnMantissa * lnMantissa, 0.0180337187f);  // 0.0180337187f : Coefficient of O(2)
    y3 = FmaImpl(y3, lnMantissa * lnMantissa, 0.120224588f);   // 0.120224588f : Coefficient of O(1)
    y3 = FmaImpl(y3, lnMantissa * lnMantissa, 0.0f);

    float r = 2.0f * (mantissa - 1.0f - lnMantissa) - lnMantissa * (mantissa - 1.0f); // 2.0 :
    logXDiff = FmaImpl(1.0f / (mantissa + 1.0f) * r, 1.44269502f, logXDiff);          //  1.44269502f : log_2(e)
    logXDiff = FmaImpl(lnMantissa, 1.92513667e-08f, logXDiff); // 1.92513667e-08f : Coefficient of O(1)
    logXDiff = FmaImpl(y3, lnMantissa, logXDiff);

    float diff0 = logX - (logX + logXDiff) + logXDiff;
    logX = logX + logXDiff;

    //  Calculates the exponent of Stirling's approximation
    float y01 = logX * (absX - 0.5f); //  0.5f : Coefficient of sqrt(x)
    float y02 = 1.44269502f * absX;   //  1.44269502f : log_2(e)
    float y0 = y01 - y02;

    //  Calculates the exponent[y01] error-value
    float diff1 = FmaImpl(logX, absX - 0.5f, -y01);
    diff1 = FmaImpl(diff0, absX - 0.5f, diff1);

    //  Calculates the exponent[y02] error-value
    float diff2 = FmaImpl(1.44269502f, absX, -y02); //  1.44269502f : log_2(e)
    diff2 = FmaImpl(1.92596303e-08f, absX, diff2);
    float y0Diff = (diff1 - diff2) - (y0 - y01 + y02);

    float offset = 0.0f;
    if (absX > 33.0f) { // 33.0f : threshold
        offset = 48.0f;
    }
    if (x < 0.0f) {
        y0 = offset - y0;
        y0Diff = -y0Diff;
    }

    //  Split a float-value into integer[i] and decimal[f]
    float i = NearByIntImpl(y0);
    float f = y0 - i + y0Diff;

    // 2^f * 2^i * sqrt(2*pi)
    float y5 = PowImpl(2.0f, f) * PowImpl(2.0f, i) * 2.5066282f; //  2.5066282f : sqrt(2*PI)

    //  Calculate Stirling's approximation remainder minus 1
    //  y6 = {[1 + 1/(12*x) + 1/(288*x^2) - 139/(51840*x^3) - 571/(2488320*x^4)] - 1}*x^-1
    float recAbsX = 1.0f / absX;
    float y6 = 0.000068413915f;                  //  0.000068413915f    : Coefficient of O(8)
    y6 = FmaImpl(y6, recAbsX, -0.000050603266f); //  -0.000050603266f   : Coefficient of O(7)
    y6 = FmaImpl(y6, recAbsX, -0.00042276637f);  //  -0.00042276637f    : Coefficient of O(6)
    y6 = FmaImpl(y6, recAbsX, 0.0009921414f);    //  0.0009921414f      : Coefficient of O(5)
    y6 = FmaImpl(y6, recAbsX, -0.00027855476f);  //  -0.00027855476f    : -571/2488320
    y6 = FmaImpl(y6, recAbsX, -0.002674901f);    //  -0.002674901f      : -139/51840
    y6 = FmaImpl(y6, recAbsX, 0.0034718033f);    //  0.0034718033f      : 1/288
    y6 = FmaImpl(y6, recAbsX, 0.08333334f);      //  0.08333334f        : 1/12
    y6 = FmaImpl(y6, recAbsX, 0.0f);
    if (x > 0) {
        //  y5 * (1.0f + y6)
        return FmaImpl(y5, y6, y5);
    } else {
        //  According Euler's Reflection Formula
        //  Gamma(x)Gamma(1-x)=PI/sin(pi*x)  ,  x<0
        //  Gamma(x)Gamma(-x)(-x)=PI/sin(pi*x)
        //  Gamma(x)
        //  = pi / {sin(pi*x) * Gamma(-x) * (-x)}
        //  = pi / {sin(pi*x) * Gamma(|x|) * (|x|)}
        //  = pi / {sin(pi*x) * sqrt(2*pi*|x|) * (|x|/e)^|x|*y6*(x)}
        //  = sqrt(2*pi*|x|) / {sin(pi*|x|) * 2 * |x| * (|x|/e)^|x|*y6*(|x|)}
        //  = {sqrt(2*pi*|x|) * (|x|/e)^(x)} / {sin(pi*|x|) * 2* |x| * y6 * (|x|)}
        //  = {[sqrt(2*pi*|x|) * (|x|/e)^(x)]/x} / {sin(pi*|x|) * 2 * x * y6}
        //  = y5 / {sin(pi*|x|) * 2 * x * y6}
        //  = y5 / {sin(pi*|x|) * x * y6} * 0.5

        //  Remaining items of Stirling's Approximation
        y6 = (y6 + 1);

        //  sin(pi*|x|)
        float sinpi = ComputeSinpi(absX);

        //  (y6 * x * sinpi)'s Error value
        float yDiff = FmaImpl(y6 * x, sinpi, -y6 * x * sinpi);
        float y7 = 1 / (y6 * x * sinpi);

        //  y5 / [sin(pi*x) * 2 * x * y6]
        float y = FmaImpl(y5, y7, -y5 * y7 * yDiff * y7);
        y = y * 0.5f;
        if (absX > 33) {            //  33 : threshold
            y = y * 3.5527136e-15f; // 3.5527136e-15 : 2^-48
        }
        return y;
    }
}

/**
 * Calculates gamma value by input x.
 * @param x a value
 * @return tgamma(x)
 * Special cases:
 *      if x is 0, return Inf
 *      if x is Nan, return Nan;
 *      if x is Inf, return Inf;
 *      if x is -Inf, return nan;
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T TgammaImpl(T x)
{
    if (x == 0.0f) {
        return 1.0f / x;
    }
    if (x < 0.0f && NearByIntImpl(x) == x) {
        return ConstantsInternal::SIMT_FP32_INF / ConstantsInternal::SIMT_FP32_INF;
    }
    float absX = AbsImpl(x);
    if (absX < 1.5f) {
        return EulerGammaFunction(x);
    } else {
        return StirlingAndEulerReflection(x);
    }
}

/**
 * Calculates lgamma value by input x.
 * @param x a value
 * @return lgamma(x)
 * Special cases:
 *      if x is 0, return Inf
 *      if x is Nan, return Nan;
 *      if x is Inf, return Inf;
 *      if x is -Inf, return Inf;
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T LgammaImpl(T x)
{
    float absX = AbsImpl(x);
    float result = 0.0f;
    if (IsInfImpl(absX)) {
        return absX;
    } else if (absX < 0.7f) {
        //  1/[gamma(x)] = 1/[gamma(x+1) * x]
        float y0 = 0.0035875155f;               //  0.0035875155f    :   Coefficient of O(7)
        y0 = FmaImpl(y0, absX, -0.0054712854f); //  -0.0054712854f   :   Coefficient of O(6)
        y0 = FmaImpl(y0, absX, -0.044627126f);  //  -0.044627126f    :   Coefficient of O(5)
        y0 = FmaImpl(y0, absX, 0.1673177f);     //  0.1673177f       :   Coefficient of O(4)
        y0 = FmaImpl(y0, absX, -0.04213598f);   //  -0.04213598f     :   Coefficient of O(3)
        y0 = FmaImpl(y0, absX, -0.6558673f);    //  -0.6558673f      :   Coefficient of O(2)
        y0 = FmaImpl(y0, absX, 0.5772154f);     //  0.5772154f       :   Euler-Mascheroni constant
        y0 = FmaImpl(y0, absX, 0.0f);
        y0 = FmaImpl(y0, absX, absX);

        //  ln[1/gamma(x)]
        result = ComputeLn(y0);

        //  ln(gamma(x))
        //  = ln(gamma(x)^{-1*-1})
        //  = -ln(1/gamma(x))
        result = -result;

        if (y0 == 0.0f) {
            result = 1.0f / 0.0f;
        }
    } else if (absX < 1.5f) {
        //  log[gamma(x)]
        //  = −γ(x-1) + ∑(n=2 to inf)[(-1)^n*h(n)/n](x-1)^n
        //  = γ(1-x) + ∑(n=2 to inf)[h(n)/n](-x+1)^n
        float oneMinusX = 1.0f - absX;
        result = 0.045882664f;                             //  0.045882664f    :  Coefficient of O(11)
        result = FmaImpl(result, oneMinusX, 0.10373967f);  //  0.10373967f     :   Coefficient of O(10)
        result = FmaImpl(result, oneMinusX, 0.122803635f); //  0.122803635f    :   Coefficient of O(9)
        result = FmaImpl(result, oneMinusX, 0.12752421f);  //  0.12752421f     :   Coefficient of O(8)
        result = FmaImpl(result, oneMinusX, 0.14321668f);  //  0.14321668f     :   Coefficient of O(7)
        result = FmaImpl(result, oneMinusX, 0.16934357f);  //  0.16934357f     :   Coefficient of O(6)
        result = FmaImpl(result, oneMinusX, 0.20740793f);  //  0.20740793f     :   Coefficient of O(5)
        result = FmaImpl(result, oneMinusX, 0.2705875f);   //  0.2705875f      :   pi^4/360
        result = FmaImpl(result, oneMinusX, 0.40068542f);  //  0.40068542f     :   1.20/3
        result = FmaImpl(result, oneMinusX, 0.82246696f);  //  0.82246696f     :   (pi^2)/12
        result = FmaImpl(result, oneMinusX, 0.5772157f);   //  0.5772157f      :   Euler-Mascheroni constant
        result = FmaImpl(result, oneMinusX, 0.0f);
    } else if (absX < 3.0f) {
        //  log[gamma(x)]
        //  = (1−γ)(x-2) + [(pi^2-6)/12](x-2)^2 + O(3)
        float xMinusTwo = absX - 2.0f;
        result = 0.0000495984932f;                            //  -0.000049598493f    :   Coefficient of O(10)
        result = FmaImpl(result, xMinusTwo, -0.00022089484f); //  -0.000220894843f    :   Coefficient of O(9)
        result = FmaImpl(result, xMinusTwo, 0.000541314250f); //   0.00054131424f    :   Coefficient of O(8)
        result = FmaImpl(result, xMinusTwo, -0.00120451697f); //  -0.001204517f      :   Coefficient of O(7)
        result = FmaImpl(result, xMinusTwo, 0.00288425176f);  //  0.0028842517f      :   Coefficient of O(6)
        result = FmaImpl(result, xMinusTwo, -0.00738275796f); //  -0.007382758f      :   Coefficient of O(5)
        result = FmaImpl(result, xMinusTwo, 0.0205813199f);   //  0.02058132f        :   Coefficient of O(4)
        result = FmaImpl(result, xMinusTwo, -0.0673524886f);  //  -0.06735249f       :   Coefficient of O(3)
        result = FmaImpl(result, xMinusTwo, 0.322467029f);    //  0.32246702f        :   (pi^2-6)/12
        result = FmaImpl(result, xMinusTwo, 0.42278432f);     //  0.42278432f        :   1-γ
        result = FmaImpl(result, absX, -result - result);
    } else if (absX < 7.8f) {
        float xMinusThree = absX - 3.0f;
        float a0 = -143033.4f, a1 = -48310.664f, a2 = -41061.375f, a3 = -12349.742f, a4 = -748.8903f;
        float b0 = -206353.58f, b1 = -92685.05f, b2 = -10777.18f, b3 = -259.25097f;

        float y0 = FmaImpl(a4, xMinusThree, a3);
        y0 = FmaImpl(y0, xMinusThree, a2);
        y0 = FmaImpl(y0, xMinusThree, a1);
        y0 = FmaImpl(y0, xMinusThree, a0);

        float y1 = FmaImpl(1.0f, xMinusThree, b3);
        y1 = FmaImpl(y1, xMinusThree, b2);
        y1 = FmaImpl(y1, xMinusThree, b1);
        y1 = FmaImpl(y1, xMinusThree, b0);

        result = FmaImpl(y0, 1.0f / y1, xMinusThree);
    } else {
        //  According Stirling's Approximation
        //  ln(gamma(x))
        //  = ln((x-1)!)
        //  = ln({sqrt(2*pi*x) * [(x/e)^x] * e^[(1/12)/x - (1/360)/x^3] + (1/1260)/x^5+O(6)]}/x)
        //  = ln(sqrt(2*pi)) + 0.5*ln(x) + xln(x)-x + [(1/12)/x-(1/360)/x^3]+(1/1260)/x^5] - ln(x)
        //  = ln(sqrt(2*pi)) + 0.5*ln(x) + xln(x)-x + y_2 -ln(x)
        //  = ln(sqrt(2*pi)) + xln(x)-x  -0.5*ln(x) + y_2
        //  = ln(sqrt(2*pi)) + (x-0.5)ln(x) -x + y2

        //  [(1/12) - (1/360)/(1/x^2) + (1/1260)/((1/x^2)^2)] * (1/x)
        float y0 = (1.0f / absX);
        float y1 = y0 * y0;
        float y2 = 0.00077783066f;            //  0.00077783066f  : 1/1260
        y2 = FmaImpl(y2, y1, -0.0027776553f); //  -0.0027776553f  : -1/360
        y2 = FmaImpl(y2, y1, 0.083333276f);   //  0.083333276     : 1/12
        y2 = FmaImpl(y2, y0, 0.0f);

        //  ln(x) * 0.5 * (|x| - 0.5)
        float y3 = ComputeLn(absX) * 0.5f * (absX - 0.5f);

        //  (x-0.5) * ln(x) * 0.5 -|x| + (x-0.5) * ln(x) * 0.5 + y2 + ln(sqrt(2*pi))
        result = y3 - absX + y3 + y2 + 0.9189385f; //  0.9189385f : ln[(2*pi)/2]
    }
    if (x < 0) {
        if (FloorIntrinsicsImpl(absX) == absX) {
            return ConstantsInternal::SIMT_FP32_INF;
        } else if (absX < 9.9999996e-20f) { // 9.9999996e-20 : minimum-value
            //  According Euler's Reflection Formula
            //  As x ~ 0 : then sin(pi*x) ~ pi*x
            //  ln(|gamma(x)|)
            //  ~ ln(pi) - ln(|sin(pi*x)|) - ln(gamma(|x|)) - ln(|x|)
            //  ~ ln(pi)-ln(-pi*x) - ln(gamma(|x|)) - ln(|x|)
            //  ~ ln(-pi/(-pi*x)) - ln(gamma(|x|)) - ln(|x|)
            //  ~ ln(-1/x) - ln(gamma(-x)) - ln(|x|)
            //  ~ -ln([-x*gamma(-x)]) - ln(|x|)
            //  ~ -ln([gamma(1-x)]) - ln(|x|)
            //  ~ -ln([gamma(1)]) - ln(|x|)
            //  ~ -ln(1!) - ln(|x|)
            //  ~ -ln(|x|)
            result = -ComputeLn(absX);
        } else {
            //  According Euler's Reflection Formula & Stirling's Approximation
            //  gamma(x)gamma(1-x) = pi/sin(pi*x)
            //  gamma(x)gamma(-x)*(-x) = pi/sin(pi*x)
            //  gamma(x)
            //  = pi/[sin(pi*x) * gamma(-x)*(-x)]
            //  = pi/[sin(pi*x) * gamma(|x|)*(|x|)] , x<0
            //  ln[|gamma(x)|]
            //  = ln{|pi/[sin(pi*x) * gamma(-x) * (-x)|]}
            //  = ln{pi/[|sin(pi*x)| * gamma(|x|) * (|x|)]}
            //  = ln(pi) - ln(|sin(pi*x)|) - ln(gamma(|x|)) - ln(|x|)
            //  = ln(pi) - ln(|sin(pi*x)|) - ln(gamma(|x|)) - ln(|x|)
            //  = ln(pi) - ln(|sin(pi*|x|)|*|x|]) - ln(gamma(|x|))
            float sinpi = ComputeSinpi(absX);
            float lnXSinpi = ComputeLn(absX * AbsImpl(sinpi));
            float y = 1.14472985f - lnXSinpi; //  1.1447298f : ln(pi)
            result = FmaImpl(y, 1.0f, -result);
        }
    }
    return result;
}

/**
 * Calculates CylBesselI0 value by input x.
 * @param x a value
 * @return CylBesselI0(x)
 * Special cases:
 *      if x is 0, return 1;
 *      if x is Nan, return Nan;
 *      if x is Inf, return Inf;
 *      if x is -Inf, return Inf;
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CylBesselI0Impl(T x)
{
    float absX = AbsImpl(x);
    if (IsInfImpl(absX)) {
        return absX;
    }
    if (absX >= 9) {
        //  I(x) ~ exp(x) * 1/sqrt(2*pi*x) * [1 + 1/(8x) + 9/(128x^2) + O(3)]
        float reciprocalX = 1.0f / absX;
        float y = 0.34872168f;                       // 0.34872168f     : Coefficient of O(5)
        y = FmaImpl(y, reciprocalX, -0.0054563344f); //  -0.0054563344f  : Coefficient of O(4)
        y = FmaImpl(y, reciprocalX, 0.033347155f);   //  0.033347155f    : Coefficient of O(3)
        y = FmaImpl(y, reciprocalX, 0.027889195f);   //  0.027889195f    : 9/[sqrt(2*pi)*128]
        y = FmaImpl(y, reciprocalX, 0.04987063f);    //  0.04987063f     : 1/[sqrt(2*pi)*8]
        y = FmaImpl(y, reciprocalX, 0.39894226f);    //  0.39894226f     : 1/sqrt(2*pi)
        y = y * RsqrtImpl(absX);
        return y * (ExpImpl(absX * 0.5f) - 1) * (ExpImpl(absX * 0.5f) + 1) + y;
    } else {
        //  I_0(x) = ∑(k=0 to inf)[1/k!Γ(k+1)*(x/2)^2k ]
        float squareX = absX * absX;
        float y = 1.551427e-19;                    // 1.551427e-19        : Coefficient of O(10)
        y = FmaImpl(y, squareX, 1.4492505e-17f);   // 1.4492505e-17        : Coefficient of O(9)
        y = FmaImpl(y, squareX, 1.0687647e-14f);   // 1.0687647e-14f       : Coefficient of O(8)
        y = FmaImpl(y, squareX, 2.3349575e-12f);   // 2.3349575e-12f       : 1/25401600*16384
        y = FmaImpl(y, squareX, 4.7306625e-10f);   // 4.7306625e-10f       : 1/518400*4096
        y = FmaImpl(y, squareX, 6.7778003e-8f);    // 6.7778003e-8f        : 1/(14400*1024)
        y = FmaImpl(y, squareX, 0.0000067820783f); // 0.0000067820783f     : 1/(576*256)
        y = FmaImpl(y, squareX, 0.00043402583f);   // 0.00043402583f       : 1/(36*64)
        y = FmaImpl(y, squareX, 0.015625f);        // 0.015625             : 1/(4*16)
        y = FmaImpl(y, squareX, 0.25f);            // 0.25f                : 1/(1*4)
        y = FmaImpl(y, squareX, 1);
        return y;
    }
}

/**
 * Calculates CylBesselI1 value by input x.
 * @param x a value
 * @return CylBesselI1(x)
 * Special cases:
 *      if x is 0, return 0;
 *      if x is Nan, return Nan;
 *      if x is Inf, return Inf;
 *      if x is -Inf, return -Inf;
 */
template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T CylBesselI1Impl(T x)
{
    float absX = AbsImpl(x);
    if (IsInfImpl(absX)) {
        return x;
    }
    if (IsNanImpl(absX)) {
        return absX;
    }
    if (absX >= 8.085f) {
        //  I(x) ~ exp(x) * 1/sqrt(2*pi*x) * [1 - 3/(8x) -15/(128x^2) + O(3)]
        float reciprocalX = 1.0f / absX;
        float y = -0.5028813f;                     //  -0.5028813   : Coefficient of O(5)
        y = FmaImpl(y, reciprocalX, 0.028471555f); //  0.028471555  : Coefficient of O(4)
        y = FmaImpl(y, reciprocalX, -0.04873671f); //  -0.04873671  : Coefficient of O(3)
        y = FmaImpl(y, reciprocalX, -0.04641596f); //  -0.04641596  : -15/[sqrt(2*pi)*128]
        y = FmaImpl(y, reciprocalX, -0.14960973f); //  -0.14960973  : -3/[sqrt(2*pi)*8]
        y = FmaImpl(y, reciprocalX, 0.39894232f);  //  0.39894232   : 1/sqrt(2*pi)
        y = y * RsqrtImpl(absX);
        y = y * (ExpImpl(absX * 0.5f) - 1) * (ExpImpl(absX * 0.5f) + 1) + y;
        return CopySignImpl(y, x);
    } else {
        //  I(x) = x * [1/2 + (x^2)/16 + (x^2)^2/384 + (x^2)^3/18432 + (x^2)^4/1474560 + (x^2)^5/176947200 + O(6)]
        float squareX = x * x;
        float y = 2.7848253e-18f;                // 2.7848253e-18f    :  Coefficient of O(9)
        y = FmaImpl(y, squareX, 3.4224707e-16f); // 3.4224707e-16f     : Coefficient of O(8)
        y = FmaImpl(y, squareX, 1.6258002e-13f); // 1.6258002e-13f     : Coefficient of O(7)
        y = FmaImpl(y, squareX, 3.3142173e-11f); // 3.3142173e-11f     : Coefficient of O(6)
        y = FmaImpl(y, squareX, 5.6632734e-9f);  // 5.6632734e-9f      : 1/176947200
        y = FmaImpl(y, squareX, 6.780027e-7f);   // 6.780027e-7f       : 1/1474560
        y = FmaImpl(y, squareX, 0.00005425474f); // 0.00005425474f     : 1/18432
        y = FmaImpl(y, squareX, 0.002604162f);   // 0.002604162f       : 1/384
        y = FmaImpl(y, squareX, 0.0625000f);     // 0.06250001f        : 1/16
        y = FmaImpl(y, squareX, 0.5f);           // 0.5f               : 1/2
        return y * x;
    }
}

template <typename T>
__SIMT_DEVICE_FUNCTIONS_DECL__ inline T NormcdfImpl(T x)
{
    if (AbsImpl(x) > 14.5f) {
        x = CopySignImpl(14.5f, x);
    }

    float oneOverSqrt2High = -0.707106769f; // -0.707106769f: -1/sqrt(2) high
    float xOverSqrt2High = x * oneOverSqrt2High;
    float compensateValue = FmaImpl(x, oneOverSqrt2High, -xOverSqrt2High);

    float oneOverSqrt2Low = -1.21016175e-8f; // -1.21016175e-8f: -1/sqrt(2) low
    float xOverSqrt2Low = FmaImpl(x, oneOverSqrt2Low, compensateValue);
    float xOverSqrt2 = xOverSqrt2High + xOverSqrt2Low;

    float erfcValue = ErfcImpl(xOverSqrt2);
    if (x <= -1.0f) {
        erfcValue = FmaImpl(-2.0f * xOverSqrt2 * erfcValue, xOverSqrt2High - xOverSqrt2 + xOverSqrt2Low, erfcValue);
    }
    return 0.5f * erfcValue;
}

} // namespace Simt
} // namespace AscendC
#endif // IMPL_SIMT_API_CPP_DAV_C310_KERNEL_SIMT_TRANSCENDENTAL_IMPL_H
