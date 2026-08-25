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
 * \file digamma_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/digamma/digamma_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/digamma.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_DIGAMMA_DIGAMMA_COMMON_IMPL_H__
#endif
#ifndef IMPL_MATH_DIGAMMA_DIGAMMA_COMMON_IMPL_H
#define IMPL_MATH_DIGAMMA_DIGAMMA_COMMON_IMPL_H
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "digamma_3510_impl.h"
#else
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "kernel_tiling/kernel_tiling.h"
#include "../../../../../include/adv_api/math/tan.h"
#include "../../../../../include/adv_api/math/sin.h"
#include "../../../../../include/adv_api/math/cos.h"
#include "digamma_common_basic_impl.h"
#include "../../common/check.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/digamma/digamma_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"
#endif
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002)

namespace AscendC {
#pragma begin_pipe(V)

// input src is x, write the calculation result on the dst, used tmpCal1 and tmpCal2
__aicore__ inline void DigammaPositiveHalf(
    const LocalTensor<float>& dst, const LocalTensor<float>& src, DigammaParams& params)
{
    // w = x + 3
    Adds<float, false>(params.tmpCal1, src, 3.0f, MASK_PLACEHOLDER, 1, params.unaryParams);
    PipeBarrier<PIPE_V>();

    // tmp0(dst) = log(w)
    Ln<float, false>(dst, params.tmpCal1, MASK_PLACEHOLDER, 1, params.unaryParams);
    PipeBarrier<PIPE_V>();

    // inv_w(tmpCal1) = 1 / w
    Duplicate<float, false>(params.tmpScalar, 1.0f, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();
    Div<float, false>(params.tmpCal1, params.tmpScalar, params.tmpCal1, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();

    // tmp0 = tmp0 - 0.5 * inv_w
    Muls<float, false>(params.tmpScalar, params.tmpCal1, 0.5f, MASK_PLACEHOLDER, 1, params.unaryParams);
    PipeBarrier<PIPE_V>();
    Sub<float, false>(dst, dst, params.tmpScalar, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();

    // tmp0 = tmp0 - (0.0833333333333333 * inv_w ** 2)
    Mul<float, false>(params.tmpCal1, params.tmpCal1, params.tmpCal1, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();
    Muls<float, false>(params.tmpScalar, params.tmpCal1, 0.0833333333333333f, MASK_PLACEHOLDER, 1, params.unaryParams);
    PipeBarrier<PIPE_V>();
    Sub<float, false>(dst, dst, params.tmpScalar, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();

    // tmp0 = tmp0 - (0.0083333333333333 * inv_w ** 4)
    Mul<float, false>(params.tmpCal2, params.tmpCal1, params.tmpCal1, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();
    Muls<float, false>(params.tmpScalar, params.tmpCal2, 0.0083333333333333f, MASK_PLACEHOLDER, 1, params.unaryParams);
    PipeBarrier<PIPE_V>();
    Sub<float, false>(dst, dst, params.tmpScalar, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();

    // tmp0 = tmp0 - (0.003968253968254 * inv_w ** 4)
    Mul<float, false>(params.tmpCal2, params.tmpCal2, params.tmpCal1, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();
    Muls<float, false>(params.tmpScalar, params.tmpCal2, 0.003968253968254f, MASK_PLACEHOLDER, 1, params.unaryParams);
    PipeBarrier<PIPE_V>();
    Sub<float, false>(dst, dst, params.tmpScalar, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();

    // tmp1 Cal1 = 1 / src
    Duplicate<float, false>(params.tmpScalar, 1.0f, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();
    Div<float, false>(params.tmpCal1, params.tmpScalar, src, MASK_PLACEHOLDER, 1, params.binaryParams);

    constexpr size_t calcSize = 2;
    for (size_t i = 0U; i < calcSize; ++i) {
        // tmpCal1 = tmpCal1 + 1 / (tmpCal1 + i)
        Adds<float, false>(params.tmpCal2, src, tmp1HalfCalcConst[i], MASK_PLACEHOLDER, 1, params.unaryParams);
        PipeBarrier<PIPE_V>();

        Div<float, false>(params.tmpCal2, params.tmpScalar, params.tmpCal2, MASK_PLACEHOLDER, 1, params.binaryParams);
        PipeBarrier<PIPE_V>();

        Add<float, false>(params.tmpCal1, params.tmpCal1, params.tmpCal2, MASK_PLACEHOLDER, 1, params.binaryParams);
        PipeBarrier<PIPE_V>();
    }

    // positive result = tmp0 - tmp1
    Sub<float, false>(dst, dst, params.tmpCal1, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();
}

// compute x is less than -0.0001 and put the result on result
__aicore__ inline void DigammaNegativeHalf(
    const LocalTensor<float>& dst, const LocalTensor<float>& src, DigammaParams& params)
{
    // tmpCal5 = 1 - x
    Duplicate<float, false>(params.tmpScalar, 1.0f, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();
    Sub<float, false>(params.tmpCal5, params.tmpScalar, src, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();

    // Positive(1 - x)
    DigammaPositiveHalf(dst, params.tmpCal5, params);

    // Save Positive(1 - x) to tmpCal2
    Adds<float, false>(params.tmpCal2, dst, 0.0f, MASK_PLACEHOLDER, 1, params.unaryParams);
    PipeBarrier<PIPE_V>();

    // Subtract the pre-calculated tmp3
    Sub<float, false>(dst, dst, params.tmpCal3, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();
}

__ASC_USE_RESERVED_UBUF__(2201, "Digamma is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void DigammaComputeImpl(
    const LocalTensor<half>& dst, const LocalTensor<half>& src, DigammaParams& params)
{
    // The half type needs to be converted to the float32 type for calculation.
    Cast<float, half, false>(
        params.tmpCal5, src, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        {1, 1, DEFAULT_REPEAT_STRIDE, HALF_DEFAULT_REPEAT_STRIDE});
    PipeBarrier<PIPE_V>();

    // To reduce memory usage, the first calculation tmpCal3 = pi / tan(pi * (x - floor(x)))
    DigammaCast(params.tmpCal4, params.tmpCal5, RoundMode::CAST_FLOOR);
    Sub<float, false>(params.tmpCal4, params.tmpCal5, params.tmpCal4, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();
    Muls<float, false>(params.tmpCal4, params.tmpCal4, DIGAMMA_PI, MASK_PLACEHOLDER, 1, params.unaryParams);
    PipeBarrier<PIPE_V>();
    TanCompute<float>(params.tmpScalar, params.tmpCal4, params.result.ReinterpretCast<uint8_t>(), params.splitSize);

    Duplicate<float, false>(params.tmpCal1, DIGAMMA_PI, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();
    Div<float, false>(params.tmpCal3, params.tmpCal1, params.tmpScalar, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();

    Duplicate<float, false>(params.tmpCal4, 0.0f, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);

    // Special data is directly output as nan. x == negative integer
    NotNumUnion notNum;
    notNum.i = F32_NAN;
    Duplicate<float, false>(params.result, notNum.f, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);

    // x <= -8388608 the result is nan, and select to params.tmpCal4
    DigammaGenCompareMask(params.mask, params.tmpCal5, params, MIN_NEG_WITH_FLOAT, CMPMODE::LE);
    DigammaSelect(params.tmpCal4, params.result, params.mask, params.tmpCal1, params);

    // negative integer between (8388608, 0) the result is nan, and select to params.tmpCal4
    DigammaGenNegIntMask(params.mask1, params.tmpCal5, params, MIN_NEG_WITH_FLOAT);
    DigammaSelect(params.tmpCal4, params.result, params.mask1, params.tmpCal1, params);

    // for nan
    DigammaGenNanMask(params.mask, params.tmpCal5, params);
    DigammaSelect(params.tmpCal4, params.tmpCal5, params.mask, params.tmpCal1, params);

    // Compute x >= 0, and select to params.tmpCal4
    DigammaGenCompareMask(params.mask, params.tmpCal5, params, 0.0f, CMPMODE::GE);
    DigammaPositiveHalf(params.result, params.tmpCal5, params);
    DigammaSelect(params.tmpCal4, params.result, params.mask, params.tmpCal1, params);

    // Compute x < -0.0001, and select to params.tmpCal4
    DigammaGenCompareMask(params.mask, params.tmpCal5, params, -0.0001f, CMPMODE::LT);
    DigammaNegativeHalf(params.result, params.tmpCal5, params);
    DigammaSelect(params.tmpCal4, params.result, params.mask, params.tmpCal1, params);

    Cast<float, half, false>(
        params.tmpCal5, src, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        {1, 1, DEFAULT_REPEAT_STRIDE, HALF_DEFAULT_REPEAT_STRIDE});
    PipeBarrier<PIPE_V>();
    // Compute -0.0001 <= x && x < 0.0, and select to params.tmpCal4
    DigammaGenRangeMask(params.mask, params.tmpCal5, params, -0.0001f, 0.0f);
    DigammaNegativeRange(params.result, params.tmpCal5, params);
    DigammaSelect(params.tmpCal4, params.result, params.mask, params.tmpCal1, params);

    Cast<half, float, false>(
        dst, params.tmpCal4, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        {1, 1, HALF_DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
    PipeBarrier<PIPE_V>();
}

// w = x + 10
// inv_w = 1 / w
// z = inv_w * inv_w
// tmp0 = log(w) - 0.5 * inv_w - z * (8.33333333333333333333e-2 - z * (8.33333333333333333333e-3 - z * (
//         3.96825396825396825397e-3 - z * (4.16666666666666666667e-3 - z * (
//         7.57575757575757575758e-3 - z * (2.10927960927960927961e-2 - z * 8.33333333333333333333e-2))))))
// used tmpCal1 and tmpCal2
__aicore__ inline void DigammaPositiveTmp0(
    const LocalTensor<float>& dst, const LocalTensor<float>& src, DigammaParams& params)
{
    // w = x + 10
    Adds<float, false>(params.tmpCal1, src, 10.0f, MASK_PLACEHOLDER, 1, params.unaryParams);
    PipeBarrier<PIPE_V>();

    // tmp0 = log(w)
    Ln<float, false>(dst, params.tmpCal1, MASK_PLACEHOLDER, 1, params.unaryParams);
    PipeBarrier<PIPE_V>();

    // inv_w = 1 / w
    Duplicate<float, false>(params.tmpScalar, 1.0f, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();
    Div<float, false>(params.tmpCal1, params.tmpScalar, params.tmpCal1, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();

    // tmp0 = tmp0 - 0.5 * inv_w
    Muls<float, false>(params.tmpCal2, params.tmpCal1, 0.5f, MASK_PLACEHOLDER, 1, params.unaryParams);
    PipeBarrier<PIPE_V>();
    Sub<float, false>(dst, dst, params.tmpCal2, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();

    // z = inv_w * inv_w
    Mul<float, false>(params.tmpCal1, params.tmpCal1, params.tmpCal1, MASK_PLACEHOLDER, 1, params.binaryParams);

    // C1 - z * c2
    Duplicate<float, false>(
        params.tmpCal2, 8.33333333333333333333e-2, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();

    for (size_t i = 0U; i < DIGAMMA_MAX_LOOP; ++i) {
        Duplicate<float, false>(
            params.tmpScalar, posCalcConst[i], MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);

        // z * ()
        Mul<float, false>(params.tmpCal2, params.tmpCal1, params.tmpCal2, MASK_PLACEHOLDER, 1, params.binaryParams);
        PipeBarrier<PIPE_V>();

        Sub<float, false>(params.tmpCal2, params.tmpScalar, params.tmpCal2, MASK_PLACEHOLDER, 1, params.binaryParams);
        PipeBarrier<PIPE_V>();
    }
    constexpr size_t calcSize = 6;
    for (size_t i = DIGAMMA_MAX_LOOP; i < calcSize; ++i) {
        Duplicate<float, false>(
            params.tmpScalar, posCalcConst[i], MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);

        // z * ()
        Mul<float, false>(params.tmpCal2, params.tmpCal1, params.tmpCal2, MASK_PLACEHOLDER, 1, params.binaryParams);
        PipeBarrier<PIPE_V>();
        Sub<float, false>(params.tmpCal2, params.tmpScalar, params.tmpCal2, MASK_PLACEHOLDER, 1, params.binaryParams);
        PipeBarrier<PIPE_V>();
    }
    Mul<float, false>(params.tmpCal2, params.tmpCal1, params.tmpCal2, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();

    // tmp0 = tmp0 - ()
    Sub<float, false>(dst, dst, params.tmpCal2, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();
}

// tmp1 = 1 / x + 1 / (x + 1) + 1 / (x + 2) + 1 / (x + 3) + 1 / (x + 4) + 1 / (x + 5) + 1 / (x + 6) + 1 /
//        ( x + 7) + 1 / (x + 8) + 1 / (x + 9)
// used tmpCal2
__aicore__ inline void DigammaPositiveTmp1(
    const LocalTensor<float>& dst, const LocalTensor<float>& src, DigammaParams& params)
{
    // dst = 1 / src
    Duplicate<float, false>(params.tmpScalar, 1.0f, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();
    Div<float, false>(dst, params.tmpScalar, src, MASK_PLACEHOLDER, 1, params.binaryParams);

    for (size_t i = 0U; i < DIGAMMA_MAX_LOOP; ++i) {
        // dst = dst + 1 / (src + i)
        Adds<float, false>(params.tmpCal2, src, tmp1CalcConst[i], MASK_PLACEHOLDER, 1, params.unaryParams);
        PipeBarrier<PIPE_V>();

        Div<float, false>(params.tmpCal2, params.tmpScalar, params.tmpCal2, MASK_PLACEHOLDER, 1, params.binaryParams);
        PipeBarrier<PIPE_V>();

        Add<float, false>(dst, dst, params.tmpCal2, MASK_PLACEHOLDER, 1, params.binaryParams);
        PipeBarrier<PIPE_V>();
    }
    constexpr size_t calcSize = 9;
    for (size_t i = DIGAMMA_MAX_LOOP; i < calcSize; ++i) {
        // dst = dst + 1 / (src + i)
        Adds<float, false>(params.tmpCal2, src, tmp1CalcConst[i], MASK_PLACEHOLDER, 1, params.unaryParams);
        PipeBarrier<PIPE_V>();

        Div<float, false>(params.tmpCal2, params.tmpScalar, params.tmpCal2, MASK_PLACEHOLDER, 1, params.binaryParams);
        PipeBarrier<PIPE_V>();

        Add<float, false>(dst, dst, params.tmpCal2, MASK_PLACEHOLDER, 1, params.binaryParams);
        PipeBarrier<PIPE_V>();
    }
}

// input src is x, write the calculation result on the dst, used tmpCal1 and tmpCal2
__aicore__ inline void DigammaPositive(
    const LocalTensor<float>& dst, const LocalTensor<float>& src, DigammaParams& params)
{
    // calculate tmp0
    DigammaPositiveTmp0(dst, src, params);

    // calculate tmp1
    DigammaPositiveTmp1(params.tmpCal1, src, params);

    // positive result = tmp0 - tmp1
    Sub<float, false>(dst, dst, params.tmpCal1, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();
}

__ASC_USE_RESERVED_UBUF__(2201, "Digamma is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void DigammaNegPicotPix(
    const LocalTensor<float>& dst, const LocalTensor<float>& src, DigammaParams& params)
{
    // f7(tmpCal1) = (x + x)
    Add<float, false>(params.tmpCal1, src, src, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();

    // f8(tmpCal2) = round(f7)
    DigammaCast(params.tmpCal2, params.tmpCal1, RoundMode::CAST_ROUND);

    // f36(tmpCal1) = (f7 - f8) * 1.5707963267948966
    Sub<float, false>(params.tmpCal1, params.tmpCal1, params.tmpCal2, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();
    Muls<float, false>(params.tmpCal1, params.tmpCal1, 1.5707963267948966f, MASK_PLACEHOLDER, 1, params.unaryParams);
    PipeBarrier<PIPE_V>();

    // r51(tmpCal2) = f8
    Cast<int32_t, float, false>(
        params.tmpCal2.ReinterpretCast<int32_t>(), params.tmpCal2, RoundMode::CAST_ROUND, MASK_PLACEHOLDER, 1,
        params.unaryParams);
    PipeBarrier<PIPE_V>();

    // p9(tmpCal2) = (bitwise_and(r51, 1) == 0)
    Duplicate<int32_t, false>(
        params.tmpCal3.ReinterpretCast<int32_t>(), 1, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();
    SetVectorMask<float>(0, params.splitSize * (sizeof(float) / sizeof(uint16_t)));
    And<uint16_t, false>(
        params.tmpCal2.ReinterpretCast<uint16_t>(), params.tmpCal2.ReinterpretCast<uint16_t>(),
        params.tmpCal3.ReinterpretCast<uint16_t>(), MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();
    SetVectorMask<float>(0, params.splitSize);

    // get p9 == 0 mask
    Cast<float, int32_t, false>(
        params.tmpCal2, params.tmpCal2.ReinterpretCast<int32_t>(), RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        params.unaryParams);
    DigammaGenCompareMask(params.mask1, params.tmpCal2, params, 0.5f, CMPMODE::LT);
    DigammaGenCompareMask(params.mask2, params.tmpCal2, params, 0.5f, CMPMODE::GE);

    // f17(tmpCal2) = f36 * f36
    Mul<float, false>(params.tmpCal2, params.tmpCal1, params.tmpCal1, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();
    // f30(dst) = (f17 * f36) * (0.333332300186 + f17 * (0.13337790966 + f17 *
    //            (0.053466796875 + f17 * (0.0242919921875 + f17 * (0.00326538085938 + 0.0093383789065 * f17))))) + f36
    Duplicate<float, false>(dst, 0.0093383789065f, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();
    constexpr size_t calcSize = 5;
    for (size_t i = 0U; i < calcSize; ++i) {
        Mul<float, false>(dst, dst, params.tmpCal2, MASK_PLACEHOLDER, 1, params.binaryParams);
        PipeBarrier<PIPE_V>();

        Adds<float, false>(dst, dst, picotCalcConst[i], MASK_PLACEHOLDER, 1, params.unaryParams);
        PipeBarrier<PIPE_V>();
    }
    // f30(tmpCal1) = f17 * f36 * dst + f36
    Mul<float, false>(dst, dst, params.tmpCal2, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();
    Mul<float, false>(dst, dst, params.tmpCal1, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();
    Add<float, false>(params.tmpCal1, dst, params.tmpCal1, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();

    // select f30
    Duplicate<float, false>(dst, 0.0f, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();
    DigammaSelect(dst, params.tmpCal1, params.mask2, params.tmpCal3, params);

    // f34(tmpCal1) = -1.0 / f30
    Duplicate<float, false>(params.tmpScalar, -1.0f, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();
    Div<float, false>(params.tmpCal1, params.tmpScalar, params.tmpCal1, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();
    // select f34
    DigammaSelect(dst, params.tmpCal1, params.mask1, params.tmpCal3, params);

    // dst = dst * pi
    Muls<float, false>(dst, dst, DIGAMMA_PI, MASK_PLACEHOLDER, 1, params.unaryParams);
    PipeBarrier<PIPE_V>();
}

// compute x is less than 0 and put the result on result
__ASC_USE_RESERVED_UBUF__(2201, "Digamma is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void DigammaNegative(
    const LocalTensor<float>& dst, const LocalTensor<float>& src, DigammaParams& params)
{
    // digamma_our_p(1 - x)
    Muls<float, false>(params.tmpCal3, src, -1.0f, MASK_PLACEHOLDER, 1, params.unaryParams);
    PipeBarrier<PIPE_V>();
    Adds<float, false>(params.tmpCal3, params.tmpCal3, 1.0f, MASK_PLACEHOLDER, 1, params.unaryParams);
    PipeBarrier<PIPE_V>();
    DigammaPositive(dst, params.tmpCal3, params);

    // picotpix_(x)
    DigammaNegPicotPix(params.tmpCal4, src, params);

    // digamma_our_p(1 - x) + picotpix_(x)
    Add<float, false>(dst, dst, params.tmpCal4, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();
}

__ASC_USE_RESERVED_UBUF__(2201, "Digamma is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void DigammaComputeImpl(
    const LocalTensor<float>& dst, const LocalTensor<float>& src, DigammaParams& params)
{
    Duplicate<float, false>(dst, 0.0f, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);

    // Special data is directly output as nan. x <= -8388608 || x == negative integer
    NotNumUnion notNum;
    notNum.i = F32_NAN;
    Duplicate<float, false>(params.result, notNum.f, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);

    // x <= -8388608 the result is nan, and select to dst
    DigammaGenCompareMask(params.mask, src, params, MIN_NEG_WITH_FLOAT, CMPMODE::LE);
    DigammaSelect(dst, params.result, params.mask, params.tmpCal3, params);

    // negative integer between (-8388608, 0) the result is nan,, and select to dst
    DigammaGenNegIntMask(params.mask1, src, params, MIN_NEG_WITH_FLOAT);
    DigammaSelect(dst, params.result, params.mask1, params.tmpCal3, params);

    // for nan
    DigammaGenNanMask(params.mask, src, params);
    DigammaSelect(dst, src, params.mask, params.tmpCal3, params);

    // Compute x >= 0, and select to dst
    DigammaGenCompareMask(params.mask, src, params, 0.0f, CMPMODE::GE);
    DigammaPositive(params.result, src, params);
    DigammaSelect(dst, params.result, params.mask, params.tmpCal3, params);

    // Compute x < 0, and select to dst
    DigammaGenCompareMask(params.mask, src, params, 0.0f, CMPMODE::LT);
    DigammaNegative(params.result, src, params);
    DigammaSelect(dst, params.result, params.mask, params.tmpCal3, params);
}

template <typename T, bool isReuseSource = false>
__ASC_USE_RESERVED_UBUF__(2201, "Digamma is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void DigammaCompute(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint8_t>& tmp, const uint32_t calCount)
{
    CHECK_FUNC_HIGHLEVEL_API(Digamma, (T, isReuseSource), (dst, src, tmp, calCount));

    LocalTensor<float> tmpBuffer = tmp.ReinterpretCast<float>();
    uint32_t tmpBufferSize = tmpBuffer.GetSize();
    uint32_t splitSize = tmpBufferSize;

    if (sizeof(T) == sizeof(float)) {
        if constexpr (isReuseSource) {
            splitSize = splitSize / DIGAMMA_FLOAT_REUSE_CALC_PROC / ONE_BLK_SIZE * ONE_BLK_SIZE;
        } else {
            splitSize = splitSize / DIGAMMA_FLOAT_NOREUSE_CALC_PROC / ONE_BLK_SIZE * ONE_BLK_SIZE;
        }
    } else {
        splitSize = splitSize / DIGAMMA_HALF_CALC_PROC / ONE_BLK_SIZE * ONE_BLK_SIZE;
    }

    CheckTmpBufferSize(splitSize, 0, tmpBufferSize);

    // init params
    DigammaParams params;
    DigammaInitParams<isReuseSource>(tmpBuffer, splitSize, src, params);

    const uint32_t loopCount = calCount / splitSize;
    uint32_t calcTail = calCount % splitSize;
    SetMaskCount();
    SetVectorMask<T>(0, splitSize);
    uint32_t offset = 0;
    for (uint32_t i = 0U; i < loopCount; ++i) {
        DigammaComputeImpl(dst[offset], src[offset], params);
        offset += splitSize;
    }

    if (calcTail > 0) {
        calcTail = (calcTail + ONE_BYTE_BIT_SIZE - 1U) / ONE_BYTE_BIT_SIZE * ONE_BYTE_BIT_SIZE;
        SetVectorMask<T>(0, calcTail);
        params.splitSize = calcTail;
        DigammaComputeImpl(dst[offset], src[offset], params);
    }
    SetMaskNorm();
    ResetMask();
}

#pragma end_pipe
} // namespace AscendC

#endif

#endif // IMPL_MATH_DIGAMMA_DIGAMMA_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_DIGAMMA_DIGAMMA_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_DIGAMMA_DIGAMMA_COMMON_IMPL_H__
#endif
