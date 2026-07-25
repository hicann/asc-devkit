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
    "impl/basic_api/dav_l311/kernel_operator_mm_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
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
    ASCENDC_ASSERT(false, {
        KERNEL_LOG(
            KERNEL_ERROR,
            "LoadDataWithTranspose from L1 Buffer(A1) to L0A Buffer(A2) is not supported on current device");
    });
}

template <typename T>
__aicore__ inline void LoadData2DL12L0ACal(__ca__ T* dst, __cbuf__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    if constexpr (1 == sizeof(T)) {
        load_cbuf_to_ca(
            (__ca__ int8_t*)dst, (__cbuf__ int8_t*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
            loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, false);
    } else if constexpr (2 == sizeof(T)) {
        load_cbuf_to_ca(
            (__ca__ int16_t*)dst, (__cbuf__ int16_t*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
            loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, false);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
    }
}

/****************************L1 To L0B******************************************/
template <typename T>
__aicore__ inline void LoadData2DL12L0BTransposeCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParams& loadDataParam)
{
    ASCENDC_ASSERT(false, {
        KERNEL_LOG(
            KERNEL_ERROR,
            "LoadDataWithTranspose with LoadData2dTransposeParams from L1 Buffer(B1) to L0B Buffer(B2) is not "
            "supported on current device");
    });
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BTransposeCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParamsV2& loadDataParam)
{
    ASCENDC_ASSERT(false, {
        KERNEL_LOG(
            KERNEL_ERROR,
            "LoadDataWithTranspose with LoadData2dTransposeParamsV2 from L1 Buffer(B1) to L0B Buffer(B2) is not "
            "supported on current device");
    });
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BCal(__cb__ T* dst, __cbuf__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    if constexpr (1 == sizeof(T)) {
        if (loadDataParam.ifTranspose) {
            load_cbuf_to_cb(
                (__cb__ int8_t*)dst, (__cbuf__ int8_t*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
                loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, true);
        } else {
            load_cbuf_to_cb(
                (__cb__ int8_t*)dst, (__cbuf__ int8_t*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
                loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, false);
        }
    } else if constexpr (2 == sizeof(T)) {
        if (loadDataParam.ifTranspose) {
            load_cbuf_to_cb(
                (__cb__ int16_t*)dst, (__cbuf__ int16_t*)src, loadDataParam.mStartPosition,
                loadDataParam.kStartPosition, loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride,
                loadDataParam.dstStride, true);
        } else {
            load_cbuf_to_cb(
                (__cb__ int16_t*)dst, (__cbuf__ int16_t*)src, loadDataParam.mStartPosition,
                loadDataParam.kStartPosition, loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride,
                loadDataParam.dstStride, false);
        }
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
    }
}

/****************************GM To L0A******************************************/

template <typename T>
__aicore__ inline void LoadData2DGM2L0ACal(__ca__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    set_mte2_src_para(uint64_t(loadDataParam.srcStride));
    if constexpr (1 == sizeof(T)) {
        load_gm_to_ca_2dv2(
            (__ca__ int8_t*)dst, (__gm__ int8_t*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
            loadDataParam.dstStride, loadDataParam.mStep, loadDataParam.kStep, loadDataParam.sid);
    } else if constexpr (2 == sizeof(T)) {
        load_gm_to_ca_2dv2(
            (__ca__ half*)dst, (__gm__ half*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
            loadDataParam.dstStride, loadDataParam.mStep, loadDataParam.kStep, loadDataParam.sid);
    } else if constexpr (4 == sizeof(T)) {
        load_gm_to_ca_2dv2(
            (__ca__ float*)dst, (__gm__ float*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
            loadDataParam.dstStride, loadDataParam.mStep, loadDataParam.kStep, loadDataParam.sid);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
    }
}

/****************************GM To L0B******************************************/
template <typename T>
__aicore__ inline void LoadData2DGM2L0BCal(__cb__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    if (loadDataParam.kStep != 1) {
        set_mte2_src_para(uint64_t(loadDataParam.srcStride));
    }
    if constexpr (1 == sizeof(T)) {
        load_gm_to_cb_2dv2(
            (__cb__ int8_t*)dst, (__gm__ int8_t*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
            loadDataParam.dstStride, loadDataParam.mStep, loadDataParam.kStep, loadDataParam.sid, 0);
    } else if constexpr (2 == sizeof(T)) {
        load_gm_to_cb_2dv2(
            (__cb__ half*)dst, (__gm__ half*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
            loadDataParam.dstStride, loadDataParam.mStep, loadDataParam.kStep, loadDataParam.sid, 0);
    } else if constexpr (4 == sizeof(T)) {
        load_gm_to_cb_2dv2(
            (__cb__ float*)dst, (__gm__ float*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
            loadDataParam.dstStride, loadDataParam.mStep, loadDataParam.kStep, loadDataParam.sid, 0);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
    }
}

/****************************GM To L1******************************************/
template <typename T>
__aicore__ inline void LoadData2DGM2L1Cal(__cbuf__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    if (loadDataParam.kStep != 1) {
        set_mte2_src_para(uint64_t(loadDataParam.srcStride));
    }
    if constexpr (1 == sizeof(T)) {
        load_gm_to_cbuf_2dv2(
            (__cbuf__ int8_t*)dst, (__gm__ int8_t*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
            loadDataParam.dstStride, loadDataParam.mStep, loadDataParam.kStep, loadDataParam.sid, 0);
    } else if constexpr (2 == sizeof(T)) {
        load_gm_to_cbuf_2dv2(
            (__cbuf__ half*)dst, (__gm__ half*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
            loadDataParam.dstStride, loadDataParam.mStep, loadDataParam.kStep, loadDataParam.sid, 0);
    } else if constexpr (4 == sizeof(T)) {
        load_gm_to_cbuf_2dv2(
            (__cbuf__ float*)dst, (__gm__ float*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
            loadDataParam.dstStride, loadDataParam.mStep, loadDataParam.kStep, loadDataParam.sid, 0);
    } else {
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current data type is not supported!"); });
    }
}

/* **************************************************************************************************
 * LoadData 2dv1                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData2DL12L0ACal(__ca__ T* dst, __cbuf__ T* src, const LoadData2DParams& loadDataParam)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 2dv1 is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BCal(__cb__ T* dst, __cbuf__ T* src, const LoadData2DParams& loadDataParam)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 2dv1 is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0ACal(__ca__ T* dst, __gm__ T* src, const LoadData2DParams& loadDataParam)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 2dv1 is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0BCal(__cb__ T* dst, __gm__ T* src, const LoadData2DParams& loadDataParam)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 2dv1 is not supported!"); });
}

template <typename T>
__aicore__ inline void LoadData2DGM2L1Cal(__cbuf__ T* dst, __gm__ T* src, const LoadData2DParams& loadDataParam)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "LoadData 2dv1 is not supported!"); });
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
        loadDataParams.channelSize);
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
        loadDataParams.channelSize);
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
template <typename T>
__aicore__ inline void LoadData3DV2L12L0ACal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_ASSERT(
        (false), { KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_3d_v2 from L1 Buffer(A1/B1) to L0A Buffer(A2)"); });
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_ASSERT(
        (false), { KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_3d_v2 from L1 Buffer(A1/B1) to L0B Buffer(B2)"); });
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

template <typename T, typename U, typename S>
__aicore__ inline void MmadCal(__cc__ T* c, __ca__ U* a, __cb__ S* b, const MmadParams& mmadParams)
{
    uint64_t config = 0;
    uint64_t gemvCtrl = 0;
    config |= (((uint64_t)mmadParams.m & 0xfff) << 0);
    config |= (((uint64_t)mmadParams.k & 0xfff) << 12);
    config |= (((uint64_t)mmadParams.n & 0xfff) << 24);
    config |= (((uint64_t)mmadParams.fmOffset & 0xff) << 36);
    config |= (((uint64_t)mmadParams.unitFlag & 0x3) << 55);
    config |= (((uint64_t)gemvCtrl & 0x1) << 61);
    config |= (((uint64_t)mmadParams.cmatrixSource & 0x1) << 62);
    config |= (((uint64_t)mmadParams.cmatrixInitVal & 0x1) << 63);
    mad(c, a, b, config);
}

template <typename U, typename S>
__aicore__ inline void MmadCal(
    __cc__ float* c, __ca__ U* a, __cb__ S* b, uint64_t bias, const MmadParams& mmadParams, bool cmatrixSource)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported float mmad"); });
}

template <typename T, typename U, typename S>
__aicore__ inline void MmadCal(
    __cc__ T* c, __ca__ U* a, __cb__ S* b, uint64_t bias, const MmadParams& mmadParams, bool cmatrixSource)
{
    ASCENDC_ASSERT((cmatrixSource == mmadParams.cmatrixSource), {
        KERNEL_LOG(KERNEL_ERROR, "Mmad cmatrixSource param config error");
    });
    uint64_t config = 0;
    uint64_t gemvCtrl = 0;
    config |= (((uint64_t)mmadParams.m & 0xfff) << 0);
    config |= (((uint64_t)mmadParams.k & 0xfff) << 12);
    config |= (((uint64_t)mmadParams.n & 0xfff) << 24);
    config |= (((uint64_t)mmadParams.fmOffset & 0xff) << 36);
    config |= (((uint64_t)mmadParams.unitFlag & 0x3) << 55);
    config |= (((uint64_t)gemvCtrl & 0x1) << 61);
    config |= (((uint64_t)mmadParams.cmatrixSource & 0x1) << 62);
    config |= (((uint64_t)mmadParams.cmatrixInitVal & 0x1) << 63);
    mad(c, a, b, bias, config);
}

/* **************************************************************************************************
 * LoadData Register                                           *
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
    uint64_t regFMatrix = 0;
    regFMatrix |= (uint64_t)l1W;

    uint32_t l1HShiftBit = 16;
    regFMatrix |= (uint64_t)l1H << l1HShiftBit;

    uint32_t padNumber = 4;
    uint32_t padListShiftBit = 8;
    uint32_t padListShiftBase = 32;
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
    __cc__ T* dst, __ubuf__ T* src, const int32_t blockCount, const uint8_t blockLen, const uint8_t srcGap,
    const uint8_t dstGap)
{
    ASCENDC_ASSERT((false), { KERNEL_LOG(KERNEL_ERROR, "unsupported broadcast from ub to l0c in Ascend910B1"); });
}

/* **************************************************************************************************
 * InitL1Buffer                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void InitL1BufferCal(__cbuf__ T* dst, const InitConstValueParams<T>& initConstValueParams)
{
    if constexpr (2 == sizeof(T)) {
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
    if constexpr (2 == sizeof(T)) {
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
    if constexpr (2 == sizeof(T)) {
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
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "unsupported SetLoadDataRepeat on this version"); });
}

/* **************************************************************************************************
 * SetLoadDataBoundary                                             *
 * ************************************************************************************************* */
__aicore__ inline void SetLoadDataBoundaryCal(uint32_t boundaryValue)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "unsupported SetLoadDataBoundary on this version"); });
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_MM_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__
#endif
