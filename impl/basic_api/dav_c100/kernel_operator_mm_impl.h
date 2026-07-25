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
    "impl/basic_api/dav_c100/kernel_operator_mm_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_MM_IMPL_H
#define ASCENDC_MODULE_OPERATOR_MM_IMPL_H
#include "../../../include/basic_api/kernel_struct_mm.h"

namespace AscendC {
/* **************************************************************************************************
 * LoadData 2dv1                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData2DL12L0ACal(__ca__ T* dst, __cbuf__ T* src, const LoadData2DParams& loadDataParam)
{
    if (loadDataParam.ifTranspose) {
        load_cbuf_to_ca(
            dst, src, loadDataParam.startIndex, loadDataParam.repeatTimes, loadDataParam.srcStride, loadDataParam.sid,
            1, inc);
    } else {
        load_cbuf_to_ca(
            dst, src, loadDataParam.startIndex, loadDataParam.repeatTimes, loadDataParam.srcStride, loadDataParam.sid,
            0, inc);
    }
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BCal(__cb__ T* dst, __cbuf__ T* src, const LoadData2DParams& loadDataParam)
{
    if (loadDataParam.ifTranspose) {
        load_cbuf_to_cb(
            dst, src, loadDataParam.startIndex, loadDataParam.repeatTimes, loadDataParam.srcStride, loadDataParam.sid,
            1, inc);
    } else {
        load_cbuf_to_cb(
            dst, src, loadDataParam.startIndex, loadDataParam.repeatTimes, loadDataParam.srcStride, loadDataParam.sid,
            0, inc);
    }
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0ACal(__ca__ T* dst, __gm__ T* src, const LoadData2DParams& loadDataParam)
{
    load_gm_to_ca(
        dst, src, loadDataParam.startIndex, loadDataParam.repeatTimes, loadDataParam.srcStride, loadDataParam.sid,
        (addr_cal_mode_t)0);
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0BCal(__cb__ T* dst, __gm__ T* src, const LoadData2DParams& loadDataParam)
{
    load_gm_to_cb(
        dst, src, loadDataParam.startIndex, loadDataParam.repeatTimes, loadDataParam.srcStride, loadDataParam.sid,
        (addr_cal_mode_t)0);
}

template <typename T>
__aicore__ inline void LoadData2DGM2L1Cal(__cbuf__ T* dst, __gm__ T* src, const LoadData2DParams& loadDataParam)
{
    load_gm_to_cbuf(
        dst, src, loadDataParam.startIndex, loadDataParam.repeatTimes, loadDataParam.srcStride, loadDataParam.sid,
        (addr_cal_mode_t)0);
}

template <typename T>
__aicore__ inline void LoadData2DL12L0ATransposeCal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParams& loadDataParam)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadDataWithTranspose from L1 Buffer(A1) to L0A Buffer(A2)");
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BTransposeCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParams& loadDataParam)
{
    ASCENDC_REPORT_NOT_SUPPORT(
        false, "LoadDataWithTranspose with LoadData2dTransposeParams from L1 Buffer(B1) to L0B Buffer(B2)");
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BTransposeCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParamsV2& loadDataParam)
{
    ASCENDC_REPORT_NOT_SUPPORT(
        false, "LoadDataWithTranspose with LoadData2dTransposeParamsV2 from L1 Buffer(B1) to L0B Buffer(B2)");
}

/* **************************************************************************************************
 * LoadData 2dv2                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData2DL12L0ACal(__ca__ T* dst, __cbuf__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData with LoadData2DParamsV2 from L1 Buffer(A1) to L0A Buffer(A2)");
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BCal(__cb__ T* dst, __cbuf__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData with LoadData2DParamsV2 from L1 Buffer(B1) to L0B Buffer(B2)");
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0ACal(__ca__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData with LoadData2DParamsV2 from GM to L0A Buffer(A2)");
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0BCal(__cb__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData with LoadData2DParamsV2 from GM to L0B Buffer(B2)");
}

template <typename T>
__aicore__ inline void LoadData2DGM2L1Cal(__cbuf__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData with LoadData2DParamsV2 from GM to L1 Buffer(A1/B1)");
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
 * Mmad                                             *
 * ************************************************************************************************* */
template <typename T, typename U, typename S>
__aicore__ inline void MmadCal(__cc__ T* c, __ca__ U* a, __cb__ S* b, const MmadParams& mmadParams)
{
    ASCENDC_ASSERT(mmadParams.cmatrixSource == 0, {
        KERNEL_LOG(KERNEL_ERROR, "the C matrix source not support BT buffer on current device");
    });
    bool cmatrixInitVal = mmadParams.cmatrixInitVal && (!mmadParams.isBias);
    return mad(c, a, b, mmadParams.m, mmadParams.k, mmadParams.n, cmatrixInitVal);
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
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadUnzipIndex");
}

/* **************************************************************************************************
 * BroadCastVecToMM                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void BroadCastVecToMMCal(
    __cc__ T* dst, __ubuf__ T* src, const int32_t blockCount, const uint8_t blockLen, const uint8_t srcGap,
    const uint8_t dstGap)
{
    broadcast_ub_to_cc(dst, src, blockCount, blockLen, srcGap, dstGap);
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
        ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "Load3DSetPaddingCal does not support sizeof(T) = 4"); });
    } else {
        paddingValue = ((static_cast<uint16_t>(padValue)) << padValueShiftBit) | static_cast<uint16_t>(padValue);
    }
    set_padding(paddingValue);
}

/* **************************************************************************************************
 * LoadData 3dv2                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData3DV2L12L0ACal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData with LoadData3DParamsV2 from L1 Buffer(A1) to L0A Buffer(A2)");
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData with LoadData3DParamsV2 from L1 Buffer(B1) to L0B Buffer(B2)");
}

template <typename T>
__aicore__ inline void LoadData3DV2L12UBCal(
    __ubuf__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData with LoadData3DParamsV2 from L1 to UB");
}

/* **************************************************************************************************
 * LoadData 3dv2Pro                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData3DV2L12L0ACal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData with LoadData3DParamsV2Pro from L1 Buffer(A1) to L0A Buffer(A2)");
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData with LoadData3DParamsV2Pro from L1 Buffer(B1) to L0B Buffer(B2)");
}

template <typename T>
__aicore__ inline void LoadData3DV2L12UBCal(
    __ubuf__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadData with LoadData3DParamsV2Pro from L1 to UB");
}

/* **************************************************************************************************
 * InitL1Buffer                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void InitL1BufferCal(__cbuf__ T* dst, const InitConstValueParams<T>& initConstValueParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "InitConstValue in L1 Buffer(A1/B1)");
}

/* **************************************************************************************************
 * InitL0ANzMatrix                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void InitL0ANzMatrixCal(__ca__ T* dst, const InitConstValueParams<T>& initConstValueParams)
{
    int64_t repeatBit = initConstValueParams.repeatTimes;
    if constexpr (IsSameType<T, half>::value) {
        create_ca_matrix((__ca__ void*)dst, repeatBit, (half)initConstValueParams.initValue);
    } else if constexpr (IsSameType<T, int16_t>::value || IsSameType<T, uint16_t>::value) {
        create_ca_matrix((__ca__ void*)dst, repeatBit, GetScalarBitcodeToHalf(initConstValueParams.initValue));
    } else {
        ASCENDC_ASSERT(
            false, { KERNEL_LOG(KERNEL_ERROR, "InitConstValue doesn't support current data type on current device"); });
    }
}

/* **************************************************************************************************
 * InitL0BNzMatrix                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void InitL0BNzMatrixCal(__cb__ T* dst, const InitConstValueParams<T>& initConstValueParams)
{
    int64_t repeatBit = initConstValueParams.repeatTimes;
    if constexpr (IsSameType<T, half>::value) {
        create_cb_matrix((__cb__ void*)dst, repeatBit, (half)initConstValueParams.initValue);
    } else if constexpr (IsSameType<T, int16_t>::value || IsSameType<T, uint16_t>::value) {
        create_cb_matrix((__cb__ void*)dst, repeatBit, GetScalarBitcodeToHalf(initConstValueParams.initValue));
    } else {
        ASCENDC_ASSERT(
            false, { KERNEL_LOG(KERNEL_ERROR, "InitConstValue doesn't support current data type on current device"); });
    }
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
    ASCENDC_REPORT_NOT_SUPPORT(false, "SetLoadDataBoundary");
}

/* **************************************************************************************************
 * LoadImageToLocalCal                                            *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadImageToLocalCal(__cbuf__ T* dst, const LoadImageToLocalParams& loadDataParams)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadImageToLocal");
}

/* **************************************************************************************************
 * LoadDataUnzip                                            *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadDataUnzipToL1Cal(__cbuf__ T* dst, __gm__ T* src)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadDataUnzip");
}

template <typename T>
__aicore__ inline void LoadDataUnzipToL0BCal(__cb__ T* dst, __gm__ T* src)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadDataUnzip");
}

template <typename T>
__aicore__ inline void LoadDataUnzipToL0ACal(__ca__ T* dst, __gm__ T* src)
{
    ASCENDC_REPORT_NOT_SUPPORT(false, "LoadDataUnzip");
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_MM_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__
#endif
