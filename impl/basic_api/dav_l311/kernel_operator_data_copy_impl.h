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
 * \brief AscendC l311 support data copy api.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_l311/kernel_operator_data_copy_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_DATA_COPY_IMPL_H
#define ASCENDC_MODULE_OPERATOR_DATA_COPY_IMPL_H
#include "kernel_operator_common_impl.h"
#include "kernel_operator_vec_template_impl.h"
namespace AscendC {
/* **************************************************************************************************
 * DataCopy                                             *
 * ************************************************************************************************* */
// all input params: C220 version, not C310 version, thus need conversion by *32
constexpr uint8_t BYTE_32_ALIGN = 32; // in unit of 32 bytes

// only support VecCore    PIPE_MTE2
// GM -> UB: copy_gm_to_ubuf_align_v2: support changing padding value
template <typename T>
__aicore__ inline void CopyGmToUbufAlignV2(
    __ubuf__ T* dst, __gm__ T* src, const uint16_t blockCount, const uint32_t blockLen, const uint8_t leftPaddingCount,
    const uint8_t rightPaddingCount, const uint32_t srcStride, const uint32_t dstStride, const bool isDataCopyPad,
    const bool isPad = false, const uint8_t sid = 0)
{
    // DataCopy is in unit of 32 bytes, DataCopyPad is in unit of 1byte / 32bytes
    uint32_t unitOfBytes = (isDataCopyPad) ? 1 : BYTE_32_ALIGN;
    uint32_t burstLength = blockLen * unitOfBytes;
    uint32_t padLength = leftPaddingCount * sizeof(T) + rightPaddingCount * sizeof(T);
    uint64_t srcStride310 = srcStride * unitOfBytes + burstLength; // GM  DataCopy:32Bytes, DataCopyPad:1Byte
    uint32_t dstStride310 =
        dstStride * BYTE_32_ALIGN + burstLength + padLength; // UB  DataCopy:32Bytes, DataCopyPad:32Bytes
    // gm stride should be 32B aligned
    dstStride310 = DivCeil(dstStride310, BYTE_32_ALIGN) * BYTE_32_ALIGN;

    if constexpr (sizeof(T) == B64_BYTE_SIZE) {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ uint32_t*)dst, (__gm__ uint32_t*)src, sid, blockCount, burstLength, leftPaddingCount * 2,
            rightPaddingCount * 2, isPad, srcStride310, dstStride310);
    } else if constexpr (sizeof(T) == B32_BYTE_SIZE) {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ uint32_t*)dst, (__gm__ uint32_t*)src, sid, blockCount, burstLength, leftPaddingCount,
            rightPaddingCount, isPad, srcStride310, dstStride310);
    } else if constexpr (sizeof(T) == B16_BYTE_SIZE) {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ uint16_t*)dst, (__gm__ uint16_t*)src, sid, blockCount, burstLength, leftPaddingCount,
            rightPaddingCount, isPad, srcStride310, dstStride310);
    } else if constexpr (sizeof(T) == B8_BYTE_SIZE) {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ uint8_t*)dst, (__gm__ uint8_t*)src, sid, blockCount, burstLength, leftPaddingCount,
            rightPaddingCount, isPad, srcStride310, dstStride310);
    } else {
        ASCENDC_ASSERT(
            false, { KERNEL_LOG(KERNEL_ERROR, "unsupported data type of copy from gm to ubuf on current device"); });
    }
}

// only support VecCore   PIPE_MTE3
// UB -> GM: copy_ubuf_to_gm_align_v2: Note: switch the order between srcStride and dstStride!!!!
template <typename T>
__aicore__ inline void CopyUbufToGmAlignV2(
    __gm__ T* dst, __ubuf__ T* src, const uint16_t blockCount, const uint32_t blockLen, const uint32_t srcStride,
    const uint32_t dstStride, const bool isDataCopyPad)
{
    // DataCopy is in unit of 32 bytes, DataCopyPad is in unit of 1byte / 32bytes
    uint32_t unitOfBytes = (isDataCopyPad) ? 1 : BYTE_32_ALIGN;
    uint32_t burstLength = blockLen * unitOfBytes;
    uint32_t srcStride310 = srcStride * BYTE_32_ALIGN + burstLength; // UB   DataCopy:32Bytes, DataCopyPad:32Bytes
    uint64_t dstStride310 = dstStride * unitOfBytes + burstLength;   // GM   DataCopy:32Bytes, DataCopyPad:1Byte
    // srcstride should be 32B aligned
    srcStride310 = DivCeil(srcStride310, BYTE_32_ALIGN) * BYTE_32_ALIGN;
    copy_ubuf_to_gm_align_v2(dst, src, 0, blockCount, burstLength, dstStride310, srcStride310);
}

// only support CubeCore   PIPE_MTE2
// GM -> L1: copy_gm_to_cbuf_align_v2
template <typename T>
__aicore__ inline void CopyGmToCbufAlignV2(
    __cbuf__ T* dst, __gm__ T* src, const uint32_t blockCount, const uint32_t blockLen, const uint8_t leftPadding,
    const uint8_t rightPadding, const bool isPad, const uint32_t srcStride, const uint32_t dstStride,
    const bool isDataCopyPad, const uint8_t sid = 0)
{
    uint32_t unitOfBytes = (isDataCopyPad) ? 1 : BYTE_32_ALIGN;
    uint32_t burstLength = blockLen * unitOfBytes;
    uint32_t padLength = leftPadding * sizeof(T) + rightPadding * sizeof(T);
    uint64_t actSrcStride = srcStride * unitOfBytes + burstLength;
    uint32_t actDstStride = AlignUp(dstStride * BYTE_32_ALIGN + burstLength + padLength, BYTE_32_ALIGN);

    if constexpr (sizeof(T) == B64_BYTE_SIZE) {
        copy_gm_to_cbuf_align_v2(
            (__cbuf__ uint32_t*)dst, (__gm__ uint32_t*)src, (uint8_t)sid, blockCount, burstLength, leftPadding * 2,
            rightPadding * 2, isPad, actSrcStride, actDstStride);
    } else if constexpr (sizeof(T) == B32_BYTE_SIZE) {
        copy_gm_to_cbuf_align_v2(
            (__cbuf__ uint32_t*)dst, (__gm__ uint32_t*)src, (uint8_t)sid, blockCount, burstLength, leftPadding,
            rightPadding, isPad, actSrcStride, actDstStride);
    } else if constexpr (sizeof(T) == B16_BYTE_SIZE) {
        copy_gm_to_cbuf_align_v2(
            (__cbuf__ uint16_t*)dst, (__gm__ uint16_t*)src, (uint8_t)sid, blockCount, burstLength, leftPadding,
            rightPadding, isPad, actSrcStride, actDstStride);
    } else if constexpr (sizeof(T) == B8_BYTE_SIZE) {
        copy_gm_to_cbuf_align_v2(
            (__cbuf__ uint8_t*)dst, (__gm__ uint8_t*)src, (uint8_t)sid, blockCount, burstLength, leftPadding,
            rightPadding, isPad, actSrcStride, actDstStride);
    } else {
        ASCENDC_ASSERT(
            false, { KERNEL_LOG(KERNEL_ERROR, "unsupported data type of copy from gm to cbuf on current device"); });
    }
}

// only support CubeCore   PIPE_MTE3
// L1 -> GM: copy_cbuf_to_gm_align_v2
template <typename T>
__aicore__ inline void CopyCbufToGmAlignV2(
    __gm__ T* dst, __cbuf__ T* src, const uint16_t blockCount, const uint32_t blockLen, const uint32_t srcStride,
    const uint32_t dstStride, const uint8_t sid = 0)
{
    uint32_t burstLength = blockLen * BYTE_32_ALIGN;
    uint64_t actSrcStride = srcStride * BYTE_32_ALIGN + burstLength;
    uint32_t actDstStride = dstStride * BYTE_32_ALIGN + burstLength;
    copy_cbuf_to_gm_align_v2(
        (__gm__ T*)dst, (__cbuf__ T*)src, sid, blockCount, burstLength, actDstStride, actSrcStride);
}

template <typename T>
__aicore__ inline void DataCopyGM2UBImpl(__ubuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams)
{
    CopyGmToUbufAlignV2(
        dst, src, intriParams.blockCount, intriParams.blockLen, 0, 0, intriParams.srcStride, intriParams.dstStride,
        false);
}

template <typename T>
__aicore__ inline void DataCopyGM2L1Impl(__cbuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams)
{
    CopyGmToCbufAlignV2(
        dst, src, intriParams.blockCount, intriParams.blockLen, 0, 0, false, intriParams.srcStride,
        intriParams.dstStride, false);
}

template <typename T>
__aicore__ inline void DataCopyUB2GMImpl(__gm__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
#ifdef ASCENDC_CPU_DEBUG
    DataCopyWithAtomic(dst, src, intriParams);
#endif // ASCENDC_CPU_DEBUG
    CopyUbufToGmAlignV2(
        dst, src, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, false);
}

template <typename T>
__aicore__ inline void DataCopyUB2UBImpl(__ubuf__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
    // 当前#1018实现有问题，需要手动拼1017的config值
    uint64_t config = ((uint64_t)intriParams.dstStride << 48) + (((uint64_t)intriParams.srcStride) << 32) +
                      (((uint64_t)intriParams.blockLen) << 16) + (((uint64_t)intriParams.blockCount) << 4);
    copy_ubuf_to_ubuf((__ubuf__ void*)dst, (__ubuf__ void*)src, config);
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
    if constexpr (std::is_same<T, float>::value || std::is_same<T, int32_t>::value || std::is_same<T, half>::value) {
        // the burst length, destination gap size must be even.
        uint16_t blockLenAlign = AlignUp(intriParams.blockLen, 2);
        uint16_t dstStrideAlign = AlignUp(intriParams.dstStride, 2);
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
    // ISA: unit of 64B    API: unit of 32B
    uint16_t burstLen = DivCeil(intriParams.blockLen, 2);
    // ISA/API: unit of 32B
    uint16_t srcGapSize = intriParams.srcStride;
    // ISA: unit of 64B    API: unit of 32B
    uint16_t dstGapSize = DivCeil(intriParams.dstStride, 2);

    copy_cbuf_to_fbuf((__fbuf__ void*)dst, (__cbuf__ void*)src, burstNum, burstLen, srcGapSize, dstGapSize);
}

template <typename T>
__aicore__ inline void DataCopyL12PTImpl(const uint64_t dst, __cbuf__ T* src, const DataCopyParams& intriParams)
{
    copy_cbuf_to_pt(
        dst, (__cbuf__ void*)src, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride,
        intriParams.dstStride);
}

template <typename T>
__aicore__ inline void TransND2NZ(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t high, uint16_t width, T scalar)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "unsupported data copy from ub to gm nd2nz on this version"); });
}

template <typename T>
__aicore__ inline void DataCopyGM2L1ND2NZImplB16(__cbuf__ T* dst, __gm__ T* src, const Nd2NzParams& intriParams)
{
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNd2Nz(src, workSpace, true, intriParams);
    }

    uint16_t loop2DstStride = intriParams.dstNzNStride;  // loop2_dst_stride = dst_nz_n_stride
    uint16_t loop3DstStride = intriParams.dstNzC0Stride; // loop3_dst_stride = dst_nz_c0_Stride
    uint16_t loop4DstStride = static_cast<uint16_t>(intriParams.dstNzMatrixStride * sizeof(T) / ONE_BLOCK_SIZE);

    uint64_t mte2NzPara = static_cast<uint64_t>(loop4DstStride) << 48; // MTE2_NZ_PARA[63:48]
    mte2NzPara |= static_cast<uint64_t>(loop3DstStride) << 32;         // MTE2_NZ_PARA[47:32]
    mte2NzPara |= static_cast<uint64_t>(loop2DstStride) << 16;         // MTE2_NZ_PARA[31:16]
    mte2NzPara |= static_cast<uint64_t>(intriParams.ndNum);            // MTE2_NZ_PARA[15:0]
    set_mte2_nz_para(mte2NzPara); // CCE: store parameters for ND2NZ DMA instructions

    uint64_t loop1SrcStride = intriParams.srcDValue * sizeof(T);
    uint64_t loop4SrcStride = intriParams.srcNdMatrixStride * sizeof(T);
    copy_gm_to_cbuf_multi_nd2nz(
        (__cbuf__ half*)dst, (__gm__ half*)src, 0, loop1SrcStride, intriParams.nValue, intriParams.dValue,
        loop4SrcStride, false, false);
}

__aicore__ inline void DataCopyGM2L1ND2NZImpl(__cbuf__ half* dst, __gm__ half* src, const Nd2NzParams& intriParams)
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
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNd2Nz(src, workSpace, true, intriParams);
    }

    uint16_t loop2DstStride = intriParams.dstNzNStride;  // loop2_dst_stride = dst_nz_n_stride
    uint16_t loop3DstStride = intriParams.dstNzC0Stride; // loop3_dst_stride = dst_nz_c0_Stride
    uint16_t loop4DstStride = static_cast<uint16_t>(intriParams.dstNzMatrixStride * sizeof(T) / ONE_BLOCK_SIZE);

    uint64_t mte2NzPara = static_cast<uint64_t>(loop4DstStride) << 48; // MTE2_NZ_PARA[63:48]
    mte2NzPara |= static_cast<uint64_t>(loop3DstStride) << 32;         // MTE2_NZ_PARA[47:32]
    mte2NzPara |= static_cast<uint64_t>(loop2DstStride) << 16;         // MTE2_NZ_PARA[31:16]
    mte2NzPara |= static_cast<uint64_t>(intriParams.ndNum);            // MTE2_NZ_PARA[15:0]
    set_mte2_nz_para(mte2NzPara); // CCE: store parameters for ND2NZ DMA instructions

    uint64_t loop1SrcStride = intriParams.srcDValue * sizeof(T);
    uint64_t loop4SrcStride = intriParams.srcNdMatrixStride * sizeof(T);
    copy_gm_to_cbuf_multi_nd2nz(
        (__cbuf__ float*)dst, (__gm__ float*)src, 0, loop1SrcStride, intriParams.nValue, intriParams.dValue,
        loop4SrcStride, false, false);
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

    uint16_t loop2DstStride = intriParams.dstNzNStride;  // loop2_dst_stride = dst_nz_n_stride
    uint16_t loop3DstStride = intriParams.dstNzC0Stride; // loop3_dst_stride = dst_nz_c0_Stride
    uint16_t loop4DstStride = static_cast<uint16_t>(intriParams.dstNzMatrixStride * sizeof(T) / ONE_BLOCK_SIZE);

    uint64_t mte2NzPara = static_cast<uint64_t>(loop4DstStride) << 48; // MTE2_NZ_PARA[63:48]
    mte2NzPara |= static_cast<uint64_t>(loop3DstStride) << 32;         // MTE2_NZ_PARA[47:32]
    mte2NzPara |= static_cast<uint64_t>(loop2DstStride) << 16;         // MTE2_NZ_PARA[31:16]
    mte2NzPara |= static_cast<uint64_t>(intriParams.ndNum);            // MTE2_NZ_PARA[15:0]
    set_mte2_nz_para(mte2NzPara); // CCE: store parameters for ND2NZ DMA instructions

    uint64_t loop1SrcStride = intriParams.srcDValue * sizeof(T);
    uint64_t loop4SrcStride = intriParams.srcNdMatrixStride * sizeof(T);
    copy_gm_to_cbuf_multi_nd2nz(
        (__cbuf__ int8_t*)dst, (__gm__ int8_t*)src, 0, loop1SrcStride, intriParams.nValue, intriParams.dValue,
        loop4SrcStride, false, false);
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
    uint16_t ndNum = intriParams.ndNum;
    uint16_t nValue = intriParams.nValue;
    uint16_t dValue = intriParams.dValue;
    uint16_t srcNdMatrixStride = intriParams.srcNdMatrixStride;
    uint16_t srcDValue = intriParams.srcDValue;
    uint16_t dstNzC0Stride = intriParams.dstNzC0Stride;
    uint16_t dstNzNStride = intriParams.dstNzNStride;
    uint16_t dstNzMatrixStride = intriParams.dstNzMatrixStride;

    ASCENDC_DEBUG_ASSERT(((dValue * sizeof(T)) % BYTE_32_ALIGN == 0), "dValue should be 32B aligned \n");
    ASCENDC_DEBUG_ASSERT(
        ((srcNdMatrixStride * sizeof(T)) % BYTE_32_ALIGN == 0), "srcNdMatrixStride should be 32B aligned \n");
    ASCENDC_DEBUG_ASSERT(((srcDValue * sizeof(T)) % BYTE_32_ALIGN == 0), "srcDValue should be 32B aligned \n");
    ASCENDC_DEBUG_ASSERT(
        ((dstNzMatrixStride * sizeof(T)) % BYTE_32_ALIGN == 0), "dstNzMatrixStride should be 32B aligned \n");

    uint32_t elementsPerBlock = ONE_BLK_SIZE / sizeof(T);
    uint32_t NBlockSize = dValue / elementsPerBlock;
    for (uint32_t ndIdx = 0; ndIdx < ndNum; ++ndIdx) {
        for (int i = 0; i < NBlockSize; ++i) {
            uint32_t offsetDst = ndIdx * dstNzMatrixStride + i * elementsPerBlock * dstNzC0Stride;
            uint32_t offsetSrc = ndIdx * srcNdMatrixStride + i * elementsPerBlock;
            copy_ubuf_to_cbuf(
                dst + offsetDst, src + offsetSrc, 0, nValue, 1, srcDValue / elementsPerBlock - 1, dstNzNStride - 1);
        }
    }
}

template <typename T>
__aicore__ inline void DataCopyL12GMImpl(__gm__ T* dst, __cbuf__ T* src, const DataCopyParams& intriParams)
{
    CopyCbufToGmAlignV2(
        dst, src, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride, intriParams.dstStride);
}

template <typename T>
__aicore__ inline void DataCopyUB2GMNZ2NDImplBase(
    __gm__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t high, uint16_t width, uint16_t srcNStride, uint16_t dstDStride,
    const uint8_t cacheMode = 0)
{
    (void)cacheMode;
    const uint16_t highBlock = MAX_REPEAT_TIMES;
    const uint16_t highBlocks = high / highBlock;
    const uint16_t highTail = high % highBlock;
    const uint16_t widthFractal = (width + BLOCK_CUBE - 1) / BLOCK_CUBE;

    for (int i = 0; i < widthFractal; ++i) {
        uint16_t computeCount = (i + 1) * BLOCK_CUBE;
        uint16_t leftLen = width >= computeCount ? BLOCK_CUBE : (width - i * BLOCK_CUBE);
        uint16_t srcLeftLen = (sizeof(T) == B32_BYTE_SIZE && leftLen <= DEFAULT_BLK_NUM) ? MIN_BLOCK_LEN : 0;
        for (int j = 0; j < highBlocks; ++j) {
            CopyUbufToGmAlignV2<T>(
                dstAddr + i * BLOCK_CUBE + j * highBlock * dstDStride,
                srcAddr + i * srcNStride * BLOCK_CUBE + j * highBlock * BLOCK_CUBE, highBlock, leftLen * sizeof(T),
                srcLeftLen, (dstDStride - leftLen) * sizeof(T), true);
        }
        if (highTail) {
            CopyUbufToGmAlignV2<T>(
                dstAddr + i * BLOCK_CUBE + highBlocks * highBlock * dstDStride,
                srcAddr + i * srcNStride * BLOCK_CUBE + highBlocks * highBlock * BLOCK_CUBE, highTail,
                leftLen * sizeof(T), srcLeftLen, (dstDStride - leftLen) * sizeof(T), true);
        }
    }
}

template <typename T>
__aicore__ inline void DataCopyUB2GMNZ2NDImpl(
    __gm__ T* dst, __ubuf__ T* src, const Nz2NdParamsFull& intriParams, const uint8_t cacheMode = 0)
{
    const uint16_t ndNum = intriParams.ndNum;
    const uint16_t nValue = intriParams.nValue;
    const uint16_t dValue = intriParams.dValue;
    const uint16_t srcNdMatrixStride = intriParams.srcNdMatrixStride;
    const uint16_t srcNStride = intriParams.srcNStride;
    const uint16_t dstDStride = intriParams.dstDStride;
    const uint16_t dstNdMatrixStride = intriParams.dstNdMatrixStride;

    for (int i = 0; i < ndNum; ++i) {
        DataCopyUB2GMNZ2NDImplBase(
            dst + i * dstNdMatrixStride, src + i * srcNdMatrixStride * BLOCK_CUBE * BLOCK_CUBE, nValue, dValue,
            srcNStride, dstDStride, cacheMode);
    }
}

template <typename T>
__aicore__ inline void DataCopyL12GMNZ2NDImpl(__gm__ T* dst, __cbuf__ T* src, const Nz2NdParamsFull& intriParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "unsupported data copy from cbuf to gm with nz2nd"); });
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
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "unsupported data copy from cbuf to l0c on this version"); });
}

/* **************************************************************************************************
 * DataCopy                                             *
 * ************************************************************************************************* */

// ------------  ------------
template <typename T, typename U>
__aicore__ inline void DataCopyL0C2UBImpl(
    __ubuf__ T* dst, __cc__ U* src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported data copy from l0c to ubuf on this version"); });
}

template <typename T, typename U>
__aicore__ inline void DataCopyUB2L0CImpl(
    __cc__ T* dst, __ubuf__ U* src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported data copy from ubuf to l0c on this version"); });
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
__aicore__ inline void DataCopyPadGm2UBInternal(
    __ubuf__ T* dst, __gm__ T* src, const uint16_t blockCount, const uint32_t blockLen, const uint32_t srcStride,
    const uint32_t dstStride, const uint8_t leftPadding, const uint8_t rightPadding, const bool isPad)
{
    CopyGmToUbufAlignV2(dst, src, blockCount, blockLen, leftPadding, rightPadding, srcStride, dstStride, true, isPad);
}

__aicore__ inline void CheckSrcGmDataCopyExtParamsRange(const DataCopyExtParams& intriParams)
{
    ASCENDC_ASSERT((intriParams.srcStride <= static_cast<int64_t>(1ul << 40 - 1)), {
        KERNEL_LOG(KERNEL_ERROR, "srcStride is %ld, which should be no more than 2^40-1", intriParams.srcStride);
    });
    ASCENDC_ASSERT((intriParams.srcStride >= (-static_cast<int64_t>(intriParams.blockLen))), {
        KERNEL_LOG(
            KERNEL_ERROR, "srcStride is %ld, which should be no less than %d", intriParams.srcStride,
            -intriParams.blockLen);
    });
    ASCENDC_ASSERT((intriParams.dstStride <= static_cast<int64_t>(65535)), {
        KERNEL_LOG(KERNEL_ERROR, "dstStride is %ld, which should be no more than 65535", intriParams.dstStride);
    });
    ASCENDC_ASSERT((intriParams.dstStride >= static_cast<int64_t>(0)), {
        KERNEL_LOG(KERNEL_ERROR, "dstStride is %ld, which should be no less than 0", intriParams.dstStride);
    });
}

template <typename T>
__aicore__ inline void DataCopyPadGm2UBImpl(
    __ubuf__ T* dst, __gm__ T* src, const DataCopyExtParams& intriParams, const DataCopyPadExtParams<T>& padParams)
{
    if ASCEND_IS_AIC {
        return;
    }
    if (padParams.isPad == true) {
        set_pad_val_outtoub(GetScalarBitcodeValue(padParams.paddingValue));
    }
    if constexpr (sizeof(T) > B32_BYTE_SIZE) {
        ASCENDC_ASSERT((padParams.paddingValue == 0), {
            KERNEL_LOG(KERNEL_ERROR, "b64 paddingValue on current device only support 0");
        });
    }
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(src, workSpace, true, true, intriParams);
    }

    CheckSrcGmDataCopyExtParamsRange(intriParams);
    CopyGmToUbufAlignV2<T>(
        dst, src, intriParams.blockCount, intriParams.blockLen, padParams.leftPadding, padParams.rightPadding,
        static_cast<uint32_t>(intriParams.srcStride), static_cast<uint32_t>(intriParams.dstStride), true,
        padParams.isPad, 0);
}

template <typename T>
__aicore__ inline void DataCopyPadGm2UBImpl(
    __ubuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams, const DataCopyPadParams& padParams)
{
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(src, workSpace, true, (uint64_t) true, intriParams);
    }
    if (padParams.isPad) {
        set_pad_val_outtoub(padParams.paddingValue);
    }
    DataCopyPadGm2UBInternal(
        dst, src, intriParams.blockCount, (uint32_t)intriParams.blockLen, (uint32_t)intriParams.srcStride,
        (uint32_t)intriParams.dstStride, padParams.leftPadding, padParams.rightPadding, padParams.isPad);
}

template <typename T>
__aicore__ inline void DataCopyPadGm2L1Impl(
    __cbuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams, const DataCopyPadParams& padParams)
{
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(src, workSpace, true, (uint64_t) true, intriParams);
    }
    if (padParams.isPad) {
        set_pad_val_outtol1(padParams.paddingValue);
    }
    CopyGmToCbufAlignV2(
        dst, src, intriParams.blockCount, intriParams.blockLen, padParams.leftPadding, padParams.rightPadding,
        padParams.isPad, intriParams.srcStride, intriParams.dstStride, true);
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
        set_pad_val_outtol1(GetScalarBitcodeValue(padParams.paddingValue));
    }
    CopyGmToCbufAlignV2(
        dst, src, intriParams.blockCount, intriParams.blockLen, padParams.leftPadding, padParams.rightPadding,
        padParams.isPad, intriParams.srcStride, intriParams.dstStride, true);
}

template <typename T>
__aicore__ inline void DataCopyPadUB2GMInternal(
    __gm__ T* dst, __ubuf__ T* src, const uint16_t blockCount, const uint32_t blockLen, const uint32_t dstStride,
    const uint32_t srcStride)
{
    CopyUbufToGmAlignV2(dst, src, blockCount, blockLen, srcStride, dstStride, true);
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
    DataCopyPadUB2GMInternal(
        dst, src, intriParams.blockCount, (uint32_t)intriParams.blockLen, (uint32_t)intriParams.dstStride,
        (uint32_t)intriParams.srcStride);
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
    DataCopyPadUB2GMInternal(
        dst, src, intriParams.blockCount, intriParams.blockLen, intriParams.dstStride, intriParams.srcStride);
}

template <typename T>
__aicore__ inline void DataCopyPadL12GMImpl(__gm__ T* dst, __cbuf__ T* src, const DataCopyParams& intriParams)
{
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(dst, workSpace, false, (uint64_t) true, intriParams);
    }

    uint64_t srcStride = AlignUp(intriParams.srcStride * BYTE_32_ALIGN + intriParams.blockLen, BYTE_32_ALIGN);
    uint32_t dstStride = intriParams.blockLen + intriParams.dstStride;

    copy_cbuf_to_gm_align_v2(dst, src, 0, intriParams.blockCount, intriParams.blockLen, dstStride, srcStride);
}

template <typename T>
__aicore__ inline void DataCopyPadL12GMImpl(__gm__ T* dst, __cbuf__ T* src, const DataCopyExtParams& intriParams)
{
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(dst, workSpace, false, (uint64_t) true, intriParams);
    }

    uint64_t srcStride = AlignUp(intriParams.srcStride * BYTE_32_ALIGN + intriParams.blockLen, BYTE_32_ALIGN);
    uint32_t dstStride = intriParams.blockLen + intriParams.dstStride;

    copy_cbuf_to_gm_align_v2(dst, src, 0, intriParams.blockCount, intriParams.blockLen, dstStride, srcStride);
}

template <typename T>
__aicore__ inline void DataCopyGM2UBSingleImpl(
    __ubuf__ T* dst, __gm__ T* src, const Nd2NzParams& intriParams, const int copyTime, const int computeNum,
    const uint8_t cacheMode = 0)
{
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
    DataCopyExtParams copyParams = {
        MAX_REPEAT_TIMES, static_cast<uint32_t>(computeLen),
        static_cast<uint32_t>(intriParams.srcDValue * sizeof(T) - computeLen),
        static_cast<uint32_t>(
            (static_cast<uint16_t>(dstNzNStride * static_cast<uint16_t>(DEFAULT_C0_SIZE)) -
             static_cast<uint16_t>(DEFAULT_C0_SIZE)) /
            static_cast<uint16_t>(DEFAULT_C0_SIZE)),
        0};
    DataCopyPadExtParams<T> padParams;
    for (int repeatTime = 0; repeatTime < repeatCount; ++repeatTime) {
        DataCopyPadGm2UBImpl(
            (__ubuf__ T*)(dst + dstCopyStartOffset + repeatTime * maxC0Count),
            (__gm__ T*)(src + srcCopyStartOffset + repeatTime * maxdValue), copyParams, padParams);
    }
    copyParams.blockCount = repeatTail;
    if (repeatTail != 0) {
        int dstOffset = (dstCopyStartOffset + repeatCount * MAX_REPEAT_TIMES * c0Count);
        int srcOffset = (srcCopyStartOffset + repeatCount * MAX_REPEAT_TIMES * dValue);
        DataCopyPadGm2UBImpl((__ubuf__ T*)(dst + dstOffset), (__gm__ T*)(src + srcOffset), copyParams, padParams);
    }
}

template <typename T>
__aicore__ inline void DataCopyGM2UBND2NZImpl(
    __ubuf__ T* dst, __gm__ T* src, const Nd2NzParams& intriParams, const uint8_t cacheMode = 0)
{
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNd2Nz(src, workSpace, true, intriParams);
    }
    const uint16_t& ndNum = intriParams.ndNum;
    const uint16_t& dValue = intriParams.dValue;
    const uint16_t& srcNdMatrixStride = intriParams.srcNdMatrixStride;
    const uint16_t& dstNzMatrixStride = static_cast<uint16_t>(intriParams.dstNzMatrixStride);
    const uint16_t& c0Count = DEFAULT_C0_SIZE / sizeof(T);
    for (int index = 0; index < ndNum; ++index) {
        int16_t copyNum = (dValue + c0Count - 1) / c0Count;
        for (int copyTime = 0; copyTime < copyNum; ++copyTime) {
            int computeCount = (dValue >= (copyTime + 1) * c0Count) ? c0Count : (dValue % c0Count);
            DataCopyGM2UBSingleImpl(
                dst + dstNzMatrixStride, src + srcNdMatrixStride, intriParams, copyTime, computeCount, cacheMode);
        }
    }
}

__aicore__ inline bool IsSupportQuantMode(QuantMode_t quantPre)
{
    return (
        quantPre == QuantMode_t::NoQuant || quantPre == QuantMode_t::F322F16 || quantPre == QuantMode_t::F322BF16 ||
        quantPre == QuantMode_t::DEQF16 || quantPre == QuantMode_t::VDEQF16 || quantPre == QuantMode_t::QF322B8_PRE ||
        quantPre == QuantMode_t::VQF322B8_PRE || quantPre == QuantMode_t::REQ8 || quantPre == QuantMode_t::VREQ8);
}

template <typename T, typename U>
__aicore__ inline void DataCopyL0C2GMImpl(
    __gm__ T* dst, __cc__ U* src, const DataCopyCO12DstParams& intriParams, uint8_t cacheMode)
{
    if ASCEND_IS_AIC {
        static_assert(
            (SupportType<
                Tuple<U, T>, Tuple<float, float>, Tuple<float, half>, Tuple<float, bfloat16_t>, Tuple<float, int8_t>,
                Tuple<float, uint8_t>, Tuple<int32_t, int32_t>, Tuple<int32_t, int16_t>, Tuple<int32_t, int8_t>,
                Tuple<int32_t, uint8_t>, Tuple<int32_t, half>>()),
            "Failed to check dtype in "
            "DataCopy from CO1 to GM, current api support dtype combination is "
            "src: float, dst: half / bfloat16_t / float / int8_t / uint8_t; "
            "src: int32_t, dst: half / int32_t / int16_t / int8_t / uint8_t.");
        if (IsSupportQuantMode(intriParams.quantPre)) {
            uint32_t dstStride = intriParams.dstStride;
            if (!intriParams.nz2ndEn) {
                dstStride = dstStride * ONE_BLK_SIZE / sizeof(T);
            }
            return copy_matrix_cc_to_gm(
                dst, src, intriParams.sid, intriParams.nSize, intriParams.mSize, dstStride, intriParams.srcStride,
                cacheMode, 0, intriParams.unitFlag, static_cast<uint64_t>(intriParams.quantPre), intriParams.reluPre,
                intriParams.channelSplit, intriParams.nz2ndEn, 0, 0, false, false, 0, false, false, false, false, false,
                false);
        } else {
            ASCENDC_ASSERT(false, {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "Failed to check quantPre value in DataCopy from CO1 "
                    "to GM, supported values are NoQuant / F322F16 / F322BF16 / DEQF16 / VDEQF16 / QF322B8_PRE / "
                    "VQF322B8_PRE / REQ8 / VREQ8.");
            });
        }
    }
}

template <typename T, typename U>
__aicore__ inline void DataCopyL0C2L1Impl(__cbuf__ T* dst, __cc__ U* src, const DataCopyCO12DstParams& intriParams)
{
    if ASCEND_IS_AIC {
        static_assert(
            (SupportType<
                Tuple<U, T>, Tuple<float, float>, Tuple<float, half>, Tuple<float, bfloat16_t>, Tuple<float, int8_t>,
                Tuple<float, uint8_t>, Tuple<int32_t, int32_t>, Tuple<int32_t, int16_t>, Tuple<int32_t, int8_t>,
                Tuple<int32_t, uint8_t>, Tuple<int32_t, half>>()),
            "Failed to check dtype in "
            "DataCopy from CO1 to A1 / B1, current api support dtype combination is "
            "src: float, dst: half / bfloat16_t / float / int8_t / uint8_t; "
            "src: int32_t, dst: half / int32_t / int16_t / int8_t / uint8_t.");
        if (IsSupportQuantMode(intriParams.quantPre)) {
            uint32_t dstStride = intriParams.dstStride;
            if (!intriParams.nz2ndEn) {
                dstStride = dstStride * ONE_BLK_SIZE / sizeof(T);
            }
            return copy_matrix_cc_to_cbuf(
                dst, src, intriParams.sid, intriParams.nSize, intriParams.mSize, dstStride, intriParams.srcStride, 0, 0,
                intriParams.unitFlag, static_cast<uint64_t>(intriParams.quantPre), intriParams.reluPre,
                intriParams.channelSplit, intriParams.nz2ndEn, 0, 0, false, false, 0, false, false, false, false, false,
                false);
        } else {
            ASCENDC_ASSERT(false, {
                KERNEL_LOG(
                    KERNEL_ERROR,
                    "Failed to check quantPre value in DataCopy from CO1 "
                    "to A1 / B1, supported values are NoQuant / F322F16 / F322BF16 / DEQF16 / VDEQF16 / QF322B8_PRE / "
                    "VQF322B8_PRE / REQ8 / VREQ8.");
            });
        }
    }
}

template <typename T>
__aicore__ inline void DataCopyPadUB2L1Impl(__cbuf__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
    ASCENDC_ASSERT(
        false, { KERNEL_LOG(KERNEL_ERROR, "unsupported data copy from ubuf to cbuf with pad on this version"); });
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
    DataCopyL12PTImpl((uint64_t)dstLocal.GetPhyAddr(), (__cbuf__ T*)srcLocal.GetPhyAddr(), repeatParams);
}

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_DATA_COPY_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_IMPL_H__
#endif
