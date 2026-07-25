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
    "impl/basic_api/dav_l300/kernel_operator_mm_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
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
/****************************L1 To L0A******************************************/
template <typename T>
__aicore__ inline void LoadData2DL12L0ATransposeCal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParams& loadDataParam)
{
    if constexpr (B8_BYTE_SIZE == sizeof(T)) {
        load_cbuf_to_ca_transpose(
            (__ca__ uint8_t*)dst, (__cbuf__ uint8_t*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, inc, loadDataParam.dstFracGap);
    } else if constexpr (B16_BYTE_SIZE == sizeof(T)) {
        load_cbuf_to_ca_transpose(
            (__ca__ half*)dst, (__cbuf__ half*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, inc, loadDataParam.dstFracGap);
    } else if constexpr (B32_BYTE_SIZE == sizeof(T)) {
        load_cbuf_to_ca_transpose(
            (__ca__ uint32_t*)dst, (__cbuf__ uint32_t*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, inc, loadDataParam.dstFracGap);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
    }
}

template <typename T>
__aicore__ inline void LoadData2DL12L0ACal(__ca__ T* dst, __cbuf__ T* src, const LoadData2DParams& loadDataParam)
{
    if constexpr (B8_BYTE_SIZE == sizeof(T)) {
        load_cbuf_to_ca(
            (__ca__ uint8_t*)dst, (__cbuf__ uint8_t*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, 0, inc);
    } else if constexpr (B16_BYTE_SIZE == sizeof(T)) {
        load_cbuf_to_ca(
            (__ca__ half*)dst, (__cbuf__ half*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, 0, inc);
    } else if constexpr (B32_BYTE_SIZE == sizeof(T)) {
        load_cbuf_to_ca(
            (__ca__ uint32_t*)dst, (__cbuf__ uint32_t*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, 0, inc);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
    }
}

/****************************L1 To L0B******************************************/
template <typename T>
__aicore__ inline void LoadData2DL12L0BTransposeCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParams& loadDataParam)
{
    if constexpr (B8_BYTE_SIZE == sizeof(T)) {
        load_cbuf_to_cb_transpose(
            (__cb__ uint8_t*)dst, (__cbuf__ uint8_t*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, inc, loadDataParam.dstFracGap);
    } else if constexpr (B16_BYTE_SIZE == sizeof(T)) {
        load_cbuf_to_cb_transpose(
            (__cb__ half*)dst, (__cbuf__ half*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, inc, loadDataParam.dstFracGap);
    } else if constexpr (B32_BYTE_SIZE == sizeof(T)) {
        load_cbuf_to_cb_transpose(
            (__cb__ uint32_t*)dst, (__cbuf__ uint32_t*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, inc, loadDataParam.dstFracGap);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
    }
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BTransposeCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParamsV2& loadDataParam)
{
    ASCENDC_REPORT_NOT_SUPPORT(
        false, "LoadDataWithTranspose with LoadData2dTransposeParamsV2 is not supported on current device");
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BCal(__cb__ T* dst, __cbuf__ T* src, const LoadData2DParams& loadDataParam)
{
    if constexpr (B8_BYTE_SIZE == sizeof(T)) {
        load_cbuf_to_cb(
            (__cb__ uint8_t*)dst, (__cbuf__ uint8_t*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, 0, inc);
    } else if constexpr (B16_BYTE_SIZE == sizeof(T)) {
        load_cbuf_to_cb(
            (__cb__ half*)dst, (__cbuf__ half*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, 0, inc);
    } else if constexpr (B32_BYTE_SIZE == sizeof(T)) {
        load_cbuf_to_cb(
            (__cb__ uint32_t*)dst, (__cbuf__ uint32_t*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, 0, inc);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
    }
}

/****************************GM To L0A******************************************/
template <typename T>
__aicore__ inline void LoadData2DGM2L0ACal(__ca__ T* dst, __gm__ T* src, const LoadData2DParams& loadDataParam)
{
    if constexpr (B8_BYTE_SIZE == sizeof(T)) {
        load_gm_to_ca(
            (__ca__ int8_t*)dst, (__gm__ int8_t*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, loadDataParam.sid, inc);
    } else if constexpr (B16_BYTE_SIZE == sizeof(T)) {
        load_gm_to_ca(
            (__ca__ half*)dst, (__gm__ half*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, loadDataParam.sid, inc);
    } else if constexpr (B32_BYTE_SIZE == sizeof(T)) {
        load_gm_to_ca(
            (__ca__ int32_t*)dst, (__gm__ int32_t*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, loadDataParam.sid, inc);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
    }
}

/****************************GM To L0B******************************************/
template <typename T>
__aicore__ inline void LoadData2DGM2L0BCal(__cb__ T* dst, __gm__ T* src, const LoadData2DParams& loadDataParam)
{
    if constexpr (B8_BYTE_SIZE == sizeof(T)) {
        load_gm_to_cb(
            (__cb__ int8_t*)dst, (__gm__ int8_t*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, loadDataParam.sid, inc);
    } else if constexpr (B16_BYTE_SIZE == sizeof(T)) {
        load_gm_to_cb(
            (__cb__ half*)dst, (__gm__ half*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, loadDataParam.sid, inc);
    } else if constexpr (B32_BYTE_SIZE == sizeof(T)) {
        load_gm_to_cb(
            (__cb__ int32_t*)dst, (__gm__ int32_t*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, loadDataParam.sid, inc);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
    }
}

/****************************GM To L1******************************************/
template <typename T>
__aicore__ inline void LoadData2DGM2L1Cal(__cbuf__ T* dst, __gm__ T* src, const LoadData2DParams& loadDataParam)
{
    if constexpr (B8_BYTE_SIZE == sizeof(T)) {
        load_gm_to_cbuf(
            (__cbuf__ int8_t*)dst, (__gm__ int8_t*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, loadDataParam.sid, inc);
    } else if constexpr (B16_BYTE_SIZE == sizeof(T)) {
        load_gm_to_cbuf(
            (__cbuf__ half*)dst, (__gm__ half*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, loadDataParam.sid, inc);
    } else if constexpr (B32_BYTE_SIZE == sizeof(T)) {
        load_gm_to_cbuf(
            (__cbuf__ int32_t*)dst, (__gm__ int32_t*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, loadDataParam.sid, inc);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
    }
}

/* **************************************************************************************************
 * LoadData 2dv1                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData2DL12L0ACal(__ca__ T* dst, __cbuf__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 2dv2 is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BCal(__cb__ T* dst, __cbuf__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 2dv2 is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0ACal(__ca__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 2dv2 is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0BCal(__cb__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 2dv2 is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData2DGM2L1Cal(__cbuf__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 2dv2 is not supported!"); });
}

/* **************************************************************************************************
 * LoadData 3dv2                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData3DV2L12L0ACal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    img2colv2_cbuf_to_ca(
        dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
        loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
        loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
        loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose, loadDataParams.fMatrixCtrl,
        loadDataParams.channelSize, BM_DISABLE);
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    img2colv2_cbuf_to_cb(
        dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
        loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
        loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
        loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose, loadDataParams.fMatrixCtrl,
        loadDataParams.channelSize);
}

__aicore__ inline void LoadData3DV2L12L0ACal(
    __ca__ half* dst, __cbuf__ half* src, const LoadData3DParamsV2<bfloat16_t>& loadDataParams)
{
    img2colv2_cbuf_to_ca(
        dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
        loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
        loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
        loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose, loadDataParams.fMatrixCtrl,
        loadDataParams.channelSize, BM_DISABLE);
}

__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ half* dst, __cbuf__ half* src, const LoadData3DParamsV2<bfloat16_t>& loadDataParams)
{
    img2colv2_cbuf_to_cb(
        dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
        loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
        loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
        loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose, loadDataParams.fMatrixCtrl,
        loadDataParams.channelSize);
}

/* **************************************************************************************************
 * LoadData 3dv2Pro                                             *
 * ************************************************************************************************* */
__aicore__ inline void LoadData3DV2L12L0ACal(
    __ca__ float* dst, __cbuf__ float* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_ASSERT((false), {
        KERNEL_LOG(KERNEL_ERROR, "unsupported float load data from L1 Buffer(A1/B1) to L0A Buffer(A2)/L0B Buffer(B2)");
    });
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0ACal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    img2colv2_cbuf_to_ca(
        dst, src, loadDataParams.extConfig, loadDataParams.extConfig >> LOAD_M_EXTENSION,
        loadDataParams.extConfig >> LOAD_K_START_POSITION, loadDataParams.extConfig >> LOAD_M_START_POSITION,
        loadDataParams.filterConfig, loadDataParams.filterConfig >> LOAD_STRIDE_H,
        loadDataParams.filterConfig >> LOAD_FILTER_W, loadDataParams.filterConfig >> LOAD_FILTER_H,
        loadDataParams.filterConfig >> LOAD_DILATION_FILTER_W, loadDataParams.filterConfig >> LOAD_DILATION_FILTER_H,
        loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose, loadDataParams.fMatrixCtrl,
        loadDataParams.channelSize, BM_DISABLE);
}

__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ float* dst, __cbuf__ float* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_ASSERT((false), {
        KERNEL_LOG(KERNEL_ERROR, "unsupported float load data from L1 Buffer(A1/B1) to L0A Buffer(A2)/L0B Buffer(B2)");
    });
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    img2colv2_cbuf_to_cb(
        dst, src, loadDataParams.extConfig, loadDataParams.extConfig >> LOAD_M_EXTENSION,
        loadDataParams.extConfig >> LOAD_K_START_POSITION, loadDataParams.extConfig >> LOAD_M_START_POSITION,
        loadDataParams.filterConfig, loadDataParams.filterConfig >> LOAD_STRIDE_H,
        loadDataParams.filterConfig >> LOAD_FILTER_W, loadDataParams.filterConfig >> LOAD_FILTER_H,
        loadDataParams.filterConfig >> LOAD_DILATION_FILTER_W, loadDataParams.filterConfig >> LOAD_DILATION_FILTER_H,
        loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose, loadDataParams.fMatrixCtrl,
        loadDataParams.channelSize);
}

template <typename T>
__aicore__ inline void LoadData3DV2L12UBCal(
    __ubuf__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_3d_v2 from L1 Buffer(A1/B1) to UB"); });
}

template <>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ uint8_t* dst, __cbuf__ uint8_t* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_ASSERT((false), {
        KERNEL_LOG(
            KERNEL_ERROR, "unsupported loaddata_3d_v2 load uint8_t dtype from L1 Buffer(A1/B1) to L0B Buffer(B2)");
    });
}

template <>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ int8_t* dst, __cbuf__ int8_t* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_ASSERT((false), {
        KERNEL_LOG(
            KERNEL_ERROR, "unsupported loaddata_3d_v2 load int8_t dtype from L1 Buffer(A1/B1) to L0B Buffer(B2)");
    });
}

/* **************************************************************************************************
 * Mmad                                             *
 * ************************************************************************************************* */
template <typename U, typename S>
__aicore__ inline void MmadCal(__cc__ float* c, __ca__ U* a, __cb__ S* b, const MmadParams& mmadParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported float mmad"); });
}

template <typename DstT, typename Src0T, typename Src1T>
__aicore__ inline void MmadCal(__cc__ DstT* c, __ca__ Src0T* a, __cb__ Src1T* b, const MmadParams& mmadParams)
{
    uint64_t config = 0;
    config |= ((static_cast<uint64_t>(mmadParams.m) & 0xfff) << 0);
    config |= ((static_cast<uint64_t>(mmadParams.k) & 0xfff) << 12);
    config |= ((static_cast<uint64_t>(mmadParams.n) & 0xfff) << 24);
    config |= ((static_cast<uint64_t>(mmadParams.fmOffset) & 0xff) << 36);
    config |= ((static_cast<uint64_t>(mmadParams.unitFlag) & 0x3) << 55);
    config |= ((static_cast<uint64_t>(mmadParams.cmatrixSource) & 0x1) << 62);
    config |= ((static_cast<uint64_t>(mmadParams.cmatrixInitVal) & 0x1) << 63);
    mad(c, a, b, config);
}

template <typename U, typename S>
__aicore__ inline void MmadCal(
    __cc__ float* c, __ca__ U* a, __cb__ S* b, uint64_t bias, const MmadParams& mmadParams, bool cmatrixSource)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported float mmad"); });
}

template <typename DstT, typename Src0T, typename Src1T>
__aicore__ inline void MmadCal(
    __cc__ DstT* c, __ca__ Src0T* a, __cb__ Src1T* b, uint64_t bias, const MmadParams& mmadParams, bool cmatrixSource)
{
    ASCENDC_ASSERT((cmatrixSource == mmadParams.cmatrixSource), {
        KERNEL_LOG(KERNEL_ERROR, "Mmad cmatrixSource param config error");
    });
    uint64_t config = 0;
    config |= ((static_cast<uint64_t>(mmadParams.m) & 0xfff) << 0);
    config |= ((static_cast<uint64_t>(mmadParams.k) & 0xfff) << 12);
    config |= ((static_cast<uint64_t>(mmadParams.n) & 0xfff) << 24);
    config |= ((static_cast<uint64_t>(mmadParams.fmOffset) & 0xff) << 36);
    config |= ((static_cast<uint64_t>(mmadParams.unitFlag) & 0x3) << 55);
    config |= ((static_cast<uint64_t>(mmadParams.cmatrixSource) & 0x1) << 62);
    config |= ((static_cast<uint64_t>(mmadParams.cmatrixInitVal) & 0x1) << 63);
    mad(c, a, b, bias, config);
}

/* **************************************************************************************************
 * LoadData 3dv1                                             *
 * ************************************************************************************************* */
__aicore__ inline void Load3DSetFMatrixCal(uint16_t l1H, uint16_t l1W, const uint8_t padList[4])
{
    uint64_t regFMatrix = 0;
    regFMatrix |= uint64_t(l1W & 0xFFFF);

    constexpr uint32_t l1HShiftBit = 16;
    regFMatrix |= uint64_t(l1H & 0xFFFF) << l1HShiftBit;

    constexpr uint32_t padNumber = 4;
    constexpr uint32_t padListShiftBit = 8;
    constexpr uint32_t padListShiftBase = 32;
    for (uint32_t i = 0; i < padNumber; i++) {
        regFMatrix |= uint64_t(padList[i] & 0xFF) << (padListShiftBase + i * padListShiftBit);
    }
    set_fmatrix(regFMatrix);
}

__aicore__ inline void Load3DSetFMatrixBCal(uint16_t l1H, uint16_t l1W, const uint8_t padList[4])
{
    uint64_t regFMatrix = 0;
    regFMatrix |= (uint64_t)l1W;

    constexpr uint32_t l1HShiftBit = 16;
    regFMatrix |= (uint64_t)l1H << l1HShiftBit;

    constexpr uint32_t padNumber = 4;
    constexpr uint32_t padListShiftBit = 8;
    constexpr uint32_t padListShiftBase = 32;
    for (uint32_t i = 0; i < padNumber; i++) {
        regFMatrix |= uint64_t(padList[i] & 0xFF) << (padListShiftBase + i * padListShiftBit);
    }
    set_fmatrix_b(regFMatrix);
}

template <typename T>
__aicore__ inline void Load3DSetPaddingCal(const T padValue)
{
    uint16_t paddingValue = 0;
    uint16_t padValueShiftBit = 8;

    if constexpr (sizeof(T) == B16_BYTE_SIZE) {
        paddingValue = (uint16_t)GetScalarBitcodeValue((T)padValue);
    } else if constexpr (sizeof(T) == B32_BYTE_SIZE) {
        paddingValue = (uint32_t)GetScalarBitcodeValue((T)padValue);
    } else {
        paddingValue = (((uint16_t)padValue) << padValueShiftBit) | (uint16_t)padValue;
    }
    set_padding(paddingValue);
}

/* **************************************************************************************************
 * LoadData 3dv1                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData3DV1L12L0ACal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData3DParamsV1<T>& loadDataParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_3d_v1 from l1 to l0a"); });
}

template <typename T>
__aicore__ inline void LoadData3DV1L12L0BCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV1<T>& loadDataParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_3d_v1 from l1 to l0b"); });
}

template <typename T>
__aicore__ inline void LoadData3DV1L12UBCal(
    __ubuf__ T* dst, __cbuf__ T* src, const LoadData3DParamsV1<T>& loadDataParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_3d_v1 from l1 to ubuf"); });
}

/* **************************************************************************************************
 * LoadData 3dv2                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData3DV2L12UBCal(
    __ubuf__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_3d_v2 from l1 to ubuf"); });
}

template <>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ int8_t* dst, __cbuf__ int8_t* src, const LoadData3DParamsV2<int8_t>& loadDataParams)
{
    ASCENDC_ASSERT(
        (false), { KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_3d_v2 load int8_t dtype from l1 to cb"); });
}

template <>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ uint8_t* dst, __cbuf__ uint8_t* src, const LoadData3DParamsV2<uint8_t>& loadDataParams)
{
    ASCENDC_ASSERT(
        (false), { KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_3d_v2 load uint8_t dtype from l1 to cb"); });
}

/* **************************************************************************************************
 * BroadCastVecToMM                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void BroadCastVecToMMCal(
    __cc__ T* dstLocal, __ubuf__ T* srcLocal, const int32_t blockCount, const uint8_t blockLen, const uint8_t srcGap,
    const uint8_t dstGap)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported broadcast from ub to l0c"); });
}

/* **************************************************************************************************
 * InitL1Buffer                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void InitL1BufferCal(__cbuf__ T* dst, const InitConstValueParams<T>& initConstValueParams)
{
    if constexpr (B16_BYTE_SIZE == sizeof(T)) {
        T tmpValue = initConstValueParams.initValue;
        // 由于set_l0_set_value_h只支持half类型，故需要将其它类型的数据的二进制用half
        half initValue = *(half*)(&tmpValue);
        set_l0_set_value_h(initValue);
        int64_t config = 0;
        config |= (uint64_t)initConstValueParams.repeatTimes;
        config |= (uint64_t)initConstValueParams.blockNum << 16;
        config |= (uint64_t)initConstValueParams.dstGap << 32;
        set_l1_2d((__cbuf__ half*)dst, config);
    } else {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported the data type!"); });
    }
}

/* **************************************************************************************************
 * InitL0ANzMatrix                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void InitL0ANzMatrixCal(__ca__ T* dst, const InitConstValueParams<T>& initConstValueParams)
{
    if constexpr (B16_BYTE_SIZE == sizeof(T)) {
        T tmpValue = initConstValueParams.initValue;
        // 由于set_l0_set_value_h只支持half类型，故需要将其它类型的数据的二进制用half
        half initValue = *(half*)(&tmpValue);
        set_l0_set_value_h(initValue);
        int64_t config = 0;
        config |= static_cast<uint64_t>(initConstValueParams.repeatTimes);
        config |= static_cast<uint64_t>(initConstValueParams.blockNum) << 16;
        config |= static_cast<uint64_t>(initConstValueParams.dstGap) << 32;
        set_l0a_2d((__ca__ half*)dst, config);
    } else {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported the data type!"); });
    }
}

/* **************************************************************************************************
 * InitL0BNzMatrix                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void InitL0BNzMatrixCal(__cb__ T* dst, const InitConstValueParams<T>& initConstValueParams)
{
    if constexpr (B16_BYTE_SIZE == sizeof(T)) {
        T tmpValue = initConstValueParams.initValue;
        // 由于set_l0_set_value_h只支持half类型，故需要将其它类型的数据的二进制用half
        half initValue = *(half*)(&tmpValue);
        set_l0_set_value_h(initValue);
        int64_t config = 0;
        config |= static_cast<uint64_t>(initConstValueParams.repeatTimes);
        config |= static_cast<uint64_t>(initConstValueParams.blockNum) << 16;
        config |= static_cast<uint64_t>(initConstValueParams.dstGap) << 32;
        set_l0b_2d((__cb__ half*)dst, config);
    } else {
        ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported the data type!"); });
    }
}

/* **************************************************************************************************
 * SetLoadDataRepeat                                             *
 * ************************************************************************************************* */
__aicore__ inline void SetLoadDataRepeatCal(const LoadDataRepeatParam& repeatParams)
{
    uint64_t rptConfig = static_cast<uint64_t>(repeatParams.repeatStride) |
                         (static_cast<uint64_t>(repeatParams.repeatTime) << 16) |
                         (static_cast<uint64_t>(repeatParams.repeatMode) << 24);
    set_l3d_rpt(rptConfig);
}

/* **************************************************************************************************
 * SetLoadDataBoundary                                             *
 * ************************************************************************************************* */
__aicore__ inline void SetLoadDataBoundaryCal(uint32_t boundaryValue)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "unsupported SetLoadDataBoundary"); });
}

} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_MM_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__
#endif
