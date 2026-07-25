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
 * \file kernel_operator_data_copy_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_m200/kernel_operator_data_copy_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tpipe.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_DATA_COPY_IMPL_H
#define ASCENDC_MODULE_OPERATOR_DATA_COPY_IMPL_H
#include "../../../include/basic_api/kernel_common.h"
#include "kernel_operator_vec_duplicate_impl.h"
#include "kernel_operator_vec_binary_scalar_impl.h"
#include "../../../include/basic_api/kernel_struct_unary.h"
#include "../../../include/basic_api/kernel_tpipe.h"

namespace AscendC {

// DataCopyParams: uint16_t blockCount: 12 bits
__aicore__ inline void CheckDataCopyParams(uint16_t blockCount, uint16_t blockLen)
{
    ASCENDC_CHECK_VALUE_RANGE(blockCount, 1, UINT12_MAX, "blockCount", "DataCopy");
    ASCENDC_CHECK_VALUE_RANGE(blockLen, 1, UINT16_MAX, "blockLen", "DataCopy");
}

template <TPosition pos>
__aicore__ inline uint64_t TransUBAddr(uint64_t addr);

/* **************************************************************************************************
 * DataCopy                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void DataCopyGM2UBImpl(__ubuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams)
{
    CheckDataCopyParams(intriParams.blockCount, intriParams.blockLen);
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(src, workSpace, true, false, intriParams);
    }
    copy_gm_to_ubuf(
        (__ubuf__ void*)dst, (__gm__ void*)src, 0, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride,
        intriParams.dstStride);
}

template <typename T>
__aicore__ inline void DataCopyGM2L1Impl(__cbuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from GM to L1 Buffer(A1/B1)");
#else
    CheckDataCopyParams(intriParams.blockCount, intriParams.blockLen);
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(src, workSpace, true, false, intriParams);
    }
    copy_gm_to_cbuf(
        (__cbuf__ void*)dst, (__gm__ void*)src, (int8_t)0, static_cast<uint16_t>(intriParams.blockCount),
        static_cast<uint16_t>(intriParams.blockLen), static_cast<uint16_t>(intriParams.srcStride),
        static_cast<uint16_t>(intriParams.dstStride), (pad_t)0);
#endif
}

template <typename T>
__aicore__ inline void DataCopyUB2GMImpl(__gm__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(src)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "src address should be 32B aligned \n"));
    CheckDataCopyParams(intriParams.blockCount, intriParams.blockLen);
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(dst, workSpace, false, false, intriParams);
    }
    copy_ubuf_to_gm(
        (__gm__ void*)dst, (__ubuf__ void*)src, 0, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride,
        intriParams.dstStride);
}

template <typename T>
__aicore__ inline void DataCopyUB2UBImpl(__ubuf__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from UB(VECIN) to UB(VECCALC), UB(VECCALC) to UB(VECOUT)");
#else
    CheckDataCopyParams(intriParams.blockCount, intriParams.blockLen);
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(src)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "src address should be 32B aligned \n"));
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    copy_ubuf_to_ubuf(
        (__ubuf__ void*)dst, (__ubuf__ void*)src, 0, intriParams.blockCount, intriParams.blockLen,
        intriParams.srcStride, intriParams.dstStride);
#endif
}

template <typename T>
__aicore__ inline __inout_pipe__(MTE3) void DataCopyUB2L1Impl(
    __cbuf__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from UB(VECIN/VECCALC/VECOUT) to L1 Buffer(A1/B1)");
#else
    CheckDataCopyParams(intriParams.blockCount, intriParams.blockLen);
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(src)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "src address should be 32B aligned \n"));
    copy_ubuf_to_cbuf(
        (__cbuf__ void*)dst, (__ubuf__ void*)src, 0, intriParams.blockCount, intriParams.blockLen,
        intriParams.srcStride, intriParams.dstStride);
#endif
}

template <typename T>
__aicore__ inline void DataCopyL12UBImpl(__ubuf__ T* dst, __cbuf__ T* src, const DataCopyParams& intriParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L1 Buffer(A1/B1) to UB(VECIN/VECCALC/VECOUT)");
#else
    CheckDataCopyParams(intriParams.blockCount, intriParams.blockLen);
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    copy_cbuf_to_ubuf(
        (__ubuf__ void*)dst, (__cbuf__ void*)src, 0, intriParams.blockCount, intriParams.blockLen,
        intriParams.srcStride, intriParams.dstStride);
#endif
}

template <typename T>
__aicore__ inline void DataCopyUB2L1ND2NZImpl(__cbuf__ T* dst, __ubuf__ T* src, const Nd2NzParams& intriParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy with Nd2NzParams from UB(VECIN/VECCALC/VECOUT) to L1 Buffer(A1/B1)");
}

template <typename T>
__aicore__ inline void DataCopyL12BTImpl(
    const uint64_t dst, __cbuf__ T* src, const uint16_t isEnableConv, const DataCopyParams& intriParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from UB(C1) to L0C Buffer(C2)");
}

template <typename T>
__aicore__ inline void TransND2NZ(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t high, uint16_t width, T scalar)
{
    struct UnaryRepeatParams intriParams;
    intriParams.srcBlkStride = 1;
    intriParams.dstBlkStride = 1;
    intriParams.srcRepStride = width * sizeof(T) / ONE_BLK_SIZE;
    intriParams.dstRepStride = 1;

    int highBlock = MAX_REPEAT_TIMES;
    int highBlocks = high / highBlock;
    int highTail = high % highBlock;

    uint64_t mask[2];
    mask[0] = (1 << (32 / sizeof(T))) - 1;
    mask[1] = 0;

    int widthFractal = width * sizeof(T) / 32;
    for (int i = 0; i < widthFractal; ++i) {
        for (int j = 0; j < highBlocks; ++j) {
            AddsImpl(
                dstAddr + i * (32 / sizeof(T)) * high + j * highBlock * (32 / sizeof(T)),
                srcAddr + i * (32 / sizeof(T)) + j * highBlock * width, scalar, mask, highBlock, intriParams);
        }
        if (highTail) {
            AddsImpl(
                dstAddr + i * (32 / sizeof(T)) * high + highBlocks * highBlock * (32 / sizeof(T)),
                srcAddr + i * (32 / sizeof(T)) + highBlocks * highBlock * width, scalar, mask, highTail, intriParams);
        }
    }
}

template <typename T>
__aicore__ inline void DataCopyGM2L1ND2NZImplBase(__cbuf__ T* dst, __gm__ T* src, Nd2NzParams& intriParams)
{
    uint16_t ndNum = intriParams.ndNum;
    uint16_t nValue = intriParams.nValue;
    uint16_t dValue = intriParams.dValue;
    uint16_t srcNdMatrixStride = intriParams.srcNdMatrixStride;
    uint16_t srcDValue = intriParams.srcDValue;
    uint16_t dstNzC0Stride = intriParams.dstNzC0Stride;
    uint16_t dstNzNStride = intriParams.dstNzNStride;
    uint16_t dstNzMatrixStride = intriParams.dstNzMatrixStride;

    uint16_t alignedDValueBlockNum = (dValue * sizeof(T) - 1) / 32 + 1;
    uint16_t alignedDValue = alignedDValueBlockNum * 32 / sizeof(T);

    event_t eventIdMTE3ToMTE2 = static_cast<event_t>(GetTPipePtr()->AllocEventID<HardEvent::MTE3_MTE2>());
    SetFlag<HardEvent::MTE3_MTE2>(eventIdMTE3ToMTE2);
    for (int i = 0; i < ndNum; ++i) {
        WaitFlag<HardEvent::MTE3_MTE2>(eventIdMTE3ToMTE2);
        __ubuf__ T* nd2nzTempBuf = AscendCUtils::GetTemporaryBufferAddr<T>(TMP_UB_OFFSET, 8 * 1024 / sizeof(T));
        if (((dValue * sizeof(T)) % 32 == 0) && ((srcDValue * sizeof(T)) % 32 == 0)) {
            DataCopyGM2UBImpl(
                nd2nzTempBuf, src + i * srcNdMatrixStride,
                {nValue, static_cast<uint16_t>(dValue * sizeof(T) / 32),
                 static_cast<uint16_t>((srcDValue - dValue) * sizeof(T) / 32), 0});
            event_t eventIdMTE2ToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_V));
            SetFlag<HardEvent::MTE2_V>(eventIdMTE2ToV);
            WaitFlag<HardEvent::MTE2_V>(eventIdMTE2ToV);
        } else {
            // copy and pad zero
            for (int j = 0; j < nValue; ++j) {
                DataCopyGM2UBImpl(
                    nd2nzTempBuf + j * alignedDValue, src + i * srcNdMatrixStride + j * srcDValue,
                    {1, static_cast<uint16_t>(alignedDValueBlockNum), 0, 0});
            }
            if (alignedDValue != dValue) {
                event_t eventIdMTE2ToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_V));
                SetFlag<HardEvent::MTE2_V>(eventIdMTE2ToV);
                WaitFlag<HardEvent::MTE2_V>(eventIdMTE2ToV);

                uint16_t downAlignedDValueBlockNum = dValue * sizeof(T) / 32;
                uint16_t downAlignedDValue = downAlignedDValueBlockNum * 32 / sizeof(T);
                uint64_t mask[2];
                mask[0] = ((1 << (alignedDValue - dValue)) - 1) << (dValue - downAlignedDValue);
                mask[1] = 0;
                DuplicateImpl(nd2nzTempBuf + downAlignedDValue, (T)0, mask, nValue, 1, alignedDValueBlockNum);
                PipeBarrier<PIPE_V>();
            } else {
                event_t eventIdMTE2ToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::MTE2_V));
                SetFlag<HardEvent::MTE2_V>(eventIdMTE2ToV);
                WaitFlag<HardEvent::MTE2_V>(eventIdMTE2ToV);
            }
        }

        __ubuf__ T* nzTempBuf = nd2nzTempBuf + (4 * 1024 / sizeof(T));
        TransND2NZ(nzTempBuf, nd2nzTempBuf, nValue, alignedDValue, (T)0);

        event_t eventIdVToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
        SetFlag<HardEvent::V_MTE3>(eventIdVToMTE3);
        WaitFlag<HardEvent::V_MTE3>(eventIdVToMTE3);

        uint16_t widthFractal = alignedDValue * sizeof(T) / 32;
        for (int j = 0; j < widthFractal; ++j) {
            DataCopyUB2L1Impl(
                dst + i * dstNzMatrixStride + j * 32 * dstNzC0Stride / sizeof(T),
                nzTempBuf + j * 32 * nValue / sizeof(T), {nValue, 1, 0, 0});
        }
        AscendCUtils::FreeTemporaryBuffer<T>(nd2nzTempBuf);
        SetFlag<HardEvent::MTE3_MTE2>(eventIdMTE3ToMTE2);
    }
    WaitFlag<HardEvent::MTE3_MTE2>(eventIdMTE3ToMTE2);
    GetTPipePtr()->ReleaseEventID<HardEvent::MTE3_MTE2>(eventIdMTE3ToMTE2);
}

template <typename T>
__aicore__ inline void DataCopyGM2L1ND2NZImpl(__cbuf__ T* dst, __gm__ T* src, const Nd2NzParams& intriParams)
{
    uint16_t ndNum = intriParams.ndNum;
    uint16_t nValue = intriParams.nValue;
    uint16_t dValue = intriParams.dValue;
    uint16_t srcNdMatrixStride = intriParams.srcNdMatrixStride;
    uint16_t srcDValue = intriParams.srcDValue;
    uint16_t dstNzC0Stride = intriParams.dstNzC0Stride;
    uint16_t dstNzNStride = intriParams.dstNzNStride;
    uint16_t dstNzMatrixStride = intriParams.dstNzMatrixStride;

    // tiling limited 8k, use half, 64*64B
    uint16_t highTiling = 64;  // Byte
    uint16_t widthTiling = 64; // Byte
    uint16_t highFractal = (nValue * sizeof(T)) / highTiling;
    uint16_t highFractalTail = (nValue * sizeof(T)) % highTiling;
    uint16_t widthFractal = (dValue * sizeof(T)) / widthTiling;
    uint16_t widthFractalTail = (dValue * sizeof(T)) % widthTiling;

    Nd2NzParams intriParamsBase{
        ndNum,
        static_cast<uint16_t>(highTiling / sizeof(T)),
        static_cast<uint16_t>(widthTiling / sizeof(T)),
        srcNdMatrixStride,
        srcDValue,
        dstNzC0Stride,
        dstNzNStride,
        dstNzMatrixStride};

    for (int i = 0; i < highFractal; ++i) {
        for (int j = 0; j < widthFractal; ++j) {
            DataCopyGM2L1ND2NZImplBase(
                dst + i * (highTiling / sizeof(T)) * (32 / sizeof(T)) + j * widthTiling * dstNzC0Stride / sizeof(T),
                src + i * highTiling * srcDValue / sizeof(T) + j * widthTiling / sizeof(T), intriParamsBase);
        }
    }

    // tail
    if (highFractalTail) {
        Nd2NzParams intriParamsBase1{
            ndNum,
            static_cast<uint16_t>(highFractalTail / sizeof(T)),
            static_cast<uint16_t>(widthTiling / sizeof(T)),
            srcNdMatrixStride,
            srcDValue,
            dstNzC0Stride,
            dstNzNStride,
            dstNzMatrixStride};

        for (int j = 0; j < widthFractal; ++j) {
            DataCopyGM2L1ND2NZImplBase(
                dst + highFractal * (highTiling / sizeof(T)) * (32 / sizeof(T)) +
                    j * widthTiling * dstNzC0Stride / sizeof(T),
                src + highFractal * highTiling * srcDValue / sizeof(T) + j * widthTiling / sizeof(T), intriParamsBase1);
        }
    }

    if (widthFractalTail) {
        Nd2NzParams intriParamsBase2{
            ndNum,
            static_cast<uint16_t>(highTiling / sizeof(T)),
            static_cast<uint16_t>(widthFractalTail / sizeof(T)),
            srcNdMatrixStride,
            srcDValue,
            dstNzC0Stride,
            dstNzNStride,
            dstNzMatrixStride};

        for (int i = 0; i < highFractal; ++i) {
            DataCopyGM2L1ND2NZImplBase(
                dst + i * (highTiling / sizeof(T)) * (32 / sizeof(T)) +
                    widthFractal * widthTiling * dstNzC0Stride / sizeof(T),
                src + i * highTiling * srcDValue / sizeof(T) + widthFractal * widthTiling / sizeof(T),
                intriParamsBase2);
        }
    }

    if (highFractalTail && widthFractalTail) {
        Nd2NzParams intriParamsBase2{
            ndNum,
            static_cast<uint16_t>(highFractalTail / sizeof(T)),
            static_cast<uint16_t>(widthFractalTail / sizeof(T)),
            srcNdMatrixStride,
            srcDValue,
            dstNzC0Stride,
            dstNzNStride,
            dstNzMatrixStride};

        DataCopyGM2L1ND2NZImplBase(
            dst + highFractal * (highTiling / sizeof(T)) * (32 / sizeof(T)) +
                widthFractal * widthTiling * dstNzC0Stride / sizeof(T),
            src + highFractal * highTiling * srcDValue / sizeof(T) + widthFractal * widthTiling / sizeof(T),
            intriParamsBase2);
    }
}

template <typename T>
__aicore__ inline void DataCopyL12GMImpl(__gm__ T* dst, __cbuf__ T* src, const DataCopyParams& intriParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L1 Buffer(A1/B1) to GM");
}

template <typename T>
__aicore__ inline void DataCopyUB2GMNZ2NDImplBase(
    __gm__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t height, uint16_t width, uint16_t srcNStride, uint16_t dstDStride)
{
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(srcAddr) % ONE_BLK_SIZE == 0)),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "srcAddr address should be 32B aligned \n"));
    const uint16_t blkCntLimit = UINT12_MAX; // DataCopy max blockCount is 4095
    const uint16_t repeatTime = height / blkCntLimit;
    const uint16_t tailBlock = height % blkCntLimit;
    const uint16_t widthBlkNum = width / BLOCK_CUBE; // 1 nzMatrix has x columns of 16 elements

    for (uint16_t i = 0; i < widthBlkNum; ++i) {
        uint16_t blockLen = BLOCK_CUBE * sizeof(T) / 32;
        uint16_t dstStride = static_cast<uint16_t>((dstDStride - BLOCK_CUBE) * sizeof(T) / 32);
        for (uint16_t j = 0; j < repeatTime; ++j) {
            DataCopyUB2GMImpl(
                dstAddr + i * BLOCK_CUBE + j * blkCntLimit * dstDStride,
                srcAddr + i * srcNStride * BLOCK_CUBE + j * blkCntLimit * BLOCK_CUBE,
                {blkCntLimit, blockLen, 0, dstStride});
        }
        if (tailBlock) {
            DataCopyUB2GMImpl(
                dstAddr + i * BLOCK_CUBE + repeatTime * blkCntLimit * dstDStride,
                srcAddr + i * srcNStride * BLOCK_CUBE + repeatTime * blkCntLimit * BLOCK_CUBE,
                {tailBlock, blockLen, 0, dstStride});
        }
    }
}

template <typename T>
__aicore__ inline void DataCopyUB2GMNZ2NDImplNotAlign(
    __gm__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t height, uint16_t width, uint16_t srcNStride, uint16_t dstDStride)
{
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(srcAddr) % ONE_BLK_SIZE == 0)),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "srcAddr address should be 32B aligned \n"));
    const uint16_t widthBlkNum = width / BLOCK_CUBE; // 1 nzMatrix has x columns of 16 elements

    for (uint16_t i = 0; i < widthBlkNum; ++i) {
        uint16_t blockLen = BLOCK_CUBE * sizeof(T) / 32;
        uint16_t dstStride = static_cast<uint16_t>((dstDStride - BLOCK_CUBE) * sizeof(T) / 32);
        for (uint16_t j = 0; j < height; ++j) {
            DataCopyUB2GMImpl(
                dstAddr + i * BLOCK_CUBE + j * dstDStride, srcAddr + i * srcNStride * BLOCK_CUBE + j * BLOCK_CUBE,
                {1, blockLen, 0, dstStride});
        }
    }
}

template <typename T>
__aicore__ inline void DataCopyUB2GMNZ2NDImpl(__gm__ T* dst, __ubuf__ T* src, const Nz2NdParamsFull& intriParams)
{
    ASCENDC_DEBUG_ASSERT(
        ((TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(src)) % ONE_BLK_SIZE) == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "src address should be 32B aligned \n"));
    ASCENDC_ASSERT((sizeof(T) == sizeof(int16_t) || sizeof(T) == sizeof(int32_t)), {
        KERNEL_LOG(KERNEL_ERROR, "DataCopy NZ2ND only supports dtype B16 and B32");
    });
    uint16_t ndNum = intriParams.ndNum;
    uint16_t nValue = intriParams.nValue;
    uint16_t dValue = intriParams.dValue;
    uint16_t srcNdMatrixStride = intriParams.srcNdMatrixStride;
    uint16_t srcNStride = intriParams.srcNStride;
    uint16_t dstDStride = intriParams.dstDStride;
    uint16_t dstNdMatrixStride = intriParams.dstNdMatrixStride;

    if (ndNum != 1) {
        ASCENDC_ASSERT(((srcNdMatrixStride * BLOCK_CUBE * BLOCK_CUBE) % (nValue * sizeof(T)) == 0), {
            KERNEL_LOG(KERNEL_ERROR, "element num between nzMatrix head must be divisible by (nValue*sizeof(T))");
        });
    }
    ASCENDC_ASSERT((dValue % BLOCK_CUBE == 0), { KERNEL_LOG(KERNEL_ERROR, "dValue must be divisible by 16"); });
    if (dstDStride % (ONE_BLK_SIZE / sizeof(T)) == 0) { // align
        for (uint16_t i = 0; i < ndNum; ++i) {
            DataCopyUB2GMNZ2NDImplBase(
                dst + i * dstNdMatrixStride, src + i * srcNdMatrixStride * BLOCK_CUBE * BLOCK_CUBE, nValue, dValue,
                srcNStride, dstDStride);
        }
    } else {
        for (uint16_t i = 0; i < ndNum; ++i) {
            DataCopyUB2GMNZ2NDImplNotAlign(
                dst + i * dstNdMatrixStride, src + i * srcNdMatrixStride * BLOCK_CUBE * BLOCK_CUBE, nValue, dValue,
                srcNStride, dstDStride);
        }
    }
}

/* **************************************************************************************************
 * Copy                                             *
 * ************************************************************************************************* */
// Copy::Level 0 - mask bit mode
template <typename T, bool isSetMask = true>
[[deprecated("NOTICE: Copy is not deprecated. Currently, Copy is an unsupported API on current device. "
             "Please check your code!")]] __aicore__ inline void
CopyImpl(__ubuf__ T* dst, __ubuf__ T* src, uint64_t mask[], uint8_t repeatTime, const CopyRepeatParams& repeatParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "Copy");
}

// Copy::Level 0 - mask count mode
template <typename T, bool isSetMask = true>
[[deprecated("NOTICE: Copy is not deprecated. Currently, Copy is an unsupported API on current device. "
             "Please check your code!")]] __aicore__ inline void
CopyImpl(__ubuf__ T* dst, __ubuf__ T* src, uint64_t mask, uint8_t repeatTime, const CopyRepeatParams& repeatParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "Copy");
}

/* **************************************************************************************************
 * DataCopy Enhanced                                             *
 * ************************************************************************************************* */
// f16 to f16
__aicore__ inline void DataCopyMatrixL0C2UBImpl(
    __ubuf__ half* dst, __cc__ half* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L0C Buffer(CO1) to UB(CO2) with type half");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    if (enhancedParams.deqScale == DeqScale::DEQ) {
        copy_matrix_cc_to_ubuf(
            (__ubuf__ half*)dst, (__cc__ half*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_F16_MUL);
    } else {
        if (enhancedParams.isRelu) {
            copy_matrix_cc_to_ubuf(
                (__ubuf__ half*)dst, (__cc__ half*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
                intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE_RELU);
        } else {
            copy_matrix_cc_to_ubuf(
                (__ubuf__ half*)dst, (__cc__ half*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
                intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE);
        }
    }
#endif
}

// f32 to f32
__aicore__ inline void DataCopyMatrixL0C2UBImpl(
    __ubuf__ float* dst, __cc__ float* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L0C Buffer(CO1) to UB(CO2) with type float");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    if (enhancedParams.isRelu) {
        copy_matrix_cc_to_ubuf(
            (__ubuf__ float*)dst, (__cc__ float*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE_RELU);
    } else {
        copy_matrix_cc_to_ubuf(
            (__ubuf__ float*)dst, (__cc__ float*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE);
    }
#endif
}

// s32 to s32
__aicore__ inline void DataCopyMatrixL0C2UBImpl(
    __ubuf__ int32_t* dst, __cc__ int32_t* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L0C Buffer(CO1) to UB(CO2) with type int32_t");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    if (enhancedParams.isRelu) {
        copy_matrix_cc_to_ubuf(
            (__ubuf__ int32_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE_RELU);
    } else {
        copy_matrix_cc_to_ubuf(
            (__ubuf__ int32_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE);
    }
#endif
}

// u32 to u32
__aicore__ inline void DataCopyMatrixL0C2UBImpl(
    __ubuf__ uint32_t* dst, __cc__ uint32_t* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L0C Buffer(CO1) to UB(CO2) with type uint32_t");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    copy_matrix_cc_to_ubuf(
        (__ubuf__ uint32_t*)dst, (__cc__ uint32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
        intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE);
#endif
}

// f32 to f16
__aicore__ inline void DataCopyMatrixL0C2UBImpl(
    __ubuf__ half* dst, __cc__ float* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L0C Buffer(CO1) to UB(CO2) with type float to half");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    if (enhancedParams.isRelu) {
        copy_matrix_cc_to_ubuf(
            (__ubuf__ half*)dst, (__cc__ float*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_F32toF16_RELU);
    } else {
        copy_matrix_cc_to_ubuf(
            (__ubuf__ half*)dst, (__cc__ float*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_F32toF16_NONE);
    }
#endif
}

// s32 to f16
__aicore__ inline void DataCopyMatrixL0C2UBImpl(
    __ubuf__ half* dst, __cc__ int32_t* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L0C Buffer(CO1) to UB(CO2) with type int32_t to half");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    if (enhancedParams.deqScale == DeqScale::DEQ) {
        copy_matrix_cc_to_ubuf(
            (__ubuf__ half*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_S32toF16_NONE);
    } else if (enhancedParams.deqScale == DeqScale::VDEQ) {
        copy_matrix_cc_to_ubuf(
            (__ubuf__ half*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_S32toF16_DEQSCALE_SPR);
    } else if (enhancedParams.deqScale == DeqScale::VDEQ16) {
        copy_matrix_cc_to_ubuf(
            (__ubuf__ half*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_DEQSCALE_VDEQ16);
    } else if (enhancedParams.deqScale == DeqScale::DEQ16) {
        copy_matrix_cc_to_ubuf(
            (__ubuf__ half*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_DEQSCALE_DEQ16);
    } else {
        ASCENDC_ASSERT(false, {
            KERNEL_LOG(
                KERNEL_ERROR, "When src is int32_t, dst is half in DataCopy from L0C Buffer(CO1) to UB(CO2),"
                              " deqScale should be DEQ / VDEQ / DEQ16 / VDEQ16");
        });
    }
#endif
}

// s32 to s8
__aicore__ inline void DataCopyMatrixL0C2UBImpl(
    __ubuf__ int8_t* dst, __cc__ int32_t* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L0C Buffer(CO1) to UB(CO2) with type int32_t to int8_t");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    if (enhancedParams.deqScale == DeqScale::VDEQ8) {
        copy_matrix_cc_to_ubuf(
            (__ubuf__ int8_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_DEQSCALE_VDEQ8);
    } else if (enhancedParams.deqScale == DeqScale::DEQ8) {
        copy_matrix_cc_to_ubuf(
            (__ubuf__ int8_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_DEQSCALE_DEQ8);
    } else {
        if (enhancedParams.isRelu) {
            copy_matrix_cc_to_ubuf(
                (__ubuf__ int8_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
                intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE_RELU);
        } else {
            copy_matrix_cc_to_ubuf(
                (__ubuf__ int8_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
                intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE);
        }
    }
#endif
}

// s32 to u8
__aicore__ inline void DataCopyMatrixL0C2UBImpl(
    __ubuf__ uint8_t* dst, __cc__ int32_t* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L0C Buffer(CO1) to UB(CO2) with type int32_t to uint8_t");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    if (enhancedParams.deqScale == DeqScale::VDEQ8) {
        copy_matrix_cc_to_ubuf(
            (__ubuf__ uint8_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_DEQSCALE_VDEQ8);
    } else if (enhancedParams.deqScale == DeqScale::DEQ8) {
        copy_matrix_cc_to_ubuf(
            (__ubuf__ uint8_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_DEQSCALE_DEQ8);
    } else {
        if (enhancedParams.isRelu) {
            copy_matrix_cc_to_ubuf(
                (__ubuf__ uint8_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
                intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE_RELU);
        } else {
            copy_matrix_cc_to_ubuf(
                (__ubuf__ uint8_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
                intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE);
        }
    }
#endif
}

// s32 to s16
__aicore__ inline void DataCopyMatrixL0C2UBImpl(
    __ubuf__ int16_t* dst, __cc__ int32_t* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L0C Buffer(CO1) to UB(CO2) with type int32_t to int16_t");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    if (enhancedParams.deqScale == DeqScale::VDEQ16) {
        copy_matrix_cc_to_ubuf(
            (__ubuf__ int16_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_DEQSCALE_VDEQS16);
    } else if (enhancedParams.deqScale == DeqScale::DEQ16) {
        copy_matrix_cc_to_ubuf(
            (__ubuf__ int16_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_DEQSCALE_DEQS16);
    } else {
        if (enhancedParams.isRelu) {
            copy_matrix_cc_to_ubuf(
                (__ubuf__ int16_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
                intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE_RELU);
        } else {
            copy_matrix_cc_to_ubuf(
                (__ubuf__ int16_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
                intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE);
        }
    }
#endif
}

// ---- vector ----
// f16 to f16
__aicore__ inline void DataCopyVectorL0C2UBImpl(
    __ubuf__ half* dst, __cc__ half* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L0C Buffer(CO1) to UB(CO2) with type half");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    if (enhancedParams.deqScale == DeqScale::DEQ) {
        copy_vector_cc_to_ubuf(
            (__ubuf__ half*)dst, (__cc__ half*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_F16_MUL);
    } else {
        if (enhancedParams.isRelu) {
            copy_vector_cc_to_ubuf(
                (__ubuf__ half*)dst, (__cc__ half*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
                intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE_RELU);
        } else {
            copy_vector_cc_to_ubuf(
                (__ubuf__ half*)dst, (__cc__ half*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
                intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE);
        }
    }
#endif
}

// f32 to f32
__aicore__ inline void DataCopyVectorL0C2UBImpl(
    __ubuf__ float* dst, __cc__ float* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L0C Buffer(CO1) to UB(CO2) with type float");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    if (enhancedParams.isRelu) {
        copy_vector_cc_to_ubuf(
            (__ubuf__ float*)dst, (__cc__ float*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE_RELU);
    } else {
        copy_vector_cc_to_ubuf(
            (__ubuf__ float*)dst, (__cc__ float*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE);
    }
#endif
}

// s32 to s32
__aicore__ inline void DataCopyVectorL0C2UBImpl(
    __ubuf__ int32_t* dst, __cc__ int32_t* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L0C Buffer(CO1) to UB(CO2) with type int32_t");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    if (enhancedParams.isRelu) {
        copy_vector_cc_to_ubuf(
            (__ubuf__ int32_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE_RELU);
    } else {
        copy_vector_cc_to_ubuf(
            (__ubuf__ int32_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE);
    }
#endif
}

// u32 to u32
__aicore__ inline void DataCopyVectorL0C2UBImpl(
    __ubuf__ uint32_t* dst, __cc__ uint32_t* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L0C Buffer(CO1) to UB(CO2) with type uint32_t");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    copy_vector_cc_to_ubuf(
        (__ubuf__ uint32_t*)dst, (__cc__ uint32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
        intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE);
#endif
}

// f32 to f16
__aicore__ inline void DataCopyVectorL0C2UBImpl(
    __ubuf__ half* dst, __cc__ float* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L0C Buffer(CO1) to UB(CO2) with type float to half");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    if (enhancedParams.isRelu) {
        copy_vector_cc_to_ubuf(
            (__ubuf__ half*)dst, (__cc__ float*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_F32toF16_RELU);
    } else {
        copy_vector_cc_to_ubuf(
            (__ubuf__ half*)dst, (__cc__ float*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_F32toF16_NONE);
    }
#endif
}

// s32 to f16
__aicore__ inline void DataCopyVectorL0C2UBImpl(
    __ubuf__ half* dst, __cc__ int32_t* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L0C Buffer(CO1) to UB(CO2) with type int32_t to half");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    if (enhancedParams.deqScale == DeqScale::DEQ) {
        copy_vector_cc_to_ubuf(
            (__ubuf__ half*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_S32toF16_NONE);
    } else if (enhancedParams.deqScale == DeqScale::VDEQ) {
        copy_vector_cc_to_ubuf(
            (__ubuf__ half*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_S32toF16_DEQSCALE_SPR);
    } else if (enhancedParams.deqScale == DeqScale::VDEQ16) {
        copy_vector_cc_to_ubuf(
            (__ubuf__ half*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_DEQSCALE_VDEQ16);
    } else if (enhancedParams.deqScale == DeqScale::DEQ16) {
        copy_vector_cc_to_ubuf(
            (__ubuf__ half*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_DEQSCALE_DEQ16);
    } else {
        ASCENDC_ASSERT(false, {
            KERNEL_LOG(
                KERNEL_ERROR, "When src is int32_t, dst is half in DataCopy from L0C Buffer(CO1) to UB(CO2),"
                              " deqScale should be DEQ / VDEQ / DEQ16 / VDEQ16");
        });
    }
#endif
}

// s32 to s8
__aicore__ inline void DataCopyVectorL0C2UBImpl(
    __ubuf__ int8_t* dst, __cc__ int32_t* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L0C Buffer(CO1) to UB(CO2) with type int32_t to int8_t");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    if (enhancedParams.deqScale == DeqScale::VDEQ8) {
        copy_vector_cc_to_ubuf(
            (__ubuf__ int8_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_DEQSCALE_VDEQ8);
    } else if (enhancedParams.deqScale == DeqScale::DEQ8) {
        copy_vector_cc_to_ubuf(
            (__ubuf__ int8_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_DEQSCALE_DEQ8);
    } else {
        if (enhancedParams.isRelu) {
            copy_vector_cc_to_ubuf(
                (__ubuf__ int8_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
                intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE_RELU);
        } else {
            copy_vector_cc_to_ubuf(
                (__ubuf__ int8_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
                intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE);
        }
    }
#endif
}

// s32 to u8
__aicore__ inline void DataCopyVectorL0C2UBImpl(
    __ubuf__ uint8_t* dst, __cc__ int32_t* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L0C Buffer(CO1) to UB(CO2) with type int32_t to uint8_t");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    if (enhancedParams.deqScale == DeqScale::VDEQ8) {
        copy_vector_cc_to_ubuf(
            (__ubuf__ uint8_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_DEQSCALE_VDEQ8);
    } else if (enhancedParams.deqScale == DeqScale::DEQ8) {
        copy_vector_cc_to_ubuf(
            (__ubuf__ uint8_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_DEQSCALE_DEQ8);
    } else {
        if (enhancedParams.isRelu) {
            copy_vector_cc_to_ubuf(
                (__ubuf__ uint8_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
                intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE_RELU);
        } else {
            copy_vector_cc_to_ubuf(
                (__ubuf__ uint8_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
                intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE);
        }
    }
#endif
}

// s32 to s16
__aicore__ inline void DataCopyVectorL0C2UBImpl(
    __ubuf__ int16_t* dst, __cc__ int32_t* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L0C Buffer(CO1) to UB(CO2) with type int32_t to int16_t");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    if (enhancedParams.deqScale == DeqScale::VDEQ16) {
        copy_vector_cc_to_ubuf(
            (__ubuf__ int16_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_DEQSCALE_VDEQS16);
    } else if (enhancedParams.deqScale == DeqScale::DEQ16) {
        copy_vector_cc_to_ubuf(
            (__ubuf__ int16_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
            intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_DEQSCALE_DEQS16);
    } else {
        if (enhancedParams.isRelu) {
            copy_vector_cc_to_ubuf(
                (__ubuf__ int16_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
                intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE_RELU);
        } else {
            copy_vector_cc_to_ubuf(
                (__ubuf__ int16_t*)dst, (__cc__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
                intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE);
        }
    }
#endif
}

// -------- ub to l0c --------
// ---- matrix ----
// f16 to f16
__aicore__ inline void DataCopyMatrixUB2L0CImpl(
    __cc__ half* dst, __ubuf__ half* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from UB(CO2) to L0C Buffer(CO1) with type half");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(src)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "src address should be 32B aligned \n"));
    copy_matrix_ubuf_to_cc(
        (__cc__ half*)dst, (__ubuf__ half*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
        intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE);
#endif
}

// f32 to f32
__aicore__ inline void DataCopyMatrixUB2L0CImpl(
    __cc__ float* dst, __ubuf__ float* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from UB(CO2) to L0C Buffer(CO1) with type float");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(src)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "src address should be 32B aligned \n"));
    copy_matrix_ubuf_to_cc(
        (__cc__ float*)dst, (__ubuf__ float*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
        intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE);
#endif
}

// f16 to f32
__aicore__ inline void DataCopyMatrixUB2L0CImpl(
    __cc__ float* dst, __ubuf__ half* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from UB(CO2) to L0C Buffer(CO1) with type half to float");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(src)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "src address should be 32B aligned \n"));
    copy_matrix_ubuf_to_cc(
        (__cc__ float*)dst, (__ubuf__ half*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
        intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_F16toF32_NONE);
#endif
}

// s32 to s32
__aicore__ inline void DataCopyMatrixUB2L0CImpl(
    __cc__ int32_t* dst, __ubuf__ int32_t* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from UB(CO2) to L0C Buffer(CO1) with type int32_t");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(src)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "src address should be 32B aligned \n"));
    copy_matrix_ubuf_to_cc(
        (__cc__ int32_t*)dst, (__ubuf__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
        intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE);
#endif
}

// u32 to u32
__aicore__ inline void DataCopyMatrixUB2L0CImpl(
    __cc__ uint32_t* dst, __ubuf__ uint32_t* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from UB(CO2) to L0C Buffer(CO1) with type uint32_t");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(src)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "src address should be 32B aligned \n"));
    copy_matrix_ubuf_to_cc(
        (__cc__ uint32_t*)dst, (__ubuf__ uint32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
        intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE);
#endif
}

// ---- vector ----
// f16 to f16
__aicore__ inline void DataCopyVectorUB2L0CImpl(
    __cc__ half* dst, __ubuf__ half* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from UB(CO2) to L0C Buffer(CO1) with type half");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(src)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "src address should be 32B aligned \n"));
    copy_vector_ubuf_to_cc(
        (__cc__ half*)dst, (__ubuf__ half*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
        intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE);
#endif
}

// f32 to f32
__aicore__ inline void DataCopyVectorUB2L0CImpl(
    __cc__ float* dst, __ubuf__ float* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from UB(CO2) to L0C Buffer(CO1) with type float");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(src)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "src address should be 32B aligned \n"));
    copy_vector_ubuf_to_cc(
        (__cc__ float*)dst, (__ubuf__ float*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
        intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE);
#endif
}

// f16 to f32
__aicore__ inline void DataCopyVectorUB2L0CImpl(
    __cc__ float* dst, __ubuf__ half* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from UB(CO2) to L0C Buffer(CO1) with type half to float");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(src)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "src address should be 32B aligned \n"));
    copy_vector_ubuf_to_cc(
        (__cc__ float*)dst, (__ubuf__ half*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
        intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_F16toF32_NONE);
#endif
}

// s32 to s32
__aicore__ inline void DataCopyVectorUB2L0CImpl(
    __cc__ int32_t* dst, __ubuf__ int32_t* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from UB(CO2) to L0C Buffer(CO1) with type int32_t");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(src)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "src address should be 32B aligned \n"));
    copy_vector_ubuf_to_cc(
        (__cc__ int32_t*)dst, (__ubuf__ int32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
        intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE);
#endif
}

// u32 to u32
__aicore__ inline void DataCopyVectorUB2L0CImpl(
    __cc__ uint32_t* dst, __ubuf__ uint32_t* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from UB(CO2) to L0C Buffer(CO1) with type uint32_t");
#else
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(src)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "src address should be 32B aligned \n"));
    copy_vector_ubuf_to_cc(
        (__cc__ uint32_t*)dst, (__ubuf__ uint32_t*)src, enhancedParams.sidStoreMode, intriParams.blockCount,
        intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, CRMODE_NONE);
#endif
}

template <typename T, typename U>
__aicore__ inline void DataCopyL12L0CImpl(
    __cc__ T* dst, __cbuf__ U* src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L1 Buffer(A1/B1) to L0C Buffer(CO1)");
}

/* **************************************************************************************************
 * DataCopy                                             *
 * ************************************************************************************************* */

__aicore__ inline void DataCopyL0C2UBImpl(
    __ubuf__ int8_t* dst, __cc__ int8_t* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L0C Buffer(CO1) to UB(CO2) with type int8_t");
}

// ------------  ------------
template <typename T, typename U>
__aicore__ inline void DataCopyL0C2UBImpl(
    __ubuf__ T* dst, __cc__ U* src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
{
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    if ((enhancedParams.deqScale == DeqScale::DEQ8) || (enhancedParams.deqScale == DeqScale::DEQ16)) {
        uint64_t deqScaleSpr = enhancedParams.deqValue;
        if (enhancedParams.isRelu) {
            deqScaleSpr |= 1ULL << 47;
        }
        if (IsSameType<T, int8_t>::value) {
            deqScaleSpr |= 1ULL << 46;
        }
        set_deqscale(deqScaleSpr);
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
        SetFlag<HardEvent::S_V>(eventIdSToV);
        WaitFlag<HardEvent::S_V>(eventIdSToV);
    } else if (
        (enhancedParams.deqScale == DeqScale::VDEQ8) || (enhancedParams.deqScale == DeqScale::VDEQ16) ||
        (enhancedParams.deqScale == DeqScale::VDEQ)) {
        uint64_t deqScaleSpr = enhancedParams.deqTensorAddr >> 5;
        if (enhancedParams.isRelu) {
            deqScaleSpr |= 1ULL << 15;
        }
        set_deqscale(deqScaleSpr);
#if ASCENDC_CPU_DEBUG
        uint64_t deqScaleSprAddr = enhancedParams.deqTensorAddr;
        SetModelDeqTensor((void*)deqScaleSprAddr);
        deqScaleSprAddr = 0;
        if (enhancedParams.isRelu) {
            deqScaleSprAddr = static_cast<uint64_t>(1) << static_cast<uint64_t>(15);
        }
        set_deqscale(deqScaleSprAddr);
#endif
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
        SetFlag<HardEvent::S_V>(eventIdSToV);
        WaitFlag<HardEvent::S_V>(eventIdSToV);
    } else if (enhancedParams.deqScale == DeqScale::DEQ) { // half -> half / int32_t -> half
        uint64_t deqScaleSpr = enhancedParams.deqValue;
        set_deqscale(deqScaleSpr);
        event_t eventIdSToV = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::S_V));
        SetFlag<HardEvent::S_V>(eventIdSToV);
        WaitFlag<HardEvent::S_V>(eventIdSToV);
    }
    if (enhancedParams.blockMode == BlockMode::BLOCK_MODE_MATRIX) {
        DataCopyMatrixL0C2UBImpl(dst, src, intriParams, enhancedParams);
    } else if (enhancedParams.blockMode == BlockMode::BLOCK_MODE_VECTOR) {
        DataCopyVectorL0C2UBImpl(dst, src, intriParams, enhancedParams);
    } else {
        ASCENDC_ASSERT(false, {
            KERNEL_LOG(
                KERNEL_ERROR, "In DataCopy from L0C Buffer(CO1) to UB(CO2), blockMode should be "
                              "BLOCK_MODE_MATRIX / BLOCK_MODE_VECTOR");
        });
    }
}

__aicore__ inline void DataCopyUB2L0CImpl(
    __cc__ int8_t* dst, __ubuf__ int8_t* src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from UB(CO2) to L0C Buffer(CO1) with type int8_t");
}

template <typename T, typename U>
__aicore__ inline void DataCopyUB2L0CImpl(
    __cc__ T* dst, __ubuf__ U* src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
{
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(src)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "src address should be 32B aligned \n"));
    if (enhancedParams.blockMode == BlockMode::BLOCK_MODE_MATRIX) {
        DataCopyMatrixUB2L0CImpl(dst, src, intriParams, enhancedParams);
    } else if (enhancedParams.blockMode == BlockMode::BLOCK_MODE_VECTOR) {
        DataCopyVectorUB2L0CImpl(dst, src, intriParams, enhancedParams);
    } else {
        ASCENDC_ASSERT(false, {
            KERNEL_LOG(
                KERNEL_ERROR, "In DataCopy from UB(CO2) to L0C Buffer(CO1), blockMode should be "
                              "BLOCK_MODE_MATRIX / BLOCK_MODE_VECTOR");
        });
    }
}

template <typename T>
__aicore__ inline void DataCopySliceGm2UBImpl(__ubuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams)
{
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    uint32_t offsetSrc = 0;
    uint32_t offsetDst = 0;
    for (uint32_t i = 0; i < intriParams.blockCount; i++) {
        offsetSrc = offsetSrc + i * (intriParams.blockLen * ONE_BLK_SIZE + intriParams.srcStride);
        offsetDst = offsetDst + i * (intriParams.blockLen * ONE_BLK_SIZE + intriParams.dstStride);
        DataCopyGM2UBImpl(dst + offsetDst / sizeof(T), src + offsetSrc / sizeof(T), {1, intriParams.blockLen, 0, 0});
    }
}

template <typename T>
__aicore__ inline void DataCopySliceUB2GMImpl(__gm__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(src)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "src address should be 32B aligned \n"));
    uint32_t offsetSrc = 0;
    uint32_t offsetDst = 0;
    for (uint32_t i = 0; i < intriParams.blockCount; i++) {
        offsetSrc = offsetSrc + i * (intriParams.blockLen * ONE_BLK_SIZE + intriParams.srcStride);
        offsetDst = offsetDst + i * (intriParams.blockLen * ONE_BLK_SIZE + intriParams.dstStride);
        DataCopyUB2GMImpl(dst + offsetDst / sizeof(T), src + offsetSrc / sizeof(T), {1, intriParams.blockLen, 0, 0});
    }
}

template <typename T>
[[deprecated("NOTICE: DataCopyPad is not deprecated. Currently, DataCopyPad is an unsupported API on current device. "
             "Please check your code!")]] __aicore__ inline void
DataCopyPadGm2UBImpl(
    __ubuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams, const DataCopyPadParams& padParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopyPad from GM to UB(VECIN/VECOUT)");
}

template <typename T>
[[deprecated("NOTICE: DataCopyPad is not deprecated. Currently, DataCopyPad is an unsupported API on current device. "
             "Please check your code!")]] __aicore__ inline void
DataCopyPadGm2UBImpl(
    __ubuf__ T* dst, __gm__ T* src, const DataCopyExtParams& intriParams, const DataCopyPadExtParams<T>& padParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopyPad from GM to UB(VECIN/VECOUT)");
}

template <typename T>
[[deprecated("NOTICE: DataCopyPad is not deprecated. Currently, DataCopyPad is an unsupported API on current device. "
             "Please check your code!")]] __aicore__ inline void
DataCopyPadUB2GMImpl(__gm__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopyPad from UB(VECIN/VECOUT) to GM");
}

template <typename T>
[[deprecated("NOTICE: DataCopyPad is not deprecated. Currently, DataCopyPad is an unsupported API on current device. "
             "Please check your code!")]] __aicore__ inline void
DataCopyPadUB2GMImpl(__gm__ T* dst, __ubuf__ T* src, const DataCopyExtParams& intriParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopyPad from UB(VECIN/VECOUT) to GM");
}

template <typename T>
__aicore__ inline void DataCopyGM2UBSingleImpl(
    __ubuf__ T* dst, __gm__ T* src, const Nd2NzParams& intriParams, const int copyTime, const int computeNum)
{
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    const uint16_t& nValue = intriParams.nValue;
    const uint16_t& dValue = intriParams.dValue;
    const uint16_t& computeLen = computeNum * sizeof(T);
    const uint16_t& c0Count = DEFAULT_C0_SIZE / sizeof(T);
    const uint16_t& maxC0Count = MAX_REPEAT_TIMES * c0Count;
    const uint16_t& maxdValue = MAX_REPEAT_TIMES * dValue;
    const uint16_t& dstNzNStride = intriParams.dstNzNStride;
    const uint16_t& dstNzC0Stride = intriParams.dstNzC0Stride;
    const uint16_t& repeatCount = nValue / MAX_REPEAT_TIMES;
    const uint16_t& repeatTail = nValue % MAX_REPEAT_TIMES;
    const uint16_t& srcCopyStartOffset = copyTime * c0Count;
    const uint16_t& dstCopyStartOffset = copyTime * dstNzC0Stride * (DEFAULT_C0_SIZE / sizeof(T));
    ASSERT(((dValue * sizeof(T) % DEFAULT_C0_SIZE) == 0) && "dValue must be 32B aligned");
    DataCopyParams copyParams = {
        MAX_REPEAT_TIMES, static_cast<uint16_t>(computeLen / DEFAULT_C0_SIZE),
        static_cast<uint16_t>(intriParams.srcDValue - computeLen / DEFAULT_C0_SIZE),
        static_cast<uint16_t>((dstNzNStride - DEFAULT_C0_SIZE) / DEFAULT_C0_SIZE)};
    for (int repeatTime = 0; repeatTime < repeatCount; ++repeatTime) {
        DataCopyGM2UBImpl(
            (__ubuf__ void*)(dst + dstCopyStartOffset + repeatTime * maxC0Count),
            (__gm__ void*)(src + srcCopyStartOffset + repeatTime * maxdValue), copyParams);
    }
    copyParams.blockCount = repeatTail;
    if (repeatTail != 0) {
        int dstOffset = (dstCopyStartOffset + repeatCount * MAX_REPEAT_TIMES * c0Count);
        int srcOffset = (srcCopyStartOffset + repeatCount * MAX_REPEAT_TIMES * dValue);
        DataCopyGM2UBImpl((__ubuf__ void*)(dst + dstOffset), (__gm__ void*)(src + srcOffset), copyParams);
    }
}

template <typename T>
__aicore__ inline void DataCopyGM2UBND2NZImpl(__ubuf__ T* dst, __gm__ T* src, const Nd2NzParams& intriParams)
{
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(dst)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dst address should be 32B aligned \n"));
    const uint16_t& ndNum = intriParams.ndNum;
    const uint16_t& dValue = intriParams.dValue;
    const uint16_t& srcNdMatrixStride = intriParams.srcNdMatrixStride;
    const uint16_t& srcDValue = intriParams.srcDValue;
    const uint16_t& dstNzC0Stride = intriParams.dstNzC0Stride;
    const uint16_t& dstNzNStride = intriParams.dstNzNStride;
    const uint16_t& dstNzMatrixStride = intriParams.dstNzMatrixStride;
    const uint16_t& c0Count = DEFAULT_C0_SIZE / sizeof(T);
    for (int index = 0; index < ndNum; ++index) {
        int16_t copyNum = (dValue + c0Count - 1) / c0Count;
        for (int copyTime = 0; copyTime < copyNum; ++copyTime) {
            int computeCount = (dValue >= (copyTime + 1) * c0Count) ? c0Count : (dValue % c0Count);
            DataCopyGM2UBSingleImpl(
                dst + dstNzMatrixStride, src + srcNdMatrixStride, intriParams, copyTime, computeCount);
        }
    }
}

template <typename T>
[[deprecated("NOTICE: DataCopyPad is not deprecated. Currently, DataCopyPad is an unsupported API on current device. "
             "Please check your code!")]] __aicore__ inline void
DataCopyPadUB2L1Impl(__cbuf__ T* dst, __ubuf__ T* src, const DataCopyExtParams& intriParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopyPad from UB(VECIN/VECOUT) to L1 Buffer(TSCM)");
}

template <typename T>
[[deprecated("NOTICE: DataCopyPad is not deprecated. Currently, DataCopyPad is an unsupported API on current device. "
             "Please check your code!")]] __aicore__ inline void
DataCopyPadUB2L1Impl(__cbuf__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopyPad from UB(VECIN/VECOUT) to L1 Buffer(TSCM)");
}

template <typename T, typename U>
__aicore__ inline void DataCopyL0C2L1Impl(__cbuf__ T* dst, __cc__ U* src, const DataCopyCO12DstParams& intriParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L0C Buffer(CO1) to L1 Buffer(A1/B1)");
}

template <typename T, typename U>
__aicore__ inline void DataCopyL0C2GMImpl(__gm__ T* dst, __cc__ U* src, const DataCopyCO12DstParams& intriParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopy from L0C Buffer(CO1) to GM");
}

template <typename T>
__aicore__ inline __inout_pipe__(MTE1) void DataCopyL12UBIntf(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& intriParams)
{
    DataCopyL12UBImpl((__ubuf__ PrimT<T>*)dst.GetPhyAddr(), (__cbuf__ PrimT<T>*)src.GetPhyAddr(), intriParams);
}

#pragma begin_pipe(V)
template <typename T>
__aicore__ inline void DataCopyUB2L0CIntf(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
    DataCopyUB2L0CImpl(
        (__cc__ PrimT<T>*)dst.GetPhyAddr(), (__ubuf__ PrimT<T>*)src.GetPhyAddr(), intriParams, enhancedParams);
}

template <typename T>
__aicore__ inline void DataCopyUB2UBIntf(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& intriParams)
{
    DataCopyUB2UBImpl((__ubuf__ PrimT<T>*)dst.GetPhyAddr(), (__ubuf__ PrimT<T>*)src.GetPhyAddr(), intriParams);
}
#pragma end_pipe

template <typename T>
__aicore__ inline void DataCopyPadL12GMImpl(__gm__ T* dst, __cbuf__ T* src, const DataCopyParams& intriParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopyPad from L1 Buffer(A1/B1)/UB(C1) to GM");
}

template <typename T>
__aicore__ inline void DataCopyPadL12GMImpl(__gm__ T* dst, __cbuf__ T* src, const DataCopyExtParams& intriParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopyPad from L1 Buffer(A1/B1)/UB(C1) to GM");
}

template <typename T>
__aicore__ inline void DataCopyPadGM2L1Impl(
    __cbuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams, const DataCopyPadParams& padParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopyPad from GM to L1 Buffer(A1/B1)/UB(C1)");
}

template <typename T>
__aicore__ inline void DataCopyPadGM2L1Impl(
    __cbuf__ T* dst, __gm__ T* src, const DataCopyExtParams& intriParams, const DataCopyPadExtParams<T>& padParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "DataCopyPad from GM to L1 Buffer(A1/B1)/UB(C1)");
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_DATA_COPY_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_IMPL_H__
#endif
