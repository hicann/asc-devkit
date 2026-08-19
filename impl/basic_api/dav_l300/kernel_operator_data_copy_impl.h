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
 * \brief AscendC l300 support data copy api.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_l300/kernel_operator_data_copy_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_DATA_COPY_IMPL_H
#define ASCENDC_MODULE_OPERATOR_DATA_COPY_IMPL_H

#include "kernel_operator_common_impl.h"
#include "kernel_operator_vec_duplicate_impl.h"
#include "kernel_operator_vec_template_impl.h"

namespace AscendC {
/* **************************************************************************************************
 * DataCopy                                             *
 * ************************************************************************************************* */
constexpr uint8_t BYTE_32_ALIGN = 32; // in unit of 32 bytes
// GM -> L1: copy_gm_to_cbuf_align
template <typename T, bool isDataCopyPad = false>
__aicore__ inline void CopyGmToCbufAlign(
    __cbuf__ T* dst, __gm__ T* src, const uint16_t blockCount, const uint32_t blockLen, const uint8_t leftPadding,
    const uint8_t rightPadding, const uint32_t srcStride, const uint32_t dstStride)
{
    uint32_t burstLength = isDataCopyPad ? blockLen : (blockLen * BYTE_32_ALIGN);
    uint64_t actSrcStride = isDataCopyPad ? srcStride : (srcStride * BYTE_32_ALIGN);
    uint32_t actDstStride = dstStride;

    if constexpr (sizeof(T) == B64_BYTE_SIZE) {
        copy_gm_to_cbuf_align(
            (__cbuf__ uint32_t*)dst, (__gm__ uint32_t*)src, 0, blockCount, burstLength, leftPadding * 2,
            rightPadding * 2, actSrcStride, actDstStride);
    } else if constexpr (sizeof(T) == B32_BYTE_SIZE) {
        copy_gm_to_cbuf_align(
            (__cbuf__ uint32_t*)dst, (__gm__ uint32_t*)src, 0, blockCount, burstLength, leftPadding, rightPadding,
            actSrcStride, actDstStride);
    } else if constexpr (sizeof(T) == B16_BYTE_SIZE) {
        copy_gm_to_cbuf_align(
            (__cbuf__ uint16_t*)dst, (__gm__ uint16_t*)src, 0, blockCount, burstLength, leftPadding, rightPadding,
            actSrcStride, actDstStride);
    } else if constexpr (sizeof(T) == B8_BYTE_SIZE) {
        copy_gm_to_cbuf_align(
            (__cbuf__ uint8_t*)dst, (__gm__ uint8_t*)src, 0, blockCount, burstLength, leftPadding, rightPadding,
            actSrcStride, actDstStride);
    } else {
        ASCENDC_ASSERT(
            false, { KERNEL_LOG(KERNEL_ERROR, "unsupported data type of copy from gm to cbuf on current device"); });
    }
}

// L1 -> GM: copy_cbuf_to_gm_align
template <typename T>
__aicore__ inline void CopyCbufToGmAlign(
    __gm__ T* dst, __cbuf__ T* src, const uint16_t blockCount, const uint32_t blockLen, const uint32_t srcStride,
    const uint32_t dstStride)
{
    uint32_t burstLength = blockLen * BYTE_32_ALIGN;
    uint64_t actSrcStride = srcStride;
    uint32_t actDstStride = dstStride * BYTE_32_ALIGN;
    copy_cbuf_to_gm_align(
        (__gm__ uint8_t*)dst, (__cbuf__ uint8_t*)src, 0, blockCount, burstLength, actSrcStride, actDstStride);
}

// GM -> UB: copy_gm_to_ubuf_align: support changing padding value
template <typename T, bool isDataCopyPad = false>
__aicore__ inline void CopyGmToUbufAlign(
    __ubuf__ T* dst, __gm__ T* src, const uint16_t blockCount, const uint32_t blockLen, const uint8_t leftPaddingCount,
    const uint8_t rightPaddingCount, const uint32_t srcStride, const uint32_t dstStride)
{
    ASCENDC_ASSERT(
        (sizeof(T) == B8_BYTE_SIZE || sizeof(T) == B16_BYTE_SIZE || sizeof(T) == B32_BYTE_SIZE ||
         sizeof(T) == B64_BYTE_SIZE),
        { KERNEL_LOG(KERNEL_ERROR, "unsupported data type of copy from gm to ubuf on this version"); });
    uint32_t burstLength = isDataCopyPad ? blockLen : (blockLen * BYTE_32_ALIGN);
    uint64_t srcStride300 = isDataCopyPad ? srcStride : (srcStride * BYTE_32_ALIGN);
    uint32_t dstStride300 = dstStride;
    uint8_t leftPaddingCountT = leftPaddingCount * sizeof(T);
    uint8_t rightPaddingCountT = rightPaddingCount * sizeof(T);
    copy_gm_to_ubuf_align(
        (__ubuf__ uint8_t*)dst, (__gm__ uint8_t*)src, 0, blockCount, burstLength, leftPaddingCountT, rightPaddingCountT,
        srcStride300, dstStride300);
}

// UB -> GM: copy_ubuf_to_gm_align: Note: switch the order between srcStride and dstStride!!!!
template <typename T>
__aicore__ inline void CopyUbufToGmAlign(
    __gm__ T* dst, __ubuf__ T* src, const uint16_t blockCount, const uint32_t blockLen, const uint32_t srcStride,
    const uint32_t dstStride)
{
    uint32_t burstLength = blockLen * BYTE_32_ALIGN;
    uint32_t srcStride300 = srcStride;
    uint64_t dstStride300 = dstStride * BYTE_32_ALIGN;
    copy_ubuf_to_gm_align(
        (__gm__ uint8_t*)dst, (__ubuf__ uint8_t*)src, 0, blockCount, burstLength, srcStride300, dstStride300);
}

template <typename T>
__aicore__ inline void DataCopyGM2UBImpl(__ubuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams)
{
    CopyGmToUbufAlign<T, false>(
        dst, src, intriParams.blockCount, intriParams.blockLen, 0, 0, intriParams.srcStride, intriParams.dstStride);
}

template <typename T>
__aicore__ inline void DataCopyGM2L1Impl(__cbuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams)
{
    CopyGmToCbufAlign<T, false>(
        dst, src, intriParams.blockCount, intriParams.blockLen, 0, 0, intriParams.srcStride, intriParams.dstStride);
}

template <typename T>
__aicore__ inline void DataCopyUB2GMImpl(__gm__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
#ifdef ASCENDC_CPU_DEBUG
    DataCopyWithAtomic(dst, src, intriParams);
#endif // ASCENDC_CPU_DEBUG
    CopyUbufToGmAlign(
        dst, src, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride, intriParams.dstStride);
}

template <typename T>
__aicore__ inline void DataCopyUB2UBImpl(__ubuf__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
    copy_ubuf_to_ubuf(
        (__ubuf__ void*)dst, (__ubuf__ void*)src, 0, intriParams.blockCount, intriParams.blockLen,
        intriParams.srcStride, intriParams.dstStride);
}

template <typename T>
__aicore__ inline void DataCopyUB2L1Impl(__cbuf__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
    copy_ubuf_to_cbuf(
        (__cbuf__ void*)dst, (__ubuf__ void*)src, 0, intriParams.blockCount, intriParams.blockLen,
        intriParams.srcStride, intriParams.dstStride);
}

template <typename T>
__aicore__ inline void DataCopyL12UBImpl(__ubuf__ T* dst, __cbuf__ T* src, const DataCopyParams& intriParams)
{
    copy_cbuf_to_ubuf(
        (__ubuf__ void*)dst, (__cbuf__ void*)src, 0, intriParams.blockCount, intriParams.blockLen,
        intriParams.srcStride, intriParams.dstStride);
}

template <typename T>
__aicore__ inline void DataCopyL12BTImpl(
    const uint64_t dst, __cbuf__ T* src, const uint16_t isenableConv, const DataCopyParams& intriParams)
{
    // ISA: unit of 64B    API: unit of 32B
    uint16_t burstLength = DivCeil(intriParams.blockLen, 2);
    uint16_t dstStride = DivCeil(intriParams.dstStride, 2);
    if constexpr (std::is_same<T, float>::value || std::is_same<T, int32_t>::value || std::is_same<T, half>::value) {
        copy_cbuf_to_bt(
            dst, src, (bool)isenableConv, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride,
            intriParams.dstStride);
    } else {
        ASCENDC_ASSERT(
            false, { KERNEL_LOG(KERNEL_ERROR, "unsupported data type of copy from cbuf to bt on this version"); });
    }
}

template <typename T>
__aicore__ inline void DataCopyL12FBImpl(__fbuf__ T* dst, __cbuf__ T* src, const DataCopyParams& intriParams)
{
    // 该API属于ISASI，并且不支持多次分配内存，只能进行一次AllocTensor操作，然后搬运所有数据到FB。因此直接设置dst =
    // 0确保数据从0开始排布
    dst = 0;

    // ISA/API: Is the burst number in total.
    uint16_t burstNum = intriParams.blockCount;
    // ISA: unit of 128B    API: unit of 32B
    uint16_t burstLen = DivCeil(intriParams.blockLen, 4);
    // ISA/API: unit of 32B
    uint16_t srcGapSize = intriParams.srcStride;
    // ISA: unit of 128B    API: unit of 32B
    uint16_t dstGapSize = DivCeil(intriParams.dstStride, 4);

    copy_cbuf_to_fbuf((__fbuf__ void*)dst, (__cbuf__ void*)src, burstNum, burstLen, srcGapSize, dstGapSize);
}

template <typename T>
__aicore__ inline void TransND2NZ(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t high, uint16_t width, T scalar)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "unsupported data copy from ub to gm nd2nz"); });
}

template <typename T>
__aicore__ inline void DataCopyGM2L1ND2NZImplB16(__cbuf__ T* dst, __gm__ T* src, const Nd2NzParams& intriParams)
{
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNd2Nz(src, workSpace, true, intriParams);
    }
    copy_gm_to_cbuf_multi_nd2nz_b16(
        (__cbuf__ T*)dst, (__gm__ T*)src, 0, intriParams.ndNum, intriParams.nValue, intriParams.dValue,
        intriParams.srcNdMatrixStride, intriParams.srcDValue, intriParams.dstNzC0Stride, intriParams.dstNzNStride,
        intriParams.dstNzMatrixStride);
}

__aicore__ inline void DataCopyGM2L1ND2NZImpl(__cbuf__ half* dst, __gm__ half* src, const Nd2NzParams& intriParams)
{
    DataCopyGM2L1ND2NZImplB16(dst, src, intriParams);
}

__aicore__ inline void DataCopyGM2L1ND2NZImpl(
    __cbuf__ bfloat16_t* dst, __gm__ bfloat16_t* src, const Nd2NzParams& intriParams)
{
    DataCopyGM2L1ND2NZImplB16(dst, src, intriParams);
}

__aicore__ inline void DataCopyGM2L1ND2NZImpl(
    __cbuf__ int16_t* dst, __gm__ int16_t* src, const Nd2NzParams& intriParams)
{
    DataCopyGM2L1ND2NZImplB16(dst, src, intriParams);
}

__aicore__ inline void DataCopyGM2L1ND2NZImpl(
    __cbuf__ uint16_t* dst, __gm__ uint16_t* src, const Nd2NzParams& intriParams)
{
    DataCopyGM2L1ND2NZImplB16(dst, src, intriParams);
}

template <typename T>
__aicore__ inline void DataCopyGM2L1ND2NZImplB32(__cbuf__ T* dst, __gm__ T* src, const Nd2NzParams& intriParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "unsupported b32 data copy from gm to l1 nz2nd"); });
}

__aicore__ inline void DataCopyGM2L1ND2NZImpl(__cbuf__ float* dst, __gm__ float* src, const Nd2NzParams& intriParams)
{
    DataCopyGM2L1ND2NZImplB32(dst, src, intriParams);
}

__aicore__ inline void DataCopyGM2L1ND2NZImpl(
    __cbuf__ int32_t* dst, __gm__ int32_t* src, const Nd2NzParams& intriParams)
{
    DataCopyGM2L1ND2NZImplB32(dst, src, intriParams);
}

__aicore__ inline void DataCopyGM2L1ND2NZImpl(
    __cbuf__ uint32_t* dst, __gm__ uint32_t* src, const Nd2NzParams& intriParams)
{
    DataCopyGM2L1ND2NZImplB32(dst, src, intriParams);
}

template <typename T>
__aicore__ inline void DataCopyGM2L1ND2NZImplB8(__cbuf__ T* dst, __gm__ T* src, const Nd2NzParams& intriParams)
{
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNd2Nz(src, workSpace, true, intriParams);
    }
    copy_gm_to_cbuf_multi_nd2nz_b8(
        (__cbuf__ T*)dst, (__gm__ T*)src, 0, intriParams.ndNum, intriParams.nValue, intriParams.dValue,
        intriParams.srcNdMatrixStride, intriParams.srcDValue, intriParams.dstNzC0Stride, intriParams.dstNzNStride,
        intriParams.dstNzMatrixStride);
}

__aicore__ inline void DataCopyGM2L1ND2NZImpl(__cbuf__ int8_t* dst, __gm__ int8_t* src, const Nd2NzParams& intriParams)
{
    DataCopyGM2L1ND2NZImplB8(dst, src, intriParams);
}

__aicore__ inline void DataCopyGM2L1ND2NZImpl(
    __cbuf__ uint8_t* dst, __gm__ uint8_t* src, const Nd2NzParams& intriParams)
{
    DataCopyGM2L1ND2NZImplB8(dst, src, intriParams);
}

template <typename T>
__aicore__ inline void DataCopyUB2L1ND2NZImpl(__cbuf__ T* dst, __ubuf__ T* src, const Nd2NzParams& intriParams)
{
    uint16_t ndNum = intriParams.ndNum;                         // nd矩阵数据
    uint16_t nValue = intriParams.nValue;                       // nd矩阵行数
    uint16_t dValue = intriParams.dValue;                       // nd矩阵列数，要求32B对齐
    uint16_t srcNdMatrixStride = intriParams.srcNdMatrixStride; // 源操作数相邻nd矩阵起始地址间的偏移，要求32B对齐
    uint16_t srcDValue =
        intriParams.srcDValue; // 源操作数同一nd矩阵的相邻行起始地址间的偏移 (切分场景，行地址不连续)，要求32B对齐
    uint16_t dstNzC0Stride =
        intriParams.dstNzC0Stride; // 目的nz矩阵中，来自源操作数同一行的多行数据相邻行起始地址间的偏移, 单位：block
    uint16_t dstNzNStride = intriParams.dstNzNStride; // 目的nz矩阵中，Z型矩阵相邻行起始地址之间的偏移，单位：block
    uint16_t dstNzMatrixStride = intriParams.dstNzMatrixStride; // 目的nz矩阵中，相邻nz矩阵起始地址间的偏移，要求32B对齐

    ASCENDC_DEBUG_ASSERT(
        ((dValue * sizeof(T)) % BYTE_32_ALIGN == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dValue should be 32B aligned \n"));
    ASCENDC_DEBUG_ASSERT(
        ((srcNdMatrixStride * sizeof(T)) % BYTE_32_ALIGN == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "srcNdMatrixStride should be 32B aligned \n"));
    ASCENDC_DEBUG_ASSERT(
        ((srcDValue * sizeof(T)) % BYTE_32_ALIGN == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dValue should be 32B aligned \n"));
    ASCENDC_DEBUG_ASSERT(
        ((dstNzMatrixStride * sizeof(T)) % BYTE_32_ALIGN == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "dValue should be 32B aligned \n"));

    uint32_t elementsPerBlock = ONE_BLK_SIZE / sizeof(T);
    uint32_t NBlockSize = dValue / elementsPerBlock;
    for (uint32_t ndIdx = 0; ndIdx < ndNum; ++ndIdx) {
        for (int i = 0; i < NBlockSize; ++i) {
            uint32_t offsetDst = ndIdx * dstNzMatrixStride + i * elementsPerBlock * dstNzC0Stride;
            uint32_t offsetSrc = ndIdx * srcNdMatrixStride + i * elementsPerBlock;
            copy_ubuf_to_cbuf(
                dst + offsetDst, src + offsetSrc, 0, nValue, 1, (srcDValue / elementsPerBlock - 1), (dstNzNStride - 1));
        }
    }
}

template <typename T>
__aicore__ inline void DataCopyL12GMImpl(__gm__ T* dst, __cbuf__ T* src, const DataCopyParams& intriParams)
{
    CopyCbufToGmAlign(
        dst, src, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride, intriParams.dstStride);
}

template <typename T>
__aicore__ inline void DataCopyUB2GMNZ2NDImplBase(
    __gm__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t high, uint16_t width, uint16_t srcNStride, uint16_t dstDStride)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "unsupported data copy from ub to gm nz2nd"); });
}

template <typename T>
__aicore__ inline void DataCopyUB2GMNZ2NDImpl(__gm__ T* dst, __ubuf__ T* src, const Nz2NdParamsFull& intriParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "unsupported data copy from ub to gm nz2nd"); });
}

template <typename T>
__aicore__ inline void DataCopyPadL12GMImpl(__gm__ T* dst, __cbuf__ T* src, const DataCopyParams& intriParams)
{
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(dst, workSpace, false, (uint64_t) true, intriParams);
    }

    copy_cbuf_to_gm_align(
        (__gm__ uint8_t*)dst, (__cbuf__ uint8_t*)src, 0, intriParams.blockCount, intriParams.blockLen,
        intriParams.srcStride, intriParams.dstStride);
}

template <typename T>
__aicore__ inline void DataCopyPadL12GMImpl(__gm__ T* dst, __cbuf__ T* src, const DataCopyExtParams& intriParams)
{
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(dst, workSpace, false, (uint64_t) true, intriParams);
    }

    copy_cbuf_to_gm_align(
        (__gm__ uint8_t*)dst, (__cbuf__ uint8_t*)src, 0, intriParams.blockCount, intriParams.blockLen,
        intriParams.srcStride, intriParams.dstStride);
}

template <TPosition pos>
__aicore__ inline uint64_t TransUBAddr(uint64_t addr);

template <typename T>
__aicore__ inline void DataCopyL12GMNZ2NDImplBase(
    __gm__ T* dstAddr, __cbuf__ T* srcAddr, uint16_t high, uint16_t width, uint16_t srcNStride, uint16_t dstDStride)
{
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::A1>(reinterpret_cast<uint64_t>(srcAddr) % ONE_BLK_SIZE == 0)),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "src address should be 32B aligned \n"));
    const uint16_t highBlock = MAX_REPEAT_TIMES;
    const uint16_t highBlocks = high / highBlock;
    const uint16_t highTail = high % highBlock;
    uint16_t widthElems = BLOCK_CUBE; // b16,b32
    if constexpr (sizeof(T) == B8_BYTE_SIZE) {
        widthElems = ONE_BLK_SIZE / sizeof(T); // b8
    }
    const uint16_t widthFractal = (width + widthElems - 1) / widthElems;

    for (int i = 0; i < widthFractal; ++i) {
        uint16_t computeCount = (i + 1) * widthElems;
        uint16_t leftLen = width >= computeCount ? widthElems : (width - i * widthElems);
        uint16_t srcLeftLen = (sizeof(T) == B32_BYTE_SIZE && leftLen <= DEFAULT_BLK_NUM) ? MIN_BLOCK_LEN : 0;
        for (int j = 0; j < highBlocks; ++j) {
            DataCopyPadL12GMImpl(
                dstAddr + i * widthElems + j * highBlock * dstDStride,
                srcAddr + i * srcNStride * BLOCK_CUBE + j * highBlock * BLOCK_CUBE,
                {highBlock, static_cast<uint32_t>(leftLen * sizeof(T)), srcLeftLen,
                 static_cast<uint32_t>((dstDStride - leftLen) * sizeof(T)), 0});
        }
        if (highTail) {
            DataCopyPadL12GMImpl(
                dstAddr + i * widthElems + highBlocks * highBlock * dstDStride,
                srcAddr + i * srcNStride * BLOCK_CUBE + highBlocks * highBlock * BLOCK_CUBE,
                {highTail, static_cast<uint32_t>(leftLen * sizeof(T)), srcLeftLen,
                 static_cast<uint32_t>((dstDStride - leftLen) * sizeof(T)), 0});
        }
    }
}

template <typename T>
__aicore__ inline void DataCopyL12GMNZ2NDImpl(__gm__ T* dst, __cbuf__ T* src, const Nz2NdParamsFull& intriParams)
{
    ASCENDC_DEBUG_ASSERT(
        (TransUBAddr<TPosition::A1>(reinterpret_cast<uint64_t>(src)) % ONE_BLK_SIZE == 0),
        KERNEL_LOG_INTERNAL(KERNEL_ERROR, "src address should be 32B aligned \n"));
    const uint16_t ndNum = intriParams.ndNum;
    const uint16_t nValue = intriParams.nValue;
    const uint16_t dValue = intriParams.dValue;
    const uint16_t srcNdMatrixStride = intriParams.srcNdMatrixStride;
    const uint16_t srcNStride = intriParams.srcNStride;
    const uint16_t dstDStride = intriParams.dstDStride;
    const uint16_t dstNdMatrixStride = intriParams.dstNdMatrixStride;

    for (int i = 0; i < ndNum; ++i) {
        DataCopyL12GMNZ2NDImplBase(
            dst + i * dstNdMatrixStride, src + i * srcNdMatrixStride * BLOCK_CUBE * BLOCK_CUBE, nValue, dValue,
            srcNStride, dstDStride);
    }
}

/* **************************************************************************************************
 * Copy                                             *
 * ************************************************************************************************* */
template <bool isSetMask, bool isMaskBitMode, bool isNormalMode, typename T>
__simd_vf__ inline void VecCopyLevel0VFImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const BasicAPIMaskStruct maskArrayStruct, const uint64_t maskCount,
    const uint8_t repeatTimes, const CopyRepeatParams repeatParams, __ubuf__ uint64_t* maskBuf)
{
    uint32_t count = Internal::VecMicroGetCount<isSetMask, isNormalMode, isMaskBitMode>(
        maskArrayStruct.maskArray, maskCount, maskBuf);
    uint16_t newRepeatTimes = 0;
    newRepeatTimes = Internal::VecMicroGetRepeatTimes<T, isNormalMode>(count, repeatTimes);
    Reg::MaskReg maskReg;
    constexpr uint8_t ElePerBlkT = GetDataBlockSizeInBytes() / sizeof(T);
    if constexpr (isNormalMode) {
        maskReg = Internal::VecMicroGetMaskReg<T, isSetMask, isNormalMode, isMaskBitMode>(maskBuf, count);
        for (uint16_t index = 0; index < newRepeatTimes; ++index) {
            Reg::RegTensor<T> srcVreg;
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                srcVreg, src + index * repeatParams.srcRepeatSize * ElePerBlkT, repeatParams.srcStride, maskReg);
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + index * repeatParams.dstRepeatSize * ElePerBlkT, srcVreg, repeatParams.dstStride, maskReg);
        }
    } else {
        Reg::RegTensor<T> srcReg;
        Reg::MaskReg maskReg;
        uint32_t sreg;
        __ubuf__ T* dstTmp = dst;
        __ubuf__ T* srcTmp = src;
        uint32_t srcRepeatStride = repeatParams.srcStride * DEFAULT_BLK_NUM;
        uint32_t dstRepeatStride = repeatParams.dstStride * DEFAULT_BLK_NUM;
        sreg = static_cast<uint32_t>(count);
        for (uint16_t i = 0; i < static_cast<uint16_t>(newRepeatTimes); ++i) {
            maskReg = Reg::UpdateMask<T>(sreg);
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                srcReg, src, repeatParams.srcStride, srcRepeatStride, maskReg);
            Reg::DataCopy<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                dst, srcReg, repeatParams.dstStride, dstRepeatStride, maskReg);
        }
    }
}

template <bool isSetMask, bool isMaskBitMode, typename T>
__aicore__ inline void VecCopyLevel0Template(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t maskArray[], const uint64_t maskCount, const uint8_t repeatTimes,
    const CopyRepeatParams& repeatParams)
{
    if constexpr (isMaskBitMode) {
        ASCENDC_ASSERT(maskCount == 0, "maskCount must be 0 when isMaskBitMode is true.");
    } else {
        ASCENDC_ASSERT(maskArray == nullptr, "maskArray must be nullptr when isMaskBitMode is false.");
    }
    __ubuf__ uint64_t* maskBuf = nullptr;
    uint16_t maskArraySize = (maskArray == nullptr) ? 0 : MASK_ARRAY_SIZE;
    BasicAPIMaskStruct maskArrayStruct;
    for (uint16_t i = 0; i < maskArraySize; i++) {
        maskArrayStruct.maskArray[i] = maskArray[i];
    }
    if (Internal::IsCounterMode()) {
        if constexpr (!isSetMask) {
            maskBuf = AscendCUtils::GetTemporaryBufferAddr<uint64_t>(TMP_UB_OFFSET, 2); // maskReg 256bit PK-> 128bit
        }
        VecCopyLevel0VFImpl<isSetMask, isMaskBitMode, false, T>(
            dst, src, maskArrayStruct, maskCount, repeatTimes, repeatParams, maskBuf);
        if constexpr (!isSetMask) {
            AscendCUtils::FreeTemporaryBuffer<uint64_t>(maskBuf);
        }
    } else {
        if constexpr (isMaskBitMode && isSetMask) {
            SetVectorMask<T>(maskArray[1], maskArray[0]); // set mask to SPR.MASK, movp in VF
        }
        VecCopyLevel0VFImpl<isSetMask, isMaskBitMode, true, T>(
            dst, src, maskArrayStruct, maskCount, repeatTimes, repeatParams, maskBuf);
    }
}

// Copy::Level 0 - mask bit mode
template <typename T, bool isSetMask = true>
__aicore__ inline void CopyImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask[], const uint8_t repeatTimes,
    const CopyRepeatParams& repeatParams)
{
    static_assert(SupportBytes<T, 2, 4>(), "Copy from ubuf to ubuf only support type b16/b32 on current device");
    VecCopyLevel0Template<isSetMask, true>(dst, src, mask, 0, repeatTimes, repeatParams);
}

template <typename T, bool isSetMask = true>
__aicore__ inline void CopyImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask, const uint8_t repeatTimes,
    const CopyRepeatParams& repeatParams)
{
    static_assert(SupportBytes<T, 2, 4>(), "Copy from ubuf to ubuf only support type b16/b32 on current device");
    VecCopyLevel0Template<isSetMask, false>(dst, src, nullptr, mask, repeatTimes, repeatParams);
}

/* **************************************************************************************************
 * DataCopy Enhanced                                             *
 * ************************************************************************************************* */

template <typename T, typename U>
__aicore__ inline void DataCopyL12L0CImpl(
    __cc__ T* dst, __cbuf__ U* src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "unsupported data copy from cbuf to l0c"); });
}

/* **************************************************************************************************
 * DataCopy                                             *
 * ************************************************************************************************* */

// ------------  ------------
template <typename T, typename U>
__aicore__ inline void DataCopyL0C2UBImpl(
    __ubuf__ T* dst, __cc__ U* src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported data copy from l0c to ubuf"); });
}

template <typename T, typename U>
__aicore__ inline void DataCopyUB2L0CImpl(
    __cc__ T* dst, __ubuf__ U* src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported data copy from ubuf to l0c"); });
}

template <typename T>
__aicore__ inline void DataCopySliceGm2UBImpl(__ubuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams)
{
    uint32_t offsetSrc = 0;
    uint32_t offsetDst = 0;
    for (uint32_t i = 0; i < intriParams.blockCount; i++) {
        DataCopyGM2UBImpl(dst + offsetDst / sizeof(T), src + offsetSrc / sizeof(T), {1, intriParams.blockLen, 0, 0});
        offsetSrc = offsetSrc + (intriParams.blockLen * ONE_BLK_SIZE + intriParams.srcStride);
        offsetDst = offsetDst + (intriParams.blockLen * ONE_BLK_SIZE + intriParams.dstStride);
    }
}

template <typename T>
__aicore__ inline void DataCopySliceUB2GMImpl(__gm__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
    uint32_t offsetSrc = 0;
    uint32_t offsetDst = 0;
    for (uint32_t i = 0; i < intriParams.blockCount; i++) {
        DataCopyUB2GMImpl(dst + offsetDst / sizeof(T), src + offsetSrc / sizeof(T), {1, intriParams.blockLen, 0, 0});
        offsetSrc = offsetSrc + (intriParams.blockLen * ONE_BLK_SIZE + intriParams.srcStride);
        offsetDst = offsetDst + (intriParams.blockLen * ONE_BLK_SIZE + intriParams.dstStride);
    }
}

template <typename T>
__aicore__ inline void DataCopyPadGm2UBImpl(
    __ubuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams, const DataCopyPadParams& padParams)
{
    if (padParams.isPad) {
        set_mov_pad_val(padParams.paddingValue);
    }
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(src, workSpace, true, static_cast<uint64_t>(true), intriParams);
    }

    // DataCopy is in unit of 32 bytes, DataCopyPad is in uint of 1byte / 32bytes
    uint32_t unitOfBytes = 1;
    uint32_t burstLength = intriParams.blockLen * unitOfBytes;
    uint64_t srcGap300 = intriParams.srcStride * unitOfBytes;
    uint32_t dstGap300 = intriParams.dstStride;
    if constexpr (sizeof(T) == 8) {
        copy_gm_to_ubuf_align(
            (__ubuf__ uint32_t*)dst, (__gm__ uint32_t*)src, 0, intriParams.blockCount, burstLength,
            padParams.leftPadding * 2, padParams.rightPadding * 2, srcGap300, dstGap300);
    } else if constexpr (sizeof(T) == 4) {
        copy_gm_to_ubuf_align(
            (__ubuf__ uint32_t*)dst, (__gm__ uint32_t*)src, 0, intriParams.blockCount, burstLength,
            padParams.leftPadding, padParams.rightPadding, srcGap300, dstGap300);
    } else if constexpr (sizeof(T) == 2) {
        copy_gm_to_ubuf_align(
            (__ubuf__ uint16_t*)dst, (__gm__ uint16_t*)src, 0, intriParams.blockCount, burstLength,
            padParams.leftPadding, padParams.rightPadding, srcGap300, dstGap300);
    } else if constexpr (sizeof(T) == 1) {
        copy_gm_to_ubuf_align(
            (__ubuf__ uint8_t*)dst, (__gm__ uint8_t*)src, 0, intriParams.blockCount, burstLength, padParams.leftPadding,
            padParams.rightPadding, srcGap300, dstGap300);
    }
}

template <typename T>
__aicore__ inline void DataCopyPadGm2UBImpl(
    __ubuf__ T* dst, __gm__ T* src, const DataCopyExtParams& intriParams, const DataCopyPadExtParams<T>& padParams)
{
    if (padParams.isPad) {
        set_mov_pad_val(GetScalarBitcodeValue((T)padParams.paddingValue));
    }
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(src, workSpace, true, static_cast<uint64_t>(true), intriParams);
    }
    uint32_t unitOfBytes = 1;
    uint32_t burstLength = intriParams.blockLen * unitOfBytes;
    uint64_t srcGap300 = intriParams.srcStride * unitOfBytes;
    uint32_t dstGap300 = intriParams.dstStride;
    if constexpr (sizeof(T) == 8) {
        copy_gm_to_ubuf_align(
            (__ubuf__ uint32_t*)dst, (__gm__ uint32_t*)src, 0, intriParams.blockCount, burstLength,
            padParams.leftPadding * 2, padParams.rightPadding * 2, srcGap300, dstGap300);
    } else if constexpr (sizeof(T) == 4) {
        copy_gm_to_ubuf_align(
            (__ubuf__ uint32_t*)dst, (__gm__ uint32_t*)src, 0, intriParams.blockCount, burstLength,
            padParams.leftPadding, padParams.rightPadding, srcGap300, dstGap300);
    } else if constexpr (sizeof(T) == 2) {
        copy_gm_to_ubuf_align(
            (__ubuf__ uint16_t*)dst, (__gm__ uint16_t*)src, 0, intriParams.blockCount, burstLength,
            padParams.leftPadding, padParams.rightPadding, srcGap300, dstGap300);
    } else if constexpr (sizeof(T) == 1) {
        copy_gm_to_ubuf_align(
            (__ubuf__ uint8_t*)dst, (__gm__ uint8_t*)src, 0, intriParams.blockCount, burstLength, padParams.leftPadding,
            padParams.rightPadding, srcGap300, dstGap300);
    }
}

template <typename T>
__aicore__ inline void DataCopyPadGm2L1Impl(
    __cbuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams, const DataCopyPadParams& padParams)
{
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(src, workSpace, true, static_cast<uint64_t>(true), intriParams);
    }
    if (padParams.isPad) {
        set_mov_pad_val(padParams.paddingValue);
    }
    CopyGmToCbufAlign<T, true>(
        dst, src, intriParams.blockCount, intriParams.blockLen, padParams.leftPadding, padParams.rightPadding,
        intriParams.srcStride, intriParams.dstStride);
}

template <typename T>
__aicore__ inline void DataCopyPadGm2L1Impl(
    __cbuf__ T* dst, __gm__ T* src, const DataCopyExtParams& intriParams, const DataCopyPadExtParams<T>& padParams)
{
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(src, workSpace, true, (uint64_t) true, intriParams);
    }
    if (padParams.isPad) {
        set_mov_pad_val(GetScalarBitcodeValue((T)padParams.paddingValue));
    }
    CopyGmToCbufAlign<T, true>(
        dst, src, intriParams.blockCount, intriParams.blockLen, padParams.leftPadding, padParams.rightPadding,
        intriParams.srcStride, intriParams.dstStride);
}

template <typename T>
__aicore__ inline void DataCopyPadUB2GMImpl(__gm__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
#if ASCENDC_CPU_DEBUG
    uint64_t absUbAddr = (uint8_t*)src - (uint8_t*)(GetTPipePtr()->GetBaseAddr((int8_t)TPosition::VECIN));
    ASCENDC_ASSERT((absUbAddr % ONE_BLK_SIZE == 0), {
        KERNEL_LOG(KERNEL_ERROR, "absUbAddr is 0x%lx, which must be 32B aligned", absUbAddr);
    });
#endif
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(dst, workSpace, false, (uint64_t) true, intriParams);
    }
    copy_ubuf_to_gm_align(
        (__gm__ uint8_t*)dst, (__ubuf__ uint8_t*)src, 0, intriParams.blockCount, intriParams.blockLen,
        static_cast<uint32_t>(intriParams.srcStride), static_cast<uint32_t>(intriParams.dstStride));
}

template <typename T>
__aicore__ inline void DataCopyPadUB2GMImpl(__gm__ T* dst, __ubuf__ T* src, const DataCopyExtParams& intriParams)
{
#if ASCENDC_CPU_DEBUG
    uint64_t absUbAddr = (uint8_t*)src - (uint8_t*)(GetTPipePtr()->GetBaseAddr((int8_t)TPosition::VECIN));
    ASCENDC_ASSERT((absUbAddr % ONE_BLK_SIZE == 0), {
        KERNEL_LOG(KERNEL_ERROR, "absUbAddr is 0x%lx, which must be 32B aligned", absUbAddr);
    });
#endif
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(dst, workSpace, false, (uint64_t) true, intriParams);
    }
    copy_ubuf_to_gm_align(
        (__gm__ uint8_t*)dst, (__ubuf__ uint8_t*)src, 0, intriParams.blockCount, intriParams.blockLen,
        static_cast<uint32_t>(intriParams.srcStride), static_cast<uint32_t>(intriParams.dstStride));
}

template <typename T>
__aicore__ inline void DataCopyGM2UBND2NZImpl(__ubuf__ T* dst, __gm__ T* src, const Nd2NzParams& intriParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported data copy from gm to ubuf nd2nz"); });
}

template <typename T>
__aicore__ inline void DataCopyPadUB2L1Impl(__cbuf__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported data copy from ubuf to cbuf with pad"); });
}

template <typename T>
__aicore__ inline void DataCopyUB2L1Intf(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const DataCopyParams& intriParams)
{
    DataCopyUB2L1Impl((__cbuf__ T*)dstLocal.GetPhyAddr(), (__ubuf__ T*)srcLocal.GetPhyAddr(), intriParams);
}

template <typename T>
__aicore__ inline void DataCopyUB2L0CIntf(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
    DataCopyUB2L0CImpl(
        (__cc__ T*)dstLocal.GetPhyAddr(), (__ubuf__ T*)srcLocal.GetPhyAddr(), intriParams, enhancedParams);
}

#pragma begin_pipe(V)
template <typename T>
__aicore__ inline void DataCopyUB2UBIntf(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const DataCopyParams& intriParams)
{
    DataCopyUB2UBImpl((__ubuf__ T*)dstLocal.GetPhyAddr(), (__ubuf__ T*)srcLocal.GetPhyAddr(), intriParams);
}
#pragma end_pipe

template <typename T>
__aicore__ inline void DataCopyL12UBIntf(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const DataCopyParams& intriParams)
{
    DataCopyL12UBImpl((__ubuf__ T*)dstLocal.GetPhyAddr(), (__cbuf__ T*)srcLocal.GetPhyAddr(), intriParams);
}

template <typename T>
__aicore__ inline void __in_pipe__(MTE1) __out_pipe__(MTE1) DataCopyL12L0CIntf(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
    DataCopyL12L0CImpl(
        (__cc__ T*)dstLocal.GetPhyAddr(), (__cbuf__ T*)srcLocal.GetPhyAddr(), intriParams, enhancedParams);
}

template <typename T>
__aicore__ inline void DataCopyL0C2UBIntf(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
    DataCopyL0C2UBImpl(
        (__ubuf__ T*)dstLocal.GetPhyAddr(), (__cc__ T*)srcLocal.GetPhyAddr(), intriParams, enhancedParams);
}

template <typename T>
__aicore__ inline __in_pipe__(MTE1) __out_pipe__(MTE1) void DataCopyL12BTIntf(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const DataCopyParams& repeatParams)
{
    DataCopyL12BTImpl((uint64_t)dstLocal.GetPhyAddr(), (__cbuf__ T*)srcLocal.GetPhyAddr(), (uint16_t)0, repeatParams);
}

template <typename T>
__aicore__ inline __in_pipe__(FIX) __out_pipe__(FIX) void DataCopyL12FBIntf(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const DataCopyParams& repeatParams)
{
    DataCopyL12FBImpl((__fbuf__ T*)dstLocal.GetPhyAddr(), (__cbuf__ T*)srcLocal.GetPhyAddr(), repeatParams);
}

template <typename T>
__aicore__ inline __in_pipe__(FIX) __out_pipe__(FIX) void DataCopyL12PTIntf(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const DataCopyParams& repeatParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported data copy from cbuf to pt"); });
}

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_DATA_COPY_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_IMPL_H__
#endif
