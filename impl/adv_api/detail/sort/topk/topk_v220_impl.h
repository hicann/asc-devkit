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
 * \file topk_v220_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/sort/topk/topk_v220_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/sort/topk.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SORT_TOPK_TOPK_V220_IMPL_H__
#endif

#ifndef IMPL_SORT_TOPK_TOPK_V220_IMPL_H
#define IMPL_SORT_TOPK_TOPK_V220_IMPL_H
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <type_traits>
#include "../../../../basic_api/kernel_log.h"
#endif

#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "kernel_tiling/kernel_tiling.h"
#include "topk_common_utils.h"

namespace AscendC {
#if ASCENDC_CPU_DEBUG
template <typename T, bool isInitIndex = false, enum TopKMode topkMode = TopKMode::TOPK_NORMAL>
void TopkInputCheck(const int32_t k, const TopKInfo& topKInfo)
{
    bool ans = (1 <= k) && (k <= topKInfo.n);
    ASCENDC_ASSERT(ans, {
        KERNEL_LOG(KERNEL_ERROR, "The value of k must be greater than or equal to 1 and less than or equal to inner.");
    });
    ans = (1 <= topKInfo.n) && (topKInfo.n <= topKInfo.inner);
    ASCENDC_ASSERT(ans, {
        KERNEL_LOG(KERNEL_ERROR, "The value of n must be greater than or equal to 1 and less than or equal to inner.");
    });
    ans = (std::is_same<T, half>::value) || (std::is_same<T, float>::value);
    ASCENDC_ASSERT(ans, { KERNEL_LOG(KERNEL_ERROR, "type must be half or float"); });
    ans = (topKInfo.inner % TOPK_INNER_ALIGN_LEN == 0);
    ASCENDC_ASSERT(ans, { KERNEL_LOG(KERNEL_ERROR, "The value of inner must be an integer multiple of 32."); });

    if constexpr (topkMode == TopKMode::TOPK_NORMAL) {
        ans = (topKInfo.inner <= TOPK_NORMAL_INNER_MAX_LEN);
        ASCENDC_ASSERT(ans, { KERNEL_LOG(KERNEL_ERROR, "The maximum value supported by inner is 4096."); });
    }
    if constexpr (topkMode == TopKMode::TOPK_NSMALL) {
        ans = (topKInfo.inner == TOPK_NSMALL_INNER_LEN);
        ASCENDC_ASSERT(ans, { KERNEL_LOG(KERNEL_ERROR, "The value of inner must be an equal 32."); });
    }
}
#endif

template <typename T>
__aicore__ inline void MrgFourQueueSort(
    const LocalTensor<T>& tmpLocal, const TopkTiling& tiling, const TopKInfo& topKInfo, uint16_t& z,
    int32_t& mrgFourQueueCount, int32_t& dstIdx)
{
    uint16_t innerU16Type = static_cast<uint16_t>(topKInfo.inner);
    for (; z * MRGSORT_VALID_QUEUE <= innerU16Type; z *= MRGSORT_VALID_QUEUE) {
        auto src = (mrgFourQueueCount % TWO == 1) ? tmpLocal[tiling.innerDataSize] : tmpLocal[0];
        dstIdx = (mrgFourQueueCount + 1) % TWO;
        auto dst = (dstIdx == 1) ? tmpLocal[tiling.innerDataSize] : tmpLocal[0];
        mrgFourQueueCount += 1;
        uint16_t elementLengths[MRG_SORT_ELEMENT_LEN] = {z, z, z, z};
        struct MrgSort4Info srcInfo(elementLengths, false, 0b1111, tiling.mrgSortRepeat / z);
        struct MrgSortSrcList<T> srcList(
            src, src[z * tiling.mrgSortSrc1offset], src[z * tiling.mrgSortSrc2offset],
            src[z * tiling.mrgSortSrc3offset]);
        MrgSort<T>(dst, srcList, srcInfo);
        PipeBarrier<PIPE_V>();
        const DataCopyParams intriParams = {static_cast<uint16_t>(tiling.copyUbToUbBlockCount), 1, 0, 0};
        DataCopy(src, dst, intriParams);
        PipeBarrier<PIPE_V>();
    }
}

template <typename T>
__aicore__ inline void MrgTwoQueueSort(
    const LocalTensor<T>& tmpLocal, const TopkTiling& tiling, const TopKInfo& topKInfo, const uint16_t z,
    const int32_t mrgFourQueueCount, int32_t& dstIdx, const int32_t k)
{
    int32_t arrayCount = 0;
    if (z < topKInfo.inner) {
        // MRG_MAX_ARRAY_SIZE is the number of four-way merging times is multiplied by three;
        int32_t mrgArray[MRG_MAX_ARRAY_SIZE] = {0};
        int32_t tmpInner = topKInfo.inner;
        for (int32_t i = z; i >= MIN_SORT32_SIZE; i /= MRGSORT_VALID_QUEUE) {
            int32_t count;
            for (count = 0; count < tmpInner / i; ++count) {
                mrgArray[arrayCount++] = i;
            }
            tmpInner -= count * i;
        }
        uint16_t mrgSortedLen = 0;
        for (int32_t i = 0; i < arrayCount - 1; ++i) {
            auto src = ((mrgFourQueueCount + i) % TWO == 1) ? tmpLocal[tiling.innerDataSize] : tmpLocal[0];
            dstIdx = (mrgFourQueueCount + 1 + i) % TWO;
            auto dst = (dstIdx == 1) ? tmpLocal[tiling.innerDataSize] : tmpLocal[0];
            mrgSortedLen += static_cast<uint16_t>(mrgArray[i]);
            uint64_t tmpMrgSortedLen = mrgSortedLen;
            uint64_t tmpMrgArray = mrgArray[i + 1];
            if (mrgSortedLen > k) {
                tmpMrgSortedLen = k;
            }
            if (mrgArray[i + 1] > k) {
                tmpMrgArray = k;
            }
            uint16_t elementLengths[MRG_SORT_ELEMENT_LEN] = {
                static_cast<uint16_t>(tmpMrgSortedLen), static_cast<uint16_t>(tmpMrgArray), 0, 0};
            struct MrgSort4Info srcInfo(elementLengths, false, 0b0011, 1);
            struct MrgSortSrcList<T> srcList(src, src[mrgSortedLen * tiling.mrgSortTwoQueueSrc1Offset], src, src);
            MrgSort<T>(dst, srcList, srcInfo);
            PipeBarrier<PIPE_V>();
        }
    }
}

template <typename T>
__aicore__ inline void GatherDstValAndDstIdx(
    const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& tmpLocal,
    const TopkTiling& tiling, const int32_t dstIdx, const int32_t dstOffsetFourBytes, const int outterIdx)
{
    uint64_t rsvdCnt = 0;
    int32_t tmpLocalDstOffset = tiling.innerDataSize * dstIdx;
    if constexpr (sizeof(T) == sizeof(float)) {
        // Get Value, The index of the odd position is obtained for each repeat.
        struct GatherMaskParams reducev2Params(DEFAULT_BLK_STRIDE, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_BLK_STRIDE);
        GatherMask<T>(
            dstValueLocal[dstOffsetFourBytes], tmpLocal[tmpLocalDstOffset], REDUCEV2_MODE_ONE, true,
            tiling.maskVreducev2FourBytes, reducev2Params, rsvdCnt);
    } else {
        int32_t dstOffsetTwoBytes = outterIdx * tiling.kAlignTwoBytes;
        // Get Value. The first element is used for every four elements in each repeat.
        struct GatherMaskParams reducev2Params(DEFAULT_BLK_STRIDE, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_BLK_STRIDE);
        GatherMask<T>(
            dstValueLocal[dstOffsetTwoBytes], tmpLocal[tmpLocalDstOffset], REDUCEV2_MODE_THREE, true,
            tiling.maskVreducev2TwoBytes, reducev2Params, rsvdCnt);
    }
    PipeBarrier<PIPE_V>();
    // Get Index, The index of the even position is obtained for each repeat.
    LocalTensor<float> tempBuffer = dstIndexLocal[dstOffsetFourBytes].template ReinterpretCast<float>();
    LocalTensor<float> tempBufferLocal = tmpLocal[tmpLocalDstOffset].template ReinterpretCast<float>();
    struct GatherMaskParams reducev2Params(DEFAULT_BLK_STRIDE, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_BLK_STRIDE);
    GatherMask<float>(
        tempBuffer, tempBufferLocal, REDUCEV2_MODE_TWO, true, tiling.maskVreducev2FourBytes, reducev2Params, rsvdCnt);
    PipeBarrier<PIPE_V>();
    SetMaskCount();
}

template <typename T, bool isInitIndex>
__aicore__ inline void TmpLocalSort32(
    const LocalTensor<T>& srcLocal, const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<T>& tmpLocal,
    const TopkTiling& tiling, const TopKInfo& topKInfo, const bool isLargest, const int outterIdx,
    const UnaryRepeatParams unaryParams)
{
    // The number of inners is divided into inner/32 groups for sorting. Each iteration completes one group of 32
    // numbers of sorting.
    int offset = outterIdx * topKInfo.inner;
    if (!isLargest) {
        SetVectorMask<T, MaskMode::COUNTER>(0, topKInfo.inner);
        Muls<T, false>(tmpLocal[tiling.innerDataSize], srcLocal[offset], T(-1), MASK_PLACEHOLDER, 1, unaryParams);
        PipeBarrier<PIPE_V>();
        if constexpr (!isInitIndex) {
            LocalTensor<uint32_t> tempBufferUint32 =
                tmpLocal[tiling.srcIndexOffset].template ReinterpretCast<uint32_t>();
            Sort32<T>(tmpLocal, tmpLocal[tiling.innerDataSize], tempBufferUint32, tiling.sortRepeat);
        } else {
            LocalTensor<uint32_t> tempBufferUint32 = srcIndexLocal.template ReinterpretCast<uint32_t>();
            Sort32<T>(tmpLocal, tmpLocal[tiling.innerDataSize], tempBufferUint32, tiling.sortRepeat);
        }
    } else {
        if constexpr (!isInitIndex) {
            LocalTensor<uint32_t> tempBufferUint32 =
                tmpLocal[tiling.srcIndexOffset].template ReinterpretCast<uint32_t>();
            Sort32<T>(tmpLocal, srcLocal[offset], tempBufferUint32, tiling.sortRepeat);
        } else {
            LocalTensor<uint32_t> tempBufferUint32 = srcIndexLocal.template ReinterpretCast<uint32_t>();
            Sort32<T>(tmpLocal, srcLocal[offset], tempBufferUint32, tiling.sortRepeat);
        }
    }
    PipeBarrier<PIPE_V>();
    const DataCopyParams intriParams = {static_cast<uint16_t>(tiling.copyUbToUbBlockCount), 1, 0, 0};
    DataCopy(tmpLocal[tiling.innerDataSize], tmpLocal, intriParams);
    PipeBarrier<PIPE_V>();
}

template <typename T, bool isInitIndex, bool isHasfinish>
__aicore__ inline void TopKCompute(
    const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal,
    const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const LocalTensor<T>& tmpLocal,
    const int32_t k, const TopkTiling& tiling, const TopKInfo& topKInfo, const bool isLargest)
{
    const UnaryRepeatParams unaryParams;
    for (int j = 0; j < topKInfo.outter; ++j) {
        int32_t dstOffsetFourBytes = j * tiling.kAlignFourBytes;
        TmpLocalSort32<T, isInitIndex>(srcLocal, srcIndexLocal, tmpLocal, tiling, topKInfo, isLargest, j, unaryParams);

        int32_t mrgFourQueueCount = 0;
        uint16_t z = MIN_SORT32_SIZE;
        int32_t dstIdx = 0;
        MrgFourQueueSort<T>(tmpLocal, tiling, topKInfo, z, mrgFourQueueCount, dstIdx);
        MrgTwoQueueSort<T>(tmpLocal, tiling, topKInfo, z, mrgFourQueueCount, dstIdx, k);
        GatherDstValAndDstIdx(dstValueLocal, dstIndexLocal, tmpLocal, tiling, dstIdx, dstOffsetFourBytes, j);

        if constexpr (isHasfinish) {
            bool finishValue = finishLocal.GetValue(j);
            auto eventID = GetTPipePtr()->FetchEventID(HardEvent::S_V);
            SetFlag<HardEvent::S_V>(eventID);
            WaitFlag<HardEvent::S_V>(eventID);

            if (finishValue) {
                SetVectorMask<T, MaskMode::COUNTER>(0, k);
                Duplicate<int32_t, false>(
                    dstIndexLocal[dstOffsetFourBytes], static_cast<int32_t>(topKInfo.n), MASK_PLACEHOLDER, 1, 1,
                    DEFAULT_REPEAT_STRIDE);
            }
        }
        PipeBarrier<PIPE_V>();
    }
}

__aicore__ inline void TopKNSmallGetFloatTopKValue(
    const LocalTensor<float>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal,
    const LocalTensor<float>& tmpLocal, const TopkTiling& tiling, const TopKInfo& topKInfo)
{
    LocalTensor<uint32_t> src1stackTensor =
        tmpLocal[tiling.topkMrgSrc1MaskSizeOffset].template ReinterpretCast<uint32_t>();
    auto eventID = GetTPipePtr()->FetchEventID(HardEvent::V_S);
    SetFlag<HardEvent::V_S>(eventID);
    WaitFlag<HardEvent::V_S>(eventID);
    src1stackTensor.SetSize(SRC1_STACK_TENSORSIZE);
    // 0b01010101010101010101010101010101
    src1stackTensor.SetValue(0, tiling.vreduceValMask0);
    // 0b01010101010101010101010101010101
    src1stackTensor.SetValue(1, tiling.vreduceValMask1);

    src1stackTensor.SetValue(EIGHT, tiling.vreduceIdxMask0);
    src1stackTensor.SetValue(NINE, tiling.vreduceIdxMask1);

    eventID = GetTPipePtr()->FetchEventID(HardEvent::S_V);
    SetFlag<HardEvent::S_V>(eventID);
    WaitFlag<HardEvent::S_V>(eventID);
    // Get Value, The index of the odd position is obtained for each repeat.
    struct GatherMaskParams reducev2Params(DEFAULT_BLK_STRIDE, topKInfo.outter, DEFAULT_REPEAT_STRIDE, 0);
    uint64_t rsvdCnt = 0;
    GatherMask<float, uint32_t>(
        dstValueLocal, tmpLocal, src1stackTensor, true, VREDUCEV2_FOUR_BYTE_MASK, reducev2Params, rsvdCnt);

    // Get Index, The index of the even position is obtained for each repeat.
    LocalTensor<float> tempBuffer = dstIndexLocal.template ReinterpretCast<float>();
    struct GatherMaskParams reducev2Params2(DEFAULT_BLK_STRIDE, topKInfo.outter, DEFAULT_REPEAT_STRIDE, 0);
    GatherMask<float, uint32_t>(
        tempBuffer, tmpLocal, tmpLocal[tiling.topkMrgSrc1MaskSizeOffset + EIGHT].ReinterpretCast<uint32_t>(), true,
        VREDUCEV2_FOUR_BYTE_MASK, reducev2Params2, rsvdCnt);
    PipeBarrier<PIPE_V>();
}

__aicore__ inline void TopKNSmallGetHalfTopKValue(
    const LocalTensor<half>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal,
    const LocalTensor<half>& tmpLocal, const TopkTiling& tiling, const TopKInfo& topKInfo)
{
    LocalTensor<uint16_t> src1stackTensor =
        tmpLocal[tiling.topkMrgSrc1MaskSizeOffset].template ReinterpretCast<uint16_t>();
    auto eventID = GetTPipePtr()->FetchEventID(HardEvent::V_S);
    SetFlag<HardEvent::V_S>(eventID);
    WaitFlag<HardEvent::V_S>(eventID);
    src1stackTensor.SetSize(EIGHT);
    src1stackTensor.SetValue(0, tiling.vreducehalfValMask0);
    src1stackTensor.SetValue(1, tiling.vreducehalfValMask1);
    src1stackTensor.SetValue(TWO, tiling.vreducehalfValMask2);
    src1stackTensor.SetValue(THREE, tiling.vreducehalfValMask3);
    src1stackTensor.SetValue(FOUR, tiling.vreducehalfValMask4);
    src1stackTensor.SetValue(FIVE, tiling.vreducehalfValMask5);
    src1stackTensor.SetValue(SIX, tiling.vreducehalfValMask6);
    src1stackTensor.SetValue(SEVEN, tiling.vreducehalfValMask7);
    LocalTensor<uint32_t> indexstackTensor =
        tmpLocal[tiling.topkMrgSrc1MaskSizeOffset + SRC1_STACK_VAL_OFFSET].template ReinterpretCast<uint32_t>();
    ;
    indexstackTensor.SetSize(TWO);
    indexstackTensor.SetValue(0, tiling.vreduceIdxMask0);
    indexstackTensor.SetValue(1, tiling.vreduceIdxMask1);
    eventID = GetTPipePtr()->FetchEventID(HardEvent::S_V);
    SetFlag<HardEvent::S_V>(eventID);
    WaitFlag<HardEvent::S_V>(eventID);
    // Get Value. The first element is used for every four elements in each repeat.
    struct GatherMaskParams reducev2Params(DEFAULT_BLK_STRIDE, topKInfo.outter, DEFAULT_REPEAT_STRIDE, 0);
    uint64_t rsvdCnt = 0;
    GatherMask<half, uint16_t>(
        dstValueLocal, tmpLocal, src1stackTensor, true, VREDUCEV2_HALF_MASK, reducev2Params, rsvdCnt);
    PipeBarrier<PIPE_V>();
    // Get Index, The index of the even position is obtained for each repeat.
    LocalTensor<float> tempBufferIndex = dstIndexLocal.template ReinterpretCast<float>();
    LocalTensor<float> tempBufferLocal = tmpLocal.template ReinterpretCast<float>();
    struct GatherMaskParams reducev2Params2(DEFAULT_BLK_STRIDE, topKInfo.outter, DEFAULT_REPEAT_STRIDE, 0);
    GatherMask<float, uint32_t>(
        tempBufferIndex, tempBufferLocal,
        tempBufferLocal[(tiling.topkMrgSrc1MaskSizeOffset + SRC1_STACK_VAL_OFFSET) / TWO].ReinterpretCast<uint32_t>(),
        true, VREDUCEV2_FOUR_BYTE_MASK, reducev2Params2, rsvdCnt);
    PipeBarrier<PIPE_V>();
}

template <typename T, bool isInitIndex, bool isHasfinish>
__aicore__ inline void TopKNSmallCompute(
    const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal,
    const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const LocalTensor<T>& tmpLocal,
    const int32_t k, const TopkTiling& tiling, const TopKInfo& topKInfo, const bool isLargest)
{
    if (!isLargest) {
        if (!isInitIndex) {
            // Repeat = inner * outter / 32, inner=32
            LocalTensor<uint32_t> tempBufferUint32 =
                tmpLocal[tiling.topkNSmallSrcIndexOffset].template ReinterpretCast<uint32_t>();
            Sort32<T>(tmpLocal, tmpLocal[tiling.innerDataSize], tempBufferUint32, topKInfo.outter);
        } else {
            // Repeat = inner * outter / 32, inner=32
            LocalTensor<uint32_t> tempBufferUint32 = srcIndexLocal.template ReinterpretCast<uint32_t>();
            Sort32<T>(tmpLocal, tmpLocal[tiling.innerDataSize], tempBufferUint32, topKInfo.outter);
        }
    } else {
        if (!isInitIndex) {
            // Repeat = inner * outter / 32, inner=32
            LocalTensor<uint32_t> tempBufferUint32 =
                tmpLocal[tiling.topkNSmallSrcIndexOffset].template ReinterpretCast<uint32_t>();
            Sort32<T>(tmpLocal, srcLocal, tempBufferUint32, topKInfo.outter);
        } else {
            // Repeat = inner * outter / 32, inner=32
            LocalTensor<uint32_t> tempBufferUint32 = srcIndexLocal.template ReinterpretCast<uint32_t>();
            Sort32<T>(tmpLocal, srcLocal, tempBufferUint32, topKInfo.outter);
        }
    }
    PipeBarrier<PIPE_V>();

    if constexpr (sizeof(T) == sizeof(float)) {
        TopKNSmallGetFloatTopKValue(dstValueLocal, dstIndexLocal, tmpLocal, tiling, topKInfo);
    } else {
        TopKNSmallGetHalfTopKValue(dstValueLocal, dstIndexLocal, tmpLocal, tiling, topKInfo);
    }
}

__aicore__ inline void CopyData(LocalTensor<int32_t> indexLocalTmp, const TopKInfo& topKInfo)
{
    Copy(indexLocalTmp[topKInfo.inner], indexLocalTmp, THIRTY_TWO, topKInfo.outter - 1, {1, 1, FOUR, 0});
    PipeBarrier<PIPE_V>();
}

} // namespace AscendC

#endif // IMPL_SORT_TOPK_TOPK_V220_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SORT_TOPK_TOPK_V220_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SORT_TOPK_TOPK_V220_IMPL_H__
#endif
