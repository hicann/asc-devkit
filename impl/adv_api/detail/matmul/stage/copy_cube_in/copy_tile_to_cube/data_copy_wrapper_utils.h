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
 * \file data_copy_wrapper_utils.h
 * \brief
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/matmul/stage/copy_cube_in/copy_tile_to_cube/data_copy_wrapper_utils.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/matmul/matmul.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_UTILS_H__
#endif

#ifndef IMPL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_UTILS_H
#define IMPL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_UTILS_H

#include "../../../utils/matmul_utils.h"

namespace AscendC {
namespace Impl {
namespace Detail {

constexpr int32_t FIRST_16BIT_OFFSET_MM_API = 16;
constexpr int32_t SECOND_16BIT_OFFSET_MM_API = 32;
constexpr int32_t THIRD_16BIT_OFFSET_MM_API = 48;
constexpr int32_t REPEAT_BLOCK_NUM_MM_API = 8;
constexpr int32_t EACH_BLOCK_BYTES_MM_API = 32;
constexpr int32_t CACHE_LINE_SIZE_MM_API = 512;
constexpr int32_t TRANS_DATA_ARRAY_SIZE_MM_API = 16;
constexpr int32_t MAX_BLOCK_COUNT_SIZE_MM_API = 4095;
constexpr int32_t MM_NUM_TWO = 2;

template <typename TransT>
__ASC_USE_RESERVED_UBUF__(3510, "Matmul is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void NDPadZeroForWidth(
    LocalTensor<TransT>& dst, const int height, const int calcWidth, const int tail, int offset)
{
    constexpr int32_t c0Size_ = AuxGetC0Size<TransT>();

    uint16_t maskTail16bit = ~((1 << tail) - 1);
    uint64_t maskTail64bit = static_cast<uint64_t>(maskTail16bit);
    if (maskTail64bit == 0) {
        return;
    }
    uint64_t mask[2];
    mask[0] = maskTail64bit + (maskTail64bit << FIRST_16BIT_OFFSET_MM_API) +
              (maskTail64bit << SECOND_16BIT_OFFSET_MM_API) + (maskTail64bit << THIRD_16BIT_OFFSET_MM_API);
    mask[1] = mask[0];
    int stride = calcWidth * (c0Size_ * sizeof(TransT) / DEFAULT_C0_SIZE);
    int32_t totalRep = CeilT<int32_t>(height, REPEAT_BLOCK_NUM_MM_API);
    if constexpr (IsSameTypeV<TransT, int8_t>) {
        LocalTensor<int16_t> tmpTransTensor = dst.template ReinterpretCast<int16_t>();
        if (stride < EACH_BLOCK_BYTES_MM_API) {
            if (totalRep <= MAX_REPEAT_TIMES) {
                Duplicate(
                    tmpTransTensor[offset], (int16_t)0, mask, CeilT<int32_t>(height, REPEAT_BLOCK_NUM_MM_API), stride,
                    REPEAT_BLOCK_NUM_MM_API * stride);
            } else {
                int32_t highBlock = totalRep / MAX_REPEAT_TIMES;
                int32_t highTail = totalRep % MAX_REPEAT_TIMES;
                int64_t dstOffset = calcWidth * BLOCK_CUBE * REPEAT_BLOCK_NUM_MM_API * MAX_REPEAT_TIMES;
                for (int32_t idx = 0; idx < highBlock; ++idx) {
                    Duplicate(
                        tmpTransTensor[offset], (int16_t)0, mask, MAX_REPEAT_TIMES, stride,
                        REPEAT_BLOCK_NUM_MM_API * stride);
                    offset += dstOffset;
                }
                if (highTail) {
                    Duplicate(
                        tmpTransTensor[offset], (int16_t)0, mask, highTail, stride, REPEAT_BLOCK_NUM_MM_API * stride);
                }
            }
        } else {
            for (int32_t i = 0; i < totalRep; ++i) {
                Duplicate(tmpTransTensor[offset], (int16_t)0, mask, 1, stride, 0);
                offset += stride * BLOCK_CUBE;
            }
        }
    } else {
        Duplicate(dst[offset], (TransT)0, mask, totalRep, stride, REPEAT_BLOCK_NUM_MM_API * stride);
    }
    PipeBarrier<PIPE_V>();
}

template <typename TransT>
__ASC_USE_RESERVED_UBUF__(3510, "Matmul is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void NDPadZeros(
    LocalTensor<TransT>& dst, const int height, const int calcWidth, const int gCol, const int width,
    bool isBankConflict)
{
    constexpr int32_t c0Size_ = AuxGetC0Size<TransT>();

    int tail = width % c0Size_;
    if ((gCol % BLOCK_CUBE != 0) && (tail != 0)) {
        // tail pad zero
        constexpr int32_t DIV_TWO = 2;
        auto offset = width / c0Size_ * c0Size_;
        if constexpr (IsSameType<TransT, int8_t>::value) {
            tail = CeilT(tail, DIV_TWO);
            offset /= DIV_TWO;
        }
        NDPadZeroForWidth(dst, height, calcWidth, tail, offset);
    }
    // If the value of high is not an integer multiple of 16, add 0.
    int tailHigh = height % BLOCK_CUBE;
    if (tailHigh) {
        auto dstOffset = height * calcWidth * BLOCK_CUBE;
        if constexpr (IsSameTypeV<TransT, int8_t>) {
            LocalTensor<int16_t> tmpDst = dst.template ReinterpretCast<int16_t>();
            Duplicate(tmpDst[dstOffset], (int16_t)0, (BLOCK_CUBE - tailHigh) * calcWidth * BLOCK_CUBE);
        } else {
            Duplicate(dst[dstOffset], (TransT)0, (BLOCK_CUBE - tailHigh) * calcWidth * BLOCK_CUBE);
        }
    }
}

template <typename SrcT, typename TransT>
__aicore__ inline void TransDataNDBMatrix(
    const LocalTensor<TransT>& dst, const LocalTensor<SrcT>& src, int height, int width)
{
    constexpr int32_t c0Size_ = AuxGetC0Size<TransT>();

    int iterK = CeilT(height, c0Size_);
    int iterN = CeilT(width, c0Size_);
    int calcWidth = iterN * c0Size_;
    int tailWidth = (width % c0Size_) > TRANS_DATA_ARRAY_SIZE_MM_API ? 0 : width % TRANS_DATA_ARRAY_SIZE_MM_API;
    TransDataTo5HDParams params;
    params.repeatTimes = iterK;
    params.dstRepStride = iterK == 1 ? 0 : calcWidth;
    params.srcRepStride = iterK == 1 ? 0 : calcWidth;
    int dstHighHalfOffset = TRANS_DATA_ARRAY_SIZE_MM_API * c0Size_;
    int srcHighHalfOffset = TRANS_DATA_ARRAY_SIZE_MM_API * calcWidth;
    iterN = tailWidth ? iterN - 1 : iterN;
    uint64_t dstLocalList[TRANS_DATA_ARRAY_SIZE_MM_API];
    uint64_t srcLocalList[TRANS_DATA_ARRAY_SIZE_MM_API];
    int dstOffset = 0;
    int srcOffset = 0;
    for (int curN = 0; curN < iterN; curN++) {
        int srcListOffset = 0;
        int dstListOffset = 0;
        for (int i = 0; i < TRANS_DATA_ARRAY_SIZE_MM_API; i++) {
            dstLocalList[i] = (uint64_t)(dst[dstOffset + dstListOffset].GetPhyAddr());
            srcLocalList[i] = (uint64_t)(src[srcOffset + srcListOffset].GetPhyAddr());
            dstListOffset += c0Size_;
            srcListOffset += calcWidth;
        }
        params.srcHighHalf = false;
        params.dstHighHalf = false;
        TransDataTo5HD<TransT>(dstLocalList, srcLocalList, params);
        PipeBarrier<PIPE_V>();
        srcListOffset = 0;
        for (int i = 0; i < TRANS_DATA_ARRAY_SIZE_MM_API; i++) {
            srcLocalList[i] = (uint64_t)(src[srcOffset + srcListOffset + srcHighHalfOffset].GetPhyAddr());
            srcListOffset += calcWidth;
        }
        params.srcHighHalf = false;
        params.dstHighHalf = true;
        TransDataTo5HD<TransT>(dstLocalList, srcLocalList, params);
        PipeBarrier<PIPE_V>();
        dstListOffset = 0;
        srcListOffset = 0;
        for (int i = 0; i < TRANS_DATA_ARRAY_SIZE_MM_API; i++) {
            dstLocalList[i] = (uint64_t)(dst[dstOffset + dstListOffset + dstHighHalfOffset].GetPhyAddr());
            srcLocalList[i] = (uint64_t)(src[srcOffset + srcListOffset].GetPhyAddr());
            dstListOffset += c0Size_;
            srcListOffset += calcWidth;
        }
        params.dstHighHalf = false;
        params.srcHighHalf = true;
        TransDataTo5HD<TransT>(dstLocalList, srcLocalList, params);
        PipeBarrier<PIPE_V>();
        srcListOffset = 0;
        for (int i = 0; i < TRANS_DATA_ARRAY_SIZE_MM_API; i++) {
            srcLocalList[i] = (uint64_t)(src[srcOffset + srcListOffset + srcHighHalfOffset].GetPhyAddr());
            srcListOffset += calcWidth;
        }
        params.dstHighHalf = true;
        params.srcHighHalf = true;
        TransDataTo5HD<TransT>(dstLocalList, srcLocalList, params);
        PipeBarrier<PIPE_V>();
        dstOffset += c0Size_ * c0Size_;
        srcOffset += c0Size_;
    }
    if (tailWidth) {
        dstOffset = iterN * c0Size_ * c0Size_;
        srcOffset = iterN * c0Size_;
        int dstListOffset = 0;
        int srcListOffset = 0;
        params.dstRepStride = iterK == 1 ? 0 : TRANS_DATA_ARRAY_SIZE_MM_API;
        for (int i = 0; i < TRANS_DATA_ARRAY_SIZE_MM_API; i++) {
            dstLocalList[i] = (uint64_t)(dst[dstOffset + dstListOffset].GetPhyAddr());
            srcLocalList[i] = (uint64_t)(src[srcOffset + srcListOffset].GetPhyAddr());
            dstListOffset += c0Size_;
            srcListOffset += calcWidth;
        }
        params.dstHighHalf = false;
        params.srcHighHalf = false;
        TransDataTo5HD<TransT>(dstLocalList, srcLocalList, params);
        PipeBarrier<PIPE_V>();
        srcListOffset = 0;
        for (int i = 0; i < TRANS_DATA_ARRAY_SIZE_MM_API; i++) {
            srcLocalList[i] = (uint64_t)(src[srcOffset + srcListOffset + srcHighHalfOffset].GetPhyAddr());
            srcListOffset += calcWidth;
        }
        params.dstHighHalf = true;
        params.srcHighHalf = false;
        TransDataTo5HD<TransT>(dstLocalList, srcLocalList, params);
        PipeBarrier<PIPE_V>();
    }
}

template <typename SrcT, typename TransT, bool IS_TRANS = false>
__aicore__ inline void TransDataNZBMatrix(
    const LocalTensor<TransT>& dst, const LocalTensor<SrcT>& src, int height, int width)
{
    constexpr int32_t c0Size_ = AuxGetC0Size<TransT>();

    int iterK = CeilT(height, c0Size_);
    int iterN = CeilT(width, c0Size_);
    int calcWidth = iterN * c0Size_;
    int tailWidth = width % c0Size_;
    TransDataTo5HDParams params;
    params.repeatTimes = iterK;
    params.dstRepStride = iterK == 1 ? 0 : calcWidth;
    params.srcRepStride = iterK == 1 ? 0 : c0Size_;
    int dstHighHalfOffset = TRANS_DATA_ARRAY_SIZE_MM_API * c0Size_;
    int srcHighHalfOffset = TRANS_DATA_ARRAY_SIZE_MM_API * c0Size_;
    uint64_t dstLocalList[TRANS_DATA_ARRAY_SIZE_MM_API];
    uint64_t srcLocalList[TRANS_DATA_ARRAY_SIZE_MM_API];
    int dstOffset = 0;
    int srcOffset = 0;
    for (int curN = 0; curN < iterN; curN++) {
        params.dstRepStride =
            (curN == iterN - 1 && tailWidth > 0 && tailWidth < c0Size_) ? tailWidth : params.dstRepStride;
        int dstListOffset = 0;
        int srcListOffset = 0;
        for (int i = 0; i < TRANS_DATA_ARRAY_SIZE_MM_API; i++) {
            dstLocalList[i] = (uint64_t)(dst[dstOffset + dstListOffset + dstHighHalfOffset].GetPhyAddr());
            srcLocalList[i] = (uint64_t)(src[srcOffset + srcListOffset + srcHighHalfOffset].GetPhyAddr());
            dstListOffset += c0Size_;
            srcListOffset += c0Size_;
        }
        params.dstHighHalf = true;
        params.srcHighHalf = true;
        TransDataTo5HD<TransT>(dstLocalList, srcLocalList, params);
        PipeBarrier<PIPE_V>();
        srcListOffset = 0;
        for (int i = 0; i < TRANS_DATA_ARRAY_SIZE_MM_API; i++) {
            srcLocalList[i] = (uint64_t)(src[srcOffset + srcListOffset].GetPhyAddr());
            srcListOffset += c0Size_;
        }
        params.dstHighHalf = false;
        params.srcHighHalf = true;
        TransDataTo5HD<TransT>(dstLocalList, srcLocalList, params);
        PipeBarrier<PIPE_V>();
        dstListOffset = 0;
        for (int i = 0; i < TRANS_DATA_ARRAY_SIZE_MM_API; i++) {
            dstLocalList[i] = (uint64_t)(dst[dstOffset + dstListOffset].GetPhyAddr());
            dstListOffset += c0Size_;
        }
        params.dstHighHalf = false;
        params.srcHighHalf = false;
        TransDataTo5HD<TransT>(dstLocalList, srcLocalList, params);
        PipeBarrier<PIPE_V>();
        srcListOffset = 0;
        for (int i = 0; i < TRANS_DATA_ARRAY_SIZE_MM_API; i++) {
            srcLocalList[i] = (uint64_t)(src[srcOffset + srcListOffset + srcHighHalfOffset].GetPhyAddr());
            srcListOffset += c0Size_;
        }
        params.dstHighHalf = true;
        params.srcHighHalf = false;
        TransDataTo5HD<TransT>(dstLocalList, srcLocalList, params);
        PipeBarrier<PIPE_V>();
        dstOffset += c0Size_ * c0Size_;
        srcOffset += height * c0Size_;
    }
}

#if __NPU_ARCH__ == 5102
template <typename SrcT, typename TransT>
__aicore__ inline void CopyNZ2NZImplByLoadData(
    const LocalTensor<TransT>& dst, const GlobalTensor<SrcT>& src, const int32_t row, const int32_t col,
    const int32_t height, const int32_t width, const int32_t gRow, const bool kAlignToC0Size = false)
{
    constexpr int32_t c0Size_ = AuxGetC0Size<TransT>();
    int32_t dstStride = 0;
    if (kAlignToC0Size) {
        dstStride = Ceil(height, c0Size_) * c0Size_ / BLOCK_CUBE;
    } else {
        dstStride = Ceil(height, BLOCK_CUBE);
    }

    LoadData2DParamsV2 loadDataParams;
    loadDataParams.srcStride = Ceil(gRow, BLOCK_CUBE);
    loadDataParams.mStartPosition = Ceil(row, BLOCK_CUBE);
    loadDataParams.kStartPosition = Ceil(col, c0Size_);
    loadDataParams.dstStride = static_cast<uint16_t>(dstStride);
    loadDataParams.mStep = Ceil(height, BLOCK_CUBE);
    loadDataParams.kStep = Ceil(width, c0Size_);
    LoadData(dst, src, loadDataParams);
}
#endif

template <typename SrcT, typename TransT, bool HasScalePos = false>
__aicore__ inline void CopyNZ2NZImpl(
    const LocalTensor<TransT>& dst, const GlobalTensor<SrcT>& src, const int32_t row, const int32_t col,
    const int32_t height, const int32_t width, const int32_t gRow, const bool kAlignToC0Size = false)
{
#if __NPU_ARCH__ == 5102
    CopyNZ2NZImplByLoadData(dst, src, row, col, height, width, gRow, kAlignToC0Size);
#else
    ASCENDC_ASSERT((gRow >= height), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "NZ2NZ height larger than origin matrix height, gRow is %d, which should be no less than height %d.", gRow,
            height);
    });
    constexpr int32_t c0Size_ = AuxGetC0Size<TransT>();
    int32_t alignedGRow = Ceil(gRow, BLOCK_CUBE) * BLOCK_CUBE;
    int64_t srcOffset = (int64_t)row * (int64_t)c0Size_ + (int64_t)col * (int64_t)alignedGRow;
    // height direction need to be 16 aligned
    auto alignHeight = Ceil(height, BLOCK_CUBE) * BLOCK_CUBE;
    int32_t blockLen = alignHeight * c0Size_ * sizeof(TransT) / ONE_BLK_SIZE;
    int32_t srcStride = (alignedGRow - alignHeight) * (c0Size_ * sizeof(TransT) / ONE_BLK_SIZE);
    if constexpr (IsSupportB4<TransT>()) {
        blockLen /= INT4_TWO;
        srcStride /= INT4_TWO;
    }
    if (srcStride >= UINT16_MAX) {
        for (int32_t i = 0; i < Ceil(width, c0Size_); ++i) {
            DataCopy(
                dst[static_cast<int64_t>(i) * static_cast<int64_t>(alignHeight) * static_cast<int64_t>(c0Size_)],
                src[srcOffset + static_cast<int64_t>(i) * static_cast<int64_t>(gRow) * static_cast<int64_t>(c0Size_)],
                {1, static_cast<uint16_t>(blockLen), 0, 0});
        }
    } else {
        uint16_t nburst = Ceil(width, c0Size_);
        int32_t dstStride = 0;
        if constexpr (IsNeedC0Align<TransT>()) {
            if (kAlignToC0Size) {
                if constexpr (HasScalePos && IsSupportB8<TransT>()) {
                    dstStride = Ceil(height, MX_BASEK_FACTOR) * MX_BASEK_FACTOR - alignHeight;
                } else {
                    dstStride = Ceil(height, c0Size_) * c0Size_ - alignHeight;
                }
            }
        }
        DataCopy(
            dst, src[srcOffset],
            {nburst, static_cast<uint16_t>(blockLen), static_cast<uint16_t>(srcStride),
             static_cast<uint16_t>(dstStride)});
    }
#endif
}

template <typename SrcT, typename TransT>
__aicore__ inline void CopyNZ2NZImpl(
    const LocalTensor<TransT>& dst, const LocalTensor<SrcT>& src, const int32_t row, const int32_t col,
    const int32_t height, const int32_t width, const int32_t gRow)
{
    ASCENDC_ASSERT((gRow >= height), {
        KERNEL_LOG(KERNEL_ERROR, "gRow is %d, which should be no less than height %d.", gRow, height);
    });
    constexpr int32_t c0Size_ = AuxGetC0Size<TransT>();
    int32_t srcOffset = row * c0Size_ + col * gRow;
    // height direction need to be 16 aligned
    auto alignHeight = (height + 15) / 16 * 16;
    int32_t blockLen = alignHeight * c0Size_ * sizeof(TransT) / ONE_BLK_SIZE;
    int32_t srcStride = (gRow - alignHeight) * (c0Size_ * sizeof(TransT) / ONE_BLK_SIZE);

    if (srcStride >= UINT16_MAX) {
        for (int32_t i = 0; i < width / c0Size_; ++i) {
            DataCopy(
                dst[i * alignHeight * c0Size_], src[srcOffset + i * gRow * c0Size_],
                {1, static_cast<uint16_t>(blockLen), 0, 0});
        }
    } else {
        DataCopy(
            dst, src[srcOffset],
            {static_cast<uint16_t>(width / c0Size_), static_cast<uint16_t>(blockLen), static_cast<uint16_t>(srcStride),
             0});
    }
}

template <typename TransT>
__ASC_USE_RESERVED_UBUF__(3510, "Matmul is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void NDTrans2NZForInt8(
    LocalTensor<TransT>& dst, LocalTensor<TransT>& src, const int calcHeight, const int calcWidth,
    const bool isBankConflict)
{
    constexpr int32_t c0Size_ = AuxGetC0Size<TransT>();

    struct UnaryRepeatParams intriParams;
    uint64_t mask[2] = {uint64_t(-1), uint64_t(-1)};
    int blkStride = isBankConflict ? calcWidth + 1 : calcWidth;
    intriParams.dstBlkStride = (c0Size_ * sizeof(TransT) / DEFAULT_C0_SIZE);
    intriParams.srcBlkStride = blkStride * (c0Size_ * sizeof(TransT) / DEFAULT_C0_SIZE);
    intriParams.dstRepStride = intriParams.dstBlkStride * DEFAULT_BLK_NUM;
    intriParams.srcRepStride = intriParams.srcBlkStride * DEFAULT_BLK_NUM;
    int dstOffset = 0;
    int srcOffset = 0;
    // ensure rep stride be less than 256
    constexpr int maxSrcBlkStride = 32;
    LocalTensor<int16_t> tmpSrc = src.template ReinterpretCast<int16_t>();
    LocalTensor<int16_t> tmpDst = dst.template ReinterpretCast<int16_t>();
    if (intriParams.srcBlkStride >= maxSrcBlkStride) {
        intriParams.dstBlkStride = 1;
        intriParams.srcBlkStride = 1;
        mask[0] = (1 << BLOCK_CUBE) - 1;
        mask[1] = 0;
        SetVectorMask<int16_t>(mask[1], mask[0]);
        for (int i = 0; i < calcWidth; i++) {
            for (int j = 0; j < calcHeight * BLOCK_CUBE; ++j) {
                dstOffset = i * calcHeight * CUBE_MAX_SIZE + j * BLOCK_CUBE;
                srcOffset = j * blkStride * BLOCK_CUBE + i * BLOCK_CUBE;
                Muls<int16_t, false>(tmpDst[dstOffset], tmpSrc[srcOffset], (int16_t)1, mask, 1, intriParams);
            }
        }
    } else {
        SetVectorMask<int16_t>(mask[1], mask[0]);
        int32_t totalRepTimes = 2 * calcHeight;
        int32_t highBlock = totalRepTimes / MAX_REPEAT_TIMES;
        int32_t highTail = totalRepTimes % MAX_REPEAT_TIMES;
        for (int i = 0; i < calcWidth; i++) {
            dstOffset = i * calcHeight * CUBE_MAX_SIZE;
            srcOffset = i * BLOCK_CUBE;
            for (int32_t idx = 0; idx < highBlock; ++idx) {
                Muls<int16_t, false>(
                    tmpDst[dstOffset], tmpSrc[srcOffset], (int16_t)1, mask, MAX_REPEAT_TIMES, intriParams);
                dstOffset += BLOCK_CUBE * MAX_REPEAT_TIMES * REPEAT_BLOCK_NUM_MM_API;
                srcOffset += calcWidth * BLOCK_CUBE * MAX_REPEAT_TIMES * REPEAT_BLOCK_NUM_MM_API;
            }
            if (highTail) {
                Muls<int16_t, false>(tmpDst[dstOffset], tmpSrc[srcOffset], (int16_t)1, mask, highTail, intriParams);
            }
        }
    }
}

template <typename SrcT, typename TransT>
__ASC_USE_RESERVED_UBUF__(3510, "Matmul is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void NDTrans2NZForFP16(
    LocalTensor<TransT>& dst, LocalTensor<TransT>& src, const int calcHeight, const int calcWidth,
    const bool isBankConflict)
{
    const int c0Count = AscendCUtils::GetC0Count(sizeof(TransT));
    struct UnaryRepeatParams intriParams;
    uint64_t mask[2] = {uint64_t(-1), uint64_t(-1)};
    int32_t padBlock = 1;
    constexpr int32_t BLOCK_NUM = 2;
    if constexpr (IsSameTypeV<TransT, half> && IsSameTypeV<SrcT, int8_t>) {
        padBlock = BLOCK_NUM;
    }
    int blkStride = isBankConflict ? calcWidth + padBlock : calcWidth;
    intriParams.dstBlkStride = (BLOCK_CUBE * sizeof(TransT) / DEFAULT_C0_SIZE);
    intriParams.srcBlkStride = blkStride * BLOCK_CUBE * sizeof(TransT) / DEFAULT_C0_SIZE;
    intriParams.dstRepStride = intriParams.dstBlkStride * DEFAULT_BLK_NUM;
    intriParams.srcRepStride = intriParams.srcBlkStride * DEFAULT_BLK_NUM;
    int dstOffset = 0;
    int srcOffset = 0;
    // ensure rep stride be less than 256
    constexpr int maxSrcBlkStride = 32;
    if (intriParams.srcBlkStride >= maxSrcBlkStride) {
        intriParams.dstBlkStride = 1;
        intriParams.srcBlkStride = 1;
        mask[0] = (1 << BLOCK_CUBE) - 1;
        mask[1] = 0;
        SetVectorMask<TransT>(mask[1], mask[0]);
        for (int i = 0; i < calcWidth; i++) {
            for (int j = 0; j < calcHeight * BLOCK_CUBE; ++j) {
                dstOffset = i * calcHeight * CUBE_MAX_SIZE + j * BLOCK_CUBE;
                srcOffset = j * blkStride * BLOCK_CUBE + i * BLOCK_CUBE;
                Muls<TransT, false>(dst[dstOffset], src[srcOffset], (TransT)1, mask, 1, intriParams);
                if constexpr (sizeof(TransT) == sizeof(float)) {
                    Muls<TransT, false>(
                        dst[dstOffset + c0Count], src[srcOffset + c0Count], (TransT)1, mask, 1, intriParams);
                }
            }
        }
    } else {
        SetVectorMask<TransT>(mask[1], mask[0]);
        for (int i = 0; i < calcWidth; i++) {
            dstOffset = i * calcHeight * CUBE_MAX_SIZE;
            srcOffset = i * BLOCK_CUBE;
            Muls<TransT, false>(dst[dstOffset], src[srcOffset], (TransT)1, mask, BLOCK_NUM * calcHeight, intriParams);
            if constexpr (sizeof(TransT) == sizeof(float)) {
                Muls<TransT, false>(
                    dst[dstOffset + c0Count], src[srcOffset + c0Count], (TransT)1, mask, BLOCK_NUM * calcHeight,
                    intriParams);
            }
        }
    }
}

template <typename SrcT, typename TransT>
__ASC_USE_RESERVED_UBUF__(3510, "Matmul is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void NDTrans2NZ(
    LocalTensor<TransT>& dst, LocalTensor<TransT>& src, const int calcHeight, const int calcWidth,
    const bool isBankConflict)
{
    // Use Muls, convert to NZ format
    if constexpr (IsSameTypeV<TransT, int8_t>) {
        NDTrans2NZForInt8(dst, src, calcHeight, calcWidth, isBankConflict);
    } else {
        NDTrans2NZForFP16<SrcT>(dst, src, calcHeight, calcWidth, isBankConflict);
    }
}

template <typename SrcT, typename TransT>
__aicore__ inline int CopyNDBlock(
    const LocalTensor<SrcT>& transTensor, const GlobalTensor<SrcT>& src, int64_t srcOffset, const int height,
    const int width, const int gCol, const bool isBankConflict)
{
    ASCENDC_ASSERT(
        (gCol >= width), { KERNEL_LOG(KERNEL_ERROR, "gCol is %d, which should be no less than %d.", gCol, width); });
    constexpr int32_t c0Size_ = AuxGetC0Size<TransT>();
    int32_t oriC0Size = AuxGetC0Size<SrcT>();
    int32_t calcWidthExr = CeilAlignT<int32_t>(width, oriC0Size);
    int32_t calcWidth = CeilT<int32_t>(calcWidthExr, c0Size_);

    // gCol unaligned
    if (gCol % oriC0Size) {
        int blockLen = calcWidthExr * sizeof(SrcT) / DEFAULT_C0_SIZE;
        int dstOffset = 0;
        int BankConflictPadSize = isBankConflict ? (EACH_BLOCK_BYTES_MM_API / sizeof(SrcT)) : 0;

        // data copy stride is unaligned, need to copy line by line
        for (int i = 0; i < height; i++) {
            DataCopy(transTensor[dstOffset], src[srcOffset], {1, static_cast<uint16_t>(blockLen), 0, 0});
            dstOffset += (calcWidthExr + BankConflictPadSize);
            srcOffset += gCol;
        }

        auto enQueEvtID = GetTPipePtr()->FetchEventID(HardEvent::MTE2_V);
        SetFlag<HardEvent::MTE2_V>((event_t)enQueEvtID);
        WaitFlag<HardEvent::MTE2_V>((event_t)enQueEvtID);
    } else {
        int srcStride = (gCol - width) * sizeof(SrcT) / ONE_BLK_SIZE;
        int blocklen = CeilT<int32_t>(width * sizeof(SrcT), ONE_BLK_SIZE);
        if (srcStride >= UINT16_MAX) {
            int dstOffset = isBankConflict ? (width + oriC0Size) : width;
            for (int i = 0; i < height; ++i) {
                DataCopy(transTensor[i * dstOffset], src[srcOffset], {1, static_cast<uint16_t>(blocklen), 0, 0});
                srcOffset += gCol;
            }
        } else {
            uint16_t dstStride = isBankConflict ? 1 : 0;
            int loopNum = CeilT<int32_t>(static_cast<uint16_t>(height), MAX_BLOCK_COUNT_SIZE_MM_API);
            int tailCount = static_cast<uint16_t>(height) % MAX_BLOCK_COUNT_SIZE_MM_API;
            for (int i = 0; i < loopNum; ++i) {
                uint16_t blockCount = (i == loopNum - 1) ? tailCount : MAX_BLOCK_COUNT_SIZE_MM_API;
                DataCopy(
                    transTensor[i * MAX_BLOCK_COUNT_SIZE_MM_API * blocklen * ONE_BLK_SIZE / sizeof(SrcT)],
                    src[srcOffset + i * MAX_BLOCK_COUNT_SIZE_MM_API * blocklen * ONE_BLK_SIZE / sizeof(SrcT)],
                    {blockCount, static_cast<uint16_t>(blocklen), static_cast<uint16_t>(srcStride), dstStride});
            }
        }
        auto enQueEvtID = GetTPipePtr()->FetchEventID(HardEvent::MTE2_V);
        SetFlag<HardEvent::MTE2_V>((event_t)enQueEvtID);
        WaitFlag<HardEvent::MTE2_V>((event_t)enQueEvtID);
    }
    return calcWidth;
}
} // namespace Detail
} // namespace Impl
} // namespace AscendC
#endif // IMPL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_UTILS_H

#if defined( \
    __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_UTILS_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_DETAIL_MATMUL_STAGE_COPY_CUBE_IN_COPY_TILE_TO_CUBE_DATA_COPY_WRAPPER_UTILS_H__
#endif
