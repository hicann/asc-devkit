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
 * \file broadcast_common_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/pad/broadcast/broadcast_common_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/pad/broadcast.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_BROADCAST_BROADCAST_COMMON_IMPL_H__
#endif

#ifndef IMPL_PAD_BROADCAST_BROADCAST_COMMON_IMPL_H
#define IMPL_PAD_BROADCAST_BROADCAST_COMMON_IMPL_H

#include "../../../../../include/basic_api/kernel_basic_intf.h"
#include "../../../../../include/basic_api/kernel_tensor.h"
#include "broadcast_common_utils.h"
#include "../../../../../include/adv_api/pad/broadcast_utils.h"
#ifdef ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_check/pad/broadcast/broadcast_check.h"
#endif // ASCENDC_CPU_DEBUG
#include "../../api_check/kernel_api_check.h"
#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
#include "broadcast_3510_impl.h"
#elif defined(__NPU_ARCH__) && __NPU_ARCH__ == 2201
#include "broadcast_v220_impl.h"
#else
#include "broadcast_v200_impl.h"
#endif
#if (                                                                                                 \
    defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 3510 || \
                              __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113))

namespace AscendC {
constexpr uint32_t TWO_DIM = 2;
constexpr uint32_t HALF_ONE_BLK_SIZE = 16;

#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
template <typename T, int constRank = -1, uint32_t* constDstShape = nullptr, uint32_t* constSrcShape = nullptr>
__aicore__ inline void GetBroadcastTilingInfoImpl(
    uint32_t rank, const uint32_t* dstShape, const uint32_t* srcShape, bool srcInnerPad, BroadcastTiling& tiling)
{
    static_assert(
        (constRank == -1) || (constRank <= 9 && constRank > 0),
        "constRank only supports -1 and the range between 1 and 9");
    ASCENDC_ASSERT((rank <= 9 && rank > 0), { KERNEL_LOG(KERNEL_ERROR, "rank should be in range [1, 9]"); });
    ASCENDC_ASSERT((constRank == -1) || (constRank == rank), {
        KERNEL_LOG(KERNEL_ERROR, "constRank should be equal to rank when constRank != -1");
    });
    constexpr uint32_t maxDim = 9;

    uint32_t srcSize = 1;
    uint32_t dstSize = 1;
    uint32_t newSrcShape[9];
    // dstshape/srcshape passed into tiling
    for (uint32_t i = 0; i < rank; i++) {
        tiling.dstShape[i] = dstShape[i];
        newSrcShape[i] = srcShape[i];
        srcSize *= srcShape[i];
        dstSize *= dstShape[i];

        tiling.oriSrcShape[i] = srcShape[i];
        tiling.oriDstShape[i] = dstShape[i];
    }
    tiling.oriRank = rank;

    // axis fusion when constRank = -1 or constRank > 4
    if constexpr (constRank == -1 || constRank > 4) {
        uint32_t begin = 0;
        uint32_t i = 0;
        uint32_t count = 0;
        uint32_t size = 1;
        while (i < tiling.oriRank) {
            while (i < tiling.oriRank && (newSrcShape[i] == 1 && tiling.dstShape[i] != 1)) {
                size *= tiling.dstShape[i++];
            }
            if (i - begin >= 1) {
                tiling.dstShape[count] = size;
                newSrcShape[count] = 1;
                rank -= (i - begin - 1);
                ++count;
            }
            begin = i;
            size = 1;
            while (i < tiling.oriRank && newSrcShape[i] == tiling.dstShape[i]) {
                size *= tiling.dstShape[i++];
            }
            if (i - begin >= 1) {
                tiling.dstShape[count] = size;
                newSrcShape[count] = size;
                rank -= (i - begin - 1);
                ++count;
            }
            begin = i;
            size = 1;
        }
        while (i < maxDim) {
            tiling.dstShape[i] = 1;
            newSrcShape[i] = 1;
            ++i;
        }
    }

    // deal with situation that datatype is b64
    if (sizeof(T) == sizeof(uint64_t) && (srcSize != dstSize)) {
        if (newSrcShape[rank - 1] == 1 && tiling.dstShape[rank - 1] != 1) {
            if (rank < maxDim) {
                tiling.dstShape[rank] = 2;
                newSrcShape[rank] = 2;
                rank += 1;
            } else {
                tiling.loopNum = tiling.dstShape[0];
            }
        } else {
            newSrcShape[rank - 1] *= 2;
            tiling.dstShape[rank - 1] *= 2;
        }
        srcSize *= 2;
        dstSize *= 2;
    }
    tiling.rank = rank;
    tiling.dstSize = dstSize;
    tiling.srcSize = srcSize;
    bool srcStrideZero = false;
    if (tiling.loopNum != 0) {
        if (newSrcShape[0] == 1 && tiling.dstShape[0] != 1) {
            tiling.srcStride[9] = 0;
            srcStrideZero = true;
        }
        for (uint32_t i = 0; i < maxDim - 1; i++) {
            tiling.dstShape[i] = tiling.dstShape[i + 1];
            newSrcShape[i] = newSrcShape[i + 1];
        }
        tiling.dstShape[maxDim - 1] = 2;
        newSrcShape[maxDim - 1] = 2;
    }

    // calculate src/dst stride
    uint32_t lastSrcStride = 1;
    uint32_t lastDstStride = 1;
    int32_t end = rank > maxDim ? maxDim : rank;
    for (int32_t i = end - 1; i >= 0; i--) {
        tiling.dstStride[i] = lastDstStride;
        lastDstStride *= tiling.dstShape[i];
        if (newSrcShape[i] == 1 && tiling.dstShape[i] != 1) {
            tiling.srcStride[i] = 0;
        } else {
            tiling.srcStride[i] = lastSrcStride;
            lastSrcStride *= newSrcShape[i];
        }
    }
    if (tiling.loopNum != 0 && !srcStrideZero) {
        tiling.srcStride[9] = lastSrcStride;
    }
}

template <typename T, int32_t constRank>
__aicore__ inline void BroadcastCompute(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t* dstShape, const uint32_t* dstStride,
    const uint32_t* srcStride, int32_t dim, uint32_t srcSize, uint32_t dstSize, uint32_t loopNum)
{
    BroadcastInternal::DstShapeCheck(dstShape, dim);
    using BrcType = typename AscendC::BroadcastInternal::ExtractUnsignedTypeBySize<sizeof(T)>::T;
    __ubuf__ BrcType* dstUb = (__ubuf__ BrcType*)dst.GetPhyAddr();
    __ubuf__ BrcType* srcUb = (__ubuf__ BrcType*)src.GetPhyAddr();
    bool isReduceBranch = false;
    if (srcSize == dstSize) {
        const uint32_t alignSize = ONE_BLK_SIZE / sizeof(T);
        DataCopy(dst, src, AlignUp(dstSize, alignSize));
    } else if (srcSize == 1) {
        BroadcastInternal::BrcDuplicate<BrcType>(dstUb, srcUb, dstSize);
    } else {
        if (srcStride[dim - 1] == 0) {
            if constexpr (constRank == 2) {
                BroadcastInternal::BrcLastWrapperForTwoDim<BrcType, constRank>(dstUb, srcUb, dstShape);
            } else if constexpr (constRank == 3) {
                BroadcastInternal::BrcLastWrapperForThreeDim<BrcType, constRank>(dstUb, srcUb, dstShape, srcStride);
            } else if constexpr (constRank == 4) {
                BroadcastInternal::BrcLastWrapperForFourDim<BrcType, constRank>(dstUb, srcUb, dstShape, srcStride);
            } else {
                if (dim == 2) {
                    isReduceBranch = BroadcastInternal::BrcLastWrapperForTwoDim<BrcType>(dstUb, srcUb, dstShape);
                } else if (dim == 3) {
                    isReduceBranch =
                        BroadcastInternal::BrcLastWrapperForThreeDim<BrcType>(dstUb, srcUb, dstShape, srcStride);
                } else if (dim == 4) {
                    isReduceBranch =
                        BroadcastInternal::BrcLastWrapperForFourDim<BrcType>(dstUb, srcUb, dstShape, srcStride);
                } else if (dim > 4) {
                    isReduceBranch = true;
                }
            }
        } else {
            if constexpr (constRank == 2) {
                if constexpr (sizeof(T) == sizeof(uint64_t)) {
                    if (dim != constRank) {
                        BroadcastInternal::BrcNlastWrapperForThreeDim<BrcType, constRank>(
                            dstUb, srcUb, dstShape, srcStride);
                    } else {
                        BroadcastInternal::BrcNlastWrapperForTwoDim<BrcType, constRank>(dstUb, srcUb, dstShape);
                    }
                } else {
                    BroadcastInternal::BrcNlastWrapperForTwoDim<BrcType, constRank>(dstUb, srcUb, dstShape);
                }
            } else if constexpr (constRank == 3) {
                if constexpr (sizeof(T) == sizeof(uint64_t)) {
                    if (dim != constRank) {
                        BroadcastInternal::BrcNlastWrapperForFourDim<BrcType, constRank>(
                            dstUb, srcUb, dstShape, srcStride);
                    } else {
                        BroadcastInternal::BrcNlastWrapperForThreeDim<BrcType, constRank>(
                            dstUb, srcUb, dstShape, srcStride);
                    }
                } else {
                    BroadcastInternal::BrcNlastWrapperForThreeDim<BrcType, constRank>(
                        dstUb, srcUb, dstShape, srcStride);
                }
            } else if constexpr (constRank == 4) {
                if constexpr (sizeof(T) == sizeof(uint64_t)) {
                    if (dim != constRank) {
                        BroadcastInternal::BrcNlastWrapperForMoreDim<BrcType>(
                            dstUb, srcUb, dstShape, dstStride, srcStride);
                    } else {
                        BroadcastInternal::BrcNlastWrapperForFourDim<BrcType, constRank>(
                            dstUb, srcUb, dstShape, srcStride);
                    }
                } else {
                    BroadcastInternal::BrcNlastWrapperForFourDim<BrcType, constRank>(dstUb, srcUb, dstShape, srcStride);
                }
            } else {
                if (dim == 2) {
                    isReduceBranch = BroadcastInternal::BrcNlastWrapperForTwoDim<BrcType>(dstUb, srcUb, dstShape);
                } else if (dim == 3) {
                    isReduceBranch =
                        BroadcastInternal::BrcNlastWrapperForThreeDim<BrcType>(dstUb, srcUb, dstShape, srcStride);
                } else if (dim == 4) {
                    isReduceBranch =
                        BroadcastInternal::BrcNlastWrapperForFourDim<BrcType>(dstUb, srcUb, dstShape, srcStride);
                } else if (dim > 4) {
                    isReduceBranch = true;
                }
            }
        }
        if (isReduceBranch) {
            loopNum = loopNum == 0 ? 1 : loopNum;
            __ubuf__ BrcType* dstUbTmp = dstUb;
            __ubuf__ BrcType* srcUbTmp = srcUb;
            for (uint16_t h = 0; h < loopNum; ++h) {
                dstUb = dstUbTmp + h * dstStride[0] * dstShape[0];
                srcUb = srcUbTmp + h * srcStride[9];
                if (srcStride[dim - 1] == 0) {
                    BroadcastInternal::BrcLastWrapperForMoreDimDynamicShape<BrcType>(
                        dstUb, srcUb, dim, dstShape, dstStride, srcStride);
                } else {
                    BroadcastInternal::BrcNlastWrapperForMoreDimDynamicShape<BrcType>(
                        dstUb, srcUb, dim, dstShape, dstStride, srcStride);
                }
            }
        }
    }
}

template <
    typename T, int constRank = -1, uint32_t* constDstShape = nullptr, uint32_t* constSrcShape = nullptr,
    bool constSrcInnerPad = false>
__aicore__ inline void BroadcastImpl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t* dstShape, const uint32_t* srcShape,
    BroadcastTiling* tiling)
{
    CheckTensorPos<T>(dst, Hardware::UB, "dstTensor", "VECIN/VECCALC/VECOUT", "Broadcast");
    CheckTensorPos<T>(src, Hardware::UB, "srcTensor", "VECIN/VECCALC/VECOUT", "Broadcast");
    static_assert(
        (constRank == -1) || (constRank <= 9 && constRank > 0),
        "constRank only supports -1 and the range between 1 and 9");
    static_assert(SupportBytes<T, 1, 2, 4, 8>(), "Broadcast only supports type b8/b16/b32/b64 on current device");
    ASCENDC_ASSERT((tiling != nullptr), "BroadcastTiling could not be empty!");
    if constexpr (constRank != -1) {
        ASCENDC_ASSERT((tiling->oriRank == constRank), {
            KERNEL_LOG(KERNEL_ERROR, "Tilling original rank and constRank should be equal!");
        });
    }
    BroadcastInternal::ShapeCheck(tiling->oriDstShape, dstShape, tiling->oriRank);
    BroadcastInternal::ShapeCheck(tiling->oriSrcShape, srcShape, tiling->oriRank);

    uint32_t srcSize = tiling->srcSize;
    uint32_t dstSize = tiling->dstSize;
    uint32_t loopNum = tiling->loopNum;
    uint32_t* dstStride = tiling->dstStride;
    uint32_t* srcStride = tiling->srcStride;
    uint32_t* newDstShape = tiling->dstShape;
    int32_t dim = tiling->rank;

    BroadcastCompute<T, constRank>(dst, src, newDstShape, dstStride, srcStride, dim, srcSize, dstSize, loopNum);
}
#else

template <typename T, int32_t dim, int32_t axis, bool isReuseSource = false>
__aicore__ inline void TwoDimBroadCastFirstDim(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dstShape[dim],
    const uint32_t srcShape[dim], LocalTensor<T>& tmpBuffer)
{
    const uint32_t firstDim = dstShape[0];
    const uint32_t numBlocks = dstShape[1];
    ASCENDC_ASSERT(
        (numBlocks * sizeof(T) % ONE_BLK_SIZE == 0), { KERNEL_LOG(KERNEL_ERROR, "Non-alignment is not supported!"); });

    constexpr uint32_t oneBlockElementNum = ONE_BLK_SIZE / sizeof(T);
    constexpr uint32_t FIRST_DIM_LOOP_LIMITE = MAX_REPEAT_NUM * oneBlockElementNum;

    auto zeroTemp = tmpBuffer;
    Duplicate(zeroTemp.template ReinterpretCast<uint16_t>(), (uint16_t)0, ONE_BLK_SIZE / sizeof(uint16_t));
    PipeBarrier<PIPE_V>();

    if (numBlocks >= FIRST_DIM_LOOP_LIMITE) {
        LoopBroadCast<T>(dstLocal, srcLocal, zeroTemp, numBlocks, firstDim);
        return;
    }

    TwoDimBroadCastDimAlign<T, isReuseSource>(dstLocal, srcLocal, zeroTemp, firstDim, numBlocks);
}

template <typename T, int32_t dim, int32_t axis, bool isReuseSource = false>
__aicore__ inline void BroadCastCompute(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dstShape[dim],
    const uint32_t srcShape[dim], LocalTensor<T>& tmpBuffer)
{
    uint32_t srcSize = 1;
    uint32_t dstSize = 1;
    for (uint32_t i = 0; i < dim; i++) {
        srcSize *= srcShape[i];
        dstSize *= dstShape[i];
    }

    if (srcSize == dstSize) {
        NoBroad(dstLocal, srcLocal, dstSize);
    } else if (srcSize == 1) {
        TEventID event1 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_S));
        SetFlag<HardEvent::V_S>(event1);
        WaitFlag<HardEvent::V_S>(event1);
        auto scalar = srcLocal.GetValue(0);
        TEventID event2 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
        SetFlag<HardEvent::S_V>(event2);
        WaitFlag<HardEvent::S_V>(event2);
        Duplicate(dstLocal, scalar, dstSize);
        PipeBarrier<PIPE_V>();
    } else {
        if constexpr (dim == TWO_DIM) {
            if constexpr (axis == 1) { // last broadcast
                TwoDimBroadCastLastDim<T, dim, axis, false>(dstLocal, srcLocal, dstShape, srcShape, tmpBuffer);
            } else { // first broadcast
                TwoDimBroadCastFirstDim<T, dim, axis, false>(dstLocal, srcLocal, dstShape, srcShape, tmpBuffer);
            }
        } else {
            KERNEL_LOG(KERNEL_ERROR, "Now only support dim = 1 or dim =2, but we get dim= %d", dim);
        }
    }
    SetMaskCount();
}
#endif

template <typename T, int32_t dim, int32_t axis, bool isReuseSource = false>
__aicore__ inline void BroadCast(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dstShape[dim],
    const uint32_t srcShape[dim]);

template <typename T, int32_t dim, int32_t axis, bool isReuseSource = false>
__aicore__ inline void BroadCast(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dstShape[dim],
    const uint32_t srcShape[dim], LocalTensor<uint8_t>& sharedTmpBuffer);

#if defined(__NPU_ARCH__) && \
    (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113)
template <typename T, int32_t dim, int32_t axis, bool isReuseSource = false>
__aicore__ inline void BroadCastCommon(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dstShape[dim],
    const uint32_t srcShape[dim])
{
    static_assert(SupportBytes<T, 1, 2, 4>(), "Broadcast only supports type b8/b16/b32 on current device");
    ASCENDC_ASSERT((dim <= 2 && dim > 0), {
        KERNEL_LOG(KERNEL_ERROR, "Now only support dim = 1 or dim = 2, but we get dim= %d", dim);
    });
    ASCENDC_ASSERT((axis == 1 || axis == 0), {
        KERNEL_LOG(KERNEL_ERROR, "Now only support axis = 0 or axis = 1, but we get axis= %d", dim);
    });

    uint32_t srcSize = 1;
    uint32_t dstSize = 1;
    uint32_t srcStride[dim];
    uint32_t dstStride[dim];

    for (uint32_t i = 0; i < dim; i++) {
        srcSize *= srcShape[i];
        dstSize *= dstShape[i];
    }

    uint32_t lastSrcStride = 1;
    uint32_t lastDstStride = 1;
    for (int32_t i = dim - 1; i >= 0; i--) {
        dstStride[i] = lastDstStride;
        lastDstStride *= dstShape[i];
        if (srcShape[i] == 1 && dstShape[i] != 1) {
            srcStride[i] = 0;
        } else {
            srcStride[i] = lastSrcStride;
            lastSrcStride *= srcShape[i];
        }
    }
    BroadcastCompute<T, dim>(dstLocal, srcLocal, dstShape, dstStride, srcStride, dim, srcSize, dstSize, 0);
}

template <typename T, int32_t dim, int32_t axis, bool isReuseSource = false>
__aicore__ inline void BroadCastCommon(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dstShape[dim],
    const uint32_t srcShape[dim], LocalTensor<uint8_t>& sharedTmpBuffer)
{
    BroadCast<T, dim, axis, isReuseSource>(dstLocal, srcLocal, dstShape, srcShape);
}

#else
template <typename T, int32_t dim, int32_t axis, bool isReuseSource = false>
__aicore__ inline void BroadCastCommon(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dstShape[dim],
    const uint32_t srcShape[dim])
{
    LocalTensor<uint8_t> sharedTmpBuffer;
    bool ans = PopStackBuffer<uint8_t, TPosition::LCM>(sharedTmpBuffer);
    ASCENDC_ASSERT((ans), { KERNEL_LOG(KERNEL_ERROR, "PopStackBuffer Error!"); });
    BroadCast<T, dim, axis, isReuseSource>(dstLocal, srcLocal, dstShape, srcShape, sharedTmpBuffer);
}

template <typename T, int32_t dim, int32_t axis, bool isReuseSource = false>
__aicore__ inline void BroadCastCommon(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dstShape[dim],
    const uint32_t srcShape[dim], LocalTensor<uint8_t>& sharedTmpBuffer)
{
    TRACE_START(TraceId::BroadCast);
    if constexpr (sizeof(T) == 1) {
        LocalTensor<half> tmpBuffer = sharedTmpBuffer.ReinterpretCast<half>();
        uint32_t srcSize = 1;
        uint32_t dstSize = 1;
        for (uint32_t i = 0; i < dim; i++) {
            srcSize *= srcShape[i];
            dstSize *= dstShape[i];
        }
        auto srcTempBuffer = tmpBuffer;
        const uint32_t alignSrcSize = ((srcSize + HALF_ONE_BLK_SIZE - 1) / HALF_ONE_BLK_SIZE) * HALF_ONE_BLK_SIZE;
        const uint32_t alignDstSize = ((dstSize + HALF_ONE_BLK_SIZE - 1) / HALF_ONE_BLK_SIZE) * HALF_ONE_BLK_SIZE;
        auto dstTempBuffer = tmpBuffer[alignSrcSize];
        auto tempTempBuffer = dstTempBuffer[alignDstSize];
        SetMaskCount();
        SetVectorMask<T, MaskMode::COUNTER>(srcSize);
        Cast<half, T, false>(
            srcTempBuffer, srcLocal, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
            {1, 1, DEFAULT_REPEAT_STRIDE, HALF_DEFAULT_REPEAT_STRIDE});
        PipeBarrier<PIPE_V>();
        // After BroadCastCompute, Reset to Counter model
        BroadCastCompute<half, dim, axis, isReuseSource>(
            dstTempBuffer, srcTempBuffer, dstShape, srcShape, tempTempBuffer);
        SetVectorMask<T, MaskMode::COUNTER>(dstSize);
        Cast<T, half, false>(
            dstLocal, dstTempBuffer, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
            {1, 1, HALF_DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
        PipeBarrier<PIPE_V>();
        SetMaskNorm();
        ResetMask();
    } else {
        LocalTensor<T> tmpBuffer = sharedTmpBuffer.ReinterpretCast<T>();
        SetMaskCount();
        BroadCastCompute<T, dim, axis, isReuseSource>(dstLocal, srcLocal, dstShape, srcShape, tmpBuffer);
        SetMaskNorm();
        ResetMask();
    }
    TRACE_STOP(TraceId::BroadCast);
}
#endif

template <typename T, int32_t dim, int32_t axis, bool isReuseSource>
__aicore__ inline void BroadCast(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dstShape[dim],
    const uint32_t srcShape[dim])
{
    // Only for AI Vector Core.
    if ASCEND_IS_AIC {
        return;
    }
    BroadCastCommon<T, dim, axis, isReuseSource>(dstLocal, srcLocal, dstShape, srcShape);
}

template <typename T, int32_t dim, int32_t axis, bool isReuseSource>
__aicore__ inline void BroadCast(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t dstShape[dim],
    const uint32_t srcShape[dim], LocalTensor<uint8_t>& sharedTmpBuffer)
{
    if ASCEND_IS_AIC {
        return;
    }
    CHECK_FUNC_HIGHLEVEL_API(
        Broadcast, (T, dim, axis, isReuseSource), (dstLocal, srcLocal, dstShape, srcShape, sharedTmpBuffer));
    BroadCastCommon<T, dim, axis, isReuseSource>(dstLocal, srcLocal, dstShape, srcShape, sharedTmpBuffer);
}
} // namespace AscendC
#endif
#endif // IMPL_PAD_BROADCAST_BROADCAST_COMMON_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_BROADCAST_BROADCAST_COMMON_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_PAD_BROADCAST_BROADCAST_COMMON_IMPL_H__
#endif
