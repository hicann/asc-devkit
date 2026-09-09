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
 * \file cos_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/math/cos/cos_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/math/cos.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_COS_COS_COMMON_IMPL_H__
#endif
#ifndef IMPL_MATH_COS_COS_COMMON_IMPL_H
#define IMPL_MATH_COS_COS_COMMON_IMPL_H
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "../../common/check.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/math/cos/cos_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || \
                              __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "cos_v220_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
#include "cos_v200_impl.h"
#endif

namespace AscendC {
const uint8_t COS_HALF_CALC_PROCEDURE = 4;
const uint8_t COS_FLOAT_NOREUSE_CALC_PROCEDURE = 3;
const uint8_t COS_FLOAT_REUSE_CALC_PROCEDURE = 2;

// define the number of x div pi
constexpr float COS_PI_FOR_X_TODIV = 0.3183098733425140380859375;
// define the PI for compute
constexpr float PI_0 = 3.140625;
constexpr float COS_KPI_FIRS_PI_MULS = 0.0009670257568359375;
constexpr float COS_KPI_TWI_PI_MULS = 6.2771141529083251953125e-7;
constexpr float COS_KPI_THIR_PI_MULS = 1.21644916362129151821136474609375e-10;
constexpr float COS_KPI_FOR_PI_MULS = -1.0290623200529979163359041220560e-13;
// define the number of down of pi_div
constexpr float COS_PI_DOWN = 1.57079637050628662109375;
// kpi_2
constexpr float COS_PI_RESDOWN_ADDS_NEG = -0.00000004371139000189375;
// define the number of cos compute
constexpr float COS_RES_MULTI_SCA = 2.604926501e-6;
constexpr float COS_RES_ADDICT_UP = -0.0001980894471;
constexpr float COS_2ADDS = 0.008333049340;
constexpr float COS_3ADDS = -0.1666665792;
constexpr float COS_POINT_FIVE = 0.5;
constexpr float COS_M4_SCA = 4.0;
constexpr float COS_K2_SCA = -2.0;

__aicore__ inline void KPI(
    const LocalTensor<float>& inputX, const LocalTensor<float>& srcTensor, const LocalTensor<float>& roundTensor,
    const LocalTensor<float>& kpi)
{
    const UnaryRepeatParams unaryParams;
    const BinaryRepeatParams binaryParams;
    // x -= k * pi_0
    Muls<float, false>(kpi, roundTensor, PI_0, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Sub<float, false>(inputX, srcTensor, kpi, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();

    // x -= k * pi_1
    Muls<float, false>(kpi, roundTensor, COS_KPI_FIRS_PI_MULS, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Sub<float, false>(inputX, inputX, kpi, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();

    // x = x + COS_PI_DOWN
    Adds<float, false>(inputX, inputX, COS_PI_DOWN, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    // x -= k * pi_2
    Muls<float, false>(kpi, roundTensor, COS_KPI_TWI_PI_MULS, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Sub<float, false>(inputX, inputX, kpi, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();

    // x -= k * pi_3
    Muls<float, false>(kpi, roundTensor, COS_KPI_THIR_PI_MULS, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Sub<float, false>(inputX, inputX, kpi, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();

    // x -= k * pi_4
    Muls<float, false>(kpi, roundTensor, COS_KPI_FOR_PI_MULS, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Sub<float, false>(inputX, inputX, kpi, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();

    // x = x + COS_PI_RESDOWN_ADDS_NEG
    Adds<float, false>(inputX, inputX, COS_PI_RESDOWN_ADDS_NEG, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
}

__aicore__ inline void CosRound(
    const LocalTensor<float>& inputX, const LocalTensor<float>& srcTensor, const LocalTensor<float>& roundTensor,
    const LocalTensor<float>& kpi)
{
    /*
    k=round(x/π + 1/2), x0=x-kπ, x0 belongs to [-π, 0], (x0 + π/2) belongs to [-π/2, π/2]
    π=π_0+π_1+π_2+π_3+π_4 achieve final precision compensation.
    Final solution�?
    k = round(x * invpi + 1/2)
    x -= k * pi_0
    x -= k * pi_1
    x = x + COS_PI_DOWN
    x -= k * pi_2
    x -= k * pi_3
    x -= k * pi_4
    x = x + COS_PI_RESDOWN_ADDS_NEG
    */
    const UnaryRepeatParams unaryParams;
    Muls<float, false>(roundTensor, srcTensor, COS_PI_FOR_X_TODIV, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Adds<float, false>(roundTensor, roundTensor, COS_POINT_FIVE, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    // tie to even
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    CosCastFullMask(roundTensor, roundTensor, RoundMode::CAST_RINT);
#else
    CosCast(roundTensor, roundTensor, RoundMode::CAST_RINT);
#endif
    KPI(inputX, srcTensor, roundTensor, kpi);
}

__aicore__ inline void SignCompute(
    const LocalTensor<float>& dstTensor, const LocalTensor<float>& inputX, const LocalTensor<float>& roundTensor,
    const LocalTensor<float>& kpi)
{
    const UnaryRepeatParams unaryParams;
    const BinaryRepeatParams binaryParams;
    // x^2 = mul(input_x, input_x)
    Mul<float, false>(kpi, inputX, inputX, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
    // kover2
    Muls<float, false>(dstTensor, roundTensor, COS_POINT_FIVE, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102 || \
                              __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
    CosCastFullMask(dstTensor, dstTensor, RoundMode::CAST_FLOOR);
#else
    CosCast(dstTensor, dstTensor, RoundMode::CAST_FLOOR);
#endif

    // kover2floorm4
    Muls<float, false>(dstTensor, dstTensor, COS_M4_SCA, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    // k2
    Muls<float, false>(roundTensor, roundTensor, COS_K2_SCA, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    // sign
    Add<float, false>(dstTensor, dstTensor, roundTensor, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
    Adds<float, false>(dstTensor, dstTensor, 1, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
}

// use polynomial approximation
__aicore__ inline void CosPolynomialApproximation(
    const LocalTensor<float>& dstTensor, const LocalTensor<float>& inputX, const LocalTensor<float>& roundTensor,
    const LocalTensor<float>& kpi)
{
    /*
    cos(x) = (-1)^k*sin(x0 + π/2)
    Finally, use sin(x) = xP(x) to calculate sin(x).
    P(x) = (((x^2 * R0 + R1) * x^2 + R2) * x^2 + R3) * x^2 + 1.0
    */
    const UnaryRepeatParams unaryParams;
    const BinaryRepeatParams binaryParams;
    SignCompute(dstTensor, inputX, roundTensor, kpi);

    // res_up = mul(x^2, 2.604926501e-6)
    Muls<float, false>(roundTensor, kpi, COS_RES_MULTI_SCA, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Adds<float, false>(roundTensor, roundTensor, COS_RES_ADDICT_UP, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    // res_up = mul(res_up, x^2)
    Mul<float, false>(roundTensor, roundTensor, kpi, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
    Adds<float, false>(roundTensor, roundTensor, COS_2ADDS, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    // res_up = mul(res_up, x^2)
    Mul<float, false>(roundTensor, roundTensor, kpi, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
    Adds<float, false>(roundTensor, roundTensor, COS_3ADDS, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    // res_up = mul(res_up, x^2)
    Mul<float, false>(roundTensor, roundTensor, kpi, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
    Adds<float, false>(roundTensor, roundTensor, 1.0, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    // sin(x) = xP(x)
    Mul<float, false>(roundTensor, roundTensor, inputX, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
    Mul<float, false>(dstTensor, roundTensor, dstTensor, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();

    Mins<float, false>(dstTensor, dstTensor, 1.0, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
    Maxs<float, false>(dstTensor, dstTensor, -1.0, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
}

template <typename T>
__aicore__ inline void CosCompute(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<float>& tmpBuffer,
    const uint32_t calSize, bool isReuseSource)
{
    const LocalTensor<T>& roundTensor = tmpBuffer;
    const LocalTensor<T>& kpi = roundTensor[calSize];
    LocalTensor<T> inputX = srcTensor;
    if (!isReuseSource) {
        inputX = roundTensor[calSize * 2];
    }

    CosRound(inputX, srcTensor, roundTensor, kpi);
    CosPolynomialApproximation(dstTensor, inputX, roundTensor, kpi);
}

template <>
__aicore__ inline void CosCompute(
    const LocalTensor<half>& dstTensor, const LocalTensor<half>& srcTensor, const LocalTensor<float>& tmpBuffer,
    const uint32_t calSize, bool isReuseSource)
{
    (void)isReuseSource;
    const LocalTensor<float>& tempTensorConv = tmpBuffer;
    const LocalTensor<float>& roundTensor = tempTensorConv[calSize];
    const LocalTensor<float>& kpi = roundTensor[calSize];
    const LocalTensor<float>& inputX = kpi[calSize];

    Cast<float, half, false>(
        tempTensorConv, srcTensor, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        {1, 1, DEFAULT_REPEAT_STRIDE, HALF_DEFAULT_REPEAT_STRIDE});
    PipeBarrier<PIPE_V>();

    CosRound(inputX, tempTensorConv, roundTensor, kpi);
    CosPolynomialApproximation(tempTensorConv, inputX, roundTensor, kpi);

    Cast<half, float, false>(
        dstTensor, tempTensorConv, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        {1, 1, HALF_DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
    PipeBarrier<PIPE_V>();
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void CosImpl(
    const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const uint32_t calCount)
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    CHECK_FUNC_HIGHLEVEL_API(Cos, (T, isReuseSource), (dstTensor, srcTensor, sharedTmpBuffer, calCount));

    const uint32_t bufferSize = sharedTmpBuffer.GetSize();
    const uint32_t tmpBufferSize = bufferSize / sizeof(float);
    CheckTmpBufferSize(tmpBufferSize, 0, bufferSize);
    // all temporary variables are float type.
    LocalTensor<float> tmpBuffer = sharedTmpBuffer.ReinterpretCast<float>();

    uint32_t calSize = 0;
    if constexpr (sizeof(T) == sizeof(half)) {
        calSize = tmpBufferSize / COS_HALF_CALC_PROCEDURE / ONE_BLK_SIZE * ONE_BLK_SIZE; // 32 byte
    } else {
        if constexpr (isReuseSource) {
            calSize = tmpBufferSize / COS_FLOAT_REUSE_CALC_PROCEDURE / ONE_BLK_SIZE * ONE_BLK_SIZE;
        } else {
            calSize = tmpBufferSize / COS_FLOAT_NOREUSE_CALC_PROCEDURE / ONE_BLK_SIZE * ONE_BLK_SIZE;
        }
    }
    CheckTmpBufferSize(calSize, 0, bufferSize);

    const uint32_t round = calCount / calSize;
    const uint32_t tail = calCount % calSize;

    SetMaskCount();
    SetVectorMask<T, MaskMode::COUNTER>(0, calSize);

    uint32_t offset = 0;
    for (uint32_t i = 0; i < round; i++) {
        CosCompute(dstTensor[offset], srcTensor[offset], tmpBuffer, calSize, isReuseSource);
        offset = offset + calSize;
    }

    if (tail > 0) {
        SetVectorMask<T, MaskMode::COUNTER>(0, tail);
        CosCompute(dstTensor[offset], srcTensor[offset], tmpBuffer, calSize, isReuseSource);
    }

    SetMaskNorm();
    ResetMask();
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void CosImpl(
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
    CosImpl<T, isReuseSource>(dstTensor, srcTensor, sharedTmpBuffer, calCount);
}
} // namespace AscendC

#endif // IMPL_MATH_COS_COS_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_COS_COS_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MATH_COS_COS_COMMON_IMPL_H__
#endif
