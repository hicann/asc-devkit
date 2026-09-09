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
 * \file digamma_common_basic_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/digamma/digamma_common_basic_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/digamma.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_DIGAMMA_DIGAMMA_COMMON_BASIC_IMPL_H__
#endif
#ifndef IMPL_MATH_DIGAMMA_DIGAMMA_COMMON_BASIC_IMPL_H
#define IMPL_MATH_DIGAMMA_DIGAMMA_COMMON_BASIC_IMPL_H
#include <cstdint>
#include "../../../../../include/basic_api/kernel_basic_intf.h"

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102)
#include "digamma_3510_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "digamma_v220_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
#include "digamma_v200_impl.h"
#endif

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002)
namespace AscendC {
namespace {
constexpr float MIN_NEG_WITH_FLOAT = -8388608.0;
constexpr float DIGAMMA_PI = 3.141592653589793238f;
constexpr float DIGAMMA_NEG_PI = -3.141592653589793238f;
constexpr uint32_t DIGAMMA_FLOAT_NOREUSE_CALC_PROC = 7;
constexpr uint32_t DIGAMMA_FLOAT_REUSE_CALC_PROC = 6;
constexpr uint32_t DIGAMMA_HALF_CALC_PROC = 8;
constexpr size_t DIGAMMA_MAX_LOOP = 5;

constexpr float posCalcConst[] = {2.10927960927960927961e-2, 7.57575757575757575758e-3, 4.16666666666666666667e-3,
                                  3.96825396825396825397e-3, 8.33333333333333333333e-3, 8.33333333333333333333e-2};
constexpr float tmp1CalcConst[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
constexpr float tmp1HalfCalcConst[] = {1.0, 2.0};
constexpr float picotCalcConst[] = {
    0.00326538085938f, 0.0242919921875f, 0.053466796875f, 0.133377909660f, 0.333332300186f};
} // namespace

struct DigammaParams {
    __aicore__ DigammaParams() {}
    LocalTensor<float> result;
    LocalTensor<float> tmpCal1;
    LocalTensor<float> tmpCal2;
    LocalTensor<float> tmpCal3;
    LocalTensor<float> tmpCal4;
    LocalTensor<float> tmpCal5;
    LocalTensor<float> tmpScalar;
    LocalTensor<uint8_t> mask;
    LocalTensor<uint8_t> mask1;
    LocalTensor<uint8_t> mask2;
    UnaryRepeatParams unaryParams;
    BinaryRepeatParams binaryParams;
    uint32_t splitSize;
};
#pragma begin_pipe(V)
// Calculate the mask based on cmpMode, will use tmpScalar
__aicore__ inline void DigammaGenCompareMask(
    const LocalTensor<uint8_t>& mask, const LocalTensor<float>& src, DigammaParams& params, const float scalar,
    CMPMODE cmpMode)
{
    Duplicate<float, false>(params.tmpScalar, scalar, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();

    uint8_t repeat = DivCeil(params.splitSize * sizeof(float), ONE_REPEAT_BYTE_SIZE);
    Compare<float, uint8_t, false>(mask, src, params.tmpScalar, cmpMode, MASK_PLACEHOLDER, repeat, params.binaryParams);
    PipeBarrier<PIPE_V>();
}

// get negative integer mask
__aicore__ inline void DigammaGenNegIntMask(
    const LocalTensor<uint8_t>& mask, const LocalTensor<float>& src, DigammaParams& params, const float scalar)
{
    // x < 0 &  x > -8388608.0
    DigammaGenCompareMask(params.mask1, src, params, 0.0f, CMPMODE::LT);
    DigammaGenCompareMask(params.mask2, src, params, MIN_NEG_WITH_FLOAT, CMPMODE::GT);

    SetVectorMask<float>(0, ConstCeil(params.splitSize, sizeof(uint16_t) * ONE_BYTE_BIT_SIZE));
    And<uint16_t, false>(
        params.mask1.ReinterpretCast<uint16_t>(), params.mask1.ReinterpretCast<uint16_t>(),
        params.mask2.ReinterpretCast<uint16_t>(), MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();
    SetVectorMask<float>(0, params.splitSize);

    DigammaCast(params.tmpCal1, src, RoundMode::CAST_ROUND);
    uint8_t repeat = DivCeil(params.splitSize * sizeof(float), ONE_REPEAT_BYTE_SIZE);
    Compare<float, uint8_t, false>(
        params.mask2, src, params.tmpCal1, CMPMODE::EQ, MASK_PLACEHOLDER, repeat, params.binaryParams);
    PipeBarrier<PIPE_V>();

    SetVectorMask<float>(0, ConstCeil(params.splitSize, sizeof(uint16_t) * ONE_BYTE_BIT_SIZE));
    And<uint16_t, false>(
        mask.ReinterpretCast<uint16_t>(), params.mask1.ReinterpretCast<uint16_t>(),
        params.mask2.ReinterpretCast<uint16_t>(), MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();
    SetVectorMask<float>(0, params.splitSize);
}

// get range mask
__aicore__ inline void DigammaGenRangeMask(
    const LocalTensor<uint8_t>& mask, const LocalTensor<float>& src, DigammaParams& params, const float min,
    const float max)
{
    //  -0.0001 <= x && x < 0.0
    DigammaGenCompareMask(params.mask1, src, params, max, CMPMODE::LT);
    DigammaGenCompareMask(params.mask2, src, params, min, CMPMODE::GE);

    SetVectorMask<float>(0, ConstCeil(params.splitSize, sizeof(uint16_t) * ONE_BYTE_BIT_SIZE));
    And<uint16_t, false>(
        mask.ReinterpretCast<uint16_t>(), params.mask1.ReinterpretCast<uint16_t>(),
        params.mask2.ReinterpretCast<uint16_t>(), MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();
    SetVectorMask<float>(0, params.splitSize);
}

// get nan mask
__aicore__ inline void DigammaGenNanMask(
    const LocalTensor<uint8_t>& mask, const LocalTensor<float>& src, DigammaParams& params)
{
    DigammaGenCompareMask(params.mask1, src, params, 0.0f, CMPMODE::LT);
    DigammaGenCompareMask(params.mask2, src, params, 0.0f, CMPMODE::GE);

    SetVectorMask<float>(0, ConstCeil(params.splitSize, sizeof(uint16_t) * ONE_BYTE_BIT_SIZE));
    Not<uint16_t, false>(
        params.mask1.ReinterpretCast<uint16_t>(), params.mask1.ReinterpretCast<uint16_t>(), MASK_PLACEHOLDER, 1,
        params.unaryParams);
    Not<uint16_t, false>(
        params.mask2.ReinterpretCast<uint16_t>(), params.mask2.ReinterpretCast<uint16_t>(), MASK_PLACEHOLDER, 1,
        params.unaryParams);
    PipeBarrier<PIPE_V>();
    And<uint16_t, false>(
        mask.ReinterpretCast<uint16_t>(), params.mask1.ReinterpretCast<uint16_t>(),
        params.mask2.ReinterpretCast<uint16_t>(), MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();
    SetVectorMask<float>(0, params.splitSize);
}

// Select the value of src at mask, and accumulate the result onto dst, used tmpScalar
__ASC_USE_RESERVED_UBUF__(2201, "Digamma is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void DigammaSelect(
    const LocalTensor<float>& dst, const LocalTensor<float>& src, const LocalTensor<uint8_t>& mask,
    const LocalTensor<float>& tmp, DigammaParams& params)
{
    Duplicate<float, false>(params.tmpScalar, 0.0f, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();

    SetCmpMask<float>(params.tmpScalar);
    PipeBarrier<PIPE_V>();
    Select<float, uint8_t>(tmp, mask, src, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();

    Add<float, false>(dst, tmp, dst, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();
}

// compute -0.0001 <= x && x < 0.0, and put the result on result
__aicore__ inline void DigammaNegativeRange(
    const LocalTensor<float>& dst, const LocalTensor<float>& src, DigammaParams& params)
{
    // tmp(tmpCal3) = sin(pi * (x - floor(x)))
    DigammaCast(params.tmpScalar, src, RoundMode::CAST_FLOOR);
    Sub<float, false>(params.tmpScalar, src, params.tmpScalar, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();
    Muls<float, false>(params.tmpScalar, params.tmpScalar, DIGAMMA_PI, MASK_PLACEHOLDER, 1, params.unaryParams);
    PipeBarrier<PIPE_V>();
    CosCompute<float>(params.tmpCal3, params.tmpScalar, params.result, params.splitSize, true);

    // tmp1(tmpScalar) = cos(-pi * x)SinCompute
    Muls<float, false>(src, src, DIGAMMA_NEG_PI, MASK_PLACEHOLDER, 1, params.unaryParams);
    PipeBarrier<PIPE_V>();
    SinCompute<float>(params.tmpScalar, src, params.result, params.splitSize, true);

    // Positive(1 - x) - pi * tmp / tmp1
    Muls<float, false>(params.tmpCal3, params.tmpCal3, DIGAMMA_PI, MASK_PLACEHOLDER, 1, params.unaryParams);
    PipeBarrier<PIPE_V>();
    Div<float, false>(params.tmpCal3, params.tmpCal3, params.tmpScalar, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();

    // tmpCal2 is Positive(1 - x), calculating saves in DigammaNegativeHalf
    Sub<float, false>(dst, params.tmpCal2, params.tmpCal3, MASK_PLACEHOLDER, 1, params.binaryParams);
    PipeBarrier<PIPE_V>();
}

template <bool isReuseSource = false>
__aicore__ inline void DigammaInitParams(
    const LocalTensor<float>& tmp, const uint32_t& splitSize, const LocalTensor<half>& src, DigammaParams& params)
{
    params.result = tmp;
    params.tmpCal1 = params.result[splitSize];
    params.tmpCal2 = params.tmpCal1[splitSize];
    params.tmpCal3 = params.tmpCal2[splitSize];
    params.tmpCal4 = params.tmpCal3[splitSize];
    params.tmpCal5 = params.tmpCal4[splitSize];
    params.tmpScalar = params.tmpCal5[splitSize];
    params.mask = params.tmpScalar[splitSize].ReinterpretCast<uint8_t>();
    params.mask1 = params.mask[splitSize];
    params.mask2 = params.mask1[splitSize];

    // result for TanCompute tmpsize, so size is splitSize * 4
    params.result.SetSize(splitSize * 4);
    params.tmpCal1.SetSize(splitSize);
    params.tmpCal2.SetSize(splitSize);
    params.tmpCal3.SetSize(splitSize);
    params.tmpCal4.SetSize(splitSize);
    params.tmpCal5.SetSize(splitSize);
    params.tmpScalar.SetSize(splitSize);
    params.mask.SetSize(splitSize);
    params.mask1.SetSize(splitSize);
    params.mask2.SetSize(splitSize);

    params.splitSize = splitSize;
}

template <bool isReuseSource = false>
__aicore__ inline void DigammaInitParams(
    const LocalTensor<float>& tmp, const uint32_t& splitSize, const LocalTensor<float>& src, DigammaParams& params)
{
    params.result = tmp;
    params.tmpCal1 = tmp[splitSize];
    params.tmpCal2 = params.tmpCal1[splitSize];
    params.tmpCal3 = params.tmpCal2[splitSize];
    if constexpr (isReuseSource) {
        params.tmpCal4 = src;
        params.tmpScalar = params.tmpCal3[splitSize];
    } else {
        params.tmpCal4 = params.tmpCal3[splitSize];
        params.tmpScalar = params.tmpCal4[splitSize];
    }
    params.mask = params.tmpScalar[splitSize].ReinterpretCast<uint8_t>();
    params.mask1 = params.mask[splitSize];
    params.mask2 = params.mask1[splitSize];

    params.result.SetSize(splitSize);
    params.tmpCal1.SetSize(splitSize);
    params.tmpCal2.SetSize(splitSize);
    params.tmpCal3.SetSize(splitSize);
    params.tmpCal4.SetSize(splitSize);
    params.tmpScalar.SetSize(splitSize);
    params.mask.SetSize(splitSize);
    params.mask1.SetSize(splitSize);
    params.mask2.SetSize(splitSize);

    params.splitSize = splitSize;
}
#pragma end_pipe
} // namespace AscendC
#endif
#endif // IMPL_MATH_DIGAMMA_DIGAMMA_COMMON_BASIC_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_DIGAMMA_DIGAMMA_COMMON_BASIC_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_DIGAMMA_DIGAMMA_COMMON_BASIC_IMPL_H__
#endif
