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
 * \file layernormgrad_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/normalization/layernormgrad/layernormgrad_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/normalization/layernorm.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORMGRAD_LAYERNORMGRAD_COMMON_IMPL_H__
#endif
#ifndef IMPL_NORMALIZATION_LAYERNORMGRAD_LAYERNORMGRAD_COMMON_IMPL_H
#define IMPL_NORMALIZATION_LAYERNORMGRAD_LAYERNORMGRAD_COMMON_IMPL_H

#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "kernel_tiling/kernel_tiling.h"
#include "../../../../../include/adv_api/normalization/layernormgrad_utils.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/normalization/layernormgrad/layernormgrad_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"
const uint32_t LAYERNORM_GRAD_B32_BYTE_SIZE = 4;
const uint32_t LAYERNORM_GRAD_B16_BYTE_SIZE = 2;

namespace AscendC {
struct LayerNormGradParams {
    __aicore__ LayerNormGradParams(LayerNormGradTiling& tiling, LocalTensor<float>& stackBuffer)
        : bLength(tiling.bLength),
          sLength(tiling.sLength),
          hLength(tiling.hLength),
          loopNum(tiling.loopNum),
          tailSize(tiling.tailSize),
          nohTailSize(tiling.nohTailSize),
          oneCalSize(tiling.oneCalSize),
          nohCalSize(tiling.nohCalSize),
          x1Tensor(stackBuffer[tiling.x1TensorPos]),
          x2Tensor(stackBuffer[tiling.x2TensorPos]),
          x3Tensor(stackBuffer[tiling.x3TensorPos]),
          pdVarTensor(stackBuffer[tiling.pdVarTensorPos]),
          pdMeanTensor(stackBuffer[tiling.pdMeanTensorPos]),
          tmpTensor(stackBuffer[tiling.tmpTensorPos]),
          tmpTensor1(stackBuffer[tiling.tmpTensor1Pos]),
          tmpTensor2(stackBuffer[tiling.tmpTensor2Pos]),
          tmpTensorBSH(stackBuffer[tiling.tmpTensorBSHPos]),
          lastDimValueBack(*(reinterpret_cast<float*>(&tiling.lastDimValueBack))),
          lastDimValueBackMulTwo(*(reinterpret_cast<float*>(&tiling.lastDimValueBackMulTwo)))
    {
        x1Tensor.SetSize(tiling.x1TensorSize);
        x2Tensor.SetSize(tiling.x2TensorSize);
        x3Tensor.SetSize(tiling.x3TensorSize);
        pdVarTensor.SetSize(tiling.pdVarTensorSize);
        pdMeanTensor.SetSize(tiling.pdMeanTensorSize);
        tmpTensor.SetSize(tiling.tmpTensorSize);
        tmpTensor1.SetSize(tiling.tmpTensor1Size);
        tmpTensor2.SetSize(tiling.tmpTensor2Size);
        tmpTensorBSH.SetSize(tiling.tmpTensorBSHSize);
    }

    __aicore__ LayerNormGradParams(uint32_t b, uint32_t s, uint32_t h)
    {
        bLength = b;
        sLength = s;
        hLength = h;
    }

    uint32_t bLength;
    uint32_t sLength;
    uint32_t hLength;

    uint32_t loopNum;
    uint32_t tailSize;
    uint32_t nohTailSize;
    uint32_t oneCalSize;
    uint32_t nohCalSize;

    float lastDimValueBack;
    float lastDimValueBackMulTwo;

    LocalTensor<float> x1Tensor;     // for inputDy * inputGamma
    LocalTensor<float> x2Tensor;     // for inputX - inputMean
    LocalTensor<float> x3Tensor;     // for x1Tensor * np.power((inputVariance + EPSILON), (-0.5))
    LocalTensor<float> pdVarTensor;  // for pdVar
    LocalTensor<float> pdMeanTensor; // for pdMean
    LocalTensor<float> tmpTensor;    // tmp for intermediate use
    LocalTensor<float> tmpTensor1;   // tmp for intermediate use
    LocalTensor<float> tmpTensor2;   // tmp for intermediate use
    LocalTensor<float> tmpTensorBSH; // tmp for intermediate use
};

__aicore__ inline void DuplicateLastDimImpl(
    const LocalTensor<float>& dst, const LocalTensor<float>& src, const uint32_t bsLength, const uint32_t hLength)
{
    auto eventIdVToS = GetTPipePtr()->FetchEventID(HardEvent::V_S);
    SetFlag<HardEvent::V_S>(eventIdVToS);
    WaitFlag<HardEvent::V_S>(eventIdVToS);

    float scalarList[BRCB_BROADCAST_NUMBER] = {0};
    const uint32_t rangeM = bsLength / BRCB_BROADCAST_NUMBER;
    const uint32_t tailM = bsLength % BRCB_BROADCAST_NUMBER;

    for (uint32_t i = 0; i < rangeM; i++) {
        for (uint32_t j = 0; j < BRCB_BROADCAST_NUMBER; j++) {
            scalarList[j] = src[i * BRCB_BROADCAST_NUMBER + j].GetValue(0);
        }
        for (uint32_t j = 0; j < BRCB_BROADCAST_NUMBER; j++) {
            Duplicate(dst[(i * BRCB_BROADCAST_NUMBER + j) * hLength], scalarList[j], hLength);
        }
    }
    if (tailM != 0) {
        for (uint32_t j = 0; j < tailM; j++) {
            scalarList[j] = src[rangeM * BRCB_BROADCAST_NUMBER + j].GetValue(0);
        }
        for (uint32_t j = 0; j < tailM; j++) {
            Duplicate(dst[(rangeM * BRCB_BROADCAST_NUMBER + j) * hLength], scalarList[j], hLength);
        }
    }
}

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510)
__aicore__ inline void BrcbLastDimImpl(
    const LocalTensor<float>& dst, const LocalTensor<float>& src, const uint32_t bsLength, const uint32_t hLength)
{
    const uint32_t maxRepeatHSize = BRCB_MAX_REPEAT_SIZE * hLength;

    const uint32_t lineRound = hLength / BRCB_BROADCAST_NUMBER;

    const uint32_t rowRound = bsLength / BRCB_BROADCAST_NUMBER;
    const uint32_t rowTail = bsLength % BRCB_BROADCAST_NUMBER;
    const uint32_t rowRoundLen = bsLength - rowTail;

    const uint32_t repeatTimes = rowRound / MAX_REPEAT_TIMES;
    const uint32_t tailTimes = rowRound % MAX_REPEAT_TIMES;

    BrcbRepeatParams repeatParams;
    repeatParams.dstBlkStride = lineRound;
    repeatParams.dstRepStride = hLength;

    for (uint32_t i = 0; i < lineRound; i++) {
        for (uint32_t j = 0; j < repeatTimes; j++) {
            Brcb(
                dst[i * BRCB_BROADCAST_NUMBER + j * maxRepeatHSize], src[j * BRCB_MAX_REPEAT_SIZE], MAX_REPEAT_TIMES,
                repeatParams);
        }

        if (tailTimes > 0) {
            Brcb(
                dst[i * BRCB_BROADCAST_NUMBER + repeatTimes * maxRepeatHSize], src[repeatTimes * BRCB_MAX_REPEAT_SIZE],
                tailTimes, repeatParams);
        }
        PipeBarrier<PIPE_V>();
    }

    if (rowTail != 0) {
        DuplicateLastDimImpl(dst[rowRoundLen * hLength], src[rowRoundLen], rowTail, hLength);
        PipeBarrier<PIPE_V>();
    }
}
#endif

__aicore__ inline void BroadcastLastDimImpl(
    const LocalTensor<float>& dst, const LocalTensor<float>& src, const uint32_t dstSize, const uint32_t srcSize)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 3510)
    BrcbLastDimImpl(dst, src, srcSize, dstSize / srcSize);
#else
    DuplicateLastDimImpl(dst, src, srcSize, dstSize / srcSize);
#endif
}

__aicore__ inline void ReduceSumImpl(
    const LocalTensor<float>& dst, const LocalTensor<float>& src, const uint32_t calSize, const uint32_t hLength)
{
    ASCENDC_ASSERT((hLength != 0), { KERNEL_LOG(KERNEL_ERROR, "error shape for layernormgrad."); });
    const uint32_t count = calSize / hLength;

    for (uint32_t i = 0; i < count; i++) {
        uint32_t totalNum = hLength;
        uint32_t iMulhLength = i * hLength;

        LocalTensor<float> srctmp = src;
        LocalTensor<float> dstTmp = dst;

        while (totalNum > 1) {
            uint32_t repeatTimes = totalNum / ONE_REPEAT_FLOAT_SIZE;
            uint32_t tailSize = totalNum % ONE_REPEAT_FLOAT_SIZE;

            uint32_t blockNum = repeatTimes / MAX_REPEAT_TIMES;
            uint32_t blockTail = repeatTimes % MAX_REPEAT_TIMES;

            for (uint32_t j = 0; j < blockNum; j++) {
                WholeReduceSum(
                    dst[iMulhLength + j * MAX_REPEAT_TIMES], srctmp[iMulhLength + j * MAX_REPEAT_FLOAT_SIZE],
                    ONE_REPEAT_FLOAT_SIZE, MAX_REPEAT_TIMES, 1, 1, DEFAULT_REPEAT_STRIDE);
            }
            PipeBarrier<PIPE_V>();

            if (totalNum == ONE_REPEAT_FLOAT_SIZE) {
                dstTmp = dst[i];
            } else {
                dstTmp = dst[iMulhLength + blockNum * MAX_REPEAT_TIMES];
            }

            if (blockTail > 0) {
                WholeReduceSum(
                    dstTmp, srctmp[iMulhLength + blockNum * MAX_REPEAT_FLOAT_SIZE], ONE_REPEAT_FLOAT_SIZE, blockTail,
                    DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
                PipeBarrier<PIPE_V>();
            }

            if (totalNum < ONE_REPEAT_FLOAT_SIZE) {
                dstTmp = dst[i];
            } else {
                dstTmp = dst[iMulhLength + totalNum / ONE_REPEAT_FLOAT_SIZE];
            }

            if (tailSize > 0) {
                WholeReduceSum(
                    dstTmp, srctmp[iMulhLength + repeatTimes * ONE_REPEAT_FLOAT_SIZE], tailSize, DEFAULT_BLK_STRIDE,
                    DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
                PipeBarrier<PIPE_V>();
            }

            totalNum = DivCeil(totalNum, ONE_REPEAT_FLOAT_SIZE);
            srctmp = dst;
        }
    }
}

template <typename T>
__aicore__ inline void DuplicateTensor(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count, const uint32_t length)
{
    BroadcastLastDimImpl(dst, src, count * length, count);
    PipeBarrier<PIPE_V>();
}

__aicore__ inline void ComputePdX1(
    const LocalTensor<float>& inputDy, const LocalTensor<float>& inputGamma, LayerNormGradParams& param,
    const uint32_t nohSize, const uint32_t hLength)
{
    // x1Tensor = inputDy * inputGamma
    for (size_t i = 0; i < nohSize; ++i) {
        Mul(param.x1Tensor[i * hLength], inputDy[i * hLength], inputGamma, hLength);
    }
    PipeBarrier<PIPE_V>();
}

template <typename T>
__aicore__ inline void ComputePdX2(
    const LocalTensor<T>& inputX, const LocalTensor<T>& inputMean, const LayerNormGradParams& param,
    const uint32_t calSize, const uint32_t nohSize, const uint32_t hLength)
{
    // duplicate inputMean
    DuplicateTensor(param.tmpTensorBSH, inputMean, nohSize, hLength);
    PipeBarrier<PIPE_V>();
    // x1Tensor = x2Tensor = inputX - inputMean
    Sub(param.x2Tensor, inputX, param.tmpTensorBSH, calSize);
    PipeBarrier<PIPE_V>();
}

__aicore__ inline void DoOneDiv(
    LocalTensor<float>& dstTensor, LocalTensor<float>& oneTensor, LocalTensor<float>& src1Tensor,
    const uint32_t nohSize)
{
    SetMaskCount();
    SetVectorMask<uint8_t, MaskMode::COUNTER>(0, B32_DATA_NUM_PER_BLOCK);
    Duplicate<float, false>(oneTensor, 1, MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE, DEFAULT_REPEAT_STRIDE);
    PipeBarrier<PIPE_V>();
    SetVectorMask<uint8_t, MaskMode::COUNTER>(0, nohSize);
    Div<float, false>(
        dstTensor, oneTensor, src1Tensor, MASK_PLACEHOLDER, 1,
        {1, 0, 1, DEFAULT_REPEAT_STRIDE, 0, DEFAULT_REPEAT_STRIDE});
    PipeBarrier<PIPE_V>();
    SetMaskNorm();
}

__aicore__ inline void ComputePdVar(
    const LocalTensor<float>& inputVariance, float epsilon, LayerNormGradParams& param, const uint32_t calSize,
    const uint32_t nohSize)
{
    const float multiplier1 = -1.5;
    const float multiplier2 = -0.5;
    // 1. inputVariance + EPSILON)
    Adds(param.tmpTensor, inputVariance, epsilon, nohSize);
    PipeBarrier<PIPE_V>();
    // 2. res = np.power((inputVariance + EPSILON), (-1.5))
    Mul(param.tmpTensorBSH, param.tmpTensor, param.tmpTensor, nohSize);
    PipeBarrier<PIPE_V>();
    Mul(param.tmpTensor, param.tmpTensorBSH, param.tmpTensor, nohSize);
    PipeBarrier<PIPE_V>();
    Sqrt(param.tmpTensor, param.tmpTensor, nohSize);
    PipeBarrier<PIPE_V>();

    DoOneDiv(param.tmpTensor, param.tmpTensorBSH, param.tmpTensor, nohSize);

    // 3. duplicate
    DuplicateTensor(param.tmpTensorBSH, param.tmpTensor, nohSize, param.hLength);
    PipeBarrier<PIPE_V>();

    // 4. res1 = (-0.5) * x1Tensor * (x2Tensor) * res
    Mul(param.tmpTensorBSH, param.x2Tensor, param.tmpTensorBSH, calSize);
    PipeBarrier<PIPE_V>();
    Mul(param.tmpTensorBSH, param.x1Tensor, param.tmpTensorBSH, calSize);
    PipeBarrier<PIPE_V>();
    Muls(param.tmpTensorBSH, param.tmpTensorBSH, static_cast<float>(multiplier2), calSize);
    PipeBarrier<PIPE_V>();

    // 5. pd_var = np.sum(res1)

    ReduceSumImpl(param.pdVarTensor, param.tmpTensorBSH, calSize, param.hLength);
    PipeBarrier<PIPE_V>();
}

__aicore__ inline void ComputePdMean(
    const LocalTensor<float>& inputVariance, const LocalTensor<float>& resForGamma, float epsilon,
    LayerNormGradParams& param, const uint32_t calSize, const uint32_t nohSize)
{
    constexpr float exponent = -0.5;
    constexpr float multiplier = -1.0;
    constexpr float multiplier2 = -2.0;
    // 1. inputVariance + EPSILON)
    Adds(param.tmpTensor, inputVariance, epsilon, nohSize);
    PipeBarrier<PIPE_V>();

    // 2. res = np.power((inputVariance + EPSILON), (-0.5))
    Sqrt(param.tmpTensor, param.tmpTensor, nohSize);
    PipeBarrier<PIPE_V>();

    DoOneDiv(param.tmpTensor, param.tmpTensorBSH, param.tmpTensor, nohSize);

    // 3. duplicate
    DuplicateTensor(param.tmpTensorBSH, param.tmpTensor, nohSize, param.hLength);
    PipeBarrier<PIPE_V>();

    // res_for_gamma = x2Tensor * res
    Mul(resForGamma, param.x2Tensor, param.tmpTensorBSH, calSize);
    PipeBarrier<PIPE_V>();

    // 4. res1 = (-1.0) * x1Tensor * res
    Mul(param.x3Tensor, param.x1Tensor, param.tmpTensorBSH, calSize);
    PipeBarrier<PIPE_V>();
    Muls(param.tmpTensorBSH, param.x3Tensor, static_cast<float>(multiplier), calSize);
    PipeBarrier<PIPE_V>();

    // 5. pd_mean = np.sum(res1)
    ReduceSumImpl(param.pdMeanTensor, param.tmpTensorBSH, calSize, param.hLength);

    // 6. res2 = np.sum(((-2.0) * (x2Tensor)))
    Muls(param.tmpTensorBSH, param.x2Tensor, static_cast<float>(multiplier2), calSize);
    PipeBarrier<PIPE_V>();

    ReduceSumImpl(param.tmpTensor, param.tmpTensorBSH, calSize, param.hLength);
    PipeBarrier<PIPE_V>();

    // 7. res3 = pd_var * (1.0/m) * res2
    Muls(param.tmpTensor, param.tmpTensor, static_cast<float>(param.lastDimValueBack), nohSize);
    PipeBarrier<PIPE_V>();
    Mul(param.tmpTensor, param.pdVarTensor, param.tmpTensor, nohSize);
    PipeBarrier<PIPE_V>();

    // 8. pd_mean = pd_mean + res3
    Add(param.pdMeanTensor, param.pdMeanTensor, param.tmpTensor, nohSize);
    PipeBarrier<PIPE_V>();
}

__aicore__ inline void ComputePdX(
    const LocalTensor<float>& inputVariance, const LocalTensor<float>& outputPdX, float epsilon,
    const LayerNormGradParams& param, const uint32_t calSize, const uint32_t nohSize)
{
    // 1. res0 = x1Tensor * np.power((inputVariance + EPSILON), (-0.5)), already store in resForGamma
    // 2. res1 = pd_var*(2.0 / H)*(x2Tensor)
    Muls(param.pdVarTensor, param.pdVarTensor, static_cast<float>(param.lastDimValueBackMulTwo), nohSize);
    PipeBarrier<PIPE_V>();

    DuplicateTensor(param.tmpTensorBSH, param.pdVarTensor, nohSize, param.hLength);
    PipeBarrier<PIPE_V>();

    Mul(param.x1Tensor, param.tmpTensorBSH, param.x2Tensor, calSize);
    PipeBarrier<PIPE_V>();

    // 3. res2 = pd_mean*(1.0 / H)
    Muls(param.pdMeanTensor, param.pdMeanTensor, static_cast<float>(param.lastDimValueBack), nohSize);
    PipeBarrier<PIPE_V>();
    DuplicateTensor(param.tmpTensorBSH, param.pdMeanTensor, nohSize, param.hLength);

    // 4. pd_x = res0 + res1 + res2
    Add(param.x1Tensor, param.x1Tensor, param.tmpTensorBSH, calSize);
    PipeBarrier<PIPE_V>();

    Add(outputPdX, param.x1Tensor, param.x3Tensor, calSize);
    PipeBarrier<PIPE_V>();
}

__aicore__ inline void GetTmpTensor(
    const LocalTensor<float>& outputPdX, const LocalTensor<float>& inputDy, const LocalTensor<float>& inputX,
    LayerNormGradParams& param, bool isReuseSource = false)
{
    param.tmpTensor = outputPdX;
    if (isReuseSource == true) {
        param.x1Tensor = inputDy;
        param.x2Tensor = inputX;
    }
}

template <typename T>
__aicore__ inline void ComputeProcess(
    const LocalTensor<T>& inputDy, const LocalTensor<T>& inputX, const LocalTensor<T>& inputVariance,
    const LocalTensor<T>& inputMean, const LocalTensor<T>& inputGamma, const LocalTensor<T>& outputPdX,
    const LocalTensor<T>& resForGamma, T epsilon, LayerNormGradParams& param, const uint32_t calSize,
    const uint32_t nohSize, bool isReuseSource)
{}

template <>
__aicore__ inline void ComputeProcess<half>(
    const LocalTensor<half>& inputDy, const LocalTensor<half>& inputX, const LocalTensor<half>& inputVariance,
    const LocalTensor<half>& inputMean, const LocalTensor<half>& inputGamma, const LocalTensor<half>& outputPdX,
    const LocalTensor<half>& resForGamma, half epsilon, LayerNormGradParams& param, const uint32_t calSize,
    const uint32_t nohSize, bool isReuseSource)
{
    Cast(param.tmpTensor1, inputDy, RoundMode::CAST_NONE, calSize);
    Cast(param.tmpTensor2, inputGamma, RoundMode::CAST_NONE, param.hLength);
    PipeBarrier<PIPE_V>();
    // 1. x1Tensor = inputDy * inputGamma
    ComputePdX1(param.tmpTensor1, param.tmpTensor2, param, nohSize, param.hLength);
    // 2. x2Tensor = inputX - inputMean
    Cast(param.tmpTensor1, inputX, RoundMode::CAST_NONE, calSize);
    Cast(param.tmpTensor2, inputMean, RoundMode::CAST_NONE, nohSize);
    PipeBarrier<PIPE_V>();
    ComputePdX2(param.tmpTensor1, param.tmpTensor2, param, calSize, nohSize, param.hLength);
    // 3. pd_var = np.sum(((-0.5) * x1Tensor * x2Tensor * np.power((inputVariance + EPSILON), (-1.5))))
    Cast(param.tmpTensor1, inputVariance, RoundMode::CAST_NONE, nohSize);
    PipeBarrier<PIPE_V>();
    ComputePdVar(param.tmpTensor1, epsilon, param, calSize, nohSize);
    // 4. pd_mean = np.sum(((-1.0) * x1Tensor * np.power((inputVariance + EPSILON), (-0.5)))) +
    //              pd_var * (1.0 / H) * np.sum(((-2.0) * (x2Tensor)))
    ComputePdMean(param.tmpTensor1, param.tmpTensor2, epsilon, param, calSize, nohSize);
    // 5. pd_x = x1Tensor * np.power((inputVariance + EPSILON), (-0.5)) +
    //           pd_var*(2.0 / H)*(x2Tensor) +
    //           pd_mean*(1.0 / H)
    ComputePdX(param.tmpTensor1, param.tmpTensor, epsilon, param, calSize, nohSize);
    // 6. transdata from float to half for output
    Cast(outputPdX, param.tmpTensor, RoundMode::CAST_NONE, calSize);
    Cast(resForGamma, param.tmpTensor2, RoundMode::CAST_NONE, calSize);
    PipeBarrier<PIPE_V>();
}

template <>
__aicore__ inline void ComputeProcess<float>(
    const LocalTensor<float>& inputDy, const LocalTensor<float>& inputX, const LocalTensor<float>& inputVariance,
    const LocalTensor<float>& inputMean, const LocalTensor<float>& inputGamma, const LocalTensor<float>& outputPdX,
    const LocalTensor<float>& resForGamma, float epsilon, LayerNormGradParams& param, const uint32_t calSize,
    const uint32_t nohSize, bool isReuseSource)
{
    GetTmpTensor(outputPdX, inputDy, inputX, param, isReuseSource);
    // 1. x1Tensor = inputDy * inputGamma
    ComputePdX1(inputDy, inputGamma, param, nohSize, param.hLength);
    // 2. x2Tensor = inputX - inputMean
    ComputePdX2(inputX, inputMean, param, calSize, nohSize, param.hLength);
    // 3. pd_var = np.sum(((-0.5) * x1Tensor * x2Tensor * np.power((inputVariance + EPSILON), (-1.5))))
    ComputePdVar(inputVariance, epsilon, param, calSize, nohSize);
    //         4. pd_mean = np.sum(((-1.0) * x1Tensor * np.power((inputVariance + EPSILON), (-0.5)))) +
    //                      pd_var * (1.0 / H) * np.sum(((-2.0) * (x2Tensor)))
    ComputePdMean(inputVariance, resForGamma, epsilon, param, calSize, nohSize);
    PipeBarrier<PIPE_V>();
    // 5. pd_x = x1Tensor * np.power((inputVariance + EPSILON), (-0.5)) +
    //           pd_var*(2.0 / H)*(x2Tensor) +
    //           pd_mean*(1.0 / H)
    ComputePdX(inputVariance, outputPdX, epsilon, param, calSize, nohSize);
}

template <typename T>
__aicore__ inline void LayerNormGradComputeND(
    const LocalTensor<T>& inputDy, const LocalTensor<T>& inputX, const LocalTensor<T>& inputVariance,
    const LocalTensor<T>& inputMean, const LocalTensor<T>& inputGamma, const LocalTensor<T>& outputPdX,
    const LocalTensor<T>& resForGamma, T epsilon, LayerNormGradParams& param, bool isReuseSource)
{
    int offset0 = 0; // offset for shape [B, S, H]
    int offset1 = 0; // offset for shape [B, S, 1]

    for (size_t i = 0; i < param.loopNum; ++i) {
        ComputeProcess<T>(
            inputDy[offset0], inputX[offset0], inputVariance[offset1], inputMean[offset1], inputGamma,
            outputPdX[offset0], resForGamma[offset0], epsilon, param, param.oneCalSize, param.nohCalSize,
            isReuseSource);
        offset0 += param.oneCalSize;
        offset1 += param.nohCalSize;
    }

    if (param.tailSize != 0) {
        ComputeProcess<T>(
            inputDy[offset0], inputX[offset0], inputVariance[offset1], inputMean[offset1], inputGamma,
            outputPdX[offset0], resForGamma[offset0], epsilon, param, param.tailSize, param.nohTailSize, isReuseSource);
    }
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void LayerNormGradImpl(
    const LocalTensor<T>& outputPdX, const LocalTensor<T>& resForGamma, const LocalTensor<T>& inputDy,
    const LocalTensor<T>& inputX, const LocalTensor<T>& inputVariance, const LocalTensor<T>& inputMean,
    const LocalTensor<T>& inputGamma, LocalTensor<uint8_t>& sharedTmpBuffer, T epsilon, LayerNormGradTiling& tiling,
    const LayerNormGradShapeInfo& shapeInfo = {})
{
    TRACE_START(TraceId::LayerNormGrad);
    CHECK_FUNC_HIGHLEVEL_API(
        LayerNormGrad, (T, isReuseSource),
        (outputPdX, resForGamma, inputDy, inputX, inputVariance, inputMean, inputGamma, sharedTmpBuffer, epsilon,
         tiling, shapeInfo));

    ASCENDC_ASSERT((sharedTmpBuffer.GetSize() >= tiling.stackBufferSize), {
        KERNEL_LOG(KERNEL_ERROR, "Stack buffer size not enough.");
    });

    LocalTensor<float> stackBuffer = sharedTmpBuffer.ReinterpretCast<float>();
    LayerNormGradParams param(tiling, stackBuffer);

    if (shapeInfo.dataFormat == DataFormat::ND) {
        LayerNormGradComputeND(
            inputDy, inputX, inputVariance, inputMean, inputGamma, outputPdX, resForGamma, epsilon, param,
            isReuseSource);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "Only support format ND now!"); });
    }
    TRACE_STOP(TraceId::LayerNormGrad);
}

template <typename T, bool isReuseSource = false>
__aicore__ inline void LayerNormGradImpl(
    const LocalTensor<T>& outputPdX, const LocalTensor<T>& resForGamma, const LocalTensor<T>& inputDy,
    const LocalTensor<T>& inputX, const LocalTensor<T>& inputVariance, const LocalTensor<T>& inputMean,
    const LocalTensor<T>& inputGamma, T epsilon, LayerNormGradTiling& tiling,
    const LayerNormGradShapeInfo& shapeInfo = {})
{
    LocalTensor<uint8_t> stackBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });

    LayerNormGradImpl<T, isReuseSource>(
        outputPdX, resForGamma, inputDy, inputX, inputVariance, inputMean, inputGamma, stackBuffer, epsilon, tiling,
        shapeInfo);
}
} // namespace AscendC
#endif // IMPL_NORMALIZATION_LAYERNORMGRAD_LAYERNORMGRAD_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORMGRAD_LAYERNORMGRAD_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_NORMALIZATION_LAYERNORMGRAD_LAYERNORMGRAD_COMMON_IMPL_H__
#endif
