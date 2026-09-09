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
 * \file ascend_antiquant_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/quantization/antiquant/ascend_antiquant_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/quantization/ascend_antiquant.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_ANTIQUANT_ASCEND_ANTIQUANT_IMPL_H__
#endif

#ifndef IMPL_QUANTIZATION_ANTIQUANT_ASCEND_ANTIQUANT_IMPL_H
#define IMPL_QUANTIZATION_ANTIQUANT_ASCEND_ANTIQUANT_IMPL_H

#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "ascend_antiquant_common.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/quantization/antiquant/antiquant_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102)
#include "ascend_antiquant_3510_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
#include "ascend_antiquant_c220_impl.h"
#elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "ascend_antiquant_l300_impl.h"
#else
#include "ascend_antiquant_m200_impl.h"
#endif

namespace AscendC {
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AntiQuantInnerLoopF16(
    const LocalTensor<half>& dst, const LocalTensor<half>& src, const LocalTensor<half>& offset,
    const LocalTensor<half>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const BinaryRepeatParams& binaryParams,
    const uint32_t calCount)
{
    SetVectorMask<half, MaskMode::COUNTER>(0, calCount);
    Add<half, false>(dst, offset, src, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
    Mul<half, false>(dst, scale, dst, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
}

template <typename SrcType, bool withOffset = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AntiQuantInnerLoop(
    const LocalTensor<half>& dst, const LocalTensor<SrcType>& src, const LocalTensor<half>& offset,
    const LocalTensor<half>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer,
    const UnaryRepeatParams& unaryParamsCastSrc, const BinaryRepeatParams& binaryParams, const uint32_t calCount)
{
    SetVectorMask<half, MaskMode::COUNTER>(0, calCount);
    Cast<half, SrcType, false>(dst, src, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1, unaryParamsCastSrc);
    PipeBarrier<PIPE_V>();
    if constexpr (withOffset) {
        Add<half, false>(dst, offset, dst, MASK_PLACEHOLDER, 1, binaryParams);
        PipeBarrier<PIPE_V>();
    }
    Mul<half, false>(dst, scale, dst, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
}

template <typename SrcType, bool withOffset = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AntiQuantInnerLoop(
    const LocalTensor<half>& dst, const LocalTensor<SrcType>& src, const half offset, const half scale,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const UnaryRepeatParams& unaryParamsCastSrc,
    const UnaryRepeatParams& unaryParamsScalar, const uint32_t calCount)
{
    SetVectorMask<half, MaskMode::COUNTER>(0, calCount);
    Cast<half, SrcType, false>(dst, src, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1, unaryParamsCastSrc);
    PipeBarrier<PIPE_V>();
    if constexpr (withOffset) {
        Adds<half, false>(dst, dst, offset, MASK_PLACEHOLDER, 1, unaryParamsScalar);
        PipeBarrier<PIPE_V>();
    }
    Muls<half, false>(dst, dst, scale, MASK_PLACEHOLDER, 1, unaryParamsScalar);
    PipeBarrier<PIPE_V>();
}

template <typename SrcType, typename DstType, bool withOffset = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AntiQuantOuterLoop(
    const LocalTensor<DstType>& dst, const LocalTensor<SrcType>& src, const LocalTensor<DstType>& offset,
    const LocalTensor<DstType>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{
    UnaryRepeatParams unaryParamsCastSrc; // srcType -> FP16
    if constexpr (IsSameType<SrcType, int8_t>::value) {
        unaryParamsCastSrc.srcRepStride = HALF_DEFAULT_REPEAT_STRIDE;
    } else {
        unaryParamsCastSrc.srcRepStride = ONE_FOURTH_DEFAULT_REPEAT_STRIDE;
    }
    BinaryRepeatParams binaryParams; // used for add and mul
    if constexpr (IsSameType<DstType, half>::value) {
        AntiQuantInnerLoop<SrcType, withOffset>(
            dst, src, offset, scale, sharedTmpBuffer, unaryParamsCastSrc, binaryParams, calCount);
    } else { // dstType = bfloat16_t
        uint32_t tmpSize = sharedTmpBuffer.GetSize() / sizeof(DstType) / ANTIQUANT_FOUR;
        uint32_t loopCount = calCount / tmpSize;
        uint32_t tailSize = calCount % tmpSize;

        UnaryRepeatParams unaryParamsFP32ToDst; // FP32 -> BF16
        unaryParamsFP32ToDst.dstRepStride = HALF_DEFAULT_REPEAT_STRIDE;
        UnaryRepeatParams unaryParamsToFP32; // FP16 -> FP32
        unaryParamsToFP32.srcRepStride = HALF_DEFAULT_REPEAT_STRIDE;

        for (uint32_t i = 0; i < loopCount; i++) {
            AntiQuantInnerLoop<SrcType, withOffset>(
                dst[i * tmpSize], src[i * tmpSize], offset, scale, sharedTmpBuffer, unaryParamsCastSrc,
                unaryParamsToFP32, unaryParamsFP32ToDst, binaryParams, tmpSize);
        }
        if (tailSize > 0) {
            AntiQuantInnerLoop<SrcType, withOffset>(
                dst[loopCount * tmpSize], src[loopCount * tmpSize], offset, scale, sharedTmpBuffer, unaryParamsCastSrc,
                unaryParamsToFP32, unaryParamsFP32ToDst, binaryParams, tailSize);
        }
    }
}

template <typename SrcType, typename DstType, bool withOffset = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AntiQuantOuterLoop(
    const LocalTensor<DstType>& dst, const LocalTensor<SrcType>& src, const DstType offset, const DstType scale,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
{
    UnaryRepeatParams unaryParamsCastSrc;
    if constexpr (IsSameType<SrcType, int8_t>::value) {
        unaryParamsCastSrc.srcRepStride = HALF_DEFAULT_REPEAT_STRIDE;
    } else {
        unaryParamsCastSrc.srcRepStride = ONE_FOURTH_DEFAULT_REPEAT_STRIDE;
    }
    UnaryRepeatParams unaryParamsScalar;
    if constexpr (IsSameType<DstType, half>::value) {
        AntiQuantInnerLoop<SrcType, withOffset>(
            dst, src, offset, scale, sharedTmpBuffer, unaryParamsCastSrc, unaryParamsScalar, calCount);
    } else {
        uint32_t tmpSize = sharedTmpBuffer.GetSize() / sizeof(DstType) / ANTIQUANT_FOUR;
        uint32_t loopCount = calCount / tmpSize;
        uint32_t tailSize = calCount % tmpSize;

        UnaryRepeatParams unaryParamsToFP32;
        unaryParamsToFP32.srcRepStride = HALF_DEFAULT_REPEAT_STRIDE;
        UnaryRepeatParams unaryParamsFP32ToDst;
        unaryParamsFP32ToDst.dstRepStride = HALF_DEFAULT_REPEAT_STRIDE;

        for (uint32_t i = 0; i < loopCount; i++) {
            AntiQuantInnerLoop<SrcType, withOffset>(
                dst[i * tmpSize], src[i * tmpSize], offset, scale, sharedTmpBuffer, unaryParamsCastSrc,
                unaryParamsToFP32, unaryParamsFP32ToDst, unaryParamsScalar, tmpSize);
        }
        if (tailSize > 0) {
            AntiQuantInnerLoop<SrcType, withOffset>(
                dst[loopCount * tmpSize], src[loopCount * tmpSize], offset, scale, sharedTmpBuffer, unaryParamsCastSrc,
                unaryParamsToFP32, unaryParamsFP32ToDst, unaryParamsScalar, tailSize);
        }
    }
}

template <typename SrcType>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AscendAntiQuantNoTransposePerformance(
    const LocalTensor<half>& dst, const LocalTensor<SrcType>& src, const LocalTensor<half>& offset,
    const LocalTensor<half>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K, const uint32_t N)
{
    BinaryRepeatParams binaryParams;
    binaryParams.src0RepStride = N * sizeof(half) / ONE_BLK_SIZE;
    binaryParams.src1RepStride = 0;
    binaryParams.dstRepStride = N * sizeof(half) / ONE_BLK_SIZE;
    uint32_t repeatEle = ONE_REPEAT_BYTE_SIZE;
    uint32_t repeatTimes = src.GetSize() % repeatEle == 0 ? src.GetSize() / repeatEle : src.GetSize() / repeatEle + 1;

    SetMaskCount();
    SetVectorMask<half, MaskMode::COUNTER>(0, ANTIQUANT_SINGLE_N_SIZE_FP16 * K);
    uint32_t loopN = N / ANTIQUANT_SINGLE_N_SIZE_FP16;
    for (uint32_t i = 0; i < loopN; i++) {
        uint32_t loopOffset = ANTIQUANT_SINGLE_N_SIZE_FP16 * i;
        // 2.add offset
        Add<half, false>(dst[loopOffset], dst[loopOffset], offset[loopOffset], MASK_PLACEHOLDER, K, binaryParams);
        PipeBarrier<PIPE_V>();
        // 3.mul scale
        Mul<half, false>(dst[loopOffset], dst[loopOffset], scale[loopOffset], MASK_PLACEHOLDER, K, binaryParams);
        PipeBarrier<PIPE_V>();
    }
    SetMaskNorm();
    ResetMask();
}

template <typename SrcType>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AscendAntiQuantNoTransposePerformanceTail(
    const LocalTensor<half>& dst, const LocalTensor<SrcType>& src, const LocalTensor<half>& offset,
    const LocalTensor<half>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K, const uint32_t N,
    const uint32_t mask)
{
    BinaryRepeatParams binaryParams;
    binaryParams.src0RepStride = N * sizeof(half) / ONE_BLK_SIZE;
    binaryParams.src1RepStride = 0;
    binaryParams.dstRepStride = N * sizeof(half) / ONE_BLK_SIZE;

    // 1.cast src to dst
    SetMaskNorm();
    SetVectorMask<half, MaskMode::NORMAL>(mask);
    // 2.add offset
    Add<half, false>(dst, dst, offset, MASK_PLACEHOLDER, K, binaryParams);
    PipeBarrier<PIPE_V>();
    // 3.mul scale
    Mul<half, false>(dst, dst, scale, MASK_PLACEHOLDER, K, binaryParams);
    PipeBarrier<PIPE_V>();
    ResetMask();
}

template <typename SrcType>
__aicore__ inline void PreCast(
    const LocalTensor<half>& dst, const LocalTensor<SrcType>& src, const LocalTensor<half>& offset,
    const LocalTensor<half>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K)
{
    UnaryRepeatParams s42f16unaryParams;
    s42f16unaryParams.srcRepStride = ONE_FOURTH_DEFAULT_REPEAT_STRIDE;
    UnaryRepeatParams unaryParams;
    unaryParams.srcRepStride = HALF_DEFAULT_REPEAT_STRIDE;
    uint32_t repeatEle = ONE_REPEAT_BYTE_SIZE;
    uint32_t repeatTimes = src.GetSize() % repeatEle == 0 ? src.GetSize() / repeatEle : src.GetSize() / repeatEle + 1;

    SetMaskCount();
    SetVectorMask<half, MaskMode::COUNTER>(0, src.GetSize());
    if constexpr (IsSameType<SrcType, int4b_t>::value) {
        Cast<half, int4b_t, false>(dst, src, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1, s42f16unaryParams);
    } else {
        Cast<half, int8_t, false>(dst, src, RoundMode::CAST_NONE, MASK_PLACEHOLDER, repeatTimes, unaryParams);
    }
    PipeBarrier<PIPE_V>();
}

template <typename SrcType, typename DstType>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AntiQuantNoTransposeImplScalar(
    const LocalTensor<DstType>& dst, const LocalTensor<SrcType>& src, const LocalTensor<DstType>& offset,
    const LocalTensor<DstType>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount,
    const uint32_t K, const uint32_t N, const AntiQuantShapeInfo& shapeInfo)
{
    uint32_t groupCount = (shapeInfo.scaleHeight == 0 ? scale.GetShapeInfo().shape[0] : shapeInfo.scaleHeight);
    uint32_t groupSize = K / groupCount;
    SetMaskCount();
    if constexpr (IsSameType<DstType, half>::value && IsSameType<SrcType, int8_t>::value) {
        SetVectorMask<half, MaskMode::COUNTER>(0, calCount);
        UnaryRepeatParams unaryParams;
        unaryParams.srcRepStride = HALF_DEFAULT_REPEAT_STRIDE;
        BinaryRepeatParams binaryParams;

        Cast<half, int8_t, false>(dst, src, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1, unaryParams);
        PipeBarrier<PIPE_V>();
        for (uint32_t j = 0; j < groupSize; j++) {
            AntiQuantInnerLoopF16(dst[j * N], dst[j * N], offset, scale, sharedTmpBuffer, binaryParams, N);
        }
        return;
    }
    for (uint32_t j = 0; j < groupSize; j++) {
        AntiQuantOuterLoop<SrcType, DstType, true>(dst[j * N], src[j * N], offset, scale, sharedTmpBuffer, N);
    }
}

template <typename SrcType, typename DstType>
__aicore__ inline void AscendAntiQuantNoTranspose(
    const LocalTensor<DstType>& dst, const LocalTensor<SrcType>& src, const LocalTensor<DstType>& offset,
    const LocalTensor<DstType>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount,
    const uint32_t K, const AntiQuantShapeInfo& shapeInfo)
{
    uint32_t N = src.GetSize() / K;
    bool isPerformance = AntiQuantCheckPerformanceMode(scale, sharedTmpBuffer, K);
    if (isPerformance) {
        // cast offset and scale to tmpbuffer
        PreCast(dst, src, offset, scale, sharedTmpBuffer, K);
        uint32_t kTail = K % ANTIQUANT_MAX_K, loopK = K / ANTIQUANT_MAX_K, nTail;
        if constexpr (IsSameType<DstType, half>::value) {
            nTail = N % ANTIQUANT_SINGLE_N_SIZE_FP16;
        } else {
            nTail = N % ANTIQUANT_SINGLE_N_SIZE_BF16;
        }
        uint32_t NAlign = N - nTail;
        for (int i = 0; i < K / ANTIQUANT_MAX_K; i++) {
            uint32_t offsetSrc = i * ANTIQUANT_MAX_K * N;
            AscendAntiQuantNoTransposePerformance(
                dst[offsetSrc], src[offsetSrc], offset, scale, sharedTmpBuffer, ANTIQUANT_MAX_K, N);
            if (nTail > 0) {
                AscendAntiQuantNoTransposePerformanceTail(
                    dst[offsetSrc + NAlign], src[offsetSrc + NAlign], offset[NAlign], scale[NAlign], sharedTmpBuffer,
                    ANTIQUANT_MAX_K, N, nTail);
            }
        }
        if (kTail > 0) {
            uint32_t offsetSrc = K / ANTIQUANT_MAX_K * ANTIQUANT_MAX_K * N;
            AscendAntiQuantNoTransposePerformance(
                dst[offsetSrc], src[offsetSrc], offset, scale, sharedTmpBuffer, kTail, N);
            if (nTail > 0) {
                AscendAntiQuantNoTransposePerformanceTail(
                    dst[offsetSrc + NAlign], src[offsetSrc + NAlign], offset[NAlign], scale[NAlign], sharedTmpBuffer,
                    kTail, N, nTail);
            }
        }
        return;
    }
    AntiQuantNoTransposeImplScalar(dst, src, offset, scale, sharedTmpBuffer, calCount, K, N, shapeInfo);
}

template <typename SrcType, typename DstType>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AscendAntiQuantNoTranspose(
    const LocalTensor<DstType>& dst, const LocalTensor<SrcType>& src, const LocalTensor<DstType>& scale,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount, const uint32_t K,
    const AntiQuantShapeInfo& shapeInfo)
{
    uint32_t groupCount = (shapeInfo.scaleHeight == 0 ? scale.GetShapeInfo().shape[0] : shapeInfo.scaleHeight);
    uint32_t groupSize = K / groupCount;
    uint32_t N = (shapeInfo.scaleWidth == 0 ? scale.GetShapeInfo().shape[1] : shapeInfo.scaleWidth);

    SetMaskCount();
    for (uint32_t i = 0; i < groupCount; i++) {
        for (uint32_t j = 0; j < groupSize; j++) {
            // withOffset is false, use scale as offset
            AntiQuantOuterLoop<SrcType, DstType, false>(
                dst[(i * groupSize + j) * N], src[(i * groupSize + j) * N], scale, scale[i * N], sharedTmpBuffer, N);
        }
    }
    SetMaskNorm();
    ResetMask();
}

template <typename SrcType, typename DstType, bool withOffset = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AscendAntiQuantNoTranspose(
    const LocalTensor<DstType>& dst, const LocalTensor<SrcType>& src, const DstType offset, const DstType scale,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount, const uint32_t K,
    const AntiQuantShapeInfo& shapeInfo)
{
    SetMaskCount();
    AntiQuantOuterLoop<SrcType, DstType, withOffset>(dst, src, offset, scale, sharedTmpBuffer, calCount);
}

template <typename SrcType, typename DstType>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AntiQuantImplScalar(
    const LocalTensor<DstType>& dst, const LocalTensor<SrcType>& src, const LocalTensor<DstType>& offset,
    const LocalTensor<DstType>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount,
    const uint32_t K, const AntiQuantShapeInfo& shapeInfo)
{
    uint32_t N = src.GetSize() / K;
    uint32_t groupSize = K / (shapeInfo.offsetWidth == 0 ? offset.GetShapeInfo().shape[1] : shapeInfo.offsetWidth);
    uint32_t offsetLength = K / groupSize;
    SetMaskCount();
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < offsetLength; j++) {
            auto offsetValue = offset.GetValue(i * offsetLength + j);
            auto scaleValue = scale.GetValue(i * offsetLength + j);
            AntiQuantOuterLoop<SrcType, DstType, true>(
                dst[i * K + j * groupSize], src[i * K + j * groupSize], offsetValue, scaleValue, sharedTmpBuffer,
                groupSize);
            PipeBarrier<PIPE_V>();
        }
    }
}

template <typename SrcType, typename DstType>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AntiQuantImplScalar(
    const LocalTensor<DstType>& dst, const LocalTensor<SrcType>& src, const LocalTensor<DstType>& scale,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount, const uint32_t K,
    const AntiQuantShapeInfo& shapeInfo)
{
    uint32_t N = src.GetSize() / K;
    uint32_t groupSize = K / (shapeInfo.scaleWidth == 0 ? scale.GetShapeInfo().shape[1] : shapeInfo.scaleWidth);
    uint32_t scaleLength = K / groupSize;

    SetMaskCount();
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < scaleLength; j++) {
            auto scaleValue = scale.GetValue(i * scaleLength + j);
            AntiQuantOuterLoop<SrcType, DstType, false>(
                dst[i * K + j * groupSize], src[i * K + j * groupSize], scaleValue, scaleValue, sharedTmpBuffer,
                groupSize);
            PipeBarrier<PIPE_V>();
        }
    }
}

template <typename SrcType, typename DstType, bool withOffset = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AntiQuantImplScalar(
    const LocalTensor<DstType>& dst, const LocalTensor<SrcType>& src, const DstType offset, const DstType scale,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount, const uint32_t K,
    const AntiQuantShapeInfo& shapeInfo)
{
    SetMaskCount();
    AntiQuantOuterLoop<SrcType, DstType, withOffset>(dst, src, offset, scale, sharedTmpBuffer, calCount);
}

template <bool withOffset = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AntiQuantFp16TransposeMainImpl(
    const LocalTensor<half>& dst, const LocalTensor<half>& src, const LocalTensor<half>& scale,
    const LocalTensor<half>& offset, const uint32_t srcN, const uint32_t K)
{
    SetMaskCount();
    // blk is continuous in dst and src0, and is same as src1
    // rep stride for dst and src0 is  from different line of N, which is K * sizeof(half) / 32B
    // rep stride for src1 is from fore-blk to next blk, which is 1
    uint32_t repStride = K * sizeof(half) / ONE_BLK_SIZE;
    BinaryRepeatParams binaryParams(1, 1, 0, repStride, repStride, 1);
    SetVectorMask<half, MaskMode::COUNTER>(0, srcN * B16_DATA_NUM_PER_REPEAT);
    const uint32_t loop = K / B16_DATA_NUM_PER_REPEAT;
    for (uint32_t i = 0; i < loop; ++i) {
        const uint32_t tmpOffset = i * B16_DATA_NUM_PER_REPEAT;
        if constexpr (withOffset) {
            Add<half, false>(dst[tmpOffset], src[tmpOffset], offset, MASK_PLACEHOLDER, srcN, binaryParams);
            PipeBarrier<PIPE_V>();
        }
        Mul<half, false>(dst[tmpOffset], dst[tmpOffset], scale, MASK_PLACEHOLDER, srcN, binaryParams);
        PipeBarrier<PIPE_V>();
    }
}

template <bool withOffset = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "AscendAntiQuant is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void AntiQuantFp16TransposeTailImpl(
    const LocalTensor<half>& dst, const LocalTensor<half>& src, const LocalTensor<half>& scale,
    const LocalTensor<half>& offset, const uint32_t srcN, const uint32_t K)
{
    SetMaskNorm();
    const uint32_t tailK = K % B16_DATA_NUM_PER_REPEAT;
    SetVectorMask<half, MaskMode::NORMAL>(tailK);
    // blk is continuous in dst and src0, and is same as src1
    // rep stride for dst and src0 is from first K to the next K, which is K * sizeof(half) / 32B(in unit of blk)
    // rep stride for src1 is from fore-blk to next blk, which is 1
    const uint32_t repStride = K * sizeof(half) / ONE_BLK_SIZE;
    BinaryRepeatParams binaryParams(1, 1, 0, repStride, repStride, 1);
    const uint32_t loop = srcN / MAX_REPEAT_TIMES;
    for (uint32_t i = 0; i < loop; ++i) {
        const uint32_t srcOffset = MAX_REPEAT_TIMES * K * i;
        const uint32_t scaleOffset = MAX_REPEAT_TIMES * B16_DATA_NUM_PER_BLOCK * i;
        if constexpr (withOffset) {
            Add<half, false>(
                dst[srcOffset], dst[srcOffset], offset[scaleOffset], MASK_PLACEHOLDER, MAX_REPEAT_TIMES, binaryParams);
            PipeBarrier<PIPE_V>();
        }
        Mul<half, false>(
            dst[srcOffset], dst[srcOffset], scale[scaleOffset], MASK_PLACEHOLDER, MAX_REPEAT_TIMES, binaryParams);
        PipeBarrier<PIPE_V>();
    }
    const uint32_t tailN = srcN % MAX_REPEAT_TIMES;
    if (tailN != 0) {
        const uint32_t srcOffset = loop * MAX_REPEAT_TIMES * K;
        const uint32_t scaleOffset = loop * MAX_REPEAT_TIMES * B16_DATA_NUM_PER_BLOCK;
        if constexpr (withOffset) {
            Add<half, false>(
                dst[srcOffset], dst[srcOffset], offset[scaleOffset], MASK_PLACEHOLDER, tailN, binaryParams);
            PipeBarrier<PIPE_V>();
        }
        Mul<half, false>(dst[srcOffset], dst[srcOffset], scale[scaleOffset], MASK_PLACEHOLDER, tailN, binaryParams);
        PipeBarrier<PIPE_V>();
    }
}

// only used when outType is half
// outType BF16 case: srcType -> FP32 (need lots of tmpSpace) -> do calculation -> BF16
// outType FP16 case: srcType -> FP16 (no need for tmpSpace) -> do calculation              better performance
template <typename SrcType, bool withOffset = true>
__aicore__ inline void AscendAntiQuantFP16Transpose(
    const LocalTensor<half>& dst, const LocalTensor<SrcType>& src, LocalTensor<half> offset,
    const LocalTensor<half>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K,
    const AntiQuantShapeInfo& shapeInfo)
{
    uint32_t calCount = src.GetSize();
    uint32_t scaleN = (shapeInfo.scaleHeight == 0 ? scale.GetShapeInfo().shape[0] : shapeInfo.scaleHeight);
    uint32_t scaleBrcbSize = ONE_BLK_SIZE / sizeof(half) * scaleN;
    uint32_t stackBufferSize = sharedTmpBuffer.GetSize() / sizeof(half);
    constexpr uint32_t tmpBufferCoeff = withOffset ? ANTIQUANT_TWO : 1;
    if (stackBufferSize < scaleBrcbSize * tmpBufferCoeff || K >= MAX_K_FOR_FP16_BRCB) {
        return withOffset ? AntiQuantImplScalar(dst, src, offset, scale, sharedTmpBuffer, calCount, K, shapeInfo) :
                            AntiQuantImplScalar(dst, src, scale, sharedTmpBuffer, calCount, K, shapeInfo);
    }

    // cast src to FP16
    SetMaskCount();
    SetVectorMask<half, MaskMode::COUNTER>(0, src.GetSize());
    if constexpr (IsSameType<SrcType, int4b_t>::value) {
        UnaryRepeatParams s42f16unaryParams;
        s42f16unaryParams.srcRepStride = ONE_FOURTH_DEFAULT_REPEAT_STRIDE;
        Cast<half, SrcType, false>(dst, src, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1, s42f16unaryParams);
    } else {
        UnaryRepeatParams unaryParams(1, 1, DEFAULT_REPEAT_STRIDE, HALF_DEFAULT_REPEAT_STRIDE);
        Cast<half, SrcType, false>(dst, src, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1, unaryParams);
    }
    PipeBarrier<PIPE_V>();

    LocalTensor<half> stackBuffer = sharedTmpBuffer.ReinterpretCast<half>();
    AntiquantParams<half> params;
    params.tempTensorScale = stackBuffer[0]; // store 16 * N * FP16    N -> brcb -> 16 * N
    if constexpr (withOffset) {
        params.tempTensorOffset = stackBuffer[B16_DATA_NUM_PER_BLOCK * scaleN];
    }
    AntiQuantFp16Brcb<withOffset>(scale, offset, params, scaleN);
    uint32_t srcN = src.GetSize() / K;
    if (K < B16_DATA_NUM_PER_REPEAT) {
        return AntiQuantFp16TransposeTailImpl<withOffset>(
            dst, dst, params.tempTensorScale, params.tempTensorOffset, srcN, K);
    }

    AntiQuantFp16TransposeMainImpl<withOffset>(dst, dst, params.tempTensorScale, params.tempTensorOffset, srcN, K);
    const uint32_t tailK = K % B16_DATA_NUM_PER_REPEAT;
    if (tailK != 0) {
        const uint32_t srcOffset = K - tailK;
        AntiQuantFp16TransposeTailImpl<withOffset>(
            dst[srcOffset], dst[srcOffset], params.tempTensorScale, params.tempTensorOffset, srcN, K);
    }
}

// only tensor scale
template <typename SrcType, typename DstType, bool isTranspose>
__aicore__ inline void AscendAntiQuantImpl(
    const LocalTensor<DstType>& dst, const LocalTensor<SrcType>& src, const LocalTensor<DstType>& scale,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K, const AntiQuantShapeInfo& shapeInfo = {})
{
    CHECK_FUNC_HIGHLEVEL_API(
        AscendAntiQuant, (SrcType, DstType, isTranspose), (dst, src, scale, sharedTmpBuffer, K, shapeInfo));

    uint32_t calCount = src.GetSize();
    if constexpr (!isTranspose) {
        AscendAntiQuantNoTranspose(dst, src, scale, sharedTmpBuffer, src.GetSize(), K, shapeInfo);
    } else if constexpr (IsSameType<DstType, half>::value) {
        AscendAntiQuantFP16Transpose<SrcType, false>(dst, src, scale, scale, sharedTmpBuffer, K, shapeInfo);
    } else {
        AscendAntiQuantBF16Transpose(dst, src, scale, sharedTmpBuffer, K, shapeInfo);
    }
    SetMaskNorm();
    ResetMask();
}

// only scalar scale
template <typename SrcType, typename DstType, bool isTranspose>
__aicore__ inline void AscendAntiQuantImpl(
    const LocalTensor<DstType>& dst, const LocalTensor<SrcType>& src, const DstType scale,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K, const AntiQuantShapeInfo& shapeInfo = {})
{
    CHECK_FUNC_HIGHLEVEL_API(
        AscendAntiQuant, (SrcType, DstType, isTranspose), (dst, src, scale, sharedTmpBuffer, K, shapeInfo));

    if constexpr (!isTranspose) {
        AscendAntiQuantNoTranspose<SrcType, DstType, false>(
            dst, src, scale, scale, sharedTmpBuffer, src.GetSize(), K, shapeInfo);
    } else {
        AntiQuantImplScalar<SrcType, DstType, false>(
            dst, src, scale, scale, sharedTmpBuffer, src.GetSize(), K, shapeInfo);
    }
    SetMaskNorm();
    ResetMask();
}

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002)
template <typename SrcType, typename DstType, bool isTranspose>
__aicore__ inline void AscendAntiQuantImplCommon(
    const LocalTensor<DstType>& dst, const LocalTensor<SrcType>& src, const LocalTensor<DstType>& offset,
    const LocalTensor<DstType>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K,
    const AntiQuantShapeInfo& shapeInfo = {})
{
    if constexpr (!isTranspose) {
        AscendAntiQuantNoTranspose(dst, src, offset, scale, sharedTmpBuffer, src.GetSize(), K, shapeInfo);
    } else if constexpr (IsSameType<DstType, half>::value) {
        AscendAntiQuantFP16Transpose<SrcType, true>(dst, src, offset, scale, sharedTmpBuffer, K, shapeInfo);
    } else { // BF16 case
        AscendAntiQuantBF16Transpose(dst, src, offset, scale, sharedTmpBuffer, K, shapeInfo);
    }
    SetMaskNorm();
    ResetMask();
}

template <typename SrcType, typename DstType, bool isTranspose>
__aicore__ inline void AscendAntiQuantImplCommon(
    const LocalTensor<DstType>& dst, const LocalTensor<SrcType>& src, const DstType offset, const DstType scale,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K, const AntiQuantShapeInfo& shapeInfo = {})
{
    if constexpr (!isTranspose) {
        AscendAntiQuantNoTranspose<SrcType, DstType, true>(
            dst, src, offset, scale, sharedTmpBuffer, src.GetSize(), K, shapeInfo);
    } else {
        AntiQuantImplScalar<SrcType, DstType, true>(
            dst, src, offset, scale, sharedTmpBuffer, src.GetSize(), K, shapeInfo);
    }
    SetMaskNorm();
    ResetMask();
}
#endif

// tensor offset + tensor scale
template <typename SrcType, typename DstType, bool isTranspose>
__aicore__ inline void AscendAntiQuantImpl(
    const LocalTensor<DstType>& dst, const LocalTensor<SrcType>& src, const LocalTensor<DstType>& offset,
    const LocalTensor<DstType>& scale, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K,
    const AntiQuantShapeInfo& shapeInfo = {})
{
    CHECK_FUNC_HIGHLEVEL_API(
        AscendAntiQuant, (SrcType, DstType, isTranspose), (dst, src, offset, scale, sharedTmpBuffer, K, shapeInfo));

    AscendAntiQuantImplCommon<SrcType, DstType, isTranspose>(dst, src, offset, scale, sharedTmpBuffer, K, shapeInfo);
}

template <typename SrcType, typename DstType, bool isTranspose>
__aicore__ inline void AscendAntiQuantImpl(
    const LocalTensor<DstType>& dst, const LocalTensor<SrcType>& src, const LocalTensor<DstType>& offset,
    const LocalTensor<DstType>& scale, const uint32_t K, const AntiQuantShapeInfo& shapeInfo = {})
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "AntiQuant PopStackBuffer Error!"); });
    AscendAntiQuantImpl<SrcType, DstType, isTranspose>(dst, src, offset, scale, sharedTmpBuffer, K, shapeInfo);
}

// scalar offset + scalar scale
template <typename SrcType, typename DstType, bool isTranspose>
__aicore__ inline void AscendAntiQuantImpl(
    const LocalTensor<DstType>& dst, const LocalTensor<SrcType>& src, const DstType offset, const DstType scale,
    const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t K, const AntiQuantShapeInfo& shapeInfo = {})
{
    CHECK_FUNC_HIGHLEVEL_API(
        AscendAntiQuant, (SrcType, DstType, isTranspose), (dst, src, offset, scale, sharedTmpBuffer, K, shapeInfo));

    AscendAntiQuantImplCommon<SrcType, DstType, isTranspose>(dst, src, offset, scale, sharedTmpBuffer, K, shapeInfo);
}

template <typename SrcType, typename DstType, bool isTranspose>
__aicore__ inline void AscendAntiQuantImpl(
    const LocalTensor<DstType>& dst, const LocalTensor<SrcType>& src, const DstType offset, const DstType scale,
    const uint32_t K, const AntiQuantShapeInfo& shapeInfo = {})
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "AntiQuant PopStackBuffer Error!"); });
    AscendAntiQuantImpl<SrcType, DstType, isTranspose>(dst, src, offset, scale, sharedTmpBuffer, K, shapeInfo);
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 9201) || __NPU_ARCH__ == 5102)
template <typename InputDataType, typename OutputDataType, bool isTranspose>
__aicore__ inline void AscendAntiQuantImpl(
    const LocalTensor<OutputDataType>& dst, const LocalTensor<InputDataType>& src, const LocalTensor<fp8_e8m0_t>& scale,
    const uint32_t K, const AntiQuantShapeInfo& shapeInfo = {})
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "AntiQuant PopStackBuffer Error!"); });
    AscendAntiQuantImpl<InputDataType, OutputDataType, isTranspose>(dst, src, scale, sharedTmpBuffer, K, shapeInfo);
}
template <
    typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config,
    const AscendAntiQuantPolicy& policy>
__aicore__ inline void AscendAntiQuantImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const LocalTensor<scaleT>& scaleTensor,
    const LocalTensor<scaleT>& offsetTensor, const AscendAntiQuantParam& para)
{
    LocalTensor<uint8_t> stackTensor;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackTensor);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    AscendAntiQuantImpl<dstT, srcT, scaleT, config, policy>(
        dstTensor, srcTensor, stackTensor, scaleTensor, offsetTensor, para);
}

template <
    typename dstT, typename srcT, typename scaleT, const AscendAntiQuantConfig& config,
    const AscendAntiQuantPolicy& policy>
__aicore__ inline void AscendAntiQuantImpl(
    const LocalTensor<dstT>& dstTensor, const LocalTensor<srcT>& srcTensor, const scaleT scale, const scaleT offset,
    const AscendAntiQuantParam& para)
{
    LocalTensor<uint8_t> stackTensor;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(stackTensor);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    AscendAntiQuantImpl<dstT, srcT, scaleT, config, policy>(dstTensor, srcTensor, stackTensor, scale, offset, para);
}
#endif
} // namespace AscendC
#endif // IMPL_QUANTIZATION_ANTIQUANT_ASCEND_ANTIQUANT_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_ANTIQUANT_ASCEND_ANTIQUANT_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_QUANTIZATION_ANTIQUANT_ASCEND_ANTIQUANT_IMPL_H__
#endif
