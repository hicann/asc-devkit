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
 * \file ascend_dequant_common_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/quantization/dequant/ascend_dequant_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/quantization/ascend_dequant.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_DEQUANT_ASCEND_DEQUANT_COMMON_IMPL_H__
#endif

#ifndef IMPL_QUANTIZATION_DEQUANT_ASCEND_DEQUANT_COMMON_IMPL_H
#define IMPL_QUANTIZATION_DEQUANT_ASCEND_DEQUANT_COMMON_IMPL_H

#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "ascend_dequant_common.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/quantization/dequant/dequant_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102)
#include "ascend_dequant_3510_impl.h"
#endif

namespace AscendC {
constexpr uint32_t FLOAT_PER_BLOCK = 8;   // 32B  = FP32(4B) * 8
constexpr uint32_t FLOAT_PER_REPEAT = 64; // 256B = FP32(4B) * 64

__aicore__ inline bool IsCalCountValid(const LocalTensor<int32_t>& srcTensor, uint32_t calCount)
{
    // calCount must be in range (0, srcTensor element num]
    ASCENDC_ASSERT((calCount > 0 && calCount <= srcTensor.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "calCount is %u, which should be in range (0, srcTensor.GetSize() %u]", calCount,
            srcTensor.GetSize());
    });
    return true;
}

// Dequant tensor function without DequantParams check
template <typename scaleT>
__aicore__ inline bool IsWithoutDequantParamsValid(
    const LocalTensor<int32_t>& srcTensor, const LocalTensor<scaleT>& deqScale)
{
    // when without dequantParams, must srcTensor.GetSize() % deqScale.GetSize() == 0
    ASCENDC_ASSERT((srcTensor.GetSize() % deqScale.GetSize() == 0), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "when Dequant function does not have DequantParams, srcTensor.GetSize() %u should be divisible by \
        deqScale.GetSize() %u",
            srcTensor.GetSize(), deqScale.GetSize());
    });
    // when scaleT is uint64_t, deqScale.GetSize() must be divisible by 8 for calCount calculation
    if constexpr (IsSameType<scaleT, uint64_t>::value) {
        ASCENDC_ASSERT((deqScale.GetSize() % 8 == 0), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "when Dequant function does not have DequantParams and scaleT is uint64_t, deqScale.GetSize() %u should \
            be divisible by 8",
                deqScale.GetSize());
        });
    }
    return true;
}

// DequantParams check
template <typename dstT>
__aicore__ inline bool IsDequantParamsValid(
    const LocalTensor<int32_t>& srcTensor, const LocalTensor<dstT>& dstTensor, DequantParams& params)
{
    // params.n must be 32B aligned as FP32
    ASCENDC_ASSERT(
        params.n % FLOAT_PER_BLOCK == 0, { KERNEL_LOG(KERNEL_ERROR, "params.n %u must be divisible by 8", params.n); });
    // params.m * params.n <= srcTensor element num
    ASCENDC_ASSERT(params.m * params.n <= srcTensor.GetSize(), {
        KERNEL_LOG(
            KERNEL_ERROR, "params.m %u * params.n %u \
        must not be larger than element num of srcTensor %u",
            params.m, params.n, srcTensor.GetSize());
    });
    // params.calCount must be in range (0, params.n]
    ASCENDC_ASSERT((params.calCount > 0 && params.calCount <= params.n), {
        KERNEL_LOG(
            KERNEL_ERROR, "params.calCount is %u, which should be in range (0, params.n %u]", params.calCount,
            params.n);
    });
    // params.m * (params.n after align with dstT) <= dstTensor elementNum
    uint32_t oneBlockNum = ONE_BLK_SIZE / sizeof(dstT);
    uint32_t alignInner = (params.n + oneBlockNum - 1) / oneBlockNum * oneBlockNum;
    ASCENDC_ASSERT((params.m * alignInner <= dstTensor.GetSize()), {
        KERNEL_LOG(KERNEL_ERROR, "dstTensor element num should be not less than %u", params.m * alignInner);
    });

    return true;
}

// tensor deqScale check
template <typename scaleT>
__aicore__ inline bool IsDeqscaleTensorValid(const LocalTensor<scaleT>& deqScale, DequantParams& params)
{
    // params.calCount must <= deqScale element num
    ASCENDC_ASSERT((params.calCount <= deqScale.GetSize()), {
        KERNEL_LOG(
            KERNEL_ERROR, "params.calCount %u must not be \
        larger than deqScale element num %u",
            params.calCount, deqScale.GetSize());
    });
    return true;
}

// tensor dtype combination check
template <typename dstT, typename scaleT, bool isTensor>
__aicore__ inline constexpr bool IsTemplateValid()
{
    if constexpr (isTensor) {
        // dtype only support deqScale dtype: uint64_t, float, bfloat16_t,  bfloat16_t
        //                    dst      dtype: half    , float, float,       bfloat16_t
        constexpr bool isValid1 = (IsSameType<scaleT, uint64_t>::value) && (IsSameType<dstT, half>::value);
        constexpr bool isValid2 = (IsSameType<scaleT, float>::value) && (IsSameType<dstT, float>::value);
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
        return isValid1 || isValid2;
#else
        constexpr bool isValid3 = (IsSameType<scaleT, bfloat16_t>::value) && (IsSameType<dstT, float>::value);
        constexpr bool isValid4 = (IsSameType<scaleT, bfloat16_t>::value) && (IsSameType<dstT, bfloat16_t>::value);
        constexpr bool isValid5 = (IsSameType<scaleT, float>::value) && (IsSameType<dstT, bfloat16_t>::value);
        return isValid1 || isValid2 || isValid3 || isValid4 || isValid5;
#endif
    } else {
        // dtype only support deqScale dtype: bfloat16_t, bfloat16_t, float
        //                    dst      dtype: bfloat16_t, float,      float
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
        constexpr bool isValid1 = (IsSameType<scaleT, float>::value) && (IsSameType<dstT, float>::value);
        return isValid1;
#else
        constexpr bool isValid1 = (IsSameType<scaleT, bfloat16_t>::value) && (IsSameType<dstT, bfloat16_t>::value);
        constexpr bool isValid2 = (IsSameType<scaleT, bfloat16_t>::value) && (IsSameType<dstT, float>::value);
        constexpr bool isValid3 = (IsSameType<scaleT, float>::value) && (IsSameType<dstT, float>::value);
        constexpr bool isValid4 = (IsSameType<scaleT, float>::value) && (IsSameType<dstT, bfloat16_t>::value);
        return isValid1 || isValid2 || isValid3 || isValid4;
#endif
    }
}

// calculate tmpBuffer address
template <typename scaleT>
__aicore__ inline void AscendDequantTmpCalc(
    const LocalTensor<float>& stackBuffer, DequantParams& dqParams, AscendDequantParams<float>& params,
    uint32_t srcSize, uint32_t deqScaleSize)
{
    uint32_t base = dqParams.n; // expect tmp to be k * dqParams.n (k >= 1), therefore base is dqParams.n

    deqScaleSize = (deqScaleSize + FLOAT_PER_BLOCK - 1) / FLOAT_PER_BLOCK * FLOAT_PER_BLOCK; // guarantee 32B aligned
    // one 256B is reserved for safety (avoid writing extra space in normal mode)
    uint32_t tmpSrcSize = (stackBuffer.GetSize() - deqScaleSize) / base * base;
    ASCENDC_ASSERT((tmpSrcSize > 0), { KERNEL_LOG(KERNEL_ERROR, "stackBuffer size is not large enough"); });
    tmpSrcSize = (tmpSrcSize > srcSize) ? srcSize : tmpSrcSize; // max tmpSize is srcTensor size
    params.tmpSize = tmpSrcSize;                                // FP32 element num of src tmpBuffer
    params.tmpAddrA = stackBuffer;                              // FP32 deqScale
    params.tmpAddrB = stackBuffer[deqScaleSize];                // FP32 src
}

// calculate tmpBuffer address + cast to FP32 deqScale tensor
template <typename scaleT>
__aicore__ inline void AscendDequantTmpCalc(
    const LocalTensor<int32_t>& srcTensor, const scaleT deqScale, const LocalTensor<float>& stackBuffer,
    DequantParams& dqParams, AscendDequantParams<float>& params)
{
    uint32_t srcSize = dqParams.m * dqParams.n;
    uint32_t deqScaleSize = (dqParams.calCount + FLOAT_PER_BLOCK - 1) / FLOAT_PER_BLOCK * FLOAT_PER_BLOCK;

    AscendDequantTmpCalc<scaleT>(stackBuffer, dqParams, params, srcSize, deqScaleSize);

    if constexpr (IsSameType<scaleT, float>::value) {
        Duplicate<float>(params.tmpAddrA, deqScale, static_cast<int32_t>(dqParams.calCount));
    } else { // bfloat16_t
        Duplicate<float>(params.tmpAddrA, ToFloat(deqScale), static_cast<int32_t>(dqParams.calCount));
    }
    PipeBarrier<PIPE_V>();
}

// float -> BF16: CAST_RINT    float -> half: CAST_NONE
template <typename dstT>
__aicore__ inline RoundMode GetFP32CastMode()
{
#if defined(__NPU_ARCH__) && __NPU_ARCH__ == 2002
    return RoundMode::CAST_NONE;
#else
    constexpr RoundMode castMode = IsSameType<dstT, bfloat16_t>::value ? RoundMode::CAST_RINT : RoundMode::CAST_NONE;
    return castMode;
#endif
}

// Update dqParams if format is {1, m*n, n}
template <typename dstT, DeQuantMode mode>
__aicore__ inline void UpdateDequantParams(DequantParams& params)
{
    if constexpr (mode == DeQuantMode::DEQUANT_WITH_SINGLE_ROW) {
        constexpr uint32_t ONE_BLK_SIZE = 32;
        uint32_t oneBlockNum = ONE_BLK_SIZE / sizeof(dstT);
        bool isCalCountAlign = (params.calCount % oneBlockNum == 0);
        bool isNDivisible = (params.n % params.calCount == 0); // params.n = k * params.calcount
        // if params.m = 1 and params.n = m * n, params.calcount = n  => convert to {m, n, n}
        // Ex: src int32_t {1, 16} +  dst BF16 + dqParams {1, 16, 8} => is still {1, 16, 8} because not isCalCountAlign
        //     src int32_t {1, 16} +  dst FP32 + dqParams {1, 16, 8} => change to {2, 8, 8} because is isCalCountAlign
        if (params.m == 1 && isCalCountAlign && isNDivisible) {
            params.m = params.n / params.calCount;
            params.n = params.calCount;
        }
    }
}

// deqScale cast: scaleT(BF16 / FP32 / UINT64_T) -> FP32
template <typename scaleT>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendDequant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void CastDeqscale(
    const LocalTensor<scaleT>& deqScale, AscendDequantParams<float>& params, uint32_t scaleSize)
{
    UnaryRepeatParams unaryParams;
    unaryParams.srcRepStride = IsSameType<scaleT, float>::value ? DEFAULT_REPEAT_STRIDE : HALF_DEFAULT_REPEAT_STRIDE;

    if constexpr (IsSameType<scaleT, float>::value) { // FP32 -> FP32
        SetVectorMask<float, MaskMode::COUNTER>(0, scaleSize);
        Adds<float, false>(params.tmpAddrA, deqScale, 0, MASK_PLACEHOLDER, 1, unaryParams);
        PipeBarrier<PIPE_V>();
    } else if constexpr (IsSameType<scaleT, uint64_t>::value) { // uint64_t -> FP32
        // uint64 = 32 + 32 => pick lower 32 value as FP32
        LocalTensor<float> deqScaleFP32 = deqScale.template ReinterpretCast<float>();

        // GatherMask only supports norm mode, 256 / sizeof(datatype) num per repeat
        GatherMaskParams reducev2Params;
        reducev2Params.repeatTimes = 1;
        uint64_t rsvdCnt = 0;
        GatherMask<float>(params.tmpAddrA, deqScaleFP32, 1, true, scaleSize * 2, reducev2Params, rsvdCnt);
        PipeBarrier<PIPE_V>();
        SetMaskCount();
    } else { // BF16 -> FP32
        SetVectorMask<float, MaskMode::COUNTER>(0, scaleSize);
        Cast<float, scaleT, false>(params.tmpAddrA, deqScale, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1, unaryParams);
        PipeBarrier<PIPE_V>();
    }
}

// srcTensor cast: int32_t -> FP32
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendDequant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void CastSrc(
    const LocalTensor<int32_t>& srcTensor, const LocalTensor<float>& dstTensor, UnaryRepeatParams& unaryParams,
    uint64_t counter)
{
    SetVectorMask<float, MaskMode::COUNTER>(0, counter);
    Cast<float, int32_t, false>(dstTensor, srcTensor, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();
}

// k means tmpSpace size is k * dqParams.n
// src FP32 * deqScale FP32  calculate k * dqParams.n (k >= 1) data to mul with deqScale
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendDequant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void DequantMul(
    const LocalTensor<float>& srcTensor, const LocalTensor<float>& deqScaleTensor, const LocalTensor<float>& dstTensor,
    BinaryRepeatParams& binaryParams, DequantParams& dqParams, uint32_t k, uint32_t loopCount, uint32_t tail)
{
    if (k == 0) {
        return;
    }

    // restriction of uint8_t repStride: dqParams.n / 8
    if (dqParams.n > MAX_REPEAT_TIMES * FLOAT_PER_BLOCK) {
        BinaryRepeatParams binaryParamsDefault;
        SetVectorMask<float, MaskMode::COUNTER>(0, dqParams.calCount);
        for (uint32_t i = 0; i < k; i++) {
            Mul<float, false>(
                dstTensor[i * dqParams.n], srcTensor[i * dqParams.n], deqScaleTensor, MASK_PLACEHOLDER, 1,
                binaryParamsDefault);
        }
        PipeBarrier<PIPE_V>();
        return;
    }

    SetVectorMask<float, MaskMode::COUNTER>(0, FLOAT_PER_REPEAT * k);
    for (uint32_t i = 0; i < loopCount; i++) {
        Mul<float, false>(
            dstTensor[i * FLOAT_PER_REPEAT], srcTensor[i * FLOAT_PER_REPEAT], deqScaleTensor[i * FLOAT_PER_REPEAT],
            MASK_PLACEHOLDER, 1, binaryParams);
    }
    PipeBarrier<PIPE_V>();

    if (tail != 0) {
        SetMaskNorm();
        // use norm mode, uint8_t repeatTimes can be at most MAX_REPEAT_TIMES 255
        uint32_t kTimes = k / MAX_REPEAT_TIMES;
        uint32_t kRemains = k % MAX_REPEAT_TIMES;
        SetVectorMask<float, MaskMode::NORMAL>(0, ((uint64_t)1 << tail) - 1);

        uint32_t baseIndex = loopCount * FLOAT_PER_REPEAT;
        for (uint32_t i = 0; i < kTimes; i++) {
            uint32_t index = baseIndex + MAX_REPEAT_TIMES * i * dqParams.n;
            Mul<float, false>(
                dstTensor[index], srcTensor[index], deqScaleTensor[baseIndex], MASK_PLACEHOLDER, MAX_REPEAT_TIMES,
                binaryParams);
            PipeBarrier<PIPE_V>();
        }
        if (kRemains > 0) {
            uint32_t index = baseIndex + MAX_REPEAT_TIMES * kTimes * dqParams.n;
            Mul<float, false>(
                dstTensor[index], srcTensor[index], deqScaleTensor[baseIndex], MASK_PLACEHOLDER, kRemains,
                binaryParams);
            PipeBarrier<PIPE_V>();
        }
    }
}

// result cast: FP32 -> dstT
template <typename dstT>
__aicore__ inline void CastDst(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<float>& srcFP32, UnaryRepeatParams& unaryParams,
    uint32_t srcInner, uint32_t dstInner, uint32_t dataNum)
{
    if constexpr (IsSameType<dstT, float>::value) {
        SetVectorMask<float, MaskMode::COUNTER>(0, dataNum);
        Adds<float, false>(dstTensor, srcFP32, 0, MASK_PLACEHOLDER, 1, unaryParams);
        PipeBarrier<PIPE_V>();
        return;
    }

    RoundMode castMode = GetFP32CastMode<dstT>();
    if (srcInner == dstInner) { // no need for padding
        SetVectorMask<float, MaskMode::COUNTER>(0, dataNum);
        Cast<dstT, float, false>(dstTensor, srcFP32, castMode, MASK_PLACEHOLDER, 1, unaryParams);
        PipeBarrier<PIPE_V>();
    } else {
        uint32_t loopNum = dataNum / srcInner;
        uint32_t tailPart = dataNum % srcInner;
        SetVectorMask<float, MaskMode::COUNTER>(0, srcInner);
        for (uint32_t i = 0; i < loopNum; i++) {
            Cast<dstT, float, false>(
                dstTensor[i * dstInner], srcFP32[i * srcInner], castMode, MASK_PLACEHOLDER, 1, unaryParams);
        }
        PipeBarrier<PIPE_V>();

        if (tailPart > 0) {
            SetVectorMask<float, MaskMode::COUNTER>(0, tailPart);
            Cast<dstT, float, false>(
                dstTensor[loopNum * dstInner], srcFP32[loopNum * srcInner], castMode, MASK_PLACEHOLDER, 1, unaryParams);
            PipeBarrier<PIPE_V>();
        }
    }
}

// each time calculate k * dqParams.n (k >= 1) data
template <typename dstT, typename scaleT, bool isPureDqParams = false>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendDequant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void CalculateByInner(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const LocalTensor<scaleT>& deqScale,
    DequantParams& dqParams, AscendDequantParams<float>& ascendDqParams, uint32_t calCount)
{
    LocalTensor<float> deqScaleFP32 = ascendDqParams.tmpAddrA;
    LocalTensor<float> srcFP32 = ascendDqParams.tmpAddrB;

    uint32_t oneBlockNum = ONE_BLK_SIZE / sizeof(dstT);
    uint32_t dstInner = (dqParams.n + oneBlockNum - 1) / oneBlockNum * oneBlockNum; // inner after pad for B16 case
    uint32_t tmpSize = ascendDqParams.tmpSize;                                      // FP32 num in one tmp buffer
    uint32_t loopCount = calCount / tmpSize;                                        // calCount / (k * inner)
    uint32_t tailSize = calCount % tmpSize;
    uint32_t k = tmpSize / dqParams.n; // tmpSize = (k * inner)

    uint32_t mainBlockLoopCount = dqParams.calCount / FLOAT_PER_REPEAT;
    uint32_t mainBlockTail = dqParams.calCount % FLOAT_PER_REPEAT;
    BinaryRepeatParams binaryParams;
    BinaryRepeatParams binaryParamsMul(1, 1, 1, dqParams.n / FLOAT_PER_BLOCK, dqParams.n / FLOAT_PER_BLOCK, 0);
    UnaryRepeatParams unaryParams;
    UnaryRepeatParams unaryParamsDst;
    if constexpr (!IsSameType<dstT, float>::value) {
        unaryParamsDst.dstRepStride = HALF_DEFAULT_REPEAT_STRIDE;
    }

    CastDeqscale(deqScale, ascendDqParams, dqParams.calCount);

    // When switch lines to cast must consider dstT     Ex: src: int32_t 8, dst: bf16 16
    uint32_t castDstIndex = (dqParams.n == dstInner) ? tmpSize : k * dstInner;
    for (uint32_t i = 0; i < loopCount; i++) { // each loop calculate k * dqParams.n (k >=1) data
        SetMaskCount();
        CastSrc(srcTensor[i * tmpSize], srcFP32, unaryParams, tmpSize);

        DequantMul(srcFP32, deqScaleFP32, srcFP32, binaryParamsMul, dqParams, k, mainBlockLoopCount, mainBlockTail);

        SetMaskCount();
        CastDst<dstT>(dstTensor[i * castDstIndex], srcFP32, unaryParamsDst, dqParams.n, dstInner, tmpSize);
    }

    // tailBlock:  Ex:  tmpSize is 5 * n, tailSize is 2 * n + 3
    if (tailSize > 0) {
        CastSrc(srcTensor[calCount - tailSize], srcFP32, unaryParams, tailSize);

        k = tailSize / dqParams.n; // tailSize = k * dqParams.n + b (b < dqParams.n)
        DequantMul(srcFP32, deqScaleFP32, srcFP32, binaryParamsMul, dqParams, k, mainBlockLoopCount, mainBlockTail);

        if constexpr (!isPureDqParams) {
            uint32_t tailK = tailSize % dqParams.n;
            if (tailK != 0) { // b in tailSize
                SetMaskCount();
                SetVectorMask<float, MaskMode::COUNTER>(0, tailK);
                uint32_t idxMul = tailSize - tailK;
                Mul<float, false>(srcFP32[idxMul], srcFP32[idxMul], deqScaleFP32, MASK_PLACEHOLDER, 1, binaryParams);
                PipeBarrier<PIPE_V>();
            }
        }

        SetMaskCount();
        // Note: need to update index based on dstInner     Ex: src: int32_t outer * 8, dst: bf16 outer * 16
        uint32_t index = (dqParams.n == dstInner) ? calCount - tailSize : (calCount - tailSize) / dqParams.n * dstInner;
        CastDst<dstT>(dstTensor[index], srcFP32, unaryParamsDst, dqParams.n, dstInner, tailSize);
    }
}

// calculate tmpAddr + for loop execution
template <typename dstT, typename scaleT, bool isPureDqParams, DeQuantMode mode>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendDequant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AscendDequantImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const LocalTensor<scaleT>& deqScale,
    const LocalTensor<uint8_t>& sharedTmpBuffer, DequantParams& params, uint32_t calCount)
{
    CHECK_FUNC_HIGHLEVEL_API(
        AscendDequant, (dstT, scaleT, isPureDqParams, mode),
        (dstTensor, srcTensor, deqScale, sharedTmpBuffer, params, calCount));
    if ASCEND_IS_AIC {
        return;
    }

    static_assert(
        IsTemplateValid<dstT, scaleT, true>(),
        "current combination of deqScale dtype and dstTensor dtype is not supported, please check the document");
    UpdateDequantParams<dstT, mode>(params);

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102)
    DequantPerchannelImpl<dstT, scaleT, mode>(dstTensor, srcTensor, deqScale, params);
    return;
#endif

    LocalTensor<float> stackBuffer = sharedTmpBuffer.ReinterpretCast<float>();

    AscendDequantParams<float> ascendDqParams;
    AscendDequantTmpCalc<scaleT>(stackBuffer, params, ascendDqParams, params.m * params.n, params.calCount);

    SetMaskCount();
    CalculateByInner<dstT, scaleT, isPureDqParams>(dstTensor, srcTensor, deqScale, params, ascendDqParams, calCount);

    SetMaskNorm();
    ResetMask();
}

template <typename dstT, typename scaleT, DeQuantMode mode>
__aicore__ inline void AscendDequantImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const LocalTensor<scaleT>& deqScale,
    DequantParams params)
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    AscendDequantImpl<dstT, scaleT, true, mode>(
        dstTensor, srcTensor, deqScale, sharedTmpBuffer, params, params.m * params.n);
}

template <typename dstT, typename scaleT, DeQuantMode mode>
__aicore__ inline void AscendDequantCalcountImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const LocalTensor<scaleT>& deqScale,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{
    if (!IsCalCountValid(srcTensor, calCount) || !IsWithoutDequantParamsValid<scaleT>(srcTensor, deqScale)) {
        return;
    }
    DequantParams params = {srcTensor.GetSize() / deqScale.GetSize(), deqScale.GetSize(), deqScale.GetSize()};
    AscendDequantImpl<dstT, scaleT, false, mode>(dstTensor, srcTensor, deqScale, sharedTmpBuffer, params, calCount);
}

template <typename dstT, typename scaleT, DeQuantMode mode>
__aicore__ inline void AscendDequantCalcountImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const LocalTensor<scaleT>& deqScale,
    const uint32_t calCount)
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    AscendDequantCalcountImpl<dstT, scaleT, mode>(dstTensor, srcTensor, deqScale, sharedTmpBuffer, calCount);
}

template <typename dstT, typename scaleT, DeQuantMode mode>
__aicore__ inline void AscendDequantNoCalcountImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const LocalTensor<scaleT>& deqScale,
    const LocalTensor<uint8_t>& sharedTmpBuffer)
{
    if (!IsWithoutDequantParamsValid<scaleT>(srcTensor, deqScale)) {
        return;
    }
    DequantParams params = {srcTensor.GetSize() / deqScale.GetSize(), deqScale.GetSize(), deqScale.GetSize()};
    AscendDequantImpl<dstT, scaleT, false, mode>(
        dstTensor, srcTensor, deqScale, sharedTmpBuffer, params, srcTensor.GetSize());
}

template <typename dstT, typename scaleT, DeQuantMode mode>
__aicore__ inline void AscendDequantNoCalcountImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const LocalTensor<scaleT>& deqScale)
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    AscendDequantNoCalcountImpl<dstT, scaleT, mode>(dstTensor, srcTensor, deqScale, sharedTmpBuffer);
}

template <typename dstT, typename scaleT, bool isPureDqParams, DeQuantMode mode>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendDequant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AscendDequantScalarImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const scaleT deqScale,
    const LocalTensor<uint8_t>& sharedTmpBuffer, DequantParams& params)
{
    CHECK_FUNC_HIGHLEVEL_API(
        AscendDequant, (dstT, scaleT, isPureDqParams, mode), (dstTensor, srcTensor, deqScale, sharedTmpBuffer, params));

    static_assert(
        IsTemplateValid<dstT, scaleT, false>(),
        "current combination of deqScale dtype and dstTensor dtype is not supported, please check the document");

    UpdateDequantParams<dstT, mode>(params);

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102)
    DequantPertensorImpl<dstT, scaleT, mode>(dstTensor, srcTensor, deqScale, params);
    return;
#endif

    LocalTensor<float> stackBuffer = sharedTmpBuffer.ReinterpretCast<float>();

    SetMaskCount();
    AscendDequantParams<float> ascendDqParams;
    AscendDequantTmpCalc<scaleT>(srcTensor, deqScale, stackBuffer, params, ascendDqParams);
    LocalTensor<float> deqScaleFP32 = ascendDqParams.tmpAddrA;

    SetMaskCount();
    CalculateByInner<dstT, float, true>(
        dstTensor, srcTensor, deqScaleFP32, params, ascendDqParams, params.m * params.n);

    SetMaskNorm();
    ResetMask();
}

template <typename dstT, typename scaleT, DeQuantMode mode>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendDequant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AscendDequantScalarImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<int32_t>& srcTensor, const scaleT deqScale,
    DequantParams& params)
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    AscendDequantScalarImpl<dstT, scaleT, true, mode>(dstTensor, srcTensor, deqScale, sharedTmpBuffer, params);
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102)
template <
    typename dstT, typename srcT, typename scaleT, const AscendDeQuantConfig& config, const AscendDeQuantPolicy& policy>
__aicore__ inline void AscendDequantImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendDeQuantParam& para)
{
    LocalTensor<uint8_t> stackTensor;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackTensor);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    AscendDequantImpl<dstT, srcT, scaleT, config, policy>(
        dstTensor, srcTensor, stackTensor, scaleTensor, offsetTensor, para);
}
#endif
} // namespace AscendC
#endif // IMPL_QUANTIZATION_DEQUANT_ASCEND_DEQUANT_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_DEQUANT_ASCEND_DEQUANT_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_DEQUANT_ASCEND_DEQUANT_COMMON_IMPL_H__
#endif
