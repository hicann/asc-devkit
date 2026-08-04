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
 * \file topk_3510_impl.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/sort/topk/topk_3510_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/sort/topk.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SORT_TOPK_TOPK_C310_IMPL_H__
#endif

#ifndef IMPL_SORT_TOPK_TOPK_C310_IMPL_H
#define IMPL_SORT_TOPK_TOPK_C310_IMPL_H
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include "../../../../basic_api/kernel_log.h"
#endif

#include "../../../../../include/basic_api/kernel_tensor.h"
#include "../../../../basic_api/kernel_pop_stack_buffer.h"
#include "topk_common_utils.h"

#include "../sort/sort_impl.h"
#include "../sort_common_utils.h"

namespace AscendC {
#if ASCENDC_CPU_DEBUG
template <
    typename T, bool isInitIndex = false, enum TopKMode topkMode = TopKMode::TOPK_NORMAL,
    const TopKConfig& config = defaultTopKConfig>
void TopkInputCheck(const int32_t k, const TopKInfo& topKInfo)
{
    ASCENDC_ASSERT((1 <= k) && (k <= topKInfo.n), {
        KERNEL_LOG(KERNEL_ERROR, "The value of k must be greater than or equal to 1 and less than or equal to inner.");
    });
    ASCENDC_ASSERT((1 <= topKInfo.n) && (topKInfo.n <= topKInfo.inner), {
        KERNEL_LOG(KERNEL_ERROR, "The value of n must be greater than or equal to 1 and less than or equal to inner.");
    });
    ASCENDC_ASSERT((topKInfo.inner % TOPK_INNER_ALIGN_LEN == 0), {
        KERNEL_LOG(KERNEL_ERROR, "The value of inner must be an integer multiple of 32.");
    });

    if constexpr (config.algo == TopKAlgo::MERGE_SORT) {
        ASCENDC_ASSERT((std::is_same<T, half>::value) || (std::is_same<T, float>::value), {
            KERNEL_LOG(KERNEL_ERROR, "Type must be half or float in merge sort algorithm.");
        });
    }
    if constexpr (topkMode == TopKMode::TOPK_NORMAL) {
        ASCENDC_ASSERT((topKInfo.inner <= TOPK_NORMAL_INNER_MAX_LEN), {
            KERNEL_LOG(KERNEL_ERROR, "The maximum value supported by inner is 4096.");
        });
    }
    if constexpr (topkMode == TopKMode::TOPK_NSMALL) {
        ASCENDC_ASSERT((topKInfo.inner == TOPK_NSMALL_INNER_LEN), {
            KERNEL_LOG(KERNEL_ERROR, "The value of inner must be an equal 32.");
        });
    }
}
#endif

template <typename T>
__ASC_USE_RESERVED_UBUF__(3510, "TopK is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void GatherDstValAndDstIdx(
    const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& tmpLocal,
    const TopkTiling& tiling, const int32_t dstOffsetFourBytes, const int outterIdx)
{
    uint64_t rsvdCnt = 0;
    struct GatherMaskParams reducev2Params(DEFAULT_BLK_STRIDE, 1, DEFAULT_REPEAT_STRIDE, DEFAULT_BLK_STRIDE);
    if constexpr (sizeof(T) == sizeof(float)) {
        // Get Value, The index of the odd position is obtained for each repeat.
        GatherMask<T>(
            dstValueLocal[dstOffsetFourBytes], tmpLocal[tiling.innerDataSize], REDUCEV2_MODE_ONE, true,
            tiling.maskVreducev2FourBytes, reducev2Params, rsvdCnt);
    } else {
        int32_t dstOffsetTwoBytes = outterIdx * tiling.kAlignTwoBytes;
        // Get Value. The first element is used for every four elements in each repeat.
        GatherMask<T>(
            dstValueLocal[dstOffsetTwoBytes], tmpLocal[tiling.innerDataSize], REDUCEV2_MODE_THREE, true,
            tiling.maskVreducev2TwoBytes, reducev2Params, rsvdCnt);
    }
    // Get Index, The index of the even position is obtained for each repeat.
    LocalTensor<int32_t> tempBufferLocal = tmpLocal[tiling.innerDataSize].template ReinterpretCast<int32_t>();
    GatherMask<int32_t>(
        dstIndexLocal[dstOffsetFourBytes], tempBufferLocal, REDUCEV2_MODE_TWO, true, tiling.maskVreducev2FourBytes,
        reducev2Params, rsvdCnt);
}

template <typename T, bool isInitIndex>
__aicore__ inline void TmpLocalSort32(
    const LocalTensor<T>& srcLocal, const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<T>& tmpLocal,
    const TopkTiling& tiling, const TopKInfo& topKInfo, const bool isLargest, const int outterIdx)
{
    int offset = outterIdx * topKInfo.inner;
    LocalTensor<T> tmpBufferLocal = tmpLocal;
    if constexpr (!isInitIndex) {
        LocalTensor<uint32_t> tempBufferUint32 = tmpLocal[tiling.srcIndexOffset].template ReinterpretCast<uint32_t>();
        Sort<T, true>(
            tmpLocal[tiling.innerDataSize], srcLocal[offset], tempBufferUint32, tmpBufferLocal, tiling.sortRepeat);
    } else {
        LocalTensor<uint32_t> tempBufferUint32 = srcIndexLocal.template ReinterpretCast<uint32_t>();
        Sort<T, true>(
            tmpLocal[tiling.innerDataSize], srcLocal[offset], tempBufferUint32, tmpBufferLocal, tiling.sortRepeat);
    }
}

template <typename T, bool isInitIndex, bool isHasfinish>
__ASC_USE_RESERVED_UBUF__(3510, "TopK is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void TopKCompute(
    const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal,
    const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const LocalTensor<T>& tmpLocal,
    const int32_t k, const TopkTiling& tiling, const TopKInfo& topKInfo, const bool isLargest)
{
    for (int j = 0; j < topKInfo.outter; ++j) {
        int32_t dstOffsetFourBytes = j * tiling.kAlignFourBytes;
        TmpLocalSort32<T, isInitIndex>(srcLocal, srcIndexLocal, tmpLocal, tiling, topKInfo, isLargest, j);

        GatherDstValAndDstIdx(dstValueLocal, dstIndexLocal, tmpLocal, tiling, dstOffsetFourBytes, j);

        if constexpr (isHasfinish) {
            bool finishValue = finishLocal.GetValue(j);
            auto eventID = GetTPipePtr()->FetchEventID(HardEvent::S_V);
            SetFlag<HardEvent::S_V>(eventID);
            WaitFlag<HardEvent::S_V>(eventID);

            if (finishValue) {
                Duplicate(dstIndexLocal[dstOffsetFourBytes], static_cast<int32_t>(topKInfo.n), k);
            }
        }
    }
}

template <typename T>
__ASC_USE_RESERVED_UBUF__(3510, "TopK is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void TopKNSmallGetTopKValue(
    const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& tmpLocal,
    const int32_t k, const TopkTiling& tiling, const TopKInfo& topKInfo)
{
    uint64_t rsvdCnt = 0;
    struct GatherMaskParams reducev2Params(DEFAULT_BLK_STRIDE, topKInfo.outter, DEFAULT_REPEAT_STRIDE, 0);
    if constexpr (sizeof(T) == sizeof(float)) {
        // Get Value, The index of the odd position is obtained for each repeat.
        GatherMask<T>(dstValueLocal, tmpLocal, REDUCEV2_MODE_ONE, true, 2 * k, reducev2Params, rsvdCnt);
    } else {
        // Get Value. The first element is used for every four elements in each repeat.
        GatherMask<T>(dstValueLocal, tmpLocal, REDUCEV2_MODE_THREE, true, 4 * k, reducev2Params, rsvdCnt);
    }
    // Get Index, The index of the even position is obtained for each repeat.
    LocalTensor<int32_t> tempBufferLocal = tmpLocal.template ReinterpretCast<int32_t>();
    GatherMask<int32_t>(dstIndexLocal, tempBufferLocal, REDUCEV2_MODE_TWO, true, 2 * k, reducev2Params, rsvdCnt);
}

template <typename T, bool isInitIndex, bool isHasfinish>
__aicore__ inline void TopKNSmallCompute(
    const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal,
    const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const LocalTensor<T>& tmpLocal,
    const int32_t k, const TopkTiling& tiling, const TopKInfo& topKInfo, const bool isLargest)
{
    if constexpr (!isInitIndex) {
        LocalTensor<uint32_t> tempBufferUint32 = tmpLocal[tiling.innerDataSize].template ReinterpretCast<uint32_t>();
        Sort32<T>(tmpLocal, srcLocal, tempBufferUint32, topKInfo.outter);
    } else {
        LocalTensor<uint32_t> tempBufferUint32 = srcIndexLocal.template ReinterpretCast<uint32_t>();
        Sort32<T>(tmpLocal, srcLocal, tempBufferUint32, topKInfo.outter);
    }

    TopKNSmallGetTopKValue(dstValueLocal, dstIndexLocal, tmpLocal, k, tiling, topKInfo);
}

namespace Reg {
namespace RadixSelectTopK {

constexpr uint32_t LOAD_NUMS_PER_ROUND = 256;
constexpr uint32_t BUCKET_BYTES = 512;

template <typename T>
__aicore__ inline constexpr bool IsFloatNum()
{
    return SupportType<T, float, half, bfloat16_t>();
}

__aicore__ inline constexpr bool NeedReverse(bool isLargest) { return !isLargest; }

template <bool isLargest>
__aicore__ inline constexpr bool NeedReverse()
{
    return !isLargest;
}

template <typename T>
__aicore__ inline constexpr bool NeedPreProcess(bool isLargest)
{
    return NeedReverse(isLargest) || Internal::IsNeedTwiddleType<T>();
}

template <typename T, bool isLargest>
__aicore__ inline constexpr bool NeedPreProcess()
{
    return NeedReverse<isLargest>() || Internal::IsNeedTwiddleType<T>();
}

template <typename T, bool isReuseSrc, bool isLargest>
__aicore__ inline void InitializeTempBuffer(
    const LocalTensor<T>& tempBuffer, const uint32_t alignCount, __ubuf__ T*& tmpSrcData,
    __ubuf__ int32_t*& tmpSrcIndex, __ubuf__ uint16_t*& tmpHistData, __ubuf__ T*& realWorkData,
    __ubuf__ T*& sortTmpBuffer)
{
    __ubuf__ uint8_t* tmp = (__ubuf__ uint8_t*)tempBuffer.GetPhyAddr();
    tmpSrcData = (__ubuf__ T*)tmp;

    if constexpr (sizeof(T) == 8) {
        uint32_t srcOffset = 0;
        if (alignCount < LOAD_NUMS_PER_ROUND) {
            srcOffset += sizeof(T) * LOAD_NUMS_PER_ROUND;
        } else {
            srcOffset += sizeof(T) * alignCount;
        }

        tmpSrcIndex = (__ubuf__ int32_t*)((__ubuf__ uint8_t*)tmpSrcData + srcOffset);
    } else {
        tmpSrcIndex = (__ubuf__ int32_t*)((__ubuf__ uint8_t*)tmpSrcData + sizeof(T) * alignCount);
    }

    tmpHistData = (__ubuf__ uint16_t*)(tmpSrcIndex);

    if constexpr (NeedPreProcess<T, isLargest>()) {
        if constexpr (!isReuseSrc) {
            uint32_t indexSpace = alignCount * sizeof(int32_t);
            if (indexSpace < BUCKET_BYTES) {
                indexSpace = BUCKET_BYTES;
            }
            realWorkData = (__ubuf__ T*)((__ubuf__ uint8_t*)tmpSrcIndex + indexSpace);
            sortTmpBuffer = realWorkData;
        } else {
            sortTmpBuffer = (__ubuf__ T*)((__ubuf__ uint8_t*)tmpSrcIndex + sizeof(int32_t) * alignCount);
        }
    } else {
        sortTmpBuffer = (__ubuf__ T*)((__ubuf__ uint8_t*)tmpSrcIndex + sizeof(int32_t) * alignCount);
    }
}

template <typename T, bool isReuseSrc>
__aicore__ inline void InitializeTempBuffer(
    const LocalTensor<T>& tempBuffer, bool isLargest, const uint32_t alignCount, __ubuf__ T*& tmpSrcData,
    __ubuf__ int32_t*& tmpSrcIndex, __ubuf__ uint16_t*& tmpHistData, __ubuf__ T*& realWorkData,
    __ubuf__ T*& sortTmpBuffer)
{
    __ubuf__ uint8_t* tmp = (__ubuf__ uint8_t*)tempBuffer.GetPhyAddr();
    tmpSrcData = (__ubuf__ T*)tmp;

    if constexpr (sizeof(T) == 8) {
        uint32_t srcOffset = 0;
        if (alignCount < LOAD_NUMS_PER_ROUND) {
            srcOffset += sizeof(T) * LOAD_NUMS_PER_ROUND;
        } else {
            srcOffset += sizeof(T) * alignCount;
        }
        tmpSrcIndex = (__ubuf__ int32_t*)((__ubuf__ uint8_t*)tmpSrcData + srcOffset);
    } else {
        tmpSrcIndex = (__ubuf__ int32_t*)((__ubuf__ uint8_t*)tmpSrcData + sizeof(T) * alignCount);
    }

    tmpHistData = (__ubuf__ uint16_t*)(tmpSrcIndex);

    if (NeedPreProcess<T>(isLargest)) {
        if constexpr (!isReuseSrc) {
            uint32_t indexSpace = alignCount * sizeof(int32_t);
            if (indexSpace < BUCKET_BYTES) {
                indexSpace = BUCKET_BYTES;
            }
            realWorkData = (__ubuf__ T*)((__ubuf__ uint8_t*)tmpSrcIndex + indexSpace);
            sortTmpBuffer = realWorkData;
        } else {
            sortTmpBuffer = (__ubuf__ T*)((__ubuf__ uint8_t*)tmpSrcIndex + sizeof(int32_t) * alignCount);
        }
    } else {
        sortTmpBuffer = (__ubuf__ T*)((__ubuf__ uint8_t*)tmpSrcIndex + sizeof(int32_t) * alignCount);
    }
}

__simd_callee__ inline void GetLowestByte(RegTensor<uint8_t>& dst, RegTensor<uint16_t>& src0, RegTensor<uint16_t>& src1)
{
    RegTensor<uint8_t> tmpU8Reg;
    DeInterleave(dst, tmpU8Reg, (RegTensor<uint8_t>&)src0, (RegTensor<uint8_t>&)src1);
}

__simd_callee__ inline void GetLowestByte(
    RegTensor<uint8_t>& dst, RegTensor<uint32_t>& src0, RegTensor<uint32_t>& src1, RegTensor<uint32_t>& src2,
    RegTensor<uint32_t>& src3)
{
    RegTensor<uint16_t> tmpU16Reg0, tmpU16Reg1, tmpU16Reg2;
    DeInterleave(tmpU16Reg0, tmpU16Reg1, (RegTensor<uint16_t>&)src0, (RegTensor<uint16_t>&)src1);
    DeInterleave(tmpU16Reg2, tmpU16Reg1, (RegTensor<uint16_t>&)src2, (RegTensor<uint16_t>&)src3);

    GetLowestByte(dst, tmpU16Reg0, tmpU16Reg2);
}

__simd_callee__ inline void TransToB8Mask(MaskReg& dst, MaskReg& u16Src0, MaskReg& u16Src1)
{
    MaskReg tmpU8Mask;
    MaskDeInterleave<uint8_t>(dst, tmpU8Mask, u16Src0, u16Src1);
}

__simd_callee__ inline void TransToB8Mask(
    MaskReg& dst, MaskReg& u32Src0, MaskReg& u32Src1, MaskReg& u32Src2, MaskReg& u32Src3)
{
    MaskReg tmpU16LowPart0, tmpU16LowPart1, tmpU16LowPart2;
    MaskDeInterleave<uint16_t>(tmpU16LowPart0, tmpU16LowPart1, u32Src0, u32Src1);
    MaskDeInterleave<uint16_t>(tmpU16LowPart2, tmpU16LowPart1, u32Src2, u32Src3);

    TransToB8Mask(dst, tmpU16LowPart0, tmpU16LowPart2);
}

__simd_callee__ inline void CollectGivenPosByte(
    RegTensor<uint8_t>& colWorkBits, __ubuf__ uint64_t* src, uint16_t byteNum, const int32_t srcOffset,
    __ubuf__ uint64_t* tmpLocal)
{
    uint32_t loadCount = GetVecLen() / sizeof(uint32_t);
    uint32_t realCount = loadCount / 2;

    int16_t offsets = static_cast<int16_t>((byteNum - 1) * 8);
    __ubuf__ uint32_t* srcU32 = (__ubuf__ uint32_t*)src;
    __ubuf__ uint32_t* tmpU32 = (__ubuf__ uint32_t*)tmpLocal;

    MaskReg fullMask = CreateMask<uint32_t>();
    MaskReg zeroMask = CreateMask<uint32_t, MaskPattern::ALLF>();
    MaskReg halfMask = CreateMask<uint32_t, MaskPattern::H>();

    constexpr uint16_t repeatTimes = sizeof(uint64_t);
    MaskReg lowMask, highMask, tmpMask;
    MaskInterleave<uint32_t>(lowMask, tmpMask, fullMask, zeroMask);
    MaskInterleave<uint32_t>(highMask, tmpMask, zeroMask, fullMask);

    int16_t highOffsets = offsets - 32;
    if (offsets >= 32) {
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            RegTensor<uint32_t> input, output;
            Reg::LoadAlign(input, srcU32 + srcOffset + i * loadCount);
            GatherMask(output, input, highMask);
            ShiftRights(output, output, highOffsets, halfMask);

            Reg::StoreAlign(tmpU32 + realCount * i, output, halfMask);
        }
    } else {
        for (uint16_t i = 0; i < repeatTimes; ++i) {
            RegTensor<uint32_t> input, output;
            Reg::LoadAlign(input, srcU32 + srcOffset + i * loadCount);
            GatherMask(output, input, lowMask);
            ShiftRights(output, output, offsets, halfMask);

            Reg::StoreAlign(tmpU32 + realCount * i, output, halfMask);
        }
    }

    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    RegTensor<uint32_t> work0, work1, work2, work3;
    Reg::LoadAlign(work0, tmpU32);
    Reg::LoadAlign(work1, tmpU32 + loadCount);
    Reg::LoadAlign(work2, tmpU32 + loadCount * 2);
    Reg::LoadAlign(work3, tmpU32 + loadCount * 3);

    GetLowestByte(colWorkBits, work0, work1, work2, work3);
}

__simd_callee__ inline void CompareHighBytesBeforePos(
    MaskReg& filterMask, __ubuf__ uint64_t*& src, MaskReg& maskReg, uint64_t value, uint16_t byteNum, int32_t srcOffset,
    __ubuf__ uint64_t* tmpLocal)
{
    __ubuf__ uint32_t* srcU32 = (__ubuf__ uint32_t*)src;
    __ubuf__ uint32_t* tmpU32 = (__ubuf__ uint32_t*)tmpLocal;

    uint32_t loadCount = GetVecLen() / sizeof(uint32_t);
    uint32_t realCount = loadCount / 2;
    int16_t maskOffsets = static_cast<int16_t>(byteNum * 8);

    MaskReg fullMask = CreateMask<uint32_t>();
    MaskReg zeroMask = CreateMask<uint32_t, MaskPattern::ALLF>();
    MaskReg halfMask = CreateMask<uint32_t, MaskPattern::H>();

    constexpr uint16_t repeatTimes = sizeof(uint64_t);
    MaskReg lowMask, highMask, tmpMask;
    MaskInterleave<uint32_t>(lowMask, tmpMask, fullMask, zeroMask);
    MaskInterleave<uint32_t>(highMask, tmpMask, zeroMask, fullMask);

    int16_t realMaskOffset = maskOffsets - 32;
    int16_t reverseMaskOffset = 32 - maskOffsets;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        RegTensor<uint32_t> input, highOut, tHighOut, lowOut;
        Reg::LoadAlign(input, srcU32 + srcOffset + i * loadCount);

        GatherMask(highOut, input, highMask);

        if (maskOffsets > 32) {
            ShiftRights(lowOut, highOut, realMaskOffset, halfMask);
            Duplicate(highOut, 0, halfMask);
        } else {
            GatherMask(lowOut, input, lowMask);
            ShiftRights(lowOut, lowOut, maskOffsets, halfMask);
            ShiftLefts(tHighOut, highOut, reverseMaskOffset, halfMask);
            Or(lowOut, lowOut, tHighOut, halfMask);
            ShiftRights(highOut, highOut, maskOffsets, halfMask);
        }

        Reg::StoreAlign(tmpU32 + realCount * i, highOut, halfMask);
        Reg::StoreAlign(tmpU32 + realCount * (i + repeatTimes), lowOut, halfMask);
    }
    Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
    RegTensor<uint32_t> highPart0, highPart1, highPart2, highPart3, lowPart0, lowPart1, lowPart2, lowPart3;
    Reg::LoadAlign(highPart0, tmpU32);
    Reg::LoadAlign(highPart1, tmpU32 + loadCount);
    Reg::LoadAlign(highPart2, tmpU32 + loadCount * 2);
    Reg::LoadAlign(highPart3, tmpU32 + loadCount * 3);
    Reg::LoadAlign(lowPart0, tmpU32 + loadCount * 4);
    Reg::LoadAlign(lowPart1, tmpU32 + loadCount * 5);
    Reg::LoadAlign(lowPart2, tmpU32 + loadCount * 6);
    Reg::LoadAlign(lowPart3, tmpU32 + loadCount * 7);

    MaskReg highMask0, highMask1, highMask2, highMask3;
    MaskReg lowMask0, lowMask1, lowMask2, lowMask3;
    uint32_t highValue = value >> 32;
    CompareScalar<uint32_t, CMPMODE::EQ>(highMask0, highPart0, highValue, fullMask);
    CompareScalar<uint32_t, CMPMODE::EQ>(highMask1, highPart1, highValue, fullMask);
    CompareScalar<uint32_t, CMPMODE::EQ>(highMask2, highPart2, highValue, fullMask);
    CompareScalar<uint32_t, CMPMODE::EQ>(highMask3, highPart3, highValue, fullMask);

    uint32_t lowValue = value & 0xffffffff;
    CompareScalar<uint32_t, CMPMODE::EQ>(lowMask0, lowPart0, lowValue, fullMask);
    CompareScalar<uint32_t, CMPMODE::EQ>(lowMask1, lowPart1, lowValue, fullMask);
    CompareScalar<uint32_t, CMPMODE::EQ>(lowMask2, lowPart2, lowValue, fullMask);
    CompareScalar<uint32_t, CMPMODE::EQ>(lowMask3, lowPart3, lowValue, fullMask);

    MaskReg highRes, lowRes;
    TransToB8Mask(lowRes, lowMask0, lowMask1, lowMask2, lowMask3);
    TransToB8Mask(highRes, highMask0, highMask1, highMask2, highMask3);

    MaskReg res;
    MaskAnd(res, highRes, lowRes, maskReg);
    MaskAnd(filterMask, maskReg, res, maskReg);
}

__simd_callee__ inline void FilterDataAndGivenByteFromOri(
    MaskReg& filterMask, RegTensor<uint8_t>& colWorkBits, __ubuf__ uint64_t* src, MaskReg& maskReg, uint64_t value,
    uint16_t byteNum, int32_t srcOffset, __ubuf__ uint64_t* tmpLocal)
{
    CollectGivenPosByte(colWorkBits, src, byteNum, srcOffset * 2, tmpLocal);
    CompareHighBytesBeforePos(filterMask, src, maskReg, value, byteNum, srcOffset * 2, tmpLocal);
}

__simd_callee__ inline void FilterDataAndGivenByteFromOri(
    MaskReg& filterMask, RegTensor<uint8_t>& colWorkBits, __ubuf__ uint32_t* src, MaskReg& maskReg, uint32_t value,
    uint16_t byteNum, int32_t srcOffset, __ubuf__ uint32_t* tmpLocal)
{
    constexpr uint32_t eleCountPerVL = GetVecLen() / sizeof(uint32_t);
    MaskReg fullMask = CreateMask<uint32_t>();
    int16_t byteOffsets = static_cast<int16_t>((byteNum - 1) * 8);
    int16_t maskOffsets = static_cast<int16_t>(byteNum * 8);

    RegTensor<uint32_t> input0, input1, input2, input3;
    Reg::LoadAlign(input0, src + srcOffset);
    Reg::LoadAlign(input1, src + srcOffset + eleCountPerVL);
    Reg::LoadAlign(input2, src + srcOffset + eleCountPerVL * 2);
    Reg::LoadAlign(input3, src + srcOffset + eleCountPerVL * 3);

    RegTensor<uint32_t> tmpU32ByteReg0, tmpU32ByteReg1, tmpU32ByteReg2, tmpU32ByteReg3;
    ShiftRights(tmpU32ByteReg0, input0, byteOffsets, fullMask);
    ShiftRights(tmpU32ByteReg1, input1, byteOffsets, fullMask);
    ShiftRights(tmpU32ByteReg2, input2, byteOffsets, fullMask);
    ShiftRights(tmpU32ByteReg3, input3, byteOffsets, fullMask);

    GetLowestByte(colWorkBits, tmpU32ByteReg0, tmpU32ByteReg1, tmpU32ByteReg2, tmpU32ByteReg3);

    RegTensor<uint32_t> tmpU32MaskReg0, tmpU32MaskReg1, tmpU32MaskReg2, tmpU32MaskReg3;
    ShiftRights(tmpU32MaskReg0, input0, maskOffsets, fullMask);
    ShiftRights(tmpU32MaskReg1, input1, maskOffsets, fullMask);
    ShiftRights(tmpU32MaskReg2, input2, maskOffsets, fullMask);
    ShiftRights(tmpU32MaskReg3, input3, maskOffsets, fullMask);

    MaskReg mask0, mask1, mask2, mask3;
    CompareScalar<uint32_t, CMPMODE::EQ>(mask0, tmpU32MaskReg0, value, fullMask);
    CompareScalar<uint32_t, CMPMODE::EQ>(mask1, tmpU32MaskReg1, value, fullMask);
    CompareScalar<uint32_t, CMPMODE::EQ>(mask2, tmpU32MaskReg2, value, fullMask);
    CompareScalar<uint32_t, CMPMODE::EQ>(mask3, tmpU32MaskReg3, value, fullMask);

    MaskReg res;
    TransToB8Mask(res, mask0, mask1, mask2, mask3);
    MaskAnd(filterMask, maskReg, res, maskReg);
}

__simd_callee__ inline void FilterDataAndGivenByteFromOri(
    MaskReg& filterMask, RegTensor<uint8_t>& colWorkBits, __ubuf__ uint16_t* src, MaskReg& maskReg, uint16_t value,
    uint16_t byteNum, int32_t srcOffset, __ubuf__ uint16_t* tmpLocal)
{
    constexpr uint32_t eleCountPerVL = GetVecLen() / sizeof(uint16_t);
    int16_t byteOffsets = static_cast<int16_t>((byteNum - 1) * 8);
    int16_t maskOffsets = static_cast<int16_t>(byteNum * 8);

    MaskReg fullMask = CreateMask<uint16_t>();

    RegTensor<uint16_t> input0, input1;
    Reg::LoadAlign(input0, src + srcOffset);
    Reg::LoadAlign(input1, src + srcOffset + eleCountPerVL);

    RegTensor<uint16_t> tmpBShift0, tmpBShift1;
    ShiftRights(tmpBShift0, input0, byteOffsets, fullMask);
    ShiftRights(tmpBShift1, input1, byteOffsets, fullMask);

    GetLowestByte(colWorkBits, tmpBShift0, tmpBShift1);

    RegTensor<uint16_t> tmpMShift0, tmpMShift1;
    ShiftRights(tmpMShift0, input0, maskOffsets, fullMask);
    ShiftRights(tmpMShift1, input1, maskOffsets, fullMask);

    MaskReg mask0, mask1;
    CompareScalar<uint16_t, CMPMODE::EQ>(mask0, tmpMShift0, value, fullMask);
    CompareScalar<uint16_t, CMPMODE::EQ>(mask1, tmpMShift1, value, fullMask);

    MaskReg res;
    TransToB8Mask(res, mask0, mask1);
    MaskAnd(filterMask, maskReg, res, maskReg);
}

__simd_callee__ inline void FilterDataAndGivenByteFromOri(
    MaskReg& filterMask, RegTensor<uint8_t>& colWorkBits, __ubuf__ uint8_t* src, MaskReg& maskReg, uint8_t value,
    uint16_t byteNum, int32_t srcOffset, __ubuf__ uint8_t* tmpLocal)
{
    Reg::LoadAlign(colWorkBits, src + srcOffset);
    MaskReg fullMask = CreateMask<uint8_t>();
    MaskAnd(filterMask, maskReg, fullMask, maskReg);
}

template <typename T>
__simd_vf__ inline void GenerateAccumulateData(
    __ubuf__ T* src, __ubuf__ uint16_t* hist, __ubuf__ T* tmpSrcData, uint32_t count, T value, uint32_t byteNum)
{
    using ConvType = typename AscendC::Internal::ExtractTypeBySize<sizeof(T)>::T;
    auto unsignedValue = static_cast<ConvType>(value);
    uint16_t repeatTimes = DivCeil(count, GetVecLen());
    ConvType workingUnsignedValue = 0;
    if (byteNum != sizeof(T)) {
        workingUnsignedValue = unsignedValue >> (byteNum * 8);
    }

    RegTensor<uint16_t> acumHistLow, acumHistHigh;
    MaskReg b16FullMask = CreateMask<uint16_t>();

    Duplicate(acumHistLow, 0, b16FullMask);
    Duplicate(acumHistHigh, 0, b16FullMask);

    for (uint16_t i = 0; i < repeatTimes; ++i) {
        MaskReg maskReg = UpdateMask<uint8_t>(count);

        MaskReg filterMask;
        RegTensor<uint8_t> colWorkBits;
        FilterDataAndGivenByteFromOri(
            filterMask, colWorkBits, src, maskReg, workingUnsignedValue, byteNum, i * GetVecLen(), tmpSrcData);

        Histograms<uint8_t, uint16_t, HistogramsBinType::BIN0, HistogramsType::ACCUMULATE>(
            acumHistLow, colWorkBits, filterMask);
        Histograms<uint8_t, uint16_t, HistogramsBinType::BIN1, HistogramsType::ACCUMULATE>(
            acumHistHigh, colWorkBits, filterMask);
    }

    Reg::StoreAlign((__ubuf__ uint16_t*&)hist, acumHistLow, b16FullMask);
    Reg::StoreAlign((__ubuf__ uint16_t*&)hist + GetVecLen() / sizeof(uint16_t), acumHistHigh, b16FullMask);
}

__simd_vf__ inline void GatherGreaterAndEqualKData(
    __ubuf__ uint64_t* src, __ubuf__ uint64_t* dst, const uint64_t value, uint32_t count)
{
    count *= 2;
    constexpr uint16_t eleCountPerVL = GetVecLen() / sizeof(uint32_t);
    uint16_t repeatTimes = DivCeil(count, eleCountPerVL);

    auto firstCount = count;
    auto secondCount = count;

    uint64_t lowValue = value & 0xffffffff;
    uint64_t highValue = value >> 32;

    __ubuf__ uint32_t* u32Src = (__ubuf__ uint32_t*)src;
    __ubuf__ uint32_t* u32Dst = (__ubuf__ uint32_t*)dst;

    ClearSpr<SpecialPurposeReg::AR>();
    MaskReg fullMask = CreateMask<uint32_t>();
    MaskReg zeroMask = CreateMask<uint32_t, MaskPattern::ALLF>();
    MaskReg halfMask = CreateMask<uint32_t, MaskPattern::H>();

    MaskReg lowMask, highMask, tmpMask;
    MaskInterleave<uint32_t>(lowMask, tmpMask, fullMask, zeroMask);
    MaskInterleave<uint32_t>(highMask, tmpMask, zeroMask, fullMask);

    UnalignReg unalignReg;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        MaskReg maskReg = UpdateMask<uint32_t>(firstCount);

        RegTensor<uint32_t> in32Data;
        Reg::LoadAlign(in32Data, u32Src + i * eleCountPerVL);

        RegTensor<uint32_t> highPart, lowPart;
        GatherMask(lowPart, in32Data, lowMask);
        GatherMask(highPart, in32Data, highMask);

        MaskReg highGTMask, highEQMask, lowPartMask;
        CompareScalar<uint32_t, CMPMODE::GT>(highGTMask, highPart, highValue, halfMask);
        CompareScalar<uint32_t, CMPMODE::EQ>(highEQMask, highPart, highValue, halfMask);
        CompareScalar<uint32_t, CMPMODE::GT>(lowPartMask, lowPart, lowValue, halfMask);

        MaskReg cmpMask;
        MaskAnd(cmpMask, highEQMask, lowPartMask, halfMask);
        MaskOr(cmpMask, highGTMask, cmpMask, halfMask);

        MaskReg cmpResMask0, cmpResMask1;
        MaskInterleave<uint32_t>(cmpResMask0, cmpResMask1, cmpMask, cmpMask);
        MaskAnd(cmpResMask0, cmpResMask0, maskReg, maskReg);

        RegTensor<uint32_t> out32Data;
        GatherMask<uint32_t, GatherMaskMode::STORE_REG>(out32Data, in32Data, cmpResMask0);
        Reg::StoreUnAlign<uint32_t>(u32Dst, out32Data, unalignReg);
    }

    for (uint16_t i = 0; i < repeatTimes; ++i) {
        MaskReg maskReg = UpdateMask<uint32_t>(secondCount);

        RegTensor<uint32_t> in32Data;
        Reg::LoadAlign(in32Data, u32Src + i * eleCountPerVL);

        RegTensor<uint32_t> highPart, lowPart;
        GatherMask(lowPart, in32Data, lowMask);
        GatherMask(highPart, in32Data, highMask);

        MaskReg highEQMask, lowPartMask;
        CompareScalar<uint32_t, CMPMODE::EQ>(highEQMask, highPart, highValue, halfMask);
        CompareScalar<uint32_t, CMPMODE::EQ>(lowPartMask, lowPart, lowValue, halfMask);

        MaskReg cmpMask;
        MaskAnd(cmpMask, highEQMask, lowPartMask, halfMask);

        MaskReg cmpResMask0, cmpResMask1;
        MaskInterleave<uint32_t>(cmpResMask0, cmpResMask1, cmpMask, cmpMask);
        MaskAnd(cmpResMask0, cmpResMask0, maskReg, maskReg);

        RegTensor<uint32_t> out32Data;
        GatherMask<uint32_t, GatherMaskMode::STORE_REG>(out32Data, in32Data, cmpResMask0);
        Reg::StoreUnAlign<uint32_t>(u32Dst, out32Data, unalignReg);
    }

    Reg::StoreUnAlignPost(u32Dst, unalignReg);
    ClearSpr<SpecialPurposeReg::AR>();
}

__simd_vf__ inline void GatherGreaterAndEqualKData(
    __ubuf__ uint32_t* src, __ubuf__ uint32_t* dst, const uint32_t value, uint32_t count)
{
    constexpr uint16_t eleCountPerVL = GetVecLen() / sizeof(uint32_t);
    uint16_t repeatTimes = DivCeil(count, eleCountPerVL);

    auto firstCount = count;
    auto secondCount = count;

    ClearSpr<SpecialPurposeReg::AR>();
    UnalignReg unalignReg;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        MaskReg maskReg = UpdateMask<uint32_t>(firstCount);

        RegTensor<uint32_t> in32Data;
        Reg::LoadAlign(in32Data, src + i * eleCountPerVL);

        MaskReg out32Mask;
        CompareScalar<uint32_t, CMPMODE::GT>(out32Mask, in32Data, value, maskReg);

        RegTensor<uint32_t> out32Data;
        GatherMask<uint32_t, GatherMaskMode::STORE_REG>(out32Data, in32Data, out32Mask);
        Reg::StoreUnAlign(dst, out32Data, unalignReg);
    }

    for (uint16_t i = 0; i < repeatTimes; ++i) {
        MaskReg maskReg = UpdateMask<uint32_t>(secondCount);

        RegTensor<uint32_t> in32Data;
        Reg::LoadAlign(in32Data, src + i * eleCountPerVL);

        MaskReg out32Mask;
        CompareScalar<uint32_t, CMPMODE::EQ>(out32Mask, in32Data, value, maskReg);

        RegTensor<uint32_t> out32Data;
        GatherMask<uint32_t, GatherMaskMode::STORE_REG>(out32Data, in32Data, out32Mask);
        Reg::StoreUnAlign(dst, out32Data, unalignReg);
    }

    Reg::StoreUnAlignPost(dst, unalignReg);
    ClearSpr<SpecialPurposeReg::AR>();
}

__simd_vf__ inline void GatherGreaterAndEqualKData(
    __ubuf__ uint16_t* src, __ubuf__ uint16_t* dst, const uint16_t value, uint32_t count)
{
    constexpr uint16_t eleCountPerVL = GetVecLen() / sizeof(uint16_t);
    uint16_t repeatTimes = DivCeil(count, eleCountPerVL);

    auto firstCount = count;
    auto secondCount = count;

    ClearSpr<SpecialPurposeReg::AR>();
    UnalignReg unalignReg0;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        MaskReg maskReg = UpdateMask<uint16_t>(firstCount);

        RegTensor<uint16_t> in32Data;
        Reg::LoadAlign(in32Data, src + i * eleCountPerVL);

        MaskReg out32Mask;
        CompareScalar<uint16_t, CMPMODE::GT>(out32Mask, in32Data, value, maskReg);

        RegTensor<uint16_t> out32Data;
        GatherMask<uint16_t, GatherMaskMode::STORE_REG>(out32Data, in32Data, out32Mask);
        Reg::StoreUnAlign(dst, out32Data, unalignReg0);
    }

    for (uint16_t i = 0; i < repeatTimes; ++i) {
        MaskReg maskReg = UpdateMask<uint16_t>(secondCount);

        RegTensor<uint16_t> in32Data;
        Reg::LoadAlign(in32Data, src + i * eleCountPerVL);

        MaskReg out32Mask;
        CompareScalar<uint16_t, CMPMODE::EQ>(out32Mask, in32Data, value, maskReg);

        RegTensor<uint16_t> out32Data;
        GatherMask<uint16_t, GatherMaskMode::STORE_REG>(out32Data, in32Data, out32Mask);
        Reg::StoreUnAlign(dst, out32Data, unalignReg0);
    }

    Reg::StoreUnAlignPost(dst, unalignReg0);
    ClearSpr<SpecialPurposeReg::AR>();
}

__simd_vf__ inline void GatherGreaterAndEqualKData(
    __ubuf__ uint8_t* src, __ubuf__ uint8_t* dst, const uint8_t value, uint32_t count)
{
    constexpr uint16_t eleCountPerVL = GetVecLen() / sizeof(uint8_t);
    uint16_t repeatTimes = DivCeil(count, eleCountPerVL);

    auto firstCount = count;
    auto secondCount = count;

    ClearSpr<SpecialPurposeReg::AR>();
    UnalignReg unalignReg;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        MaskReg fullMask = UpdateMask<uint8_t>(firstCount);
        RegTensor<uint8_t> in8Data;
        Reg::LoadAlign(in8Data, src + i * eleCountPerVL);

        MaskReg out8Mask;
        CompareScalar<uint8_t, CMPMODE::GT>(out8Mask, in8Data, value, fullMask);

        RegTensor<uint8_t> out8Data;
        GatherMask<uint8_t, GatherMaskMode::STORE_REG>(out8Data, in8Data, out8Mask);
        Reg::StoreUnAlign(dst, out8Data, unalignReg);
    }

    for (uint16_t i = 0; i < repeatTimes; ++i) {
        MaskReg fullMask = UpdateMask<uint8_t>(secondCount);
        RegTensor<uint8_t> in8Data;
        Reg::LoadAlign(in8Data, src + i * eleCountPerVL);

        MaskReg out8Mask;
        CompareScalar<uint8_t, CMPMODE::EQ>(out8Mask, in8Data, value, fullMask);

        RegTensor<uint8_t> out8Data;
        GatherMask<uint8_t, GatherMaskMode::STORE_REG>(out8Data, in8Data, out8Mask);
        Reg::StoreUnAlign(dst, out8Data, unalignReg);
    }

    Reg::StoreUnAlignPost(dst, unalignReg);
    ClearSpr<SpecialPurposeReg::AR>();
}

__simd_vf__ inline void GatherGreaterAndEqualKIndex(
    __ubuf__ uint64_t* src, __ubuf__ int32_t* inputIndex, __ubuf__ int32_t* dstIndex, const uint64_t value,
    uint32_t count)
{
    count *= 2;

    constexpr uint16_t eleCountPerVL = GetVecLen() / sizeof(uint32_t);
    uint16_t repeatTimes = DivCeil(count, eleCountPerVL);
    constexpr uint16_t numsPerRound = eleCountPerVL / 2;

    auto firstCount = count;
    auto secondCount = count;

    uint64_t lowValue = value & 0xffffffff;
    uint64_t highValue = value >> 32;

    __ubuf__ uint32_t* u32Src = (__ubuf__ uint32_t*)src;

    MaskReg fullMask = CreateMask<uint32_t>();
    MaskReg zeroMask = CreateMask<uint32_t, MaskPattern::ALLF>();
    MaskReg halfMask = CreateMask<uint32_t, MaskPattern::H>();

    MaskReg lowMask, highMask, tmpMask;
    MaskInterleave<uint32_t>(lowMask, tmpMask, fullMask, zeroMask);
    MaskInterleave<uint32_t>(highMask, tmpMask, zeroMask, fullMask);

    UnalignReg unalignReg;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        MaskReg maskReg = UpdateMask<uint32_t>(firstCount);
        MaskReg indexMask, tmpMask;
        MaskDeInterleave<uint32_t>(indexMask, tmpMask, maskReg, zeroMask);

        RegTensor<uint32_t> in32Data;
        Reg::LoadAlign(in32Data, u32Src + i * eleCountPerVL);

        RegTensor<uint32_t> highPart, lowPart;
        GatherMask(lowPart, in32Data, lowMask);
        GatherMask(highPart, in32Data, highMask);

        MaskReg highGTMask, highEQMask, lowPartMask;
        CompareScalar<uint32_t, CMPMODE::GT>(highGTMask, highPart, highValue, halfMask);
        CompareScalar<uint32_t, CMPMODE::EQ>(highEQMask, highPart, highValue, halfMask);
        CompareScalar<uint32_t, CMPMODE::GT>(lowPartMask, lowPart, lowValue, halfMask);

        MaskReg cmpMask;
        MaskAnd(cmpMask, highEQMask, lowPartMask, halfMask);
        MaskOr(cmpMask, highGTMask, cmpMask, halfMask);
        MaskAnd(cmpMask, cmpMask, indexMask, indexMask);

        RegTensor<int32_t> index;
        Reg::LoadAlign(index, inputIndex + i * numsPerRound);

        RegTensor<int32_t> outIndex;
        GatherMask<int32_t, GatherMaskMode::STORE_REG>(outIndex, index, cmpMask);
        Reg::StoreUnAlign<int32_t, PostLiteral::POST_MODE_UPDATE>(dstIndex, outIndex, unalignReg);
    }

    for (uint16_t i = 0; i < repeatTimes; ++i) {
        MaskReg maskReg = UpdateMask<uint32_t>(secondCount);
        MaskReg indexMask, tmpMask;
        MaskDeInterleave<uint32_t>(indexMask, tmpMask, maskReg, zeroMask);

        RegTensor<uint32_t> in32Data;
        Reg::LoadAlign(in32Data, u32Src + i * eleCountPerVL);

        RegTensor<uint32_t> highPart, lowPart;
        GatherMask(lowPart, in32Data, lowMask);
        GatherMask(highPart, in32Data, highMask);

        MaskReg highEQMask, lowPartMask;
        CompareScalar<uint32_t, CMPMODE::EQ>(highEQMask, highPart, highValue, halfMask);
        CompareScalar<uint32_t, CMPMODE::EQ>(lowPartMask, lowPart, lowValue, halfMask);

        MaskReg cmpMask;
        MaskAnd(cmpMask, highEQMask, lowPartMask, halfMask);
        MaskAnd(cmpMask, cmpMask, indexMask, indexMask);

        RegTensor<int32_t> index;
        Reg::LoadAlign(index, inputIndex + i * numsPerRound);

        RegTensor<int32_t> outIndex;
        GatherMask<int32_t, GatherMaskMode::STORE_REG>(outIndex, index, cmpMask);
        Reg::StoreUnAlign<int32_t, PostLiteral::POST_MODE_UPDATE>(dstIndex, outIndex, unalignReg);
    }

    Reg::StoreUnAlignPost(dstIndex, unalignReg);
    ClearSpr<SpecialPurposeReg::AR>();
}

__simd_vf__ inline void GatherGreaterAndEqualKIndex(
    __ubuf__ uint32_t* src, __ubuf__ int32_t* inputIndex, __ubuf__ int32_t* dstIndex, const uint32_t value,
    uint32_t count)
{
    constexpr uint32_t eleCountPerVL = GetVecLen() / sizeof(uint32_t);
    uint16_t repeatTimes = DivCeil(count, eleCountPerVL);

    auto firstCount = count;
    auto secondCount = count;

    UnalignReg unalignReg;
    for (uint16_t i = 0; i < repeatTimes; ++i) {
        MaskReg maskReg = UpdateMask<int32_t>(firstCount);
        RegTensor<uint32_t> in32Data;
        Reg::LoadAlign(in32Data, src + i * eleCountPerVL);

        MaskReg out32Mask;
        CompareScalar<uint32_t, CMPMODE::GT>(out32Mask, in32Data, value, maskReg);

        RegTensor<int32_t> index;
        Reg::LoadAlign(index, inputIndex + i * eleCountPerVL);

        RegTensor<int32_t> outIndex;
        GatherMask<int32_t, GatherMaskMode::STORE_REG>(outIndex, index, out32Mask);
        Reg::StoreUnAlign<int32_t, PostLiteral::POST_MODE_UPDATE>(dstIndex, outIndex, unalignReg);
    }

    for (uint16_t i = 0; i < repeatTimes; ++i) {
        MaskReg maskReg = UpdateMask<int32_t>(secondCount);
        RegTensor<uint32_t> in32Data;
        Reg::LoadAlign(in32Data, src + i * eleCountPerVL);

        MaskReg out32Mask;
        CompareScalar<uint32_t, CMPMODE::EQ>(out32Mask, in32Data, value, maskReg);

        RegTensor<int32_t> index;
        Reg::LoadAlign(index, inputIndex + i * eleCountPerVL);

        RegTensor<int32_t> outIndex;
        GatherMask<int32_t, GatherMaskMode::STORE_REG>(outIndex, index, out32Mask);
        Reg::StoreUnAlign<int32_t, PostLiteral::POST_MODE_UPDATE>(dstIndex, outIndex, unalignReg);
    }

    Reg::StoreUnAlignPost(dstIndex, unalignReg);
    ClearSpr<SpecialPurposeReg::AR>();
}

__simd_vf__ inline void GatherGreaterAndEqualKIndex(
    __ubuf__ uint16_t* src, __ubuf__ int32_t* inputIndex, __ubuf__ int32_t* dstIndex, const uint16_t value,
    uint32_t count)
{
    constexpr uint32_t u32EleCountPerVL = GetVecLen() / sizeof(int32_t);
    constexpr uint32_t u16EleCountPerVL = GetVecLen() / sizeof(uint16_t);
    uint16_t dataRepeatTimes = DivCeil(count, u16EleCountPerVL);
    uint16_t indexRepeatTimes = DivCeil(count, u32EleCountPerVL);

    auto firstCount = count;
    auto secondCount = count;

    UnalignReg unalignReg1;
    for (uint16_t i = 0; i < dataRepeatTimes; ++i) {
        MaskReg maskReg = UpdateMask<uint16_t>(firstCount);
        RegTensor<uint16_t> in16Data;
        Reg::LoadAlign(in16Data, src + i * u16EleCountPerVL);

        MaskReg out16Mask;
        CompareScalar<uint16_t, CMPMODE::GT>(out16Mask, in16Data, value, maskReg);

        MaskReg zero16Mask = CreateMask<uint16_t, MaskPattern::ALLF>();
        MaskReg out32Mask0, out32Mask1;
        MaskInterleave<uint16_t>(out32Mask0, out32Mask1, out16Mask, zero16Mask);

        RegTensor<int32_t> index0, index1;
        Reg::LoadAlign(index0, inputIndex + i * u16EleCountPerVL);
        Reg::LoadAlign(index1, inputIndex + i * u16EleCountPerVL + u32EleCountPerVL);

        RegTensor<int32_t> outIndex0, outIndex1;
        GatherMask<int32_t, GatherMaskMode::STORE_REG>(outIndex0, index0, out32Mask0);
        Reg::StoreUnAlign<int32_t, PostLiteral::POST_MODE_UPDATE>(dstIndex, outIndex0, unalignReg1);
        GatherMask<int32_t, GatherMaskMode::STORE_REG>(outIndex1, index1, out32Mask1);
        Reg::StoreUnAlign<int32_t, PostLiteral::POST_MODE_UPDATE>(dstIndex, outIndex1, unalignReg1);
    }

    for (uint16_t i = 0; i < dataRepeatTimes; ++i) {
        MaskReg maskReg = UpdateMask<uint16_t>(secondCount);
        RegTensor<uint16_t> in16Data;
        Reg::LoadAlign(in16Data, src + i * u16EleCountPerVL);

        MaskReg out16Mask;
        CompareScalar<uint16_t, CMPMODE::EQ>(out16Mask, in16Data, value, maskReg);

        MaskReg zero16Mask = CreateMask<uint16_t, MaskPattern::ALLF>();
        MaskReg out32Mask0, out32Mask1;
        MaskInterleave<uint16_t>(out32Mask0, out32Mask1, out16Mask, zero16Mask);

        RegTensor<int32_t> index0, index1;
        Reg::LoadAlign(index0, inputIndex + i * u16EleCountPerVL);
        Reg::LoadAlign(index1, inputIndex + i * u16EleCountPerVL + u32EleCountPerVL);

        RegTensor<int32_t> outIndex0, outIndex1;
        GatherMask<int32_t, GatherMaskMode::STORE_REG>(outIndex0, index0, out32Mask0);
        Reg::StoreUnAlign<int32_t, PostLiteral::POST_MODE_UPDATE>(dstIndex, outIndex0, unalignReg1);
        GatherMask<int32_t, GatherMaskMode::STORE_REG>(outIndex1, index1, out32Mask1);
        Reg::StoreUnAlign<int32_t, PostLiteral::POST_MODE_UPDATE>(dstIndex, outIndex1, unalignReg1);
    }

    Reg::StoreUnAlignPost(dstIndex, unalignReg1);
    ClearSpr<SpecialPurposeReg::AR>();
}

__simd_vf__ inline void GatherGreaterAndEqualKIndex(
    __ubuf__ uint8_t* src, __ubuf__ int32_t* inputIndex, __ubuf__ int32_t* dstIndex, const uint8_t value,
    uint32_t count)
{
    constexpr uint16_t u8EleCountPerVL = GetVecLen() / sizeof(uint8_t);
    constexpr uint32_t u32EleCountPerVL = GetVecLen() / sizeof(int32_t);

    uint16_t dataRepeatTimes = DivCeil(count, u8EleCountPerVL);
    uint16_t indexRepeatTimes = DivCeil(count, u32EleCountPerVL);

    auto firstCount = count;
    auto secondCount = count;

    UnalignReg unalignReg;
    for (uint16_t i = 0; i < dataRepeatTimes; ++i) {
        MaskReg maskReg = UpdateMask<uint8_t>(firstCount);
        RegTensor<uint8_t> in8Data;
        Reg::LoadAlign(in8Data, src + i * u8EleCountPerVL);

        MaskReg out8Mask;
        CompareScalar<uint8_t, CMPMODE::GT>(out8Mask, in8Data, value, maskReg);

        MaskReg zero8Mask = CreateMask<uint8_t, MaskPattern::ALLF>();
        MaskReg out16Mask0, out16Mask1;
        MaskInterleave<uint8_t>(out16Mask0, out16Mask1, out8Mask, zero8Mask);

        MaskReg zero16Mask = CreateMask<uint16_t, MaskPattern::ALLF>();
        MaskReg out32Mask0, out32Mask1, out32Mask2, out32Mask3;
        MaskInterleave<uint16_t>(out32Mask0, out32Mask1, out16Mask0, zero16Mask);
        MaskInterleave<uint16_t>(out32Mask2, out32Mask3, out16Mask1, zero16Mask);

        RegTensor<int32_t> index0, index1, index2, index3;
        Reg::LoadAlign(index0, inputIndex + i * GetVecLen());
        Reg::LoadAlign(index1, inputIndex + i * GetVecLen() + u32EleCountPerVL);
        Reg::LoadAlign(index2, inputIndex + i * GetVecLen() + u32EleCountPerVL * 2);
        Reg::LoadAlign(index3, inputIndex + i * GetVecLen() + u32EleCountPerVL * 3);

        RegTensor<int32_t> outIndex0, outIndex1, outIndex2, outIndex3;
        GatherMask<int32_t, GatherMaskMode::STORE_REG>(outIndex0, index0, out32Mask0);
        Reg::StoreUnAlign<int32_t, PostLiteral::POST_MODE_UPDATE>(dstIndex, outIndex0, unalignReg);
        GatherMask<int32_t, GatherMaskMode::STORE_REG>(outIndex1, index1, out32Mask1);
        Reg::StoreUnAlign<int32_t, PostLiteral::POST_MODE_UPDATE>(dstIndex, outIndex1, unalignReg);
        GatherMask<int32_t, GatherMaskMode::STORE_REG>(outIndex2, index2, out32Mask2);
        Reg::StoreUnAlign<int32_t, PostLiteral::POST_MODE_UPDATE>(dstIndex, outIndex2, unalignReg);
        GatherMask<int32_t, GatherMaskMode::STORE_REG>(outIndex3, index3, out32Mask3);
        Reg::StoreUnAlign<int32_t, PostLiteral::POST_MODE_UPDATE>(dstIndex, outIndex3, unalignReg);
    }

    for (uint16_t i = 0; i < dataRepeatTimes; ++i) {
        MaskReg maskReg = UpdateMask<uint8_t>(secondCount);
        RegTensor<uint8_t> in8Data;
        Reg::LoadAlign(in8Data, src + i * u8EleCountPerVL);

        MaskReg out8Mask;
        CompareScalar<uint8_t, CMPMODE::EQ>(out8Mask, in8Data, value, maskReg);

        MaskReg zero8Mask = CreateMask<uint8_t, MaskPattern::ALLF>();
        MaskReg out16Mask0, out16Mask1;
        MaskInterleave<uint8_t>(out16Mask0, out16Mask1, out8Mask, zero8Mask);

        MaskReg zero16Mask = CreateMask<uint16_t, MaskPattern::ALLF>();
        MaskReg out32Mask0, out32Mask1, out32Mask2, out32Mask3;
        MaskInterleave<uint16_t>(out32Mask0, out32Mask1, out16Mask0, zero16Mask);
        MaskInterleave<uint16_t>(out32Mask2, out32Mask3, out16Mask1, zero16Mask);

        RegTensor<int32_t> index0, index1, index2, index3;
        Reg::LoadAlign(index0, inputIndex + i * GetVecLen());
        Reg::LoadAlign(index1, inputIndex + i * GetVecLen() + u32EleCountPerVL);
        Reg::LoadAlign(index2, inputIndex + i * GetVecLen() + u32EleCountPerVL * 2);
        Reg::LoadAlign(index3, inputIndex + i * GetVecLen() + u32EleCountPerVL * 3);

        RegTensor<int32_t> outIndex0, outIndex1, outIndex2, outIndex3;
        GatherMask<int32_t, GatherMaskMode::STORE_REG>(outIndex0, index0, out32Mask0);
        Reg::StoreUnAlign<int32_t, PostLiteral::POST_MODE_UPDATE>(dstIndex, outIndex0, unalignReg);
        GatherMask<int32_t, GatherMaskMode::STORE_REG>(outIndex1, index1, out32Mask1);
        Reg::StoreUnAlign<int32_t, PostLiteral::POST_MODE_UPDATE>(dstIndex, outIndex1, unalignReg);
        GatherMask<int32_t, GatherMaskMode::STORE_REG>(outIndex2, index2, out32Mask2);
        Reg::StoreUnAlign<int32_t, PostLiteral::POST_MODE_UPDATE>(dstIndex, outIndex2, unalignReg);
        GatherMask<int32_t, GatherMaskMode::STORE_REG>(outIndex3, index3, out32Mask3);
        Reg::StoreUnAlign<int32_t, PostLiteral::POST_MODE_UPDATE>(dstIndex, outIndex3, unalignReg);
    }

    Reg::StoreUnAlignPost(dstIndex, unalignReg);
    ClearSpr<SpecialPurposeReg::AR>();
}

template <typename T>
__simd_callee__ inline int32_t GetKPad(int32_t k)
{
    constexpr int32_t alignBytes = 32;
    return AlignUp(k, alignBytes / sizeof(T));
}

template <typename T>
__simd_vf__ inline void SaveData(
    __ubuf__ T* dst, __ubuf__ int32_t* dstIndex, __ubuf__ T* src, __ubuf__ int32_t* srcIndex, const uint32_t count)
{
    constexpr uint32_t dataCountPerTime = GetVecLen() / sizeof(T);
    uint16_t dataRepeatTimes = DivCeil(count, dataCountPerTime);
    constexpr uint32_t indexCountPerTime = GetVecLen() / sizeof(int32_t);
    uint16_t indexRepeatTimes = DivCeil(count, indexCountPerTime);

    uint32_t dCount = count;
    uint32_t iCount = count;

    for (uint16_t i = 0; i < dataRepeatTimes; ++i) {
        MaskReg maskReg = UpdateMask<T>(dCount);
        RegTensor<T> reg;
        Reg::LoadAlign<T, PostLiteral::POST_MODE_UPDATE>(reg, (__ubuf__ T*&)src, dataCountPerTime);
        Reg::StoreAlign<T, PostLiteral::POST_MODE_UPDATE>((__ubuf__ T*&)dst, reg, dataCountPerTime, maskReg);
    }

    for (uint16_t i = 0; i < indexRepeatTimes; ++i) {
        MaskReg maskReg = UpdateMask<int32_t>(iCount);
        RegTensor<int32_t> reg;
        Reg::LoadAlign<int32_t, PostLiteral::POST_MODE_UPDATE>(reg, (__ubuf__ int32_t*&)srcIndex, indexCountPerTime);
        Reg::StoreAlign<int32_t, PostLiteral::POST_MODE_UPDATE>(
            (__ubuf__ int32_t*&)dstIndex, reg, indexCountPerTime, maskReg);
    }
}

template <typename T>
__simd_vf__ inline void SaveDataUnAlignVF(
    __ubuf__ T* dst, __ubuf__ int32_t* dstIndex, __ubuf__ T* src, __ubuf__ int32_t* srcIndex, const TopKInfo topKInfo,
    const uint32_t k)
{
    uint32_t dataMainCountPerTime = GetVecLen() / sizeof(T);
    uint32_t indexMainCountPerTime = GetVecLen() / sizeof(int32_t);

    int32_t kPad = GetKPad<T>(k);
    int32_t kIndexPad = GetKPad<int32_t>(k);
    uint16_t dataMainRepeatTime = DivCeil(kPad, dataMainCountPerTime);
    uint16_t indexMainRepeatTime = DivCeil(kIndexPad, indexMainCountPerTime);

    for (uint16_t j = 0; j < static_cast<uint16_t>(topKInfo.outter); ++j) {
        for (uint16_t i = 0; i < dataMainRepeatTime; i++) {
            UnalignReg ureg;
            RegTensor<T> reg;
            auto dstUBT = dst + j * k + i * kPad;
            Reg::LoadAlign(reg, src + j * kPad + i * kPad);
            Reg::StoreUnAlign((__ubuf__ T*&)dstUBT, reg, ureg, k);
            Reg::StoreUnAlignPost(dstUBT, ureg, 0);
        }

        for (uint16_t i = 0; i < indexMainRepeatTime; i++) {
            UnalignReg ureg;
            RegTensor<int32_t> reg;
            auto dstUBT = dstIndex + j * k + i * kIndexPad;
            Reg::LoadAlign(reg, srcIndex + j * kIndexPad + i * kIndexPad);
            Reg::StoreUnAlign((__ubuf__ int32_t*&)dstUBT, reg, ureg, k);
            Reg::StoreUnAlignPost(dstUBT, ureg, 0);
        }
    }
}

template <typename T>
__aicore__ inline void SaveDataUnAlign(
    const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal,
    const LocalTensor<int32_t>& srcIndexLocal, const TopKInfo& topKInfo, const uint32_t k)
{
    __ubuf__ T* src = (__ubuf__ T*)srcLocal.GetPhyAddr();
    __ubuf__ int32_t* srcIndex = (__ubuf__ int32_t*)srcIndexLocal.GetPhyAddr();
    __ubuf__ T* dst = (__ubuf__ T*)dstValueLocal.GetPhyAddr();
    __ubuf__ int32_t* dstIndex = (__ubuf__ int32_t*)dstIndexLocal.GetPhyAddr();

    SaveDataUnAlignVF<T>(dst, dstIndex, src, srcIndex, topKInfo, k);
}

template <typename T, bool isInitIndex = false, bool isReuseSrc = false, const TopKConfig& config = defaultTopKConfig>
__aicore__ inline void TopKRaidxSelect(
    const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal,
    const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<T>& tempBuffer, const int32_t k,
    const TopkTiling& tiling, const TopKInfo& topKInfo, const bool isLargest)
{
    using ConvType = typename AscendC::Internal::ExtractTypeBySize<sizeof(T)>::T;

    constexpr bool isOrderFromTemplate = config.order != TopKOrder::UNSET;
    constexpr bool isLargestInTemplate = config.order == TopKOrder::LARGEST;

    __ubuf__ ConvType* src = (__ubuf__ ConvType*)srcLocal.GetPhyAddr();
    __ubuf__ int32_t* srcIndex = (__ubuf__ int32_t*)srcIndexLocal.GetPhyAddr();
    __ubuf__ ConvType* dst = (__ubuf__ ConvType*)dstValueLocal.GetPhyAddr();
    __ubuf__ int32_t* dstIndex = (__ubuf__ int32_t*)dstIndexLocal.GetPhyAddr();
    __ubuf__ uint8_t* tmp = (__ubuf__ uint8_t*)tempBuffer.GetPhyAddr();

    // temp data for storing values which are greater and equal than the topk value, same size with src
    __ubuf__ ConvType* tmpSrcData;
    // temp data for storing indexes related tmpSrcData, same size with srcIndex
    __ubuf__ int32_t* tmpSrcIndex;
    // temp data for storing accumulate data, 512B
    __ubuf__ uint16_t* tmpHistData;
    // temp data for storing work src values
    __ubuf__ ConvType* realWorkData = src;
    // temp data for sort
    __ubuf__ ConvType* sortTmpBuffer;

    uint32_t count = topKInfo.inner;
    uint32_t realCount = topKInfo.n;

    if constexpr (isOrderFromTemplate) {
        InitializeTempBuffer<T, isReuseSrc, isLargestInTemplate>(
            tempBuffer, count, (__ubuf__ T*&)tmpSrcData, tmpSrcIndex, tmpHistData, (__ubuf__ T*&)realWorkData,
            (__ubuf__ T*&)sortTmpBuffer);

        if constexpr (NeedPreProcess<T, isLargestInTemplate>()) {
            Internal::TwiddleInData<T, ConvType, !isLargestInTemplate>(src, realWorkData, count);
        }
    } else {
        InitializeTempBuffer<T, isReuseSrc>(
            tempBuffer, isLargest, count, (__ubuf__ T*&)tmpSrcData, tmpSrcIndex, tmpHistData,
            (__ubuf__ T*&)realWorkData, (__ubuf__ T*&)sortTmpBuffer);

        if (NeedPreProcess<T>(isLargest)) {
            if (isLargest) {
                Internal::TwiddleInData<T, ConvType, false>(src, realWorkData, count);
            } else {
                Internal::TwiddleInData<T, ConvType, true>(src, realWorkData, count);
            }
        }
    }

    constexpr uint16_t typeBytes = sizeof(T);
    int32_t remainK = k;
    ConvType kthValue = 0;
    event_t eventVS = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_S));

    for (uint16_t i = typeBytes; i > 0 && remainK > 0; --i) {
        GenerateAccumulateData<ConvType>(realWorkData, tmpHistData, tmpSrcData, realCount, kthValue, i);

        SetFlag<HardEvent::V_S>(eventVS);
        WaitFlag<HardEvent::V_S>(eventVS);

        int32_t expValue = tmpHistData[255] - remainK;
        int16_t left = 0;
        int16_t right = 255;
        bool found = false;
        while (left <= right) {
            int16_t mid = left + (right - left) / 2;
            if (tmpHistData[mid] == expValue) {
                kthValue |= (static_cast<ConvType>(mid + 1) << ((i - 1) * 8));
                remainK = 0;
                found = true;
                break;
            } else if (tmpHistData[mid] > expValue) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        if (!found) {
            if (right >= 0) {
                kthValue |= (static_cast<ConvType>(right + 1) << ((i - 1) * 8));
                remainK -= (tmpHistData[255] - tmpHistData[right + 1]);
            } else {
                remainK = tmpHistData[0] - expValue;
            }
        }
    }

    GatherGreaterAndEqualKData(realWorkData, tmpSrcData, kthValue, realCount);
    GatherGreaterAndEqualKIndex(realWorkData, srcIndex, tmpSrcIndex, kthValue, realCount);

    if constexpr (config.sorted) {
        static constexpr SortConfig sortConfig = {SortType::RADIX_SORT, true};

        LocalTensor<ConvType> valueTensor = tempBuffer.template ReinterpretCast<ConvType>();
        LocalTensor<ConvType> sortDataSrc = valueTensor[(__ubuf__ T*)tmpSrcData - (__ubuf__ T*)tmp];

        LocalTensor<int32_t> indexTensor = tempBuffer.template ReinterpretCast<int32_t>();
        LocalTensor<int32_t> sortIndexSrc = indexTensor[(__ubuf__ int32_t*)tmpSrcIndex - (__ubuf__ int32_t*)tmp];

        LocalTensor<uint8_t> tmpTensor = tempBuffer.template ReinterpretCast<uint8_t>();
        LocalTensor<uint8_t> sortBufferTensor = tmpTensor[(__ubuf__ uint8_t*)sortTmpBuffer - (__ubuf__ uint8_t*)tmp];

        LocalTensor<ConvType> dstValueTensor = dstValueLocal.template ReinterpretCast<ConvType>();
        LocalTensor<int32_t> dstIndexTensor = dstIndexLocal.template ReinterpretCast<int32_t>();
        Sort<ConvType, int32_t, false, sortConfig>(
            dstValueTensor, dstIndexTensor, sortDataSrc, sortIndexSrc, sortBufferTensor, static_cast<uint32_t>(k));
    } else {
        SaveData<T>((__ubuf__ T*)dst, dstIndex, (__ubuf__ T*)tmpSrcData, tmpSrcIndex, static_cast<uint32_t>(k));
    }

    if constexpr (isOrderFromTemplate) {
        if constexpr (NeedPreProcess<T, isLargestInTemplate>()) {
            Internal::TwiddleOutData<T, ConvType, !isLargestInTemplate>(dst, dst, k);
        }
    } else {
        if (NeedPreProcess<T>(isLargest)) {
            if (isLargest) {
                Internal::TwiddleOutData<T, ConvType, false>(dst, dst, k);
            } else {
                Internal::TwiddleOutData<T, ConvType, true>(dst, dst, k);
            }
        }
    }
}

template <
    typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false,
    const TopKConfig& config = defaultTopKConfig>
__aicore__ inline void TopKNormal(
    const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal,
    const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const LocalTensor<T>& tempBuffer,
    const int32_t k, const TopkTiling& tiling, const TopKInfo& topKInfo, const bool isLargest = true)
{
    // if isInitIndex is false, The index of the input data needs to be generated here.
    LocalTensor<int32_t> realIndexSrc(srcIndexLocal);
    uint32_t initIndexTempBufferSize = 0;
    if constexpr (!isInitIndex) {
        LocalTensor<int32_t> indexSrcTmp = tempBuffer.template ReinterpretCast<int32_t>();
        CreateVecIndex(indexSrcTmp, static_cast<int32_t>(0), topKInfo.inner);
        realIndexSrc = indexSrcTmp;
        initIndexTempBufferSize += sizeof(int32_t) * topKInfo.inner / sizeof(T);
    }

    int32_t kPad = GetKPad<T>(k);
    int32_t indexKPad = GetKPad<int32_t>(k);
    for (int32_t i = 0; i < topKInfo.outter; ++i) {
        TopKRaidxSelect<T, isInitIndex, isReuseSrc, config>(
            dstValueLocal[i * kPad], dstIndexLocal[i * indexKPad], srcLocal[i * topKInfo.inner], realIndexSrc,
            tempBuffer[initIndexTempBufferSize], k, tiling, topKInfo, isLargest);
    }
}

template <
    typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false,
    const TopKConfig& config = defaultTopKConfig>
__ASC_USE_RESERVED_UBUF__(3510, "TopK is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void TopKNSmall(
    const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal,
    const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const LocalTensor<T>& tempBuffer,
    const int32_t k, const TopkTiling& tiling, const TopKInfo& topKInfo, const bool isLargest = true)
{
    // if isInitIndex is false, The index of the input data needs to be generated here.
    LocalTensor<int32_t> realIndexSrc(srcIndexLocal);
    LocalTensor<T> tmpDstValueLocal(dstValueLocal);
    LocalTensor<int32_t> tmpDstIndexLocal(dstIndexLocal);

    uint32_t initIndexTempBufferSize = 0;
    if constexpr (!isInitIndex) {
        LocalTensor<int32_t> indexLocalTmp = tempBuffer.template ReinterpretCast<int32_t>();
        CreateVecIndex(indexLocalTmp, static_cast<int32_t>(0), topKInfo.inner);
        if (topKInfo.outter > 1) {
            Copy(indexLocalTmp[topKInfo.inner], indexLocalTmp, topKInfo.inner, topKInfo.outter - 1, {1, 1, 4, 0});
        }
        realIndexSrc = indexLocalTmp;
        initIndexTempBufferSize += sizeof(int32_t) * (topKInfo.inner * topKInfo.outter) / sizeof(T);
    }

    int32_t kPad = GetKPad<T>(k);
    int32_t indexKPad = GetKPad<int32_t>(k);
    event_t eventVS = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_S));
    for (int32_t i = 0; i < topKInfo.outter; ++i) {
        TopKRaidxSelect<T, isInitIndex, isReuseSrc, config>(
            tmpDstValueLocal[i * kPad], tmpDstIndexLocal[i * indexKPad], srcLocal[i * topKInfo.inner],
            realIndexSrc[i * topKInfo.inner], tempBuffer[initIndexTempBufferSize], k, tiling, topKInfo, isLargest);
    }
    SetFlag<HardEvent::V_S>(eventVS);
    WaitFlag<HardEvent::V_S>(eventVS);
    SaveDataUnAlign<T>(dstValueLocal, dstIndexLocal, tmpDstValueLocal, tmpDstIndexLocal, topKInfo, k);
}

} // namespace RadixSelectTopK
} // namespace Reg

template <typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false>
__aicore__ inline void TopKNormal(
    const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal,
    const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const LocalTensor<T>& tempBuffer,
    const int32_t k, const TopkTiling& tiling, const TopKInfo& topKInfo, const bool isLargest = true)
{
    // if isInitIndex is false, The index of the input data needs to be generated here.
    if constexpr (!isInitIndex) {
        LocalTensor<int32_t> indexLocalTmp = tempBuffer[tiling.srcIndexOffset].template ReinterpretCast<int32_t>();
        CreateVecIndex(indexLocalTmp, static_cast<int32_t>(0), topKInfo.inner);
    }
    if (!isLargest) {
        Muls(srcLocal, srcLocal, T(-1), topKInfo.outter * topKInfo.inner);
    }

    TopKCompute<T, isInitIndex, isHasfinish>(
        dstValueLocal, dstIndexLocal, srcLocal, srcIndexLocal, finishLocal, tempBuffer, k, tiling, topKInfo, isLargest);

    if (!isLargest) {
        Muls(dstValueLocal, dstValueLocal, T(-1), tiling.maskOffset);
        Muls(srcLocal, srcLocal, T(-1), topKInfo.outter * topKInfo.inner);
    }
}

template <typename T, bool isInitIndex = false, bool isHasfinish = false, bool isReuseSrc = false>
__ASC_USE_RESERVED_UBUF__(3510, "TopK is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void TopKNSmall(
    const LocalTensor<T>& dstValueLocal, const LocalTensor<int32_t>& dstIndexLocal, const LocalTensor<T>& srcLocal,
    const LocalTensor<int32_t>& srcIndexLocal, const LocalTensor<bool>& finishLocal, const LocalTensor<T>& tempBuffer,
    const int32_t k, const TopkTiling& tiling, const TopKInfo& topKInfo, const bool isLargest = true)
{
    // if isInitIndex is false, The index of the input data needs to be generated here.
    if constexpr (!isInitIndex) {
        LocalTensor<int32_t> indexLocalTmp = tempBuffer[tiling.innerDataSize].template ReinterpretCast<int32_t>();
        CreateVecIndex(indexLocalTmp, static_cast<int32_t>(0), topKInfo.inner);
        if (topKInfo.outter > 1) {
            Copy(indexLocalTmp[topKInfo.inner], indexLocalTmp, topKInfo.inner, topKInfo.outter - 1, {1, 1, 4, 0});
        }
    }
    // if isLargest if false, sort Ascending
    if (!isLargest) {
        Muls(srcLocal, srcLocal, T(-1), topKInfo.outter * topKInfo.inner);
    }

    TopKNSmallCompute<T, isInitIndex, isHasfinish>(
        dstValueLocal, dstIndexLocal, srcLocal, srcIndexLocal, finishLocal, tempBuffer, k, tiling, topKInfo, isLargest);

    if (!isLargest) {
        Muls(dstValueLocal, dstValueLocal, T(-1), tiling.maskOffset);
        Muls(srcLocal, srcLocal, T(-1), topKInfo.outter * topKInfo.inner);
    }
}
} // namespace AscendC

#endif // IMPL_SORT_TOPK_TOPK_C310_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SORT_TOPK_TOPK_C310_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_SORT_TOPK_TOPK_C310_IMPL_H__
#endif
