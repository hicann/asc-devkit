/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/* !
 * \file layernorm_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/normalization/layernorm/layernorm_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/layernorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORM_LAYERNORM_COMMON_IMPL_H__
#endif
#ifndef IMPL_NORMALIZATION_LAYERNORM_LAYERNORM_COMMON_IMPL_H
#define IMPL_NORMALIZATION_LAYERNORM_LAYERNORM_COMMON_IMPL_H

#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "kernel_tiling/kernel_tiling.h"
#include "../../../../../include/adv_api/normalization/normalize.h"
#include "layernorm_common_basic_impl.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/normalization/layernorm/layernorm_check.h"
#include "../../api_check/kernel_check/normalization/welforupdate/welfordupdate_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"

namespace AscendC {

template <typename T>
__aicore__ inline void GetLayerNormOutput(
    const LocalTensor<T>& output, const LocalTensor<float>& inputY, const LocalTensor<T>& gamma,
    const LocalTensor<T>& beta, const LayerNormTiling& tiling, const LayerNormParams<float>& params)
{}

template <>
__aicore__ inline void GetLayerNormOutput<half>(
    const LocalTensor<half>& output, const LocalTensor<float>& inputY, const LocalTensor<half>& gamma,
    const LocalTensor<half>& beta, const LayerNormTiling& tiling, const LayerNormParams<float>& params)
{
    LocalTensor<float> tempTensorA = params.tempTensorA;
    LocalTensor<float> tempTensorB = params.tempTensorB;
    LocalTensor<float> tempTensorC = params.tempTensorC;

    SetVectorMask<uint8_t, MaskMode::COUNTER>(0, tiling.hLength);

    UnaryRepeatParams unaryParams;
    unaryParams.srcRepStride = DEFAULT_REPEAT_STRIDE / sizeof(half);
    Cast<float, half, false>(tempTensorA, gamma, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    DuplicateMulImpl(tempTensorB, inputY, tempTensorA, tiling.bsCurLength, tiling.hLength);

    Cast<float, half, false>(tempTensorC, beta, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    DuplicateAddImpl(tempTensorA, tempTensorB, tempTensorC, tiling.bsCurLength, tiling.hLength);

    SetVectorMask<uint8_t, MaskMode::COUNTER>(0, tiling.bshCurLength);
    unaryParams.srcRepStride = DEFAULT_REPEAT_STRIDE;
    unaryParams.dstRepStride = DEFAULT_REPEAT_STRIDE / sizeof(half);

    Cast<half, float, false>(output, tempTensorA, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
}

template <>
__aicore__ inline void GetLayerNormOutput<float>(
    const LocalTensor<float>& output, const LocalTensor<float>& inputY, const LocalTensor<float>& gamma,
    const LocalTensor<float>& beta, const LayerNormTiling& tiling, const LayerNormParams<float>& params)
{
    LocalTensor<float> tempTensorA = params.tempTensorA;

    SetVectorMask<uint8_t, MaskMode::COUNTER>(0, tiling.hLength);

    DuplicateMulImpl(tempTensorA, inputY, gamma, tiling.bsCurLength, tiling.hLength);

    DuplicateAddImpl(output, tempTensorA, beta, tiling.bsCurLength, tiling.hLength);
}

template <typename T>
__aicore__ inline void LayerNormExe(
    const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta, const LocalTensor<T>& output,
    const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance, const T epsilon,
    const LayerNormTiling& tiling, const LayerNormParams<float>& params)
{}

template <>
__ASC_USE_RESERVED_UBUF__(2201, "LayerNorm is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void LayerNormExe<half>(
    const LocalTensor<half>& inputX, const LocalTensor<half>& gamma, const LocalTensor<half>& beta,
    const LocalTensor<half>& output, const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance,
    const half epsilon, const LayerNormTiling& tiling, const LayerNormParams<float>& params)
{
    LocalTensor<float> tempTensorA = params.tempTensorA;
    LocalTensor<float> tempTensorB = params.tempTensorB;
    LocalTensor<float> tempTensorC = params.tempTensorC;

    SetVectorMask<uint8_t, MaskMode::COUNTER>(0, tiling.bshCurLength);

    UnaryRepeatParams unaryParams;
    unaryParams.srcRepStride = DEFAULT_REPEAT_STRIDE / sizeof(half);
    Cast<float, half, false>(tempTensorA, inputX, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    GetLayerNormOutputMean(tempTensorB, tempTensorA, tiling, params, outputMean);

    GetLayerNormOutputVariance(tempTensorC, tempTensorA, tempTensorB, tiling, params, outputVariance);

    GetLayerNormOutputPre(tempTensorB, tempTensorC, static_cast<float>(epsilon), tiling, params);

    GetLayerNormOutput(output, tempTensorC, gamma, beta, tiling, params);
}

template <>
__ASC_USE_RESERVED_UBUF__(2201, "LayerNorm is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void LayerNormExe<float>(
    const LocalTensor<float>& inputX, const LocalTensor<float>& gamma, const LocalTensor<float>& beta,
    const LocalTensor<float>& output, const LocalTensor<float>& outputMean, const LocalTensor<float>& outputVariance,
    const float epsilon, const LayerNormTiling& tiling, const LayerNormParams<float>& params)
{
    LocalTensor<float> tempTensorA = params.tempTensorA;
    LocalTensor<float> tempTensorB = params.tempTensorB;
    LocalTensor<float> tempTensorC = params.tempTensorC;

    GetLayerNormOutputMean(tempTensorB, inputX, tiling, params, outputMean);

    GetLayerNormOutputVariance(tempTensorC, inputX, tempTensorB, tiling, params, outputVariance);

    GetLayerNormOutputPre(tempTensorB, tempTensorC, epsilon, tiling, params);

    GetLayerNormOutput(output, tempTensorC, gamma, beta, tiling, params);
}

template <typename T>
__aicore__ inline void LayerNormND(
    const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta, const LocalTensor<T>& output,
    const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance, const T epsilon, LayerNormTiling& tiling,
    const LayerNormParams<float>& params)
{
    uint32_t inputOffset = 0;
    uint32_t mvOffset = 0;

    for (uint32_t index = 0; index < tiling.loopRound; index++) {
        LayerNormExe<T>(
            inputX[inputOffset], gamma, beta, output[inputOffset], params.meanTmpTensor[mvOffset],
            params.varianceTmpTensor[mvOffset], epsilon, tiling, params);

        inputOffset += tiling.inputRoundSize;
        mvOffset += tiling.meanVarRoundSize;
    }

    if (tiling.inputTailSize > 0) {
        tiling.bshCurLength = tiling.inputTailSize;
        tiling.bsCurLength = tiling.meanVarTailSize;

        inputOffset = tiling.inputTailPos;
        mvOffset = tiling.meanVarTailPos;

        LayerNormExe<T>(
            inputX[inputOffset], gamma, beta, output[inputOffset], params.meanTmpTensor[mvOffset],
            params.varianceTmpTensor[mvOffset], epsilon, tiling, params);
    }

    if constexpr (sizeof(T) == sizeof(half)) {
        GetOutputMeanVariance(outputMean, outputVariance, tiling, params);
    }
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void LayerNormImpl(
    const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance,
    const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const T epsilon, LayerNormTiling& tiling)
{
    TRACE_START(TraceId::LayerNorm);
    ASCENDC_ASSERT((tiling.oneTmpSize > 0), { KERNEL_LOG(KERNEL_ERROR, "tiling.oneTmpSize must > 0!"); });

    if ASCEND_IS_AIC {
        TRACE_STOP(TraceId::LayerNorm);
        return;
    }

    CHECK_FUNC_HIGHLEVEL_API(
        LayerNorm, (T, isReuseSource),
        (output, outputMean, outputVariance, inputX, gamma, beta, sharedTmpBuffer, epsilon, tiling));

    LocalTensor<float> stackBuffer = sharedTmpBuffer.ReinterpretCast<float>();
    ASCENDC_ASSERT((stackBuffer.GetSize() > 0), { KERNEL_LOG(KERNEL_ERROR, "sharedTmpBuffer size must > 0!"); });

    LayerNormParams<float> params;
    GetLayerNormNDTensorInfo<T, isReuseSource>(inputX, outputMean, outputVariance, stackBuffer, tiling, params);

    SetMaskCount();
    LayerNormND<T>(inputX, gamma, beta, output, outputMean, outputVariance, epsilon, tiling, params);

    SetMaskNorm();
    ResetMask();
    TRACE_STOP(TraceId::LayerNorm);
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void LayerNormImpl(
    const LocalTensor<T>& output, const LocalTensor<T>& outputMean, const LocalTensor<T>& outputVariance,
    const LocalTensor<T>& inputX, const LocalTensor<T>& gamma, const LocalTensor<T>& beta, const T epsilon,
    LayerNormTiling& tiling)
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });

    LayerNormImpl<T, isReuseSource>(
        output, outputMean, outputVariance, inputX, gamma, beta, sharedTmpBuffer, epsilon, tiling);
}

template <typename T>
__aicore__ inline void ComputeMeanVariance(
    const LocalTensor<float>& outputMean, const LocalTensor<float>& variance, const LocalTensor<T>& src,
    const LayerNormRstdTmpTensorParams<float>& params, const LayerNormPara& para, const LayerNormSeparateTiling& tiling)
{
    if constexpr (IsSameType<T, half>::value) {
        Cast<float, T>(params.tempTensorA, src, RoundMode::CAST_NONE, tiling.arCurLength);
        PipeBarrier<PIPE_V>();
    } else {
        DataCopy(params.tempTensorA, src, tiling.arCurLength);
        PipeBarrier<PIPE_V>();
    }

    Muls<float>(params.tempTensorA, params.tempTensorA, tiling.rValueBack, tiling.arCurLength);
    PipeBarrier<PIPE_V>();

    // outputMean[1,A], B[A,1], C[A,RWP]
    LayerNormReduceSumImpl(
        outputMean, params.tempTensorB, params.tempTensorA, tiling.aCurLength, para.rLengthWithPadding);
    auto eventId = GetTPipePtr()->FetchEventID(HardEvent::V_S);
    SetFlag<HardEvent::V_S>(eventId);
    WaitFlag<HardEvent::V_S>(eventId);

    if constexpr (IsSameType<T, half>::value) {
        PipeBarrier<PIPE_V>();
        Cast<float, T>(params.tempTensorA, src, RoundMode::CAST_NONE, tiling.arCurLength);
        PipeBarrier<PIPE_V>();
    } else {
        DataCopy(params.tempTensorA, src, tiling.arCurLength);
        PipeBarrier<PIPE_V>();
    }

    eventId = GetTPipePtr()->FetchEventID(HardEvent::S_V);
    for (uint32_t j = 0; j < tiling.aCurLength; j++) {
        float scalar = static_cast<float>(-1) * outputMean.GetValue(j);
        SetFlag<HardEvent::S_V>(eventId);
        WaitFlag<HardEvent::S_V>(eventId);
        Adds<float>(
            params.tempTensorA[j * para.rLengthWithPadding], params.tempTensorA[j * para.rLengthWithPadding], scalar,
            tiling.rLength);
    }
    PipeBarrier<PIPE_V>();

    Mul<float>(params.tempTensorA, params.tempTensorA, params.tempTensorA, tiling.arCurLength);
    PipeBarrier<PIPE_V>();

    Muls<float>(params.tempTensorA, params.tempTensorA, tiling.rValueBack, tiling.arCurLength);
    PipeBarrier<PIPE_V>();

    LayerNormReduceSumImpl(
        variance, params.tempTensorB, params.tempTensorA, tiling.aCurLength, para.rLengthWithPadding);
    PipeBarrier<PIPE_V>();
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void LayerNormCalMeanVarImpl(
    const LocalTensor<float>& OutputMean, const LocalTensor<float>& variance, const LocalTensor<T>& inputX,
    const LocalTensor<float>& stackBuffer, const LayerNormPara& para, LayerNormSeparateTiling& tiling)
{
    if (para.rLength != para.rLengthWithPadding) {
        LayerNormPreProc<T>(inputX, stackBuffer, para);
    }
    // allocate temp buffer
    LayerNormRstdTmpTensorParams<float> params;
    GetLayerNormRstdTensorInfo<isReuseSource>(stackBuffer, tiling, params);

    uint32_t inputOffset = 0;
    uint32_t mvOffset = 0;

    for (uint32_t index = 0; index < tiling.loopRound; index++) {
        ComputeMeanVariance<T>(OutputMean[mvOffset], variance[mvOffset], inputX[inputOffset], params, para, tiling);

        inputOffset += tiling.inputRoundSize;
        mvOffset += tiling.meanVarRoundSize;
    }
    if (tiling.inputTailSize > 0) {
        tiling.arCurLength = tiling.inputTailSize;
        tiling.aCurLength = tiling.meanVarTailSize;

        inputOffset = tiling.inputTailPos;
        mvOffset = tiling.meanVarTailPos;

        ComputeMeanVariance<T>(OutputMean[mvOffset], variance[mvOffset], inputX[inputOffset], params, para, tiling);
    }
}

template <typename U, typename T, bool isReuseSource = false, const LayerNormConfig& config = LNCFG_NORM>
__aicore__ inline void LayerNormImpl(
    const LocalTensor<T>& output, const LocalTensor<float>& outputMean, const LocalTensor<float>& outputRstd,
    const LocalTensor<T>& inputX, const LocalTensor<U>& gamma, const LocalTensor<U>& beta, const float epsilon,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const LayerNormPara& para, const LayerNormSeparateTiling& tiling)
{
    CHECK_FUNC_HIGHLEVEL_API(
        LayerNorm, (U, T, isReuseSource, config),
        (output, outputMean, outputRstd, inputX, gamma, beta, epsilon, sharedTmpBuffer, para, tiling));
    const LocalTensor<float> stackBuffer = sharedTmpBuffer.ReinterpretCast<float>();
    CheckLayerNormRstd<U, T, config>(stackBuffer, para);

    const LocalTensor<float> variance = stackBuffer[tiling.varianceTmpTensorPos];
    LayerNormSeparateTiling& Tiling = const_cast<LayerNormSeparateTiling&>(tiling);
    LayerNormCalMeanVarImpl<T, isReuseSource>(outputMean, variance, inputX, stackBuffer, para, Tiling);
    const LocalTensor<uint8_t> shareTmpBuffer = stackBuffer[tiling.firstTmpStartPos].ReinterpretCast<uint8_t>();

    NormalizePara normallizepara = {para.aLength, para.rLength, para.rLengthWithPadding};
    if (config.isNoBeta == false && config.isNoGamma == false) {
        Normalize<U, T, false, NLCFG_NORM>(
            output, outputRstd, outputMean, variance, inputX, gamma, beta, shareTmpBuffer, epsilon, normallizepara);
    } else if (config.isNoBeta == true && config.isNoGamma == false) {
        Normalize<U, T, false, NLCFG_NOBETA>(
            output, outputRstd, outputMean, variance, inputX, gamma, beta, shareTmpBuffer, epsilon, normallizepara);
    } else if (config.isNoBeta == false && config.isNoGamma == true) {
        Normalize<U, T, false, NLCFG_NOGAMMA>(
            output, outputRstd, outputMean, variance, inputX, gamma, beta, shareTmpBuffer, epsilon, normallizepara);
    } else {
        Normalize<U, T, false, NLCFG_NOOPT>(
            output, outputRstd, outputMean, variance, inputX, gamma, beta, shareTmpBuffer, epsilon, normallizepara);
    }
}

template <typename U, typename T, bool isReuseSource = false, const LayerNormConfig& config = LNCFG_NORM>
__aicore__ inline void LayerNormImpl(
    const LocalTensor<T>& output, const LocalTensor<float>& outputMean, const LocalTensor<float>& outputRstd,
    const LocalTensor<T>& inputX, const LocalTensor<U>& gamma, const LocalTensor<U>& beta, const float epsilon,
    const LayerNormPara& para, const LayerNormSeparateTiling& tiling)
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });

    LayerNormImpl<U, T, isReuseSource, config>(
        output, outputMean, outputRstd, inputX, gamma, beta, epsilon, sharedTmpBuffer, para, tiling);
}

template <bool isReuseSource = false>
__aicore__ inline void WelfordUpdateCompute(
    const LocalTensor<float>& outMean, const LocalTensor<float>& outVar, const LocalTensor<half>& src,
    const LocalTensor<float>& inMean, const LocalTensor<float>& inVar, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const WelfordUpdateParam& para, const uint32_t tmpNum, const UnaryRepeatParams unaryParams,
    const BinaryRepeatParams binaryParams)
{
    LocalTensor<float> srcVreg = sharedTmpBuffer.ReinterpretCast<float>();
    uint32_t tmpIndex = B32_DATA_NUM_PER_REPEAT * tmpNum;
    LocalTensor<float> tmpVreg = srcVreg[tmpIndex];
    LocalTensor<float> outVreg = srcVreg[tmpIndex + tmpIndex];

    PipeBarrier<PIPE_V>();
    Cast<float, half, false>(
        srcVreg, src, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        {1, 1, DEFAULT_REPEAT_STRIDE, HALF_DEFAULT_REPEAT_STRIDE});

    WelfordUpdateComputeMean(tmpVreg, srcVreg, inMean, outVreg, outMean, unaryParams, binaryParams, para);

    Sub<float, false>(outVreg, srcVreg, outMean, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
    Mul<float, false>(tmpVreg, tmpVreg, outVreg, MASK_PLACEHOLDER, 1, binaryParams);

    WelfordUpdateComputeVar(tmpVreg, inVar, outVar, unaryParams, binaryParams, para);
}

__aicore__ inline void WelfordUpdateComputeTo32Res(
    const LocalTensor<float>& outMean, const LocalTensor<float>& outVar, const LocalTensor<float>& src,
    const LocalTensor<float>& inMean, const LocalTensor<float>& inVar, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const WelfordUpdateParam& para, const uint32_t tmpNum, const UnaryRepeatParams unaryParams,
    const BinaryRepeatParams binaryParams)
{
    LocalTensor<float> tmpVreg = sharedTmpBuffer.ReinterpretCast<float>();

    WelfordUpdateComputeMean(tmpVreg, src, inMean, tmpVreg, outMean, unaryParams, binaryParams, para);

    Sub<float, false>(tmpVreg, src, outMean, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
    Sub<float, false>(src, src, inMean, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
    Mul<float, false>(tmpVreg, tmpVreg, src, MASK_PLACEHOLDER, 1, binaryParams);

    WelfordUpdateComputeVar(tmpVreg, inVar, outVar, unaryParams, binaryParams, para);
}

__aicore__ inline void WelfordUpdateComputeTo32(
    const LocalTensor<float>& outMean, const LocalTensor<float>& outVar, const LocalTensor<float>& src,
    const LocalTensor<float>& inMean, const LocalTensor<float>& inVar, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const WelfordUpdateParam& para, const uint32_t tmpNum, const UnaryRepeatParams unaryParams,
    const BinaryRepeatParams binaryParams)
{
    LocalTensor<float> tmpVreg = sharedTmpBuffer.ReinterpretCast<float>();
    LocalTensor<float> outVreg = tmpVreg[B32_DATA_NUM_PER_REPEAT * tmpNum];

    WelfordUpdateComputeMean(tmpVreg, src, inMean, outVreg, outMean, unaryParams, binaryParams, para);

    Sub<float, false>(outVreg, src, outMean, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
    Mul<float, false>(tmpVreg, tmpVreg, outVreg, MASK_PLACEHOLDER, 1, binaryParams);

    WelfordUpdateComputeVar(tmpVreg, inVar, outVar, unaryParams, binaryParams, para);
}

template <bool isReuseSource = false>
__aicore__ inline void WelfordUpdateCompute(
    const LocalTensor<float>& outMean, const LocalTensor<float>& outVar, const LocalTensor<float>& src,
    const LocalTensor<float>& inMean, const LocalTensor<float>& inVar, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const WelfordUpdateParam& para, const uint32_t tmpNum, const UnaryRepeatParams unaryParams,
    const BinaryRepeatParams binaryParams)
{
    if (isReuseSource) {
        WelfordUpdateComputeTo32Res(
            outMean, outVar, src, inMean, inVar, sharedTmpBuffer, para, tmpNum, unaryParams, binaryParams);
    } else {
        WelfordUpdateComputeTo32(
            outMean, outVar, src, inMean, inVar, sharedTmpBuffer, para, tmpNum, unaryParams, binaryParams);
    }
}

template <typename T, typename U, bool isReuseSource = false>
__aicore__ inline void WelfordUpdateComputeImpl(
    const LocalTensor<U>& outMean, const LocalTensor<U>& outVar, const LocalTensor<T>& src,
    const LocalTensor<U>& inMean, const LocalTensor<U>& inVar, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const WelfordUpdateParam& para)
{
    constexpr uint32_t tmpBufNum = WelfordUpdateGetTmpSize<T, U, isReuseSource>();

    uint32_t tmpNum = sharedTmpBuffer.GetSize() / (ONE_REPEAT_BYTE_SIZE * tmpBufNum);
#if ASCENDC_CPU_DEBUG
    ASCENDC_ASSERT((tmpNum != 0), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to check the size of sharedTmpBuffer, the size of sharedTmpBuffer is %dB, it is smaller.",
            sharedTmpBuffer.GetSize());
    });
#endif

    const uint32_t round = para.abComputeLength / (B32_DATA_NUM_PER_REPEAT * tmpNum);
    const uint32_t tail = para.abComputeLength % (B32_DATA_NUM_PER_REPEAT * tmpNum);

    SetVectorMask<float, MaskMode::COUNTER>(0, B32_DATA_NUM_PER_REPEAT * tmpNum);
    uint32_t offset = 0;

    const UnaryRepeatParams unaryParams;
    const BinaryRepeatParams binaryParams;

    for (uint32_t i = 0; i < round; ++i) {
        WelfordUpdateCompute<isReuseSource>(
            outMean[offset], outVar[offset], src[offset], inMean[offset], inVar[offset], sharedTmpBuffer, para, tmpNum,
            unaryParams, binaryParams);
        offset = offset + B32_DATA_NUM_PER_REPEAT * tmpNum;
    }

    if (tail != 0) {
        SetVectorMask<float, MaskMode::COUNTER>(0, tail);
        WelfordUpdateCompute<isReuseSource>(
            outMean[offset], outVar[offset], src[offset], inMean[offset], inVar[offset], sharedTmpBuffer, para, tmpNum,
            unaryParams, binaryParams);
    }
}

template <typename T, typename U, bool isReuseSource = false, const WelfordUpdateConfig& config = WFUPDATE_DEFAULT_CFG>
__aicore__ inline void WelfordUpdateImpl(
    const LocalTensor<U>& outputMean, const LocalTensor<U>& outputVariance, const LocalTensor<U>& inputMean,
    const LocalTensor<U>& inputVariance, const LocalTensor<T>& inputX, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const WelfordUpdateParam& para)
{
    CHECK_FUNC_HIGHLEVEL_API(
        WelfordUpdate, (T, U, isReuseSource, config),
        (outputMean, outputVariance, inputMean, inputVariance, inputX, sharedTmpBuffer, para));
    static_assert(
        (std::is_same<T, float>::value || std::is_same<T, half>::value),
        "Failed to check dtype of inputX, inputX support dtype is: half/float.");
    static_assert(
        (std::is_same<U, float>::value), "Failed to check dtype of mean/var, mean/var support dtype is: float.");
#if ASCENDC_CPU_DEBUG
    ASCENDC_ASSERT(
        ((TPosition)inputX.GetPosition() == TPosition::VECIN || (TPosition)inputX.GetPosition() == TPosition::VECOUT ||
         (TPosition)inputX.GetPosition() == TPosition::VECCALC),
        {
            KERNEL_LOG(
                KERNEL_ERROR, "Failed to check tensor position of input in WelfordUpdate, support positions are "
                              "UB(VECIN/VECOUT/VECCALC).");
        });
    ASCENDC_ASSERT((para.abLength <= inputX.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to check para.abLength, current size is %u, which should not larger than inputX size %u.",
            para.abLength, inputX.GetSize());
    });
    ASCENDC_ASSERT((para.abComputeLength <= para.abLength), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to check para.abComputeLength, current size is %u, which should not larger than abLength size %u.",
            para.abComputeLength, para.abLength);
    });
    ASCENDC_ASSERT((para.abComputeLength > 0), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check para.abComputeLength, para.abComputeLength (%u) should be greater than 0.",
            para.abComputeLength, para.abLength);
    });
    ASCENDC_ASSERT((para.rnLength == 1), {
        KERNEL_LOG(KERNEL_ERROR, "Failed to check para.rnLength, rnLength is %u, which should is 1.", para.rnLength);
    });
    ASCENDC_ASSERT((para.abLength % (ONE_BLK_SIZE / sizeof(T)) == 0), {
        KERNEL_LOG(KERNEL_ERROR, "Failed to check para.abLength, para.abLength should be 32B aligned.");
    });
#endif
    SetMaskCount();
    if (config.isInplace && (para.abComputeLength < para.abLength)) {
        WelfordUpdateInplace(outputMean, outputVariance, inputMean, inputVariance, para);
    }
    WelfordUpdateComputeImpl<T, U, isReuseSource>(
        outputMean, outputVariance, inputX, inputMean, inputVariance, sharedTmpBuffer, para);
    SetMaskNorm();
    ResetMask();
}

template <typename T, typename U, bool isReuseSource = false, const WelfordUpdateConfig& config = WFUPDATE_DEFAULT_CFG>
__aicore__ inline void WelfordUpdateImpl(
    const LocalTensor<U>& outMean, const LocalTensor<U>& outVar, const LocalTensor<U>& inMean,
    const LocalTensor<U>& inVar, const LocalTensor<T>& srcUb, const WelfordUpdateParam& para)
{
    LocalTensor<uint8_t> stackTensor;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackTensor);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });

    WelfordUpdateImpl<T, U, isReuseSource, config>(outMean, outVar, inMean, inVar, srcUb, stackTensor, para);
}

} // namespace AscendC
#endif // IMPL_NORMALIZATION_LAYERNORM_LAYERNORM_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORM_LAYERNORM_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORM_LAYERNORM_COMMON_IMPL_H__
#endif
