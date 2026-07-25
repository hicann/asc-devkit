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
    "impl/basic_api/dav_m310/kernel_operator_mm_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
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
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "not support LoadData2DParams on current device"); });
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BCal(__cb__ T* dst, __cbuf__ T* src, const LoadData2DParams& loadDataParam)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "not support LoadData2DParams on current device"); });
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0ACal(__ca__ T* dst, __gm__ T* src, const LoadData2DParams& loadDataParam)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "not support LoadData2DParams on current device"); });
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0BCal(__cb__ T* dst, __gm__ T* src, const LoadData2DParams& loadDataParam)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "not support LoadData2DParams on current device"); });
}

template <typename T>
__aicore__ inline void LoadData2DGM2L1Cal(__cbuf__ T* dst, __gm__ T* src, const LoadData2DParams& loadDataParam)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "not support LoadData2DParams on current device"); });
}

template <typename T>
__aicore__ inline void LoadData2DL12L0ATransposeCal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParams& loadDataParam)
{
    ASCENDC_ASSERT(false, {
        KERNEL_LOG(
            KERNEL_ERROR, "not support LoadDataWithTranspose from L1 Buffer(A1/B1) to L0A Buffer(A2) "
                          "on current device");
    });
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BTransposeCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParams& loadDataParam)
{
    uint32_t unitMultiples = 1;
    if (IsSameType<T, int8_t>::value || IsSameType<T, uint8_t>::value) {
        unitMultiples = 2;
    }
    // Value multiplication due to unit mismatch
    LoadData2dTransposeParamsV2 loadDataParamsV2(
        loadDataParam.startIndex * unitMultiples, loadDataParam.repeatTimes, loadDataParam.srcStride * unitMultiples,
        loadDataParam.dstGap, loadDataParam.dstFracGap, 0, loadDataParam.addrMode);
    LoadData2DL12L0BTransposeCal(dst, src, loadDataParamsV2);
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BTransposeCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData2dTransposeParamsV2& loadDataParam)
{
    if constexpr (IsSameType<T, int4b_t>::value) {
        load_cbuf_to_cb_transpose_s4(
            (__cb__ void*)dst, (__cbuf__ void*)src, loadDataParam.startIndex, loadDataParam.repeatTimes,
            loadDataParam.srcStride, loadDataParam.dstGap, inc, loadDataParam.dstFracGap, loadDataParam.srcFracGap);
    } else {
        load_cbuf_to_cb_transpose(
            dst, src, loadDataParam.startIndex, loadDataParam.repeatTimes, loadDataParam.srcStride,
            loadDataParam.dstGap, inc, loadDataParam.dstFracGap, loadDataParam.srcFracGap);
    }
}

/* **************************************************************************************************
 * LoadData 2dv2                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData2DL12L0ACal(__ca__ T* dst, __cbuf__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    if constexpr (IsSameType<T, int4b_t>::value) {
        if (loadDataParam.ifTranspose) {
            load_cbuf_to_ca_s4(
                (__ca__ void*)dst, (__cbuf__ void*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
                loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 1);
        } else {
            load_cbuf_to_ca_s4(
                (__ca__ void*)dst, (__cbuf__ void*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
                loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 0);
        }
    } else {
        if (loadDataParam.ifTranspose) {
            load_cbuf_to_ca(
                dst, src, loadDataParam.mStartPosition, loadDataParam.kStartPosition, loadDataParam.mStep,
                loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 1);
        } else {
            load_cbuf_to_ca(
                dst, src, loadDataParam.mStartPosition, loadDataParam.kStartPosition, loadDataParam.mStep,
                loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 0);
        }
    }
}

template <typename T>
__aicore__ inline void LoadData2DL12L0BCal(__cb__ T* dst, __cbuf__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    if constexpr (IsSameType<T, int4b_t>::value) {
        if (loadDataParam.ifTranspose) {
            load_cbuf_to_cb_s4(
                (__cb__ void*)dst, (__cbuf__ void*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
                loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 1);
        } else {
            load_cbuf_to_cb_s4(
                (__cb__ void*)dst, (__cbuf__ void*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
                loadDataParam.mStep, loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 0);
        }
    } else {
        if (loadDataParam.ifTranspose) {
            load_cbuf_to_cb(
                dst, src, loadDataParam.mStartPosition, loadDataParam.kStartPosition, loadDataParam.mStep,
                loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 1);
        } else {
            load_cbuf_to_cb(
                dst, src, loadDataParam.mStartPosition, loadDataParam.kStartPosition, loadDataParam.mStep,
                loadDataParam.kStep, loadDataParam.srcStride, loadDataParam.dstStride, 0);
        }
    }
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0ACal(__ca__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    if constexpr (IsSameType<T, int4b_t>::value) {
        load_gm_to_ca_2dv2_s4(
            (__ca__ void*)dst, (__gm__ void*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
            loadDataParam.srcStride, loadDataParam.dstStride, loadDataParam.mStep, loadDataParam.kStep,
            loadDataParam.sid);
    } else {
        load_gm_to_ca_2dv2(
            dst, src, loadDataParam.mStartPosition, loadDataParam.kStartPosition, loadDataParam.srcStride,
            loadDataParam.dstStride, loadDataParam.mStep, loadDataParam.kStep, loadDataParam.sid);
    }
}

template <typename T>
__aicore__ inline void LoadData2DGM2L0BCal(__cb__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    if constexpr (IsSameType<T, int4b_t>::value) {
        load_gm_to_cb_2dv2_s4(
            (__cb__ void*)dst, (__gm__ void*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
            loadDataParam.srcStride, loadDataParam.dstStride, loadDataParam.mStep, loadDataParam.kStep,
            loadDataParam.sid);
    } else {
        load_gm_to_cb_2dv2(
            dst, src, loadDataParam.mStartPosition, loadDataParam.kStartPosition, loadDataParam.srcStride,
            loadDataParam.dstStride, loadDataParam.mStep, loadDataParam.kStep, loadDataParam.sid);
    }
}

template <typename T>
__aicore__ inline void LoadData2DGM2L1Cal(__cbuf__ T* dst, __gm__ T* src, const LoadData2DParamsV2& loadDataParam)
{
    if constexpr (IsSameType<T, int4b_t>::value) {
        load_gm_to_cbuf_2dv2_s4(
            (__cbuf__ void*)dst, (__gm__ void*)src, loadDataParam.mStartPosition, loadDataParam.kStartPosition,
            loadDataParam.srcStride, loadDataParam.dstStride, loadDataParam.mStep, loadDataParam.kStep,
            loadDataParam.sid);
    } else {
        load_gm_to_cbuf_2dv2(
            dst, src, loadDataParam.mStartPosition, loadDataParam.kStartPosition, loadDataParam.srcStride,
            loadDataParam.dstStride, loadDataParam.mStep, loadDataParam.kStep, loadDataParam.sid);
    }
}

/* **************************************************************************************************
 * LoadData 3dv1                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData3DV1L12L0ACal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData3DParamsV1<T>& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "not support LoadData3DV1 on current device"); });
}

template <typename T>
__aicore__ inline void LoadData3DV1L12L0BCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV1<T>& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "not support LoadData3DV1 on current device"); });
}

template <typename T>
__aicore__ inline void LoadData3DV1L12UBCal(
    __ubuf__ T* dst, __cbuf__ T* src, const LoadData3DParamsV1<T>& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "not support LoadData3DV1 on current device"); });
}

/* **************************************************************************************************
 * Mmad                                             *
 * ************************************************************************************************* */
template <typename T, typename U, typename V>
__aicore__ inline void MmadCal(__cc__ T* c, __ca__ U* a, __cb__ V* b, const MmadParams& mmadParams)
{
    bool cmatrixInitVal = mmadParams.cmatrixInitVal && (!mmadParams.isBias);
    if constexpr ((IsSameType<U, int4b_t>::value) && (IsSameType<V, int4b_t>::value)) {
        mad_s4(
            c, (__ca__ void*)a, (__cb__ void*)b, mmadParams.m, mmadParams.k, mmadParams.n, mmadParams.unitFlag, 0,
            mmadParams.cmatrixSource, cmatrixInitVal);
    } else {
        mad(c, a, b, mmadParams.m, mmadParams.k, mmadParams.n, mmadParams.unitFlag, 0, mmadParams.cmatrixSource,
            cmatrixInitVal);
    }
}

template <typename T, typename U, typename V>
__aicore__ inline void MmadCal(
    __cc__ T* c, __ca__ U* a, __cb__ V* b, uint64_t bias, const MmadParams& mmadParams, bool cmatrixSource)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "not support mmad with bias calculate on current device"); });
}

__aicore__ inline void MmadSpCal(__cc__ int32_t* c, __ca__ int8_t* a, __cb__ int8_t* b, const MmadParams& mmadParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "not support mmadSp calculate on current device"); });
}

template <
    typename T = int8_t, typename U = uint8_t,
    typename std::enable_if<IsSameType<PrimT<T>, int8_t>::value, bool>::type = true,
    typename std::enable_if<IsSameType<PrimT<U>, uint8_t>::value, bool>::type = true>
__aicore__ inline void LoadDataWithSparseCal(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<U>& idx,
    const LoadData2dParams& loadDataParam)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "not support LoadDataWithSparse calculate on current device"); });
}

template <typename T = int8_t, typename std::enable_if<IsSameType<PrimT<T>, int8_t>::value, bool>::type = true>
__aicore__ inline void LoadUnzipIndexCal(const GlobalTensor<T>& src, uint32_t numOfIndexTabEntry)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "not support LoadUnzipIndex on current device"); });
}
/* **************************************************************************************************
 * BroadCastVecToMM                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void BroadCastVecToMMCal(
    __cc__ T* dstLocal, __ubuf__ T* srcLocal, const int32_t blockCount, const uint8_t blockLen, const uint8_t srcGap,
    const uint8_t dstGap)
{
    broadcast_ub_to_cc(dstLocal, srcLocal, blockCount, blockLen, srcGap, dstGap);
}

/* **************************************************************************************************
 * LoadData 3dv2                                             *
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
    regFMatrix |= static_cast<uint64_t>(l1W);

    uint32_t l1HShiftBit = 16;
    regFMatrix |= static_cast<uint64_t>(l1H) << l1HShiftBit;

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
    uint16_t padValueS4ShiftBit = 4;

    if constexpr (IsSameType<T, int4b_t>::value) {
        paddingValue = ((static_cast<uint16_t>(padValue)) << padValueS4ShiftBit) | static_cast<uint16_t>(padValue);
        paddingValue = ((static_cast<uint16_t>(paddingValue)) << padValueShiftBit) | paddingValue;
    } else if constexpr (sizeof(T) == B16_BYTE_SIZE) {
        paddingValue = static_cast<uint16_t>(GetScalarBitcodeValue((T)padValue));
    } else if constexpr (sizeof(T) == B32_BYTE_SIZE) {
        paddingValue = static_cast<uint32_t>(GetScalarBitcodeValue((T)padValue));
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
    if constexpr (IsSameType<T, int4b_t>::value) {
        img2colv2_cbuf_to_ca_s4(
            dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
            loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
            loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
            loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose,
            loadDataParams.fMatrixCtrl, loadDataParams.channelSize);
    } else {
        img2colv2_cbuf_to_ca(
            dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
            loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
            loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
            loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose,
            loadDataParams.fMatrixCtrl, loadDataParams.channelSize);
    }
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    if constexpr (IsSameType<T, int16_t>::value || IsSameType<T, uint16_t>::value || IsSameType<T, half>::value) {
        img2colv2_cbuf_to_cb(
            dst, src, loadDataParams.kExtension, loadDataParams.mExtension, loadDataParams.kStartPt,
            loadDataParams.mStartPt, loadDataParams.strideW, loadDataParams.strideH, loadDataParams.filterW,
            loadDataParams.filterH, loadDataParams.dilationFilterW, loadDataParams.dilationFilterH,
            loadDataParams.filterSizeW, loadDataParams.filterSizeH, loadDataParams.enTranspose,
            loadDataParams.fMatrixCtrl, loadDataParams.channelSize);
    } else {
        ASCENDC_ASSERT(
            false, { KERNEL_LOG(KERNEL_ERROR, "unsupported data type for loaddata_3d_v2 on current device"); });
    }
}

template <typename T>
__aicore__ inline void LoadData3DV2L12UBCal(
    __ubuf__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2<T>& loadDataParams)
{
    ASCENDC_ASSERT(false, {
        KERNEL_LOG(
            KERNEL_ERROR, "unsupported loaddata_3d_v2 from L1 Buffer(A1/B1) L1 Buffer(A1/B1) to UB "
                          "on current device");
    });
}

/* **************************************************************************************************
 * LoadData 3dv2Pro                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadData3DV2L12L0ACal(
    __ca__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_ASSERT((false), {
        KERNEL_LOG(
            KERNEL_ERROR, "unsupported loaddata_3d_v2 from L1 Buffer(A1/B1) to L0A Buffer(A2) on current device");
    });
}

template <typename T>
__aicore__ inline void LoadData3DV2L12L0BCal(
    __cb__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_ASSERT((false), {
        KERNEL_LOG(
            KERNEL_ERROR, "unsupported loaddata_3d_v2 from L1 Buffer(A1/B1) to L0B Buffer(B2) on current device");
    });
}

template <typename T>
__aicore__ inline void LoadData3DV2L12UBCal(
    __ubuf__ T* dst, __cbuf__ T* src, const LoadData3DParamsV2Pro& loadDataParams)
{
    ASCENDC_ASSERT((false), {
        KERNEL_LOG(KERNEL_ERROR, "unsupported loaddata_3d_v2 from L1 Buffer(A1/B1) to UB on current device");
    });
}

__aicore__ inline void CheckInitConstValueParams(uint16_t repeatTime, uint16_t blockNum, uint16_t dstGap)
{
    constexpr uint16_t validRange = 32767; // valid range for Xm[0:14]/Xm[16:30]/Xm[32:46]
    ASCENDC_ASSERT((repeatTime <= validRange), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to check repeatTime value in InitConstValue, its valid range is 0 ~ 32767, current value is %u",
            repeatTime);
    });
    ASCENDC_ASSERT((blockNum <= validRange), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to check blockNum value in InitConstValue, its valid range is 0 ~ 32767, current value is %u",
            blockNum);
    });
    ASCENDC_ASSERT((dstGap <= validRange), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to check dstGap value in InitConstValue, its valid range is 0 ~ 32767, current value is %u",
            dstGap);
    });
}

/* **************************************************************************************************
 * InitL1Buffer                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void InitL1BufferCal(__cbuf__ T* dst, const InitConstValueParams<T>& initConstValueParams)
{
    CheckInitConstValueParams(
        initConstValueParams.repeatTimes, initConstValueParams.blockNum, initConstValueParams.dstGap);
    int64_t repeatBit = (static_cast<uint64_t>(initConstValueParams.blockNum) << 16) |
                        (static_cast<uint64_t>(initConstValueParams.dstGap) << 32) | initConstValueParams.repeatTimes;
    if constexpr (IsSameType<T, half>::value) {
        create_cbuf_matrix(dst, repeatBit, (T)initConstValueParams.initValue);
    } else if constexpr (IsSameType<T, int16_t>::value || IsSameType<T, uint16_t>::value) {
        create_cbuf_matrix(dst, repeatBit, GetScalarBitcodeToHalf(initConstValueParams.initValue));
    } else {
        ASCENDC_ASSERT(
            false, { KERNEL_LOG(KERNEL_ERROR, "InitConstValue doesn't support current data type on current device"); });
    }
}

/* **************************************************************************************************
 * InitL0ANzMatrix                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void InitL0ANzMatrixCal(__ca__ T* dst, const InitConstValueParams<T>& initConstValueParams)
{
    CheckInitConstValueParams(
        initConstValueParams.repeatTimes, initConstValueParams.blockNum, initConstValueParams.dstGap);
    int64_t repeatBit = (static_cast<uint64_t>(initConstValueParams.blockNum) << 16) |
                        (static_cast<uint64_t>(initConstValueParams.dstGap) << 32) | initConstValueParams.repeatTimes;
    if constexpr (IsSameType<T, half>::value) {
        create_ca_matrix(dst, repeatBit, (T)initConstValueParams.initValue);
    } else if constexpr (IsSameType<T, int16_t>::value || IsSameType<T, uint16_t>::value) {
        create_ca_matrix(dst, repeatBit, GetScalarBitcodeToHalf(initConstValueParams.initValue));
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
    CheckInitConstValueParams(
        initConstValueParams.repeatTimes, initConstValueParams.blockNum, initConstValueParams.dstGap);
    int64_t repeatBit = (static_cast<uint64_t>(initConstValueParams.blockNum) << 16) |
                        (static_cast<uint64_t>(initConstValueParams.dstGap) << 32) | initConstValueParams.repeatTimes;
    if constexpr (IsSameType<T, half>::value) {
        create_cb_matrix(dst, repeatBit, (T)initConstValueParams.initValue);
    } else if constexpr (IsSameType<T, int16_t>::value || IsSameType<T, uint16_t>::value) {
        create_cb_matrix(dst, repeatBit, GetScalarBitcodeToHalf(initConstValueParams.initValue));
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
    uint64_t rptConfig = 0;
    constexpr uint32_t repeatTimeShiftBit = 16;
    constexpr uint32_t repeatModeShiftBit = 24;
    constexpr uint32_t dstStrideShiftBit = 32;
    constexpr uint32_t dstStartPosShiftBit = 48;
    constexpr uint32_t defaultDstStartPos = 0;
    rptConfig |= uint64_t(repeatParams.repeatStride);
    rptConfig |= uint64_t(repeatParams.repeatTime) << repeatTimeShiftBit;
    rptConfig |= uint64_t(repeatParams.repeatMode) << repeatModeShiftBit;
    rptConfig |= uint64_t(repeatParams.dstStride) << dstStrideShiftBit;
    rptConfig |= uint64_t(defaultDstStartPos) << dstStartPosShiftBit;
    set_l3d_rpt(rptConfig);
}

/* **************************************************************************************************
 * SetLoadDataBoundary                                             *
 * ************************************************************************************************* */
__aicore__ inline void SetLoadDataBoundaryCal(uint32_t boundaryValue)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current device does not support SetLoadDataBoundary"); });
}

/* **************************************************************************************************
 * LoadImageToLocalCal                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadImageToLocalCal(__cbuf__ T* dst, const LoadImageToLocalParams& loadDataParams)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current device does not support LoadImageToLocal"); });
}
/* **************************************************************************************************
 * LoadDataUnzip                                            *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline void LoadDataUnzipToL1Cal(__cbuf__ T* dst, __gm__ T* src)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current device does not support LoadDataUnzip"); });
}

template <typename T>
__aicore__ inline void LoadDataUnzipToL0BCal(__cb__ T* dst, __gm__ T* src)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current device does not support LoadDataUnzip"); });
}

template <typename T>
__aicore__ inline void LoadDataUnzipToL0ACal(__ca__ T* dst, __gm__ T* src)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "current device does not support LoadDataUnzip"); });
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_MM_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_IMPL_H__
#endif
