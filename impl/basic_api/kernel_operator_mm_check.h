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
 * \file kernel_operator_mm_check.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_mm_check.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_mm_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_CHECK_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_MM_CHECK_H
#define ASCENDC_MODULE_OPERATOR_MM_CHECK_H

#include "kernel_check.h"
#include "kernel_npu_debug.h"
#include "kernel_log.h"
#include "../../include/basic_api/kernel_struct_mm.h"
#include "../../include/basic_api/kernel_struct_fixpipe.h"

namespace AscendC {

template <typename T>
__aicore__ static inline bool ChannelSizeRemainder(const uint16_t channelSize, uint16_t remainder[], uint16_t size)
{
    uint16_t oneBlkNum = ONE_BLK_SIZE / sizeof(T);
    if constexpr (IsSameType<T, int4b_t>::value) {
        oneBlkNum = 64; // 1 block = 64 int4b_t
    }
    for (uint16_t i = 0; i < size; i++) {
        if (channelSize % oneBlkNum == remainder[i]) {
            return true;
        }
    }
    return false;
}
// check fm, filter align
template <typename T, typename U, typename S>
__aicore__ static inline void CheckMmadAlign(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter)
{
    constexpr uint64_t align1024B = 1024;
    if constexpr (
        (IsSameType<PrimT<U>, half>::value) && (IsSameType<PrimT<S>, half>::value) &&
        (IsSameType<PrimT<T>, half>::value)) {
        CheckTensorAlign<T>(dst, VALUE_512, "dst", "Mmad");
    } else {
        CheckTensorAlign<T>(dst, align1024B, "dst", "Mmad");
    }
    CheckTensorAlign<U>(fm, VALUE_512, "fm", "Mmad");
    CheckTensorAlign<S>(filter, VALUE_512, "filter", "Mmad");
}

__aicore__ inline void CheckMmadParamsCommon(const MmadParams& mmadParams, const __gm__ char* apiName)
{
    CheckValueRange<uint16_t>(mmadParams.m, 0, UINT12_MAX, "m", apiName);
    CheckValueRange<uint16_t>(mmadParams.n, 0, UINT12_MAX, "n", apiName);
    CheckValueRange<uint16_t>(mmadParams.k, 0, UINT12_MAX, "k", apiName);
    ASCENDC_DEBUG_ASSERT(
        (mmadParams.unitFlag == 0 || mmadParams.unitFlag == 2 || mmadParams.unitFlag == 3),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Failed to check unitFlag value in %s, supported values are 0, 2, and 3.\n", apiName));
    ReportNopWarning<uint16_t>(mmadParams.m, "mmadParams.m", apiName);
    ReportNopWarning<uint16_t>(mmadParams.n, "mmadParams.n", apiName);
    ReportNopWarning<uint16_t>(mmadParams.k, "mmadParams.k", apiName);
}

template <typename T, typename U, typename S>
__aicore__ inline void CheckMmadTensorCommon(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const MmadParams& mmadParams,
    const __gm__ char* apiName)
{
    constexpr uint32_t align1024B = 1024;

    CheckMmadParamsCommon(mmadParams, apiName);
    CheckTensorPhyPosition<Hardware::L0C>(dst, "dstLocal", "L0C Buffer(CO1)", apiName);
    CheckTensorPhyPosition<Hardware::L0A>(fm, "fmLocal", "L0A Buffer(A2)", apiName);
    CheckTensorPhyPosition<Hardware::L0B>(filter, "filterLocal", "L0B Buffer(B2)", apiName);
    CheckTensorAlignment(dst, align1024B, "dst", apiName);
    CheckTensorAlignment(fm, VALUE_512, "fm", apiName);
    CheckTensorAlignment(filter, VALUE_512, "filter", apiName);
}

template <typename T, typename U, typename S, typename V>
__aicore__ inline void CheckMmadTensorCommon(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias,
    const MmadParams& mmadParams, const __gm__ char* apiName)
{
    CheckMmadTensorCommon(dst, fm, filter, mmadParams, apiName);
    CheckTensorAlignment(bias, 128, "bias", apiName);
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 1001) || (__NPU_ARCH__ == 2002))
    CheckTensorPhyPosition<Hardware::L0C>(bias, "bias", "L0C Buffer(CO1)", apiName);
#else
    CheckTensorPhyPosition<Hardware::L0C, Hardware::BIAS>(
        bias, "bias", "L0C Buffer(CO1)/BiasTable Buffer(C2)", apiName);
#endif
}

__aicore__ inline void CheckFixpipeQuantPreWithWorkspaceCommon(const QuantMode_t quantPre, const __gm__ char* apiName)
{
    ASCENDC_DEBUG_ASSERT(
        (quantPre == QuantMode_t::VDEQF16 || quantPre == QuantMode_t::VQF322B8_PRE || quantPre == QuantMode_t::VREQ8),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to check quantPre value in %s, "
            "when cbufWorkspace is given, supported values are VDEQF16 / VQF322B8_PRE / VREQ8.\n",
            apiName));
}

__aicore__ inline void CheckFixpipeQuantPreValid(const QuantMode_t quantPre, const __gm__ char* apiName)
{
    ASCENDC_DEBUG_ASSERT(
        (quantPre == QuantMode_t::NoQuant || quantPre == QuantMode_t::F322F16 || quantPre == QuantMode_t::F322BF16 ||
         quantPre == QuantMode_t::DEQF16 || quantPre == QuantMode_t::VDEQF16 || quantPre == QuantMode_t::QF322B8_PRE ||
         quantPre == QuantMode_t::VQF322B8_PRE || quantPre == QuantMode_t::REQ8 || quantPre == QuantMode_t::VREQ8),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to check quantPre value in %s, supported values are NoQuant / F322F16 / F322BF16 / DEQF16 / "
            "VDEQF16 / QF322B8_PRE / VQF322B8_PRE / REQ8 / VREQ8.\n",
            apiName));
}

template <typename T, typename U>
__aicore__ inline void CheckFixpipeQuantPreCommon(const QuantMode_t quantPre, const __gm__ char* apiName)
{
    CheckFixpipeQuantPreValid(quantPre, apiName);
    if constexpr (IsSameType<PrimT<U>, float>::value && SupportType<PrimT<T>, int8_t, uint8_t>()) {
        ASCENDC_DEBUG_ASSERT(
            (quantPre == QuantMode_t::QF322B8_PRE || quantPre == QuantMode_t::VQF322B8_PRE),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check quantPre value in %s, when src is float and dst is int8_t / uint8_t, supported values "
                "are QF322B8_PRE and VQF322B8_PRE.\n",
                apiName));
    } else if constexpr (IsSameType<PrimT<U>, float>::value && IsSameType<PrimT<T>, half>::value) {
        ASCENDC_DEBUG_ASSERT(
            (quantPre == QuantMode_t::F322F16), KERNEL_LOG_INTERNAL(
                                                    KERNEL_ERROR,
                                                    "Failed to check quantPre value in %s, when src is float and dst "
                                                    "is half, supported value is F322F16.\n",
                                                    apiName));
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
    } else if constexpr (IsSameType<PrimT<U>, float>::value && IsSameType<PrimT<T>, bfloat16_t>::value) {
        ASCENDC_DEBUG_ASSERT(
            (quantPre == QuantMode_t::F322BF16),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check quantPre value in %s, when src is float and dst is bfloat16_t, supported value is "
                "F322BF16.\n",
                apiName));
#endif
    } else if constexpr (IsSameType<PrimT<U>, int32_t>::value && SupportType<PrimT<T>, int8_t, uint8_t>()) {
        ASCENDC_DEBUG_ASSERT(
            (quantPre == QuantMode_t::REQ8 || quantPre == QuantMode_t::VREQ8),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check quantPre value in %s, when src is int32_t and dst is int8_t / uint8_t, supported "
                "values "
                "are REQ8 and VREQ8.\n",
                apiName));
    } else if constexpr (IsSameType<PrimT<U>, int32_t>::value && IsSameType<PrimT<T>, half>::value) {
        ASCENDC_DEBUG_ASSERT(
            (quantPre == QuantMode_t::DEQF16 || quantPre == QuantMode_t::VDEQF16),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check quantPre value in %s, when src is int32_t and dst is half, supported values are "
                "DEQF16 "
                "and VDEQF16.\n",
                apiName));
    }
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void CheckFixpipeParamsV220Common(const FixpipeParamsV220& intriParams, const __gm__ char* apiName)
{
    if (intriParams.isChannelSplit) {
        ASCENDC_DEBUG_ASSERT(
            (intriParams.nSize >= 0 && intriParams.nSize <= UINT12_MAX && intriParams.nSize % 8 == 0),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check nSize value in %s, "
                "when isChannelSplit is true, its valid range is 0 ~ 4095 and must be divisible by 8, current value "
                "is %u.\n",
                apiName, intriParams.nSize));
        ASCENDC_DEBUG_ASSERT(
            (IsSameType<PrimT<T>, float>::value && IsSameType<PrimT<U>, float>::value),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check isChannelSplit value in %s, isChannelSplit can be "
                "enabled only when src and dst are float.\n",
                apiName));
        ASCENDC_DEBUG_ASSERT(
            (config.format != CO2Layout::ROW_MAJOR),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check isChannelSplit value in %s, isChannelSplit and NZ2ND cannot be enabled at the same "
                "time.\n",
                apiName));
    } else if constexpr (config.format == CO2Layout::ROW_MAJOR) {
        CheckValueRange<uint16_t>(intriParams.nSize, 0, UINT12_MAX, "nSize", apiName);
    } else {
        ASCENDC_DEBUG_ASSERT(
            (intriParams.nSize >= 0 && intriParams.nSize <= UINT12_MAX && intriParams.nSize % 16 == 0),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check nSize value in %s, "
                "when isChannelSplit is false and format is NZ, its valid range is 0 ~ 4095 and must be divisible by "
                "16, "
                "current value is %u.\n",
                apiName, intriParams.nSize));
    }
    ReportNopWarning<uint16_t>(intriParams.nSize, "intriParams.nSize", apiName);

    constexpr uint16_t maxMSize = config.format == CO2Layout::ROW_MAJOR ? 8192 : UINT16_MAX;
    CheckValueRange<uint16_t>(intriParams.mSize, 0, maxMSize, "mSize", apiName);
    ReportNopWarning<uint16_t>(intriParams.mSize, "intriParams.mSize", apiName);

    ASCENDC_DEBUG_ASSERT(
        (intriParams.dstStride != 0),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to check dstStride value in %s, its valid range is 1 ~ 4294967295, current value is %u.\n", apiName,
            intriParams.dstStride));
    ReportNopWarning<uint16_t>(intriParams.ndNum, "intriParams.ndNum", apiName);

    if (intriParams.ndNum > 1) {
        CheckValueRange<uint16_t>(intriParams.srcNdStride, 1, VALUE_512, "srcNdStride", apiName);
        CheckValueRange<uint16_t>(intriParams.dstNdStride, 1, UINT16_MAX, "dstNdStride", apiName);
    }
    ASCENDC_DEBUG_ASSERT(
        (intriParams.unitFlag == 0 || intriParams.unitFlag == 2 || intriParams.unitFlag == 3),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Failed to check unitFlag value in %s, supported values are 0, 2, and 3.\n", apiName));

    CheckFixpipeQuantPreCommon<T, U>(intriParams.quantPre, apiName);
}

template <typename T>
__aicore__ inline void CheckFixpipeWorkspace(
    const LocalTensor<T>& cbufWorkspace, const FixpipeParamsV220& intriParams, const __gm__ char* apiName)
{
    ASCENDC_DEBUG_ASSERT(
        (SupportType<PrimT<T>, uint64_t>()),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Failed to check cbufWorkspace dtype in %s, supported dtype is uint64_t.\n", apiName));
    CheckTensorPhyPosition<Hardware::L1>(cbufWorkspace, "cbufWorkspace", "L1 Buffer(C1)", apiName);

    ASCENDC_DEBUG_ASSERT(
        (intriParams.quantPre == QuantMode_t::VDEQF16 || intriParams.quantPre == QuantMode_t::VQF322B8_PRE ||
         intriParams.quantPre == QuantMode_t::VREQ8),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to check quantPre value in %s, "
            "when cbufWorkspace is given, supported values are VDEQF16 / VQF322B8_PRE / VREQ8.\n",
            apiName));
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void CheckFixpipeTensor(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsV220& intriParams,
    const __gm__ char* apiName)
{
    CheckFixpipeParamsV220Common<T, U, config>(intriParams, apiName);
    CheckTensorPhyPosition<Hardware::L0C>(src, "src", "L0C Buffer(CO1)", apiName);
    const uint32_t L0C_SRC_ALIGN = 16 * sizeof(float);
    CheckTensorAlignment(src, L0C_SRC_ALIGN, "src", apiName);
    CheckTensorAlignment(dst, ONE_BLK_SIZE, "dst", apiName);
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102))
    CheckTensorPhyPosition<Hardware::L1, Hardware::UB>(dst, "dst", "L1 Buffer(C1)/UB(VECIN/VECOUT/VECCALC)", apiName);
#else
    CheckTensorPhyPosition<Hardware::L1>(dst, "dst", "L1 Buffer(C1)", apiName);
#endif
}

template <typename T, typename U, const FixpipeConfig& config, typename S>
__aicore__ inline void CheckFixpipeTensor(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<S>& cbufWorkspace,
    const FixpipeParamsV220& intriParams, const __gm__ char* apiName)
{
    CheckFixpipeTensor<T, U, config>(dst, src, intriParams, apiName);
    CheckFixpipeWorkspace(cbufWorkspace, intriParams, apiName);
}

template <typename T, typename U, const FixpipeConfig& config>
__aicore__ inline void CheckFixpipeTensor(
    const GlobalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsV220& intriParams,
    const __gm__ char* apiName)
{
    (void)dst;
    CheckFixpipeParamsV220Common<T, U, config>(intriParams, apiName);
    CheckTensorPhyPosition<Hardware::L0C>(src, "src", "L0C Buffer(CO1)", apiName);
}

template <typename T, typename U, const FixpipeConfig& config, typename S>
__aicore__ inline void CheckFixpipeTensor(
    const GlobalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<S>& cbufWorkspace,
    const FixpipeParamsV220& intriParams, const __gm__ char* apiName)
{
    CheckFixpipeTensor<T, U, config>(dst, src, intriParams, apiName);
    CheckFixpipeWorkspace(cbufWorkspace, intriParams, apiName);
}

// check LoadData2D datatype
template <typename T>
__aicore__ static inline void CheckLoadData2dDatatype()
{
#if __NPU_ARCH__ == 2002
    ASCENDC_ASSERT((SupportType<PrimT<T>, uint8_t, int8_t, uint16_t, int16_t, half, int4b_t>()), {
        KERNEL_LOG(
            KERNEL_ERROR, "Failed to check dtype in LoadData with LoadData2DParams, current api support dtype "
                          "combination is src and dst both: uint8_t / int8_t / uint16_t / int16_t / half / int4b_t.");
    });
#elif __NPU_ARCH__ == 2201
    ASCENDC_DEBUG_ASSERT(
        (SupportType<
            PrimT<T>, uint8_t, int8_t, uint16_t, int16_t, half, bfloat16_t, uint32_t, int32_t, float, int4b_t>()),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR, "Failed to check dtype in LoadData with LoadData2DParams,"
                          " current api support dtype combination is src and dst both: uint8_t / int8_t / uint16_t / "
                          "int16_t / half / "
                          "bfloat16_t / uint32_t / int32_t / float / int4b_t.\n"));
#elif __NPU_ARCH__ == 3102
    ASCENDC_ASSERT((SupportType<PrimT<T>, uint8_t, int8_t, half, uint16_t, int16_t, int4b_t>()), {
        KERNEL_LOG(
            KERNEL_ERROR,
            "Failed to check dtype in LoadData with LoadData2DParamsV2, current api support "
            "dtype combination is src and dst both: uint8_t / int8_t / half / uint16_t / int16_t / int4b_t.");
    });
#endif
}

template <typename T>
__aicore__ static inline void CheckLoadData2dLocal2Local(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const __gm__ char* apiName)
{
    CheckTensorPhyPosition<Hardware::L1>(src, "src", "L1 Buffer(A1/B1)", apiName);
    CheckTensorPhyPosition<Hardware::L0A, Hardware::L0B>(dst, "dst", "L0A Buffer(A2)/L0B Buffer(B2)", apiName);
    CheckTensorAlignment(src, ONE_BLK_SIZE, "src", apiName);
    CheckTensorAlignment(dst, VALUE_512, "dst", apiName);
}

template <typename T>
__aicore__ static inline void CheckLoadData2dGlobal2Local(const LocalTensor<T>& dst, const __gm__ char* apiName)
{
#if __NPU_ARCH__ == 3510
    // only support GM -> A1 / B1
    CheckTensorPhyPosition<Hardware::L1>(dst, "dst", "L1 Buffer(A1/B1)", apiName);
    CheckTensorAlignment(dst, ONE_BLK_SIZE, "dst", apiName);
#else
    CheckTensorPhyPosition<Hardware::L1, Hardware::L0A, Hardware::L0B>(
        dst, "dst", "L1 Buffer(A1/B1)/L0A Buffer(A2)/L0B Buffer(B2)", apiName);
    const Hardware dstScope = GetPhyType((TPosition)dst.GetPosition());
    if (dstScope == Hardware::L0A || dstScope == Hardware::L0B) {
        CheckTensorAlignment(dst, VALUE_512, "dst", apiName);
    } else {
        CheckTensorAlignment(dst, ONE_BLK_SIZE, "dst", apiName);
    }
#endif
}

template <typename T>
__aicore__ static inline void CheckLoadData2dParams(const LoadData2DParams& loadDataParams, bool checkTranspose)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    // sid range check is valid only when debug mode
    CheckValueRange<uint8_t>(
        loadDataParams.sid, 0, Internal::ASC_MAX_LOAD2D_SID, "loadDataParams.sid", "LoadData with LoadData2DParams");
#endif
    ReportNopWarning<uint8_t>(
        loadDataParams.repeatTimes, "loadDataParams.repeatTimes", "LoadData with LoadData2DParams");

    // Only when A1->A2 / B1->B2 + dtype B16
    if (loadDataParams.ifTranspose) {
        if (checkTranspose) {
            ASCENDC_DEBUG_WARNING(
                (sizeof(T) == 2),
                KERNEL_LOG_INTERNAL(
                    KERNEL_WARN, "ifTranspose in LoadData2DParams "
                                 "should be enabled only when dtype is uint16_t / int16_t / half / bfloat16_t.\n"));
        } else {
            ASCENDC_DEBUG_WARNING(
                (false), KERNEL_LOG_INTERNAL(
                             KERNEL_WARN, "ifTranspose is effective in LoadData with "
                                          "LoadData2DParams when src->dst is L1 Buffer(A1)->L0A Buffer(A2) / "
                                          "L1 Buffer(B1)->L0B Buffer(B2).\n"));
        }
    }
}

template <typename T>
__aicore__ static inline void CheckLoadDataWithTransposeDtype(const __gm__ char* apiName, bool tPosIsA1)
{
#if __NPU_ARCH__ == 2201
    if (tPosIsA1) { // A1 -> A2
        ASCENDC_DEBUG_ASSERT(
            (SupportType<PrimT<T>, uint8_t, int8_t, half, bfloat16_t, uint32_t, int32_t, float>()),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check dtype in %s, current api support dtype combination is "
                "src and dst both: uint8_t / int8_t / half / bfloat16_t / uint32_t / int32_t / float when dst "
                "position is L0A Buffer(A2).\n",
                apiName));
    } else { // B1 -> B2
        ASCENDC_DEBUG_ASSERT(
            (SupportType<PrimT<T>, uint8_t, int8_t, half, bfloat16_t, uint32_t, int32_t, float, int4b_t>()),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check dtype in %s, current api support dtype "
                "combination is src and dst both: uint8_t / int8_t / half / bfloat16_t / uint32_t / int32_t / float / "
                "int4b_t when dst position is L0B Buffer(B2).\n",
                apiName));
    }
#endif
}

template <typename T>
__aicore__ static inline void CheckLoadDataWithTranspose(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const __gm__ char* apiName)
{
// dav_c310 + dav_m510 + dav_m310 not support A1 -> A2
#if __NPU_ARCH__ != 3510 && __NPU_ARCH__ != 5102 && __NPU_ARCH__ != 3102
    CheckTensorPhyPosition<Hardware::L1>(src, "src", "L1 Buffer(A1/B1)", apiName);
    CheckTensorPhyPosition<Hardware::L0A, Hardware::L0B>(dst, "dst", "L0A Buffer(A2)/L0B Buffer(B2)", apiName);
    if ((TPosition)dst.GetPosition() == TPosition::A2) {
        CheckLoadDataWithTransposeDtype<T>(apiName, true);
    } else {
        CheckLoadDataWithTransposeDtype<T>(apiName, false);
    }
#endif
    CheckTensorAlignment(src, ONE_BLK_SIZE, "src", apiName);
    CheckTensorAlignment(dst, VALUE_512, "dst", apiName);
}

// check LoadData3D params
__aicore__ static inline void CheckLoadData3dParams(
    const uint16_t srcHeight, const uint16_t srcWidth, const uint8_t srcWStride, const uint8_t srcHStride)
{
#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
    CheckValueRange<uint16_t>(srcHeight, 0, Internal::ASC_MAX_LOAD3D_L1, "l1H", "LoadData with LoadData3DParams");
    CheckValueRange<uint16_t>(srcWidth, 0, Internal::ASC_MAX_LOAD3D_L1, "l1W", "LoadData with LoadData3DParams");
    CheckValueRange<uint8_t>(
        srcWStride, static_cast<uint8_t>(Internal::ASC_MIN_LOAD3D_STRIDE),
        static_cast<uint8_t>(Internal::ASC_MAX_LOAD3D_STRIDE), "strideW", "LoadData with LoadData3DParams");
    CheckValueRange<uint8_t>(
        srcHStride, static_cast<uint8_t>(Internal::ASC_MIN_LOAD3D_STRIDE),
        static_cast<uint8_t>(Internal::ASC_MAX_LOAD3D_STRIDE), "strideH", "LoadData with LoadData3DParams");
#endif
}

#if defined(ASCENDC_DEBUG) || defined(ASCENDC_CPU_DEBUG)
template <typename U>
__aicore__ inline void CheckLoadData3dv1Params(const LoadData3DParamsV1<U>& loadDataParams)
{
    CheckValueRange<uint16_t>(
        loadDataParams.c1Index, static_cast<uint16_t>(Internal::ASC_MIN_LOAD3D_C1_IDX),
        static_cast<uint16_t>(Internal::ASC_MAX_LOAD3D_C1_IDX), "c1Index", "LoadData with LoadData3DParamsV1");
    CheckValueRange<uint8_t>(
        loadDataParams.fetchFilterW, static_cast<uint8_t>(Internal::ASC_MIN_LOAD3D_FETCH_FILTER),
        static_cast<uint8_t>(Internal::ASC_MAX_LOAD3D_FETCH_FILTER), "fetchFilterW",
        "LoadData with LoadData3DParamsV1");
    CheckValueRange<uint8_t>(
        loadDataParams.fetchFilterH, static_cast<uint8_t>(Internal::ASC_MIN_LOAD3D_FETCH_FILTER),
        static_cast<uint8_t>(Internal::ASC_MAX_LOAD3D_FETCH_FILTER), "fetchFilterH",
        "LoadData with LoadData3DParamsV1");
    CheckValueRange<int16_t>(
        loadDataParams.leftTopW, static_cast<int16_t>(Internal::ASC_MIN_LOAD3D_LEFT_TOP),
        static_cast<int16_t>(Internal::ASC_MAX_LOAD3D_LEFT_TOP), "leftTopW", "LoadData with LoadData3DParamsV1");
    CheckValueRange<int16_t>(
        loadDataParams.leftTopH, static_cast<int16_t>(Internal::ASC_MIN_LOAD3D_LEFT_TOP),
        static_cast<int16_t>(Internal::ASC_MAX_LOAD3D_LEFT_TOP), "leftTopH", "LoadData with LoadData3DParamsV1");
    CheckValueRange<uint8_t>(
        loadDataParams.filterW, static_cast<uint8_t>(Internal::ASC_MIN_LOAD3D_FILTER),
        static_cast<uint8_t>(Internal::ASC_MAX_LOAD3D_FILTER), "filterW", "LoadData with LoadData3DParamsV1");
    CheckValueRange<uint8_t>(
        loadDataParams.filterH, static_cast<uint8_t>(Internal::ASC_MIN_LOAD3D_FILTER),
        static_cast<uint8_t>(Internal::ASC_MAX_LOAD3D_FILTER), "filterH", "LoadData with LoadData3DParamsV1");
    CheckValueRange<uint8_t>(
        loadDataParams.dilationFilterW, static_cast<uint8_t>(Internal::ASC_MIN_LOAD3D_DILATION_FILTER),
        static_cast<uint8_t>(Internal::ASC_MAX_LOAD3D_FILTER), "dilationFilterW", "LoadData with LoadData3DParamsV1");
    CheckValueRange<uint8_t>(
        loadDataParams.dilationFilterH, static_cast<uint8_t>(Internal::ASC_MIN_LOAD3D_DILATION_FILTER),
        static_cast<uint8_t>(Internal::ASC_MAX_LOAD3D_FILTER), "dilationFilterH", "LoadData with LoadData3DParamsV1");
    CheckValueRange<uint8_t>(
        loadDataParams.jumpStride, static_cast<uint8_t>(Internal::ASC_MIN_LOAD3D_JUMP_STRIDE),
        static_cast<uint8_t>(Internal::ASC_MAX_LOAD3D_JUMP_STRIDE), "jumpStride", "LoadData with LoadData3DParamsV1");
    CheckValueRange<uint8_t>(loadDataParams.repeatMode, 0, 1, "repeatMode", "LoadData with LoadData3DParamsV1");
    CheckValueRange<uint8_t>(loadDataParams.cSize, 0, 1, "cSize", "LoadData with LoadData3DParamsV1");
    CheckValueRange<uint8_t>(
        loadDataParams.repeatTime, static_cast<uint8_t>(Internal::ASC_MIN_LOAD3D_REPEAT_TIMES),
        static_cast<uint8_t>(Internal::ASC_MAX_LOAD3D_FILTER), "repeatTime", "LoadData with LoadData3DParamsV1");
}

template <typename U>
__aicore__ inline void CheckLoadData3dv2Params(const LoadData3DParamsV2<U>& loadDataParams)
{
    CheckValueRange<uint8_t>(
        loadDataParams.dilationFilterW, static_cast<uint8_t>(Internal::ASC_MIN_LOAD3D_DILATION_FILTER),
        static_cast<uint8_t>(Internal::ASC_MAX_LOAD3D_FILTER), "dilationFilterW", "LoadData with LoadData3DParamsV2");
    CheckValueRange<uint8_t>(
        loadDataParams.dilationFilterH, static_cast<uint8_t>(Internal::ASC_MIN_LOAD3D_DILATION_FILTER),
        static_cast<uint8_t>(Internal::ASC_MAX_LOAD3D_FILTER), "dilationFilterH", "LoadData with LoadData3DParamsV2");

    ReportNopWarning<uint16_t>(
        loadDataParams.kExtension, "loadDataParams.kExtension", "LoadData with LoadData3DParamsV2");
    ReportNopWarning<uint16_t>(
        loadDataParams.mExtension, "loadDataParams.mExtension", "LoadData with LoadData3DParamsV2");
    ReportNopWarning<uint16_t>(
        loadDataParams.channelSize, "loadDataParams.channelSize", "LoadData with LoadData3DParamsV2");
    ReportNopWarning<uint16_t>(loadDataParams.l1H, "loadDataParams.l1H", "LoadData with LoadData3DParamsV2");
    ReportNopWarning<uint16_t>(loadDataParams.l1W, "loadDataParams.l1W", "LoadData with LoadData3DParamsV2");

    ASCENDC_DEBUG_WARNING(
        (!(loadDataParams.filterW == 0 && loadDataParams.filterSizeW == false)),
        KERNEL_LOG_INTERNAL(
            KERNEL_WARN, "In LoadData with LoadData3DParamsV2, loadDataParams.filterW = 0 and "
                         "loadDataParams.filterSizeW == false, which makes LoadData with LoadData3DParamsV2 equivalent "
                         "to a NOP.\n"));
    ASCENDC_DEBUG_WARNING(
        (!(loadDataParams.filterH == 0 && loadDataParams.filterSizeH == false)),
        KERNEL_LOG_INTERNAL(
            KERNEL_WARN, "In LoadData with LoadData3DParamsV2, loadDataParams.filterH = 0 and "
                         "loadDataParams.filterSizeH == false, which makes LoadData with LoadData3DParamsV2 equivalent "
                         "to a NOP.\n"));
}
#endif

// check Load3dv2 ChannelSize
template <typename T>
__aicore__ static inline void CheckLoadData3dv2ChannelSize(const uint16_t channelSize)
{
#if __NPU_ARCH__ == 2002
    if constexpr (IsSameType<PrimT<T>, half>::value) {
        uint16_t remainderList[] = {4, 8};
        ASCENDC_DEBUG_ASSERT(
            (ChannelSizeRemainder<PrimT<T>>(channelSize, remainderList, 2) || channelSize == 16),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check param channelSize value in LoadData with "
                "LoadData3DParamsV2 with dtype half, allowed value is 16, or allowed remainders when divided by 16 "
                "are 4 and 8, current value is %u.\n",
                channelSize));
    } else if constexpr (SupportType<PrimT<T>, int8_t, uint8_t>()) {
        uint16_t remainderList[] = {4, 8, 16};
        ASCENDC_DEBUG_ASSERT(
            (ChannelSizeRemainder<PrimT<T>>(channelSize, remainderList, 3) || channelSize == 32),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check param channelSize value in LoadData with "
                "LoadData3DParamsV2 with dtype int8_t / uint8_t, allowed value is 32, or allowed remainders when "
                "divided by 32 are 4, 8, and 16, current value is %u.\n",
                channelSize));
    } else if constexpr (IsSameType<PrimT<T>, int4b_t>::value) {
        uint16_t remainderList[] = {8, 16, 32};
        ASCENDC_DEBUG_ASSERT(
            (ChannelSizeRemainder<PrimT<T>>(channelSize, remainderList, 3) || channelSize == 64),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check param channelSize value in LoadData with "
                "LoadData3DParamsV2 with dtype int4b_t, allowed value is 64, or allowed remainders when divided by "
                "64 are 8, 16, and 32, current value is %u.\n",
                channelSize));
    }
#elif defined(__NPU_ARCH__) &&                                                                               \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 5102) || \
     (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 3510))
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3102 || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113))
    if constexpr (IsSameType<PrimT<T>, half>::value) {
        uint16_t remainderList[] = {0, 4, 8};
        ASCENDC_DEBUG_ASSERT(
            (ChannelSizeRemainder<PrimT<T>>(channelSize, remainderList, 3)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check param channelSize value in LoadData with "
                "LoadData3DParamsV2 with dtype half, allowed remainders when divided by 16 are 0, 4, and 8, current "
                "value is %u.\n",
                channelSize));
    }
#else
    if constexpr (SupportType<PrimT<T>, half, bfloat16_t>()) {
        uint16_t remainderList[] = {0, 4, 8};
        ASCENDC_DEBUG_ASSERT(
            (ChannelSizeRemainder<PrimT<T>>(channelSize, remainderList, 3)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check param channelSize value in LoadData with "
                "LoadData3DParamsV2 with dtype half / bfloat16_t, allowed remainders when divided by 16 are 0, 4, "
                "and 8, current value is %u.\n",
                channelSize));
    }
#endif
    if constexpr (SupportType<PrimT<T>, float, int32_t, uint32_t>()) {
        uint16_t remainderList[] = {0, 4};
        ASCENDC_DEBUG_ASSERT(
            (ChannelSizeRemainder<PrimT<T>>(channelSize, remainderList, 2)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check param channelSize value in LoadData with "
                "LoadData3DParamsV2 with dtype float / int32_t / uint32_t, allowed remainders when divided by 8 are "
                "0 and 4, current value is %u.\n",
                channelSize));
    } else if constexpr (SupportType<PrimT<T>, int8_t, uint8_t>()) {
        uint16_t remainderList[] = {0, 4, 8, 16};
        ASCENDC_DEBUG_ASSERT(
            (ChannelSizeRemainder<PrimT<T>>(channelSize, remainderList, 4)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check param channelSize value in LoadData with "
                "LoadData3DParamsV2 with dtype int8_t / uint8_t, allowed remainders when divided by 32 are 0, 4, 8, "
                "and 16, current value is %u.\n",
                channelSize));
    } else if constexpr (IsSameType<PrimT<T>, int4b_t>::value) {
        uint16_t remainderList[] = {0, 8, 16, 32};
        ASCENDC_DEBUG_ASSERT(
            (ChannelSizeRemainder<PrimT<T>>(channelSize, remainderList, 4)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check param channelSize value in LoadData with "
                "LoadData3DParamsV2 with dtype int4b_t, allowed remainders when divided by 64 are 0, 8, 16, and 32, "
                "current value is %u.\n",
                channelSize));
    }
#endif
}

// check LoadData3dv2 matrix params
template <typename T>
__aicore__ static inline void CheckLoadData3dv2MatrixParams(
    const uint16_t kExtension, const uint16_t mExtension, const uint16_t kStartPt, const uint16_t mStartPt)
{
    constexpr uint16_t base16 = 16;
    if constexpr (SupportType<PrimT<T>, half, int8_t, uint8_t, int4b_t>()) {
        ASCENDC_DEBUG_ASSERT(
            (mExtension % base16 == 0),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check "
                "mExtension value in LoadData with LoadData3DParamsV2 when dtype is half / int8_t / uint8_t / int4b_t, "
                "it should be divisible by 16, current value is %u.\n",
                mExtension));
    }
    uint16_t kExtBase = (SupportType<PrimT<T>, int4b_t>()) ? 64 : ONE_BLK_SIZE / sizeof(PrimT<T>);
    if constexpr (SupportType<PrimT<T>, half, int8_t, uint8_t, int4b_t, int32_t, uint32_t, float>()) {
        ASCENDC_DEBUG_ASSERT(
            (kExtension % kExtBase == 0),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check "
                "kExtension value in LoadData with LoadData3DParamsV2 when dtype is half / int8_t / uint8_t / int4b_t "
                "/ "
                "int32_t / uint32_t / float, it should be divisible by %u, current value is %u.\n",
                kExtBase, kExtension));
        ASCENDC_DEBUG_ASSERT(
            (kStartPt % kExtBase == 0),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check "
                "kStartPt value in LoadData with LoadData3DParamsV2 when dtype is half / int8_t / uint8_t / int4b_t / "
                "int32_t / uint32_t / float, it should be divisible by %u, current value is %u.\n",
                kExtBase, kStartPt));
    }
#if __NPU_ARCH__ == 2002
    if constexpr (SupportType<PrimT<T>, half, int8_t, uint8_t, int4b_t>()) {
        ASCENDC_DEBUG_ASSERT(
            (mStartPt % base16 == 0), KERNEL_LOG_INTERNAL(
                                          KERNEL_ERROR,
                                          "Failed to check "
                                          "mStartPt value in LoadData with LoadData3DParamsV2 when dtype is half / "
                                          "int8_t / uint8_t / int4b_t, it "
                                          "should be divisible by 16, current value is %u.\n",
                                          mStartPt));
    }
#elif __NPU_ARCH__ == 2201
    CheckValueRange<uint16_t>(
        mStartPt, static_cast<uint16_t>(0), static_cast<uint16_t>(UINT15_MAX), "mStartPt",
        "LoadData with LoadData3DParamsV2");
    if constexpr (SupportType<PrimT<T>, half, int8_t, uint8_t>()) {
        ASCENDC_DEBUG_ASSERT(
            (mStartPt % base16 == 0), KERNEL_LOG_INTERNAL(
                                          KERNEL_ERROR,
                                          "Failed to check "
                                          "mStartPt value in LoadData with LoadData3DParamsV2 when dtype is half / "
                                          "int8_t / uint8_t, it should be "
                                          "divisible by 16, current value is %u.\n",
                                          mStartPt));
    }
#endif
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_MM_CHECK_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_CHECK_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_MM_CHECK_H__
#endif
