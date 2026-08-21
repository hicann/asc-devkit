/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_3510/kernel_operator_data_copy_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_DATA_COPY_IMPL_H
#define ASCENDC_MODULE_OPERATOR_DATA_COPY_IMPL_H
#include "kernel_operator_scm_data_copy_impl.h"
#include "kernel_operator_vec_duplicate_impl.h"
#include "kernel_operator_sync_impl.h"
#include "../kernel_pop_stack_buffer.h"
#include "../../../include/basic_api/kernel_struct_unary.h"
#include "kernel_operator_vec_template_impl.h"
#include "kernel_operator_vec_binary_scalar_impl.h"
#include "../../../include/basic_api/kernel_operator_swap_mem_intf.h"
#include <utility>

namespace AscendC {
// all input params: C220 version, not C310 version, thus need conversion by *32
constexpr uint8_t BYTE_32_ALIGN = 32; // in unit of 32 bytes

__aicore__ inline void ValidateUbL1Address(uint64_t absUbAddr, uint64_t absL1Addr, uint32_t tensorSize)
{
    ASCENDC_ASSERT((absUbAddr < TOTAL_UB_SIZE), {
        KERNEL_LOG(KERNEL_ERROR, "absUbAddr is 0x%lx, which should be in range of [0, %u)", absUbAddr, TOTAL_UB_SIZE);
    });
    ASCENDC_ASSERT((static_cast<uint64_t>(absUbAddr + tensorSize) < TOTAL_UB_SIZE), {
        KERNEL_LOG(
            KERNEL_ERROR, "absUbAddr is 0x%lx, tensorSize is %u, which exceeds the limit of ub %d)", absUbAddr,
            tensorSize, TOTAL_UB_SIZE);
    });
    ASCENDC_ASSERT((absL1Addr < TOTAL_L1_SIZE), {
        KERNEL_LOG(KERNEL_ERROR, "absL1Addr is 0x%lx, which should be in range [0, %u)", absL1Addr, TOTAL_L1_SIZE);
    });
    ASCENDC_ASSERT((static_cast<uint64_t>(absL1Addr + tensorSize) < TOTAL_L1_SIZE), {
        KERNEL_LOG(
            KERNEL_ERROR, "absL1Addr is 0x%lx, tensorSize is %u, which exceeds the limit of l1 %u)", absL1Addr,
            tensorSize, TOTAL_L1_SIZE);
    });
}

// only support VecCore    PIPE_MTE2
// GM -> UB: copy_gm_to_ubuf_align_v2: support changing padding value
// Note: dst stride in normal mode is in unit of 32Bytes, in compact mode is in unit of is 1Byte
template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline void CopyGmToUbufAlignV2(
    __ubuf__ T* dst, __gm__ T* src, const uint16_t blockCount, const uint32_t blockLen, const uint8_t leftPaddingCount,
    const uint8_t rightPaddingCount, const int64_t srcStride, const int64_t dstStride, const bool isDataCopyPad,
    const uint8_t cacheMode = 0, const bool isPad = true)
{
    if ASCEND_IS_AIV {
        // DataCopy is in unit of 32 bytes, DataCopyPad is in unit of 1byte
        uint32_t unitOfBytes = (isDataCopyPad) ? 1 : BYTE_32_ALIGN;
        uint32_t burstLength = blockLen * unitOfBytes;
        uint32_t padLength = leftPaddingCount * sizeof(T) + rightPaddingCount * sizeof(T);
        uint64_t srcStride310 = srcStride * unitOfBytes + burstLength; // GM  DataCopy:32Bytes, DataCopyPad:1Byte
        uint32_t dstStride310 = 0;
        uint8_t leftPaddingCnt = leftPaddingCount;
        uint8_t rigntPaddingCnt = rightPaddingCount;
        if constexpr (mode == PaddingMode::Normal) {
            dstStride310 =
                AlignUp(static_cast<uint32_t>(dstStride) * BYTE_32_ALIGN + burstLength + padLength, BYTE_32_ALIGN);
        } else {
            dstStride310 = burstLength;
            rigntPaddingCnt = 0;
            leftPaddingCnt = 0;
        }
        if constexpr (sizeof(T) == 8) { // B64
            copy_gm_to_ubuf_align_v2(
                (__ubuf__ uint32_t*)dst, (__gm__ uint32_t*)src, 0, blockCount, burstLength, leftPaddingCnt * 2,
                rigntPaddingCnt * 2, isPad, cacheMode, srcStride310, dstStride310);
        } else if constexpr (sizeof(T) == 4) {
            copy_gm_to_ubuf_align_v2(
                (__ubuf__ uint32_t*)dst, (__gm__ uint32_t*)src, 0, blockCount, burstLength, leftPaddingCnt,
                rigntPaddingCnt, isPad, cacheMode, srcStride310, dstStride310);
        } else if constexpr (sizeof(T) == 2) {
            copy_gm_to_ubuf_align_v2(
                (__ubuf__ uint16_t*)dst, (__gm__ uint16_t*)src, 0, blockCount, burstLength, leftPaddingCnt,
                rigntPaddingCnt, isPad, cacheMode, srcStride310, dstStride310);
        } else if constexpr (sizeof(T) == 1) {
            copy_gm_to_ubuf_align_v2(
                (__ubuf__ uint8_t*)dst, (__gm__ uint8_t*)src, 0, blockCount, burstLength, leftPaddingCnt,
                rigntPaddingCnt, isPad, cacheMode, srcStride310, dstStride310);
        }
    }
}

// only support VecCore   PIPE_MTE3
// UB -> GM: copy_ubuf_to_gm_align_v2
template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline void CopyUbufToGmAlignV2(
    __gm__ T* dst, __ubuf__ T* src, const uint16_t blockCount, const uint32_t blockLen, const int64_t srcStride,
    const int64_t dstStride, const bool isDataCopyPad, const uint8_t cacheMode = 0)
{
    if ASCEND_IS_AIV {
        // DataCopy is in unit of 32 bytes, DataCopyPad is in unit of 1byte / 32bytes
        uint32_t unitOfBytes = (isDataCopyPad) ? 1 : BYTE_32_ALIGN;
        uint32_t burstLength = blockLen * unitOfBytes;
        uint32_t srcStride310 = 0;
        if constexpr (mode == PaddingMode::Normal) {
            srcStride310 = static_cast<uint32_t>(srcStride) * BYTE_32_ALIGN +
                           burstLength; // UB   DataCopy:32Bytes, DataCopyPad:32Bytes
            // srcstride should be 32B aligned
            srcStride310 = DivCeil(srcStride310, BYTE_32_ALIGN) * BYTE_32_ALIGN;
        } else {
            srcStride310 = burstLength;
        }
        uint64_t dstStride310 = dstStride * unitOfBytes + burstLength; // GM   DataCopy:32Bytes, DataCopyPad:1Byte
        copy_ubuf_to_gm_align_v2(
            (__gm__ void*)dst, (__ubuf__ void*)src, 0, blockCount, burstLength, cacheMode, dstStride310, srcStride310);
    }
}

// only support CubeCore   PIPE_MTE2
// GM -> L1: copy_gm_to_cbuf_align_v2
// Note: stride in normal mode is in unit of 32Bytes, in compact mode is in unit of is 1Byte
template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline void CopyGmToCbufAlignV2(
    __cbuf__ T* dst, __gm__ T* src, const uint16_t blockCount, const uint32_t blockLen, const uint8_t leftPaddingCount,
    const uint8_t rightPaddingCount, const int64_t srcStride, const int64_t dstStride, const bool isDataCopyPad,
    const uint8_t cacheMode = 0)
{
    if ASCEND_IS_AIC {
        uint32_t unitOfBytes = (isDataCopyPad) ? 1 : BYTE_32_ALIGN;
        uint32_t burstLength = blockLen * unitOfBytes;
        uint32_t padLength = leftPaddingCount * sizeof(T) + rightPaddingCount * sizeof(T);
        uint64_t actSrcStride = srcStride * unitOfBytes + burstLength; // GM   DataCopy: 32Bytes
        uint32_t actDstStride = 0;
        uint8_t leftPaddingCnt = leftPaddingCount;
        uint8_t rigntPaddingCnt = rightPaddingCount;
        if constexpr (mode == PaddingMode::Normal) {
            actDstStride = AlignUp(
                static_cast<uint32_t>(dstStride) * BYTE_32_ALIGN + burstLength + padLength,
                BYTE_32_ALIGN); // L1   DataCopy: 32Bytes
        } else {
            actDstStride = burstLength;
            leftPaddingCnt = 0;
            rigntPaddingCnt = 0;
        }

        if constexpr (sizeof(T) == 8) { // B64
            copy_gm_to_cbuf_align_v2(
                (__cbuf__ uint32_t*)dst, (__gm__ uint32_t*)src, 0, blockCount, burstLength, leftPaddingCnt * 2,
                rigntPaddingCnt * 2, true, cacheMode, actSrcStride, actDstStride);
        } else if constexpr (sizeof(T) == 4) {
            copy_gm_to_cbuf_align_v2(
                (__cbuf__ uint32_t*)dst, (__gm__ uint32_t*)src, 0, blockCount, burstLength, leftPaddingCnt,
                rigntPaddingCnt, true, cacheMode, actSrcStride, actDstStride);
        } else if constexpr (sizeof(T) == 2) {
            copy_gm_to_cbuf_align_v2(
                (__cbuf__ uint16_t*)dst, (__gm__ uint16_t*)src, 0, blockCount, burstLength, leftPaddingCnt,
                rigntPaddingCnt, true, cacheMode, actSrcStride, actDstStride);
        } else if constexpr (sizeof(T) == 1) {
            copy_gm_to_cbuf_align_v2(
                (__cbuf__ uint8_t*)dst, (__gm__ uint8_t*)src, 0, blockCount, burstLength, leftPaddingCnt,
                rigntPaddingCnt, true, cacheMode, actSrcStride, actDstStride);
        }
    }
}

// only support VecCore   PIPE_V
// UB -> UB: copy_ubuf_to_ubuf:  Note: burstLen + stride is unit of 32B
template <typename T>
__aicore__ inline void CopyUbufToUbuf(
    __ubuf__ T* dst, __ubuf__ T* src, const uint16_t blockCount, const uint16_t blockLen, const uint16_t srcStride,
    const uint16_t dstStride)
{
    if ASCEND_IS_AIV {
        copy_ubuf_to_ubuf((__ubuf__ void*)dst, (__ubuf__ void*)src, 0, blockCount, blockLen, srcStride, dstStride);
    }
}

// only support VecCore   PIPE_MTE3
// UB -> L1: copy_ubuf_to_cbuf:  Note: burstLen + stride is unit of 32B
template <typename T>
__aicore__ inline void CopyUbufToCbuf(
    __cbuf__ T* dst, __ubuf__ T* src, const uint16_t blockCount, const uint16_t blockLen, const uint16_t srcStride,
    const uint16_t dstStride)
{
    if ASCEND_IS_AIV {
        copy_ubuf_to_cbuf((__cbuf__ void*)dst, (__ubuf__ void*)src, 0, blockCount, blockLen, srcStride, dstStride);
    }
}

// only support CubeCore   PIPE_MTE1
// L1 -> UB: copy_cbuf_to_ubuf
template <typename T, uint8_t subBlockId = 0>
__aicore__ inline void CopyCbufToUbuf(
    __ubuf__ T* dst, __cbuf__ T* src, const uint16_t blockCount, const uint16_t blockLen, const uint16_t srcStride,
    const uint16_t dstStride)
{
    if ASCEND_IS_AIC {
        copy_cbuf_to_ubuf(
            (__ubuf__ void*)dst, (__cbuf__ void*)src, static_cast<bool>(subBlockId), blockCount, blockLen, srcStride,
            dstStride);
    }
}

// only support CubeCore   PIPE_MTE1
// L1 -> BT: copy_cbuf_to_bt          Note: Only support half + float + int32_t
template <typename T>
__aicore__ inline void CopyCbufToBt(
    uint64_t dst, __cbuf__ T* src, const uint16_t convControl, const uint16_t blockCount, const uint16_t blockLen,
    const uint16_t srcStride, const uint16_t dstStride)
{
    if ASCEND_IS_AIC {
        if constexpr (
            std::is_same<T, bfloat16_t>::value || std::is_same<T, float>::value || std::is_same<T, int32_t>::value ||
            std::is_same<T, half>::value) {
            if constexpr (sizeof(T) == 4) {
                ASCENDC_DEBUG_ASSERT(
                    (blockLen % 2 == 0),
                    KERNEL_LOG_INTERNAL(
                        KERNEL_ERROR,
                        "For float or int32_t types, blockLen must be even when DataCopy from L1 to BIAS."));
            }
            copy_cbuf_to_bt(dst, src, static_cast<bool>(convControl), blockCount, blockLen, srcStride, dstStride);
        }
    }
}

// Small C0 Mode: only when D value <= 4 can enable
__aicore__ inline void SmallC0Checker(bool enableSmallC0, uint16_t dValue)
{
    if (enableSmallC0) {
        ASCENDC_ASSERT((dValue <= 4), {
            KERNEL_LOG(KERNEL_ERROR, "Small C0 can be enabled only when D value is less or equal to 4.");
        });
    }
}

/* **************************************************************************************************
 * DataCopy                                             *
 * ************************************************************************************************* */

// SRC: GM
template <typename T>
__aicore__ inline void DataCopyGM2UBImpl(
    __ubuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams, const uint8_t cacheMode = 0)
{
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(src, workSpace, true, false, intriParams);
    }
    if constexpr (!std::is_same<T, void>::value) {
        if constexpr (SupportType<T, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
            CopyGmToUbufAlignV2(
                (__ubuf__ uint16_t*)dst, (__gm__ uint16_t*)src, intriParams.blockCount, intriParams.blockLen, 0, 0,
                intriParams.srcStride, intriParams.dstStride, false, cacheMode, true);
        } else {
            CopyGmToUbufAlignV2(
                dst, src, intriParams.blockCount, intriParams.blockLen, 0, 0, intriParams.srcStride,
                intriParams.dstStride, false, cacheMode, true);
        }
    }
}

// Keep for compile
template <>
__aicore__ inline void DataCopyGM2UBImpl(
    __ubuf__ void* dst, __gm__ void* src, const DataCopyParams& intriParams, const uint8_t cacheMode)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "DataCopyGM2UBImpl not support type void*"); });
}

template <typename T>
__aicore__ inline void DataCopyGM2L1Impl(
    __cbuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams, const uint8_t cacheMode = 0)
{
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(src, workSpace, true, false, intriParams);
    }
    if ASCEND_IS_AIC {
        if constexpr (SupportType<T, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
            CopyGmToCbufAlignV2(
                (__cbuf__ uint8_t*)dst, (__gm__ uint8_t*)src, intriParams.blockCount, intriParams.blockLen, 0, 0,
                intriParams.srcStride, intriParams.dstStride, false, cacheMode);
        } else {
            CopyGmToCbufAlignV2(
                dst, src, intriParams.blockCount, intriParams.blockLen, 0, 0, intriParams.srcStride,
                intriParams.dstStride, false, cacheMode);
        }
    } else if ASCEND_IS_AIV { // Add for TSCM AIV: just send the message to aic;
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        uint8_t* tscmCpuBaseAddr = GetTPipePtr()->GetBaseAddr(int8_t(TPosition::TSCM));
        uint64_t l1Addr = (uint8_t*)dst - tscmCpuBaseAddr;
        ScmDataCopyMsg((__cbuf__ void*)l1Addr, (__gm__ void*)src, intriParams, -1);
#else
        // scm data copy will call kfc msg, must have kfc server
        // this macro is used to infer pure cube channel op
#ifndef __NO_KFC_SERVER__
        ScmDataCopyMsg((__cbuf__ void*)dst, (__gm__ void*)src, intriParams, -1);
#endif
#endif
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

        uint16_t loop2DstStride = intriParams.dstNzNStride;  // loop2_dst_stride = dst_nz_n_stride
        uint16_t loop3DstStride = intriParams.dstNzC0Stride; // loop3_dst_stride = dst_nz_c0_Stride
        // loop4_dst_stride: dst_nz_matrix_stride * size_of_dst_type / C0_size
        uint16_t loop4DstStride = static_cast<uint16_t>(intriParams.dstNzMatrixStride * sizeof(T) / ONE_BLK_SIZE);

        uint64_t mte2NzPara = static_cast<uint64_t>(loop4DstStride) << 48; // MTE2_NZ_PARA[63:48]
        mte2NzPara |= static_cast<uint64_t>(loop3DstStride) << 32;         // MTE2_NZ_PARA[47:32]
        mte2NzPara |= static_cast<uint64_t>(loop2DstStride) << 16;         // MTE2_NZ_PARA[31:16]
        mte2NzPara |= static_cast<uint64_t>(intriParams.ndNum);            // MTE2_NZ_PARA[15:0]
        set_mte2_nz_para(mte2NzPara); // CCE: store parameters for ND2NZ DMA instructions

        // input params: srcDValue, srcNdMatrixStride                        unit of element nums
        // expected params for ISA: loop1_src_stride, loop4_src_stride       uint of bytes
        // loop1SrcStride = srcD * sizeof(srcType)     loop4SrcStride = srcNdMatrixStride * sizeof(srcType)
        uint64_t loop1SrcStride = intriParams.srcDValue * sizeof(T);
        uint64_t loop4SrcStride = intriParams.srcNdMatrixStride * sizeof(T);
        if constexpr (sizeof(T) == B8_BYTE_SIZE) {
            copy_gm_to_cbuf_multi_nd2nz(
                (__cbuf__ int8_t*)dst, (__gm__ int8_t*)src, 0, loop1SrcStride, cacheMode, intriParams.nValue,
                intriParams.dValue, loop4SrcStride, enableSmallC0);
        }
        if constexpr (sizeof(T) == B16_BYTE_SIZE) {
            copy_gm_to_cbuf_multi_nd2nz(
                (__cbuf__ half*)dst, (__gm__ half*)src, 0, loop1SrcStride, cacheMode, intriParams.nValue,
                intriParams.dValue, loop4SrcStride, enableSmallC0);
        }
        if constexpr (sizeof(T) == B32_BYTE_SIZE) {
            copy_gm_to_cbuf_multi_nd2nz(
                (__cbuf__ float*)dst, (__gm__ float*)src, 0, loop1SrcStride, cacheMode, intriParams.nValue,
                intriParams.dValue, loop4SrcStride, enableSmallC0);
        }
    } else if ASCEND_IS_AIV { // Add for TSCM: aiv just send the message
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
        uint8_t* tscmCpuBaseAddr = GetTPipePtr()->GetBaseAddr(int8_t(TPosition::TSCM));
        uint64_t l1Addr = (uint8_t*)dst - tscmCpuBaseAddr;
        ScmDataCopyND2NZMsg((__cbuf__ void*)l1Addr, (__gm__ void*)src, sizeof(T), intriParams, -1);
#else
        ScmDataCopyND2NZMsg(dst, src, sizeof(T), intriParams, -1);
#endif
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
    if constexpr (SupportType<T, fp4x2_e2m1_t, fp4x2_e1m2_t>()) {
        // data is transferred based on the b8 type, and parameters are set based on the b8 type.
        DataCopyGM2L1ND2NZImplBase<uint8_t, enableSmallC0>(
            (__cbuf__ uint8_t*)dst, (__gm__ uint8_t*)src, intriParams, cacheMode);
    } else {
        DataCopyGM2L1ND2NZImplBase<T, enableSmallC0>((__cbuf__ T*)dst, (__gm__ T*)src, intriParams, cacheMode);
    }
}

template <typename T>
__aicore__ inline void DataCopyGM2L1DN2NZImplBase(
    __cbuf__ T* dst, __gm__ T* src, const Dn2NzParams& intriParams, bool enableSmallC0, const uint8_t cacheMode = 0)
{
    if ASCEND_IS_AIC {
        SmallC0Checker(enableSmallC0, intriParams.dValue);

        if constexpr (g_gm_overflow_check) {
            __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
            AscendCUtils::CheckGmMemOverflowDn2Nz(src, workSpace, true, intriParams);
        }

        uint16_t loop2DstStride = intriParams.dstNzNStride;  // loop2_dst_stride = dst_nz_n_stride
        uint16_t loop3DstStride = intriParams.dstNzC0Stride; // loop3_dst_stride = dst_nz_c0_Stride
        // loop4_dst_stride: dst_nz_matrix_stride * size_of_dst_type / C0_size
        uint16_t loop4DstStride = static_cast<uint16_t>(intriParams.dstNzMatrixStride * sizeof(T) / ONE_BLK_SIZE);

        uint64_t mte2NzPara = static_cast<uint64_t>(loop4DstStride) << 48; // MTE2_NZ_PARA[63:48]
        mte2NzPara |= static_cast<uint64_t>(loop3DstStride) << 32;         // MTE2_NZ_PARA[47:32]
        mte2NzPara |= static_cast<uint64_t>(loop2DstStride) << 16;         // MTE2_NZ_PARA[31:16]
        mte2NzPara |= static_cast<uint64_t>(intriParams.dnNum);            // MTE2_NZ_PARA[15:0]
        set_mte2_nz_para(mte2NzPara); // CCE: store parameters for DN2NZ DMA instructions

        // input params: srcDValue, srcDnMatrixStride                        unit of element nums
        // expected params for ISA: loop1_src_stride, loop4_src_stride       uint of bytes
        // loop1SrcStride = srcD * sizeof(srcType)     loop4SrcStride = srcDnMatrixStride * sizeof(srcType)
        uint64_t loop1SrcStride = intriParams.srcDValue * sizeof(T);
        uint64_t loop4SrcStride = intriParams.srcDnMatrixStride * sizeof(T);

        copy_gm_to_cbuf_multi_dn2nz(
            (__cbuf__ T*)dst, (__gm__ T*)src, 0, loop1SrcStride, cacheMode, intriParams.nValue, intriParams.dValue,
            loop4SrcStride, enableSmallC0);
    }
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
            (dstNzNStride - static_cast<uint16_t>(DEFAULT_C0_SIZE)) / static_cast<uint16_t>(DEFAULT_C0_SIZE)),
        0};
    DataCopyPadExtParams<T> padParams;
    for (int repeatTime = 0; repeatTime < repeatCount; ++repeatTime) {
        DataCopyPadGm2UBImpl(
            (__ubuf__ T*)(dst + dstCopyStartOffset + repeatTime * maxC0Count),
            (__gm__ T*)(src + srcCopyStartOffset + repeatTime * maxdValue), copyParams, padParams, cacheMode);
    }
    copyParams.blockCount = repeatTail;
    if (repeatTail != 0) {
        int dstOffset = (dstCopyStartOffset + repeatCount * MAX_REPEAT_TIMES * c0Count);
        int srcOffset = (srcCopyStartOffset + repeatCount * MAX_REPEAT_TIMES * dValue);
        DataCopyPadGm2UBImpl(
            (__ubuf__ T*)(dst + dstOffset), (__gm__ T*)(src + srcOffset), copyParams, padParams, cacheMode);
    }
}

template <typename T>
__aicore__ inline void DataCopyGM2UBND2NZImpl(
    __ubuf__ T* dst, __gm__ T* src, const Nd2NzParams& intriParams, const uint8_t cacheMode = 0)
{
    if ASCEND_IS_AIV {
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
}

__aicore__ inline void SetLoopModeOutParaImpl(const LoopModeParams& loopParams)
{
    ASCENDC_ASSERT((loopParams.loop2Size < (1ul << 21)), {
        KERNEL_LOG(KERNEL_ERROR, "loopParams.loop2Size is too big and needs to be less than 2^21");
    });
    ASCENDC_ASSERT((loopParams.loop1Size < (1ul << 21)), {
        KERNEL_LOG(KERNEL_ERROR, "loopParams.loop1Size is too big and needs to be less than 2^21");
    });
    ASCENDC_ASSERT((loopParams.loop1DstStride < (1ul << 21)), {
        KERNEL_LOG(KERNEL_ERROR, "loopParams.loop1DstStride is too big and needs to be less than 2^21");
    });
    ASCENDC_ASSERT((loopParams.loop2DstStride < (1ul << 21)), {
        KERNEL_LOG(KERNEL_ERROR, "loopParams.loop2DstStride is too big and needs to be less than 2^21");
    });
    ASCENDC_ASSERT((loopParams.loop1SrcStride < (1ul << 40)), {
        KERNEL_LOG(KERNEL_ERROR, "loopParams.loop1SrcStride is too big and needs to be less than 2^40");
    });
    ASCENDC_ASSERT((loopParams.loop2SrcStride < (1ul << 40)), {
        KERNEL_LOG(KERNEL_ERROR, "loopParams.loop2SrcStride is too big and needs to be less than 2^40");
    });
    uint64_t loopSizePara = static_cast<uint64_t>(loopParams.loop2Size) << 21; // LOOP_SIZE_OUTTOUB[42:21]
    loopSizePara |= static_cast<uint64_t>(loopParams.loop1Size);               // LOOP_SIZE_OUTTOUB[20:0]

    // LOOP1_STRIDE_OUTTOUB[60:40], must be 32B aligned
    ASCENDC_ASSERT((loopParams.loop1DstStride % GetDataBlockSizeInBytes() == 0), {
        KERNEL_LOG(KERNEL_ERROR, "loop1DstStride must be 32B aligned");
    });
    uint64_t loop1StridePara = (loopParams.loop1DstStride) << 40; // loop1DstStride must be 32B aligned
    loop1StridePara |= loopParams.loop1SrcStride;                 // LOOP1_STRIDE_OUTTOUB[39:0]
    // LOOP2_STRIDE_OUTTOUB[60:40], must be 32B aligned
    ASCENDC_ASSERT((loopParams.loop2DstStride % GetDataBlockSizeInBytes() == 0), {
        KERNEL_LOG(KERNEL_ERROR, "loop2DstStride must be 32B aligned");
    });
    uint64_t loop2StridePara = (loopParams.loop2DstStride) << 40; // loop2DstStride must be 32B aligned
    loop2StridePara |= loopParams.loop2SrcStride;                 // LOOP2_STRIDE_OUTTOUB[39:0]

    set_loop_size_outtoub(loopSizePara);
    set_loop1_stride_outtoub(loop1StridePara);
    set_loop2_stride_outtoub(loop2StridePara);
}

__aicore__ inline void SetLoopModeUBParaImpl(const LoopModeParams& loopParams)
{
    ASCENDC_ASSERT((loopParams.loop2Size < (1ul << 21)), {
        KERNEL_LOG(KERNEL_ERROR, "loopParams.loop2Size is too big and needs to be less than 2^21");
    });
    ASCENDC_ASSERT((loopParams.loop1Size < (1ul << 21)), {
        KERNEL_LOG(KERNEL_ERROR, "loopParams.loop1Size is too big and needs to be less than 2^21");
    });
    ASCENDC_ASSERT((loopParams.loop1SrcStride < (1ul << 21)), {
        KERNEL_LOG(KERNEL_ERROR, "loopParams.loop1SrcStride is too big and needs to be less than 2^21");
    });
    ASCENDC_ASSERT((loopParams.loop2SrcStride < (1ul << 21)), {
        KERNEL_LOG(KERNEL_ERROR, "loopParams.loop2SrcStride is too big and needs to be less than 2^21");
    });
    ASCENDC_ASSERT((loopParams.loop1DstStride < (1ul << 40)), {
        KERNEL_LOG(KERNEL_ERROR, "loopParams.loop1DstStride is too big and needs to be less than 2^40");
    });
    ASCENDC_ASSERT((loopParams.loop2DstStride < (1ul << 40)), {
        KERNEL_LOG(KERNEL_ERROR, "loopParams.loop2DstStride is too big and needs to be less than 2^40");
    });
    uint64_t loopSizePara = static_cast<uint64_t>(loopParams.loop2Size) << 21; // LOOP_SIZE_UBTOOUT[42:21]
    loopSizePara |= static_cast<uint64_t>(loopParams.loop1Size);               // LOOP_SIZE_UBTOOUT[20:0]

    // LOOP1_STRIDE_UBTOOUT[60:40], must be 32B aligned
    ASCENDC_ASSERT((loopParams.loop1SrcStride % GetDataBlockSizeInBytes() == 0), {
        KERNEL_LOG(KERNEL_ERROR, "loop1SrcStride must be 32B aligned");
    });
    uint64_t loop1StridePara = (loopParams.loop1SrcStride) << 40; // loop1SrcStride must be 32B aligned
    loop1StridePara |= loopParams.loop1DstStride;                 // LOOP1_STRIDE_UBTOOUT[39:0]
    // LOOP2_STRIDE_UBTOOUT[60:40], must be 32B aligned
    ASCENDC_ASSERT((loopParams.loop2SrcStride % GetDataBlockSizeInBytes() == 0), {
        KERNEL_LOG(KERNEL_ERROR, "loop2SrcStride must be 32B aligned");
    });
    uint64_t loop2StridePara = (loopParams.loop2SrcStride) << 40; // loop2SrcStride must be 32B aligned
    loop2StridePara |= loopParams.loop2DstStride;                 // LOOP2_STRIDE_UBTOOUT[39:0]

    set_loop_size_ubtoout(loopSizePara);
    set_loop1_stride_ubtoout(loop1StridePara);
    set_loop2_stride_ubtoout(loop2StridePara);
}

__aicore__ inline void ResetUBLoopModeParaImpl(void)
{
    constexpr uint64_t loopSizePara = (1ul << 21) | 1ul; // 设置SPR寄存器，硬件约定置1为normal

    set_loop_size_ubtoout(loopSizePara);
}

__aicore__ inline void ResetOutLoopModeParaImpl(void)
{
    constexpr uint64_t loopSizePara = (1ul << 21) | 1ul; // 设置SPR寄存器，硬件约定置1为normal

    set_loop_size_outtoub(loopSizePara);
}

template <typename T>
__aicore__ inline void DataCopyUB2GMImpl(
    __gm__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams, const uint8_t cacheMode = 0)
{
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(dst, workSpace, false, false, intriParams);
    }
    CopyUbufToGmAlignV2(
        dst, src, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, false,
        cacheMode);
}

template <typename T>
__aicore__ inline void DataCopyUB2UBImpl(__ubuf__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
    CopyUbufToUbuf(
        dst, src, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride, intriParams.dstStride);
}

template <TPosition pos>
__aicore__ inline uint64_t TransUBAddr(uint64_t addr);

template <typename T>
__aicore__ inline __in_pipe__(MTE3)
    __out_pipe__(MTE3) void DataCopyUB2L1Impl(__cbuf__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams)
{
#if KFC_C310_SSBUF == 1 || __MIX_CORE_AIC_RATION__ != 1
    CopyUbufToCbuf(
        dst, src, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride, intriParams.dstStride);
#else
    // Add for TSCM AIV: Copy buffer into workspace, and send message to AIC
    ASCENDC_ASSERT((GetTPipePtr() != nullptr), { KERNEL_LOG(KERNEL_ERROR, "tpipe ptr can not be nullptr"); });
    ASCENDC_ASSERT((dst != nullptr), { KERNEL_LOG(KERNEL_ERROR, "dst ptr can not be nullptr"); });
    ASCENDC_ASSERT((src != nullptr), { KERNEL_LOG(KERNEL_ERROR, "src ptr can not be nullptr"); });
    if ASCEND_IS_AIV {
        ASCENDC_ASSERT((GetKfcClient() != nullptr), { KERNEL_LOG(KERNEL_ERROR, "kfc client ptr can not be nullptr"); });
        ASCENDC_DEBUG_ASSERT(
            (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(src)) % ONE_BLK_SIZE == 0),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "src address should be 32B aligned \n"));
        // 1.get GM addr
        uint32_t tensorSize = intriParams.blockCount * intriParams.blockLen * 32;
        int32_t ubAddr = -1;
#if ASCENDC_CPU_DEBUG
        uint64_t absSrc = (uint8_t*)src - (uint8_t*)(GetTPipePtr()->GetBaseAddr((int8_t)TPosition::VECIN));
        ASCENDC_ASSERT((static_cast<uint64_t>(absSrc) >= 0 && static_cast<uint64_t>(absSrc) < TOTAL_UB_SIZE), {
            KERNEL_LOG(KERNEL_ERROR, "abs src is 0x%lx, which should be in range of [0, %u)", absSrc, TOTAL_UB_SIZE);
        });
        GM_ADDR gmAddr = (GetKfcClient()->ubStart) + static_cast<uint64_t>(absSrc);
#else
        GM_ADDR gmAddr = (GetKfcClient()->AllocUB(tensorSize, ubAddr));
#endif
        event_t eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
        SetFlag<HardEvent::V_MTE3>(eventID);
        WaitFlag<HardEvent::V_MTE3>(eventID);
        // 2.copy ub->GM
        if constexpr (g_gm_overflow_check) {
            __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
            AscendCUtils::CheckGmMemOverflowNormal(gmAddr, workSpace, false, false, intriParams);
        }
        CopyUbufToGmAlignV2(
            (__gm__ T*)gmAddr, src, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride,
            intriParams.dstStride, false, 0);

        // 3.send message, copy GM to L1
#if ASCENDC_CPU_DEBUG
        uint8_t* aicBaseTSCMAddr = GetTPipePtr()->GetBaseAddr(int8_t(TPosition::TSCM));
        ASCENDC_ASSERT(
            (aicBaseTSCMAddr != nullptr), { KERNEL_LOG(KERNEL_ERROR, "aicBaseTSCMAddr can not be nullptr"); });
        uint64_t l1AddrDst = (uint8_t*)dst - (uint8_t*)aicBaseTSCMAddr;
        ASCENDC_ASSERT((l1AddrDst < TOTAL_L1_SIZE), {
            KERNEL_LOG(
                KERNEL_ERROR, "l1AddrDst is 0x%lx, which should be in range of [0, %u)", l1AddrDst, TOTAL_L1_SIZE);
        });
        ScmDataCopyMsg((__cbuf__ void*)l1AddrDst, (__gm__ void*)gmAddr, intriParams, ubAddr);
#else
        ScmDataCopyMsg((__cbuf__ void*)dst, (__gm__ void*)gmAddr, intriParams, ubAddr);
#endif
    }
#endif
}

template <typename T>
__aicore__ inline void DataCopyUB2GMNZ2NDImplBase(
    __gm__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t high, uint16_t width, uint16_t srcNStride, uint16_t dstDStride,
    const uint8_t cacheMode = 0)
{
    const uint16_t highBlock = MAX_REPEAT_TIMES;
    const uint16_t highBlocks = high / highBlock;
    const uint16_t highTail = high % highBlock;
    const uint16_t widthFractal = (width + BLOCK_CUBE - 1) / BLOCK_CUBE;

    for (int i = 0; i < widthFractal; ++i) {
        uint16_t computeCount = (i + 1) * BLOCK_CUBE;
        uint16_t leftLen = width >= computeCount ? BLOCK_CUBE : (width - i * BLOCK_CUBE);
        uint16_t srcLeftLen = (sizeof(T) == B32_BYTE_SIZE && leftLen <= DEFAULT_BLK_NUM) ? MIN_BLOCK_LEN : 0;
        for (int j = 0; j < highBlocks; ++j) {
            DataCopyPadUB2GMImpl(
                dstAddr + i * BLOCK_CUBE + j * highBlock * dstDStride,
                srcAddr + i * srcNStride * BLOCK_CUBE + j * highBlock * BLOCK_CUBE,
                DataCopyExtParams{
                    highBlock, static_cast<uint32_t>(leftLen * sizeof(T)), srcLeftLen,
                    static_cast<uint32_t>((dstDStride - leftLen) * sizeof(T)), 0});
        }
        if (highTail) {
            DataCopyPadUB2GMImpl(
                dstAddr + i * BLOCK_CUBE + highBlocks * highBlock * dstDStride,
                srcAddr + i * srcNStride * BLOCK_CUBE + highBlocks * highBlock * BLOCK_CUBE,
                DataCopyExtParams{
                    highTail, static_cast<uint32_t>(leftLen * sizeof(T)), srcLeftLen,
                    static_cast<uint32_t>((dstDStride - leftLen) * sizeof(T)), 0});
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
__aicore__ inline void TransND2NZ(__ubuf__ T* dstAddr, __ubuf__ T* srcAddr, uint16_t high, uint16_t width, T scalar)
{
    struct UnaryRepeatParams intriParams;
    intriParams.srcBlkStride = 1;
    intriParams.dstBlkStride = 1;
    intriParams.srcRepStride = width * sizeof(T) / ONE_BLK_SIZE;
    intriParams.dstRepStride = 1;

    uint32_t highBlock = MAX_REPEAT_TIMES;
    uint32_t highBlocks = high / highBlock;
    uint32_t highTail = high % highBlock;

    constexpr uint16_t elePerBlock = ONE_BLK_SIZE / sizeof(T);

    uint64_t mask[2] = {(1 << elePerBlock) - 1, 0};

    uint32_t widthFractal = width * sizeof(T) / ONE_BLK_SIZE;
    for (uint32_t i = 0; i < widthFractal; ++i) {
        for (uint32_t j = 0; j < highBlocks; ++j) {
            AddsImpl(
                dstAddr + i * elePerBlock * high + j * highBlock * elePerBlock,
                srcAddr + i * elePerBlock + j * highBlock * width, scalar, mask, highBlock, intriParams);
        }
        if (highTail != 0) {
            AddsImpl(
                dstAddr + i * elePerBlock * high + highBlocks * highBlock * elePerBlock,
                srcAddr + i * elePerBlock + highBlocks * highBlock * width, scalar, mask, highTail, intriParams);
        }
    }
}

template <typename T>
__aicore__ inline void DataCopyUB2L1ND2NZImpl(__cbuf__ T* dst, __ubuf__ T* src, const Nd2NzParams& intriParams)
{
#if KFC_C310_SSBUF == 1 || __MIX_CORE_AIC_RATION__ != 1
    ASCENDC_ASSERT((GetTPipePtr() != nullptr), { KERNEL_LOG(KERNEL_ERROR, "tpipe ptr can not be nullptr"); });
    ASCENDC_ASSERT((dst != nullptr), { KERNEL_LOG(KERNEL_ERROR, "dst ptr can not be nullptr"); });
    ASCENDC_ASSERT((src != nullptr), { KERNEL_LOG(KERNEL_ERROR, "src ptr can not be nullptr"); });
    ASCENDC_ASSERT((intriParams.dValue * sizeof(T) % ONE_BLK_SIZE == 0), {
        KERNEL_LOG(KERNEL_ERROR, "dValue must be 32B aligned");
    });
    ASCENDC_ASSERT((intriParams.srcDValue * sizeof(T) % ONE_BLK_SIZE == 0), {
        KERNEL_LOG(KERNEL_ERROR, "srcDValue must be 32B aligned");
    });
    ASCENDC_ASSERT((intriParams.nValue > 0 && intriParams.dValue > 0 && intriParams.srcDValue > 0), {
        KERNEL_LOG(KERNEL_ERROR, "nvalue, dValue and srcDValue must be greater than 0");
    });
    ASCENDC_ASSERT((intriParams.ndNum == 1), {
        KERNEL_LOG(KERNEL_ERROR, "intriParams.ndNum is %hu, which can only be 1", intriParams.ndNum);
    });
    if ASCEND_IS_AIV {
        LocalTensor<T> popBuffer;
        const bool ret = PopStackBuffer<T, TPosition::LCM>(popBuffer);
        ASCENDC_ASSERT((ret), { KERNEL_LOG(KERNEL_ERROR, "src ptr can not be nullptr"); });
        const uint32_t bufferSize = popBuffer.GetSize() * sizeof(T);
        const uint32_t actualSize = ((intriParams.dValue * sizeof(T) / ONE_BLK_SIZE - 1) * intriParams.dstNzC0Stride +
                                     (intriParams.nValue - 1) * intriParams.dstNzNStride + 1) *
                                    ONE_BLK_SIZE;
        ASCENDC_ASSERT((bufferSize >= actualSize), { KERNEL_LOG(KERNEL_ERROR, "src ptr can not be nullptr"); });

        const uint32_t rows = intriParams.nValue;
        const uint32_t cols = AlignUp(intriParams.dValue * sizeof(T), ONE_BLK_SIZE) / sizeof(T);
        uint16_t alignedDValueBlockNum = (intriParams.dValue * sizeof(T) - 1) / ONE_BLK_SIZE + 1;
        uint16_t alignedDValue = cols * ONE_BLK_SIZE / sizeof(T);
        TransND2NZ((__ubuf__ T*)popBuffer.GetPhyAddr(), src, rows, cols, static_cast<T>(0));
        auto eventID = GetTPipePtr()->FetchEventID(HardEvent::V_MTE3);
        SetFlag<HardEvent::V_MTE3>(eventID);
        WaitFlag<HardEvent::V_MTE3>(eventID);
        CopyUbufToCbuf(dst, (__ubuf__ T*)popBuffer.GetPhyAddr(), 1, actualSize / ONE_BLK_SIZE, 0, 0);
    }
#else
    // Add for TSCM AIV: Copy buffer into workspace, and send message to AIC
    ASCENDC_ASSERT((GetTPipePtr() != nullptr), { KERNEL_LOG(KERNEL_ERROR, "tpipe ptr can not be nullptr"); });
    ASCENDC_ASSERT((dst != nullptr), { KERNEL_LOG(KERNEL_ERROR, "dst ptr can not be nullptr"); });
    ASCENDC_ASSERT((src != nullptr), { KERNEL_LOG(KERNEL_ERROR, "src ptr can not be nullptr"); });
    if ASCEND_IS_AIV {
        ASSERT(GetKfcClient() != nullptr);
        ASCENDC_DEBUG_ASSERT(
            (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(src)) % ONE_BLK_SIZE == 0),
            KERNEL_LOG_INTERNAL(KERNEL_ERROR, "src address should be 32B aligned \n"));
        uint32_t tensorSize = intriParams.nValue * intriParams.dValue;
        int32_t ubAddr = -1;
        // 1. get GM addr, copy corresponding data to GM, ND->ND
#if ASCENDC_CPU_DEBUG
        uint64_t absUbAddr = (uint8_t*)src - (uint8_t*)(GetTPipePtr()->GetBaseAddr((int8_t)TPosition::VECIN));
        uint64_t absL1Addr = (uint8_t*)dst - (uint8_t*)(GetTPipePtr()->GetBaseAddr((int8_t)TPosition::TSCM));
        ASCENDC_ASSERT((absL1Addr % ONE_BLK_SIZE == 0), {
            KERNEL_LOG(
                KERNEL_ERROR, "Failed to check dst tensor address "
                              "alignment in DataCopy with Nd2NzParams from UB(VECIN/VECOUT) to L1 Buffer(TSCM), it "
                              "should be 32B aligned");
        });
        ASCENDC_ASSERT((intriParams.ndNum == 1), {
            KERNEL_LOG(KERNEL_ERROR, "intriParams.ndNum is %hu, which can only be 1", intriParams.ndNum);
        });
        GM_ADDR gmAddr = (GetKfcClient()->AllocUB(tensorSize, ubAddr));
#else
        GM_ADDR gmAddr = (GetKfcClient()->AllocUB(tensorSize, ubAddr));
#endif
        event_t eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
        SetFlag<HardEvent::V_MTE3>(eventID);
        WaitFlag<HardEvent::V_MTE3>(eventID);
        // 2. copy ub->GM
        CopyUbufToGmAlignV2((__gm__ T*)gmAddr, src, 0, 1, tensorSize * sizeof(T) / 32, 0, false, 0);

        // 3. send message, copy GM to L1
#if ASCENDC_CPU_DEBUG
        ScmDataCopyND2NZMsg((__cbuf__ void*)absL1Addr, (__gm__ void*)gmAddr, sizeof(T), intriParams, ubAddr);
#else
        ScmDataCopyND2NZMsg((__cbuf__ void*)dst, (__gm__ void*)gmAddr, sizeof(T), intriParams, ubAddr);
#endif
    }
#endif
}

// Ascend950 MTE3 UB->L1, but it requires the srcDValue must be 32B aligned
template <typename T>
__aicore__ inline void DataCopyUB2L1ND2NZImplV2(__cbuf__ T* dst, __ubuf__ T* src, const Nd2NzParams& intriParams)
{
    uint32_t rows = intriParams.nValue;
    uint32_t cols = AlignUp(intriParams.dValue * sizeof(T), ONE_BLK_SIZE) / ONE_BLK_SIZE;
    // by row
    if (cols > rows) {
        for (uint32_t i = 0; i < rows; i++) {
            CopyUbufToCbuf(
                dst + i * intriParams.dstNzNStride * ONE_BLK_SIZE / sizeof(T), src + i * intriParams.srcDValue,
                intriParams.dValue * sizeof(T) / ONE_BLK_SIZE, 1, 0, intriParams.dstNzC0Stride - 1);
        }
    } else {
        // by column
        for (uint32_t i = 0; i < cols; i++) {
            CopyUbufToCbuf(
                dst + i * intriParams.dstNzC0Stride * ONE_BLK_SIZE / sizeof(T), src + i * ONE_BLK_SIZE / sizeof(T),
                rows, 1, intriParams.srcDValue * sizeof(T) / ONE_BLK_SIZE - 1, intriParams.dstNzNStride - 1);
        }
    }
    PipeBarrierInternal<PIPE_MTE3>();
}

template <typename T, typename U>
__aicore__ inline void DataCopyUB2L0CImpl(
    __cc__ T* dst, __ubuf__ U* src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported data copy from ubuf to L0C on current device"); });
}

// SRC: L1
template <typename T, uint8_t subBlockId = 0>
__aicore__ inline void DataCopyL12UBImpl(__ubuf__ T* dst, __cbuf__ T* src, const DataCopyParams& intriParams)
{
#if __MIX_CORE_AIC_RATION__ != 1
    static_assert(subBlockId < 2, "subBlockId must be less than 2 when AIC:AIV ratio is 1:2");
    CopyCbufToUbuf<T, subBlockId>(
        dst, src, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride, intriParams.dstStride);
#else
    ASCENDC_ASSERT((false), {
        KERNEL_LOG(
            KERNEL_ERROR, "unsupported data copy from L1 Buffer(A1/B1) to UB(VECIN/VECOUT) on current device when "
                          "AIC:AIV ratio is 1:1");
    });
#endif
}

template <typename T>
__aicore__ inline void DataCopyL12GMImpl(
    __gm__ T* dst, __cbuf__ T* src, const DataCopyParams& intriParams, const uint8_t cacheMode = 0)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported data copy from cbuf to GM on current device"); });
}

template <typename T>
__aicore__ inline __in_pipe__(MTE1) __out_pipe__(MTE1) void DataCopyL12BTImpl(
    const uint64_t dst, __cbuf__ T* src, const uint16_t isEnableConv, const DataCopyParams& intriParams)
{
    CopyCbufToBt(
        dst, src, isEnableConv, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride,
        intriParams.dstStride);
}

template <typename T, typename U>
__aicore__ inline void DataCopyL12L0CImpl(
    __cc__ T* dst, __cbuf__ U* src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported data copy from cbuf to L0C on current device"); });
}

// SRC: L0C
template <typename T, typename U>
__aicore__ inline void DataCopyL0C2UBImpl(
    __ubuf__ T* dst, __cc__ U* src, const DataCopyParams& intriParams, const DataCopyEnhancedParams& enhancedParams)
{
    ASCENDC_ASSERT((false), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "unsupported data copy from l0c to ubuf on current device, please use FixPipe function instead");
    });
}

/* **************************************************************************************************
 * Copy                                             *
 * ************************************************************************************************* */
namespace Internal {
template <bool isSetMask, bool isMaskBitMode, bool isNormalMode, typename T>
__simd_vf__ inline void VecCopyLevel0VFImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const BasicAPIMaskStruct maskArrayStruct, const uint64_t maskCount,
    const uint8_t repeatTime, const CopyRepeatParams repeatParams, __ubuf__ uint64_t* maskBuf)
{
    uint32_t count =
        VecMicroGetCount<isSetMask, isNormalMode, isMaskBitMode>(maskArrayStruct.maskArray, maskCount, maskBuf);
    uint16_t newRepeatTimes = 0;
    newRepeatTimes = VecMicroGetRepeatTimes<T, isNormalMode>(count, repeatTime);
    Reg::MaskReg maskReg;
    constexpr uint8_t ElePerBlkT = GetDataBlockSizeInBytes() / sizeof(T);
    if constexpr (isNormalMode) {
        maskReg = VecMicroGetMaskReg<T, isSetMask, isNormalMode, isMaskBitMode>(maskBuf, count);
        for (uint16_t index = 0; index < newRepeatTimes; ++index) {
            Reg::RegTensor<T> srcVreg;
#ifndef NO_OVERLAP_IN_MULTI_REPEAT
            Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
#endif
            Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                srcVreg, src + index * repeatParams.srcRepeatSize * ElePerBlkT, repeatParams.srcStride, maskReg);
            Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY>(
                dst + index * repeatParams.dstRepeatSize * ElePerBlkT, srcVreg, repeatParams.dstStride, maskReg);
        }
    } else {
        Reg::RegTensor<T> srcReg;
        Reg::MaskReg maskReg;
        uint32_t oneRepeatBlock = (count * sizeof(T) + GetDataBlockSizeInBytes() - 1) / GetDataBlockSizeInBytes();
        uint32_t oneBlockElm = GetVecLen() / sizeof(T) / DEFAULT_BLK_NUM;
        uint32_t innerRepeatTimes = (oneRepeatBlock + DEFAULT_BLK_NUM - 1) / DEFAULT_BLK_NUM;
        uint32_t sreg;
        __ubuf__ T* dstTmp = dst;
        __ubuf__ T* srcTmp = src;
        uint32_t srcRepeatStride = repeatParams.srcStride * DEFAULT_BLK_NUM;
        uint32_t dstRepeatStride = repeatParams.dstStride * DEFAULT_BLK_NUM;
        for (uint16_t i = 0; i < static_cast<uint16_t>(repeatTime); ++i) {
            sreg = static_cast<uint32_t>(count);
            for (uint16_t j = 0; j <= static_cast<uint16_t>(innerRepeatTimes); ++j) {
                maskReg = Reg::UpdateMask<T>(sreg);
#ifndef NO_OVERLAP_IN_MULTI_REPEAT
                Reg::LocalMemBar<Reg::MemType::VEC_STORE, Reg::MemType::VEC_LOAD>();
#endif
                Reg::LoadAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                    srcReg, src, repeatParams.srcStride, srcRepeatStride, maskReg);
                Reg::StoreAlign<T, Reg::DataCopyMode::DATA_BLOCK_COPY, Reg::PostLiteral::POST_MODE_UPDATE>(
                    dst, srcReg, repeatParams.dstStride, dstRepeatStride, maskReg);
            }
            src = srcTmp + repeatParams.srcRepeatSize * oneBlockElm;
            dst = dstTmp + repeatParams.dstRepeatSize * oneBlockElm;
            srcTmp = src;
            dstTmp = dst;
        }
    }
}

template <bool isSetMask, bool isMaskBitMode, typename T>
__aicore__ inline void VecCopyLevel0Template(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t maskArray[], const uint64_t maskCount, const uint8_t repeatTime,
    const CopyRepeatParams& repeatParams)
{
    BasicAPIMaskStruct maskArrayStruct;
    if constexpr (isMaskBitMode) {
        ASCENDC_ASSERT(
            maskCount == 0, { KERNEL_LOG(KERNEL_ERROR, "maskCount must be 0 when isMaskBitMode is true."); });
        maskArrayStruct = *(reinterpret_cast<const BasicAPIMaskStruct*>(maskArray));
    } else {
        ASCENDC_ASSERT(maskArray == nullptr, {
            KERNEL_LOG(KERNEL_ERROR, "maskArray must be nullptr when isMaskBitMode is false.");
        });
    }
    __ubuf__ uint64_t* maskBuf = nullptr;

    if (Internal::IsCounterMode()) {
        if constexpr (!isSetMask) {
            maskBuf =
                AscendCUtils::GetTemporaryBufferAddr<uint64_t>(GetRuntimeUBSize(), 2); // maskReg 256bit PK-> 128bit
        }
        VecCopyLevel0VFImpl<isSetMask, isMaskBitMode, false, T>(
            dst, src, maskArrayStruct, maskCount, repeatTime, repeatParams, maskBuf);
        if constexpr (!isSetMask) {
            AscendCUtils::FreeTemporaryBuffer<uint64_t>(maskBuf);
        }
    } else {
        if constexpr (isMaskBitMode && isSetMask) {
            SetVectorMask<T>(maskArray[1], maskArray[0]); // set mask to SPR.MASK, movp in VF
        }
        VecCopyLevel0VFImpl<isSetMask, isMaskBitMode, true, T>(
            dst, src, maskArrayStruct, maskCount, repeatTime, repeatParams, maskBuf);
    }
}
} // namespace Internal
// Copy::Level 0 - mask bit mode
template <typename T, bool isSetMask = true>
__aicore__ inline void CopyImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask[], const uint8_t repeatTime,
    const CopyRepeatParams& repeatParams)
{
    static_assert(SupportBytes<T, 2, 4>(), "Copy from ubuf to ubuf only support type b16/b32 on current device");
    Internal::VecCopyLevel0Template<isSetMask, true>(dst, src, mask, 0, repeatTime, repeatParams);
}

// Copy::Level 0 - mask count mode
template <typename T, bool isSetMask = true>
__aicore__ inline void CopyImpl(
    __ubuf__ T* dst, __ubuf__ T* src, const uint64_t mask, const uint8_t repeatTime,
    const CopyRepeatParams& repeatParams)
{
    static_assert(SupportBytes<T, 2, 4>(), "Copy from ubuf to ubuf only support type b16/b32 on current device");
    Internal::VecCopyLevel0Template<isSetMask, false>(dst, src, nullptr, mask, repeatTime, repeatParams);
}

// Copy::Level 2
template <typename T>
__simd_vf__ inline void CopyImpl(__ubuf__ T* dst, __ubuf__ T* src, const uint32_t calCount)
{
    static_assert(
        SupportBytes<T, 1, 2, 4, 8>(), "Copy from ubuf to ubuf only support type b8/b16/b32/b64 on current device");
    if constexpr (sizeof(T) == B8_BYTE_SIZE || sizeof(T) == B16_BYTE_SIZE || sizeof(T) == B32_BYTE_SIZE) {
        constexpr uint32_t repeatElm = GetVecLen() / sizeof(T);
        uint32_t sreg = calCount;
        uint16_t repeatTime = CeilDivision(calCount, repeatElm);
        Reg::RegTensor<T> vreg;
        Reg::MaskReg preg;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            preg = Reg::UpdateMask<T>(sreg);
            Reg::LoadAlign(vreg, src + i * repeatElm);
            Reg::StoreAlign(dst + i * repeatElm, vreg, preg);
        }
    } else if constexpr (sizeof(T) == B64_BYTE_SIZE) {
        uint32_t sreg = calCount * 2;
        uint32_t oneRepSize = GetVecLen() / sizeof(uint32_t);
        uint16_t repeatTime = static_cast<uint16_t>(CeilDivision(sreg, oneRepSize));
        Reg::MaskReg preg;
        Reg::RegTensor<uint32_t> srcReg;
        for (uint16_t i = 0; i < repeatTime; ++i) {
            preg = Reg::UpdateMask<uint32_t>(sreg);
            Reg::LoadAlign(srcReg, (__ubuf__ uint32_t*)src + i * oneRepSize);
            Reg::StoreAlign((__ubuf__ uint32_t*)dst + i * oneRepSize, srcReg, preg);
        }
    }
}

/* **************************************************************************************************
 * DataCopySlice                                             *
 * ************************************************************************************************* */

template <typename T>
__aicore__ inline void DataCopySliceGm2UBImpl(
    __ubuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams, const uint8_t cacheMode = 0)
{
    DataCopyPadExtParams<T> padParams{false, 0, 0, 0};
    // DataCopyParams: unit of 32Bytes(DataCopy) / Bytes(DataCopyPad)
    uint16_t burstLen = intriParams.blockLen * ONE_BLK_SIZE;
    DataCopyExtParams extParams{intriParams.blockCount, burstLen, intriParams.srcStride, intriParams.dstStride, 0};
    DataCopyPadGm2UBImpl(dst, src, extParams, padParams, cacheMode);
}

template <typename T>
__aicore__ inline void DataCopySliceUB2GMImpl(
    __gm__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams, const uint8_t cacheMode = 0)
{
    uint32_t burstLen = intriParams.blockLen * ONE_BLK_SIZE;
    DataCopyExtParams extParams{intriParams.blockCount, burstLen, intriParams.srcStride, intriParams.dstStride, 0};
    DataCopyPadUB2GMImpl(dst, src, extParams, cacheMode);
}

/* **************************************************************************************************
 * DataCopyPad                                             *
 * ************************************************************************************************* */
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

template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline void DataCopyPadGm2UBImpl(
    __ubuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams, const DataCopyPadParams& padParams,
    const uint8_t cacheMode = 0)
{
    if ASCEND_IS_AIC {
        return;
    }
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(src, workSpace, true, true, intriParams);
    }
    if (padParams.isPad == true) {
        set_mov_pad_val(padParams.paddingValue);
    }
    if constexpr (sizeof(T) > B32_BYTE_SIZE) {
        ASCENDC_ASSERT((padParams.paddingValue == 0), {
            KERNEL_LOG(KERNEL_ERROR, "b64 paddingValue on current device only support 0");
        });
    }
    CopyGmToUbufAlignV2<T, mode>(
        dst, src, intriParams.blockCount, intriParams.blockLen, padParams.leftPadding, padParams.rightPadding,
        intriParams.srcStride, intriParams.dstStride, true, cacheMode,
        padParams.isPad); // padding is 0
}

template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline void DataCopyPadGm2UBImpl(
    __ubuf__ T* dst, __gm__ T* src, const DataCopyExtParams& intriParams, const DataCopyPadExtParams<T>& padParams,
    const uint8_t cacheMode = 0)
{
    if ASCEND_IS_AIC {
        return;
    }
    if (padParams.isPad == true) {
        set_mov_pad_val(GetScalarBitcodeValue(padParams.paddingValue));
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
        intriParams.srcStride, intriParams.dstStride, true, cacheMode, padParams.isPad);
}

template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline void DataCopyPadGm2L1Impl(
    __cbuf__ T* dst, __gm__ T* src, const DataCopyParams& intriParams, const DataCopyPadParams& padParams,
    const uint8_t cacheMode = 0)
{
    if ASCEND_IS_AIV {
        return;
    }
    if (padParams.isPad == true) {
        set_mov_pad_val(padParams.paddingValue);
    }
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(src, workSpace, true, true, intriParams);
    }
    if constexpr (sizeof(T) > B32_BYTE_SIZE) {
        ASCENDC_ASSERT((padParams.paddingValue == 0), {
            KERNEL_LOG(KERNEL_ERROR, "DataCopyPad GM to L1 b64 paddingValue on current device only support 0");
        });
    }
    CopyGmToCbufAlignV2<T, mode>(
        dst, src, intriParams.blockCount, intriParams.blockLen, padParams.leftPadding, padParams.rightPadding,
        intriParams.srcStride, intriParams.dstStride, true, cacheMode);
}

template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline void DataCopyPadGm2L1Impl(
    __cbuf__ T* dst, __gm__ T* src, const DataCopyExtParams& intriParams, const DataCopyPadExtParams<T>& padParams,
    const uint8_t cacheMode = 0)
{
    if ASCEND_IS_AIV {
        return;
    }
    if (padParams.isPad == true) {
        set_mov_pad_val(GetScalarBitcodeValue((T)padParams.paddingValue));
    }
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(src, workSpace, true, true, intriParams);
    }
    if constexpr (sizeof(T) > B32_BYTE_SIZE) {
        ASCENDC_ASSERT((padParams.paddingValue == 0), {
            KERNEL_LOG(KERNEL_ERROR, "DataCopyPad GM to L1 b64 paddingValue on current device only support 0");
        });
    }

    CheckSrcGmDataCopyExtParamsRange(intriParams);
    CopyGmToCbufAlignV2<T, mode>(
        dst, src, intriParams.blockCount, intriParams.blockLen, padParams.leftPadding, padParams.rightPadding,
        intriParams.srcStride, intriParams.dstStride, true, cacheMode);
}

// UB -> GM   DataCopyPad with DataCopyParams
template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline void DataCopyPadUB2GMImpl(
    __gm__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams, const uint8_t cacheMode = 0)
{
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(dst, workSpace, false, true, intriParams);
    }
    CopyUbufToGmAlignV2<T, mode>(
        dst, src, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, true,
        cacheMode);
}

// UB -> GM   DataCopyPad with DataCopyExtParams
template <typename T, PaddingMode mode = PaddingMode::Normal>
__aicore__ inline void DataCopyPadUB2GMImpl(
    __gm__ T* dst, __ubuf__ T* src, const DataCopyExtParams& intriParams, const uint8_t cacheMode = 0)
{
    if constexpr (g_gm_overflow_check) {
        __gm__ uint8_t* workSpace = GetSysWorkSpacePtr();
        AscendCUtils::CheckGmMemOverflowNormal(dst, workSpace, false, true, intriParams);
    }
    ASCENDC_ASSERT((intriParams.dstStride <= static_cast<int64_t>(1ul << 40 - 1)), {
        KERNEL_LOG(KERNEL_ERROR, "dstStride is %ld, which should be no more than 2^40-1", intriParams.dstStride);
    });
    ASCENDC_ASSERT((intriParams.dstStride >= static_cast<int64_t>(0)), {
        KERNEL_LOG(KERNEL_ERROR, "dstStride is %ld, which should be no less than 0", intriParams.dstStride);
    });
    ASCENDC_ASSERT((intriParams.srcStride <= static_cast<int64_t>(65535)), {
        KERNEL_LOG(KERNEL_ERROR, "srcStride is %ld, which should be no more than 65535", intriParams.srcStride);
    });
    ASCENDC_ASSERT((intriParams.srcStride >= static_cast<int64_t>(0)), {
        KERNEL_LOG(KERNEL_ERROR, "srcStride is %ld, which should be no less than 0", intriParams.srcStride);
    });
    CopyUbufToGmAlignV2<T, mode>(
        dst, src, intriParams.blockCount, intriParams.blockLen, intriParams.srcStride, intriParams.dstStride, true,
        cacheMode);
}

template <typename T, typename IntrinParams>
__aicore__ inline void DataCopyPadUB2L1ImplCommon(
    __cbuf__ T* dst, __ubuf__ T* src, const IntrinParams& intriParams, const Nd2NzParams& nd2nzParams)
{
    // Add for TSCM AIV: Copy buffer into workspace, and send message to AIC
    ASCENDC_ASSERT((GetTPipePtr() != nullptr), { KERNEL_LOG(KERNEL_ERROR, "tpipe ptr can not be nullptr"); });
    ASCENDC_ASSERT((dst != nullptr), { KERNEL_LOG(KERNEL_ERROR, "dst ptr can not be nullptr"); });
    ASCENDC_ASSERT((src != nullptr), { KERNEL_LOG(KERNEL_ERROR, "src ptr can not be nullptr"); });
    if ASCEND_IS_AIV {
        ASCENDC_ASSERT((GetKfcClient() != nullptr), { KERNEL_LOG(KERNEL_ERROR, "kfc client ptr can not be nullptr"); });
        ASCENDC_DEBUG_ASSERT(
            (TransUBAddr<TPosition::VECIN>(reinterpret_cast<uint64_t>(src)) % ONE_BLK_SIZE == 0),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR, "Failed to check src tensor address alignment in DataCopyPad from UB(VECIN/VECOUT) to "
                              "L1 Buffer(TSCM), it should be 32B "
                              "aligned.\n"));
        uint32_t tensorSize = nd2nzParams.nValue * nd2nzParams.dValue;
        int32_t ubAddr = -1;
        // 1. get GM addr, first copy to GM, ND->ND
#if ASCENDC_CPU_DEBUG
        uint64_t absUbAddr = (uint8_t*)src - (uint8_t*)(GetTPipePtr()->GetBaseAddr((int8_t)TPosition::VECIN));
        uint64_t absL1Addr = (uint8_t*)dst - (uint8_t*)(GetTPipePtr()->GetBaseAddr((int8_t)TPosition::TSCM));
        ValidateUbL1Address(absUbAddr, absL1Addr, tensorSize);
        ASCENDC_ASSERT((absL1Addr % ONE_BLK_SIZE == 0), {
            KERNEL_LOG(
                KERNEL_ERROR,
                "Failed to check dst tensor address "
                "alignment in DataCopyPad from UB(VECIN/VECOUT) to L1 Buffer(TSCM), it should be 32B aligned");
        });
        ASCENDC_ASSERT((nd2nzParams.ndNum == 1), {
            KERNEL_LOG(KERNEL_ERROR, "nd2nzParams.ndNum is %hu, which can only be 1", nd2nzParams.ndNum);
        });
        GM_ADDR gmAddr = (GetKfcClient()->AllocUB(tensorSize, ubAddr));
#else
        GM_ADDR gmAddr = (GetKfcClient()->AllocUB(tensorSize, ubAddr));
#endif
        event_t eventID = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
        SetFlag<HardEvent::V_MTE3>(eventID);
        WaitFlag<HardEvent::V_MTE3>(eventID);
        // 2. copy ub->GM
        ASCENDC_ASSERT(
            (intriParams.blockCount != 0), { KERNEL_LOG(KERNEL_ERROR, "intriParams.blockCount can not be 0"); });
        ASCENDC_ASSERT((intriParams.blockLen != 0), { KERNEL_LOG(KERNEL_ERROR, "intriParams.blockLen can not be 0"); });
        DataCopyPadUB2GMImpl((__gm__ T*)gmAddr, (__ubuf__ T*)src, intriParams);

#if ASCENDC_CPU_DEBUG
        ScmDataCopyND2NZMsg((__cbuf__ void*)absL1Addr, (__gm__ void*)gmAddr, sizeof(T), nd2nzParams, ubAddr);
#else
        ScmDataCopyND2NZMsg((__cbuf__ void*)dst, (__gm__ void*)gmAddr, sizeof(T), nd2nzParams, ubAddr);
#endif
    }
}

template <typename T>
__aicore__ inline void DataCopyPadUB2L1Impl(
    __cbuf__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams, const Nd2NzParams& nd2nzParams,
    const uint8_t cacheMode = 0)
{
    DataCopyPadUB2L1ImplCommon<T, DataCopyParams>(dst, src, intriParams, nd2nzParams);
}

template <typename T>
__aicore__ inline void DataCopyPadUB2L1Impl(
    __cbuf__ T* dst, __ubuf__ T* src, const DataCopyExtParams& intriParams, const Nd2NzParams& nd2nzParams,
    const uint8_t cacheMode = 0)
{
    DataCopyPadUB2L1ImplCommon<T, DataCopyExtParams>(dst, src, intriParams, nd2nzParams);
}

__aicore__ inline bool IsSupportQuantMode(QuantMode_t quantPre)
{
    return (
        quantPre == QuantMode_t::NoQuant || quantPre == QuantMode_t::F322F16 || quantPre == QuantMode_t::F322BF16 ||
        quantPre == QuantMode_t::DEQF16 || quantPre == QuantMode_t::VDEQF16 || quantPre == QuantMode_t::QF322B8_PRE ||
        quantPre == QuantMode_t::VQF322B8_PRE || quantPre == QuantMode_t::REQ8 || quantPre == QuantMode_t::VREQ8);
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
            "DataCopy from L0C Buffer(CO1) to L1 Buffer(A1/B1), current api support dtype combination is "
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
                    KERNEL_ERROR, "Failed to check quantPre value in DataCopy from L0C Buffer(CO1) "
                                  "to L1 Buffer(A1/B1), supported values are NoQuant / F322F16 / F322BF16 / DEQF16 / "
                                  "VDEQF16 / QF322B8_PRE / "
                                  "VQF322B8_PRE / REQ8 / VREQ8.");
            });
        }
    }
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
            "DataCopy from L0C Buffer(CO1) to GM, current api support dtype combination is "
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
                    "Failed to check quantPre value in DataCopy from L0C Buffer(CO1) "
                    "to GM, supported values are NoQuant / F322F16 / F322BF16 / DEQF16 / VDEQF16 / QF322B8_PRE / "
                    "VQF322B8_PRE / REQ8 / VREQ8.");
            });
        }
    }
}

template <typename T>
__aicore__ inline __in_pipe__(MTE1) __out_pipe__(MTE1) void DataCopyL12UBIntf(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const DataCopyParams& intriParams)
{
    ASCENDC_ASSERT((false), {
        KERNEL_LOG(KERNEL_ERROR, "unsupported data copy from L1 Buffer(A1/B1) to UB(VECIN/VECOUT) on current device");
    });
}

template <typename T>
__aicore__ inline void DataCopyUB2L0CIntf(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
    DataCopyUB2L0CImpl(
        (__cc__ PrimT<T>*)dstLocal.GetPhyAddr(), (__ubuf__ PrimT<T>*)srcLocal.GetPhyAddr(), intriParams,
        enhancedParams);
}

#pragma begin_pipe(V)
template <typename T>
__aicore__ inline void DataCopyUB2UBIntf(
    const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const DataCopyParams& intriParams)
{
    DataCopyUB2UBImpl(
        (__ubuf__ PrimT<T>*)dstLocal.GetPhyAddr(), (__ubuf__ PrimT<T>*)srcLocal.GetPhyAddr(), intriParams);
}
#pragma end_pipe

template <typename T>
__aicore__ inline __in_pipe__(FIX)
    __out_pipe__(FIX) void DataCopyL12FBImpl(__fbuf__ T* dst, __cbuf__ T* src, const DataCopyParams& intriParams)
{
    constexpr uint16_t unitCoeff = 2;
    if ASCEND_IS_AIC {
        copy_cbuf_to_fbuf(
            (__fbuf__ void*)dst, (__cbuf__ void*)src, intriParams.blockCount, intriParams.blockLen * unitCoeff,
            intriParams.srcStride, intriParams.dstStride * unitCoeff);
    }
}

/**
    AIC
* */
__aicore__ inline void ScmDataCopy(MSG_POS void* kfcMsgPtr)
{
    ASCENDC_ASSERT((g_coreType == AIC), { KERNEL_LOG(KERNEL_ERROR, "core type must be AIC"); });
    auto scmCopyParams = reinterpret_cast<MSG_POS struct Gm2L1Params*>(kfcMsgPtr);
    ASCENDC_ASSERT((reinterpret_cast<uint64_t>(scmCopyParams->dst) < TOTAL_L1_SIZE), {
        KERNEL_LOG(
            KERNEL_ERROR, "dst ptr is %p, which should be less than total l1 size %d", scmCopyParams->dst,
            TOTAL_L1_SIZE);
    });
    uint32_t burstLength = scmCopyParams->blockLen * BYTE_32_ALIGN;
    uint64_t srcStride = scmCopyParams->srcStride * BYTE_32_ALIGN + burstLength; // GM   DataCopy: 32Bytes
    uint32_t dstStride = scmCopyParams->dstStride * BYTE_32_ALIGN + burstLength;
#if KFC_C310_SSBUF == 1
    wait_intra_block(
        PIPE_MTE2, static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::GM_L1_UB_GM) + INTRA_NUM * scmCopyParams->subBlockID);
#endif
    uint8_t cacheMode = ExtractCacheMode((__gm__ uint32_t*)scmCopyParams->src);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    uint8_t* aicBaseTSCMAddr = GetTPipePtr()->GetBaseAddr(int8_t(TPosition::TSCM));
    uint8_t* l1AddrDst = aicBaseTSCMAddr + (uint64_t)scmCopyParams->dst;
    copy_gm_to_cbuf_align_v2(
        (__cbuf__ uint32_t*)l1AddrDst, (__gm__ uint32_t*)scmCopyParams->src, 0, scmCopyParams->blockCount, burstLength,
        0, 0, false, cacheMode, srcStride, dstStride);
#else
    auto dst = reinterpret_cast<__cbuf__ void*>(scmCopyParams->dst);
    copy_gm_to_cbuf_align_v2(
        (__cbuf__ uint32_t*)dst, (__gm__ uint32_t*)scmCopyParams->src, 0, scmCopyParams->blockCount, burstLength, 0, 0,
        false, cacheMode, srcStride, dstStride);
#endif
    auto eventID = GetTPipePtr()->FetchEventID(HardEvent::MTE2_MTE1);
    set_flag(PIPE_MTE2, PIPE_MTE1, (event_t)eventID);
    wait_flag(PIPE_MTE2, PIPE_MTE1, (event_t)eventID);
}

__aicore__ inline void ScmDataCopyND2NZ(MSG_POS void* kfcMsgPtr)
{
    ASCENDC_ASSERT((g_coreType == AIC), { KERNEL_LOG(KERNEL_ERROR, "core type must be AIC"); });
    auto scmCopyParams = reinterpret_cast<MSG_POS struct Gm2L1Nd2NzParams*>(kfcMsgPtr);
    auto l1AddrDst = reinterpret_cast<__cbuf__ void*>(scmCopyParams->dst);
#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
    uint8_t* aicBaseTSCMAddr = GetTPipePtr()->GetBaseAddr(int8_t(TPosition::TSCM));
    ASCENDC_ASSERT((aicBaseTSCMAddr != nullptr), { KERNEL_LOG(KERNEL_ERROR, "aicBaseTSCMAddr can not be nullptr"); });
    uint64_t l1AbsAddr = reinterpret_cast<uint64_t>(l1AddrDst);
    l1AddrDst = l1AbsAddr + reinterpret_cast<uint8_t*>(aicBaseTSCMAddr);
    ASCENDC_ASSERT((static_cast<uint64_t>(l1AbsAddr) >= 0 && static_cast<uint64_t>(l1AbsAddr) < TOTAL_L1_SIZE), {
        KERNEL_LOG(KERNEL_ERROR, "l1AbsAddr is 0x%lx, which should be in range of [0, %u)", l1AbsAddr, TOTAL_L1_SIZE);
    });
#endif
#if KFC_C310_SSBUF == 1
    wait_intra_block(
        PIPE_MTE2, static_cast<uint8_t>(CUBE_WAIT_INTRA_Enum::GM_L1_UB_GM) + INTRA_NUM * scmCopyParams->subBlockID);
#endif
    uint16_t loop2_dst_stride = scmCopyParams->dstNzNStride;
    uint16_t loop3_dst_stride = scmCopyParams->dstNzC0Stride;
    uint16_t loop4_dst_stride =
        static_cast<uint16_t>(scmCopyParams->dstNzMatrixStride * scmCopyParams->dataTypeLen / ONE_BLK_SIZE);
    uint64_t mte2NzPara = static_cast<uint64_t>(loop4_dst_stride) << 48; // MTE2_NZ_PARA[63:48]
    mte2NzPara |= static_cast<uint64_t>(loop3_dst_stride) << 32;         // MTE2_NZ_PARA[47:32]
    mte2NzPara |= static_cast<uint64_t>(loop2_dst_stride) << 16;         // MTE2_NZ_PARA[31:16]
    mte2NzPara |= static_cast<uint64_t>(scmCopyParams->ndNum);           // MTE2_NZ_PARA[15:0]
    set_mte2_nz_para(mte2NzPara);
    uint64_t loop1_src_stride = scmCopyParams->srcDValue * scmCopyParams->dataTypeLen;
    uint16_t n_value = scmCopyParams->nValue;
    uint32_t d_value = scmCopyParams->dValue;
    uint64_t loop4_src_stride = scmCopyParams->srcNdMatrixStride * scmCopyParams->dataTypeLen;

    uint8_t cacheMode = ExtractCacheMode((__gm__ float*)scmCopyParams->src);
    if (scmCopyParams->dataTypeLen == 2) {
        copy_gm_to_cbuf_multi_nd2nz(
            (__cbuf__ half*)l1AddrDst, (__gm__ half*)scmCopyParams->src, uint8_t(0), loop1_src_stride, cacheMode,
            n_value, d_value, loop4_src_stride, false);
    } else if (scmCopyParams->dataTypeLen == 4) {
        copy_gm_to_cbuf_multi_nd2nz(
            (__cbuf__ float*)l1AddrDst, (__gm__ float*)scmCopyParams->src, uint8_t(0), loop1_src_stride, cacheMode,
            n_value, d_value, loop4_src_stride, false);
    } else {
        ASCENDC_ASSERT((scmCopyParams->dataTypeLen == 1), {
            KERNEL_LOG(
                KERNEL_ERROR, "type len is %d bytes, which should only be 1/2/4 bytes", scmCopyParams->dataTypeLen);
        });
        copy_gm_to_cbuf_multi_nd2nz(
            (__cbuf__ int8_t*)l1AddrDst, (__gm__ int8_t*)scmCopyParams->src, uint8_t(0), loop1_src_stride, cacheMode,
            n_value, d_value, loop4_src_stride, false);
    }
    auto eventID = GetTPipePtr()->FetchEventID(HardEvent::MTE2_MTE1);
    set_flag(PIPE_MTE2, PIPE_MTE1, (event_t)eventID);
    wait_flag(PIPE_MTE2, PIPE_MTE1, (event_t)eventID);
}

template <typename T>
__aicore__ inline void DataCopyPadUB2L1Impl(
    __cbuf__ T* dst, __ubuf__ T* src, const DataCopyParams& intriParams, const uint8_t cacheMode = 0)
{
    ASSERT(false && "unsupported data copy from gm to ubuf with pad on current device");
}

__aicore__ inline void NdDmaDciImpl()
{
#if ((!defined(ASCENDC_CPU_DEBUG)) || ASCENDC_CPU_DEBUG == 0)
    nd_dma_dci();
#endif
}

template <uint8_t dim, const NdDmaConfig& config>
__aicore__ inline void SetNDDMALoopInfo(
    const MultiCopyLoopInfo<dim>& params, const uint8_t idx, uint64_t& padCount, uint32_t& loopSize,
    uint64_t& loopStride)
{
    constexpr uint8_t loopEncodingOffset = 16;
    constexpr uint8_t lpEncodingOffset = 16;
    constexpr uint8_t rpEncodingOffset = 8;
    constexpr uint8_t srcStrideEncodingOffset = 20;

    const uint8_t loopLpSize = config.loopLpSize == NdDmaConfig::unsetPad ? params.loopLpSize[idx] : config.loopLpSize;
    const uint8_t loopRpSize = config.loopRpSize == NdDmaConfig::unsetPad ? params.loopRpSize[idx] : config.loopRpSize;

    padCount |= static_cast<uint64_t>(loopLpSize & 0xff) << (loopEncodingOffset * idx - lpEncodingOffset);
    padCount |= static_cast<uint64_t>(loopRpSize & 0xff) << (loopEncodingOffset * idx - rpEncodingOffset);
    loopSize = params.loopSize[idx];
    loopStride = (params.loopSrcStride[idx] << srcStrideEncodingOffset) |
                 static_cast<uint64_t>(params.loopDstStride[idx] & 0xfffff);
}

template <uint8_t dim>
__aicore__ inline MultiCopyLoopInfo<dim> BuildLoopInfoForTwiceB32(const MultiCopyLoopInfo<dim>& params)
{
    MultiCopyLoopInfo<dim> newParams;
    for (uint32_t i = 0; i < dim; i++) {
        newParams.loopSize[i] = params.loopSize[i];
        newParams.loopSrcStride[i] = params.loopSrcStride[i] * 2;
        newParams.loopDstStride[i] = params.loopDstStride[i] * 2;
        newParams.loopLpSize[i] = params.loopLpSize[i];
        newParams.loopRpSize[i] = params.loopRpSize[i];
    }
    return newParams;
}

template <uint8_t prevDim, uint8_t actualDim, bool hasPad>
__aicore__ inline MultiCopyLoopInfo<actualDim> BuildLoopInfoByFuseAxis(const MultiCopyLoopInfo<prevDim> params)
{
    MultiCopyLoopInfo<actualDim> newParams;
    newParams.loopSize[0] = 2;
    newParams.loopSrcStride[0] = 1;
    newParams.loopDstStride[0] = 1;
    for (uint32_t i = 1; i < actualDim; i++) {
        newParams.loopSize[i] = params.loopSize[i - 1];
        newParams.loopSrcStride[i] = params.loopSrcStride[i - 1] * 2;
        newParams.loopDstStride[i] = params.loopDstStride[i - 1] * 2;
        if constexpr (hasPad) {
            newParams.loopLpSize[i] = params.loopLpSize[i - 1];
            newParams.loopRpSize[i] = params.loopRpSize[i - 1];
        }
    }
    return newParams;
}

template <typename T, uint8_t dim, const NdDmaConfig& config>
__aicore__ inline void DataCopyWithNDDMAB64Impl(
    __ubuf__ T* dst, __gm__ T* src, const MultiCopyLoopInfo<dim>& params, const T constValue, const uint8_t cacheMode);

template <typename T, uint8_t dim, const NdDmaConfig& config>
__aicore__ inline void DataCopyWithNDDMAImpl(
    __ubuf__ T* dst, __gm__ T* src, const MultiCopyLoopInfo<dim>& params, const T constValue,
    const uint8_t cacheMode = 0)
{
    static_assert(dim <= 5, "nddma only support dim <= 5");
    static_assert(
        config.loopLpSize <= 0xff || config.loopLpSize == NdDmaConfig::unsetPad,
        "loopLpSize in config must be either <= 255 or unset");
    static_assert(
        config.loopRpSize <= 0xff || config.loopRpSize == NdDmaConfig::unsetPad,
        "loopRpSize in config must be either <= 255 or unset");
    if constexpr (sizeof(T) == 8) {
        DataCopyWithNDDMAB64Impl<T, dim, config>(dst, src, params, constValue, cacheMode);
        return;
    }
    if constexpr (g_gm_overflow_check) {
        AscendCUtils::CheckGmMemOverflowNddma(src, params);
    }
    constexpr uint8_t sid{0};
    constexpr uint8_t dim0Idx{0}, dim1Idx{1}, dim2Idx{2}, dim3Idx{3}, dim4Idx{4};
    uint32_t loop0Size{params.loopSize[dim0Idx]}, loop1Size{1}, loop2Size{1}, loop3Size{1}, loop4Size{1};
    uint64_t padCount{0};
    uint8_t loop0LeftPadSize =
        config.loopLpSize == NdDmaConfig::unsetPad ? params.loopLpSize[dim0Idx] : config.loopLpSize;
    uint8_t loop0RightPadSize =
        config.loopRpSize == NdDmaConfig::unsetPad ? params.loopRpSize[dim0Idx] : config.loopRpSize;
    uint64_t loop0Stride{0};
    uint64_t loop1Stride{0};
    uint64_t loop2Stride{0};
    uint64_t loop3Stride{0};
    uint64_t loop4Stride{0};
    // LoopStride: [59: 20] srcStride, [19: 0] dstStride
    loop0Stride =
        (params.loopSrcStride[dim0Idx] << 20) | static_cast<uint64_t>(params.loopDstStride[dim0Idx] & 0xfffff);
    if constexpr (dim > 1) {
        SetNDDMALoopInfo<dim, config>(params, dim1Idx, padCount, loop1Size, loop1Stride);
    }
    if constexpr (dim > 2) {
        SetNDDMALoopInfo<dim, config>(params, dim2Idx, padCount, loop2Size, loop2Stride);
    }
    if constexpr (dim > 3) {
        SetNDDMALoopInfo<dim, config>(params, dim3Idx, padCount, loop3Size, loop3Stride);
    }
    if constexpr (dim > 4) {
        SetNDDMALoopInfo<dim, config>(params, dim4Idx, padCount, loop4Size, loop4Stride);
    }
    set_pad_cnt_nddma(padCount);

    set_loop0_stride_nddma(loop0Stride);
    set_loop1_stride_nddma(loop1Stride);
    set_loop2_stride_nddma(loop2Stride);
    set_loop3_stride_nddma(loop3Stride);
    set_loop4_stride_nddma(loop4Stride);
    set_pcie_rd_ctrl(0);

    if constexpr (sizeof(T) == 1) {
        set_pad_val_nddma(*reinterpret_cast<const uint8_t*>(&constValue));
        nddma_out_to_ub_b8(
            dst, src, sid, loop0Size, loop1Size, loop2Size, loop3Size, loop4Size, loop0LeftPadSize, loop0RightPadSize,
            !config.isNearestValueMode, cacheMode);
    } else if constexpr (sizeof(T) == 2) {
        set_pad_val_nddma(*reinterpret_cast<const uint16_t*>(&constValue));
        nddma_out_to_ub_b16(
            dst, src, sid, loop0Size, loop1Size, loop2Size, loop3Size, loop4Size, loop0LeftPadSize, loop0RightPadSize,
            !config.isNearestValueMode, cacheMode);
    } else if constexpr (sizeof(T) == 4) {
        set_pad_val_nddma(*reinterpret_cast<const uint32_t*>(&constValue));
        nddma_out_to_ub_b32(
            dst, src, sid, loop0Size, loop1Size, loop2Size, loop3Size, loop4Size, loop0LeftPadSize, loop0RightPadSize,
            !config.isNearestValueMode, cacheMode);
    }
}

template <typename T, uint8_t dim, const NdDmaConfig& config>
__aicore__ inline void DataCopyWithNDDMAB64Impl(
    __ubuf__ T* dst, __gm__ T* src, const MultiCopyLoopInfo<dim>& params, const T constValue, const uint8_t cacheMode)
{
    static_assert(!config.isNearestValueMode, "nddma b64 only supports constant value mode");
    ASCENDC_ASSERT((constValue == 0), { KERNEL_LOG(KERNEL_ERROR, "nddma b64 only support const pad value 0"); });
    if constexpr (dim <= 4) {
        auto newParams = BuildLoopInfoByFuseAxis<dim, dim + 1, true>(params);
        DataCopyWithNDDMAImpl<uint32_t, dim + 1, config>(
            reinterpret_cast<__ubuf__ uint32_t*>(dst), reinterpret_cast<__gm__ uint32_t*>(src), newParams, 0u,
            cacheMode);
    } else if constexpr (config.loopLpSize == 0 && config.loopRpSize == 0 && !config.ascOptimize) {
        if (params.loopSize[dim - 1] == 1) {
            auto newParams = BuildLoopInfoByFuseAxis<dim, dim, false>(params);
            DataCopyWithNDDMAImpl<uint32_t, dim, config>(
                reinterpret_cast<__ubuf__ uint32_t*>(dst), reinterpret_cast<__gm__ uint32_t*>(src), newParams, 0u,
                cacheMode);
        } else {
            auto newParams = BuildLoopInfoForTwiceB32(params);
            // Do NDDMA B32 twice, one for odd index, one for even index. ND DMA Instr allows ub addr byte align.
            for (uint32_t i = 0; i < 2; i++) {
                DataCopyWithNDDMAImpl<uint32_t, dim, config>(
                    reinterpret_cast<__ubuf__ uint32_t*>(dst) + i, reinterpret_cast<__gm__ uint32_t*>(src) + i,
                    newParams, 0u, cacheMode);
            }
        }
    } else {
        auto newParams = BuildLoopInfoForTwiceB32(params);
        // Do NDDMA B32 twice, one for odd index, one for even index. ND DMA Instr allows ub addr byte align.
        for (uint32_t i = 0; i < 2; i++) {
            DataCopyWithNDDMAImpl<uint32_t, dim, config>(
                reinterpret_cast<__ubuf__ uint32_t*>(dst) + i, reinterpret_cast<__gm__ uint32_t*>(src) + i, newParams,
                0u, cacheMode);
        }
    }
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_DATA_COPY_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_IMPL_H__
#endif
