/*
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
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
    "impl/basic_api/dav_5161/kernel_operator_data_copy_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_DATA_COPY_IMPL_H
#define ASCENDC_MODULE_OPERATOR_DATA_COPY_IMPL_H
#include "kernel_operator_common_impl.h"
#include "kernel_utils.h"

namespace AscendC {
// all input params: need conversion by *32
constexpr uint8_t BYTE_32_ALIGN = 32; // in unit of 32 bytes
constexpr uint8_t FIX_VAL = 16;

/* **************************************************************************************************
 * DataCopy                                         *
 * ************************************************************************************************* */

// only support VecCore    PIPE_MTE2
// GM -> UB: copy_gm_to_ubuf_align_v2: support changing padding value
// Note: dst stride in normal mode is in unit of 32Bytes, in compact mode is in unit of is 1Byte
template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline void CopyGmToUbufAlignV2(
    __ubuf__ T* dst, __gm__ T* src, const uint16_t blockCount, const uint32_t blockLen, const uint8_t leftPaddingCount,
    const uint8_t rightPaddingCount, const int64_t srcStride, const int64_t dstStride, const bool isDataCopyPad,
    const bool isPad = true, const uint8_t sid = 0)
{
    // DataCopy is in unit of 32 bytes, DataCopyPad is in unit of 1byte
    uint32_t unitOfBytes = (isDataCopyPad) ? 1 : BYTE_32_ALIGN;
    uint32_t burstLength = blockLen * unitOfBytes;
    uint32_t padLength = leftPaddingCount * sizeof(T) + rightPaddingCount * sizeof(T);
    uint64_t actSrcStride = srcStride * unitOfBytes + burstLength; // GM  DataCopy:32Bytes, DataCopyPad:1Byte
    uint32_t actDstStride = 0;
    uint8_t leftPaddingCnt = leftPaddingCount;
    uint8_t rigntPaddingCnt = rightPaddingCount;
    if constexpr (mode == PaddingMode::Normal) {
        actDstStride =
            AlignUp(static_cast<uint32_t>(dstStride) * BYTE_32_ALIGN + burstLength + padLength, BYTE_32_ALIGN);
    } else {
        actDstStride = burstLength;
        rigntPaddingCnt = 0;
        leftPaddingCnt = 0;
    }
    if constexpr (sizeof(T) == B64_BYTE_SIZE) { // B64
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ uint32_t*)dst, (__gm__ uint32_t*)src, sid, blockCount, burstLength, leftPaddingCnt * 2,
            rigntPaddingCnt * 2, isPad, actSrcStride, actDstStride);
    } else if constexpr (sizeof(T) == B32_BYTE_SIZE) {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ uint32_t*)dst, (__gm__ uint32_t*)src, sid, blockCount, burstLength, leftPaddingCnt,
            rigntPaddingCnt, isPad, actSrcStride, actDstStride);
    } else if constexpr (sizeof(T) == B16_BYTE_SIZE) {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ uint16_t*)dst, (__gm__ uint16_t*)src, sid, blockCount, burstLength, leftPaddingCnt,
            rigntPaddingCnt, isPad, actSrcStride, actDstStride);
    } else if constexpr (sizeof(T) == B8_BYTE_SIZE) {
        copy_gm_to_ubuf_align_v2(
            (__ubuf__ uint8_t*)dst, (__gm__ uint8_t*)src, sid, blockCount, burstLength, leftPaddingCnt, rigntPaddingCnt,
            isPad, actSrcStride, actDstStride);
    } else {
        ASSERT(false && "unsupported data type of copy from gm to ubuf on current device");
    }
}

// only support VecCore   PIPE_MTE3
// UB -> GM: copy_ubuf_to_gm_align_v2
template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline void CopyUbufToGmAlignV2(
    __gm__ T* dst, __ubuf__ T* src, const uint16_t blockCount, const uint32_t blockLen, const int64_t srcStride,
    const int64_t dstStride, const bool isDataCopyPad, const uint8_t sid = 0)
{
    // DataCopy is in unit of 32 bytes, DataCopyPad is in unit of 1byte / 32bytes
    uint32_t unitOfBytes = (isDataCopyPad) ? 1 : BYTE_32_ALIGN;
    uint32_t burstLength = blockLen * unitOfBytes;
    uint32_t actSrcStride = 0;
    if constexpr (mode == PaddingMode::Normal) {
        // UB   DataCopy:32Bytes, DataCopyPad:32Bytes
        actSrcStride = static_cast<uint32_t>(srcStride) * BYTE_32_ALIGN + burstLength;
        // srcstride should be 32B aligned
        actSrcStride = DivCeil(actSrcStride, BYTE_32_ALIGN) * BYTE_32_ALIGN;
    } else {
        actSrcStride = burstLength;
    }
    // GM   DataCopy:32Bytes, DataCopyPad:1Byte
    uint64_t actDstStride = dstStride * unitOfBytes + burstLength;
    copy_ubuf_to_gm_align_v2(dst, src, sid, blockCount, burstLength, actDstStride, actSrcStride);
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
        ASSERT(false && "unsupported data type of copy from gm to cbuf on current device");
    }
}

// only support CubeCore   PIPE_MTE3
// L1 -> GM: copy_cbuf_to_gm_align_v2
template <typename T>
__aicore__ inline void CopyCbufToGmAlignV2(
    __gm__ T* dst, __cbuf__ T* src, const uint32_t blockCount, const uint32_t blockLen, const uint32_t srcStride,
    const uint32_t dstStride, const uint8_t sid = 0)
{
    uint32_t burstLength = blockLen * BYTE_32_ALIGN;
    uint32_t actSrcStride = srcStride * BYTE_32_ALIGN + burstLength;
    uint64_t actDstStride = dstStride * BYTE_32_ALIGN + burstLength;
    copy_cbuf_to_gm_align_v2(
        (__gm__ T*)dst, (__cbuf__ T*)src, sid, blockCount, burstLength, actDstStride, actSrcStride);
}

template <typename T>
__aicore__ inline void DataCopyGM2UBImpl(__ubuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams)
{
    CopyGmToUbufAlignV2(
        dst, src, intriParams.blockCount, intriParams.blockLen, 0, 0, intriParams.srcStride, intriParams.dstStride,
        false, true, intriParams.sid);
}

template <typename T>
__aicore__ inline void DataCopyGM2L1Impl(__cbuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams)
{
    CopyGmToCbufAlignV2(
        dst, src, intriParams.blockCount, intriParams.blockLen, 0, 0, false, intriParams.srcStride,
        intriParams.dstStride, false, intriParams.sid);
}

template <typename T>
__aicore__ inline void DataCopyUB2GMImpl(__gm__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
#ifdef ASCENDC_CPU_DEBUG
    DataCopyWithAtomic(dst, src, intriParams);
#endif // ASCENDC_CPU_DEBUG
    CopyUbufToGmAlignV2(
        dst, src, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, false,
        intriParams.sid);
}

template <typename T>
__aicore__ inline void DataCopyUB2UBImpl(__ubuf__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
    // 当前#1018实现有问题，需要手动拼1017的config值
    uint64_t config = ((uint64_t)intriParams.dstStride << 48) + (((uint64_t)intriParams.srcStride) << 32) +
                      (((uint64_t)intriParams.blockLen) << 16) + ((uint64_t)intriParams.blockCount);
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
    constexpr uint32_t POS_LOOP3_SIZE = 40;
    constexpr uint32_t POS_MODE = 61;
    constexpr uint32_t POS_LOOP4_SRC_STRIDE = 48;

    uint64_t xm = 0;
    xm |= ((uint64_t)intriParams.blockLen * 32 / sizeof(T)) << POS_LOOP3_SIZE;
    xm |= (uint64_t)fixp_trans_mode_t::NORMAL_DMA << POS_MODE;

    uint64_t xt = ((uint64_t)intriParams.blockLen + (uint64_t)intriParams.srcStride);

    uint64_t config = 0;
    config |= (uint64_t)intriParams.blockCount;
    config |= ((uint64_t)intriParams.blockLen + (uint64_t)intriParams.dstStride) << POS_LOOP4_SRC_STRIDE;

    set_fixp_nz_para(config);
    fix_cbuf_to_ubuf((__ubuf__ int8_t*)dst, (__cbuf__ int8_t*)src, xm, xt);
}

template <typename T>
__aicore__ inline void DataCopyL12GMImpl(__gm__ T* dst, __cbuf__ T* src, const DataCopyParams& intriParams)
{
    CopyCbufToGmAlignV2(
        dst, src, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride, intriParams.dstStride,
        intriParams.sid);
}

template <typename T>
__aicore__ inline void DataCopyL12BTImpl(
    const uint64_t dst, __cbuf__ T* src, const uint16_t isenableConv, const DataCopyParams& intriParams)
{
    uint8_t fixVal = 0;
    if constexpr (std::is_same<T, half>::value) {
        fixVal = intriParams.fixShiftVal == 0 ? FIX_VAL : intriParams.fixShiftVal;
    }
    if constexpr (std::is_same<T, float>::value || std::is_same<T, int32_t>::value || std::is_same<T, half>::value) {
        // the destination gap size must be even.
        uint16_t dstStrideAlign = AlignUp(intriParams.dstStride, 2);
        copy_cbuf_to_bt(
            dst, src, isenableConv, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride, dstStrideAlign,
            fixVal);
    } else {
        ASSERT(false && "unsupported data type of copy from cbuf to bt on current device");
    }
}

template <typename T>
__aicore__ inline void DataCopyL12FBImpl(uint64_t dst, __cbuf__ T* src, const DataCopyParams& intriParams)
{
    constexpr uint8_t POS_DST_MEM_BLOCK = 16;
    dst |= static_cast<uint64_t>(intriParams.postProcBufBlock) << POS_DST_MEM_BLOCK;
    copy_cbuf_to_fbuf_v2(
        (__fbuf__ void*)dst, (__cbuf__ void*)src, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride,
        intriParams.dstStride);
}

template <typename T>
__aicore__ inline void DataCopyL12PTImpl(const uint64_t dst, __cbuf__ T* src, const DataCopyParams& intriParams)
{
    copy_cbuf_to_pt(
        dst, (__cbuf__ void*)src, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride,
        intriParams.dstStride);
}

/* **************************************************************************************************
 * ND2NZ                                         *
 * ************************************************************************************************* */
// Small C0 Mode: only when D value <= 4 can enable
__aicore__ inline void SmallC0Checker(bool enableSmallC0, uint16_t dValue)
{
    if (enableSmallC0) {
        ASCENDC_ASSERT((dValue <= 4), {
            KERNEL_LOG(KERNEL_ERROR, "Small C0 can be enabled only when D value is less or equal to 4.");
        });
    }
}

template <typename T, bool enableSmallC0 = false>
__aicore__ inline void DataCopyGM2L1ND2NZImplBase(
    __cbuf__ T* dst, __gm__ T* src, const Nd2NzParams& intriParams, const uint8_t cacheMode = 0)
{
    if ASCEND_IS_AIC {
        SmallC0Checker(enableSmallC0, intriParams.dValue);

        if constexpr (g_gm_overflow_check) {
            __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
            AscendCUtils::CheckGmMemOverflowNd2Nz(src, workSpace, true, intriParams);
        }

        uint16_t loop2DstStride = intriParams.dstNzNStride;  // loop2DstStride = dst_nz_n_stride
        uint16_t loop3DstStride = intriParams.dstNzC0Stride; // loop3_dst_stride = dst_nz_c0_Stride
        // loop4DstStride: dst_nz_matrix_stride * size_of_dst_type / C0_size
        uint16_t loop4DstStride = static_cast<uint16_t>(intriParams.dstNzMatrixStride * sizeof(T) / ONE_BLK_SIZE);

        uint64_t mte2NzPara = static_cast<uint64_t>(loop4DstStride) << 48; // MTE2_NZ_PARA[63:48]
        mte2NzPara |= static_cast<uint64_t>(loop3DstStride) << 32;         // MTE2_NZ_PARA[47:32]
        mte2NzPara |= static_cast<uint64_t>(loop2DstStride) << 16;         // MTE2_NZ_PARA[31:16]
        mte2NzPara |= static_cast<uint64_t>(intriParams.ndNum);            // MTE2_NZ_PARA[15:0]
        set_mte2_nz_para(mte2NzPara); // CCE: store parameters for ND2NZ DMA instructions

        // input params: srcDValue, srcNdMatrixStride                        unit of element nums
        // expected params for ISA: loop1_src_stride, loop4SrcStride       uint of bytes
        // loop1SrcStride = srcD * sizeof(srcType)     loop4SrcStride = srcNdMatrixStride * sizeof(srcType)
        uint64_t loop1SrcStride = intriParams.srcDValue * sizeof(T);
        uint64_t loop4SrcStride = intriParams.srcNdMatrixStride * sizeof(T);
        if constexpr (sizeof(T) == B8_BYTE_SIZE) {
            copy_gm_to_cbuf_multi_nd2nz(
                (__cbuf__ int8_t*)dst, (__gm__ int8_t*)src, intriParams.sid, loop1SrcStride, cacheMode,
                intriParams.nValue, intriParams.dValue, loop4SrcStride, enableSmallC0);
        }
        if constexpr (sizeof(T) == B16_BYTE_SIZE) {
            copy_gm_to_cbuf_multi_nd2nz(
                (__cbuf__ half*)dst, (__gm__ half*)src, intriParams.sid, loop1SrcStride, cacheMode, intriParams.nValue,
                intriParams.dValue, loop4SrcStride, enableSmallC0);
        }
        if constexpr (sizeof(T) == B32_BYTE_SIZE) {
            copy_gm_to_cbuf_multi_nd2nz(
                (__cbuf__ float*)dst, (__gm__ float*)src, intriParams.sid, loop1SrcStride, cacheMode,
                intriParams.nValue, intriParams.dValue, loop4SrcStride, enableSmallC0);
        }
    }
}

template <typename T, bool enableSmallC0 = false>
__aicore__ inline void DataCopyGM2L1ND2NZImpl(
    __cbuf__ T* dst, __gm__ T* src, const Nd2NzParams& intriParams, const uint8_t cacheMode = 0)
{
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNd2Nz(src, workSpace, true, intriParams);
    }
    DataCopyGM2L1ND2NZImplBase<T, enableSmallC0>((__cbuf__ T*)dst, (__gm__ T*)src, intriParams, cacheMode);
}

template <typename T>
__aicore__ inline void DataCopyGM2UBND2NZImpl(__ubuf__ T* dst, __gm__ T* src, const Nd2NzParams& intriParams)
{
    ASSERT(false && "unsupported data copy GM to UB ND2NZ on current device");
}

/* **************************************************************************************************
 * DN2NZ                                         *
 * ************************************************************************************************* */

template <typename T>
__aicore__ inline void DataCopyGM2L1DN2NZImplBase(
    __cbuf__ T* dst, __gm__ T* src, const Dn2NzParams& intriParams, bool enableSmallC0, const uint8_t cacheMode = 0)
{
    SmallC0Checker(enableSmallC0, intriParams.dValue);

    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowDn2Nz(src, workSpace, true, intriParams);
    }

    uint16_t loop2DstStride = intriParams.dstNzNStride;  // loop2DstStride = dst_nz_n_stride
    uint16_t loop3DstStride = intriParams.dstNzC0Stride; // loop3_dst_stride = dst_nz_c0_Stride
    // loop4DstStride: dst_nz_matrix_stride * size_of_dst_type / C0_size
    uint16_t loop4DstStride = static_cast<uint16_t>(intriParams.dstNzMatrixStride * sizeof(T) / ONE_BLK_SIZE);

    uint64_t mte2NzPara = static_cast<uint64_t>(loop4DstStride) << 48; // MTE2_NZ_PARA[63:48]
    mte2NzPara |= static_cast<uint64_t>(loop3DstStride) << 32;         // MTE2_NZ_PARA[47:32]
    mte2NzPara |= static_cast<uint64_t>(loop2DstStride) << 16;         // MTE2_NZ_PARA[31:16]
    mte2NzPara |= static_cast<uint64_t>(intriParams.dnNum);            // MTE2_NZ_PARA[15:0]
    set_mte2_nz_para(mte2NzPara); // CCE: store parameters for DN2NZ DMA instructions

    // input params: srcDValue, srcDnMatrixStride                        unit of element nums
    // expected params for ISA: loop1_src_stride, loop4SrcStride       uint of bytes
    // loop1SrcStride = srcD * sizeof(srcType)     loop4SrcStride = srcDnMatrixStride * sizeof(srcType)
    uint64_t loop1SrcStride = intriParams.srcDValue * sizeof(T);
    uint64_t loop4SrcStride = intriParams.srcDnMatrixStride * sizeof(T);

    copy_gm_to_cbuf_multi_dn2nz(
        (__cbuf__ T*)dst, (__gm__ T*)src, intriParams.sid, loop1SrcStride, cacheMode, intriParams.nValue,
        intriParams.dValue, loop4SrcStride, enableSmallC0);
}

template <typename T>
__aicore__ inline void DataCopyGM2L1DN2NZImpl(
    __cbuf__ T* dst, __gm__ T* src, const Dn2NzParams& intriParams, bool enableSmallC0, const uint8_t cacheMode = 0)
{
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowDn2Nz(src, workSpace, true, intriParams);
    }
    if constexpr (SupportType<T, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
        ASCENDC_ASSERT((intriParams.nValue % 2 == 0), {
            KERNEL_LOG(KERNEL_ERROR, "if src datatype is 4bit, the inner axis must be an even number.");
        });
        // data is transferred based on the b8 type, and parameters are set based on the b8 type
        DataCopyGM2L1DN2NZImplBase((__cbuf__ uint8_t*)dst, (__gm__ uint8_t*)src, intriParams, enableSmallC0, cacheMode);
    } else {
        DataCopyGM2L1DN2NZImplBase((__cbuf__ T*)dst, (__gm__ T*)src, intriParams, enableSmallC0, cacheMode);
    }
}

/* **************************************************************************************************
 * NZ2ND                                         *
 * ************************************************************************************************* */

template <typename T>
__aicore__ inline void DataCopyL12GMNZ2NDImpl(__gm__ T* dst, __cbuf__ T* src, const Nz2NdParamsFull& intriParams)
{
    if constexpr (sizeof(T) == B8_BYTE_SIZE || sizeof(T) == B16_BYTE_SIZE) {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5101)
        // ISA/API: Is the ND matrix number to be moved
        uint64_t ndNum = intriParams.ndNum;
        // ISA/API: unit of element
        uint16_t loop2Size = intriParams.nValue;
        // ISA/API: unit of element
        uint32_t loop3Size = intriParams.dValue;
        // ISA: loop2SrcStride = 1, and does not to be set by programmer.
        constexpr uint64_t loop2SrcStride = 1;
        // ISA: unit of C0_size(32B)    API: unit of 16 elements
        uint64_t loop3SrcStride = intriParams.srcNStride * sizeof(T) >> 1;
        // ISA: unit of C0_size(32B)    API: unit of 256 elements
        uint64_t loop4SrcStride = intriParams.srcNdMatrixStride * sizeof(T) * 8;
        // ISA: unit of byte    API: unit of element
        uint64_t loop2DstStride = intriParams.dstDStride * sizeof(T);
        // ISA: unit of byte    API: unit of element
        uint64_t loop4DstStride = intriParams.dstNdMatrixStride * sizeof(T);

        // SPR.FIXP_NZ_PARA
        uint64_t config = ndNum | (loop2SrcStride << 16) | (loop3SrcStride << 32) | (loop4SrcStride << 48);
        set_fixp_nz_para(config);

        fix_cbuf_to_gm(
            dst, src, loop2DstStride, loop3Size, fixp_trans_mode_t::NZ2ND, loop4DstStride, loop2Size, intriParams.sid);
#endif

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
        // ISA/API: Is the ND matrix number to be moved
        uint64_t ndNum = intriParams.ndNum;
        // ISA/API: unit of element
        uint16_t loop2Size = intriParams.nValue;
        // ISA/API: unit of element
        uint32_t loop3Size = intriParams.dValue;
        // ISA: loop2SrcStride = 1, and does not to be set by programmer.
        constexpr uint64_t loop2SrcStride = 1;
        // ISA: unit of C0_size(32B)    API: unit of 16 elements
        uint64_t loop3SrcStride = intriParams.srcNStride * sizeof(T) >> 1;
        // ISA: unit of C0_size(32B)    API: unit of 256 elements
        uint64_t loop4SrcStride = intriParams.srcNdMatrixStride * sizeof(T) * 8;
        // ISA: unit of byte    API: unit of element
        uint64_t loop2DstStride = intriParams.dstDStride * sizeof(T);
        // ISA: unit of byte    API: unit of element
        uint64_t loop4DstStride = intriParams.dstNdMatrixStride * sizeof(T);

        // SPR.FIXP_NZ_PARA
        uint64_t config = ndNum | (loop4SrcStride << 16) | (loop4DstStride << 32);
        set_fixp_nz_para(config);

        fix_cbuf_to_gm(
            dst, src, loop3Size, loop2Size, loop2DstStride, loop3SrcStride, loop2SrcStride, fixp_trans_mode_t::NZ2ND,
            intriParams.sid);
#endif
    } else {
        ASSERT(false && "unsupported data type of copy L1 to GM NZ2ND on current device");
    }
}

template <typename T>
__aicore__ inline void DataCopyL12UBNZ2NDImpl(__ubuf__ T* dst, __cbuf__ T* src, const Nz2NdParamsFull& intriParams)
{
    if constexpr (sizeof(T) == B8_BYTE_SIZE || sizeof(T) == B16_BYTE_SIZE) {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5101)
        // ISA/API: Is the ND matrix number to be moved
        uint64_t ndNum = intriParams.ndNum;
        // ISA/API: unit of element
        uint16_t loop2Size = intriParams.nValue;
        // ISA/API: unit of element
        uint32_t loop3Size = intriParams.dValue;
        // ISA: loop2SrcStride = 1, and does not to be set by programmer.
        constexpr uint64_t loop2SrcStride = 1;
        // ISA: unit of C0_size(32B)    API: unit of 16 elements
        uint64_t loop3SrcStride = intriParams.srcNStride * sizeof(T) >> 1;
        // ISA: unit of C0_size(32B)    API: unit of 256 elements
        uint64_t loop4SrcStride = intriParams.srcNdMatrixStride * sizeof(T) * 8;
        // ISA: unit of byte    API: unit of element
        uint64_t loop2DstStride = intriParams.dstDStride * sizeof(T);
        // ISA: unit of byte    API: unit of element
        uint64_t loop4DstStride = intriParams.dstNdMatrixStride * sizeof(T);

        // SPR.FIXP_NZ_PARA
        uint64_t config = ndNum | (loop2SrcStride << 16) | (loop3SrcStride << 32) | (loop4SrcStride << 48);
        set_fixp_nz_para(config);

        fix_cbuf_to_ubuf(dst, src, loop2DstStride, loop3Size, fixp_trans_mode_t::NZ2ND, loop4DstStride, loop2Size);
#endif

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
        // ISA/API: Is the ND matrix number to be moved
        uint64_t ndNum = intriParams.ndNum;
        // ISA/API: unit of element
        uint16_t loop2Size = intriParams.nValue;
        // ISA/API: unit of element
        uint32_t loop3Size = intriParams.dValue;
        // ISA: loop2SrcStride = 1, and does not to be set by programmer.
        constexpr uint64_t loop2SrcStride = 1;
        // ISA: unit of C0_size(32B)    API: unit of 16 elements
        uint64_t loop3SrcStride = intriParams.srcNStride * sizeof(T) >> 1;
        // ISA: unit of C0_size(32B)    API: unit of 256 elements
        uint64_t loop4SrcStride = intriParams.srcNdMatrixStride * sizeof(T) * 8;
        // ISA: unit of byte    API: unit of element
        uint64_t loop2DstStride = intriParams.dstDStride * sizeof(T);
        // ISA: unit of byte    API: unit of element
        uint64_t loop4DstStride = intriParams.dstNdMatrixStride * sizeof(T);

        // SPR.FIXP_NZ_PARA
        uint64_t config = ndNum | (loop4SrcStride << 16) | (loop4DstStride << 32);
        set_fixp_nz_para(config);

        fix_cbuf_to_ubuf(
            dst, src, loop3Size, loop2Size, loop2DstStride, loop3SrcStride, loop2SrcStride, fixp_trans_mode_t::NZ2ND);
#endif
    } else {
        ASSERT(false && "unsupported data type of copy L1 to GM NZ2ND on current device");
    }
}

template <typename T>
__aicore__ inline void DataCopyUB2GMNZ2NDImpl(__gm__ T* dst, __ubuf__ T* src, const Nz2NdParamsFull& intriParams)
{
    ASSERT(false && "unsupported data copy UB to GM NZ2ND on current device");
}

/* **************************************************************************************************
 * NZ2DN                                         *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void DataCopyL12GMNZ2DNImpl(__gm__ T* dst, __cbuf__ T* src, const Nz2DnParamsFull& intriParams)
{
    if constexpr (sizeof(T) == B8_BYTE_SIZE || sizeof(T) == B16_BYTE_SIZE) {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5101)
        // ISA/API: Is the ND matrix number to be moved
        uint64_t dnNum = intriParams.dnNum;
        // ISA/API: unit of element
        uint16_t loop2Size = intriParams.nValue;
        // ISA/API: unit of element
        uint32_t loop3Size = intriParams.dValue;
        // ISA: loop2SrcStride = 1, and does not to be set by programmer.
        constexpr uint64_t loop2SrcStride = 1;
        // ISA: unit of C0_size(32B)    API: unit of 16 elements
        uint64_t loop3SrcStride = intriParams.srcNStride * sizeof(T) >> 1;
        // ISA: unit of C0_size(32B)    API: unit of 256 elements
        uint64_t loop4SrcStride = intriParams.srcNzMatrixStride * sizeof(T) * 8;
        // ISA: unit of byte    API: unit of element
        uint64_t loop2DstStride = intriParams.dstNStride * sizeof(T);
        // ISA: unit of byte    API: unit of element
        uint64_t loop4DstStride = intriParams.dstDnMatrixStride * sizeof(T);

        // SPR.FIXP_NZ_PARA
        uint64_t config = dnNum | (loop2SrcStride << 16) | (loop3SrcStride << 32) | (loop4SrcStride << 48);
        set_fixp_nz_para(config);

        fix_cbuf_to_gm(
            dst, src, loop2DstStride, loop3Size, fixp_trans_mode_t::NZ2DN, loop4DstStride, loop2Size, intriParams.sid);
#endif

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
        // ISA/API: Is the ND matrix number to be moved
        uint64_t dnNum = intriParams.dnNum;
        // ISA/API: unit of element
        uint16_t loop2Size = intriParams.nValue;
        // ISA/API: unit of element
        uint32_t loop3Size = intriParams.dValue;
        // ISA: loop2SrcStride = 1, and does not to be set by programmer.
        constexpr uint64_t loop2SrcStride = 1;
        // ISA: unit of C0_size(32B)    API: unit of 16 elements
        uint64_t loop3SrcStride = intriParams.srcNStride * sizeof(T) >> 1;
        // ISA: unit of C0_size(32B)    API: unit of 256 elements
        uint64_t loop4SrcStride = intriParams.srcNzMatrixStride * sizeof(T) * 8;
        // ISA: unit of byte    API: unit of element
        uint64_t loop2DstStride = intriParams.dstNStride * sizeof(T);
        // ISA: unit of byte    API: unit of element
        uint64_t loop4DstStride = intriParams.dstDnMatrixStride * sizeof(T);

        // SPR.FIXP_NZ_PARA
        uint64_t config = dnNum | (loop4SrcStride << 16) | (loop4DstStride << 32);
        set_fixp_nz_para(config);

        fix_cbuf_to_gm(
            dst, src, loop3Size, loop2Size, loop2DstStride, loop3SrcStride, loop2SrcStride, fixp_trans_mode_t::NZ2DN,
            intriParams.sid);
#endif
    } else {
        ASSERT(false && "unsupported data type of copy L1 to GM NZ2ND on current device");
    }
}

template <typename T>
__aicore__ inline void DataCopyL12UBNZ2DNImpl(__ubuf__ T* dst, __cbuf__ T* src, const Nz2DnParamsFull& intriParams)
{
    if constexpr (sizeof(T) == B8_BYTE_SIZE || sizeof(T) == B16_BYTE_SIZE) {
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5101)
        // ISA/API: Is the ND matrix number to be moved
        uint64_t dnNum = intriParams.dnNum;
        // ISA/API: unit of element
        uint16_t loop2Size = intriParams.nValue;
        // ISA/API: unit of element
        uint32_t loop3Size = intriParams.dValue;
        // ISA: loop2SrcStride = 1, and does not to be set by programmer.
        constexpr uint64_t loop2SrcStride = 1;
        // ISA: unit of C0_size(32B)    API: unit of 16 elements
        uint64_t loop3SrcStride = intriParams.srcNStride * sizeof(T) >> 1;
        // ISA: unit of C0_size(32B)    API: unit of 256 elements
        uint64_t loop4SrcStride = intriParams.srcNzMatrixStride * sizeof(T) * 8;
        // ISA: unit of byte    API: unit of element
        uint64_t loop2DstStride = intriParams.dstNStride * sizeof(T);
        // ISA: unit of byte    API: unit of element
        uint64_t loop4DstStride = intriParams.dstDnMatrixStride * sizeof(T);

        // SPR.FIXP_NZ_PARA
        uint64_t config = dnNum | (loop2SrcStride << 16) | (loop3SrcStride << 32) | (loop4SrcStride << 48);
        set_fixp_nz_para(config);

        fix_cbuf_to_ubuf(dst, src, loop2DstStride, loop3Size, fixp_trans_mode_t::NZ2DN, loop4DstStride, loop2Size);
#endif

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
        // ISA/API: Is the ND matrix number to be moved
        uint64_t dnNum = intriParams.dnNum;
        // ISA/API: unit of element
        uint16_t loop2Size = intriParams.nValue;
        // ISA/API: unit of element
        uint32_t loop3Size = intriParams.dValue;
        // ISA: loop2SrcStride = 1, and does not to be set by programmer.
        constexpr uint64_t loop2SrcStride = 1;
        // ISA: unit of C0_size(32B)    API: unit of 16 elements
        uint64_t loop3SrcStride = intriParams.srcNStride * sizeof(T) >> 1;
        // ISA: unit of C0_size(32B)    API: unit of 256 elements
        uint64_t loop4SrcStride = intriParams.srcNzMatrixStride * sizeof(T) * 8;
        // ISA: unit of byte    API: unit of element
        uint64_t loop2DstStride = intriParams.dstNStride * sizeof(T);
        // ISA: unit of byte    API: unit of element
        uint64_t loop4DstStride = intriParams.dstDnMatrixStride * sizeof(T);

        // SPR.FIXP_NZ_PARA
        uint64_t config = dnNum | (loop4SrcStride << 16) | (loop4DstStride << 32);
        set_fixp_nz_para(config);

        fix_cbuf_to_ubuf(
            dst, src, loop3Size, loop2Size, loop2DstStride, loop3SrcStride, loop2SrcStride, fixp_trans_mode_t::NZ2DN);
#endif
    } else {
        ASSERT(false && "unsupported data type of copy L1 to GM NZ2ND on current device");
    }
}

template <typename T>
__aicore__ inline void DataCopyUB2GMNZ2DNImpl(__gm__ T* dst, __ubuf__ T* src, const Nz2DnParamsFull& intriParams)
{
    ASSERT(false && "unsupported data copy UB to GM NZ2DN on current device");
}

/* **************************************************************************************************
 * Copy                                             *
 * ************************************************************************************************* */

// Copy::Level 0 - mask bit mode
template <typename T, bool isSetMask = true>
__aicore__ inline void CopyImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask[2], const uint8_t repeatTimes,
    const CopyRepeatParams& repeatParams)
{
    ASSERT(false && "unsupported data copy from gm to ubuf nd2nz on this version");
}

// Copy::Level 0 - mask count mode
template <typename T, bool isSetMask = true>
__aicore__ inline void CopyImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask, const uint8_t repeatTimes,
    const CopyRepeatParams& repeatParams)
{
    ASSERT(false && "unsupported data copy from gm to ubuf nd2nz on this version");
}

/* **************************************************************************************************
 * DataCopy Enhanced                                             *
 * ************************************************************************************************* */

template <typename T, typename U>
__aicore__ inline void DataCopyL12L0CImpl(
    __cc__ T* dst, __cbuf__ U* src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
{
    ASSERT(false && "unsupported data copy L1 to L0C on current device");
}

template <typename T, typename U>
__aicore__ inline void DataCopyL0C2UBImpl(
    __ubuf__ T* dst, __cc__ U* src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
{
    ASSERT(false && "unsupported data copy L0C to UB on current device");
}

template <typename T, typename U>
__aicore__ inline void DataCopyUB2L0CImpl(
    __cc__ T* dst, __ubuf__ U* src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
{
    ASSERT(false && "unsupported data copy UB to L0C on current device");
}

template <typename T>
__aicore__ inline void DataCopySliceGm2UBImpl(__ubuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams)
{
    uint32_t offsetSrc = 0;
    uint32_t offsetDst = 0;
    for (uint32_t i = 0; i < intriParams.blockCount; i++) {
        offsetSrc = offsetSrc + i * (intriParams.blockLen * ONE_BLK_SIZE + intriParams.srcStride);
        offsetDst = offsetDst + i * (intriParams.blockLen * ONE_BLK_SIZE + intriParams.dstStride);
        DataCopyGM2UBImpl(
            dst + offsetDst / sizeof(T), src + offsetSrc / sizeof(T), {1, intriParams.blockLen, 0, 0, intriParams.sid});
    }
}

template <typename T>
__aicore__ inline void DataCopySliceUB2GMImpl(__gm__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
    uint32_t offsetSrc = 0;
    uint32_t offsetDst = 0;
    for (uint32_t i = 0; i < intriParams.blockCount; i++) {
        offsetSrc = offsetSrc + i * (intriParams.blockLen * ONE_BLK_SIZE + intriParams.srcStride);
        offsetDst = offsetDst + i * (intriParams.blockLen * ONE_BLK_SIZE + intriParams.dstStride);
        DataCopyUB2GMImpl(
            dst + offsetDst / sizeof(T), src + offsetSrc / sizeof(T), {1, intriParams.blockLen, 0, 0, intriParams.sid});
    }
}

/* **************************************************************************************************
 * DataCopyPad                                             *
 * ************************************************************************************************* */

__aicore__ inline void CheckSrcGmDataCopyExtParamsRange(const DataCopyExtParams& intriParams)
{
    ASCENDC_ASSERT((intriParams.srcStride <= static_cast<int64_t>((1ul << 40) - 1)), {
        KERNEL_LOG(KERNEL_ERROR, "srcStride is %d, which should be no more than 2^40-1", intriParams.srcStride);
    });
    ASCENDC_ASSERT((intriParams.srcStride >= (-static_cast<int64_t>(intriParams.blockLen))), {
        KERNEL_LOG(
            KERNEL_ERROR, "srcStride is %d, which should be no less than %d", intriParams.srcStride,
            -intriParams.blockLen);
    });
    ASCENDC_ASSERT((intriParams.dstStride <= static_cast<int64_t>(65535)), {
        KERNEL_LOG(KERNEL_ERROR, "dstStride is %d, which should be no more than 65535", intriParams.dstStride);
    });
    ASCENDC_ASSERT((intriParams.dstStride >= static_cast<int64_t>(0)), {
        KERNEL_LOG(KERNEL_ERROR, "dstStride is %d, which should be no less than 0", intriParams.dstStride);
    });
}

template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline void DataCopyPadGm2UBImpl(
    __ubuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams, const DataCopyPadParams& padParams)
{
    if ASCEND_IS_AIC {
        return;
    }
    if (padParams.isPad == true) {
        set_pad_val_outtoub(padParams.paddingValue);
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
    CopyGmToUbufAlignV2<T, mode>(
        dst, src, intriParams.blockCount, intriParams.blockLen, padParams.leftPadding, padParams.rightPadding,
        intriParams.srcStride, intriParams.dstStride, true, padParams.isPad, intriParams.sid); // padding is 0
}

template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline void DataCopyPadGm2UBImpl(
    __ubuf__ T* dst, __gm__ T* src, const DataCopyExtParams& intriParams, const DataCopyPadExtParams<T>& padParams,
    const uint8_t cacheMode = 0)
{
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
    CopyGmToUbufAlignV2<T, mode>(
        dst, src, intriParams.blockCount, intriParams.blockLen, padParams.leftPadding, padParams.rightPadding,
        intriParams.srcStride, intriParams.dstStride, true, padParams.isPad, intriParams.sid);
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
        padParams.isPad, intriParams.srcStride, intriParams.dstStride, true, intriParams.sid);
}

template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline void DataCopyPadGm2L1Impl(
    __cbuf__ T* dst, __gm__ T* src, const DataCopyExtParams& intriParams, const DataCopyPadExtParams<T>& padParams,
    const uint8_t cacheMode = 0)
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
        padParams.isPad, intriParams.srcStride, intriParams.dstStride, true, intriParams.sid);
}

// UB -> GM   DataCopyPad with DataCopyParams
template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline void DataCopyPadUB2GMImpl(__gm__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
    CopyUbufToGmAlignV2<T, mode>(
        dst, src, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, true,
        intriParams.sid);
}

// UB -> GM   DataCopyPad with DataCopyExtParams
template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline void DataCopyPadUB2GMImpl(
    __gm__ T* dst, __ubuf__ T* src, const DataCopyExtParams& intriParams, const uint8_t cacheMode = 0)
{
    ASCENDC_ASSERT((intriParams.dstStride <= static_cast<int64_t>(1ul << 40 - 1)), {
        KERNEL_LOG(KERNEL_ERROR, "dstStride is %d, which should be no more than 2^40-1", intriParams.dstStride);
    });
    ASCENDC_ASSERT((intriParams.dstStride >= static_cast<int64_t>(0)), {
        KERNEL_LOG(KERNEL_ERROR, "dstStride is %d, which should be no less than 0", intriParams.dstStride);
    });
    ASCENDC_ASSERT((intriParams.srcStride <= static_cast<int64_t>(65535)), {
        KERNEL_LOG(KERNEL_ERROR, "srcStride is %d, which should be no more than 65535", intriParams.srcStride);
    });
    ASCENDC_ASSERT((intriParams.srcStride >= static_cast<int64_t>(0)), {
        KERNEL_LOG(KERNEL_ERROR, "srcStride is %d, which should be no less than 0", intriParams.srcStride);
    });
    CopyUbufToGmAlignV2<T, mode>(
        dst, src, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, true,
        intriParams.sid);
}

template <typename T>
__aicore__ inline void DataCopyPadUB2L1Impl(__cbuf__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
    ASSERT(false && "unsupported data copy UB to L1 on current device");
}

template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline void DataCopyPadL12GMImpl(__gm__ T* dst, __cbuf__ T* src, const DataCopyParams& intriParams)
{
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(dst, workSpace, false, (uint64_t) true, intriParams);
    }

    uint32_t srcStride = 0;
    if constexpr (mode == PaddingMode::Normal) {
        srcStride = AlignUp(intriParams.srcStride * BYTE_32_ALIGN + intriParams.blockLen, BYTE_32_ALIGN);
    } else {
        srcStride = intriParams.blockLen;
    }
    uint64_t dstStride = intriParams.blockLen + intriParams.dstStride;

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5101)
    copy_cbuf_to_gm_align_v2(
        dst, src, (uint8_t)intriParams.sid, (uint32_t)intriParams.blockCount, (uint32_t)intriParams.blockLen, dstStride,
        srcStride);
#endif

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
    // ISA/API: Is the ND matrix number to be moved
    uint64_t ndNum = 1;
    // ISA/API: unit of element
    uint16_t loop2Size = intriParams.blockLen;
    // ISA/API: unit of element
    uint32_t loop3Size = intriParams.blockCount;
    // ISA: loop2SrcStride = 1, and does not to be set by programmer.
    constexpr uint64_t loop2SrcStride = 1;
    // ISA: unit of C0_size(32B)
    uint64_t loop3SrcStride = DivCeil(srcStride, BYTE_32_ALIGN);
    // ISA: unit of C0_size(32B)
    uint64_t loop4SrcStride = 0;
    // ISA: unit of byte
    uint64_t loop2DstStride = dstStride;
    // ISA: unit of byte
    uint64_t loop4DstStride = 0;

    // SPR.FIXP_NZ_PARA
    uint64_t config = ndNum | (loop4SrcStride << 16) | (loop4DstStride << 32);
    set_fixp_nz_para(config);

    fix_cbuf_to_gm(
        dst, src, loop3Size, loop2Size, loop2DstStride, loop3SrcStride, loop2SrcStride, fixp_trans_mode_t::NORMAL_DMA,
        intriParams.sid);
#endif
}

template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline void DataCopyPadL12GMImpl(__gm__ T* dst, __cbuf__ T* src, const DataCopyExtParams& intriParams)
{
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(dst, workSpace, false, (uint64_t) true, intriParams);
    }

    uint32_t srcStride = 0;
    if constexpr (mode == PaddingMode::Normal) {
        srcStride = AlignUp(intriParams.srcStride * BYTE_32_ALIGN + intriParams.blockLen, BYTE_32_ALIGN);
    } else {
        srcStride = intriParams.blockLen;
    }
    uint64_t dstStride = intriParams.blockLen + intriParams.dstStride;

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 5101)
    copy_cbuf_to_gm_align_v2(
        dst, src, (uint8_t)intriParams.sid, (uint32_t)intriParams.blockCount, (uint32_t)intriParams.blockLen, dstStride,
        srcStride);
#endif

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
    // ISA/API: Is the ND matrix number to be moved
    uint64_t ndNum = 1;
    // ISA/API: unit of element
    uint16_t loop2Size = intriParams.blockLen;
    // ISA/API: unit of element
    uint32_t loop3Size = intriParams.blockCount;
    // ISA: loop2SrcStride = 1, and does not to be set by programmer.
    constexpr uint64_t loop2SrcStride = 1;
    // ISA: unit of C0_size(32B)
    uint64_t loop3SrcStride = DivCeil(srcStride, BYTE_32_ALIGN);
    // ISA: unit of C0_size(32B)
    uint64_t loop4SrcStride = 0;
    // ISA: unit of byte
    uint64_t loop2DstStride = dstStride;
    // ISA: unit of byte
    uint64_t loop4DstStride = 0;

    // SPR.FIXP_NZ_PARA
    uint64_t config = ndNum | (loop4SrcStride << 16) | (loop4DstStride << 32);
    set_fixp_nz_para(config);

    fix_cbuf_to_gm(
        dst, src, loop3Size, loop2Size, loop2DstStride, loop3SrcStride, loop2SrcStride, fixp_trans_mode_t::NORMAL_DMA,
        intriParams.sid);
#endif
}

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_DATA_COPY_IMPL_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_IMPL_H__
#endif
