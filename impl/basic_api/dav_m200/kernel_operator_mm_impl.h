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
 * \file kernel_operator_mm_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_m200/kernel_operator_mm_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_MM_IMPL_H
#define ASCENDC_MODULE_OPERATOR_MM_IMPL_H
#include "../../../include/basic_api/kernel_struct_mm.h"

namespace AscendC {
/* **************************************************************************************************
 * LoadData 2dv2                                             *
 * ************************************************************************************************* */

template <typename T>
__aicore__ inline void CheckLoadData2DType()
{
    ASCENDC_ASSERT((SupportType<T, uint8_t, int8_t, uint16_t, int16_t, half, int4b_t>()), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to "
            "check dtype in LoadData with LoadData2DParams, current api support dtype combination is src and dst both: "
            "uint8_t, int8_t, uint16_t, int16_t, half.");
    });
}

template <typename T>
__aicore__ inline void LoadData2DL12L0ACal(__ca__ T* dst, __cbuf__ T* src, const LoadData2DParams& loadDataParam)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData from L1 Buffer(A1) to L0A Buffer(A2)");
#else
    CheckLoadData2DType<T>();
    if constexpr (IsSameType<T, int4b_t>::value) {
        load_cbuf_to_ca_s4(
            (__ca__ void*)dst, (__cbuf__ void*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, loadDataParam.sid, 0, inc);
    } else {
        using U = typename Conditional<SupportType<T, uint16_t, int16_t>(), half, T>::type;
        if (loadDataParam.ifTranspose) {
            load_cbuf_to_ca(
                (__ca__ U*)dst, (__cbuf__ U*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
                loadDataParam.srcStride, loadDataParam.dstGap, loadDataParam.sid, 1, inc);
        } else {
            load_cbuf_to_ca(
                (__ca__ U*)dst, (__cbuf__ U*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
                loadDataParam.srcStride, loadDataParam.dstGap, loadDataParam.sid, 0, inc);
        }
    }

#endif
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BCal(__cb__ T* dst, __cbuf__ T* src, const LoadData2DParams& loadDataParam)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData from L1 Buffer(B1) to L0B Buffer(B2)");
#else
    CheckLoadData2DType<T>();
    if constexpr (IsSameType<T, int4b_t>::value) {
        load_cbuf_to_cb_s4(
            (__cb__ void*)dst, (__cbuf__ void*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, loadDataParam.sid, 0, inc);
    } else {
        using U = typename Conditional<SupportType<T, uint16_t, int16_t>(), half, T>::type;
        if (loadDataParam.ifTranspose) {
            load_cbuf_to_cb(
                (__cb__ U*)dst, (__cbuf__ U*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
                loadDataParam.srcStride, loadDataParam.dstGap, loadDataParam.sid, 1, inc);
        } else {
            load_cbuf_to_cb(
                (__cb__ U*)dst, (__cbuf__ U*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
                loadDataParam.srcStride, loadDataParam.dstGap, loadDataParam.sid, 0, inc);
        }
    }

#endif
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0ACal(__ca__ T* dst, __gm__ T* src, const LoadData2DParams& loadDataParam)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData from GM to L0A Buffer(A2)");
#else
    CheckLoadData2DType<T>();
    using U = typename Conditional<SupportType<T, uint16_t, int16_t>(), half, T>::type;
    load_gm_to_ca(
        (__ca__ U*)dst, (__gm__ U*)src, loadDataParam.startIndex, loadDataParam.repeatTimes, loadDataParam.srcStride,
        loadDataParam.dstGap, loadDataParam.sid, (addr_cal_mode_t)0);
#endif
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0BCal(__cb__ T* dst, __gm__ T* src, const LoadData2DParams& loadDataParam)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData from GM to L0B Buffer(B2)");
#else
    CheckLoadData2DType<T>();
    using U = typename Conditional<SupportType<T, uint16_t, int16_t>(), half, T>::type;
    load_gm_to_cb(
        (__cb__ U*)dst, (__gm__ U*)src, loadDataParam.startIndex, loadDataParam.repeatTimes, loadDataParam.srcStride,
        loadDataParam.dstGap, loadDataParam.sid, (addr_cal_mode_t)0);
#endif
}

template <typename T>
__aicore__ inline void LoadData2DGM2L1Cal(__cbuf__ T* dst, __gm__ T* src, const LoadData2DParams& loadDataParam)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData from GM to L1 Buffer(A1/B1)");
#else
    CheckLoadData2DType<T>();
    using U = typename Conditional<SupportType<T, uint16_t, int16_t>(), half, T>::type;
    if (loadDataParam.addrMode == 0) {
        load_gm_to_cbuf(
            (__cbuf__ U*)dst, (__gm__ U*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, loadDataParam.sid, inc);
    } else {
        load_gm_to_cbuf(
            (__cbuf__ U*)dst, (__gm__ U*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, loadDataParam.sid, dec);
    }
#endif
}

template <typename T>
__aicore__ inline void LoadData2DL12L0ACal(__ca__ T* dst, __cbuf__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData with LoadData2DParamsV2");
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BCal(__cb__ T* dst, __cbuf__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData with LoadData2DParamsV2");
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0ACal(__ca__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData with LoadData2DParamsV2");
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0BCal(__cb__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData with LoadData2DParamsV2");
}

template <typename T>
__aicore__ inline void LoadData2DGM2L1Cal(__cbuf__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData with LoadData2DParamsV2");
}

template <typename T>
__aicore__ inline void LoadData2DL12L0ATransposeCal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParams& loadDataParam)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadDataWithTranspose");
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BTransposeCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParams& loadDataParam)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadDataWithTranspose");
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BTransposeCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParamsV2& loadDataParam)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadDataWithTranspose");
}

/* **************************************************************************************************
 * LoadData 3dv1                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData3DV1L12L0ACal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData3DParamsV1<T>& loadDataParams)
{
    if (loadDataParams.cSize == 0) {
        img2col_cbuf_to_ca(
            dst, src, loadDataParams.fetchFilterW, loadDataParams.fetchFilterH, loadDataParams.leftTopW,
            loadDataParams.leftTopH, loadDataParams.c1Index, loadDataParams.strideW, loadDataParams.strideH,
            loadDataParams.filterW, loadDataParams.filterH, loadDataParams.dilationFilterW,
            loadDataParams.dilationFilterH, loadDataParams.jumpStride, loadDataParams.repeatMode,
            loadDataParams.repeatTime, CSIZE0);
    } else {
        img2col_cbuf_to_ca(
            dst, src, loadDataParams.fetchFilterW, loadDataParams.fetchFilterH, loadDataParams.leftTopW,
            loadDataParams.leftTopH, loadDataParams.c1Index, loadDataParams.strideW, loadDataParams.strideH,
            loadDataParams.filterW, loadDataParams.filterH, loadDataParams.dilationFilterW,
            loadDataParams.dilationFilterH, loadDataParams.jumpStride, loadDataParams.repeatMode,
            loadDataParams.repeatTime, CSIZE1);
    }
}

template <typename T>
__aicore__ inline void LoadData3DV1L12L0BCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV1<T>& loadDataParams)
{
    if (loadDataParams.cSize == 0) {
        img2col_cbuf_to_cb(
            dst, src, loadDataParams.fetchFilterW, loadDataParams.fetchFilterH, loadDataParams.leftTopW,
            loadDataParams.leftTopH, loadDataParams.c1Index, loadDataParams.strideW, loadDataParams.strideH,
            loadDataParams.filterW, loadDataParams.filterH, loadDataParams.dilationFilterW,
            loadDataParams.dilationFilterH, loadDataParams.jumpStride, loadDataParams.repeatMode,
            loadDataParams.repeatTime, CSIZE0);
    } else {
        img2col_cbuf_to_cb(
            dst, src, loadDataParams.fetchFilterW, loadDataParams.fetchFilterH, loadDataParams.leftTopW,
            loadDataParams.leftTopH, loadDataParams.c1Index, loadDataParams.strideW, loadDataParams.strideH,
            loadDataParams.filterW, loadDataParams.filterH, loadDataParams.dilationFilterW,
            loadDataParams.dilationFilterH, loadDataParams.jumpStride, loadDataParams.repeatMode,
            loadDataParams.repeatTime, CSIZE1);
    }
}

template <typename T>
__aicore__ inline void LoadData3DV1L12UBCal(
    __ubuf__ T* dst, __cbuf__ T* src, const LoadData3DParamsV1<T>& loadDataParams)
{
    if (loadDataParams.cSize == 0) {
        img2col_cbuf_to_ub(
            dst, src, loadDataParams.fetchFilterW, loadDataParams.fetchFilterH, loadDataParams.leftTopW,
            loadDataParams.leftTopH, loadDataParams.c1Index, loadDataParams.strideW, loadDataParams.strideH,
            loadDataParams.filterW, loadDataParams.filterH, loadDataParams.dilationFilterW,
            loadDataParams.dilationFilterH, loadDataParams.jumpStride, loadDataParams.repeatMode,
            loadDataParams.repeatTime, CSIZE0);
    } else {
        img2col_cbuf_to_ub(
            dst, src, loadDataParams.fetchFilterW, loadDataParams.fetchFilterH, loadDataParams.leftTopW,
            loadDataParams.leftTopH, loadDataParams.c1Index, loadDataParams.strideW, loadDataParams.strideH,
            loadDataParams.filterW, loadDataParams.filterH, loadDataParams.dilationFilterW,
            loadDataParams.dilationFilterH, loadDataParams.jumpStride, loadDataParams.repeatMode,
            loadDataParams.repeatTime, CSIZE1);
    }
}

/* **************************************************************************************************
 * LoadData 3dv2                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData3DV2L12L0ACal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData with LoadData3DParamsV2 from L1 Buffer(A1) to L0A Buffer(A2)");
#else
    if constexpr (IsSameType<T, int4b_t>::value) {
        img2colv2_cbuf_to_ca_s4(
            (__ca__ void*)dst, (__cbuf__ void*)src, loadDataParams.kExtension, loadDataParams.mExtension,
            loadDataParams.kStartPt, loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH,
            loadDataParams.filterW, loadDataParams.filterH, loadDataParams.dilationFilterW,
            loadDataParams.dilationFilterH, loadDataParams.enTranspose, loadDataParams.channelSize);
    } else {
        img2colv2_cbuf_to_ca(
            dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
            loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
            loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
            loadDataParams.enTranspose, loadDataParams.channelSize);
    }
#endif
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData with LoadData3DParamsV2 from L1 Buffer(B1) to L0B Buffer(B2)");
#else
    if constexpr (IsSameType<T, int4b_t>::value) {
        img2colv2_cbuf_to_cb_s4(
            (__cb__ void*)dst, (__cbuf__ void*)src, loadDataParams.kExtension, loadDataParams.mExtension,
            loadDataParams.kStartPt, loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH,
            loadDataParams.filterW, loadDataParams.filterH, loadDataParams.dilationFilterW,
            loadDataParams.dilationFilterH, loadDataParams.enTranspose, loadDataParams.channelSize);
    } else {
        img2colv2_cbuf_to_cb(
            dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
            loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
            loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
            loadDataParams.enTranspose, loadDataParams.channelSize);
    }
#endif
}

template <typename T>
__aicore__ inline void LoadData3DV2L12UBCal(
    __ubuf__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    if constexpr (!IsSameType<T, int4b_t>::value) {
        img2colv2_cbuf_to_ub(
            dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
            loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
            loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
            loadDataParams.enTranspose, loadDataParams.channelSize);
    }
}

/* **************************************************************************************************
 * LoadData 3dv2Pro                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData3DV2L12L0ACal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2Pro& loadDataParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData with LoadData3DParamsV2Pro from L1 Buffer(A1) to L0A Buffer(A2)");
#else
    if constexpr (IsSameType<T, int4b_t>::value) {
        img2colv2_cbuf_to_ca_s4(
            (__ca__ void*)dst, (__cbuf__ void*)src, loadDataParams.extConfig,
            loadDataParams.extConfig >> LOAD_M_EXTENSION, loadDataParams.extConfig >> LOAD_K_START_POSITION,
            loadDataParams.extConfig >> LOAD_M_START_POSITION, loadDataParams.filterConfig,
            loadDataParams.filterConfig >> LOAD_STRIDE_H, loadDataParams.filterConfig >> LOAD_FILTER_W,
            loadDataParams.filterConfig >> LOAD_FILTER_H, loadDataParams.filterConfig >> LOAD_DILATION_FILTER_W,
            loadDataParams.filterConfig >> LOAD_DILATION_FILTER_H, loadDataParams.enTranspose,
            loadDataParams.channelSize);
    } else {
        img2colv2_cbuf_to_ca(
            dst, src, loadDataParams.extConfig, loadDataParams.extConfig >> LOAD_M_EXTENSION,
            loadDataParams.extConfig >> LOAD_K_START_POSITION, loadDataParams.extConfig >> LOAD_M_START_POSITION,
            loadDataParams.filterConfig, loadDataParams.filterConfig >> LOAD_STRIDE_H,
            loadDataParams.filterConfig >> LOAD_FILTER_W, loadDataParams.filterConfig >> LOAD_FILTER_H,
            loadDataParams.filterConfig >> LOAD_DILATION_FILTER_W,
            loadDataParams.filterConfig >> LOAD_DILATION_FILTER_H, loadDataParams.enTranspose,
            loadDataParams.channelSize);
    }
#endif
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2Pro& loadDataParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData with LoadData3DParamsV2Pro from L1 Buffer(B1) to L0B Buffer(B2)");
#else
    if constexpr (IsSameType<T, int4b_t>::value) {
        img2colv2_cbuf_to_cb_s4(
            (__cb__ void*)dst, (__cbuf__ void*)src, loadDataParams.extConfig,
            loadDataParams.extConfig >> LOAD_M_EXTENSION, loadDataParams.extConfig >> LOAD_K_START_POSITION,
            loadDataParams.extConfig >> LOAD_M_START_POSITION, loadDataParams.filterConfig,
            loadDataParams.filterConfig >> LOAD_STRIDE_H, loadDataParams.filterConfig >> LOAD_FILTER_W,
            loadDataParams.filterConfig >> LOAD_FILTER_H, loadDataParams.filterConfig >> LOAD_DILATION_FILTER_W,
            loadDataParams.filterConfig >> LOAD_DILATION_FILTER_H, loadDataParams.enTranspose,
            loadDataParams.channelSize);
    } else {
        img2colv2_cbuf_to_cb(
            dst, src, loadDataParams.extConfig, loadDataParams.extConfig >> LOAD_M_EXTENSION,
            loadDataParams.extConfig >> LOAD_K_START_POSITION, loadDataParams.extConfig >> LOAD_M_START_POSITION,
            loadDataParams.filterConfig, loadDataParams.filterConfig >> LOAD_STRIDE_H,
            loadDataParams.filterConfig >> LOAD_FILTER_W, loadDataParams.filterConfig >> LOAD_FILTER_H,
            loadDataParams.filterConfig >> LOAD_DILATION_FILTER_W,
            loadDataParams.filterConfig >> LOAD_DILATION_FILTER_H, loadDataParams.enTranspose,
            loadDataParams.channelSize);
    }
#endif
}

template <typename T>
__aicore__ inline void LoadData3DV2L12UBCal(
    __ubuf__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData with LoadData3DParamsV2Pro from L1 to UB");
}

template <>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ int8_t* dst, __cbuf__ int8_t* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(
        false, "LoadData with LoadData3DParamsV2Pro from L1 Buffer(B1) to L0B Buffer(B2) with type int8_t");
}

template <>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ uint8_t* dst, __cbuf__ uint8_t* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(
        false, "LoadData with LoadData3DParamsV2Pro from L1 Buffer(B1) to L0B Buffer(B2) with type uint8_t");
}

/* **************************************************************************************************
 * Mmad                                             *
 * ************************************************************************************************* */
template <typename T, typename U, typename S>
__aicore__ inline void MmadCal(__cc__ T* c, __ca__ U* a, __cb__ S* b, const MmadParams& mmadParams)
{
    ASCENDC_ASSERT(mmadParams.cmatrixSource == 0, {
        KERNEL_LOG(KERNEL_ERROR, "the C matrix source doesn't support BT buffer on current device");
    });
    bool cmatrixInitVal = mmadParams.cmatrixInitVal && (!mmadParams.isBias);
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "Mmad");
#else
    ASCENDC_ASSERT(
        (SupportType<
            Tuple<T, U, S>, Tuple<int32_t, int8_t, int8_t>, Tuple<int32_t, uint8_t, int8_t>,
            Tuple<int32_t, uint8_t, uint8_t>, Tuple<half, half, half>, Tuple<float, half, half>,
            Tuple<int32_t, int4b_t, int4b_t> >()),
        {
            KERNEL_LOG(
                KERNEL_ERROR,
                "Failed to check dtype in Mmad, current api support dtype combination is Dst: int32_t, src0: int8_t, "
                "src1: int8_t; Dst: int32_t, src0: uint8_t, src1: int8_t; Dst: int32_t, src0: uint8_t, src1: uint8_t; "
                "Dst: half, src0: half, src1: half; Dst: float, src0: half, src1: half"
                "Dst: int32_t, src0: int4b_t, src1: int4b_t");
        });
    if constexpr ((IsSameType<U, int4b_t>::value) && (IsSameType<S, int4b_t>::value)) {
        ASCENDC_ASSERT(mmadParams.k % 2 == 0, {
            KERNEL_LOG(KERNEL_ERROR, "When src0 and src1 are int4b_t, k must be even number.");
        });
        mad_s4(c, a, b, mmadParams.m, mmadParams.k, mmadParams.n, cmatrixInitVal);
    } else {
        mad(c, a, b, mmadParams.m, mmadParams.k, mmadParams.n, cmatrixInitVal);
    }
#endif
}

template <typename T, typename U, typename S>
__aicore__ inline void MmadCal(
    __cc__ T* c, __ca__ U* a, __cb__ S* b, uint64_t bias, const MmadParams& mmadParams, bool cmatrixSource)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "Mmad with biasLocal");
}

__aicore__ inline void MmadSpCal(__cc__ int32_t* c, __ca__ int8_t* a, __cb__ int8_t* b, const MmadParams& mmadParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "MmadWithSparse");
}

template <
    typename T = int8_t, typename U = uint8_t,
    typename std::enable_if<IsSameType<PrimT<T>, int8_t>::value, bool>::type = true,
    typename std::enable_if<IsSameType<PrimT<U>, uint8_t>::value, bool>::type = true>
__aicore__ inline void LoadDataWithSparseCal(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<U>& idx,
    const LoadData2dParams& loadDataParam)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadDataWithSparse");
}

template <typename T = int8_t, typename std::enable_if<IsSameType<PrimT<T>, int8_t>::value, bool>::type = true>
__aicore__ inline void LoadUnzipIndexCal(const GlobalTensor<T>& src, uint32_t numOfIndexTabEntry)
{
#if ASCENDC_CPU_DEBUG
    const __gm__ int64_t* srcAddr = reinterpret_cast<const __gm__ int64_t*>(src.GetPhyAddr());
    constexpr uint32_t unitOfLength = 128;
    for (int i = 0; i < numOfIndexTabEntry; i++) {
        uint32_t compressedLength = *(srcAddr + i) & 0x1fff;
        SetUnzipCompressedLen(compressedLength * unitOfLength);
    }
    constexpr uint32_t maxSize = 32 * 1024; // max is 32 * 1024  = 32KB
    ASCENDC_ASSERT((src.GetSize() % VALUE_512 == 0 && src.GetSize() <= maxSize), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to check src size in LoadUnzipIndex, it must be less than 32KB and divisible by 512, current "
            "value is %u.",
            src.GetSize());
    });
    ASCENDC_CHECK_VALUE_RANGE(numOfIndexTabEntry, 1, UINT32_MAX, "numOfIndexTabEntry", "LoadUnzipIndex");
#endif
    load_unzip_index_from_gm((__gm__ void*)src.GetPhyAddr(), numOfIndexTabEntry, 0);
}

/* **************************************************************************************************
 * BroadCastVecToMM                                             *
 * ************************************************************************************************* */
template <typename T, typename U>
__aicore__ inline void BroadCastVecToMMCal(
    __cc__ T* dst, __ubuf__ U* src, const int32_t blockCount, const uint8_t blockLen, const uint8_t srcGap,
    const uint8_t dstGap)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "BroadCastVecToMM");
#else
    ASCENDC_CHECK_VALUE_RANGE(blockCount, 1, 255, "blockCount", "BroadCastVecToMM");
    ASCENDC_CHECK_VALUE_RANGE(blockLen, 1, 255, "blockLen", "BroadCastVecToMM");
    ASCENDC_ASSERT((SupportType<Tuple<T, U>, Tuple<int32_t, int32_t>, Tuple<float, float>, Tuple<half, half> >()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in BroadCastVecToMM, current api support dtype combination is "
                          "src and dst both: half / float / int32_t.");
    });
    broadcast_ub_to_cc(dst, src, blockCount, blockLen, srcGap, dstGap);
#endif
}

/* **************************************************************************************************
 * LoadData 3dv1                                             *
 * ************************************************************************************************* */
__aicore__ inline void Load3DSetFMatrixCal(uint16_t l1H, uint16_t l1W, const uint8_t padList[4])
{
    uint64_t regFMatrix = 0;
    regFMatrix |= uint64_t(l1W & 0xFFFF);

    uint32_t l1HShiftBit = 16;
    regFMatrix |= uint64_t(l1H & 0xFFFF) << l1HShiftBit;

    uint32_t padNumber = 4;
    uint32_t padListShiftBit = 8;
    uint32_t padListShiftBase = 32;
    for (uint32_t i = 0; i < padNumber; i++) {
        regFMatrix |= uint64_t(padList[i] & 0xFF) << (padListShiftBase + i * padListShiftBit);
    }
    set_fmatrix(regFMatrix);
}

__aicore__ inline void Load3DSetFMatrixBCal(uint16_t l1H, uint16_t l1W, const uint8_t padList[4])
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "SetFMatrix with fmatrixMode = FMATRIX_RIGHT");
}

template <typename T>
__aicore__ inline void Load3DSetPaddingCal(const T padValue)
{
    uint16_t paddingValue = 0;
    uint16_t padValueShiftBit = 8;
    if constexpr (sizeof(T) == B16_BYTE_SIZE) {
        paddingValue = static_cast<uint16_t>(GetScalarBitcodeValue((T)padValue));
    } else if constexpr (sizeof(T) == B32_BYTE_SIZE) {
        ASCENDC_ASSERT(false, {
            KERNEL_LOG(
                KERNEL_ERROR, "Failed to check dtype in SetLoadDataPaddingValue, current "
                              "api support dtype combination is: int8_t / uint8_t / half / int16_t / uint16_t.");
        });
    } else {
        paddingValue = ((static_cast<uint16_t>(padValue)) << padValueShiftBit) | static_cast<uint16_t>(padValue);
    }
    set_padding(paddingValue);
}

/* **************************************************************************************************
 * InitL1Buffer                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void InitL1BufferCal(__cbuf__ T* dst, const InitConstValueParams<T>& initConstValueParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "InitConstValue in L1 Buffer(A1/B1)");
#else
    ASCENDC_CHECK_VALUE_RANGE(initConstValueParams.repeatTimes, 0, UINT8_MAX, "repeatTimes", "InitConstValue");
    int64_t repeatBit = initConstValueParams.repeatTimes;
    if constexpr (IsSameType<T, half>::value) {
        create_cbuf_matrix((__cbuf__ void*)dst, repeatBit, (half)initConstValueParams.initValue);
    } else if constexpr (IsSameType<T, int16_t>::value || IsSameType<T, uint16_t>::value) {
        create_cbuf_matrix((__cbuf__ void*)dst, repeatBit, GetScalarBitcodeToHalf(initConstValueParams.initValue));
    } else {
        ASCENDC_ASSERT(false, {
            KERNEL_LOG(
                KERNEL_ERROR, "Failed to check dtype in InitConstValue, current api "
                              "support dtype combination is dst: half, int16_t, uint16_t");
        });
    }
#endif
}

/* **************************************************************************************************
 * InitL0ANzMatrix                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void InitL0ANzMatrixCal(__ca__ T* dst, const InitConstValueParams<T>& initConstValueParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "InitConstValue in L0A Buffer(A2)");
#else
    ASCENDC_CHECK_VALUE_RANGE(initConstValueParams.repeatTimes, 0, UINT8_MAX, "repeatTimes", "InitConstValue");
    int64_t repeatBit = initConstValueParams.repeatTimes;
    if constexpr (IsSameType<T, half>::value) {
        create_ca_matrix((__ca__ void*)dst, repeatBit, (half)initConstValueParams.initValue);
    } else if constexpr (IsSameType<T, int16_t>::value || IsSameType<T, uint16_t>::value) {
        create_ca_matrix((__ca__ void*)dst, repeatBit, GetScalarBitcodeToHalf(initConstValueParams.initValue));
    } else {
        ASCENDC_ASSERT(false, {
            KERNEL_LOG(
                KERNEL_ERROR, "Failed to check dtype in InitConstValue, current api "
                              "support dtype combination is dst: half, int16_t, uint16_t");
        });
    }
#endif
}

/* **************************************************************************************************
 * InitL0BNzMatrix                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void InitL0BNzMatrixCal(__cb__ T* dst, const InitConstValueParams<T>& initConstValueParams)
{
#if defined(__DAV_M200_VEC__)
    ASCENDC_REPORT_NOT_SUPPORT(false, "InitConstValue in L0B Buffer(B2)");
#else
    ASCENDC_CHECK_VALUE_RANGE(initConstValueParams.repeatTimes, 0, UINT8_MAX, "repeatTimes", "InitConstValue");
    int64_t repeatBit = initConstValueParams.repeatTimes;
    if constexpr (IsSameType<T, half>::value) {
        create_cb_matrix((__cb__ void*)dst, repeatBit, (half)initConstValueParams.initValue);
    } else if constexpr (IsSameType<T, int16_t>::value || IsSameType<T, uint16_t>::value) {
        create_cb_matrix((__cb__ void*)dst, repeatBit, GetScalarBitcodeToHalf(initConstValueParams.initValue));
    } else {
        ASCENDC_ASSERT(false, {
            KERNEL_LOG(
                KERNEL_ERROR, "Failed to check dtype in InitConstValue, current api "
                              "support dtype combination is dst: half, int16_t, uint16_t");
        });
    }
#endif
}

/* **************************************************************************************************
 * SetLoadDataRepeat                                             *
 * ************************************************************************************************* */
__aicore__ inline void SetLoadDataRepeatCal(const LoadDataRepeatParam& repeatParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "SetLoadDataRepeat");
}

/* **************************************************************************************************
 * SetLoadDataBoundary                                             *
 * ************************************************************************************************* */
__aicore__ inline void SetLoadDataBoundaryCal(uint32_t boundaryValue)
{
    set_l1_3d_size(static_cast<uint64_t>(boundaryValue));
}

/* **************************************************************************************************
 * LoadImageToLocalCal                                            *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadImageToLocalCal(__cbuf__ T* dst, const LoadImageToLocalParams& loadDataParams)
{
    ASCENDC_ASSERT((SupportType<T, uint8_t, int8_t, half>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in "
                          "LoadImageToLocal, current api support dtype combination is dst: uint8_t / int8_t / half.");
    });
    load_image_to_cbuf(
        dst, static_cast<uint16_t>(loadDataParams.horizSize - 1), static_cast<uint16_t>(loadDataParams.vertSize - 1),
        loadDataParams.horizStartPos, loadDataParams.vertStartPos,
        static_cast<uint16_t>(loadDataParams.srcHorizSize - 1), loadDataParams.topPadSize, loadDataParams.botPadSize,
        loadDataParams.leftPadSize, loadDataParams.rightPadSize, loadDataParams.sid);
}

/* **************************************************************************************************
 * LoadDataUnzip                                            *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadDataUnzipToL1Cal(__cbuf__ T* dst, __gm__ T* src)
{
    load_gm_to_cbuf_unzip((__cbuf__ void*)dst, (__gm__ void*)src);
}

template <typename T>
__aicore__ inline void LoadDataUnzipToL0BCal(__cb__ T* dst, __gm__ T* src)
{
    load_gm_to_cb_unzip((__cb__ void*)dst, (__gm__ void*)src);
}

template <typename T>
__aicore__ inline void LoadDataUnzipToL0ACal(__ca__ T* dst, __gm__ T* src)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadDataUnzip to L0A Buffer(A2)");
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_MM_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__
#endif
