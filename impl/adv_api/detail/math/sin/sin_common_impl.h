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
 * \file sin_common_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/sin/sin_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/sin.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_SIN_SIN_COMMON_IMPL_H__
#endif
#ifndef IMPL_MATH_SIN_SIN_COMMON_IMPL_H
#define IMPL_MATH_SIN_SIN_COMMON_IMPL_H
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "../../common/check.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/sin/sin_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510)
#include "sin_v220_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
#include "sin_v200_impl.h"
#endif

namespace AscendC {
const uint8_t SIN_HALF_CALC_PROCEDURE = 4;
const uint8_t SIN_FLOAT_NOREUSE_CALC_PROCEDURE = 3;
const uint8_t SIN_FLOAT_REUSE_CALC_PROCEDURE = 2;

// define the number of x div pi
constexpr float SIN_PI_FOR_X_TODIV = 0.3183098733425140380859375;
// define the PI for compute
constexpr float SIN_PI_V2 = 3.140625;
constexpr float SIN_KPI_FIRS_PI_MULS = 0.0009670257568359375;
constexpr float SIN_KPI_TWI_PI_MULS = 6.2771141529083251953125e-7;
constexpr float SIN_KPI_THIR_PI_MULS = 1.21644916362129151821136474609375e-10;
// define the number of sin compute
constexpr float SIN_RES_MULTI_SCA = 2.604926501e-6;
constexpr float SIN_RES_ADDICT_UP = -0.0001980894471;
constexpr float SIN_2ADDS = 0.008333049340;
constexpr float SIN_3ADDS = -0.1666665792;
constexpr float SIN_POINT_FIVE = 0.5;
constexpr float SIN_M4_SCA = 4.0;
constexpr float SIN_K2_SCA = -2.0;

__aicore__ inline void SinSignCompute(
    const LocalTensor<float>& dstTensor, const LocalTensor<float>& inputX, const LocalTensor<float>& roundTensor,
    const LocalTensor<float>& kpi)
{
    const UnaryRepeatParams unaryParams;
    const BinaryRepeatParams binaryParams;
    // x^2 = mul(input_x, input_x)
    Mul<float, false>(kpi, inputX, inputX, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
    // kover2
    Muls<float, false>(dstTensor, roundTensor, SIN_POINT_FIVE, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    SinCastFullMask(dstTensor, dstTensor, RoundMode::CAST_FLOOR);
#else
    SinCast(dstTensor, dstTensor, RoundMode::CAST_FLOOR);
#endif
    // kover2floorm4
    Muls<float, false>(dstTensor, dstTensor, SIN_M4_SCA, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    // k2
    Muls<float, false>(roundTensor, roundTensor, SIN_K2_SCA, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    // sign
    Add<float, false>(dstTensor, dstTensor, roundTensor, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
    Adds<float, false>(dstTensor, dstTensor, 1, MASK_PLACEHOLDER, 1, unaryParams); // sign is dstTensor
    PipeBarrier<PIPE_V>();
}

// in [-pi/2, pi/2] use polynomial approximation
__aicore__ inline void SinPolynomialApproximation(
    const LocalTensor<float>& dstTensor, const LocalTensor<float>& inputX, const LocalTensor<float>& roundTensor,
    const LocalTensor<float>& kpi)
{
    /*
    sin(x) = (-1)^k*sin(x0)
    Finally, use sin(x) = xP(x) to calculate sin(x).
    P(x) = (((x^2 * R0 + R1) * x^2 + R2) * x^2 + R3) * x^2 + 1.0
    */
    const UnaryRepeatParams unaryParams;
    const BinaryRepeatParams binaryParams;
    SinSignCompute(dstTensor, inputX, roundTensor, kpi);

    // res_up = mul(x^2, 2.604926501e-6)
    Muls<float, false>(roundTensor, kpi, SIN_RES_MULTI_SCA, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Adds<float, false>(roundTensor, roundTensor, SIN_RES_ADDICT_UP, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    // res_up = mul(res_up, x^2)
    Mul<float, false>(roundTensor, roundTensor, kpi, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
    Adds<float, false>(roundTensor, roundTensor, SIN_2ADDS, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    // res_up = mul(res_up, x^2)
    Mul<float, false>(roundTensor, roundTensor, kpi, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
    Adds<float, false>(roundTensor, roundTensor, SIN_3ADDS, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    // res_up = mul(res_up, x^2)
    Mul<float, false>(roundTensor, roundTensor, kpi, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
    Adds<float, false>(roundTensor, roundTensor, 1.0, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    // res_up = mul(res_up, input_x)
    Mul<float, false>(roundTensor, roundTensor, inputX, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
    Mul<float, false>(dstTensor, roundTensor, dstTensor, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
}

__aicore__ inline void SinKpi(
    const LocalTensor<float>& inputX, const LocalTensor<float>& srcTensor, const LocalTensor<float>& roundTensor,
    const LocalTensor<float>& kpi)
{
    const UnaryRepeatParams unaryParams;
    const BinaryRepeatParams binaryParams;

    // x -= k * pi_0
    Muls<float, false>(kpi, roundTensor, SIN_PI_V2, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Sub<float, false>(inputX, srcTensor, kpi, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();

    // x -= k * pi_1
    Muls<float, false>(kpi, roundTensor, SIN_KPI_FIRS_PI_MULS, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Sub<float, false>(inputX, inputX, kpi, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();

    // x -= k * pi_2
    Muls<float, false>(kpi, roundTensor, SIN_KPI_TWI_PI_MULS, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Sub<float, false>(inputX, inputX, kpi, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();

    // x -= k * pi_3
    Muls<float, false>(kpi, roundTensor, SIN_KPI_THIR_PI_MULS, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Sub<float, false>(inputX, inputX, kpi, MASK_PLACEHOLDER, 1, binaryParams); // inputX has compute
    PipeBarrier<PIPE_V>();
}

// normalized x to [-π/2, π/2] using x = x-round(x/π)*π
__aicore__ inline void SinRound(
    const LocalTensor<float>& inputX, const LocalTensor<float>& srcTensor, const LocalTensor<float>& roundTensor,
    const LocalTensor<float>& kpi)
{
    /*
    k=round(x/π), x0=x-kπ, x0 belongs to [-π/2, π/2]
    π=π_0+π_1+π_2+π_3 achieve final precision compensation.
    Final solution:
    k = round(x * invpi)
    x -= k * pi_0
    x -= k * pi_1
    x -= k * pi_2
    x -= k * pi_3
    */
    const UnaryRepeatParams unaryParams;
    Muls<float, false>(roundTensor, srcTensor, SIN_PI_FOR_X_TODIV, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
    SinCastFullMask(roundTensor, roundTensor, RoundMode::CAST_ROUND);
#else
    SinCast(roundTensor, roundTensor, RoundMode::CAST_ROUND);
#endif
    SinKpi(inputX, srcTensor, roundTensor, kpi); // roundTensor has compute
}

template <typename T>
__aicore__ inline void SinCompute(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<float>& tmpTensor,
    const uint32_t splitSize, bool isReuseSource)
{
    const BinaryRepeatParams binParams;
    LocalTensor<T> roundTensor = tmpTensor;
    LocalTensor<T> kpi = tmpTensor[splitSize];
    LocalTensor<T> inputX = srcTensor;
    if (!isReuseSource) {
        inputX = tmpTensor[splitSize * 2];
    }
    SinRound(inputX, srcTensor, roundTensor, kpi);
    SinPolynomialApproximation(dstTensor, inputX, roundTensor, kpi);
}

template <>
__aicore__ inline void SinCompute(
    const LocalTensor<half>& dstTensor, const LocalTensor<half>& srcTensor, const LocalTensor<float>& tmpTensor,
    const uint32_t splitSize, bool isReuseSource)
{
    (void)isReuseSource;
    const BinaryRepeatParams binParams;
    const LocalTensor<float>& tmpBuffer = tmpTensor;
    const LocalTensor<float>& roundTensor = tmpBuffer[splitSize];
    const LocalTensor<float>& kpi = roundTensor[splitSize];
    const LocalTensor<float>& inputX = kpi[splitSize];

    Cast<float, half, false>(
        tmpBuffer, srcTensor, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        {1, 1, DEFAULT_REPEAT_STRIDE, HALF_DEFAULT_REPEAT_STRIDE});
    PipeBarrier<PIPE_V>();

    // The input is normalized to [-pi/2, pi/2].
    SinRound(inputX, tmpBuffer, roundTensor, kpi); // half cannot use dst as a temporary buffer.
    SinPolynomialApproximation(tmpBuffer, inputX, roundTensor, kpi);

    Cast<half, float, false>(
        dstTensor, tmpBuffer, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        {1, 1, HALF_DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
    PipeBarrier<PIPE_V>();
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void SinImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
    CHECK_FUNC_HIGHLEVEL_API(Sin, (T, isReuseSource), (dstTensor, srcTensor, sharedTmpBuffer, calCount));

    const uint32_t bufferSize = sharedTmpBuffer.GetSize();
    const uint32_t tmpBufferSize = bufferSize / sizeof(float);
    CheckTmpBufferSize(tmpBufferSize, 0, bufferSize);
    LocalTensor<float> tmpBuffer = sharedTmpBuffer.ReinterpretCast<float>();
    uint32_t stackSize = 0;
    if constexpr (sizeof(T) == sizeof(half)) {
        stackSize = tmpBufferSize / SIN_HALF_CALC_PROCEDURE / ONE_BLK_SIZE * ONE_BLK_SIZE; // 32 byte
    } else {
        if constexpr (isReuseSource) {
            stackSize = tmpBufferSize / SIN_FLOAT_REUSE_CALC_PROCEDURE / ONE_BLK_SIZE * ONE_BLK_SIZE;
        } else {
            stackSize = tmpBufferSize / SIN_FLOAT_NOREUSE_CALC_PROCEDURE / ONE_BLK_SIZE * ONE_BLK_SIZE;
        }
    }
    CheckTmpBufferSize(stackSize, 0, bufferSize);

    const uint32_t round = calCount / stackSize;
    const uint32_t tail = calCount % stackSize;
    SetMaskCount();
    SetVectorMask<T, MaskMode::COUNTER>(0, stackSize);
    uint32_t offset = 0;
    for (uint32_t i = 0; i < round; i++) {
        SinCompute(dstTensor[offset], srcTensor[offset], tmpBuffer, stackSize, isReuseSource);
        offset = offset + stackSize;
    }

    if (tail != 0) {
        SetVectorMask<T, MaskMode::COUNTER>(0, tail);
        SinCompute(dstTensor[offset], srcTensor[offset], tmpBuffer, stackSize, isReuseSource);
    }
    SetMaskNorm();
    ResetMask();
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void SinImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }

    // Using the Stack Space to Allocate tmpBuffer
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    SinImpl<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, calCount);
}
} // namespace AscendC

#endif // IMPL_MATH_SIN_SIN_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_SIN_SIN_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_SIN_SIN_COMMON_IMPL_H__
#endif
