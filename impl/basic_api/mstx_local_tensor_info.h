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
 * \file mstx_local_tensor_info.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/mstx_local_tensor_info.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MSTX_LOCAL_TENSOR_INFO_H__
#endif
#ifndef MSTX_TENSOR_INFO_H
#define MSTX_TENSOR_INFO_H
#include "../../include/basic_api/kernel_tensor.h"
#include "../../include/basic_api/kernel_struct_unary.h"
#include "../../include/basic_api/kernel_struct_binary.h"
#include "../../include/basic_api/kernel_struct_brcb.h"
#include "../../include/basic_api/kernel_struct_gather.h"

#ifdef __MSTX_DFX_REPORT__

namespace AscendC {
namespace MstxTensor {

enum MstxTensorAddressSpace {
    MSTX_TENSOR_AT_PRIVATE = 0,
    MSTX_TENSOR_AT_GM,
    MSTX_TENSOR_AT_L1,
    MSTX_TENSOR_AT_L0A,
    MSTX_TENSOR_AT_L0B,
    MSTX_TENSOR_AT_L0C,
    MSTX_TENSOR_AT_UB,
};

struct MstxTensorDesc {
    MstxTensorAddressSpace space;
    uint64_t addr;
    uint64_t size;
    uint8_t dataBits;
};

template <typename T>
__aicore__ inline MstxTensorDesc From(const LocalTensor<T>& dst)
{
    MstxTensorAddressSpace space;
    Hardware dstHWPos = GetPhyType((TPosition)dst.GetPosition());
    if (dstHWPos == Hardware::GM) {
        space = MSTX_TENSOR_AT_GM;
    } else if (dstHWPos == Hardware::UB) {
        space = MSTX_TENSOR_AT_UB;
    } else if (dstHWPos == Hardware::L1) {
        space = MSTX_TENSOR_AT_L1;
    } else if (dstHWPos == Hardware::L0A) {
        space = MSTX_TENSOR_AT_L0A;
    } else if (dstHWPos == Hardware::L0B) {
        space = MSTX_TENSOR_AT_L0B;
    } else if (dstHWPos == Hardware::L0C) {
        space = MSTX_TENSOR_AT_L0C;
    } else {
        space = MSTX_TENSOR_AT_PRIVATE;
    }

    uint8_t sizebit;
    if constexpr (IsSameType<PrimT<T>, int4b_t>::value) {
        sizebit = static_cast<uint8_t>(4 * sizeof(PrimT<T>));
    } else {
        sizebit = static_cast<uint8_t>(8 * sizeof(PrimT<T>));
    }

    return MstxTensorDesc{space, reinterpret_cast<uint64_t>(dst.GetPhyAddr()), dst.GetSize(), sizebit};
}

template <typename T>
__aicore__ inline MstxTensorDesc FromGm(const GlobalTensor<T>& dst)
{
    uint8_t sizebit;
    if constexpr (IsSameType<PrimT<T>, int4b_t>::value) {
        sizebit = static_cast<uint8_t>(4 * sizeof(PrimT<T>));
    } else {
        sizebit = static_cast<uint8_t>(8 * sizeof(PrimT<T>));
    }
    return MstxTensorDesc{MSTX_TENSOR_AT_GM, reinterpret_cast<uint64_t>(dst.GetPhyAddr()), 0, sizebit};
}

enum class MstxReportType : uint32_t {
    MSTX_VEC_UNARY = 3000,
    MSTX_VEC_BINARY = 3001,
    MSTX_VEC_GATHER = 3002,
    MSTX_VEC_BINARY_SCALAR,
    MSTX_VEC_BILINEAR_INTERPOLATION,
    MSTX_VEC_TENARY,
    MSTX_VEC_CAST,
    MSTX_VEC_CASTDEQ,
    MSTX_VEC_SET_DEQ_SCALE,
    MSTX_VEC_CMP,
    MSTX_VEC_CMPS,
    MSTX_VEC_SEL,
    MSTX_VEC_GATHER_MASK,
    MSTX_VEC_TRANSDATA,
    MSTX_VEC_TRANSPOSE,
    MSTX_VEC_WHOLE_REDUCE,
    MSTX_VEC_BLK_REDUCE,
    MSTX_VEC_REDUCE,
    MSTX_VEC_PAIR_REDUCE,
    MSTX_VEC_REPEAT_REDUCE,
    MSTX_VEC_DUP,
    MSTX_VEC_BROADCAST,
    MSTX_VEC_VCI,
    MSTX_VEC_COPY,
    MSTX_DATA_COPY = 4001,
    MSTX_DATA_COPY_PAD = 4002,
    MSTX_DATA_COPY_PAD_V2 = 4003,
};

enum MstxMaskMode : uint32_t {
    MSTX_MASK_NORM = 0,
    MSTX_MASK_COUNT,
    MSTX_MASK_FROM_REG = 0xff,
};

struct MstxVectorMask {
    uint64_t mask0;
    uint64_t mask1;
};

struct MstxVecWrapper {
    MstxMaskMode maskMode;
    MstxVectorMask mask;
    uint32_t reserveBufSize;
    bool useMask;
};

__aicore__ inline MstxVecWrapper WrapperFrom(
    MstxMaskMode maskMode, uint64_t mask0, uint64_t mask1, bool isSetMask, uint32_t reserveBufSize)
{
    return MstxVecWrapper{maskMode, {mask0, mask1}, reserveBufSize, isSetMask};
}

template <typename T>
__aicore__ inline MstxVecWrapper WrapperFrom(
    MstxMaskMode maskMode, uint64_t mask, bool isSetMask, uint32_t reserveBufSize)
{
    int32_t typeLen = 0;
    constexpr int32_t halfTypeLen = 64; // 1 register -> 64 bits -> 64 elements
    constexpr int32_t lenCoeff = 2;
    uint64_t trueMask0, trueMask1;
    if constexpr (IsSameType<T, int4b_t>::value) {
        typeLen = DEFAULT_BLOCK_SIZE * INT4_TWO;
    } else {
        typeLen = DEFAULT_BLOCK_SIZE / sizeof(T);
    }
    if (mask == halfTypeLen) {
        trueMask0 = FULL_MASK;
        trueMask1 = 0;
    } else if (mask == typeLen || mask >= halfTypeLen * lenCoeff) {
        trueMask0 = FULL_MASK;
        trueMask1 = FULL_MASK;
    } else {
        trueMask0 =
            (mask > halfTypeLen) ? FULL_MASK : (((static_cast<uint64_t>(1)) << static_cast<uint32_t>(mask)) - 1);
        trueMask1 =
            (mask > halfTypeLen) ? (((static_cast<uint64_t>(1)) << static_cast<uint32_t>(mask - halfTypeLen)) - 1) : 0;
    }

    return MstxVecWrapper{maskMode, {trueMask0, trueMask1}, reserveBufSize, isSetMask};
}

struct MstxVecUnaryDesc {
    MstxTensorDesc dst;
    MstxTensorDesc src;
    MstxVecWrapper wrapper;
    uint32_t blockNum;
    uint32_t dstBlockStride;
    uint32_t srcBlockStride;
    uint32_t repeatTimes;
    uint32_t dstRepeatStride;
    uint32_t srcRepeatStride;
    char name[64];
};

struct MstxVecReduceDesc {
    MstxTensorDesc dst;
    MstxTensorDesc src;
    MstxVecWrapper wrapper;
    uint32_t srcBlockStride;
    uint32_t repeatTimes;
    uint32_t dstRepeatStride;
    uint32_t srcRepeatStride;
    char name[64];
};

struct MstxVecBrcbDesc {
    MstxTensorDesc dst;
    MstxTensorDesc src;
    MstxVecWrapper wrapper;
    uint32_t dstBlockStride;
    uint32_t repeatTimes;
    uint32_t dstRepeatStride;
    char name[64];
};

struct MstxVecComplexReduceDesc {
    MstxTensorDesc dst;
    MstxTensorDesc src;
    MstxTensorDesc tmp;
    MstxVecWrapper wrapper;
    uint32_t repeatTimes;
    uint32_t srcRepeatStride;
    char name[64];
};

struct MstxVecDupDesc {
    MstxTensorDesc dst;
    MstxVecWrapper wrapper;
    uint32_t dstBlockStride;
    uint32_t repeatTimes;
    uint32_t dstRepeatStride;
    char name[64];
};

struct MstxVecCopy {
    MstxTensorDesc dst;
    MstxTensorDesc src;
    MstxVecWrapper wrapper;
    uint32_t repeatTimes;
    uint32_t dstStride;
    uint32_t srcStride;
    uint32_t dstRepeatSize;
    uint32_t srcRepeatSize;
    char name[64];
};

struct MstxVecCastDeqDesc {
    MstxTensorDesc dst;
    MstxTensorDesc src;
    MstxVecWrapper wrapper;
    uint32_t blockNum;
    uint32_t dstBlockStride;
    uint32_t srcBlockStride;
    uint32_t repeatTimes;
    uint32_t dstRepeatStride;
    uint32_t srcRepeatStride;
    bool halfBlock;
    char name[64];
};

struct MstxDataCopyDesc {
    MstxTensorDesc dst;
    MstxTensorDesc src;
    uint32_t lenBurst;
    uint32_t nBurst;
    uint32_t srcGap;
    uint32_t dstGap;
    char name[64];
};

struct MstxDataCopyPadDesc {
    MstxTensorDesc dst;
    MstxTensorDesc src;
    uint32_t lenBurst;
    uint32_t nBurst;
    uint32_t srcGap;
    uint32_t dstGap;
    uint32_t leftPad;
    uint32_t rightPad;
    char name[64];
};

enum class MstxDataCopyPadMode : uint8_t {
    NORMAL = 0,
    COMPACT,
};

struct MstxDataCopyPadV2Desc {
    MstxTensorDesc dst;
    MstxTensorDesc src;
    MstxDataCopyPadMode padMode;
    uint32_t lenBurst;
    uint32_t nBurst;
    uint32_t srcGap;
    uint32_t dstGap;
    uint32_t leftPad;
    uint32_t rightPad;
    char name[64];
};

struct MstxVecBinaryDesc {
    MstxTensorDesc dst;
    MstxTensorDesc src0;
    MstxTensorDesc src1;
    MstxVecWrapper wrapper;
    uint32_t blockNum;
    uint32_t dstBlockStride;
    uint32_t src0BlockStride;
    uint32_t src1BlockStride;
    uint32_t repeatTimes;
    uint32_t dstRepeatStride;
    uint32_t src0RepeatStride;
    uint32_t src1RepeatStride;
    char name[64];
};

struct MstxVecBilinearInterpolation {
    MstxTensorDesc dst;
    MstxTensorDesc src0;
    MstxTensorDesc src1;
    MstxTensorDesc src0Offset;
    MstxTensorDesc shared;
    MstxVecWrapper wrapper;
    uint32_t hRepeat;
    bool repeatMode;
    uint32_t dstBlockStride;
    uint32_t vROffset;
    uint32_t vRepeat;
    char name[64];
};

struct MstxVecTranspose {
    MstxTensorDesc dst;
    MstxTensorDesc src;
    MstxTensorDesc shared;
    bool temp;
    char name[64];
};

struct MstxVecSelDesc {
    MstxTensorDesc dst;
    MstxTensorDesc src0;
    MstxTensorDesc src1;
    MstxTensorDesc mask;
    MstxVecWrapper wrapper;
    bool scalarMode;
    uint32_t blockNum;
    uint32_t dstBlockStride;
    uint32_t src0BlockStride;
    uint32_t src1BlockStride;
    uint32_t repeatTimes;
    uint32_t dstRepeatStride;
    uint32_t src0RepeatStride;
    uint32_t src1RepeatStride;
    char name[64];
};

enum class MstxGatherMaskMode { V1, V2 };

struct MstxVecGatherMaskDesc {
    MstxTensorDesc dst;
    MstxTensorDesc src;
    MstxVecWrapper wrapper;
    MstxGatherMaskMode mode;
    uint32_t repeatTimes;
    uint32_t src0BlockStride;
    uint32_t src0RepeatStride;
    uint32_t src1RepeatStride;
    char name[64];
};

__aicore__ inline void CopyName(char b[64], __gm__ const char* a)
{
    uint32_t i = 0;
    for (; i < 63; ++i) {
        b[i] = a[i];
        if (a[i] == '\0') {
            break;
        }
    }
    b[i] = '\0';
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void GetMstxVecUnaryInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, uint64_t mask0, uint64_t mask1, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, __gm__ const char* name)
{
    MstxVecUnaryDesc mstxVecUnaryDesc;
    mstxVecUnaryDesc.dst = From(dst);
    mstxVecUnaryDesc.src = From(src);
    mstxVecUnaryDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, isSetMask, static_cast<uint32_t>(0));
    mstxVecUnaryDesc.blockNum = repeatParams.blockNumber;
    mstxVecUnaryDesc.repeatTimes = repeatTime;
    mstxVecUnaryDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecUnaryDesc.srcBlockStride = repeatParams.srcBlkStride;
    mstxVecUnaryDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecUnaryDesc.srcRepeatStride = repeatParams.srcRepStride;
    CopyName(mstxVecUnaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_UNARY), sizeof(mstxVecUnaryDesc), &mstxVecUnaryDesc);
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void GetMstxVecUnaryInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, __gm__ const char* name)
{
    MstxVecUnaryDesc mstxVecUnaryDesc;
    mstxVecUnaryDesc.dst = From(dst);
    mstxVecUnaryDesc.src = From(src);
    mstxVecUnaryDesc.wrapper = WrapperFrom<T>(MSTX_MASK_FROM_REG, mask, isSetMask, static_cast<uint32_t>(0));
    mstxVecUnaryDesc.blockNum = repeatParams.blockNumber;
    mstxVecUnaryDesc.repeatTimes = repeatTime;
    mstxVecUnaryDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecUnaryDesc.srcBlockStride = repeatParams.srcBlkStride;
    mstxVecUnaryDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecUnaryDesc.srcRepeatStride = repeatParams.srcRepStride;
    CopyName(mstxVecUnaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_UNARY), sizeof(mstxVecUnaryDesc), &mstxVecUnaryDesc);
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void GetMstxVecUnaryInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, __gm__ const char* name, const int32_t count)
{
    MstxVecUnaryDesc mstxVecUnaryDesc;
    mstxVecUnaryDesc.dst = From(dst);
    mstxVecUnaryDesc.src = From(src);
    mstxVecUnaryDesc.wrapper =
        WrapperFrom(MSTX_MASK_COUNT, count, static_cast<uint64_t>(0), isSetMask, static_cast<uint32_t>(0));
    mstxVecUnaryDesc.blockNum = 8;
    mstxVecUnaryDesc.repeatTimes = 1;
    mstxVecUnaryDesc.dstBlockStride = 1;
    mstxVecUnaryDesc.srcBlockStride = 1;
    mstxVecUnaryDesc.dstRepeatStride = 8;
    mstxVecUnaryDesc.srcRepeatStride = 8;
    CopyName(mstxVecUnaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_UNARY), sizeof(mstxVecUnaryDesc), &mstxVecUnaryDesc);
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void GetMstxVecUnaryTenaryInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, uint64_t mask0, uint64_t mask1, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, __gm__ const char* name)
{
    MstxVecUnaryDesc mstxVecUnaryDesc;
    mstxVecUnaryDesc.dst = From(dst);
    mstxVecUnaryDesc.src = From(src);
    mstxVecUnaryDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, isSetMask, static_cast<uint32_t>(0));
    mstxVecUnaryDesc.blockNum = repeatParams.blockNumber;
    mstxVecUnaryDesc.repeatTimes = repeatTime;
    mstxVecUnaryDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecUnaryDesc.srcBlockStride = repeatParams.srcBlkStride;
    mstxVecUnaryDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecUnaryDesc.srcRepeatStride = repeatParams.srcRepStride;
    CopyName(mstxVecUnaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_TENARY), sizeof(mstxVecUnaryDesc), &mstxVecUnaryDesc);
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void GetMstxVecUnaryTenaryInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, __gm__ const char* name)
{
    MstxVecUnaryDesc mstxVecUnaryDesc;
    mstxVecUnaryDesc.dst = From(dst);
    mstxVecUnaryDesc.src = From(src);
    mstxVecUnaryDesc.wrapper = WrapperFrom<T>(MSTX_MASK_FROM_REG, mask, isSetMask, static_cast<uint32_t>(0));
    mstxVecUnaryDesc.blockNum = repeatParams.blockNumber;
    mstxVecUnaryDesc.repeatTimes = repeatTime;
    mstxVecUnaryDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecUnaryDesc.srcBlockStride = repeatParams.srcBlkStride;
    mstxVecUnaryDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecUnaryDesc.srcRepeatStride = repeatParams.srcRepStride;
    CopyName(mstxVecUnaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_TENARY), sizeof(mstxVecUnaryDesc), &mstxVecUnaryDesc);
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void GetMstxVecUnaryTenaryInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, __gm__ const char* name, const int32_t count)
{
    MstxVecUnaryDesc mstxVecUnaryDesc;
    mstxVecUnaryDesc.dst = From(dst);
    mstxVecUnaryDesc.src = From(src);
    mstxVecUnaryDesc.wrapper =
        WrapperFrom(MSTX_MASK_COUNT, count, static_cast<uint64_t>(0), isSetMask, static_cast<uint32_t>(0));
    mstxVecUnaryDesc.blockNum = 8;
    mstxVecUnaryDesc.repeatTimes = 1;
    mstxVecUnaryDesc.dstBlockStride = 1;
    mstxVecUnaryDesc.srcBlockStride = 1;
    mstxVecUnaryDesc.dstRepeatStride = 8;
    if constexpr (sizeof(T) > sizeof(U)) {
        mstxVecUnaryDesc.srcRepeatStride = DEFAULT_REPEAT_STRIDE / 2;
    } else {
        mstxVecUnaryDesc.srcRepeatStride = DEFAULT_REPEAT_STRIDE;
    }
    CopyName(mstxVecUnaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_TENARY), sizeof(mstxVecUnaryDesc), &mstxVecUnaryDesc);
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void GetMstxVecUnaryCastInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, uint64_t mask0, uint64_t mask1, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, __gm__ const char* name)
{
    MstxVecUnaryDesc mstxVecUnaryDesc;
    mstxVecUnaryDesc.dst = From(dst);
    mstxVecUnaryDesc.src = From(src);
    mstxVecUnaryDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, isSetMask, static_cast<uint32_t>(0));
    mstxVecUnaryDesc.blockNum = repeatParams.blockNumber;
    mstxVecUnaryDesc.repeatTimes = repeatTime;
    mstxVecUnaryDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecUnaryDesc.srcBlockStride = repeatParams.srcBlkStride;
    mstxVecUnaryDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecUnaryDesc.srcRepeatStride = repeatParams.srcRepStride;
    CopyName(mstxVecUnaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_CAST), sizeof(mstxVecUnaryDesc), &mstxVecUnaryDesc);
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void GetMstxVecUnaryCastInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, __gm__ const char* name)
{
    MstxVecUnaryDesc mstxVecUnaryDesc;
    mstxVecUnaryDesc.dst = From(dst);
    mstxVecUnaryDesc.src = From(src);
    mstxVecUnaryDesc.wrapper = WrapperFrom<T>(MSTX_MASK_FROM_REG, mask, isSetMask, static_cast<uint32_t>(0));
    mstxVecUnaryDesc.blockNum = repeatParams.blockNumber;
    mstxVecUnaryDesc.repeatTimes = repeatTime;
    mstxVecUnaryDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecUnaryDesc.srcBlockStride = repeatParams.srcBlkStride;
    mstxVecUnaryDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecUnaryDesc.srcRepeatStride = repeatParams.srcRepStride;
    CopyName(mstxVecUnaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_CAST), sizeof(mstxVecUnaryDesc), &mstxVecUnaryDesc);
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void GetMstxVecUnaryCastInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, __gm__ const char* name, const int32_t count)
{
    MstxVecUnaryDesc mstxVecUnaryDesc;
    mstxVecUnaryDesc.dst = From(dst);
    mstxVecUnaryDesc.src = From(src);
    mstxVecUnaryDesc.wrapper =
        WrapperFrom(MSTX_MASK_COUNT, count, static_cast<uint64_t>(0), isSetMask, static_cast<uint32_t>(0));
    mstxVecUnaryDesc.blockNum = 8;
    mstxVecUnaryDesc.repeatTimes = 1;
    mstxVecUnaryDesc.dstBlockStride = 1;
    mstxVecUnaryDesc.srcBlockStride = 1;
    if constexpr (sizeof(T) > sizeof(U)) {
        if constexpr (IsSameType<U, int4b_t>::value) {
            mstxVecUnaryDesc.dstRepeatStride = DEFAULT_REPEAT_STRIDE;
            mstxVecUnaryDesc.srcRepeatStride = ONE_FOURTH_DEFAULT_REPEAT_STRIDE;
        } else {
            mstxVecUnaryDesc.dstRepeatStride = DEFAULT_REPEAT_STRIDE;
            mstxVecUnaryDesc.srcRepeatStride = DEFAULT_REPEAT_STRIDE / 2;
        }
    } else if constexpr (sizeof(T) < sizeof(U)) {
        if constexpr (IsSameType<T, int4b_t>::value) {
            mstxVecUnaryDesc.dstRepeatStride = ONE_FOURTH_DEFAULT_REPEAT_STRIDE;
            mstxVecUnaryDesc.srcRepeatStride = DEFAULT_REPEAT_STRIDE;
        } else {
            mstxVecUnaryDesc.dstRepeatStride = DEFAULT_REPEAT_STRIDE / 2;
            mstxVecUnaryDesc.srcRepeatStride = DEFAULT_REPEAT_STRIDE;
        }
    } else {
        mstxVecUnaryDesc.dstRepeatStride = DEFAULT_REPEAT_STRIDE;
        mstxVecUnaryDesc.srcRepeatStride = DEFAULT_REPEAT_STRIDE;
    }
    CopyName(mstxVecUnaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_CAST), sizeof(mstxVecUnaryDesc), &mstxVecUnaryDesc);
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void GetMstxVecUnaryCastDeqInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, uint64_t mask0, uint64_t mask1, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, __gm__ const char* name, bool halfBlock)
{
    MstxVecCastDeqDesc mstxVecCastDeqDesc;
    mstxVecCastDeqDesc.dst = From(dst);
    mstxVecCastDeqDesc.src = From(src);
    mstxVecCastDeqDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, isSetMask, static_cast<uint32_t>(0));
    mstxVecCastDeqDesc.blockNum = repeatParams.blockNumber;
    mstxVecCastDeqDesc.repeatTimes = repeatTime;
    mstxVecCastDeqDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecCastDeqDesc.srcBlockStride = repeatParams.srcBlkStride;
    mstxVecCastDeqDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecCastDeqDesc.srcRepeatStride = repeatParams.srcRepStride;
    mstxVecCastDeqDesc.halfBlock = halfBlock;
    CopyName(mstxVecCastDeqDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_CASTDEQ), sizeof(mstxVecCastDeqDesc), &mstxVecCastDeqDesc);
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void GetMstxVecUnaryCastDeqInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, __gm__ const char* name, bool halfBlock)
{
    MstxVecCastDeqDesc mstxVecCastDeqDesc;
    mstxVecCastDeqDesc.dst = From(dst);
    mstxVecCastDeqDesc.src = From(src);
    mstxVecCastDeqDesc.wrapper = WrapperFrom<U>(MSTX_MASK_FROM_REG, mask, isSetMask, static_cast<uint32_t>(0));
    mstxVecCastDeqDesc.blockNum = repeatParams.blockNumber;
    mstxVecCastDeqDesc.repeatTimes = repeatTime;
    mstxVecCastDeqDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecCastDeqDesc.srcBlockStride = repeatParams.srcBlkStride;
    mstxVecCastDeqDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecCastDeqDesc.srcRepeatStride = repeatParams.srcRepStride;
    mstxVecCastDeqDesc.halfBlock = halfBlock;
    CopyName(mstxVecCastDeqDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_CASTDEQ), sizeof(mstxVecCastDeqDesc), &mstxVecCastDeqDesc);
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void GetMstxVecUnaryCastDeqInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, __gm__ const char* name, const int32_t count, bool halfBlock)
{
    MstxVecCastDeqDesc mstxVecCastDeqDesc;
    mstxVecCastDeqDesc.dst = From(dst);
    mstxVecCastDeqDesc.src = From(src);
    mstxVecCastDeqDesc.wrapper =
        WrapperFrom(MSTX_MASK_COUNT, count, static_cast<uint64_t>(0), isSetMask, static_cast<uint32_t>(0));
    mstxVecCastDeqDesc.blockNum = 8;
    mstxVecCastDeqDesc.repeatTimes = 1;
    mstxVecCastDeqDesc.dstBlockStride = 1;
    mstxVecCastDeqDesc.srcBlockStride = 1;
    mstxVecCastDeqDesc.dstRepeatStride = 8;
    mstxVecCastDeqDesc.srcRepeatStride = 8;
    if constexpr (sizeof(T) > sizeof(U)) {
        if constexpr (IsSameType<U, int4b_t>::value) {
            mstxVecCastDeqDesc.dstRepeatStride = DEFAULT_REPEAT_STRIDE;
            mstxVecCastDeqDesc.srcRepeatStride = ONE_FOURTH_DEFAULT_REPEAT_STRIDE;
        } else {
            mstxVecCastDeqDesc.dstRepeatStride = DEFAULT_REPEAT_STRIDE;
            mstxVecCastDeqDesc.srcRepeatStride = DEFAULT_REPEAT_STRIDE / 2;
        }
    } else if constexpr (sizeof(T) < sizeof(U)) {
        if constexpr (IsSameType<T, int4b_t>::value) {
            mstxVecCastDeqDesc.dstRepeatStride = ONE_FOURTH_DEFAULT_REPEAT_STRIDE;
            mstxVecCastDeqDesc.srcRepeatStride = DEFAULT_REPEAT_STRIDE;
        } else {
            mstxVecCastDeqDesc.dstRepeatStride = DEFAULT_REPEAT_STRIDE / 2;
            mstxVecCastDeqDesc.srcRepeatStride = DEFAULT_REPEAT_STRIDE;
        }
    } else {
        mstxVecCastDeqDesc.dstRepeatStride = DEFAULT_REPEAT_STRIDE;
        mstxVecCastDeqDesc.srcRepeatStride = DEFAULT_REPEAT_STRIDE;
    }

    mstxVecCastDeqDesc.halfBlock = halfBlock;
    CopyName(mstxVecCastDeqDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_CASTDEQ), sizeof(mstxVecCastDeqDesc), &mstxVecCastDeqDesc);
}

// comparescalar双目的
template <typename T, typename U, bool isSetMask>
__aicore__ inline void GetMstxVecUnaryCmpsInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, uint64_t mask0, uint64_t mask1, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, __gm__ const char* name)
{
    MstxVecUnaryDesc mstxVecUnaryDesc;
    mstxVecUnaryDesc.dst = From(dst);
    mstxVecUnaryDesc.src = From(src);
    mstxVecUnaryDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, isSetMask, static_cast<uint32_t>(0));
    mstxVecUnaryDesc.blockNum = repeatParams.blockNumber;
    mstxVecUnaryDesc.repeatTimes = repeatTime;
    mstxVecUnaryDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecUnaryDesc.srcBlockStride = repeatParams.srcBlkStride;
    mstxVecUnaryDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecUnaryDesc.srcRepeatStride = repeatParams.srcRepStride;
    CopyName(mstxVecUnaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_CMPS), sizeof(mstxVecUnaryDesc), &mstxVecUnaryDesc);
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void GetMstxVecUnaryCmpsInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, __gm__ const char* name)
{
    MstxVecUnaryDesc mstxVecUnaryDesc;
    mstxVecUnaryDesc.dst = From(dst);
    mstxVecUnaryDesc.src = From(src);
    mstxVecUnaryDesc.wrapper = WrapperFrom<T>(MSTX_MASK_FROM_REG, mask, isSetMask, static_cast<uint32_t>(0));
    mstxVecUnaryDesc.blockNum = repeatParams.blockNumber;
    mstxVecUnaryDesc.repeatTimes = repeatTime;
    mstxVecUnaryDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecUnaryDesc.srcBlockStride = repeatParams.srcBlkStride;
    mstxVecUnaryDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecUnaryDesc.srcRepeatStride = repeatParams.srcRepStride;
    CopyName(mstxVecUnaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_CMPS), sizeof(mstxVecUnaryDesc), &mstxVecUnaryDesc);
}

template <typename T, typename U, bool isSetMask>
__aicore__ inline void GetMstxVecUnaryCmpsInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, __gm__ const char* name, const int32_t count)
{
    MstxVecUnaryDesc mstxVecUnaryDesc;
    mstxVecUnaryDesc.dst = From(dst);
    mstxVecUnaryDesc.src = From(src);
    mstxVecUnaryDesc.wrapper =
        WrapperFrom(MSTX_MASK_COUNT, count, static_cast<uint64_t>(0), isSetMask, static_cast<uint32_t>(0));
    mstxVecUnaryDesc.blockNum = 8;
    mstxVecUnaryDesc.repeatTimes = 1;
    mstxVecUnaryDesc.dstBlockStride = 1;
    mstxVecUnaryDesc.srcBlockStride = 1;
    mstxVecUnaryDesc.dstRepeatStride = 8;
    mstxVecUnaryDesc.srcRepeatStride = 8;
    CopyName(mstxVecUnaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_CMPS), sizeof(mstxVecUnaryDesc), &mstxVecUnaryDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxVecReduceInfo(
    const LocalTensor<U>& dst, const LocalTensor<T>& src, uint64_t mask0, uint64_t mask1, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, bool isSetMask,
    __gm__ const char* name)
{
    MstxVecReduceDesc mstxVecReduceDesc;
    mstxVecReduceDesc.dst = From(dst);
    mstxVecReduceDesc.src = From(src);
    mstxVecReduceDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, isSetMask, static_cast<uint32_t>(0));
    mstxVecReduceDesc.repeatTimes = repeatTime;
    mstxVecReduceDesc.srcBlockStride = srcBlkStride;
    mstxVecReduceDesc.dstRepeatStride = dstRepStride;
    mstxVecReduceDesc.srcRepeatStride = srcRepStride;
    CopyName(mstxVecReduceDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_WHOLE_REDUCE), sizeof(mstxVecReduceDesc), &mstxVecReduceDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxVecReduceInfo(
    const LocalTensor<U>& dst, const LocalTensor<T>& src, uint64_t mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, bool isSetMask,
    __gm__ const char* name)
{
    MstxVecReduceDesc mstxVecReduceDesc;
    mstxVecReduceDesc.dst = From(dst);
    mstxVecReduceDesc.src = From(src);
    mstxVecReduceDesc.wrapper = WrapperFrom<T>(MSTX_MASK_FROM_REG, mask, isSetMask, static_cast<uint32_t>(0));
    mstxVecReduceDesc.repeatTimes = repeatTime;
    mstxVecReduceDesc.srcBlockStride = srcBlkStride;
    mstxVecReduceDesc.dstRepeatStride = dstRepStride;
    mstxVecReduceDesc.srcRepeatStride = srcRepStride;
    CopyName(mstxVecReduceDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_WHOLE_REDUCE), sizeof(mstxVecReduceDesc), &mstxVecReduceDesc);
}

template <typename T>
__aicore__ inline void GetMstxVecCopyInfo(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask0, uint64_t mask1, const int32_t repeatTime,
    const CopyRepeatParams& repeatParams, bool isSetMask, __gm__ const char* name)
{
    MstxVecCopy mstxVecCopy;
    mstxVecCopy.dst = From(dst);
    mstxVecCopy.src = From(src);
    mstxVecCopy.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, isSetMask, static_cast<uint32_t>(0));
    mstxVecCopy.repeatTimes = repeatTime;
    mstxVecCopy.dstStride = repeatParams.dstStride;
    mstxVecCopy.srcStride = repeatParams.srcStride;
    mstxVecCopy.dstRepeatSize = repeatParams.dstRepeatSize;
    mstxVecCopy.srcRepeatSize = repeatParams.srcRepeatSize;
    CopyName(mstxVecCopy.name, name);

    __mstx_dfx_report_stub(static_cast<uint32_t>(MstxReportType::MSTX_VEC_COPY), sizeof(mstxVecCopy), &mstxVecCopy);
}

template <typename T>
__aicore__ inline void GetMstxVecCopyInfo(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const int32_t repeatTime,
    const CopyRepeatParams& repeatParams, bool isSetMask, __gm__ const char* name)
{
    MstxVecCopy mstxVecCopy;
    mstxVecCopy.dst = From(dst);
    mstxVecCopy.src = From(src);
    mstxVecCopy.wrapper = WrapperFrom<T>(MSTX_MASK_FROM_REG, mask, isSetMask, static_cast<uint32_t>(0));
    mstxVecCopy.repeatTimes = repeatTime;
    mstxVecCopy.dstStride = repeatParams.dstStride;
    mstxVecCopy.srcStride = repeatParams.srcStride;
    mstxVecCopy.dstRepeatSize = repeatParams.dstRepeatSize;
    mstxVecCopy.srcRepeatSize = repeatParams.srcRepeatSize;
    CopyName(mstxVecCopy.name, name);

    __mstx_dfx_report_stub(static_cast<uint32_t>(MstxReportType::MSTX_VEC_COPY), sizeof(mstxVecCopy), &mstxVecCopy);
}

template <typename T, typename U>
__aicore__ inline void GetMstxVecReduceBlkInfo(
    const LocalTensor<U>& dst, const LocalTensor<T>& src, uint64_t mask0, uint64_t mask1, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, bool isSetMask,
    __gm__ const char* name)
{
    MstxVecReduceDesc mstxVecReduceDesc;
    mstxVecReduceDesc.dst = From(dst);
    mstxVecReduceDesc.src = From(src);
    mstxVecReduceDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, isSetMask, static_cast<uint32_t>(0));
    mstxVecReduceDesc.repeatTimes = repeatTime;
    mstxVecReduceDesc.srcBlockStride = srcBlkStride;
    mstxVecReduceDesc.dstRepeatStride = dstRepStride;
    mstxVecReduceDesc.srcRepeatStride = srcRepStride;
    CopyName(mstxVecReduceDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_BLK_REDUCE), sizeof(mstxVecReduceDesc), &mstxVecReduceDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxVecReduceBlkInfo(
    const LocalTensor<U>& dst, const LocalTensor<T>& src, uint64_t mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, bool isSetMask,
    __gm__ const char* name)
{
    MstxVecReduceDesc mstxVecReduceDesc;
    mstxVecReduceDesc.dst = From(dst);
    mstxVecReduceDesc.src = From(src);
    mstxVecReduceDesc.wrapper = WrapperFrom<T>(MSTX_MASK_FROM_REG, mask, isSetMask, static_cast<uint32_t>(0));
    mstxVecReduceDesc.repeatTimes = repeatTime;
    mstxVecReduceDesc.srcBlockStride = srcBlkStride;
    mstxVecReduceDesc.dstRepeatStride = dstRepStride;
    mstxVecReduceDesc.srcRepeatStride = srcRepStride;
    CopyName(mstxVecReduceDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_BLK_REDUCE), sizeof(mstxVecReduceDesc), &mstxVecReduceDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxVecReducePairInfo(
    const LocalTensor<U>& dst, const LocalTensor<T>& src, uint64_t mask0, uint64_t mask1, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, bool isSetMask,
    __gm__ const char* name)
{
    MstxVecReduceDesc mstxVecReduceDesc;
    mstxVecReduceDesc.dst = From(dst);
    mstxVecReduceDesc.src = From(src);
    mstxVecReduceDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, isSetMask, static_cast<uint32_t>(0));
    mstxVecReduceDesc.repeatTimes = repeatTime;
    mstxVecReduceDesc.srcBlockStride = srcBlkStride;
    mstxVecReduceDesc.dstRepeatStride = dstRepStride;
    mstxVecReduceDesc.srcRepeatStride = srcRepStride;
    CopyName(mstxVecReduceDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_PAIR_REDUCE), sizeof(mstxVecReduceDesc), &mstxVecReduceDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxVecReducePairInfo(
    const LocalTensor<U>& dst, const LocalTensor<T>& src, uint64_t mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, bool isSetMask,
    __gm__ const char* name)
{
    MstxVecReduceDesc mstxVecReduceDesc;
    mstxVecReduceDesc.dst = From(dst);
    mstxVecReduceDesc.src = From(src);
    mstxVecReduceDesc.wrapper = WrapperFrom<T>(MSTX_MASK_FROM_REG, mask, isSetMask, static_cast<uint32_t>(0));
    mstxVecReduceDesc.repeatTimes = repeatTime;
    mstxVecReduceDesc.srcBlockStride = srcBlkStride;
    mstxVecReduceDesc.dstRepeatStride = dstRepStride;
    mstxVecReduceDesc.srcRepeatStride = srcRepStride;
    CopyName(mstxVecReduceDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_PAIR_REDUCE), sizeof(mstxVecReduceDesc), &mstxVecReduceDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxVecReduceRepeatInfo(
    const LocalTensor<U>& dst, const LocalTensor<T>& src, uint64_t mask0, uint64_t mask1, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, bool isSetMask,
    __gm__ const char* name)
{
    MstxVecReduceDesc mstxVecReduceDesc;
    mstxVecReduceDesc.dst = From(dst);
    mstxVecReduceDesc.src = From(src);
    mstxVecReduceDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, isSetMask, static_cast<uint32_t>(0));
    mstxVecReduceDesc.repeatTimes = repeatTime;
    mstxVecReduceDesc.srcBlockStride = srcBlkStride;
    mstxVecReduceDesc.dstRepeatStride = dstRepStride;
    mstxVecReduceDesc.srcRepeatStride = srcRepStride;
    CopyName(mstxVecReduceDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_REPEAT_REDUCE), sizeof(mstxVecReduceDesc), &mstxVecReduceDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxVecReduceRepeatInfo(
    const LocalTensor<U>& dst, const LocalTensor<T>& src, uint64_t mask, const int32_t repeatTime,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, bool isSetMask,
    __gm__ const char* name)
{
    MstxVecReduceDesc mstxVecReduceDesc;
    mstxVecReduceDesc.dst = From(dst);
    mstxVecReduceDesc.src = From(src);
    mstxVecReduceDesc.wrapper = WrapperFrom<T>(MSTX_MASK_FROM_REG, mask, isSetMask, static_cast<uint32_t>(0));
    mstxVecReduceDesc.repeatTimes = repeatTime;
    mstxVecReduceDesc.srcBlockStride = srcBlkStride;
    mstxVecReduceDesc.dstRepeatStride = dstRepStride;
    mstxVecReduceDesc.srcRepeatStride = srcRepStride;
    CopyName(mstxVecReduceDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_REPEAT_REDUCE), sizeof(mstxVecReduceDesc), &mstxVecReduceDesc);
}

template <typename T>
__aicore__ inline void GetMstxVecBrcbInfo(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint8_t repeatTime,
    const BrcbRepeatParams& repeatParams, __gm__ const char* name)
{
    MstxVecBrcbDesc mstxVecBrcbDesc;
    mstxVecBrcbDesc.dst = From(dst);
    mstxVecBrcbDesc.src = From(src);
    mstxVecBrcbDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, 0, 0, false, static_cast<uint32_t>(0));
    mstxVecBrcbDesc.repeatTimes = repeatTime;
    mstxVecBrcbDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecBrcbDesc.dstRepeatStride = repeatParams.dstRepStride;
    CopyName(mstxVecBrcbDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_BROADCAST), sizeof(mstxVecBrcbDesc), &mstxVecBrcbDesc);
}

template <typename T>
__aicore__ inline void GetMstxVecTransposeInfo(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, __gm__ const char* name)
{
    MstxVecTranspose mstxVecTranspose;
    mstxVecTranspose.dst = From(dst);
    mstxVecTranspose.src = From(src);
    mstxVecTranspose.temp = false;
    CopyName(mstxVecTranspose.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_TRANSPOSE), sizeof(mstxVecTranspose), &mstxVecTranspose);
}

template <typename T, typename U>
__aicore__ inline void GetMstxVecTransposeTempInfo(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<U>& sharedTmpBuffer,
    __gm__ const char* name)
{
    MstxVecTranspose mstxVecTranspose;
    mstxVecTranspose.dst = From(dst);
    mstxVecTranspose.src = From(src);
    mstxVecTranspose.shared = From(sharedTmpBuffer);
    mstxVecTranspose.temp = true;
    CopyName(mstxVecTranspose.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_TRANSPOSE), sizeof(mstxVecTranspose), &mstxVecTranspose);
}

template <typename T>
__aicore__ inline void GetMstxVecDupInfo(
    const LocalTensor<T>& dst, uint64_t mask0, uint64_t mask1, const uint8_t repeatTime, const uint16_t dstBlockStride,
    const uint8_t dstRepeatStride, bool isSetMask, __gm__ const char* name)
{
    MstxVecDupDesc mstxVecDupDesc;
    mstxVecDupDesc.dst = From(dst);
    mstxVecDupDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, isSetMask, static_cast<uint32_t>(0));
    mstxVecDupDesc.repeatTimes = repeatTime;
    mstxVecDupDesc.dstBlockStride = dstBlockStride;
    mstxVecDupDesc.dstRepeatStride = dstRepeatStride;
    CopyName(mstxVecDupDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_DUP), sizeof(mstxVecDupDesc), &mstxVecDupDesc);
}

template <typename T>
__aicore__ inline void GetMstxVecDupInfo(
    const LocalTensor<T>& dst, uint64_t mask, const uint8_t repeatTime, const uint16_t dstBlockStride,
    const uint8_t dstRepeatStride, bool isSetMask, __gm__ const char* name)
{
    MstxVecDupDesc mstxVecDupDesc;
    mstxVecDupDesc.dst = From(dst);
    mstxVecDupDesc.wrapper = WrapperFrom<T>(MSTX_MASK_FROM_REG, mask, isSetMask, static_cast<uint32_t>(0));
    mstxVecDupDesc.repeatTimes = repeatTime;
    mstxVecDupDesc.dstBlockStride = dstBlockStride;
    mstxVecDupDesc.dstRepeatStride = dstRepeatStride;
    CopyName(mstxVecDupDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_DUP), sizeof(mstxVecDupDesc), &mstxVecDupDesc);
}

template <typename T>
__aicore__ inline void GetMstxVecDupInfo(const LocalTensor<T>& dst, const int32_t& count, __gm__ const char* name)
{
    MstxVecDupDesc mstxVecDupDesc;
    mstxVecDupDesc.dst = From(dst);
    mstxVecDupDesc.wrapper =
        WrapperFrom(MSTX_MASK_COUNT, count, static_cast<uint64_t>(0), true, static_cast<uint32_t>(0));
    mstxVecDupDesc.repeatTimes = 1;
    mstxVecDupDesc.dstBlockStride = 1;
    mstxVecDupDesc.dstRepeatStride = 8;
    CopyName(mstxVecDupDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_DUP), sizeof(mstxVecDupDesc), &mstxVecDupDesc);
}

template <typename T>
__aicore__ inline void GetMstxVecIndexInfo(
    const LocalTensor<T>& dst, uint64_t mask0, uint64_t mask1, const uint8_t repeatTime, const uint16_t dstBlockStride,
    const uint8_t dstRepeatStride, __gm__ const char* name)
{
    MstxVecDupDesc mstxVecDupDesc;
    mstxVecDupDesc.dst = From(dst);
    mstxVecDupDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, true, static_cast<uint32_t>(8192));
    mstxVecDupDesc.repeatTimes = repeatTime;
    mstxVecDupDesc.dstBlockStride = dstBlockStride;
    mstxVecDupDesc.dstRepeatStride = dstRepeatStride;
    CopyName(mstxVecDupDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_VCI), sizeof(mstxVecDupDesc), &mstxVecDupDesc);
}

template <typename T>
__aicore__ inline void GetMstxVecIndexInfo(
    const LocalTensor<T>& dst, uint64_t mask, const uint8_t repeatTime, const uint16_t dstBlockStride,
    const uint8_t dstRepeatStride, __gm__ const char* name)
{
    MstxVecDupDesc mstxVecDupDesc;
    mstxVecDupDesc.dst = From(dst);
    mstxVecDupDesc.wrapper = WrapperFrom<T>(MSTX_MASK_FROM_REG, mask, true, static_cast<uint32_t>(8192));
    mstxVecDupDesc.repeatTimes = repeatTime;
    mstxVecDupDesc.dstBlockStride = dstBlockStride;
    mstxVecDupDesc.dstRepeatStride = dstRepeatStride;
    CopyName(mstxVecDupDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_VCI), sizeof(mstxVecDupDesc), &mstxVecDupDesc);
}

template <typename T>
__aicore__ inline void GetMstxVecIndexInfo(const LocalTensor<T>& dst, uint32_t count, __gm__ const char* name)
{
    MstxVecDupDesc mstxVecDupDesc;
    mstxVecDupDesc.dst = From(dst);
    mstxVecDupDesc.wrapper =
        WrapperFrom(MSTX_MASK_COUNT, count, static_cast<uint64_t>(0), true, static_cast<uint32_t>(8192));
    mstxVecDupDesc.repeatTimes = 1;
    mstxVecDupDesc.dstBlockStride = 1;
    mstxVecDupDesc.dstRepeatStride = 8;
    CopyName(mstxVecDupDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_VCI), sizeof(mstxVecDupDesc), &mstxVecDupDesc);
}

// 软仿
template <typename T, typename U>
__aicore__ inline void GetMstxVecReduceComplexInfo(
    const LocalTensor<U>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t mask0,
    const int32_t mask1, const int32_t repeatTime, const int32_t srcRepStride, __gm__ const char* name)
{
    MstxVecComplexReduceDesc mstxVecComplexReduceDesc;
    mstxVecComplexReduceDesc.dst = From(dst);
    mstxVecComplexReduceDesc.src = From(src);
    mstxVecComplexReduceDesc.tmp = From(sharedTmpBuffer);
    mstxVecComplexReduceDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, true, static_cast<uint32_t>(8192));
    mstxVecComplexReduceDesc.repeatTimes = repeatTime;
    mstxVecComplexReduceDesc.srcRepeatStride = srcRepStride;
    CopyName(mstxVecComplexReduceDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_REDUCE), sizeof(mstxVecComplexReduceDesc),
        &mstxVecComplexReduceDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxVecReduceComplexInfo(
    const LocalTensor<U>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t mask,
    const int32_t repeatTime, const int32_t srcRepStride, __gm__ const char* name)
{
    MstxVecComplexReduceDesc mstxVecComplexReduceDesc;
    mstxVecComplexReduceDesc.dst = From(dst);
    mstxVecComplexReduceDesc.src = From(src);
    mstxVecComplexReduceDesc.tmp = From(sharedTmpBuffer);
    mstxVecComplexReduceDesc.wrapper = WrapperFrom<T>(MSTX_MASK_FROM_REG, mask, true, static_cast<uint32_t>(8192));
    mstxVecComplexReduceDesc.repeatTimes = repeatTime;
    mstxVecComplexReduceDesc.srcRepeatStride = srcRepStride;
    CopyName(mstxVecComplexReduceDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_REDUCE), sizeof(mstxVecComplexReduceDesc),
        &mstxVecComplexReduceDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxVecReduceComplexInfo(
    const LocalTensor<U>& dst, const LocalTensor<T>& src, const LocalTensor<T>& sharedTmpBuffer, const int32_t count,
    __gm__ const char* name)
{
    MstxVecComplexReduceDesc mstxVecComplexReduceDesc;
    mstxVecComplexReduceDesc.dst = From(dst);
    mstxVecComplexReduceDesc.src = From(src);
    mstxVecComplexReduceDesc.tmp = From(sharedTmpBuffer);
    mstxVecComplexReduceDesc.wrapper =
        WrapperFrom(MSTX_MASK_COUNT, count, static_cast<uint64_t>(0), true, static_cast<uint32_t>(8192));
    mstxVecComplexReduceDesc.repeatTimes = 1;
    mstxVecComplexReduceDesc.srcRepeatStride = 8;
    CopyName(mstxVecComplexReduceDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_REDUCE), sizeof(mstxVecComplexReduceDesc),
        &mstxVecComplexReduceDesc);
}

template <typename T, typename U, typename V>
__aicore__ inline void GetMstxVecBinaryInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<V>& src1, uint64_t mask0, uint64_t mask1,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams, bool isSetMask, __gm__ const char* name)
{
    MstxVecBinaryDesc mstxVecBinaryDesc;
    mstxVecBinaryDesc.dst = From(dst);
    mstxVecBinaryDesc.src0 = From(src0);
    mstxVecBinaryDesc.src1 = From(src1);
    mstxVecBinaryDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, isSetMask, static_cast<uint32_t>(0));
    mstxVecBinaryDesc.blockNum = repeatParams.blockNumber;
    mstxVecBinaryDesc.repeatTimes = repeatTime;
    mstxVecBinaryDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecBinaryDesc.src0BlockStride = repeatParams.src0BlkStride;
    mstxVecBinaryDesc.src1BlockStride = repeatParams.src1BlkStride;
    mstxVecBinaryDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecBinaryDesc.src0RepeatStride = repeatParams.src0RepStride;
    mstxVecBinaryDesc.src1RepeatStride = repeatParams.src1RepStride;
    CopyName(mstxVecBinaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_BINARY), sizeof(mstxVecBinaryDesc), &mstxVecBinaryDesc);
}

template <typename T, typename U, typename V>
__aicore__ inline void GetMstxVecBinaryInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<V>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams, bool isSetMask, __gm__ const char* name)
{
    MstxVecBinaryDesc mstxVecBinaryDesc;
    mstxVecBinaryDesc.dst = From(dst);
    mstxVecBinaryDesc.src0 = From(src0);
    mstxVecBinaryDesc.src1 = From(src1);
    mstxVecBinaryDesc.wrapper = WrapperFrom<T>(MSTX_MASK_FROM_REG, mask, isSetMask, static_cast<uint32_t>(0));
    mstxVecBinaryDesc.blockNum = repeatParams.blockNumber;
    mstxVecBinaryDesc.repeatTimes = repeatTime;
    mstxVecBinaryDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecBinaryDesc.src0BlockStride = repeatParams.src0BlkStride;
    mstxVecBinaryDesc.src1BlockStride = repeatParams.src1BlkStride;
    mstxVecBinaryDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecBinaryDesc.src0RepeatStride = repeatParams.src0RepStride;
    mstxVecBinaryDesc.src1RepeatStride = repeatParams.src1RepStride;
    CopyName(mstxVecBinaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_BINARY), sizeof(mstxVecBinaryDesc), &mstxVecBinaryDesc);
}

template <typename T, typename U, typename V>
__aicore__ inline void GetMstxVecBinaryInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<V>& src1, __gm__ const char* name,
    const int32_t count)
{
    MstxVecBinaryDesc mstxVecBinaryDesc;
    mstxVecBinaryDesc.dst = From(dst);
    mstxVecBinaryDesc.src0 = From(src0);
    mstxVecBinaryDesc.src1 = From(src1);
    mstxVecBinaryDesc.wrapper =
        WrapperFrom(MSTX_MASK_COUNT, count, static_cast<uint64_t>(0), true, static_cast<uint32_t>(0));
    mstxVecBinaryDesc.blockNum = 8;
    mstxVecBinaryDesc.repeatTimes = 1;
    mstxVecBinaryDesc.dstBlockStride = 1;
    mstxVecBinaryDesc.src0BlockStride = 1;
    mstxVecBinaryDesc.src1BlockStride = 1;
    mstxVecBinaryDesc.dstRepeatStride = 8;
    mstxVecBinaryDesc.src0RepeatStride = 8;
    mstxVecBinaryDesc.src1RepeatStride = 8;
    CopyName(mstxVecBinaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_BINARY), sizeof(mstxVecBinaryDesc), &mstxVecBinaryDesc);
}

template <typename T, typename U, typename V>
__aicore__ inline void GetMstxVecBilinearInterpolationInfo(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<U>& src0Offset, const LocalTensor<T>& src1,
    uint64_t mask0, uint64_t mask1, uint8_t hRepeat, bool repeatMode, uint16_t dstBlkStride, uint16_t vROffset,
    uint8_t vRepeat, const LocalTensor<V>& sharedTmpBuffer, __gm__ const char* name)
{
    MstxVecBilinearInterpolation mstxVecBilinearInterpolation;
    mstxVecBilinearInterpolation.dst = From(dst);
    mstxVecBilinearInterpolation.src0 = From(src0);
    mstxVecBilinearInterpolation.src1 = From(src1);
    mstxVecBilinearInterpolation.src0Offset = From(src0Offset);
    mstxVecBilinearInterpolation.shared = From(sharedTmpBuffer);
    mstxVecBilinearInterpolation.wrapper =
        WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, true, static_cast<uint32_t>(8192));
    mstxVecBilinearInterpolation.hRepeat = hRepeat;
    mstxVecBilinearInterpolation.repeatMode = repeatMode;
    mstxVecBilinearInterpolation.dstBlockStride = dstBlkStride;
    mstxVecBilinearInterpolation.vROffset = vROffset;
    mstxVecBilinearInterpolation.vRepeat = vRepeat;
    CopyName(mstxVecBilinearInterpolation.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_BILINEAR_INTERPOLATION), sizeof(mstxVecBilinearInterpolation),
        &mstxVecBilinearInterpolation);
}

template <typename T, typename U, typename V>
__aicore__ inline void GetMstxVecBilinearInterpolationInfo(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<U>& src0Offset, const LocalTensor<T>& src1,
    uint64_t mask, uint8_t hRepeat, bool repeatMode, uint16_t dstBlkStride, uint16_t vROffset, uint8_t vRepeat,
    const LocalTensor<V>& sharedTmpBuffer, __gm__ const char* name)
{
    MstxVecBilinearInterpolation mstxVecBilinearInterpolation;
    mstxVecBilinearInterpolation.dst = From(dst);
    mstxVecBilinearInterpolation.src0 = From(src0);
    mstxVecBilinearInterpolation.src1 = From(src1);
    mstxVecBilinearInterpolation.src0Offset = From(src0Offset);
    mstxVecBilinearInterpolation.shared = From(sharedTmpBuffer);
    mstxVecBilinearInterpolation.wrapper = WrapperFrom<T>(MSTX_MASK_FROM_REG, mask, true, static_cast<uint32_t>(8192));
    mstxVecBilinearInterpolation.hRepeat = hRepeat;
    mstxVecBilinearInterpolation.repeatMode = repeatMode;
    mstxVecBilinearInterpolation.dstBlockStride = dstBlkStride;
    mstxVecBilinearInterpolation.vROffset = vROffset;
    mstxVecBilinearInterpolation.vRepeat = vRepeat;
    CopyName(mstxVecBilinearInterpolation.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_BILINEAR_INTERPOLATION), sizeof(mstxVecBilinearInterpolation),
        &mstxVecBilinearInterpolation);
}

// select
// 无软仿
template <typename T, typename U>
__aicore__ inline void GetMstxVecBinarySelInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
    uint64_t mask0, uint64_t mask1, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams, bool isSetMask,
    SELMODE selMode, __gm__ const char* name)
{
    MstxVecSelDesc mstxVecSelDesc;
    mstxVecSelDesc.dst = From(dst);
    mstxVecSelDesc.src0 = From(src0);
    mstxVecSelDesc.src1 = From(src1);
    mstxVecSelDesc.mask = From(selMask);
    mstxVecSelDesc.scalarMode = (selMode == AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE);
    if (mstxVecSelDesc.scalarMode) {
        mstxVecSelDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, isSetMask, static_cast<uint32_t>(8192));
    } else {
        mstxVecSelDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, isSetMask, static_cast<uint32_t>(0));
    }
    mstxVecSelDesc.blockNum = repeatParams.blockNumber;
    mstxVecSelDesc.repeatTimes = repeatTime;
    mstxVecSelDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecSelDesc.src0BlockStride = repeatParams.src0BlkStride;
    mstxVecSelDesc.src1BlockStride = repeatParams.src1BlkStride;
    mstxVecSelDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecSelDesc.src0RepeatStride = repeatParams.src0RepStride;
    mstxVecSelDesc.src1RepeatStride = repeatParams.src1RepStride;
    CopyName(mstxVecSelDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_SEL), sizeof(mstxVecSelDesc), &mstxVecSelDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxVecBinarySelInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
    uint64_t mask, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams, bool isSetMask, SELMODE selMode,
    __gm__ const char* name)
{
    MstxVecSelDesc mstxVecSelDesc;
    mstxVecSelDesc.dst = From(dst);
    mstxVecSelDesc.src0 = From(src0);
    mstxVecSelDesc.src1 = From(src1);
    mstxVecSelDesc.mask = From(selMask);
    mstxVecSelDesc.scalarMode = (selMode == AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE);
    if (mstxVecSelDesc.scalarMode) {
        mstxVecSelDesc.wrapper = WrapperFrom<T>(MSTX_MASK_FROM_REG, mask, isSetMask, static_cast<uint32_t>(8192));
    } else {
        mstxVecSelDesc.wrapper = WrapperFrom<T>(MSTX_MASK_FROM_REG, mask, isSetMask, static_cast<uint32_t>(0));
    }
    mstxVecSelDesc.blockNum = repeatParams.blockNumber;
    mstxVecSelDesc.repeatTimes = repeatTime;
    mstxVecSelDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecSelDesc.src0BlockStride = repeatParams.src0BlkStride;
    mstxVecSelDesc.src1BlockStride = repeatParams.src1BlkStride;
    mstxVecSelDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecSelDesc.src0RepeatStride = repeatParams.src0RepStride;
    mstxVecSelDesc.src1RepeatStride = repeatParams.src1RepStride;
    CopyName(mstxVecSelDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_SEL), sizeof(mstxVecSelDesc), &mstxVecSelDesc);
}

template <typename T>
__aicore__ inline void GetMstxVecBinarySelInfo(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams, SELMODE selMode, __gm__ const char* name)
{
    MstxVecSelDesc mstxVecSelDesc;
    mstxVecSelDesc.dst = From(dst);
    mstxVecSelDesc.src0 = From(src0);
    mstxVecSelDesc.src1 = From(src1);
    mstxVecSelDesc.scalarMode = (selMode == AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE);
    if (mstxVecSelDesc.scalarMode) {
        mstxVecSelDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, 0, 0, false, static_cast<uint32_t>(8192));
    } else {
        mstxVecSelDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, 0, 0, false, static_cast<uint32_t>(0));
    }
    mstxVecSelDesc.blockNum = repeatParams.blockNumber;
    mstxVecSelDesc.repeatTimes = repeatTime;
    mstxVecSelDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecSelDesc.src0BlockStride = repeatParams.src0BlkStride;
    mstxVecSelDesc.src1BlockStride = repeatParams.src1BlkStride;
    mstxVecSelDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecSelDesc.src0RepeatStride = repeatParams.src0RepStride;
    mstxVecSelDesc.src1RepeatStride = repeatParams.src1RepStride;
    CopyName(mstxVecSelDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_SEL), sizeof(mstxVecSelDesc), &mstxVecSelDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxVecBinarySelInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams, SELMODE selMode, __gm__ const char* name)
{
    MstxVecSelDesc mstxVecSelDesc;
    mstxVecSelDesc.dst = From(dst);
    mstxVecSelDesc.src0 = From(src0);
    mstxVecSelDesc.mask = From(selMask);
    mstxVecSelDesc.scalarMode = (selMode == AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE);
    if (mstxVecSelDesc.scalarMode) {
        mstxVecSelDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, 0, 0, false, static_cast<uint32_t>(8192));
    } else {
        mstxVecSelDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, 0, 0, false, static_cast<uint32_t>(0));
    }
    mstxVecSelDesc.blockNum = repeatParams.blockNumber;
    mstxVecSelDesc.repeatTimes = repeatTime;
    mstxVecSelDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecSelDesc.src0BlockStride = repeatParams.src0BlkStride;
    mstxVecSelDesc.src1BlockStride = repeatParams.src1BlkStride;
    mstxVecSelDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecSelDesc.src0RepeatStride = repeatParams.src0RepStride;
    mstxVecSelDesc.src1RepeatStride = repeatParams.src1RepStride;
    CopyName(mstxVecSelDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_SEL), sizeof(mstxVecSelDesc), &mstxVecSelDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxVecBinarySelInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
    uint32_t count, bool isSetMask, SELMODE selMode, __gm__ const char* name)
{
    MstxVecSelDesc mstxVecSelDesc;
    mstxVecSelDesc.dst = From(dst);
    mstxVecSelDesc.src0 = From(src0);
    mstxVecSelDesc.src1 = From(src1);
    mstxVecSelDesc.mask = From(selMask);
    mstxVecSelDesc.scalarMode = (selMode == AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE);
    if (mstxVecSelDesc.scalarMode) {
        mstxVecSelDesc.wrapper =
            WrapperFrom(MSTX_MASK_COUNT, count, static_cast<uint64_t>(0), isSetMask, static_cast<uint32_t>(8192));
    } else {
        mstxVecSelDesc.wrapper =
            WrapperFrom(MSTX_MASK_COUNT, count, static_cast<uint64_t>(0), isSetMask, static_cast<uint32_t>(0));
    }
    mstxVecSelDesc.blockNum = 8;
    mstxVecSelDesc.repeatTimes = 1;
    mstxVecSelDesc.dstBlockStride = 1;
    mstxVecSelDesc.src0BlockStride = 1;
    mstxVecSelDesc.src1BlockStride = 1;
    mstxVecSelDesc.dstRepeatStride = 8;
    mstxVecSelDesc.src0RepeatStride = 8;
    mstxVecSelDesc.src1RepeatStride = 8;
    CopyName(mstxVecSelDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_SEL), sizeof(mstxVecSelDesc), &mstxVecSelDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxVecBinarySelInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, uint64_t mask0,
    uint64_t mask1, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams, bool isSetMask, SELMODE selMode,
    __gm__ const char* name)
{
    MstxVecSelDesc mstxVecSelDesc;
    mstxVecSelDesc.dst = From(dst);
    mstxVecSelDesc.src0 = From(src0);
    mstxVecSelDesc.mask = From(selMask);
    mstxVecSelDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, isSetMask, static_cast<uint32_t>(8192));
    mstxVecSelDesc.blockNum = repeatParams.blockNumber;
    mstxVecSelDesc.repeatTimes = repeatTime;
    mstxVecSelDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecSelDesc.src0BlockStride = repeatParams.src0BlkStride;
    mstxVecSelDesc.src1BlockStride = repeatParams.src1BlkStride;
    mstxVecSelDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecSelDesc.src0RepeatStride = repeatParams.src0RepStride;
    mstxVecSelDesc.src1RepeatStride = repeatParams.src1RepStride;
    mstxVecSelDesc.scalarMode = (selMode == AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE);
    CopyName(mstxVecSelDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_SEL), sizeof(mstxVecSelDesc), &mstxVecSelDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxVecBinarySelInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams, bool isSetMask, SELMODE selMode,
    __gm__ const char* name)
{
    MstxVecSelDesc mstxVecSelDesc;
    mstxVecSelDesc.dst = From(dst);
    mstxVecSelDesc.src0 = From(src0);
    mstxVecSelDesc.mask = From(selMask);
    mstxVecSelDesc.wrapper = WrapperFrom<T>(MSTX_MASK_FROM_REG, mask, isSetMask, static_cast<uint32_t>(8192));
    mstxVecSelDesc.blockNum = repeatParams.blockNumber;
    mstxVecSelDesc.repeatTimes = repeatTime;
    mstxVecSelDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecSelDesc.src0BlockStride = repeatParams.src0BlkStride;
    mstxVecSelDesc.src1BlockStride = repeatParams.src1BlkStride;
    mstxVecSelDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecSelDesc.src0RepeatStride = repeatParams.src0RepStride;
    mstxVecSelDesc.src1RepeatStride = repeatParams.src1RepStride;
    mstxVecSelDesc.scalarMode = (selMode == AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE);
    CopyName(mstxVecSelDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_SEL), sizeof(mstxVecSelDesc), &mstxVecSelDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxVecBinarySelInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, uint32_t count,
    bool isSetMask, SELMODE selMode, __gm__ const char* name)
{
    MstxVecSelDesc mstxVecSelDesc;
    mstxVecSelDesc.dst = From(dst);
    mstxVecSelDesc.src0 = From(src0);
    mstxVecSelDesc.mask = From(selMask);
    mstxVecSelDesc.wrapper =
        WrapperFrom(MSTX_MASK_COUNT, count, static_cast<uint64_t>(0), isSetMask, static_cast<uint32_t>(8192));
    mstxVecSelDesc.blockNum = 8;
    mstxVecSelDesc.repeatTimes = 1;
    mstxVecSelDesc.dstBlockStride = 1;
    mstxVecSelDesc.src0BlockStride = 1;
    mstxVecSelDesc.src1BlockStride = 1;
    mstxVecSelDesc.dstRepeatStride = 8;
    mstxVecSelDesc.src0RepeatStride = 8;
    mstxVecSelDesc.src1RepeatStride = 8;
    mstxVecSelDesc.scalarMode = (selMode == AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE);
    CopyName(mstxVecSelDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_SEL), sizeof(mstxVecSelDesc), &mstxVecSelDesc);
}

// compare
template <typename T, typename U, typename V>
__aicore__ inline void GetMstxVecBinaryCmpInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<V>& src1, uint64_t mask0, uint64_t mask1,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams, bool isSetMask, __gm__ const char* name)
{
    MstxVecBinaryDesc mstxVecBinaryDesc;
    mstxVecBinaryDesc.dst = From(dst);
    mstxVecBinaryDesc.src0 = From(src0);
    mstxVecBinaryDesc.src1 = From(src1);
    mstxVecBinaryDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, isSetMask, static_cast<uint32_t>(0));
    mstxVecBinaryDesc.blockNum = repeatParams.blockNumber;
    mstxVecBinaryDesc.repeatTimes = repeatTime;
    mstxVecBinaryDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecBinaryDesc.src0BlockStride = repeatParams.src0BlkStride;
    mstxVecBinaryDesc.src1BlockStride = repeatParams.src1BlkStride;
    mstxVecBinaryDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecBinaryDesc.src0RepeatStride = repeatParams.src0RepStride;
    mstxVecBinaryDesc.src1RepeatStride = repeatParams.src1RepStride;
    CopyName(mstxVecBinaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_CMP), sizeof(mstxVecBinaryDesc), &mstxVecBinaryDesc);
}

template <typename T, typename U, typename V>
__aicore__ inline void GetMstxVecBinaryCmpInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<V>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams, bool isSetMask, __gm__ const char* name)
{
    MstxVecBinaryDesc mstxVecBinaryDesc;
    mstxVecBinaryDesc.dst = From(dst);
    mstxVecBinaryDesc.src0 = From(src0);
    mstxVecBinaryDesc.src1 = From(src1);
    mstxVecBinaryDesc.wrapper = WrapperFrom<T>(MSTX_MASK_FROM_REG, mask, isSetMask, static_cast<uint32_t>(0));
    mstxVecBinaryDesc.blockNum = repeatParams.blockNumber;
    mstxVecBinaryDesc.repeatTimes = repeatTime;
    mstxVecBinaryDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecBinaryDesc.src0BlockStride = repeatParams.src0BlkStride;
    mstxVecBinaryDesc.src1BlockStride = repeatParams.src1BlkStride;
    mstxVecBinaryDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecBinaryDesc.src0RepeatStride = repeatParams.src0RepStride;
    mstxVecBinaryDesc.src1RepeatStride = repeatParams.src1RepStride;
    CopyName(mstxVecBinaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_CMP), sizeof(mstxVecBinaryDesc), &mstxVecBinaryDesc);
}

template <typename T, typename U, typename V>
__aicore__ inline void GetMstxVecBinaryCmpInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<V>& src1, __gm__ const char* name,
    const int32_t count)
{
    MstxVecBinaryDesc mstxVecBinaryDesc;
    mstxVecBinaryDesc.dst = From(dst);
    mstxVecBinaryDesc.src0 = From(src0);
    mstxVecBinaryDesc.src1 = From(src1);
    mstxVecBinaryDesc.wrapper =
        WrapperFrom(MSTX_MASK_COUNT, count, static_cast<uint64_t>(0), true, static_cast<uint32_t>(0));
    mstxVecBinaryDesc.blockNum = 8;
    mstxVecBinaryDesc.repeatTimes = 1;
    mstxVecBinaryDesc.dstBlockStride = 1;
    mstxVecBinaryDesc.src0BlockStride = 1;
    mstxVecBinaryDesc.src1BlockStride = 1;
    mstxVecBinaryDesc.dstRepeatStride = 8;
    mstxVecBinaryDesc.src0RepeatStride = 8;
    mstxVecBinaryDesc.src1RepeatStride = 8;
    CopyName(mstxVecBinaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_CMP), sizeof(mstxVecBinaryDesc), &mstxVecBinaryDesc);
}

// 软仿的
template <typename T, typename U, typename V>
__aicore__ inline void GetMstxVecBinaryAddReqReluInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<V>& src1, uint64_t mask0, uint64_t mask1,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams, bool isSetMask, __gm__ const char* name)
{
    MstxVecBinaryDesc mstxVecBinaryDesc;
    mstxVecBinaryDesc.dst = From(dst);
    mstxVecBinaryDesc.src0 = From(src0);
    mstxVecBinaryDesc.src1 = From(src1);
    mstxVecBinaryDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, isSetMask, static_cast<uint32_t>(8192));
    mstxVecBinaryDesc.blockNum = repeatParams.blockNumber;
    mstxVecBinaryDesc.repeatTimes = repeatTime;
    mstxVecBinaryDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecBinaryDesc.src0BlockStride = repeatParams.src0BlkStride;
    mstxVecBinaryDesc.src1BlockStride = repeatParams.src1BlkStride;
    mstxVecBinaryDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecBinaryDesc.src0RepeatStride = repeatParams.src0RepStride;
    mstxVecBinaryDesc.src1RepeatStride = repeatParams.src1RepStride;
    CopyName(mstxVecBinaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_BINARY), sizeof(mstxVecBinaryDesc), &mstxVecBinaryDesc);
}

template <typename T, typename U, typename V>
__aicore__ inline void GetMstxVecBinaryAddReqReluInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<V>& src1, uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams, bool isSetMask, __gm__ const char* name)
{
    MstxVecBinaryDesc mstxVecBinaryDesc;
    mstxVecBinaryDesc.dst = From(dst);
    mstxVecBinaryDesc.src0 = From(src0);
    mstxVecBinaryDesc.src1 = From(src1);
    mstxVecBinaryDesc.wrapper = WrapperFrom<T>(MSTX_MASK_FROM_REG, mask, isSetMask, static_cast<uint32_t>(8192));
    mstxVecBinaryDesc.blockNum = repeatParams.blockNumber;
    mstxVecBinaryDesc.repeatTimes = repeatTime;
    mstxVecBinaryDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecBinaryDesc.src0BlockStride = repeatParams.src0BlkStride;
    mstxVecBinaryDesc.src1BlockStride = repeatParams.src1BlkStride;
    mstxVecBinaryDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecBinaryDesc.src0RepeatStride = repeatParams.src0RepStride;
    mstxVecBinaryDesc.src1RepeatStride = repeatParams.src1RepStride;
    CopyName(mstxVecBinaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_BINARY), sizeof(mstxVecBinaryDesc), &mstxVecBinaryDesc);
}

template <typename T, typename U, typename V>
__aicore__ inline void GetMstxVecBinaryAddReqReluInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src0, const LocalTensor<V>& src1, __gm__ const char* name,
    const int32_t count)
{
    MstxVecBinaryDesc mstxVecBinaryDesc;
    mstxVecBinaryDesc.dst = From(dst);
    mstxVecBinaryDesc.src0 = From(src0);
    mstxVecBinaryDesc.src1 = From(src1);
    mstxVecBinaryDesc.wrapper =
        WrapperFrom(MSTX_MASK_COUNT, count, static_cast<uint64_t>(0), true, static_cast<uint32_t>(8192));
    mstxVecBinaryDesc.blockNum = 8;
    mstxVecBinaryDesc.repeatTimes = 1;
    mstxVecBinaryDesc.dstBlockStride = 1;
    mstxVecBinaryDesc.src0BlockStride = 1;
    mstxVecBinaryDesc.src1BlockStride = 1;
    mstxVecBinaryDesc.dstRepeatStride = 8;
    mstxVecBinaryDesc.src0RepeatStride = 8;
    mstxVecBinaryDesc.src1RepeatStride = 8;
    CopyName(mstxVecBinaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_BINARY), sizeof(mstxVecBinaryDesc), &mstxVecBinaryDesc);
}

template <typename T>
__aicore__ inline void GetMstxVecBinaryScalarInfo(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask0, uint64_t mask1,
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, bool isSetMask, __gm__ const char* name)
{
    MstxVecBinaryDesc mstxVecBinaryDesc;
    mstxVecBinaryDesc.dst = From(dst);
    mstxVecBinaryDesc.src0 = From(src0);
    mstxVecBinaryDesc.src1 = From(src1);
    mstxVecBinaryDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, isSetMask, static_cast<uint32_t>(0));
    mstxVecBinaryDesc.blockNum = repeatParams.blockNumber;
    mstxVecBinaryDesc.repeatTimes = repeatTime;
    mstxVecBinaryDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecBinaryDesc.src0BlockStride = repeatParams.srcBlkStride;
    mstxVecBinaryDesc.src1BlockStride = repeatParams.srcBlkStride;
    mstxVecBinaryDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecBinaryDesc.src0RepeatStride = repeatParams.srcRepStride;
    mstxVecBinaryDesc.src1RepeatStride = repeatParams.srcRepStride;
    CopyName(mstxVecBinaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_BINARY_SCALAR), sizeof(mstxVecBinaryDesc), &mstxVecBinaryDesc);
}

template <typename T>
__aicore__ inline void GetMstxVecBinaryScalarInfo(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask,
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, bool isSetMask, __gm__ const char* name)
{
    MstxVecBinaryDesc mstxVecBinaryDesc;
    mstxVecBinaryDesc.dst = From(dst);
    mstxVecBinaryDesc.src0 = From(src0);
    mstxVecBinaryDesc.src1 = From(src1);
    mstxVecBinaryDesc.wrapper = WrapperFrom<T>(MSTX_MASK_FROM_REG, mask, isSetMask, static_cast<uint32_t>(0));
    mstxVecBinaryDesc.blockNum = repeatParams.blockNumber;
    mstxVecBinaryDesc.repeatTimes = repeatTime;
    mstxVecBinaryDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecBinaryDesc.src0BlockStride = repeatParams.srcBlkStride;
    mstxVecBinaryDesc.src1BlockStride = repeatParams.srcBlkStride;
    mstxVecBinaryDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecBinaryDesc.src0RepeatStride = repeatParams.srcRepStride;
    mstxVecBinaryDesc.src1RepeatStride = repeatParams.srcRepStride;
    CopyName(mstxVecBinaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_BINARY_SCALAR), sizeof(mstxVecBinaryDesc), &mstxVecBinaryDesc);
}

template <typename T>
__aicore__ inline void GetMstxVecBinaryScalarInfo(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, bool isSetMask,
    __gm__ const char* name, const int32_t count)
{
    MstxVecBinaryDesc mstxVecBinaryDesc;
    mstxVecBinaryDesc.dst = From(dst);
    mstxVecBinaryDesc.src0 = From(src0);
    mstxVecBinaryDesc.src1 = From(src1);
    mstxVecBinaryDesc.wrapper =
        WrapperFrom(MSTX_MASK_COUNT, count, static_cast<uint64_t>(0), isSetMask, static_cast<uint32_t>(0));
    mstxVecBinaryDesc.blockNum = 8;
    mstxVecBinaryDesc.repeatTimes = 1;
    mstxVecBinaryDesc.dstBlockStride = 1;
    mstxVecBinaryDesc.src0BlockStride = 1;
    mstxVecBinaryDesc.src1BlockStride = 1;
    mstxVecBinaryDesc.dstRepeatStride = 8;
    mstxVecBinaryDesc.src0RepeatStride = 8;
    mstxVecBinaryDesc.src1RepeatStride = 8;
    CopyName(mstxVecBinaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_BINARY_SCALAR), sizeof(mstxVecBinaryDesc), &mstxVecBinaryDesc);
}

// comparescalar的三目的
template <typename T>
__aicore__ inline void GetMstxVecBinaryScalarCmpsInfo(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask0, uint64_t mask1,
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, bool isSetMask, __gm__ const char* name)
{
    MstxVecBinaryDesc mstxVecBinaryDesc;
    mstxVecBinaryDesc.dst = From(dst);
    mstxVecBinaryDesc.src0 = From(src0);
    mstxVecBinaryDesc.src1 = From(src1);
    mstxVecBinaryDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, isSetMask, static_cast<uint32_t>(0));
    mstxVecBinaryDesc.blockNum = repeatParams.blockNumber;
    mstxVecBinaryDesc.repeatTimes = repeatTime;
    mstxVecBinaryDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecBinaryDesc.src0BlockStride = repeatParams.srcBlkStride;
    mstxVecBinaryDesc.src1BlockStride = repeatParams.srcBlkStride;
    mstxVecBinaryDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecBinaryDesc.src0RepeatStride = repeatParams.srcRepStride;
    mstxVecBinaryDesc.src1RepeatStride = repeatParams.srcRepStride;
    CopyName(mstxVecBinaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_CMPS), sizeof(mstxVecBinaryDesc), &mstxVecBinaryDesc);
}

template <typename T>
__aicore__ inline void GetMstxVecBinaryScalarCmpsInfo(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint64_t mask,
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, bool isSetMask, __gm__ const char* name)
{
    MstxVecBinaryDesc mstxVecBinaryDesc;
    mstxVecBinaryDesc.dst = From(dst);
    mstxVecBinaryDesc.src0 = From(src0);
    mstxVecBinaryDesc.src1 = From(src1);
    mstxVecBinaryDesc.wrapper = WrapperFrom<T>(MSTX_MASK_FROM_REG, mask, isSetMask, static_cast<uint32_t>(0));
    mstxVecBinaryDesc.blockNum = repeatParams.blockNumber;
    mstxVecBinaryDesc.repeatTimes = repeatTime;
    mstxVecBinaryDesc.dstBlockStride = repeatParams.dstBlkStride;
    mstxVecBinaryDesc.src0BlockStride = repeatParams.srcBlkStride;
    mstxVecBinaryDesc.src1BlockStride = repeatParams.srcBlkStride;
    mstxVecBinaryDesc.dstRepeatStride = repeatParams.dstRepStride;
    mstxVecBinaryDesc.src0RepeatStride = repeatParams.srcRepStride;
    mstxVecBinaryDesc.src1RepeatStride = repeatParams.srcRepStride;
    CopyName(mstxVecBinaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_CMPS), sizeof(mstxVecBinaryDesc), &mstxVecBinaryDesc);
}

template <typename T>
__aicore__ inline void GetMstxVecBinaryScalarCmpsInfo(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, bool isSetMask,
    __gm__ const char* name, const int32_t count)
{
    MstxVecBinaryDesc mstxVecBinaryDesc;
    mstxVecBinaryDesc.dst = From(dst);
    mstxVecBinaryDesc.src0 = From(src0);
    mstxVecBinaryDesc.src1 = From(src1);
    mstxVecBinaryDesc.wrapper =
        WrapperFrom(MSTX_MASK_COUNT, count, static_cast<uint64_t>(0), isSetMask, static_cast<uint32_t>(0));
    mstxVecBinaryDesc.blockNum = 8;
    mstxVecBinaryDesc.repeatTimes = 1;
    mstxVecBinaryDesc.dstBlockStride = 1;
    mstxVecBinaryDesc.src0BlockStride = 1;
    mstxVecBinaryDesc.src1BlockStride = 1;
    mstxVecBinaryDesc.dstRepeatStride = 8;
    mstxVecBinaryDesc.src0RepeatStride = 8;
    mstxVecBinaryDesc.src1RepeatStride = 8;
    CopyName(mstxVecBinaryDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_CMPS), sizeof(mstxVecBinaryDesc), &mstxVecBinaryDesc);
}

// GatherMask
template <typename T>
__aicore__ inline void GetMstxVecGatherMaskInfo(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint32_t mask0, uint32_t mask1,
    const GatherMaskParams& gatherMaskParams, GatherMaskMode mode, __gm__ const char* name)
{
    MstxVecGatherMaskDesc mstxVecGatherMaskDesc;
    mstxVecGatherMaskDesc.dst = From(dst);
    mstxVecGatherMaskDesc.src = From(src);
    mstxVecGatherMaskDesc.wrapper = WrapperFrom(MSTX_MASK_FROM_REG, mask0, mask1, true, static_cast<uint32_t>(0));
    mstxVecGatherMaskDesc.mode = (mode == GatherMaskMode::VERSION_V2) ? AscendC::MstxTensor::MstxGatherMaskMode::V2 :
                                                                        AscendC::MstxTensor::MstxGatherMaskMode::V1;
    mstxVecGatherMaskDesc.repeatTimes = gatherMaskParams.repeatTimes;
    mstxVecGatherMaskDesc.src0BlockStride = gatherMaskParams.src0BlockStride;
    mstxVecGatherMaskDesc.src0RepeatStride = gatherMaskParams.src0RepeatStride;
    mstxVecGatherMaskDesc.src1RepeatStride = gatherMaskParams.src1RepeatStride;
    CopyName(mstxVecGatherMaskDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_GATHER_MASK), sizeof(mstxVecGatherMaskDesc),
        &mstxVecGatherMaskDesc);
}

template <typename T>
__aicore__ inline void GetMstxVecGatherMaskInfo(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint32_t mask, const GatherMaskParams& gatherMaskParams,
    GatherMaskMode mode, __gm__ const char* name)
{
    MstxVecGatherMaskDesc mstxVecGatherMaskDesc;
    mstxVecGatherMaskDesc.dst = From(dst);
    mstxVecGatherMaskDesc.src = From(src);
    mstxVecGatherMaskDesc.wrapper = WrapperFrom<T>(MSTX_MASK_FROM_REG, mask, true, static_cast<uint32_t>(0));
    mstxVecGatherMaskDesc.mode = (mode == GatherMaskMode::VERSION_V2) ? AscendC::MstxTensor::MstxGatherMaskMode::V2 :
                                                                        AscendC::MstxTensor::MstxGatherMaskMode::V1;
    mstxVecGatherMaskDesc.repeatTimes = gatherMaskParams.repeatTimes;
    mstxVecGatherMaskDesc.src0BlockStride = gatherMaskParams.src0BlockStride;
    mstxVecGatherMaskDesc.src0RepeatStride = gatherMaskParams.src0RepeatStride;
    mstxVecGatherMaskDesc.src1RepeatStride = gatherMaskParams.src1RepeatStride;
    CopyName(mstxVecGatherMaskDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_VEC_GATHER_MASK), sizeof(mstxVecGatherMaskDesc),
        &mstxVecGatherMaskDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxDataCopyInfo(
    const LocalTensor<T>& dst, const GlobalTensor<U>& src, const DataCopyParams& repeatParams, __gm__ const char* name)
{
    MstxDataCopyDesc mstxDataCopyDesc;
    mstxDataCopyDesc.dst = From(dst);
    mstxDataCopyDesc.src = FromGm(src);
    mstxDataCopyDesc.nBurst = repeatParams.blockCount;
    mstxDataCopyDesc.lenBurst = repeatParams.blockLen;
    mstxDataCopyDesc.srcGap = repeatParams.srcStride;
    mstxDataCopyDesc.dstGap = repeatParams.dstStride;
    CopyName(mstxDataCopyDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_DATA_COPY), sizeof(mstxDataCopyDesc), &mstxDataCopyDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxDataCopyInfo(
    const GlobalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& repeatParams, __gm__ const char* name)
{
    MstxDataCopyDesc mstxDataCopyDesc;
    mstxDataCopyDesc.dst = FromGm(dst);
    mstxDataCopyDesc.src = From(src);
    mstxDataCopyDesc.nBurst = repeatParams.blockCount;
    mstxDataCopyDesc.lenBurst = repeatParams.blockLen;
    mstxDataCopyDesc.srcGap = repeatParams.srcStride;
    mstxDataCopyDesc.dstGap = repeatParams.dstStride;
    CopyName(mstxDataCopyDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_DATA_COPY), sizeof(mstxDataCopyDesc), &mstxDataCopyDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxDataCopyInfo(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& repeatParams, __gm__ const char* name)
{
    MstxDataCopyDesc mstxDataCopyDesc;
    mstxDataCopyDesc.dst = From(dst);
    mstxDataCopyDesc.src = From(src);
    mstxDataCopyDesc.nBurst = repeatParams.blockCount;
    mstxDataCopyDesc.lenBurst = repeatParams.blockLen;
    mstxDataCopyDesc.srcGap = repeatParams.srcStride;
    mstxDataCopyDesc.dstGap = repeatParams.dstStride;
    CopyName(mstxDataCopyDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_DATA_COPY), sizeof(mstxDataCopyDesc), &mstxDataCopyDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxDataCopyPadInfo(
    const LocalTensor<T>& dst, const GlobalTensor<U>& src, const DataCopyParams& dataCopyParams,
    const DataCopyPadParams& padParams, __gm__ const char* name)
{
    MstxDataCopyPadDesc mstxDataCopyPadDesc;
    mstxDataCopyPadDesc.dst = From(dst);
    mstxDataCopyPadDesc.src = FromGm(src);
    mstxDataCopyPadDesc.nBurst = dataCopyParams.blockCount;
    mstxDataCopyPadDesc.lenBurst = dataCopyParams.blockLen;
    mstxDataCopyPadDesc.srcGap = dataCopyParams.srcStride;
    mstxDataCopyPadDesc.dstGap = dataCopyParams.dstStride;
    mstxDataCopyPadDesc.leftPad = padParams.leftPadding;
    mstxDataCopyPadDesc.rightPad = padParams.rightPadding;
    CopyName(mstxDataCopyPadDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_DATA_COPY_PAD), sizeof(mstxDataCopyPadDesc), &mstxDataCopyPadDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxDataCopyPadInfo(
    const GlobalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& dataCopyParams,
    __gm__ const char* name)
{
    MstxDataCopyPadDesc mstxDataCopyPadDesc;
    mstxDataCopyPadDesc.dst = FromGm(dst);
    mstxDataCopyPadDesc.src = From(src);
    mstxDataCopyPadDesc.nBurst = dataCopyParams.blockCount;
    mstxDataCopyPadDesc.lenBurst = dataCopyParams.blockLen;
    mstxDataCopyPadDesc.srcGap = dataCopyParams.srcStride;
    mstxDataCopyPadDesc.dstGap = dataCopyParams.dstStride;
    mstxDataCopyPadDesc.leftPad = 0;
    mstxDataCopyPadDesc.rightPad = 0;
    CopyName(mstxDataCopyPadDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_DATA_COPY_PAD), sizeof(mstxDataCopyPadDesc), &mstxDataCopyPadDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxDataCopyPadInfo(
    const LocalTensor<T>& dst, const GlobalTensor<U>& src, const DataCopyExtParams& dataCopyParams,
    const DataCopyPadExtParams<T>& padParams, __gm__ const char* name)
{
    MstxDataCopyPadDesc mstxDataCopyPadDesc;
    mstxDataCopyPadDesc.dst = From(dst);
    mstxDataCopyPadDesc.src = FromGm(src);
    mstxDataCopyPadDesc.nBurst = dataCopyParams.blockCount;
    mstxDataCopyPadDesc.lenBurst = dataCopyParams.blockLen;
    mstxDataCopyPadDesc.srcGap = dataCopyParams.srcStride;
    mstxDataCopyPadDesc.dstGap = dataCopyParams.dstStride;
    mstxDataCopyPadDesc.leftPad = padParams.leftPadding;
    mstxDataCopyPadDesc.rightPad = padParams.rightPadding;
    CopyName(mstxDataCopyPadDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_DATA_COPY_PAD), sizeof(mstxDataCopyPadDesc), &mstxDataCopyPadDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxDataCopyPadInfo(
    const GlobalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyExtParams& dataCopyParams,
    __gm__ const char* name)
{
    MstxDataCopyPadDesc mstxDataCopyPadDesc;
    mstxDataCopyPadDesc.dst = FromGm(dst);
    mstxDataCopyPadDesc.src = From(src);
    mstxDataCopyPadDesc.nBurst = dataCopyParams.blockCount;
    mstxDataCopyPadDesc.lenBurst = dataCopyParams.blockLen;
    mstxDataCopyPadDesc.srcGap = dataCopyParams.srcStride;
    mstxDataCopyPadDesc.dstGap = dataCopyParams.dstStride;
    mstxDataCopyPadDesc.leftPad = 0;
    mstxDataCopyPadDesc.rightPad = 0;
    CopyName(mstxDataCopyPadDesc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_DATA_COPY_PAD), sizeof(mstxDataCopyPadDesc), &mstxDataCopyPadDesc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxDataCopyPadInfoV2(
    const LocalTensor<T>& dst, const GlobalTensor<U>& src, const DataCopyParams& dataCopyParams,
    const DataCopyPadParams& padParams, const PaddingMode mode, __gm__ const char* name)
{
    MstxDataCopyPadV2Desc mstxDataCopyPadV2Desc;
    mstxDataCopyPadV2Desc.dst = From(dst);
    mstxDataCopyPadV2Desc.src = FromGm(src);
    mstxDataCopyPadV2Desc.padMode =
        (mode == PaddingMode::Compact) ? MstxDataCopyPadMode::COMPACT : MstxDataCopyPadMode::NORMAL;
    mstxDataCopyPadV2Desc.nBurst = dataCopyParams.blockCount;
    mstxDataCopyPadV2Desc.lenBurst = dataCopyParams.blockLen;
    mstxDataCopyPadV2Desc.srcGap = dataCopyParams.srcStride;
    mstxDataCopyPadV2Desc.dstGap = dataCopyParams.dstStride;
    mstxDataCopyPadV2Desc.leftPad = padParams.leftPadding;
    mstxDataCopyPadV2Desc.rightPad = padParams.rightPadding;
    CopyName(mstxDataCopyPadV2Desc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_DATA_COPY_PAD_V2), sizeof(mstxDataCopyPadV2Desc),
        &mstxDataCopyPadV2Desc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxDataCopyPadInfoV2(
    const GlobalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& dataCopyParams, const PaddingMode mode,
    __gm__ const char* name)
{
    MstxDataCopyPadV2Desc mstxDataCopyPadV2Desc;
    mstxDataCopyPadV2Desc.dst = FromGm(dst);
    mstxDataCopyPadV2Desc.src = From(src);
    mstxDataCopyPadV2Desc.padMode =
        (mode == PaddingMode::Compact) ? MstxDataCopyPadMode::COMPACT : MstxDataCopyPadMode::NORMAL;
    mstxDataCopyPadV2Desc.nBurst = dataCopyParams.blockCount;
    mstxDataCopyPadV2Desc.lenBurst = dataCopyParams.blockLen;
    mstxDataCopyPadV2Desc.srcGap = dataCopyParams.srcStride;
    mstxDataCopyPadV2Desc.dstGap = dataCopyParams.dstStride;
    mstxDataCopyPadV2Desc.leftPad = 0;
    mstxDataCopyPadV2Desc.rightPad = 0;
    CopyName(mstxDataCopyPadV2Desc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_DATA_COPY_PAD_V2), sizeof(mstxDataCopyPadV2Desc),
        &mstxDataCopyPadV2Desc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxDataCopyPadInfoV2(
    const LocalTensor<T>& dst, const GlobalTensor<U>& src, const DataCopyExtParams& dataCopyParams,
    const DataCopyPadExtParams<T>& padParams, const PaddingMode mode, __gm__ const char* name)
{
    MstxDataCopyPadV2Desc mstxDataCopyPadV2Desc;
    mstxDataCopyPadV2Desc.dst = From(dst);
    mstxDataCopyPadV2Desc.src = FromGm(src);
    mstxDataCopyPadV2Desc.padMode =
        (mode == PaddingMode::Compact) ? MstxDataCopyPadMode::COMPACT : MstxDataCopyPadMode::NORMAL;
    mstxDataCopyPadV2Desc.nBurst = dataCopyParams.blockCount;
    mstxDataCopyPadV2Desc.lenBurst = dataCopyParams.blockLen;
    mstxDataCopyPadV2Desc.srcGap = dataCopyParams.srcStride;
    mstxDataCopyPadV2Desc.dstGap = dataCopyParams.dstStride;
    mstxDataCopyPadV2Desc.leftPad = padParams.leftPadding;
    mstxDataCopyPadV2Desc.rightPad = padParams.rightPadding;
    CopyName(mstxDataCopyPadV2Desc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_DATA_COPY_PAD_V2), sizeof(mstxDataCopyPadV2Desc),
        &mstxDataCopyPadV2Desc);
}

template <typename T, typename U>
__aicore__ inline void GetMstxDataCopyPadInfoV2(
    const GlobalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyExtParams& dataCopyParams,
    const PaddingMode mode, __gm__ const char* name)
{
    MstxDataCopyPadV2Desc mstxDataCopyPadV2Desc;
    mstxDataCopyPadV2Desc.dst = FromGm(dst);
    mstxDataCopyPadV2Desc.src = From(src);
    mstxDataCopyPadV2Desc.padMode =
        (mode == PaddingMode::Compact) ? MstxDataCopyPadMode::COMPACT : MstxDataCopyPadMode::NORMAL;
    mstxDataCopyPadV2Desc.nBurst = dataCopyParams.blockCount;
    mstxDataCopyPadV2Desc.lenBurst = dataCopyParams.blockLen;
    mstxDataCopyPadV2Desc.srcGap = dataCopyParams.srcStride;
    mstxDataCopyPadV2Desc.dstGap = dataCopyParams.dstStride;
    mstxDataCopyPadV2Desc.leftPad = 0;
    mstxDataCopyPadV2Desc.rightPad = 0;
    CopyName(mstxDataCopyPadV2Desc.name, name);

    __mstx_dfx_report_stub(
        static_cast<uint32_t>(MstxReportType::MSTX_DATA_COPY_PAD_V2), sizeof(mstxDataCopyPadV2Desc),
        &mstxDataCopyPadV2Desc);
}
} // namespace MstxTensor
} // namespace AscendC
#endif //__MSTX_DFX_REPORT__
#endif // MSTX_TENSOR_INFO_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MSTX_LOCAL_TENSOR_INFO_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_MSTX_LOCAL_TENSOR_INFO_H__
#endif
